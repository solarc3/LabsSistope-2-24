#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
struct arguments {
    char *input_file;
    char *output_file;
    char separator;
    int *columns;
    int num_columns;
    int all_columns;
};

int parse_columns(char *optarg, int **columns, int *num_columns);
void extract_columns(FILE *in, FILE *out, char sep, int *columns,
                     int num_columns, int all_columns);
int str_compare(const char *s1, const char *s2);
char *str_tok(char *str, const char delim);

static char doc[] = "cut -- Extrae columnas de un archivo de texto.";

// Definición de las opciones
static struct argp_option options[] = {
    {.key = 'i', .arg = "FILE", .flags = 0, .doc = "Archivo de entrada"},
    {.key = 'o', .arg = "FILE", .flags = 0, .doc = "Archivo de salida"},
    {.key = 'd',
     .arg = "SEP",
     .flags = 0,
     .doc = "Separador de columnas (default: tab)"},
    {.key = 'c',
     .arg = "COLUMNS",
     .flags = 0,
     .doc = "Columnas a extraer, separadas por coma"},
    {0}
};

int str_compare(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (*s1 != *s2)
            return 0;
        s1++;
        s2++;
    }
    return (*s1 == '\0' && *s2 == '\0') ? 1 : 0;
}

char *str_tok(char *str, const char delim) {
    static char *next_str = NULL;
    if (str != NULL)
        next_str = str;
    if (next_str == NULL)
        return NULL;
    char *token_start = next_str;
    while (*next_str && *next_str != delim)
        next_str++;

    if (*next_str == delim) {
        *next_str = '\0';
        next_str++;
    } else {
        next_str = NULL;
    }
    return token_start;
}
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 'i':
            arguments->input_file = arg;
            break;
        case 'o':
            arguments->output_file = arg;
            break;
        case 'd':
            if (arg && arg[0] != '\0') {
                if (str_compare(arg, "b") && arg[1] == '\0') {
                    arguments->separator = ' ';
                } else if (str_compare(arg, "t") && arg[1] == '\0') {
                    arguments->separator = '\t';
                } else {
                    arguments->separator = arg[0];
                }
            } else {
                argp_error(state, "Separador no valido para -d");
            }
            break;
        case 'c':
            if (parse_columns(arg, &arguments->columns,
                              &arguments->num_columns) != 0) {
                argp_error(state, "Columna no valida para -c");
            }
            arguments->all_columns = 0;
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
    .doc = doc,
};

int parse_columns(char *optarg, int **columns, int *num_columns) {
    if (optarg == NULL) {
        fprintf(stderr, "Error: No se proporcionaron columnas para -c\n");
        return 1;
    }

    char *col = optarg;
    int capacity = 10; // Capacidad inicial
    *columns = malloc(capacity * sizeof(int));
    if (*columns == NULL) {
        fprintf(stderr, "Error: Fallo en la asignacion de memoria\n");
        return 1;
    }

    *num_columns = 0;
    long val;
    char *endptr;

    while (*col) {
        // Convertir string a número
        val = strtol(col, &endptr, 10);
        if (val <= 0) {
            fprintf(stderr, "Error: Numero de columna invalida %ld\n", val);
            free(*columns);
            return 1;
        }

        // Aumentar la capacidad si es necesario
        if (*num_columns >= capacity) {
            capacity *= 2;
            int *temp = realloc(*columns, capacity * sizeof(int));
            if (temp == NULL) {
                fprintf(stderr, "Error: realloc fallo\n");
                free(*columns);
                return 1;
            }
            *columns = temp;
        }

        // Almacenar la columna en el arreglo
        (*columns)[(*num_columns)++] = (int)val;

        // Siguiente valor o fin de la cadena
        if (*endptr == ',') {
            col = endptr + 1;
        } else if (*endptr != '\0') {
            fprintf(stderr, "Error: Caracter invalido\n");
            free(*columns);
            return 1;
        } else {
            break;
        }
    }

    if (*num_columns == 0) {
        fprintf(stderr, "Error: Columnas especificadas no son validas\n");
        free(*columns);
        return 1;
    }

    return 0;
}
void extract_columns(FILE *in, FILE *out, char sep, int *columns,
                     int num_columns, int all_columns) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_number = 0;

    while ((read = getline(&line, &len, in)) != -1) {
        line_number++;
        int col_count = 1;
        char *ptr = line;

        while (*ptr) {
            if (*ptr == sep)
                col_count++;
            ptr++;
        }

        // Verificar que las columnas existan
        if (!all_columns) {
            for (int i = 0; i < num_columns; i++) {
                if (columns[i] > col_count) {
                    fprintf(stderr,
                            "Error: La columna %d excede el numero de columnas "
                            "del archivo ingresado "
                            "(%d) en la linea %d\n",
                            columns[i], col_count, line_number);
                    free(line);
                    exit(EXIT_FAILURE);
                }
            }
        }

        // Procesamiento normal
        int print_count = 0;
        char *start = line;
        ptr = line;
        col_count = 1;

        while (*ptr) {
            if (*ptr == sep || *ptr == '\n') {
                char temp = *ptr;
                *ptr = '\0';

                int print_column = all_columns;
                if (!all_columns) {
                    for (int i = 0; i < num_columns; i++) {
                        if (col_count == columns[i]) {
                            print_column = 1;
                            break;
                        }
                    }
                }

                if (print_column) {
                    if (print_count > 0)
                        fputc(sep, out);
                    fputs(start, out);
                    print_count++;
                }

                *ptr = temp;
                start = ptr + 1;
                col_count++;
            }
            ptr++;
        }
        fputc('\n', out);
    }

    free(line);
}

int main(int argc, char **argv) {
    struct arguments arguments;

    // default para el struct
    arguments.input_file = NULL;
    arguments.output_file = NULL;
    arguments.separator = '\t';
    arguments.columns = NULL;
    arguments.num_columns = 0;
    arguments.all_columns = 1; // default, todas las columas

    // Parsear las opciones
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    FILE *in = arguments.input_file ? fopen(arguments.input_file, "r") : stdin;
    if (!in) {
        fprintf(stderr, "Error: No se puede abrir el archivo de entrada\n");
        exit(EXIT_FAILURE);
    }

    FILE *out =
        arguments.output_file ? fopen(arguments.output_file, "w") : stdout;
    if (!out) {
        fprintf(stderr, "Error: No se puede abrir el archivo de salida\n");
        if (in != stdin)
            fclose(in);
        exit(EXIT_FAILURE);
    }

    extract_columns(in, out, arguments.separator, arguments.columns,
                    arguments.num_columns, arguments.all_columns);

    if (in != stdin)
        fclose(in);
    if (out != stdout)
        fclose(out);

    free(arguments.columns);

    return EXIT_SUCCESS;
}
