LEX = lex
CC = gcc

lexer: clean
	@$(LEX) ds.l
	@$(CC) lex.yy.c inc/debug.c 

parser: clean
	bison -d ds.y
	@$(LEX) ds.l
	@# -lm to link the math std lib
	@$(CC) lex.yy.c inc/debug.c ds.tab.c -lm

clean:
	rm -f lex.yy.c lexer
	rm -f ds.tab.* *.out