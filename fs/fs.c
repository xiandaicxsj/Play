#include"inode.h"
#include"fs.h"
#include"schdule.h"
#include"buffer.h"
#include"test.h"


#ifdef TEST_FS
#include"string.h"
static int g_fidx = 0;
struct file_struct *g_files[20];
#endif

struct file_operations fs_ops{
	.open = fs_open,
	.close = fs_close,
	.read = fs_read,
	.write = fs_write,
	.seek = fs_seek,
	.mkdir = fs_mkdir,
};

void init_fs()
{
	struct device *dev;
	dev = get_device(ROOT_DEV);
	/* this buffer is */
	init_buffer(dev);
	init_super_block(dev, fs_ops);
}

u32 fs_close(struct file_struct *f)
{
	put_inode(f->inode);
#ifndef TEST_FS
	current->fs[fd] = NULL;
	/* FIXME delete fd */
#endif
}

u32 fs_mkdir(char *dir_name)
{

}

/* not sure */
u3 fs_open(char *file_path, u32 file_atter)
{
	struct m_inode *inode;
	struct file_struct *f;
	int fd = -1;
	u32 is_alloc;

#if 1 
	fd = alloc_file_struct(current);
#endif
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

	f->inode = inode;
	f->file_attr = file_attr;
	f->pos = 0;

	g_fidx ++;
#endif

	return fd;
}

u32 fs_seek(struct file_struct *f, u32 off)
{
	struct file_struct *f ;
#ifndef TEST_FS
	f = current->file[fd];
#else
	f = g_files[fd];
#endif
	f->pos = off;

}

u32 fs_read(struct file_struct *f, void *buffer, u32 size)
{
	struct buffer_head *bh;
	int left = size;
	
#ifndef TEST_FS
	f = current->file[fd];
#else
	f = g_files[fd];
#endif

	u32 size_off = 0;
	u32 size_read = 0;
	u32 t_size_read = 0;

	u32 block_nr = 0;
	u32 off_in_block = 0;

	while(left > 0)
	{

		off_in_block = f->pos % BUF_SIZE;
		block_nr = f->pos / BUF_SIZE;

		bh = get_inode_bh(f->inode, block_nr, O_RD);
 
		if (!bh)
			goto out;

		t_size_read = left < BUF_SIZE ? left : BUF_SIZE;

		/* mark bh as dirty */
		memcpy(buffer + size_off, bh->data + off_in_block, t_size_read);

		left -= t_size_read; 
		f->pos += t_size_read;
		size_read += t_size_read;
		size_off += t_size_read;

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
	f = current->file[fd];
#else
	f = g_files[fd];
#endif

	if (!(f->file_attr & O_RDWR))
		return -1;

	u32 size_off = 0;
	u32 size_write = 0;
	u32 t_size_write = 0;

	u32 block_nr = 0;
	u32 off_in_block = 0;

	while(left > 0)
	{
		
		off_in_block = f->pos % BUF_SIZE;
		block_nr = f->pos / BUF_SIZE;

		bh = get_inode_bh(f->inode, block_nr, O_RDWR);
 
		if (!bh)
			goto out;

		t_size_write = left < BUF_SIZE ? left : BUF_SIZE;

		memcpy(bh->data + off_in_block, buffer + size_off,  t_size_write);

		set_bh_dirty(bh);
		left -= t_size_write; 
		f->pos += t_size_write;
		size_write += t_size_write;
		size_off += t_size_write;
		block_nr ++;

	}
out:
	return size_write;
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
	int fd = 0;
	char buf[20];
	char k[20];
	int ret;
	init_devices();
	init_fs();

	fd = _sys_open("/txt", O_CREATE |O_RDWR);
	sprintf(buf, "aaa");
	ret = _sys_write(fd, buf, sizeof(buf));
	/*
	fd = _sys_open("/txt", O_RD);
	printf("%d\n", ret);
	_sys_seek(fd, 0);
	_sys_read(fd, k, sizeof(k));
	printf("%s\n", k);
	*/
	_sys_close(fd);
	des_devices();
	return 0;
}

#endif
