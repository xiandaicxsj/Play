SRCS := $(*.c)
OBJS := $(patsubst %.c, %.d, $(SRCS))
DEPS := $(patsubst %.c,%.d,$(SRCS))

%.d: %.c
	$(CC) -MM $(CCFLAGS) $< > $@
	$(CC) -MM $(CCFLAGS) $< | sed s///.o/.d/ >> $@
	

$(objects): %.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all:  $(OBJS) $(DEPS)
depen: $(DEPS)

