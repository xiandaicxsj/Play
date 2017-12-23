#ifndef _H_SLAB
#define _H_SLAB
#include"type.h"
#include"list.h"
#define TEST_SLAB
int init_kmem_cache(void);
int free_from_mem_cache(void *addr);
struct slab_cache {
	u32 objs_free;/* free objs num */
	u32 objs_num; /* total objs num in the slab */
	void *objs_base_addr; /* the base addr of the objs */
	struct page *pg;
	struct list_head list; /* the list used to link in owner smc */
	struct slab_mem_cache *owner; /* point to mem_cache */
	struct list_head free_objs_list; /* objs in the slab is arraged into a list */
};

struct slab_mem_cache{

	u32 obj_size; /* obj_size in this slab */
	u32 objs_all; /* all objs in the slab list */
	u32 objs_used; /* all used */

	struct list_head sc_list; /* slab cacahe list */
	u32 nr_slab; /* slab num */
	struct slab_cache *cur_cache;
	struct list_head page_list;/* why do i need this */
	struct list_head list; /* this should be the list used to link all slab_mem_cache */
	char name[20]; /* name is not used now */

};

struct slab_mem_cache_head {
	struct slab_mem_cache slab_mem_cache_struct;
	u32 mem_cache_num;
	struct list_head mem_cache_list;

};
struct slab_mem_cache *create_kmem_cache(char *name, u32 obj_size);
#ifdef TEST_SLAB
int dump_smc(struct slab_mem_cache *smc);
int dump_smc_all();
#endif
#endif
