/* use for blokc cache */ 
/* block cache is 4k */
#define HASH_SIZE 100 /* we use block num to hash this */
#define BUF_HASH(size) ((size %  HASH_SIZE) + 1) /* 0 is used for free buf */
#define BUF_NUM 100
#define BUF_SIZE 4096
struct buffer_head
{
	struct device *device; /* this is import */

	struct list_head list;
	void *data;/* 4k or may be 1k 2k */
	u32 dirty;
	u32 block_num;
	int icount; /* who refer to this */

	struct list_head wait_queue; /* task wait for this */
};

struct buffer_head buf_hash[HASH_SIZE];

static struct buffer_head * alloc_new_bh(u32 block_num)
{
	u32 free_hash_idx = 0;
	struct buffer_head *bh = NULL;
	struct list_head *list;
	if(!buf_head[free_hash_idx].icount)
		return bh;
	list = &buf_head[free_hash_idx].list->next;
	bh = container_of(list, struct buffer_head, list);
	buf_head[free_hash_idx].icount --;

	list_del(list);
	list_add(list, &buf_head[BUF_HASH(block_num)].list);
	buf_head[BUF_HASH(block_num)].icount ++;
	return bh;
}
/* search in hash table */
static struct buffer_head *_look_up_buffer(u32 block_num)
{
	struct buffer_head *bh = &buf_hash[BUF_HASH(block_num)];
	if (!bh->icount)
		return NULL;
	/* search for certain */
	/* need write this part */
	/* for_each_list */
	struct list_head *pos;
	struct buffer_head *cur;
	list_for_each(&bh->list, pos)
	{
		cur = container_of(pos, struct buffer_head, list);
		if (cur->block_num == block_num)
			break;
	}
	return cur; /* bh may be NULL */
}

u32 free_buffer(struct buffer_head *bh)
{
	u32 free_hash_idx = 0;
	/* other del should be done here */
	/* need to write buffer to disk */
	/* need to check icount */
	list_del(&bh->list);
	buf_hash[BUF_HASH(bh->block_num)].icount --;	
	list_add(&bh->list, &buf_hash[free_hash_idx].list);
	buf_hash[free_hash_idx].icount++ ;
}

void get_bh(struct buffer_head *bh)
{
	struct device *device = bh->device;
	switch (device->type)
	{
		case DEVICE_BLOCK:
			blk_read(device, bh);
			wait_on(&bh->list, current);
			break;
		default :
			break;
	}
}

void put_bh(struct buffer_head *bh, u32 block_num)
struct buffer_head* look_up_buffer(u32 block_num)
{
	struct buffer_head *bh  = _look_up_buffer(block_num);
	if (bh)
		return bh;
	bh = alloc_new_bh(block_num);
	if (!bh)
		return NULL;
	get_bh(bh);
	/* read */
	return bh;
}

void init_buffer(u32 dev_num)
{
	struct device = get_device(dev_num);

	u32 bh_per_page = (PAGE_SIZE) / sizeof(struct buffer_head);
	struct page * bh_page ;
	struct page * bf_page;
	void* addr;
	struct buffer_head *bh;
	u32 nr_bh;
	u32 hash_idx = 0;

	list_init(&buf_hash[hash_idx].list);
	buf_hash[hash_idx].icount = 0;

	while(nr_bh < BUF_NUM)
	{
		bh_page =  kalloc_page(MEM_KERN);
		if (!bh_page)
			return 0;
		addr = phy_to_virt(bh_page->pfn);  
		bh = (struct buffer_head *) addr;
		while(nr_bh_p < bh_per_page)
		{
			list_init(bh->list);
			bh->icount = 0;
			bh->block_num = -1;
			bh->dirty = 0;
			bh->device = device;

			bf_page =  kalloc_page(MEM_KERN);
			if (!bf_page)
				return 0;
			bh->data =  phy_to_virt(bf_page->pfn);
			list_add(&bn->list, &buf_hash[free_hash_idx].list);
			bh ++;
			nr_bh_p ++;
			buf_hash[hash_idx].icount ++;
		}
		nr_bh += bh_per_page;
	}

	hash_idx ++;
	for (; hash_idx <; hash_idex ++)
	{
		buf_hash[hash_idx].icount = 0;
	}
}
