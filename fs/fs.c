#include"inode.h"
#include"fs.h"
#include"schdule.h"
#include"buffer.h"
#include"test.h"


#ifdef TEST_FS
#include"string.h"
int g_fidx = 0;
struct file_struct g_files[20];
#endif

void init_fs()
{
	init_buffer(ROOT_DEV);
	init_super_block(ROOT_DEV);
}


u32 alloc_file_struct(struct task_struct *current)
{

	return -1;
}

u32 _sys_close(u32 fd)
{

}

u32 _sys_open(char *file_path, u32 file_attr)
{
	struct m_inode *inode;
	struct file_struct *f;
	int fd = -1;
	u32 is_alloc;
	
	inode = get_inode(file_path, file_attr);

	if (!inode)
		return fd;
#ifndef TEST_FS
	fd = alloc_file_struct(current);
	if (fd < 0)
		return fd;

	f = &current->file[fd];

	f->inode = inode;
	f->file_attr = file_attr;
	f->pos = 0;
#else
	fd = g_fidx;
	f =  &g_files[fd];
	g_fidx ++;
#endif

	return fd;
}

u32 _sys_read(int fd, void *buffer, u32 size)
{
	struct file_struct *f ;
	struct buffer_head *bh;
	int left = size;
	
#ifndef TEST_FS
	f = &current->file[fd];
#else
	f = &g_files[fd];
#endif

	u32 size_off = 0;
	u32 size_read = 0;
	u32 t_size_read = 0;

	u32 block_nr = 0;
	u32 off_in_block = 0;

	off_in_block = f->pos % BUF_SIZE;
	block_nr = f->pos / BUF_SIZE;
	while(left > 0)
	{
		off_in_block = f->pos / BUF_SIZE;

		bh = get_inode_bh(f->inode, block_nr);
 
		if (!bh)
			goto out;

		t_size_read = left > BUF_SIZE ? left : BUF_SIZE;

#ifdef TEST_FS 
		memcpy(buffer + size_off, bh->data + off_in_block, t_size_read);
#else
		if (copy_to_user(buffer + size_off, bh->data + off_in_block, t_size_read))
			goto out;
#endif

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

	struct file_struct *f;
	struct buffer_head *bh;
	int left = size;

#ifndef TEST_FS
	f = &current->file[fd];
#else
	f = &g_files[fd];
#endif

	u32 size_off = 0;
	u32 size_read = 0;
	u32 t_size_read = 0;

	u32 block_nr = 0;
	u32 off_in_block = 0;

	off_in_block = f->pos % BUF_SIZE;
	block_nr = f->pos / BUF_SIZE;
	while(left > 0)
	{
		off_in_block = f->pos / BUF_SIZE;

		bh = get_inode_bh(f->inode, block_nr);
 
		if (!bh)
			goto out;

		t_size_read = left > BUF_SIZE ? left : BUF_SIZE;

#ifdef TEST_FS
		memcpy(bh->data + off_in_block, buffer + size_off,  t_size_read);
#else
		if (copy_from_user(buffer + size_off, bh->data + off_in_block, t_size_read))
		{
			bh->dirty = 1;
			goto out;
		}
#endif

		bh->dirty = 1;
		left -= t_size_read; 
		f->pos += t_size_read;

		block_nr ++;

		off_in_block = 0;
	}
out:
	return size_read;
}


#ifdef TEST_FS
void test1()
{

}

void test2()
{

}

void test3()
{

}

int main()
{
	init_devices();
	init_fs();
	_sys_open("/root/txt", O_CREATE);
}

#endif
