#include"slab.h"
#define NORMAL_MEM_CACHE_NR  11
static struct normal_mem_caches[NORMAL_MEM_CACHE_NR];
struct slab_mem_cache_head mem_caches;

static u32 obj_size[] = {
2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048
};

static u32 order_size[] = {
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

static u32 obj_size_to_idx(int size)
{
	return log(i)-1;
}

static u32 idx_to_obj(int idx)
{
	return obj_size[idx];
}

static u32 idx_to_order(int idx)
{
	return order_size[idx];
}

static int init_slab_mem_cache(struct slab_mem_cache *smc, u32 obj_size)
{
	if (!smc)
		return -1;

	memset(smc, 0, sizeof(*smc));
	smc->obj_size = obj_size;
	/* FIXME as the order should be caculated by obj_size */
	smc->order = idx_to_oder(idx);

	list_init(&smc->smc_list);
	list_init(&smc->page_list);
	list_init(&smc->list);
	list_add(&mem_caches->mem_cache_list, &smc->list);
	return 0;
}

static u32 slab_align(u32 offset)
{
	return offset;
}

static int init_slab_area(struct slab_mem_cache *smc, struct slab_cache *sc)
{
	u32 remain;
	u32 offset;
	u32 order;

	order = smc->order;
	/* the offset align is wrong, the alignment should be based on addr + offset */
	remain = PAGE_SIZE;
	offset = 0;

	remain -= sizeof(*sc);
	offset += sizeof(*sc);
	
	sc->bit_map = (u8 *)sc + offset;
	sc->bit_map_size = bit_map_needed[order];

	offset += bit_map_needed[order];
	offset = slab_align(offset);
	
	remain -= offset ;
	remain += sizeof(*sc);

	sc->objs_num = sc->objs_free = (remain / smc->objs_size);
	sc->objs_map = (u8 *)sc + offset;
	sc->ower = smc;

	list_init(&sc->list);
	return 0;
}

static int alloc_slab(struct slab_mem_cache *smc)
{
	struct page *pg;
	struct slab_cache *sc;

	if (smc->obj_size != smc->obj_used)
		return 0;
	
	pg = kalloc_page(MEM_KERN);
	if (!pg)
		return -1;

	sc = (struct slab *) phy_to_virt(pfn_to_addr(pg->pfn));
	memset(sc, 0, sizeof(*sc));
	sc->pg = pg;

	init_slab_area(smc, sc);


	list_add(&smc->sc_list, &sc->list);
	list_add(&smc->page_list, &page->list);
	smc->objs_all += sc->objs_num;
	smc->nr_slab ++;
	smc->cur_cache = sc;

	return 0;
}

static int update_slab_cache(struct slab_mem_cache *smc)
{
	struct list_head *head = &smc->sc_list;
	struct list_head *pos = NULL;

	struct slab_cache *sc;
	struct slab_cache *sc_update = NULL;

	u32 objs_free_max = 0;
	
	list_for_each(head, pos) {
  		sc = container_of(pos, struct slab_cache, list);
		if (sc->objs_free > objs_free_max) {
			sc_update = sc;
			objs_free_max = sc->objs_free;
		}
        }

	if (!sc_update) {
		smc->cur_cache = NULL;
		return -1;
	}

	smc->cur_cache = sc_update;

	return 0;
}

static void *alloc_from_slab(struct slab_cache *sc)
{
	u32 obj_idx = find_first_avail_bit(sc->bit_map, sc->bit_map_size);

	set_bit(sc->bit_map, obj_idx);

	sc->objs_free --;
	sc->ower->used ++;
	return sc->objs_map + (obj_idx * sc->ower->objs_size);
}

/* we may create specific mem cache for other */
static void *kmalloc_from_slab(struct slab_mem_cache *smc)
{
	if (!smc)
		return NULL;

	if (!sc->nr_slab || (sc->objs_all == sc->objs_used))
		alloc_slab(smc);

	sc = smc->cur_cache;
	if (!sc)
		return NULL;

	if (!sc->objs_free) {

		if(update_slab_cache(smc) < 0)
			return NULL;

		sc = smc->cur_cache;
	}

	return alloc_from_slab(sc);
}

/* this is used only for common kmalloc */
void *kmalloc_from_normal_slab(u32 size, u32 align)
{
	struct slab_mem_cache *smc;
	u32 order;

	order = 1;
	while(size > (1 << order))
		order ++;
	/* order is what we want */
	smc = normal_mem_caches[order];

	return kmalloc_from_slab(smc);
}

static int free_slab_cache(struct slab_cache *sc)
{
	struct slab_mem_cache *smc;
	struct page *sc->pg;

	smc = sc->owner;
	if (sc->objs_free != sc->objs_num)
		return -1;

	if((smc->objs_all - smc->objs_used) - sc->objs_num < sc->objs_num)
		return 0;

	list_del(&sc->list);
	list_del(&sc->pg->list);

	smc->objs_all -= sc->objs_num;
	smc->nr_slab --;
	if (smc->cur_cache == sc)
		update_slab_cache(smc)

	free_page(pg);
	return 0;
}

static int free_from_slab(struct slab_cache *sc, void *addr) {

	u32 idx ;

	if (!sc)
		return -1;

	idx = (addr - sc->objs_map)/obj_size;
	sc->objs_free ++;
	sc->ower->used --;
	clear_bit(sc->bit_map, obj_idx);

	if (sc->objs_free == sc->objs_num)
		free_slab_cache(sc);
		/* we don need to free the slab page */
	return 0;
}

static int free_addr_match_slab(struct slab_cache *sc, void *addr)
{
	/* current on slab one page */
	if ((u32)addr > (u32) sc &&  (u32)addr < ((u32) sc + PAGE_SIZE));
		return;
}

static void try_free_from(struct slab_mem_cache *smc, void *addr)
{
	struct list_head *head = &smc.sc_list;
	struct list_head *pos = NULL;
	struct slab_cache *sc;

	list_for_each(head, pos) {
  		sc = container_of(pos, struct slab_cache, list);
		if (free_addr_match_slab(sc, addr))
			return free_from_slab(sc, addr);
	}

	return -1;
}

int free_from_mem_cache(void *addr)
{
	struct list_head *head = &mem_cachesu.mem_cache_list;
	struct list_head *pos = NULL;

	struct slab_mem_cache *smc;

	list_for_each(head, pos) {
  		smc = container_of(pos, struct slab_mem_cache, list);
		if (!try_free_from(smc, addr))
			return 0;
	}
	/* how to determine the addr belong to */
	return - 1;
}

void int init_normal_mem_cache()
{
	int idx = 0;
	int ret = 0;

	for(idx; idx< NORMAL_MEM_CACHE_NR; idx++) {

		struct slab_mem_cache *smc = normal_mem_caches[idx];

		ret = init_slab_mem_cache(smc, idx_to_obj(idx));
		if (ret < 0)
			return ret;
	}
}

void create_mem_cache(char *name, u32 obj_size) {
	/* one question how mem_cache struct should be a cache two ...*/

}

void init_mem_cache()
{
	list_init(&mem_caches.mem_cache_list);
	init_normal_mem_cache();
}
