#include "funciones.h"
#include <stdlib.h>

void limpiarLista(Lista *lista) {
    if (lista->cabeza != NULL) {
        Nodo *actual = lista->cabeza;
        Nodo *temp;
        for (int i = 0; i < lista->longitud; i++) {
            temp = actual;
            actual = actual->siguiente;
            free(temp);
        }
        lista->cabeza = NULL;
        lista->cola = NULL;
        lista->longitud = 0;
    }
}
Lista *crearLista(FILE *archivo, int *longitud) {
    Lista *lista = (Lista *)malloc(sizeof(Lista));
    if (lista == NULL) {
        exit(EXIT_FAILURE);
    }

    lista->cabeza = NULL;
    lista->cola = NULL;
    lista->longitud = 0;
    int valor;
    while (fscanf(archivo, "%d", &valor) == 1) {
        Nodo *nuevoNodo = (Nodo *)malloc(sizeof(Nodo));
        if (nuevoNodo == NULL) {
            limpiarLista(lista);
            free(lista);
            exit(EXIT_FAILURE);
        }
        nuevoNodo->valor = valor;
        nuevoNodo->siguiente = NULL;
        nuevoNodo->anterior = lista->cola;

        if (lista->cola != NULL) {
            lista->cola->siguiente = nuevoNodo;
        }
        lista->cola = nuevoNodo;

        if (lista->cabeza == NULL) {
            lista->cabeza = nuevoNodo;
        }

        lista->longitud++;
    }
    if (lista->longitud > 0) {
        lista->cola->siguiente = lista->cabeza;
        lista->cabeza->anterior = lista->cola;
    }

    *longitud = lista->longitud;
    return lista;
}
void imprimirLista(const Lista *lista) {
    if (lista->cabeza == NULL) {
        printf("La lista esta vacia.\n");
        return;
    }

    Nodo *nodo = lista->cabeza;
    for (int i = 0; i < lista->longitud; i++) {

        printf("%d ", nodo->valor);
        nodo = nodo->siguiente;
    }
}
void escribirEnArchivo(const Lista *lista, const char *nombreArchivo) {
    FILE *fp = fopen(nombreArchivo, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir el archivo de salida");
        return;
    }

    if (lista->cabeza == NULL) {
        fprintf(fp, "La lista esta vacia.\n");
        fclose(fp);
        return;
    }

    Nodo *nodo = lista->cabeza;
    for (int i = 0; i < lista->longitud; i++) {
        fprintf(fp, "%d ", nodo->valor);
        nodo = nodo->siguiente;
    }
    fprintf(fp, "\n");
    fclose(fp);
}
