#ifndef _H_GATE
#define _H_GATE
#include"type.h"
#define IDT_NUM 256
#define GDT_NUM 128

#define DA_32		0x4000	// 32 位段
#define DA_LIMIT_4K	0x8000	// 段界限粒度为 4K 字节
#define DA_DPL0		0x00	//DPL = 0
#define DA_DPL1		0x20	// DPL = 1
#define DA_DPL2		0x40	//DPL = 2
#define DA_DPL3		0x60	//DPL = 3

#define DA_DR		0x90	//; 存在的只读数据段类型值
#define DA_DRW		0x92	//; 存在的可读写数据段属性值
#define DA_DRWA		0x93	//; 存在的已访问可读写数据段类型值
#define DA_C		0x98	//; 存在的只执行代码段属性值
#define DA_CR		0x9A	//; 存在的可执行可读代码段属性值
#define DA_CCO		0x9C	//; 存在的只执行一致代码段属性值
#define DA_CCOR		0x9E	//; 存在的可执行可读一致代码段属性值

#define DA_DPL0		0x00	
#define DA_DPL1			0x20	//; DPL = 1
#define DA_DPL2			0x40	//; DPL = 2
#define DA_DPL3			0x60	//; DPL = 3
#define SA_RPL0		0	//; ┓
#define SA_RPL1		1	//; ┣ RPL
#define SA_RPL2		2	//; ┃
#define SA_RPL3		3	
#define DA_LDT	 	0x82	// 局部描述符表段类型值
#define DA_TaskGate	0x85	//任务门类型值
#define DA_386TSS	0x89	//可用 386 任务状态段类型值
#define DA_386CGate	0x8C	//386 调用门类型值
#define DA_386IGate	0x8E	//386 中断门类型值
#define DA_386TGate	0x8F	//386 陷阱门类型值
#define SA_TIL 		0x04
#define SA_TIG		0x00


typedef struct 
gate_dec//这里代表的是idt的处理结构
{
	u16 low_off;
	u16 base_sel;
	u8 attr;
	u8 type;
	u16 high_off;
}gate_dec; 
typedef struct seg_dec
{
	u16 limit;
	u16 base_low;
	u8  base_heigh;
	u8  type;
	u8  attr;
	u8  base_heigh1;
}seg_dec;
//seg_dec *gdt_dec;
typedef struct idt_base_dec
{
	u16 length;
	u32 base;
}idt_base_dec;
//gate_dec idt_dec[IDT_NUM];
void set_tss(int vector, void *base_addr);
void set_ldt(int vector, void *base_addr, u32 seg_limit);
void set_gdt_seg(void *gdt_base, int vector, void *base_addr, u8 _type, u32 seg_limit);
void set_idt(int vector, void (*func)(),u8 type);
seg_dec *gdt_dec;
gate_dec *idt_dec;
#endif


