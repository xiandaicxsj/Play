#include"char_device.h"

void init_char_device()
{
	init_key_board();
	init_mouse();
	init_console();
}

void register_char_device(dev_t dev_num, struct char_device *cdev, struct char_device_ops *ops)
{
	cdev->ops = ops;
	register_device(dev_num, &cdev->dev);
}
