#include"inode.h"
#include"fs.h"
#include"schdule.h"
#include"buffer.h"
#include"test.h"
#include"mem.h"
#include"page.h"
#include"inode.h"
//#include"vfs.h"
#include"test.h"
#include"bitop.h"
#include"type.h"

#ifdef TEST_FS
#include"string.h"
static u32 fd_bit_map; 
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

/* search mean we need to create if necessary */
struct file_struct *search_file_struct(struct minode *inode)
{
	struct file_struct *empty_fs = NULL;
	struct file_struct *fs_ptr = NULL;
	u32 idx = 0;

	fs_ptr = vfs.file;
	if (!fs_ptr)
		return NULL;

	while(idx < vfs.file_struct_count)
	{

		if (fs_ptr->count && fs_ptr->inode == inode)
			return fs_ptr;

		if(fs_ptr->count == 0 && !empty_fs)
			empty_fs =  fs_ptr;

		idx ++;
		fs_ptr ++;
	}

	return empty_fs;
}

void init_file_struct()
{
	struct page *page; 
	struct file_struct *f_ptr;
	u32 count = 0;
	u32 idx = 0;

	page = kalloc_page(MEM_KERN);
	if (!page)
		return ;

	count = (PAGE_SIZE) / sizeof(struct file_struct);

#ifndef TEST_FS
	f_ptr = (struct file_struct *)phy_to_virt(pfn_to_addr(page->pfn));
#else
	f_ptr =  (struct file_struct *)page->pfn;
#endif

	vfs.file_struct_count = count;
	vfs.file = f_ptr;

	while(idx < count)
	{
		memset(f_ptr, 0, sizeof(*f_ptr));
		f_ptr++;
		idx ++;
	}
	/* FIXME */
}

void init_vfs()
{
    init_file_struct();
    //init_std_file_struct();
}

void destroy_vfs()
{

}


int put_file_struct(struct file_struct *f)
{
	if (!f)
		return -1;
	if (f->count > 1) {
		f->count --;
		return ;
	}
	/* just clear this */
	memset(f, 0, sizeof(*f));
}
/* for file system one inode vs one file_struct */
/* for 0-3 stdin/out/error we can duplicate this */ 
struct file_struct * get_file_struct(struct m_inode *inode, u32 file_attr)
{
	/* find */
	struct file_struct *f = NULL;
	if (!inode)
		return NULL;

	f = search_file_struct(inode);
	if (!f) 
		return NULL;

	if (f->count >= 1) {
		f->count ++;
		return f;
	}

#if 0
	/* only one can open to one file
	 * device can be access by two more users at the same time
	 */

	if (f->count && f->type != INODE_DEV )
		return NULL;
#endif

	f->type = inode->type;
	f->ops = inode->ops;
	f->data = inode->data;
	f->inode = inode;
	f->file_attr = file_attr;
	f->pos = 0;
	/* count shoule be 1 */
	f->count = 1;

	return f;
}

int put_fd(u32 fd)
{
#ifdef TEST_FS
	if (!test_bit(&fd_bit_map, fd))
		return -1;
	clear_bit(&fd_bit_map, fd);
	g_files[fd] = NULL;
#else

#endif

}

int get_fd(struct file_struct *file)
{
	/* fd_count should be 3 */
	/* 0 stdin
	 * 1 stdout
	 * 2 stderr
	 */
	u32 fd;
#ifdef TEST_FS
	fd = find_first_avail_bit(&fd_bit_map, 64);
	set_bit(&fd_bit_map, fd);
	g_files[fd] = file;
#else

	if (current->fd_count >= MAX_FILE_PROCESS)
		return -1;

	if (current->fd_count < 3) {
		/* we need to set the 0 1 2 */
		return -1;
	}

	/* FIXME we should change fd in task to bitmap */
	fd = current->fd_count;
	current->file[fd] = file;
	current->fd_count ++;
#endif
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
	f->ops->close(f);
	//put_inode(f->inode);

	put_file_struct(f);
	put_fd(fd);
}

u32 _sys_mkdir(char *file_path)
{
	u32 file_attr = O_CREATE;
	struct m_inode *inode;
	inode = get_inode(file_path, file_attr, INODE_DIR);

	if (!inode)
		return -1;
	return 0;
}

int _sys_open(char *file_path, u32 file_attr)
{
	struct m_inode *inode;
	struct file_path;
	struct file_struct *file;
	u32 type;
	int fd = -1;

	if (file_attr & O_CREATE)
		type = INODE_FILE;
	else 
		type = INODE_NONE;

	inode = get_inode(file_path, file_attr, INODE_FILE);
	if (!inode)
		return fd;

	if (IS_DEV(inode->type)) {
		if (inode->ops->open(file_path, file_attr))
			return -1;
	}

	/* do we need file , yes*/
	file = get_file_struct(inode, file_attr);
	if (!file)
		return -1;

#ifndef TEST_FS
	fd = get_fd(file);
#else
	fd = get_fd(file);
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
	if (f->type == INODE_DEV)
		return 0;
	f->pos = off;
}

u32 _sys_read(int fd, void *buffer, u32 _size)
{
	struct file_struct *f ;
	struct buffer_head *bh;
	u32 left = _size;
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
		if (f->ops->read(f, addr, size) != size)
			return off;

#ifdef TEST_FS
		memcpy(buffer + off, addr, size);
#else
		if (copy_to_user(buffer + off, addr, size))
			goto out;
#endif

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
#ifndef TEST_FS
	addr = phy_to_virt(pfn_to_addr(p->pfn));
#else
	addr = (void *)p->pfn;
#endif

	while(left > 0) {
		size = left > PAGE_SIZE ? PAGE_SIZE : left;

#ifdef TEST_FS
		memcpy(addr, buffer+ off, size);
#else
		if (copy_from_user(buffer + off, addr, size))
			return off;
#endif

		if (f->ops->write(f, addr, size) != size)
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
	init_vfs();
	pre_init_devices();
	init_fs();
	post_init_device();
	
	fd = _sys_open("/txt", O_CREATE |O_RDWR);
	sprintf(buf, "aaa");
	ret = _sys_write(fd, buf, sizeof(buf));
	_sys_seek(fd, 0);
	_sys_read(fd, k, sizeof(k));
	printf("%s\n", k);
	_sys_close(fd);

	fd = _sys_open("/dev/kb", O_RDWR);
	ret = _sys_write(fd, buf, sizeof(buf));
	des_devices();
	return 0;
}

#endif
