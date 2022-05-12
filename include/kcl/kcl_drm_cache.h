/**************************************************************************
 *
 * Copyright 2009 Red Hat Inc.
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
 * Authors:
 * Dave Airlie <airlied@redhat.com>
 */
#ifndef AMDKCL_DRM_CACHE_H
#define AMDKCL_DRM_CACHE_H
#include <linux/types.h>
#include <drm/drm_cache.h>

#if !defined(HAVE_DRM_NEED_SWIOTLB)
bool drm_need_swiotlb(int dma_bits);
#endif /* HAVE_DRM_NEED_SWIOTLB */

/*
 * Copied from include/drm/drm_cache.h
 * v5.4-rc2-80-g268a2d600130 MIPS: Loongson64: Rename CPU TYPES
 */
static inline bool kcl_drm_arch_can_wc_memory(void)
{
#if defined(CONFIG_PPC) && !defined(CONFIG_NOT_COHERENT_CACHE)
	return false;
#elif defined(CONFIG_MIPS) && \
	(defined(CONFIG_CPU_LOONGSON64) || defined(CPU_LOONGSON3))

	return false;
#elif defined(CONFIG_ARM) || defined(CONFIG_ARM64)
	/*
	 * The DRM driver stack is designed to work with cache coherent devices
	 * only, but permits an optimization to be enabled in some cases, where
	 * for some buffers, both the CPU and the GPU use uncached mappings,
	 * removing the need for DMA snooping and allocation in the CPU caches.
	 *
	 * The use of uncached GPU mappings relies on the correct implementation
	 * of the PCIe NoSnoop TLP attribute by the platform, otherwise the GPU
	 * will use cached mappings nonetheless. On x86 platforms, this does not
	 * seem to matter, as uncached CPU mappings will snoop the caches in any
	 * case. However, on ARM and arm64, enabling this optimization on a
	 * platform where NoSnoop is ignored results in loss of coherency, which
	 * breaks correct operation of the device. Since we have no way of
	 * detecting whether NoSnoop works or not, just disable this
	 * optimization entirely for ARM and arm64.
	 */
	return false;
#else
	return true;
#endif
}

#endif /* AMDKCL_DRM_CACHE_H */
