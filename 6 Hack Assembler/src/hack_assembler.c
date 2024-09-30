#ifndef _HACK_ASSEMBLER_H
#define _HACK_ASSEMBLER_H

#include "coder.c"
#include "parser.c"
#include "symbol_table.c"

typedef struct HackAssembler HackAssembler;

/// @brief responsible for converting .asm file to .hack
struct HackAssembler
{
    // fields
    char *file_name;
    Coder *coder;
    Parser *parser;
    SymbolTable *table;

    // function pointers
    void (*assemble)(HackAssembler *, char *);
};

/// @brief reads .asm file and add the (LABELS) to the symbol table
/// @param this pointer to HackAssembler instance
static void firstPass(HackAssembler *this)
{
    FILE *file = fopen(this->file_name, "r");
    if (file == NULL)
    {
        printf("Cannot open the file HackAssembler::firstPass()\n");
        return;
    }

    char line[256];
    memset(line, 0, sizeof(line));

    int inst_num = 0;
    while (fgets(line, sizeof(line), file))
    {
        char *inst = this->parser->getInstruction(line);

        Type inst_type = this->parser->type(line);
        if (inst_type == ERROR)
            continue;
        if (inst_type != LABEL)
        {
            // add all previous (LABELS, line_num)
            if (inst_type != SINGLE_COMMENT)
            {
                inst_num++;
            }
            free(inst);
            continue;
        }
        // a label, need to add it to the symbol table
        // add it at when the first instruction that isn't LABEL after it
        // first instruction that isn't LABEL = inst_num
        char *label = this->parser->label(inst);
        this->table->insert(this->table, label, inst_num);

        free(inst);
        // free(label);
    }

    fclose(file);
}

/// @brief reads .asm file and convert it to .hack file
/// @param this pointer to HackAssembler instance
static void secondPass(HackAssembler *this)
{
    FILE *file = fopen(this->file_name, "r");
    FILE *outf = fopen(this->parser->targetFile(this->file_name), "w");
    if (file == NULL || outf == NULL)
    {
        printf("Cannot open the file HackAssembler::secondPass()\n");
        return;
    }

    char line[256];
    memset(line, 0, sizeof(line));

    int inst_num = 0;
    int cur_address = 16;
    while (fgets(line, sizeof(line), file))
    {
        char *inst = this->parser->getInstruction(line);

        Type inst_type = this->parser->type(inst);
        if (inst_type == LABEL || inst_type == SINGLE_COMMENT ||
            inst_type == ERROR)
            continue;
        if (inst_type == A_VAL || inst_type == A_VAR)
        {
            // if it's A_VAL, will get the value direct, if it =-1
            // @123, val = 123
            int val = this->parser->value(inst);
            if (inst_type == A_VAR)
            {
                char *label = this->parser->label(inst);
                // label: could be LABEL or new variable
                int added = this->table->get(this->table, label);

                // not added in first pass: (new variable)
                // add it, then use its value
                if (added == -1)
                {
                    val = cur_address;
                    this->table->insert(this->table, label, cur_address);
                    cur_address++; // move to next available one
                }
                else
                    val = added;

                // deallocate memory
                // free(label);
            }
            char *machine_code = this->coder->convertA(val);

            // writing machine_code to the output file
            fputs(machine_code, outf);
            fputs("\n", outf);
            free(machine_code);
        }
        else
        {
            char *d = this->parser->dest(this->parser, inst);
            char *c = this->parser->comp(this->parser, inst);
            char *j = this->parser->jump(this->parser, inst);
            char *machine_code = this->coder->convertC(this->coder, d, c, j);

            // writing machine_code to the output file
            fputs(machine_code, outf);
            fputs("\n", outf);

            // deallocate memory
            // free(d);
            // free(c);
            // free(j);
            free(machine_code);
        }
    }
}

/// @brief Converts file at path from .asm to .hack
/// @param this pointer to HackAssembler instance
/// @param path of the file to be assembled/ translated
void assemble(HackAssembler *this, char *path)
{
    this->file_name = strdup(path);

    // clear old table content
    // pre defined symbols
    this->table->count = 23;

    firstPass(this);
    secondPass(this);

    free(this->file_name);
    this->file_name = NULL;
}

/// @brief creates new instance of HackAssembler
/// @return pointer to an HackAssembler
HackAssembler *newHackAssembler()
{
    HackAssembler *instance = (HackAssembler *)malloc(sizeof(HackAssembler));

    // initialize fields
    instance->file_name = NULL;
    instance->coder = newCoder();
    instance->parser = newParser();
    instance->table = newSymbolTable(100);
    instance->table->addPredefined(instance->table);

    // initialize function pointers
    instance->assemble = &assemble;

    return instance;
}

/// @brief deallocates memory of this
/// @param this pointer to HackAssembler instance
void deleteHackAssembler(HackAssembler **this)
{
    HackAssembler *assembler = *this;

    // free fields
    assembler->file_name = NULL;
    deleteCoder(&assembler->coder);
    deleteParser(&assembler->parser);
    deleteSymbolTable(&assembler->table);

    // free function pointers
    assembler->assemble = NULL;

    // TODO: uncomment this after unit testing
    free(*this);
    *this = NULL;
}

#endif