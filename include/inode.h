#ifndef _H_INODE
#define _H_INODE
#include"type.h"
#include"buffer.h"

/* 0-5 directly block */
#define NR_BLOCK 10
typedef u32 zone_t;

struct super_block
{
	u32 inode_num;  /* total inode num */
	u32 inode_used; /* already used inode */
	u32 block_num;
	u32 block_used;
	u32 magic; /* determine certain magic number */

	u32 inode_block_off; /* fist block of inode */
	u32 inode_block_num; /* block size */
	u32 inode_bitmap_block;
	u32 block_bitmap_block;
	u32 root_node; /* block nr of root node */
};

struct m_super_block
{
	struct super_block *hsb;
	struct buffer_head *bh; /* bh -> hsb */
	/* used for memory mem */
	u32 dirty;
	/* if this two is dirty should write back */
	/* but how to do this */
	void * inode_bit_map;
	void * block_bit_map;
	struct buffer_head *inode_bm_bh;
	struct buffer_head *block_bm_bh;
	struct m_inode * inode_map;
	struct file_operations *ops;
	void *data;
};

#define INODE_NONE 0
#define INODE_DEV 1
#define INODE_FILE 2

#define MODE_FILE (1 << 0)
#define MODE_DIR (1 << 1)
#define IS_DIR(mod) (mod & MODE_DIR)
#define IS_FILE(mod) (mod & MODE_FILE)
#define ERR_FILE(mod) 
struct inode
{
	u32 mode;
	u32 file_size;
	u32 index;
	u32 bock_used;
	zone_t zone[NR_BLOCK];
	/* this may used furthure */
	u32 access_time;
	u32 modify_time;
	u32 used; /* whether is inode is used */
	/* we need is field to get ops of m_inode */
};

struct m_inode
{
	struct inode* hinode;
	struct m_super_block *sb;
	/* bh -> m_inode struct , not the content of file */
	struct buffer_head *bh;
	u32 count;
	u32 dirty; /* the inode is dirty, not the content is dirty */
	/* type means whether m_inode is juest device/file */ 
	u32 type;
	/* inode_ops */
	struct file_operations *ops;
	/* */
	u32 dev_id;
	void *data;
};

struct m_inode *get_inode(char *file_path, u32 file_mode);
int put_inode(struct m_inode *inode);

struct buffer_head *get_inode_bh(struct m_inode *inode, u32 block_nr, u32 attr);
int init_super_block(u32 dev_num);
#endif
