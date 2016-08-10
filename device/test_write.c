#include<stdio.h>
#include<string.h>
typedef unsigned int u32;
#define PAGE_SIZE 4096
#define DIR_LEN 20
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

int write_block(FILE *f, u32 block_nr, char *b, u32 size)
{
	fseek(f, sizeof(char) * size * (block_nr - 1), SEEK_SET);
	if (fwrite(b, sizeof(char), size, f) != size)
		printf("error\n");

}	 

int main()
{
	union first_block fb;
	FILE *f;
	union root_block rb;

	memset(&fb, 0, sizeof(fb)); 
	struct super_block *sb = &fb.sb;
	sb->inode_num = 100;
	sb->inode_used = 1;
	sb->block_num = 100;
	sb->block_used = 1;
	sb->magic = 0xf321;
	sb->inode_block_off = 4;
	sb->inode_block_num = 10;
	/* 14 bit contain real content */
	sb->inode_bitmap_block = 2;
	sb->block_bitmap_block = 3;
	sb->root_node = 4;
	f = fopen("test_backen.bin", "w");
	if (fwrite(&fb, sizeof(fb),1, f) != 1)
		printf("error\n");
	int i = 0;

	memset(&fb, 0, sizeof(fb)); 
	while(i < 100)
	{
		if (fwrite(&fb, sizeof(fb),1, f) != 1)
			printf("error\n");
		i++;
	}

	memset(&rb, 0, sizeof(rb)); 
	struct inode *id = &rb.i;
	id->mode = 2;
	id->index = 0;
	id->zone[0] = 11;

	write_block(f, 4 ,rb.b, sizeof(rb));

	union dir_block db;
	db.d[0].inode_idx = 0;
	sprintf(db.d[0].name, ".");

	db.d[1].inode_idx = 0;
	sprintf(db.d[1].name, "..");
	
	write_block(f, 11, db.b, sizeof(db));


	struct block bb;
	memset(&bb, 0, sizeof(bb));
	bb.b[0] = 0x1;
	write_block(f, 2, bb.b, sizeof(bb));

	memset(&bb, 0, sizeof(bb));
	// 1 - 13
	bb.b[0] = 0xff;
	bb.b[1] = 0x1f;
	write_block(f, 3, bb.b, sizeof(bb));
	fclose(f);
	return 0;
}
