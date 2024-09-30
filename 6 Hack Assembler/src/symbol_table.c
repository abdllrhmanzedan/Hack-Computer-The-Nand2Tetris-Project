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
struct TableEntry
{
    char *symbol;
    int address;
};

/// @brief table contains entries of TableEntry
struct SymbolTable
{
    // fields
    TableEntry *table;
    int capacity;
    int count;

    // function pointers
    void (*insert)(SymbolTable *, char *, int);
    int (*get)(SymbolTable *, char *);
    void (*addCompSymbols)(SymbolTable *);
    void (*addPredefined)(SymbolTable *);
};

/// @brief initializing entries to NULL to avoid dangling pointer
/// @param this pointer to SymbolTable instance
/// @param start
void initializeEntries(SymbolTable *this, int start)
{
    for (int i = start; i < this->capacity; i++)
    {
        this->table[i].symbol = NULL;
        this->table[i].address = -1;
    }
}

/// @brief expands the capacity of the table
/// @param this pointer to SymbolTable instance
void expand(SymbolTable *this)
{
    int start = this->capacity;
    this->capacity *= 2;
    this->table =
        (TableEntry *)realloc(this->table, this->capacity * sizeof(TableEntry));

    if (this->table == NULL)
    {
        printf("Bad allocation in SymbolTable::expand(SymbolTable*)");
        assert(false);
    }

    initializeEntries(this, start);
}

/// @brief inserts the pair {symbol, address} to the table
/// @param this pointer to SymbolTable instance
/// @param symbol string represents label/variable name
/// @param address instruction address in memory
void insert(SymbolTable *this, char *symbol, int address)
{
    if (this->count == this->capacity)
    {
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
int get(SymbolTable *this, char *symbol)
{
    for (int i = 0; i < this->count; i++)
    {
        if (strcmp(symbol, this->table[i].symbol) == 0)
            return this->table[i].address;
    }
    return -1;
}

/// @brief adds the C instruction comp field mapping
/// @param this pointer to SymbolTable instance
void addCompSymbols(SymbolTable *this)
{
    // replace A, M with X as they differ only bey bit-a
    // converting the binary to decimal to reuse the symbol table
    this->insert(this, "0", 42);
    this->insert(this, "1", 63);
    this->insert(this, "-1", 58);
    this->insert(this, "D", 12);
    this->insert(this, "X", 48);
    this->insert(this, "!D", 13);
    this->insert(this, "!X", 49);
    this->insert(this, "-D", 15);
    this->insert(this, "-X", 51);
    this->insert(this, "D+1", 31);
    this->insert(this, "X+1", 55);
    this->insert(this, "D-1", 14);
    this->insert(this, "X-1", 50);
    this->insert(this, "D+X", 2);
    this->insert(this, "D-X", 19);
    this->insert(this, "X-D", 7);
    this->insert(this, "D&X", 0);
    this->insert(this, "D|X", 21);
}

void addPredefined(SymbolTable *this)
{
    this->insert(this, "R0", 0);
    this->insert(this, "R1", 1);
    this->insert(this, "R2", 2);
    this->insert(this, "R3", 3);
    this->insert(this, "R4", 4);
    this->insert(this, "R5", 5);
    this->insert(this, "R6", 6);
    this->insert(this, "R7", 7);
    this->insert(this, "R8", 8);
    this->insert(this, "R9", 9);
    this->insert(this, "R10", 10);
    this->insert(this, "R11", 11);
    this->insert(this, "R12", 12);
    this->insert(this, "R13", 13);
    this->insert(this, "R14", 14);
    this->insert(this, "R15", 15);

    this->insert(this, "SCREEN", 16384);
    this->insert(this, "KBD", 24576);
    this->insert(this, "SP", 0);
    this->insert(this, "LCL", 1);
    this->insert(this, "ARG", 2);
    this->insert(this, "THIS", 3);
    this->insert(this, "THAT", 4);
}

/// @brief creates SymbolTable instance and initializes it
/// @param capacity the initial capacity
/// @return pointer to an instance of SymbolTable
SymbolTable *newSymbolTable(int capacity)
{
    // to avoid wrong values
    if (capacity <= 0)
        capacity = 1;

    // initialize fields
    SymbolTable *instance = (SymbolTable *)malloc(sizeof(SymbolTable));
    instance->table = (TableEntry *)malloc(capacity * sizeof(TableEntry));
    instance->capacity = capacity;
    instance->count = 0;

    initializeEntries(instance, 0);

    // Initialize function pointers
    instance->insert = &insert;
    instance->get = &get;
    instance->addCompSymbols = &addCompSymbols;
    instance->addPredefined = &addPredefined;

    return instance;
}

/// @brief deallocates memory of this
/// @param this pointer to SymbolTable instance pointer
void deleteSymbolTable(SymbolTable **this)
{
    // free each symbol (string) in the table
    SymbolTable *table = *this;

    for (int i = 0; i < table->capacity; i++)
    {
        if (table->table[i].symbol != NULL)
            free(table->table[i].symbol);
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