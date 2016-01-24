#!/bin/sh
gcc -I include/ -m32 -o head_32.o -c head_32.S
gcc -I include/ -m32 -o kernel.o -c kernel.c
ld -m elf_i386 -o k kernel.o head_32.o -T kernel.lds

