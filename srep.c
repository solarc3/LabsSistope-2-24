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
size_t str_len(char *str);
char *str_str(char *haystack, char *needle);
char *replace_str(char *line, char *target, char *replace);
char *resize_buffer(char *line, int *capacity, int new_size);
int str_compare(char *s1, char *s2);

size_t str_len(char *str) {
    int size = 0;
    // delimiter es \0
    while (str[size]) {
        size++;
    }
    return size;
}

char *replace_str(char *line, char *target, char *replace) {
    size_t target_len = str_len(target);
    size_t replace_len = str_len(replace);
    size_t line_len = str_len(line);
    // ver cantidad de veces que sucede el target en la linea, asi se tiene size
    // necesario con anterioridad
    size_t count = 0;
    char *tmp = line;
    while ((tmp = str_str(tmp, target))) {
        ++count;
        tmp += target_len;
    }

    // nuevo size, se multiplica por la cantidad de hits en la linea
    size_t new_len = line_len + count * (replace_len - target_len) + 1;
    char *result = malloc(new_len);
    if (!result)
        return NULL;
    // dest se usa para copiar el resultado, src para recorrer la linea
    // original found para el target
    char *dest = result;
    char *src = line;
    char *found;

    while ((found = str_str(src, target))) {
        size_t prefix_len = found - src;
        // copiar lo que antecede, el prefijo
        for (size_t i = 0; i < prefix_len; ++i)
            *dest++ = *src++;
        // copiar el reemplazo
        for (size_t i = 0; i < replace_len; ++i)
            *dest++ = replace[i];
        // avanzamos en el string
        src += target_len;
    }

    // se agrega el resto de la linea original, como se acaba en el ultimo
    // reemplazo el ciclo while, faltaria agrega lo que sigue despues
    while (*src)
        *dest++ = *src++;
    *dest = '\0';

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
int main(int argc, char **argv) {
    extern char *optarg;
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
        fprintf(stderr,
                "Uso: %s [-s string_objetivo] [-S string_nuevo] [-i "
                "archivo_entrada] [-o archivo_salida]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    FILE *file = input_file ? fopen(input_file, "r") : stdin;
    FILE *out_file = output_file ? fopen(output_file, "w") : stdout;
    if (!file || !out_file) {
        fprintf(stderr, "Error al abrir los archivos\n");
        exit(EXIT_FAILURE);
    }
    // ya que es memoria, no podemos saber el maximo, size_t permite tener el
    // size maximo teorico, en bytes, pero no negativo
    // ssize_t es lo mismo pero incluye el -1 para errores
    char *content = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
        char *new_content;
        // https://www.gnu.org/software/libc/manual/html_node/Dynamic-Output.html
        // asprintf permite concadenar el contenido automaticamente
        // este metodo se encarga de generar memoria suficiente para la cadena
        // resultante, elimina la necesidad de tener que precalcular un buffer
        if (asprintf(&new_content, "%s%s", content ? content : "", line) ==
            -1) {
            fprintf(stderr, "No se pudo asignar memoria\n");
            free(content);
            free(line);
            exit(EXIT_FAILURE);
        }
        free(content);
        content = new_content;
    }
    free(line);
    if (content) {
        char *result = replace_str(content, target, replacement);
        if (result) {
            // if (out_file == stdout)
            // printf("\n");
            fprintf(out_file, "%s", result);
            free(result);
        } else {
            fprintf(stderr, "No se pudo reemplazar\n");
        }
        free(content);
    }
    // como ya no se hacen mas exits, se cierran los streams
    if (file != stdin)
        fclose(file);
    if (out_file != stdout)
        fclose(out_file);
    return EXIT_SUCCESS;
}
