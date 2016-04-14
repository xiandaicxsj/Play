#ifndef _H_PIC
#define _H_PIC
#include"type.h"
#define PIC_MASTER 0x20
#define PIC_SLAVE 0x21
#define ICW1  0x11
#define MS_INTER_BASE 0x20
#define SL_INTER_BASE 0x28


void init_pic();
void mask_interrupt(u32 index);
#endif
