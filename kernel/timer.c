#include"timer.h"
#include"pit.h"
struct jiffes;
void register_timer_notifier();

void timer_tick()
{
}

void init_timer()
{
	/* in this way we should choose the timer source */
	init_pit();
}
