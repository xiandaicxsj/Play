##################################################
# Makefile
##################################################
ASM=gcc
CC=gcc
LD=ld
LDFLAGS = -m elf_i386  -T kernel.lds
SRC_DIR = $(shell pwd)
ASMFLAGS = -I $(SRC_DIR)/include -m32
CCFLAGS= -I $(SRC_DIR)/include -c -fno-builtin -m32 -fno-stack-protector  
CCFLAGS_DEP= -I $(SRC_DIR)/include
#LDFLAGS = -m elf_i386  -Ttext 0x30400
KERNEL_BIN:=$(subst .c,.bin,$(KERNEL))

IMG:=a.img
FLOPPY:=./mnt/
DIRS = $(SRC_DIR)/mm $(SRC_DIR)/kernel $(SRC_DIR)/loader 
OBJS_DIR= $(SRC_DIR)/objs

subdirs:
	@for dir in $(DIRS); do \
	$(MAKE) -C $$dir all; \
	done
#all: $(KERNEL_BIN) create_img
#$(KERNEL_BIN) : subdirs $(OBJECTS)
#        $(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(OBJECTS)
#
all: subdirs $(KERNEL_BIN) 
$(KERNEL_BIN) : $(OBJS_DIR)/%.o
	@echo link
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $@




	

clean:
	@for dir in $(DIRS); do $(MAKE) -C $$dir clean; done

create_img:
	dd if=$(BOOT_BIN) of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp $(LDR_BIN) $(FLOPPY) -v
	sudo cp $(KERNEL_BIN) $(FLOPPY) -v
	sudo umount $(FLOPPY)

#export this two flags so it can be used in the subdir make
export CC
export CCFLAGS
export CCFLAGS_DEP
export OBJS_DIR
export ASMFLAGS
export ASM
