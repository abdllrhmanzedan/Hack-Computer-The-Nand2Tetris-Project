#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define MAX_NUMERIC_VALUE (1 << 15) - 1
#define INSTRUCTION_LENGTH 16
#define FIELD_LENGTH 5
#define DEST_LENGTH 3
#define COMP_LENGTH 7
#define JUMP_LENGTH 3
#define INVALID "-1"

typedef enum Type {
    A_VAL,
    A_VAR,
    C,
    LABEL,
    SINGLE_COMMENT,
    ERROR
} Type;

#endif
