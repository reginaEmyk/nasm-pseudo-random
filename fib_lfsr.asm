; Fibonacci LFSR from https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Example_in_Python
section .data
    int_format	    db  "%i", 10; Null terminator and padding byte
    starting_bit 24; 24th bit
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

; loads non-0 16bit seed to eax based on clock time
; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time
    loop_seed:
        RDTSC 
        shr eax,8 ; 24 bits
        cmp eax,0 ; seed 0 garantees 0 output so we'll avoid it
        je loop_seed  
        
    push eax ; stack: start_state
    mov ebx, eax ; start_state is constant in ebx

    ; push TAP bit 24, 23, 21, 20) to stack

get_polynomial:
    ; lfsr must be in eax
    

get_ith_bit:
; edx has a decimal, the i position for the ith bit 
; eax has a number
shr eax, edx;   ; shift eax right by i bits
push eax;

    get_bit_and_xor: 
        pop edx ; get TAP bit
        and eax, edx ; keeps 24th bit and zeroes all others
        xor eax,edx ;xor lfsr with bit
    ; todo test above

; push TAP bits to stack
    load_TAP_to_stack:
        pop edx
        ; eax must be correct bit!!!
        and eax, edx ; keeps ith bit and zeroes all others
        ; sould load 
        push eax
       
         


; todo ensure the hex to bit position is correct
    push 800000 ; 2^24 in hex
    call get_bit_and_xor
    push 400000 ; 2^23 in hex
    call get_bit_and_xor
    push 100000 ; 2^21 in hex
    call get_bit_and_xor
    push 50000 ; 2^20 in hex
    call get_bit_and_xor
; TAP is in eax


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

