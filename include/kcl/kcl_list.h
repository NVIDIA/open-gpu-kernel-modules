/* SPDX-License-Identifier: GPL-2.0 */
#ifndef AMDKCL_LIST_H
#define AMDKCL_LIST_H

#include <linux/list.h>

/* Copied from include/linux/list.h */
#if !defined(HAVE_LIST_BULK_MOVE_TAIL)
static inline void list_bulk_move_tail(struct list_head *head,
				       struct list_head *first,
				       struct list_head *last)
{
	first->prev->next = last->next;
	last->next->prev = first->prev;

	head->prev->next = first;
	first->prev = head->prev;

	last->next = head;
	head->prev = last;
}
#endif

#if !defined(HAVE_LIST_ROTATE_TO_FRONT)
static inline void list_rotate_to_front(struct list_head *list,
					struct list_head *head)
{
	list_move_tail(head, list);
}
#endif

#if !defined(HAVE_LIST_IS_FIRST)
static inline int list_is_first(const struct list_head *list,
					const struct list_head *head)
{
	return list->prev == head;
}
#endif

#endif /*AMDKCL_LIST_H*/
