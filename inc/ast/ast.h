#ifndef AST_H
#define AST_H

#include <stdlib.h>

typedef enum {
    NODE_NUMBER,
    NODE_STRING,
    NODE_BOOLEAN,
} NodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;
    union {
        int number;
        char *string;
        u_int8_t boolean;
    } value;
};

ASTNode *create_node(NodeType type);

ASTNode *create_number_node(int value);
#endif // AST_H