#ifndef _H_PIC
#define _H_PIC
#include"type.h"
#define PIC_MASTER 1
#define PIC_SLAVER 0

#define PIC_MASTER_CMD 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVER_CMD 0xA0
#define PIC_SLAVER_DATA 0xA1
#define MS_IRQ_BASE 0x20
#define SL_IRQ_BASE 0x28

#define PIC_EOI 0x20
#define ICW1  0x11



void init_pic();
void mask_interrupt(u32 index);
void send_eoi(u32 vector);
#endif
