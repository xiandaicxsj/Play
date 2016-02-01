##################################################
# Makefile
##################################################
ASM=gcc
CC=gcc
CCFLAGS= -I include/  -c -fno-builtin -m32 -std=c99  -fno-stack-protector  
LD=ld
LDFLAGS = -m elf_i386  -T kernel.lds
#LDFLAGS = -m elf_i386  -Ttext 0x30400
BOOT:=boot.asm
LDR:=loader.asm
KERNEL:=kernel.c
BOOT_BIN:=$(subst .asm,.bin,$(BOOT))
LDR_BIN:=$(subst .asm,.bin,$(LDR))
KERNEL_BIN:=$(subst .c,.bin,$(KERNEL))
OBJECTS = head_32.o kernel.o
IMG:=a.img
FLOPPY:=/mnt/floppy/

.PHONY : everything

everything : $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN)
	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp $(LDR_BIN) $(FLOPPY) -v
	sudo cp $(KERNEL_BIN) $(FLOPPY) -v
	sudo umount $(FLOPPY)

clean :
	rm -f $(BOOT_BIN) $(LDR_BIN) $(KERNEL_BIN) *.o

$(BOOT_BIN) : $(BOOT)
	nasm $< -o $@

$(LDR_BIN) : $(LDR)
	nasm $< -o $@

$(KERNEL_BIN) : $(OBJECTS)
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(OBJECTS)

kernel.o:kernel.c
	$(CC) $(CCFLAGS) -o $@ $<
head_32.o: head_32.S
	$(CC)  $(CCFLAGS) -o $@ $<
	
	
	
