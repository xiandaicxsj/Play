#ifndef _H_EXCEPTION
#define _H_EXCEPTION
#include"type.h"
#define DIEVIDE_ERROR   0
#define DEBUG  1
#define NMI 2
#define BREAKPOINT 3
#define OVERFLOW 4
#define BOUND_RANGE_EXCEDD 5
#define INVALID_OP 6
#define DEVICE_NOT_AVAL 7
#define DOUBL_FAULT 8
#define COP_SEGMENT 9
#define IVALID_TSS 10
#define SEGMENT_NOT_PRESETNT 11
#define STACK_FAULT 12
#define GENERAL_FAULT 13
#define PAGE_FAULT 14
#define TIMER   0x20
#define KEYBOARD 0x21
#define SLAVE   0x22
#define SERIAL2 0x23
#define SERIAL1 0x24
#define PARAL2  0x25
#define FLOPPY  0x26
#define PARAL1  0x27
#define RTC     0x28
#define MOUSE   0x2c
#define COMATH  0x2d
#define HARDDISK 0x2e
void timer_handler();
void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void copr_error();
void timer();
void init_idt();
void setup_idt();
#endif
