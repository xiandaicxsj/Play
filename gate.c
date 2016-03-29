#include"gate.h"
#include"global.h"
#include"bitop.h"
static void *gdt_bit_map;
static void set_gdt_seg(struct seg_desc *gdt_desc, int vector, void * _base_addr ,u32 type, u32 attr, u32 seg_limit)
{
	/* 
	 *  struct seg_desc * gdt_desc ;
	 *  gdt_desc=(struct seg_desc *)(gdt_base);
	 */
	u32 base_addr = (u32) _base_addr;
	gdt_desc[vector].lo = X86_GDT_SLL(seg_limit) | X86_GDT_BAL(base_addr) | type | attr;
	gdt_desc[vector].hi = X86_GDT_SLH(seg_limit) | X86_GDT_BAH(base_addr);
}

void set_idt(struct idt_desc *idt, int vector,void (*func)(), u16 type)
{
	u32 handler=(u32)func;
	idt[vector].lo = (( 16ul) << 16)  | (handler & 0xffff);
	idt[vector].hi = ((u32)func & 0xffff0000 ) | (type << 8);
}

int get_gdt_vector()
{
    vector = find_first_avail_bit(gdt_bit_map, GDT_NUM);
}

void set_tss(int vector, void *base_addr)
{
 set_gdt_seg((void *)(&gdt), vector, base_addr, X86_GDT_TSS | X86_GDT_DPL0 | X86_GDT_P, 0, X86_GDT_LIMIT_FULL)    ;
 return 0;
}

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

int set_ldt(int vector, void* base_addr, u32 seg_limit)
{
	set_gdt_seg((void *)(&gdt), vector, base_addr, X86_GDT_LDT | X86_GDT_DPL0 | X86_GDT_P, 0, seg_limit);//这里设
	set_bit(&gdt_bit_map, vector);
	return 0;
}
