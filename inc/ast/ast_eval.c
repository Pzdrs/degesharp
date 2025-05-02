#include "ast_eval.h"
#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

extern void yyerror(const char *s);

SymbolTable *symbol_table = NULL;

void interpret(ASTNode *root) {
    printf("\nInterpreting the AST\n");
    printf("--------------------------------\n");
    if (root == NULL) {
        return;
    }

    symbol_table = init_symbol_table(); 

    eval_statement(root);
}

bool is_lvalue(ASTNode *node) {
    if (node == NULL) return false;
    if (node->type == NODE_VARIABLE) {
        return true;
    }
    return false;
}

Value default_value_for_type(VariableType type) {
    Value v;
    v.type = type;
    switch (type) {
        case T_INT:
            v.value.integer = 0;
            break;
        case T_STRING:
            v.value.string = "";
            break;
        case T_BOOL:
            v.value.boolean = false;
            break;
        default:
            yyerror("Unknown variable type");
            break;
    }
    return v;
}

Value eval_expression(ASTNode *node) {
    switch (node->type) {
        case NODE_RELATION_OP: {
            printf("Evaluating relation operation\n"); 

            Value result;
            result.type = T_BOOL;

            Value left = eval_expression(node->meta.relation.left);
            Value right = eval_expression(node->meta.relation.right);

            if (left.type == T_INT && right.type == T_INT) {
                switch (node->meta.relation.op) {
                    case OP_LT:
                        result.value.boolean = left.value.integer < right.value.integer;
                        break;
                    case OP_GT:
                        result.value.boolean = left.value.integer > right.value.integer;
                        break;
                    case OP_LE:
                        result.value.boolean = left.value.integer <= right.value.integer;
                        break;
                    case OP_GE:
                        result.value.boolean = left.value.integer >= right.value.integer;
                        break;
                    default:
                        yyerror("invalid relation operator");
                }
            } else yyerror("invalid relation operation: both operands must be numbers");
            
            printf("Result: %d\n", result.value.boolean);
            return result;
        }
        case NODE_EQUALITY_OP: {
            printf("Evaluating equality operation\n");

            Value result;
            result.type = T_BOOL;

            Value left_val = eval_expression(node->meta.equality.left);
            Value right_val = eval_expression(node->meta.equality.right);
            VariableType left_type = left_val.type;
            VariableType right_type = right_val.type;

            if (left_type == T_INT && right_type == T_INT) {
                printf("Comparing numbers\n");
                switch (node->meta.equality.op) {
                    case OP_EQ:
                        result.value.boolean = left_val.value.integer == right_val.value.integer;
                        break;
                    case OP_NE:
                        result.value.boolean = left_val.value.integer != right_val.value.integer;
                        break;
                }
            }
            else if (left_type == T_STRING && right_type == T_STRING) {
                printf("Comparing strings\n");
                switch (node->meta.equality.op) {
                    case OP_EQ:
                        result.value.boolean = strcmp(left_val.value.string, right_val.value.string) == 0;
                        break;
                    case OP_NE:
                        result.value.boolean = strcmp(left_val.value.string, right_val.value.string) != 0;
                        break;
                }
            }
            else if (left_type == T_BOOL && right_type == T_BOOL) {
                printf("Comparing booleans\n");
                switch (node->meta.equality.op) {
                    case OP_EQ:
                        result.value.boolean = left_val.value.boolean == right_val.value.boolean;
                        break;
                    case OP_NE:
                        result.value.boolean = left_val.value.boolean != right_val.value.boolean;
                        break;
                }
            } else {
                char buffer[256];
                snprintf(buffer, sizeof(buffer), "invalid equality operands: %s and %s", variable_type_name(left_type), variable_type_name(right_type));
                yyerror(buffer);
            } 
            
            printf("Result: %d\n", result.value.boolean);
            return result;
        }
        case NODE_BINARY_OP: {
            printf("Evaluating binary operation\n");

            Value result;

            Value left_val = eval_expression(node->meta.binary_op.left);
            Value right_val = eval_expression(node->meta.binary_op.right);
            VariableType left_type = left_val.type;
            VariableType right_type = right_val.type;

            if (left_type == T_INT && right_type == T_INT) {
                result.type = T_INT;

                switch (node->meta.binary_op.op) {
                    case OP_ADD:
                        result.value.integer = left_val.value.integer + right_val.value.integer;
                        break;
                    case OP_SUB:
                        result.value.integer = left_val.value.integer - right_val.value.integer;
                        break;
                    case OP_MUL:
                        result.value.integer = left_val.value.integer * right_val.value.integer;
                        break;
                    case OP_DIV:
                        if (right_val.value.integer == 0) {
                            yyerror("division by zero");
                            return result;
                        }
                        result.value.integer = left_val.value.integer / right_val.value.integer;
                        break;
                    default:
                        yyerror("invalid binary operator");
                }
            } else if (left_type == T_BOOL && right_type == T_BOOL) {
                result.type = T_BOOL;

                switch (node->meta.binary_op.op) {
                    case OP_LOGICAL_AND:
                        result.value.integer = left_val.value.boolean && right_val.value.boolean;
                        break;
                    case OP_LOGICAL_OR:
                        result.value.integer = left_val.value.boolean || right_val.value.boolean;
                        break;
                    default:
                        yyerror("invalid binary operator");
                }
            } else if (left_type == T_STRING && right_type == T_STRING) {
                if (node->meta.binary_op.op == OP_ADD) {
                    result.type = T_STRING;
                    result.value.string = malloc(strlen(left_val.value.string) + strlen(right_val.value.string) + 1);
                    strcpy(result.value.string, left_val.value.string);
                    strcat(result.value.string, right_val.value.string);
                } else {
                    yyerror("invalid binary operator for strings");
                }
            } else yyerror("invalid binary operation: invalid operand combination");            
            
            return result;
        }
        case NODE_VARIABLE: {
            printf("Evaluating variable\n");
            SymbolTableEntry *entry = lookup_symbol(symbol_table, node->meta.variable.name);

            if (entry != NULL) {
                return entry->data.variable.value;
            } else yyerror("Variable not found");
            
            break;
        }
        case NODE_ASSIGNMENT: {
            Value result;

            if(!is_lvalue(node->meta.assignment.var)) {
                yyerror("lvalue required as left operand of assignment");
                return result;
            }

            SymbolTableEntry *entry = lookup_symbol(symbol_table, node->meta.assignment.var->meta.variable.name);

            if (entry == NULL) {
                yyerror("Variable not found");
                return result;
            }

            Value val = eval_expression(node->meta.assignment.value);

            if (entry->data.variable.value.type != val.type) {
                yyerror("type mismatch: variable type and assigned type do not match");
                return result;
            }

            entry->data.variable.value = val;
            result.type = entry->data.variable.value.type;
            switch (result.type) {
                case T_INT:
                    result.value.integer = entry->data.variable.value.value.integer;
                    break;
                case T_STRING:
                    result.value.string = entry->data.variable.value.value.string;
                    break;
                case T_BOOL:
                    result.value.boolean = entry->data.variable.value.value.boolean;
                    break;
            }
            print_st(symbol_table);
            return result;
        }
        case NODE_NUMBER:
            return (Value){
                .type = T_INT,
                .value.integer = node->meta.number.value
            };
        case NODE_STRING:
            return (Value){
                .type = T_STRING,
                .value.string = node->meta.string.value
            };
        case NODE_BOOLEAN:
            return (Value){
                .type = T_BOOL,
                .value.boolean = node->meta.boolean.value
            };
    }
}

void eval_statement(ASTNode *node) {
    if (node == NULL) return;
    if (node->type == NODE_STATEMENT_LIST) {
        eval_statement(node->meta.statement_list.statement);
        eval_statement(node->meta.statement_list.next);
        return;
    }
    if (node->type == NODE_COMPOUND_STATEMENT) {
        eval_statement(node->meta.compound_statement.statement_list);
        return;
    }

    printf("\nEvaluating node: %s\n", node_type_name(node->type)); 
    switch (node->type) {
        case NODE_DECLARATION: {
            const char *name = node->meta.declaration.name;
            VariableType declared_type = node->meta.declaration.type;
            ASTNode *init_expr = node->meta.declaration.init;

            printf("Name, type: %s, %s\n", name, variable_type_name(declared_type));

            SymbolTableEntry *entry = lookup_symbol(symbol_table, name);
            if (entry != NULL) {
                yyerror("Variable already declared");
                return;
            }

            if (init_expr != NULL) {
                Value val = eval_expression(init_expr);
                if (val.type != declared_type) {
                    yyerror("Type mismatch: declaration type and assigned type do not match");
                    return;
                }
                add_symbol(symbol_table, name, VARIABLE, val);
            } else {
                // No initializer: add default value or mark as uninitialized
                Value default_val = default_value_for_type(declared_type); // You must define this
                add_symbol(symbol_table, name, VARIABLE, default_val);
            }
            break;
        }
        case NODE_ASSIGNMENT: {
            eval_expression(node);
            break;
        }
        case NODE_CONDITION: {
            Value cond = eval_expression(node->meta.condition.cond);

            switch (cond.type) {
                case T_BOOL:
                    if (cond.value.boolean) {
                        printf("Condition is true\n");
                        eval_statement(node->meta.condition.then_case);
                    } else {
                        printf("Condition is false\n");
                        eval_statement(node->meta.condition.else_case);
                    }
                    break;
                case T_INT:
                    if (cond.value.integer) {
                        printf("Condition is true\n");
                        eval_statement(node->meta.condition.then_case);
                    } else {
                        printf("Condition is false\n");
                        eval_statement(node->meta.condition.else_case);
                    }
                    break;
                case T_STRING:
                    if (strcmp(cond.value.string, "") != 0) {
                        printf("Condition is true\n");
                        eval_statement(node->meta.condition.then_case);
                    } else {
                        printf("Condition is false\n");
                        eval_statement(node->meta.condition.else_case);
                    }
                    break;
                default:
                    yyerror("Invalid condition type");
            }
            break;
        }
        case NODE_FOR: {
            ASTNode *init = node->meta.iteration.for_loop.init;
            if (init != NULL) {
                eval_statement(init);
            }
            
            while(eval_expression(node->meta.iteration.for_loop.cond).value.boolean) {
                printf("For loop body\n");
                eval_statement(node->meta.iteration.for_loop.body);
                
                if (node->meta.iteration.for_loop.iter != NULL) eval_expression(node->meta.iteration.for_loop.iter);
            }
            break;
        }
        case NODE_FUNCTION_CALL: {
            char* name = node->meta.function_call.name;
            printf("Function call: %s\n", name);
            if (strcmp(name, "povidam") == 0) {
                Value arg = eval_expression(node->meta.function_call.arg);
                switch (arg.type) {
                    case T_INT:
                        printf("%d\n", arg.value.integer);
                        break;
                    case T_STRING:
                        printf("%s\n", arg.value.string);
                        break;
                    case T_BOOL:
                        printf("%s\n", arg.value.boolean ? "true" : "false");
                        break;
                    default:
                        yyerror("Invalid argument type for print function");
                }
            } else {
                yyerror("Unknown function call");
            }
            break;
        }
        default:
            printf("Unknown statement type: %d\n", node->type);
            break;
    }
}
    