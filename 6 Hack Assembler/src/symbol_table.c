#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Constants.c"

typedef struct SymbolTable SymbolTable;
typedef struct TableEntry TableEntry;

/// @brief pair of {symbol, address}
struct TableEntry {
    char *symbol;
    int address;
};

/// @brief table contains entries of TableEntry
struct SymbolTable {
    // fields
    TableEntry *table;
    int capacity;
    int count;

    // function pointers
    void (*insert)(SymbolTable *, char *, int);
    int (*get)(SymbolTable *, char *);
};

/// @brief initializing entries to NULL to avoid dangling pointer
/// @param this pointer to SymbolTable instance
/// @param start
void initializeEntries(SymbolTable *this, int start) {
    for (int i = start; i < this->capacity; i++) {
        this->table[i].symbol = NULL;
        this->table[i].address = -1;
    }
}

/// @brief expands the capacity of the table
/// @param this pointer to SymbolTable instance
void expand(SymbolTable *this) {
    int start = this->capacity;
    this->capacity *= 2;
    this->table =
        (TableEntry *)realloc(this->table, this->capacity * sizeof(TableEntry));

    if (this->table == NULL) {
        printf("Bad allocation in SymbolTable::expand(SymbolTable*)");
        assert(false);
    }

    initializeEntries(this, start);
}

/// @brief inserts the pair {symbol, address} to the table
/// @param this pointer to SymbolTable instance
/// @param symbol string represents label/variable name
/// @param address instruction address in memory
void insert(SymbolTable *this, char *symbol, int address) {
    if (this->count == this->capacity) {
        expand(this);
    }
    int idx = this->count;
    this->table[idx].symbol = strdup(symbol);
    this->table[idx].address = address;
    this->count++;
}

/// @brief gets the address value in the pair {symbol, address}
/// @param this pointer to SymbolTable instance
/// @param symbol string represents label/variable name
/// @return -1 if not found, its address otherwise
int get(SymbolTable *this, char *symbol) {
    for (int i = 0; i < this->count; i++) {
        if (strcmp(symbol, this->table[i].symbol) == 0)
            return this->table[i].address;
    }
    return -1;
}

/// @brief creates SymbolTable instance and initializes it
/// @param capacity the initial capacity
/// @return pointer to an instance of SymbolTable
SymbolTable *SymbolTableConstructor(int capacity) {
    // to avoid wrong values
    if (capacity <= 0) capacity = 1;

    // initialize fields
    SymbolTable *instance = (SymbolTable *)malloc(sizeof(SymbolTable));
    instance->table = (TableEntry *)malloc(capacity * sizeof(TableEntry));
    instance->capacity = capacity;
    instance->count = 0;

    initializeEntries(instance, 0);

    // Initialize function pointers
    instance->insert = &insert;
    instance->get = &get;

    return instance;
}

void SymbolTableDestructor(SymbolTable **this) {
    // free each symbol (string) in the table
    SymbolTable *table = *this;

    for (int i = 0; i < table->capacity; i++) {
        if (table->table[i].symbol != NULL) free(table->table[i].symbol);
        table->table[i].symbol = NULL;
    }
    // setting values to zero
    table->capacity = 0;
    table->count = 0;

    // free function pointers
    table->get = NULL;
    table->insert = NULL;

    // free the table array
    free(table->table);
    table->table = NULL;

    // TODO: uncomment this after unit testing
    free(*this);
    *this = NULL;
}

#endif