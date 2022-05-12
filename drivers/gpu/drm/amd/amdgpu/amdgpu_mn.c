/*
 * Copyright 2014 Advanced Micro Devices, Inc.
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 */
/*
 * Authors:
 *    Christian König <christian.koenig@amd.com>
 */

/**
 * DOC: MMU Notifier
 *
 * For coherent userptr handling registers an MMU notifier to inform the driver
 * about updates on the page tables of a process.
 *
 * When somebody tries to invalidate the page tables we block the update until
 * all operations on the pages in question are completed, then those pages are
 * marked as accessed and also dirty if it wasn't a read only access.
 *
 * New command submissions using the userptrs in question are delayed until all
 * page table invalidation are completed and we once more see a coherent process
 * address space.
 */

#include <linux/firmware.h>
#include <linux/module.h>
#include <drm/drm.h>

#include "amdgpu.h"
#include "amdgpu_amdkfd.h"

#ifndef HAVE_AMDKCL_HMM_MIRROR_ENABLED
/**
 * struct amdgpu_mn
 *
 * @adev: amdgpu device pointer
 * @mm: process address space
 * @mn: MMU notifier structure
 * @type: type of MMU notifier
 * @work: destruction work item
 * @node: hash table node to find structure by adev and mn
 * @lock: rw semaphore protecting the notifier nodes
 * @objects: interval tree containing amdgpu_mn_nodes
 * @read_lock: mutex for recursive locking of @lock
 * @recursion: depth of recursion
 *
 * Data for each amdgpu device and process address space.
 */
struct amdgpu_mn {
	/* constant after initialisation */
	struct amdgpu_device	*adev;
	struct mm_struct	*mm;
	struct mmu_notifier	mn;
	enum amdgpu_mn_type	type;

	/* protected by adev->mn_lock */
	struct hlist_node	node;

	/* objects protected by lock */
	struct rw_semaphore	lock;
#ifndef HAVE_TREE_INSERT_HAVE_RB_ROOT_CACHED
	struct rb_root		objects;
#else
	struct rb_root_cached	objects;
#endif
	struct mutex		read_lock;
	atomic_t		recursion;
#if !defined(HAVE_MMU_NOTIFIER_PUT)
	struct rcu_head	rcu;
#endif
};

/**
 * struct amdgpu_mn_node
 *
 * @it: interval node defining start-last of the affected address range
 * @bos: list of all BOs in the affected address range
 *
 * Manages all BOs which are affected of a certain range of address space.
 */
struct amdgpu_mn_node {
	struct interval_tree_node	it;
	struct list_head		bos;
};

#ifdef HAVE_MMU_NOTIFIER_PUT
static void amdgpu_mn_free(struct mmu_notifier *mn)
{
	kfree(container_of(mn, struct amdgpu_mn, mn));
}
#else
static void amdgpu_mn_free(struct rcu_head *rcu)
{
	kfree(container_of(rcu, struct amdgpu_mn, rcu));
}
#endif

/**
 * amdgpu_mn_destroy - destroy the MMU notifier
 *
 * @amn: our notifier
 *
 * Destroy the notifier
 */
static void amdgpu_mn_destroy(struct amdgpu_mn *amn)
{
	struct amdgpu_device *adev = amn->adev;
	struct amdgpu_mn_node *node, *next_node;
	struct amdgpu_bo *bo, *next_bo;

	mutex_lock(&adev->mn_lock);
	down_write(&amn->lock);
	hash_del(&amn->node);
	rbtree_postorder_for_each_entry_safe(node, next_node,
#ifndef HAVE_TREE_INSERT_HAVE_RB_ROOT_CACHED
					     &amn->objects, it.rb) {
#else
					     &amn->objects.rb_root, it.rb) {
#endif
		list_for_each_entry_safe(bo, next_bo, &node->bos, mn_list) {
			bo->mn = NULL;
			list_del_init(&bo->mn_list);
		}
		kfree(node);
	}

#ifndef HAVE_TREE_INSERT_HAVE_RB_ROOT_CACHED
	amn->objects = RB_ROOT;
#else
	amn->objects = RB_ROOT_CACHED;
#endif

	up_write(&amn->lock);

#ifdef HAVE_MMU_NOTIFIER_PUT
	mmu_notifier_put(&amn->mn);
#else
	mmu_notifier_unregister_no_release(&amn->mn, amn->mm);
	mmu_notifier_call_srcu(&amn->rcu, amdgpu_mn_free);
#endif

	mutex_unlock(&adev->mn_lock);
}

/**
 * amdgpu_mn_release - callback to notify about mm destruction
 *
 * @mn: our notifier
 * @mm: the mm this callback is about
 *
 * Destroy our notifier.
 */
static void amdgpu_mn_release(struct mmu_notifier *mn,
			      struct mm_struct *mm)
{
	struct amdgpu_mn *amn = container_of(mn, struct amdgpu_mn, mn);

	amdgpu_mn_destroy(amn);
}


/**
 * amdgpu_mn_lock - take the write side lock for this notifier
 *
 * @mn: our notifier
 */
void amdgpu_mn_lock(struct amdgpu_mn *mn)
{
	if (mn)
		down_write(&mn->lock);
}

/**
 * amdgpu_mn_unlock - drop the write side lock for this notifier
 *
 * @mn: our notifier
 */
void amdgpu_mn_unlock(struct amdgpu_mn *mn)
{
	if (mn)
		up_write(&mn->lock);
}

#if !defined(HAVE_5ARGS_INVALIDATE_RANGE_START) && !defined(HAVE_2ARGS_INVALIDATE_RANGE_START)
/**
 * amdgpu_mn_read_lock - take the read side lock for this notifier
 *
 * @amn: our notifier
 */
static void amdgpu_mn_read_lock(struct amdgpu_mn *amn)
{
	/* FIXME: Need figure out one way to detect
	 * if we are in oom reaper context.
	 */
	mutex_lock(&amn->read_lock);
	if (atomic_inc_return(&amn->recursion) == 1)
		down_read_non_owner(&amn->lock);
	mutex_unlock(&amn->read_lock);
}
#else
/**
 * amdgpu_mn_read_lock - take the read side lock for this notifier
 *
 * @amn: our notifier
 */
static int amdgpu_mn_read_lock(struct amdgpu_mn *amn, bool blockable)
{
	/* Non blockable occurs only in oom reaper context.
	 * In this case, process is going to be killed anyway.
	 * Let oom reaper fail at this stage.
	 */
	if (!blockable)
		return -EAGAIN;

	mutex_lock(&amn->read_lock);
	if (atomic_inc_return(&amn->recursion) == 1)
		down_read_non_owner(&amn->lock);
	mutex_unlock(&amn->read_lock);

	return 0;
}
#endif

/**
 * amdgpu_mn_read_unlock - drop the read side lock for this notifier
 *
 * @amn: our notifier
 */
static void amdgpu_mn_read_unlock(struct amdgpu_mn *amn)
{
	if (atomic_dec_return(&amn->recursion) == 0)
		up_read_non_owner(&amn->lock);
}

/**
 * amdgpu_mn_invalidate_node - unmap all BOs of a node
 *
 * @node: the node with the BOs to unmap
 * @start: start of address range affected
 * @end: end of address range affected
 *
 * Block for operations on BOs to finish and mark pages as accessed and
 * potentially dirty.
 */
static void amdgpu_mn_invalidate_node(struct amdgpu_mn_node *node,
				      unsigned long start,
				      unsigned long end)
{
	struct amdgpu_bo *bo;
	long r;
	unsigned long userptr;

	list_for_each_entry(bo, &node->bos, mn_list) {

		if (!amdgpu_ttm_tt_affect_userptr(bo->tbo.ttm, start, end, &userptr))
			continue;

		r = dma_resv_wait_timeout(amdkcl_ttm_resvp(&bo->tbo),
			true, false, MAX_SCHEDULE_TIMEOUT);
		if (r <= 0)
			DRM_ERROR("(%ld) failed to wait for user bo\n", r);

		amdgpu_ttm_tt_mark_user_pages(bo->tbo.ttm);
	}
}

#if defined(HAVE_2ARGS_INVALIDATE_RANGE_START)
/**
 * amdgpu_mn_invalidate_range_start_gfx - callback to notify about mm change
 *
 * @mn: our notifier
 * @range: mmu notifier context
 *
 * Block for operations on BOs to finish and mark pages as accessed and
 * potentially dirty.
 */
static int amdgpu_mn_invalidate_range_start_gfx(struct mmu_notifier *mn,
			const struct mmu_notifier_range *range)
{
	struct amdgpu_mn *amn = container_of(mn, struct amdgpu_mn, mn);
	struct interval_tree_node *it;
	unsigned long end;

	/* notification is exclusive, but interval is inclusive */
	end = range->end - 1;

	/* TODO we should be able to split locking for interval tree and
	 * amdgpu_mn_invalidate_node
	 */
	if (amdgpu_mn_read_lock(amn, mmu_notifier_range_blockable(range)))
		return -EAGAIN;

	it = interval_tree_iter_first(&amn->objects, range->start, end);
	while (it) {
		struct amdgpu_mn_node *node;

		node = container_of(it, struct amdgpu_mn_node, it);
		it = interval_tree_iter_next(it, range->start, end);

		amdgpu_mn_invalidate_node(node, range->start, end);
	}

	return 0;
}

/**
 * amdgpu_mn_invalidate_range_start_hsa - callback to notify about mm change
 *
 * @mn: our notifier
 * @mm: the mm this callback is about
 * @start: start of updated range
 * @end: end of updated range
 *
 * We temporarily evict all BOs between start and end. This
 * necessitates evicting all user-mode queues of the process. The BOs
 * are restorted in amdgpu_mn_invalidate_range_end_hsa.
 */
static int amdgpu_mn_invalidate_range_start_hsa(struct mmu_notifier *mn,
			const struct mmu_notifier_range *range)
{
	struct amdgpu_mn *amn = container_of(mn, struct amdgpu_mn, mn);
	struct interval_tree_node *it;
	unsigned long end;
	unsigned long userptr;

	/* notification is exclusive, but interval is inclusive */
	end = range->end - 1;

	if (amdgpu_mn_read_lock(amn, mmu_notifier_range_blockable(range)))
		return -EAGAIN;

	it = interval_tree_iter_first(&amn->objects, range->start, end);
	while (it) {
		struct amdgpu_mn_node *node;
		struct amdgpu_bo *bo;

		node = container_of(it, struct amdgpu_mn_node, it);
		it = interval_tree_iter_next(it, range->start, end);

		list_for_each_entry(bo, &node->bos, mn_list) {
			struct kgd_mem *mem = bo->kfd_bo;

			if (amdgpu_ttm_tt_affect_userptr(bo->tbo.ttm,
							 range->start,
							 end, &userptr))
				amdgpu_amdkfd_evict_userptr(mem, range->mm);
		}
	}

	return 0;
}

#else

/**
 * amdgpu_mn_invalidate_range_start_gfx - callback to notify about mm change
 *
 * @mn: our notifier
 * @mm: the mm this callback is about
 * @start: start of updated range
 * @end: end of updated range
 *
 * Block for operations on BOs to finish and mark pages as accessed and
 * potentially dirty.
 */
#if defined(HAVE_5ARGS_INVALIDATE_RANGE_START)
static int amdgpu_mn_invalidate_range_start_gfx(struct mmu_notifier *mn,
						 struct mm_struct *mm,
						 unsigned long start,
						 unsigned long end,
						 bool blockable)
#else
static void amdgpu_mn_invalidate_range_start_gfx(struct mmu_notifier *mn,
						 struct mm_struct *mm,
						 unsigned long start,
						 unsigned long end)
#endif
{
	struct amdgpu_mn *amn = container_of(mn, struct amdgpu_mn, mn);
	struct interval_tree_node *it;

	/* notification is exclusive, but interval is inclusive */
	end -= 1;

#if defined(HAVE_5ARGS_INVALIDATE_RANGE_START)
	if (amdgpu_mn_read_lock(amn, blockable))
		 return -EAGAIN;
#else
	amdgpu_mn_read_lock(amn);
#endif

	it = interval_tree_iter_first(&amn->objects, start, end);
	while (it) {
		struct amdgpu_mn_node *node;

		node = container_of(it, struct amdgpu_mn_node, it);
		it = interval_tree_iter_next(it, start, end);

		amdgpu_mn_invalidate_node(node, start, end);
	}

#if defined(HAVE_5ARGS_INVALIDATE_RANGE_START)
	return 0;
#endif
}


/**
 * amdgpu_mn_invalidate_range_start_hsa - callback to notify about mm change
 *
 * @mn: our notifier
 * @mm: the mm this callback is about
 * @start: start of updated range
 * @end: end of updated range
 *
 * We temporarily evict all BOs between start and end. This
 * necessitates evicting all user-mode queues of the process. The BOs
 * are restorted in amdgpu_mn_invalidate_range_end_hsa.
 */
#if defined(HAVE_5ARGS_INVALIDATE_RANGE_START)
static int amdgpu_mn_invalidate_range_start_hsa(struct mmu_notifier *mn,
						 struct mm_struct *mm,
						 unsigned long start,
						 unsigned long end,
						 bool blockable)
#else
static void amdgpu_mn_invalidate_range_start_hsa(struct mmu_notifier *mn,
						 struct mm_struct *mm,
						 unsigned long start,
						 unsigned long end)
#endif
{
	struct amdgpu_mn *amn = container_of(mn, struct amdgpu_mn, mn);
	struct interval_tree_node *it;
	unsigned long userptr;

	/* notification is exclusive, but interval is inclusive */
	end -= 1;

#if defined(HAVE_5ARGS_INVALIDATE_RANGE_START)
	if (amdgpu_mn_read_lock(amn, blockable))
		 return -EAGAIN;
#else
	amdgpu_mn_read_lock(amn);
#endif

	it = interval_tree_iter_first(&amn->objects, start, end);
	while (it) {
		struct amdgpu_mn_node *node;
		struct amdgpu_bo *bo;

		node = container_of(it, struct amdgpu_mn_node, it);
		it = interval_tree_iter_next(it, start, end);

		list_for_each_entry(bo, &node->bos, mn_list) {
			struct kgd_mem *mem = bo->kfd_bo;

			if (amdgpu_ttm_tt_affect_userptr(bo->tbo.ttm,
							 start, end, &userptr))
				amdgpu_amdkfd_evict_userptr(mem, mm);
		}
	}

#if defined(HAVE_5ARGS_INVALIDATE_RANGE_START)
	return 0;
#endif
}

#endif

/**
 * amdgpu_mn_invalidate_range_end - callback to notify about mm change
 *
 * @mn: our notifier
 * @mm: the mm this callback is about
 * @start: start of updated range
 * @end: end of updated range
 *
 * Release the lock again to allow new command submissions.
 */
static void amdgpu_mn_invalidate_range_end(struct mmu_notifier *mn,
#ifdef HAVE_2ARGS_INVALIDATE_RANGE_START
			const struct mmu_notifier_range *range)
#else
					   struct mm_struct *mm,
					   unsigned long start,
					   unsigned long end)
#endif
{
	struct amdgpu_mn *amn = container_of(mn, struct amdgpu_mn, mn);

	amdgpu_mn_read_unlock(amn);
}

static const struct mmu_notifier_ops amdgpu_mn_ops[] = {
	[AMDGPU_MN_TYPE_GFX] = {
#ifdef HAVE_MMU_NOTIFIER_PUT
		.free_notifier = amdgpu_mn_free,
#endif
		.release = amdgpu_mn_release,
		.invalidate_range_start = amdgpu_mn_invalidate_range_start_gfx,
		.invalidate_range_end = amdgpu_mn_invalidate_range_end,
	},
	[AMDGPU_MN_TYPE_HSA] = {
#ifdef HAVE_MMU_NOTIFIER_PUT
		.free_notifier = amdgpu_mn_free,
#endif
		.release = amdgpu_mn_release,
		.invalidate_range_start = amdgpu_mn_invalidate_range_start_hsa,
		.invalidate_range_end = amdgpu_mn_invalidate_range_end,
	},
};

/* Low bits of any reasonable mm pointer will be unused due to struct
 * alignment. Use these bits to make a unique key from the mm pointer
 * and notifier type.
 */
#define AMDGPU_MN_KEY(mm, type) ((unsigned long)(mm) + (type))

/**
 * amdgpu_mn_get - create notifier context
 *
 * @adev: amdgpu device pointer
 * @type: type of MMU notifier context
 *
 * Creates a notifier context for current->mm.
 */
struct amdgpu_mn *amdgpu_mn_get(struct amdgpu_device *adev,
				enum amdgpu_mn_type type)
{
	struct mm_struct *mm = current->mm;
	struct amdgpu_mn *amn;
	unsigned long key = AMDGPU_MN_KEY(mm, type);
	int r;

	mutex_lock(&adev->mn_lock);
#ifndef HAVE_DOWN_WRITE_KILLABLE
	down_write(&mm->mmap_sem);
#else
	if (down_write_killable(&mm->mmap_sem)) {
		mutex_unlock(&adev->mn_lock);
		return ERR_PTR(-EINTR);
	}
#endif

	hash_for_each_possible(adev->mn_hash, amn, node, key)
		if (AMDGPU_MN_KEY(amn->mm, amn->type) == key)
			goto release_locks;

	amn = kzalloc(sizeof(*amn), GFP_KERNEL);
	if (!amn) {
		amn = ERR_PTR(-ENOMEM);
		goto release_locks;
	}

	amn->adev = adev;
	amn->mm = mm;
	init_rwsem(&amn->lock);
	amn->type = type;
	amn->mn.ops = &amdgpu_mn_ops[type];
#ifndef HAVE_TREE_INSERT_HAVE_RB_ROOT_CACHED
	amn->objects = RB_ROOT;
#else
	amn->objects = RB_ROOT_CACHED;
#endif
	mutex_init(&amn->read_lock);
	atomic_set(&amn->recursion, 0);

	r = __mmu_notifier_register(&amn->mn, mm);
	if (r)
		goto free_amn;

	hash_add(adev->mn_hash, &amn->node, AMDGPU_MN_KEY(mm, type));

release_locks:
	up_write(&mm->mmap_sem);
	mutex_unlock(&adev->mn_lock);

	return amn;

free_amn:
	up_write(&mm->mmap_sem);
	mutex_unlock(&adev->mn_lock);
	kfree(amn);

	return ERR_PTR(r);
}

/**
 * amdgpu_mn_register - register a BO for notifier updates
 *
 * @bo: amdgpu buffer object
 * @addr: userptr addr we should monitor
 *
 * Registers an MMU notifier for the given BO at the specified address.
 * Returns 0 on success, -ERRNO if anything goes wrong.
 */
int amdgpu_mn_register(struct amdgpu_bo *bo, unsigned long addr)
{
	unsigned long end = addr + amdgpu_bo_size(bo) - 1;
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->tbo.bdev);
	enum amdgpu_mn_type type =
		bo->kfd_bo ? AMDGPU_MN_TYPE_HSA : AMDGPU_MN_TYPE_GFX;
	struct amdgpu_mn *amn;
	struct amdgpu_mn_node *node = NULL, *new_node;
	struct list_head bos;
	struct interval_tree_node *it;

	amn = amdgpu_mn_get(adev, type);
	if (IS_ERR(amn))
		return PTR_ERR(amn);

	new_node = kmalloc(sizeof(*new_node), GFP_KERNEL);
	if (!new_node)
		return -ENOMEM;

	INIT_LIST_HEAD(&bos);

	down_write(&amn->lock);

	while ((it = interval_tree_iter_first(&amn->objects, addr, end))) {
		kfree(node);
		node = container_of(it, struct amdgpu_mn_node, it);
		interval_tree_remove(&node->it, &amn->objects);
		addr = min(it->start, addr);
		end = max(it->last, end);
		list_splice(&node->bos, &bos);
	}

	if (!node)
		node = new_node;
	else
		kfree(new_node);

	bo->mn = amn;

	node->it.start = addr;
	node->it.last = end;
	INIT_LIST_HEAD(&node->bos);
	list_splice(&bos, &node->bos);
	list_add(&bo->mn_list, &node->bos);

	interval_tree_insert(&node->it, &amn->objects);

	up_write(&amn->lock);

	return 0;
}

/**
 * amdgpu_mn_unregister - unregister a BO for notifier updates
 *
 * @bo: amdgpu buffer object
 *
 * Remove any registration of MMU notifier updates from the buffer object.
 */
void amdgpu_mn_unregister(struct amdgpu_bo *bo)
{
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->tbo.bdev);
	struct amdgpu_mn *amn;
	struct list_head *head;

	mutex_lock(&adev->mn_lock);

	amn = bo->mn;
	if (amn == NULL) {
		mutex_unlock(&adev->mn_lock);
		return;
	}

	down_write(&amn->lock);

	/* save the next list entry for later */
	head = bo->mn_list.next;

	bo->mn = NULL;
	list_del_init(&bo->mn_list);

	if (list_empty(head)) {
		struct amdgpu_mn_node *node;

		node = container_of(head, struct amdgpu_mn_node, bos);
		interval_tree_remove(&node->it, &amn->objects);
		kfree(node);
	}

	up_write(&amn->lock);
	mutex_unlock(&adev->mn_lock);
}

#else /* HAVE_AMDKCL_HMM_MIRROR_ENABLED */

/**
 * amdgpu_mn_invalidate_gfx - callback to notify about mm change
 *
 * @mni: the range (mm) is about to update
 * @range: details on the invalidation
 * @cur_seq: Value to pass to mmu_interval_set_seq()
 *
 * Block for operations on BOs to finish and mark pages as accessed and
 * potentially dirty.
 */
static bool amdgpu_mn_invalidate_gfx(struct mmu_interval_notifier *mni,
				     const struct mmu_notifier_range *range,
				     unsigned long cur_seq)
{
	struct amdgpu_bo *bo = container_of(mni, struct amdgpu_bo, notifier);
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->tbo.bdev);
	long r;

	if (!mmu_notifier_range_blockable(range))
		return false;

	mutex_lock(&adev->notifier_lock);

	mmu_interval_set_seq(mni, cur_seq);

	r = dma_resv_wait_timeout(amdkcl_ttm_resvp(&bo->tbo), true, false,
				  MAX_SCHEDULE_TIMEOUT);
	mutex_unlock(&adev->notifier_lock);
	if (r <= 0)
		DRM_ERROR("(%ld) failed to wait for user bo\n", r);
	return true;
}

static const struct mmu_interval_notifier_ops amdgpu_mn_gfx_ops = {
	.invalidate = amdgpu_mn_invalidate_gfx,
};

/**
 * amdgpu_mn_invalidate_hsa - callback to notify about mm change
 *
 * @mni: the range (mm) is about to update
 * @range: details on the invalidation
 * @cur_seq: Value to pass to mmu_interval_set_seq()
 *
 * We temporarily evict the BO attached to this range. This necessitates
 * evicting all user-mode queues of the process.
 */
static bool amdgpu_mn_invalidate_hsa(struct mmu_interval_notifier *mni,
				     const struct mmu_notifier_range *range,
				     unsigned long cur_seq)
{
	struct amdgpu_bo *bo = container_of(mni, struct amdgpu_bo, notifier);
	struct amdgpu_device *adev = amdgpu_ttm_adev(bo->tbo.bdev);

	if (!mmu_notifier_range_blockable(range))
		return false;

	mutex_lock(&adev->notifier_lock);

	mmu_interval_set_seq(mni, cur_seq);

	amdgpu_amdkfd_evict_userptr(bo->kfd_bo, bo->notifier.mm);
	mutex_unlock(&adev->notifier_lock);

	return true;
}

static const struct mmu_interval_notifier_ops amdgpu_mn_hsa_ops = {
	.invalidate = amdgpu_mn_invalidate_hsa,
};

/**
 * amdgpu_mn_register - register a BO for notifier updates
 *
 * @bo: amdgpu buffer object
 * @addr: userptr addr we should monitor
 *
 * Registers a mmu_notifier for the given BO at the specified address.
 * Returns 0 on success, -ERRNO if anything goes wrong.
 */
int amdgpu_mn_register(struct amdgpu_bo *bo, unsigned long addr)
{
	if (bo->kfd_bo)
		return mmu_interval_notifier_insert(&bo->notifier, current->mm,
						    addr, amdgpu_bo_size(bo),
						    &amdgpu_mn_hsa_ops);
	return mmu_interval_notifier_insert(&bo->notifier, current->mm, addr,
					    amdgpu_bo_size(bo),
					    &amdgpu_mn_gfx_ops);
}

/**
 * amdgpu_mn_unregister - unregister a BO for notifier updates
 *
 * @bo: amdgpu buffer object
 *
 * Remove any registration of mmu notifier updates from the buffer object.
 */
void amdgpu_mn_unregister(struct amdgpu_bo *bo)
{
	if (!bo->notifier.mm)
		return;
	mmu_interval_notifier_remove(&bo->notifier);
	bo->notifier.mm = NULL;
}

#ifndef HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT
/* flags used by HMM internal, not related to CPU/GPU PTE flags */
const uint64_t hmm_range_flags[HMM_PFN_FLAG_MAX] = {
	(1 << 0), /* HMM_PFN_VALID */
	(1 << 1), /* HMM_PFN_WRITE */
	0 /* HMM_PFN_DEVICE_PRIVATE */
};

const uint64_t hmm_range_values[HMM_PFN_VALUE_MAX] = {
	0xfffffffffffffffeUL, /* HMM_PFN_ERROR */
	0, /* HMM_PFN_NONE */
	0xfffffffffffffffcUL /* HMM_PFN_SPECIAL */
};
#endif

int amdgpu_hmm_range_get_pages(struct mmu_interval_notifier *notifier,
			       struct mm_struct *mm, struct page **pages,
			       uint64_t start, uint64_t npages,
			       struct hmm_range **phmm_range, bool readonly,
			       bool mmap_locked, void *owner)
{
	struct hmm_range *hmm_range;
	unsigned long timeout;
	unsigned long i;
	unsigned long *pfns;
	int r = 0;

	hmm_range = kzalloc(sizeof(*hmm_range), GFP_KERNEL);
	if (unlikely(!hmm_range))
		return -ENOMEM;

	pfns = kvmalloc_array(npages, sizeof(*pfns), GFP_KERNEL);
	if (unlikely(!pfns)) {
		r = -ENOMEM;
		goto out_free_range;
	}

	hmm_range->notifier = notifier;
#ifndef HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT
	hmm_range->flags = hmm_range_flags;
	hmm_range->values = hmm_range_values;
	hmm_range->pfn_shift = PAGE_SHIFT;
	hmm_range->default_flags = hmm_range_flags[HMM_PFN_VALID];
	if (!readonly)
		hmm_range->default_flags |= hmm_range->flags[HMM_PFN_WRITE];
	hmm_range->pfns = (uint64_t *)pfns;
#else
	hmm_range->default_flags = HMM_PFN_REQ_FAULT;
	if (!readonly)
		hmm_range->default_flags |= HMM_PFN_REQ_WRITE;
	hmm_range->hmm_pfns = pfns;
#endif
	hmm_range->start = start;
	hmm_range->end = start + npages * PAGE_SIZE;
	hmm_range->dev_private_owner = owner;

	/* Assuming 512MB takes maxmium 1 second to fault page address */
	timeout = max(npages >> 17, 1ULL) * HMM_RANGE_DEFAULT_TIMEOUT;
	timeout = jiffies + msecs_to_jiffies(timeout);

retry:
	hmm_range->notifier_seq = mmu_interval_read_begin(notifier);

	if (likely(!mmap_locked))
		mmap_read_lock(mm);

	r = hmm_range_fault(hmm_range);

	if (likely(!mmap_locked))
		mmap_read_unlock(mm);

#ifndef HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT
	if (unlikely(r <= 0)) {
#else
	if (unlikely(r)) {
#endif
		/*
		 * FIXME: This timeout should encompass the retry from
		 * mmu_interval_read_retry() as well.
		 */
#ifndef HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT
		if ((r == 0 || r == -EBUSY) && !time_after(jiffies, timeout))
#else
		if (r == -EBUSY && !time_after(jiffies, timeout))
#endif
			goto retry;
		goto out_free_pfns;
	}

	/*
	 * Due to default_flags, all pages are HMM_PFN_VALID or
	 * hmm_range_fault() fails. FIXME: The pages cannot be touched outside
	 * the notifier_lock, and mmu_interval_read_retry() must be done first.
	 */
	for (i = 0; pages && i < npages; i++) {
#ifndef HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT
		pages[i] = hmm_device_entry_to_page(hmm_range, pfns[i]);
		if (unlikely(!pages[i])) {
			pr_err("Page fault failed for pfn[%lu] = 0x%llx\n",
			       i, pfns[i]);
			r = -ENOMEM;

			goto out_free_pfns;
		}
#else
		pages[i] = hmm_pfn_to_page(pfns[i]);
#endif
	}

	*phmm_range = hmm_range;

	return 0;

out_free_pfns:
	kvfree(pfns);
out_free_range:
	kfree(hmm_range);

	return r;
}

int amdgpu_hmm_range_get_pages_done(struct hmm_range *hmm_range)
{
	int r;

	r = mmu_interval_read_retry(hmm_range->notifier,
				    hmm_range->notifier_seq);
#ifndef HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT
	kvfree(hmm_range->pfns);
#else
	kvfree(hmm_range->hmm_pfns);
#endif
	kfree(hmm_range);

	return r;
}
#endif /* HAVE_AMDKCL_HMM_MIRROR_ENABLED */
