#ifndef _H_PORT
#define _H_PORT

static inline void _outb(u8 port, u8 data)
{
		asm volatile ("outb %%al, %%dx"
		              : : "a" (data), "d"(port):);
}

static inline void io_delay()
{
	asm volatile (
	"nop \n\t"
	"nop \n\t"
	"nop \n\t"
	"nop \n\t");
}

/* out with time delay */
static inline void outb(u8 port, u8 data)
{
		_outb(port, data);
		io_delay();
}

static inline u8 inb(u8 port) 
{
	u8 data;

	asm volatile ("inb %%dx, %%al"
		      : "=a"(data) :"d"(port));
	return data;
}

#endif
