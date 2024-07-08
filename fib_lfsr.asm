; %include "asm_io.inc"

; Fibonacci LFSR from https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Example_in_Python
section .data
    %assign _STARTING_BIT 0x800000; only the last 24 bits are 
    %assign _24_BITS 0xFFFFFF; only the last 24 bits are 1
    %assign _BIT_MASK_TAPS 0xE10000 ; mask where only TAPS are one (24,22,21,20) 

section .text
    global _lfsr ; rename to start with underscore if not in linux, __lfsr
    global get_polynomial
    global loop_seed



    ; 24 23 21 20
    ; 24 23 22 17
    get_polynomial:
    ; shifts lfsr in edx so 23rd bit is the bit to perform xor on the 23rd bit of lfsr in eax
        mov edx, eax; eax has lfsr ; 
        and edx, _BIT_MASK_TAPS ; 
        shl edx, 1 ; 24th bit: original 23rd bit
        xor eax, edx ; 24th bit xor 23rd
        shl edx, 1  ; 24th bit: original 21st bit
        xor eax, edx; 24th bit xor 23rd xor 21st
        shl edx, 5 ; 24th bit: original 20th bit
        xor eax, edx ; 24th bit xor 23rd xor 21st xor 20th
        ; shl edx, 19; 24th bit : original 0 bit
        ; shl eax, 23
        and eax, _STARTING_BIT; 24th bit xor 23rd xor 21st xor 20th and 1
        
        ; xor eax, edx 
        ; xor eax, _STARTING_BIT; 24th bit xor 23rd xor 21st xor 20th xor 1ste
        ; and eax, _STARTING_BIT ; only the 24th bit can be 1, to push into lfsr later
        ret  ; 24th bit in eax is polynomial for TAP 23,22, 20,19 (0 index)
    ; lfsr has been shifted in eax by polynomial with TAP bit 24, 23, 21, 20)
    ; ITS GETTING OUT OF 24 BITS LIMIT
    

    loop_seed:
            RDTSC 
            shr eax,8 ; 24 bits
            cmp eax,0 ; seed 0 garantees 0 output so we'll avoid it
            je loop_seed  
            
    _lfsr: ; _start is in stdlib with printf 
    push ebp
        ; get seed from current clock time
    ; https://www.diva-portal.org/smash/get/diva2:812512/FULLTEXT01.pdf    
    ; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time
        
            
        mov edx, eax
        mov ebp, esp ; starts stack: lfsr


    ; shifts lfsr in edx so 23rd bit is the bit to perform xor on the 23rd bit of lfsr in eax
        mov edx, eax; eax has lfsr ; 
        and edx, _BIT_MASK_TAPS ; 
        shl edx, 1 ; 24th bit: original 23rd bit
        xor eax, edx ; 24th bit xor 23rd
        shl edx, 1  ; 24th bit: original 21st bit
        xor eax, edx; 24th bit xor 23rd xor 21st
        shl edx, 5 ; 24th bit: original 20th bit
        xor eax, edx ; 24th bit xor 23rd xor 21st xor 20th
        ; shl edx, 19; 24th bit : original 0 bit
        ; shl eax, 23
        and eax, _STARTING_BIT; 24th bit xor 23rd xor 21st xor 20th and 1

        ; call get_polynomial ; puts polynomial in eax, eax must have lfsr
        shr edx, 1 ; shift lfsr by 1
        or edx, eax ; pushes polynomial to leftmost of lfsr. Note: edx must have bits from 0-22 as 0. This is deont in get_polynomial
    ; lfsr = polynomial | lfsr << 1 , upper 8 bits are 0. polynomial must be 0 except in 24th bit
        mov eax, edx
        ; AL, AX or DX:A
        pop ebp
        ret

; this was made for linux
