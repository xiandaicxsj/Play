#include"memory.h"
#include"page.h"
#include"e820.h"
/* step only page32
 * 1. read e820 from bios
 * 2. init paging of the kernel
 * 3. juest del with 4G
 * now paging and memory is the some, may be split later
 */
static struct page * free_page_list; //how to get the page_addr in the list
static struct e820_table_type _e820_table;
static u32 e820_table_num;
static u64 memory_size;
u32 _kernel_end; // this can be get from the link script
u32 _kernel_start;//this should be in the kernel.lds
u32 _reserved_start;
u32 _reserved_end;// reserved_end
void * kmalloc(u32 size)
{
	return NULL;
}
static u32 caculate_memory_size()
{
	u32 i = 0 ;
	for (; i <  e820_table_num; i++)
	{
		memory_size += head[i].length_low;
	}
	return memory_size;
}
static void get_e820_table()
{
	head = (struct e820_table_type *)((void *)&_e820_table);
	e820_table_num = *((u32 *)((void *)&_e820_num));
}
static u32 get_memory_size()
{
	if ( !memory_size )
		memory_size = caculate_memory_size();
	return memory_size;
}
u32 alloc_boot_memory(u32 size)/* only used by setup page */
{
	u32 ret;

	if ( _reserved_start + size > _reserved_end)
		return -1;
	ret = _reserved_start;
	_reserved_start += size;

	return ret;
}
/* Reserve some page between  __kernel_end to free_page_begin
 * These page can be used for map pags and start up memory allocation.
 * And the last page should be the bit page used to index which is used 
 */
u32 kmap(u32 phy_addr)
{
	return phy_addr + KERNEL_OFFSET;
}
static u32 entry_present(u32 entry)
{
	return entry | PTE_PRESENT;
}
/* 4G / 4K = 1M 
 * 1M / 8 = 125 K 
 * 125 k /3 = 42 PAGE. We should use 42 pfn for bit map
 */
static void mark_pfn_free(u32 pfn)
{
	//pfn_bit_map &= ~(1U < pfn);
}
static void mark_pfn_used(u32 pfn)
{
	//pfn_bit_map |= (1U < pfn);
}
static void init_mem_bit_map(u32 nr_pages)
{
		int bit_index = 0;
		while ( bit_index < nr_pages )
		{
			// need to do some thing 
		}
}
/*
 * map _start + 0xc0000 - _end + 0xc0000 ---> _start + _end
 */
void paget_fault()
{
		
}

static void set_pte(u32 *pte, u32 phy_addr, u32 attr)
{
		*pte = phy_addr;
		*pte |= attr;
}

static void set_pde(u32 *pde, u32 phy_addr, u32 attr)
{
		*pde = phy_addr;
		*pde |= attr;
}

static void mapping_kernel(u32 _start, u32 _end)
{
	u32 start = PAGE_ALIGN(_start);	
	u32 end = PAGE_ALIGN(_end);	
	u32 level = 3;
	u32 pgd_addr = 0;
	u32 pgt_addr = 0;
	for ( ; start < end; start += _SIZE(level))
	{
		level --;
		u32 pgde_index = INDEX(KERNEL_ADDR(start), level);
		u32 *pgde =(u32 *)(KERNEL_ADDR(CR3_ADDR(pgd_addr) + pgde_index* 4));
		if ( !entry_present(*pgde))
		{
			u32 phy_addr = alloc_boot_memory(PAGE_SIZE);//这里设置为
			set_pde(pgde, phy_addr, 0);
		}
		pgt_addr = KERNEL_ADDR(PDE_ADDR(*pgde));

		u32 *pgte;
		u32 pgte_index;

		level --;
		u32 base_start = start;
		for ( ; base_start < end; base_start += _SIZE(level))//4k
		{
			pgte_index = INDEX(KERNEL_ADDR(base_start), level);
			pgte = (u32 *)(pgt_addr + pgte_index*4);
			if ( !entry_present(*pgte))
			{
				set_pte(pgte, base_start, 0);
			}
		}
	}
	return ;
}
void init_page(struct page* phy_page)
{
	phy_page -> count = 0;
	phy_page -> next = NULL;
}

u32 get_page_phy_addr(struct page *phy_page)
{
	return (u32) phy_page - KERNEL_OFFSET;
}
void insert_free_page(struct page *phy_page)
{
	init_page(phy_page);
	if( !free_page_list )
	{
		free_page_list = phy_page;
		return;
	}
	free_page_list->next = phy_page;
	free_page_list = phy_page;
}

void init_free_page(u32 nr_pages)
{
	//u32 reserves_pages = PFN(__reserved_end) - 1;
	u32 start_pfn = PFN(_reserved_end);
	free_page_list = (struct page*)KERNEL_ADDR(start_pfn >> PAGE_OFFSET);
	init_page(free_page_list);
	for (; start_pfn < nr_pages; start_pfn++) 
	  insert_free_page((struct page*)KERNEL_ADDR(start_pfn >> PAGE_OFFSET));
}
static void mark_page_used(u32 pfn)
{

}
static void mark_pages_used(u32 start_pfn, u32 nr_pages)
{
		while(nr_pages)
		{
				mark_page_used(start_pfn + nr_pages);
				nr_pages --;
		}
}
struct page *alloc_page()//current is can not alloc size > 4k, how to del with 
{
	if ( !free_page_list )
		return NULL;
	struct page* tmp = free_page_list;
	free_page_list = tmp->next;
	mark_page_used(PFN(tmp->phy_addr));
	return tmp;
}
int free_page(struct page *phy_page)
{
	if ( !phy_page )
		return -1;
	insert_free_page(phy_page);
}
static void setup_paging()
{
	u64 memory_size = get_memory_size();
	u64 nr_pages = memory_size / PAGE_SIZE;
	init_mem_bit_map(nr_pages);
	mapping_kernel (0, _reserved_end);
	mark_pages_used(0,_reserved_end);
	init_free_page(PFN(memory_size) - PFN(_reserved_end));//from _reserve_end  -- memory_end
}
void init_buddy()
{
	/* this should init buddy algorithm to memory */
}
void init_memory()
{
	get_e820_table();
	setup_paging();
	return ;
}


