%{
#include <stdio.h>
#include <string.h>

// Jako funguje to i bez toho ale hazi to warning
extern int yylex();
void yyerror(const char *str);

// Zatim parsujem po jednom filu, takze nic neresime
int yywrap() {} 

extern FILE *yyin;
%}

%union {
    int num;
    char *str;
}

%token 
    <num>   INTEGER
    <str>   IDENTIFIER
            INC_OP      "++"
            DEC_OP      "--"
            AND_OP      "a"
            OR_OP       "nene"
            EQ_OP       "je"
            NE_OP       "neni"
            LE_OP       "<="
            GE_OP       ">="

%type <num> expr

%left '+' '-' '*' '/'

%%

expr: expr '+' expr { $$ = $1 + $3; }
    | expr '-' expr { $$ = $1 - $3; }
    | expr '*' expr { $$ = $1 * $3; }
    | expr '/' expr { $$ = $1 / $3; }
    | INTEGER       { $$ = $1; }
    ;

%%

// Parse error handler
void yyerror(const char *str) {
    fprintf(stderr,"error: %s\n", str);
}

int main(int argc, char **argv)
{
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening the file");
        return 1;
    }

    yyin = file;
    yyparse();
    fclose(file);
}