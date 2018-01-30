#include"timer.h"
#include"pit.h"
struct timer_notifier timer_notify_list;

void register_timer_notifier(struct timer_notifier *notifier)
{
	if (!notifier)
		return;
	list_add(&notifier->list, &timer_notify_list.list);
}

static u32 get_timer_scale(void)
{
	return 1;
}

void timer_tick()
{
	u32 scale = get_timer_scale();
	struct list_head *head =  &timer_notify_list.list;
	struct list_head *pos = NULL;
	struct timer_notifier *notifier;

	list_for_each(head, pos) {
		notifier = container_of(pos, struct timer_notifier, list);
		if (!notifier || !notifier->call_back)
			continue;
		notifier->call_back(notifier->param, scale);
	}
}

void init_timer()
{
	timer_notify_list.call_back = NULL;
	timer_notify_list.param = NULL;
	list_init(&timer_notify_list.list);
	/* in this way we should choose the timer source */
	init_pit();
}
