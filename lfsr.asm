; TAPS from  pág. 3: https://www.infineon.com/dgdl/Infineon-24_BIT_Pseudo_Random_Sequence_Generator_Datasheet_PRS24_V_3.4-Software%20Module%20Datasheets-v03_04-EN.pdf?fileId=8ac78c8c7d0d8da4017d0f985e3d0761
section .data
    %assign _LAST_BIT_ 0x000001; only the last 24 bits are 
    %assign _24_BITS 0xFFFFFF; only the last 24 bits are 1
    %assign _BIT_MASK_TAPS 0x00001B ; mask where only TAPS are one (24,22,21,20) 
    %assign array_size 16777216
    pseudo_randoms_ times array_size dd 0; 
    
section .text
    global _lfsr 
    global lfsr ; rename to start with underscore if not in linux, __lfsr
    global get_polynomial
    global loop_seed

   loop_seed:
            RDTSC 
            shr eax,8 ; 24 bits
            cmp eax,0 ; seed 0 garantees 0 output so we'll avoid it
            je loop_seed  
            ret

    ; get seed from current clock time
    ; https://www.diva-portal.org/smash/get/diva2:812512/FULLTEXT01.pdf    
    ; https://stackoverflow.com/questions/17182182/how-to-create-random-number-in-nasm-getting-system-time
    call_loop_seed:
        call loop_seed
        jmp gen_lfsr

    get_polynomial:
        mov edx, eax; eax has lfsr seed ; 
        and edx, _BIT_MASK_TAPS ; 
        shr edx, 1 ; 24th bit: original 23rd bit
        xor eax, edx ; 24th bit xor 23rd
        shr edx, 2  ; 24th bit: original 21st bit
        xor eax, edx; 24th bit xor 23rd xor 21st
        shr edx, 1 ; 24th bit: original 20th bit
        xor eax, edx ; 24th bit xor 23rd xor 21st xor 20th
        and eax, _LAST_BIT_; 24th bit xor 23rd xor 21st xor 20th xor 1, to avoid 0
        
        shl eax, 23
        ret
            
    _lfsr:
    lfsr: ; _start is in stdlib with printf 
        push ebp
        mov  ebp, esp
        mov eax, [esp+8] ; previous stack + 4 for eax
        mov ecx, 0 ; to access array
        
    cmp eax, 0
    je call_loop_seed ; puts clock time in eax and jumps to gen_lfsr

    gen_lfsr: 
        push eax ; preserve lfsr in stack

   ; shifts lfsr in edx so 23rd bit is the bit to perform xor on the 23rd bit of lfsr in eax
        call get_polynomial ; puts polynomial in eax, eax must have lfsr
        pop edx ; restore lfsr
        shr edx, 1 ; shift lfsr by 1
        or edx, eax ; pushes polynomial to leftmost of lfsr. Note: edx must have bits from 0-22 as 0. This is deont in get_polynomial
    ; lfsr = polynomial | lfsr << 1 , upper 8 bits are 0. polynomial must be 0 except in 24th bit
            mov [pseudo_randoms_ + ecx*4], edx ; push pseudo random number to array
        mov eax,edx; push lfsr in eax
        inc ecx ; increment counter
        cmp ecx, array_size ; loop until all 'array_size' amount of pseudo randoms are pushed to array
        jl gen_lfsr ; todo jl or jle

        ; AL, AX or DX:A
            lea eax, [pseudo_randoms_]
        ; mov [esp+8], eax
        mov esp, ebp
        pop ebp ; restore stack to return to main
        
        ret

; this was made for linux