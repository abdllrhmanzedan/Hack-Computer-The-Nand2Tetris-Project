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
    void (*writeArithmeticLogical)(FILE *out, const char *filename, const char *instr);

    /**
     * @brief writes the equivalent hack-assembly instructions for push operation
     * @param segment which segment to operate on
     * @param index the index inside the segment
     * @param out stream to write to
     */
    void (*writePush)(FILE *out, const char *filename, const char *segment, const char *index);

    /**
     * @brief writes the equivalent hack-assembly instructions for pop operation
     * @param instr vm instruction
     * @param out stream to write to
     */
    void (*writePop)(FILE *out, const char *filename, const char *segment, const char *index);

    void (*writeBranching)(FILE *out, const char *instr, const char *label);

    void (*writeCall)(FILE *out, const char *function_name, const char *n_args);

    void (*writeFunction)(FILE *out, const char *function_name, const char *n_vars);

    void (*writeReturn)(FILE *out);

    void (*writeInit)(FILE *out);
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