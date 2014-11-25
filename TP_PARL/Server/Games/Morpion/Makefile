CC = gcc
CFLAGS = -W -Wall
EXEC = Morpion
SRC = $(wildcard *.c ../../../*.c)
INCLUDES = $(wildcard *.h ../../../*.h)
OBJ = $(SRC:.c=.o)

all: $(EXEC)

$(EXEC): morpion.o $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ) $(INCLUDES)

main.o: morpion.c
	$(CC) $(CFLAGS) -c $^


%.o: ../%.c
	$(CC) $(CFLAGS) -c $^

echo:
	@echo "$(OBJ)"

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
