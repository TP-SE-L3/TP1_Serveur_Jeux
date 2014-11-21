CC = gcc
CFLAGS = -W -Wall
EXEC = Client
SRC = $(wildcard *.c ../*.c)
INCLUDES = $(wildcard *.h ../*.h)
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ) $(INCLUDES)

mainC.o : mainC.c
	$(CC) $(CFLAGS) -c $^

%.o: ../%.c
	$(CC) $(CFLAGS) -c $^


.PHONY: clean mrproper listfiles

clean:
	@rm -rf *.o 
	@echo "Objects(.o) removed: $(wildcard *.o)"
	
mrproper:
	@rm -rf  $(EXEC)
	@echo "Exec removed : $(EXEC)"

listfiles:
	@echo "Sources: $(SRC)"
	@echo "Includes: $(INCLUDES)"
	@echo "Objets: $(OBJ)"
	@echo "Exec: $(EXEC)"
