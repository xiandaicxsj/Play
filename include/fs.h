#ifndef _H_FS
#define _H_FS
#define ROOT_DEV 1
#define O_RD (1 << 0)
#define O_RDWR (1 << 1)
#define O_CREATE (1 << 2)
void init_fs();
struct file
{
	u32 pos;
	u32 fd;
	u32 file_attr; /* open attr of file */
	struct inode *inode;
};
#endif
