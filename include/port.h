#ifndef _H_PORT
#define _H_PORT

static inline void _outb(u16 port, u8 data)
{
		asm volatile ("outb %0, %1"
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
static inline void outb(u32 port, u8 data)
{
		_outb(port, data);
		io_delay();
}

static inline void outw(u32 port, u16 data)
{
		asm volatile ("outw %0, %1"
		              : : "a" (data), "d"(port):);
		io_delay();
}

static inline void outl(u16 port, u32 data)
{
		asm volatile ("outl %0,%1" : : "a" (data), "d"(port):);
		io_delay();
}

static inline u32 inl(u16 port) 
{
	u32 data;
	asm volatile ("inl %1, %0" : "=a"(data) :"dN"(port));
	return data;
}

static inline u16 inw(u16 port) 
{
	u16 data;
	asm volatile ("inw %1, %0"
		      : "=a"(data) :"d"(port));
	return data;
}
static inline u8 inb(u16 port) 
{
	u8 data;

	asm volatile ("inb %1, %0"
		      : "=a"(data) :"d"(port));
	return data;
}

#endif
