; Fibonacci LFSR from https://en.wikipedia.org/wiki/Linear-feedback_shift_register
%include "asm_io.inc"

section .data
    int_format	    db  "%i", 10; Null terminator and padding byte
    

section .text
; Program entry point
global main
extern printf
; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time

global main
main: ; _start is in stdlib with printf 
    ; get seed from current time
    
    
    ; start_state is in ebx
    ; lfsr is in aex
    ; bit is in edx
    ; period is in ecx
; loads random seed to eax and edx 32 32
   ; RDTSC ; start state is a pseudo random seed from clock time in eax. a val from 0 to   
        ; Test if EAX is negative
    ; start state is the last bit of seed, not 1 as in start_state = 1 << 15 | 1
    ; mov eax, 1 << 15 | 1

    ; loop_RDTSC:
    ;     RDTSC
    ;     cmp eax, 1    ; Compare eax with 1
    ;     jl loop_RDTSC ; Jump to loop_RDTSC if EAX is less than 1 
    
    mov eax, 1 << 15 | 1
    ; shr eax, 16
    ; and eax, 0xFFFF ; 16bit only
    ; or eax, 1 ; RDTSC << 16 | 1

   
        ; Code to handle the case where EAX is negative
;   shr eax, 16  ; Shift right by 16 (might not be portable across architectures)
; and eax, 0xFFFF  ; 
    ; lfsr = start_state
    mov edx, 0 ; helper
    mov ecx, 0 ; period
    

    mov ebx, eax ; for shift ops
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


    
    shr eax, 7 ;(lfsr >> 7)
    xor edx, eax ; (lfsr ) ^ (lfsr >> 7)
    shl eax, 9 ;(lfsr << 9)
    xor edx, eax ; (lfsr ) ^ (lfsr >> 7) ^ (lfsr << 9)
    shr eax, 13 ;(lfsr >> 13)
    xor edx, eax ; (lfsr ) ^ (lfsr >> 7) ^ (lfsr << 9) ^ (lfsr >> 13)
    mov eax, edx ; lfsr = start_state

    inc ecx ;period += 1

    cmp ebp, eax ; if start_state == lfsr 
    je print_int ; then exit loop_lfsr and print period

    jmp loop_lfsr

; http://pacman128.github.io/static/pcasm-book.pdf
print_int: 
; mov eax, ecx            ; move period into eax
push ecx              ; push period onto the stack
push int_format       ; push format string onto the stack
call printf           ; call printf
add esp, 8            ; clean up the stack (2 * 4 bytes)

exit:
; call print_inte
; Exit program (syscall for Linux/OSX)
mov eax, 1 ; System call number for exit
mov ebx, 0 ; Exit status (0 for success)
int 0x80   ; Interrupt for system call
