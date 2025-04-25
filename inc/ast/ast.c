#include <stdio.h>
#include "ast.h"

ASTNode *create_node(NodeType type) {
    printf("Creating node of type: %d\n", type);
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    return node;
}

ASTNode *create_number_node(int value) {
    ASTNode *node = create_node(NODE_NUMBER);
    node->value.number = value;
    return node;
}