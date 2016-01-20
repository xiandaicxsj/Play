#include"page.h"
#include"type.h"
/* phy_addr should be 4k page_align */
void set_pte(u32 *pte, u32 phy_addr, u32 attr)
{
		*pte = phy_addr;
		*pte |= attr;
}

void set_pde(u32 *pde, u32 phy_addr, u32 attr)
{
		*pde = phy_addr;
		*pde |= attr;
}

