// SPDX-License-Identifier: GPL-2.0
#include <asm/bug.h>
#include <linux/rbtree_augmented.h>
#include "drbd_interval.h"

/*
 * interval_end  -  return end of @node
 */
static inline
sector_t interval_end(struct rb_node *node)
{
	struct drbd_interval *this = rb_entry(node, struct drbd_interval, rb);
	return this->end;
}

#define NODE_END(node) ((node)->sector + ((node)->size >> 9))

RB_DECLARE_CALLBACKS_MAX(static, augment_callbacks,
			 struct drbd_interval, rb, sector_t, end, NODE_END);

/*
 * drbd_insert_interval  -  insert a new interval into a tree
 */
bool
drbd_insert_interval(struct rb_root *root, struct drbd_interval *this)
{
	struct rb_node **new = &root->rb_node, *parent = NULL;
	sector_t this_end = this->sector + (this->size >> 9);

	BUG_ON(!IS_ALIGNED(this->size, 512));

	while (*new) {
		struct drbd_interval *here =
			rb_entry(*new, struct drbd_interval, rb);

		parent = *new;
		if (here->end < this_end)
			here->end = this_end;
		if (this->sector < here->sector)
			new = &(*new)->rb_left;
		else if (this->sector > here->sector)
			new = &(*new)->rb_right;
		else if (this < here)
			new = &(*new)->rb_left;
		else if (this > here)
			new = &(*new)->rb_right;
		else
			return false;
	}

	this->end = this_end;
	rb_link_node(&this->rb, parent, new);
	rb_insert_augmented(&this->rb, root, &augment_callbacks);
	return true;
}

/**
 * drbd_contains_interval  -  check if a tree contains a given interval
 * @root:	red black tree root
 * @sector:	start sector of @interval
 * @interval:	may not be a valid pointer
 *
 * Returns if the tree contains the node @interval with start sector @start.
 * Does not dereference @interval until @interval is known to be a valid object
 * in @tree.  Returns %false if @interval is in the tree but with a different
 * sector number.
 */
bool
drbd_contains_interval(struct rb_root *root, sector_t sector,
		       struct drbd_interval *interval)
{
	struct rb_node *node = root->rb_node;

	while (node) {
		struct drbd_interval *here =
			rb_entry(node, struct drbd_interval, rb);

		if (sector < here->sector)
			node = node->rb_left;
		else if (sector > here->sector)
			node = node->rb_right;
		else if (interval < here)
			node = node->rb_left;
		else if (interval > here)
			node = node->rb_right;
		else
			return true;
	}
	return false;
}

/*
 * drbd_remove_interval  -  remove an interval from a tree
 */
void
drbd_remove_interval(struct rb_root *root, struct drbd_interval *this)
{
	rb_erase_augmented(&this->rb, root, &augment_callbacks);
}

/**
 * drbd_find_overlap  - search for an interval overlapping with [sector, sector + size)
 * @root:	red black tree root
 * @sector:	start sector
 * @size:	size, aligned to 512 bytes
 *
 * Returns an interval overlapping with [sector, sector + size), or NULL if
 * there is none.  When there is more than one overlapping interval in the
 * tree, the interval with the lowest start sector is returned, and all other
 * overlapping intervals will be on the right side of the tree, reachable with
 * rb_next().
 */
struct drbd_interval *
drbd_find_overlap(struct rb_root *root, sector_t sector, unsigned int size)
{
	struct rb_node *node = root->rb_node;
	struct drbd_interval *overlap = NULL;
	sector_t end = sector + (size >> 9);

	BUG_ON(!IS_ALIGNED(size, 512));

	while (node) {
		struct drbd_interval *here =
			rb_entry(node, struct drbd_interval, rb);

		if (node->rb_left &&
		    sector < interval_end(node->rb_left)) {
			/* Overlap if any must be on left side */
			node = node->rb_left;
		} else if (here->sector < end &&
			   sector < here->sector + (here->size >> 9)) {
			overlap = here;
			break;
		} else if (sector >= here->sector) {
			/* Overlap if any must be on right side */
			node = node->rb_right;
		} else
			break;
	}
	return overlap;
}

struct drbd_interval *
drbd_next_overlap(struct drbd_interval *i, sector_t sector, unsigned int size)
{
	sector_t end = sector + (size >> 9);
	struct rb_node *node;

	for (;;) {
		node = rb_next(&i->rb);
		if (!node)
			return NULL;
		i = rb_entry(node, struct drbd_interval, rb);
		if (i->sector >= end)
			return NULL;
		if (sector < i->sector + (i->size >> 9))
			return i;
	}
}
