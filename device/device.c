#include"device.h"

struct device_head
{
	u16 dev_num_bit_map;
	struct list list;
	u16 dev_num;
};

struct device_head dev_type_list[DEV_TYPE]; 

int register_device(dev_t dev_num , struct device *device)
{
	struct device_head * dh ;
	u8 major = DEV_MAJ(dev_num);
	u8 minor = DEV_MIN(dev_nu=m);

	dh =  &dev_type_list[major];

	if (test_bit(&dh->dev_num_bit_map, minor))
		return -1;

	set_bit(&dh->dev_num_bit_map, minor);
	list_add(device->list, &dh->list); 
}

int alloc_minor(u8 major)
{
	struct device_head * dh ;
	u8 major = DEV_MAJ(dev_num);
	u8 minor = DEV_MIN(dev_nu=m);

	dh =  &dev_type_list[major];
	return find_first_avail_bit(&dh->dev_num_bit_map);
}

void init_device()
{
	int dev_type_idx = 0;
	for (dev_type_idx; dev_type_idx < DEV_TYPE; dev_type_idex ++) 
	{
		list_init(&dev_type_list[dev_type_idx].list);
		dev_type_list[dev_type_idex].dev_num_bit_map = 0;
	}
	regiset_blk_device();
}

struct device *get_device(dev_t dev_num)
{
	/* need to be done */
}
