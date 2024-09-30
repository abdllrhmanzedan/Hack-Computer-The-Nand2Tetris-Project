#include "../src/coder.c"

#include <stdio.h>

#include "testing_util.c"

// note: remember to free your instances!
// note: when testing comment the free/NULL in destructor!

Coder *constructorTest(char *test_name)
{
    bool passed = true;
    Coder *coder = newCoder();

    // check if the table is constructed
    passed &= (coder->table != NULL);

    // check the function pointers are assigned
    passed &= (coder->convertC != NULL && coder->convertC != NULL);

    passed &=
        (coder->dest != NULL && coder->comp != NULL && coder->jump != NULL);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    return coder;
}
void destructorTest(char *test_name, Coder *coder)
{
    bool passed = true;
    deleteCoder(&coder);

    // check table is destructed
    passed &= (coder->table == NULL);

    // check the function pointers are assigned
    passed &= (coder->convertC == NULL && coder->convertC == NULL);

    passed &=
        (coder->dest == NULL && coder->comp == NULL && coder->jump == NULL);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
}
void convertATest(char *test_name)
{
    bool passed = true;
    Coder *coder = newCoder();

    // check normal use
    passed &= (strcmp(coder->convertA(0), "0000000000000000") == 0);
    passed &= (strcmp(coder->convertA(1), "0000000000000001") == 0);
    passed &= (strcmp(coder->convertA(10), "0000000000001010") == 0);
    passed &=
        (strcmp(coder->convertA(MAX_NUMERIC_VALUE), "0111111111111111") == 0);

    // check for handling negative values
    passed &= (strcmp(coder->convertA(-1), "0111111111111111") == 0);
    passed &= (strcmp(coder->convertA(-2), "0111111111111110") == 0);

    // check for overflow
    passed &= (strcmp(coder->convertA(MAX_NUMERIC_VALUE + 1),
                      "0000000000000000") == 0);
    passed &= (strcmp(coder->convertA(MAX_NUMERIC_VALUE + 3),
                      "0000000000000010") == 0);
    passed &= (strcmp(coder->convertA(MAX_NUMERIC_VALUE + 9),
                      "0000000000001000") == 0);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after Convert A instruction", coder);
}

void destTest(char *test_name)
{
    bool passed = true;
    Coder *coder = newCoder();

    // check handling null, A, D, M, AD, AM, DM, ADM
    passed &= (strcmp(coder->dest(coder, INVALID), "000") == 0);
    passed &= (strcmp(coder->dest(coder, "A"), "100") == 0);
    passed &= (strcmp(coder->dest(coder, "D"), "010") == 0);
    passed &= (strcmp(coder->dest(coder, "M"), "001") == 0);
    passed &= (strcmp(coder->dest(coder, "AD"), "110") == 0);
    passed &= (strcmp(coder->dest(coder, "AM"), "101") == 0);
    passed &= (strcmp(coder->dest(coder, "MD"), "011") == 0);
    passed &= (strcmp(coder->dest(coder, "ADM"), "111") == 0);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);

    destructorTest("Destructor after encoding dest field", coder);
}
void compTest(char *test_name)
{
    bool passed = true;
    Coder *coder = newCoder();

    // check the table values
    passed &= (strcmp(coder->comp(coder, "0"), "0101010") == 0);
    passed &= (strcmp(coder->comp(coder, "1"), "0111111") == 0);
    passed &= (strcmp(coder->comp(coder, "-1"), "0111010") == 0);

    passed &= (strcmp(coder->comp(coder, "D"), "0001100") == 0);
    passed &= (strcmp(coder->comp(coder, "A"), "0110000") == 0);
    passed &= (strcmp(coder->comp(coder, "M"), "1110000") == 0);

    passed &= (strcmp(coder->comp(coder, "!D"), "0001101") == 0);
    passed &= (strcmp(coder->comp(coder, "!A"), "0110001") == 0);
    passed &= (strcmp(coder->comp(coder, "!M"), "1110001") == 0);

    passed &= (strcmp(coder->comp(coder, "-D"), "0001111") == 0);
    passed &= (strcmp(coder->comp(coder, "-A"), "0110011") == 0);
    passed &= (strcmp(coder->comp(coder, "-M"), "1110011") == 0);

    passed &= (strcmp(coder->comp(coder, "D+1"), "0011111") == 0);
    passed &= (strcmp(coder->comp(coder, "A+1"), "0110111") == 0);
    passed &= (strcmp(coder->comp(coder, "M+1"), "1110111") == 0);

    passed &= (strcmp(coder->comp(coder, "D-1"), "0001110") == 0);
    passed &= (strcmp(coder->comp(coder, "A-1"), "0110010") == 0);
    passed &= (strcmp(coder->comp(coder, "M-1"), "1110010") == 0);

    passed &= (strcmp(coder->comp(coder, "D+A"), "0000010") == 0);
    passed &= (strcmp(coder->comp(coder, "D+M"), "1000010") == 0);

    passed &= (strcmp(coder->comp(coder, "D-A"), "0010011") == 0);
    passed &= (strcmp(coder->comp(coder, "D-M"), "1010011") == 0);

    passed &= (strcmp(coder->comp(coder, "A-D"), "0000111") == 0);
    passed &= (strcmp(coder->comp(coder, "M-D"), "1000111") == 0);

    passed &= (strcmp(coder->comp(coder, "D&A"), "0000000") == 0);
    passed &= (strcmp(coder->comp(coder, "D&M"), "1000000") == 0);

    passed &= (strcmp(coder->comp(coder, "D|A"), "0010101") == 0);
    passed &= (strcmp(coder->comp(coder, "D|M"), "1010101") == 0);

    // check invalid calls
    passed &= (strcmp(coder->comp(coder, "zzz"), "1111111") == 0);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after encoding comp field", coder);
}

void jumpTest(char *test_name)
{
    bool passed = true;
    Coder *coder = newCoder();

    // check handling null, A, JGT, JEQ, JGE, JLT, JNE, JLE
    passed &= (strcmp(coder->jump(coder, INVALID), "000") == 0);
    passed &= (strcmp(coder->jump(coder, "JGT"), "001") == 0);
    passed &= (strcmp(coder->jump(coder, "JEQ"), "010") == 0);
    passed &= (strcmp(coder->jump(coder, "JGE"), "011") == 0);
    passed &= (strcmp(coder->jump(coder, "JLT"), "100") == 0);
    passed &= (strcmp(coder->jump(coder, "JNE"), "101") == 0);
    passed &= (strcmp(coder->jump(coder, "JLE"), "110") == 0);
    passed &= (strcmp(coder->jump(coder, "JMP"), "111") == 0);

    // check invalid calls
    passed &= (strcmp(coder->jump(coder, "JO"), "000") == 0);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after encoding jump field", coder);
}

void convertCTest(char *test_name)
{
    bool passed = true;
    Coder *coder = newCoder();

    // check handling: dest=comp, comp;jmp, dest=comp;jmp
    // comp (covered in compTest)

    // dest=comp
    // D=A
    passed &= (strcmp(coder->convertC(coder, "D", "A", INVALID),
                      "1110110000010000") == 0);
    // D=M
    passed &= (strcmp(coder->convertC(coder, "D", "M", INVALID),
                      "1111110000010000") == 0);
    // MD=A
    passed &= (strcmp(coder->convertC(coder, "MD", "A", INVALID),
                      "1110110000011000") == 0);
    // ADM=A+1
    passed &= (strcmp(coder->convertC(coder, "ADM", "A+1", INVALID),
                      "1110110111111000") == 0);

    // comp;jmp
    // 0;jmp
    passed &= (strcmp(coder->convertC(coder, INVALID, "0", "JMP"),
                      "1110101010000111") == 0);
    // 1;jmp
    passed &= (strcmp(coder->convertC(coder, INVALID, "1", "JMP"),
                      "1110111111000111") == 0);
    // D;JNE
    passed &= (strcmp(coder->convertC(coder, INVALID, "D", "JNE"),
                      "1110001100000101") == 0);
    // M;JMP
    passed &= (strcmp(coder->convertC(coder, INVALID, "M", "JGT"),
                      "1111110000000001") == 0);

    // dest=comp;jmp
    // D=D-1;JLT
    passed &= (strcmp(coder->convertC(coder, "D", "D-1", "JLT"),
                      "1110001110010100") == 0);
    // M=M-D;JLE
    passed &= (strcmp(coder->convertC(coder, "M", "M-D", "JLE"),
                      "1111000111001110") == 0);
    // MD=M|D;JGT
    passed &= (strcmp(coder->convertC(coder, "MD", "D|M", "JGT"),
                      "1111010101011001") == 0);
    // ADM=0;JMP
    passed &= (strcmp(coder->convertC(coder, "ADM", "0", "JMP"),
                      "1110101010111111") == 0);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    destructorTest("Destructor after convert C instruction", coder);
}
void CODER_TEST_SUITE()
{
    TEST_SUITE_START("Coder");

    Coder *coder = constructorTest("Constructor");
    destructorTest("Destructor", coder);
    convertATest("Convert A Instruction to Machine Code");
    destTest("Getting machine code of dest filed");
    compTest("Getting machine code of comp field");
    jumpTest("Getting machine code of jump field");
    convertCTest("Convert C Instruction to Machine Code");

    TEST_SUITE_SUCCESS("Coder");
}

int main(int argc, char const *argv[])
{
    CODER_TEST_SUITE();
    return 0;
}
