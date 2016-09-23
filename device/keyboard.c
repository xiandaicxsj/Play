#include"char_device.h"
#include"fs.h"
struct kb_device 
{
	struct char_device dev;
};

struct kb_device kb;

void kb_read(struct file_struct *f, void *buffer, u32 size)
{
	struct kb_device *dev = (struct kb_device *)f->data;
	/* if dev is not ready, then wait */
}

struct file_operations kb_ops = 
{
	.read = kb_read,
};

void init_key_board()
{
	/* del with irq related */
	//register_irq_handler();
	create_inode("/dev/kb", &kb_ops, &kb);
}

