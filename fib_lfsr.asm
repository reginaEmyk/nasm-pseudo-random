; Fibonacci LFSR from https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Example_in_Python
section .data
    int_format	    db  "%i", 10; Null terminator and padding byte
    starting_bit 23; 24th bit
    BIT_MASK_24 0x800000; 2^24 in hex
    BIT_MASK_23 0x400000; 2^23 in hex
    BIT_MASK_21 0x100000; 2^21 in hex
    BIT_MASK_20 0x50000; 2^20 in hex 


section .text
    global lfsr
    extern printf

global lfsr
lfsr: ; _start is in stdlib with printf 
    ; get seed from current time
    
    ; start_state is in ebx
    ; lfsr is in aex
    ; bit is in edx
    ; period is in ecx


     %assign STARTING_BIT 23; 24th bit
; loads non-0 16bit seed to eax based on clock time
; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time
    loop_seed:
        RDTSC 
        shr eax,8 ; 24 bits
        cmp eax,0 ; seed 0 garantees 0 output so we'll avoid it
        je loop_seed  
    mov ebp, eax
    push ebp ; stack: start_state
    mov ebx, [ebp] ; start_state is constant in ebx

    ;  get TAP with bits  24, 23, 21, 20)
get_polynomial: ;for 24,23,21,20th bits only
    ; lfsr must be in eax
    push eax
    mov edx, eax
    and edx, BIT_MASK_24
    shr 23;
    and eax, BIT_MASK_23
    shr 22;
    xor eax, edx ; 24th xor 23rd
    
    mov edx, esp
    and edx, BIT_MASK_21
    shr 20;
    xor eax, edx
    mov edx, esp
    and edx, BIT_MASK_20
    shr 19
    xor eax, edx
    ret ; polynomial is in edx



get_bit:
       ; stack must have: 1st, the amount of args, 2nd: each arg
       ; ecx must hav amount of args
       ; eax must hav a bit to xor to
       ; stack must hav each arg, from top to bottom
       ; esp must have an int to shift left eax to, in order to compare it to edx
       ; ebs must have seed
       mov eax, [ebx]
       pop ecx ; amount of args
       loop_xor:
        
        dec ecx
        cmp ecx,0
        jg loop_xor
       
get_bit_and_xor:
    mov eax, ebp
    shr eax, esp
    pop
    xor eax, edx
    mov edx, eax

; todo ensure the hex to bit position is correct
    mov edx, 0  
    push 24 - 1 ; 2^24 in hex
    call get_bit_and_xor
    push 23 - 1 ; 2^23 in hex
    call get_bit_and_xor
    push 21 - 1 ; 2^21 in hex
    call get_bit_and_xor
    push 20 - 1 ; 2^20 in hex
    call get_bit_and_xor
    shl edx, 23; to push polynomial to lfsr 1st bit
; TAP is in eax

; got polynomial above

; push polynomial to lfsr
    shr [ebp], 1
    xor ebp, edx ; shifted and added TAP

    ret ebp


; lfsr has been shifted in eax by TAP bit 24, 23, 21, 20)



    ; lfsr = start_state
    mov edx, 0 ; bit
    mov ecx, 0 ; period
    push eax ; stack: start_state 
    mov edx, eax
    mov ebp, eax ; save start state in ebp
    
    loop_lfsr :
    ; taps: 16 15 13 4; feedback polynomial: x^16 + x^15 + x^13 + x^4 + 1
    ; bit (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12)) & 1
    ; lfsr (lfsr >> 1) | (bit << 15)
    ; period period + 1
    
    
    ; start_state is in ebp 
    ; lfsr is in aex
    ; bit is in edx
    ; period is in ecx

    push eax ; stack: lfsr, start_state
    mov ebx, eax 
    mov edx, eax

    shr eax, 1 ;(lfsr >> 1)
    xor edx, eax ;lfsr ^ (lfsr >> 1) 

    shr eax,2 ;(lfsr >> 3)
    xor edx, eax ; lfsr ^ (lfsr >> 1) ^ (lfsr >> 3)

    shr eax,9 ;(lfsr >> 12)
    xor edx, eax ; lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12))
    
    and edx, 1; (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12)) & 1
; bit in edx is bit = (lfsr ^ (lfsr >> 1) ^ (lfsr >> 3) ^ (lfsr >> 12)) & 1


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

