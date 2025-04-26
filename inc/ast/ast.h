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

    NODE_RELATION_OP,
    NODE_EQUALITY_OP,
} NodeType;


typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LOGICAL_AND,
    OP_LOGICAL_OR,
} BinaryOpType;

typedef enum {
    OP_UNARY_MINUS,
    OP_LOGICAL_NOT,
} UnaryOpType;

typedef enum {
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
} RelationOpType;
typedef enum {
    OP_EQ,
    OP_NE,
} EqualityOpType;

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

        struct {
            EqualityOpType op;
            ASTNode *left;
            ASTNode *right;
        } equality;
        struct {
            RelationOpType op;
            ASTNode *left;
            ASTNode *right;
        } relation;
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

ASTNode *create_relation_op_node(RelationOpType op, ASTNode *left, ASTNode *right);
ASTNode *create_equality_op_node(EqualityOpType op, ASTNode *left, ASTNode *right);
#endif // AST_H