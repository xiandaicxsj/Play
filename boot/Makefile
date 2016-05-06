SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
DEPS := $(patsubst %.c, %.d, $(SRCS))
sinclude $(SRCS:.c=.d)

#all: depend all_1
# now mak 
.PHONY: all compile depend
all: compile depend

depend: $(DEPS)
compile: $(OBJS)

clean:
	@echo "clean"
	rm *.d
	rm *.d*
	rm *.o

%.d: %.c
	@echo create $< dependency file
	@set -e; rm -f $@; \
	$(CC) -MM $(CCFLAGS_DEP) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	

$(OBJS):%.o: %.c
	@echo compile $< $@
	$(CC) $(CCFLAGS) $< -o $@
# &< first prerequisite
# $@ name of target

	

