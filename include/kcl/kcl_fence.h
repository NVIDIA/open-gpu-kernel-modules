/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Fence mechanism for dma-buf to allow for asynchronous dma access
 *
 * Copyright (C) 2012 Canonical Ltd
 * Copyright (C) 2012 Texas Instruments
 *
 * Authors:
 * Rob Clark <robdclark@gmail.com>
 * Maarten Lankhorst <maarten.lankhorst@canonical.com>
 */
#ifndef AMDKCL_FENCE_H
#define AMDKCL_FENCE_H

#include <linux/version.h>
#include <kcl/kcl_rcupdate.h>
#include <linux/dma-fence.h>
#include <kcl/kcl_fence_array.h>

#if !defined(HAVE_LINUX_DMA_FENCE_H)
#define dma_fence_cb fence_cb
#define dma_fence_ops fence_ops
#define dma_fence_array fence_array
#define dma_fence fence
#define dma_fence_init fence_init
#define dma_fence_context_alloc fence_context_alloc
#define DMA_FENCE_TRACE FENCE_TRACE
#define DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT FENCE_FLAG_ENABLE_SIGNAL_BIT
#define DMA_FENCE_FLAG_SIGNALED_BIT FENCE_FLAG_SIGNALED_BIT
#define DMA_FENCE_FLAG_USER_BITS FENCE_FLAG_USER_BITS
#define dma_fence_wait fence_wait
#define dma_fence_get fence_get
#define dma_fence_put fence_put
#define dma_fence_is_signaled fence_is_signaled
#define dma_fence_signal fence_signal
#define dma_fence_signal_locked fence_signal_locked
#define dma_fence_get_rcu fence_get_rcu
#define dma_fence_array_create fence_array_create
#define dma_fence_add_callback fence_add_callback
#define dma_fence_remove_callback fence_remove_callback
#define dma_fence_enable_sw_signaling fence_enable_sw_signaling
#define dma_fence_default_wait fence_default_wait

#if defined(HAVE_DMA_FENCE_SET_ERROR)
#define dma_fence_set_error fence_set_error
#endif
#endif

/* commit v4.5-rc3-715-gb47bcb93bbf2
 * fall back to HAVE_LINUX_DMA_FENCE_H check directly
 * as it's hard to detect the implementation in kernel
 */
#if !defined(HAVE_LINUX_DMA_FENCE_H)
static inline bool dma_fence_is_later(struct dma_fence *f1, struct dma_fence *f2)
{
	if (WARN_ON(f1->context != f2->context))
		return false;

	return (int)(f1->seqno - f2->seqno) > 0;
}
#endif

/*
 * commit v4.18-rc2-533-g418cc6ca0607
 * dma-fence: Allow wait_any_timeout for all fences)
 */
#if DRM_VERSION_CODE < DRM_VERSION(4, 19, 0)
#define AMDKCL_FENCE_WAIT_ANY_TIMEOUT
signed long
_kcl_fence_wait_any_timeout(struct dma_fence **fences, uint32_t count,
			   bool intr, signed long timeout, uint32_t *idx);
#endif

/*
 * commit  v4.9-rc2-472-gbcc004b629d2
 * dma-buf/fence: make timeout handling in fence_default_wait consistent (v2))
 *
 * commit v4.9-rc2-473-g698c0f7ff216
 * dma-buf/fence: revert "don't wait when specified timeout is zero" (v2)
 */
#if DRM_VERSION_CODE < DRM_VERSION(4, 10, 0)
#define AMDKCL_FENCE_DEFAULT_WAIT_TIMEOUT
signed long
_kcl_fence_default_wait(struct dma_fence *fence, bool intr, signed long timeout);
extern signed long _kcl_fence_wait_timeout(struct fence *fence, bool intr,
				signed long timeout);
#endif

/*
 * commit v4.14-rc3-601-g5f72db59160c
 * dma-buf/fence: Sparse wants __rcu on the object itself
 */
#if DRM_VERSION_CODE < DRM_VERSION(4, 15, 0)
#define AMDKCL_FENCE_GET_RCU_SAFE
static inline struct dma_fence *
_kcl_fence_get_rcu_safe(struct dma_fence __rcu **fencep)
{
	do {
		struct dma_fence *fence;

		fence = rcu_dereference(*fencep);
		if (!fence)
			return NULL;

		if (!dma_fence_get_rcu(fence))
			continue;

		/* The atomic_inc_not_zero() inside dma_fence_get_rcu()
		 * provides a full memory barrier upon success (such as now).
		 * This is paired with the write barrier from assigning
		 * to the __rcu protected fence pointer so that if that
		 * pointer still matches the current fence, we know we
		 * have successfully acquire a reference to it. If it no
		 * longer matches, we are holding a reference to some other
		 * reallocated pointer. This is possible if the allocator
		 * is using a freelist like SLAB_TYPESAFE_BY_RCU where the
		 * fence remains valid for the RCU grace period, but it
		 * may be reallocated. When using such allocators, we are
		 * responsible for ensuring the reference we get is to
		 * the right fence, as below.
		 */
		if (fence == rcu_access_pointer(*fencep))
			return rcu_pointer_handoff(fence);

		dma_fence_put(fence);
	} while (1);
}
#endif

/*
 * commit v4.18-rc2-519-gc701317a3eb8
 * dma-fence: Make ->enable_signaling optional
 */
#if DRM_VERSION_CODE < DRM_VERSION(4, 19, 0)
#define AMDKCL_DMA_FENCE_OPS_ENABLE_SIGNALING
bool _kcl_fence_enable_signaling(struct dma_fence *f);
#define AMDKCL_DMA_FENCE_OPS_ENABLE_SIGNALING_OPTIONAL \
	.enable_signaling = _kcl_fence_enable_signaling,
#else
#define AMDKCL_DMA_FENCE_OPS_ENABLE_SIGNALING_OPTIONAL
#endif

#if !defined(HAVE_DMA_FENCE_SET_ERROR)
/* Copied from include/linux/dma-fence.h and modified for KCL */
static inline void dma_fence_set_error(struct dma_fence *fence,
				       int error)
{
	BUG_ON(test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags));
	BUG_ON(error >= 0 || error < -MAX_ERRNO);

	fence->status = error;
}
#endif

/*
 * commit v4.18-rc2-533-g418cc6ca0607
 * dma-fence: Make ->wait callback optional
 */
#if DRM_VERSION_CODE < DRM_VERSION(4, 19, 0)
#define AMDKCL_DMA_FENCE_OPS_WAIT_OPTIONAL \
	.wait = dma_fence_default_wait,
#else
#define AMDKCL_DMA_FENCE_OPS_WAIT_OPTIONAL
#endif

#if !defined(HAVE_DMA_FENCE_GET_STUB)
struct dma_fence *_kcl_dma_fence_get_stub(void);
static inline struct dma_fence *dma_fence_get_stub(void)
{
	return _kcl_dma_fence_get_stub();
}
#endif

#endif /* AMDKCL_FENCE_H */
