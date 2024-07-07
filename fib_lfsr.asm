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
    ; get seed from current time
    
    ; start_state is in ebx
    ; lfsr is in aex
    ; bit is in edx
    ; period is in ecx


    ;  %assign 24_BITS 23; 24th bit
; loads non-0 16bit seed to eax based on clock time
; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time
    loop_seed:
        RDTSC 
        shr eax,8 ; 24 bits
        cmp eax,0 ; seed 0 garantees 0 output so we'll avoid it
        je loop_seed  
    ; mov eax, 0
    push eax 
    mov ebp, esp
    


call get_bit_and_xor
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


; oget_bit_and_xor:
;     mov edx, 0
;     mov eax, [ebp]
;     and eax, BIT_MASK_23
;     shl eax, 22
;     mov edx, eax ; gets 24th bit

;     mov eax, [ebp]
;     and eax, BIT_MASK_22
;     shl eax, 23 - 1
;     xor edx, eax ; 24th xor 23rd

;     mov eax, [ebp]
;     and eax, BIT_MASK_20
;     shl eax, 21 - 2
;     xor edx, eax ; 21 xor (24th xor 23rd)

;     mov eax, [ebp]
;     and eax, BIT_MASK_19
;     shl eax, 20 - 1
;     xor edx, eax; 20th xor (21 xor (24th xor 23rd))

;     push edx
;      ret ; polynomial is in edx and top of stack


    ;  get TAP with bits  24, 23, 21, 20)
; get_polynomial: ;for 24,23,21,20th bits only
;     ; lfsr must be in eax
;     push eax
;     mov edx, eax
;     and edx, BIT_MASK_24
;     shr 23;
;     and eax, BIT_MASK_23
;     shr 22;
;     xor eax, edx ; 24th xor 23rd
    
;     mov edx, esp
;     and edx, BIT_MASK_21
;     shr 20;
;     xor eax, edx
;     mov edx, esp
;     and edx, BIT_MASK_20
;     shr 19
;     xor eax, edx
;     ret ; polynomial is in edx







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

