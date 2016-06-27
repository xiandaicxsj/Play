#include"blk_device.h"
#include"device.h"
#include"test.h"

/* read/write for blk device */

void blk_read(struct device *dev, struct buffer_head *bh)
{

	struct blk_req *req;
	struct blk_device * blk_device = container_of(dev, struct blk_device, dev);


	req = blk_device->ops->get_req(blk_device);

	req->cmd = BLK_READ;
	req->block_num  = bh->block_num;
	req->device = blk_device;
	req->bh = bh;

	blk_device->ops->sub_req(blk_device, req);
	/* same with blk_write, only used for read sys */
	//wait_on(&bh->list, current);
	/* cacule the block_num to device specific cender/... */
}

void blk_write(struct device *dev, struct buffer_head *bh)
{

	u32 block_num = bh->block_num;
	/* this should not be a fuction */
	struct blk_req *req;
	struct blk_device * blk_device = container_of(dev, struct blk_device, dev);

	if (!bh->dirty)
		return ;
	/* req should be maintained by blk_device */
	req = blk_device->ops->get_req(blk_device);

	req->cmd = BLK_WRITE;
	req->block_num  = bh->block_num;
	req->device = blk_device;
	req->bh = bh;

	blk_device->ops->sub_req(blk_device, req);
	/* this should realse lock */
	/* write need to wait */
	/* bugs */
	// wait_on(&bh->list, current);
}

/*
void register_blk_device(struct blk_device *dev, u32 dev_num, struct blk_device_ops *ops)
{

}
*/
