CC = gcc
CFLAGS = -Wall -g
TARGETS = cut srep count

all: $(TARGETS)

cut: cut.o rcut.o
	$(CC) $(CFLAGS) -o cut cut.o rcut.o

cut.o: cut.c
	$(CC) $(CFLAGS) -c cut.c

strcut.o: rcut.c
	$(CC) $(CFLAGS) -c rcut.c

srep: srep.o
	$(CC) $(CFLAGS) -o srep srep.o

srep.o: srep.c
	$(CC) $(CFLAGS) -c srep.c

count: count.o
	$(CC) $(CFLAGS) -o count count.o

count.o: count.c
	$(CC) $(CFLAGS) -c count.c

clean:
	rm -f *.o $(TARGETS)
