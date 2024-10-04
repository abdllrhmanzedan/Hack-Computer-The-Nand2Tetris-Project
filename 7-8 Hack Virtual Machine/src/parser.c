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
     {"return", RETURN},
     {"//", COMMENT}};

/********************************************************************************************/
void outPath(const char *in_path, char **out_path)
{
    // check valid extention/format
    const char *extention = strrchr(in_path, '.');
    if (extention == NULL || strcmp(extention, ".vm") != 0)
    {
        consoleLog("Wrong file formate in outPath", WARNING);
        return;
    }

    // returned path = beginning to . + ".asm"
    // +2 to replace "vm" with "asm"
    int size = strlen(in_path) + 2;
    *out_path = (char *)malloc(size);
    if (*out_path == NULL)
    {
        consoleLog("Memory allocation failed in outPath", CRITICAL);
        return;
    }
    memset(*out_path, 0, size);
    strncpy(*out_path, in_path, extention - in_path);
    strcat(*out_path, ".asm");
}

void fileName(const char *in_path, char **filename)
{
    char *last_slash = strchr(in_path, '\\');
    if (last_slash == NULL)
        *filename = strdup(in_path);
    else
        *filename = strdup(last_slash + 1);
    char *temp = *filename + strlen(*filename) - 1;
    while (*temp != '.')
    {
        *temp = '\0';
        temp--;
    }
    // remove '.'
    *temp = '\0';
}

void removeSpaces(char **line)
{
    // remove leading spaces
    while (**line && **line == '\t')
        (*line)++;

    char *end = *line + strlen(*line) - 1;

    // remove trailing spaces
    while (end > *line && (*end == ' ' || *end == '\n'))
    {
        *end = '\0';
        end--;
    }
}

void fixInstruction(char *line, char **instr)
{
    // remove leading and trailing spaces in line
    removeSpaces(&line);
    *instr = strdup(line);
    if (*instr == NULL)
        return;

    // remove comments
    int n = strlen(*instr);
    bool comment = 0;
    for (int i = 0; i < n; i++)
    {
        // start of the comment
        if (comment || (*instr)[i] == '/' || (*instr)[i] == '\n')
        {
            (*instr)[i] = '\0';
            comment = 1;
        }
    }

    // remove trailing
    removeSpaces(&(*instr));
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

void arg1(const char *instr, char **arg)
{
    // all instruction has first argument
    Type instr_type = type(instr);

    // arithmetic/logical: arg1 = operation
    if (instr_type == ARITHMETIC || instr_type == LOGICAL || instr_type == RETURN)
    {
        *arg = strdup(instr);
        return;
    }
    char *space1 = strchr(instr, ' ');
    char *space2 = strrchr(instr, ' ');

    // label/goto/if: arg1 = label
    if (instr_type == LABEL || instr_type == GOTO || instr_type == IF)
    {
        if (space1 != NULL)
        {
            *arg = strdup(space1 + 1); // +1 to ignore the space
            if (*arg == NULL)
                return;
        }
        return;
    }

    // push/pop: arg1 = segment
    // function/call: arg1 = function_name
    if (instr_type == PUSH || instr_type == POP || instr_type == FUNCTION || instr_type == CALL)
    {
        // space1+1 => to ignore the space
        // space2 - space1 - 1 => number of elements in range (space1, space2)
        *arg = strdup(space1 + 1);
        if (*arg == NULL)
            return;

        int end = space2 - space1 - 1;
        (*arg)[end] = '\0';
    }
}

void arg2(const char *instr, char **arg)
{
    // only push/pop/function/call has second argument
    Type instr_type = type(instr);

    if (instr_type != PUSH && instr_type != POP && instr_type != FUNCTION && instr_type != CALL)
        return;

    char *space2 = strrchr(instr, ' ');
    *arg = strdup(space2 + 1);
}

Parser *newParser()
{
    Parser *instance = (Parser *)malloc(sizeof(Parser));
    instance->outPath = &outPath;
    instance->fileName = &fileName;
    instance->type = &type;
    instance->fixInstruction = &fixInstruction;
    instance->arg1 = &arg1;
    instance->arg2 = &arg2;
}

void deleteParser(Parser **this)
{
    Parser *instance = *this;
    instance->outPath = NULL;
    instance->type = NULL;
    instance->fixInstruction = NULL;
    instance->arg1 = NULL;
    instance->arg2 = NULL;
    free(*this);
    *this = NULL;
}