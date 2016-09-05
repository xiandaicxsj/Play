#ifndef _H_FS
#define _H_FS
#define ROOT_DEV 1
#define O_RD (1 << 0)
#define O_RDWR (1 << 1)
#define O_CREATE (1 << 2)
#define O_ERROR ( ~(O_RD | O_RDWR | O_CREATE) )
#define ERROR_FILE(file_mod) (file_mod & O_ERROR)

struct file_operations
{
	int (*open)(char *file_name, u32 attr);
	int (*close)(int fd);
	int (*seek)(int fd, u32 off, u32 beg);
	int (*read)(int fd, void *buffer, u32 size);
	int (*write)(int fd, void *buffer, u32 size);
	int (*flush)(int fd);
};

void init_fs();
struct file_struct
{
	u32 pos;
	u32 fd;
	u32 file_attr; /* open attr of file */
	/* file is file/device */
	u32 type;
	u32 count;
	void *data;
	struct m_inode *inode;
	struct file_operations *ops;
};
#endif
