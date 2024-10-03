/**
 * @file vm_translator.c
 * @author abdllrhmanzedan
 * @brief declares the implementation and internals of the VMTranslator
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/vm_translator.h"
#include "../include/parser.h"
#include "../include/code_writer.h"

void translate(const char *path)
{
    Parser *parser = newParser();
    CodeWriter *code_writer = newCodeWriter();

    // opening files for reading/writing
    FILE *in_file = fopen(path, "r");

    char *out_path = NULL;
    parser->outPath(path, &out_path);
    if (out_path == NULL)
        return;

    char *filename = NULL;
    parser->fileName(path, &filename);

    FILE *out_file = fopen(out_path, "w");

    char line[256];
    memset(line, 0, sizeof(line));
    while (fgets(line, sizeof(line), in_file) != NULL)
    {
        char *instr = NULL;
        parser->fixInstruction(line, &instr);

        char *arg1 = NULL;
        parser->arg1(instr, &arg1);

        char *arg2 = NULL;
        parser->arg2(instr, &arg2);

        // translate to assembly by coder
        Type instr_type = parser->type(instr);
        if (instr_type == ARITHMETIC || instr_type == LOGICAL)
            code_writer->writeArithmeticLogical(out_file, filename, arg1);
        else if (instr_type == PUSH)
            code_writer->writePush(out_file, filename, arg1, arg2);
        else if (instr_type == POP)
            code_writer->writePop(out_file, filename, arg1, arg2);
    }

    // deallocating memory
    fclose(in_file);
    free(out_path);
    deleteParser(&parser);
    deleteCodeWriter(&code_writer);
}

VMTranslator *newVMTranslator()
{
    VMTranslator *instance = (VMTranslator *)malloc(sizeof(VMTranslator));
    instance->translate = translate;
}

void deleteVMTranslator(VMTranslator **this)
{
    (*this)->translate = NULL;
    free(*this);
    *this = NULL;
}