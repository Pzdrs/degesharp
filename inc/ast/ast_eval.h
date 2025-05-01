#ifndef AST_EVAL_H
#define AST_EVAL_H

#include "ast.h"
#include "types.h"

void interpret(ASTNode *node);
void eval_statement(ASTNode *node);

Value eval_expression(ASTNode *node);

#endif // AST_EVAL_H