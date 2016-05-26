#include"inode.h"
#include"fs.h"
void init_fs()
{
	init_buffer();
	init_super_block();
}


u32 _sys_open(char *file_path, u32 file_attr)
{
	struct inode *inode;
	struct file *f;
	u32 fd = -1;
	u32 is_alloc;
	
	is_alloc = file_attr & O_CREATE ? 1 : 0;

	inode = get_inode(file_path, is_alloc);

	fd = alloc_file_struct(current);
	if (fd < 0)
		return ;

	f = &current->fs[fd];

	f->inode = inode;
	f->attr = file_attr;
	f->pos = 0;
	return fd;
}

u32 _sys_read(int fd, void *buffer, u32 size)
{
	struct file *f = current->file[fd];
	u32 r;
	r = inode_read_data(f->inode, buffer, size);
	return r;
}

u32 _sys_write(int fd, void *buffer, u32 size)
{
	struct file *f = current->file[fd];
}

