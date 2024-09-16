// Program: fill.asm
// Description: Fills the entire screen with black (sets all pixels to 1) once any key is pressed.
// Usage: Press any key on the keyboard, and the screen will turn black.

(LOOP)
    // if (!NOT_PRESSED)
    //    val=0 (don't change)
    // else
    //    val=-1
    @val
    M=0

    @KBD
    D=M
    @NOT_PRESSED
    D;JEQ
    
    @val
    M=-1

(NOT_PRESSED)
    // set address to the start of the screen-map memory
    @SCREEN
    D=A
    @address
    M=D

(DRAW_LOOP)
    // SCREEN_END = KBD-1
    @KBD
    D=A
    @address
    D=D-M
    @END
    D;JEQ

    // set the current screen row to the value of 'val'
    @val
    D=M
    @address
    A=M
    M=D

    // move to the next screen address
    @address
    M=M+1

    // repeat the loop to fill the next screen row
    @DRAW_LOOP
    0;JMP
    
(END)
    @LOOP
    0;JMP