#ifndef _H_GLOBAL
#define _H_GLOBAL
#include"type.h"
//extern void *_idt;
//extern void *_gdt;
#define MF 	16
#define AC	17
#define MC 	18
#define XF 	19
#define TIMER	0x20
#define KEYBOARD 0x21
#define SLAVE	0x22
#define SERIAL2	0x23
#define SERIAL1	0x24
#define PARAL2	0x25
#define FLOPPY	0x26
#define PARAL1	0x27
#define RTC	0x28
#define MOUSE	0x2c
#define COMATH	0x2d
#define HARDDISK 0x2e
#define KERNEL_CODE 0x08
#define KERNEL_DATA 0x10
extern void *gdt;
extern u32 sys_cs;
extern u32 sys_ds;
#define TASK_VECTOR_EMP 0x4
#define TASK_VECTOR 0x5
#endif

