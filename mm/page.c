#include"page.h"
#include"print.h"
#include"exception.h"
#include"mem.h"
#include"debug.h"
#include"test.h"
typedef u32 pde_t;
typedef u32 pte_t;
/* used the static page to the */
static u8 page_dir[PAGE_SIZE];

void page_fault(void)
{
	u32 fault_addr;
	struct page *p;
	void *pgt;

	/* FIXME why mov cause error
	asm volatile ("movl %%cr2, %%eax"
		      :"=a"(fault_addr)::);
	*/

	p = kalloc_page(MEM_USER);
	if (!p)  {
		/* may need release some page */
		return ;
	}

	pgt = (void *)current->pgt;
	if (!pgt)
		return;

	map_page(addr_to_pfn(fault_addr), p->pfn, MEM_USER, pgt);
	while(1);
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
	u32 pde_flags;
	u32 pte_flags;
	u32 user_flag;

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

	pde = (pde_t *) pdt + pde_idx;
	/* no 4M page is support */
	if ( *pde & PGD_P )
		pt = (pte_t *)phy_to_virt(PT_ADDR(*pde));
	else 
	{
		struct page *page = kalloc_page(MEM_KERN);
		/* not sure of the flags used here */
		pde_flags |= PGD_P;
		*pde = pfn_to_addr(page->pfn) | pde_flags;
		pt = (pte_t *)phy_to_virt(PT_ADDR(*pde));
	}

	pte_idx = INDEX(virt_addr, PTE_LEVEL);
	pte = pt + pte_idx;

	if ( *pte & PGT_P)
	{
		/* if p is set, how could this happen ? crash */
		/* we del with flags here */ 
	}
	else
	{
		pte_flags |= PGT_P;
		*pte = pfn_to_addr(ppfn) | pte_flags;
	}
	return 0;
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
