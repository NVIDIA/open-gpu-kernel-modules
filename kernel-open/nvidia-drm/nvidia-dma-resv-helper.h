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

static inline void nv_dma_resv_lock(nv_dma_resv_t *obj,
                                    struct ww_acquire_ctx *ctx)
{
#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
    dma_resv_lock(obj, ctx);
#else
    ww_mutex_lock(&obj->lock, ctx);
#endif
}

static inline void nv_dma_resv_unlock(nv_dma_resv_t *obj)
{
#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
    dma_resv_unlock(obj);
#else
    ww_mutex_unlock(&obj->lock);
#endif
}

static inline int nv_dma_resv_reserve_fences(nv_dma_resv_t *obj,
                                             unsigned int num_fences,
                                             NvBool shared)
{
#if defined(NV_DMA_RESV_RESERVE_FENCES_PRESENT)
    return dma_resv_reserve_fences(obj, num_fences);
#else
    if (shared) {
#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
        return dma_resv_reserve_shared(obj, num_fences);
#elif defined(NV_RESERVATION_OBJECT_RESERVE_SHARED_HAS_NUM_FENCES_ARG)
        return reservation_object_reserve_shared(obj, num_fences);
#else
        unsigned int i;
        for (i = 0; i < num_fences; i++) {
            reservation_object_reserve_shared(obj);
        }
#endif
    }
    return 0;
#endif
}

static inline void nv_dma_resv_add_excl_fence(nv_dma_resv_t *obj,
                                              nv_dma_fence_t *fence)
{
#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
#if defined(NV_DMA_RESV_ADD_FENCE_PRESENT)
    dma_resv_add_fence(obj, fence, DMA_RESV_USAGE_WRITE);
#else
    dma_resv_add_excl_fence(obj, fence);
#endif
#else
    reservation_object_add_excl_fence(obj, fence);
#endif
}

static inline void nv_dma_resv_add_shared_fence(nv_dma_resv_t *obj,
                                                nv_dma_fence_t *fence)
{
#if defined(NV_LINUX_DMA_RESV_H_PRESENT)
#if defined(NV_DMA_RESV_ADD_FENCE_PRESENT)
    dma_resv_add_fence(obj, fence, DMA_RESV_USAGE_READ);
#else
    dma_resv_add_shared_fence(obj, fence);
#endif
#else
    reservation_object_add_shared_fence(obj, fence);
#endif
}

#endif /* defined(NV_DRM_FENCE_AVAILABLE) */

#endif /* __NVIDIA_DMA_RESV_HELPER_H__ */
