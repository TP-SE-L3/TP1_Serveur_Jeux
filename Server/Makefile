all: Server

Server: mainS.o coreS.o
	gcc -Wall -o Server mainS.o coreS.o

mainS.o : mainS.c coreS.h
	gcc -Wall -c mainS.c

coreS.o : coreS.c coreS.h
	gcc -Wall -c coreS.c

clean:
	rm *.o
