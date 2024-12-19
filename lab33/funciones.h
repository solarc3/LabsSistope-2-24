#pragma once
#include <stdio.h>

typedef struct Nodo {
    int valor;
    struct Nodo *siguiente;
    struct Nodo *anterior;
} Nodo;

typedef struct Lista {
    Nodo *cabeza;
    Nodo *cola;
    int longitud;
} Lista;

void limpiarLista(Lista *lista);
Lista *crearLista(FILE *archivo, int *longitud);
void imprimirLista(const Lista *lista);
void escribirEnArchivo(const Lista *lista, const char *nombreArchivo);
