void blk_read(u32 dev_num, u32 block_num, void *mem)
{
	struct device * blk_device = get_device(dev_num);
	/* cacule the block_num to device specific cender/... */
}

void blk_write(u32 dev_num, u32 block_num, void *mem)
{
	struct device *blk_device = get_device(dev_num);
}
