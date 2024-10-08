#ifndef _CODE_H
#define _CODE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "constants.c"
#include "symbol_table.c"

typedef struct Coder Coder;

/// @brief responsible for converting assembly instruction fields to machine code
struct Coder
{
    // fields
    SymbolTable *table;

    // function pointers
    // coding A-instruction
    char *(*convertA)(int);

    // coding C-instruction
    char *(*dest)(Coder *, char *);
    char *(*comp)(Coder *, char *);
    char *(*jump)(Coder *, char *);
    char *(*convertC)(Coder *, char *, char *, char *);
};

/// @brief converts the given address to 15-bit binary string and appends '0'
/// (op-code)
/// @param address the label/variable address in the memory or numeric value
/// @return machine code of A instruction
char *convertA(int address)
{
    if (address > MAX_NUMERIC_VALUE)
        address %= (MAX_NUMERIC_VALUE + 1);
    if (address < 0)
    {
        address %= (MAX_NUMERIC_VALUE + 1);
        address += (MAX_NUMERIC_VALUE + 1);
    }

    char *ret = (char *)malloc(INSTRUCTION_LENGTH + 1);
    // ret[0]='0' + [1:15] binary number
    ret[0] = '0';
    for (int i = 0; i < 15; i++)
    {
        int idx = INSTRUCTION_LENGTH - i - 1;
        ret[idx] = '0';
        if ((1 << i) & address)
            ret[idx] = '1';
    }

    ret[INSTRUCTION_LENGTH] = '\0';
    return ret;
}

static char *dest(Coder *this, char *dest_field)
{
    char *ret = (char *)malloc(DEST_LENGTH + 1);
    for (int i = 0; i < DEST_LENGTH; i++)
        ret[i] = '0';

    // A=ret[0], D=ret[1], M=ret[2]
    if (strchr(dest_field, 'A') != NULL)
        ret[0] = '1';
    if (strchr(dest_field, 'D') != NULL)
        ret[1] = '1';
    if (strchr(dest_field, 'M') != NULL)
        ret[2] = '1';

    ret[DEST_LENGTH] = '\0';
    return ret;
}

static char *comp(Coder *this, char *comp_field)
{
    char *ret = (char *)malloc(COMP_LENGTH + 1);
    for (int i = 0; i < COMP_LENGTH; i++)
        ret[i] = '0';

    char *cur = strdup(comp_field);

    // replace A, M with X as they differ only bey bit-a
    bool a = 0; // A
    for (int i = 0; i < strlen(cur); i++)
    {
        a |= (cur[i] == 'M');
        if (cur[i] == 'A' || cur[i] == 'M')
        {
            cur[i] = 'X';
        }
    }
    // get the machine code (but it's in decimal) of the instruction
    int machine_code = this->table->get(this->table, cur);
    if (machine_code == -1)
        a = 1;

    // a + (6-bit binary number)
    ret[0] = (char)('0' + a);
    for (int i = 0; i < 6; i++)
    {
        int idx = COMP_LENGTH - i - 1;
        ret[idx] = '0';
        if ((1 << i) & machine_code)
        {
            ret[idx] = '1';
        }
    }

    ret[COMP_LENGTH] = '\0';
    return ret;
}

static char *jump(Coder *this, char *jump_field)
{
    char *ret = (char *)malloc(JUMP_LENGTH + 1);
    char val = '0';
    if (strcmp(jump_field, "JMP") == 0)
        val = '1';
    for (int i = 0; i < JUMP_LENGTH; i++)
        ret[i] = val;

    // neg(L)=ret[0], zero(E)=ret[1], pos(G)=ret[2]
    if (strchr(jump_field, 'L') != NULL)
        ret[0] = '1';
    if (strchr(jump_field, 'E') != NULL)
        ret[1] = '1';
    if (strchr(jump_field, 'G') != NULL)
        ret[2] = '1';

    // handling not equal zero
    if (strcmp(jump_field, "JNE") == 0)
        strcpy(ret, "101");

    ret[JUMP_LENGTH] = '\0';
    return ret;
}

char *convertC(Coder *this, char *dest_field, char *comp_field,
               char *jump_field)
{
    char *ret = (char *)malloc(INSTRUCTION_LENGTH + 1);
    memset(ret, 0, INSTRUCTION_LENGTH + 1);

    // "111" + comp + dest + jump -> 3 + 7 + 3 + 2 = 16-bit
    strcat(ret, "111");
    strcat(ret, comp(this, comp_field));
    strcat(ret, dest(this, dest_field));
    strcat(ret, jump(this, jump_field));

    ret[INSTRUCTION_LENGTH] = '\0';
    return ret;
}

Coder *newCoder()
{
    Coder *instance = (Coder *)malloc(sizeof(Coder));

    // initialize fields
    instance->table = newSymbolTable(20);
    instance->table->addCompSymbols(instance->table);

    // Initialize function pointers
    instance->convertA = &convertA;
    instance->dest = &dest;
    instance->comp = &comp;
    instance->jump = &jump;
    instance->convertC = *convertC;
}

void deleteCoder(Coder **this)
{
    Coder *table = *this;
    // free fields
    deleteSymbolTable(&table->table);

    // free function pointers
    table->convertA = NULL;
    table->dest = NULL;
    table->comp = NULL;
    table->jump = NULL;
    table->convertC = NULL;

    // TODO: uncomment this after unit testing
    free(*this);
    *this = NULL;
}

#endif