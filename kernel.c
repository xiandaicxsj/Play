

void start_kernel(void )
{
		init_gdt();
		init_idt();
		setup_paging();
		return ;
}
