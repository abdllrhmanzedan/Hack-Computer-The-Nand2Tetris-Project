#ifndef _HACK_ASSEMBLER_H
#define _HACK_ASSEMBLER_H

#include "coder.c"
#include "parser.c"
#include "symbol_table.c"

typedef struct HackAssembler HackAssembler;

struct HackAssembler {
    // fields
    FILE *in_file;
    FILE *out_file;
    int instruction_number;
    Coder *code;
    Parser *parser;
    SymbolTable *table;

    // function pointers
};

void firstPass() {}
void secondPass();

void writeMachineCode();

HackAssembler *HackAssemblerConstructor(char *path) {
    HackAssembler *instance = (HackAssembler *)malloc(sizeof(HackAssembler));
    instance->in_file = fopen(path, "r");
    instance->out_file;
    instance->instruction_number = 0;

    instance->code = CodeConstructor();
    instance->parser = ParserConstructor();
    instance->table = SymbolTableConstructor(100);

    return instance;
}

#endif