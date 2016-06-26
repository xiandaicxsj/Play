#ifndef _H_BLK_DEVICE
#define _H_BLK_DEVICE

#include"buffer.h"
#include"device.h"

#define BLK_READ (1 << 0)
#define BLK_WRITE (1 << 1)
//extern struct device;
struct blk_device;
struct blk_device_ops;

struct blk_req
{
	struct blk_device *device;
	u32 cmd;
	u32 block_num;
	struct buffer_head *bh;
	struct list_head list;
	struct blk_device_ops* blk_dev_ops;
};

struct blk_device_ops
{
		int (*init)(struct blk_device *dev);
		int (*sub_req)(struct blk_device *dev, struct blk_req *req);
		struct blk_req * (*get_req)(struct blk_device *dev);
};

struct blk_device
{
	struct device dev;
	struct blk_device_ops * ops;	
	struct list_head req_list;
};

void init_blk_device(struct blk_device *device, struct blk_device_ops *ops);
#endif
