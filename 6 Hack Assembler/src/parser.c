#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Constants.c"

typedef struct Parser Parser;

struct Parser
{
    // fields

    // function pointers
    char *(*targetFile)(char *);
    char *(*getInstruction)(char *);
    Type (*type)(char *);

    // handling A-instruction
    int (*value)(char *);
    char *(*label)(char *);

    // handling C-instruction
    char *(*dest)(Parser *, char *);
    char *(*comp)(Parser *, char *);
    char *(*jump)(Parser *, char *);
};

char *targetFile(char *path)
{
    // returned path = beginning to . + ".hack"
    char *ret = (char *)malloc(strlen(path) + 5);
    memset(ret, 0, strlen(path) + 5);

    char *dot = strrchr(path, '.');
    if (dot == NULL)
    {
        strcpy(ret, path);
    }
    else
    {
        strncpy(ret, path, dot - path);
    }
    strcat(ret, ".hack");
    return ret;
}

/// @brief removes spaces from a given string
/// @param line to be cleared from spaces
static void removeSpaces(char *line)
{
    int idx = 0;
    for (int i = 0; i < strlen(line); i++)
    {
        if (line[i] != ' ')
            line[idx++] = line[i];
    }
    line[idx] = '\0';
}

/// @brief extract the instruction from a line
/// @param line to get the instruction from
/// @return instruction
char *getInstruction(char *line)
{
    removeSpaces(line);
    char *ret = strdup(line);
    bool comment = 0;
    for (int i = 0; i < strlen(ret); i++)
    {
        // start of the comment
        if (comment || ret[i] == '/' || ret[i] == '\n')
        {
            ret[i] = '\0';
            comment = 1;
        }
    }
    return ret;
}

Type type(char *inst)
{
    Type ret = ERROR;
    if (inst[0] == '@')
    {
        // A_VAL or A_VAR
        ret = A_VAL;
        for (int i = 1; i < strlen(inst); i++)
        {
            bool isdigit = inst[i] >= '0' && inst[i] <= '9';
            if (!isdigit)
                ret = A_VAR;
        }
    }
    else if (inst[0] == '/')
    {
        // SINGLE_COMMENT or MULTI_COMMENT
        ret = SINGLE_COMMENT;
    }
    else if (inst[0] == '(')
    {
        ret = LABEL;
    }
    else if (inst[0] == '0' || inst[0] == '1' ||
             (inst[0] == '-' && inst[1] == '1') || inst[0] == 'A' ||
             inst[0] == 'D' || inst[0] == 'M')
    {
        ret = C;
    }
    return ret;
}

int value(char *inst)
{
    if (type(inst) != A_VAL)
        return atoi(INVALID);

    int val = atoi(strdup(inst + 1));
    if (val < 0 || val > MAX_NUMERIC_VALUE)
        return atoi(INVALID);

    return atoi(strdup(inst + 1)); // +1 to ignore '@'
}

char *label(char *inst)
{
    if (type(inst) != LABEL && type(inst) != A_VAR)
    {
        printf("%s \n", inst);
        return INVALID;
    }
    char *ret = (char *)malloc(strlen(inst));
    memset(ret, 0, strlen(inst));

    // +1 to ignore ( and to, len-1 to ignore )
    if (type(inst) == LABEL)
    {
        strncpy(ret, inst + 1, strlen(inst) - 2);
    }
    else
    {
        strcpy(ret, inst + 1);
    }

    return ret;
}

static char *destP(Parser *this, char *inst)
{
    if (type(inst) != C)
        return INVALID;

    // destination exist iff there is '='
    char *equal = strrchr(inst, '=');
    if (equal == NULL)
        return INVALID;

    // dest=comp
    char *ret = (char *)malloc(sizeof(char) * FIELD_LENGTH);
    memset(ret, 0, FIELD_LENGTH);
    strncpy(ret, inst, equal - inst);

    return ret;
}
static char *compP(Parser *this, char *inst)
{
    if (type(inst) != C)
        return INVALID;

    char *ret = (char *)malloc(sizeof(char) * FIELD_LENGTH);
    memset(ret, 0, FIELD_LENGTH);

    char *equal = strrchr(inst, '=');
    char *semi = strrchr(inst, ';');

    // comp always exist:
    // 1- comp
    // 2- comp;jmp
    // 3- dest=comp
    // 4- dest=comp;jmp

    // no equal: case 1, 2
    if (equal == NULL)
    {
        if (semi == NULL)
        {
            // case 1: comp
            strcpy(ret, inst);
        }
        else
        {
            // case 2: comp;jmp
            strncpy(ret, inst, semi - inst);
        }
        return ret;
    }

    if (semi == NULL)
    {
        // case  3: dest=comp
        strcpy(ret, equal + 1);
    }
    else
    {
        // case 4: dest=comp;jmp
        strncpy(ret, equal + 1, semi - equal - 1);
    }
    return ret;
}
static char *jumpP(Parser *this, char *inst)
{
    if (type(inst) != C)
        return INVALID;

    // jump exist iff there is ';'
    char *semi = strrchr(inst, ';');
    if (semi == NULL)
        return INVALID;

    // dest=comp;jmp or comp;jmp
    return strdup(semi + 1);
}

Parser *newParser()
{
    Parser *instance = (Parser *)malloc(sizeof(Parser));

    // Initialize function pointers
    instance->targetFile = &targetFile;
    instance->getInstruction = &getInstruction;
    instance->type = &type;
    instance->value = &value;
    instance->label = &label;
    instance->dest = &destP;
    instance->comp = &compP;
    instance->jump = &jumpP;

    return instance;
}

void deleteParser(Parser **this)
{
    Parser *parser = *this;

    // free function pointers
    parser->targetFile = NULL;
    parser->type = NULL;
    parser->value = NULL;
    parser->label = NULL;
    parser->dest = NULL;
    parser->comp = NULL;
    parser->jump = NULL;

    // TODO: uncomment this after unit testing
    free(*this);
    *this = NULL;
}

#endif