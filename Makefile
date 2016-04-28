##################################################
# Makefile
##################################################
ASM=gcc
CC=gcc
CCFLAGS= -I include/  -c -fno-builtin -m32   -fno-stack-protector  
LD=ld
LDFLAGS = -m elf_i386  -T kernel.lds
#LDFLAGS = -m elf_i386  -Ttext 0x30400
KERNEL_BIN:=$(subst .c,.bin,$(KERNEL))
IMG:=a.img
FLOPPY:=./mnt/
DIRS = ./mm

subdirs:
	@for dir in $(DIRS); do $(MAKE) -C $$dir; done

all:
$(KERNEL_BIN) : subdirs 

clean:
	@for dir in $(DIRS); do $(MAKE) -C $$dir clean; done

