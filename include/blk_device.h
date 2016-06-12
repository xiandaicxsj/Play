#ifndef _H_BLK_DEVICE
#define _H_BLK_DEVICE
#include"device.h"
#include"buffer.h"
struct blk_device;

struct blk_req
{
	struct blk_device *device;
	u32 cmd;
	u32 nr_block;
	struct buffer_head *bh;
	struct list_head list;
};

struct blk_device_ops
{
		int (*init)(struct blk_device *dev);
		int (*sub_req)(struct blk_device *dev, struct blk_req *req);
};

struct blk_device
{
	struct device;
	struct blk_device_ops;	
	struct list_head req_list;
};

void register_blk_device(void);

#endif
