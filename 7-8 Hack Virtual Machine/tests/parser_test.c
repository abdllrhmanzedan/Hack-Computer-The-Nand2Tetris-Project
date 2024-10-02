#include <stdio.h>
#include <assert.h>

#include "../src/parser.c"
#include "../include/parser.h"
#include "../include/logger.h"

void printCase(const char *case_, const char *test)
{
    char test_name[256];
    sprintf(test_name, "%s case test running in %s test", case_, test);
    consoleLog(test_name, INFO);
}

void eval_outPath(const char *case_, const char *in, const char *acc)
{
    // printCase(case_, "outPath");

    // testing
    bool passed = true;
    char *file = NULL;
    outPath(in, &file);
    if (file == NULL)
        passed = (acc == NULL);
    else
        passed &= (strcmp(acc, file) == 0);

    // evaluating
    if (!passed)
        consoleLog("Failed in outPath test", CRITICAL);
    else
        consoleLog("Passed in outPath test", SUCCESS);

    // deallocating
    free(file);
    file = NULL;
}

void test_outPath()
{
    // regular case
    eval_outPath("Regular case", "file.vm", "file.asm");

    // regular case with spaces+backslashes
    eval_outPath("Regular case with full path", "meow/love/sad blurry/file.vm",
                 "meow/love/sad blurry/file.asm");

    // invalid format
    eval_outPath("Invalid format case", "file", NULL);

    // invalid format 2
    eval_outPath("Invalid format case", "sds/sds/.vm/file", NULL);
}

void test_fileName()
{
    char *filename = NULL;
    fileName("meow.vm", &filename);
    printf("%s mwo", filename);
}

void eval_fixInstruction(const char *case_, const char *in, const char *acc)
{
    // printCase(case_, "fixInstruction");

    // testing
    bool passed = true;
    char *instr = NULL;
    fixInstruction(strdup(in), &instr);
    if (instr == NULL)
        passed = false;
    else
        passed = (strcmp(instr, acc) == 0);

    // evaluating
    if (!passed)
        consoleLog("Failed in fixInstruction test", CRITICAL);
    else
        consoleLog("Passed in fixInstruction test", SUCCESS);

    // deallocating
    free(instr);
    instr = NULL;
}
void test_fixInstruction()
{
    // regular case
    eval_fixInstruction("Regular case", "add", "add");
    eval_fixInstruction("Regular case", "push love 1", "push love 1");
    eval_fixInstruction("Regular case", "goto label", "goto label");
    eval_fixInstruction("Regular case", "function meow 2", "function meow 2");

    // spaces
    eval_fixInstruction("Regular with spaces", "   add    ", "add");
    eval_fixInstruction("Regular case", "      push love 1    ", "push love 1");
    eval_fixInstruction("Regular case", "   goto label   ", "goto label");
    eval_fixInstruction("Regular case", " function meow 2  ", "function meow 2");

    // comments
    eval_fixInstruction("Regular with spaces", "   add//  zedanov", "add");
    eval_fixInstruction("Regular case", "      push love 1  //zzzzz sleepy  ", "push love 1");
    eval_fixInstruction("Regular case", "   goto label   // meow comment", "goto label");
    eval_fixInstruction("Regular case", " function meow 2   //zedanov is meowing  ", "function meow 2");
}

void eval_type(const char *case_, const char *in, Type acc)
{
    // printCase(case_, "type");

    // testing
    bool passed = true;
    Type instr_type = type(in);

    passed = (instr_type == acc);

    // evaluating
    if (!passed)
        consoleLog("Failed in Type test", CRITICAL);
    else
        consoleLog("Passed in Type test", SUCCESS);
}
void test_type()
{
    // Arithmetic
    eval_type("Arithmetic type", "add", ARITHMETIC);
    eval_type("Arithmetic type", "sub", ARITHMETIC);
    eval_type("Arithmetic type", "neg", ARITHMETIC);

    // Logical
    eval_type("Logical type", "eq", LOGICAL);
    eval_type("Logical type", "gt", LOGICAL);
    eval_type("Logical type", "lt", LOGICAL);
    eval_type("Logical type", "or", LOGICAL);
    eval_type("Logical type", "and", LOGICAL);
    eval_type("Logical type", "not", LOGICAL);
    eval_type("Logical with wrong spelling type", "Not", UNKOWN);

    // memory
    eval_type("Pop type", "pop meow 2", POP);
    eval_type("Push type", "push ass 3", PUSH);
    eval_type("Push with wrong spelling", "Push ass 3", UNKOWN);

    // Branching
    eval_type("Logical type", "label zedanov", LABEL);
    eval_type("Logical type", "goto zedanov", GOTO);
    eval_type("Logical type", "if-goto zedanov", IF);
    eval_type("Logical with wrong spelling", "if-Goto zedanov", UNKOWN);

    // Function
    eval_type("Function type", "function zedanov xxx", FUNCTION);
    eval_type("Call type", "call zedanov xxx", CALL);
    eval_type("Call type", "return", RETURN);
    eval_type("Call type", "reTurn", UNKOWN);
}

void eval_arg(const char *in, const char *acc, int op)
{
    // printCase(case_, "arg1");

    // testing
    bool passed = true;
    char *arg = NULL;
    if (op == 1)
        arg1(in, &arg); // will receive fixed instruction
    else
        arg2(in, &arg);

    if (arg == NULL)
        passed = (acc == NULL);
    else
        passed = (strcmp(arg, acc) == 0);

    // evaluating
    if (!passed)
        consoleLog("Failed in arg 1 test", CRITICAL);
    else
        consoleLog("Passed in arg 1 test", SUCCESS);

    // deallocating
    free(arg);
    arg = NULL;
}

void test_arg()
{
    // arg1
    // arithmetic and logical
    eval_arg("add", "add", 1);
    eval_arg("sub", "sub", 1);
    eval_arg("neg", "neg", 1);
    eval_arg("eq", "eq", 1);
    eval_arg("gt", "gt", 1);
    eval_arg("lt", "lt", 1);
    eval_arg("or", "or", 1);
    eval_arg("and", "and", 1);
    eval_arg("not", "not", 1);

    // memory access
    eval_arg("push segment i", "segment", 1);
    eval_arg("pop local i", "local", 1);

    // branching
    eval_arg("label meow", "meow", 1);
    eval_arg("goto zedanov", "zedanov", 1);
    eval_arg("if-goto hoh", "hoh", 1);

    // function
    eval_arg("function hoh 8", "hoh", 1);
    eval_arg("call doggy 8", "doggy", 1);
    eval_arg("return", "return", 1);
    /*********************************************************************/

    // arithmetic and logical
    eval_arg("add", NULL, 2);

    // memory
    eval_arg("push segment 7878", "7878", 2);
    eval_arg("pop local 321", "321", 2);

    // branching
    eval_arg("label meow", NULL, 2);

    /// function
    eval_arg("function hoh 8", "8", 2);
    eval_arg("call doggy 888", "888", 2);
    eval_arg("return", NULL, 2);
}

void test_parser()
{
    Parser *parser = newParser();
    bool passed = true;

    passed &= (parser->outPath == &outPath);
    passed &= (parser->type == &type);
    passed &= (parser->arg1 == &arg1);
    passed &= (parser->arg2 == &arg2);

    assert(passed);

    deleteParser(&parser);
    // passed &= (parser->outPath == NULL);
    // passed &= (parser->type == NULL);
    // passed &= (parser->arg1 == NULL);
    // passed &= (parser->arg2 == NULL);
}
int main(int argc, char const *argv[])
{
    // test_outPath();
    test_fileName();
    // test_fixInstruction();
    // test_type();
    // test_arg();
    // test_parser();
    consoleLog("Parser testing DONE!", SUCCESS);
    return 0;
}