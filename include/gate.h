#ifndef _H_GATE
#define _H_GATE
#include"type.h"
#define IDT_NUM 256
#define GDT_NUM 128

#define TI_GDT (0 << 2)
#define TI_LDT (1 << 2)
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

#define X86_DPL0		0x00	
#define X86_DPL1			0x20	//; DPL = 1
#define X86_DPL2			0x40	//; DPL = 2
#define X86_DPL3			0x60	//; DPL = 3
#define SA_RPL0		0	//; ┓
#define SA_RPL1		1	//; ┣ RPL
#define SA_RPL2		2	//; ┃
#define SA_RPL3		3	
#define DA_LDT	 	0x82	// 局部描述符表段类型值
#define DA_TaskGate	0x85	//任务门类型值
#define DA_386CGate	0x8C	//386 调用门类型值
#define DA_386IGate	0x8E	//386 中断门类型值
#define DA_386TGate	0x8F	//386 陷阱门类型值
#define SA_TIL 		0x04
#define SA_TIG		0x00
/* gdt type */
#define X86_GDT_GRA (1u << 23)
#define X86_GDT_D (1u << 22) // d = 32-bit seg
#define X86_GDT_B (0u << 22)
#define X86_GDT_BIT64 (1u << 21)
#define X86_GDT_AVL (0u << 20)
#define X86_GDT_SLH(seg_limit) (((seg_limit >> 16) & 0xf) << 16)
#define X86_GDT_SLL(seg_limit) (seg_limit & 0xffff)
#define X86_GDT_BAH(base_addr) (((base_addr >> 16) & 0xff) | (base_addr & 0xff000000))
#define X86_GDT_BAL(base_addr) ((base_addr & 0xffff) << 16)
#define X86_GDT_SYSTEM (1u <<  11)
#define X86_GDT_CD (0u <<  11)
#define X86_GDT_P (1u << 15)
/*  type */
#define X86_GDT_DATA_T (0 << 3)
#define X86_GDT_CODE_T (1 << 3)
#define X86_GDT_DA (1u << 0) 
#define X86_GDT_DW (1u << 1)
#define X86_GDT_DE (1u << 2) 
#define X86_GDT_CA (1u << 0) 
#define X86_GDT_CR (1u << 1)
#define X86_GDT_CC (1u << 2) 
#define X86_GDT_LDT (0x2 << 8)
#define X86_GDT_TSS (0x9 << 8)
#define X86_GDT_CALL_GATE ( 0xc << 8)
#define X86_GDT_DPL0 ( X86_DPL0 << 12)
#define X86_GDT_DATA ( X86_GDT_P | X86_DPL0 << 12 | (X86_GDT_DATA_T | X86_GDT_DW | X86_GDT_DA) << 8 )
#define X86_GDT_CODE ( X86_GDT_P | X86_DPL0 << 12 | (X86_GDT_CODE_T | X86_GDT_CR | X86_GDT_CA) << 8 )
#define X86_GDT_LIMIT_FULL  0xfffff

struct seg_desc {
	u32 lo;
	u32 hi;
};

struct gate_desc {
	u32 lo;
	u32 hi;
};
struct idt_desc {
        u32 lo;
        u32 hi;
};

void set_idt(struct idt_desc *idt, int vector, void (*func)(), u16 type);
int set_ldt(int vector, void* base_addr, u32 seg_limit);
void set_tss(int vector, void *base_addr);
#endif



