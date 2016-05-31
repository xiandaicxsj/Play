#ifndef _H_BLK_DEVICE
#define _H_BLK_DEVICE
#include"device.h"
#include"buffer.h"
struct blk_device;

struct blk_request
{
	struct blk_device *device;
	u32 cmd;
	u32 nr_block;
	void *data;
	struct list_head list;
};

struct blk_device_ops
{
		int (*init)(struct blk_device *dev);
		int (*sub_request)(struct blk_device *dev, struct blk_request *req);
};

struct blk_device
{
	struct device;
	struct blk_device_ops;	
	struct list_head req_list;
	u32 block_size;
};

#endif
