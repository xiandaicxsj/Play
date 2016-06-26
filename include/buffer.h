#ifndef _H_BUF
#define _H_BUF
#include"type.h"
#include"list.h"
#include"device.h"
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
	int locked;
	struct list_head wait_queue; /* task wait for this */
};
void init_buffer(u32 dev_num);
struct buffer_head* look_up_buffer(u32 block_num);
void put_bh(struct buffer_head *bh);
void get_bh(struct buffer_head *bh);
u32 free_buffer(struct buffer_head *bh);
void set_bh_dirty(struct buffer_head *bh);
#endif
