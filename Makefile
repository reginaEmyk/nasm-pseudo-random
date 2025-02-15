lfsr: main.o fib_lfsr.o
	gcc -g -m32 main.o fib_lfsr.o -o lfsr

main.o: main.c
	gcc -g -m32 -c main.c

fib_lfsr.o: fib_lfsr.asm
	nasm -g -f elf32 fib_lfsr.asm

run: lfsr
	./lfsr

clean:
	rm -f lfsr main.o fib_lfsr.o	
