# Machine Language and Assembly
This project explores how binary code (0s, 1s) is translated into programs, such as games e.g. Tetris.
It focuses on creating structured sequences of bits that computers can understand and execute.

## Machine Language usage
![machine language](https://i.imgur.com/GVXKA0U.png)

Machine Language is the fundamental layer of software development for the Hack Computer.
It consists of binary instructions that the Hack *CPU* can directly execute.

## What It Does?
- **Addressing and Memory Accessing (A-instruction):** Hack-Machine-Language can access memory to retrieve instructions and store data. This allows the computer to deal with any memory location.
- **Arithmetic and Logical Operations (C-instruction):** Hack-Machine-Language supports arithmetic and logical operations using the registers `A`, `D`, and `M`. These operations allows making computations and decisions within programs.

## How It Works?
Hack Machine Language can be written in two ways:
- **Binary Code:** directly using (0s, 1s), which the Hack CPU can execute immediately.
- **Symbolic Code:** written in a more human-readable format and converted into binary code using as `assembler`. The assembler translates symbolic instructions into the binary format understood by the CPU.

## Example
Consider a program that computes the sum of values stored in `RAM[0]` and `RAM[1]`, and then stores the result in `RAM[2]`.
The equivalent Hack-Assembly code is:
```as
@0         // Load the value 0 into register A, and sets M=RAM[0]
D=M        // Set D register to the value of M, which is RAM[0]
@1         // Load the value 1 into register A, and sets M=RAM[1]
D=D+M      // Add the value of M (RAM[1]) to D (RAM[0]), resulting in D = RAM[0]+RAM[1]
@2         // Load the value 2 into register A, and sets M=RAM[2]
M=D        // Set RAM[2]=D (RAM[0]+RAM[1])

```