#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// contabiliza lineas (definida mas abajo)
int count_lines(FILE *file);

// contabiliza caracteres (definida mas abajo)
int count_characters(FILE *file);

// Entrada: recibe 3 punteros -> (*file) con el que se contaran lineas y
// caracteres,
//          (*lines) almacena lineas encontradas y (*chars) almacena caracteres
//          contabilizados.
// Salida: actualiza punteros.
// Descripcion: actualiza los valores de los punteros (*lines y *chars).
void count_lines_and_chars(FILE *file, int *lines, int *chars) {
    int character;
    *lines = 0;
    *chars = 0;

    while ((character = fgetc(file)) != EOF) {
        (*chars)++;
        if (character == '\n') {
            (*lines)++;
        }
    }
}

// se procesa lo ingresado por consola mediante getopt, contabilizando el numero
// de lineas (-L) y caracteres (-C; se consideran las mayusculas, minusculas,
// vocales y tildes) para guardarlo en un archivo de salida, si no se especifica
// archivo (-i) se usa stdin.
int main(int argc, char **argv) {
    int opt;
    int flag_L = 0;
    int flag_C = 0;
    extern char *optarg;
    extern int optind;
    char *input_file = NULL;
    int used_flags[128] = {0};
    while ((opt = getopt(argc, argv, "CLi:")) != -1) {
        if (used_flags[opt]) {
            fprintf(stderr, "Error: Opcion '-%c' ya fue especificada!\n", opt);
            exit(EXIT_FAILURE);
        }
        used_flags[opt] = 1;
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

    // caso de borde, si no se agrega ni L ni C, como wc, se hacen ambas
    // operaciones
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


/*
#include <argp.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
// contabiliza lineas (definida mas abajo)
int count_lines(FILE *file);

// contabiliza caracteres (definida mas abajo)
int count_characters(FILE *file);

// Entrada: recibe 3 punteros -> (*file) con el que se contaran lineas y
// caracteres,
//          (*lines) almacena lineas encontradas y (*chars) almacena caracteres
//          contabilizados.
// Salida: actualiza punteros.
// Descripcion: actualiza los valores de los punteros (*lines y *chars).
void count_lines_and_chars(FILE *file, int *lines, int *chars) {
    int character;
    *lines = 0;
    *chars = 0;

    while ((character = fgetc(file)) != EOF) {
        (*chars)++;
        if (character == '\n') {
            (*lines)++;
        }
    }
}

// se procesa lo ingresado por consola mediante getopt, contabilizando el numero
// de lineas (-L) y caracteres (-C; se consideran las mayusculas, minusculas,
// vocales y tildes) para guardarlo en un archivo de salida, si no se especifica
// archivo (-i) se usa stdin.

struct arguments {
    int flag_L;
    int flag_C;
    char *input_file;
};
static struct argp_option options[] = {
    {.key = 'L', .arg = 0, .flags = 0, .doc = "Contar líneas"},
    {.key = 'C', .arg = 0, .flags = 0, .doc = "Contar caracteres"},
    {.key = 'i', .arg = "FILE", .flags = 0, .doc = "Archivo de entrada"},
    {0}
};
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'L':
            arguments->flag_L = 1;
            break;
        case 'C':
            arguments->flag_C = 1;
            break;
        case 'i':
            arguments->input_file = arg;
            break;
        case ARGP_KEY_ARG:
            return 0;
        case ARGP_KEY_END:
            return 0;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}
static struct argp argp = {
    .options = options,
    .parser = parse_opt,
    .args_doc = 0,
};

int main(int argc, char **argv) {
    struct arguments arguments;
    arguments.flag_L = 0;
    arguments.flag_C = 0;
    arguments.input_file = NULL;

    // Parsear las opciones
    // "error_t argp_parse (const struct argp *argp, int argc, char **argv,
    // unsigned flags, int *arg_index, void *input)"
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    // Abrir el archivo de entrada o usar stdin
    FILE *file =
        arguments.input_file ? fopen(arguments.input_file, "r") : stdin;
    if (arguments.input_file && !file) {
        fprintf(stderr, "Error al abrir el archivo\n");
        exit(EXIT_FAILURE);
    }

    // Caso de borde: si no se agregan ni L ni C, contar ambas
    if (!arguments.flag_C && !arguments.flag_L) {
        arguments.flag_C = 1;
        arguments.flag_L = 1;
    }

    int total_lines = 0;
    int total_chars = 0;

    count_lines_and_chars(file, &total_lines, &total_chars);

    // Imprimir resultados según las banderas
    if (arguments.flag_L && arguments.flag_C) {
        printf("%d %d\n", total_lines, total_chars);
    } else if (arguments.flag_L) {
        printf("%d\n", total_lines);
    } else if (arguments.flag_C) {
        printf("%d\n", total_chars);
    }

    if (file != stdin) {
        fclose(file);
    }

    return EXIT_SUCCESS;
}
*/