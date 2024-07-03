grupo:
Regina Emy Da Nóbrega Kamada.
Os algoritmos geradores foram majoritariamente pegos de outras fontes, para garantir que gerariam pseudo-aleatórios, assim como a função de exit e print_int.
O que fiz foi principalmante passar de python ou C pra nasm. 
A única alteração foi usar uma seed RDTSC e rodar o loop do gen_fib_lfsr por ebp (lower 32 bits of seed) vezes.
As fontes foram comentadas.

os outros integrantes não participaram, 
Gustavo Valentim tentou ajudar um pouco pra printar um número com printf, mas não mandou código,
os outros disseram que trancaram


Programas:
xor_lfsr.asm: o mais simples, gera pseudo-aleatórios, seguindo http://www.retroprogramming.com/2017/07/xorshift-pseudorandom-numbers-in-z80.html

Nota: a fonte de referência é a mesma para ambos abaixo, porém não é especificamente para geradores, mas de um linear shift register tipicamente utilizado para tal.

gen_fib_lfsr.asm: mais complexo, acredito que gera pseudo-aleatórios, altera https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Example_in_Python. Alteração: roda loop de shift em <16 dos higher bits> da seed por <lower 32 bits da seed> vezes, printa os bits que passaram por shift. 

fib_lfsr.asm: segue a fonte de 'gen_fib_lfsr' mais à risca, mas não gera pseudo-aleatórios exatamente, printa a seed inicial porque o <period> (contador de loop) sempre dava o mesmo valor pra diferentes seeds. Fonte:  https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Example_in_Python.



comandos de compilação (testado em ubuntu, amd 64)

nasm -g -f elf32 xor_lfsr.asm # compile for 32 bits arch
gcc -static -g -m32  -o xor_lfsr xor_lfsr.o  # link executable
./xor_lfsr



nasm -g -f elf32 fib_lfsr.asm # compile for 32 bits arch
gcc -static -g -m32  -o fib_lfsr fib_lfsr.o  # link executable
./fib_lfsr





nasm -g -f elf32 gen_fib_lfsr.asm # compile for 32 bits arch
gcc -static -g -m32  -o gen_fib_lfsr gen_fib_lfsr.o  # link executable
