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

/* remain to be done 
void copy_from_bh(struct buffer_head *bh, u32 off, u32 size, void *dest)
{

}

void copy_to_bh(struct buffer_head *bh, u32 off, u32 size, void *src)
{

}
*/

void flush_bhs(void)
{
	u32 free_hash_idx ;
	struct buffer_head *bh;
	struct list_head *pos;
	struct buffer_head *cur;

	free_hash_idx = 1;
	for(; free_hash_idx < HASH_SIZE; free_hash_idx ++) {
		bh = &buf_hash[free_hash_idx];
		if (!bh->count)
			continue;
	/* search for certain */
	/* need write this part */
	/* for_each_list */
		pos = NULL;
		cur = NULL;

		list_for_each(&bh->list, pos) {
			cur = container_of(pos, struct buffer_head, list);
			if (cur->dirty) {
				put_bh(cur);
				cur->dirty = 0;
			}
		}
	}
}

/* FIXME bh->dirty */
/* May think of a good way to handle this */
static int try_free_bh(struct buffer_head *bh)
{

	int free_hash_idx = 0;
	if (bh->locked)
		return -1;

	if (bh->count > 0)
		return -1;

	if (bh->dirty)
		put_bh(bh);

	list_del(&bh->list);
	buf_hash[BUF_HASH(bh->block_num)].count --;	
	list_add(&bh->list, &buf_hash[free_hash_idx].list);
	buf_hash[free_hash_idx].count++ ;
}

/* when bh is not enough we should force
   free some bh */
static int force_free_bhs()
{
	u32 free_hash_idx ;
	struct buffer_head *bh;
	struct list_head *pos;
	struct buffer_head *cur;

	free_hash_idx = 1;
	for(;free_hash_idx < HASH_SIZE; free_hash_idx++) {
		bh = &buf_hash[free_hash_idx];
		if (!bh->count)
			continue;
	/* search for certain */
	/* need write this part */
	/* for_each_list */
		pos = NULL;
		cur = NULL;

		list_for_each(&bh->list, pos) {
			cur = container_of(pos, struct buffer_head, list);
			try_free_bh(cur);
		}
	}
}

int free_bh(struct buffer_head *bh)
{
	u32 free_hash_idx = 0;
	/* other del should be done here */
	/* need to write buffer to disk */
	/* need to check count */
	/* FIXME do we need to set this */
	if (bh->locked)
		return -1;

	/* count should be 0 */
	if (bh->count <= 0)
		return -1;

	bh->count --;

	list_del(&bh->list);
	buf_hash[BUF_HASH(bh->block_num)].count --;	
	list_add(&bh->list, &buf_hash[free_hash_idx].list);
	buf_hash[free_hash_idx].count++ ;
	return 0;
}

static struct buffer_head *alloc_bh(u32 block_num)
{
	u32 free_hash_idx = 0;
	struct buffer_head *bh = NULL;
	struct list_head *list;
	if(!buf_hash[free_hash_idx].count)
	{
		if (force_free_bhs())
			return NULL;
	}
	/* this need to be locked */
	list = buf_hash[free_hash_idx].list.next;
	bh = container_of(list, struct buffer_head, list);
	buf_hash[free_hash_idx].count --;

	list_del(list);
	list_add(list, &buf_hash[BUF_HASH(block_num)].list);
	buf_hash[BUF_HASH(block_num)].count ++;

	bh->dirty = 0;
	bh->block_num = block_num;
	bh->count = 1;/* who refer to this */
	bh->locked = 0;

	return bh;
}

/* search in hash table */
static struct buffer_head *_look_up_buffer(u32 block_num)
{
	struct buffer_head *bh = &buf_hash[BUF_HASH(block_num)];
	if (!bh->count)
		return NULL;
	/* search for certain */
	/* need write this part */
	/* for_each_list */
	struct list_head *pos;
	struct buffer_head *cur = NULL;

	list_for_each(&bh->list, pos)
	{
		cur = container_of(pos, struct buffer_head, list);
		if (cur->block_num == block_num)
			break;
	}
	if (cur)
		cur->count ++;

	return cur; /* bh may be NULL */
}

void get_bh(struct buffer_head *bh)
{
	struct device *device = bh->device;

#ifndef TEST_FS
	while(!bh->blocked)
		wait_on(&bh->wait_queue, current, TASK_UNINTERRUPT);
#endif
	bh->blocked = 1;

	/* lock */
	switch (DEV_MAJ(device->dev_num)) {
		case DEV_BLK:
			blk_read(device, bh);
			break;
		default :
			break;
	}

#ifndef TEST_FS
	bh->locked = 0;
	wake_up(&bh->wait_queue);
#endif
}

void put_bh(struct buffer_head *bh)
{
	struct device *device = bh->device;

#ifndef TEST_FS
	while(!bh->blocked)
		wait_on(&bh->wait_queue, current, TASK_UNINTERRUPT);
#endif
	bh->blocked = 1;

	switch (DEV_MAJ(device->dev_num)) {
		case DEV_BLK:
			blk_write(device, bh);
			break;
		default :
			break;
	}

#ifndef TEST_FS
	bh->locked = 0;
	wake_up(&bh->wait_queue);
#endif
}

/* we do not touch device
 * init_buffer will set bh->device
 */
struct buffer_head* look_up_buffer(u32 block_num)
{
	struct buffer_head *bh  = _look_up_buffer(block_num);
	if (bh)
		return bh;
	bh = alloc_bh(block_num);
	if (!bh)
		return NULL;
	/* read */
	get_bh(bh);
	return bh;
}

/* this buffer is used for block cache*/
void init_buffer(struct device *dev)
{
	u32 bh_per_page = (PAGE_SIZE) / sizeof(struct buffer_head);
	struct page * bh_page ;
	struct page * bf_page;
	void* addr;
	struct buffer_head *bh;
	u32 nr_bh = 0;
	u32 bh_pp_idx = 0;
	u32 hash_idx = 0;

	list_init(&buf_hash[hash_idx].list);
	buf_hash[hash_idx].count = 0;

	while(nr_bh < BUF_NUM)
	{
		bh_page =  kalloc_page(MEM_KERN);
		if (!bh_page)
			return ;
#ifndef TEST_FS
		addr = (void *)phy_to_virt(phy_to_addr(bh_page->pfn));  
#else
		bh = (struct buffer_head *) (bh_page->pfn);
#endif

		bh_pp_idx = 0;
		while(bh_pp_idx < bh_per_page)
		{
			list_init(&bh->list);
			bh->count = 0;
			bh->block_num = -1;
			bh->dirty = 0;
			/* we need to set the device */
			bh->device = dev;

			bf_page =  kalloc_page(MEM_KERN);
			if (!bf_page)
				return ;
#ifndef TEST_FS
			bh->data =  (void *)phy_to_virt(pfn_to_addr(bf_page->pfn));
#else
			bh->data = (void *)(bf_page->pfn);
#endif
			list_add(&bh->list, &buf_hash[hash_idx].list);
			bh ++;
			bh_pp_idx ++;
			buf_hash[hash_idx].count ++;
		}
		nr_bh += bh_per_page;
	}

	hash_idx ++;
	for (; hash_idx < HASH_SIZE; hash_idx ++)
	{
		buf_hash[hash_idx].count = 0;
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
		if (buf_hash[hash_idx].count)
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
	/* juest test */
#ifdef TEST_FS
	put_bh(bh);
	bh->dirty = 0;
#endif 
}
/* we need to create a kernle thread to flush the file automatic*/

