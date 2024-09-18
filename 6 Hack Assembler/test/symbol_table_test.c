#include "../src/symbol_table.c"

#include <stdio.h>

#include "testing_util.c"

// note: remember to free your instances!
// note: when testing comment the free/NULL in destructor!

SymbolTable *constructorTest(char *test_name) {
    bool passed = true;
    SymbolTable *st = newSymbolTable(10);

    // check the table has the right values
    passed &= (st->capacity == 10 && st->count == 0);

    // check the table is initialized
    passed &= (st->table != NULL);

    // check the function pointers are assigned
    passed &= (st->insert != NULL && st->get != NULL);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    // assert(passed);

    return st;
}

void destructorTest(char *test_name, SymbolTable *st) {
    bool passed = true;
    deleteSymbolTable(&st);

    // check the table has the right values
    passed &= (st->capacity == 0 && st->count == 0);

    // check the table is initialized
    passed &= (st->table == NULL);

    // check the function pointers are assigned
    passed &= (st->insert == NULL && st->get == NULL);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    // assert(passed);

    // free Symbol Table itself after checking its internals
    free(st);
}

void insertTest(char *test_name) {
    bool passed = true;
    SymbolTable *st = newSymbolTable(5);
    st->insert(st, "instruction 1", 1);
    passed &= (st->count == 1);

    st->insert(st, "instruction 2", 2);
    passed &= (st->count == 2);

    st->insert(st, "instruction 3", 3);
    passed &= (st->count == 3);

    st->insert(st, "instruction 4", 4);
    passed &= (st->count == 4);

    st->insert(st, "instruction 5", 5);
    passed &= (st->count == st->capacity);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    // assert(passed);
    destructorTest("Destructor after insertion", st);
}

void getTest(char *test_name) {
    bool passed = true;
    SymbolTable *st = newSymbolTable(5);
    st->insert(st, "instruction 1", 1);
    passed &= (st->get(st, "instruction 1") == 1);

    st->insert(st, "instruction 2", 2);
    passed &= (st->get(st, "instruction 2") == 2);

    st->insert(st, "instruction 3", 3);
    passed &= (st->get(st, "instruction 3") == 3);

    st->insert(st, "instruction 4", 4);
    passed &= (st->get(st, "instruction 4") == 4);

    st->insert(st, "instruction 5", 5);
    passed &= (st->get(st, "instruction 5") == 5);

    // checking for handling non exists entries
    passed &= (st->get(st, "instruction 6") == -1);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    // assert(passed);
    destructorTest("Destructor after insertion and retrieval", st);
}

void expandTest(char *test_name) {
    bool passed = true;
    SymbolTable *st = newSymbolTable(3);
    passed &= (st->capacity == 3);

    // filling the table
    st->insert(st, "instruction 1", 1);
    st->insert(st, "instruction 2", 2);
    st->insert(st, "instruction 3", 3);

    // table full but didn't expand yet
    passed &= (st->capacity == 3 && st->count == st->capacity);

    // adding entries after the expand and check the retrieval
    st->insert(st, "instruction 4", 4);
    passed &= (st->get(st, "instruction 4") == 4);

    st->insert(st, "instruction 5", 5);
    passed &= (st->get(st, "instruction 5") == 5);

    st->insert(st, "instruction 6", 6);
    passed &= (st->get(st, "instruction 6") == 6);

    // table expanded by 2, and full
    passed &= (st->capacity == 6 && st->capacity == st->capacity);

    // table expanded by 2, again
    st->insert(st, "instruction 7", 7);
    passed &= (st->capacity == 12 && st->capacity == st->capacity);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    // assert(passed);
    destructorTest("Destructor after expand", st);
}

void reinitializeTest(char *test_name) {
    bool passed = true;
    SymbolTable *st = newSymbolTable(3);

    st->insert(st, "instruction 1", 1);
    st->insert(st, "instruction 2", 2);
    st->insert(st, "instruction 3", 3);

    deleteSymbolTable(&st);
    free(st);

    st = newSymbolTable(3);
    passed &= (st->capacity == 3 && st->count == 0);

    // checking insertion/ retrieval after reinitialize
    st->insert(st, "instruction 1", 1);
    passed &= (st->count == 1);
    passed &= (st->get(st, "instruction 1") == 1);

    st->insert(st, "instruction 2", 2);
    passed &= (st->count == 2);
    passed &= (st->get(st, "instruction 2") == 2);

    st->insert(st, "instruction 3", 3);
    passed &= (st->count == 3);
    passed &= (st->get(st, "instruction 3") == 3);

    st->insert(st, "instruction 4", 4);
    passed &= (st->get(st, "instruction 4") == 4);

    // table expanded by 2
    passed &= (st->capacity == 6 && st->capacity == st->capacity);

    if (passed)
        TEST_PASS(test_name);
    else
        TEST_FAIL(test_name);
    // assert(passed);
    destructorTest("Destructor after reinitialize", st);
}

void SYMBOL_TABLE_TEST_SUITE() {
    TEST_SUITE_START("Symbol Table");

    SymbolTable *st = constructorTest("Constructor");
    destructorTest("Destructor", st);

    insertTest("Insertion");
    getTest("Retrieve/ get");
    expandTest("Expand");
    reinitializeTest("Table Reinitialize");

    TEST_SUITE_SUCCESS("Symbol Table");
}
int main(int argc, char const *argv[])
{
    SYMBOL_TABLE_TEST_SUITE();
    return 0;
}
