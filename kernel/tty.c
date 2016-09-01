#include"tty.h"
#define COM1_BASE	0x3F8
#define COM2_BASE	0x3F8
#define COM3_BASE	0x3F8
#define COM4_BASE	0x3F8

u32 base_port;
static void init_serial()
{ 
	/* we only COM1 here */
	base_port = COM1_BASE;
	outb(base_port + 1, 0x00);    // Disable all interrupts
	outb(base_port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
	outb(base_port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
	outb(base_port + 1, 0x00);    //                  (hi byte)
	outb(base_port + 3, 0x03);    // 8 bits, no parity, one stop bit
	outb(base_port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
	outb(base_port + 4, 0x0B);    // IRQs enabled, RTS/DSR set

}

u8 serial_received()
{
	return inb(base_port + 5) & 1;
}
 
char read_serial()
{
	while (serial_received() == 0);
	return inb(base_port);
}

int is_transmit_empty() 
{
	return inb(base_port + 5) & 0x20;
}
 
void write_serial(char a) 
{
	while (is_transmit_empty() == 0);

	outb(base_port,a);
}

void init_tty()
{
	init_serial();
}

