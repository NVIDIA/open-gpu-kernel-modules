/*
 * Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVIDIA_DMA_FENCE_HELPER_H__
#define __NVIDIA_DMA_FENCE_HELPER_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_FENCE_AVAILABLE)

/*
 * Fence headers are moved to file dma-fence.h and struct fence has
 * been renamed to dma_fence by commit -
 *
 *      2016-10-25 : f54d1867005c3323f5d8ad83eed823e84226c429
 */

#if defined(NV_LINUX_FENCE_H_PRESENT)
#include <linux/fence.h>
#else
#include <linux/dma-fence.h>
#endif

#if defined(NV_LINUX_FENCE_H_PRESENT)
typedef struct fence nv_dma_fence_t;
typedef struct fence_ops nv_dma_fence_ops_t;
typedef struct fence_cb nv_dma_fence_cb_t;
typedef fence_func_t nv_dma_fence_func_t;
#else
typedef struct dma_fence nv_dma_fence_t;
typedef struct dma_fence_ops nv_dma_fence_ops_t;
typedef struct dma_fence_cb nv_dma_fence_cb_t;
typedef dma_fence_func_t nv_dma_fence_func_t;
#endif

#if defined(NV_LINUX_FENCE_H_PRESENT)
#define NV_DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT FENCE_FLAG_ENABLE_SIGNAL_BIT
#else
#define NV_DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT
#endif

static inline bool nv_dma_fence_is_signaled(nv_dma_fence_t *fence) {
#if defined(NV_LINUX_FENCE_H_PRESENT)
    return fence_is_signaled(fence);
#else
    return dma_fence_is_signaled(fence);
#endif
}

static inline nv_dma_fence_t *nv_dma_fence_get(nv_dma_fence_t *fence)
{
#if defined(NV_LINUX_FENCE_H_PRESENT)
    return fence_get(fence);
#else
    return dma_fence_get(fence);
#endif
}

static inline void nv_dma_fence_put(nv_dma_fence_t *fence) {
#if defined(NV_LINUX_FENCE_H_PRESENT)
    fence_put(fence);
#else
    dma_fence_put(fence);
#endif
}

static inline signed long
nv_dma_fence_default_wait(nv_dma_fence_t *fence,
                          bool intr, signed long timeout) {
#if defined(NV_LINUX_FENCE_H_PRESENT)
    return fence_default_wait(fence, intr, timeout);
#else
    return dma_fence_default_wait(fence, intr, timeout);
#endif
}

static inline int nv_dma_fence_signal(nv_dma_fence_t *fence) {
#if defined(NV_LINUX_FENCE_H_PRESENT)
    return fence_signal(fence);
#else
    return dma_fence_signal(fence);
#endif
}

static inline int nv_dma_fence_signal_locked(nv_dma_fence_t *fence) {
#if defined(NV_LINUX_FENCE_H_PRESENT)
    return fence_signal_locked(fence);
#else
    return dma_fence_signal_locked(fence);
#endif
}

static inline u64 nv_dma_fence_context_alloc(unsigned num) {
#if defined(NV_LINUX_FENCE_H_PRESENT)
    return fence_context_alloc(num);
#else
    return dma_fence_context_alloc(num);
#endif
}

static inline void
nv_dma_fence_init(nv_dma_fence_t *fence,
                  const nv_dma_fence_ops_t *ops,
                  spinlock_t *lock, u64 context, uint64_t seqno) {
#if defined(NV_LINUX_FENCE_H_PRESENT)
    fence_init(fence, ops, lock, context, seqno);
#else
    dma_fence_init(fence, ops, lock, context, seqno);
#endif
}

static inline void
nv_dma_fence_set_error(nv_dma_fence_t *fence,
                       int error) {
#if defined(NV_DMA_FENCE_SET_ERROR_PRESENT)
    return dma_fence_set_error(fence, error);
#elif defined(NV_FENCE_SET_ERROR_PRESENT)
    return fence_set_error(fence, error);
#else
    fence->status = error;
#endif
}

static inline int
nv_dma_fence_add_callback(nv_dma_fence_t *fence,
                          nv_dma_fence_cb_t *cb,
                          nv_dma_fence_func_t func) {
#if defined(NV_LINUX_FENCE_H_PRESENT)
    return fence_add_callback(fence, cb, func);
#else
    return dma_fence_add_callback(fence, cb, func);
#endif
}

#endif /* defined(NV_DRM_FENCE_AVAILABLE) */

#endif /* __NVIDIA_DMA_FENCE_HELPER_H__ */
