/*
 * Copyright (C) 2011 Red Hat, Inc.
 *
 * This file is released under the GPL.
 */

#include "dm-btree-internal.h"
#include "dm-transaction-manager.h"

#include <linux/device-mapper.h>

#define DM_MSG_PREFIX "btree spine"

/*----------------------------------------------------------------*/

#define BTREE_CSUM_XOR 121107

static int node_check(struct dm_block_validator *v,
		      struct dm_block *b,
		      size_t block_size);

static void node_prepare_for_write(struct dm_block_validator *v,
				   struct dm_block *b,
				   size_t block_size)
{
	struct btree_node *n = dm_block_data(b);
	struct node_header *h = &n->header;

	h->blocknr = cpu_to_le64(dm_block_location(b));
	h->csum = cpu_to_le32(dm_bm_checksum(&h->flags,
					     block_size - sizeof(__le32),
					     BTREE_CSUM_XOR));
}

static int node_check(struct dm_block_validator *v,
		      struct dm_block *b,
		      size_t block_size)
{
	struct btree_node *n = dm_block_data(b);
	struct node_header *h = &n->header;
	size_t value_size;
	__le32 csum_disk;
	uint32_t flags;

	if (dm_block_location(b) != le64_to_cpu(h->blocknr)) {
		DMERR_LIMIT("node_check failed: blocknr %llu != wanted %llu",
			    le64_to_cpu(h->blocknr), dm_block_location(b));
		return -ENOTBLK;
	}

	csum_disk = cpu_to_le32(dm_bm_checksum(&h->flags,
					       block_size - sizeof(__le32),
					       BTREE_CSUM_XOR));
	if (csum_disk != h->csum) {
		DMERR_LIMIT("node_check failed: csum %u != wanted %u",
			    le32_to_cpu(csum_disk), le32_to_cpu(h->csum));
		return -EILSEQ;
	}

	value_size = le32_to_cpu(h->value_size);

	if (sizeof(struct node_header) +
	    (sizeof(__le64) + value_size) * le32_to_cpu(h->max_entries) > block_size) {
		DMERR_LIMIT("node_check failed: max_entries too large");
		return -EILSEQ;
	}

	if (le32_to_cpu(h->nr_entries) > le32_to_cpu(h->max_entries)) {
		DMERR_LIMIT("node_check failed: too many entries");
		return -EILSEQ;
	}

	/*
	 * The node must be either INTERNAL or LEAF.
	 */
	flags = le32_to_cpu(h->flags);
	if (!(flags & INTERNAL_NODE) && !(flags & LEAF_NODE)) {
		DMERR_LIMIT("node_check failed: node is neither INTERNAL or LEAF");
		return -EILSEQ;
	}

	return 0;
}

struct dm_block_validator btree_node_validator = {
	.name = "btree_node",
	.prepare_for_write = node_prepare_for_write,
	.check = node_check
};

/*----------------------------------------------------------------*/

int bn_read_lock(struct dm_btree_info *info, dm_block_t b,
		 struct dm_block **result)
{
	return dm_tm_read_lock(info->tm, b, &btree_node_validator, result);
}

static int bn_shadow(struct dm_btree_info *info, dm_block_t orig,
	      struct dm_btree_value_type *vt,
	      struct dm_block **result)
{
	int r, inc;

	r = dm_tm_shadow_block(info->tm, orig, &btree_node_validator,
			       result, &inc);
	if (!r && inc)
		inc_children(info->tm, dm_block_data(*result), vt);

	return r;
}

int new_block(struct dm_btree_info *info, struct dm_block **result)
{
	return dm_tm_new_block(info->tm, &btree_node_validator, result);
}

void unlock_block(struct dm_btree_info *info, struct dm_block *b)
{
	dm_tm_unlock(info->tm, b);
}

/*----------------------------------------------------------------*/

void init_ro_spine(struct ro_spine *s, struct dm_btree_info *info)
{
	s->info = info;
	s->count = 0;
	s->nodes[0] = NULL;
	s->nodes[1] = NULL;
}

void exit_ro_spine(struct ro_spine *s)
{
	int i;

	for (i = 0; i < s->count; i++) {
		unlock_block(s->info, s->nodes[i]);
	}
}

int ro_step(struct ro_spine *s, dm_block_t new_child)
{
	int r;

	if (s->count == 2) {
		unlock_block(s->info, s->nodes[0]);
		s->nodes[0] = s->nodes[1];
		s->count--;
	}

	r = bn_read_lock(s->info, new_child, s->nodes + s->count);
	if (!r)
		s->count++;

	return r;
}

void ro_pop(struct ro_spine *s)
{
	BUG_ON(!s->count);
	--s->count;
	unlock_block(s->info, s->nodes[s->count]);
}

struct btree_node *ro_node(struct ro_spine *s)
{
	struct dm_block *block;

	BUG_ON(!s->count);
	block = s->nodes[s->count - 1];

	return dm_block_data(block);
}

/*----------------------------------------------------------------*/

void init_shadow_spine(struct shadow_spine *s, struct dm_btree_info *info)
{
	s->info = info;
	s->count = 0;
}

void exit_shadow_spine(struct shadow_spine *s)
{
	int i;

	for (i = 0; i < s->count; i++) {
		unlock_block(s->info, s->nodes[i]);
	}
}

int shadow_step(struct shadow_spine *s, dm_block_t b,
		struct dm_btree_value_type *vt)
{
	int r;

	if (s->count == 2) {
		unlock_block(s->info, s->nodes[0]);
		s->nodes[0] = s->nodes[1];
		s->count--;
	}

	r = bn_shadow(s->info, b, vt, s->nodes + s->count);
	if (!r) {
		if (!s->count)
			s->root = dm_block_location(s->nodes[0]);

		s->count++;
	}

	return r;
}

struct dm_block *shadow_current(struct shadow_spine *s)
{
	BUG_ON(!s->count);

	return s->nodes[s->count - 1];
}

struct dm_block *shadow_parent(struct shadow_spine *s)
{
	BUG_ON(s->count != 2);

	return s->count == 2 ? s->nodes[0] : NULL;
}

int shadow_has_parent(struct shadow_spine *s)
{
	return s->count >= 2;
}

dm_block_t shadow_root(struct shadow_spine *s)
{
	return s->root;
}

static void le64_inc(void *context, const void *value_le)
{
	struct dm_transaction_manager *tm = context;
	__le64 v_le;

	memcpy(&v_le, value_le, sizeof(v_le));
	dm_tm_inc(tm, le64_to_cpu(v_le));
}

static void le64_dec(void *context, const void *value_le)
{
	struct dm_transaction_manager *tm = context;
	__le64 v_le;

	memcpy(&v_le, value_le, sizeof(v_le));
	dm_tm_dec(tm, le64_to_cpu(v_le));
}

static int le64_equal(void *context, const void *value1_le, const void *value2_le)
{
	__le64 v1_le, v2_le;

	memcpy(&v1_le, value1_le, sizeof(v1_le));
	memcpy(&v2_le, value2_le, sizeof(v2_le));
	return v1_le == v2_le;
}

void init_le64_type(struct dm_transaction_manager *tm,
		    struct dm_btree_value_type *vt)
{
	vt->context = tm;
	vt->size = sizeof(__le64);
	vt->inc = le64_inc;
	vt->dec = le64_dec;
	vt->equal = le64_equal;
}
