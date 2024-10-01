#include "../include/logger.h"

int main(int argc, char const *argv[])
{
    consoleLog("The meow logger is here!", SUCCESS);

    const char *str = "meow1 meow2 ";
    char *space2 = strrchr(str, ' ');
    printf("%s", space2+1);
    return 0;
}