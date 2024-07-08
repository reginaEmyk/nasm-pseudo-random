; %include "asm_io.inc"

; Fibonacci LFSR from https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Example_in_Python
section .data
    %assign _STARTING_BIT 0x800000; only the last 24 bits are 
    %assign _24_BITS 0xFFFFFF; only the last 24 bits are 1
    %assign _BIT_MASK_TAPS 0xE10000 ; mask where only TAPS are one (24,22,21,20) 
    %assign array_size 16777
    array times array_size db 0; 
    

; todo use mask that will pick from first bit so that it won't push out the 1's and leave with no 1 ever again
; ie 000000...1 bye all that wasted rthats why urs didnt work?

section .text
    global _lfsr ; rename to start with underscore if not in linux, __lfsr
    global get_polynomial
    global loop_seed

    loop_seed:
            RDTSC 
            shr eax,8 ; 24 bits
            cmp eax,0 ; seed 0 garantees 0 output so we'll avoid it
            je loop_seed  
            ret

    get_polynomial:
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
        xor eax, _STARTING_BIT; 24th bit xor 23rd xor 21st xor 20th xor 1, to avoid 0
        ret
            
    _lfsr: ; _start is in stdlib with printf 
    push ebp
    mov  ebp, esp
    mov eax, [esp+8] ; previous stack + 4 for eax
    mov ecx, 0 ; to access array

    ; get seed from current clock time
    ; https://www.diva-portal.org/smash/get/diva2:812512/FULLTEXT01.pdf    
    ; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time
    call_loop_seed:
        call loop_seed
        ; todo generate 1M per wewerwer
    cmp eax, 0
    je call_loop_seed

    gen_lfsr: 
        push eax

   ; shifts lfsr in edx so 23rd bit is the bit to perform xor on the 23rd bit of lfsr in eax
        call get_polynomial ; puts polynomial in eax, eax must have lfsr
        pop edx ; restore lfsr
        shr edx, 1 ; shift lfsr by 1
        or edx, eax ; pushes polynomial to leftmost of lfsr. Note: edx must have bits from 0-22 as 0. This is deont in get_polynomial
    ; lfsr = polynomial | lfsr << 1 , upper 8 bits are 0. polynomial must be 0 except in 24th bit
        mov [array+ecx], edx ; push pseudo random number to array
        inc ecx ; increment counter
        cmp ecx, array_size
        jl gen_lfsr ; todo jl or jle

        ; AL, AX or DX:A
        mov eax, array
        mov esp, ebp
        pop ebp ; restore stack to return to main
        
        ret

; this was made for linux
