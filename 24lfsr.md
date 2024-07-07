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

6029786 20001
65535
131070
000110011 00101111 0011111

2147483574

5457894

ignore 2 16777216


1448436125
894746
16157861

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
