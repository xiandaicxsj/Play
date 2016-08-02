/* use for blokc cache */ 
/* block cache is 4k */
#include"buffer.h"
#include"page.h"
#include"mem.h"
#include"blk.h"
#include"test.h"
#define HASH_SIZE 100 /* we use block num to hash this */
#define BUF_HASH(size) ((size %  HASH_SIZE) + 1) /* 0 is used for free buf */
struct buffer_head buf_hash[HASH_SIZE];

static struct buffer_head *alloc_new_bh(u32 block_num)
{
	u32 free_hash_idx = 0;
	struct buffer_head *bh = NULL;
	struct list_head *list;
	if(!buf_hash[free_hash_idx].icount)
		return bh;

	/* this need to be locked */
	list = buf_hash[free_hash_idx].list.next;
	bh = container_of(list, struct buffer_head, list);
	buf_hash[free_hash_idx].icount --;

	list_del(list);
	list_add(list, &buf_hash[BUF_HASH(block_num)].list);
	buf_hash[BUF_HASH(block_num)].icount ++;

	bh->dirty = 0;
	bh->block_num = block_num;
	bh->icount = 0;/* who refer to this */
	bh->locked = 0;

	return bh;
}

/* when some bh is needed, we need to free some bh */
static u32 release_bh()
{
	u32 hash_idx = 0;
	if (buf_hash[hash_idx].icount)
		return -1;
	for(; hash_idx < HASH_SIZE; hash_idx ++);

	/* how to release the buffer */
	return 0;
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
	return 0;
}

void get_bh(struct buffer_head *bh)
{
	struct device *device = bh->device;
	bh->locked = 1;
	/* lock */
	switch (DEV_MAJ(device->dev_num))
	{
		case DEV_BLK:
			blk_read(device, bh);
			break;
		default :
			break;
	}
	bh->locked = 0;
}

void put_bh(struct buffer_head *bh)
{
	struct device *device = bh->device;

	bh->locked = 1;
	switch (DEV_MAJ(device->dev_num))
	{
		case DEV_BLK:
			blk_write(device, bh);
			break;
		default :
			break;
	}
	bh->locked = 0;
}

struct buffer_head* look_up_buffer(u32 block_num)
{
	struct buffer_head *bh  = _look_up_buffer(block_num);
	if (bh)
		return bh;
	bh = alloc_new_bh(block_num);
	if (!bh)
		return NULL;
	/* read */
	get_bh(bh);
	return bh;
}

void init_buffer(u32 dev_num)
{
	struct device *dev = get_device(dev_num);

	u32 bh_per_page = (PAGE_SIZE) / sizeof(struct buffer_head);
	struct page * bh_page ;
	struct page * bf_page;
	void* addr;
	struct buffer_head *bh;
	u32 nr_bh = 0;
	u32 bh_pp_idx = 0;
	u32 hash_idx = 0;

	list_init(&buf_hash[hash_idx].list);
	buf_hash[hash_idx].icount = 0;

	while(nr_bh < BUF_NUM)
	{
		bh_page =  kalloc_page(MEM_KERN);
		if (!bh_page)
			return ;
#ifndef TEST_FS
		addr = (void *)phy_to_virt(bh_page->pfn);  
#else
		bh = (struct buffer_head *) addr;
#endif

		bh_pp_idx = 0;
		while(bh_pp_idx < bh_per_page)
		{
			list_init(&bh->list);
			bh->icount = 0;
			bh->block_num = -1;
			bh->dirty = 0;
			bh->device = dev;

			bf_page =  kalloc_page(MEM_KERN);
			if (!bf_page)
				return ;
#ifndef TEST_FS
			bh->data =  (void *)phy_to_virt(bf_page->pfn);
#else
			bh->data = (void *)(bf_page->pfn);
#endif
			list_add(&bh->list, &buf_hash[hash_idx].list);
			bh ++;
			bh_pp_idx ++;
			buf_hash[hash_idx].icount ++;
		}
		nr_bh += bh_per_page;
	}

	hash_idx ++;
	for (; hash_idx < HASH_SIZE; hash_idx ++)
	{
		buf_hash[hash_idx].icount = 0;
		list_init(&buf_hash[hash_idx].list);
	}
}

void sync_buffer()
{
	u32 hash_idx = 0;
	struct buffer_head *bh;
	struct list_head *plist;
	for (; hash_idx < HASH_SIZE; hash_idx ++)
	{
		if (buf_hash[hash_idx].icount)
			list_for_each(&buf_hash[hash_idx].list, plist)
		{
			bh = container_of(plist, struct buffer_head, list); 
			if (bh->locked)
				continue;
			if (bh->dirty)
				put_bh(bh);
		}
	}
}

u32 lock_bh(struct buffer_head *bh)
{
	if (bh->locked)
		return -1;
	bh->locked = 1;
	return 0;
}

void set_bh_dirty(struct buffer_head *bh)
{
	bh->dirty = 1;
}
