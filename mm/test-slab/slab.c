#include"slab.h"
#include"math.h"
#include"page.h"
#include"mem.h"
#include"bitop.h"
#define TEST_SLAB
#ifdef TEST_SLAB
#include<string.h>
#endif
#define NORMAL_MEM_CACHE_NR  11
static struct slab_mem_cache normal_mem_caches[NORMAL_MEM_CACHE_NR];
struct slab_mem_cache_head mem_caches;

static u32 obj_size[] = {
2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048
};

static u32 order_size[] = {
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

static u32 obj_size_to_idx(int size)
{
	return _log(size)-1;
}

static u32 idx_to_size(int idx)
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

	list_init(&smc->sc_list);
	list_init(&smc->page_list);
	list_init(&smc->list);
	list_add(&mem_caches.mem_cache_list, &smc->list);
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

	order = _log(smc->obj_size);
	/* the offset align is wrong, the alignment should be based on addr + offset */
	remain = PAGE_SIZE;
	offset = 0;

	remain -= sizeof(*sc);
	offset += sizeof(*sc);

	/* this seems wrong */
	sc->bit_map = (u8 *)sc + offset;
	sc->bit_map_size = bit_map_needed[order];

	offset += bit_map_needed[order];
	offset = slab_align(offset);
	
	remain -= offset ;
	remain += sizeof(*sc);

	sc->objs_num = sc->objs_free = (remain / smc->obj_size);
	sc->objs_map = (u8 *)sc + offset;
	sc->owner = smc;

	list_init(&sc->list);
	return 0;
}

static int alloc_slab(struct slab_mem_cache *smc)
{
	struct page *pg;
	struct slab_cache *sc;

	if (smc->obj_size != smc->objs_used)
		return 0;
	
	pg = kalloc_page(MEM_KERN);
	if (!pg)
		return -1;

#ifdef TEST_SLAB
	sc = (struct slab_cache *) pg->pfn;
#else
	sc = (struct slab_cache *) phy_to_virt(pfn_to_addr(pg->pfn));
#endif
	memset(sc, 0, sizeof(*sc));
	sc->pg = pg;

	init_slab_area(smc, sc);

	list_add(&smc->sc_list, &sc->list);
	list_add(&smc->page_list, &pg->list);
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
	sc->owner->objs_used ++;
	return sc->objs_map + (obj_idx * sc->owner->obj_size);
}

/* align is not del in this cache */
void *kmalloc_from_mem_cache(struct slab_mem_cache *smc)
{
	struct slab_cache *sc;
	if (!smc)
		return NULL;

	if (!smc->nr_slab || (smc->objs_all == smc->objs_used))
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


void *kmalloc_from_normal_slab(u32 size, u32 align)
{
	struct slab_mem_cache *smc;
	struct slab_cache *sc;
	u32 order;

	order = 1;
	while(size > (1 << order))
		order ++;
	/* order is what we want */
	smc = &normal_mem_caches[order];
	
	return kmalloc_from_mem_cache(smc);
}

static int free_slab_cache(struct slab_cache *sc)
{
	struct slab_mem_cache *smc;
	struct page * pg = sc->pg;

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
		update_slab_cache(smc);

	free_page(pg);
	return 0;
}

static int free_from_slab(struct slab_cache *sc, void *addr) {

	u32 idx ;
	u32 obj_size = sc->owner->obj_size;

	if (!sc)
		return -1;

	idx = ((u32)addr - (u32)sc->objs_map)/obj_size;
	sc->objs_free ++;
	sc->owner->objs_used --;
	clear_bit(sc->bit_map, idx);

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

static int try_free_from(struct slab_mem_cache *smc, void *addr)
{
	struct list_head *head = &smc->sc_list;
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
	struct list_head *head = &mem_caches.mem_cache_list;
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

static int init_normal_mem_cache(void)
{
	int idx = 0;
	int ret = 0;

	for(idx; idx< NORMAL_MEM_CACHE_NR; idx++) {

		struct slab_mem_cache *smc = &normal_mem_caches[idx];

		ret = init_slab_mem_cache(smc, idx_to_size(idx));
		if (ret < 0)
			return ret;
	}
}

void create_mem_cache(char *name, u32 obj_size) {
	/* one question how mem_cache struct should be a cache two ...*/

}

int init_mem_cache(void)
{
	list_init(&mem_caches.mem_cache_list);
	init_normal_mem_cache();
}

