#include"inode.h"
#define DIR_LEN 20
struct super_block *sb;

struct dir_entry 
{
	u32 inode_id;
	char name[DIR_LEN];
};

/* dev_num is not used */
/* need to fix */

static void get_sb(u32 dev_num, struct super_block *sb)
{
	struct buffer_head *bh = NULL;
	bh = look_up_buffer(1);
	sb->hsb = (struct m_super_block *)bh->data;
	sb->bh = bh;
}

static void init_block_bit_map(struct m_super_block *sb)
{
	//struct page *page = NULL;
	struct buffer_head *bh = NULL;
		
	//page = kmalloc_page(MEM_KERN);

	bh = look_up_buffer(sb->hsb.block_bitmap_block);

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
	struct buffer_head *bh = NULL;
		
	//page = kmalloc_page(MEM_KERN);

	bh = look_up_buffer(sb->hsb.inode_bitmap_block);

	sb->inode_bit_map = bh->data;
	sb->inode_bm_bh = bh;
	//sb->inode_bit_map = pfn_to_virt(page->pfn);
	//memcpy(sb->inode_bit_map, bh->data, BUF_SIZE);
	return ;
	/* do we need this */
}

static init_inode_map(struct m_super_block *sb)
{
	struct page *page = NULL;
	struct buffer_head *bh = NULL;
	struct m_inode *m_head;
	struct inode *h_head;
	u32 num  = sb->inode_num;
	u32 pages_num = ROUND_UP((num *sizeof(struct m_inode)), PAGE_SIZE);

	u32 block_num = sb->inode_block_num;
	u32 block_off = sb->inode_block_off;

	/* i don't know whether this isn needed */
	u32 nr_inode_per_buf = BUFF_SIZE/sizeof(struct inode);
	u32 idx = 0;

	page = kalloc_page(pages_num, MEM_KERN);
	sb->inode_map = (struct m_inode *) phy_to_virt(page->pfn);
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
			h_head ++;
			m_head ++;
			idx ++;
			m_head->bh = bh;
		}

		block_num --;
		block_off ++;
	}
	return ;
}

static int init_inode(struct super_block *sb)
{
	init_inode_bit_map(sb);
	init_inode_map(sb);
}

struct put_minode(struct super_block *sb, struct m_inode *inode)
{

}

static m_inode *get_minode(struct m_super_block *sb, u32 file_mode)
{
	/* dirty */
	struct m_inode *inode;
	u32 index = find_set_first_aval_bit(sb->inode_bit_map, BUF_SIZE);

	sb->hsb->inode_used ++;
	inode = sb->inode_map + idex; /* the dirty bit of struct m_inode */
	set_bh_dirty(sb->inode_bm_bh);
	/* init inode */	
	memset(inode->hinode, 0, sizeof(struct inode));
	inode->hinode->used = INODE_USED;
	inode->hinode->mode = file_mode;
	inode->hinode->index = index;

	set_bh_dirty(inode->bh);

	return inode;
	/* this used for alloc small num of structure */
}

/* so why is block_list needed */
static int init_block(struct super_block *sb)
{
	/* only init bit map here is ok */
	init_block_bit_map(sb);
}

/* return block num */
static u32 alloc_block(struct super_block *sb)
{
	u32 idx = find_set_first_aval_bit(sb->block_bit_map, BUF_SIZE);
	sb->hsb->block_used ++;
	set_bh_dirty(sb->bh);

	return idx;
	/* get availule block */
}

int init_super_block(struct sb)
{
        sb = kmalloc(sizeof(*sb), 0, MEM_KERN);
	        if (!sb)
			return -1;
        get_sb(ROOT_DEV, sb);
	init_inode(sb);
	init_block(sb);
}

char get_char(char *array)
{
	return *array;
}

/* get inode by  inode_idx */
static struct m_inode *get_inode_by_idx(struct m_super_block *sb, u32 inode_idx)
{
	struct m_inode *inode;

	if (!test_bit(sb->inode_bit_map, inode_idx))
		return NULL;

	inode = sb->inode_map + inode_idx;
	/* how to determined whether inode is real file or not */
	return inode->hinode.used ? inode: NULL;
}

/* 
 * dir: name 
 * len name len
 */
struct m_inode *get_dir_entry_inode(char *dir, u32 dir_len, struct m_inode *inode, struct dir_entry ** de_ptr)
{
	/* m_inode is different from inode */
	u32 nr_block = inode->zone[0];
	struct buffer_head *bh;
	struct dir_entry *de;
	struct m_inode *inode;
	struct dir_entry *emp_de;

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
		if (emp_de && de->name =='\0')
			emp_de = de;
		de++;
		dir_idx ++;
	}

	if (dir_idx != dir_entry_num)
	{
		inode = get_inode_by_idx(inode->sb, de->inode);
		if (!inode) 
			return NULL;

		if ( !IS_DIR(inode->mode) )
			return NULL;
		return inode;
	}
	*de_ptr = emp_de;
	return NULL;
}

/* inode can be a dir or file */
void insert_parent_inode(struct minode *pinode, struct dir_entry *de, struct minode *inode, char *name, u32 len)
{	
	memcpy(de->name, name, len);
	de->inode = inode->hinode->index;
	/* bugs */	

	/* this bug de has not bh memeber */
	/* need to change this */
	/* change the fucntion param contain struct buffer *bh */
	set_bh_dirty(de->bh);

	/* do we need to set pinode->bh dirty */
	set_bh_dirty(pinode->bh);
}


struct m_inode *get_inode(char *file_path, u32 file_mode)
{
	struct m_inode * parent_inode;
	struct m_inode * inode = NULL;
	struct dir_entry **de_ptr;
	char c ;
	char *dir;
	u32 dir_len ;
	u8 is_alloc = file_mode & O_CREATE;

#ifdef TEST_FS
	if ((c = get_char(file_path)) == '\')
		inode = current->root_node;
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
		/* bugs */
		dir = file_path;
		dir_len = 0;
		for (; c = get_char(file_path) && c != '\' && c != '\0'; file_path++, dir_len ++;;);

		if (c == '\0' ) {
			get_pdir_ok = 1;
		}

		inode = get_dir_entry_inode(dir, dir_len, parent_inode, de_ptr);
		if (!inode)
			break;

		file_path ++;
		parent_inode = inode;
	}

	if (!inode && is_alloc && get_pdir_ok)
	{
		if (is_alloc && ERROR_FILE(file_mode))
				return NULL;
		inode = get_minode(parent_inode->sb, file_mode);
		/* find */
		/* del with this dir */
		insert_parent_inode(parent_inode, *de_ptr, inode, dir, dir_len);
		/* init_inode */

	}
	return inode;
}

struct buffer_head *get_inode_bh(struct m_inode *inode, u32 block_nr)
{
	/* wo should  del with large file here */
	/* block_nr is the offset in the zone array */
	/* block_num is the real block num */
	u32 block_num = inode->zone[block_nr];

	if(block_num = -1)
		return NULL;

	return loop_up_buffer(block_num);
}

