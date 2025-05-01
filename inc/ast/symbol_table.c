#include <stdio.h>
#include <string.h>
#include "symbol_table.h"

#define INITIAL_SIZE 10

char  *entry_type_name(EntryType type) {
    switch (type) {
        case FUNCTION: return "Func";
        case VARIABLE: return "Var";
        default: return "IDK";
    }
}

SymbolTable *init_symbol_table() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if (table == NULL) {
        fprintf(stderr, "Failed to allocate memory for symbol table\n");
        exit(EXIT_FAILURE);
    }

    table->entries = malloc(sizeof(SymbolTableEntry) * INITIAL_SIZE);
    table->size = 0;
    return table;
}

void print_st(SymbolTable *table) {
    printf("\n==================== SYMBOL TABLE ====================\n");
    printf("| %-10s | %-8s | %-20s |\n", "Name", "Kind", "Value");
    printf("------------------------------------------------------\n");

    for (size_t i = 0; i < table->size; i++) {
        SymbolTableEntry *entry = &table->entries[i];

        switch (entry->type) {
            case FUNCTION:
                printf("| %-10s | %-8s | %-20s |\n", entry->name, "Function", "-");
                break;
            case VARIABLE: {
                const char *type_name = variable_type_name(entry->data.variable.value.type);
                char value_buf[64];

                switch (entry->data.variable.value.type) {
                    case T_INT:
                        snprintf(value_buf, sizeof(value_buf), "%d", entry->data.variable.value.value.integer);
                        break;
                    case T_STRING:
                        snprintf(value_buf, sizeof(value_buf), "\"%s\"", entry->data.variable.value.value.string);
                        break;
                    case T_BOOL:
                        snprintf(value_buf, sizeof(value_buf), "%s", entry->data.variable.value.value.boolean ? "true" : "false");
                        break;
                    default:
                        snprintf(value_buf, sizeof(value_buf), "Unknown");
                }

                char kind_buf[32];
                snprintf(kind_buf, sizeof(kind_buf), "Var (%s)", type_name);

                printf("| %-10s | %-8s | %-20s |\n", entry->name, kind_buf, value_buf);
                break;
            }
        }
    }

    printf("======================================================\n");
}

SymbolTableEntry *lookup_symbol(SymbolTable *table, const char *name) {
    printf("Looking up symbol: %s\n", name);
    print_st(table);
    for (size_t i = 0; i < table->size; i++) {
        if (strcmp(table->entries[i].name, name) == 0) {
            SymbolTableEntry *entry = & table->entries[i];
            return entry;
        }
    }
    return NULL;
}

void add_symbol(SymbolTable *table, const char *name, EntryType type, Value value) {
    printf("Adding symbol: %s, type: %s\n", name, variable_type_name(value.type));
    table->entries = realloc(table->entries, sizeof(SymbolTableEntry) * (table->size + 1));
    if (table->entries == NULL) {
        fprintf(stderr, "Failed to allocate memory for symbol table entry\n");
        exit(EXIT_FAILURE);
    }

    SymbolTableEntry *entry = &table->entries[table->size];
    entry->type = type;
    entry->name = strdup(name);
    entry->data.variable.value = value;
    table->size++;

    print_st(table);
}