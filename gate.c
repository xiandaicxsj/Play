#include"gate.h"
#include"global.h"
#include"bitop.h"
static void *gdt_bit_map;
static void set_gdt_seg(struct seg_desc *gdt_desc, int vector, void * _base_addr ,u8 type, u8 attr, u32 seg_limit)
{
	/* 
	 *  struct seg_desc * gdt_desc ;
	 *  gdt_desc=(struct seg_desc *)(gdt_base);
	 */
	u32 base_addr = (u32) _base_addr;
	gdt_desc[vector].lo = (seg_limit & 0xffff) |
			      ((base_addr & 0xffff) << 16); 
	gdt_desc[vector].hi = ((base_addr >> 16 ) & 0xff) |
			      ((type) & 0xf) << 8 |
			      ((attr) & 0xf) << 12 |
			      ((seg_limit >> 16) & 0xf) << 16 |
			      ((attr >> 4) & 0xf) << 20 |
			      (base_addr & 0xff00000);
}

void set_idt(struct idt_desc *idt, int vector,void (*func)(), u16 type)
{
	u32 handler=(u32)func;
	idt[vector].lo = (( 16ul) << 16)  | (handler & 0xffff);
	idt[vector].hi = ((u32)func & 0xffff0000 ) | (type << 8);
}

int set_tss(int vector, void *base_addr)
{
	if (vector < 0)
		vector = find_first_avail_bit(gdt_bit_map, GDT_NUM);

	set_gdt_seg((void *)(&gdt), vector, base_addr, 0x89, 0x104, 0xffff);

	if (!gdt_bit_map)
		gdt_bit_map = kmalloc(8, );
	set_bit(gdt_bit_map, vector);

	return vector;
}
/* when gdt is not used */
static void _clear_gdt_seg(struct seg_desc *gdt_desc, int vector)
{

}

void clear_gdt_seg(struct seg_desc *gdt_desc, int vector)
{
		_clear_gdt_seg(gdt_desc, vector);
		clear_bit(gdt_bit_map, vector);
}

int set_ldt(int vector,void* base_addr,u32 seg_limit)
{
	if (vector < 0)
		vector = find_first_avail_bit(gdt_bit_map, GDT_NUM);
	set_gdt_seg((void *)(&gdt), vector, base_addr, 0x82, 0x104, 0xffffffff);//这里设置的是默认的这里是在GDT中设置LDT，但是如果其他的设置
	set_bit(&gdt_bit_map, vector);
	return 0;
}
