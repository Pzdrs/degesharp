#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    // Atoms
    NODE_VARIABLE,
    NODE_NUMBER,
    NODE_STRING,
    NODE_BOOLEAN,

    NODE_ASSIGNMENT,
    NODE_BINARY_OP,
    NODE_UNARY_OP,
} NodeType;


typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
} BinaryOpType;

typedef enum {
    OP_UNARY_MINUS,
    OP_UNARY_PLUS,
    OP_LOGICAL_NOT,
} UnaryOpType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    NodeType type;
    union {
        // Atoms
        struct {
            int value;
        } number;
        struct {
            char *value;
        } string;
        struct {
            u_int8_t value;
        } boolean;
        struct {
            char *name;
        } variable;

        struct {
            ASTNode *var;
            ASTNode *value;
        } assignment;

        struct {
            BinaryOpType op;
            ASTNode *left;
            ASTNode *right;
        } binary_op;
        struct {
            UnaryOpType op;
            ASTNode *operand;
        } unary_op;
    } meta;
};

ASTNode *create_node(NodeType type);

ASTNode *create_number_node(int value);
ASTNode *create_string_node(char *string);
ASTNode *create_bool_node(u_int8_t boolean);
ASTNode *create_var_node(char *var_name);

ASTNode *create_assignment_node(ASTNode *var, ASTNode *value);

ASTNode *create_binary_op_node(BinaryOpType op, ASTNode *left, ASTNode *right);
ASTNode *create_unary_op_node(UnaryOpType op, ASTNode *operand);
#endif // AST_H