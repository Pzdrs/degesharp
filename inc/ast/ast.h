#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    NODE_ROOT,
    NODE_DECLARATION,
    NODE_STATEMENT_LIST,
    NODE_STATEMENT,

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
    NODE_TERNARY_OP,
    NODE_CONDITION,
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

typedef enum {
    T_INT,
    T_STRING,
    T_BOOL,
} VariableType;

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
        struct {
            ASTNode *cond;
            ASTNode *then_case;
            ASTNode *else_case;
        } condition;

        struct {
            char *name;
            VariableType type;
            ASTNode *init;
        } declaration;

        struct {
            struct ASTNode *statement;
            struct ASTNode *next;
        } statement_list;
    } meta;
};

void print_ast(ASTNode *node, int indent);

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
ASTNode *create_conditional_expression_node(ASTNode *cond, ASTNode *then_expr, ASTNode *else_expr);
ASTNode *create_condition_node(ASTNode *cond, ASTNode *then_case, ASTNode *else_case);

ASTNode *create_var_declaration_node(char *name, VariableType type, ASTNode *init);
ASTNode *create_statement_list_node(ASTNode *statement, ASTNode *next);
ASTNode *create_statement_node(ASTNode *statement);
#endif // AST_H