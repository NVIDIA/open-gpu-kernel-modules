// SPDX-License-Identifier: GPL-2.0-only
/*
 * Fence mechanism for dma-buf and to allow for asynchronous dma access
 *
 * Copyright (C) 2012 Canonical Ltd
 * Copyright (C) 2012 Texas Instruments
 *
 * Authors:
 * Rob Clark <robdclark@gmail.com>
 * Maarten Lankhorst <maarten.lankhorst@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

#include <linux/slab.h>
#include <kcl/kcl_fence.h>

#define CREATE_TRACE_POINTS
#include "kcl_fence_trace.h"

/* Copied from drivers/dma-buf/dma-fence.c */
#if defined(AMDKCL_FENCE_DEFAULT_WAIT_TIMEOUT) || defined(AMDKCL_FENCE_WAIT_ANY_TIMEOUT)
static bool
dma_fence_test_signaled_any(struct dma_fence **fences, uint32_t count,
			    uint32_t *idx)
{
	int i;

	for (i = 0; i < count; ++i) {
		struct dma_fence *fence = fences[i];
		if (test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags)) {
			if (idx)
				*idx = i;
			return true;
		}
	}
	return false;
}
#endif

struct default_wait_cb {
	struct dma_fence_cb base;
	struct task_struct *task;
};

static void (*_kcl_fence_default_wait_cb)(struct dma_fence *fence, struct dma_fence_cb *cb);

#ifdef AMDKCL_FENCE_DEFAULT_WAIT_TIMEOUT
signed long
_kcl_fence_default_wait(struct dma_fence *fence, bool intr, signed long timeout)
{
	struct default_wait_cb cb;
	unsigned long flags;
	signed long ret = timeout ? timeout : 1;
	bool was_set;

	if (test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags))
		return ret;

	spin_lock_irqsave(fence->lock, flags);

	if (intr && signal_pending(current)) {
		ret = -ERESTARTSYS;
		goto out;
	}

	was_set = test_and_set_bit(DMA_FENCE_FLAG_ENABLE_SIGNAL_BIT,
			&fence->flags);

	if (test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags))
		goto out;

	if (!was_set && fence->ops->enable_signaling) {
		/*
		 * Modifications [2017-03-29] (c) [2017]
		 * Advanced Micro Devices, Inc.
		 */
		trace_kcl_fence_enable_signal(fence);

		if (!fence->ops->enable_signaling(fence)) {
			dma_fence_signal_locked(fence);
			goto out;
		}
	}

	if (!timeout) {
		ret = 0;
		goto out;
	}

	cb.base.func = _kcl_fence_default_wait_cb;
	cb.task = current;
	list_add(&cb.base.node, &fence->cb_list);

	while (!test_bit(DMA_FENCE_FLAG_SIGNALED_BIT, &fence->flags) && ret > 0) {
		if (intr)
			__set_current_state(TASK_INTERRUPTIBLE);
		else
			__set_current_state(TASK_UNINTERRUPTIBLE);
		spin_unlock_irqrestore(fence->lock, flags);

		ret = schedule_timeout(ret);

		spin_lock_irqsave(fence->lock, flags);
		if (ret > 0 && intr && signal_pending(current))
			ret = -ERESTARTSYS;
	}

	if (!list_empty(&cb.base.node))
		list_del(&cb.base.node);
	__set_current_state(TASK_RUNNING);

out:
	spin_unlock_irqrestore(fence->lock, flags);
	return ret;
}
EXPORT_SYMBOL(_kcl_fence_default_wait);
#endif


/*
 * Modifications [2017-09-19] (c) [2017]
 * Advanced Micro Devices, Inc.
 */
#ifdef AMDKCL_FENCE_WAIT_ANY_TIMEOUT
signed long
_kcl_fence_wait_any_timeout(struct dma_fence **fences, uint32_t count,
			   bool intr, signed long timeout, uint32_t *idx)
{
	struct default_wait_cb *cb;
	signed long ret = timeout;
	unsigned i;

	if (WARN_ON(!fences || !count || timeout < 0))
		return -EINVAL;

	if (timeout == 0) {
		for (i = 0; i < count; ++i)
			if (dma_fence_is_signaled(fences[i])) {
				if (idx)
					*idx = i;
				return 1;
			}

		return 0;
	}

	cb = kcalloc(count, sizeof(struct default_wait_cb), GFP_KERNEL);
	if (cb == NULL) {
		ret = -ENOMEM;
		goto err_free_cb;
	}

	for (i = 0; i < count; ++i) {
		struct dma_fence *fence = fences[i];


		cb[i].task = current;
		if (dma_fence_add_callback(fence, &cb[i].base,
					   _kcl_fence_default_wait_cb)) {
			/* This fence is already signaled */
			if (idx)
				*idx = i;
			goto fence_rm_cb;
		}
	}

	while (ret > 0) {
		if (intr)
			set_current_state(TASK_INTERRUPTIBLE);
		else
			set_current_state(TASK_UNINTERRUPTIBLE);

		if (dma_fence_test_signaled_any(fences, count, idx))
			break;

		ret = schedule_timeout(ret);

		if (ret > 0 && intr && signal_pending(current))
			ret = -ERESTARTSYS;
	}

	__set_current_state(TASK_RUNNING);

fence_rm_cb:
	while (i-- > 0)
		dma_fence_remove_callback(fences[i], &cb[i].base);

err_free_cb:
	kfree(cb);

	return ret;
}
EXPORT_SYMBOL(_kcl_fence_wait_any_timeout);
#endif

#ifdef AMDKCL_FENCE_DEFAULT_WAIT_TIMEOUT
signed long
_kcl_fence_wait_timeout(struct dma_fence *fence, bool intr, signed long timeout)
{
	signed long ret;

	if (WARN_ON(timeout < 0))
		return -EINVAL;

	/*
	 * Modifications [2017-03-29] (c) [2017]
	 * Advanced Micro Devices, Inc.
	 */
	trace_kcl_fence_wait_start(fence);
	if (fence->ops->wait)
		ret = fence->ops->wait(fence, intr, timeout);
	else
		ret = _kcl_fence_default_wait(fence, intr, timeout);
	trace_kcl_fence_wait_end(fence);
	return ret;
}
EXPORT_SYMBOL(_kcl_fence_wait_timeout);
#endif

#ifdef AMDKCL_DMA_FENCE_OPS_ENABLE_SIGNALING
bool _kcl_fence_enable_signaling(struct dma_fence *f)
{
	return true;
}
EXPORT_SYMBOL(_kcl_fence_enable_signaling);
#endif
/*
 * Modifications [2016-12-23] (c) [2016]
 * Advanced Micro Devices, Inc.
 */
void amdkcl_fence_init(void)
{
#if defined(HAVE_LINUX_DMA_FENCE_H)
	_kcl_fence_default_wait_cb = amdkcl_fp_setup("dma_fence_default_wait_cb", NULL);
#else
	_kcl_fence_default_wait_cb = amdkcl_fp_setup("fence_default_wait_cb", NULL);
#endif
}

#if !defined(HAVE_DMA_FENCE_GET_STUB)
/* Copied from drivers/dma-buf/dma-fence.c and modified for KCL */
static DEFINE_SPINLOCK(dma_fence_stub_lock);
static struct dma_fence dma_fence_stub;

static const char *dma_fence_stub_get_name(struct dma_fence *fence)
{
        return "stub";
}

static const struct dma_fence_ops dma_fence_stub_ops = {
	.get_driver_name = dma_fence_stub_get_name,
	.get_timeline_name = dma_fence_stub_get_name,
	AMDKCL_DMA_FENCE_OPS_ENABLE_SIGNALING_OPTIONAL
	AMDKCL_DMA_FENCE_OPS_WAIT_OPTIONAL
};

struct dma_fence *_kcl_dma_fence_get_stub(void)
{
	spin_lock(&dma_fence_stub_lock);
	if (!dma_fence_stub.ops) {
		u64 fence_context = dma_fence_context_alloc(1);
		dma_fence_init(&dma_fence_stub,
			       &dma_fence_stub_ops,
			       &dma_fence_stub_lock,
			       fence_context, 0);
		dma_fence_signal_locked(&dma_fence_stub);
	}
	spin_unlock(&dma_fence_stub_lock);

	return dma_fence_get(&dma_fence_stub);
}
EXPORT_SYMBOL(_kcl_dma_fence_get_stub);
#endif
