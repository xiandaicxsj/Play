#ifndef _H_LIST
#define _H_LIST
#include"type.h"
struct list_head
{
	struct list_head *next;
	struct list_head *prev;
};

/* double rotate list */

void list_add(struct list_head *new, struct list_head *head);
void list_init(struct list_head *new);
void list_del(struct list_head *entry);

/* member -> type */
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#define container_of(ptr, type, member) ({ \
	const typeof( ((type *)0)->member) * _mptr = (ptr); \
	(type *)( (char *)_mptr - offsetof(type, member));})
#endif
