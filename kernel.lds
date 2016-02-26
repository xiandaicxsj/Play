ENTRY(_start)
SECTIONS
{
	. = 0xc0000000;
	_text_start = .;
	.text : AT(ADDR(.text) - 0xc0000000) 
	{
	 	*(.text) 
	}
	.rodata : AT(ADDR(.rodata) - 0xc0000000)
	{ *(.rodata) }
	.data : AT(ADDR(.data) - 0xc0000000)
	{
		*(.data)
	}
	.bss : AT(ADDR(.bss) - 0xc0000000)
	{
		_bss_start = .;
		*(.bss)
	}
		_bss_end = .;
}
