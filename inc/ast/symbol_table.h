#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"
#include "types.h"

typedef enum {
    FUNCTION, VARIABLE
} EntryType;

typedef struct {
    EntryType type;
    char *name;
    union {
        struct {
            Value value;
        } variable;
        struct {} function;
    } data;
    
} SymbolTableEntry;

typedef struct {
    SymbolTableEntry *entries;
    size_t size;
} SymbolTable;

SymbolTable *init_symbol_table();

void print_st(SymbolTable *table);

SymbolTableEntry *lookup_symbol(SymbolTable *table, const char *name);
void add_symbol(SymbolTable *table, const char *name, EntryType type, Value value);

#endif // SYMBOL_TABLE_H