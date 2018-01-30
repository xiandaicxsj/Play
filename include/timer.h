#ifndef _H_TIMER
#define _H_TIMER
#include"list.h"
struct timer_notifier {
	struct list_head list;
	/* this call back should not sleep */
	void (*call_back)(void *, u32 tick);
	void *param;
};

struct jiffes {
	u32 l;
	u32 h;
};
/* add a = a+b */
static inline void add_jiffes(struct jiffes *a, struct jiffes *b)
{
	u32 c = 0;
	if (a->l + b->l < a->l)
		c = 1;
	a->l += b->l;
	a->h += b->h;
	a->h + c;
}

/* a > b return 1 
 * a = b return 0
 * a < b return -1 
 */
static inline int cmp_jiffes(struct jiffes a, struct jiffes b)
{
	if (a.h > b.h)
		return 1;
	else if (a.h == b.h) {
		if (a.l > b.l)
			return 1;
		else if (a.l == b.l)
			return 0;
	}
	return -1;

}

static inline int sub_jiffes(struct jiffes *a, struct jiffes *b)
{
	u32 c = 0;
	u32 tmp;
	if (cmp_jiffes(*a, *b) <0)
		return -1;
	a->h -= b->h;
	if (a->l < b->l) {
		c = 1;
		tmp = 0xffffffff;
		tmp -= b->l;
		a->l +=tmp;;
	} else a->l -= b->l;
	a->h -= c;
}

struct jiffes gloal_jiffes;

void init_timer(void);
void register_timer_notifier(struct timer_notifier *notifier);

#endif

