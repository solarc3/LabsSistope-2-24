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
int str_len(char *str);
char *str_str(char *haystack, char *needle);
char *replace_str(char *line, char *target, char *replace);
char *resize_buffer(char *line, int *capacity, int new_size);
int str_compare(char *s1, char *s2);

int str_len(char *str) {
    int size = 0;
    // delimiter es \0
    while (str[size]) {
        size++;
    }
    return size;
}

char *replace_str(char *line, char *target, char *replace) {
    int target_len = str_len(target);
    int replacement_len = str_len(replace);

    int capacity = 128; // initial size
    char *result = malloc(capacity);

    int result_len = 0;
    char *position = line;
    char *found;
    while ((found = str_str(position, target)) != NULL) {
        // como calculamos linea por linea, calculamos la diferencia para llegar
        // al hit
        //
        int prefix_length = found - position;

        // tenemos espacio para copiar?
        char *new_result =
            resize_buffer(result, &capacity,
                          result_len + prefix_length + replacement_len + 1);
        if (!new_result) {
            free(result);
            return NULL;
        }
        result = new_result;
        // copiamos lo anterior al target
        for (int i = 0; i < prefix_length; i++) {
            result[result_len++] = position[i];
        }
        // copiamos el target
        for (int i = 0; i < replacement_len; i++) {
            result[result_len++] = replace[i];
        }
        // seguimos revisando
        position = found + target_len;
    }
    // si ya no ocurren mas, copiamos el resto
    int suffix_length = str_len(position);
    char *new_result =
        resize_buffer(result, &capacity, result_len + suffix_length + 1);
    if (!new_result) {
        free(result);
        return NULL;
    }
    result = new_result;
    for (int i = 0; i < suffix_length; i++) {
        result[result_len++] = position[i];
    }
    result[result_len] = '\0'; // paso final de una linea
    return result;
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
    if (needle[0] == '\0') {
        return haystack;
    }
    for (shift = 0; haystack[shift] != '\0'; shift++) {
        for (position = 0; haystack[position + shift] == needle[position];
             position++)
            if (needle[position + 1] == '\0') {
                return haystack + shift;
            }
    }
    return NULL;
}

char *resize_buffer(char *line, int *capacity, int new_size) {
    while (*capacity < new_size) {
        *capacity *= 2;
    }
    return realloc(line, *capacity);
}

int main(int argc, char **argv) {
    extern char *optarg;
    extern int optind;
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    char *target = NULL;
    char *replacement = NULL;

    while ((opt = getopt(argc, argv, "s:S:i:o:")) != -1) {
        switch (opt) {
            case 's':
                target = optarg;
                break;
            case 'S':
                replacement = optarg;
                break;
            case 'i':
                input_file = optarg;
                break;
            case 'o':
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

    FILE *file = stdin;
    FILE *out_file = stdout;

    if (input_file) {
        file = fopen(input_file, "r");
        if (!file) {
            printf("Error al abrir el archivo de entrada");
            return 1;
        }
    }

    if (output_file) {
        out_file = fopen(output_file, "w");
        if (!out_file) {
            printf("Error al abrir el archivo de salida");
            if (file != stdin) {
                fclose(file);
            }
            return 1;
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }

        char *line_replace = replace_str(line, target, replacement);
        if (line_replace) {
            if (out_file == stdout) {
                puts(line_replace);
            } else {
                fprintf(out_file, "%s\n", line_replace);
            }
            free(line_replace);
        }
    }
    free(line);

    if (file != stdin) {
        fclose(file);
    }
    if (out_file != stdout) {
        fclose(out_file);
    }
    return 0;
}
