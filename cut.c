#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

// EN ESTE ARCHIVO ESTAN TODOS LOS PROCESOS DE TRABAJO DE LAS FLAGS Y LOS RESPECTIVOS ERRORES QUE PUEDAN GENERAR

// esto es lo que esta en el otro archivo (strcut.c), el cual selecciona las columnas para trabajar en ellas con las distintas flags 
void extraer_columnas(char *input_file, char *output_file, char sep, int *columnas, int num_columna);

// se definen las flags para luego operarlas en el main 
int entrada(char *optarg, char **input_file);
int salida(char *optarg, char **output_file);
int separador(char *optarg, char *sep);
int columna(char *optarg, int *columnas, int *num_columna);

int main(int argc, char *argv[]) {
    int opt;
    char *input_file = NULL;
    char *output_file = NULL;
    char sep = '\t';  // separador (x si no se entiende lol)
    int columnas[50]; // cantidad de columnas que se estiman (max), en este caso dado que el archivo no es tan largo lo deje en 50, pero puede variar a numeros mayores como 1000+
    int num_columna = 0;
    int flag_status = 0; // 0 = ejecutado correctamente (verifica directamente que los casos se esten cumpliendo) -> != 0 = error 

    // mediante getopt se operan las flags definidas arriba, aqui tambien se fijan los prefijos de cada operador (mismo proceso que en menu interactivo)
    while ((opt = getopt(argc, argv, "i:o:d:c:")) != -1) {
        switch (opt) {
            case 'i':
                flag_status = entrada(optarg, &input_file);
                break;
            case 'o':
                flag_status = salida(optarg, &output_file);
                break;
            case 'd':
                flag_status = separador(optarg, &sep);
                break;
            case 'c':
                flag_status = columna(optarg, columnas, &num_columna);
                break;
            default:
                printf("Opcion invalida '-%c'\n", optopt); // si se ingresan mal los valores u opciones, se genera un mensaje de error avisando por terminal
                return 1;
        }

        // si se ingresan mal los valores u opciones o no se ingresa nada, se genera un mensaje de error avisando por terminal
        // por ejemplo si se ingresa esto emitira el mensaje (falta ingreso de columnas): ./cut -i example2.txt -o output4.txt -d : -c 
        if (flag_status != 0) {
            printf("Opcion invalida '-%c'\n", opt);
            return 1;
        }
    }

    // nuevamente error si no se ingresan los parametros solicitados de forma correcta
    if (num_columna == 0) {
        printf("Error al especificar la columna -c\n");
        return 1;
    }

    // extraccion de columnas (luego de pasar correctamente por cada flag)
    printf("Extraccion de columnas de '%s' con el separador '%c'.\n", input_file, sep);
    extraer_columnas(input_file, output_file, sep, columnas, num_columna);

    return 0;
}

// funcion que maneja los archivos de entrada (-i)
int entrada(char *optarg, char **input_file) {
    if (optarg != NULL) {
        *input_file = optarg;
        return 0;
    }
    printf("Error en el archivo de entrada -i\n");
    return 1;
}

// funcion que maneja los archivos de salida (-o)
int salida(char *optarg, char **output_file) {
    if (optarg != NULL) {
        *output_file = optarg;
        return 0;
    }
    printf("Error en el archivo de salida -o\n");
    return 1;
}

// funcion que maneja los separadores (-d), se define cual sera el separador de las columnas
int separador(char *optarg, char *sep) {
    if (optarg != NULL && optarg[0] != '\0') {
        *sep = *optarg;
        return 0;
    }
    printf("Error en el separador solicitado -d\n");
    return 1;
}

// funcion que maneja las columnas (-c), columnas = columnas que serán extraidas del archivo de entrada para ser retornadas en output
int columna(char *optarg, int *columnas, int *num_columna) {
    if (optarg != NULL) {
        char *col = optarg;
        while (*col) {
            columnas[(*num_columna)++] = strtol(col, &col, 10);
            if (*col == ',') col++;
        }
        return 0;
    }
    printf("Error en la extraccion de las columnas -c\n");
    return 1;
}
