#include"pic.h"
#include"type.h"
inline void _outs_8(u8 port, u8 data)
{
		asm volatile ("outb %%al, %%dx"
		              : : "a" (data), "d"(port):);
}

inline void io_delay()
{
	asm volatile (
	"nop \n\t"
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

	asm volatile ("inb %%dx, %%al"
		      : "=a"(data) :"d"(port));
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
	return ;
	/*
	asm volatile(
  "movb $0x11, %%al \n\t"
	"outb %%al, $0x20 \n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"outb %%al, $0xa0\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"movb $0x20, %%al\n\t"
	"outb %%al, $0x21\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"movb $0x28, %%al \n\t"
	"outb %%al, $0xa1 \n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"movb $0x4, %%al \n\t"
	"outb %%al, $0x21 \n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"movb $0x2, %%al \n\t"
	"outb %%al, $0xa1 \n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"movb $0x1, %%al\n\t"
	"outb %%al, $0x21\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"outb %%al, $0xa1\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"movb $0xfe, %%al\n\t"
	"outb  %%al, $0x21 \n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"movb $0xff, %%al\n\t"
	"outb  %%al, $0xa1 \n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t");
	*/
	outs_8(PIC_MASTER, 0x11);
	outs_8(PIC_SLAVE, 0x11);

	asm volatile ("outb %al, $0x21\n\t"
								"nop \n\t"
								"nop \n\t"
								"nop \n\t");
	while(1);
	outs_8(PIC_MASTER + 1, 0x20);
	outs_8(PIC_SLAVE + 1, 0x28);
	outs_8(PIC_MASTER + 1, 0x4);
	outs_8(PIC_SLAVE + 1, 0x2);

	outs_8(PIC_MASTER + 1, 0x1);
	outs_8(PIC_SLAVE + 1, 0x1);

	outs_8(PIC_SLAVE + 1, 0xfe);
	outs_8(PIC_MASTER + 1, 0xff);
	while(1);
}
/*
void init_pic()
{
	_init_pic(PIC_MASTER, MS_INTER_BASE, 1);
	_init_pic(PIC_SLAVE, SL_INTER_BASE, 0);
	_mask_interrupt(PIC_MASTER, 0xfe);
}
*/
