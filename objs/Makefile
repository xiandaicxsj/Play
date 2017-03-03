SRCS := $(wildcard *.o)

.PHONY: all clean
all: $(SRCS)
	@echo ld $(SRCS) 
	@echo $(KERNEL_BIN)
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $^
clean: 
	rm *.o
	rm $(KERNEL_BIN)

