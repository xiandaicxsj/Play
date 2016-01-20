#include<stdio.h>
extern void print(char *a);

void test_print()
{
	print("aab");
}
int main()
{
		test_print();
}


