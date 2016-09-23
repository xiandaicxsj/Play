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
#endif
