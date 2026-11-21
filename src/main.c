/*
 * main.c
 * Programa principal del Sistema de Gestion para Supermercado.
 * Tiene el menu principal, el menu del modo Cajero, el menu del modo Repositor
 * y el informe de ventas, que recorre ventas.dat con corte de control
 * agrupando los items por numero de ticket.
 * Solo incluye ticket.h, porque ticket.h ya incluye a producto.h.
 */

#include "ticket.h"

/*Declaracion de variables globales*/

/* Variables que usa el corte de control del informe de ventas */
tRegistroVenta registro;     /* registro que se acaba de leer de ventas.dat */
FILE * archivo;              /* archivo ventas.dat abierto para leer */
int codTicketAnt;            /* numero del ticket que se esta procesando (la clave del corte) */
float totalTicket;           /* acumulador del total de un ticket */
float totalGeneral;          /* acumulador de todas las ventas */
int cantidadTickets;         /* contador de tickets */

/*Declaracion de prototipos*/
void menuPrincipal();
void menuCajero();
void menuRepositor();

void nuevaVenta(int, tFecha);

void ingresarProducto(tProducto *);
void mostrarProductoBuscado();

/*Funciones corte de control*/
void informeVentas();
void inicializacion();
void procesoCorte();
void finalizacion();
void principioCorte();
void unTicket();
void finCorte();

/* Al empezar se prepara la lista y se cargan los productos guardados en productos.dat,
   despues se muestra el menu principal hasta que el usuario elija salir */
int main() {
    inicializarListaProductos();
    cargarProductos();
    menuPrincipal();
    return 0;
}

/*implementacion de funciones*/

/* Menu principal: permite elegir el modo de uso o ver el informe de ventas.
   Al salir se guardan los productos para no perder los cambios */
void menuPrincipal() {
    int opcion;

    do {
        printf("\n===== SUPERMERCADO =====\n");
        printf("1. Modo Cajero\n");
        printf("2. Modo Repositor\n");
        printf("3. Informe de ventas\n");
        printf("4. Salir\n");
        printf("Ingrese que opcion desea realizar: ");
        opcion = leerEntero();

        switch (opcion) {
            case 1:
                menuCajero();
                break;
            case 2:
                menuRepositor();
                break;
            case 3:
                informeVentas();
                break;
            case 4:
                guardarProductos();
                printf("Productos guardados. Saliendo del programa...\n");
                break;
            default:
                printf("Opcion invalida\n");
                break;
        }
    } while (opcion != 4);
}

/* Modo Cajero: al entrar se pide el numero de caja y la fecha una sola vez,
   y se usan para todas las ventas que se hagan hasta volver al menu principal */
void menuCajero() {
    int opcion;
    int nroCaja;
    tFecha fecha;

    printf("Ingrese el numero de caja: ");
    nroCaja = leerEntero();
    fecha = ingresarFecha();

    do {
        printf("\n===== MODO CAJERO (Caja %d) =====\n", nroCaja);
        printf("1. Nueva venta\n");
        printf("2. Volver al menu principal\n");
        printf("Ingrese que opcion desea realizar: ");
        opcion = leerEntero();

        switch (opcion) {
            case 1:
                nuevaVenta(nroCaja, fecha);
                break;
            case 2:
                printf("Volviendo al menu principal...\n");
                break;
            default:
                printf("Opcion invalida\n");
                break;
        }
    } while (opcion != 2);
}

/* Modo Repositor: gestion del stock. Cada opcion llama a una funcion de producto.h
   y muestra un mensaje segun si la operacion se pudo hacer o no */
void menuRepositor() {
    int opcion;
    int codigo;
    int cantidad;
    tProducto producto;

    do {
        printf("\n===== MODO REPOSITOR =====\n");
        printf("1. Listar productos\n");
        printf("2. Alta de producto\n");
        printf("3. Baja de producto\n");
        printf("4. Reponer stock\n");
        printf("5. Alertas de stock minimo\n");
        printf("6. Buscar producto por codigo\n");
        printf("7. Volver al menu principal\n");
        printf("Ingrese que opcion desea realizar: ");
        opcion = leerEntero();

        switch (opcion) {
            case 1:
                /* se ordena por codigo antes de mostrar, asi la lista sale ordenada */
                ordenarPorCodigo();
                visualizarListaProductos(lista);
                break;
            case 2:
                ingresarProducto(&producto);
                if (altaProducto(producto)) {
                    printf("Producto agregado\n");
                } else {
                    printf("Error: ya existe un producto activo con el codigo %d\n", producto.codProducto);
                }
                break;
            case 3:
                printf("Ingrese el codigo del producto a dar de baja: ");
                codigo = leerEntero();
                if (bajaProducto(codigo)) {
                    printf("Producto dado de baja\n");
                } else {
                    printf("Error: el codigo no existe o ya estaba dado de baja\n");
                }
                break;
            case 4:
                /* solo se aceptan cantidades positivas, porque es una reposicion */
                printf("Ingrese el codigo del producto: ");
                codigo = leerEntero();
                printf("Ingrese la cantidad a reponer: ");
                cantidad = leerEntero();
                if (cantidad > 0 && actualizarStock(codigo, cantidad)) {
                    printf("Stock actualizado\n");
                } else {
                    printf("Error: codigo inexistente, dado de baja o cantidad invalida\n");
                }
                break;
            case 5:
                mostrarAlertasStock();
                break;
            case 6:
                mostrarProductoBuscado();
                break;
            case 7:
                printf("Volviendo al menu principal...\n");
                break;
            default:
                printf("Opcion invalida\n");
                break;
        }
    } while (opcion != 7);
}

/* Registra una venta completa: abre un ticket nuevo y va pidiendo codigo
   y cantidad hasta que el cajero ingresa 0. Despues imprime el ticket,
   lo guarda en ventas.dat y guarda los productos para que quede
   registrado el stock descontado */
void nuevaVenta(int pNroCaja, tFecha pFecha) {
    int codigo;
    int cantidad;

    iniciarTicket(pNroCaja, pFecha);

    printf("Ingrese el codigo del producto (0 para terminar): ");
    codigo = leerEntero();

    while (codigo != 0) {
        printf("Ingrese la cantidad: ");
        cantidad = leerEntero();

        if (agregarItem(codigo, cantidad)) {
            printf("Producto agregado al ticket\n");
        } else {
            printf("No se pudo agregar: codigo inexistente, dado de baja, cantidad invalida o sin stock\n");
        }

        printf("Ingrese el codigo del producto (0 para terminar): ");
        codigo = leerEntero();
    }

    imprimirTicket();

    if (cerrarTicket()) {
        guardarProductos();
        printf("Venta registrada en ventas.dat\n");
    } else {
        printf("El ticket esta vacio: no se registro ninguna venta\n");
    }
}

/* Pide por teclado los datos de un producto nuevo para darlo de alta.
   Recibe un puntero para poder cargar los datos en la variable de menuRepositor.
   El producto siempre empieza activo */
void ingresarProducto(tProducto * pProducto) {
    printf("Ingrese el codigo: ");
    pProducto->codProducto = leerEntero();
    printf("Ingrese la descripcion: ");
    leerTexto(pProducto->descripcion);
    printf("Ingrese el precio: ");
    pProducto->precio = leerFloat();
    printf("Ingrese el stock: ");
    pProducto->stock = leerEntero();
    printf("Ingrese el stock minimo: ");
    pProducto->stockMinimo = leerEntero();
    pProducto->activo = true;
}

/* Pide un codigo, lo busca en la lista y muestra los datos del producto.
   Si no existe o esta dado de baja muestra un mensaje */
void mostrarProductoBuscado() {
    int codigo;
    tListaProductos * nodo;

    printf("Ingrese el codigo a buscar: ");
    codigo = leerEntero();

    nodo = buscarProducto(codigo);

    if (nodo == NULL || nodo->producto.activo == false) {
        printf("No existe un producto activo con ese codigo\n");
    } else {
        printf("%d | %s | $%.2f | Stock: %d | Minimo: %d\n",
               nodo->producto.codProducto,
               nodo->producto.descripcion,
               nodo->producto.precio,
               nodo->producto.stock,
               nodo->producto.stockMinimo);
    }
}

/* ===================== CORTE DE CONTROL =====================
   Informe de ventas a partir de ventas.dat.
   En el archivo hay un registro por cada item vendido, y los items de un mismo
   ticket quedan seguidos porque se guardan juntos al cerrar el ticket.
   La clave del corte es el numero de ticket: mientras el registro leido tenga
   el mismo numero se acumula en el total de ese ticket, y cuando cambia
   se muestra el total y se empieza el siguiente.*/
void informeVentas() {
    inicializacion();
    if (archivo != NULL) {
        procesoCorte();
        finalizacion();
    }
}

/* Abre el archivo, hace la primera lectura, guarda el primer numero de ticket
   como clave anterior y pone los acumuladores generales en 0 */
void inicializacion() {
    archivo = fopen("ventas.dat", "rb");

    if (archivo == NULL) {
        printf("Todavia no hay ventas registradas\n");
    } else {
        fread(&registro, sizeof(tRegistroVenta), 1, archivo);
        codTicketAnt = registro.ticket.nroTicket;
        totalGeneral = 0;
        cantidadTickets = 0;
        printf("\n=== INFORME DE VENTAS POR TICKET ===\n");
    }
}

/* Ciclo principal del corte. El while de afuera recorre todo el archivo
   (un ticket por vuelta) y el de adentro procesa los items mientras
   el numero de ticket siga siendo el mismo */
void procesoCorte() {
    while (!feof(archivo)) {
        principioCorte();

        while (!feof(archivo) && registro.ticket.nroTicket == codTicketAnt) {
            unTicket();
        }

        finCorte();
    }
}

/* Comienzo de un grupo: se reinicia el total del ticket y se muestra su cabecera */
void principioCorte() {
    totalTicket = 0;
    printf("\nTicket N: %d - Fecha: %d/%d/%d - Caja: %d\n",
           registro.ticket.nroTicket,
           registro.ticket.fecha.dia, registro.ticket.fecha.mes, registro.ticket.fecha.anio,
           registro.ticket.nroCaja);
}

/* Procesa un item del ticket: lo suma al total, lo muestra y lee el siguiente registro */
void unTicket() {
    totalTicket = totalTicket + registro.subtotal;
    printf("   Cod %d x %d   $%.2f\n", registro.codProducto, registro.cantidad, registro.subtotal);
    fread(&registro, sizeof(tRegistroVenta), 1, archivo);
}

/* Fin de un grupo: se muestra el total del ticket, se suma al total general
   y la clave anterior pasa a ser el numero del proximo ticket */
void finCorte() {
    printf("   Total del ticket: $%.2f\n", totalTicket);
    codTicketAnt = registro.ticket.nroTicket;
    totalGeneral = totalGeneral + totalTicket;
    cantidadTickets++;
}

/* Al terminar el archivo se muestran los totales generales y se cierra */
void finalizacion() {
    printf("\nCantidad de tickets: %d\n", cantidadTickets);
    printf("TOTAL GENERAL: $%.2f\n", totalGeneral);
    fclose(archivo);
}
