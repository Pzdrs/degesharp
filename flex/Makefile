LEX = lex
CC = gcc
OUTPUT = a.out

all: $(OUTPUT)
	./$(OUTPUT)

$(OUTPUT): ds.l
	@$(LEX) ds.l
	@$(CC) lex.yy.c -o $(OUTPUT)

clean:
	rm -f lex.yy.c $(OUTPUT)