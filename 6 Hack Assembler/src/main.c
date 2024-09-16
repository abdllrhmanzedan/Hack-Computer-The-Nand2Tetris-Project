#include <stdio.h>
#include <string.h>

void ffff(char** x) {
    (*x)[0] = '1';  // Dereference the pointer correctly to modify the string.
}

int main(int argc, char const* argv[]) {
    char* m = strdup("wtfff");  // Use a character array to allow modification.
    
    printf("%s\n", m);   // Print before modification.
    ffff(&m);
    printf("%s\n", m);   // Print after modification.

    return 0;
}
