#include"console.h"
#include"tty.h"
struct console 
{
};

struct console_dev kb;

struct file_operation key_board_operation
{
	.read = kb_read;
}

void kb_read(struct file_struct *f, void *buffer, u32 size)
{
	struct kb_device *dev = (struct kb_device *)f->data;
	/* if dev is not ready, then wait */
}

void init_key_board()
{
	create_inode("/dev/kb", &kb_operation, &kb);
}

