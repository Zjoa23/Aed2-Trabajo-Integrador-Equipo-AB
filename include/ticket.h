/*
 * ticket.h
 * TDA Ticket: representa una venta del modo Cajero.
 * Cada ticket tiene su numero, la fecha, el numero de caja y una lista enlazada
 * con los productos que lleva el cliente (los items). Al cerrar el ticket
 * la venta se guarda en el archivo binario ventas.dat.
 * Incluye a producto.h porque necesita buscar los productos y descontar el stock.
 */

#include "producto.h"

/* Fecha de la venta */
typedef struct {
    int dia;
    int mes;
    int anio;
} tFecha;

/* Un renglon del ticket. Se copian la descripcion y el precio del producto
   en el momento de la venta, asi el ticket no cambia aunque despues cambie el precio */
typedef struct {
    int     codProducto;
    tString descripcion;
    int     cantidad;
    float   precioUnitario;
    float   subtotal;
} tItem;

/* Nodo de la lista de items del ticket */
typedef struct nodoItem {
    tItem             item;
    struct nodoItem * siguiente;
} tListaItems;

/* Datos generales del ticket (la cabecera) */
typedef struct {
    int    nroTicket;
    tFecha fecha;
    int    nroCaja;
    float  total;
} tTicket;

/* Registro que se guarda en ventas.dat: se escribe uno por cada item vendido
   y cada uno lleva los datos del ticket al que pertenece.
   Asi en el informe se pueden agrupar los items por numero de ticket (corte de control) */
typedef struct {
    tTicket ticket;
    int     codProducto;
    int     cantidad;
    float   subtotal;
} tRegistroVenta;

/* Variables globales: la lista de items y el ticket que se esta armando ahora */
tListaItems * listaItems;
tTicket       ticket;

/*Declaracion de prototipos*/
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

/*implementacion de funciones*/

/* Controla que la fecha tenga valores logicos: dia entre 1 y 31,
   mes entre 1 y 12 y anio desde el 2000 */
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

/* Pide dia, mes y anio por teclado y los vuelve a pedir
   hasta que la fecha sea valida */
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

/* Recorre ventas.dat hasta el final para saber cual fue el ultimo numero
   de ticket usado. Como las ventas se agregan siempre al final, el ultimo
   registro leido es el del ticket mas nuevo. Si no hay ventas devuelve 0 */
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

/* Prepara un ticket nuevo: lista de items vacia, numero siguiente al ultimo
   guardado, la fecha y la caja que ingreso el cajero, y total en 0 */
void iniciarTicket(int pNroCaja, tFecha pFecha) {
    listaItems = NULL;
    ticket.nroTicket = ultimoNroTicket() + 1;
    ticket.fecha = pFecha;
    ticket.nroCaja = pNroCaja;
    ticket.total = 0;
}

/* Crea el primer nodo de la lista de items */
void insertarPrimerItem(tItem pItem) {
    tListaItems * nuevoNodo;
    nuevoNodo = (tListaItems *) malloc(sizeof(tListaItems));
    nuevoNodo->item = pItem;
    nuevoNodo->siguiente = NULL;
    listaItems = nuevoNodo;
}

/* Agrega un item al principio de la lista (mismo esquema que en producto.h) */
void insertarItemAdelante(tItem pItem) {
    tListaItems * nuevoNodo;
    nuevoNodo = (tListaItems *) malloc(sizeof(tListaItems));
    nuevoNodo->item = pItem;
    nuevoNodo->siguiente = listaItems;
    listaItems = nuevoNodo;
}

/* Elige que insercion usar segun si la lista de items esta vacia o no */
void insertarItem(tItem pItem) {
    if (listaItems == NULL) {
        insertarPrimerItem(pItem);
    } else {
        insertarItemAdelante(pItem);
    }
}

/* Agrega un producto al ticket. El cajero solo ingresa el codigo y la cantidad:
   se busca el producto en la lista, se controla que este activo y que alcance
   el stock, se descuenta el stock y se arma el item copiando la descripcion
   y el precio. Devuelve false si no se pudo agregar */
bool agregarItem(int codigo, int cantidad) {
    tListaProductos * nodo;
    tItem item;
    bool resultado;

    nodo = buscarProducto(codigo);

    if (cantidad <= 0 || nodo == NULL) {
        resultado = false;   /* Cantidad invalida o el codigo no existe */
    } else {
        if (nodo->producto.activo == false || nodo->producto.stock < cantidad) {
            resultado = false;   /* Dado de baja o no hay stock suficiente */
        } else {
            /* Se descuenta el stock directamente en el nodo de la lista */
            nodo->producto.stock = nodo->producto.stock - cantidad;

            /* Se arma el item con los datos del producto */
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

/* Subtotal de un item: cantidad por precio unitario */
float calcularSubtotal(int pCantidad, float pPrecio) {
    float subtotal;

    subtotal = pCantidad * pPrecio;
    return subtotal;
}

/* Recorre la lista de items y suma los subtotales para obtener el total */
void calcularTotal() {
    tListaItems * aux;

    ticket.total = 0;
    aux = listaItems;
    while (aux != NULL) {
        ticket.total = ticket.total + aux->item.subtotal;
        aux = aux->siguiente;
    }
}

/* Muestra el ticket por pantalla como uno de supermercado:
   cabecera, cada item con su cantidad, precio y subtotal, y el total al final */
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

/* Libera la memoria de todos los nodos de la lista de items.
   Se va guardando el nodo a borrar, se avanza al siguiente y recien ahi se hace free */
void liberarItems() {
    tListaItems * nodoSuprimir;

    while (listaItems != NULL) {
        nodoSuprimir = listaItems;
        listaItems = listaItems->siguiente;
        free(nodoSuprimir);
    }
}

/* Cierra la venta: guarda un registro en ventas.dat por cada item del ticket
   y despues libera la lista de items.
   Se abre con "ab" para agregar al final sin borrar las ventas anteriores.
   Si el ticket esta vacio no se guarda nada y devuelve false */
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
                /* cada registro lleva los datos del ticket y los del item */
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
