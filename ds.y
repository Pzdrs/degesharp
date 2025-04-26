%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "inc/ast/ast.h"

// Jako funguje to i bez toho ale hazi to warning
extern int yylex();
void yyerror(const char *str);

// Zatim parsujem po jednom filu, takze nic neresime
int yywrap() {} 

extern FILE *yyin;

ASTNode *root = NULL;
%}
%code requires {
    #include "inc/ast/ast.h"
    typedef enum {
        ASSIGN_OP,
        ADD_ASSIGN_OP,
        SUB_ASSIGN_OP,
        MUL_ASSIGN_OP,
        DIV_ASSIGN_OP
    } AssignmentOperator;
}

%union {
    int num_val;
    char *str_val;
    u_int8_t bool_val;
    ASTNode *node;
    AssignmentOperator assign_op;
}

%token <num_val>
    CONSTANT 

%token <str_val>
    IDENTIFIER
    STRING_LITERAL

%token <bool_val>
    TRUE_LITERAL
    FALSE_LITERAL

%token 
    DECLARE
    TYPE_STRING
    TYPE_INT
    TYPE_BOOL
    INC_OP      "++"
    DEC_OP      "--"
    AND_OP      "a"
    OR_OP       "nebo"
    NOT_OP      "nene"
    EQ_OP       "je"
    NE_OP       "neni"
    LE_OP       "<="
    GE_OP       ">="
    ADD_ASSIGN  "+="
    SUB_ASSIGN  "-="
    MUL_ASSIGN  "*="
    DIV_ASSIGN  "/="

%type 
    <str_val> declaration type_specifier
    <assign_op> assignment_operator
    <node> degesharp statement statement_list atom expression postfix_expression unary_expression assignment_expression multiplicative_expression additive_expression equality_expression relational_expression logical_and_expression logical_or_expression conditional_expression

%%

degesharp: 
    statement_list
    {
        root = $1;
        printf("Parsed successfully!\n");
        print_ast(root);
    }
;

statement_list: 
    statement
|   statement_list statement
;

statement: 
   expression_statement 
|  declaration
;

type_specifier:
    TYPE_INT { $$ = "TYPE_INT"; }
|   TYPE_STRING { $$ = "TYPE_STRING"; }
|   TYPE_BOOL { $$ = "TYPE_BOOL"; }
;

declaration: 
    DECLARE IDENTIFIER[var_name] ':' type_specifier[type] ';'
        {
            printf(
                "\nDeclaring %s variable %s\n", 
                $type, $var_name
            );
        }
|   DECLARE IDENTIFIER[var_name] ':' type_specifier[type] assignment_operator expression ';'
        {
            printf(
                "\nDeclaring and initializating %s variable %s\n", 
                $type, $var_name
            );
        }
;


expression_statement:
    ';'
|   expression ';'

expression:
    assignment_expression
    // Allows for multiple assignments within an expression, e.g., x = 5, y = 10, z = x + y;
|   expression ',' assignment_expression
;

// Allows the use of assignment operator in expressions
/*
c = 10 returns 10, so we can do multiple assignments 
- a = b = c = 10

(c = 10) + 5 returns 15
*/
assignment_expression: 
    conditional_expression
|   unary_expression assignment_operator assignment_expression
    {
        switch ($assignment_operator) {
            case ASSIGN_OP:
                $$ = create_assignment_node($1, $3);
                break;
            case ADD_ASSIGN_OP:
                $$ = create_assignment_node(
                    $1, create_binary_op_node(OP_ADD, $1, $3)
                );
                break;
            case SUB_ASSIGN_OP:
                $$ = create_assignment_node(
                    $1, create_binary_op_node(OP_SUB, $1, $3)
                );
                break;
            case MUL_ASSIGN_OP:
                $$ = create_assignment_node(
                    $1, create_binary_op_node(OP_MUL, $1, $3)
                );
                break;
            case DIV_ASSIGN_OP:
                $$ = create_assignment_node(
                    $1, create_binary_op_node(OP_DIV, $1, $3)
                );
                break;
        }
    }
;

assignment_operator:
    '='         { $$ = ASSIGN_OP; }
|   ADD_ASSIGN  { $$ = ADD_ASSIGN_OP; }
|   SUB_ASSIGN  { $$ = SUB_ASSIGN_OP; }
|   MUL_ASSIGN  { $$ = MUL_ASSIGN_OP; }
|   DIV_ASSIGN  { $$ = DIV_ASSIGN_OP; }
;

// Ternary operator
conditional_expression:
    logical_or_expression
|   logical_or_expression '?' expression ':' conditional_expression
    {
        $$ = create_condition_node($1, $3, $5);
    }
;

logical_or_expression:
    logical_and_expression
|   logical_or_expression OR_OP logical_and_expression
    {
        $$ = create_binary_op_node(OP_LOGICAL_OR, $1, $3);
    }
;

logical_and_expression:
    equality_expression
|   logical_and_expression AND_OP equality_expression
    {
        $$ = create_binary_op_node(OP_LOGICAL_AND, $1, $3);
    }
;

equality_expression:
    relational_expression
|   equality_expression EQ_OP relational_expression
    { $$ = create_equality_op_node(OP_EQ, $1, $3); }
|   equality_expression NE_OP relational_expression
    { $$ = create_equality_op_node(OP_NE, $1, $3); }
;

relational_expression:
    additive_expression
|   relational_expression GE_OP additive_expression
    { $$ = create_relation_op_node(OP_GE, $1, $3); }
|   relational_expression LE_OP additive_expression
    { $$ = create_relation_op_node(OP_LE, $1, $3); }
|   relational_expression '<' additive_expression
    { $$ = create_relation_op_node(OP_LT, $1, $3); }
|   relational_expression '>' additive_expression
    { $$ = create_relation_op_node(OP_GT, $1, $3); }
;

additive_expression:
    multiplicative_expression
|   additive_expression '+' multiplicative_expression
    {
        $$ = create_binary_op_node(
            OP_ADD, $1, $3
        );
    }
|   additive_expression '-' multiplicative_expression
    {
        $$ = create_binary_op_node(
            OP_SUB, $1, $3
        );
    }
;

multiplicative_expression:
    unary_expression
|   multiplicative_expression '*' unary_expression
    {
        $$ = create_binary_op_node(
            OP_MUL, $1, $3
        );
    }
|   multiplicative_expression '/' unary_expression
    {
        $$ = create_binary_op_node(
            OP_DIV, $1, $3
        );
    }
;

unary_expression:
    postfix_expression
|   INC_OP unary_expression
    {
        ASTNode *num_node = create_number_node(1);
        ASTNode *add_node = create_binary_op_node(OP_ADD, $2, num_node);
        $$ = create_assignment_node($2, add_node);
    }
|   DEC_OP unary_expression
    {
        ASTNode *num_node = create_number_node(1);
        ASTNode *sub_node = create_binary_op_node(OP_SUB, $2, num_node);
        $$ = create_assignment_node($2, sub_node);
    }
|   '+' unary_expression 
    { $$ = $2; }
|   '-' unary_expression
    {
        $$ = create_unary_op_node(
            OP_UNARY_MINUS, $2
        );
    }
|   NOT_OP unary_expression
    {
        $$ = create_unary_op_node(
            OP_LOGICAL_NOT, $2
        );
    }
;

postfix_expression:
    atom
|   postfix_expression INC_OP
    {
        ASTNode *num_node = create_number_node(1);
        ASTNode *add_node = create_binary_op_node(OP_ADD, $1, num_node);
        $$ = create_assignment_node($1, add_node);
    }
|   postfix_expression DEC_OP
    {
        ASTNode *num_node = create_number_node(1);
        ASTNode *sub_node = create_binary_op_node(OP_SUB, $1, num_node);
        $$ = create_assignment_node($1, sub_node);
    }
/* Function calls
|   postfix_expression '(' ')'
|   postfix_expression '(' argument_list ')'
*/
;
atom:
    IDENTIFIER
    { $$ = create_var_node($1); }
|   TRUE_LITERAL
    { $$ = create_bool_node($1); } 
|   FALSE_LITERAL
    { $$ = create_bool_node($1); }
|   STRING_LITERAL
    { $$ = create_string_node($1); }
|   CONSTANT
    { $$ = create_number_node($1); }
|   '(' expression ')'
    { $$ = $expression;}
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