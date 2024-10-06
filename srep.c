#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// para el string replace
// tokenizar el string de entrada en base al separador
// meter cada token a una lista enlazada
// recorrer la lista y comparar, reemplazar los que sean validos
// concatenar los tokens en un string de salida
// liberar la memoria de la lista

// flags getopt()
// -s string objetivo
// -S string nuevo
// -i archivo de entrada, si no se especifica, stdin
// -o archivo de salida, si no se especifica, stdout

int str_len(char *str) {
    int size = 0;
    // delimiter es \0
    while (str[size] != '\0') {
        size++;
    }
    return size;
}

int str_compare(char *s1, char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 == *s2;
}
// fuerza bruta
char *str_str(char *haystack, char *needle) {
    int position, shift;
    if (needle[0] == "\0") {
        return haystack;
    }
    for (shift = 0; haystack[shift] != "\0"; shift++) {
        for (position = 0; haystack[position + shift] == needle[position];
             position++)
            if (needle[position + 1] == "\0") {
                return haystack + shift;
            }
    }
    return NULL;
}

int main(int argc, char **argv) {
    extern char *optarg;
    extern int optind;
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    char *target = NULL;
    char *replacement = NULL;
    // salidas en caso de no tener archivo de entrada o salida

    while ((opt = getopt(argc, argv, "s:S:i:o:")) != -1) {
        switch (opt) {
            case 's':
                printf("String objetivo: %s\n", optarg);
                target = optarg;
                break;
            case 'S':
                printf("String nuevo: %s\n", optarg);
                replacement = optarg;
                break;
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                printf("Archivo de salida: %s\n", optarg);
                output_file = optarg;
                break;
        }
    }
    if (target == NULL || replacement == NULL) {
        printf("Uso: %s [-s string_objetivo] [-S string_nuevo] [-i "
               "archivo_entrada] [-o archivo_salida]\n",
               argv[0]);
        return 1;
    }
}

char *replace_str(char *line, char *target, char *replace) {
    int target_len = str_len(target);
    int replace_len = str_len(replace);

    int capacity = 128; // initial size
    char *result = malloc(capacity);

    while (line) {
    }
}
