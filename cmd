ld -m elf_i386 -o k kernel.o -T kernel.lds
gcc -I include/ -m32 -o kernel.o -c head_32.S

gcc -I include/ -m32 -o kernel.o -c head_32.S
ld -m elf_i386 -o k kernel.o -T kernel.lds

