/**
 * @file code_writer.h
 * @author abdllrhmanzedan
 * @brief declares the interface of the CodeWriter
 */

#ifndef _CODER_WRITER_H
#define _CODER_WRITER_H

#include <stdio.h>

typedef struct CodeWriter CodeWriter;

struct CodeWriter
{
    /**
     * @brief writes the equivalent hack-assembly instructions for arithmetic and logical operations
     * @param instr vm instruction
     * @param out stream to write to
     */
    void (*writeArithmeticLogical)(char *instr, FILE *out);

    /**
     * @brief writes the equivalent hack-assembly instructions for push operation
     * @param instr vm instruction
     * @param out stream to write to
     */
    void (*writePush)(char *instr, FILE *out);

    /**
     * @brief writes the equivalent hack-assembly instructions for pop operation
     * @param instr vm instruction
     * @param out stream to write to
     */
    void (*writePop)(char *instr, FILE *out);
};

/**
 * @brief initializes a CodeWriter instance
 * @return A pointer to a initialized CodeWriter instance
 */
CodeWriter *newCodeWriter();

/**
 * @brief deallocates any memory by CodeWriter
 * @param this pointer to a CodeWriter pointer, to deallocate
 */
void deleteCodeWriter(CodeWriter **this);

#endif