#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "../include/vm_translator.h"

int isFile(const char *filename)
{
    struct stat path;
    stat(filename, &path);
    return S_ISREG(path.st_mode);
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("few arguments!");
        return 1;
    }

    VMTranslator *vmTranslator = newVMTranslator();
    if (isFile(argv[1]))
    {
        vmTranslator->translateFile(argv[1]);
        return 0;
    }

    vmTranslator->translateDir(argv[1]);

    deleteVMTranslator(&vmTranslator);
    return 0;
}