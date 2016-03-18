#include"gate.h"
#include"global.h"
static void set_gdt_seg(void *gdt_base, int vector, void * _base_addr ,u8 type, u8 attr, u32 seg_limit)
{
	struct seg_desc * gdt_desc;
	u32 base_addr = (u32) _base_addr;
	gdt_desc=(struct seg_desc *)(gdt_base);
	gdt_desc[vector].lo = (seg_limit & 0xffff) |
			      ((base_addr & 0xffff) << 16); 
	gdt_desc[vector].hi = ((base_addr >> 16 ) & 0xff) |
			      ((type) & 0xf) << 8 |
			      ((attr) & 0xf) << 12 |
			      ((seg_limit >> 16) & 0xf) << 16 |
			      ((attr >> 4) & 0xf) << 20 |
			      (base_addr && 0xff00000);

}

void set_idt(struct idt_desc *idt, int vector,void (*func)(), u16 type)
{
	u32 handler=(u32)func;
	idt[vector].lo = (( 16ul) << 16)  | (handler & 0xffff);
	idt[vector].hi = ((u32)func & 0xffff0000 ) | (type << 8);
}

void set_tss(int vector,void *base_addr)
{
	set_gdt_seg((void *)(&gdt), vector, base_addr, 0x89, 0x104, 0xffff);
}

void set_ldt(int vector,void* base_addr,u32 seg_limit)
{
	set_gdt_seg((void *)(&gdt), vector, base_addr, 0x82, 0x104, 0xffffffff);//这里设置的是默认的这里是在GDT中设置LDT，但是如果其他的设置
}
