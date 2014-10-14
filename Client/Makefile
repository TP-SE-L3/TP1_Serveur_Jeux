all: Client

Client: mainC.o core.o
	gcc -Wall -o Server core.o mainC.o

mainC.o : mainC.c ../core.h
	gcc -Wall -c mainC.c

core.o : ../core.c ../core.h
	gcc -Wall -c ../core.c

clean:
	rm *.o
