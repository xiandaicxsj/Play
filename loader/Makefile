BOOT:=boot.asm
LDR:=loader.asm
BOOT_BIN:=$(subst .asm,.bin,$(BOOT))
LDR_BIN:=$(subst .asm,.bin,$(LDR))
$(BOOT_BIN) : $(BOOT)
	        nasm $< -o $@

$(LDR_BIN) : $(LDR)
	        nasm $< -o $@

all: $(BOOT_BIN) $(LDR_BIN)
clean:
	rm *bin

	

