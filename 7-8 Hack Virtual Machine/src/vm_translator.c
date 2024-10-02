/**
 * @file vm_translator.c
 * @author abdllrhmanzedan
 * @brief declares the implementation and internals of the VMTranslator
 */

#include <stdio.h>
#include <string.h>

#include "../include/vm_translator.h"
#include "../include/parser.h"

void translate(const char *path)
{
    Parser *parser = newParser();

    // opening files for reading/writing
    FILE *in_file = fopen(path, "r");

    char *out_path = NULL;
    parser->outPath(in_file, out_path);
    if (out_path == NULL)
        return;

    FILE *out_file = fopen(out_path, "w");

    char line[256];
    memset(line, 0, sizeof(line));
    while (fgets(line, sizeof(line), in_file) != NULL)
    {
    }

    // deallocating memory
    fclose(in_file);
    free(out_path);
    deleteParser(&parser);
}

VMTranslator *newVMTranslator()
{
}

void deleteVMTranslator(VMTranslator **this)
{
}