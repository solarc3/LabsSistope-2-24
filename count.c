#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// contabiliza lineas (definida mas abajo)
int count_lines(FILE *file);

// contabiliza caracteres (definida mas abajo)
int count_characters(FILE *file);

// Entrada: recibe 3 punteros -> (*file) con el que se contaran lineas y caracteres, 
//          (*lines) almacena lineas encontradas y (*chars) almacena caracteres contabilizados.
// Salida: actualiza punteros.
// Descripcion: actualiza los valores de los punteros (*lines y *chars).
void count_lines_and_chars(FILE *file, int *lines, int *chars) {
    int c;
    *lines = 0;
    *chars = 0;

    while ((c = fgetc(file)) != EOF) {
        (*chars)++;
        if (c == '\n') {
            (*lines)++;
        }
    }
}

// se procesa lo ingresado por consola mediante getopt, contabilizando el numero de lineas (-L) y 
// caracteres (-C; se consideran las mayusculas, minusculas, vocales y tildes) para guardarlo en 
// un archivo de salida, si no se especifica archivo (-i) se usa stdin.
int main(int argc, char **argv) {
    int opt;
    int flag_L = 0;
    int flag_C = 0;
    extern char *optarg;
    extern int optind;
    char *input_file = NULL;
    while ((opt = getopt(argc, argv, "CLi:")) != -1) {
        switch (opt) {
            case 'C':
                flag_C = 1;
                break;
            case 'L':
                flag_L = 1;
                break;
            case 'i':
                input_file = optarg;
                break;
            default:
                fprintf(stderr, "Uso: %s [-C] [-L] [-i archivo_entrada]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    FILE *file = input_file ? fopen(input_file, "r") : stdin;
    if (!file) {
        fprintf(stderr, "Error al abrir el archivo");
        // EXIT_FAILURE es lo mismo que poner 1
        exit(EXIT_FAILURE);
    }

    // caso de borde, si no se agrega ni L ni C, como wc, se hacen ambas operaciones
    if (!flag_C && !flag_L) {
        flag_C = 1;
        flag_L = 1;
    }

    int total_lines = 0;
    int total_chars = 0;

    // cuenta lineas y caracteres
    count_lines_and_chars(file, &total_lines, &total_chars);

    if (flag_L && flag_C) {
        printf("%d %d\n", total_lines, total_chars);
    } else if (flag_L) {
        printf("%d\n", total_lines);
    } else if (flag_C) {
        printf("%d\n", total_chars);
    }

    if (file != stdin) {
        fclose(file);
    }

    return EXIT_SUCCESS;
}

// Entrada: recibe un puntero (*file) con el que se contaran las lineas.
// Salida: numero de lineas del archivo.
// Descripcion: recorre el archivo contabilizando uno por uno las lineas. (= \n)
int count_lines(FILE *file) {
    int lines = 0;
    int c;
    while ((c = getc(file)) != EOF) {
        if (c == '\n') {
            lines++;
        }
    }
    fflush(file);
    return lines;
}

// Entrada: recibe un puntero (*file) con el que se contaran los caracteres.
// Salida: numero de caracteres en el archivo.
// Descripcion: recorre el archivo uno por uno incrementando el contador de caracteres.
int count_characters(FILE *file) {
    int chars = 0;
    int c;
    while ((c = getc(file)) != EOF) {
        chars++;
    }
    fflush(file);
    return chars;
}
