#include"page.h"
#include"print.h"
#include"exception.h"
#include"mem.h"
#include"debug.h"
#include"test.h"
#ifndef TEST_MEM
typedef u32 pde_t;
typedef u32 pte_t;
#define MAP_FAIL -1
#define MAP_EXIT 1
#define MAP_OK 2

u32 get_global_page_dir()
{
	return &init_page_dir;
}

void flush_tlb()
{
	asm volatile ("movl %%cr3, %%eax\n\t"
		      "movl %%eax, %%cr3":::);
}
/* force using eax to pass the parm */
 __attribute__((regparm(1))) void page_fault(u32 fault_addr)
{
	struct page *p;
	void *pgt;
	print_str("in page\n");
	while(1);

	//ASSERT(fault_addr < KERNEL_OFFSET);
	p = kalloc_page(MEM_USER);
	if (!p) 
		return ;

	pgt = (void *)current->pgt;
	if (!pgt)
		return;
	map_page(addr_to_pfn(fault_addr), p->pfn, MEM_USER, pgt);
}

int map_pages(u32 start_vpfn, u32 start_ppfn, u32 nr_pages, u32 flags, void *pgt) {
	u32 vpfn = start_vpfn;
	u32 ppfn = start_ppfn;
	u32 pidx = 0;
	int ret = 0;

	while(pidx < nr_pages) {
		ret = map_page(vpfn, ppfn, flags, pgt);
		if (ret == MAP_FAIL) {
	       		asm volatile ("jmp ."
                               ::"a" (vpfn), "b"(ppfn):);
			return ret;
		}
		vpfn ++;
		ppfn ++;
		pidx ++;
	}
	flush_tlb(pgt);
	return ret;
}

/* umap_page not done where to use it ? */
u32 umap_page(u32 vpfn, void *pdt)
{
	u32 virt_addr = pfn_to_addr(vpfn);
	u32 pde_idx = INDEX(virt_addr, PDE_LEVEL); 
	u32 pte_idx;
	pde_t *pde;
	pte_t *pte;
	pte_t *pt;

	pde = (pde_t *) pdt + pde_idx;
	/* no 4M page is support */
	if ( *pde & PGD_P )
		pt = (pte_t *)phy_to_virt(PT_ADDR(*pde));
	else
		return -1;

	pte_idx = INDEX(virt_addr, PTE_LEVEL);
	pte = pt + pte_idx;

	if ( *pte & PGT_P)
		return -1;
	else
		return -1;
}
/* vpfn : virtaddress pfn
 * ppfn :  current alloc pfn
 * flags : page table attr
 * pdt: virt address of page table
 */
/* how to detect whether the page is use as read/write */

/* flags is define mem.h */
u32 map_page(u32 vpfn, u32 ppfn, u32 flags, void *pdt)
{
	/* should be 4K align */
	u32 virt_addr = pfn_to_addr(vpfn);
	u32 pde_idx = INDEX(virt_addr, PDE_LEVEL); 
	u32 pte_idx;
	pde_t *pde;
	pte_t *pte;
	pte_t *pt;
	u32 pde_flags = 0;
	u32 pte_flags = 0;
	u32 user_flag = 0;
	u32 pfn = 0;

	if (!pdt)
		pdt = &init_page_dir;

	if (flags & MEM_USER)
		user_flag = PGT_U;
	else
		user_flag = PGT_S;

	/* pgd always writable */
	pde_flags |= PGD_W;
	if (flags & MEM_READ)
		pte_flags |= PGT_R;
	else if(flags & MEM_RDWR)
		pte_flags |= PGT_W;

	pde = ((pde_t *) pdt) + pde_idx;
	/* no 4M page is support */
	if ( *pde & PGD_P )
		pt = (pte_t *)phy_to_virt(PT_ADDR(*pde));
	else {
		pfn = kalloc_page_frame(MEM_KERN);
		/* not sure of the flags used here */
		pde_flags |= PGD_P;
		*pde = pfn_to_addr(pfn) | pde_flags;
		pt = (pte_t *)phy_to_virt(PT_ADDR(*pde));
	}

	pte_idx = INDEX(virt_addr, PTE_LEVEL);
	pte = pt + pte_idx;

	if ( *pte & PGT_P) {
		/* in this case the page is mapped */
		return MAP_EXIT;
	}
	else {
		pte_flags |= PGT_P;
		*pte = pfn_to_addr(ppfn) | pte_flags;
		flush_tlb();
		return MAP_OK;
	}
	return MAP_FAIL;
}

/*  copy only kernel part of the pgd */
static void *copy_kernel_pdt(void *_pdt, void *pdt)
{
	struct page *page;
	pde_t *_pde;
	pde_t *pde;
	pde_t t;
	pde_t _t;

	u32 addr;
	u32 pde_idx  = 0;
	
	addr = KERNEL_OFFSET;
	pde_idx = INDEX(addr, PDE_LEVEL);

	pde = (pde_t *) pdt + pde_idx;
	_pde = (pde_t *) _pdt + pde_idx; 

	while(pde_idx < PDE_OFFSET)
	{
		if ( *pde & PGD_P )
				*_pde = *pde; /* just copy is ok */
		pde = (pde_t *) pdt + pde_idx;
		_pde = (pde_t *)_pdt + pde_idx;
		//addr +=  1 << PAGE_SHIFT(PDE_LEVEL);
		pde_idx ++;
		//pde_idx = INDEX(addr, PDE_LEVEL);
	}
	return _pdt;
}

/* return virt addr of pg table */
static void* alloc_page_table()
{
	struct page *page= kalloc_page(MEM_KERN);
	u32 phy_addr = pfn_to_addr(page->pfn);
	return (void *)phy_to_virt(phy_addr);
}

void *copy_page_table(struct task_struct *p)
{
	void *new_cr3 = alloc_page_table();
	copy_kernel_pdt(new_cr3, p->pgt);
	/* we should also mask user part as non-present*/
	return new_cr3;
}
#endif
