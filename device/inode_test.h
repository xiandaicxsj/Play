#ifndef _H_INODE
#define _H_INODE
#define DIR_LEN 20

#define FILE_TYPE (1 << 0)
#define DIR_TYPE (1 << 1)
#define DEV_TYPE (1 << 2)


typedef unsigned char u8;
typedef unsigned int  u32;
typedef unsigned short u16;
/* 0-5 directly block */
#define NR_BLOCK 10
typedef u32 zone_t;

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

#endif
