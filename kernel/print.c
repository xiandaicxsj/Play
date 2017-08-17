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
	return VBUFFER + (r * 80 + c ) * 2 + 0xc0000000;
}

static void new_line()
{
	r++;
	c = 0;
}

static void update_r_c()
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
	u8 *buffer = (u8 *)offset;
	*buffer = b;
	buffer ++;
	*buffer = 0x7;
	
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

void reverse_str(char *str_buf, u32 buf_idx)
{
	char tmp_str[32];
	int i=0;
	for (i = 0; i < buf_idx; i++)
	{
		tmp_str[i] = str_buf[buf_idx - 1 - i];
	}
	for (i = 0; i < buf_idx; i++)
	str_buf[i] = tmp_str[i];
	str_buf[i] = '\0';
}

void print_int(u32 num)
{
	char str_buf[32];
	int buf_idx = 0;
	u32 tmp = num/16;
	while(tmp)
	{
		if (tmp >= 10)
			str_buf[buf_idx] = (tmp - 10)+'a'; 
		else
			str_buf[buf_idx] = tmp + '0';

		num = num %16;
		tmp = num/16;
		buf_idx ++;
	}

	if (num >= 10)
		str_buf[buf_idx] = (num - 10)+'a'; 
	else
		str_buf[buf_idx] = num + '0';
	buf_idx ++;
	str_buf[buf_idx] = '\0';
	print_str(str_buf);
}

