#include"timer.h"
#include"pit.h"

void init_timer()
{
	/* in this way we should choose the timer source */
	init_pit();
}
