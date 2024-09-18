#include "../src/parser.c"

#include <stdio.h>

#include "testing_util.c"

// note: remember to free your instances!
// note: when testing comment the free/NULL in deleteParser!
bool validate(Parser *parser, char *op, char *in, char *ac) {
    char *test = strdup(in);
    bool passed = true;
    if (strcmp(op, "path") == 0) {
        passed &= (strcmp(parser->targetFile(test), ac) == 0);
    } else if (strcmp(op, "inst") == 0) {
        passed &= (strcmp(parser->getInstruction(test), ac) == 0);
    } else if (strcmp(op, "value") == 0) {
        passed &= (parser->value(test) == atoi(ac));
    } else if (strcmp(op, "label") == 0) {
        passed &= (strcmp(parser->label(test), ac) == 0);
    } else if (strcmp(op, "dest") == 0) {
        passed &= (strcmp(parser->dest(parser, test), ac) == 0);
    } else if (strcmp(op, "comp") == 0) {
        passed &= (strcmp(parser->comp(parser, test), ac) == 0);
    } else if (strcmp(op, "jump") == 0) {
        passed &= (strcmp(parser->jump(parser, test), ac) == 0);
    }

    free(test);
    return passed;
}

Parser *constructorTest(char *test_name) {
    bool passed = true;
    Parser *parser = newParser();

    // check the function pointers are assigned
    passed &= (parser->targetFile != NULL && parser->type != NULL);
    passed &= (parser->value != NULL && parser->label != NULL);
    passed *=
        (parser->dest != NULL && parser->comp != NULL && parser->jump != NULL);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    return parser;
}
void destructorTest(char *test_name, Parser *parser) {
    bool passed = true;
    deleteParser(&parser);

    // check the function pointers are assigned
    passed &= (parser->targetFile == NULL && parser->type == NULL);
    passed &= (parser->value == NULL && parser->label == NULL);
    passed *=
        (parser->dest == NULL && parser->comp == NULL && parser->jump == NULL);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
}
void filePathTest(char *test_name) {
    bool passed = true;
    Parser *parser = newParser();

    // check handling {slash, dot}: 00, 01, 10, 11
    passed &= validate(parser, "path", "file", "file.hack");
    passed &= validate(parser, "path", "file.asm", "file.hack");
    passed &=
        validate(parser, "path", "input/meow/file", "input/meow/file.hack");
    passed &=
        validate(parser, "path", "input/meow/file.asm", "input/meow/file.hack");

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after getting target file path", parser);
}

void getInstructionTest(char *test_name) {
    bool passed = true;
    Parser *parser = newParser();
    char *test;

    // test handling {spaces, comment}: 00 01 10 11
    passed &= validate(parser, "inst", "@var", "@var");
    passed &= validate(parser, "inst", "@99", "@99");
    passed &= validate(parser, "inst", "@D", "@D");
    passed &= validate(parser, "inst", "D=M", "D=M");
    passed &= validate(parser, "inst", "0;JMP", "0;JMP");
    passed &= validate(parser, "inst", "D=0;JMP", "D=0;JMP");

    passed &= validate(parser, "inst", "      @var  ", "@var");
    passed &= validate(parser, "inst", " @99         ", "@99");
    passed &= validate(parser, "inst", "       @D     ", "@D");
    passed &= validate(parser, "inst", " D   =   M   ", "D=M");
    passed &= validate(parser, "inst", "  0 ; JMP  ", "0;JMP");
    passed &= validate(parser, "inst", "D =0 ;JMP", "D=0;JMP");

    passed &= validate(parser, "inst", "@var//author:zedanov", "@var");
    passed &= validate(parser, "inst", "@9//9         ", "@9");
    passed &= validate(parser, "inst", "//@D     ", "");
    passed &= validate(parser, "inst", "D//   =   M   ", "D");
    passed &=
        validate(parser, "inst", " 0 ; JMP//unconditional jump  ", "0;JMP");
    passed &= validate(parser, "inst", "D =0 //;JMP", "D=0");

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after getting instruction from a line", parser);
}

void typeTest(char *test_name) {
    bool passed = true;
    Parser *parser = newParser();

    passed &= (parser->type("@var") == A_VAR);
    passed &= (parser->type("@9595") == A_VAL);
    passed &= (parser->type("D+1") == C);
    passed &= (parser->type("D-1") == C);
    passed &= (parser->type("M-1") == C);
    passed &= (parser->type("M+1") == C);
    passed &= (parser->type("0;JMP") == C);
    passed &= (parser->type("D=A+1;JMP") == C);
    passed &= (parser->type("ADM=D+1;JMP") == C);
    passed &= (parser->type("D&M;JMP") == C);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after getting type of an A instruction", parser);
}

void valueTest(char *test_name) {
    bool passed = true;
    Parser *parser = newParser();

    // test regular values
    passed &= validate(parser, "value", "@0", "0");
    passed &= validate(parser, "value", "@1", "1");
    passed &= validate(parser, "value", "@32767", "32767");

    // test invalid types (has no value)
    passed &= validate(parser, "value", "D+1", INVALID);
    passed &= validate(parser, "value", "D+M", INVALID);

    // test negative
    passed &= validate(parser, "value", "@-1", INVALID);

    // test overflow
    passed &= validate(parser, "value", "@32768", INVALID);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after getting value of an A instruction",
                   parser);
}

void labelTest(char *test_name) {
    bool passed = true;
    Parser *parser = newParser();

    // test not having a label
    passed &= validate(parser, "label", "@123", INVALID);
    passed &= validate(parser, "label", "ADM", INVALID);
    passed &= validate(parser, "label", "D+1", INVALID);

    // test regular case
    passed &= validate(parser, "label", "@var", "var");
    passed &= validate(parser, "label", "@_", "_");

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after getting label of an A instruction",
                   parser);
}
void destTest(char *test_name) {
    bool passed = true;
    Parser *parser = newParser();

    // test regular case
    passed &= validate(parser, "dest", "D=A", "D");
    passed &= validate(parser, "dest", "M=D-1", "M");
    passed &= validate(parser, "dest", "A=D|A", "A");
    passed &= validate(parser, "dest", "MD=D+1", "MD");
    passed &= validate(parser, "dest", "ADM=D&M", "ADM");

    // test invalid case
    passed &= validate(parser, "dest", "@var", INVALID);
    passed &= validate(parser, "dest", "D", INVALID);
    passed &= validate(parser, "dest", "A", INVALID);
    passed &= validate(parser, "dest", "A-1", INVALID);
    passed &= validate(parser, "dest", "M-1", INVALID);
    passed &= validate(parser, "dest", "D;JMP", INVALID);
    passed &= validate(parser, "dest", "D;JMP", INVALID);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);

    destructorTest("Destructor after getting dest field of an instruction",
                   parser);
}
void compTest(char *test_name) {
    bool passed = true;
    Parser *parser = newParser();

    // test regular case: comp, comp;jmp, dest=comp, dest=comp;jmp
    passed &= validate(parser, "comp", "D+1", "D+1");
    passed &= validate(parser, "comp", "D|M", "D|M");

    passed &= validate(parser, "comp", "0;JMP", "0");
    passed &= validate(parser, "comp", "D;JMP", "D");
    passed &= validate(parser, "comp", "D+1;JGT", "D+1");
    passed &= validate(parser, "comp", "M-1;JEQ", "M-1");

    passed &= validate(parser, "comp", "M=M-1", "M-1");
    passed &= validate(parser, "comp", "D=D|M", "D|M");
    passed &= validate(parser, "comp", "D=1", "1");
    passed &= validate(parser, "comp", "ADM=0", "0");
    passed &= validate(parser, "comp", "M=-1", "-1");

    passed &= validate(parser, "comp", "D=D-1;JEQ", "D-1");
    passed &= validate(parser, "comp", "D=D&M;JGT", "D&M");
    passed &= validate(parser, "comp", "M=D+M;JGE", "D+M");

    // test invalid case
    passed &= validate(parser, "comp", "@meow", INVALID);
    passed &= validate(parser, "comp", "@1919", INVALID);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after getting comp field of an C instruction",
                   parser);
}
void jumpTest(char *test_name) {
    bool passed = true;
    Parser *parser = newParser();

    // test regular case
    passed &= validate(parser, "jump", "0;JGT", "JGT");
    passed &= validate(parser, "jump", "D-1;JEQ", "JEQ");
    passed &= validate(parser, "jump", "D+1;JGT", "JGT");
    passed &= validate(parser, "jump", "D-1;JLT", "JLT");
    passed &= validate(parser, "jump", "1;JNE", "JNE");
    passed &= validate(parser, "jump", "D|M;JLE", "JLE");
    passed &= validate(parser, "jump", "D&M;JMP", "JMP");

    // test invalid case
    passed &= validate(parser, "jump", "0", INVALID);
    passed &= validate(parser, "jump", "D+1", INVALID);
    passed &= validate(parser, "jump", "D+D-1", INVALID);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after getting jump field of an C instruction",
                   parser);
}
void PARSER_TEST_SUITE() {
    TEST_SUITE_START("Parser");

    Parser *parser = constructorTest("Constructor");
    destructorTest("Destructor", parser);
    filePathTest("Getting target file path");
    getInstructionTest("Extracting instruction from a line");
    typeTest("Getting type of an instruction");
    valueTest("Getting value of an A instruction");
    labelTest("Getting label of an A instruction");
    destTest("Getting the dest field of an C instruction");
    compTest("Getting the comp field of an C instruction");
    jumpTest("Getting the jump field of an C instruction");

    TEST_SUITE_SUCCESS("Parser");
}
int main(int argc, char const *argv[]) {
    PARSER_TEST_SUITE();
    return 0;
}
