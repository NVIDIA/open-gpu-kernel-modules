/*
 * Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef __NVIDIA_DMA_RESV_HELPER_H__
#define __NVIDIA_DMA_RESV_HELPER_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_FENCE_AVAILABLE)

/*
 * linux/reservation.h is renamed to linux/dma-resv.h, by commit
 * 52791eeec1d9 (dma-buf: rename reservation_object to dma_resv)
 * in v5.4.
 */

#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
#include <linux/dma-resv.h>
#else
#include <linux/reservation.h>
#endif

#include <nvidia-dma-fence-helper.h>

#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
typedef struct dma_resv nv_dma_resv_t;
#else
typedef struct reservation_object nv_dma_resv_t;
#endif

static inline void nv_dma_resv_init(nv_dma_resv_t *obj)
{
#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
    dma_resv_init(obj);
#else
    reservation_object_init(obj);
#endif
}

static inline void nv_dma_resv_fini(nv_dma_resv_t *obj)
{
#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
    dma_resv_fini(obj);
#else
    reservation_object_init(obj);
#endif
}

static inline void nv_dma_resv_add_excl_fence(nv_dma_resv_t *obj,
                                              nv_dma_fence_t *fence)
{
#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
    dma_resv_add_excl_fence(obj, fence);
#else
    reservation_object_add_excl_fence(obj, fence);
#endif
}

#endif /* defined(NV_DRM_FENCE_AVAILABLE) */

#endif /* __NVIDIA_DMA_RESV_HELPER_H__ */
