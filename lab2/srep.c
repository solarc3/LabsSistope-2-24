#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#define _GNU_SOURCE // Extensión GNU librería STD de C.

// Prototipos de funciones
size_t str_len(char *str);
char *str_str(char *haystack, char *needle);
char *replace_str(char *line, char *target, char *replace);
int str_compare(char *s1, char *s2);

// Estructura para almacenar los argumentos
struct arguments {
    char *target;
    char *replacement;
    char *input_file;
    char *output_file;
    int target_set;
    int replacement_set;
};

// Documentación del programa
static char doc[] = "replace -- Reemplaza cadenas de texto en un archivo.";

// Definición de las opciones sin el parámetro `name`
static struct argp_option options[] = {
    {.key = 's', "STRING_OBJETIVO", 0, "Cadena a buscar para reemplazar"},
    {.key = 'S', "STRING_NUEVO", 0, "Cadena con la que reemplazar"},
    {.key = 'i', "FILE", 0, "Archivo de entrada"},
    {.key = 'o', "FILE", 0, "Archivo de salida"},
    {0}
};

// Función para comparar dos cadenas
int str_compare(char *s1, char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 == '\0' && *s2 == '\0';
}

// Función para obtener la longitud de una cadena
size_t str_len(char *str) {
    size_t size = 0;
    while (str[size]) {
        size++;
    }
    return size;
}

// Función para buscar un substring en una cadena (similar a strstr)
char *str_str(char *haystack, char *needle) {
    if (!*needle)
        return haystack;
    for (; *haystack; haystack++) {
        if (*haystack == *needle) {
            char *h, *n;
            for (h = haystack, n = needle; *n && *h == *n; h++, n++)
                ;
            if (!*n)
                return haystack;
        }
    }
    return NULL;
}

// Función para reemplazar substrings en una cadena
char *replace_str(char *line, char *target, char *replace) {
    size_t target_len = str_len(target);
    size_t replace_len = str_len(replace);
    size_t line_len = str_len(line);
    size_t count = 0;
    char *tmp = line;

    while ((tmp = str_str(tmp, target))) {
        count++;
        tmp += target_len;
    }

    size_t new_len = line_len + count * (replace_len - target_len) + 1;
    char *result = malloc(new_len);
    if (!result)
        return NULL;

    char *dest = result;
    char *src = line;
    char *found;

    while ((found = str_str(src, target))) {
        size_t prefix_len = found - src;
        for (size_t i = 0; i < prefix_len; ++i)
            *dest++ = *src++;
        for (size_t i = 0; i < replace_len; ++i)
            *dest++ = replace[i];
        src += target_len;
    }

    while (*src)
        *dest++ = *src++;
    *dest = '\0';

    return result;
}

// Función para procesar cada opción
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch (key) {
        case 's':
            arguments->target = arg;
            arguments->target_set = 1;
            break;
        case 'S':
            arguments->replacement = arg;
            arguments->replacement_set = 1;
            break;
        case 'i':
            arguments->input_file = arg;
            break;
        case 'o':
            arguments->output_file = arg;
            break;
        case ARGP_KEY_END:
            if (!arguments->target_set || !arguments->replacement_set) {
                fprintf(stderr, "Error: Las opciones -s (STRING_OBJETIVO) y -S "
                                "(STRING_NUEVO) son requeridas.\n");
                argp_usage(state);
            }
            return 0;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

// Definición de argp
static struct argp argp = {
    .options = options,
    .parser = parse_opt,
    .doc = doc,
};

// Función principal
int main(int argc, char **argv) {
    struct arguments arguments;

    // Inicializar valores por defecto
    arguments.target = NULL;
    arguments.replacement = NULL;
    arguments.input_file = NULL;
    arguments.output_file = NULL;
    arguments.target_set = 0;
    arguments.replacement_set = 0;

    // Parsear las opciones
    // agregar ARGP_NO_ERRS si no quiero errores, como flag
    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    // Abrir archivos de entrada y salida
    FILE *file =
        arguments.input_file ? fopen(arguments.input_file, "r") : stdin;
    FILE *out_file =
        arguments.output_file ? fopen(arguments.output_file, "w") : stdout;
    if ((arguments.input_file && !file) ||
        (arguments.output_file && !out_file)) {
        fprintf(stderr, "Error al abrir los archivos\n");
        exit(EXIT_FAILURE);
    }

    char *content = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read_len;

    while ((read_len = getline(&line, &len, file)) != -1) {
        char *new_content;
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
        char *result =
            replace_str(content, arguments.target, arguments.replacement);
        if (result) {
            fprintf(out_file, "%s", result);
            free(result);
        } else {
            fprintf(stderr, "No se pudo reemplazar\n");
        }
        free(content);
    }

    // Cerrar archivos si no son stdin/stdout
    if (file != stdin)
        fclose(file);
    if (out_file != stdout)
        fclose(out_file);

    return EXIT_SUCCESS;
}
