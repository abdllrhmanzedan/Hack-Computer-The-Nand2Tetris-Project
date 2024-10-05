/**
 * @file vm_translator.c
 * @author abdllrhmanzedan
 * @brief declares the implementation and internals of the VMTranslator
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "../include/vm_translator.h"
#include "../include/parser.h"
#include "../include/code_writer.h"

void translate(FILE *out_file, FILE *in_file, const char *filename, Parser **_parser, CodeWriter **_coder)
{
    Parser *parser = *_parser;
    CodeWriter *code_writer = *_coder;

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
        {
            code_writer->writeArithmeticLogical(out_file, filename, arg1);
        }
        else if (instr_type == PUSH)
        {
            code_writer->writePush(out_file, filename, arg1, arg2);
        }
        else if (instr_type == POP)
        {
            code_writer->writePop(out_file, filename, arg1, arg2);
        }
        else if (instr_type == LABEL || instr_type == GOTO || instr_type == IF)
        {
            char *type_str = NULL;

            if (instr_type == LABEL)
            {
                type_str = strdup("label");
            }
            else if (instr_type == GOTO)
            {
                type_str = strdup("goto");
            }
            else
            {
                type_str = strdup("if-goto");
            }

            code_writer->writeBranching(out_file, type_str, arg1);
            free(type_str);
        }
        else if (instr_type == FUNCTION)
        {
            code_writer->writeFunction(out_file, arg1, arg2);
        }
        else if (instr_type == CALL)
        {
            code_writer->writeCall(out_file, arg1, arg2);
        }
        else if (instr_type == RETURN)
        {
            code_writer->writeReturn(out_file);
        }
    }
}

void translateFile(const char *path)
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

    translate(out_file, in_file, filename, &parser, &code_writer);

    // deallocating memory
    fclose(in_file);
    free(out_path);
    deleteParser(&parser);
    deleteCodeWriter(&code_writer);
}

void translateDir(const char *dir_path)
{
    Parser *parser = newParser();
    CodeWriter *code_writer = newCodeWriter();

    DIR *input_dir = opendir(dir_path);
    if (input_dir == NULL)
    {
        printf("Unable to open input directory  vm_translator::translateDir()");
        return;
    }
    struct dirent *file;
    char out_filename[256];
    sprintf(out_filename, "%s/%s.asm", dir_path, dir_path);
    FILE *out_file = fopen(out_filename, "w");
    if (out_file == NULL)
    {
        printf("Unable to open out file vm_translator::translateDir()");
        return;
    }
    code_writer->writeInit(out_file);

    char path[256];
    while (file = readdir(input_dir))
    {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            continue;
        const char *dot = strrchr(file->d_name, '.');
        if (dot == NULL || strcmp(dot + 1, "asm") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", dir_path, file->d_name);
        FILE *in_file = fopen(path, "r");
        if (in_file == NULL)
        {
            printf("Unable to open input .vm file vm_translator::translateDir()");
            return;
        }

        translate(out_file, in_file, file->d_name, &parser, &code_writer);
    }

    fclose(out_file);
    closedir(input_dir);
    deleteParser(&parser);
    deleteCodeWriter(&code_writer);
}

VMTranslator *newVMTranslator()
{
    VMTranslator *instance = (VMTranslator *)malloc(sizeof(VMTranslator));
    instance->translateFile = &translateFile;
    instance->translateDir = &translateDir;
}

void deleteVMTranslator(VMTranslator **this)
{
    (*this)->translateFile = NULL;
    (*this)->translateDir = NULL;
    free(*this);
    *this = NULL;
}