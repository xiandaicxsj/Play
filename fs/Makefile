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
test_fs: compile depend cp_test

depend: $(DEPS)
compile: $(OBJS)
cp:
	mv *.o  $(OBJS_DIR)/ 

cp_test:
	mv *.o $(OBJS_TEST)/
	

clean:
	@echo "clean"
	-rm *.o
	-rm *.d
	-rm *.d*

	

