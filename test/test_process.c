#include"test_process.h"
#include"print.h"
#include"irq.h"

void delay()
{
	int c;
	int i;
	int j;
		for(i=0; i<10; i++)
			for(j=0; j<10; j++)
				c = i+j;
}
void test_process(void *args)
{
	asm volatile("int $0x30":::);
	while(1);
	/* */
	  //enable_interrupt();
	 
	/*
	int a=0;		
	int b=0;
	int c=0;
	int i;
	int j=0;
	while(1)
	{	
		for(i=0; i<10; i++)
			for(j=0; j<10; j++)
				c = i+j;
		print('a');
	}
	*/
/*
#ifdef TEST_USER_INT
	asm volatile ("movw $23, %%ax\n\t"
		      "INT $48":::);
#endif
*/
}

void test_process1(void *args)
{
	/* */
	  //enable_interrupt();
	  local_irq_disable();
	 while(1);
	 
	int a=0;		
	int b=0;
	int c;
	int i,j;
	while(1)
	{
		for(i=0; i<10; i++)
			for(j=0; j<10; j++)
				c = i+j;
		print('b');
	}
}
