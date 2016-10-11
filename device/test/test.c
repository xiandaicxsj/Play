#include<stdio.h>
#include<string.h>
typedef unsigned int u32;
#define PAGE_SIZE 4096
#define DIR_LEN 20
#define BACK_FILE "./test_backen.bin"
struct super_block
{
       		u32 inode_num;  /* total inode num */
       		u32 inode_used; /* already used inode */
       		u32 block_num;
       		u32 block_used;
       		u32 magic; /* determine certain magic number */
       		u32 inode_block_off; /* fist block store struct inode data struct */
       		u32 inode_block_num; /* sizeof block used to store struct inode data  struct*/
       		u32 inode_bitmap_block; /* block store the inode_bit_map */
       		u32 block_bitmap_block;
       		u32 root_node; /* block nr of root node */
};

struct dir_entry
{
	        u32 inode_idx;
		char name[DIR_LEN];
};

union dir_block {
	struct dir_entry d[20];
	char b[PAGE_SIZE];
};

union first_block{
	struct super_block sb;
	char b[PAGE_SIZE];
};

struct block {
	char b[PAGE_SIZE];
};

#define NR_BLOCK 10
typedef u32 zone_t;
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
};

union root_block 
{
	struct inode i;
	char b[PAGE_SIZE];
	
};

int read_block(FILE *f, u32 block_nr, char *b, u32 size)
{
	fseek(f, sizeof(char) * size * (block_nr - 1), SEEK_SET);
	if (fread(b, sizeof(char), size, f) != size)
		printf("error\n");

}	 

int main(int argc ,char *argv[])
{
	FILE *f;
	struct block bk; 
	int br  = 3;

	f = fopen(BACK_FILE, "w+");

	union first_block fb;
	memset(&fb, 0, sizeof(fb)); 

	if (fread(&fb, sizeof(fb),1, f) != 1)
		printf("read erro\n");
	struct super_block *sb = &fb.sb;
	printf(" inode_num is %d \n", sb->inode_num);
	printf(" inode_used is %d \n", sb->inode_used);
	printf(" block_num is %d \n", sb->block_num);
	printf(" block_used is %d \n", sb->block_used);
	printf(" block_used is %d \n", sb->magic);
	printf(" inode_block_off is %d \n", sb->inode_block_off);
	printf(" inode_block_num is %d \n", sb->inode_block_num);
	/* 14 bit contain real content */
	printf(" inode_bitmap_block is %d \n", sb->inode_bitmap_block);
	printf(" block_bitmap_block is %d \n", sb->block_bitmap_block);
	printf(" root_node is %d \n", sb->root_node);


	int i= 0;
	if (fread(&bk, sizeof(bk), 1, f) != 1)
		printf("error");
	for( i; i < 10; i++)
	{
		printf("%d: 0x%x\n",i, bk.b[i]);
	}

}
