/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __KCL_DRM_PRIME_H__
#define __KCL_DRM_PRIME_H__

#include <linux/mutex.h>
#include <linux/rbtree.h>
#include <linux/scatterlist.h>

#ifndef HAVE_DRM_PRIME_SG_TO_DMA_ADDR_ARRAY
static inline
int drm_prime_sg_to_dma_addr_array(struct sg_table *sgt, dma_addr_t *addrs,
				   int max_entries)
{
#ifdef HAVE_TTM_SG_TT_INIT
	return drm_prime_sg_to_page_addr_arrays(sgt, NULL, addrs, max_entries);
#else
	/*
	 * the page array stands right next to dma address array,
	 * so get the page array pointer directly by max_entries offset
	 * refer to ttm_sg_tt_init() for initial array allocation and
	 * c67e62790f5c drm/prime: split array import functions v4 for
	 * the change to drm_prime_sg_to_page_addr_arrays()
	 */
	struct page **pages = (void*)((unsigned long)addrs - max_entries*sizeof(dma_addr_t));
	return drm_prime_sg_to_page_addr_arrays(sgt, pages, addrs, max_entries);
#endif
}
#endif /* HAVE_DRM_PRIME_SG_TO_DMA_ADDR_ARRAY */
#endif
