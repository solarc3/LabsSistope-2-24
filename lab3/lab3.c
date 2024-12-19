#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex;
pthread_barrier_t barrier;
int huboCambios = 0;
int valoresSinCambio = 0;
int totalNodos;
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
        perror("Error al abrir el archivo de salida");
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

int main(int argc, char *argv[]) {
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    int display_flag = 0;
    while ((opt = getopt(argc, argv, "i:o:D")) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'D':
                display_flag = 1;
                break;
            default:
                fprintf(stderr, "Uso: %s -i input.txt -o output.txt -D\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (input_file == NULL) {
        fprintf(stderr, "Error: Se requiere un archivo de entrada.\n");
        exit(EXIT_FAILURE);
    }
    FILE *fp = fopen(input_file, "r");
    if (fp == NULL) {
        perror("Error al abrir el archivo de entrada");
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
    // Limpiar recursos
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    if (display_flag) {
        imprimirLista(lista);
    }
    limpiarLista(lista);
    free(lista);

    return 0;
}
