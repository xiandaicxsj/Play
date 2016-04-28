#include"test_process.h"
void test_process()
{
	/* */
	  //enable_interrupt();
	 
	int a=0;		
	int b=0;
	int c;
	c= a+b+1;
	print('a');
#ifdef TEST_USER_INT
	asm volatile ("movw $23, %%ax\n\t"
		      "INT $48":::);
#endif
	while(1);
}

void test_process1()
{
	/* */
	  //enable_interrupt();
	 
	int a=0;		
	int b=0;
	int c;
	c= a+b+1;
	print('b');
	while(1);
}
