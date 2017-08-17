#include"console.h"
#include"char_device.h"
#include"fs.h"
#include"page.h"
#include"inode.h"
#define CONSOLE_WITH 80
#define CONSOLE_LEN 20
/* dev console */
struct console_device
{
	struct char_device dev;

	u32 c_start; /* start of video memory */
	u32 c_limit; /* limit video memory*/
	u32 c_colum; /* current colum */
	u32 c_row; /* current row */
	u32 c_cur; /* current video memory */
	u32 c_base;
};

struct console_device con_dev;

int con_dev_open(struct m_inode *inode, u32 attr)
{

}

int con_dev_read(struct file_struct *f, void *buffer, u32 size)
{
	return 0;
}

static void scroll(struct console_device *dev)
{
}

static void con_dev_new_line(struct console_device *dev)
{
	dev->c_colum = 0;
	dev->c_row ++;
	if (dev->c_row == CONSOLE_LEN) 
		scroll(dev);
	dev->c_row --;
	/* we need to update c_cur */
}

static void con_dev_out_char(struct console_device *dev, char a)
{
	u32 off = dev->c_base + dev->c_cur;

	asm volatile ( "movb 0xf0, %%ah \n\t"
                       "movw %%ax, (%%edi)"
                       :: "a"(a), "D"(off):);

	dev->c_cur ++ ;
}

int con_dev_write(struct file_struct *f, void *buffer, u32 size)
{
	int i = 0;
	struct console_device *con_dev = f->data;

	while(i < size) {
		/*
		switch (buffer[i]) {
		case '':
			con_dev_new_line(con_dev);
			break;
		case '':
			break;
		defaut:
			con_dev_out_char(buffer[i]);	
			break;
		}
		*/
		i++;
	}
	/* no cache */
	/* write buffer to console */
	/* here we juest write to */
}

int con_dev_flush(struct file_struct *f)
{

}

struct file_operations con_dev_ops = 
{
	.read = con_dev_read,
	.write = con_dev_write,
	.open = con_dev_open,
	.flush = con_dev_flush,
};

static void init_console_device(struct console_device *dev)
{
	dev->c_base = phy_to_virt(0xb8000);
	dev->c_start = 0;
	dev->c_limit = 2000;/* size */
	dev->c_colum = 0;
	dev->c_row = 0;
	dev->c_cur = 0;
 /* init console_dev */
}


void init_console()
{
	u8 major;
	u8 minor;
	dev_t dev_num;
	char dev_name[20];

	major = DEV_CHAR;
	minor = alloc_minor(major);
	dev_num = DEV_NUM(major, minor);

	/* FIXME */
	/*
	sprintf(dev_name, "/dev/console%d", minor);
	*/
	/* init console specific */
	init_console_device(&con_dev);

	//register_char_device(dev_num, &con_dev.dev, &con_dev_ops);
	/* create file relate ops */
	create_inode_dev(dev_name, &con_dev_ops, &con_dev);
}

