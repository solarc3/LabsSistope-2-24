/*obtener cantidad de lineas, palabras o bytes en un archivo csv */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

int count_lines(FILE *file);
int count_characters(FILE *file);
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
int main(int argc, char **argv) {
    /*
    - se indica un archivo de entrada,sino, sera por stdin
    para cualquier caracter se consideran las mayus y minusculas y vocales tilde
    tbm parametros para el getotp

    - se guarda en un archivo de slaida

    - flags getopt

        -C cuenta el numero de caracteres
        -L cuenta el numero de lineas
        -i nombre del archivo de entrada, si no se especifica se asume stdin

        considerar el comando wc como comportamiento default

    - casos de borde
        - que pasa si no agrego cada flag?
            - con -i es std in
            - TODO: que pasa con -L
            - TODO: que pasa con -C
            - se sabe que si se agregan L y C tiene cierto orden y cada uno solo
    solo se ejecuta ese
    */
    int opt;
    int flag_L = 0;
    int flag_C = 0;
    extern char *optarg;
    extern int optind;
    // https://people.cs.rutgers.edu/~pxk/416/notes/c-tutorials/getopt.html
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
        // https://www.gnu.org/software/libc/manual/html_node/Termination-Internals.html
        // EXIT_FAILURE es lo mismo que poner 1
        exit(EXIT_FAILURE);
    }

    // caso de borde, si no se agrega ni L ni C, como wc, se hacen ambas
    // operaciones
    if (!flag_C && !flag_L) {
        flag_C = 1;
        flag_L = 1;
    }

    int total_lines = 0;
    int total_chars = 0;

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

int count_characters(FILE *file) {
    int chars = 0;
    int c;
    while ((c = getc(file)) != EOF) {
        chars++;
    }
    fflush(file);
    return chars;
}
