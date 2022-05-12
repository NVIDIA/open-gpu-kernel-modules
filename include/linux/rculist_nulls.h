/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_RCULIST_NULLS_H
#define _LINUX_RCULIST_NULLS_H

#ifdef __KERNEL__

/*
 * RCU-protected list version
 */
#include <linux/list_nulls.h>
#include <linux/rcupdate.h>

/**
 * hlist_nulls_del_init_rcu - deletes entry from hash list with re-initialization
 * @n: the element to delete from the hash list.
 *
 * Note: hlist_nulls_unhashed() on the node return true after this. It is
 * useful for RCU based read lockfree traversal if the writer side
 * must know if the list entry is still hashed or already unhashed.
 *
 * In particular, it means that we can not poison the forward pointers
 * that may still be used for walking the hash list and we can only
 * zero the pprev pointer so list_unhashed() will return true after
 * this.
 *
 * The caller must take whatever precautions are necessary (such as
 * holding appropriate locks) to avoid racing with another
 * list-mutation primitive, such as hlist_nulls_add_head_rcu() or
 * hlist_nulls_del_rcu(), running on this same list.  However, it is
 * perfectly legal to run concurrently with the _rcu list-traversal
 * primitives, such as hlist_nulls_for_each_entry_rcu().
 */
static inline void hlist_nulls_del_init_rcu(struct hlist_nulls_node *n)
{
	if (!hlist_nulls_unhashed(n)) {
		__hlist_nulls_del(n);
		WRITE_ONCE(n->pprev, NULL);
	}
}

/**
 * hlist_nulls_first_rcu - returns the first element of the hash list.
 * @head: the head of the list.
 */
#define hlist_nulls_first_rcu(head) \
	(*((struct hlist_nulls_node __rcu __force **)&(head)->first))

/**
 * hlist_nulls_next_rcu - returns the element of the list after @node.
 * @node: element of the list.
 */
#define hlist_nulls_next_rcu(node) \
	(*((struct hlist_nulls_node __rcu __force **)&(node)->next))

/**
 * hlist_nulls_del_rcu - deletes entry from hash list without re-initialization
 * @n: the element to delete from the hash list.
 *
 * Note: hlist_nulls_unhashed() on entry does not return true after this,
 * the entry is in an undefined state. It is useful for RCU based
 * lockfree traversal.
 *
 * In particular, it means that we can not poison the forward
 * pointers that may still be used for walking the hash list.
 *
 * The caller must take whatever precautions are necessary
 * (such as holding appropriate locks) to avoid racing
 * with another list-mutation primitive, such as hlist_nulls_add_head_rcu()
 * or hlist_nulls_del_rcu(), running on this same list.
 * However, it is perfectly legal to run concurrently with
 * the _rcu list-traversal primitives, such as
 * hlist_nulls_for_each_entry().
 */
static inline void hlist_nulls_del_rcu(struct hlist_nulls_node *n)
{
	__hlist_nulls_del(n);
	WRITE_ONCE(n->pprev, LIST_POISON2);
}

/**
 * hlist_nulls_add_head_rcu
 * @n: the element to add to the hash list.
 * @h: the list to add to.
 *
 * Description:
 * Adds the specified element to the specified hlist_nulls,
 * while permitting racing traversals.
 *
 * The caller must take whatever precautions are necessary
 * (such as holding appropriate locks) to avoid racing
 * with another list-mutation primitive, such as hlist_nulls_add_head_rcu()
 * or hlist_nulls_del_rcu(), running on this same list.
 * However, it is perfectly legal to run concurrently with
 * the _rcu list-traversal primitives, such as
 * hlist_nulls_for_each_entry_rcu(), used to prevent memory-consistency
 * problems on Alpha CPUs.  Regardless of the type of CPU, the
 * list-traversal primitive must be guarded by rcu_read_lock().
 */
static inline void hlist_nulls_add_head_rcu(struct hlist_nulls_node *n,
					struct hlist_nulls_head *h)
{
	struct hlist_nulls_node *first = h->first;

	n->next = first;
	WRITE_ONCE(n->pprev, &h->first);
	rcu_assign_pointer(hlist_nulls_first_rcu(h), n);
	if (!is_a_nulls(first))
		WRITE_ONCE(first->pprev, &n->next);
}

/**
 * hlist_nulls_add_tail_rcu
 * @n: the element to add to the hash list.
 * @h: the list to add to.
 *
 * Description:
 * Adds the specified element to the specified hlist_nulls,
 * while permitting racing traversals.
 *
 * The caller must take whatever precautions are necessary
 * (such as holding appropriate locks) to avoid racing
 * with another list-mutation primitive, such as hlist_nulls_add_head_rcu()
 * or hlist_nulls_del_rcu(), running on this same list.
 * However, it is perfectly legal to run concurrently with
 * the _rcu list-traversal primitives, such as
 * hlist_nulls_for_each_entry_rcu(), used to prevent memory-consistency
 * problems on Alpha CPUs.  Regardless of the type of CPU, the
 * list-traversal primitive must be guarded by rcu_read_lock().
 */
static inline void hlist_nulls_add_tail_rcu(struct hlist_nulls_node *n,
					    struct hlist_nulls_head *h)
{
	struct hlist_nulls_node *i, *last = NULL;

	/* Note: write side code, so rcu accessors are not needed. */
	for (i = h->first; !is_a_nulls(i); i = i->next)
		last = i;

	if (last) {
		n->next = last->next;
		n->pprev = &last->next;
		rcu_assign_pointer(hlist_next_rcu(last), n);
	} else {
		hlist_nulls_add_head_rcu(n, h);
	}
}

/* after that hlist_nulls_del will work */
static inline void hlist_nulls_add_fake(struct hlist_nulls_node *n)
{
	n->pprev = &n->next;
	n->next = (struct hlist_nulls_node *)NULLS_MARKER(NULL);
}

/**
 * hlist_nulls_for_each_entry_rcu - iterate over rcu list of given type
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_nulls_node to use as a loop cursor.
 * @head:	the head of the list.
 * @member:	the name of the hlist_nulls_node within the struct.
 *
 * The barrier() is needed to make sure compiler doesn't cache first element [1],
 * as this loop can be restarted [2]
 * [1] Documentation/memory-barriers.txt around line 1533
 * [2] Documentation/RCU/rculist_nulls.rst around line 146
 */
#define hlist_nulls_for_each_entry_rcu(tpos, pos, head, member)			\
	for (({barrier();}),							\
	     pos = rcu_dereference_raw(hlist_nulls_first_rcu(head));		\
		(!is_a_nulls(pos)) &&						\
		({ tpos = hlist_nulls_entry(pos, typeof(*tpos), member); 1; }); \
		pos = rcu_dereference_raw(hlist_nulls_next_rcu(pos)))

/**
 * hlist_nulls_for_each_entry_safe -
 *   iterate over list of given type safe against removal of list entry
 * @tpos:	the type * to use as a loop cursor.
 * @pos:	the &struct hlist_nulls_node to use as a loop cursor.
 * @head:	the head of the list.
 * @member:	the name of the hlist_nulls_node within the struct.
 */
#define hlist_nulls_for_each_entry_safe(tpos, pos, head, member)		\
	for (({barrier();}),							\
	     pos = rcu_dereference_raw(hlist_nulls_first_rcu(head));		\
		(!is_a_nulls(pos)) &&						\
		({ tpos = hlist_nulls_entry(pos, typeof(*tpos), member);	\
		   pos = rcu_dereference_raw(hlist_nulls_next_rcu(pos)); 1; });)
#endif
#endif
