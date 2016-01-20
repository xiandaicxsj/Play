#include"gate.h"
#include"global.h"
extern u32 _idt;
extern u32 _gdt;
void set_tss(int vector,void *base_addr)
{
	set_gdt_seg((void *)(&_gdt),vector,base_addr,0x89,104);

}
void set_ldt(int vector,void* base_addr,u32 seg_limit)
{
	set_gdt_seg((void *)(&_gdt),vector,base_addr,0x82,0xffffffff);//这里设置的是默认的这里是在GDT中设置LDT，但是如果其他的设置
}
void set_gdt_seg(void *gdt_base,int vector,void * _base_addr ,u8 _type,u32 seg_limit)
{
	unsigned long base_addr = (unsigned long)base_addr;
	gdt_dec=(seg_dec *)(gdt_base);
	gdt_dec[vector].limit=seg_limit&(0xffff);//这是确定的值
	gdt_dec[vector].base_low=base_addr&(0x0000ffff);
	gdt_dec[vector].base_heigh=(base_addr>>16)&(0x00ff);
	gdt_dec[vector].base_heigh1=(base_addr>>24)%(0xff);
	gdt_dec[vector].type=_type;
	gdt_dec[vector].attr=0x50;
	gdt_dec[vector].attr|=seg_limit>>16&0x000f;//这里处理的是limit的最高4位
	//这里没有设计到G 相关的内如
}
void set_idt(int vector,void (*func)(),u8 type)
{
		gate_dec *idt_dec=(gate_dec *)((void *)&_idt);
		u32 handler=(u32)func;
		gate_dec* sidt=&(idt_dec[vector]);
		sidt->low_off=handler&0xffff;
		sidt->high_off=(handler>>16)&0xffff;
		sidt->base_sel=KERNEL_CODE;
		sidt->type=type;
		sidt->attr=0x00;//这里设置的是后
}

