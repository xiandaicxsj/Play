#include"blk_device.h"

/* read/write for blk device */

void blk_read(struct device *device, struct buffer_head *bh)
{

	u32 block_num = bh->block_num;
	struct blk_req req;
	struct blk_device * blk_device = container_of(device, struct blk_device, device);

	req.cmd = BLK_READ;
	req.block_num  = bh->block_num;
	req.device = blk_device;
	req.bh = bh;
	init_list(&req->list);

	blk_device->ops->sub_req(blk_device, &req);
	wait_on(&bh->list, current);
	/* cacule the block_num to device specific cender/... */
}

void blk_write(struct device *device, struct buffer_head *bh)
{

	u32 block_num = bh->block_num;
	struct blk_req req;
	struct blk_device * blk_device = container_of(device, struct blk_device, device);

	if (!bh->dirty)
		return ;
	req.cmd = BLK_WRITE;
	req.block_num  = bh->block_num;
	req.device = blk_device;
	req.bh = bh;
	init_list(&req->list);
	blk_device->ops->sub_req(blk_device, &req);
	/* this should realse lock */
	/* write need to wait */
	wait_on(&bh->list, current);
}

/*
void register_blk_device(struct blk_device *dev, u32 dev_num, struct blk_device_ops *ops)
{

}
*/
