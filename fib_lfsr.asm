; Fibonacci LFSR from https://en.wikipedia.org/wiki/Linear-feedback_shift_register
%include "asm_io.inc"

section .data
    int_format	    db  "%i", 10; Null terminator and padding byte
    

section .text
    global main
    extern printf

global main
main: ; _start is in stdlib with printf 
    ; get seed from current time
    
    
    ; start_state is in ebx
    ; lfsr is in aex
    ; bit is in edx
    ; period is in ecx

; loads non-0 16bit seed to eax based on clock time
; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time
    loop_seed:
        RDTSC 
        shr eax,16 ;takes too long otherwise
        cmp eax,0
        je loop_seed  
        
    
    mov ebx, eax ; start_state is constant in ebx

    ; lfsr = start_state
    mov edx, 0 ; bit
    mov ecx, 0 ; period
    push eax ; start state on stack
    mov ebp, esp  
    mov edx, eax
    mov ebp, eax ; save start state in ebp
    
    loop_lfsr :
    ; taps: 16 15 13 4; feedback polynomial: x^16 + x^15 + x^13 + x^4 + 1
    ; bit (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12)) & 1
    ; lfsr (lfsr >> 1) | (bit << 15)
    ; period period + 1
    
    
    ; start_state is in ebx
    ; lfsr is in aex
    ; bit is in edx
    ; period is in ecx

    push eax
    mov ebx, eax 
    mov edx, eax

    shr eax, 1 ;(lfsr >> 1)
    xor edx, eax ;lfsr ^ (lfsr >> 1) 

    shr eax,2 ;(lfsr >> 3)
    xor edx, eax ; lfsr ^ (lfsr >> 1) ^ (lfsr >> 3)

    shr eax,9 ;(lfsr >> 12)
    xor edx, eax ; lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12))
    
    and edx, 1; (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12)) & 1
; bit in edx is (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12)) & 1


    pop  eax; eax = lfsr, stack: start_state
    push edx; stack: bit, start_state

    shr eax, 1 ; (lfsr >> 1)
    shl edx, 15; (bit << 15)
    or eax, edx; (lfsr >> 1) | (bit << 15)
;lfsr in eax is lfsr = (lfsr >> 1) | (bit << 15)

    pop edx ; bit in edx, stack: start_state

    inc ecx ;period += 1

    cmp ebp, eax ; if start_state == lfsr 
    je print_int ; prints pseudo random then program is interrupted

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

