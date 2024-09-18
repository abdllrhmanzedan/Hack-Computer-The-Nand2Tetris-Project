#include <dirent.h>

#include "hack_assembler.c"

int main() {
    DIR* input_dir = opendir("../input");
    struct dirent* file;
    if (input_dir == NULL) {
        printf("Unable to open input directory ::main()");
        return 0;
    }

    HackAssembler* assembler = newHackAssembler();
    while (file = readdir(input_dir)) {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            continue;
        char* asm_file = strdup("../input/");
        strcat(asm_file, file->d_name);
        
        // assembling to .hack
        assembler->assemble(assembler, asm_file);
    }
    closedir(input_dir);
    file = NULL;
    input_dir = NULL;
    return 0;
}
