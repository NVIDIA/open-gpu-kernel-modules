/* SPDX-License-Identifier: GPL-2.0 */

#ifndef BTRFS_MISC_H
#define BTRFS_MISC_H

#include <linux/sched.h>
#include <linux/wait.h>
#include <asm/div64.h>
#include <linux/rbtree.h>

#define in_range(b, first, len) ((b) >= (first) && (b) < (first) + (len))

static inline void cond_wake_up(struct wait_queue_head *wq)
{
	/*
	 * This implies a full smp_mb barrier, see comments for
	 * waitqueue_active why.
	 */
	if (wq_has_sleeper(wq))
		wake_up(wq);
}

static inline void cond_wake_up_nomb(struct wait_queue_head *wq)
{
	/*
	 * Special case for conditional wakeup where the barrier required for
	 * waitqueue_active is implied by some of the preceding code. Eg. one
	 * of such atomic operations (atomic_dec_and_return, ...), or a
	 * unlock/lock sequence, etc.
	 */
	if (waitqueue_active(wq))
		wake_up(wq);
}

static inline u64 div_factor(u64 num, int factor)
{
	if (factor == 10)
		return num;
	num *= factor;
	return div_u64(num, 10);
}

static inline u64 div_factor_fine(u64 num, int factor)
{
	if (factor == 100)
		return num;
	num *= factor;
	return div_u64(num, 100);
}

/* Copy of is_power_of_two that is 64bit safe */
static inline bool is_power_of_two_u64(u64 n)
{
	return n != 0 && (n & (n - 1)) == 0;
}

static inline bool has_single_bit_set(u64 n)
{
	return is_power_of_two_u64(n);
}

/*
 * Simple bytenr based rb_tree relate structures
 *
 * Any structure wants to use bytenr as single search index should have their
 * structure start with these members.
 */
struct rb_simple_node {
	struct rb_node rb_node;
	u64 bytenr;
};

static inline struct rb_node *rb_simple_search(struct rb_root *root, u64 bytenr)
{
	struct rb_node *node = root->rb_node;
	struct rb_simple_node *entry;

	while (node) {
		entry = rb_entry(node, struct rb_simple_node, rb_node);

		if (bytenr < entry->bytenr)
			node = node->rb_left;
		else if (bytenr > entry->bytenr)
			node = node->rb_right;
		else
			return node;
	}
	return NULL;
}

static inline struct rb_node *rb_simple_insert(struct rb_root *root, u64 bytenr,
					       struct rb_node *node)
{
	struct rb_node **p = &root->rb_node;
	struct rb_node *parent = NULL;
	struct rb_simple_node *entry;

	while (*p) {
		parent = *p;
		entry = rb_entry(parent, struct rb_simple_node, rb_node);

		if (bytenr < entry->bytenr)
			p = &(*p)->rb_left;
		else if (bytenr > entry->bytenr)
			p = &(*p)->rb_right;
		else
			return parent;
	}

	rb_link_node(node, parent, p);
	rb_insert_color(node, root);
	return NULL;
}

#endif
