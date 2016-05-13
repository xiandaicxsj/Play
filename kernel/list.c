#include"list.h"
void init_list(struct list_head *new)
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

