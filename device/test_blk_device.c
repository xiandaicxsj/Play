
void test_blk_init(struct blk_device *device)
{

}

void test_blk_sub_req(struct blk_device *device)
{

}

struct blk_device_ops test_blk_ops
{
	.init = test_blk_init; 
	.sub_req = test_blk_sub_req;

}

struct test_blk_device
{
	struct blk_device blk_dev;
	u32 block_size;
}

static test_blk_device tbd ={
	.block_size = 512;
}

void init_test_blk_device()
{
	u8 minor = alloc_minor(DEV_BLK);
	init_blk_device(&tbd.blk_dev, test_blk_ops);
}
