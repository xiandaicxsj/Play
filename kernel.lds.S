ENTRY(_start)
SECTIONS
{
	. = 0xc000000 + 0x9c00;
	.text : AT(ADDR(.text) - 0xc000000) 
	{ *(.text) }
	.data : AT(ADDR(.data) - 0xc000000)
	{
		*(.data)
	}
	.bss : AT(ADDR(.bss) - 0xc000000)
	{
		_bss_start = .;
		*(.bss)
		_bss_end = .;
	}
}
