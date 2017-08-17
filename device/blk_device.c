#include"blk_device.h"
#include"test.h"
#include"test_blk_device.h"

void init_blk_devices()
{
	init_test_blk_device();
}

void des_blk_devices()
{
	des_test_blk_device();
}

void register_blk_device(dev_t dev_num, struct blk_device *bdev, struct blk_device_ops *ops)
{
	list_init(&bdev->req_list);
	bdev->ops = ops;
	register_device(dev_num, &bdev->dev);
}
