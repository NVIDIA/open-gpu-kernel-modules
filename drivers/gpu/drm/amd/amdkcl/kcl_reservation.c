// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2012-2014 Canonical Ltd (Maarten Lankhorst)
 *
 * Based on bo.c which bears the following copyright notice,
 * but is dual licensed:
 *
 * Copyright (c) 2006-2009 VMware, Inc., Palo Alto, CA., USA
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
 **************************************************************************/
/*
 * Authors: Thomas Hellstrom <thellstrom-at-vmware-dot-com>
 */
#include <kcl/kcl_reservation.h>

void amdkcl_reservation_init(void)
{
	amdkcl_fp_setup("reservation_ww_class", NULL);
}

#if defined(HAVE_RESERVATION_OBJECT_STAGED)
/*
 * Copied from v4.19-rc6-1514-g27836b641c1b^:drivers/dma-buf/reservation.c
 * and modified for KCL
 */
static void
reservation_object_add_shared_inplace(struct reservation_object *obj,
				      struct reservation_object_list *fobj,
				      struct dma_fence *fence)
{
	struct dma_fence *signaled = NULL;
	u32 i, signaled_idx;

	dma_fence_get(fence);

	preempt_disable();
	write_seqcount_begin(&obj->seq);

	for (i = 0; i < fobj->shared_count; ++i) {
		struct dma_fence *old_fence;

		old_fence = rcu_dereference_protected(fobj->shared[i],
						dma_resv_held(obj));

		if (old_fence->context == fence->context) {
			/* memory barrier is added by write_seqcount_begin */
			RCU_INIT_POINTER(fobj->shared[i], fence);
			write_seqcount_end(&obj->seq);
			preempt_enable();

			dma_fence_put(old_fence);
			return;
		}

		if (!signaled && dma_fence_is_signaled(old_fence)) {
			signaled = old_fence;
			signaled_idx = i;
		}
	}

	/*
	 * memory barrier is added by write_seqcount_begin,
	 * fobj->shared_count is protected by this lock too
	 */
	if (signaled) {
		RCU_INIT_POINTER(fobj->shared[signaled_idx], fence);
	} else {
		BUG_ON(fobj->shared_count >= fobj->shared_max);
		RCU_INIT_POINTER(fobj->shared[fobj->shared_count], fence);
		fobj->shared_count++;
	}

	write_seqcount_end(&obj->seq);
	preempt_enable();

	dma_fence_put(signaled);
}

static void
reservation_object_add_shared_replace(struct reservation_object *obj,
				      struct reservation_object_list *old,
				      struct reservation_object_list *fobj,
				      struct dma_fence *fence)
{
	unsigned i, j, k;

	dma_fence_get(fence);

	if (!old) {
		RCU_INIT_POINTER(fobj->shared[0], fence);
		fobj->shared_count = 1;
		goto done;
	}

	/*
	 * no need to bump fence refcounts, rcu_read access
	 * requires the use of kref_get_unless_zero, and the
	 * references from the old struct are carried over to
	 * the new.
	 */
	for (i = 0, j = 0, k = fobj->shared_max; i < old->shared_count; ++i) {
		struct dma_fence *check;

		check = rcu_dereference_protected(old->shared[i],
						dma_resv_held(obj));

		if (check->context == fence->context ||
		    dma_fence_is_signaled(check))
			RCU_INIT_POINTER(fobj->shared[--k], check);
		else
			RCU_INIT_POINTER(fobj->shared[j++], check);
	}
	fobj->shared_count = j;
	RCU_INIT_POINTER(fobj->shared[fobj->shared_count], fence);
	fobj->shared_count++;

done:
	preempt_disable();
	write_seqcount_begin(&obj->seq);
	/*
	 * RCU_INIT_POINTER can be used here,
	 * seqcount provides the necessary barriers
	 */
	RCU_INIT_POINTER(obj->fence, fobj);
	write_seqcount_end(&obj->seq);
	preempt_enable();

	if (!old)
		return;

	/* Drop the references to the signaled fences */
	for (i = k; i < fobj->shared_max; ++i) {
		struct dma_fence *f;

		f = rcu_dereference_protected(fobj->shared[i],
					      dma_resv_held(obj));
		dma_fence_put(f);
	}
	kfree_rcu(old, rcu);
}

void _kcl_dma_resv_add_shared_fence(struct dma_resv *obj, struct dma_fence *fence)
{
	struct dma_resv_list *old, *fobj = obj->staged;

	old = dma_resv_shared_list(obj);
	obj->staged = NULL;

	if (!fobj)
		reservation_object_add_shared_inplace(obj, old, fence);
	else
		reservation_object_add_shared_replace(obj, old, fobj, fence);
}
EXPORT_SYMBOL(_kcl_dma_resv_add_shared_fence);

int _kcl_dma_resv_copy_fences(struct dma_resv *dst, struct dma_resv *src)
{
	int ret;

	ret = dma_resv_copy_fences(dst, src);
	if (ret)
		return ret;

	kfree(dst->staged);
	dst->staged = NULL;

	return ret;
}
EXPORT_SYMBOL(_kcl_dma_resv_copy_fences);
#endif
