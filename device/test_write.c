#include<stdio.h>
#include<string.h>
#include"inode_test.h"
typedef unsigned int u32;
#define PAGE_SIZE 4096
#define DIR_LEN 20
#define O_RD (1 << 0)
#define O_RDWR (1 << 1)
#define O_CREATE (1 << 2)
#define O_DEL (1 << 3)


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

union root_block 
{
	struct inode i;
	char b[PAGE_SIZE];
	
};

int write_block(FILE *f, u32 block_nr, char *b, u32 size)
{
	fseek(f, sizeof(char) * size * block_nr , SEEK_SET);
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
	sb->inode_used = 2;
	sb->block_num = 100;
	sb->block_used = 12;
	sb->magic = 0xf321;
	sb->inode_block_off = 3;
	sb->inode_block_num = 10;
	/* 14 bit contain real content */
	sb->inode_bitmap_block = 1;
	sb->block_bitmap_block = 2;
	sb->root_node = 3;

	f = fopen("test_backen.bin", "w");
	if (fwrite(&fb, sizeof(fb), 1, f) != 1)
		printf("error\n");

	//memset(&fb, 0, sizeof(fb));
	/*
	int i = 1;
	for(; i<100; i++)
		fwrite(&fb, sizeof(fb), 1, f);
		*/
	

	memset(&rb, 0, sizeof(rb)); 
	struct inode *id = &rb.i;
	id->type = DIR_TYPE;
	id->mode = O_RDWR;
	id->index = 0;
	id->used = 1;
	id->zone[0] = 13;

	/* /dev */
	id ++;
	id->type = DIR_TYPE | DEV_TYPE;
	id->mode = O_RDWR;
	id->index = 1;
	id->used = 1;
	id->zone[0] = 14;

	write_block(f, sb->root_node ,rb.b, sizeof(rb));

	union dir_block db;
	db.d[0].inode_idx = 0;
	sprintf(db.d[0].name, ".");

	db.d[1].inode_idx = 0;
	sprintf(db.d[1].name, "..");

	db.d[2].inode_idx = 1;
	sprintf(db.d[2].name, "dev");
	
	write_block(f, 13, db.b, sizeof(db));

	/* /dev */ 
	memset(&db, 0, sizeof(db)); 

	db.d[0].inode_idx = 0;
	sprintf(db.d[0].name, ".");

	db.d[1].inode_idx = 0;
	sprintf(db.d[1].name, "..");
	write_block(f, 14, db.b, sizeof(db));


	struct block bb;
	memset(&bb, 0, sizeof(bb));
	bb.b[0] = 0x3;
	write_block(f, sb->inode_bitmap_block, bb.b, sizeof(bb));

	memset(&bb, 0, sizeof(bb));
	// 0 - 14
	bb.b[0] = 0xff;
	bb.b[1] = 0x7f;

	write_block(f, sb->block_bitmap_block, bb.b, sizeof(bb));
	fclose(f);
	return 0;
}
