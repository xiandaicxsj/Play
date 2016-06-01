#ifdef 	_H_DEVICE
#define _H_DEVICE
#include"list.h"
struct device
{
	u32 device_id;
	u32 type;
	struct list_head list;
}
struct *device  get_device(u32 dev_num);
#endif
