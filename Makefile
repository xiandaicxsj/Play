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
KERNEL_BIN:=kernel.bin
DIRS = $(SRC_DIR)/mm $(SRC_DIR)/kernel $(SRC_DIR)/loader $(SRC_DIR)/test
OBJS_DIR= $(SRC_DIR)/objs

IMG:=a.img
FLOPPY:=mnt/

subdirs:
	@for dir in $(DIRS); do \
	$(MAKE) -C $$dir all; \
	done
link_objs:
	@echo link objs
	$(MAKE) -C $(OBJS_DIR) all
#all: $(KERNEL_BIN) create_img
#$(KERNEL_BIN) : subdirs $(OBJECTS)
#        $(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(OBJECTS)
#

all: subdirs  link_objs create_img

clean:
	@for dir in $(DIRS); do $(MAKE) -C $$dir clean; done
	$(MAKE) -C $(OBJS_DIR) clean

create_img:
	dd if=$(SRC_DIR)/loader/boot.bin of=$(IMG) bs=512 count=1 conv=notrunc
	sudo mount -o loop $(IMG) $(FLOPPY)
	sudo cp $(SRC_DIR)/loader/loader.bin $(FLOPPY) -v
	sudo cp $(OBJS_DIR)/$(KERNEL_BIN) $(FLOPPY) -v
	sudo umount $(FLOPPY)

#export this two flags so it can be used in the subdir make
export CC
export CCFLAGS
export CCFLAGS_DEP
export OBJS_DIR
export ASMFLAGS
export ASM
export KERNEL_BIN
export LD
export LDFLAGS
