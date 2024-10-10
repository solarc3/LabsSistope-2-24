#include <stdio.h>
#include <stdlib.h>

// EN ESTE OTRO ARCHIVO ESTAN LAS LECTURAS DE ARCHIVOS + EXTRACCION DE COLUMNAS (invocado en cut.c)

// funcion para extraer columnas del archivo de entrada y retornarlos por el output_file, se especifican mediante consola
void extraer_columnas(char *input_file, char *output_file, char sep, int *columna, int num_columna) {
    FILE *in = stdin, *out = stdout;

    // MANEJO DE ARCHIVOS: si estos no se especifican se utilizará stdin o stdout

    // abrir archivo de entrada, si este no se encuentra o se ingresa mal, retorna un mensaje con el error
    if (input_file) {
        in = fopen(input_file, "r");
        if (!in) {
            printf("Error en el archivo de entrada, no fue posible abrirlo");
            return;
        }
    }

    // abrir archivo de salida, si este no se encuentra o no se genera, retorna un mensaje con el error,
    // además, si el archivo ya se encuentra abierto, se cerrará
    if (output_file) {
        out = fopen(output_file, "w");
        if (!out) {
            printf("Error en el archivo de salida, no fue posible abrirlo");
            if (in && in != stdin) fclose(in);
            return;
        }
    }

    char *line = NULL;  
    size_t len = 0;     
    ssize_t read;       
    int contador, print_flag; 

    // se lee el archivo linea por linea para contabilizar las columnas o elementos de la linea que se esta leyendo (contador --> incrementa al encontrar un separador), 
    // iniciando en 1, luego se lleva un registro de las columnas que ya fueron impresas o revisadas (columnas_recorridas, ++), en base
    // al separador solicitado (-d) para luego retornarlas mediante el archivo de salida (-o).
    while ((read = getline(&line, &len, in)) != -1) {
        contador = 1;
        print_flag = 0; // columnas que deben imprimirse respecto a lo solicitado por consola
        int columnas_recorridas = 0; 
        char *first = line; 
        char *ptr = line;

        while (*ptr) {
            if (*ptr == sep || *ptr == '\n') {
                *ptr = '\0';

                if (contador == columna[print_flag]) {
                    if (columnas_recorridas) {
                        fputc(sep, out); // escritura del separador por cada columna valida procesada
                    }
                    fputs(first, out);
                    columnas_recorridas = 1;
                    print_flag++;

                    if (print_flag == num_columna) {
                        break;
                    }
                }

                first = ptr + 1;
                contador++;
            }
            ptr++;
        }

        fprintf(out, "\n");
    }

}


