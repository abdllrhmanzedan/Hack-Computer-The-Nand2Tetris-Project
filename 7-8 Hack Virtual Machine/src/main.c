// #include "../include/logger.h"
#include "../include/vm_translator.h"

int main(int argc, char const *argv[])
{
    // consoleLog("The meow logger is here!", SUCCESS);

    VMTranslator *vmTranslator = newVMTranslator();

    vmTranslator->translate("meow.vm");
}