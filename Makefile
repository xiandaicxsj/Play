##################################################
# Makefile
##################################################
ENTRYPOINT=0x30400
CC=gcc
ASM= gcc
ASMFLAGS= -I include/  -m32
CCFLAGS= -I include/  -c -fno-builtin -m32 -std=c99  -fno-stack-protector  
LD=ld
LDFLAFS = -T kernel.lds
BOOT:=boot.asm
LDR:=loader.asm
KERNEL:=kernel
BOOT_BIN:=$(subst .asm,.bin,$(BOOT))
LDR_BIN:=$(subst .asm,.bin,$(LDR))
KERNEL_BIN:=$(subst .s,.bin,$(KERNEL))
OBJECTS = kernel.o 
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

kernel.o: head_32.S
	$(ASM) $(ASMFLAGS) -o $@ -s $<
	
	
	
