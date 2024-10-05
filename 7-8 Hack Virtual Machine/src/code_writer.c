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

    // doesn't need other operand
    if (strcmp(instr, "neg") == 0)
    {
        fprintf(out, "@SP\n");
        fprintf(out, "A=M-1\n");
        fprintf(out, "M=-M\n");
    }
    else if (strcmp(instr, "not") == 0)
    {
        fprintf(out, "@SP\n");
        fprintf(out, "A=M-1\n");
        fprintf(out, "M=!M\n");
    }
    else
    {
        fprintf(out, "@SP\n");
        fprintf(out, "AM=M-1\n");
        fprintf(out, "D=M\n");

        fprintf(out, "A=A-1\n");

        // operation
        if (strcmp(instr, "add") == 0)
        {
            fprintf(out, "M=D+M\n");
        }
        else if (strcmp(instr, "sub") == 0)
        {
            fprintf(out, "M=M-D\n");
        }
        else if (strcmp(instr, "and") == 0)
        {
            fprintf(out, "M=D&M\n");
        }
        else if (strcmp(instr, "or") == 0)
        {
            fprintf(out, "M=D|M\n");
        }
        else
        {
            fprintf(out, "D=M-D\n");

            // filename_operation_cnt
            fprintf(out, "@%s%s%dtrue\n", filename, instr, cnt);

            if (strcmp(instr, "eq") == 0)
                fprintf(out, "D;JEQ\n");
            else if (strcmp(instr, "gt") == 0)
                fprintf(out, "D;JGT\n");
            else
                fprintf(out, "D;JLT\n");

            fprintf(out, "D=0\n");
            fprintf(out, "@%s_%s_%dfalse\n", filename, instr, cnt);
            fprintf(out, "0;JMP\n");

            fprintf(out, "(%s%s%dtrue)\n", filename, instr, cnt);
            fprintf(out, "D=-1\n");

            fprintf(out, "(%s_%s_%dfalse)\n", filename, instr, cnt);
            cnt++;

            // stack.push(D)
            fprintf(out, "@SP\n");
            fprintf(out, "A=M-1\n");
            fprintf(out, "M=D\n");
        }
    }
    // fprintf(out, "\n");
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
    fprintf(out, "AM=M+1\n");
    fprintf(out, "A=A-1\n");
    fprintf(out, "M=D\n");
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
    fprintf(out, "M=D\n");
}

void writeBranching(FILE *out, const char *instr, const char *label)
{
    fprintf(out, "// %s %s\n", instr, label);
    if (strcmp(instr, "goto") == 0)
    {
        fprintf(out, "@%s\n", label);
        fprintf(out, "0;JMP\n");
    }
    else if (strcmp(instr, "if-goto") == 0)
    {
        fprintf(out, "@SP\n");
        fprintf(out, "AM=M-1\n");
        fprintf(out, "D=M\n");
        fprintf(out, "@%s\n", label);
        fprintf(out, "D;JNE\n");
    }
    else
    {
        fprintf(out, "(%s)\n", label);
    }
}

void push(FILE *out, const char *val, int direct)
{
    fprintf(out, "@%s\n", val);
    if (direct)
        fprintf(out, "D=A\n");
    else
        fprintf(out, "D=M\n");

    fprintf(out, "@SP\n");
    fprintf(out, "AM=M+1\n");
    fprintf(out, "A=A-1\n");
    fprintf(out, "M=D\n");
}

void writeCall(FILE *out, const char *function_name, const char *n_args)
{
    /**
     * push return_address
     * push LCL
     * push ARG
     * push THIS
     * push THAT
     *
     * ARG = SP-5-n_args
     * LCL = SP
     *
     * goto function_name
     *
     * (return_address)
     */
    fprintf(out, "// call %s %s\n", function_name, n_args);
    static int cnt = 0;
    char buffer[50];
    sprintf(buffer, "returnAddress%d", cnt++);

    // pushing 5 addresses to stack
    push(out, buffer, 1);
    push(out, "LCL", 0);
    push(out, "ARG", 0);
    push(out, "THIS", 0);
    push(out, "THAT", 0);

    // ARG = SP-5-n_args
    fprintf(out, "@SP\n");
    fprintf(out, "D=M\n");
    fprintf(out, "@5\n");
    fprintf(out, "D=D-A\n");
    fprintf(out, "@%s\n", n_args);
    fprintf(out, "D=D-A\n");
    fprintf(out, "@ARG\n");
    fprintf(out, "M=D\n");

    // LCL = SP
    fprintf(out, "@SP\n");
    fprintf(out, "D=M\n");
    fprintf(out, "@LCL\n");
    fprintf(out, "M=D\n");

    // goto
    writeBranching(out, "goto", function_name);

    // return address
    writeBranching(out, "label", buffer);
}

void writeFunction(FILE *out, const char *function_name, const char *n_vars)
{
    fprintf(out, "// function %s %s\n", function_name, n_vars);
    writeBranching(out, "label", function_name);

    int cnt = atoi(n_vars);
    while (cnt--)
        push(out, "0", 1);
}

void restore(FILE *out, const char *seg)
{
    fprintf(out, "@LCL\n");
    fprintf(out, "AM=M-1\n");
    fprintf(out, "D=M\n");
    fprintf(out, "@%s\n", seg);
    fprintf(out, "M=D\n");
}

void writeReturn(FILE *out)
{
    fprintf(out, "// return\n");

    fprintf(out, "@LCL\n");
    fprintf(out, "D=M\n");
    fprintf(out, "@5\n");
    fprintf(out, "A=D-A\n");
    fprintf(out, "D=M\n");
    fprintf(out, "@R14\n"); // R14 = return_address
    fprintf(out, "M=D\n");

    // *arg = pop (return value)
    fprintf(out, "@SP\n");
    fprintf(out, "AM=M-1\n");
    fprintf(out, "D=M\n");
    fprintf(out, "@ARG\n");
    fprintf(out, "A=M\n");
    fprintf(out, "M=D\n");

    // SP = arg+1
    fprintf(out, "@ARG\n");
    fprintf(out, "D=M\n");
    fprintf(out, "@SP\n");
    fprintf(out, "M=D+1\n");

    // restoring segments
    restore(out, "THAT");
    restore(out, "THIS");
    restore(out, "ARG");
    restore(out, "LCL");

    // jump to return address
    fprintf(out, "@R14\n");
    fprintf(out, "A=M\n");
    fprintf(out, "0;JMP\n");
}

void writeInit(FILE *out)
{
    // sp=256
    fprintf(out, "@256\n");
    fprintf(out, "D=A\n");
    fprintf(out, "@SP\n");
    fprintf(out, "M=D\n");

    // call Sys.init
    writeCall(out, "Sys.init", "0");
}

CodeWriter *newCodeWriter()
{
    CodeWriter *instance = (CodeWriter *)malloc(sizeof(CodeWriter));
    instance->writeArithmeticLogical = &writeArithmeticLogical;
    instance->writePop = &writePop;
    instance->writePush = &writePush;
    instance->writeBranching = &writeBranching;
    instance->writeCall = &writeCall;
    instance->writeFunction = &writeFunction;
    instance->writeReturn = &writeReturn;
    instance->writeInit = &writeInit;
}

void deleteCodeWriter(CodeWriter **this)
{
    CodeWriter *instance = *this;
    instance->writeArithmeticLogical = NULL;
    instance->writePop = NULL;
    instance->writePush = NULL;
    instance->writeBranching = NULL;
    instance->writeCall = NULL;
    instance->writeFunction = NULL;
    instance->writeReturn = NULL;
    instance->writeInit = NULL;

    free(*this);
    *this = NULL;
}