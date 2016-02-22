#include"memory.h"
#include"tty.h"
/*#include"exception.h"
 *#include"memory.h"
 */
void start_kernel(void )
{
		setup_memory();
		init_console();
		setup_idt();
		while(1)
		{
				
		}
		return ;
}
