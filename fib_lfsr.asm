; Fibonacci LFSR from https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Example_in_Python
section .data
    int_format	    db  "%i", 10; Null terminator and padding byte
    %assign _STARTING_BIT 0x800000; only the last 24 bits are 
    %assign _24_BITS 0xFFFFFF; only the last 24 bits are 1
    %assign _BIT_MASK_TAPS 0xD80000 ; mask where only TAPS are one (24,23,21,20)

section .text
    global lfsr
    extern printf

global lfsr
lfsr: ; _start is in stdlib with printf 
    ; get seed from current clock time
    
; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time
    loop_seed:
        RDTSC 
        shr eax,8 ; 24 bits
        cmp eax,0 ; seed 0 garantees 0 output so we'll avoid it
        je loop_seed  

    push eax 
    mov ebp, esp ; starts stack: lfsr
    call get_bit_and_xor
    mov edx, [ebp] ; copy lfsr to edx to shift and add polynomial
    shr edx, 1 ; shift lfsr by 1
    or edx, eax ; pushes polynomial to leftmost of lfsr. Note: edx must have bits from 0-22 as 0. This is deont in get_bit_and_xor


mov eax, 13579135 ;1,0 

    push eax 
    mov ebp, esp
call get_bit_and_xor
mov eax, 24680246 ;1, 0

    push eax 
    mov ebp, esp
call get_bit_and_xor
mov eax, 31415926 ;0, 2^23

    push eax 
    mov ebp, esp
call get_bit_and_xor
mov eax, 27182818 ; 1

    push eax 
    mov ebp, esp
call get_bit_and_xor
mov eax, 11235813 ; 0

    push eax 
    mov ebp, esp
call get_bit_and_xor
mov eax, 16180339 ;1

    push eax 
    mov ebp, esp
call get_bit_and_xor
mov eax, 14142135

    push eax 
    mov ebp, esp
call get_bit_and_xor
mov eax, 17320508

    push eax 
    mov ebp, esp
call get_bit_and_xor

    call get_bit_and_xor
    mov ebp, eax
    push ebp ; stack: start_state
    mov ebx, [ebp] ; start_state is constant in ebx



get_bit_and_xor:
; shifts lfsr in edx so 23rd bit is the bit to perform xor on the 23rd bit of lfsr in eax
    mov edx, eax; eax has lfsr ; todo get from ebp
    shl edx, 1 ; 23ith bit: original 22nd bit
    xor eax, edx ; 23ith bit xor 22nd
    shl edx, 2  ; 23ith bit: original 20th bit
    xor eax, edx; 23ith bit xor 22nd xor 20th
    shl edx, 1 ; 23ith bit: original 19th bit
    xor eax, edx ; 23ith bit xor 22nd xor 20th xor 19th
    xor eax, _STARTING_BIT; 23ith bit xor 22nd xor 20th xor 19th xor 1
    and eax, _STARTING_BIT ; only the 23ith bit can be 1, to push into lfsr later
    retn  ; 23ith bit in eax is polynomial for TAP 23,22, 20,19 (0 index)
; lfsr has been shifted in eax by polynomial with TAP bit 24, 23, 21, 20)


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

