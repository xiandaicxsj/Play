#include"blk_device.h"

void blk_read(struct device *device, struct buffer_head *bh)
{

	u32 block_num = bh->block_num;
	struct blk_request req;
	struct blk_device * blk_device = container_of(device, struct blk_device, device);

	req.cmd = BLK_READ;
	req.block_num  = bh->block_num;
	req.device = blk_device;
	req.data = bh->data;
	init_list(&req);

	blk_device->ops->sub_request(blk_device, &req);

	/* cacule the block_num to device specific cender/... */
}

void blk_write(struct device *device, struct buffer_head *bh)
{

	u32 block_num = bh->block_num;
	struct blk_request req;
	struct blk_device * blk_device = container_of(device, struct blk_device, device);

	req.cmd = BLK_WRITE;
	req.block_num  = bh->block_num;
	req.device = blk_device;
	req.data = bh->data;
	init_list(&req);

	blk_device->ops->sub_request(blk_device, &req);
}

/*
void register_blk_device(struct blk_device *dev, u32 dev_num, struct blk_device_ops *ops)
{

}
*/
