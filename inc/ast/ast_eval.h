#ifndef AST_EVAL_H
#define AST_EVAL_H

#include "ast.h"

typedef union Value {
    int integer;
    char *string;
    bool boolean;
} Value;

void interpret(ASTNode *node);
void eval_statement(ASTNode *node);

Value eval_expression(ASTNode *node);

#endif // AST_EVAL_H