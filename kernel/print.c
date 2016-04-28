#include"print.h"
#define COLUM 40
#define VBUFFER  0xb8000
static u32 r = 0;
static u32 c = 0;

static u32 cal_cur_off()
{
	if (r == 30)
	{
		r = 0;
		c = 0;
	}
	return VBUFFER + (r * 80 + c)*2;
}

static new_line()
{
	r++;
	c = 0;
}

static update_r_c()
{
	if (c + 1 >= 80)
	{
		c = 0;
		r++;
	}

	if (r >=30)
	{
	   r = 0;
	   c = 0;
	}
	else c++;
}

void print(char b)
{
	u32 offset = cal_cur_off();
        asm volatile ( "movb 0xf0, %%ah \n\t"
		       "movw %%eax, (%%edi)"
		       :: "a"(b), "D"(offset):);
	update_r_c();
}

void print_str(char *b)
{
	int i = 0;
	while(b[i] !='\0')
	{
		if (b[i] != '\n')
			print(b[i]);
		else
		  	new_line();
		i ++;
	}
}

