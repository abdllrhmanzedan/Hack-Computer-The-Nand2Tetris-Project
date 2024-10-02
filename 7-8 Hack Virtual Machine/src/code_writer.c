/**
 * @file code_writer.c
 * @author abdllrhmanzedan
 * @brief declares the implementation and internals of the CodeWriter
 */

#include "../include/code_writer.h"

const char *segments[] = {"LCL", "ARG", "THIS", "THAT"};

void writeArithmeticLogical(char *instr, FILE *out)
{
}

void writePush(FILE *out, const char *filename, const char *segment, const char *index)
{
    /**
     * // D constains the data to push
     * @SP
     * A=M    // *sp
     * M=D    // *sp=D
     * @SP    // sp++
     * M=M+1
     */
    fprintf(out, "// push %s %s", segment, index);
    for (int i = 0; i < 4; i++)
    {
        if (strcmp(segment, segments[i]) == 0)
        {
            /**
             * @i     (value not symbol) base address
             * D=M
             * @index (value not symbol)
             * A=D+M
             * D=M
             * ...
             */
            fprintf(out, "@%d\n", i);
            fprintf(out, "D=M\n");
            fprintf(out, "@%d\n", index);
            fprintf(out, "A=D+M\n");
            fprintf(out, "D=M\n");
            return;
        }
    }

    if (strcmp(segment, "constant") == 0)
    {
        fprintf(out, "@%d\n", index);
        fprintf(out, "D=A\n");
    }

    if (strcmp(segment, "static") == 0)
    {
        fprintf(out, "@%s%d\n", filename, index);
        fprintf(out, "D=M");
    }

    if (strcmp(segment, "temp") == 0)
    {
        // RAM[5+index]
        fprintf(out, "@5\n");
        fprintf(out, "D=A\n");
        fprintf(out, "@%d\n", index);
        fprintf(out, "D=D+A\n");
        fprintf(out, "A=D");
        fprintf(out, "D=M");
    }

    if (strcmp(segment, "pointer") == 0)
    {
        if (index == 0)
            fprintf(out, "@3\n"); // THIS

        else
            fprintf(out, "@4\n"); // THAT
        fprintf(out, "D=M\n");
    }

    fprintf(out, "@SP\n");
    fprintf(out, "A=M\n");
    fprintf(out, "M=D\n");
    fprintf(out, "@SP\n");
    fprintf(out, "M=M+1\n\n");
}

void writePop(char *instr, FILE *out)
{
}

CodeWriter *newCodeWriter()
{
}

void deleteCodeWriter(CodeWriter **this)
{
}