SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
DEPS := $(patsubst %.c, %.d, $(SRCS))
sinclude $(SRCS:.c=.d)

%.d: %.c
	@echo create $< dependency file
	@set -e; rm -f $@; \
	$(CC) -MM $(CCFLAGS_DEP) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	

%.o: %.S
	$(ASM) $(ASMFLAGS) -o $@ -c $<

%.o: %.c
	@echo $(OBJS)
	@echo compile $< $@
	$(CC) $(CCFLAGS) $< -o $@
# &< first prerequisite
#all: depend all_1
# now mak 
.PHONY: all compile depend
all: compile depend cp
test_fs: compile depend last 

depend: $(DEPS)
compile: $(OBJS)
cp:
	mv *.o  $(OBJS_DIR)/ 

last:
	@echo "a"
	

clean:
	@echo "clean"
	-rm *.o
	-rm *.d
	-rm *.d*

	

