#include"pic.h"
#include"type.h"
#include"port.h"

static void _init_pic(u8 is_master, u8 irq_vec_off)
{
	u8 cmd_port = is_master ? PIC_MASTER_CMD : PIC_SLAVER_CMD;
	u8 data_port = is_master ? PIC_MASTER_DATA : PIC_SLAVER_DATA;

	outb(cmd_port, 0x11); /* icw1 */
	outb(data_port, irq_vec_off);  /* icw2 */

	if (is_master)
		outb(data_port, 0x40); /*icw3 */
	else 
		outb(data_port, 0x20);
	outb(data_port, 0x01); /* icw4 */
}

static void pic_set_irqmask(u8 is_master, u8 mask)
{
	u8 data_port;

       	data_port = is_master ? PIC_MASTER_DATA : PIC_SLAVER_DATA;
	outb(data_port, mask);
}

void pic_mask_irq(u32 vector)
{
	u8 is_master;
	u8 mask;
	u8 data_port;
	
	is_master = vector < SL_IRQ_BASE ? PIC_MASTER : PIC_SLAVER;
       	data_port = is_master ? PIC_MASTER_DATA : PIC_SLAVER_DATA;
	mask = inb(data_port);
	
	mask |= 1 << (is_master ? vector - MS_IRQ_BASE : vector - SL_IRQ_BASE);
	pic_set_irqmask(is_master, mask);
}

void send_eoi(u32 vector)
{
	u8 cmd_port;

	cmd_port = vector < SL_IRQ_BASE ? PIC_MASTER_DATA : PIC_SLAVER_DATA;
	outb(cmd_port, PIC_EOI);
}

void init_pic()
{
	_init_pic(PIC_MASTER, MS_IRQ_BASE);
	_init_pic(PIC_SLAVER, SL_IRQ_BASE);
	pic_set_irqmask(PIC_MASTER, 0x0);
	pic_set_irqmask(PIC_SLAVER, 0x0);
}
