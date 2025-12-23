// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

//// Replace this comment with your code.
(LOOP)
    @KBD
    D=M
    @FILL_BLACK
    D;JNE
    @FILL_WHITE
    0;JMP
(FILL_BLACK)
    @SCREEN
    D=A
    @addr
    M=D
(BLACK_LOOP)
    @addr
    A=M
    M=1
    @addr
    M=M+1
    @addr
    D=M
    @24576
    D=D-A
    @loop
    D;JEQ
    @BLACK_LOOP
    0;JMP
(FILL_WHITE)
    @SCREEN
    D=A
    @addr
    M=D
(WHITE_LOOP)
    @addr
    A=M
    M=0
    @addr
    M=M+1
    @addr
    D=M
    @24576
    D=D-A
    @LOOP
    D;JEQ
    @WHITE_LOOP
    0;JMP

