/*obtener cantidad de lineas, palabras o bytes en un archivo csv */
#include <getopt.h>
#include <stdio.h>
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
    FILE *file = stdin;
    while ((opt = getopt(argc, argv, "CLi:")) != -1) {
        switch (opt) {
            case 'C':
                printf("C flag\n");
                flag_C = 1;
                break;
            case 'L':
                printf("L flag\n");
                flag_L = 1;
                break;
            case 'i':
                input_file = optarg;
                break;
            default:
                printf("Uso: %s [-C] [-L] [-i archivo_entrada]\n", argv[0]);
                break;
        }
    }
    // caso de borde, si no se agrega ni L ni C, como wc, se hacen ambas
    // operaciones
    if (input_file) {
        file = fopen(input_file, "r");
        if (file == NULL) {
            printf("Error al abrir el archivo");
            return 1;
        }
    }
    if (!flag_C && !flag_L) {
        flag_C = 1;
        flag_L = 1;
    }
}
// funciones gpetadas las tengo que probar
int count_lines(FILE *file) {
    int lines = 0;
    char c;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            lines++;
        }
    }
    rewind(
        file); // Volver al inicio del archivo para no afectar otras operaciones
    return lines;
}

int count_characters(FILE *file) {
    int chars = 0;
    while (fgetc(file) != EOF) {
        chars++;
    }
    rewind(
        file); // Volver al inicio del archivo para no afectar otras operaciones
    return chars;
}

/*
void count_file(FILE *input, int *lines, int *chars) {
    int c;
    while ((c = fgetc(input)) != EOF) {
        (*chars)++;
        if (c == '\n') {
            (*lines)++;
        }
    }
}
*/
