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
	struct buffer_head *bh;
	int left = size;

	u32 size_off = 0;
	u32 size_read = 0;
	u32 t_size_read = 0;

	u32 block_nr = 0;
	u32 off_in_block = 0;

	off_in_block = f->pos % BUF_SIZE:
	block_nr = f->pos / BUF_SIZE;
	while(left > 0)
	{
		off_in_block = f->pos / BUF_SIZE;

		bh = get_inode_bh(f->inode, block_nr);
 
		if (!bh)
			goto out;

		t_size_read = left > BUF_SIZE ? left : BUF_SIZE;

		if (copy_to_user(buffer + size_off, bh->data + off_in_block, t_size_read))
			goto out;

		left -= t_size_read; 
		f->pos += t_size_read;

		block_nr ++;

		off_in_block = 0;
	}
out:
	return size_read;
}

u32 _sys_write(int fd, void *buffer, u32 size)
{

	struct file *f = current->file[fd];
	struct buffer_head *bh;
	int left = size;

	u32 size_off = 0;
	u32 size_read = 0;
	u32 t_size_read = 0;

	u32 block_nr = 0;
	u32 off_in_block = 0;

	off_in_block = f->pos % BUF_SIZE:
	block_nr = f->pos / BUF_SIZE;
	while(left > 0)
	{
		off_in_block = f->pos / BUF_SIZE;

		bh = get_inode_bh(f->inode, block_nr);
 
		if (!bh)
			goto out;

		t_size_read = left > BUF_SIZE ? left : BUF_SIZE;

		if (copy_from_user(buffer + size_off, bh->data + off_in_block, t_size_read))
		{
			bh->dirty = 1;
			goto out;
		}

		bh->dirty = 1;
		left -= t_size_read; 
		f->pos += t_size_read;

		block_nr ++;

		off_in_block = 0;
	}
out:
	return size_read;
}


#ifdef TEST_FILE

#endif
