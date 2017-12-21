#include"list.h"
void list_init(struct list_head *new)
{
	new->next = new;
	new->prev = new;
}


static void _list_add(struct list_head *prev, struct list_head *new, struct list_head *next)
{
	next->prev= new;
	new->next = next;
	prev->next =new;
	new->prev = prev;

}

int list_empty(struct list_head *head)
{
	if (head->next == head)
		return 1;
	return 0;
}

void list_add(struct list_head *new, struct list_head *head)
{
	_list_add(head->prev, new, head);
}

static void _list_del(struct list_head *prev, struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

void list_del(struct list_head *entry)
{
	_list_del(entry->prev, entry->next);
}

