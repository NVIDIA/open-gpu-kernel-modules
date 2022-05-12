/**************************************************************************
 *
 * Copyright 2006 Tungsten Graphics, Inc., Bismack, ND. USA.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 **************************************************************************/
/*
 * Simple open hash tab implementation.
 *
 * Authors:
 * Thomas Hellström <thomas-at-tungstengraphics-dot-com>
 */

#ifndef DRM_HASHTAB_H
#define DRM_HASHTAB_H

#include <linux/list.h>

#define drm_hash_entry(_ptr, _type, _member) container_of(_ptr, _type, _member)

struct drm_hash_item {
	struct hlist_node head;
	unsigned long key;
};

struct drm_open_hash {
	struct hlist_head *table;
	u8 order;
};

int drm_ht_create(struct drm_open_hash *ht, unsigned int order);
int drm_ht_insert_item(struct drm_open_hash *ht, struct drm_hash_item *item);
int drm_ht_just_insert_please(struct drm_open_hash *ht, struct drm_hash_item *item,
			      unsigned long seed, int bits, int shift,
			      unsigned long add);
int drm_ht_find_item(struct drm_open_hash *ht, unsigned long key, struct drm_hash_item **item);

void drm_ht_verbose_list(struct drm_open_hash *ht, unsigned long key);
int drm_ht_remove_key(struct drm_open_hash *ht, unsigned long key);
int drm_ht_remove_item(struct drm_open_hash *ht, struct drm_hash_item *item);
void drm_ht_remove(struct drm_open_hash *ht);

/*
 * RCU-safe interface
 *
 * The user of this API needs to make sure that two or more instances of the
 * hash table manipulation functions are never run simultaneously.
 * The lookup function drm_ht_find_item_rcu may, however, run simultaneously
 * with any of the manipulation functions as long as it's called from within
 * an RCU read-locked section.
 */
#define drm_ht_insert_item_rcu drm_ht_insert_item
#define drm_ht_just_insert_please_rcu drm_ht_just_insert_please
#define drm_ht_remove_key_rcu drm_ht_remove_key
#define drm_ht_remove_item_rcu drm_ht_remove_item
#define drm_ht_find_item_rcu drm_ht_find_item

#endif
