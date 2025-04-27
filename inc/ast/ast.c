#include <stdio.h>
#include "ast.h"

char *binary_op_name(BinaryOpType op) {
    switch (op) {
        case OP_ADD: return "addition";
        case OP_SUB: return "subtraction";
        case OP_MUL: return "multiplication";
        case OP_DIV: return "division";
        case OP_LOGICAL_AND: return "logical AND";
        case OP_LOGICAL_OR: return "logical OR";
        default: return "unknown operation";
    }
}

char *unary_op_name(UnaryOpType op) {
    switch (op) {
        case OP_UNARY_MINUS: return "unary minus";
        case OP_LOGICAL_NOT: return "logical NOT";
        default: return "unknown operation";
    }
}

char *relation_op_name(RelationOpType op) {
    switch (op) {
        case OP_LT: return "less than";
        case OP_GT: return "greater than";
        case OP_LE: return "less than or equal to";
        case OP_GE: return "greater than or equal to";
        default: return "unknown operation";
    }
}

char *equality_op_name(EqualityOpType op) {
    switch (op) {
        case OP_EQ: return "equal to";
        case OP_NE: return "not equal to";
        default: return "unknown operation";
    }
}

char *variable_type_name(VariableType type) {
    switch (type) {
        case T_INT: return "integer";
        case T_STRING: return "string";
        case T_BOOL: return "boolean";
        default: return "unknown type";
    }
}

void print_ast(ASTNode *node, int indent) {
    if (!node) return;

    if (node->type != NODE_STATEMENT_LIST) {
        for (int i = 0; i < indent; i++) {
            printf("  ");
        }
    }

    switch (node->type) {
        case NODE_STATEMENT_LIST:
            print_ast(node->meta.statement_list.statement, indent);
            print_ast(node->meta.statement_list.next, indent);
            break;
        case NODE_BINARY_OP:
            printf("Binary operation node (%s)\n", binary_op_name(node->meta.binary_op.op));
            print_ast(node->meta.binary_op.left, indent + 1);
            print_ast(node->meta.binary_op.right, indent + 1);
            break;
        case NODE_DECLARATION:
            printf("Declaration node (var_name: %s, type: %s)\n", node->meta.declaration.name, variable_type_name(node->meta.declaration.type));
            print_ast(node->meta.declaration.init, indent + 1);
            break;
        case NODE_ASSIGNMENT:
            printf("Assignment node\n");
            print_ast(node->meta.assignment.var, indent + 1);
            print_ast(node->meta.assignment.value, indent + 1);
            break;
        case NODE_NUMBER:
            printf("Number node: %d\n", node->meta.number.value);
            break;
        case NODE_VARIABLE:
            printf("Variable node: %s\n", node->meta.variable.name);
            break;
        default:
            printf("Unknown node type: %d\n", node->type);
    }
}

ASTNode *create_node(NodeType type) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    return node;
}

ASTNode *create_number_node(int value) {
    printf("\nCreating a number node with value: %d\n", value);
    ASTNode *node = create_node(NODE_NUMBER);
    node->meta.number.value = value;
    return node;
}

ASTNode *create_string_node(char *string) {
    printf("\nCreating a string node with value: %s\n", string);
    ASTNode *node = create_node(NODE_STRING);
    node->meta.string.value = string;
    return node;
}

ASTNode *create_bool_node(u_int8_t boolean) {
    printf("\nCreating a boolean node with value: %d\n", boolean);
    ASTNode *node = create_node(NODE_BOOLEAN);
    node->meta.boolean.value = boolean;
    return node;
}

ASTNode *create_var_node(char *name) {
    printf("\nCreating a variable node with name: %s\n", name);
    ASTNode *node = create_node(NODE_VARIABLE);
    node->meta.variable.name = name;
    return node;
}

ASTNode *create_assignment_node(ASTNode *var, ASTNode *value) {
    printf("\nCreating an assignment node\n");
    ASTNode *node = create_node(NODE_ASSIGNMENT);
    node->meta.assignment.var = var;
    node->meta.assignment.value = value;
    return node;
}

ASTNode *create_binary_op_node(BinaryOpType op, ASTNode *left, ASTNode *right) {
    printf("\nCreating a binary operation node (%s)\n", binary_op_name(op));
    ASTNode *node = create_node(NODE_BINARY_OP);
    node->meta.binary_op.op = op;
    node->meta.binary_op.left = left;
    node->meta.binary_op.right = right;
    return node;
}

ASTNode *create_unary_op_node(UnaryOpType op, ASTNode *operand) {
    printf("\nCreating a unary operation node (%s)\n", unary_op_name(op));
    ASTNode *node = create_node(NODE_UNARY_OP);
    node->meta.unary_op.op = op;
    node->meta.unary_op.operand = operand;
    return node;
}

ASTNode *create_relation_op_node(RelationOpType op, ASTNode *left, ASTNode *right) {
    printf("\nCreating a relation operation node (%s)\n", relation_op_name(op));
    ASTNode *node = create_node(NODE_RELATION_OP);
    node->meta.relation.op = op;
    node->meta.relation.left = left;
    node->meta.relation.right = right;
    return node;
}

ASTNode *create_equality_op_node(EqualityOpType op, ASTNode *left, ASTNode *right) {
    printf("\nCreating an equality operation node (%s)\n", equality_op_name(op));
    ASTNode *node = create_node(NODE_EQUALITY_OP);
    node->meta.equality.op = op;
    node->meta.equality.left = left;
    node->meta.equality.right = right;
    return node;
}

ASTNode *create_condition_node(ASTNode *condition, ASTNode *then_case, ASTNode *else_case) {
    printf("\nCreating a condition node\n");
    ASTNode *node = create_node(NODE_CONDITION);
    node->meta.condition.cond = condition;
    node->meta.condition.then_case = then_case;
    node->meta.condition.else_case = else_case;
    return node;
}

ASTNode *create_var_declaration_node(char *name, VariableType type, ASTNode *init) {
    if (init == NULL) {
        printf("\nCreating a declaration node (var_name: %s, type: %s)\n", name, variable_type_name(type));
    } else {
        printf("\nCreating a declaration+initialization node (var_name: %s, type: %s)\n", name, variable_type_name(type));
    }
    ASTNode *node = create_node(NODE_DECLARATION);
    node->meta.declaration.name = name;
    node->meta.declaration.init = init;
    return node; 
}

ASTNode *create_statement_list_node(ASTNode *statement, ASTNode *next) {
    printf("\nCreating a statement list node\n");
    ASTNode *node = create_node(NODE_STATEMENT_LIST);
    node->meta.statement_list.statement = statement;
    node->meta.statement_list.next = next;
    return node;
}