#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// funcion que maneja las columnas (-c), columnas = columnas que seran extraidas
// del archivo de entrada para ser retornadas en output. (definida mas abajo)
int parse_columns(char *optarg, int **columns, int *num_columns);

// funcion que extrae columnas del archivo de entrada para retornarlas por el de
// salida. (definida mas abajo)
void extract_columns(FILE *in, FILE *out, char sep, int *columns,
                     int num_columns, int all_columns);

// Entrada: recibe 2 strings.
// Salida: retorna 1 si los strings son iguales, caso contrario, retorna 0.
// Descripción: compara ambos strings entregados, si son iguales retorna 1, sino
// 0.
int str_compare(char *s1, char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

int main(int argc, char *argv[]) {
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    char sep = '\t';
    int *columns = NULL;
    int num_columns = 0;
    int all_columns = 1; // default, todas las columnas
    while ((opt = getopt(argc, argv, "i:o:d:c:")) != -1) {
        switch (opt) {
            case 'i':
                input_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'd':
                if (optarg && optarg[0] != '\0') {
                    if (optarg[0] == 'b' && optarg[1] == '\0') {
                        sep = ' ';
                    } else if (optarg[0] == 't' && optarg[1] == '\0') {
                        sep = '\t';
                    } else {
                        sep = optarg[0];
                    }
                } else {
                    fprintf(stderr, "Error: Separador no valido -d\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'c':
                if (parse_columns(optarg, &columns, &num_columns) != 0) {
                    fprintf(stderr, "Error: Columna no valida para -c\n");
                    exit(EXIT_FAILURE);
                }
                all_columns =
                    0; // si se especifican columnas, no usar todas las columnas
                break;
            default:
                fprintf(stderr,
                        "Uso: %s [-i input_file] [-o output_file] [-d "
                        "separator] [-c columnas]\n",
                        argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // abrir archivos entrada y salida --> stdin y stdout (predeterminado)
    FILE *in = input_file ? fopen(input_file, "r") : stdin;
    FILE *out = output_file ? fopen(output_file, "w") : stdout;
    if (!in || !out) {
        fprintf(stderr, "Error al abrir los archivos\n");
        exit(EXIT_FAILURE);
    }
    // extraccion de columnas (luego de pasar correctamente por cada flag)
    extract_columns(in, out, sep, columns, num_columns, all_columns);

    if (in != stdin)
        fclose(in);
    if (out != stdout)
        fclose(out);

    free(columns);
    return EXIT_SUCCESS;
}

// Entradas: recibe un string (columnas a extraer del archivo), un arreglo que
// almacena columnas (**columns apunta a un arreglo)
//           y un puntero (*num_columns) que almacena la cantidad de columnas
//           que cumplen con lo solicitado.
// Salidas:  retorna 0 si la ejecucion fue correcta, por el contrario, retorna 1
// si existen errores. Descripcion: convierte un string en una lista (arreglo)
// de columnas que deben ser extraidas del archivo, si se genera
//              un error retorna un mensaje retroalimentando por consola.
int parse_columns(char *optarg, int **columns, int *num_columns) {
    if (optarg == NULL) {
        fprintf(stderr, "Error: No column specification provided\n");
        return 1;
    }

    char *col = optarg;
    int capacity = 10; // capacidad inicial
    *columns = malloc(capacity * sizeof(int));
    if (*columns == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return 1;
    }

    *num_columns = 0;
    long val;
    char *endptr;

    while (*col) {
        // string a numero
        val = strtol(col, &endptr, 10);
        if (val <= 0) {
            fprintf(stderr, "Error: Numero de columna invalida %ld\n", val);
            free(*columns);
            return 1;
        }

        // aumentar la capacidad si es necesario
        if (*num_columns >= capacity) {
            capacity *= 2;
            int *temp = realloc(*columns, capacity * sizeof(int));
            if (temp == NULL) {
                fprintf(stderr, "Error: realloc fallo \n");
                free(*columns);
                return 1;
            }
            *columns = temp;
        }

        // se almacena la columna en el array
        (*columns)[(*num_columns)++] = (int)val;

        // siguiente valor o fin de la cadena
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

// Entradas: recibe el archivo de entrada (input_file), archivo de salida
// (output_file), el separador de las columnas que se
//           desean extraer, una lista (arreglo) con los numeros y cantidad de
//           columnas a extraer, y finalmente, un flag que indica si todas las
//           columnas deben ser extraidas.
// Salidas:  retorna mediante el archivo de salida, las columnas que fueron
// extraidas dependiendo del separador señalado. Descripción: lee el archivo de
// entrada linea por linea, separando cada una de estas en columnas dependiendo
// del separador solicitado, para luego retornarlas mediante el archivo de
//              salida. Si no se especifican columnas, se retornaran todas las
//              columnas del archivo de entrada al de salida. (all_columns)
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

        // cantidad de columnas en la linea, para verificar que no se pase de
        // largo el separador pedido
        for (; *ptr; ptr++) {
            if (*ptr == sep) {
                col_count++;
            }
        }

        // revisar que la columna este en el rango de la linea
        if (!all_columns) {
            for (int i = 0; i < num_columns; i++) {
                if (columns[i] > col_count) {
                    fprintf(stderr,
                            "Error: La columna %d excede el numero de columnas "
                            "en el archivo "
                            "(%d) en la linea %d\n",
                            columns[i], col_count, line_number);
                    free(line);
                    exit(EXIT_FAILURE);
                }
            }
        }

        // procesamiento normal
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
