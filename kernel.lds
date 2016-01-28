ENTRY(_start)
SECTIONS
{
	. =  0x9c000;
	.text : AT(ADDR(.text) ) 
	{ *(.text) }
	.data : AT(ADDR(.data))
	{
		*(.data)
	}
	.bss : AT(ADDR(.bss))
	{
		_bss_start = .;
		*(.bss)
		_bss_end = .;
	}
}
