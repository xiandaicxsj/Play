#ifndef _H_SLAB
#define _H_SLAB
void init_slab();

struct slab_cache {
	u32 objs_free;
	u32 objs_num;
	void *objs_map;
	u8 *bit_map;
	void *free_obj
	struct page *pg;
	struct list_head list;
	struct slab_mem_cache *owner;
};

struct slab_mem_cache{

	u32 objs_size;
	u32 objs_all;
	u32 objs_used;

	struct list_head list; /* slab cacahe list */
	u32 nr_slab;
	struct slab_cache *cur_cache;
};
#endif
