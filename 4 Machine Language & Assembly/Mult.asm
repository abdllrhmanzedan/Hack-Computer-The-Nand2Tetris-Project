// Program: mult.asm
// Multiplies the values stored in R0 and R1, and stores the result in R2 (i.e., R2 = R0 * R1)
// Usage: Put the first number in RAM[0] (R0), the second number in RAM[1] (R1), and the result will be stored in RAM[2]

// Initialize result and loop counter:
@R2
M=0

@i
M=0

@pow
M=1

(LOOP)
    @15
    D=A
    @i
    D=M-D
    @END
    D;JGT // if (i-15 > 0) goto END

    // Check if the ith bit of R0 is set (i.e., R0 & pow != 0), where pow=2^i
    @R0
    D=M
    @pow
    D=D&M
    @SKIP_ADD
    D;JEQ           // If D == 0, skip the addition (bit not set)

    // Add R1 to R2 (R2 += R1) if the ith bit of R0 is set
    @R2
    D=M
    @R1
    D=D+M
    @R2
    M=D

(SKIP_ADD)
    // Double R1 (R1 *= 2)
    @R1
    D=M
    @R1
    M=D+M

    // Double pow (pow *= 2)
    @pow
    D=M
    @pow
    M=D+M

    // Increment loop counter i (i++)
    @i
    M=M+1

    @LOOP
    0;JMP

(END)
@END
0;JMP