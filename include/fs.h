#ifndef _H_FS
#define _H_FS
#include"inode.h"
#define ROOT_DEV 1
#define NONE_TYPE 0
#define FILE_TYPE (1 << 0)
#define DIR_TYPE (1 << 1)
#define DEV_TYPE (1 << 2)
#define O_RD (1 << 0)
#define O_RDWR (1 << 1)
#define O_CREATE (1 << 2)
#define O_DEL (1 << 3)
#define O_ERROR ( ~(O_RD | O_RDWR | O_CREATE) )
#define ERROR_FILE(file_mod) (file_mod & O_ERROR)

struct file_struct;
struct m_inode;
struct file_operations
{
	int (*open)(struct m_inode *inode, u32 attr);
	int (*close)(struct file_struct *file);
	int (*seek)(struct file_struct *file, u32 off, u32 set);
	int (*read)(struct file_struct *file, void *buffer, u32 size);
	int (*write)(struct file_struct *file, void *buffer, u32 size);
	int (*flush)(struct file_struct *file);
};

int init_fs();
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
