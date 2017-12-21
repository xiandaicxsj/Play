#ifndef _H_PAGE
#define _H_PAGE
#include"type.h"
#include"list.h"
#include"page_type.h"
#define CR0_PG (1ull << 31)

typedef u32 addr_t;
#define KERNEL_OFFSET 0xc0000000
#define KERNEL_ADDR(addr) (KERNEL_OFFSET + (addr_t)addr)
#define PHY_ADDR(addr) ((addr_t)addr - KERNEL_OFFSET)
#define phy_to_virt(addr) (((addr_t)addr) + KERNEL_OFFSET)
#define virt_to_phy(addr) (((addr_t)addr) - KERNEL_OFFSET)
#define pfn_to_addr(pfn) ((addr_t)((pfn) << PAGE_SHIFT))
#define addr_to_pfn(addr) (((addr_t)addr) >> PAGE_SHIFT)
#define _SIZE(level) (1 << 12 * (level))
#define ALIGN(addr, size) (((addr_t)addr) & ~((size) - 1))
#define PAGE_ALIGN(addr) ALIGN((addr_t)addr, PAGE_SIZE)
#define PTE_PRESENT  (1 << 0)
#define PDE_PRESENT  (1 << 0)
#define PE_SIZE 512
#define PE_MASK (PE_SIZE - 1)
/* no use now ,should use addr_to_pfh */
#define PFN(addr) (((addr_t)addr) >> PAGE_SHIFT)
#define PDE_LEVEL (2)
#define PTE_LEVEL (1)
#define PTE_SIZE 1024
#define PDE_SIZE 1024
#define PTE_OFFSET 1024
#define PDE_OFFSET 1024
#define PGDT_SHIFT(level)	( level == 1 ? 12 : 22 )
#define INDEX(addr, level) (((((addr_t)addr) >> PGDT_SHIFT(level)) & 0x3ff))
#define PDE_ADDR(entry) ((entry) & ~PAGE_MASK)
#define PT_ADDR(entry) ((entry) & ~PAGE_MASK)
#define PG_ADDR(entry) ((entry) & ~PAGE_MASK)
#define CR3_ADDR(addr) (((addr_t)addr) & PAGE_MASK)
struct page
{
    struct list_head list;
    u32 count; // link count
    u32 pfn;
    u32 order;
};
#endif



