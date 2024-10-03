/**
 * @file code_writer.c
 * @author abdllrhmanzedan
 * @brief declares the implementation and internals of the CodeWriter
 */

#include <string.h>
#include <stdlib.h>

#include "../include/code_writer.h"

const char *segments_name[] = {"local", "argument", "this", "that"};
const char *segments_base[] = {"LCL", "ARG", "THIS", "THAT"};

void writeArithmeticLogical(FILE *out, const char *filename, const char *instr)
{
    /**
     * for operations on last 2 elements:
     * @SP
     * A=M
     * D=D+M  // D=D+st.top()
     */
    fprintf(out, "// %s\n", instr);

    static int cnt = 0;

    fprintf(out, "@SP\n");
    fprintf(out, "M=M-1\n");

    // D=stack.top()
    fprintf(out, "@SP\n");
    fprintf(out, "A=M\n");
    fprintf(out, "D=M\n");

    // doesn't need other operand
    if (strcmp(instr, "neg") == 0)
    {
        // negating
        fprintf(out, "D=-D\n");
    }
    else if (strcmp(instr, "not") == 0)
    {
        fprintf(out, "D=!D\n");
    }
    else
    {

        // stack.pop()
        fprintf(out, "@SP\n");
        fprintf(out, "M=M-1\n");

        // D=stack.top()
        fprintf(out, "@SP\n");
        fprintf(out, "A=M\n");

        // operation
        if (strcmp(instr, "add") == 0)
        {
            fprintf(out, "D=D+M\n");
        }
        else if (strcmp(instr, "sub") == 0)
        {
            fprintf(out, "D=D-M\n");
            fprintf(out, "D=-D\n");
        }
        else if (strcmp(instr, "and") == 0)
        {
            fprintf(out, "D=D&M\n");
        }
        else if (strcmp(instr, "or") == 0)
        {
            fprintf(out, "D=D&M\n");
        }
        else
        {
            fprintf(out, "D=D-M\n");
            // filename_operation_cnt
            fprintf(out, "@%s_%s_%d\n", filename, instr, cnt);

            if (strcmp(instr, "eq") == 0)
                fprintf(out, "D;JEQ\n");
            else if (strcmp(instr, "gt") == 0)
                fprintf(out, "D;JLT\n");
            else
                fprintf(out, "D;JGT\n");

            // not true
            fprintf(out, "D=0\n");

            // true
            fprintf(out, "(%s_%s_%d)\n", filename, instr, cnt);
            fprintf(out, "D=1\n");
            cnt++;
        }
    }

    // stack.push(D)
    fprintf(out, "@SP\n");
    fprintf(out, "A=M\n");
    fprintf(out, "M=D\n");
    fprintf(out, "@SP\n");
    fprintf(out, "M=M+1\n\n");
}

int loadAddress(FILE *out, const char *filename, const char *segment, const char *index)
{
    // local, arg, this, that
    for (int i = 0; i < 4; i++)
    {
        if (strcmp(segment, segments_name[i]) == 0)
        {
            /**
             * @segment
             * D=M         D=base address
             * @index      value not symbol
             * A=D+A       A=base address + index
             * ...
             */
            fprintf(out, "@%s\n", segments_base[i]); //
            fprintf(out, "D=M\n");                   // D=base_address
            fprintf(out, "@%s\n", index);            //
            fprintf(out, "AD=D+A\n");                // M = RAM[segment + i]
            return 0;
        }
    }

    if (strcmp(segment, "constant") == 0)
    {
        fprintf(out, "@%s\n", index);
        return 1;
    }

    if (strcmp(segment, "static") == 0)
    {
        fprintf(out, "@%s.%s\n", filename, index);
    }

    if (strcmp(segment, "temp") == 0)
    {
        fprintf(out, "@5\n");
        fprintf(out, "D=A\n");
        fprintf(out, "@%s\n", index);
        fprintf(out, "AD=D+A\n");
    }

    if (strcmp(segment, "pointer") == 0)
    {
        if (strcmp(index, "0") == 0)
            fprintf(out, "@THIS\n");

        else
            fprintf(out, "@THAT\n");
    }
    return 0;
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
    fprintf(out, "// push %s %s\n", segment, index);

    int seg = loadAddress(out, filename, segment, index);
    if (seg == 0)
    {
        // we get value from M, RAM[x]
        fprintf(out, "D=M\n");
    }
    else
    {
        // we get value from A (only with constant)
        fprintf(out, "D=A\n");
    }

    fprintf(out, "@SP\n");
    fprintf(out, "A=M\n");
    fprintf(out, "M=D\n");
    fprintf(out, "@SP\n");
    fprintf(out, "M=M+1\n\n");
}

void writePop(FILE *out, const char *filename, const char *segment, const char *index)
{
    /**
     * loading the address to A
     * D=A
     * @SP
     * M=M-1
     * @SP
     * A=M
     * M=D
     */
    fprintf(out, "// pop %s %s\n", segment, index);

    loadAddress(out, filename, segment, index); // no pop constant;

    if (strcmp(segment, "static") == 0 || strcmp(segment, "pointer") == 0)
        fprintf(out, "D=A\n");

    // saving address in R13
    fprintf(out, "@R13\n");
    fprintf(out, "M=D\n");

    // loading data in D
    fprintf(out, "@SP\n");
    fprintf(out, "AM=M-1\n");
    fprintf(out, "D=M\n");

    // RAM[address] = D
    fprintf(out, "@R13\n");
    fprintf(out, "A=M\n");
    fprintf(out, "M=D\n\n");
}

CodeWriter *newCodeWriter()
{
    CodeWriter *instance = (CodeWriter *)malloc(sizeof(CodeWriter));
    instance->writeArithmeticLogical = &writeArithmeticLogical;
    instance->writePop = &writePop;
    instance->writePush = &writePush;
}

void deleteCodeWriter(CodeWriter **this)
{
    CodeWriter *instance = *this;
    instance->writeArithmeticLogical = NULL;
    instance->writePop = NULL;
    instance->writePush = NULL;

    free(*this);
    *this = NULL;
}