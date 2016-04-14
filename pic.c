#include"pic.h"
#include"type.h"
inline void _outs_8(u8 port, u8 data)
{
		asm volatile ("outsb %%eax,%[port]"
									: : "a" (data), [port] "m"(port):);
}

inline void  io_delay()
{
		asm volatile ("nop \n\t"
									"nop \n\t"
									"nop \n\t"
									"nop \n\t");
}

/* out with time delay */
inline void outs_8(u8 port, u8 data)
{
		_outs_8(port, data);
		io_delay();
}

inline u8 ins_8(u8 port) 
{
		u8 data;
		asm volatile ("insb %[port], %[data]"
									: : [data] "m"(data), [port] "m"(port):);
		return data;
}

static void _init_pic(u8 port, u8 inter_offset, u32 is_master)
{
		outs_8(port, 0x11); /* icw1 */
		outs_8(port + 1, inter_offset);  /* icw2 */
		if (is_master)
			outs_8(port + 1, 0x40); /*icw3 */
		else 
			outs_8(port + 1, 0x20);
		outs_8(port + 1, 0x01); /* icw4 */
}

void _mask_interrupt(u8 port, u8 mask)
{
		outs_8(port + 1, mask); 	
}

void mask_interrupt(u32 index)
{
	  u8 mask = 0;
		u8 port = index < SL_INTER_BASE ? PIC_MASTER:PIC_SLAVE;
		u8 pic_off = index < SL_INTER_BASE ? index - MS_INTER_BASE :
																				 index - SL_INTER_BASE;

		mask = ins_8(port + 1);
		mask &= 0 << pic_off;
		_mask_interrupt(PIC_MASTER, mask);
}

void init_pic()
{
	_init_pic(PIC_MASTER, MS_INTER_BASE, 1);
	_init_pic(PIC_SLAVE, SL_INTER_BASE, 0);
}
