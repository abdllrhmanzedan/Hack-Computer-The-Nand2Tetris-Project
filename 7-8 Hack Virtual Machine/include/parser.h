/**
 * @file parser.h
 * @author abdllrhmanzedan
 * @brief declares the interface of the Parser
 */

#ifndef _PARSER_H
#define _PARSER_H

typedef struct Parser Parser;
typedef enum Type Type;

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
    COMMENT,
    ERROR
};

struct Parser
{
    /**
     * @brief processes in_path to make an out_path in same directory and same name
     * but different file extention
     *
     * @param in_path the path to the .vm file
     * @param out_path the path to .asm to write data to
     */
    void (*outPath)(const char *in_path, char **out_path);

    /**
     * @brief gets the type of some instruction
     * @param instr instruction from .vm file to get its type
     * @returns enum Type indicates the type instr
     */
    Type (*type)(const char *instr);

    /**
     * @brief extracts an instruction our of the line
     * @param line input raw line from .vm file
     * @param instr to save the extracted instruction
     */
    void (*fixInstruction)(char *line, char **instr);

    /**
     * @brief gets the first part of some instruction
     * @param instr instruction to process
     * @param arg buffer to hold the content of the first part of some instruction
     */
    void (*arg1)(const char *instr, char **arg);

    /**
     * @brief gets the second part of some instruction
     * @param instr instruction to process
     * @param arg buffer to hold the content of the second part of some instruction
     */
    void (*arg2)(const char *instr, char **arg);
};

/**
 * @brief initializes a Parser instance
 * @return A pointer to a initialized Parser instance
 */
Parser *newParser();

/**
 * @brief deallocates any memory by Parser
 * @param this pointer to a Parser pointer, to deallocate
 */
void deleteParser(Parser **this);

#endif