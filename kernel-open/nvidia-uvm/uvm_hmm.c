/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "uvm_hmm.h"

// Support for HMM ( https://docs.kernel.org/mm/hmm.html ):

#ifdef NVCPU_X86_64
static bool uvm_disable_hmm = false;
MODULE_PARM_DESC(uvm_disable_hmm,
                 "Force-disable HMM functionality in the UVM driver. "
                 "Default: false (HMM is enabled if possible). "
                 "However, even with uvm_disable_hmm=false, HMM will not be "
                 "enabled if is not supported in this driver build "
                 "configuration, or if ATS settings conflict with HMM.");
#else
// TODO: Bug 4103580: UVM: HMM: implement HMM support on ARM64 (aarch64)
// So far, we've only tested HMM on x86_64 and aarch64 and it is broken on
// aarch64 so disable it by default everywhere except x86_64.
static bool uvm_disable_hmm = true;
MODULE_PARM_DESC(uvm_disable_hmm,
                 "Force-disable HMM functionality in the UVM driver. "
                 "Default: true (HMM is not enabled on this CPU architecture). "
                 "However, even with uvm_disable_hmm=false, HMM will not be "
                 "enabled if is not supported in this driver build "
                 "configuration, or if ATS settings conflict with HMM.");
#endif

module_param(uvm_disable_hmm, bool, 0444);

#if UVM_IS_CONFIG_HMM()

#include <linux/hmm.h>
#include <linux/rmap.h>
#include <linux/migrate.h>
#include <linux/userfaultfd_k.h>
#include <linux/memremap.h>
#include <linux/wait.h>

#include "uvm_common.h"
#include "uvm_gpu.h"
#include "uvm_pmm_gpu.h"
#include "uvm_hal_types.h"
#include "uvm_push.h"
#include "uvm_hal.h"
#include "uvm_va_block_types.h"
#include "uvm_va_space_mm.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_range_tree.h"
#include "uvm_pmm_sysmem.h"
#include "uvm_lock.h"
#include "uvm_api.h"
#include "uvm_va_policy.h"
#include "uvm_tools.h"

// The function nv_PageSwapCache() wraps the check for page swap cache flag in
// order to support a wide variety of kernel versions.
// The function PageSwapCache() is removed after 32f51ead3d77 ("mm: remove
// PageSwapCache") in v6.12-rc1.
// The function folio_test_swapcache() was added in Linux 5.16 (d389a4a811551
// "mm: Add folio flag manipulation functions")
// Systems with HMM patches backported to 5.14 are possible, but those systems
// do not include folio_test_swapcache()
// TODO: Bug 4050579: Remove this when migration of swap cached pages is updated
static __always_inline bool nv_PageSwapCache(struct page *page)
{
#if defined(NV_FOLIO_TEST_SWAPCACHE_PRESENT)
    return folio_test_swapcache(page_folio(page));
#else
    return PageSwapCache(page);
#endif
}

static NV_STATUS gpu_chunk_add(uvm_va_block_t *va_block,
                               uvm_page_index_t page_index,
                               struct page *page);

typedef struct
{
    uvm_processor_id_t processor_id;
    uvm_processor_id_t new_residency;
    uvm_va_block_t *va_block;
    uvm_va_block_retry_t *va_block_retry;
    uvm_service_block_context_t *service_context;
    uvm_page_mask_t page_mask;
    uvm_page_mask_t same_devmem_page_mask;
} uvm_hmm_gpu_fault_event_t;

typedef struct
{
    uvm_va_block_t *va_block;
    uvm_va_block_retry_t *va_block_retry;
    uvm_service_block_context_t *service_context;
    uvm_va_block_region_t region;
    uvm_processor_id_t dest_id;
    uvm_make_resident_cause_t cause;
    uvm_page_mask_t page_mask;
    uvm_page_mask_t same_devmem_page_mask;
} uvm_hmm_migrate_event_t;

typedef struct
{
    uvm_processor_id_t processor_id;
    uvm_va_block_t *va_block;
    uvm_va_block_retry_t *va_block_retry;
    uvm_service_block_context_t *service_context;
    uvm_page_mask_t page_mask;
    uvm_page_mask_t same_devmem_page_mask;
} uvm_hmm_devmem_fault_context_t;

bool uvm_hmm_is_enabled_system_wide(void)
{
    if (uvm_disable_hmm)
        return false;

    if (g_uvm_global.ats.enabled)
        return false;

    // Confidential Computing and HMM impose mutually exclusive constraints. In
    // Confidential Computing the GPU can only access pages resident in vidmem,
    // but in HMM pages may be required to be resident in sysmem: file backed
    // VMAs, huge pages, etc.
    if (g_uvm_global.conf_computing_enabled)
        return false;

    return uvm_va_space_mm_enabled_system();
}

bool uvm_hmm_is_enabled(uvm_va_space_t *va_space)
{
    return uvm_hmm_is_enabled_system_wide() &&
           uvm_va_space_mm_enabled(va_space) &&
           !(va_space->initialization_flags & UVM_INIT_FLAGS_DISABLE_HMM);
}

static uvm_va_block_t *hmm_va_block_from_node(uvm_range_tree_node_t *node)
{
    if (!node)
        return NULL;
    return container_of(node, uvm_va_block_t, hmm.node);
}

// Copies the contents of the source device-private page to the
// destination CPU page. This will invalidate mappings, so cannot be
// called while holding any va_block locks.
static NV_STATUS hmm_copy_devmem_page(struct page *dst_page, struct page *src_page)
{
    uvm_tracker_t tracker = UVM_TRACKER_INIT();
    uvm_gpu_phys_address_t src_addr;
    uvm_gpu_phys_address_t dst_addr;
    uvm_gpu_chunk_t *gpu_chunk;
    NvU64 dma_addr;
    uvm_push_t push;
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu;

    // Holding a reference on the device-private page ensures the gpu
    // is already retained. This is because when a GPU is unregistered
    // all device-private pages are migrated back to the CPU and freed
    // before releasing the GPU. Therefore if we could get a reference
    // to the page the GPU must be retained.
    UVM_ASSERT(is_device_private_page(src_page) && page_count(src_page));
    gpu_chunk = uvm_pmm_devmem_page_to_chunk(src_page);
    gpu = uvm_gpu_chunk_get_gpu(gpu_chunk);
    status = uvm_mmu_chunk_map(gpu_chunk);
    if (status != NV_OK)
        goto out;

    status = uvm_gpu_map_cpu_page(gpu, dst_page, &dma_addr);
    if (status != NV_OK)
        goto out_unmap_gpu;

    dst_addr = uvm_gpu_phys_address(UVM_APERTURE_SYS, dma_addr);
    src_addr = uvm_gpu_phys_address(UVM_APERTURE_VID, gpu_chunk->address);
    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_GPU_TO_CPU,
                                    &tracker,
                                    &push,
                                    "Copy for remote process fault");
    if (status != NV_OK)
        goto out_unmap_cpu;

    gpu->parent->ce_hal->memcopy(&push,
                                 uvm_gpu_address_copy(gpu, dst_addr),
                                 uvm_gpu_address_copy(gpu, src_addr),
                                 PAGE_SIZE);
    uvm_push_end(&push);
    status = uvm_tracker_add_push_safe(&tracker, &push);
    if (status == NV_OK)
        status = uvm_tracker_wait_deinit(&tracker);

out_unmap_cpu:
    uvm_parent_gpu_unmap_cpu_pages(gpu->parent, dma_addr, PAGE_SIZE);

out_unmap_gpu:
    uvm_mmu_chunk_unmap(gpu_chunk, NULL);

out:
    return status;
}

static NV_STATUS uvm_hmm_pmm_gpu_evict_pfn(unsigned long pfn)
{
    unsigned long src_pfn = 0;
    unsigned long dst_pfn = 0;
    struct page *dst_page;
    NV_STATUS status = NV_OK;
    int ret;

    ret = migrate_device_range(&src_pfn, pfn, 1);
    if (ret)
        return errno_to_nv_status(ret);

    if (src_pfn & MIGRATE_PFN_MIGRATE) {

        dst_page = alloc_page(GFP_HIGHUSER_MOVABLE);
        if (!dst_page) {
            status = NV_ERR_NO_MEMORY;
            goto out;
        }

        lock_page(dst_page);

        // We can't fail eviction because we need to free the device-private
        // pages so the GPU can be unregistered. So the best we can do is warn
        // on any failures and zero the uninitialized page. This could result
        // in data loss in the application but failures are not expected.
        if (hmm_copy_devmem_page(dst_page, migrate_pfn_to_page(src_pfn)) != NV_OK)
            memzero_page(dst_page, 0, PAGE_SIZE);
        dst_pfn = migrate_pfn(page_to_pfn(dst_page));
        migrate_device_pages(&src_pfn, &dst_pfn, 1);
    }

out:
    migrate_device_finalize(&src_pfn, &dst_pfn, 1);

    if (!(src_pfn & MIGRATE_PFN_MIGRATE))
        status = NV_ERR_BUSY_RETRY;

    return status;
}

void uvm_hmm_va_space_initialize(uvm_va_space_t *va_space)
{
    uvm_hmm_va_space_t *hmm_va_space = &va_space->hmm;

    if (!uvm_hmm_is_enabled(va_space))
        return;

    uvm_range_tree_init(&hmm_va_space->blocks);
    uvm_mutex_init(&hmm_va_space->blocks_lock, UVM_LOCK_ORDER_LEAF);

    return;
}

void uvm_hmm_va_space_destroy(uvm_va_space_t *va_space)
{
    uvm_hmm_va_space_t *hmm_va_space = &va_space->hmm;
    uvm_range_tree_node_t *node, *next;
    uvm_va_block_t *va_block;

    if (!uvm_hmm_is_enabled(va_space))
        return;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    // The blocks_lock is not needed when the va_space lock is held for write.
    uvm_range_tree_for_each_safe(node, next, &hmm_va_space->blocks) {
        va_block = hmm_va_block_from_node(node);
        uvm_range_tree_remove(&hmm_va_space->blocks, node);
        mmu_interval_notifier_remove(&va_block->hmm.notifier);
        uvm_va_block_kill(va_block);
    }
}

static void hmm_va_block_unregister_gpu(uvm_va_block_t *va_block,
                                        uvm_gpu_t *gpu,
                                        struct mm_struct *mm)
{
    uvm_va_policy_node_t *node;

    uvm_mutex_lock(&va_block->lock);

    // Reset preferred location and accessed-by of policy nodes if needed.
    uvm_for_each_va_policy_node_in(node, va_block, va_block->start, va_block->end) {
        if (uvm_va_policy_preferred_location_equal(&node->policy, gpu->id, NUMA_NO_NODE)) {
            node->policy.preferred_location = UVM_ID_INVALID;
            node->policy.preferred_nid = NUMA_NO_NODE;
        }

        uvm_processor_mask_clear(&node->policy.accessed_by, gpu->id);
    }

    // Migrate and free any remaining resident allocations on this GPU.
    uvm_va_block_unregister_gpu_locked(va_block, gpu, mm);

    uvm_mutex_unlock(&va_block->lock);
}

void uvm_hmm_unregister_gpu(uvm_va_space_t *va_space, uvm_gpu_t *gpu, struct mm_struct *mm)
{
    uvm_range_tree_node_t *node;
    uvm_va_block_t *va_block;
    unsigned long devmem_start;
    unsigned long devmem_end;
    unsigned long pfn;
    bool retry;

    if (!uvm_hmm_is_enabled(va_space))
        return;

    devmem_start = gpu->parent->devmem->pagemap.range.start + gpu->mem_info.phys_start;
    devmem_end = devmem_start + gpu->mem_info.size;

    if (mm)
        uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);

    // There could be pages with page->zone_device_data pointing to the va_space
    // which may be about to be freed. Migrate those back to the CPU so we don't
    // fault on them. Normally infinite retries are bad, but we don't have any
    // option here. Device-private pages can't be pinned so migration should
    // eventually succeed. Even if we did eventually bail out of the loop we'd
    // just stall in memunmap_pages() anyway.
    do {
        retry = false;

        for (pfn = __phys_to_pfn(devmem_start); pfn <= __phys_to_pfn(devmem_end); pfn++) {
            struct page *page = pfn_to_page(pfn);

            UVM_ASSERT(is_device_private_page(page));

            // This check is racy because nothing stops the page being freed and
            // even reused. That doesn't matter though - worst case the
            // migration fails, we retry and find the va_space doesn't match.
            if (uvm_pmm_devmem_page_to_va_space(page) == va_space)
                if (uvm_hmm_pmm_gpu_evict_pfn(pfn) != NV_OK)
                    retry = true;
        }
    } while (retry);

    uvm_range_tree_for_each(node, &va_space->hmm.blocks) {
        va_block = hmm_va_block_from_node(node);

        hmm_va_block_unregister_gpu(va_block, gpu, mm);
    }
}

static void hmm_va_block_remove_gpu_va_space(uvm_va_block_t *va_block,
                                             uvm_gpu_va_space_t *gpu_va_space,
                                             uvm_va_block_context_t *va_block_context)
{
    uvm_mutex_lock(&va_block->lock);

    uvm_va_block_remove_gpu_va_space(va_block, gpu_va_space, va_block_context);

    uvm_mutex_unlock(&va_block->lock);

    // TODO: Bug 3660922: Need to handle read duplication at some point.
    // See range_remove_gpu_va_space_managed().
}

void uvm_hmm_remove_gpu_va_space(uvm_va_space_t *va_space,
                                 uvm_gpu_va_space_t *gpu_va_space,
                                 struct mm_struct *mm)
{
    uvm_va_block_context_t *va_block_context;
    uvm_range_tree_node_t *node, *next;
    uvm_va_block_t *va_block;

    if (!uvm_hmm_is_enabled(va_space))
        return;

    if (mm)
        uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);

    va_block_context = uvm_va_space_block_context(va_space, mm);

    uvm_range_tree_for_each_safe(node, next, &va_space->hmm.blocks) {
        va_block = hmm_va_block_from_node(node);

        hmm_va_block_remove_gpu_va_space(va_block, gpu_va_space, va_block_context);
    }
}

static bool hmm_invalidate(uvm_va_block_t *va_block,
                           const struct mmu_notifier_range *range,
                           unsigned long cur_seq)
{
    uvm_thread_context_t *uvm_context = uvm_thread_context();
    struct mmu_interval_notifier *mni = &va_block->hmm.notifier;
    struct mm_struct *mm = mni->mm;
    uvm_va_block_context_t *va_block_context;
    uvm_va_block_region_t region;
    NvU64 start, end;
    uvm_processor_id_t id;
    NV_STATUS status = NV_OK;

    // The MMU_NOTIFY_RELEASE event isn't really needed since mn_itree_release()
    // doesn't remove the interval notifiers from the struct_mm so there will
    // be a full range MMU_NOTIFY_UNMAP event after the release from
    // unmap_vmas() during exit_mmap().
    if (range->event == MMU_NOTIFY_SOFT_DIRTY || range->event == MMU_NOTIFY_RELEASE)
        return true;

    // Blockable is only set false by
    // mmu_notifier_invalidate_range_start_nonblock() which is only called in
    // __oom_reap_task_mm().
    if (!mmu_notifier_range_blockable(range))
        return false;

    // We only ignore invalidations in this context whilst holding the
    // va_block lock. This prevents deadlock when try_to_migrate()
    // calls the notifier, but holding the lock prevents other threads
    // invalidating PTEs so we can safely assume the results of
    // migrate_vma_setup() are correct.
    if (uvm_context->ignore_hmm_invalidate_va_block == va_block ||
        ((range->event == MMU_NOTIFY_MIGRATE || range->event == MMU_NOTIFY_EXCLUSIVE) &&
         range->owner == &g_uvm_global))
        return true;

    va_block_context = uvm_va_block_context_alloc(mm);
    if (!va_block_context)
        return true;

    uvm_mutex_lock(&va_block->lock);

    // mmu_interval_notifier_remove() is always called before marking a
    // va_block as dead so this va_block has to be alive.
    UVM_ASSERT(!uvm_va_block_is_dead(va_block));

    // Note: unmap_vmas() does MMU_NOTIFY_UNMAP [0, 0xffffffffffffffff]
    // Also note that hmm_invalidate() can be called when a new va_block is not
    // yet inserted into the va_space->hmm.blocks table while the original
    // va_block is being split. The original va_block may have its end address
    // updated before the mmu interval notifier is updated so this invalidate
    // may be for a range past the va_block end address.
    start = range->start;
    end = (range->end == ULONG_MAX) ? range->end : range->end - 1;
    if (start < va_block->start)
        start = va_block->start;
    if (end > va_block->end)
        end = va_block->end;
    if (start > end)
        goto unlock;

    // These will be equal if no other thread causes an invalidation
    // whilst the va_block lock was dropped.
    uvm_context->hmm_invalidate_seqnum++;
    va_block->hmm.changed++;

    mmu_interval_set_seq(mni, cur_seq);

    region = uvm_va_block_region_from_start_end(va_block, start, end);

    va_block_context->hmm.vma = NULL;

    // We only need to unmap GPUs since Linux handles the CPUs.
    for_each_gpu_id_in_mask(id, &va_block->mapped) {
        status = uvm_va_block_unmap(va_block,
                                    va_block_context,
                                    id,
                                    region,
                                    uvm_va_block_map_mask_get(va_block, id),
                                    &va_block->tracker);
        // Note that the va_block lock can be dropped, relocked, and
        // NV_ERR_MORE_PROCESSING_REQUIRED returned.
        if (status != NV_OK)
            break;
    }

    if (range->event == MMU_NOTIFY_UNMAP || range->event == MMU_NOTIFY_CLEAR)
        uvm_va_block_munmap_region(va_block, region);

    if (status == NV_OK)
        status = uvm_tracker_wait(&va_block->tracker);

    // Remove stale HMM struct page pointers to system memory.
    uvm_va_block_remove_cpu_chunks(va_block, region);

unlock:
    uvm_mutex_unlock(&va_block->lock);

    uvm_va_block_context_free(va_block_context);

    UVM_ASSERT(status == NV_OK);
    return true;
}

static bool uvm_hmm_invalidate_entry(struct mmu_interval_notifier *mni,
                                     const struct mmu_notifier_range *range,
                                     unsigned long cur_seq)
{
    uvm_va_block_t *va_block = container_of(mni, uvm_va_block_t, hmm.notifier);

    UVM_ENTRY_RET(hmm_invalidate(va_block, range, cur_seq));
}

static const struct mmu_interval_notifier_ops uvm_hmm_notifier_ops =
{
    .invalidate = uvm_hmm_invalidate_entry,
};

NV_STATUS uvm_hmm_va_block_find(uvm_va_space_t *va_space,
                                NvU64 addr,
                                uvm_va_block_t **va_block_ptr)
{
    uvm_range_tree_node_t *node;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_rwsem_locked(&va_space->lock);

    uvm_mutex_lock(&va_space->hmm.blocks_lock);
    node = uvm_range_tree_find(&va_space->hmm.blocks, addr);
    uvm_mutex_unlock(&va_space->hmm.blocks_lock);

    if (!node)
        return NV_ERR_OBJECT_NOT_FOUND;

    *va_block_ptr = hmm_va_block_from_node(node);

    return NV_OK;
}

static int migrate_vma_setup_locked(struct migrate_vma *args, uvm_va_block_t *va_block)
{
    uvm_thread_context_t *uvm_context = uvm_thread_context();
    int ret;

    // It's only safe to ignore invalidations whilst doing a migration
    // and holding the va_block lock.
    uvm_assert_mutex_locked(&va_block->lock);
    uvm_context->ignore_hmm_invalidate_va_block = va_block;
    ret = migrate_vma_setup(args);

    // We shouldn't be generating any more invalidations now.
    uvm_context->ignore_hmm_invalidate_va_block = NULL;
    return ret;
}

static bool uvm_hmm_vma_is_valid(struct vm_area_struct *vma,
                                 unsigned long addr,
                                 bool allow_unreadable_vma)
{
    // UVM doesn't support userfaultfd. hmm_range_fault() doesn't support
    // VM_IO or VM_PFNMAP VMAs. It also doesn't support VMAs without VM_READ
    // but we allow those VMAs to have policy set on them.
    // migrate_vma_setup() doesn't support VM_SPECIAL VMAs but that is handled
    // by uvm_hmm_must_use_sysmem() forcing residency to the CPU.
    return vma &&
           addr >= vma->vm_start &&
           !userfaultfd_armed(vma) &&
           !(vma->vm_flags & (VM_IO | VM_PFNMAP)) &&
           !uvm_vma_is_managed(vma) &&
           (allow_unreadable_vma || (vma->vm_flags & VM_READ));
}

static void hmm_va_block_init(uvm_va_block_t *va_block,
                              uvm_va_space_t *va_space,
                              NvU64 start,
                              NvU64 end)
{
    va_block->hmm.va_space = va_space;
    va_block->hmm.node.start = start;
    va_block->hmm.node.end = end;
    uvm_range_tree_init(&va_block->hmm.va_policy_tree);
    uvm_mutex_init(&va_block->hmm.migrate_lock, UVM_LOCK_ORDER_VA_BLOCK_MIGRATE);
}

static NV_STATUS hmm_va_block_find_create(uvm_va_space_t *va_space,
                                          NvU64 addr,
                                          bool allow_unreadable_vma,
                                          struct vm_area_struct **vma_out,
                                          uvm_va_block_t **va_block_ptr)
{
    struct mm_struct *mm;
    struct vm_area_struct *va_block_vma;
    uvm_va_block_t *va_block;
    NvU64 start, end;
    NV_STATUS status;
    int ret;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    mm = va_space->va_space_mm.mm;
    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_space->lock);
    UVM_ASSERT(PAGE_ALIGNED(addr));

    // Note that we have to allow PROT_NONE VMAs so that policies can be set.
    va_block_vma = find_vma(mm, addr);
    if (!uvm_hmm_vma_is_valid(va_block_vma, addr, allow_unreadable_vma))
        return NV_ERR_INVALID_ADDRESS;

    // Since we only hold the va_space read lock, there can be multiple
    // parallel va_block insertions.
    uvm_mutex_lock(&va_space->hmm.blocks_lock);

    va_block = hmm_va_block_from_node(uvm_range_tree_find(&va_space->hmm.blocks, addr));
    if (va_block)
        goto done;

    // The va_block is always created to cover the whole aligned
    // UVM_VA_BLOCK_SIZE interval unless there are existing UVM va_ranges or
    // HMM va_blocks. In that case, the new HMM va_block size is adjusted so it
    // doesn't overlap.
    start = UVM_VA_BLOCK_ALIGN_DOWN(addr);
    end = start + UVM_VA_BLOCK_SIZE - 1;

    // Search for existing UVM va_ranges in the start/end interval and create
    // a maximum interval that doesn't overlap any existing UVM va_ranges.
    // We know that 'addr' is not within a va_range or
    // hmm_va_block_find_create() wouldn't be called.
    status = uvm_range_tree_find_hole_in(&va_space->va_range_tree, addr, &start, &end);
    UVM_ASSERT(status == NV_OK);

    // Search for existing HMM va_blocks in the start/end interval and create
    // a maximum interval that doesn't overlap any existing HMM va_blocks.
    status = uvm_range_tree_find_hole_in(&va_space->hmm.blocks, addr, &start, &end);
    UVM_ASSERT(status == NV_OK);

    // Create a HMM va_block with a NULL va_range pointer.
    status = uvm_va_block_create(NULL, start, end, &va_block);
    if (status != NV_OK)
        goto err_unlock;

    hmm_va_block_init(va_block, va_space, start, end);

    ret = mmu_interval_notifier_insert(&va_block->hmm.notifier,
                                       mm,
                                       start,
                                       end - start + 1,
                                       &uvm_hmm_notifier_ops);
    if (ret) {
        status = errno_to_nv_status(ret);
        goto err_release;
    }

    status = uvm_range_tree_add(&va_space->hmm.blocks, &va_block->hmm.node);
    UVM_ASSERT(status == NV_OK);

done:
    uvm_mutex_unlock(&va_space->hmm.blocks_lock);
    if (vma_out)
        *vma_out = va_block_vma;
    *va_block_ptr = va_block;
    return NV_OK;

err_release:
    uvm_va_block_release(va_block);

err_unlock:
    uvm_mutex_unlock(&va_space->hmm.blocks_lock);
    return status;
}

NV_STATUS uvm_hmm_va_block_find_create(uvm_va_space_t *va_space,
                                       NvU64 addr,
                                       struct vm_area_struct **vma,
                                       uvm_va_block_t **va_block_ptr)
{
    return hmm_va_block_find_create(va_space, addr, false, vma, va_block_ptr);
}

NV_STATUS uvm_hmm_find_vma(struct mm_struct *mm, struct vm_area_struct **vma_out, NvU64 addr)
{
    if (!mm)
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(mm);

    *vma_out = find_vma(mm, addr);
    if (!uvm_hmm_vma_is_valid(*vma_out, addr, false))
        return NV_ERR_INVALID_ADDRESS;

    return NV_OK;
}

bool uvm_hmm_check_context_vma_is_valid(uvm_va_block_t *va_block,
                                        struct vm_area_struct *vma,
                                        uvm_va_block_region_t region)
{
    uvm_assert_mutex_locked(&va_block->lock);

    if (uvm_va_block_is_hmm(va_block)) {
        UVM_ASSERT(vma);
        UVM_ASSERT(va_block->hmm.va_space->va_space_mm.mm == vma->vm_mm);
        uvm_assert_mmap_lock_locked(va_block->hmm.va_space->va_space_mm.mm);
        UVM_ASSERT(vma->vm_start <= uvm_va_block_region_start(va_block, region));
        UVM_ASSERT(vma->vm_end > uvm_va_block_region_end(va_block, region));
    }

    return true;
}

NV_STATUS uvm_hmm_migrate_begin(uvm_va_block_t *va_block)
{
    if (uvm_mutex_trylock(&va_block->hmm.migrate_lock))
        return NV_OK;

    return NV_ERR_BUSY_RETRY;
}

void uvm_hmm_migrate_begin_wait(uvm_va_block_t *va_block)
{
    uvm_mutex_lock(&va_block->hmm.migrate_lock);
}

void uvm_hmm_migrate_finish(uvm_va_block_t *va_block)
{
    uvm_mutex_unlock(&va_block->hmm.migrate_lock);
}

// Migrate the given range [start end] within a va_block to dest_id.
static NV_STATUS hmm_migrate_range(uvm_va_block_t *va_block,
                                   uvm_va_block_retry_t *va_block_retry,
                                   uvm_service_block_context_t *service_context,
                                   uvm_processor_id_t dest_id,
                                   NvU64 start,
                                   NvU64 end,
                                   uvm_migrate_mode_t mode,
                                   uvm_tracker_t *out_tracker)
{
    uvm_va_block_region_t region;
    uvm_va_policy_node_t *node;
    const uvm_va_policy_t *policy;
    NV_STATUS status = NV_OK;

    uvm_hmm_migrate_begin_wait(va_block);
    uvm_mutex_lock(&va_block->lock);

    uvm_for_each_va_policy_in(policy, va_block, start, end, node, region) {
        // Even though UVM_VA_BLOCK_RETRY_LOCKED() may unlock and relock the
        // va_block lock, the policy remains valid because we hold the mmap
        // lock so munmap can't remove the policy, and the va_space lock so the
        // policy APIs can't change the policy.
        status = UVM_VA_BLOCK_RETRY_LOCKED(va_block,
                                           va_block_retry,
                                           uvm_va_block_migrate_locked(va_block,
                                                                       va_block_retry,
                                                                       service_context,
                                                                       region,
                                                                       dest_id,
                                                                       mode,
                                                                       out_tracker));
        if (status != NV_OK)
            break;
    }

    uvm_mutex_unlock(&va_block->lock);
    uvm_hmm_migrate_finish(va_block);

    return status;
}

NV_STATUS uvm_hmm_test_va_block_inject_split_error(uvm_va_space_t *va_space, NvU64 addr)
{
    uvm_va_block_test_t *block_test;
    uvm_va_block_t *va_block;
    NV_STATUS status;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    status = hmm_va_block_find_create(va_space, addr, false, NULL, &va_block);
    if (status != NV_OK)
        return status;

    block_test = uvm_va_block_get_test(va_block);
    if (block_test)
        block_test->inject_split_error = true;

    return NV_OK;
}

typedef struct {
    struct mmu_interval_notifier notifier;
    uvm_va_block_t *existing_block;
} hmm_split_invalidate_data_t;

static bool hmm_split_invalidate(struct mmu_interval_notifier *mni,
                                 const struct mmu_notifier_range *range,
                                 unsigned long cur_seq)
{
    hmm_split_invalidate_data_t *split_data = container_of(mni, hmm_split_invalidate_data_t, notifier);

    uvm_tools_test_hmm_split_invalidate(split_data->existing_block->hmm.va_space);
    hmm_invalidate(split_data->existing_block, range, cur_seq);

    return true;
}

static bool hmm_split_invalidate_entry(struct mmu_interval_notifier *mni,
                                       const struct mmu_notifier_range *range,
                                       unsigned long cur_seq)
{
    UVM_ENTRY_RET(hmm_split_invalidate(mni, range, cur_seq));
}

static const struct mmu_interval_notifier_ops hmm_notifier_split_ops =
{
    .invalidate = hmm_split_invalidate_entry,
};

// Splits existing va_block into two pieces, with new_va_block always after
// va_block. va_block is updated to have new_end. new_end+1 must be page-
// aligned.
//
// Before: [----------- existing ------------]
// After:  [---- existing ----][---- new ----]
//                            ^new_end
//
// On error, va_block is still accessible and is left in its original
// functional state.
static NV_STATUS hmm_split_block(uvm_va_block_t *va_block,
                                 NvU64 new_end,
                                 uvm_va_block_t **new_block_ptr)
{
    uvm_va_space_t *va_space = va_block->hmm.va_space;
    struct mm_struct *mm = va_space->va_space_mm.mm;
    hmm_split_invalidate_data_t split_data;
    NvU64 delay_us;
    uvm_va_block_t *new_va_block;
    NV_STATUS status;
    int ret;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    UVM_ASSERT(new_end > va_block->start);
    UVM_ASSERT(new_end < va_block->end);
    UVM_ASSERT(PAGE_ALIGNED(new_end + 1));

    status = uvm_va_block_create(NULL, new_end + 1, va_block->end, &new_va_block);
    if (status != NV_OK)
        return status;

    // Initialize the newly created HMM va_block.
    hmm_va_block_init(new_va_block, va_space, new_va_block->start, new_va_block->end);

    ret = mmu_interval_notifier_insert(&new_va_block->hmm.notifier,
                                       mm,
                                       new_va_block->start,
                                       uvm_va_block_size(new_va_block),
                                       &uvm_hmm_notifier_ops);

    // Since __mmu_notifier_register() was called when the va_space was
    // initially created, we know that mm->notifier_subscriptions is valid
    // and mmu_interval_notifier_insert() can't return ENOMEM.
    // The only error return is for start + length overflowing but we already
    // registered the same address range before so there should be no error.
    UVM_ASSERT(!ret);

    uvm_mutex_lock(&va_block->lock);

    status = uvm_va_block_split_locked(va_block, new_end, new_va_block);
    if (status != NV_OK)
        goto err;

    uvm_mutex_unlock(&va_block->lock);

    // The MMU interval notifier has to be removed in order to resize it.
    // That means there would be a window of time when invalidation callbacks
    // could be missed. To handle this case, we register a temporary notifier
    // to cover the address range while resizing the old notifier (it is
    // OK to have multiple notifiers for the same range, we may simply try to
    // invalidate twice).
    split_data.existing_block = va_block;
    ret = mmu_interval_notifier_insert(&split_data.notifier,
                                       mm,
                                       va_block->start,
                                       new_end - va_block->start + 1,
                                       &hmm_notifier_split_ops);
    UVM_ASSERT(!ret);

    // Delay to allow hmm_sanity test to trigger an mmu_notifier during the
    // critical window where the split invalidate callback is active.
    delay_us = atomic64_read(&va_space->test.split_invalidate_delay_us);
    if (delay_us)
        udelay(delay_us);

    mmu_interval_notifier_remove(&va_block->hmm.notifier);

    // Enable notifications on the old block with the smaller size.
    ret = mmu_interval_notifier_insert(&va_block->hmm.notifier,
                                       mm,
                                       va_block->start,
                                       uvm_va_block_size(va_block),
                                       &uvm_hmm_notifier_ops);
    UVM_ASSERT(!ret);

    mmu_interval_notifier_remove(&split_data.notifier);

    if (new_block_ptr)
        *new_block_ptr = new_va_block;

    return status;

err:
    uvm_mutex_unlock(&va_block->lock);
    mmu_interval_notifier_remove(&new_va_block->hmm.notifier);
    uvm_va_block_release(new_va_block);
    return status;
}

// Check to see if the HMM va_block would overlap the range start/end and
// split it so it can be removed. That breaks down to the following cases:
// start/end could cover all of the HMM va_block ->
//     remove the va_block
// start/end could cover the left part of the HMM va_block ->
//     remove the left part
// start/end could cover the right part of the HMM va_block ->
//     remove the right part
// or start/end could "punch a hole" in the middle and leave the ends intact.
// In each case, only one HMM va_block is removed so return it in out_va_block.
static NV_STATUS split_block_if_needed(uvm_va_block_t *va_block,
                                       NvU64 start,
                                       NvU64 end,
                                       uvm_va_block_t **out_va_block)
{
    uvm_service_block_context_t *service_context;
    uvm_va_space_t *va_space;
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    uvm_va_block_region_t region;
    NvU64 addr, from, to;
    uvm_va_block_t *new;
    NV_STATUS status = NV_OK;

    if (va_block->start < start) {
        status = hmm_split_block(va_block, start - 1, &new);
        if (status != NV_OK)
            return status;

        // Keep the left part, the right part will be deleted.
        va_block = new;
    }

    if (va_block->end > end) {
        status = hmm_split_block(va_block, end, NULL);
        if (status != NV_OK)
            return status;

        // Keep the right part, the left part will be deleted.
    }

    // Migrate any GPU data to sysmem before destroying the HMM va_block.
    // We do this because the new va_range might be for a UVM external
    // allocation which could be converting an address range that was first
    // operated on by UVM-HMM and the exteral allocation should see that data.
    va_space = va_block->hmm.va_space;
    mm = va_space->va_space_mm.mm;

    service_context = uvm_service_block_context_alloc(mm);
    if (!service_context)
        return NV_ERR_NO_MEMORY;

    *out_va_block = va_block;

    for (addr = va_block->start; addr < va_block->end; addr = to + 1) {
        vma = find_vma_intersection(mm, addr, va_block->end);
        if (!vma)
            break;

        from = max(addr, (NvU64)vma->vm_start);
        to = min(va_block->end, (NvU64)vma->vm_end - 1);
        region = uvm_va_block_region_from_start_end(va_block, from, to);

        if (!uvm_hmm_vma_is_valid(vma, from, false))
            continue;

        service_context->block_context->hmm.vma = vma;

        status = hmm_migrate_range(va_block,
                                   NULL,
                                   service_context,
                                   UVM_ID_CPU,
                                   from,
                                   to,
                                   UVM_MIGRATE_MODE_MAKE_RESIDENT_AND_MAP,
                                   NULL);
        if (status != NV_OK)
            break;
    }

    uvm_service_block_context_free(service_context);

    return status;
}

// Normally, the HMM va_block is destroyed when the va_space is destroyed
// (i.e., when the /dev/nvidia-uvm device is closed). A munmap() call triggers
// a uvm_hmm_invalidate() callback which unmaps the VMA's range from the GPU's
// page tables. However, it doesn't destroy the va_block because that would
// require calling mmu_interval_notifier_remove() which can't be called from
// the invalidate callback due to Linux locking constraints. If a process
// calls mmap()/munmap() for SAM and then creates a managed allocation,
// the same VMA range can be picked and there would be a UVM/HMM va_block
// conflict. Creating a managed allocation, external allocation, or other
// va_range types, calls this function to remove stale HMM va_blocks or split
// the HMM va_block so there is no overlap.
NV_STATUS uvm_hmm_va_block_reclaim(uvm_va_space_t *va_space,
                                   struct mm_struct *mm,
                                   NvU64 start,
                                   NvU64 end)
{
    uvm_range_tree_node_t *node, *next;
    uvm_va_block_t *va_block;
    NV_STATUS status;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_OK;

    if (mm)
        uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);

    // Process each HMM va_block that overlaps the interval [start, end].
    // Note that end is inclusive.
    // The blocks_lock is not needed when the va_space lock is held for write.
    uvm_range_tree_for_each_in_safe(node, next, &va_space->hmm.blocks, start, end) {
        va_block = hmm_va_block_from_node(node);

        if (mm) {
            status = split_block_if_needed(va_block, start, end, &va_block);
            if (status != NV_OK)
                return status;
        }

        // Note that this waits for any invalidations callbacks to complete
        // so uvm_hmm_invalidate() won't see a block disapear.
        // The va_space write lock should prevent uvm_hmm_va_block_find_create()
        // from adding it back.
        mmu_interval_notifier_remove(&va_block->hmm.notifier);
        uvm_range_tree_remove(&va_space->hmm.blocks, &va_block->hmm.node);
        uvm_va_block_kill(va_block);
    }

    UVM_ASSERT(!uvm_range_tree_iter_first(&va_space->hmm.blocks, start, end));

    return NV_OK;
}

void uvm_hmm_va_block_split_tree(uvm_va_block_t *existing_va_block, uvm_va_block_t *new_block)
{
    uvm_va_space_t *va_space = existing_va_block->hmm.va_space;

    UVM_ASSERT(uvm_va_block_is_hmm(existing_va_block));
    uvm_assert_rwsem_locked_write(&va_space->lock);

    uvm_range_tree_split(&existing_va_block->hmm.va_space->hmm.blocks,
                         &existing_va_block->hmm.node,
                         &new_block->hmm.node);
}

NV_STATUS uvm_hmm_split_as_needed(uvm_va_space_t *va_space,
                                  NvU64 addr,
                                  uvm_va_policy_is_split_needed_t split_needed_cb,
                                  void *data)
{
    uvm_va_block_t *va_block;
    uvm_va_policy_node_t *node;
    NV_STATUS status;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    // If there is no HMM va_block or the va_block doesn't span the policy
    // addr, there is no need to split.
    status = uvm_hmm_va_block_find(va_space, addr, &va_block);
    if (status != NV_OK || va_block->start == addr)
        return NV_OK;

    uvm_mutex_lock(&va_block->lock);

    node = uvm_va_policy_node_find(va_block, addr);
    if (!node)
        goto done;

    // If the policy range doesn't span addr, we're done.
    if (addr == node->node.start)
        goto done;

    if (split_needed_cb(&node->policy, data))
        status = uvm_va_policy_node_split(va_block, node, addr - 1, NULL);

done:
    uvm_mutex_unlock(&va_block->lock);
    return status;
}

static NV_STATUS hmm_set_preferred_location_locked(uvm_va_block_t *va_block,
                                                   uvm_va_block_context_t *va_block_context,
                                                   uvm_processor_id_t preferred_location,
                                                   int preferred_cpu_nid,
                                                   NvU64 addr,
                                                   NvU64 end,
                                                   uvm_tracker_t *out_tracker)
{
    uvm_processor_mask_t *set_accessed_by_processors;
    const uvm_va_policy_t *old_policy;
    uvm_va_policy_node_t *node;
    uvm_va_block_region_t region;
    uvm_processor_id_t id;
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status;

    set_accessed_by_processors = uvm_processor_mask_cache_alloc();
    if (!set_accessed_by_processors)
        return NV_ERR_NO_MEMORY;

    // Note that we can't just call uvm_va_policy_set_range() for the whole
    // range [addr end] because we need to examine the old value of
    // policy->preferred_location and policy->preferred_nid before setting it.
    // Thus we iterate over the existing policy nodes.
    uvm_for_each_va_policy_in(old_policy, va_block, addr, end, node, region) {
        if (uvm_va_policy_preferred_location_equal(old_policy, preferred_location, preferred_cpu_nid))
            continue;

        // If the old preferred location is a valid processor ID, remote
        // mappings should be established to the new preferred location if
        // accessed-by is set.
        uvm_processor_mask_zero(set_accessed_by_processors);

        if (UVM_ID_IS_VALID(old_policy->preferred_location) &&
            uvm_processor_mask_test(&old_policy->accessed_by, old_policy->preferred_location))
            uvm_processor_mask_set(set_accessed_by_processors, old_policy->preferred_location);

        if (!uvm_va_policy_set_preferred_location(va_block,
                                                  region,
                                                  preferred_location,
                                                  preferred_cpu_nid,
                                                  old_policy)) {
            status = NV_ERR_NO_MEMORY;
            break;
        }

        // Establish new remote mappings if the old preferred location had
        // accessed-by set.
        for_each_id_in_mask(id, set_accessed_by_processors) {
            status = uvm_va_block_set_accessed_by_locked(va_block, va_block_context, id, region, out_tracker);
            if (status != NV_OK)
                break;
        }

        // Even though the UVM_VA_BLOCK_RETRY_LOCKED() may unlock and relock
        // the va_block lock, the policy remains valid because we hold the mmap
        // lock so munmap can't remove the policy, and the va_space lock so the
        // policy APIs can't change the policy.
        status = UVM_VA_BLOCK_RETRY_LOCKED(va_block,
                                           NULL,
                                           uvm_va_block_set_preferred_location_locked(va_block,
                                                                                      va_block_context,
                                                                                      region));

        tracker_status = uvm_tracker_add_tracker_safe(out_tracker, &va_block->tracker);
        if (status == NV_OK)
            status = tracker_status;

        if (status != NV_OK)
            break;
    }

    uvm_processor_mask_cache_free(set_accessed_by_processors);
    return status;
}

NV_STATUS uvm_hmm_set_preferred_location(uvm_va_space_t *va_space,
                                         uvm_processor_id_t preferred_location,
                                         int preferred_cpu_nid,
                                         NvU64 base,
                                         NvU64 last_address,
                                         uvm_tracker_t *out_tracker)
{
    uvm_va_block_context_t *va_block_context;
    uvm_va_block_t *va_block;
    NvU64 addr;
    NV_STATUS status = NV_OK;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(va_space->va_space_mm.mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);
    UVM_ASSERT(PAGE_ALIGNED(base));
    UVM_ASSERT(PAGE_ALIGNED(last_address + 1));
    UVM_ASSERT(base < last_address);

    // Update HMM preferred location policy.

    va_block_context = uvm_va_space_block_context(va_space, va_space->va_space_mm.mm);

    for (addr = base; addr < last_address; addr = va_block->end + 1) {
        NvU64 end;

        status = hmm_va_block_find_create(va_space, addr, true, &va_block_context->hmm.vma, &va_block);
        if (status != NV_OK)
            break;

        end = min(last_address, va_block->end);

        uvm_mutex_lock(&va_block->lock);

        status = hmm_set_preferred_location_locked(va_block,
                                                   va_block_context,
                                                   preferred_location,
                                                   preferred_cpu_nid,
                                                   addr,
                                                   end,
                                                   out_tracker);

        uvm_mutex_unlock(&va_block->lock);

        if (status != NV_OK)
            break;
    }

    return status;
}

static NV_STATUS hmm_set_accessed_by_start_end_locked(uvm_va_block_t *va_block,
                                                      uvm_va_block_context_t *va_block_context,
                                                      uvm_processor_id_t processor_id,
                                                      NvU64 start,
                                                      NvU64 end,
                                                      uvm_tracker_t *out_tracker)
{
    uvm_va_space_t *va_space = va_block->hmm.va_space;
    uvm_va_policy_node_t *node;
    uvm_va_block_region_t region;
    NV_STATUS status = NV_OK;

    uvm_for_each_va_policy_node_in(node, va_block, start, end) {
        // Read duplication takes precedence over SetAccessedBy.
        // Do not add mappings if read duplication is enabled.
        if (uvm_va_policy_is_read_duplicate(&node->policy, va_space))
            continue;

        region = uvm_va_block_region_from_start_end(va_block,
                                                    max(start, node->node.start),
                                                    min(end, node->node.end));

        status = uvm_va_block_set_accessed_by_locked(va_block,
                                                     va_block_context,
                                                     processor_id,
                                                     region,
                                                     out_tracker);
        if (status != NV_OK)
            break;
    }

    return status;
}

NV_STATUS uvm_hmm_set_accessed_by(uvm_va_space_t *va_space,
                                  uvm_processor_id_t processor_id,
                                  bool set_bit,
                                  NvU64 base,
                                  NvU64 last_address,
                                  uvm_tracker_t *out_tracker)
{
    uvm_va_block_context_t *va_block_context;
    uvm_va_block_t *va_block;
    NvU64 addr;
    NV_STATUS status = NV_OK;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(va_space->va_space_mm.mm);
    uvm_assert_rwsem_locked_write(&va_space->lock);
    UVM_ASSERT(PAGE_ALIGNED(base));
    UVM_ASSERT(PAGE_ALIGNED(last_address + 1));
    UVM_ASSERT(base < last_address);

    // Update HMM accessed by policy.

    va_block_context = uvm_va_space_block_context(va_space, va_space->va_space_mm.mm);

    for (addr = base; addr < last_address; addr = va_block->end + 1) {
        NvU64 end;

        status = hmm_va_block_find_create(va_space, addr, true, &va_block_context->hmm.vma, &va_block);
        if (status != NV_OK)
            break;

        end = min(last_address, va_block->end);

        uvm_mutex_lock(&va_block->lock);

        status = uvm_va_policy_set_range(va_block,
                                         addr,
                                         end,
                                         UVM_VA_POLICY_ACCESSED_BY,
                                         !set_bit,
                                         processor_id,
                                         NUMA_NO_NODE,
                                         UVM_READ_DUPLICATION_MAX);

        if (status == NV_OK && set_bit) {
            status = hmm_set_accessed_by_start_end_locked(va_block,
                                                          va_block_context,
                                                          processor_id,
                                                          addr,
                                                          end,
                                                          out_tracker);
        }

        uvm_mutex_unlock(&va_block->lock);

        if (status != NV_OK)
            break;
    }

    return status;
}

void uvm_hmm_block_add_eviction_mappings(uvm_va_space_t *va_space,
                                         uvm_va_block_t *va_block,
                                         uvm_va_block_context_t *block_context)
{
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    uvm_va_policy_node_t *node;
    uvm_va_block_region_t region;
    uvm_processor_mask_t *map_processors = &block_context->hmm.map_processors_eviction;
    uvm_processor_id_t id;
    NV_STATUS tracker_status;
    NV_STATUS status = NV_OK;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mmap_lock_locked(va_space->va_space_mm.mm);
    uvm_assert_rwsem_locked(&va_space->lock);

    uvm_mutex_lock(&va_block->lock);

    uvm_for_each_va_policy_node_in(node, va_block, va_block->start, va_block->end) {
        for_each_id_in_mask(id, &node->policy.accessed_by) {
            status = hmm_set_accessed_by_start_end_locked(va_block,
                                                          block_context,
                                                          id,
                                                          node->node.start,
                                                          node->node.end,
                                                          &local_tracker);
            if (status != NV_OK)
                break;

            if (!uvm_va_space_map_remote_on_eviction(va_space))
                continue;

            // Exclude the processors that have been already mapped due to
            // AccessedBy.
            uvm_processor_mask_andnot(map_processors, &va_block->evicted_gpus, &node->policy.accessed_by);

            for_each_gpu_id_in_mask(id, map_processors) {
                uvm_gpu_t *gpu = uvm_gpu_get(id);
                uvm_va_block_gpu_state_t *gpu_state;

                if (!gpu->parent->access_counters_supported)
                    continue;

                gpu_state = uvm_va_block_gpu_state_get(va_block, id);
                UVM_ASSERT(gpu_state);

                // TODO: Bug 2096389: uvm_va_block_add_mappings does not add
                // remote mappings to read-duplicated pages. Add support for it
                // or create a new function.
                status = uvm_va_block_add_mappings(va_block,
                                                   block_context,
                                                   id,
                                                   region,
                                                   &gpu_state->evicted,
                                                   UvmEventMapRemoteCauseEviction);
                tracker_status = uvm_tracker_add_tracker_safe(&local_tracker, &va_block->tracker);
                status = (status == NV_OK) ? tracker_status : status;
                if (status != NV_OK) {
                    UVM_ASSERT(status != NV_ERR_MORE_PROCESSING_REQUIRED);
                    break;
                }
            }
        }
    }

    uvm_mutex_unlock(&va_block->lock);

    tracker_status = uvm_tracker_wait_deinit(&local_tracker);
    status = (status == NV_OK) ? tracker_status : status;
    if (status != NV_OK) {
        UVM_ERR_PRINT("Deferred mappings to evicted memory for block [0x%llx, 0x%llx] failed %s\n",
                      va_block->start,
                      va_block->end,
                      nvstatusToString(status));
    }
}

const uvm_va_policy_t *uvm_hmm_find_policy_end(uvm_va_block_t *va_block,
                                               struct vm_area_struct *vma,
                                               unsigned long addr,
                                               NvU64 *endp)
{
    const uvm_va_policy_node_t *node;
    const uvm_va_policy_t *policy;
    NvU64 end = va_block->end;

    uvm_assert_mmap_lock_locked(vma->vm_mm);
    uvm_assert_mutex_locked(&va_block->lock);

    if (end > vma->vm_end - 1)
        end = vma->vm_end - 1;

    node = uvm_va_policy_node_find(va_block, addr);
    if (node) {
        policy = &node->policy;
        if (end > node->node.end)
            end = node->node.end;
    }
    else {
        policy = &uvm_va_policy_default;
    }

    *endp = end;

    return policy;
}

NV_STATUS uvm_hmm_find_policy_vma_and_outer(uvm_va_block_t *va_block,
                                            struct vm_area_struct **vma_out,
                                            uvm_page_index_t page_index,
                                            const uvm_va_policy_t **policy,
                                            uvm_page_index_t *outerp)
{
    unsigned long addr;
    NvU64 end;
    uvm_page_index_t outer;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    struct mm_struct *mm = va_space->va_space_mm.mm;

    if (!mm)
        return NV_ERR_INVALID_ADDRESS;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_mutex_locked(&va_block->lock);

    addr = uvm_va_block_cpu_page_address(va_block, page_index);

    *vma_out = vma_lookup(mm, addr);
    if (!*vma_out || !((*vma_out)->vm_flags & VM_READ))
        return NV_ERR_INVALID_ADDRESS;

    *policy = uvm_hmm_find_policy_end(va_block, *vma_out, addr, &end);

    outer = uvm_va_block_cpu_page_index(va_block, end) + 1;
    if (*outerp > outer)
        *outerp = outer;

    return NV_OK;
}

static NV_STATUS hmm_clear_thrashing_policy(uvm_va_block_t *va_block,
                                            uvm_va_block_context_t *block_context)
{
    const uvm_va_policy_t *policy;
    uvm_va_policy_node_t *node;
    uvm_va_block_region_t region;
    NV_STATUS status = NV_OK;

    uvm_mutex_lock(&va_block->lock);

    uvm_for_each_va_policy_in(policy, va_block, va_block->start, va_block->end, node, region) {
        // Unmap may split PTEs and require a retry. Needs to be called
        // before the pinned pages information is destroyed.
        status = UVM_VA_BLOCK_RETRY_LOCKED(va_block,
                                           NULL,
                                           uvm_perf_thrashing_unmap_remote_pinned_pages_all(va_block,
                                                                                            block_context,
                                                                                            region));

        uvm_perf_thrashing_info_destroy(va_block);

        if (status != NV_OK)
            break;
    }

    uvm_mutex_unlock(&va_block->lock);

    return status;
}

NV_STATUS uvm_hmm_clear_thrashing_policy(uvm_va_space_t *va_space)
{
    uvm_va_block_context_t *block_context = uvm_va_space_block_context(va_space, NULL);
    uvm_range_tree_node_t *node, *next;
    uvm_va_block_t *va_block;
    NV_STATUS status = NV_OK;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_OK;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    uvm_range_tree_for_each_safe(node, next, &va_space->hmm.blocks) {
        va_block = hmm_va_block_from_node(node);

        status = hmm_clear_thrashing_policy(va_block, block_context);
        if (status != NV_OK)
            break;
    }

    return status;
}

uvm_va_block_region_t uvm_hmm_get_prefetch_region(uvm_va_block_t *va_block,
                                                  struct vm_area_struct *vma,
                                                  const uvm_va_policy_t *policy,
                                                  NvU64 address)
{
    NvU64 start, end;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));

    // We need to limit the prefetch region to the VMA.
    start = max(va_block->start, (NvU64)vma->vm_start);
    end = min(va_block->end, (NvU64)vma->vm_end - 1);

    // Also, we need to limit the prefetch region to the policy range.
    if (uvm_va_policy_is_default(policy)) {
        NV_STATUS status = uvm_range_tree_find_hole_in(&va_block->hmm.va_policy_tree,
                                                       address,
                                                       &start,
                                                       &end);
        // We already know the hole exists and covers the fault region.
        UVM_ASSERT(status == NV_OK);
    }
    else {
        const uvm_va_policy_node_t *node = uvm_va_policy_node_from_policy(policy);

        start = max(start, node->node.start);
        end = min(end, node->node.end);
    }

    return uvm_va_block_region_from_start_end(va_block, start, end);
}

uvm_prot_t uvm_hmm_compute_logical_prot(uvm_va_block_t *va_block,
                                        struct vm_area_struct *vma,
                                        NvU64 addr)
{
    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    uvm_assert_mmap_lock_locked(va_block->hmm.va_space->va_space_mm.mm);
    UVM_ASSERT(vma && addr >= vma->vm_start && addr < vma->vm_end);

    if (!(vma->vm_flags & VM_READ))
        return UVM_PROT_NONE;
    else if (!(vma->vm_flags & VM_WRITE))
        return UVM_PROT_READ_ONLY;
    else
        return UVM_PROT_READ_WRITE_ATOMIC;
}

static NV_STATUS hmm_va_block_cpu_page_populate(uvm_va_block_t *va_block,
                                                uvm_page_index_t page_index,
                                                struct page *page)
{
    uvm_cpu_chunk_t *chunk;
    NV_STATUS status;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));
    UVM_ASSERT(!uvm_page_mask_test(&va_block->cpu.allocated, page_index));

    if (page == ZERO_PAGE(uvm_va_block_cpu_page_address(va_block, page_index)))
        return NV_ERR_INVALID_ADDRESS;

    status = uvm_cpu_chunk_alloc_hmm(page, &chunk);
    if (status != NV_OK)
        return status;

    status = uvm_cpu_chunk_insert_in_block(va_block, chunk, page_index);
    if (status != NV_OK) {
        uvm_cpu_chunk_free(chunk);
        return status;
    }

    status = uvm_va_block_map_cpu_chunk_on_gpus(va_block, chunk, page_index);
    if (status != NV_OK) {
        uvm_cpu_chunk_remove_from_block(va_block, page_to_nid(page), page_index);
        uvm_cpu_chunk_free(chunk);
    }

    return status;
}

static void hmm_va_block_cpu_unpopulate_chunk(uvm_va_block_t *va_block,
                                              uvm_cpu_chunk_t *chunk,
                                              int chunk_nid,
                                              uvm_page_index_t page_index)
{
    if (!chunk)
        return;

    UVM_ASSERT(!uvm_processor_mask_test(&va_block->resident, UVM_ID_CPU) ||
               !uvm_va_block_cpu_is_page_resident_on(va_block, NUMA_NO_NODE, page_index));
    UVM_ASSERT(uvm_cpu_chunk_get_size(chunk) == PAGE_SIZE);

    uvm_cpu_chunk_remove_from_block(va_block, chunk_nid, page_index);
    uvm_va_block_unmap_cpu_chunk_on_gpus(va_block, chunk);
    uvm_cpu_chunk_free(chunk);
}

static void hmm_va_block_cpu_page_unpopulate(uvm_va_block_t *va_block, uvm_page_index_t page_index, struct page *page)
{
    uvm_cpu_chunk_t *chunk;

    UVM_ASSERT(uvm_va_block_is_hmm(va_block));

    if (page) {
        chunk = uvm_cpu_chunk_get_chunk_for_page(va_block, page_to_nid(page), page_index);
        hmm_va_block_cpu_unpopulate_chunk(va_block, chunk, page_to_nid(page), page_index);
    }
    else {
        int nid;

        for_each_possible_uvm_node(nid) {
            chunk = uvm_cpu_chunk_get_chunk_for_page(va_block, nid, page_index);
            hmm_va_block_cpu_unpopulate_chunk(va_block, chunk, nid, page_index);
        }
    }
}

static bool hmm_va_block_cpu_page_is_same(uvm_va_block_t *va_block,
                                          uvm_page_index_t page_index,
                                          struct page *page)
{
    struct page *old_page = uvm_va_block_get_cpu_page(va_block, page_index);

    UVM_ASSERT(uvm_cpu_chunk_is_hmm(uvm_cpu_chunk_get_chunk_for_page(va_block, page_to_nid(page), page_index)));
    return old_page == page;
}

// uvm_va_block_service_copy() and uvm_va_block_service_finish() expect the
// service_context masks to match what is being processed. Since a page
// that was expected to be processed isn't migrating, we have to clear the
// masks to make service_context consistent with what is actually being
// handled.
static void clear_service_context_masks(uvm_service_block_context_t *service_context,
                                        uvm_processor_id_t new_residency,
                                        uvm_page_index_t page_index)
{
    uvm_page_mask_clear(&service_context->block_context->caller_page_mask, page_index);

    uvm_page_mask_clear(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency,
                        page_index);

    if (uvm_page_mask_empty(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency))
        uvm_processor_mask_clear(&service_context->resident_processors, new_residency);

    if (UVM_ID_IS_VALID(service_context->prefetch_hint.residency))
        uvm_page_mask_clear(&service_context->prefetch_hint.prefetch_pages_mask, page_index);

    if (service_context->thrashing_pin_count > 0 &&
        uvm_page_mask_test_and_clear(&service_context->thrashing_pin_mask, page_index)) {
        service_context->thrashing_pin_count--;
    }

    if (service_context->read_duplicate_count > 0 &&
        uvm_page_mask_test_and_clear(&service_context->read_duplicate_mask, page_index)) {
        service_context->read_duplicate_count--;
    }
}

static void cpu_mapping_set(uvm_va_block_t *va_block,
                            bool is_write,
                            uvm_page_index_t page_index)
{
    uvm_processor_mask_set(&va_block->mapped, UVM_ID_CPU);
    uvm_page_mask_set(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ], page_index);
    if (is_write)
        uvm_page_mask_set(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE], page_index);
    else
        uvm_page_mask_clear(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE], page_index);
}

static void cpu_mapping_clear(uvm_va_block_t *va_block, uvm_page_index_t page_index)
{
    uvm_page_mask_clear(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_WRITE], page_index);
    uvm_page_mask_clear(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ], page_index);
    if (uvm_page_mask_empty(&va_block->cpu.pte_bits[UVM_PTE_BITS_CPU_READ]))
        uvm_processor_mask_clear(&va_block->mapped, UVM_ID_CPU);
}

static void gpu_chunk_remove(uvm_va_block_t *va_block,
                             uvm_page_index_t page_index,
                             struct page *page)
{
    uvm_va_block_gpu_state_t *gpu_state;
    uvm_gpu_chunk_t *gpu_chunk;
    uvm_gpu_id_t id;

    id = uvm_gpu_chunk_get_gpu(uvm_pmm_devmem_page_to_chunk(page))->id;
    gpu_state = uvm_va_block_gpu_state_get(va_block, id);
    UVM_ASSERT(gpu_state);

    gpu_chunk = gpu_state->chunks[page_index];
    if (!gpu_chunk) {
        // If we didn't find a chunk it's because the page was unmapped for
        // mremap and no fault has established a new mapping.
        UVM_ASSERT(!uvm_page_mask_test(&gpu_state->resident, page_index));
        return;
    }

    UVM_ASSERT(gpu_chunk->state == UVM_PMM_GPU_CHUNK_STATE_ALLOCATED);
    UVM_ASSERT(gpu_chunk->is_referenced);

    uvm_page_mask_clear(&gpu_state->resident, page_index);

    uvm_mmu_chunk_unmap(gpu_chunk, &va_block->tracker);
    gpu_state->chunks[page_index] = NULL;
}

static NV_STATUS gpu_chunk_add(uvm_va_block_t *va_block,
                               uvm_page_index_t page_index,
                               struct page *page)
{
    uvm_va_block_gpu_state_t *gpu_state;
    uvm_gpu_chunk_t *gpu_chunk;
    uvm_gpu_id_t id;
    NV_STATUS status;

    id = uvm_gpu_chunk_get_gpu(uvm_pmm_devmem_page_to_chunk(page))->id;
    gpu_state = uvm_va_block_gpu_state_get(va_block, id);

    // It's possible that this is a fresh va_block we're trying to add an
    // existing gpu_chunk to. This occurs for example when a GPU faults on a
    // virtual address that has been remapped with mremap().
    if (!gpu_state) {
        status = uvm_va_block_gpu_state_alloc(va_block);
        if (status != NV_OK)
            return status;
        gpu_state = uvm_va_block_gpu_state_get(va_block, id);
    }

    UVM_ASSERT(gpu_state);

    // Note that a mremap() might be to a CPU virtual address that is nolonger
    // aligned with a larger GPU chunk size. We would need to allocate a new
    // aligned GPU chunk and copy from old to new.
    // TODO: Bug 3368756: add support for large GPU pages.
    gpu_chunk = uvm_pmm_devmem_page_to_chunk(page);
    UVM_ASSERT(gpu_chunk->state == UVM_PMM_GPU_CHUNK_STATE_ALLOCATED);
    UVM_ASSERT(gpu_chunk->is_referenced);
    UVM_ASSERT(uvm_pmm_devmem_page_to_va_space(page) == va_block->hmm.va_space);

    if (gpu_state->chunks[page_index] == gpu_chunk)
        return NV_OK;

    UVM_ASSERT(!gpu_state->chunks[page_index]);

    // In some configurations such as SR-IOV heavy, the chunk cannot be
    // referenced using its physical address. Create a virtual mapping.
    status = uvm_mmu_chunk_map(gpu_chunk);
    if (status != NV_OK)
        return status;

    uvm_processor_mask_set(&va_block->resident, id);
    uvm_page_mask_set(&gpu_state->resident, page_index);

    // It is safe to modify the page index field without holding any PMM locks
    // because the chunk is allocated, which means that none of the other
    // fields in the bitmap can change.
    gpu_chunk->va_block = va_block;
    gpu_chunk->va_block_page_index = page_index;

    gpu_state->chunks[page_index] = gpu_chunk;

    return NV_OK;
}

// This is called just before calling migrate_vma_finalize() in order to wait
// for GPU operations to complete and update the va_block state to match which
// pages migrated (or not) and therefore which pages will be released by
// migrate_vma_finalize().
// 'migrated_pages' is the mask of pages that migrated,
// 'same_devmem_page_mask' is the mask of pages that are the same in src_pfns
// and dst_pfns and therefore appear to migrate_vma_*() to be not migrating.
// 'region' is the page index region of all migrated, non-migrated, and
// same_devmem_page_mask pages.
static NV_STATUS sync_page_and_chunk_state(uvm_va_block_t *va_block,
                                           const unsigned long *src_pfns,
                                           const unsigned long *dst_pfns,
                                           uvm_va_block_region_t region,
                                           const uvm_page_mask_t *migrated_pages,
                                           const uvm_page_mask_t *same_devmem_page_mask)
{
    uvm_page_index_t page_index;
    NV_STATUS status;

    // Wait for the GPU to finish. migrate_vma_finalize() will release the
    // migrated source pages (or non migrating destination pages), so GPU
    // opererations must be finished by then.
    status = uvm_tracker_wait(&va_block->tracker);

    for_each_va_block_page_in_region(page_index, region) {
        struct page *page;

        if (uvm_page_mask_test(same_devmem_page_mask, page_index))
            continue;

        // If a page migrated, clean up the source page.
        // Otherwise, clean up the destination page.
        if (uvm_page_mask_test(migrated_pages, page_index))
            page = migrate_pfn_to_page(src_pfns[page_index]);
        else
            page = migrate_pfn_to_page(dst_pfns[page_index]);

        if (!page)
            continue;

        if (is_device_private_page(page)) {
            gpu_chunk_remove(va_block, page_index, page);
        }
        else {
            // If the source page is a system memory page,
            // migrate_vma_finalize() will release the reference so we should
            // clear our pointer to it.
            // TODO: Bug 3660922: Need to handle read duplication at some point.
            hmm_va_block_cpu_page_unpopulate(va_block, page_index, page);
        }
    }

    return status;
}

// Update va_block state to reflect that the page isn't migrating.
static void clean_up_non_migrating_page(uvm_va_block_t *va_block,
                                        const unsigned long *src_pfns,
                                        unsigned long *dst_pfns,
                                        uvm_page_index_t page_index)
{
    struct page *dst_page = migrate_pfn_to_page(dst_pfns[page_index]);

    if (!dst_page)
        return;

    // migrate_vma_finalize() will release the dst_page reference so don't keep
    // a pointer to it.
    if (is_device_private_page(dst_page)) {
        gpu_chunk_remove(va_block, page_index, dst_page);
    }
    else {
        UVM_ASSERT(page_ref_count(dst_page) == 1);

        hmm_va_block_cpu_page_unpopulate(va_block, page_index, dst_page);
    }

    unlock_page(dst_page);
    put_page(dst_page);
    dst_pfns[page_index] = 0;
}

static void clean_up_non_migrating_pages(uvm_va_block_t *va_block,
                                         const unsigned long *src_pfns,
                                         unsigned long *dst_pfns,
                                         uvm_va_block_region_t region,
                                         uvm_page_mask_t *page_mask)
{
    uvm_page_index_t page_index;
    NV_STATUS status;

    status = uvm_tracker_wait(&va_block->tracker);
    UVM_ASSERT(status == NV_OK);

    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        clean_up_non_migrating_page(va_block, src_pfns, dst_pfns, page_index);
    }
}

// CPU page fault handling.

// Fill in the dst_pfns[page_index] entry given that there is an allocated
// CPU page.
static void lock_block_cpu_page(uvm_va_block_t *va_block,
                                uvm_page_index_t page_index,
                                struct page *src_page,
                                unsigned long *dst_pfns,
                                uvm_page_mask_t *same_devmem_page_mask)
{
    uvm_cpu_chunk_t *chunk = uvm_cpu_chunk_get_any_chunk_for_page(va_block, page_index);
    uvm_va_block_region_t chunk_region;
    struct page *dst_page;

    UVM_ASSERT(chunk);
    UVM_ASSERT(chunk->page);

    chunk_region = uvm_va_block_chunk_region(va_block, uvm_cpu_chunk_get_size(chunk), page_index);

    dst_page = chunk->page + (page_index - chunk_region.first);

    UVM_ASSERT(dst_page != ZERO_PAGE(uvm_va_block_cpu_page_address(va_block, page_index)));
    UVM_ASSERT(!is_device_private_page(dst_page));

    // The source page is usually a device private page but it could be a GPU
    // remote mapped system memory page. It could also be a driver allocated
    // page for GPU-to-GPU staged copies (i.e., not a resident copy and owned
    // by the driver).
    if (is_device_private_page(src_page)) {
        // Since the page isn't mirrored, it was allocated by alloc_pages()
        // and UVM owns the reference. We leave the reference count unchanged
        // and mark the page pointer as mirrored since UVM is transferring
        // ownership to Linux and we don't want UVM to double free the page in
        // hmm_va_block_cpu_page_unpopulate() or block_kill(). If the page
        // does not migrate, it will be freed though.
        UVM_ASSERT(!uvm_processor_mask_test(&va_block->resident, UVM_ID_CPU) ||
                   !uvm_va_block_cpu_is_page_resident_on(va_block, NUMA_NO_NODE, page_index));
        UVM_ASSERT(chunk->type == UVM_CPU_CHUNK_TYPE_PHYSICAL);
        UVM_ASSERT(page_ref_count(dst_page) == 1);
        uvm_cpu_chunk_make_hmm(chunk);
    }
    else {
        UVM_ASSERT(same_devmem_page_mask);
        UVM_ASSERT(src_page == dst_page);
        uvm_page_mask_set(same_devmem_page_mask, page_index);

        // The call to migrate_vma_setup() will have inserted a migration PTE
        // so the CPU has no access.
        cpu_mapping_clear(va_block, page_index);
        return;
    }

    lock_page(dst_page);
    dst_pfns[page_index] = migrate_pfn(page_to_pfn(dst_page));
}

static void hmm_mark_gpu_chunk_referenced(uvm_va_block_t *va_block,
                                          uvm_gpu_t *gpu,
                                          uvm_gpu_chunk_t *gpu_chunk)
{
    // Tell PMM to expect a callback from Linux to free the page since the
    // device private struct page reference count will determine when the
    // GPU chunk is free.
    UVM_ASSERT(gpu_chunk->state == UVM_PMM_GPU_CHUNK_STATE_TEMP_PINNED);
    list_del_init(&gpu_chunk->list);
    uvm_pmm_gpu_unpin_referenced(&gpu->pmm, gpu_chunk, va_block);
}

static void fill_dst_pfn(uvm_va_block_t *va_block,
                         uvm_gpu_t *gpu,
                         const unsigned long *src_pfns,
                         unsigned long *dst_pfns,
                         uvm_page_index_t page_index,
                         uvm_page_mask_t *same_devmem_page_mask)
{
    unsigned long src_pfn = src_pfns[page_index];
    uvm_gpu_chunk_t *gpu_chunk;
    unsigned long pfn;
    struct page *dpage;

    gpu_chunk = uvm_va_block_lookup_gpu_chunk(va_block, gpu, uvm_va_block_cpu_page_address(va_block, page_index));
    UVM_ASSERT(gpu_chunk);
    UVM_ASSERT(gpu_chunk->log2_size == PAGE_SHIFT);
    pfn = uvm_pmm_gpu_devmem_get_pfn(&gpu->pmm, gpu_chunk);

    // If the same GPU page is both source and destination, migrate_vma_pages()
    // will see the wrong "expected" reference count and not migrate it, so we
    // mark it as not migrating but we keep track of this so we don't confuse
    // it with a page that migrate_vma_pages() actually does not migrate.
    if ((src_pfn & MIGRATE_PFN_VALID) && (src_pfn >> MIGRATE_PFN_SHIFT) == pfn) {
        uvm_page_mask_set(same_devmem_page_mask, page_index);
        return;
    }

    dpage = pfn_to_page(pfn);
    UVM_ASSERT(is_device_private_page(dpage));
    UVM_ASSERT(page_pgmap(dpage)->owner == &g_uvm_global);

    hmm_mark_gpu_chunk_referenced(va_block, gpu, gpu_chunk);
    UVM_ASSERT(!page_count(dpage));
    zone_device_page_init(dpage);
    dpage->zone_device_data = gpu_chunk;

    dst_pfns[page_index] = migrate_pfn(pfn);
}

static void fill_dst_pfns(uvm_va_block_t *va_block,
                          const unsigned long *src_pfns,
                          unsigned long *dst_pfns,
                          uvm_va_block_region_t region,
                          uvm_page_mask_t *page_mask,
                          uvm_page_mask_t *same_devmem_page_mask,
                          uvm_processor_id_t dest_id)
{
    uvm_gpu_t *gpu = uvm_gpu_get(dest_id);
    uvm_page_index_t page_index;

    uvm_page_mask_zero(same_devmem_page_mask);

    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        if (!(src_pfns[page_index] & MIGRATE_PFN_MIGRATE))
            continue;

        fill_dst_pfn(va_block,
                     gpu,
                     src_pfns,
                     dst_pfns,
                     page_index,
                     same_devmem_page_mask);
    }
}

static NV_STATUS alloc_page_on_cpu(uvm_va_block_t *va_block,
                                   uvm_page_index_t page_index,
                                   const unsigned long *src_pfns,
                                   unsigned long *dst_pfns,
                                   uvm_page_mask_t *same_devmem_page_mask,
                                   uvm_va_block_context_t *block_context)
{
    NV_STATUS status;
    struct page *src_page;
    struct page *dst_page;

    // This is the page that will be copied to system memory.
    src_page = migrate_pfn_to_page(src_pfns[page_index]);

    if (src_page) {
        // mremap may have caused us to lose the gpu_chunk associated with
        // this va_block/page_index so make sure we have the correct chunk.
        if (is_device_private_page(src_page))
            gpu_chunk_add(va_block, page_index, src_page);

        if (uvm_page_mask_test(&va_block->cpu.allocated, page_index)) {
            lock_block_cpu_page(va_block, page_index, src_page, dst_pfns, same_devmem_page_mask);
            return NV_OK;
        }
    }

    UVM_ASSERT(!uvm_processor_mask_test(&va_block->resident, UVM_ID_CPU) ||
                !uvm_va_block_cpu_is_page_resident_on(va_block, NUMA_NO_NODE, page_index));

    status = uvm_va_block_populate_page_cpu(va_block, page_index, block_context);
    if (status != NV_OK)
        return status;

    // TODO: Bug 3368756: add support for transparent huge pages
    // Support for large CPU pages means the page_index may need fixing
    dst_page = migrate_pfn_to_page(block_context->hmm.dst_pfns[page_index]);

    // Note that we don't call get_page(dst_page) since alloc_page_vma()
    // returns with a page reference count of one and we are passing
    // ownership to Linux. Also, uvm_va_block_cpu_page_populate() recorded
    // the page as "mirrored" so that migrate_vma_finalize() and
    // hmm_va_block_cpu_page_unpopulate() don't double free the page.
    lock_page(dst_page);
    dst_pfns[page_index] = migrate_pfn(page_to_pfn(dst_page));

    return NV_OK;
}

// Allocates pages on the CPU to handle migration due to a page fault
static NV_STATUS fault_alloc_on_cpu(uvm_va_block_t *va_block,
                                    const unsigned long *src_pfns,
                                    unsigned long *dst_pfns,
                                    uvm_va_block_region_t region,
                                    uvm_page_mask_t *page_mask,
                                    uvm_page_mask_t *same_devmem_page_mask,
                                    uvm_processor_id_t fault_processor_id,
                                    uvm_service_block_context_t *service_context)
{
    uvm_page_index_t page_index;
    NV_STATUS status = NV_OK;

    UVM_ASSERT(service_context);

    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        if (!(src_pfns[page_index] & MIGRATE_PFN_MIGRATE)) {
            // Device exclusive PTEs are not selected but we still want to
            // process the page so record it as such.
            if (!UVM_ID_IS_CPU(fault_processor_id) &&
                service_context->access_type[page_index] == UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG) {
                uvm_page_mask_set(same_devmem_page_mask, page_index);
                continue;
            }

            // We have previously found a page that is CPU resident which can't
            // be migrated (probably a shared mapping) so make sure we establish
            // a remote mapping for it.
            if (uvm_page_mask_test(same_devmem_page_mask, page_index))
                continue;

            goto clr_mask;
        }

        status = alloc_page_on_cpu(va_block, page_index, src_pfns, dst_pfns, same_devmem_page_mask, service_context->block_context);
        if (status != NV_OK) {
            // Ignore errors if the page is only for prefetching.
            if (service_context &&
                service_context->access_type[page_index] == UVM_FAULT_ACCESS_TYPE_PREFETCH)
                goto clr_mask;
            break;
        }
        continue;

    clr_mask:
        // TODO: Bug 3900774: clean up murky mess of mask clearing.
        uvm_page_mask_clear(page_mask, page_index);
        clear_service_context_masks(service_context, UVM_ID_CPU, page_index);
    }

    if (status != NV_OK)
        clean_up_non_migrating_pages(va_block, src_pfns, dst_pfns, region, page_mask);
    else if (uvm_page_mask_empty(page_mask))
        return NV_WARN_MORE_PROCESSING_REQUIRED;

    return status;
}

// Allocates pages on the CPU for explicit migration calls.
static NV_STATUS migrate_alloc_on_cpu(uvm_va_block_t *va_block,
                                      const unsigned long *src_pfns,
                                      unsigned long *dst_pfns,
                                      uvm_va_block_region_t region,
                                      uvm_page_mask_t *page_mask,
                                      uvm_page_mask_t *same_devmem_page_mask,
                                      uvm_va_block_context_t *block_context)
{
    uvm_page_index_t page_index;
    NV_STATUS status = NV_OK;

    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        if (!(src_pfns[page_index] & MIGRATE_PFN_MIGRATE)) {
            // We have previously found a page that is CPU resident which can't
            // be migrated (probably a shared mapping) so make sure we establish
            // a remote mapping for it.
            if (uvm_page_mask_test(same_devmem_page_mask, page_index))
                continue;

            uvm_page_mask_clear(page_mask, page_index);
            continue;
        }

        status = alloc_page_on_cpu(va_block, page_index, src_pfns, dst_pfns, same_devmem_page_mask, block_context);
    }

    if (status != NV_OK)
        clean_up_non_migrating_pages(va_block, src_pfns, dst_pfns, region, page_mask);
    else if (uvm_page_mask_empty(page_mask))
        return NV_WARN_MORE_PROCESSING_REQUIRED;

    return status;
}

static NV_STATUS uvm_hmm_devmem_fault_alloc_and_copy(uvm_hmm_devmem_fault_context_t *devmem_fault_context)
{
    uvm_processor_id_t processor_id;
    uvm_service_block_context_t *service_context;
    uvm_va_block_retry_t *va_block_retry;
    const unsigned long *src_pfns;
    unsigned long *dst_pfns;
    uvm_page_mask_t *page_mask;
    uvm_page_mask_t *same_devmem_page_mask = &devmem_fault_context->same_devmem_page_mask;
    uvm_va_block_t *va_block;
    NV_STATUS status = NV_OK;

    processor_id = devmem_fault_context->processor_id;
    service_context = devmem_fault_context->service_context;
    va_block_retry = devmem_fault_context->va_block_retry;
    va_block = devmem_fault_context->va_block;
    src_pfns = service_context->block_context->hmm.src_pfns;
    dst_pfns = service_context->block_context->hmm.dst_pfns;

    // Build the migration page mask.
    // Note that thrashing pinned pages and prefetch pages are already
    // accounted for in service_context->per_processor_masks.
    page_mask = &devmem_fault_context->page_mask;
    uvm_page_mask_copy(page_mask, &service_context->per_processor_masks[UVM_ID_CPU_VALUE].new_residency);

    status = fault_alloc_on_cpu(va_block,
                                src_pfns,
                                dst_pfns,
                                service_context->region,
                                page_mask,
                                same_devmem_page_mask,
                                processor_id,
                                service_context);
    if (status != NV_OK)
        return status;

    // Do the copy but don't update the residency or mapping for the new
    // location yet.
    status = uvm_va_block_service_copy(processor_id, UVM_ID_CPU, va_block, va_block_retry, service_context);
    if (status != NV_OK)
        clean_up_non_migrating_pages(va_block, src_pfns, dst_pfns, service_context->region, page_mask);

    return status;
}

static NV_STATUS uvm_hmm_devmem_fault_finalize_and_map(uvm_hmm_devmem_fault_context_t *devmem_fault_context)
{
    uvm_processor_id_t processor_id;
    uvm_service_block_context_t *service_context;
    uvm_perf_prefetch_hint_t *prefetch_hint;
    uvm_va_block_retry_t *va_block_retry;
    const unsigned long *src_pfns;
    unsigned long *dst_pfns;
    uvm_page_mask_t *page_mask;
    uvm_va_block_t *va_block;
    uvm_va_block_region_t region;
    uvm_page_index_t page_index;
    NV_STATUS status, tracker_status;

    processor_id = devmem_fault_context->processor_id;
    service_context = devmem_fault_context->service_context;
    prefetch_hint = &service_context->prefetch_hint;
    va_block = devmem_fault_context->va_block;
    va_block_retry = devmem_fault_context->va_block_retry;
    src_pfns = service_context->block_context->hmm.src_pfns;
    dst_pfns = service_context->block_context->hmm.dst_pfns;
    region = service_context->region;

    page_mask = &devmem_fault_context->page_mask;

    // There are a number of reasons why HMM will mark a page as not migrating
    // even if we set a valid entry in dst_pfns[]. Mark these pages accordingly.
    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        if (src_pfns[page_index] & MIGRATE_PFN_MIGRATE)
            continue;

        // If a page isn't migrating and only the GPU page table is being
        // updated, continue to process it normally.
        if (uvm_page_mask_test(&devmem_fault_context->same_devmem_page_mask, page_index))
            continue;

        // TODO: Bug 3900774: clean up murky mess of mask clearing.
        uvm_page_mask_clear(page_mask, page_index);
        clear_service_context_masks(service_context, UVM_ID_CPU, page_index);
    }

    if (uvm_page_mask_empty(page_mask))
        status = NV_WARN_MORE_PROCESSING_REQUIRED;
    else
        status = uvm_va_block_service_finish(processor_id, va_block, service_context);

    tracker_status = sync_page_and_chunk_state(va_block,
                                               src_pfns,
                                               dst_pfns,
                                               region,
                                               page_mask,
                                               &devmem_fault_context->same_devmem_page_mask);

    return status == NV_OK ? tracker_status : status;
}

static NV_STATUS populate_region(uvm_va_block_t *va_block,
                                 unsigned long *pfns,
                                 uvm_va_block_region_t region,
                                 uvm_page_mask_t *populated_page_mask)
{
    uvm_page_index_t page_index;
    NV_STATUS status;

    // Make sure GPU state is allocated or else the GPU DMA mappings to
    // system memory won't be saved.
    status = uvm_va_block_gpu_state_alloc(va_block);
    if (status != NV_OK)
        return status;

    for_each_va_block_page_in_region(page_index, region) {
        struct page *page;

        // This case should only happen when querying CPU residency and we ask
        // for something not covered by a VMA. Otherwise, hmm_range_fault()
        // returns -EFAULT instead of setting the HMM_PFN_ERROR bit.
        if (pfns[page_index] & HMM_PFN_ERROR)
            return NV_ERR_INVALID_ADDRESS;

        if (pfns[page_index] & HMM_PFN_VALID) {
            page = hmm_pfn_to_page(pfns[page_index]);
        }
        else {
            // The page can't be evicted since it has to be migrated to the GPU
            // first which would leave a device private page entry so this has
            // to be a pte_none(), swapped out, or similar entry.
            // The page would have been allocated if populate_region() is being
            // called from uvm_hmm_va_block_service_locked() so this must be
            // for uvm_hmm_va_block_update_residency_info(). Just leave the
            // residency/populated information unchanged since
            // uvm_hmm_invalidate() should handle that if the underlying page
            // is invalidated.
            // Also note there can be an allocated page due to GPU-to-GPU
            // migration between non-peer GPUs.
            continue;
        }

        if (is_device_private_page(page)) {
            // Linux can call hmm_invalidate() and we have to clear the GPU
            // chunk pointer in uvm_va_block_gpu_state_t::chunks[] but it might
            // not release the device private struct page reference. Since
            // hmm_range_fault() did find a device private PTE, we can
            // re-establish the GPU chunk pointer.
            status = gpu_chunk_add(va_block, page_index, page);
            if (status != NV_OK)
                return status;
            continue;
        }

        // If a CPU chunk is already allocated, check to see it matches what
        // hmm_range_fault() found.
        if (uvm_page_mask_test(&va_block->cpu.allocated, page_index)) {
            UVM_ASSERT(hmm_va_block_cpu_page_is_same(va_block, page_index, page));
        }
        else {
            status = hmm_va_block_cpu_page_populate(va_block, page_index, page);
            if (status != NV_OK)
                return status;

            // Record that we populated this page. hmm_block_cpu_fault_locked()
            // uses this to ensure pages that don't migrate get remote mapped.
            if (populated_page_mask)
                uvm_page_mask_set(populated_page_mask, page_index);
        }

        // Since we have a stable snapshot of the CPU pages, we can
        // update the residency and protection information.
        uvm_va_block_cpu_set_resident_page(va_block, page_to_nid(page), page_index);

        cpu_mapping_set(va_block, pfns[page_index] & HMM_PFN_WRITE, page_index);
    }

    return NV_OK;
}

static void hmm_range_fault_begin(uvm_va_block_t *va_block)
{
    uvm_thread_context_t *uvm_context = uvm_thread_context();

    uvm_assert_mutex_locked(&va_block->lock);
    uvm_context->hmm_invalidate_seqnum = va_block->hmm.changed;
}

static bool hmm_range_fault_retry(uvm_va_block_t *va_block)
{
    uvm_thread_context_t *uvm_context = uvm_thread_context();

    uvm_assert_mutex_locked(&va_block->lock);
    return uvm_context->hmm_invalidate_seqnum != va_block->hmm.changed;
}

// Make the region be resident on the CPU by calling hmm_range_fault() to fault
// in CPU pages.
static NV_STATUS hmm_make_resident_cpu(uvm_va_block_t *va_block,
                                       struct vm_area_struct *vma,
                                       unsigned long *hmm_pfns,
                                       uvm_va_block_region_t region,
                                       NvU8 *access_type,
                                       uvm_page_mask_t *populated_page_mask)
{
    uvm_page_index_t page_index;
    int ret;
    struct hmm_range range = {
        .notifier = &va_block->hmm.notifier,
        .start = uvm_va_block_region_start(va_block, region),
        .end = uvm_va_block_region_end(va_block, region) + 1,
        .hmm_pfns = hmm_pfns + region.first,
        .pfn_flags_mask = HMM_PFN_REQ_FAULT | HMM_PFN_REQ_WRITE,
        .dev_private_owner = &g_uvm_global,
    };

    for_each_va_block_page_in_region(page_index, region) {
        if ((access_type && access_type[page_index] >= UVM_FAULT_ACCESS_TYPE_WRITE) ||
            (vma->vm_flags & VM_WRITE))
            hmm_pfns[page_index] = HMM_PFN_REQ_FAULT | HMM_PFN_REQ_WRITE;
        else
            hmm_pfns[page_index] = HMM_PFN_REQ_FAULT;
    }

    hmm_range_fault_begin(va_block);

    // Mirror the VA block to the HMM address range.
    // Note that we request HMM to handle page faults, which means that it will
    // populate and map potentially not-yet-existing pages to the VMA.
    // Also note that mmu_interval_read_begin() calls wait_event() for any
    // parallel invalidation callbacks to finish so we can't hold locks that
    // the invalidation callback acquires.
    uvm_mutex_unlock(&va_block->lock);

    range.notifier_seq = mmu_interval_read_begin(range.notifier);
    ret = hmm_range_fault(&range);

    uvm_mutex_lock(&va_block->lock);

    if (ret)
        return (ret == -EBUSY) ? NV_WARN_MORE_PROCESSING_REQUIRED : errno_to_nv_status(ret);

    if (hmm_range_fault_retry(va_block))
        return NV_WARN_MORE_PROCESSING_REQUIRED;

    return populate_region(va_block,
                           hmm_pfns,
                           region,
                           populated_page_mask);
}

// Release the reference count on any pages that were made device exclusive.
static void hmm_release_atomic_pages(uvm_va_block_t *va_block,
                                     uvm_service_block_context_t *service_context)
{
    uvm_va_block_region_t region = service_context->region;
    uvm_page_index_t page_index;

    for_each_va_block_page_in_region(page_index, region) {
        struct page *page = service_context->block_context->hmm.pages[page_index];

        if (!page)
            continue;

        unlock_page(page);
        put_page(page);
    }
}

static int hmm_make_device_exclusive_range(struct mm_struct *mm,
                                           unsigned long start,
                                           unsigned long end,
                                           struct page **pages)
{
#if NV_IS_EXPORT_SYMBOL_PRESENT_make_device_exclusive
    unsigned long addr;
    int npages = 0;

    for (addr = start; addr < end; addr += PAGE_SIZE) {
        struct folio *folio;
        struct page *page;

        page = make_device_exclusive(mm, addr, &g_uvm_global, &folio);
        if (IS_ERR(page)) {
            while (npages) {
                page = pages[--npages];
                unlock_page(page);
                put_page(page);
            }
            npages = PTR_ERR(page);
            break;
        }

        pages[npages++] = page;
    }

    return npages;
#else
    return make_device_exclusive_range(mm, start, end, pages, &g_uvm_global);
#endif
}

static NV_STATUS hmm_block_atomic_fault_locked(uvm_processor_id_t processor_id,
                                               uvm_va_block_t *va_block,
                                               uvm_va_block_retry_t *va_block_retry,
                                               uvm_service_block_context_t *service_context)
{
    uvm_va_block_region_t region = service_context->region;
    struct page **pages = service_context->block_context->hmm.pages;
    struct vm_area_struct *vma = service_context->block_context->hmm.vma;
    int npages;
    uvm_page_index_t page_index;
    uvm_make_resident_cause_t cause;
    NV_STATUS status;

    if (!uvm_processor_mask_test(&va_block->resident, UVM_ID_CPU) ||
        !uvm_va_block_cpu_is_region_resident_on(va_block, NUMA_NO_NODE, region)) {
        // There is an atomic GPU fault. We need to make sure no pages are
        // GPU resident so that make_device_exclusive_range() doesn't call
        // migrate_to_ram() and cause a va_space lock recursion problem.
        if (service_context->operation == UVM_SERVICE_OPERATION_REPLAYABLE_FAULTS)
            cause = UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT;
        else if (service_context->operation == UVM_SERVICE_OPERATION_NON_REPLAYABLE_FAULTS)
            cause = UVM_MAKE_RESIDENT_CAUSE_NON_REPLAYABLE_FAULT;
        else
            cause = UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER;

        UVM_ASSERT(uvm_hmm_check_context_vma_is_valid(va_block, vma, region));

        status = uvm_hmm_va_block_migrate_locked(va_block, va_block_retry, service_context, UVM_ID_CPU, region, cause);
        if (status != NV_OK)
            goto done;

        // make_device_exclusive_range() will try to call migrate_to_ram()
        // and deadlock with ourself if the data isn't CPU resident.
        if (!uvm_processor_mask_test(&va_block->resident, UVM_ID_CPU) ||
            !uvm_va_block_cpu_is_region_resident_on(va_block, NUMA_NO_NODE, region)) {
            status = NV_WARN_MORE_PROCESSING_REQUIRED;
            goto done;
        }
    }

    // TODO: Bug 4014681: atomic GPU operations are not supported on MAP_SHARED
    // mmap() files so we check for that here and report a fatal fault.
    // Otherwise with the current Linux 6.1 make_device_exclusive_range(),
    // it doesn't make the page exclusive and we end up in an endless loop.
    if (vma->vm_flags & (VM_SHARED | VM_HUGETLB)) {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    hmm_range_fault_begin(va_block);

    uvm_mutex_unlock(&va_block->lock);

    npages = hmm_make_device_exclusive_range(service_context->block_context->mm,
        uvm_va_block_cpu_page_address(va_block, region.first),
        uvm_va_block_cpu_page_address(va_block, region.outer - 1) + PAGE_SIZE,
        pages + region.first);

    uvm_mutex_lock(&va_block->lock);

    if (npages < 0) {
        status = (npages == -EBUSY) ? NV_WARN_MORE_PROCESSING_REQUIRED : errno_to_nv_status(npages);
        goto done;
    }

    while ((size_t)npages < uvm_va_block_region_num_pages(region))
        pages[region.first + npages++] = NULL;

    if (hmm_range_fault_retry(va_block)) {
        status = NV_WARN_MORE_PROCESSING_REQUIRED;
        goto release;
    }

    status = NV_OK;

    for_each_va_block_page_in_region(page_index, region) {
        struct page *page = pages[page_index];

        if (!page) {
            // Record that one of the pages isn't exclusive but keep converting
            // the others.
            status = NV_WARN_MORE_PROCESSING_REQUIRED;
            continue;
        }

        // If a CPU chunk is already allocated, check to see it matches what
        // make_device_exclusive_range() found.
        if (uvm_page_mask_test(&va_block->cpu.allocated, page_index)) {
            UVM_ASSERT(hmm_va_block_cpu_page_is_same(va_block, page_index, page));
            UVM_ASSERT(uvm_processor_mask_test(&va_block->resident, UVM_ID_CPU));
            UVM_ASSERT(uvm_va_block_cpu_is_page_resident_on(va_block, NUMA_NO_NODE, page_index));
        }
        else {
            NV_STATUS s = hmm_va_block_cpu_page_populate(va_block, page_index, page);

            if (s == NV_OK)
                uvm_va_block_cpu_set_resident_page(va_block, page_to_nid(page), page_index);
        }

        cpu_mapping_clear(va_block, page_index);
    }

    if (status != NV_OK)
        goto release;

    status = uvm_va_block_service_copy(processor_id, UVM_ID_CPU, va_block, va_block_retry, service_context);
    if (status != NV_OK)
        goto release;

    status = uvm_va_block_service_finish(processor_id, va_block, service_context);

release:
    hmm_release_atomic_pages(va_block, service_context);

done:
    return status;
}

static bool is_atomic_fault(NvU8 *access_type, uvm_va_block_region_t region)
{
    uvm_page_index_t page_index;

    for_each_va_block_page_in_region(page_index, region) {
        if (access_type[page_index] == UVM_FAULT_ACCESS_TYPE_ATOMIC_STRONG)
            return true;
    }

    return false;
}

static bool is_gpu_resident(uvm_va_block_t *va_block, uvm_va_block_region_t region)
{
    uvm_processor_id_t gpu_id;

    for_each_gpu_id_in_mask(gpu_id, &va_block->resident) {
        uvm_va_block_gpu_state_t *gpu_state;

        gpu_state = uvm_va_block_gpu_state_get(va_block, gpu_id);
        if (!uvm_page_mask_region_empty(&gpu_state->resident, region))
            return true;
    }

    return false;
}

static NV_STATUS hmm_block_cpu_fault_locked(uvm_processor_id_t processor_id,
                                            uvm_va_block_t *va_block,
                                            uvm_va_block_retry_t *va_block_retry,
                                            uvm_service_block_context_t *service_context)
{
    uvm_va_block_region_t region = service_context->region;
    struct migrate_vma *args = &service_context->block_context->hmm.migrate_vma_args;
    NV_STATUS status;
    int ret;
    uvm_hmm_devmem_fault_context_t fault_context = {
        .processor_id = processor_id,
        .va_block = va_block,
        .va_block_retry = va_block_retry,
        .service_context = service_context,
    };

    // Normally the source page will be a device private page that is being
    // migrated to system memory. However, when it is a GPU fault, the source
    // page can be a system memory page that the GPU needs to remote map
    // instead. However migrate_vma_setup() won't select these types of
    // mappings/pages:
    //  - device exclusive PTEs
    //  - shared mappings
    //  - file backed mappings
    // Also, if the source and destination page are the same, the page reference
    // count won't be the "expected" count and migrate_vma_pages() won't migrate
    // it. This mask records that uvm_hmm_devmem_fault_alloc_and_copy() and
    // uvm_hmm_devmem_fault_finalize_and_map() still needs to process these
    // pages even if src_pfn indicates they are not migrating.
    uvm_page_mask_zero(&fault_context.same_devmem_page_mask);

    if (!UVM_ID_IS_CPU(processor_id)) {
        if (is_atomic_fault(service_context->access_type, region)) {
            return hmm_block_atomic_fault_locked(processor_id,
                                                 va_block,
                                                 va_block_retry,
                                                 service_context);
        }

        status = hmm_make_resident_cpu(va_block,
                                       service_context->block_context->hmm.vma,
                                       service_context->block_context->hmm.src_pfns,
                                       region,
                                       service_context->access_type,
                                       &fault_context.same_devmem_page_mask);
        if (status != NV_OK)
            return status;

        // If no GPU has a resident copy, we can skip the migrate_vma_*().
        // This is necessary if uvm_hmm_must_use_sysmem() returned true.
        if (!is_gpu_resident(va_block, region)) {
            status = uvm_va_block_service_copy(processor_id,
                                               UVM_ID_CPU,
                                               va_block,
                                               va_block_retry,
                                               service_context);
            if (status != NV_OK)
                return status;

            return uvm_va_block_service_finish(processor_id, va_block, service_context);
        }
    }

    args->vma = service_context->block_context->hmm.vma;
    args->src = service_context->block_context->hmm.src_pfns + region.first;
    args->dst = service_context->block_context->hmm.dst_pfns + region.first;
    args->start = uvm_va_block_region_start(va_block, region);
    args->end = uvm_va_block_region_end(va_block, region) + 1;
    args->flags = MIGRATE_VMA_SELECT_DEVICE_PRIVATE;
    args->pgmap_owner = &g_uvm_global;

    if (UVM_ID_IS_CPU(processor_id)) {
        args->fault_page = service_context->cpu_fault.vmf->page;
    }
    else {
        args->flags |= MIGRATE_VMA_SELECT_SYSTEM;
        args->fault_page = NULL;
    }

    ret = migrate_vma_setup_locked(args, va_block);
    UVM_ASSERT(!ret);

    // The overall process here is to migrate pages from the GPU to the CPU
    // and possibly remote map the GPU to sysmem if accessed_by is set.
    // This is safe because we hold the va_block lock across the calls to
    // uvm_hmm_devmem_fault_alloc_and_copy(), migrate_vma_pages(),
    // uvm_hmm_devmem_fault_finalize_and_map(), and migrate_vma_finalize().
    // If uvm_hmm_devmem_fault_alloc_and_copy() needs to drop the va_block
    // lock, a sequence number is used to tell if an invalidate() callback
    // occurred while not holding the lock. If the sequence number changes,
    // all the locks need to be dropped (mm, va_space, va_block) and the whole
    // uvm_va_block_service_locked() called again. Otherwise, there were no
    // conflicting invalidate callbacks and our snapshots of the CPU page
    // tables are accurate and can be used to DMA pages and update GPU page
    // tables.
    status = uvm_hmm_devmem_fault_alloc_and_copy(&fault_context);
    if (status == NV_OK) {
        migrate_vma_pages(args);
        status = uvm_hmm_devmem_fault_finalize_and_map(&fault_context);
    }

    migrate_vma_finalize(args);

    if (status == NV_WARN_NOTHING_TO_DO)
        status = NV_OK;

    return status;
}

static NV_STATUS dmamap_src_sysmem_pages(uvm_va_block_t *va_block,
                                         struct vm_area_struct *vma,
                                         const unsigned long *src_pfns,
                                         unsigned long *dst_pfns,
                                         uvm_va_block_region_t region,
                                         uvm_page_mask_t *page_mask,
                                         uvm_processor_id_t dest_id,
                                         uvm_service_block_context_t *service_context)
{
    uvm_page_index_t page_index;
    NV_STATUS status = NV_OK;

    UVM_ASSERT(service_context);

    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        struct page *src_page;

        if (!(src_pfns[page_index] & MIGRATE_PFN_MIGRATE)) {
            // HMM currently has some limitations on what pages can be migrated.
            // For example, no file backed pages, device private pages owned by
            // a different device, device exclusive or swapped out pages.
            goto clr_mask;
        }

        // This is the page that will be copied to the destination GPU.
        src_page = migrate_pfn_to_page(src_pfns[page_index]);
        if (src_page) {
            if (is_device_private_page(src_page)) {
                status = gpu_chunk_add(va_block, page_index, src_page);
                if (status != NV_OK)
                    break;
                continue;
            }

            if (nv_PageSwapCache(src_page)) {
                // TODO: Bug 4050579: Remove this when swap cached pages can be
                // migrated.
                status = NV_WARN_MISMATCHED_TARGET;
                break;
            }

            // If the page is already allocated, it is most likely a mirrored
            // page. Check to be sure it matches what we have recorded. The
            // page shouldn't be a staging page from a GPU to GPU migration
            // or a remote mapped atomic sysmem page because migrate_vma_setup()
            // found a normal page and non-mirrored pages are only known
            // privately to the UVM driver.
            if (uvm_page_mask_test(&va_block->cpu.allocated, page_index)) {
                UVM_ASSERT(hmm_va_block_cpu_page_is_same(va_block, page_index, src_page));
                UVM_ASSERT(uvm_processor_mask_test(&va_block->resident, UVM_ID_CPU));
                UVM_ASSERT(uvm_va_block_cpu_is_page_resident_on(va_block, NUMA_NO_NODE, page_index));
            }
            else {
                status = hmm_va_block_cpu_page_populate(va_block, page_index, src_page);
                if (status != NV_OK)
                    goto clr_mask;

                // Since there is a CPU resident page, there shouldn't be one
                // anywhere else. TODO: Bug 3660922: Need to handle read
                // duplication at some point.
                UVM_ASSERT(!uvm_va_block_page_resident_processors_count(va_block,
                                                                        service_context->block_context,
                                                                        page_index));

                // migrate_vma_setup() was able to isolate and lock the page;
                // therefore, it is CPU resident and not mapped.
                uvm_va_block_cpu_set_resident_page(va_block, page_to_nid(src_page), page_index);
            }

            // The call to migrate_vma_setup() will have inserted a migration
            // PTE so the CPU has no access.
            cpu_mapping_clear(va_block, page_index);
        }
        else {
            // It is OK to migrate an empty anonymous page, a zero page will
            // be allocated on the GPU. Just be sure to free any pages
            // used for GPU to GPU copies. It can't be an evicted page because
            // migrate_vma_setup() would have found a source page.
            if (uvm_page_mask_test(&va_block->cpu.allocated, page_index)) {
                UVM_ASSERT(!uvm_va_block_page_resident_processors_count(va_block,
                                                                        service_context->block_context,
                                                                        page_index));
                hmm_va_block_cpu_page_unpopulate(va_block, page_index, NULL);
            }
        }

        continue;

    clr_mask:
        // TODO: Bug 3900774: clean up murky mess of mask clearing.
        uvm_page_mask_clear(page_mask, page_index);
        if (service_context)
            clear_service_context_masks(service_context, dest_id, page_index);
    }

    if (uvm_page_mask_empty(page_mask))
        status = NV_WARN_MORE_PROCESSING_REQUIRED;

    if (status != NV_OK)
        clean_up_non_migrating_pages(va_block, src_pfns, dst_pfns, region, page_mask);

    return status;
}

static NV_STATUS uvm_hmm_gpu_fault_alloc_and_copy(struct vm_area_struct *vma,
                                                  uvm_hmm_gpu_fault_event_t *uvm_hmm_gpu_fault_event)
{
    uvm_processor_id_t processor_id;
    uvm_processor_id_t new_residency;
    uvm_va_block_t *va_block;
    uvm_va_block_retry_t *va_block_retry;
    uvm_service_block_context_t *service_context;
    uvm_perf_prefetch_hint_t *prefetch_hint;
    const unsigned long *src_pfns;
    unsigned long *dst_pfns;
    uvm_va_block_region_t region;
    uvm_page_mask_t *page_mask;
    NV_STATUS status;

    processor_id = uvm_hmm_gpu_fault_event->processor_id;
    new_residency = uvm_hmm_gpu_fault_event->new_residency;
    va_block = uvm_hmm_gpu_fault_event->va_block;
    va_block_retry = uvm_hmm_gpu_fault_event->va_block_retry;
    service_context = uvm_hmm_gpu_fault_event->service_context;
    region = service_context->region;
    prefetch_hint = &service_context->prefetch_hint;
    src_pfns = service_context->block_context->hmm.src_pfns;
    dst_pfns = service_context->block_context->hmm.dst_pfns;

    // Build the migration mask.
    // Note that thrashing pinned pages are already accounted for in
    // service_context->resident_processors.
    page_mask = &uvm_hmm_gpu_fault_event->page_mask;
    uvm_page_mask_copy(page_mask,
                       &service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency);

    status = dmamap_src_sysmem_pages(va_block,
                                     vma,
                                     src_pfns,
                                     dst_pfns,
                                     region,
                                     page_mask,
                                     new_residency,
                                     service_context);
    if (status != NV_OK)
        return status;

    // Do the alloc and copy but don't update the residency or mapping for the
    // new location yet.
    status = uvm_va_block_service_copy(processor_id, new_residency, va_block, va_block_retry, service_context);
    if (status != NV_OK)
        return status;

    // Record the destination PFNs of device private struct pages now that
    // uvm_va_block_service_copy() has populated the GPU destination pages.
    fill_dst_pfns(va_block,
                  src_pfns,
                  dst_pfns,
                  region,
                  page_mask,
                  &uvm_hmm_gpu_fault_event->same_devmem_page_mask,
                  new_residency);

    return status;
}

static NV_STATUS uvm_hmm_gpu_fault_finalize_and_map(uvm_hmm_gpu_fault_event_t *uvm_hmm_gpu_fault_event)
{
    uvm_processor_id_t processor_id;
    uvm_processor_id_t new_residency;
    uvm_va_block_t *va_block;
    uvm_va_block_retry_t *va_block_retry;
    uvm_service_block_context_t *service_context;
    const unsigned long *src_pfns;
    unsigned long *dst_pfns;
    uvm_va_block_region_t region;
    uvm_page_index_t page_index;
    uvm_page_mask_t *page_mask;
    NV_STATUS status, tracker_status;

    processor_id = uvm_hmm_gpu_fault_event->processor_id;
    new_residency = uvm_hmm_gpu_fault_event->new_residency;
    va_block = uvm_hmm_gpu_fault_event->va_block;
    va_block_retry = uvm_hmm_gpu_fault_event->va_block_retry;
    service_context = uvm_hmm_gpu_fault_event->service_context;
    src_pfns = service_context->block_context->hmm.src_pfns;
    dst_pfns = service_context->block_context->hmm.dst_pfns;
    region = service_context->region;
    page_mask = &uvm_hmm_gpu_fault_event->page_mask;

    // There are a number of reasons why HMM will mark a page as not migrating
    // even if we set a valid entry in dst_pfns[]. Mark these pages accordingly.
    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        unsigned long src_pfn = src_pfns[page_index];

        if (src_pfn & MIGRATE_PFN_MIGRATE)
            continue;

        // If a device private page isn't migrating and only the GPU page table
        // is being updated, continue to process it normally.
        if (uvm_page_mask_test(&uvm_hmm_gpu_fault_event->same_devmem_page_mask, page_index))
            continue;

        // TODO: Bug 3900774: clean up murky mess of mask clearing.
        uvm_page_mask_clear(page_mask, page_index);
        clear_service_context_masks(service_context, new_residency, page_index);
    }

    if (uvm_page_mask_empty(page_mask))
        status = NV_WARN_MORE_PROCESSING_REQUIRED;
    else
        status = uvm_va_block_service_finish(processor_id, va_block, service_context);

    tracker_status = sync_page_and_chunk_state(va_block,
                                               src_pfns,
                                               dst_pfns,
                                               region,
                                               page_mask,
                                               &uvm_hmm_gpu_fault_event->same_devmem_page_mask);

    return status == NV_OK ? tracker_status : status;
}

NV_STATUS uvm_hmm_va_block_service_locked(uvm_processor_id_t processor_id,
                                          uvm_processor_id_t new_residency,
                                          uvm_va_block_t *va_block,
                                          uvm_va_block_retry_t *va_block_retry,
                                          uvm_service_block_context_t *service_context)
{
    struct mm_struct *mm = service_context->block_context->mm;
    struct vm_area_struct *vma = service_context->block_context->hmm.vma;
    uvm_va_block_region_t region = service_context->region;
    uvm_hmm_gpu_fault_event_t uvm_hmm_gpu_fault_event;
    struct migrate_vma *args = &service_context->block_context->hmm.migrate_vma_args;
    int ret;
    NV_STATUS status = NV_ERR_INVALID_ADDRESS;

    if (!mm)
        return status;

    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_block->hmm.va_space->lock);
    uvm_assert_mutex_locked(&va_block->hmm.migrate_lock);
    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(vma);

    // If the desired destination is the CPU, try to fault in CPU pages.
    if (UVM_ID_IS_CPU(new_residency))
        return hmm_block_cpu_fault_locked(processor_id, va_block, va_block_retry, service_context);

    uvm_hmm_gpu_fault_event.processor_id = processor_id;
    uvm_hmm_gpu_fault_event.new_residency = new_residency;
    uvm_hmm_gpu_fault_event.va_block = va_block;
    uvm_hmm_gpu_fault_event.va_block_retry = va_block_retry;
    uvm_hmm_gpu_fault_event.service_context = service_context;

    args->vma = vma;
    args->src = service_context->block_context->hmm.src_pfns + region.first;
    args->dst = service_context->block_context->hmm.dst_pfns + region.first;
    args->start = uvm_va_block_region_start(va_block, region);
    args->end = uvm_va_block_region_end(va_block, region) + 1;
    args->flags = MIGRATE_VMA_SELECT_DEVICE_PRIVATE | MIGRATE_VMA_SELECT_SYSTEM;
    args->pgmap_owner = &g_uvm_global;
    args->fault_page = NULL;

    ret = migrate_vma_setup_locked(args, va_block);
    UVM_ASSERT(!ret);

    // The overall process here is to migrate pages from the CPU or GPUs to the
    // faulting GPU.
    // This is safe because we hold the va_block lock across the calls to
    // uvm_hmm_gpu_fault_alloc_and_copy(), migrate_vma_pages(),
    // uvm_hmm_gpu_fault_finalize_and_map(), and migrate_vma_finalize().
    // If uvm_hmm_gpu_fault_alloc_and_copy() needs to drop the va_block
    // lock, a sequence number is used to tell if an invalidate() callback
    // occurred while not holding the lock. If the sequence number changes,
    // all the locks need to be dropped (mm, va_space, va_block) and the whole
    // uvm_va_block_service_locked() called again. Otherwise, there were no
    // conflicting invalidate callbacks and our snapshots of the CPU page
    // tables are accurate and can be used to DMA pages and update GPU page
    // tables. TODO: Bug 3901904: there might be better ways of handling no
    // page being migrated.
    status = uvm_hmm_gpu_fault_alloc_and_copy(vma, &uvm_hmm_gpu_fault_event);
    if (status == NV_WARN_MORE_PROCESSING_REQUIRED) {
        migrate_vma_finalize(args);

        // migrate_vma_setup() might have not been able to lock/isolate any
        // pages because they are swapped out or are device exclusive.
        // We do know that none of the pages in the region are zero pages
        // since migrate_vma_setup() would have reported that information.
        // Try to make it resident in system memory and retry the migration.
        status = hmm_make_resident_cpu(va_block,
                                       service_context->block_context->hmm.vma,
                                       service_context->block_context->hmm.src_pfns,
                                       region,
                                       service_context->access_type,
                                       NULL);
        return NV_WARN_MORE_PROCESSING_REQUIRED;
    }

    if (status == NV_OK) {
        migrate_vma_pages(args);
        status = uvm_hmm_gpu_fault_finalize_and_map(&uvm_hmm_gpu_fault_event);
    }

    migrate_vma_finalize(args);

    if (status == NV_WARN_NOTHING_TO_DO)
        status = NV_OK;

    return status;
}

static NV_STATUS uvm_hmm_migrate_alloc_and_copy(struct vm_area_struct *vma,
                                                uvm_hmm_migrate_event_t *uvm_hmm_migrate_event)
{
    uvm_va_block_t *va_block;
    uvm_va_block_retry_t *va_block_retry;
    uvm_service_block_context_t *service_context;
    const unsigned long *src_pfns;
    unsigned long *dst_pfns;
    uvm_va_block_region_t region;
    uvm_processor_id_t dest_id;
    uvm_page_mask_t *page_mask;
    NV_STATUS status;

    va_block = uvm_hmm_migrate_event->va_block;
    va_block_retry = uvm_hmm_migrate_event->va_block_retry;
    service_context = uvm_hmm_migrate_event->service_context;
    src_pfns = service_context->block_context->hmm.src_pfns;
    dst_pfns = service_context->block_context->hmm.dst_pfns;
    region = uvm_hmm_migrate_event->region;
    dest_id = uvm_hmm_migrate_event->dest_id;
    page_mask = &uvm_hmm_migrate_event->page_mask;
    uvm_page_mask_init_from_region(page_mask, region, NULL);
    uvm_page_mask_zero(&uvm_hmm_migrate_event->same_devmem_page_mask);

    uvm_assert_mutex_locked(&va_block->lock);

    if (UVM_ID_IS_CPU(dest_id)) {
        status = migrate_alloc_on_cpu(va_block,
                                      src_pfns,
                                      dst_pfns,
                                      region,
                                      page_mask,
                                      &uvm_hmm_migrate_event->same_devmem_page_mask,
                                      service_context->block_context);
    }
    else {
        status = dmamap_src_sysmem_pages(va_block,
                                         vma,
                                         src_pfns,
                                         dst_pfns,
                                         region,
                                         page_mask,
                                         dest_id,
                                         service_context);
    }

    if (status != NV_OK)
        return status;

    status = uvm_va_block_make_resident_copy(va_block,
                                             va_block_retry,
                                             service_context->block_context,
                                             dest_id,
                                             region,
                                             page_mask,
                                             NULL,
                                             uvm_hmm_migrate_event->cause);
    if (status != NV_OK)
        return status;

    if (!UVM_ID_IS_CPU(dest_id)) {
        // Record the destination PFNs of device private struct pages now that
        // uvm_va_block_make_resident_copy() has populated the GPU destination
        // pages.
        fill_dst_pfns(va_block,
                      src_pfns,
                      dst_pfns,
                      region,
                      page_mask,
                      &uvm_hmm_migrate_event->same_devmem_page_mask,
                      dest_id);
    }

    return status;
}

static NV_STATUS uvm_hmm_migrate_finalize(uvm_hmm_migrate_event_t *uvm_hmm_migrate_event)
{
    uvm_va_block_t *va_block;
    uvm_va_block_retry_t *va_block_retry;
    uvm_va_block_context_t *va_block_context;
    uvm_va_block_region_t region;
    uvm_processor_id_t dest_id;
    uvm_page_index_t page_index;
    uvm_page_mask_t *page_mask;
    const unsigned long *src_pfns;
    unsigned long *dst_pfns;

    va_block = uvm_hmm_migrate_event->va_block;
    va_block_retry = uvm_hmm_migrate_event->va_block_retry;
    va_block_context = uvm_hmm_migrate_event->service_context->block_context;
    region = uvm_hmm_migrate_event->region;
    dest_id = uvm_hmm_migrate_event->dest_id;
    page_mask = &uvm_hmm_migrate_event->page_mask;
    src_pfns = va_block_context->hmm.src_pfns;
    dst_pfns = va_block_context->hmm.dst_pfns;

    uvm_assert_mutex_locked(&va_block->lock);

    // There are a number of reasons why HMM will mark a page as not migrating
    // even if we set a valid entry in dst_pfns[]. Mark these pages accordingly.
    for_each_va_block_page_in_region_mask(page_index, page_mask, region) {
        unsigned long src_pfn = src_pfns[page_index];

        if (src_pfn & MIGRATE_PFN_MIGRATE)
            continue;

        // If a device private page isn't migrating and only the GPU page table
        // is being updated, continue to process it normally.
        if (uvm_page_mask_test(&uvm_hmm_migrate_event->same_devmem_page_mask, page_index))
            continue;

        uvm_page_mask_clear(page_mask, page_index);
    }

    uvm_va_block_make_resident_finish(va_block, va_block_context, region, page_mask);

    return sync_page_and_chunk_state(va_block,
                                     src_pfns,
                                     dst_pfns,
                                     region,
                                     page_mask,
                                     &uvm_hmm_migrate_event->same_devmem_page_mask);
}

// Note that migrate_vma_*() doesn't handle asynchronous migrations so the
// migration flag UVM_MIGRATE_FLAG_SKIP_CPU_MAP doesn't have an effect.
// TODO: Bug 3900785: investigate ways to implement async migration.
NV_STATUS uvm_hmm_va_block_migrate_locked(uvm_va_block_t *va_block,
                                          uvm_va_block_retry_t *va_block_retry,
                                          uvm_service_block_context_t *service_context,
                                          uvm_processor_id_t dest_id,
                                          uvm_va_block_region_t region,
                                          uvm_make_resident_cause_t cause)
{
    uvm_hmm_migrate_event_t uvm_hmm_migrate_event;
    uvm_va_block_context_t *va_block_context = service_context->block_context;
    struct vm_area_struct *vma = va_block_context->hmm.vma;
    NvU64 start;
    NvU64 end;
    struct migrate_vma *args = &va_block_context->hmm.migrate_vma_args;
    NV_STATUS status;
    int ret;

    UVM_ASSERT(vma);
    UVM_ASSERT(va_block_context->mm == vma->vm_mm);
    uvm_assert_mmap_lock_locked(va_block_context->mm);
    UVM_ASSERT(uvm_hmm_check_context_vma_is_valid(va_block, vma, region));
    uvm_assert_rwsem_locked(&va_block->hmm.va_space->lock);
    uvm_assert_mutex_locked(&va_block->hmm.migrate_lock);
    uvm_assert_mutex_locked(&va_block->lock);

    start = uvm_va_block_region_start(va_block, region);
    end = uvm_va_block_region_end(va_block, region);
    UVM_ASSERT(vma->vm_start <= start && end < vma->vm_end);

    uvm_hmm_migrate_event.va_block = va_block;
    uvm_hmm_migrate_event.va_block_retry = va_block_retry;
    uvm_hmm_migrate_event.service_context = service_context;
    uvm_hmm_migrate_event.region = region;
    uvm_hmm_migrate_event.dest_id = dest_id;
    uvm_hmm_migrate_event.cause = cause;

    args->vma = vma;
    args->src = va_block_context->hmm.src_pfns + region.first;
    args->dst = va_block_context->hmm.dst_pfns + region.first;
    args->start = uvm_va_block_region_start(va_block, region);
    args->end = uvm_va_block_region_end(va_block, region) + 1;
    args->flags = UVM_ID_IS_CPU(dest_id) ? MIGRATE_VMA_SELECT_DEVICE_PRIVATE :
                                           MIGRATE_VMA_SELECT_DEVICE_PRIVATE | MIGRATE_VMA_SELECT_SYSTEM;
    args->pgmap_owner = &g_uvm_global;
    args->fault_page = NULL;

    // Note that migrate_vma_setup() doesn't handle file backed or VM_SPECIAL
    // VMAs so if UvmMigrate() tries to migrate such a region, -EINVAL will
    // be returned and we will only try to make the pages be CPU resident.
    ret = migrate_vma_setup_locked(args, va_block);
    if (ret)
        return hmm_make_resident_cpu(va_block,
                                     vma,
                                     va_block_context->hmm.src_pfns,
                                     region,
                                     NULL,
                                     NULL);

    // The overall process here is to migrate pages from the CPU or GPUs to the
    // destination processor. Note that block_migrate_add_mappings() handles
    // updating GPU mappings after the migration.
    // This is safe because we hold the va_block lock across the calls to
    // uvm_hmm_migrate_alloc_and_copy(), migrate_vma_pages(),
    // uvm_hmm_migrate_finalize(), migrate_vma_finalize() and
    // block_migrate_add_mappings().
    // If uvm_hmm_migrate_alloc_and_copy() needs to drop the va_block
    // lock, a sequence number is used to tell if an invalidate() callback
    // occurred while not holding the lock. If the sequence number changes,
    // all the locks need to be dropped (mm, va_space, va_block) and the whole
    // uvm_hmm_va_block_migrate_locked() called again. Otherwise, there were no
    // conflicting invalidate callbacks and our snapshots of the CPU page
    // tables are accurate and can be used to DMA pages and update GPU page
    // tables.
    status = uvm_hmm_migrate_alloc_and_copy(vma, &uvm_hmm_migrate_event);
    if (status == NV_WARN_MORE_PROCESSING_REQUIRED) {
        uvm_processor_id_t id;
        uvm_page_mask_t *page_mask;

        migrate_vma_finalize(args);

        // The CPU pages tables might contain only device private pages or
        // the migrate_vma_setup() might have not been able to lock/isolate
        // any pages because they are swapped out, or on another device.
        // We do know that none of the pages in the region are zero pages
        // since migrate_vma_setup() would have reported that information.
        // Collect all the pages that need to be faulted in and made CPU
        // resident, then do the hmm_range_fault() and retry.
        page_mask = &va_block_context->caller_page_mask;
        uvm_page_mask_init_from_region(page_mask, region, NULL);

        for_each_id_in_mask(id, &va_block->resident) {
            if (!uvm_page_mask_andnot(page_mask, page_mask, uvm_va_block_resident_mask_get(va_block, id, NUMA_NO_NODE)))
                return NV_OK;
        }

        return hmm_make_resident_cpu(va_block,
                                     vma,
                                     va_block_context->hmm.src_pfns,
                                     region,
                                     NULL,
                                     NULL);
    }

    if (status == NV_OK) {
        migrate_vma_pages(args);
        status = uvm_hmm_migrate_finalize(&uvm_hmm_migrate_event);
    }

    migrate_vma_finalize(args);

    if (status == NV_WARN_NOTHING_TO_DO || status == NV_WARN_MISMATCHED_TARGET)
        status = NV_OK;

    return status;
}

NV_STATUS uvm_hmm_migrate_ranges(uvm_va_space_t *va_space,
                                 uvm_service_block_context_t *service_context,
                                 NvU64 base,
                                 NvU64 length,
                                 uvm_processor_id_t dest_id,
                                 uvm_migrate_mode_t mode,
                                 uvm_tracker_t *out_tracker)
{
    struct mm_struct *mm;
    uvm_va_block_t *va_block;
    uvm_va_block_retry_t va_block_retry;
    NvU64 addr, end, last_address;
    NV_STATUS status = NV_OK;
    uvm_va_block_context_t *block_context = service_context->block_context;

    if (!uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    mm = block_context->mm;
    UVM_ASSERT(mm == va_space->va_space_mm.mm);
    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_space->lock);

    last_address = base + length - 1;

    for (addr = base; addr < last_address; addr = end + 1) {
        struct vm_area_struct *vma;

        status = hmm_va_block_find_create(va_space, addr, false, &block_context->hmm.vma, &va_block);
        if (status != NV_OK)
            return status;

        end = va_block->end;
        if (end > last_address)
            end = last_address;

        vma = block_context->hmm.vma;
        if (end > vma->vm_end - 1)
            end = vma->vm_end - 1;

        status = hmm_migrate_range(va_block, &va_block_retry, service_context, dest_id, addr, end, mode, out_tracker);
        if (status != NV_OK)
            break;
    }

    return status;
}

NV_STATUS uvm_hmm_va_block_evict_chunk_prep(uvm_va_block_t *va_block,
                                            uvm_va_block_context_t *va_block_context,
                                            uvm_gpu_chunk_t *gpu_chunk,
                                            uvm_va_block_region_t chunk_region)
{
    uvm_thread_context_t *uvm_context = uvm_thread_context();
    unsigned long *src_pfns = va_block_context->hmm.src_pfns;
    uvm_gpu_t *gpu = uvm_gpu_chunk_get_gpu(gpu_chunk);
    unsigned long pfn = uvm_pmm_gpu_devmem_get_pfn(&gpu->pmm, gpu_chunk);
    uvm_page_index_t page_index = chunk_region.first;
    int ret;

    uvm_assert_mutex_locked(&va_block->lock);
    // TODO: Bug 3368756: add support for large GPU pages.
    UVM_ASSERT(uvm_va_block_region_num_pages(chunk_region) == 1);

    uvm_context->ignore_hmm_invalidate_va_block = va_block;
    ret = migrate_device_range(src_pfns + page_index, pfn, uvm_va_block_region_num_pages(chunk_region));
    uvm_context->ignore_hmm_invalidate_va_block = NULL;
    if (ret)
        return errno_to_nv_status(ret);

    return NV_OK;
}

// Note that the caller must initialize va_block_context->hmm.src_pfns by
// calling uvm_hmm_va_block_evict_chunk_prep() before calling this.
static NV_STATUS hmm_va_block_evict_chunks(uvm_va_block_t *va_block,
                                           uvm_service_block_context_t *service_context,
                                           const uvm_page_mask_t *pages_to_evict,
                                           uvm_va_block_region_t region,
                                           uvm_make_resident_cause_t cause,
                                           bool *out_accessed_by_set)
{
    uvm_va_block_context_t *va_block_context = service_context->block_context;
    NvU64 start = uvm_va_block_region_start(va_block, region);
    NvU64 end = uvm_va_block_region_end(va_block, region);
    unsigned long *src_pfns = va_block_context->hmm.src_pfns;
    unsigned long *dst_pfns = va_block_context->hmm.dst_pfns;
    uvm_hmm_migrate_event_t uvm_hmm_migrate_event = {
        .va_block = va_block,
        .va_block_retry = NULL,
        .service_context = service_context,
        .region = region,
        .dest_id = UVM_ID_CPU,
        .cause = cause,
    };
    uvm_page_mask_t *page_mask = &uvm_hmm_migrate_event.page_mask;
    const uvm_va_policy_t *policy;
    uvm_va_policy_node_t *node;
    uvm_page_mask_t *cpu_resident_mask = uvm_va_block_resident_mask_get(va_block, UVM_ID_CPU, NUMA_NO_NODE);
    unsigned long npages;
    NV_STATUS status;

    uvm_assert_mutex_locked(&va_block->lock);

    if (out_accessed_by_set)
        *out_accessed_by_set = false;

    // Note that there is no VMA available when evicting HMM pages.
    va_block_context->hmm.vma = NULL;

    uvm_page_mask_copy(page_mask, pages_to_evict);

    uvm_for_each_va_policy_in(policy, va_block, start, end, node, region) {
        npages = uvm_va_block_region_num_pages(region);

        if (out_accessed_by_set && uvm_processor_mask_get_count(&policy->accessed_by) > 0)
            *out_accessed_by_set = true;

        // Pages resident on the GPU should not have a resident page in system
        // memory.
        // TODO: Bug 3660922: Need to handle read duplication at some point.
        UVM_ASSERT(uvm_page_mask_region_empty(cpu_resident_mask, region));

        status = migrate_alloc_on_cpu(va_block, src_pfns, dst_pfns, region, page_mask, NULL, va_block_context);
        if (status != NV_OK)
            goto err;

        status = uvm_va_block_make_resident_copy(va_block,
                                                 NULL,
                                                 va_block_context,
                                                 UVM_ID_CPU,
                                                 region,
                                                 page_mask,
                                                 NULL,
                                                 cause);
        if (status != NV_OK)
            goto err;

        migrate_device_pages(src_pfns + region.first, dst_pfns + region.first, npages);

        uvm_hmm_migrate_event.region = region;

        status = uvm_hmm_migrate_finalize(&uvm_hmm_migrate_event);
        if (status != NV_OK)
            goto err;

        migrate_device_finalize(src_pfns + region.first, dst_pfns + region.first, npages);
    }

    return NV_OK;

err:
    migrate_device_finalize(src_pfns + region.first, dst_pfns + region.first, npages);
    return status;
}

NV_STATUS uvm_hmm_va_block_evict_chunks(uvm_va_block_t *va_block,
                                        uvm_service_block_context_t *service_context,
                                        const uvm_page_mask_t *pages_to_evict,
                                        uvm_va_block_region_t region,
                                        bool *out_accessed_by_set)
{
    return hmm_va_block_evict_chunks(va_block,
                                     service_context,
                                     pages_to_evict,
                                     region,
                                     UVM_MAKE_RESIDENT_CAUSE_EVICTION,
                                     out_accessed_by_set);
}

NV_STATUS uvm_hmm_va_block_evict_pages_from_gpu(uvm_va_block_t *va_block,
                                                uvm_gpu_t *gpu,
                                                uvm_service_block_context_t *service_context,
                                                const uvm_page_mask_t *pages_to_evict,
                                                uvm_va_block_region_t region)
{
    uvm_va_block_context_t *block_context = service_context->block_context;
    unsigned long *src_pfns = block_context->hmm.src_pfns;
    uvm_va_block_gpu_state_t *gpu_state;
    uvm_page_index_t page_index;
    uvm_gpu_chunk_t *gpu_chunk;
    NV_STATUS status;

    uvm_assert_mutex_locked(&va_block->lock);

    gpu_state = uvm_va_block_gpu_state_get(va_block, gpu->id);
    UVM_ASSERT(gpu_state);
    UVM_ASSERT(gpu_state->chunks);

    // Fill in the src_pfns[] with the ZONE_DEVICE private PFNs of the GPU.
    memset(src_pfns, 0, sizeof(block_context->hmm.src_pfns));

    // TODO: Bug 3368756: add support for large GPU pages.
    for_each_va_block_page_in_region_mask(page_index, pages_to_evict, region) {
        gpu_chunk = uvm_va_block_lookup_gpu_chunk(va_block,
                                                  gpu,
                                                  uvm_va_block_cpu_page_address(va_block, page_index));
        status = uvm_hmm_va_block_evict_chunk_prep(va_block,
                                                   block_context,
                                                   gpu_chunk,
                                                   uvm_va_block_region_for_page(page_index));
        if (status != NV_OK)
            return status;
    }

    return hmm_va_block_evict_chunks(va_block,
                                     service_context,
                                     pages_to_evict,
                                     region,
                                     UVM_MAKE_RESIDENT_CAUSE_API_MIGRATE,
                                     NULL);
}

NV_STATUS uvm_hmm_remote_cpu_fault(struct vm_fault *vmf)
{
    NV_STATUS status = NV_OK;
    unsigned long src_pfn;
    unsigned long dst_pfn;
    struct migrate_vma args;
    struct page *src_page = vmf->page;
    int ret;

    args.vma = vmf->vma;
    args.src = &src_pfn;
    args.dst = &dst_pfn;
    args.start = nv_page_fault_va(vmf);
    args.end = args.start + PAGE_SIZE;
    args.pgmap_owner = &g_uvm_global;
    args.flags = MIGRATE_VMA_SELECT_DEVICE_PRIVATE;
    args.fault_page = src_page;

    // We don't call migrate_vma_setup_locked() here because we don't
    // have a va_block and don't want to ignore invalidations.
    ret = migrate_vma_setup(&args);
    UVM_ASSERT(!ret);

    if (src_pfn & MIGRATE_PFN_MIGRATE) {
        struct page *dst_page;

        dst_page = alloc_page(GFP_HIGHUSER_MOVABLE);
        if (!dst_page) {
            status = NV_ERR_NO_MEMORY;
            goto out;
        }

        lock_page(dst_page);
        dst_pfn = migrate_pfn(page_to_pfn(dst_page));

        status = hmm_copy_devmem_page(dst_page, src_page);
        if (status != NV_OK) {
            unlock_page(dst_page);
            __free_page(dst_page);
            dst_pfn = 0;
        }
    }

out:
    if (status == NV_OK)
        migrate_vma_pages(&args);
    migrate_vma_finalize(&args);

    return status;
}

// The routines below are all for UVM-HMM tests.

NV_STATUS uvm_hmm_va_block_range_bounds(uvm_va_space_t *va_space,
                                        struct mm_struct *mm,
                                        NvU64 lookup_address,
                                        NvU64 *startp,
                                        NvU64 *endp,
                                        UVM_TEST_VA_RESIDENCY_INFO_PARAMS *params)
{
    struct vm_area_struct *vma;
    NvU64 start;
    NvU64 end;

    if (!uvm_hmm_is_enabled(va_space) || !mm)
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_space->lock);

    // The VMA might have changed while not holding mmap_lock so check it.
    vma = find_vma(mm, lookup_address);
    if (!uvm_hmm_vma_is_valid(vma, lookup_address, false))
        return NV_ERR_INVALID_ADDRESS;

    // Since managed VA ranges don't cover more than one VMA, return only the
    // intersecting range of the VA block and VMA.
    start = UVM_VA_BLOCK_ALIGN_DOWN(lookup_address);
    end = start + UVM_VA_BLOCK_SIZE - 1;
    if (start < vma->vm_start)
        start = vma->vm_start;
    if (end > vma->vm_end - 1)
        end = vma->vm_end - 1;

    *startp = start;
    *endp   = end;

    if (params) {
        uvm_processor_get_uuid(UVM_ID_CPU, &params->resident_on[0]);
        params->resident_physical_size[0] = PAGE_SIZE;
        params->resident_on_count = 1;

        uvm_processor_get_uuid(UVM_ID_CPU, &params->mapped_on[0]);
        params->mapping_type[0] = (vma->vm_flags & VM_WRITE) ?
                                  UVM_PROT_READ_WRITE_ATOMIC : UVM_PROT_READ_ONLY;
        params->page_size[0] = PAGE_SIZE;
        params->mapped_on_count = 1;

        uvm_processor_get_uuid(UVM_ID_CPU, &params->populated_on[0]);
        params->populated_on_count = 1;
    }

    return NV_OK;
}

NV_STATUS uvm_hmm_va_block_update_residency_info(uvm_va_block_t *va_block,
                                                 struct mm_struct *mm,
                                                 NvU64 lookup_address,
                                                 bool populate)
{
    uvm_va_space_t *va_space = va_block->hmm.va_space;
    struct vm_area_struct *vma;
    struct hmm_range range;
    uvm_va_block_region_t region;
    unsigned long pfn;
    NvU64 end;
    int ret;
    NV_STATUS status;

    if (!uvm_hmm_is_enabled(va_space) || !mm)
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_space->lock);

    // The VMA might have changed while not holding mmap_lock so check it.
    vma = find_vma(mm, lookup_address);
    if (!uvm_hmm_vma_is_valid(vma, lookup_address, false))
        return NV_ERR_INVALID_ADDRESS;

    end = lookup_address + PAGE_SIZE;
    region = uvm_va_block_region_from_start_end(va_block, lookup_address, end - 1);

    range.notifier = &va_block->hmm.notifier;
    range.start = lookup_address;
    range.end = end;
    range.hmm_pfns = &pfn;
    range.default_flags = 0;
    range.pfn_flags_mask = 0;
    range.dev_private_owner = &g_uvm_global;

    if (populate) {
        range.default_flags = HMM_PFN_REQ_FAULT;
        if (vma->vm_flags & VM_WRITE)
            range.default_flags |= HMM_PFN_REQ_WRITE;
    }

    uvm_hmm_migrate_begin_wait(va_block);

    while (true) {
        range.notifier_seq = mmu_interval_read_begin(range.notifier);
        ret = hmm_range_fault(&range);
        if (ret == -EBUSY)
            continue;
        if (ret) {
            uvm_hmm_migrate_finish(va_block);
            return errno_to_nv_status(ret);
        }

        uvm_mutex_lock(&va_block->lock);

        if (!mmu_interval_read_retry(range.notifier, range.notifier_seq))
            break;

        uvm_mutex_unlock(&va_block->lock);
    }

    // Update the va_block CPU state based on the snapshot.
    // Note that we have to adjust the pfns address since it will be indexed
    // by region.first.
    status = populate_region(va_block, &pfn - region.first, region, NULL);

    uvm_mutex_unlock(&va_block->lock);
    uvm_hmm_migrate_finish(va_block);

    return NV_OK;
}

NV_STATUS uvm_test_split_invalidate_delay(UVM_TEST_SPLIT_INVALIDATE_DELAY_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    atomic64_set(&va_space->test.split_invalidate_delay_us, params->delay_us);

    return NV_OK;
}

NV_STATUS uvm_hmm_va_range_info(uvm_va_space_t *va_space,
                                struct mm_struct *mm,
                                UVM_TEST_VA_RANGE_INFO_PARAMS *params)
{
    uvm_range_tree_node_t *tree_node;
    const uvm_va_policy_node_t *node;
    struct vm_area_struct *vma;
    uvm_va_block_t *va_block;

    if (!mm || !uvm_hmm_is_enabled(va_space))
        return NV_ERR_INVALID_ADDRESS;

    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_space->lock);

    params->type = UVM_TEST_VA_RANGE_TYPE_MANAGED;
    params->managed.subtype = UVM_TEST_RANGE_SUBTYPE_HMM;
    params->va_range_start = 0;
    params->va_range_end = ULONG_MAX;
    params->read_duplication = UVM_TEST_READ_DUPLICATION_UNSET;
    memset(&params->preferred_location, 0, sizeof(params->preferred_location));
    params->preferred_cpu_nid = NUMA_NO_NODE;
    params->accessed_by_count = 0;
    params->managed.vma_start = 0;
    params->managed.vma_end = 0;
    params->managed.is_zombie = NV_FALSE;
    params->managed.owned_by_calling_process = (mm == current->mm ? NV_TRUE : NV_FALSE);

    vma = find_vma(mm, params->lookup_address);
    if (!uvm_hmm_vma_is_valid(vma, params->lookup_address, false))
        return NV_ERR_INVALID_ADDRESS;

    params->va_range_start = vma->vm_start;
    params->va_range_end   = vma->vm_end - 1;
    params->managed.vma_start = vma->vm_start;
    params->managed.vma_end   = vma->vm_end - 1;

    uvm_mutex_lock(&va_space->hmm.blocks_lock);
    tree_node = uvm_range_tree_find(&va_space->hmm.blocks, params->lookup_address);
    if (!tree_node) {
        UVM_ASSERT(uvm_range_tree_find_hole_in(&va_space->hmm.blocks, params->lookup_address,
                                               &params->va_range_start, &params->va_range_end) == NV_OK);
        uvm_mutex_unlock(&va_space->hmm.blocks_lock);
        return NV_OK;
    }

    uvm_mutex_unlock(&va_space->hmm.blocks_lock);
    va_block = hmm_va_block_from_node(tree_node);
    uvm_mutex_lock(&va_block->lock);

    params->va_range_start = va_block->start;
    params->va_range_end   = va_block->end;

    node = uvm_va_policy_node_find(va_block, params->lookup_address);
    if (node) {
        uvm_processor_id_t processor_id;

        if (params->va_range_start < node->node.start)
            params->va_range_start = node->node.start;
        if (params->va_range_end > node->node.end)
            params->va_range_end = node->node.end;

        params->read_duplication = node->policy.read_duplication;

        if (!UVM_ID_IS_INVALID(node->policy.preferred_location)) {
            uvm_processor_get_uuid(node->policy.preferred_location, &params->preferred_location);
            params->preferred_cpu_nid = node->policy.preferred_nid;
        }

        for_each_id_in_mask(processor_id, &node->policy.accessed_by)
            uvm_processor_get_uuid(processor_id, &params->accessed_by[params->accessed_by_count++]);
    }
    else {
        uvm_range_tree_find_hole_in(&va_block->hmm.va_policy_tree, params->lookup_address,
                                    &params->va_range_start, &params->va_range_end);
    }

    uvm_mutex_unlock(&va_block->lock);

    return NV_OK;
}

// TODO: Bug 3660968: Remove this hack as soon as HMM migration is implemented
// for VMAs other than anonymous private memory.
bool uvm_hmm_must_use_sysmem(uvm_va_block_t *va_block,
                             struct vm_area_struct *vma)
{
    uvm_assert_mutex_locked(&va_block->lock);

    if (!uvm_va_block_is_hmm(va_block))
        return false;

    UVM_ASSERT(vma);
    UVM_ASSERT(va_block->hmm.va_space->va_space_mm.mm == vma->vm_mm);
    uvm_assert_mmap_lock_locked(vma->vm_mm);

    // migrate_vma_setup() can't migrate VM_SPECIAL so we have to force GPU
    // remote mapping.
    // TODO: Bug 3660968: add support for file-backed migrations.
    // TODO: Bug 3368756: add support for transparent huge page migrations.
    return !vma_is_anonymous(vma) ||
           (vma->vm_flags & VM_SPECIAL) ||
           vma_is_dax(vma) ||
           is_vm_hugetlb_page(vma);
}

#endif // UVM_IS_CONFIG_HMM()
