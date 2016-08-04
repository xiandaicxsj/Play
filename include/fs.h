#ifndef _H_FS
#define _H_FS
#define ROOT_DEV 1
#define O_RD (1 << 0)
#define O_RDWR (1 << 1)
#define O_CREATE (1 << 2)
#define O_ERROR ( ~(O_RD | O_RDWR | O_CREATE) )
#define ERROR_FILE(file_mod) (file_mod & O_ERROR)

void init_fs();
struct file_struct
{
	u32 pos;
	u32 fd;
	u32 file_attr; /* open attr of file */
	struct m_inode *inode;
};
#endif
