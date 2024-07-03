; Fibonacci LFSR from https://en.wikipedia.org/wiki/Linear-feedback_shift_register
%include "asm_io.inc"

section .data
    int_format	    db  "%i", 10; Null terminator and padding byte
    

section .text
    global main
    extern printf

    main:  

; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time
        loop_RDTSC:
            RDTSC
            cmp eax, 0  
            je loop_RDTSC ; seed can't be 0
        
        ; lfsr = start_state
        
        mov edx, 0 ;
        mov ecx, 0 ; period
        

        mov ebx, eax ; for shift operations
        mov edx, eax
        mov ebp, eax ; save start state in ebp
        

        
        loop_lfsr : ; john metcalf tripets xorshift
        ; taps: 16 15 13 4; feedback polynomial: x^16 + x^15 + x^13 + x^4 + 1
        ; bit (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12)) & 1
        ; lfsr (lfsr >> 1) | (bit << 15)
        ; period period + 1
        
        
        ; start_state is in ebp
        ; lfsr is in aex
        ; bit is in edx
        ; period is in ecx

        
        shl eax, 13 ;(lfsr << 7)
        xor edx, eax ; (lfsr ) ^ (lfsr << 7)
        mov eax, edx ; 

        shr eax, 17 ;(lfsr >> 9)
        xor edx, eax ; (lfsr ) ^ (lfsr << 7) ^ (lfsr >> 9)
        mov eax, edx ; 

        shl eax, 5 ;(lfsr << 13)
        xor edx, eax ; (lfsr ) ^ (lfsr << 7) ^ (lfsr >> 9) ^ (lfsr >> 13)
        mov eax, edx ; lfsr = start_state


        cmp eax, ebp ; if start_state == lfsr 
        je print_int ; exit loop_lfsr and print period

        jmp loop_lfsr

    ; http://pacman128.github.io/static/pcasm-book.pdf
    print_int: 
        push eax              ; push period onto the stack
        push int_format       ; push format string onto the stack
        call printf           ; call printf
        add esp, 8            ; clean up the stack (2 * 4 bytes)

    exit:
        mov eax, 1 ; System call number for exit
        mov ebx, 0 ; Exit status (0 for success)
        int 0x80   ; Interrupt for system call
