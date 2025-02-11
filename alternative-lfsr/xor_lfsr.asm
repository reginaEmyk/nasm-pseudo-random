; XOR LFSR from http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html

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
        

        
        loop_lfsr : ; john metcalf triplets xorshift, 
    ; http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html
                ; unsigned xorshift( )
        ; {
            ; x ^= x << 7;
            ; x ^= x >> 9;
            ; x ^= x << 8;
            ; return x;
        ; }
        
        shl eax, 7 ;;
        xor edx, eax ;
        mov eax, edx ;

        shr eax, 9 ;
        xor edx, eax ;
        mov eax, edx ;

        shl eax, 8 ;
        xor edx, eax ;
        mov eax, edx ;


    ; http://pacman128.github.io/static/pcasm-book.pdf
    print_int: 
        push eax              ; push number onto the stack
        push int_format       ; push format string onto the stack
        call printf           ; call printf to print number
        add esp, 8            ; clean up the stack (2 * 4 bytes)

    exit:
        mov eax, 1 ; System call number for exit
        mov ebx, 0 ; Exit status (0 for success)
        int 0x80   ; Interrupt for system call
