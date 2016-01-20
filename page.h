#ifndef _H_PAGE
#define _H_PAGE
#include"type.h"
#define CR0_PG (1ul << 31)

#define PAGE_OFFSET 12
#define KERNEL_OFFSET 0xc000000
#define KERNEL_ADDR(addr) (KERNEL_OFFSET + addr)
#define PHY_ADDR(addr) (addr - KERNEL_OFFSET)
#define PAGE_SIZE ( 1 << PAGE_OFFSET ) 
#define _SIZE(level) (1 << 12*level)
#define PAGE_MASK (PAGE_SIZE -1)
#define ALIGN(addr, size) (addr & ~(size - 1))
#define PAGE_ALIGN(addr) ALIGN(addr, PAGE_SIZE)
#define PTE_PRESENT  1 << 2
#define PDE_PRESENT  1 << 2
#define PE_SIZE 512
#define PE_MASK (PE_SIZE - 1)
#define PFN(addr) addr >> PAGE_OFFSET
#define PAGE_SHIFT(level)	( level == 1 ? 12 : 20 )
#define INDEX(addr, level) (addr >> PAGE_SHIFT(level)) && PE_MASK
#define PDE_ADDR(entry) (entry & PAGE_MASK)
#define CR3_ADDR(addr) (addr & PAGE_MASK)
void set_pte(u32 *pte, u32 phy_addr, u32 atrr);
void set_pde(u32 *pde, u32 phy_addr, u32 atrr);
struct page
{
    u32 dirty; //dirty or not
    u32 count; // link count
    u32 phy_addr;
    u32 user;// user or kernel
    struct page *next;
};

#endif



