export CC = gcc
export CFLAGS = -W -Wall
EXEC1 = Client
EXEC2 = Server


all: $(EXEC1) $(EXEC2)
	
.PHONY: clean mrproper listfiles $(EXEC1) $(EXEC2)

$(EXEC1): 
	@(cd $@; make)
	
$(EXEC2):
	@(cd $@; make)


clean:
	@(cd $(EXEC1) && make $@)
	@(cd $(EXEC2) && make $@)
	@rm -rf $(wildcard *.o)
	@echo "Objects(.o) removed: $(wildcard *.o)"
	
mrproper:
	@(cd $(EXEC1) && make $@)
	@(cd $(EXEC2) && make $@)

listfiles:
	@echo "Sources: $(wildcard *.c $(EXEC1)/*.c $(EXEC2)/*.c)"
	@echo "Includes: $(wildcard *.h $(EXEC1)/*.h $(EXEC2)/*.h)"
	@echo "Objets: $(wildcard *.o $(EXEC1)/*.o $(EXEC2)/*.o)"
	@echo "Exec: $(EXEC1) $(EXEC2)"
