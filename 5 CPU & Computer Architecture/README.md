# CPU and Computer Architecture
This project integrates the chips built in Projects 1, 2, and 3 (Logic Chips, ALU, and Memory Units) into a *architecture* that can execute any program written in the Hack machine language.

## Hack Computer Architecture
![architecture](https://i.imgur.com/JyDYjVa.png)

The Hack Computer is designed to execute programs written in Hack Machine Language, which are stored and run on the Hack CPU.
This architecture enables the computer to perform a wide range of tasks by executing machine instructions sequentially.

## What It Does?
- **Arithmetic and Logical Operations:** the Hack Computer performs arithmetic and logical operations using the ALU and registers.
- **Stored Program Concept:** it utilizes two memory chips, `RAM` and `ROM`, to store and execute programs.
- **Instruction Execution:** executes machine instructions one by one to run an entire programs.
- **General-Purpose Capability:** capable of running complex programs, including games e.g. Tetris.

## How It Works?
The Hack Computer architecture is designed with abstraction in mind:
- **CPU**: 
  - **Inputs:** `inM`, `instruction`, `reset`
  - **Outputs:** `outM`, `writeM`, `addressM`, `pc`
  - **Process:** the CPU fetches instructions, performs computations, and stores results.
- **ROM:** stores the program code in a read-only, which the CPU fetches and executes.
- **RAM:** stores data and variables that the CPU accesses and modifies during execution.

By abstracting each component, we create a complete computer where each part can be developed and tested independently, resulting in a complete functional Hack-Computer.

## Testing
Each component has been tested using provided test scripts to ensure standalone functionality and correctness.