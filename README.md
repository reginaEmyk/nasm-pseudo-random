# C/NASM Resposta da questao
``` 
nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
 gcc -g -m32 main.c fib_lfsr.o -o lfsr
 ./lfsr 
 ```


# misc
### nasm only
```
nasm -g -f elf32 xor_lfsr.asm # compile for 32 bits arch
gcc -static -g -m32  -o xor_lfsr xor_lfsr.o  # link executable
./xor_lfsr
```

para testes rápidos

```
nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
gcc -static -g -m32  -o fib_lfsr fib_lfsr.o  # link executable
./fib_lfsr
```
