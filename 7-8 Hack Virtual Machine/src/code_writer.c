/**
 * @file code_writer.c
 * @author abdllrhmanzedan
 * @brief declares the implementation and internals of the CodeWriter
 */

#include "../include/code_writer.h"

const char *segments[] = {"LCL", "ARG", "THIS", "THAT"};

void writeArithmeticLogical(FILE *out, const char *filename, const char *instr)
{
    /**
     * for operations on last 2 elements:
     * @SP
     * A=M
     * D=D+M  // D=D+st.top()
     */
    fprintf(out, "\\ %s\n", instr);

    static int cnt = 0;

    // D=stack.top()
    fprintf(out, "@SP\n");
    fprintf(out, "A=M\n");
    fprintf(out, "D=M\n");

    // stack.pop()
    fprintf(out, "@SP\n");
    fprintf(out, "M=M-1\n");

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
        // D=stack.top()
        fprintf(out, "@SP\n");
        fprintf(out, "A=M\n");

        // operation
        if (strcmp(instr, "add") == 0)
        {
            fprintf(out, "D=M+D\n");
        }
        else if (strcmp(instr, "sub") == 0)
        {
            fprintf(out, "D=M-D\n");
        }
        else if (strcmp(instr, "and") == 0)
        {
            fprintf(out, "D=M&D\n");
        }
        else if (strcmp(instr, "or") == 0)
        {
            fprintf(out, "D=M|D\n");
        }
        else
        {
            fprintf(out, "D=M-D\n");
            // filename_operation_cnt
            fprintf(out, "@%s_%s_%d\n", filename, instr, cnt);

            if (strcmp(instr, "eq") == 0)
                fprintf(out, "D;JEQ");
            else if (strcmp(instr, "gt") == 0)
                fprintf(out, "D;JGT");
            else
                fprintf(out, "D;JLT");

            // not true
            fprintf(out, "D=0\n");

            // true
            fprintf(out, "(%s_%s_%d)\n", filename, instr, cnt);
            fprintf(out, "D=1\n");
            cnt++;
        }
        // stack.pop()
        fprintf(out, "@SP");
        fprintf(out, "M=M-1");
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
        if (strcmp(segment, segments[i]) == 0)
        {
            /**
             * @segment
             * D=M         D=base address
             * @index      value not symbol
             * A=D+A       A=base address + index
             * ...
             */
            fprintf(out, "@%s\n", segment); //
            fprintf(out, "D=M\n");          // D=base_address
            fprintf(out, "@%d\n", index);   //
            fprintf(out, "A=D+A\n");        // M = RAM[segment + i]
            return 0;
        }
    }

    if (strcmp(segment, "constant") == 0)
    {
        fprintf(out, "@%d\n", index);
        return 1;
    }

    if (strcmp(segment, "static") == 0)
    {
        fprintf(out, "@%s.%d\n", filename, index);
    }

    if (strcmp(segment, "temp") == 0)
    {
        fprintf(out, "@5\n");
        fprintf(out, "D=A\n");
        fprintf(out, "@%d\n", index);
        fprintf(out, "D=D+A\n");
        fprintf(out, "A=D"); // A=5+index
    }

    if (strcmp(segment, "pointer") == 0)
    {
        if (index == 0)
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
    fprintf(out, "// push %s %s", segment, index);

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
     * @SP
     * M=M-1
     * @SP
     * A=M
     * D=M
     * loading the address to A
     * M=D
     */
    fprintf(out, "// pop %s %s", segment, index);

    printf(out, "@SP\n");
    printf(out, "M=M-1\n");
    printf(out, "@SP\n");
    printf(out, "A=M\n");
    printf(out, "D=M\n");
    loadAddress(out, filename, segment, index); // no pop constant;
    printf(out, "M=D\n");
}

CodeWriter *newCodeWriter()
{
}

void deleteCodeWriter(CodeWriter **this)
{
}