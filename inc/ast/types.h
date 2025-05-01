#ifndef TYPES_H
#define TYPES_H

typedef enum {
    T_INT,
    T_STRING,
    T_BOOL,
} VariableType;

typedef struct {
    VariableType type;
    union {
        int integer;
        char *string;
        bool boolean;
    } value;
} Value;

#endif // TYPES_H