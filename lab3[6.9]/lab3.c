#include "funciones.h"
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// variables globales 
pthread_mutex_t mutex;
pthread_barrier_t barrier;
int huboCambios = 0;
int valoresSinCambio = 0;
int totalNodos;

// Entrada: puntero a nodo de la lista
// Salida: no hay salida en especifico, a modo general, se retorna cuando no hay
//         mas cambios en los valores de los nodos
// Descripcion: actualiza los valores de los nodos en la lista
void *threadFunc(void *args) {
    Nodo *nodo = (Nodo *)args;
    while (1) {
        int cambiolocal = 0;

        pthread_barrier_wait(&barrier);

        pthread_mutex_lock(&mutex);
        if (nodo->anterior->valor > nodo->valor) {
            nodo->valor = nodo->anterior->valor;
            cambiolocal = 1;
        }
        pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier);

        pthread_mutex_lock(&mutex);
        if (!cambiolocal) {
            valoresSinCambio++;
        }
        pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier);

        if (valoresSinCambio == totalNodos) {
            return NULL;
        }

        valoresSinCambio = 0;

        pthread_barrier_wait(&barrier);
    }
}

// codigo principal  
int main(int argc, char *argv[]) {
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    int display_flag = 0;

    // contadores para que no se usen mas de una vez -.....-
    int i_count = 0, o_count = 0, D_count = 0;

    while ((opt = getopt(argc, argv, "i:o:D")) != -1) {
        switch (opt) {
            case 'i':
                i_count++;
                if (i_count > 1) {
                    fprintf(stderr, "Error: La opcion -i no puede usarse más "
                                    "de una vez.\n");
                    exit(EXIT_FAILURE);
                }
                input_file = optarg;
                break;
            case 'o':
                o_count++;
                if (o_count > 1) {
                    fprintf(stderr, "Error: La opcion -o no puede usarse más "
                                    "de una vez.\n");
                    exit(EXIT_FAILURE);
                }
                output_file = optarg;
                break;
            case 'D':
                D_count++;
                if (D_count > 1) {
                    fprintf(stderr, "Error: La opcion -D no puede usarse más "
                                    "de una vez.\n");
                    exit(EXIT_FAILURE);
                }
                display_flag = 1;
                break;
            default:
                fprintf(stderr, "Uso: %s -i input.txt (-o output.txt | -D)\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (input_file == NULL) {
        fprintf(stderr, "Error: Se necesita un archivo de entrada, usar -i.\n");
        exit(EXIT_FAILURE);
    }

    if (output_file == NULL && display_flag == 0) {
        fprintf(stderr, "Error: Debe usar al menos una de las opciones "
                        "salida (-o salida.txt o -D).\n");
        exit(EXIT_FAILURE);
    }

    // abrir archivo
    FILE *fp = fopen(input_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir el archivo de entrada: %s\n",
                input_file);
        exit(EXIT_FAILURE);
    }

    int longitud;
    Lista *lista = crearLista(fp, &longitud);
    fclose(fp);

    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, lista->longitud);
    totalNodos = lista->longitud;
    valoresSinCambio = 0;

    pthread_t threads[lista->longitud];
    Nodo *nodo = lista->cabeza;
    for (int i = 0; i < lista->longitud; i++) {
        pthread_create(&threads[i], NULL, threadFunc, (void *)nodo);
        nodo = nodo->siguiente;
    }
    for (int i = 0; i < lista->longitud; i++) {
        pthread_join(threads[i], NULL);
    }
    if (output_file != NULL) {
        escribirEnArchivo(lista, output_file);
    }

    // limpiar
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);

    if (display_flag) {
        imprimirLista(lista);
    }

    limpiarLista(lista);
    free(lista);

    return 0;
}
