/**
 * @file parser.c
 * @author abdllrhmanzedan
 * @brief declares the implementation and internals of the Parser
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "../include/parser.h"
#include "../include/logger.h"

// global variables
typedef struct TypeMapping TypeMapping;

enum Type
{
    ARITHMETIC,
    LOGICAL,
    PUSH,
    POP,
    LABEL,
    GOTO,
    IF,
    FUNCTION,
    CALL,
    RETURN,
    UNKOWN,
    ERROR
};

struct TypeMapping
{
    const char *keyword;
    Type type;
};

const char *arithmetic[] = {"add", "sub", "neg"};
const char *logical[] = {"eq", "gt", "lt", "and", "or", "not"};

static TypeMapping mapping[] =
    {{"push", PUSH},
     {"pop", POP},
     {"label", LABEL},
     {"goto", GOTO},
     {"if-goto", IF},
     {"function", FUNCTION},
     {"call", CALL},
     {"return", RETURN}};

/**************************************************************************************/
void outPath(const char *in_path, char *out_path)
{
    // check valid .vmm format
    if (strstr(in_path, ".vm") == NULL)
    {
        consoleLog("Wrong file formate in parser.c::outPath(char*, char*)", WARNING);
        return;
    }

    // returned path = beginning to . + ".asm"
    out_path = (char *)malloc(sizeof(*out_path) * (strlen(in_path) + 1));

    char *dot = strrchr(in_path, '.');
    strcpy(out_path, dot - in_path);
    strcat(out_path, ".asm");
}

bool isArithmetic(const char *instr)
{
    int n = sizeof(arithmetic) / sizeof(arithmetic[0]);
    for (int i = 0; i < n; i++)
    {
        if (strstr(instr, arithmetic[i]) == instr)
            return true;
    }
    return false;
}

bool isLogical(const char *instr)
{
    int n = sizeof(logical) / sizeof(logical[0]);
    for (int i = 0; i < n; i++)
    {
        if (strstr(instr, logical[i]) == instr)
            return true;
    }
    return false;
}

Type type(const char *instr)
{
    Type ret = UNKOWN;
    int cnt = 0;
    if (isArithmetic(instr))
    {
        ret = ARITHMETIC;
        cnt++;
    }

    if (isLogical(instr))
    {
        ret = LOGICAL;
        cnt++;
    }

    int n = sizeof(mapping) / sizeof(mapping[0]);
    for (int i = 0; i < n; i++)
    {
        if (strstr(instr, mapping[i].keyword) == instr)
        {
            ret = mapping[i].type;
            cnt++;
        }
    }

    // wrong usage of keyword
    if (cnt > 1)
        ret = ERROR;
    return ret;
}

void arg1(const char *instr, char *arg)
{
    // all instruction has first argument
    Type instr_type = type(instr);

    // arithmetic/logical: arg1 = operation
    if (instr_type == ARITHMETIC || instr_type == LOGICAL)
    {
        strcpy(arg, instr);
        return;
    }
    char *space1 = strchr(instr, ' ');
    char *space2 = strrchr(instr, ' ');

    // label/goto/if: arg1 = label
    if (instr_type == LABEL || instr_type == GOTO || instr_type == IF)
    {
        if (space1 != NULL)
        {
            strcpy(arg, space1 + 1); // +1 to ignore the space
        }
        return;
    }

    // push/pop: arg1 = segment
    // function/call: arg1 = function_name
    if (instr_type == PUSH || instr_type == POP || instr_type == FUNCTION || instr_type == CALL)
    {
        // space1+1 => to ignore the space
        // space2 - space1 - 1 => number of elements in range (space1, space2)
        strncpy(arg, space1 + 1, space2 - space1 - 1);
    }
}

void arg2(const char *instr, char *arg)
{
    // only push/pop/function/call has second argument
    Type instr_type = type(instr);

    if (instr_type != PUSH && instr_type != POP && instr_type != FUNCTION || instr_type != CALL)
        return;

    char *space2 = strrchr(instr, ' ');
    strcpy(arg, space2 + 1);
}

Parser *newParser()
{
    Parser *instance = (Parser *)malloc(sizeof(Parser));
    instance->outPath = &outPath;
    instance->type = &type;
    instance->arg1 = &arg1;
    instance->arg2 = &arg2;
}

void deleteParser(Parser **this)
{
    Parser *instance = *this;
    instance->outPath = NULL;
    instance->type = NULL;
    instance->arg1 = NULL;
    instance->arg2 = NULL;
    free(*this);
    *this = NULL;
}