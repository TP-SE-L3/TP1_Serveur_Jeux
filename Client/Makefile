all: Client

Client: mainC.o core.o utils.o
	gcc -Wall -o Client core.o utils.o mainC.o

mainC.o : mainC.c ../core.h
	gcc -Wall -c mainC.c

core.o : ../core.c ../core.h
	gcc -Wall -c ../core.c

utils.o : ../utils.c ../utils.h
	gcc -Wall -c ../utils.c
	
clean:
	rm *.o
