/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2016 Intel Corporation
 */

#ifndef I915_SCATTERLIST_H
#define I915_SCATTERLIST_H

#include <linux/pfn.h>
#include <linux/scatterlist.h>
#include <linux/swiotlb.h>

#include "i915_gem.h"

struct drm_mm_node;

/*
 * Optimised SGL iterator for GEM objects
 */
static __always_inline struct sgt_iter {
	struct scatterlist *sgp;
	union {
		unsigned long pfn;
		dma_addr_t dma;
	};
	unsigned int curr;
	unsigned int max;
} __sgt_iter(struct scatterlist *sgl, bool dma) {
	struct sgt_iter s = { .sgp = sgl };

	if (dma && s.sgp && sg_dma_len(s.sgp) == 0) {
		s.sgp = NULL;
	} else if (s.sgp) {
		s.max = s.curr = s.sgp->offset;
		if (dma) {
			s.dma = sg_dma_address(s.sgp);
			s.max += sg_dma_len(s.sgp);
		} else {
			s.pfn = page_to_pfn(sg_page(s.sgp));
			s.max += s.sgp->length;
		}
	}

	return s;
}

static inline int __sg_page_count(const struct scatterlist *sg)
{
	return sg->length >> PAGE_SHIFT;
}

static inline int __sg_dma_page_count(const struct scatterlist *sg)
{
	return sg_dma_len(sg) >> PAGE_SHIFT;
}

static inline struct scatterlist *____sg_next(struct scatterlist *sg)
{
	++sg;
	if (unlikely(sg_is_chain(sg)))
		sg = sg_chain_ptr(sg);
	return sg;
}

/**
 * __sg_next - return the next scatterlist entry in a list
 * @sg:		The current sg entry
 *
 * Description:
 *   If the entry is the last, return NULL; otherwise, step to the next
 *   element in the array (@sg@+1). If that's a chain pointer, follow it;
 *   otherwise just return the pointer to the current element.
 **/
static inline struct scatterlist *__sg_next(struct scatterlist *sg)
{
	return sg_is_last(sg) ? NULL : ____sg_next(sg);
}

/**
 * __for_each_sgt_daddr - iterate over the device addresses of the given sg_table
 * @__dp:	Device address (output)
 * @__iter:	'struct sgt_iter' (iterator state, internal)
 * @__sgt:	sg_table to iterate over (input)
 * @__step:	step size
 */
#define __for_each_sgt_daddr(__dp, __iter, __sgt, __step)		\
	for ((__iter) = __sgt_iter((__sgt)->sgl, true);			\
	     ((__dp) = (__iter).dma + (__iter).curr), (__iter).sgp;	\
	     (((__iter).curr += (__step)) >= (__iter).max) ?		\
	     (__iter) = __sgt_iter(__sg_next((__iter).sgp), true), 0 : 0)

/**
 * for_each_sgt_page - iterate over the pages of the given sg_table
 * @__pp:	page pointer (output)
 * @__iter:	'struct sgt_iter' (iterator state, internal)
 * @__sgt:	sg_table to iterate over (input)
 */
#define for_each_sgt_page(__pp, __iter, __sgt)				\
	for ((__iter) = __sgt_iter((__sgt)->sgl, false);		\
	     ((__pp) = (__iter).pfn == 0 ? NULL :			\
	      pfn_to_page((__iter).pfn + ((__iter).curr >> PAGE_SHIFT))); \
	     (((__iter).curr += PAGE_SIZE) >= (__iter).max) ?		\
	     (__iter) = __sgt_iter(__sg_next((__iter).sgp), false), 0 : 0)

/**
 * i915_sg_dma_sizes - Record the dma segment sizes of a scatterlist
 * @sg: The scatterlist
 *
 * Return: An unsigned int with segment sizes logically or'ed together.
 * A caller can use this information to determine what hardware page table
 * entry sizes can be used to map the memory represented by the scatterlist.
 */
static inline unsigned int i915_sg_dma_sizes(struct scatterlist *sg)
{
	unsigned int page_sizes;

	page_sizes = 0;
	while (sg && sg_dma_len(sg)) {
		GEM_BUG_ON(sg->offset);
		GEM_BUG_ON(!IS_ALIGNED(sg_dma_len(sg), PAGE_SIZE));
		page_sizes |= sg_dma_len(sg);
		sg = __sg_next(sg);
	}

	return page_sizes;
}

static inline unsigned int i915_sg_segment_size(void)
{
	unsigned int size = swiotlb_max_segment();

	if (size == 0)
		size = UINT_MAX;

	size = rounddown(size, PAGE_SIZE);
	/* swiotlb_max_segment_size can return 1 byte when it means one page. */
	if (size < PAGE_SIZE)
		size = PAGE_SIZE;

	return size;
}

bool i915_sg_trim(struct sg_table *orig_st);

struct sg_table *i915_sg_from_mm_node(const struct drm_mm_node *node,
				      u64 region_start);
#endif
