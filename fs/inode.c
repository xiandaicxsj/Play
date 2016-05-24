#include"inode.h"
struct super_block *sb;

static int get_sb(u32 dev_num, struct super_block *sb)
{
        blk_read(dev_num, 1, sb);// no for this
}

static int init_inode(struct super_block *sb)
{
	u32 inode_num = sb->inode_num;
	u32 idx = 0;
	list_init(&sb->inode_list);
}

static int alloc_inode(struct super_block *sb)
{
	/* this used for alloc small num of structure */
}

static int init_block(struct super_block *sb)
{
	list_init(&sb->block_list);
}

static int alloc_block(struct super_block *sb)
{
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

