#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>

#define MAXNOMBRE 50

/*Declaracion de tipo de datos personalizados*/
typedef char tString[MAXNOMBRE];

typedef struct {
int codProducto;
float precio;
tString descripcion;
int stock;
bool activo; 
int stockMinimo;
}tProducto;

typedef struct nodo{
    tProducto producto;
    struct nodo * siguiente;
}tListaProductos;

/*Declaracion de prototipos*/
void inicializarListaProductos();
bool listaProductosVacia(tListaProductos*);
void insertarPrimerProducto(tProducto);
void insertarProductoAdelante(tProducto);
void insertarProducto(tProducto);
tListaProductos* buscarProducto(int);
void visualizarListaProductos(tListaProductos*);

/*Funciones repositor*/
bool altaProducto(tProducto);
bool bajaProducto(int);
bool actualizarStock(int,int);
void mostrarAlertasStock();
void ordenarPorCodigo();
void cargarProductos();
void guardarProductos();
void limpiarBuffer();
int leerEntero();
float leerFloat();
void leerTexto(tString);
/*Declaracion de variables globales*/
tListaProductos * lista;

/*implementacion de funciones*/
void inicializarListaProductos(){
    lista=NULL;
}

bool listaProductosVacia(tListaProductos* pLista){
    return pLista==NULL;
}

void insertarPrimerProducto(tProducto pProducto){
    tListaProductos * nuevoNodo;
    nuevoNodo=(tListaProductos*)malloc(sizeof(tListaProductos));
    nuevoNodo->producto=pProducto;
    nuevoNodo->siguiente=NULL;
    lista=nuevoNodo;

}

void insertarProductoAdelante(tProducto pProducto){
    tListaProductos * nuevoNodo;
    nuevoNodo=(tListaProductos*)malloc(sizeof(tListaProductos));
    nuevoNodo->producto=pProducto;
    nuevoNodo->siguiente=lista;
    lista=nuevoNodo;
}

void insertarProducto(tProducto pProducto){
    if(listaProductosVacia(lista)){
        insertarPrimerProducto(pProducto);
    }else{
        insertarProductoAdelante(pProducto);
    }

}

tListaProductos * buscarProducto(int pCodigo) {
    tListaProductos * aux = lista;

    while (aux != NULL && aux->producto.codProducto != pCodigo) {
        aux = aux->siguiente;
    }
    return aux;
}

void visualizarListaProductos(tListaProductos* pLista){
    tListaProductos * aux = pLista;

    if (listaProductosVacia(aux)) {
        printf("No hay productos cargados\n");
        
    }else{
        printf("\nCodigo | Descripcion | Precio | Stock | Stock minimo\n");

        while (aux != NULL) {
            if (aux->producto.activo) {
                printf("%d | %s | $%.2f | %d | %d\n",
                       aux->producto.codProducto,
                       aux->producto.descripcion,
                       aux->producto.precio,
                       aux->producto.stock,
                       aux->producto.stockMinimo);
            }
            aux = aux->siguiente;
        }
    }
}

bool altaProducto(tProducto pProducto){
    tListaProductos * nodo;
    bool resultado;

    nodo = buscarProducto(pProducto.codProducto);

    if (nodo == NULL) {
        insertarProducto(pProducto);
        resultado = true;
    } else if (nodo->producto.activo == false) {
        nodo->producto = pProducto;
        resultado = true;
    } else {
        resultado = false;
    }
    return resultado;

}

bool bajaProducto(int pCodigo){
     tListaProductos * nodoBusqueda;
    bool resultado;

    nodoBusqueda = buscarProducto(pCodigo);

    if (nodoBusqueda == NULL || nodoBusqueda->producto.activo == false) {
        resultado = false;
    } else {
        nodoBusqueda->producto.activo = false;
        resultado = true;
    }
    return resultado;
}

bool actualizarStock(int pCodigo,int pCantidad){
    tListaProductos * nodoStock;
    bool resultado;

    nodoStock=buscarProducto(pCodigo);

     if (nodoStock == NULL || nodoStock->producto.activo == false) {
        resultado = false;
    } else if (nodoStock->producto.stock + pCantidad < 0) {
        resultado = false;                       
    } else {
        nodoStock->producto.stock = nodoStock->producto.stock + pCantidad;
        resultado = true;
    }
    return resultado;
}

void mostrarAlertasStock() {
    tListaProductos * aux = lista;
    int cantidadAlertas = 0;

    printf("\n=== ALERTAS DE STOCK MINIMO ===\n");

    while (aux != NULL) {
        if (aux->producto.activo && aux->producto.stock <= aux->producto.stockMinimo) {
            printf("%d | %s | Stock: %d | Minimo: %d\n",aux->producto.codProducto,aux->producto.descripcion,aux->producto.stock,aux->producto.stockMinimo);
            cantidadAlertas++;
        }
        aux = aux->siguiente;
    }

    if (cantidadAlertas == 0) {
        printf("Todos los productos tienen stock suficiente\n");
    } else {
        printf("Productos a reponer: %d\n", cantidadAlertas);
    }
}

void ordenarPorCodigo() {
    tListaProductos * i;
    tListaProductos * j;
    tProducto aux;

    i = lista;
    while (i != NULL) {                        
        j = lista;
        while (j->siguiente != NULL) {         
            if (j->producto.codProducto > j->siguiente->producto.codProducto) {
                aux = j->producto;
                j->producto = j->siguiente->producto;
                j->siguiente->producto = aux;
            }
            j = j->siguiente;
        }
        i = i->siguiente;
    }
}

void cargarProductos() {
    FILE * archivo;
    tProducto producto;

    archivo = fopen("productos.dat", "rb");

    if (archivo == NULL) {
        printf("No existe productos.dat: se empieza sin productos\n");
    } else {
        fread(&producto, sizeof(tProducto), 1, archivo);         
        while (!feof(archivo)) {
            insertarProducto(producto);                           
            fread(&producto, sizeof(tProducto), 1, archivo);     
        }
        fclose(archivo);
    }
}

void guardarProductos() {
    FILE * archivo;
    tListaProductos * aux = lista;

    archivo = fopen("productos.dat", "wb");

    if (archivo == NULL) {
        printf("Error: no se pudo guardar productos.dat\n");
    } else {
        while (aux != NULL) {
            fwrite(&aux->producto, sizeof(tProducto), 1, archivo);
            aux = aux->siguiente;
        }
        fclose(archivo);
    }
}

void limpiarBuffer() {
    int c;

    c = getchar();
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

int leerEntero() {
    int numero;

    while (scanf("%d", &numero) != 1 || getchar() != '\n') {
        limpiarBuffer();
        printf("Valor invalido, ingrese solo numeros: ");
    }
    return numero;
}

float leerFloat() {
    float numero;

    while (scanf("%f", &numero) != 1 || getchar() != '\n') {
        limpiarBuffer();
        printf("Valor invalido, ingrese solo numeros (use punto para los decimales): ");
    }
    return numero;
}

void leerTexto(tString pTexto) {
    scanf(" %49[^\n]", pTexto);
    limpiarBuffer();
}
