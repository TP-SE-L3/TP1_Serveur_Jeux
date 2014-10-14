all: Server

Server: mainS.o core.o
	gcc -Wall -o Server core.o mainS.o

mainS.o : mainS.c ../core.h
	gcc -Wall -c mainS.c

core.o : ../core.c ../core.h
	#gcc `pkg-config --cflags --libs glib-2.0` coreS.c
	gcc -Wall -c ../core.c

clean:
	rm *.o
