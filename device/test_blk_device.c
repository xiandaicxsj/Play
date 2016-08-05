#include"test.h"
#include"blk_device.h"
#include"type.h"
#include"fs.h"
#ifdef TEST_FS
#include<stdio.h>
#define FILE_BACKEN "../device/test_backen.bin"
#endif
#define TEST_DEV_BLK_SIZE 4096
#define TEST_DEV_BLK_NUM 33
#define TEST_DEV_BLK_REV 3
#define BLOCK_ALIGN char b[TEST_DEV_BLK_SIZE]

struct test_blk_device
{
	struct blk_device blk_dev;
	u32 block_size;
	char data [TEST_DEV_BLK_NUM][TEST_DEV_BLK_SIZE];
#ifdef TEST_FS
	FILE *fd;
#endif
	struct blk_req req;
	int req_idx;
	int req_max;
};

void test_blk_write(struct test_blk_device *tbd, u32 block_num, void *buf)
{
#ifdef TEST_FS
	u32 off = (block_num > 1 ? block_num -1 :0)* TEST_DEV_BLK_SIZE;
	fseek(tbd->fd, off, SEEK_SET);
	fwrite(buf, 1, TEST_DEV_BLK_SIZE, tbd->fd);
#endif
}

void test_blk_read(struct test_blk_device *tbd, u32 block_num, void *buf)
{

#ifdef TEST_FS
	u32 off = (block_num > 1 ? block_num -1 :0)* TEST_DEV_BLK_SIZE;
	fseek(tbd->fd, off, SEEK_SET);
	fread(buf, 1, TEST_DEV_BLK_SIZE, tbd->fd);
#endif
}

void test_blk_init(struct blk_device *device)
{
	struct test_blk_device *tbd = container_of(device, struct test_blk_device, blk_dev);
	tbd->req_idx = 0;
	tbd->req_max = 2;
	return 0;
}

static struct blk_req* test_blk_get_req(struct blk_device *device)
{
	struct test_blk_device *tbd = container_of(device, struct test_blk_device, blk_dev);

	/*if (tbd->req_idx >= tbd->req_max)
		return NULL;
	*/
	return &tbd->req;
}

static void test_blk_free_req(struct blk_device *device, struct blk_req *req)
{
}

void test_blk_sub_req(struct blk_device *device, struct blk_req *req)
{
	struct test_blk_device *tbd = container_of(device, struct test_blk_device, blk_dev);
	switch(req->cmd) {
	case BLK_READ:
		test_blk_read(tbd, req->block_num, req->bh->data);
		break;
	case BLK_WRITE:
		test_blk_write(tbd,  req->block_num, req->bh->data);
		break;
	}
	test_blk_free_req(device, req);
	return ;
}

struct blk_device_ops test_blk_ops =
{
	.init = test_blk_init,
	.sub_req = test_blk_sub_req,
	.get_req = test_blk_get_req
};

static struct test_blk_device tbd =
{
	.block_size = TEST_DEV_BLK_SIZE
};

static void contruct_test_blk_data(struct test_blk_device *tbd)
{
	int inode_bit_map_bn = 1;
	int block_bit_map_bn = 2;
	void *inode_bm;
	void *block_bm;
/* contruct super block */
	struct super_block *sb  = (struct super_block *)&tbd->data[0][0];
/* contruct inode bit map*/
	inode_bm = &tbd->data[inode_bit_map_bn][0];
	block_bm = &tbd->data[block_bit_map_bn][0];
/* */
}

static void init_test_blk_req()
{

}

void init_test_blk_device(void)
{
	dev_t dev_num = ROOT_DEV;
	//contruct_test_blk_data(&tbd);
	init_test_blk_req();
#ifdef TEST_FS
	tbd.fd = fopen(FILE_BACKEN, "a+");
#endif
	init_blk_device(dev_num, &tbd.blk_dev, &test_blk_ops);
	test_blk_init(&tbd.blk_dev);
}
