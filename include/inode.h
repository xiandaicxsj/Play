#ifndef _H_INODE
#define _H_INODE
#include"type.h"
#include"buffer.h"
#include"fs.h"

/* 0-5 directly block */
#define NR_BLOCK 10
typedef u32 zone_t;

#define DIR_LEN 20
struct dir_entry
{
       u32 inode_idx;
       char name[DIR_LEN];
};

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

#define INODE_NONE NONE_TYPE
#define INODE_DIR DIR_TYPE
#define INODE_FILE FILE_TYPE
#define INODE_DEV DEV_TYPE

#define IS_DIR(mod) (mod & INODE_DIR)
#define IS_FILE(mod) (mod & INODE_FILE)
#define IS_DEV(mod) (mod & INODE_DEV)
#define IS_NONE(mod) (mod == NONE_TYPE)
#define ERR_FILE(mod) 0
struct inode
{
	u32 mode;
	u32 type; /*file or dir */
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
	u32 type; /* type means whether m_inode is juest device/file */ 

	/* inode_ops */
	struct file_operations *ops;
	void *data;
};

struct m_inode *get_inode(char *file_path, u32 file_mode, u32 type);
int put_inode(struct m_inode *inode);
struct m_inode *get_root_node();

struct buffer_head *get_inode_bh(struct m_inode *inode, u32 block_nr, u32 attr);
int init_super_block(struct device *dev, struct file_operations *ops);
#endif
