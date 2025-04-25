LEX = lex
CC = gcc

lexer: clean
	@$(LEX) ds.l
	@$(CC) lex.yy.c inc/debug.c 

parser: clean
	bison -d ds.y -Wcounterexamples
	@$(LEX) ds.l
	@# -lm to link the math std lib
	@$(CC) lex.yy.c inc/debug.c ds.tab.c -lm

test: parser
	@for file in tests/*; do \
		echo "Running parser on $$file"; \
		./a.out "$${file}"; \
	done

clean:
	rm -f lex.yy.c lexer
	rm -f ds.tab.* *.out