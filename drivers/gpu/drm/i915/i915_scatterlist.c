/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2016 Intel Corporation
 */

#include "i915_scatterlist.h"

#include <drm/drm_mm.h>

#include <linux/slab.h>

bool i915_sg_trim(struct sg_table *orig_st)
{
	struct sg_table new_st;
	struct scatterlist *sg, *new_sg;
	unsigned int i;

	if (orig_st->nents == orig_st->orig_nents)
		return false;

	if (sg_alloc_table(&new_st, orig_st->nents, GFP_KERNEL | __GFP_NOWARN))
		return false;

	new_sg = new_st.sgl;
	for_each_sg(orig_st->sgl, sg, orig_st->nents, i) {
		sg_set_page(new_sg, sg_page(sg), sg->length, 0);
		sg_dma_address(new_sg) = sg_dma_address(sg);
		sg_dma_len(new_sg) = sg_dma_len(sg);

		new_sg = sg_next(new_sg);
	}
	GEM_BUG_ON(new_sg); /* Should walk exactly nents and hit the end */

	sg_free_table(orig_st);

	*orig_st = new_st;
	return true;
}

/**
 * i915_sg_from_mm_node - Create an sg_table from a struct drm_mm_node
 * @node: The drm_mm_node.
 * @region_start: An offset to add to the dma addresses of the sg list.
 *
 * Create a struct sg_table, initializing it from a struct drm_mm_node,
 * taking a maximum segment length into account, splitting into segments
 * if necessary.
 *
 * Return: A pointer to a kmalloced struct sg_table on success, negative
 * error code cast to an error pointer on failure.
 */
struct sg_table *i915_sg_from_mm_node(const struct drm_mm_node *node,
				      u64 region_start)
{
	const u64 max_segment = SZ_1G; /* Do we have a limit on this? */
	u64 segment_pages = max_segment >> PAGE_SHIFT;
	u64 block_size, offset, prev_end;
	struct sg_table *st;
	struct scatterlist *sg;

	st = kmalloc(sizeof(*st), GFP_KERNEL);
	if (!st)
		return ERR_PTR(-ENOMEM);

	if (sg_alloc_table(st, DIV_ROUND_UP(node->size, segment_pages),
			   GFP_KERNEL)) {
		kfree(st);
		return ERR_PTR(-ENOMEM);
	}

	sg = st->sgl;
	st->nents = 0;
	prev_end = (resource_size_t)-1;
	block_size = node->size << PAGE_SHIFT;
	offset = node->start << PAGE_SHIFT;

	while (block_size) {
		u64 len;

		if (offset != prev_end || sg->length >= max_segment) {
			if (st->nents)
				sg = __sg_next(sg);

			sg_dma_address(sg) = region_start + offset;
			sg_dma_len(sg) = 0;
			sg->length = 0;
			st->nents++;
		}

		len = min(block_size, max_segment - sg->length);
		sg->length += len;
		sg_dma_len(sg) += len;

		offset += len;
		block_size -= len;

		prev_end = offset;
	}

	sg_mark_end(sg);
	i915_sg_trim(st);

	return st;
}

#if IS_ENABLED(CONFIG_DRM_I915_SELFTEST)
#include "selftests/scatterlist.c"
#endif
