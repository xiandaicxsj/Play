#include"slab.h"
#include"math.h"
#include"page.h"
#include"mem.h"
#include"bitop.h"
#define TEST_SLAB
#define SLAB_CACHE_NR  11
#ifdef TEST_SLAB
#include<string.h>
#endif
static struct slab_mem_cache mem_caches[SLAB_CACHE_NR];

static u32 obj_size[] = {
2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048
};

static u32 obj_size_to_idx(int size)
{
	return _log(size)-1;
}

static u32 idx_to_obj_size(int idx)
{
	return obj_size[idx];
}

static int init_per_slab(u32 idx)
{
	struct slab_mem_cache *smc;

	if(idx >= SLAB_CACHE_NR)
		return -1;
	smc = &mem_caches[idx];
	memset(smc, 0, sizeof(*smc));
	smc->objs_size = idx_to_obj_size(idx);
	list_init(&smc->list);
	return 0;
}

static u32 slab_align(u32 offset)
{
	return offset;
}

static int init_slab_area(struct slab_mem_cache *smc, struct slab_cache *sc, u32 order)
{
	u32 remain;
	u32 offset;

	/* the offset align is wrong, the alignment should be based on addr + offset */
	remain = PAGE_SIZE;
	offset = 0;

	remain -= sizeof(*sc);
	offset += sizeof(*sc);
	
	sc->bit_map = (u8 *)sc + offset;
	/* FIXME
	sc->bit_map_size = bit_map_needed[order];

	offset += bit_map_needed[order];
	*/
	offset = slab_align(offset);
	
	remain -= offset ;
	remain += sizeof(*sc);

	sc->objs_num = sc->objs_free = (remain / smc->objs_size);
	sc->objs_map = (u8 *)sc + offset;
	sc->owner = smc;

	list_init(&sc->list);
	return 0;
}

static int alloc_slab(struct slab_mem_cache *smc, u32 order)
{
	struct page *pg;
	struct slab_cache *sc;

	if (smc->objs_size != smc->objs_used)
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

	init_slab_area(smc, sc, order);

	list_add(&smc->list, &sc->list);
	smc->objs_all += sc->objs_num;
	smc->nr_slab ++;
	smc->cur_cache = sc;

	return 0;
}

static void update_slab_cache(struct slab_mem_cache *smc)
{
	return;
}

static void *alloc_from_slab(struct slab_cache *sc)
{
	u32 obj_idx = find_first_avail_bit(sc->bit_map, sc->bit_map_size);

	set_bit(sc->bit_map, obj_idx);

	sc->objs_free --;
	sc->owner->objs_used ++;
	return sc->objs_map + (obj_idx * sc->owner->objs_size);
}

/* align is not del in this cache */
void *_kmalloc_from_slab(u32 size, u32 align) {
	struct slab_mem_cache *smc;
	struct slab_cache *sc;
	u32 order;

	order = 1;
	while(size > (1 << order))
		order ++;
	/* order is what we want */
	smc = &mem_caches[order];

	if (!smc->nr_slab || (smc->objs_all == smc->objs_used))
		alloc_slab(smc, order);
	sc = smc->cur_cache;
	if (!sc)
		return NULL;

	if (!sc->objs_free) {
		update_slab_cache(smc);
		sc = smc->cur_cache;
	}

	return alloc_from_slab(sc);
}

int _free_from_slab(void *addr)
{
	/* how to determine the addr belong to */
	return 0;
}

void init_slab(void)
{
	u32 idx = 0;
	int ret = 0;
	for(idx; idx< SLAB_CACHE_NR; idx++) {
		ret = init_per_slab(idx);
		if (ret < 0)
			return;
	}
}

