#include "producto.h"

typedef struct {
    int dia;
    int mes;
    int anio;
} tFecha;

typedef struct {
    int     codProducto;
    tString descripcion;     
    int     cantidad;
    float   precioUnitario;  
    float   subtotal;        
} tItem;

typedef struct nodoItem {
    tItem             item;
    struct nodoItem * siguiente;
} tListaItems;

typedef struct {
    int    nroTicket;
    tFecha fecha;
    int    nroCaja;
    float  total;
} tTicket;

typedef struct {
    tTicket ticket;
    int     codProducto;
    int     cantidad;
    float   subtotal;
} tRegistroVenta;

tListaItems * listaItems;   
tTicket       ticket;       

bool   fechaValida(tFecha);
tFecha ingresarFecha();
int    ultimoNroTicket();
void   iniciarTicket(int, tFecha);
void   insertarPrimerItem(tItem);
void   insertarItemAdelante(tItem);
void   insertarItem(tItem);
bool   agregarItem(int, int);
float  calcularSubtotal(int, float);
void   calcularTotal();
void   imprimirTicket();
void   liberarItems();
bool   cerrarTicket();


bool fechaValida(tFecha pFecha) {
    bool resultado;

    if (pFecha.dia < 1 || pFecha.dia > 31) {
        resultado = false;
    } else if (pFecha.mes < 1 || pFecha.mes > 12) {
        resultado = false;
    } else if (pFecha.anio < 2000) {
        resultado = false;
    } else {
        resultado = true;
    }
    return resultado;
}

tFecha ingresarFecha() {
    tFecha fecha;

    do {
        printf("Ingrese el dia: ");
        fecha.dia = leerEntero();
        printf("Ingrese el mes: ");
        fecha.mes = leerEntero();
        printf("Ingrese el anio: ");
        fecha.anio = leerEntero();
        if (!fechaValida(fecha)) {
            printf("Fecha invalida, intente de nuevo.\n");
        }
    } while (!fechaValida(fecha));

    return fecha;
}
 
int ultimoNroTicket() {
    FILE * archivo;
    tRegistroVenta registro;
    int ultimo;

    ultimo = 0;
    archivo = fopen("ventas.dat", "rb");
    if (archivo != NULL) {
        
        fread(&registro, sizeof(tRegistroVenta), 1, archivo);
        while (!feof(archivo)) {
            ultimo = registro.ticket.nroTicket;
            fread(&registro, sizeof(tRegistroVenta), 1, archivo);
        }
        fclose(archivo);
    }
    
    return ultimo;
}

void iniciarTicket(int pNroCaja, tFecha pFecha) {
    listaItems = NULL;
    ticket.nroTicket = ultimoNroTicket() + 1;
    ticket.fecha = pFecha;   
    ticket.nroCaja = pNroCaja;
    ticket.total = 0;
}

void insertarPrimerItem(tItem pItem) {
    tListaItems * nuevoNodo;
    nuevoNodo = (tListaItems *) malloc(sizeof(tListaItems));
    nuevoNodo->item = pItem;
    nuevoNodo->siguiente = NULL;
    listaItems = nuevoNodo;
}

void insertarItemAdelante(tItem pItem) {
    tListaItems * nuevoNodo;
    nuevoNodo = (tListaItems *) malloc(sizeof(tListaItems));
    nuevoNodo->item = pItem;
    nuevoNodo->siguiente = listaItems;
    listaItems = nuevoNodo;
}

void insertarItem(tItem pItem) {
    if (listaItems == NULL) {
        insertarPrimerItem(pItem);
    } else {
        insertarItemAdelante(pItem);
    }
}
 
bool agregarItem(int codigo, int cantidad) {
    tListaProductos * nodo;
    tItem item;
    bool resultado;

    nodo = buscarProducto(codigo);

    if (cantidad <= 0 || nodo == NULL) {
        resultado = false;   /* Cantidad invalida o el codigo no existe */
    } else {
        if (nodo->producto.activo == false || nodo->producto.stock < cantidad) {
            resultado = false;   
        } else {
            
            nodo->producto.stock = nodo->producto.stock - cantidad;

           
            item.codProducto = nodo->producto.codProducto;
            strcpy(item.descripcion, nodo->producto.descripcion);
            item.precioUnitario = nodo->producto.precio;
            item.cantidad = cantidad;
            item.subtotal = calcularSubtotal(cantidad, item.precioUnitario);

            insertarItem(item);
            resultado = true;
        }
    }
    return resultado;
}

float calcularSubtotal(int pCantidad, float pPrecio) {
    float subtotal;

    subtotal = pCantidad * pPrecio;
    return subtotal;
}

void calcularTotal() {
    tListaItems * aux;

    ticket.total = 0;
    aux = listaItems;
    while (aux != NULL) {
        ticket.total = ticket.total + aux->item.subtotal;
        aux = aux->siguiente;
    }
}

void imprimirTicket() {
    tListaItems * aux;

    calcularTotal();

    printf("\n==============================\n");
    printf("SUPERMERCADO\n");
    printf("Ticket N: %d   Caja: %d\n", ticket.nroTicket, ticket.nroCaja);
    printf("Fecha: %d/%d/%d\n", ticket.fecha.dia, ticket.fecha.mes, ticket.fecha.anio);
    printf("------------------------------\n");

    if (listaItems == NULL) {
        printf("No hay productos en el ticket\n");
    } else {
        aux = listaItems;
        while (aux != NULL) {
            printf("%d x $%.2f\n", aux->item.cantidad, aux->item.precioUnitario);
            printf("%s   $%.2f\n", aux->item.descripcion, aux->item.subtotal);
            aux = aux->siguiente;
        }
    }

    printf("------------------------------\n");
    printf("TOTAL: $%.2f\n", ticket.total);
    printf("==============================\n");
}

void liberarItems() {
    tListaItems * nodoSuprimir;

    while (listaItems != NULL) {
        nodoSuprimir = listaItems;
        listaItems = listaItems->siguiente;
        free(nodoSuprimir);
    }
}

bool cerrarTicket() {
    FILE * archivo;
    tListaItems * aux;
    tRegistroVenta registro;
    bool resultado;

    if (listaItems == NULL) {
        resultado = false;   
    } else {
     
        archivo = fopen("ventas.dat", "ab");
        if (archivo == NULL) {
            resultado = false;
        } else {
            calcularTotal();   

            aux = listaItems;
            while (aux != NULL) {
                
                registro.ticket = ticket;
                
                registro.codProducto = aux->item.codProducto;
                registro.cantidad = aux->item.cantidad;
                registro.subtotal = aux->item.subtotal;

                fwrite(&registro, sizeof(tRegistroVenta), 1, archivo);
                aux = aux->siguiente;
            }
            fclose(archivo);
            liberarItems();
            resultado = true;
        }
    }
    return resultado;
}
