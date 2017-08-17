#include"char_device.h"
#include"fs.h"
#include"keyboard.h"
struct kb_device 
{
	struct char_device dev;
};

struct kb_device kb;

int kb_open(struct m_inode *inode, u32 attr)
{
	return 0;
}	

int kb_write(struct file_struct *f, void *buffer, u32 size)
{
	struct kb_device *dev = (struct kb_device *)f->data;
}

int kb_read(struct file_struct *f, void *buffer, u32 size)
{
	struct kb_device *dev = (struct kb_device *)f->data;
	/* if dev is not ready, then wait */
}

struct file_operations kb_ops = 
{
	.open = kb_open,
	.read = kb_read,
	.write = kb_write,
};

void init_key_board()
{
	/* del with irq related */
	//register_irq_handler();
	create_inode_dev("/dev/kb", &kb_ops, &kb);
}

