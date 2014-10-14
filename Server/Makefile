all: Server

Server: mainS.o core.o utils.o
	gcc -Wall -o Server core.o utils.o mainS.o

mainS.o: mainS.c ../core.h
	gcc -Wall -c mainS.c

core.o: ../core.c ../core.h
	#gcc `pkg-config --cflags --libs glib-2.0` coreS.c
	gcc -Wall -c ../core.c

utils.o: ../utils.c ../utils.o
	gcc -Wall -c ../utils.c

clean:
	rm *.o
