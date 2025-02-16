lfsr: main.o lfsr.o
	gcc -Wall -g -m32 main.o lfsr.o -o lfsr

main.o: main.c
	gcc -Wall -g -m32 -c main.c

lfsr.o: lfsr.asm
	nasm -g -f elf32 lfsr.asm

run: lfsr
	./lfsr

clean:
	rm -f lfsr main.o lfsr.o	
