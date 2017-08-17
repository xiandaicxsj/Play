#ifndef _H_CHAR_DEV
#define _H_CHAR_DEV
#include"device.h"

struct char_device_ops
{

};

struct char_device
{
	struct device dev;
	struct char_device_ops * ops;
	struct list_head req_list;
};
void init_char_devices();
void register_char_device(dev_t dev_num, struct char_device *cdev, struct char_device_ops *ops);
#endif
