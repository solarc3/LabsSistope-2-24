Para el buen funcionamiento del codigo, se dejan estas explicaciones a tener en consideracion antes de su uso:
1. Para el archivo cut.c es necesario tener en cuenta que si se ingresan consultas con los delimitadores ; o / 
   se deben escribir con las comillas simples ';' y '/', dado que la consola interpreta que se estan haciendo llamados
   a diferentes funcionalidades del mismo lenguaje. Para los demas delimitadores, se pueden ingresar de forma normas,
   es decir, sin las comillas simples.
   EJEMPLO: 
        $ ./cut -i input.txt -o output.txt -d ';' -c 2,4
        $ ./cut -i input.txt -o output.txt -d : -c 2,4 

2. Se añadio #define _GNU_SOURCE a srep.c debido a que al compilar se generaba un warning, y esta al ser una 
   extensión de GNU de la libreria STD de C, resuelve el aviso. [https://man7.org/linux/man-pages/man3/asprintf.3.html]