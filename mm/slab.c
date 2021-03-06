#include"slab.h"
#include"math.h"
#include"page.h"
#include"mem.h"
#include"bitop.h"
#include"string.h"
#ifdef TEST_SLAB
#include<string.h>
#include<stdio.h>
#endif
#define NORMAL_MEM_CACHE_NR  11
static struct slab_mem_cache *normal_mem_caches[NORMAL_MEM_CACHE_NR];
static struct slab_mem_cache_head kmem_caches_head;

static u32 obj_size[] = {
2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048
};

static u32 order_size[] = {
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

static u32 obj_size_to_idx(int size)
{
	return _log(size)-1;
}

static inline u32 idx_to_size(int idx)
{
	return obj_size[idx];
}

static u32 idx_to_order(int idx)
{
	return order_size[idx];
}
#ifdef TEST_SLAB
int dump_objs(struct slab_cache *sc)
{
	struct list_head *head = &sc->free_objs_list;
	struct list_head *pos = head;

	int count = 0;

	do {
		count ++;
		pos = pos->next;
	} while(pos != head);

	printf("\t\tsc: free_objs_list contain %d\n", count - 1);

}

int dump_sc(struct slab_cache *sc)
{
	printf("\t\t##########################\n");
	printf("\t\tsc: addr %d\n", (u32)sc);
	printf("\t\tsc: objs_free %d\n", sc->objs_free);
	printf("\t\tsc: objs_num %d\n", sc->objs_num);
	printf("\t\tsc: base addr %d\n", sc->objs_base_addr);
	dump_objs(sc);
	printf("\t\t##########################\n");
}

int dump_smc(struct slab_mem_cache *smc)
{
	printf("-------------------------\n");
	printf("\tsmc: obj_size %d\n", smc->obj_size);
	printf("\tsmc: objs_all %d\n", smc->objs_all);
	printf("\tsmc: objs_used %d\n", smc->objs_used);
	printf("\tsmc: nr_slab %d\n", smc->nr_slab);
	printf("\tsmc: cur_cache %d\n", smc->cur_cache);

	struct list_head *head = &smc->sc_list;
	struct list_head *pos = NULL;
	struct slab_cache *sc;

	list_for_each(head, pos) {
		sc = container_of(pos, struct slab_cache, list);
		dump_sc(sc);
	}
	printf("-------------------------\n");
}

int dump_smc_all()
{
	struct list_head *head = &kmem_caches_head.mem_cache_list;
	struct list_head *pos = NULL;

	struct slab_mem_cache *smc;

	list_for_each(head, pos) {
		printf("New smc lab\n");
		smc = container_of(pos, struct slab_mem_cache, list);
		dump_smc(smc);
		printf("END smc lab\n");
	}
	
}
#endif

static int init_slab_mem_cache(struct slab_mem_cache *smc, u32 obj_size)
{
	if (!smc)
		return -1;

	memset(smc, 0, sizeof(*smc));
	smc->obj_size = obj_size;

	list_init(&smc->sc_list);
	list_init(&smc->page_list);
	list_init(&smc->list);
	list_add(&smc->list, &kmem_caches_head.mem_cache_list);
	return 0;
}
static u32 slab_align(u32 offset)
{
	return offset;
}

static void init_free_objs(struct slab_cache *sc)
{
	struct list_head *head;
	int idx = sc->objs_num - 1;
	u32 obj_size = sc->owner->obj_size;

	while(idx >= 0) {

		head = (struct list_head *)(sc->objs_base_addr + idx * obj_size);
		list_init(head);
		list_add(head, &sc->free_objs_list);
		idx --;
	}

	return;
}

static int init_slab_area(struct slab_mem_cache *smc, struct slab_cache *sc)
{
	u32 remain;

	/* the offset align is wrong, the alignment should be based on addr + offset */
	remain = PAGE_SIZE;
	remain -= sizeof(*sc);
	/* this seems wrong */
	sc->objs_num = sc->objs_free = (remain / smc->obj_size);
	sc->owner = smc;
	sc->objs_base_addr = (void *)sc + sizeof(*sc);
	list_init(&sc->free_objs_list);
	list_init(&sc->list);
	init_free_objs(sc);
	return 0;
}

static int alloc_slab_cache(struct slab_mem_cache *smc)
{
	struct page *pg;
	struct slab_cache *sc;

	if (smc->objs_all != smc->objs_used)
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

	list_add(&sc->list, &smc->sc_list);
	list_add(&pg->list, &smc->page_list);
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

	/* smc may run out of objs */
	if (!sc_update)
		return alloc_slab_cache(smc);

	smc->cur_cache = sc_update;

	return 0;
}


static void *alloc_from_slab_cache(struct slab_cache *sc)
{
	struct list_head *pos;
	pos = sc->free_objs_list.next;
	list_del(pos);

	sc->objs_free --;
	sc->owner->objs_used ++;

	return (void *) pos;
}

/* align is not del in this cache */
void *kmalloc_from_mem_cache(struct slab_mem_cache *smc)
{
	struct slab_cache *sc;
	if (!smc)
		return NULL;

	if (!smc->nr_slab || (smc->objs_all == smc->objs_used))
		alloc_slab_cache(smc);

	sc = smc->cur_cache;
	if (!sc || !sc->objs_free) {
		/* if sc == NULL, may be we forget to update the cur_cache */
		/* if sc->objs_free == 0 this may need alloc slab cache */
		if(update_slab_cache(smc) < 0)
			return NULL;
		sc = smc->cur_cache;
	}

	return alloc_from_slab_cache(sc);
}


void *kmalloc_from_normal_slab(u32 size, u32 align)
{
	struct slab_mem_cache *smc;
	u32 idx;

	idx = 1;
	while(size > idx_to_size(idx))
		idx ++;
	smc = normal_mem_caches[idx];
	
	return kmalloc_from_mem_cache(smc);
}

static int free_slab_cache(struct slab_cache *sc)
{
	struct slab_mem_cache *smc;
	struct page * pg = sc->pg;

	smc = sc->owner;
	if(sc->objs_free != sc->objs_num)
		return -1;

	if(smc->objs_all - smc->objs_used < 2 * sc->objs_num)
		return 0;

	list_del(&sc->list);
	list_del(&sc->pg->list);

	smc->objs_all -= sc->objs_num;
	smc->nr_slab --;
	if (smc->cur_cache == sc)
		update_slab_cache(smc);

	kfree_page(pg);
	return 0;
}

static int free_from_slab(struct slab_cache *sc, void *addr)
{
	struct list_head *head;

	if (!sc)
		return -1;

	head = (struct list_head *) addr;

	list_init(head);
	list_add(head, &sc->free_objs_list);
	sc->objs_free ++;
	sc->owner->objs_used --;

	if (sc->objs_free == sc->objs_num)
		free_slab_cache(sc);
		/* we don need to free the slab page */
	return 0;
}

static int free_addr_match_slab(struct slab_cache *sc, void *addr)
{
	/* current on slab one page */
	if ((((u32)addr) > ((u32) sc)) && (((u32)addr) < (u32) sc + PAGE_SIZE))
		return 1;
	return 0;
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
	struct list_head *head = &kmem_caches_head.mem_cache_list;
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

static int init_normal_kmem_cache(void)
{
	int idx = 0;
	int ret = 0;

	for(idx; idx < NORMAL_MEM_CACHE_NR; idx++) {

		
		struct slab_mem_cache *smc = kmalloc_from_mem_cache(&kmem_caches_head.slab_mem_cache_struct);
		normal_mem_caches[idx] = smc;

		ret = init_slab_mem_cache(smc, idx_to_size(idx));
		if (ret < 0)
			return ret;
	}

	return ret;
}

static int init_kmem_cache_head_cache(char *name, u32 obj_size)
{
	
	list_init(&kmem_caches_head.mem_cache_list);
	if (init_slab_mem_cache(&kmem_caches_head.slab_mem_cache_struct, obj_size))
		return -1;
	return 0;
}

struct slab_mem_cache *create_kmem_cache(char *name, u32 obj_size)
{
	struct slab_mem_cache *smc = kmalloc_from_mem_cache(&kmem_caches_head.slab_mem_cache_struct);

	if (!smc)
		return NULL;

	if (init_slab_mem_cache(smc, obj_size))
		return NULL;

	return smc;
}

int init_kmem_cache(void)
{
	init_kmem_cache_head_cache("mem_cache_head", sizeof(struct slab_mem_cache));
	init_normal_kmem_cache();
}

