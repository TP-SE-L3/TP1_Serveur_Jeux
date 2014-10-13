all: Client

Client: mainC.o coreC.o
	gcc -Wall -o Client mainC.o coreC.o

mainC.o : mainC.c coreC.h
	gcc -Wall -c mainC.c

coreS.o : coreC.c coreC.h
	gcc -Wall -c coreC.c

clean:
	rm *.o
