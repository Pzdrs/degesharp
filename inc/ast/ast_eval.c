#include "ast_eval.h"
#include <stdio.h>

void interpret(ASTNode *root) {
    printf("\nInterpreting the AST\n");
    printf("--------------------------------\n");
    if (root == NULL) {
        return;
    }

    eval_statement(root);
}

Value eval_expression(ASTNode *node) {
    switch (node->type) {
        case NODE_RELATION_OP:
            switch (node->meta.relation.op) {
                case OP_LT:
                    return (Value){.boolean = eval_expression(node->meta.relation.left).integer < eval_expression(node->meta.relation.right).integer};
                case OP_GT:
                    return (Value){.boolean = eval_expression(node->meta.relation.left).integer > eval_expression(node->meta.relation.right).integer};
                case OP_LE:
                    return (Value){.boolean = eval_expression(node->meta.relation.left).integer <= eval_expression(node->meta.relation.right).integer};
                case OP_GE:
                    return (Value){.boolean = eval_expression(node->meta.relation.left).integer >= eval_expression(node->meta.relation.right).integer};
            }
        case NODE_NUMBER:
            return (Value){.integer = node->meta.number.value};
        case NODE_STRING:
            return (Value){.string = node->meta.string.value};
        case NODE_BOOLEAN:
            return (Value){.boolean = node->meta.boolean.value};
    }
}

void eval_statement(ASTNode *node) {
    if (node == NULL) return;
    if (node->type == NODE_STATEMENT_LIST) {
        eval_statement(node->meta.statement_list.statement);
        eval_statement(node->meta.statement_list.next);
        return;
    }

    printf("\nEvaluating node: %s\n", node_type_name(node->type)); 
    switch (node->type) {
        case NODE_DECLARATION: {
            if (node->meta.declaration.init != NULL) {
                Value val = eval_expression(node->meta.declaration.init);
                printf("Name, type: %s, %s\n", 
                    node->meta.declaration.name,
                    variable_type_name(node->meta.declaration.type)
                );

                switch(node->meta.declaration.type) {
                    case T_INT:
                        printf("Value: %d\n", val.integer);
                        break;
                    case T_STRING:
                        printf("Value: %s\n", val.string);
                        break;
                    case T_BOOL:
                        printf("Value: %d\n", val.boolean);
                        break;
                    default:
                        printf("Unknown variable type\n");
                }                
            } else {
                // No initialization, just add the variable to the ST
            }
            break;
        }
        case NODE_ASSIGNMENT: {
            Value val = eval_expression(node->meta.assignment.value);
            // Check if var exists, if not, throw an error
            // Then verify the type and assign the value if possible
            //yyerror("var not found");
            break;
        }
        case NODE_CONDITION: {
            Value cond = eval_expression(node->meta.condition.cond);
            printf("Condition evaluated to: %d\n", cond.boolean);
            break;
        }
        case NODE_FOR: {
            ASTNode *init = node->meta.iteration.for_loop.init;
            if (init != NULL) {
                eval_statement(init);
            }
            
            break; // Variables aren't implemented yet, this would possibly result in an infinite loop
            while(eval_expression(node->meta.iteration.for_loop.cond).boolean) {
                eval_statement(node->meta.iteration.for_loop.body);
                eval_expression(node->meta.iteration.for_loop.iter);
            }
            break;
        }
        default:
            printf("Unknown statement type: %d\n", node->type);
            break;
    }
}
    