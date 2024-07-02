nasm -g -f elf32 r.asm # compile for 32 bits arch
gcc -g -m32 -o r r.o -nostdlib # link executable




nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
gcc -g -m32 -o fib_lfsr fib_lfsr.o -nostdlib # link executable
gdb ./fib_lfsr



nasm -g -f elf32 hello.asm # compile for 32 bits arch
gcc -g -m32 -o hello hello.o -nostdlib # link executable
# -no-pie  for no position independent


gdb ./hello # note: lines below should be individually entered, don't input the '#' lines

# enables user interface for gdb
tui enable

# display all code and current line 
layout asm 

# display all registers 
layout regs

# break at _start 
b _start 

# run program
r 

# next line
n 

# print message: get message address then print it as string
p &message
x/i <0x804a000>
# print message_len: 

# print all eax registers
info registers eax





















# or, for 64 architecture NOT WORKING
nasm -g -f elf64 hello.asm
gcc -g -o hello hello.o -nostdlib
#