all: Client

Client: mainC.o core.o utils.o parser.o
	gcc -Wall -o Client core.o utils.o parser.o mainC.o

mainC.o : mainC.c ../core.h
	gcc -Wall -c mainC.c

core.o : ../core.c ../core.h
	gcc -Wall -c ../core.c

utils.o : ../utils.c ../utils.h
	gcc -Wall -c ../utils.c

parser.o: ../parser.c ../parser.h
	gcc -Wall -c ../parser.c
	
clean:
	rm *.o
