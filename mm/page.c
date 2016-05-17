#include"page.h"
#include"print.h"
#include"exception.h"
#include"mem.h"
typedef u32 pde_t;
typedef u32 pte_t;
/* used the static page to the */
static u8 page_dir[PAGE_SIZE];

void page_fault(u32 fault_addr)
{
	/*
	asm volatile ("movl %%cr2 , %%eax"
		      :"=a"(addr)::);
	*/
	print_str("page fault\n");
	print_int(fault_addr);
	while(1);
}

/* map one page */
/* 1. need to check write/read flags 
	 2. need to do privil check. used will not map into kernel space 
*/

/* vpfn : virtaddress pfn
 * ppfn :  current alloc pfn
 * flags : page table attr
 * pdt: virt address of page table
 */

static u32 map_page(u32 vpfn, u32 ppfn, u32 flags, void *pdt)
{
	/* should be 4K align */
	u32 virt_addr = pfn_to_addr(vpfn);
	u32 pde_idx = INDEX(virt_addr, PDE_LEVEL); 
	u32 pte_idx;
	pde_t *pde;
	pte_t *pte;
	pte_t *pt;

	pde = (pde_t *) pdt + pde_idx;
	/* no 4M page is support */
	if ( *pde & PGD_P )
	 pt = phy_to_virt(PT_ADDR(*pde));
	else 
	{
		struct page *page = kalloc_page(MEM_KERN);
		/* not sure of the flags used here */
		*pde = pfn_to_addr(page->pfn) | flags | PDE_PRESENT;
		pt = phy_to_virt(PT_ADDR(*pde));
	}

	pte_idx = INDEX(virt_addr, PTE_LEVEL);
	pte = pt + pte_idx;
	if ( *pte & PGT_P)
	{
		/* we del with flags here */
	}
	else
	*pte = pfn_to_addr(ppfn) | flags | PGD_P;
	return 0;
}

/*  copy only kernel part of the pgd */
static void *copy_kernel_pdt(void *_pdt, void *pdt)
{
	struct page *page;
	pde_t *_pde;
	pde_t *pde;

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
		addr +=  1 << PAGE_SHIFT(PDE_LEVEL);
		pde_idx = INDEX(addr, PDE_LEVEL);
	}
	return pdt;
}

/* return virt addr of pg table */
void* alloc_page_table()
{
	struct page *page= kalloc_page(MEM_KERN);
	u32 phy_addr = pfn_to_addr(page->pfn);

	copy_kernel_pdt((void *)phy_to_virt(phy_addr), &init_page_dir);
	return (void *)phy_to_virt(phy_addr);
}
