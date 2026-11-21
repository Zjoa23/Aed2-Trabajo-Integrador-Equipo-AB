/*
 * producto.h
 * TDA Producto: guarda los productos del supermercado en una lista enlazada.
 * Aca estan los tipos de datos, los prototipos y la implementacion de todas
 * las funciones que usa el modo Repositor (y algunas que usa tambien el Cajero,
 * como buscarProducto). Tambien tiene la carga y el guardado de productos.dat
 * y unas funciones para leer datos del teclado sin que se rompa el programa.
 */

#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<stdlib.h>

#define MAXNOMBRE 50

/*Declaracion de tipo de datos personalizados*/

/* Tipo para guardar textos como la descripcion del producto */
typedef char tString[MAXNOMBRE];

/* Datos de un producto. "activo" se usa para la baja logica:
   si esta en false el producto sigue en la lista pero no se muestra ni se vende */
typedef struct {
int codProducto;
float precio;
tString descripcion;
int stock;
bool activo;
int stockMinimo;
}tProducto;

/* Nodo de la lista enlazada: tiene un producto y un puntero al siguiente nodo */
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

/*Funciones para leer datos del teclado*/
void limpiarBuffer();
int leerEntero();
float leerFloat();
void leerTexto(tString);

/*Declaracion de variables globales*/

/* Puntero al primer nodo de la lista de productos (la usan todas las funciones) */
tListaProductos * lista;

/*implementacion de funciones*/

/* Deja la lista vacia, se llama una sola vez al empezar el programa */
void inicializarListaProductos(){
    lista=NULL;
}

/* Devuelve true si la lista no tiene ningun nodo */
bool listaProductosVacia(tListaProductos* pLista){
    return pLista==NULL;
}

/* Crea el primer nodo de la lista (cuando la lista todavia esta vacia) */
void insertarPrimerProducto(tProducto pProducto){
    tListaProductos * nuevoNodo;
    nuevoNodo=(tListaProductos*)malloc(sizeof(tListaProductos));
    nuevoNodo->producto=pProducto;
    nuevoNodo->siguiente=NULL;   /* es el unico nodo, no tiene siguiente */
    lista=nuevoNodo;

}

/* Agrega un nodo al principio de la lista: el nuevo apunta al que era el primero
   y despues la lista pasa a empezar en el nuevo */
void insertarProductoAdelante(tProducto pProducto){
    tListaProductos * nuevoNodo;
    nuevoNodo=(tListaProductos*)malloc(sizeof(tListaProductos));
    nuevoNodo->producto=pProducto;
    nuevoNodo->siguiente=lista;
    lista=nuevoNodo;
}

/* Decide cual de las dos inserciones usar segun si la lista esta vacia o no */
void insertarProducto(tProducto pProducto){
    if(listaProductosVacia(lista)){
        insertarPrimerProducto(pProducto);
    }else{
        insertarProductoAdelante(pProducto);
    }

}

/* Busqueda secuencial por codigo: recorre la lista nodo por nodo hasta encontrarlo.
   Devuelve un puntero al nodo, asi quien la llama puede modificar el producto real
   (por ejemplo el stock). Si no lo encuentra devuelve NULL */
tListaProductos * buscarProducto(int pCodigo) {
    tListaProductos * aux = lista;

    while (aux != NULL && aux->producto.codProducto != pCodigo) {
        aux = aux->siguiente;
    }
    return aux;
}

/* Muestra todos los productos activos en forma de tabla.
   Los que estan dados de baja se saltean */
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

/* Alta de un producto nuevo. Antes de agregarlo se busca el codigo para no repetirlo:
   - si no existe, se inserta en la lista
   - si existe pero estaba dado de baja, se reemplazan sus datos (se vuelve a activar)
   - si existe y esta activo, no se agrega y devuelve false */
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

/* Baja logica: no se borra el nodo, solo se marca el producto como inactivo.
   Devuelve false si el codigo no existe o si ya estaba dado de baja */
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

/* Le suma pCantidad al stock del producto (el repositor la usa para reponer).
   No deja hacerlo si el producto no existe, esta dado de baja
   o si el stock quedaria negativo */
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

/* Recorre la lista y muestra los productos activos que tienen
   el stock igual o por debajo del minimo, y al final cuantos hay que reponer */
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

/* Ordenamiento por burbuja de menor a mayor codigo.
   En vez de mover los nodos se intercambian los datos (los productos) entre
   dos nodos seguidos, que es mas simple. El ciclo de afuera (i) repite las pasadas
   y el de adentro (j) compara cada nodo con el siguiente */
void ordenarPorCodigo() {
    tListaProductos * i;
    tListaProductos * j;
    tProducto aux;

    i = lista;
    while (i != NULL) {
        j = lista;
        while (j->siguiente != NULL) {
            if (j->producto.codProducto > j->siguiente->producto.codProducto) {
                /* intercambio usando una variable auxiliar */
                aux = j->producto;
                j->producto = j->siguiente->producto;
                j->siguiente->producto = aux;
            }
            j = j->siguiente;
        }
        i = i->siguiente;
    }
}

/* Lee productos.dat (archivo binario) y carga cada producto en la lista.
   Se hace una lectura antes del while y otra al final del ciclo,
   asi no se procesa dos veces el ultimo registro.
   Si el archivo no existe se arranca con la lista vacia */
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

/* Guarda toda la lista en productos.dat. Se abre con "wb" para sobreescribir
   el archivo, asi queda igual a como esta la lista en ese momento
   (incluye los dados de baja, para no perderlos) */
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

/* Descarta lo que haya quedado escrito en el teclado hasta el Enter,
   para que no lo lea el proximo scanf */
void limpiarBuffer() {
    int c;

    c = getchar();
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

/* Lee un numero entero. Si el usuario escribe letras o algo que no es
   un numero, lo vuelve a pedir en lugar de seguir con un valor incorrecto */
int leerEntero() {
    int numero;

    while (scanf("%d", &numero) != 1 || getchar() != '\n') {
        limpiarBuffer();
        printf("Valor invalido, ingrese solo numeros: ");
    }
    return numero;
}

/* Igual que leerEntero pero para numeros con decimales (el precio) */
float leerFloat() {
    float numero;

    while (scanf("%f", &numero) != 1 || getchar() != '\n') {
        limpiarBuffer();
        printf("Valor invalido, ingrese solo numeros (use punto para los decimales): ");
    }
    return numero;
}

/* Lee un texto que puede tener espacios (por ejemplo "Leche entera").
   %49[^\n] lee hasta el Enter y como maximo 49 letras para no pasarse del tString */
void leerTexto(tString pTexto) {
    scanf(" %49[^\n]", pTexto);
    limpiarBuffer();
}
