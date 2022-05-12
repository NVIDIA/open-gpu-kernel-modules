/*
 * \file drm_memory.c
 * Memory management wrappers for DRM
 *
 * \author Rickard E. (Rik) Faith <faith@valinux.com>
 * \author Gareth Hughes <gareth@valinux.com>
 */

/*
 * Created: Thu Feb  4 14:00:34 1999 by faith@valinux.com
 *
 * Copyright 1999 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#include <kcl/kcl_drm_cache.h>

#include <linux/ioport.h>
#include <xen/xen.h>

/* Copied from drivers/gpu/drm/drm_memory.c */
#if !defined(HAVE_DRM_NEED_SWIOTLB)
bool drm_need_swiotlb(int dma_bits)
{
	struct resource *tmp;
	resource_size_t max_iomem = 0;

	/*
	 * Xen paravirtual hosts require swiotlb regardless of requested dma
	 * transfer size.
	 *
	 * NOTE: Really, what it requires is use of the dma_alloc_coherent
	 *       allocator used in ttm_dma_populate() instead of
	 *       ttm_populate_and_map_pages(), which bounce buffers so much in
	 *       Xen it leads to swiotlb buffer exhaustion.
	 */
	if (xen_pv_domain())
		return true;

	for (tmp = iomem_resource.child; tmp; tmp = tmp->sibling) {
		max_iomem = max(max_iomem,  tmp->end);
	}

	return max_iomem > ((u64)1 << dma_bits);
}
EXPORT_SYMBOL(drm_need_swiotlb);
#endif /* HAVE_DRM_NEED_SWIOTLB */
