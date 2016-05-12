#ifndef _H_LIST
#define _H_LIST
struct list_head
{
	struct list_head *next;
	struct list_head *prev;
};

/* double rotate list */
inline void _list_add(struct list_head *prev, struct list_head *new, struct list_head *next)
{
	next->prev= new;
	new->next = next;
	prev->next =new;
	new->prev = prev;

}

inline void list_add(struct list_head *new, struct list_head *head)
{
	_list_add(head->prev, new, head);
}

inline void _list_del(struct list_head *prev, struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

inline void init_list(struct list_head *new)
{
	new->next = new;
	new->prev = new;
}

static inline void list_del(struct list_head *entry)
{
	_list_del(entry->prev, entry->next);
}

/* member -> type */
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#define container_of(ptr, type, member) ({ \
	const typeof( ((type *)0)->member) * _mptr = (ptr); \
	(type *)( (char *)_mptr - offsetof(type, member));})
#endif
