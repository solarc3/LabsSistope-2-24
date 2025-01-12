all: lab3
funciones.o: funciones.c
	gcc -Wall -pthread -c funciones.c -o funciones.o

lab3.o: lab3.c
	gcc -Wall -pthread -c lab3.c -o lab3.o

lab3: funciones.o lab3.o
	gcc -Wall -pthread funciones.o lab3.o -o lab3
clean:
	rm -f *.o
	rm -f lab3
