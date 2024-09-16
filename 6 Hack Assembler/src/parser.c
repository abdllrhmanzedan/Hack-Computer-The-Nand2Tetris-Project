#ifndef _PARSER_H
#define _PARSER_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "Constants.c"

typedef struct Parser Parser;

struct Parser {
    // fields

    // function pointers
    char *(*filePath)(char *);

    Type (*type)(char *);

    // handling A-instruction
    int (*value)(char *);
    char *(*label)(char *);

    // handling C-instruction
    char *(*dest)(char *);
    char *(*comp)(char *);
    char *(*jump)(char *);
};

char *filePath(char *path) {
    // returned path = beginning to last_slash + (last_slash + 1) to dot + .hack
    char *ret = (char *)malloc(sizeof(char) * (strlen(path) + 10));
    memset(ret, 0, strlen(path) + 10);

    // first part
    char *last_slash = strrchr(path, '/');
    char *part1 = "";  // if no slash exist, file in same directory. part1=empty

    if (last_slash != NULL) {
        part1 = (char *)malloc(sizeof(char) * strlen(path));
        memset(part1, 0, strlen(path));

        // beginning to last_slash
        strncpy(part1, path, last_slash - path + 1);
    }

    // second part
    char *dot = strrchr(path, '.');
    char *part2 = path;  // if no dot exist, file is just a name. part2=path

    if (dot != NULL) {
        part2 = (char *)malloc(sizeof(char) * strlen(path));
        memset(part2, 0, strlen(path));

        if (last_slash != NULL) {
            // (last_slash + 1) to dot
            strncpy(part2, last_slash + 1, dot - last_slash - 1);
        } else {
            // "xxx.asm" path to dot
            strncpy(part2, path, dot - path);
        }
    }

    strcat(ret, part1);
    strcat(ret, part2);
    strcat(ret, ".hack");

    return ret;
}

Type type(char *inst) {
    Type ret = C;
    if (inst[0] == '@') {
        // A_VAL or A_VAR
        ret = A_VAL;
        for (int i = 1; i < strlen(inst); i++) {
            bool isdigit = inst[i] >= '0' && inst[i] <= '9';
            if (!isdigit) ret = A_VAR;
        }
    } else if (inst[0] == '/') {
        // SINGLE_COMMENT or MULTI_COMMENT
        ret = SINGLE_COMMENT;
    } else if (inst[0] == '(') {
        ret = LABEL;
    }
    return ret;
}

int value(char *inst) {
    if (type(inst) != A_VAL) return atoi(INVALID);

    return atoi(strdup(inst + 1));  // +1 to ignore '@'
}

char *label(char *inst) {
    if (type(inst) != A_VAR) return INVALID;

    return strdup(inst + 1);  // +1 to ignore '@'
}

char *dest(char *inst) {
    if (type(inst) != C) return INVALID;

    // destination exist iff there is '='
    char *equal = strrchr(inst, '=');
    if (equal == NULL) return INVALID;

    // dest=comp
    char *ret = (char *)malloc(sizeof(char) * FIELD_LENGTH);
    memset(ret, 0, FIELD_LENGTH);
    strncpy(ret, inst, equal - inst);

    return ret;
}
char *comp(char *inst) {
    if (type(inst) != C) return INVALID;

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
    if (equal == NULL) {
        if (semi == NULL) {
            // case 1: comp
            strcpy(ret, inst);
        } else {
            // case 2: comp;jmp
            strncpy(ret, inst, semi - inst);
        }
        return ret;
    }

    if (semi == NULL) {
        // case3 3: dest=comp
        strcpy(ret, equal + 1);
    } else {
        // case 4: dest=comp;jmp
        strncpy(ret, equal + 1, semi - equal - 1);
    }
    return ret;
}
char *jump(char *inst) {
    if (type(inst) != C) return INVALID;

    // jump exist iff there is ';'
    char *semi = strrchr(inst, ';');
    if (semi == NULL) return INVALID;

    // dest=comp;jmp or comp;jmp
    return strdup(semi + 1);
}

Parser *ParserConstructor() {
    Parser *instance = (Parser *)malloc(sizeof(Parser));

    // Initialize function pointers
    instance->filePath = &filePath;
    instance->type = &type;
    instance->value = &value;
    instance->label = &label;
    instance->dest = &dest;
    instance->comp = &comp;
    instance->jump = &jump;

    return instance;
}

void ParserDestructor(Parser *this) {
    // free function pointers
    this->filePath = NULL;
    this->type = NULL;
    this->value = NULL;
    this->label = NULL;
    this->dest = NULL;
    this->comp = NULL;
    this->jump = NULL;

    // TODO: uncomment this after unit testing
    // free(this)
}

#endif