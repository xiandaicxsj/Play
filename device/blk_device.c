#include"blk_device.h"

void init_blk_device(struct blk_device *dev, struct blk_device_ops *ops)
{
	u8 minor = alloc_minor(DEV_BLK);
	list_init(&dev->req_list);
	dev->ops = ops;
	init_device(&dev->dev, DEV_BLK, minor);
}
