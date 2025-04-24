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
%code requires {
    typedef enum {
        ASSIGN_OP,
        ADD_ASSIGN_OP,
        SUB_ASSIGN_OP,
        MUL_ASSIGN_OP,
        DIV_ASSIGN_OP
    } AssignmentOperator;
}

%union {
    int num;
    char *str;
    AssignmentOperator assign_op;
}

%token <num>
    CONSTANT 

%token <str>
    IDENTIFIER
    STRING_LITERAL

%token 
    DECLARE
    INC_OP      "++"
    DEC_OP      "--"
    AND_OP      "a"
    OR_OP       "nene"
    EQ_OP       "je"
    NE_OP       "neni"
    LE_OP       "<="
    GE_OP       ">="
    ADD_ASSIGN  "+="
    SUB_ASSIGN  "-="
    MUL_ASSIGN  "*="
    DIV_ASSIGN  "/="

%type 
    <num> expr
    <str> var_declaration
    <assign_op> assignment_operator

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
    '='         { $$ = ASSIGN_OP; }
|   ADD_ASSIGN  { $$ = ADD_ASSIGN_OP; }
|   SUB_ASSIGN  { $$ = SUB_ASSIGN_OP; }
|   MUL_ASSIGN  { $$ = MUL_ASSIGN_OP; }
|   DIV_ASSIGN  { $$ = DIV_ASSIGN_OP; }
;

assignment: 
    IDENTIFIER[var_name] assignment_operator[op] expr[value] 
        {
            switch ($op) {
                case ASSIGN_OP:
                    printf("Assigning %d to %s\n", $value, $var_name);
                    break;
                case ADD_ASSIGN_OP:
                    printf("Adding %d to %s\n", $value, $var_name);
                    break;
                case SUB_ASSIGN_OP:
                    printf("Subtracting %d from %s\n", $value, $var_name);
                    break;
                case MUL_ASSIGN_OP:
                    printf("Multiplying %s by %d\n", $var_name, $value);
                    break;
                case DIV_ASSIGN_OP:
                    printf("Dividing %s by %d\n", $var_name, $value);
                    break;
            }
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
    CONSTANT    
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