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
    */
    int opt;
    extern char *optarg;
    extern int optind;
    // https://people.cs.rutgers.edu/~pxk/416/notes/c-tutorials/getopt.html
    while ((opt = getopt(argc, argv, "CLi:")) != -1) {
        switch (opt) {
            case 'C':
                printf("C flag\n");
                break;
            case 'L':
                printf("L flag\n");
                break;
            case 'i':
                printf("i flag\n");
                break;
            default:
                printf("default\n");
                break;
        }
    }
}
