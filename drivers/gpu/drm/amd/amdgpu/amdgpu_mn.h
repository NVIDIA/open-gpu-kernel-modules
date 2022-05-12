/*
 * Copyright 2017 Advanced Micro Devices, Inc.
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
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors: Christian König
 */
#ifndef __AMDGPU_MN_H__
#define __AMDGPU_MN_H__

#ifndef HAVE_AMDKCL_HMM_MIRROR_ENABLED
#include <linux/mmu_notifier.h>
#include <linux/interval_tree.h>
/*
 * MMU Notifier
 */
struct amdgpu_mn;

enum amdgpu_mn_type {
	AMDGPU_MN_TYPE_GFX,
	AMDGPU_MN_TYPE_HSA,
};

#if defined(CONFIG_MMU_NOTIFIER)
void amdgpu_mn_lock(struct amdgpu_mn *mn);
void amdgpu_mn_unlock(struct amdgpu_mn *mn);
struct amdgpu_mn *amdgpu_mn_get(struct amdgpu_device *adev,
				enum amdgpu_mn_type type);
int amdgpu_mn_register(struct amdgpu_bo *bo, unsigned long addr);
void amdgpu_mn_unregister(struct amdgpu_bo *bo);
#else /* !CONFIG_MMU_NOTIFIER */
static inline void amdgpu_mn_lock(struct amdgpu_mn *mn) {}
static inline void amdgpu_mn_unlock(struct amdgpu_mn *mn) {}
static inline struct amdgpu_mn *amdgpu_mn_get(struct amdgpu_device *adev,
					      enum amdgpu_mn_type type)
{
	return NULL;
}
static inline int amdgpu_mn_register(struct amdgpu_bo *bo, unsigned long addr)
{
	return -ENODEV;
}
static inline void amdgpu_mn_unregister(struct amdgpu_bo *bo) {}
#endif /* CONFIG_MMU_NOTIFIER */
#else /* HAVE_AMDKCL_HMM_MIRROR_ENABLED */
#include <linux/types.h>
#include <linux/hmm.h>
#include <linux/rwsem.h>
#include <linux/workqueue.h>
#include <linux/interval_tree.h>

#ifndef HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT
/* flags used by HMM internal, not related to CPU/GPU PTE flags */
extern const uint64_t hmm_range_flags[HMM_PFN_FLAG_MAX];
extern const uint64_t hmm_range_values[HMM_PFN_VALUE_MAX];

static inline struct page *hmm_pfn_to_page(unsigned long hmm_pfn)
{
	struct hmm_range hmm_range = {
		.flags = hmm_range_flags,
		.values = hmm_range_values,
		.pfn_shift = PAGE_SHIFT,
	};

	return hmm_device_entry_to_page(&hmm_range, hmm_pfn);
}
#endif

int amdgpu_hmm_range_get_pages(struct mmu_interval_notifier *notifier,
			       struct mm_struct *mm, struct page **pages,
			       uint64_t start, uint64_t npages,
			       struct hmm_range **phmm_range, bool readonly,
			       bool mmap_locked, void *owner);
int amdgpu_hmm_range_get_pages_done(struct hmm_range *hmm_range);

#if defined(CONFIG_HMM_MIRROR)
int amdgpu_mn_register(struct amdgpu_bo *bo, unsigned long addr);
void amdgpu_mn_unregister(struct amdgpu_bo *bo);
#else
static inline int amdgpu_mn_register(struct amdgpu_bo *bo, unsigned long addr)
{
	DRM_WARN_ONCE("HMM_MIRROR kernel config option is not enabled, "
		      "add CONFIG_ZONE_DEVICE=y in config file to fix this\n");
	return -ENODEV;
}
static inline void amdgpu_mn_unregister(struct amdgpu_bo *bo) {}
#endif
#endif /* HAVE_AMDKCL_HMM_MIRROR_ENABLED */

#endif
