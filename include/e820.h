#ifndef H_E820
#define H_E820
struct e820_table_type
{
	u32 base_addr_low;
	u32 base_addr_heigh;
	u32 length_low;
	u32 length_heigh;
	u32 type;
	struct e820_table_type *next;
};
#endif
