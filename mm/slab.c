#include"slab.h"
#define SLAB_CACHE_NR  11
static struct slab_mem_saches[SLAB_CACHE_NR];

static u32 obj_size[] = {
2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048
}

static u32 obj_size_to_idx(int size)
{
	return log(i)-1;
}

static u32 idx_to_obj_size(int idx)
{
	return obj_size[idx];
}

static int init_per_slab(idx)
{
	struct slab_mem_sache *sc;

	if(idx >= SLAB_CHACHE_NR)
		return -1;
	sc = slab_mem_saches[idx];
	memset(sc, 0, sizeof(*sc));
	sc->obj_size = idx_to_obj_size(idx);
	list_init(&sc->list);
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

static int alloc_slab(struct slab_mem_cache *smc, u32 order)
{
	struct page *pg;
	struct slab *sc;

	if (smc->obj_size != smc->obj_used)
		return 0;
	
	pg = kmlloc_page(MEM_KERN);
	if (!pg)
		return -1;

	sc = (struct slab *) phy_to_virt(pfn_to_addr(pg->pfn));
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
	
}

static void *alloc_from_slab(struct slab_cache *sc)
{
	u32 obj_idx = find_first_avail_bit(sc->bit_map, sc->bit_map_size);

	set_bit(sc->bit_map, obj_idx);

	sc->objs_free --;
	sc->ower->used ++;
	return sc->objs_map + (obj_idx * sc->ower->objs_size);
}

/* align is not del in this cache */
void *_kmalloc_from_slab(u32 size, u32 align) {
	struct slab_mem_cache *smc;
	u32 order;

	order = 1;
	while(size > (1 << order))
		order ++;
	/* order is what we want */
	smc = slab_mem_saches[order];

	if (!sc->nr_slab || (sc->objs_all == sc->objs_used))
		alloc_slab(smc, order);
	sc = smc->cur_cache;
	if (!sc)
		return NULL;

	if (!sc->objs_free) {
		update_slab_cache(slab_mem_cache);
		sc = smc->cur_cache;
	}

	return alloc_from_slab(sc);
}

int _free_from_slab(void *addr)
{
	/* how to determine the addr belong to */
	return;
}

void int init_slab()
{
	int idx = 0;
	int ret = 0;
	for(idx; idx< SLAB_CACHE_NR; idx++) {
		ret = init_per_slab(idx);
		if (ret < 0)
			return ret;
	}
}

