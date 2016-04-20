#include"page.h"
#include"print.h"
void page_fault()
{
	print_str("page fault\n");
	while(1);
}
void * alloc_page_table()
{
	// the page table of kernel_page should be copy from.. but set write proteced
	//juest copy is ok;
}
