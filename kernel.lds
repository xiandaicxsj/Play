ENTRY(LOAD_ADDR)
SECTIONS
{
	. = KERNEL_BASE_ADDR + LOAD_ADDR;
	.text: AT(_pa(ADDR(.text)))
        {
		*(.text)
	}
	.data: AT(_pa(ADDR(.data)))
	{
		*(.data)
	}
	.bss: AT(_pa(ADDR(.bss)))
	_bss_start = .;
	{
		*(.bss)
	}
	_bss_end = .;
}
