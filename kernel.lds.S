#define KERNEL_ADDR 0xc000000
#define LOAD_ADDR 0x9c00
ENTRY(LOAD_ADDR)
SECTIONS
{
	. = KERNEL_ADDR + LOAD_ADDR;
	.text: AT(ADDR(.text)-KERNEL_ADDR)
        {
		*(.text)
	}
	.data: AT(ADDR(.data)-KERNEL_ADDR)
	{
		*(.data)
	}
	.bss: AT(ADDR(.bss)-KERNEL_ADDR)
	_bss_start = .;
	{
		*(.bss)
	}
	_bss_end = .;
}
