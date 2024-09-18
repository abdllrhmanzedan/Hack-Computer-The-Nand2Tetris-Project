# Hack Assembler
It's the **linchpin** that connects the hardware platform and the software hierarchy.
This project introduces key software engineering techniques (multi-pass assemblers, parsing, code generation, symbol tables, etc..).

## Hack Assembler with hardware
![assembler](https://i.imgur.com/GVXKA0U.png)

It translates Hack Assembly Language into Hack Machine Code, enabling programs to run on the Hack CPU.

## What It Does?
- **File Handling:** opens `.asm` files and reads their content.
- **Parsing and Cleaning:** processes each line of the `.asm` file, removing unnecessary elements to create a `canonical form` suitable to operate on.
- **Instruction Classification:** classifies instructions into distinct types:
  - `A_VAR`: an A-instruction that on form `@var`.
  - `A_VAL`: an A-instruction with a numeric value, (e.g. `@123`).
  - `C`: a  ny type of C-instruction.
  - `SINGLE_COMMENT`: a comment line that is ignored.
  - `ERROR`: a comment line that is ignored.
- **Instruction Parsing:** breaks down instructions into simpler components for processing:
  - A-instruction are reduced to either a `value` or a `label` that will later be converted to a value.
  - C-instructions are split into `dest`, `comp`, and `jump` fields.
- **Code Generation:** converts each component of the parsed instruction into its machine code equivalent.
- **Two-Pass Assembly:**
  1. First pass: identifies and adds all `(LABEL)` declarations with their corresponding memory addresses to the symbol table, in order to be ready when needed in `Second Pass`.
  2. Second pass: replaces symbols with their values from the symbol table, translates the instructions into machine code, and outputs the `.hack` file.

By testing these components, I’ve ensured that the assembler functions accurately and efficiently.

## How to Use?
1. Place your Hack Assembly Language file `prog.asm` into the `input/` directory.
2. Run `src/main.c`.
3. The resulting machine code will be saved as `prog.hack` in the same directory.

## Example Usage
To demonstrate how the Hack Assembler works, here’s a simple example of an input assembly file and its corresponding machine code output:

**Input File** `prog.ams` **:**
```as
@2         // Load the value 2 into register A
D=A        // Set D register to the value of A (which is 2)
@3         // Load the value 3 into register A
D=D+A      // Add the value of A (3) to D (2), resulting in D = 5
(LOOP)     // Define a label 'LOOP' that can be referenced later
@LOOP      // Load the value of LOOP into register A
0;JMP      // Unconditional jump to address 0 (infinite loop)
```

**Output File** `prog.hack` **:**
```hack
0000000000000010  // @2
1110110000010000  // D=A
0000000000000011  // @3
1110000010010000  // D=D+A
0000000000000100  // @LOOP
1110101010000111  // 0;JMP
```

## Tested Components
By applying unit testing to ensure the core components of the Hack Assembler work as expected:
- **Coder:** converts parsed instructions into machine code.
- **Parser:** reads and processes assembly lines into a clean, workable format.
- **Symbol Table:** manages labels and variables, mapping them to memory addresses.