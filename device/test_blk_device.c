#include"test.h"
#include"blk_device.h"
#include"type.h"
#include"fs.h"
#include"page.h"
#include"mem.h"
#ifdef TEST_FS
#include<stdio.h>
#define FILE_BACKEN "../device/test_backen.bin"
#endif

#ifdef TEST_FS
#define TEST_DEV_BLK_SIZE 4096
#define TEST_DEV_BLK_NUM 33
#else
#define TEST_DEV_BLK_SIZE 1
#define TEST_DEV_BLK_NUM 33
#endif

#define TEST_DEV_BLK_REV 3
#define BLOCK_ALIGN char b[TEST_DEV_BLK_SIZE]

#define BLOCK_OFF(num)  (block_num * TEST_DEV_BLK_SIZE)

struct test_blk_device
{
	struct blk_device blk_dev;
	u32 block_size;
	char data [TEST_DEV_BLK_NUM][TEST_DEV_BLK_SIZE];
#ifdef TEST_FS
	FILE *fd;
#endif
	struct list_head free_req_list;
	struct list_head used_req_list;
	u32 req_count;
	struct page *req_page;
};

void test_blk_write(struct test_blk_device *tbd, u32 block_num, void *buf)
{
#ifdef TEST_FS
	void *mem = kmalloc(TEST_DEV_BLK_SIZE , 0 ,0);
	int ret = 0;

	u32 off = BLOCK_OFF(block_num);
	fseek(tbd->fd, off, SEEK_SET);
	ret = fwrite(buf, 1, TEST_DEV_BLK_SIZE, tbd->fd);
	fflush(tbd->fd);

	fseek(tbd->fd, off, SEEK_SET);
	fread(mem, 1, TEST_DEV_BLK_SIZE, tbd->fd);
#endif
}

void test_blk_read(struct test_blk_device *tbd, u32 block_num, void *buf)
{

#ifdef TEST_FS
	u32 off = BLOCK_OFF(block_num);
	fseek(tbd->fd, off, SEEK_SET);
	fread(buf, 1, TEST_DEV_BLK_SIZE, tbd->fd);
#endif
}

int test_blk_init(struct blk_device *device)
{
	struct test_blk_device *tbd = container_of(device, struct test_blk_device, blk_dev);
	return 0;
}

static struct blk_req *test_blk_get_req(struct blk_device *device)
{
	struct test_blk_device *tbd = container_of(device, struct test_blk_device, blk_dev);
	struct list_head *cur;
	struct blk_req *req;

	if (tbd->req_count <= 0)
		return NULL;

	cur = tbd->free_req_list.next;
	tbd->req_count --;
	if (!cur)
		return NULL;
	req = container_of(cur, struct blk_req, list);
	return req;
}

static void test_blk_free_req(struct blk_device *device, struct blk_req *req)
{
	struct test_blk_device *tbd = container_of(device, struct test_blk_device, blk_dev);

	list_add(&req->list, &tbd->free_req_list);
}

int test_blk_sub_req(struct blk_device *device, struct blk_req *req)
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
	struct blk_req * req_addr;
	u32 req_num  = 0;
	u32 req_idx = 0;

	list_init(&tbd.free_req_list);
	list_init(&tbd.used_req_list);
	tbd.req_page = kalloc_page(MEM_KERN);

#ifdef TEST_FS
	req_addr = (struct blk_req *)(tbd.req_page->pfn);
#else
	req_addr = (struct blk_req *)phy_to_virt(tbd.req_page->pfn);
#endif
	
	req_num = PAGE_SIZE / sizeof(*req_addr);

	for(req_idx; req_idx < req_num; req_idx ++)	
	{
		list_init(&req_addr->list);
		list_add(&req_addr->list, &tbd.free_req_list);
		req_addr++;
	}
	tbd.req_count = req_num;
}

void des_test_blk_device(void)
{
#ifdef TEST_FS
	if (tbd.fd)
		fclose(tbd.fd);
#endif
}

/* this should be call in .init */
void init_test_blk_device(void)
{
        u32 major = DEV_BLK;
        u32 minor = alloc_minor(major);

        dev_t dev_num = DEV_NUM(major, minor);
	dev_num = ROOT_DEV;
	//contruct_test_blk_data(&tbd);
	init_test_blk_req();
#ifdef TEST_FS
	tbd.fd = fopen(FILE_BACKEN, "r+");
#endif
	register_blk_device(dev_num, &tbd.blk_dev, &test_blk_ops);
	test_blk_init(&tbd.blk_dev);
}
