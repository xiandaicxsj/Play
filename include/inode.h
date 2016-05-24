#ifndef _H_INODE
#define _H_INODE

/* 0-5 directly block */
#define NR_BLOCK 10
typedef zone_t u32;

struct super_block
{
	u32 inode_num;  /* total inode num */
	u32 inode_used; /* already used inode */
	u32 block_num;
	u32 block_used;
	u32 magic; /* determine certain magic number */

	u32 inode_map; /* block size */
	u32 block_map;
	u32 block_size;
	u32 is_dirty;

	u32 root_node; /* block nr of root node */

	/* used for memory mem */
	void * inode_mem_map;
	struct list_head inode_list;

	void * block_mem_map;
};

struct inode
{
	u32 icount;
	u32 mode;
	u32 file_size;
	u32 dirty;
	zone_t zone[NR_BLOCK];
	/* this may used furthure */
	u32 access_time;
	u32 modify_time;
};

#endif
