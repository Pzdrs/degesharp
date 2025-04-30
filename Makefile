LEX = lex
CC = gcc

ROOT_DIR=$(shell pwd)
INCDIR=$(ROOT_DIR)/inc
INCFILES := $(shell find $(INCDIR) -type f -name "*.c")

lexer: clean
	@$(LEX) ds.l
	@$(CC) lex.yy.c inc/debug.c 

parser: clean
	bison -d ds.y -Wcounterexamples
	flex ds.l
	@$(CC) lex.yy.c ds.tab.c inc/debug.c inc/ast/ast.c inc/ast/ast_eval.c -lm

test: parser
	@for file in tests/*; do \
		echo "Running parser on $$file"; \
		./a.out "$${file}"; \
	done

clean:
	rm -f lex.yy.c lexer
	rm -f ds.tab.* *.out