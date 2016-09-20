#include"inode.h"
#include"fs.h"
#include"schdule.h"
#include"buffer.h"
#include"test.h"
#include"vfs.h"

#ifdef TEST_FS
#include"string.h"
static int g_fidx = 0;
struct file_struct *g_files[20];
#endif

struct vfs
{
	struct file_struct *file;
	u32 file_struct_count;
};

struct vfs vfs;

struct file_struct *get_stdio_file_struct()
{

}

struct file_struct *get_stdout_file_struct()
{

}

struct file_struct *get_stderr_file_struct()
{

}

struct file_struct *search_file_struct(struct minode *inode)
{
	struct file_struct *empty_fs = NULL;
	struct file_struct *fs_ptr = NULL;
	u32 idx = 0;

	fs_ptr = vfs.file;
	if (!fs_ptr)
		return NULL;

	while(idx < vfs->file_struct_count)
	{

		if (fs_ptr->count && fs_ptr->inode == inode)
			return fs_ptr;

		if(fs_ptr->count == 0 && !empty_fs)
			empty_fs =  fs_ptr;

		idx ++;
	}

	return empty_fs;
}

void init_file_struct()
{
	struct page *file_page; 
	struct file_struct *f_ptr;
	u32 count = 0;
	u32 idx = 0;

	page = kalloc_page(MEM_KERN);
	if (!page)
		return ;

	count = (PAGE_SIZE) / sizeof(struct file_struct);

#ifdef TEST_FS
	f_ptr = (struct file_struct *)phy_to_virt(pfn_to_addr(page->pfn));
#else
	f_ptr =  (struct file_struct *)page->pfn;
#endif

	vfs.file_struct_count = count;
	vfs.file = f_ptr;

	while(idx < count)
	{
		memset(f_ptr, 0, sizeof(*fptr));
		f_ptr++;
		idx ++;
	}
	/* FIXME */
}

void init_vfs()
{
    init_file_struct();
    init_std_file_struct();
}

void destroy_vfs()
{

}

/* for file system one inode vs one file_struct */
/* for 0-3 stdin/out/error we can duplicate this */ 
struct file_struct * find_file_struct(struct minode *inode, u32 file_attr)
{
	/* find */
	struct file_struct *f = NULL;
	if (!inode)
		return NULL;

	f = search_file_struct(inode);
	if (!f)
		return NULL;

	f->type = inode->type;
	f->ops = inode->ops;
	f->data = inode->data;

#if 1 
	/* only one can open to one file
	 * device can be access by two more users at the same time
	 */

	if (f->count && f->type != INODE_DEV )
		return NULL;
#endif

	f->inode = inode;
	f->file_attr = file_attr;
	f->pos = 0;
	/* count shoule be 1 */
	f->count = 1;

	return f;
}

u32 alloc_file_fd(struct task_struct *current)
{
	/* fd_count should be 3 */
	/* 0 stdin
	 * 1 stdout
	 * 2 stderr
	 */
	u32 fd;

	if (current->fd_count >= MAX_FILE_PROCESS)
		return -1;

	if (current->fd_count < 3) {
		/* we need to set the 0 1 2 */
		return -1;
	}

	fd = current->fd_count;
	current->fd_count ++;
	return fd;
}

u32 _sys_close(u32 fd)
{
	struct file_struct *f = NULL;

	if (fd < 0 )
		return -1;
#ifndef TEST_FS
	f = current->file[fd];
	if (!f)
		return NULL;
#else
	f =  g_files[fd];
	if (!f)
		return NULL;
#endif
	f->close(f);
	//put_inode(f->inode);

#ifndef TEST_FS
	current->fs[fd] = NULL;
	/* FIXME delete fd */
#endif
}

u32 _sys_mkdir(char *dir_name)
{

}

u32 _sys_open(char *file_path, u32 file_attr)
{
	struct m_inode *inode;
	struct file_path;
	struct file_struct *file;
	int fd = -1;

	inode = get_inode(file_path, file_attr, INODE_NONE);
	if (!inode)
		return fd;

	if (IS_DEV(inode->type)) {
		if (inode->ops->open(file_path, file_attr))
			return -1;
	}

	/* do we need file , yes*/
	file = find_file_struct(inode, file_attr);

#ifndef TEST_FS
	fd = alloc_file_fd(current);
	current->file[fd] = file;

#else
	fd = g_fidx;
	g_fidx ++;
	g_files[fd] = file;
#endif
	return fd;
}

u32 _sys_seek(int fd, u32 off)
{
	struct file_struct *f ;
#ifndef TEST_FS
	f = &current->file[fd];
#else
	f = g_files[fd];
#endif
	if (f->type == INODE_DE)
		return 0;
	f->pos = off;
}

u32 _sys_read(int fd, void *buffer, u32 size)
{
	struct file_struct *f ;
	struct buffer_head *bh;
	u32 left = size;
	u32 size = 0;
	u32 off = 0;
	struct page *p;
	void *addr;
	
#ifndef TEST_FS
	f = current->file[fd];
#else
	f = g_files[fd];
#endif
	p = kalloc_page(MEM_KERN);
#ifndef TEST_FS
	addr = phy_to_virt(pfn_to_addr(p->pfn));
#else
	addr = (void *)p->pfn;
#endif

	while(left > 0) {

		size = left > PAGE_SIZE ? PAGE_SIZE : left;
		if (f->ops->read(f, addr, size) != SIZE)
			return off;

		if (copy_to_user(buffer + off, addr, size))
			goto out;

		left -= size;
		off += size;
	}

out:
	kfree_page(p);
	return off;
}

u32 _sys_write(int fd, void *buffer, u32 size)
{
	struct file_struct *f ;
	struct buffer_head *bh;
	u32 left = size;
	u32 size = 0;
	u32 off = 0;
	struct page *p;
	void *addr;

#ifndef TEST_FS
	f = current->file[fd];
#else
	f = g_files[fd];
#endif
	if (!(f->file_attr & O_RDWR))
		return -1;

	p = kalloc_page(MEM_KERN);
	addr = phy_to_virt(pfn_to_addr(p->pfn));

	while(left > 0) {

		if (copy_from_user(buffer + off, addr, size))
			return off;

		if (f->ops->write(f, addr, size) != SIZE)
			return off;

		left -= size;
		off += size;
	}

out:
	kfree_page(p);
	return off;
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
