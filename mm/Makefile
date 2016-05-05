CC = gcc
vpath %.h ../include

SRCS := $(*.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
DEPS := $(patsubst %.c, %.d, $(SRCS))

$(DEPS): %.d: %.c
	$(CC) -MM $(CCFLAGS) $< > $@
	$(CC) -MM $(CCFLAGS) $< | sed s///.o/.d/ >> $@
	

$(OBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@
# &< prerequisite
# $@ name of target


all:  $(OBJS) $(DEPS)
depen: $(DEPS)
.PHONY: clean
clean:
	rm *.o
	

