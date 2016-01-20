SECTIONS
{
	. = 0x10000;
	.text : { *(.text) }
	. = 0x80000;
	.data :{*(.data) }
	.bss : {*(.bss) }
}
