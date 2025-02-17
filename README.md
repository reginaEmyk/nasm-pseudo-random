# Rodar o programa (resposta da questão)
``` 
nasm -g -f elf32 lfsr.asm # compile for 32 bits arch
gcc -g -m32 main.c lfsr.o -o lfsr
./lfsr 
```
### OU 
(mesmo comando acima)
```
make && ./lfsr
```
Para escolher o número de números a serem gerandos é preciso alterar o #define ARRAY_SIZE para o valor desejado. Até 16777216 é suportado

## Obeservações Windows
Para compilar em windows é necessário o seu gcc ser capaz de compilar códigos de 32 bits, para isso recomenda-se usar o gcc instaldado dessa maneira:
```powershell
winget install MSYS2.MSYS2 # ou instala normal mesmo
# adiciona c:/msys64/mingw32/bin e c:/msys64/usr/bin para o PATH
pacman -Suy
pacman -S mingw-w64-i686-gcc
```
