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
#define TIMER   32
#define KEYBOARD 33
#define SLAVE   34
#define SERIAL2 35
#define SERIAL1 36
#define PARAL2  37
#define FLOPPY  38
#define PARAL1  39
#define RTC     40
#define MOUSE   44
#define COMATH  45
#define HARDDISK 46

#define HW_VC(num) hw##num
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
void stack_fault();
void general_protection();
void copr_error();

void setup_interrupt();

void disable_interrupt();
void enable_interrupt();

void send_eoi(u32 vec);
#endif
