/**
 * @file vm_translator.h
 * @author abdllrhmanzedan
 * @brief declares the interface of the VMTranslator
 */

#ifndef _VM_TRANSLATOR_H
#define _VM_TRANSLATOR_H

typedef struct VMTranslator VMTranslator;

struct VMTranslator
{
    /**
     * @brief translates .vm file to .asm file
     * @param path to the .vm file
     */
    void (*translateFile)(const char *path);

    void (*translateDir)(const char *dir_path);
};

/**
 * @brief initializes a VMTranslator instance
 * @return A pointer to a initialized VMTranslator instance
 */
VMTranslator *newVMTranslator();

/**
 * @brief deallocates any memory by VMTranslator
 * @param this pointer to a VMTranslator pointer, to deallocate
 */
void deleteVMTranslator(VMTranslator **this);

#endif