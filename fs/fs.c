#include"inode.h"
#include"fs.h"
#include"schdule.h"
#include"buffer.h"
#include"test.h"
#include"string.h"

#ifdef TEST_FS
static int g_fidx = 0;
struct file_struct *g_files[20];
#endif

int fs_close(struct file_struct *f)
{
	put_inode(f->inode);
}

int fs_mkdir(char *dir_name)
{

}

/* not sure */
int fs_open(struct m_inode *inode, u32 file_attr)
{
	return 0;
}

int fs_seek(struct file_struct *f, u32 off, u32 set)
{
	/* FIXME */
	f->pos = set + off;
}

int fs_read(struct file_struct *f, void *buffer, u32 size)
{
	struct buffer_head *bh;
	int left = size;
	
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

int fs_write(struct file_struct *f, void *buffer, u32 size)
{

	struct buffer_head *bh;
	int left = size;

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

/*
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
	fd = _sys_open("/txt", O_RD);
	printf("%d\n", ret);
	_sys_seek(fd, 0);
	_sys_read(fd, k, sizeof(k));
	printf("%s\n", k);
	_sys_close(fd);
	des_devices();
	return 0;
}

#endif
*/
struct file_operations fs_ops = {
	.open = fs_open,
	.close = fs_close,
	.read = fs_read,
	.write = fs_write,
	.seek = fs_seek,
};

int init_fs()
{
	struct device *dev;
	dev = get_device(ROOT_DEV);
	if (!dev)
		return -1;
	/* this buffer is */
	init_buffer(dev);
	init_super_block(dev, &fs_ops);
	return 0;
}
