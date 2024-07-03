; xorshift64s, variant A_1(12,25,27) with multiplier M_32

section .bss ; Uninitialized data segment
    x resb 8 ; Allocate 8 bytes for the static variable 'x'

section .text ; Code segment
global xorshift64star

xorshift64star:
    ; Get the address of the static variable 'x'
    mov eax, offset x

    ; Load the initial value (1) or the current state of 'x'
    mov edx, [eax]

    ; x ^= x >> 12
    shr edx, 12
    xor edx, [eax]

    ; x ^= x << 25
    shl edx, 25
    xor edx, [eax]

    ; x ^= x >> 27
    shr edx, 27
    xor edx, [eax]

    ; x = x * 0x2545F4914F6CDD1DULL (multiply by the constant)
    ; NASM doesn't directly support 64-bit multiplication
    ; We can simulate it using a combination of shifts and additions

    ; edx:edx contains the lower 32 bits of 'x' after the bitwise operations
    mov eax, edx  ; Move edx to eax for multiplication

    ; Multiply by the lower part of the constant (0x2545F491)
    mov ecx, 0x2545F491
    mul ecx

    ; Add the upper part of the constant shifted left by 32 bits (0x4F6CDD1D * 2^32)
    mov ecx, 0x4F6CDD1D
    shl ecx, 32
    add eax, ecx

    ; Store the lower 32 bits of the result back into 'x'
    mov [eax], edx

    ret ; Return from the function
