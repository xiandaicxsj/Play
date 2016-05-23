#include"inode.h"
#include"fs.h"
struct super_block *sb;
static get_sb(u32 dev_num, struct super_block *sb)
{
	blk_read(dev_num, 1, sb);// no for this
}
static init_supoer_block(struct sb)
{
	sb = kmalloc(sizeof(*sb), 0, MEM_KERN);
	if (!sb)
		return ;
	get_sb(ROOT_DEV, sb);
}
void init_fs()
{
	init_page_cache();
	init_super_block();
}

