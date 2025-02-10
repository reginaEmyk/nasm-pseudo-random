![alt text](image.png)
https://datacipy.cz/lfsr_table.pdf
https://www.physics.otago.ac.nz/reports/electronics/ETR2012-1.pdf
24:  24, 23, 21, 20. ∗Department of Physics, University of Otago, Box 56, Dunedin, New Zealand.


https://www.partow.net/programming/polynomials/index.html#deg24

http://poincare.matf.bg.ac.rs/~ezivkovm/publications/primpol1.pdf 3 6 7 16 23

http://users.ece.cmu.edu/~koopman/lfsr/


nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
gcc -g -m32 lfsr.c fib_lfsr.o -o lfsr
gdb ./lfsr 




nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
gcc -g -m32 chi.c fib_lfsr.o -o chi
gdb ./chi

p * nasmPseudoRandom@10 
print *(int *)($ebp+4)

x/10dw 0x56559008

esp - 44?







gcc -g -m32 c_fib_lfsr.c fib_lfsr.o -o c_fib_lfsr
gdb ./c_fib_lfsr


nasm -g -f elf32 fib_lfsr.asm 
 gcc -o fib_lfsr fib_lfsr.o chi.c asm_io.o

6029786 20001
ignore 1 1677721699999

01100100 00010111 01101010
01011100 00000001 11011010

00101110 00000000 11101101
01000100 00010001 00011110

1448436125
894746
16157861

13437803

![alt text](image-2.png)
2 5 9 24 

545259520

111000010000000000000000 17 24-22

606851 // oversteps 24 bits at 100000

why? how? Cycle Size 2n −1

note: reference is 1 indexed not 0

2147483611


nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
gcc -g -m32 lfsr.c fib_lfsr.o -o lfsr
gdb ./lfsr 

ca7c

nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
gcc -g -m32 chi.c fib_lfsr.o -o chi
gdb ./chi 



nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
gcc -g -m32 main.c fib_lfsr.o -o lfsr
./lfsr 




0xffffca7c

print *(int *)($ebp+4)
esp - 44?


bit is a polynomial of xors
whole number is right shifted and bit is added to front



to call asm from C: just link asm to C and use sm foo within C, no need for includes
(make asm executable by itself?)
gcc main.c asm.o -z noexecstack 

![alt text](image-1.png)


send POSITION, get bit in stack. d



24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01
 x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  x  
             1                                                           

 24 xor 23 xor 21 xor 20 xor 1 == 01






./chi &&  ./chi && ./chi &&  ./chi && ./chi &&  ./chi && ./chi &&  ./chi 