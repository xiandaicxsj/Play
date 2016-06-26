#ifndef _H_BLK
#define _H_BLK
#include"device.h"
#include"buffer.h"
void blk_read(struct device *dev, struct buffer_head *bh);
void blk_write(struct device *dev, struct buffer_head *bh);



#endif
