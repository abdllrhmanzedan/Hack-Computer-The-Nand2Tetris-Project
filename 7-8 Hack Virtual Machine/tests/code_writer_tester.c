#include "../src/code_writer.c"

void test_ArithmeticLogic()
{
    FILE *file = fopen("meow.asm", "w");

    writeArithmeticLogical(file, "out", "neg");
    writeArithmeticLogical(file, "out", "not");
    writeArithmeticLogical(file, "out", "add");
    writeArithmeticLogical(file, "out", "sub");
    writeArithmeticLogical(file, "out", "and");
    writeArithmeticLogical(file, "out", "or");
    writeArithmeticLogical(file, "out", "eq");
    writeArithmeticLogical(file, "out", "gt");
    writeArithmeticLogical(file, "out", "lt");

    fclose(file);
}

void test_push()
{
    FILE *file = fopen("meow.asm", "w");

    writePush(file, "meow", "local", "15");
    // writePush(file, "meow", "argument", "15");
    // writePush(file, "meow", "this", "15");
    // writePush(file, "meow", "that", "15");
    // writePush(file, "meow", "constant", "15");
    // writePush(file, "meow", "static", "15");
    // writePush(file, "meow", "temp", "15");
    // writePush(file, "meow", "pointer", "0");
    // writePush(file, "meow", "pointer", "1");

    fclose(file);
}

void test_pop()
{
    FILE *file = fopen("meow.asm", "w");

    writePop(file, "meow", "local", "15");
    writePop(file, "meow", "argument", "15");
    writePop(file, "meow", "this", "15");
    writePop(file, "meow", "that", "15");
    writePop(file, "meow", "static", "15");
    writePop(file, "meow", "temp", "15");
    writePop(file, "meow", "pointer", "0");
    writePop(file, "meow", "pointer", "1");

    fclose(file);
}
int main(int argc, char const *argv[])
{
    // test_ArithmeticLogic();
    // test_push();
    // test_pop();
    return 0;
}
