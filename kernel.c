#include"memory.h"
#include"tty.h"
#include"exception.h"
#include"schdule.h"
void start_kernel(void )
{
		setup_memory();
                setup_idt();
		init_console();
		while(1);
		return ;
}
