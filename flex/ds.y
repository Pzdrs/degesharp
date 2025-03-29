%{
#include <stdio.h>
#include <string.h>
#include <math.h>

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
            DECLARE
            INC_OP      "++"
            DEC_OP      "--"
            AND_OP      "a"
            OR_OP       "nene"
            EQ_OP       "je"
            NE_OP       "neni"
            LE_OP       "<="
            GE_OP       ">="

%type 
    <num> expr
    <str> var_declaration

%left '+' '-' 
%left '*' '/'
%precedence NEG /* unary minus (negation), nonassociative -> use %precedence */
%right '^'      /* exponentiation, highest priority */

%%

degesharp: 
    statement_list
;

statement_list: 
    statement ';'
|   statement ';' statement_list
;

statement: 
    assignment
|   var_declaration 
;

assignment_operator:
    '='
;

assignment: 
    IDENTIFIER assignment_operator expr 
        {
            printf(
                "\nAssigning the value '%d' to variable %s\n", 
                $3, $1
            );
        }
;

var_declaration: 
    DECLARE IDENTIFIER[var_name] 
        {
            printf(
                "\nDeclaring variable %s\n", 
                $var_name
            );
        }
|   DECLARE IDENTIFIER[var_name] assignment_operator expr[var_value]     
        {
            printf(
                "\nDeclaring and initializating variable %s to '%d'\n", 
                $var_name, $var_value
            );
        }
;

expr:
    INTEGER             /* $$ = $1 by default */
|   expr '+' expr       { $$ = $1 + $3; }
|   expr '-' expr       { $$ = $1 - $3; }
|   expr '*' expr       { $$ = $1 * $3; }
|   expr '/' expr       
        {
            if ($3) {
                // If the divisor is nonzero simply divide
                $$ = $1 / $3;
            } else {
                fprintf(
                    stderr, "division by zero"
                );
            }
        }
|   expr '^' expr       { $$ = pow($1, $3); }

    /* zavorkovani */
|   '-' expr %prec NEG  { $$ = -$2; }
    /* negace */
|   '(' expr ')'        { $$ = $2; }
;

%%

// Parse error handler
void yyerror(const char *str) {
    fprintf(stderr,"error: %s\n", str);
}

int main(int argc, char **argv) {
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [source_file]\n", argv[0]);
        return 1;
    }

    if (argc == 2) {
        FILE *file = fopen(argv[1], "r");
        if (!file) {
            perror("Error opening the file");
            return 1;
        }
        yyin = file;
    } else {
        yyin = stdin;  // Read from standard input if no file is provided
    }

    int result = yyparse();

    if (yyin != stdin) {
        fclose(yyin);
    }
    
    return result;
}