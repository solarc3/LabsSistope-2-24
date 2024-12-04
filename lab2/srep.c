#define _GNU_SOURCE // extension GNU libreria STD de C.
                    // [https://man7.org/linux/man-pages/man3/asprintf.3.html]
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>


size_t str_len(char *str);
char *str_str(char *haystack, char *needle);
char *replace_str(char *line, char *target, char *replace);
char *resize_buffer(char *line, int *capacity, int new_size);
int str_compare(char *s1, char *s2);

// Entrada: cadena de caracteres (*str)
// Salida: largo de la cadena de caracteres (size_t)
// Descripcion: retorna el largo de la cadena de caracteres hasta encontrar un
// separador (\0).
size_t str_len(char *str) {
    int size = 0;
    // delimiter es \0
    while (str[size]) {
        size++;
    }
    return size;
}

// Entrada: recibe una linea (*line), cadena a buscar en la linea (*target) y
// cadena que reemplaza el target (*replace). Salida: retorna una nueva cadena
// con los reemplazos realizados, si falla retorna nulo Descripcion: reemplaza
// los target por replace en la linea, calculando el nuevo tamaño de la cadena
// resultante.
char *replace_str(char *line, char *target, char *replace) {
    size_t target_len = str_len(target);
    size_t replace_len = str_len(replace);
    size_t line_len = str_len(line);
    // ver cantidad de veces que sucede el target en la linea, asi se tiene el
    // size necesario con anterioridad
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
    // dest se usa para copiar el resultado, src para recorrer la linea original
    // y found para el target
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

    // se agrega el resto de la linea original despues del ultimo
    // reemplazo del ciclo while, faltaria agregar lo que sigue despues.
    // (termina la cadena con el separador \0)
    while (*src)
        *dest++ = *src++;
    *dest = '\0';

    return result;
}

// Entrada: recibe 2 cadenas de strings (*s1, *s2)
// Salida: retorna 1 si ambas son iguales, caso contrario, retorna 0.
// Descripcion: compara las cadenas de strings hasta que termine (considerar que
// va caracter por caracter),
//              o hasta que se encuentre una diferencia.
int str_compare(char *s1, char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 == *s2;
}

// Entrada: recibe una cadena string(haystack) y la subcadena a buscar en
// esta(needle). Salida: si encuentra la subcadena se retorna la posicion +
// shift(para que parta en ella),
//         si no la encuentra, NULL.
// Descripcion: realiza la busqueda de un substring en un string mas grande,
// mediante fuerza bruta
//              revisa cada caracter y va verificando si parte con el mismo que
//              se busca y luego de eso verifica si coincide en la totalidad, si
//              es que lo hace, retorna el desplazamiento necesario para partir
//              en ella via haystack+shift

char *str_str(char *haystack, char *needle) {
    int position, shift;
    if (needle[0] == '\0') {
        return haystack; // caso que no se pida un substring
    }
    for (shift = 0; haystack[shift] != '\0'; shift++) {
        for (position = 0; haystack[position + shift] == needle[position];
             position++)
            if (needle[position + 1] == '\0') {
                return haystack +
                       shift; // se encontre y ademas termina, se retorna la
                              // posicion en el string grande donde parte el
                              // substring que buscamos.
            }
    }
    return NULL; // no se encuentra, no se retorna nada
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
    // size maximo teorico en bytes pero no negativo, ssize_t es lo mismo pero
    // incluye el -1 para errores.
    char *content = NULL;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
        char *new_content;
        // asprintf permite concadenar el contenido automaticamente, este metodo
        // se encarga de generar memoria suficiente para la cadena resultante,
        // elimina la necesidad de tener que precalcular un buffer.
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



/*
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
*/