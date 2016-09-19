#include"console.h"
#include"char_dev.h"
/* dev console */
struct console 
{
	struct char_dev dev;

};

struct console_dev con_dev;

struct file_operation con_dev_operation
{
	.read = con_dev_read,
	.write = con_dev_write,
	.open = con_dev_open,
}

void con_dev_read(struct file_struct *f, void *buffer, u32 size)
{
	struct kb_device *dev = (struct kb_device *)f->data;
	/* if dev is not ready, then wait */
}

struct char_dev_ops con_dev_ops =
{

};

static void init_console_dev(struct console_dev *dev)
{
 /* init console_dev */
}


void init_console()
{
	u32 major = DEV_CHAR; 
	u32 minor = alloc_minor(major);

	dev_t dev_num = DEV_NUM(major, minor);

	/* init console specific */
	init_console_dev(&con_dev.dev);

	register_char_dev(dev_num, &con_dev.dev, &con_dev_ops);
	/* create file relate ops */
	create_inode("/dev/console", &con_dev_operation, &kb);
}

