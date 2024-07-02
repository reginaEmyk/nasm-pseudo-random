; Fibonacci LFSR from https://en.wikipedia.org/wiki/Linear-feedback_shift_register

section .data
    start_state dw 1 ; << 15 | 1 ; dw is 16 bits
    lfsr dw start_state
    period dw 0 ; the pseudo-random number

section .text

; Program entry point
global _start

_start:
    ; Write message to stdout
	mov eax,4            ; 'write' system call = 4
	mov ebx,1            ; file descriptor 1 = STDOUT
	mov ecx,start_state        ; string to write
	mov edx,22     ; length of string to write
	int 0x80             ; call the kernel

    ; Exit program (syscall for Linux/OSX)
    mov eax, 1 ; System call number for exit
    mov ebx, 0 ; Exit status (0 for success)
    int 0x80   ; Interrupt for system call

; End of program

