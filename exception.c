#include "exception.h"
#include "schdule.h"
extern int print(char *s);
extern void enable_timer();
void default_handler1()
{
	print("this is default handler\n");
}
void divide_error()
{
	print("devide_error");
}
void single_step_exception()
{
	print("single");
}
void nmi()
{
	print("nmi");
}
void breakpoint_exception()
{
	print("break");
}
void overflow()
{

}
void timer_handler()
{
	enable_timer();
	switch_task();
	//print("timer");
}
void bounds_check()
{
}
void inval_opcode()
{
}
void copr_not_available()
{
}
void double_fault()
{
}
void copr_seg_overrun()
{
}
void inval_tss()////
{

}
void segment_not_present()
{

}
void stack_exception()
{
}
void general_protection()
{
   print("general_protection");
}
void page_fault()
{
}
void copr_error()
{
	
}
