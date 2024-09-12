// Program: fill.asm
// Description: Fills the entire screen with black (sets all pixels to 1) once any key is pressed.
// Usage: Press any key on the keyboard, and the screen will turn black.

(LOOP)
    // Reset val to 0 (no key pressed)
    @val
    M=0

    @KBD
    D=M
    @NOT_PRESSED
    D;JEQ
    
    // If a key is pressed, set val to -1 (prepare to fill screen)
    @val
    M=-1

(NOT_PRESSED)
    // Set address to the start of the screen-map memory
    @SCREEN
    D=A
    @address
    M=D

(DRAW_LOOP)
    // SCREEN_END = KBD-1
    @KBD
    D=M
    @address
    D=D-M
    @END
    D;JEQ

    // Set the current screen row to the value of 'val' (-1 if key pressed)
    @val
    D=M
    @address
    A=M
    M=D

    // Move to the next screen address
    @address
    M=M+1

    // Repeat the loop to fill the next screen row
    @DRAW_LOOP
    0;JMP
    
(END)
@LOOP
0;JMP