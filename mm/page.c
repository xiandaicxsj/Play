#include"page.h"
#include"print.h"
#include"exception.h"
#include"mem.h"
void page_fault()
{
	disable_interrupt();
	print_str("page fault\n");
	while(1);
}
void * alloc_page_table()
{
	// the page table of kernel_page should be copy from.. but set write proteced
	//juest copy is ok;
}
void * copy_page_table(void *old_page_table)
{
}
