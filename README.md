# Aed2-Trabajo-Integrador-Equipo-AB
# Sistema de Gestión para Supermercado — Modo Cajero y Repositor

## Integrantes

- Benjamín Navarro Ríos
- Brian Rodríguez
- Joaquín Gabriel Romero
- Yohan Esteban Urbina Ávalos

## Descripción

Sistema que simula el funcionamiento de un supermercado mediante dos modos de uso: **Cajero**, para registrar las ventas realizadas, y **Repositor**, para gestionar el stock de los productos disponibles (alta, baja, actualización de cantidades y alertas de stock mínimo). El objetivo es aplicar de forma práctica los temas vistos en la materia sobre una situación cotidiana y fácil de entender.

## Estructuras de datos utilizadas

- Listas
- TDA (Tipos de Datos Abstractos)
- Archivos

## Algoritmos y técnicas

- Corte de Control
- Búsqueda
- Ordenamiento

## Cómo compilar y ejecutar

Desde la carpeta raíz del repositorio:

```
gcc -Iinclude src/main.c -o supermercado
./supermercado
```

En Windows se ejecuta con `supermercado.exe` (CMD) o `.\supermercado` (PowerShell).

Los archivos `productos.dat` y `ventas.dat` se crean automáticamente en la carpeta desde donde se ejecuta el programa.

## Estructura de carpetas

```
.
├── include/   # Archivos de cabecera (.h) con los tipos de datos, prototipos e implementaciones
├── src/       # Implementación del código fuente (.c)
├── data/      # Archivos de datos (.dat, .txt) usados por el programa
└── docs/      # Documentación adicional del proyecto (diagramas, apuntes, etc.)
