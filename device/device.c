#include"device.h"
#include"test.h"
#include"type.h"
#include"char_device.h"
struct device_head
{
	u16 dev_num_bit_map;
	struct list_head list;
	u16 dev_num;
};

struct device_head dev_type_list[DEV_TYPE_COUNT]; 

int register_device(dev_t dev_num, struct device *dev)
{
	struct device_head * dh ;
	u8 major = DEV_MAJ(dev_num);
	u8 minor = DEV_MIN(dev_num);

	dh =  &dev_type_list[major];

	if (test_bit(&dh->dev_num_bit_map, minor))
		return -1;

	set_bit(&dh->dev_num_bit_map, minor);

	dev->dev_num = dev_num;
	list_init(&dev->list);
	list_add(&dev->list, &dh->list); 
}

int alloc_minor(u8 major)
{
	struct device_head * dh ;
	dh =  &dev_type_list[major];
	return find_first_avail_bit(&dh->dev_num_bit_map);
}

static void init_device_inode()
{
}

void pre_init_devices()
{
	int dev_type_idx = 0;
	for (dev_type_idx; dev_type_idx < DEV_TYPE_COUNT; dev_type_idx ++) {
		list_init(&dev_type_list[dev_type_idx].list);
		dev_type_list[dev_type_idx].dev_num_bit_map = 0;
	}

	/* /dev/ */
	//init_device_inode();
	init_blk_devices();
	//init_char_devices();
}

void post_init_device()
{
	init_char_devices();
}

void des_devices()
{
	des_blk_devices();
}

struct device *get_device(dev_t dev_num)
{
	struct device_head *dh;
	struct list_head *pos;
	struct device *res = NULL;
	u8 major = DEV_MAJ(dev_num);
	u8 minor = DEV_MIN(dev_num);

	dh =  &dev_type_list[major];

	if (!test_bit(&dh->dev_num_bit_map, minor))
		return res;

	list_for_each(&dh->list, pos) {
		res = container_of(pos, struct device, list);
		if ( res->dev_num == dev_num )
			break;
	}
	return res;
}
