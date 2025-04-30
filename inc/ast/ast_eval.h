#include "ast.h"

typedef enum { VAL_INTEGER, VAL_STRING, VAL_BOOLEAN } ValueType;

typedef struct {
    ValueType type;
    union {
        int integer;
        char *string;
        bool boolean;
    } value;
} Value;

void eval_statement(ASTNode *node);

Value *eval_expression(ASTNode *node);