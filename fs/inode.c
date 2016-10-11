#include"inode.h"
#include"page.h"
#include"mem.h"
#include"math.h"
#include"bitop.h"
#include"device.h"
#include"fs.h"
#include"string.h"
#include"test.h"
#define INODE_USED 1
#define INODE_UNUSED 0
struct m_super_block *sb;
static struct m_inode *root_inode;
#define ROOT_INODE 0
#define SB_BLK_NR 0
#define DEV_INODE_NUM 100

struct m_inode *get_root_node()
{
	return root_inode;
}

/* dev_num is not used */
/* need to fix */

/* the dev num should be used like this ??*/
static void get_sb(struct device *dev, struct m_super_block *sb, struct file_operations *ops)
{
	struct buffer_head *bh = NULL;
	sb->data = dev;
	sb->ops = ops;
	bh = look_up_buffer(SB_BLK_NR); // the first 1 super block 
	sb->hsb = (struct super_block *)bh->data;
	sb->bh = bh;

	sb->dev_inode_base = sb->hsb->inode_num;
	sb->dev_inode_num = DEV_INODE_NUM;
}

static void init_block_bit_map(struct m_super_block *sb)
{
	//struct page *page = NULL;
	struct buffer_head *bh = NULL;
		
	//page = kmalloc_page(MEM_KERN);

	bh = look_up_buffer(sb->hsb->block_bitmap_block);

	//sb->block_bit_map = pfn_to_virt(page->pfn);
	//memcpy(sb->block_bit_map, bh->data, BUF_SIZE);
	sb->block_bit_map = bh->data;
	sb->block_bm_bh = bh;
	return ;
	/* do we need this */
}

static void init_inode_bit_map(struct m_super_block *sb)
{
	//struct page *page = NULL;
	/* fs related bit map */
	struct buffer_head *bh = NULL;
	struct page *p;
		
	bh = look_up_buffer(sb->hsb->inode_bitmap_block);

	sb->inode_bit_map = bh->data;
	sb->inode_bm_bh = bh;

	/* dev related bit map */

	p = kalloc_page(MEM_KERN);
#ifndef TEST_FS
	sb->dev_inode_bit_map = phy_to_virt(pfn_to_addr(p->pfn));
#else
	sb->dev_inode_bit_map = p->pfn;
#endif
	memset(sb->dev_inode_bit_map, 0, PAGE_SIZE);
	
	return ;
	/* do we need this */
}

static void init_inode_map(struct m_super_block *sb, struct file_operations *ops,u32 type)
{
	struct page *page = NULL;
	struct buffer_head *bh = NULL;
	struct m_inode *m_head;
	struct inode *h_head;
	u32 idx = 0;
	u32 iidx = 0;
	u32 num  = sb->hsb->inode_num;
	u32 pages_num = round_up((num *sizeof(struct m_inode)), PAGE_SIZE) / PAGE_SIZE;

	u32 block_num = sb->hsb->inode_block_num;
	u32 block_off = sb->hsb->inode_block_off;

	/* the base of inode used by inode */
	//u32 dev_inode_base = sb->dev_inode_base;
	u32 dev_inode_num = sb->dev_inode_num;

	/* i don't know whether this isn needed */
	u32 nr_inode_per_buf = BUF_SIZE/sizeof(struct inode);

	page = kalloc_pages(pages_num, MEM_KERN);
#ifndef TEST_FS
	sb->inode_map = (struct m_inode *) phy_to_virt(page->pfn);
#else
	sb->inode_map = (struct m_inode *) page->pfn;
#endif

	if (!sb->inode_map)
		return ;

	m_head = sb->inode_map;
	while(block_num)	
	{
		bh = look_up_buffer(block_off);
		idx = 0; 

		h_head = (struct inode *) bh->data;
		while (idx < nr_inode_per_buf)
		{
			m_head->hinode = h_head;
			/*
			if (h_head->used == INODE_USED)
				m_head->used = INODE_USED;
			else
				m_head->used = INODE_NUSED;
			*/
			/* FIXME we need to zero m_head */
			m_head->bh = bh;
			m_head->sb = sb;
			m_head->dirty = 0;

			if (test_bit(sb->inode_bit_map, iidx)) {
				m_head->ops = !ops ? sb->ops: ops;
			/* data port to device */
				m_head->data = sb->data; 

				m_head->type = h_head->type;
				m_head->index = h_head->index;
			} else {

					m_head->ops = 0;
					m_head->data = 0;
					m_head->type = INODE_NONE;
			}

			h_head ++;
			m_head ++;
			idx ++;
			iidx ++;
		}

		block_num --;
		block_off ++;
	}

	/* dev inode */

	pages_num = round_up((dev_inode_num *sizeof(struct m_inode)), PAGE_SIZE) / PAGE_SIZE;

	page = kalloc_pages(pages_num, MEM_KERN);
#ifndef TEST_FS
	sb->dev_inode_map = (struct m_inode *) phy_to_virt(page->pfn);
#else
	sb->dev_inode_map = (struct m_inode *) page->pfn;
#endif
	memset(sb->dev_inode_map, 0, PAGE_SIZE * pages_num);

	m_head = sb->dev_inode_map; 
	idx = 0;
	while(idx < sb->dev_inode_num) {

		memset(m_head, 0, sizeof(*m_head));
		m_head->type = INODE_DEV;
		idx++;
		m_head ++;
	}

	return ;
}

static int put_minode(struct m_super_block *sb, struct m_inode *inode)
{
	return 0;
}

static int reset_minode(struct m_inode *inode)
{
	inode->count = 0;
	inode->type = 0;
}
/* each inode->hinode */
static int rm_minode(struct m_super_block *sb, struct m_inode *inode)
{
	clear_bit(sb->inode_bit_map, inode->hinode->index);
	set_bh_dirty(sb->inode_bm_bh);

	/* supper block realted */
	sb->hsb->inode_used --;
	set_bh_dirty(sb->bh);

	memset(inode->hinode, 0, sizeof(*(inode->hinode)));

	inode->hinode->used = INODE_UNUSED;
	set_bh_dirty(inode->bh);

	reset_minode(inode);

}
/* FIXME we need to think of this 
 * as we do not want file in /dev 
 * in sb and flush disk
 */
static struct m_inode *get_minode(struct m_super_block *sb, u32 file_mode,
				  u32 type)
{
	/* dirty */
	struct m_inode *inode;
	u32 idx;
	if (type == INODE_DEV) {
		idx = find_first_avail_bit(sb->dev_inode_bit_map, BUF_SIZE);
		set_bit(sb->dev_inode_bit_map, idx);
		inode = sb->dev_inode_map + idx;

		inode->type = type;
		inode->index = idx + sb->dev_inode_base;
		inode->count = 0;
		inode->sb = sb;

		sb->dev_inode_used ++;
	}
	else {
		idx = find_first_avail_bit(sb->inode_bit_map, BUF_SIZE);
		set_bit(sb->inode_bit_map, idx);
		set_bh_dirty(sb->inode_bm_bh);

		inode->sb = sb;
		sb->hsb->inode_used ++;
		inode = sb->inode_map + idx; /* the dirty bit of struct m_inode */
		set_bh_dirty(sb->bh);
	/* init inode */	
		memset(inode->hinode, 0, sizeof(struct inode));
		inode->hinode->used = INODE_USED;
		inode->hinode->mode = file_mode;
		inode->hinode->type = type;
		inode->hinode->index = idx;

		inode->type = type;
		inode->index = idx;
		set_bh_dirty(inode->bh);
        }

	return inode;
	/* this used for alloc small num of structure */
}

/* so why is block_list needed */
static int init_block(struct m_super_block *sb)
{
	/* only init bit map here is ok */
	init_block_bit_map(sb);
	return 0;
}

/* return block num */
static u32 alloc_block(struct m_super_block *sb)
{
	u32 idx = find_first_avail_bit(sb->block_bit_map, BUF_SIZE);
	set_bit(sb->block_bit_map, idx);
	sb->hsb->block_used ++;
	set_bh_dirty(sb->bh);
	set_bh_dirty(sb->block_bm_bh);

	return idx;
	/* get availule block */
}

char get_char(char *array)
{
	return *array;
}

/* get inode by  inode_idx */
static struct m_inode *get_inode_by_idx(struct m_super_block *sb, u32 inode_idx)
{
	struct m_inode *inode_map;
	void *bit_map;
	u32 idx;

	if (inode_idx  < sb->dev_inode_base) {
		idx = inode_idx;
		inode_map = sb->inode_map;
		bit_map = sb->inode_bit_map;
	} else {
	 	idx = inode_idx - sb->dev_inode_base;
		inode_map = sb->dev_inode_map;
		bit_map = sb->dev_inode_bit_map;
	}

	if (!test_bit(bit_map, idx))
		return NULL;

	/* how to determined whether inode is real file or not */
	/* return inode->hinode->used ? inode: NULL; */
	return inode_map + idx;
}

/* 
 * dir: name 
 * len name len
 */
struct m_inode *get_dir_entry_inode(char *dir, u32 dir_len, struct m_inode *inode, struct dir_entry ** de_ptr)
{
	/* m_inode is different from inode */
	u32 nr_block = inode->hinode->zone[0];
	struct buffer_head *bh;
	struct dir_entry *de;
	struct dir_entry *emp_de = NULL;
	struct m_inode *r_inode;

	/* bh should contain the data */
	u32 dir_entry_num = BUF_SIZE / sizeof(struct dir_entry);
	u32 dir_idx;
	bh = look_up_buffer(nr_block);
	/* now bh contain data */
	de = (struct dir_entry *) bh->data;
	
	dir_idx = 0;
	while(dir_idx < dir_entry_num)
	{
		if(!str_cmp(de->name, dir, dir_len))
			break;
		if (!emp_de && *(de->name) =='\0')
			emp_de = de;
		de++;
		dir_idx ++;
	}

	if (dir_idx != dir_entry_num)
	{
		r_inode = get_inode_by_idx(inode->sb, de->inode_idx);
		if (!r_inode) 
			return NULL;

		return r_inode;
	}
	*de_ptr = emp_de;
	return NULL;
}

void delete_parent_inode(struct m_inode *pinode, struct dir_entry *de, struct m_inode *inode)
{
	struct buffer_head *bh;
	memset(de, 0, sizeof(*de));

	bh = look_up_buffer(pinode->hinode->zone[0]);
	set_bh_dirty(bh);
	/* not sure of this */
	set_bh_dirty(pinode->bh);
}
/* inode can be a dir or file */
void insert_parent_inode(struct m_inode *pinode, struct dir_entry *de, struct m_inode *inode, char *name, u32 len)
{	
	struct buffer_head *bh;
	memcpy(de->name, name, len);
	//de->inode_idx = inode->hinode->index;
	de->inode_idx = inode->index;
	/* bugs */	

	/* this bug de has not bh memeber */
	/* need to change this */
	/* change the fucntion param contain struct buffer *bh */
	bh = look_up_buffer(pinode->hinode->zone[0]);
	
	/* dev */
	if (IS_DIR(pinode->type) && IS_DEV(pinode->type))
	{

	} else {
		set_bh_dirty(pinode->bh);
		set_bh_dirty(bh);
	}

	/* refine this */
	if (IS_FILE(inode->type) || IS_DIR(inode->type)) {
		if (!pinode)
			return ;
		inode->ops = pinode->ops;
		inode->data = pinode->data;
	}
	
	/* do we need to set pinode->bh dirty */
}

/* put_inode means we flush the inode buffer
   and dec inode->count, we will never free the 
   inode related buffer */
int put_inode(struct m_inode *inode)
{
	if(!inode)
		return -1;
	if (inode->count <= 0)
		/* FIXME */
		return 0;
	inode->count --;
	if (inode->count)
		return 0;
	/* think of mort */
	if (inode->dirty)
		put_bh(inode->bh);
	/* do we need to sync the inode buffer */
	/* inode->dirty ??? */
	inode->dirty = 0;
	flush_bhs();
	return 0;
}

/* root inode is 0 */
/* /dev
 * we do't need to flush to disk */
int create_inode_dev(char *file_path, struct file_operations *ops, void *data)
{
	create_inode(file_path, ops, data, INODE_DEV);

}

int create_inode(char *file_path, struct file_operations *ops, void *data, u32 type)
{
	struct m_inode *inode = get_inode(file_path, O_CREATE, type);

	if (!inode)
		return -1;

	inode->ops = ops;
	inode->data = data;
	return 0;
}

/* type will be used only for create inode for a device */
struct m_inode *get_inode(char *file_path, u32 file_mode, u32 type)
{
	struct m_inode * parent_inode;
	struct m_inode * inode = NULL;
	struct dir_entry *de_ptr;
	char c ;
	char *dir;
	u32 dir_len ;
	u8 need_create = file_mode & O_CREATE;
	u8 need_rm = file_mode & O_DEL;
	u8 get_pdir_ok = 0;

#ifndef TEST_FS
	c = get_char(file_path);
	if (c == '/')
		inode = current->root;
	else
		inode = current->pwd;
#else
	/* need to write */
	inode = get_root_node();
#endif

	parent_inode = inode;

	file_path ++;
	while(1)
	{
		dir = file_path;
		dir_len = 0;
		for (c = get_char(file_path); c != '/' && c != '\0'; file_path++) {
			c = get_char(file_path);
			dir_len ++;
			if (c == '/')
				break;
		}

		/* /a/b/c means we get /a/b */
		if (c == '\0' && need_create) {
			get_pdir_ok = 1;
		}

		/* de_ptr will be get from later func */
		/* de_ptr is the place to put the file (not dir)*/
		inode = get_dir_entry_inode(dir, dir_len, parent_inode, &de_ptr);
		if (!inode)
			break;

		if (c == '\0' && !need_create) {
			if (need_rm)
				break;
			inode->count ++;
			return inode;
		}
		file_path ++;
		parent_inode = inode;
	}

	if (inode && need_rm)
	{
		if (!de_ptr)
			return NULL;
		delete_parent_inode(parent_inode, de_ptr, inode);
		rm_minode(parent_inode->sb, inode);
	}

	if (!inode && need_create && get_pdir_ok)
	{
		if (ERROR_FILE(file_mode))
				return NULL;
		/* if create  */
		if (IS_NONE(type))
				return NULL;
		inode = get_minode(parent_inode->sb, file_mode, type);
		/* find */
		/* del with this dir */
		insert_parent_inode(parent_inode, de_ptr, inode, dir, dir_len);

		inode->count ++;
		/* init_inode */
	}
	return inode;
}

static void set_inode_dirty(struct m_inode *inode)
{
	if (IS_DEV(inode->type) && IS_DIR(inode->type))
		return ;
	inode->dirty = 1;
	set_bh_dirty(inode->bh);

}

int put_inode_bh(struct buffer_head *bh)
{
	if (bh->dirty)
		put_bh(bh);
	/* check whethe locked bit of bh is set */
	return free_bh(bh);
}

struct buffer_head *get_inode_bh(struct m_inode *inode, u32 block_nr, u32 attr)
{
	/* wo should  del with large file here */
	/* block_nr is the offset in the zone array */
	/* block_num is the real block num */
	u32 block_num = inode->hinode->zone[block_nr];

	/* if is read and block_is none */
	if(block_num == 0 )
	{
		if (attr & O_RDWR)
		{
			block_num = alloc_block(inode->sb);
			inode->hinode->zone[block_nr] = block_num;
			set_inode_dirty(inode);
		} else 
			return NULL;

	}

	return look_up_buffer(block_num);
}

static void init_root(struct m_super_block *sb)
{
	root_inode = get_inode_by_idx(sb, ROOT_INODE);
}

static void init_inode(struct m_super_block *sb, struct file_operations *ops)
{
	init_inode_bit_map(sb);
	init_inode_map(sb, ops, INODE_FILE);
	init_root(sb);
}

int init_super_block(struct device *dev, struct file_operations *ops)
{
	if (!dev) {
		return -1;

	}

    	sb = kmalloc(sizeof(*sb), 0, MEM_KERN);
	if (!sb)
		return -1;
    	get_sb(dev, sb, ops);
	init_inode(sb, ops);
	init_block(sb);
	return 0;
}
