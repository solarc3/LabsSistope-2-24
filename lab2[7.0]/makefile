CC = gcc
CFLAGS = -Wall -g
TARGETS = cut srep count lab2

all: $(TARGETS)

cut: cut.o
	$(CC) $(CFLAGS) -o cut cut.o

cut.o: cut.c
	$(CC) $(CFLAGS) -c cut.c

srep: srep.o
	$(CC) $(CFLAGS) -o srep srep.o

srep.o: srep.c
	$(CC) $(CFLAGS) -c srep.c

count: count.o
	$(CC) $(CFLAGS) -o count count.o

count.o: count.c
	$(CC) $(CFLAGS) -c count.c

lab2: lab2.o
	$(CC) $(CFLAGS) -o lab2 lab2.o

lab2.o: lab2.c
	$(CC) $(CFLAGS) -c lab2.c

clean:
	rm -f *.o $(TARGETS)
