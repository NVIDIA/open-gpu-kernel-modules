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

#include "uvm_common.h"
#include "uvm_ioctl.h"
#include "uvm_linux.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_lock.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_va_block.h"
#include "uvm_tracker.h"
#include "uvm_api.h"
#include "uvm_channel.h"
#include "uvm_processors.h"
#include "uvm_push.h"
#include "uvm_hal.h"
#include "uvm_tools.h"
#include "uvm_migrate.h"
#include "uvm_migrate_pageable.h"
#include "uvm_va_space_mm.h"
#include "nv_speculation_barrier.h"

typedef enum
{
    UVM_MIGRATE_PASS_FIRST,
    UVM_MIGRATE_PASS_SECOND
} uvm_migrate_pass_t;

static int uvm_perf_migrate_cpu_preunmap_enable = 1;
module_param(uvm_perf_migrate_cpu_preunmap_enable, int, S_IRUGO);

#define UVM_PERF_MIGRATE_CPU_PREUNMAP_BLOCK_ORDER_DEFAULT 2
#define UVM_PERF_MIGRATE_CPU_PREUNMAP_BLOCK_ORDER_MAX     10
static unsigned uvm_perf_migrate_cpu_preunmap_block_order = UVM_PERF_MIGRATE_CPU_PREUNMAP_BLOCK_ORDER_DEFAULT;
module_param(uvm_perf_migrate_cpu_preunmap_block_order, uint, S_IRUGO);

// Global post-processed values of the module parameters
static bool g_uvm_perf_migrate_cpu_preunmap_enable __read_mostly;
static NvU64 g_uvm_perf_migrate_cpu_preunmap_size __read_mostly;

static bool is_migration_single_block(uvm_va_range_managed_t *first_managed_range, NvU64 base, NvU64 length)
{
    NvU64 end = base + length - 1;

    if (end > first_managed_range->va_range.node.end)
        return false;

    return uvm_va_range_block_index(first_managed_range, base) == uvm_va_range_block_index(first_managed_range, end);
}

static NV_STATUS block_migrate_map_mapped_pages(uvm_va_block_t *va_block,
                                                uvm_va_block_retry_t *va_block_retry,
                                                uvm_va_block_context_t *va_block_context,
                                                uvm_va_block_region_t region,
                                                uvm_processor_id_t dest_id)
{
    uvm_prot_t prot;
    uvm_page_index_t page_index;
    NV_STATUS status = NV_OK;
    const uvm_page_mask_t *pages_mapped_on_destination = uvm_va_block_map_mask_get(va_block, dest_id);

    for (prot = UVM_PROT_READ_ONLY; prot <= UVM_PROT_READ_WRITE_ATOMIC; ++prot)
        va_block_context->mask_by_prot[prot - 1].count = 0;

    // Only map those pages that are not already mapped on destination
    for_each_va_block_unset_page_in_region_mask(page_index, pages_mapped_on_destination, region) {
        prot = uvm_va_block_page_compute_highest_permission(va_block, va_block_context, dest_id, page_index);
        if (prot == UVM_PROT_NONE)
            continue;

        if (va_block_context->mask_by_prot[prot - 1].count++ == 0)
            uvm_page_mask_zero(&va_block_context->mask_by_prot[prot - 1].page_mask);

        uvm_page_mask_set(&va_block_context->mask_by_prot[prot - 1].page_mask, page_index);
    }

    for (prot = UVM_PROT_READ_ONLY; prot <= UVM_PROT_READ_WRITE_ATOMIC; ++prot) {
        if (va_block_context->mask_by_prot[prot - 1].count == 0)
            continue;

        // We pass UvmEventMapRemoteCauseInvalid since the destination processor
        // of a migration will never be mapped remotely
        status = uvm_va_block_map(va_block,
                                  va_block_context,
                                  dest_id,
                                  region,
                                  &va_block_context->mask_by_prot[prot - 1].page_mask,
                                  prot,
                                  UvmEventMapRemoteCauseInvalid,
                                  &va_block->tracker);
        if (status != NV_OK)
            break;

        // Whoever added the other mapping(s) should have already added
        // SetAccessedBy processors
    }

    return status;
}

static NV_STATUS block_migrate_map_unmapped_pages(uvm_va_block_t *va_block,
                                                  uvm_va_block_retry_t *va_block_retry,
                                                  uvm_va_block_context_t *va_block_context,
                                                  uvm_va_block_region_t region,
                                                  uvm_processor_id_t dest_id)

{
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status;
    uvm_prot_t prot = UVM_PROT_READ_WRITE_ATOMIC;

    // Get the mask of unmapped pages because it will change after the
    // first map operation
    uvm_va_block_unmapped_pages_get(va_block, region, &va_block_context->caller_page_mask);

    // Only map those pages that are not mapped anywhere else (likely due
    // to a first touch or a migration). We pass
    // UvmEventMapRemoteCauseInvalid since the destination processor of a
    // migration will never be mapped remotely.
    status = uvm_va_block_map(va_block,
                              va_block_context,
                              dest_id,
                              region,
                              &va_block_context->caller_page_mask,
                              UVM_PROT_READ_WRITE_ATOMIC,
                              UvmEventMapRemoteCauseInvalid,
                              &local_tracker);
    if (status != NV_OK)
        goto out;

    if (uvm_va_block_is_hmm(va_block) && UVM_ID_IS_CPU(dest_id)) {
        uvm_processor_id_t id;

        // Do not atomically map pages that are resident on the CPU. This is in
        // order to avoid breaking system-wide atomic operations on HMM. HMM's
        // implementation of system-side atomic operations involves restricting
        // mappings to one processor (CPU or a GPU) at a time. If we were to
        // grant a GPU a mapping to system memory, this gets into trouble
        // because, on the CPU side, Linux can silently upgrade PTE permissions
        // (move from read-only, to read-write, without any MMU notifiers
        // firing), thus breaking the model by allowing simultaneous read-write
        // access from two separate processors. To avoid that, don't remote map
        // such pages atomically, after migrating.
        // Also note that HMM sets CPU mapping for resident pages so the mask
        // of pages to be mapped needs to be recomputed without including the
        // CPU mapping.
        prot = UVM_PROT_READ_WRITE;
        uvm_page_mask_region_fill(&va_block_context->caller_page_mask, region);
        for_each_gpu_id_in_mask(id, &va_block->mapped) {
            uvm_page_mask_andnot(&va_block_context->caller_page_mask,
                                 &va_block_context->caller_page_mask,
                                 uvm_va_block_map_mask_get(va_block, id));
        }
    }

    // Add mappings for AccessedBy processors
    //
    // No mappings within this call will operate on dest_id, so we don't
    // need to acquire the map operation above.
    status = uvm_va_block_add_mappings_after_migration(va_block,
                                                       va_block_context,
                                                       dest_id,
                                                       dest_id,
                                                       region,
                                                       &va_block_context->caller_page_mask,
                                                       prot,
                                                       NULL);

out:
    tracker_status = uvm_tracker_add_tracker_safe(&va_block->tracker, &local_tracker);
    uvm_tracker_deinit(&local_tracker);
    return status == NV_OK ? tracker_status : status;
}

// Pages that are not mapped anywhere can be safely mapped with RWA permission.
// The rest of pages need to individually compute the maximum permission that
// does not require a revocation.
static NV_STATUS block_migrate_add_mappings(uvm_va_block_t *va_block,
                                            uvm_va_block_retry_t *va_block_retry,
                                            uvm_va_block_context_t *va_block_context,
                                            uvm_va_block_region_t region,
                                            uvm_processor_id_t dest_id)

{
    NV_STATUS status;

    status = block_migrate_map_unmapped_pages(va_block,
                                              va_block_retry,
                                              va_block_context,
                                              region,
                                              dest_id);
    if (status != NV_OK)
        return status;

    return block_migrate_map_mapped_pages(va_block,
                                          va_block_retry,
                                          va_block_context,
                                          region,
                                          dest_id);
}

NV_STATUS uvm_va_block_migrate_locked(uvm_va_block_t *va_block,
                                      uvm_va_block_retry_t *va_block_retry,
                                      uvm_service_block_context_t *service_context,
                                      uvm_va_block_region_t region,
                                      uvm_processor_id_t dest_id,
                                      uvm_migrate_mode_t mode,
                                      uvm_tracker_t *out_tracker)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_va_block_context_t *va_block_context = service_context->block_context;
    NV_STATUS status, tracker_status = NV_OK;

    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(uvm_hmm_check_context_vma_is_valid(va_block, va_block_context->hmm.vma, region));

    uvm_processor_mask_zero(&va_block_context->make_resident.all_involved_processors);

    if (uvm_va_block_is_hmm(va_block)) {
        status = uvm_hmm_va_block_migrate_locked(va_block,
                                                 va_block_retry,
                                                 service_context,
                                                 dest_id,
                                                 region,
                                                 UVM_MAKE_RESIDENT_CAUSE_API_MIGRATE);
    }
    else {
        uvm_va_policy_t *policy = &va_block->managed_range->policy;

        if (uvm_va_policy_is_read_duplicate(policy, va_space)) {
            status = uvm_va_block_make_resident_read_duplicate(va_block,
                                                               va_block_retry,
                                                               va_block_context,
                                                               dest_id,
                                                               region,
                                                               NULL,
                                                               NULL,
                                                               UVM_MAKE_RESIDENT_CAUSE_API_MIGRATE);
        }
        else {
            status = uvm_va_block_make_resident(va_block,
                                                va_block_retry,
                                                va_block_context,
                                                dest_id,
                                                region,
                                                NULL,
                                                NULL,
                                                UVM_MAKE_RESIDENT_CAUSE_API_MIGRATE);
        }
    }

    if (status == NV_OK && mode == UVM_MIGRATE_MODE_MAKE_RESIDENT_AND_MAP) {
        // block_migrate_add_mappings will acquire the work from the above
        // make_resident call and update the VA block tracker.
        status = block_migrate_add_mappings(va_block, va_block_retry, va_block_context, region, dest_id);
    }

    if (out_tracker)
        tracker_status = uvm_tracker_add_tracker_safe(out_tracker, &va_block->tracker);

    uvm_processor_mask_or(&service_context->gpus_to_check_for_nvlink_errors,
                          &service_context->gpus_to_check_for_nvlink_errors,
                          &va_block_context->make_resident.all_involved_processors);

    return status == NV_OK ? tracker_status : status;
}

// Unmapping CPU pages on P9 systems is very costly, to the point that it
// becomes the bottleneck of UvmMigrate. We have measured up to 3x lower BW for
// migrations that need to remove CPU mappings compared to migrations that only
// create CPU mappings. The overhead can be fully attributed to the TLB
// shootdown. When a CPU page is unmapped, it needs to (1) invalidate any copy
// in the P9 cores, and (2) if ATS is enabled, issue ATSD messages over NVLINK
// to remove the corresponding entries in the GPUs' TLBs. ATSDs are not even
// required when migration managed memory since UVM ensures that there are no
// ATS entries cached in the GPU TLBs for the managed VA ranges. However, we
// don't have a way to skip them as of today.
//
// In order to minimize the overhead of CPU unmaps during UvmMigrate we try to
// call unmap_mapping_range on VA regions larger than the VA block granularity
// before the actual migration so that TLB invalidations are batched better by
// the OS. This also has an impact in the number of ATSD messages issued. This
// is because the NPU code uses MMU notifiers in order to get a callback
// (invalidate_range) when a TLB invalidation is required. Fortunately, this
// callback is not called if there is nothing to be invalidated. Therefore, if
// we issue a large unmap, subsequent unmaps within that region will not invoke
// the callback.
//
// However, due to (1), even issuing a single invalidate for the whole migrated
// range introduces a noticeable overhead (20-30%) on systems with 3xNVLINK2.
// This is only expected to get worse if CPU-GPU interconnects' BW keeps
// increasing.
//
// Thus, VA range migrations are split into groups of contiguous VA blocks, and
// trigger a single pre-unmap of the group of VA blocks in the Linux kernel
// before the VA blocks' migration starts. This way, we trigger larger (more
// efficient) TLB invalidations than when we do it one VA block a time, while
// still being able to pipeline the migration, which allows to hide most of the
// costs of (1).
//
// However, there are some cases in which the CPU has mappings to the pages
// being migrated but they don't need to be removed (which can introduce
// unnecessary CPU faults later on). Therefore, we skip the pre-unmap step
// under the following conditions:
// - Pages mapped by the CPU that are *already* in the destination.
// - Pages mapped by the CPU that are *not* in the destination but
// read-duplication is enabled in the VA range.

// This function checks if the pre-unmap optimization is required given the
// system capabilities and the destination of the migration. This is to skip
// any subsequent checks required by the optimization, which can be costly.
//
// The current logic checks that:
// - We are in the first pass of the migration (see the explanation of the
// two-pass strategy in uvm_migrate).
// - The CPU has an NVLINK or C2C interconnect to the GPUs. Otherwise, we don't
// need this optimization since we are already limited by PCIe BW.
// - If the migration spans several VA blocks, otherwise skip the preunmap to
// avoid the overhead.
static bool migration_should_do_cpu_preunmap(uvm_va_space_t *va_space,
                                             uvm_migrate_pass_t pass,
                                             bool is_single_block)

{
    if (!g_uvm_perf_migrate_cpu_preunmap_enable)
        return false;

    if (pass != UVM_MIGRATE_PASS_FIRST || is_single_block)
        return false;

    if (uvm_processor_mask_get_gpu_count(&va_space->has_fast_link[UVM_ID_CPU_VALUE]) == 0)
        return false;

    return true;
}

// This function determines if the VA range properties avoid the need to remove
// CPU mappings on UvmMigrate. Currently, it only checks whether
// read-duplication is enabled in the VA range. This is because, when migrating
// read-duplicated VA blocks, the source processor doesn't need to be unmapped
// (though it may need write access revoked).
static bool va_range_should_do_cpu_preunmap(const uvm_va_policy_t *policy,
                                            uvm_va_space_t *va_space)
{
    return !uvm_va_policy_is_read_duplicate(policy, va_space);
}

// Function that determines if the VA block to be migrated contains pages with
// CPU mappings that don't need to be removed (see the comment above). In that
// case false is returned. Otherwise it returns true, and stores in the
// variable pointed by num_unmap_pages the number of pages that do need to
// remove their CPU mappings.
static bool va_block_should_do_cpu_preunmap(uvm_va_block_t *va_block,
                                            uvm_va_block_context_t *va_block_context,
                                            NvU64 start,
                                            NvU64 end,
                                            uvm_processor_id_t dest_id,
                                            NvU32 *num_unmap_pages)
{
    const uvm_page_mask_t *mapped_pages_cpu;
    NvU32 num_cpu_unchanged_pages = 0;
    uvm_va_block_region_t region;

    *num_unmap_pages = 0;

    if (!va_block)
        return true;

    region = uvm_va_block_region_from_start_end(va_block, max(start, va_block->start), min(end, va_block->end));

    uvm_mutex_lock(&va_block->lock);

    mapped_pages_cpu = uvm_va_block_map_mask_get(va_block, UVM_ID_CPU);
    if (uvm_processor_mask_test(&va_block->resident, dest_id)) {
        const uvm_page_mask_t *resident_pages_dest = uvm_va_block_resident_mask_get(va_block, dest_id, NUMA_NO_NODE);
        uvm_page_mask_t *do_not_unmap_pages = &va_block_context->scratch_page_mask;

        // TODO: Bug 1877578
        //
        // We assume that if pages are mapped on the CPU and not resident on
        // the destination, the pages will change residency so the CPU must be
        // unmapped. If we implement automatic read-duplication heuristics in
        // the future, we'll also need to check if the pages are being
        // read-duplicated.
        uvm_page_mask_and(do_not_unmap_pages, mapped_pages_cpu, resident_pages_dest);

        num_cpu_unchanged_pages = uvm_page_mask_region_weight(do_not_unmap_pages, region);
    }

    *num_unmap_pages = uvm_page_mask_region_weight(mapped_pages_cpu, region) - num_cpu_unchanged_pages;

    uvm_mutex_unlock(&va_block->lock);

    return num_cpu_unchanged_pages == 0;
}

static void preunmap_multi_block(uvm_va_range_managed_t *managed_range,
                                 uvm_va_block_context_t *va_block_context,
                                 NvU64 start,
                                 NvU64 end,
                                 uvm_processor_id_t dest_id)
{
    size_t i;
    const size_t first_block_index = uvm_va_range_block_index(managed_range, start);
    const size_t last_block_index = uvm_va_range_block_index(managed_range, end);
    NvU32 num_unmap_pages = 0;

    UVM_ASSERT(start >= managed_range->va_range.node.start);
    UVM_ASSERT(end  <= managed_range->va_range.node.end);
    uvm_assert_rwsem_locked(&managed_range->va_range.va_space->lock);

    UVM_ASSERT(uvm_range_group_all_migratable(managed_range->va_range.va_space, start, end));

    for (i = first_block_index; i <= last_block_index; i++) {
        NvU32 num_block_unmap_pages;

        if (!va_block_should_do_cpu_preunmap(uvm_va_range_block(managed_range, i),
                                             va_block_context,
                                             start,
                                             end,
                                             dest_id,
                                             &num_block_unmap_pages)) {
            return;
        }

        num_unmap_pages += num_block_unmap_pages;
    }

    if (num_unmap_pages > 0)
        unmap_mapping_range(managed_range->va_range.va_space->mapping, start, end - start + 1, 1);
}

static NV_STATUS uvm_va_range_migrate_multi_block(uvm_va_range_managed_t *managed_range,
                                                  uvm_service_block_context_t *service_context,
                                                  NvU64 start,
                                                  NvU64 end,
                                                  uvm_processor_id_t dest_id,
                                                  uvm_migrate_mode_t mode,
                                                  uvm_tracker_t *out_tracker)
{
    size_t i;
    const size_t first_block_index = uvm_va_range_block_index(managed_range, start);
    const size_t last_block_index = uvm_va_range_block_index(managed_range, end);

    UVM_ASSERT(start >= managed_range->va_range.node.start);
    UVM_ASSERT(end  <= managed_range->va_range.node.end);
    uvm_assert_rwsem_locked(&managed_range->va_range.va_space->lock);

    UVM_ASSERT(uvm_range_group_all_migratable(managed_range->va_range.va_space, start, end));

    // Iterate over blocks, populating them if necessary
    for (i = first_block_index; i <= last_block_index; i++) {
        uvm_va_block_retry_t va_block_retry;
        uvm_va_block_region_t region;
        uvm_va_block_t *va_block;
        NV_STATUS status = uvm_va_range_block_create(managed_range, i, &va_block);

        if (status != NV_OK)
            return status;

        region = uvm_va_block_region_from_start_end(va_block,
                                                    max(start, va_block->start),
                                                    min(end, va_block->end));

        status = UVM_VA_BLOCK_LOCK_RETRY(va_block,
                                         &va_block_retry,
                                         uvm_va_block_migrate_locked(va_block,
                                                                     &va_block_retry,
                                                                     service_context,
                                                                     region,
                                                                     dest_id,
                                                                     mode,
                                                                     out_tracker));
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static NV_STATUS uvm_va_range_migrate(uvm_va_range_managed_t *managed_range,
                                      uvm_service_block_context_t *service_context,
                                      NvU64 start,
                                      NvU64 end,
                                      uvm_processor_id_t dest_id,
                                      uvm_migrate_mode_t mode,
                                      bool should_do_cpu_preunmap,
                                      uvm_tracker_t *out_tracker)
{
    NvU64 preunmap_range_start = start;
    uvm_va_policy_t *policy = &managed_range->policy;

    should_do_cpu_preunmap = should_do_cpu_preunmap &&
                             va_range_should_do_cpu_preunmap(policy, managed_range->va_range.va_space);

    // Divide migrations into groups of contiguous VA blocks. This is to trigger
    // CPU unmaps for that region before the migration starts.
    while (preunmap_range_start < end) {
        NV_STATUS status;
        NvU64 preunmap_range_end;

        if (should_do_cpu_preunmap) {
            preunmap_range_end = UVM_ALIGN_UP(preunmap_range_start + 1, g_uvm_perf_migrate_cpu_preunmap_size);
            preunmap_range_end = min(preunmap_range_end - 1, end);

            preunmap_multi_block(managed_range,
                                 service_context->block_context,
                                 preunmap_range_start,
                                 preunmap_range_end,
                                 dest_id);
        }
        else {
            preunmap_range_end = end;
        }

        status = uvm_va_range_migrate_multi_block(managed_range,
                                                  service_context,
                                                  preunmap_range_start,
                                                  preunmap_range_end,
                                                  dest_id,
                                                  mode,
                                                  out_tracker);
        if (status != NV_OK)
            return status;

        preunmap_range_start = preunmap_range_end + 1;
    }

    return NV_OK;
}

static NV_STATUS uvm_migrate_ranges(uvm_va_space_t *va_space,
                                    uvm_service_block_context_t *service_context,
                                    uvm_va_range_managed_t *first_managed_range,
                                    NvU64 base,
                                    NvU64 length,
                                    uvm_processor_id_t dest_id,
                                    uvm_migrate_mode_t mode,
                                    bool should_do_cpu_preunmap,
                                    uvm_tracker_t *out_tracker)
{
    uvm_va_range_managed_t *managed_range, *managed_range_last;
    NvU64 end = base + length - 1;
    NV_STATUS status = NV_OK;
    bool skipped_migrate = false;

    if (!first_managed_range) {
        // For HMM, we iterate over va_blocks since there is no managed_range.
        return uvm_hmm_migrate_ranges(va_space, service_context, base, length, dest_id, mode, out_tracker);
    }

    UVM_ASSERT(first_managed_range == uvm_va_space_iter_managed_first(va_space, base, base));

    managed_range_last = NULL;
    uvm_for_each_va_range_managed_in_contig_from(managed_range, first_managed_range, end) {
        uvm_range_group_range_iter_t iter;
        uvm_va_policy_t *policy = &managed_range->policy;

        managed_range_last = managed_range;

        // For UVM-Lite GPUs, the CUDA driver may suballocate a single
        // managed_range into many range groups. For this reason, we iterate
        // over each managed_range first then through the range groups within.
        uvm_range_group_for_each_migratability_in(&iter,
                                                  va_space,
                                                  max(base, managed_range->va_range.node.start),
                                                  min(end, managed_range->va_range.node.end)) {
            // Skip non-migratable ranges
            if (!iter.migratable) {
                // Only return NV_WARN_MORE_PROCESSING_REQUIRED if the pages aren't
                // already resident at dest_id.
                if (!uvm_va_policy_preferred_location_equal(policy,
                                                            dest_id,
                                                            service_context->block_context->make_resident.dest_nid))
                    skipped_migrate = true;
            }
            else if (uvm_processor_mask_test(&managed_range->va_range.uvm_lite_gpus, dest_id) &&
                     !uvm_va_policy_preferred_location_equal(policy, dest_id, NUMA_NO_NODE)) {
                // Don't migrate to a non-faultable GPU that is in UVM-Lite mode,
                // unless it's the preferred location
                status = NV_ERR_INVALID_DEVICE;
                break;
            }
            else {
                status = uvm_va_range_migrate(managed_range,
                                              service_context,
                                              iter.start,
                                              iter.end,
                                              dest_id,
                                              mode,
                                              should_do_cpu_preunmap,
                                              out_tracker);
                if (status != NV_OK)
                    break;
            }
        }
    }

    if (status != NV_OK)
        return status;

    // Check that we were able to iterate over the entire range without any gaps
    if (!managed_range_last || managed_range_last->va_range.node.end < end)
        return NV_ERR_INVALID_ADDRESS;

    if (skipped_migrate)
        return NV_WARN_MORE_PROCESSING_REQUIRED;

    return NV_OK;
}

static NV_STATUS uvm_migrate(uvm_va_space_t *va_space,
                             struct mm_struct *mm,
                             NvU64 base,
                             NvU64 length,
                             uvm_processor_id_t dest_id,
                             int dest_nid,
                             NvU32 migrate_flags,
                             uvm_va_range_managed_t *first_managed_range,
                             uvm_tracker_t *out_tracker,
                             uvm_processor_mask_t *gpus_to_check_for_nvlink_errors)
{
    NV_STATUS status = NV_OK;
    uvm_service_block_context_t *service_context;
    bool do_mappings;
    bool do_two_passes;
    bool is_single_block;
    bool should_do_cpu_preunmap;

    uvm_assert_rwsem_locked(&va_space->lock);

    // If the GPU has its memory disabled, just skip the migration and let
    // faults take care of things.
    if (!uvm_processor_has_memory(dest_id))
        return NV_OK;

    if (mm)
        uvm_assert_mmap_lock_locked(mm);
    else if (!first_managed_range)
        return NV_ERR_INVALID_ADDRESS;

    service_context = uvm_service_block_context_alloc(mm);
    if (!service_context)
        return NV_ERR_NO_MEMORY;

    service_context->block_context->make_resident.dest_nid = dest_nid;

    uvm_processor_mask_zero(&service_context->gpus_to_check_for_nvlink_errors);

    // We perform two passes (unless the migration only covers a single VA
    // block or UVM_MIGRATE_FLAG_SKIP_CPU_MAP is passed). This helps in the
    // following scenarios:
    //
    // - Migrations that add CPU mappings, since they are synchronous operations
    // that delay the migration of the next VA blocks.
    // - Concurrent migrations. This is due to our current channel selection
    // logic that doesn't prevent false dependencies between independent
    // operations. For example, removal of mappings for outgoing transfers are
    // delayed by the mappings added by incoming transfers.
    // TODO: Bug 1764953: Re-evaluate the two-pass logic when channel selection
    // is overhauled.
    //
    // The two passes are as follows:
    //
    // 1- Transfer all VA blocks (do not add mappings)
    // 2- Go block by block reexecuting the transfer (in case someone moved it
    // since the first pass), and adding the mappings.
    //
    // For HMM (!first_managed_range), we always do a single pass.
    is_single_block = !first_managed_range || is_migration_single_block(first_managed_range, base, length);
    do_mappings = UVM_ID_IS_GPU(dest_id) || !(migrate_flags & UVM_MIGRATE_FLAG_SKIP_CPU_MAP);
    do_two_passes = do_mappings && !is_single_block;

    if (do_two_passes) {
        should_do_cpu_preunmap = migration_should_do_cpu_preunmap(va_space, UVM_MIGRATE_PASS_FIRST, is_single_block);

        status = uvm_migrate_ranges(va_space,
                                    service_context,
                                    first_managed_range,
                                    base,
                                    length,
                                    dest_id,
                                    UVM_MIGRATE_MODE_MAKE_RESIDENT,
                                    should_do_cpu_preunmap,
                                    out_tracker);
    }

    if (status == NV_OK) {
        uvm_migrate_mode_t mode = do_mappings? UVM_MIGRATE_MODE_MAKE_RESIDENT_AND_MAP:
                                               UVM_MIGRATE_MODE_MAKE_RESIDENT;
        uvm_migrate_pass_t pass = do_two_passes? UVM_MIGRATE_PASS_SECOND:
                                                 UVM_MIGRATE_PASS_FIRST;
        should_do_cpu_preunmap = migration_should_do_cpu_preunmap(va_space, pass, is_single_block);

        status = uvm_migrate_ranges(va_space,
                                    service_context,
                                    first_managed_range,
                                    base,
                                    length,
                                    dest_id,
                                    mode,
                                    should_do_cpu_preunmap,
                                    out_tracker);
    }

    uvm_processor_mask_copy(gpus_to_check_for_nvlink_errors, &service_context->gpus_to_check_for_nvlink_errors);
    uvm_service_block_context_free(service_context);

    return status;
}

static NV_STATUS semaphore_release_from_gpu(uvm_gpu_t *gpu,
                                            uvm_va_range_semaphore_pool_t *semaphore_va_range,
                                            NvU64 semaphore_user_addr,
                                            NvU32 semaphore_payload,
                                            uvm_tracker_t *release_after_tracker)
{
    NV_STATUS status;
    uvm_push_t push;
    uvm_channel_type_t channel_type;
    NvU64 semaphore_gpu_va;
    NvU64 semaphore_offset;

    UVM_ASSERT(uvm_mem_mapped_on_gpu_kernel(semaphore_va_range->mem, gpu));

    semaphore_offset = semaphore_user_addr - (NvU64)(uintptr_t)semaphore_va_range->mem->user->addr;
    semaphore_gpu_va = uvm_mem_get_gpu_va_kernel(semaphore_va_range->mem, gpu) + semaphore_offset;

    // Outside of SR-IOV heavy, using UVM_CHANNEL_TYPE_MEMOPS is optimal from a
    // performance standpoint because if the migration is targeting a GPU, it is
    // likely that the channel used for the GPU page table update (pushed to
    // UVM_CHANNEL_TYPE_MEMOPS) will also be used for the release. The
    // inter-channel dependency avoided by using a single channel can add a
    // significant overhead to the enclosing migration.
    //
    // In SR-IOV heavy, the user semaphore release is functionally forbidden
    // from being pushed to a UVM_CHANNEL_TYPE_MEMOPS channel, because it is not
    // a page tree operation.
    if (uvm_parent_gpu_is_virt_mode_sriov_heavy(gpu->parent))
        channel_type = UVM_CHANNEL_TYPE_GPU_INTERNAL;
    else
        channel_type = UVM_CHANNEL_TYPE_MEMOPS;

    status = uvm_push_begin_acquire(gpu->channel_manager,
                                    channel_type,
                                    release_after_tracker,
                                    &push,
                                    "Pushing semaphore release (*0x%llx = %u)",
                                    semaphore_user_addr,
                                    semaphore_payload);
    if (status != NV_OK)
        return status;

    gpu->parent->ce_hal->semaphore_release(&push, semaphore_gpu_va, semaphore_payload);
    uvm_push_end(&push);

    uvm_mutex_lock(&semaphore_va_range->tracker_lock);
    status = uvm_tracker_add_push_safe(&semaphore_va_range->tracker, &push);
    uvm_tracker_remove_completed(&semaphore_va_range->tracker);
    uvm_mutex_unlock(&semaphore_va_range->tracker_lock);

    return status;
}

static void semaphore_release_from_cpu(uvm_mem_t *semaphore_mem, NvU64 semaphore_user_addr, NvU32 semaphore_payload)
{
    char *semaphore_cpu_va;
    NvU64 semaphore_offset;

    UVM_ASSERT(uvm_mem_mapped_on_cpu_kernel(semaphore_mem));

    semaphore_offset = semaphore_user_addr - (NvU64)(uintptr_t)semaphore_mem->user->addr;

    // Prevent processor speculation prior to accessing user-mapped memory to
    // avoid leaking information from side-channel attacks. Under speculation, a
    // valid VA range which does not contain this semaphore could be used by the
    // caller. It's unclear but likely that the user might be able to control
    // the data at that address. Auditing all potential ways that could happen
    // is difficult and error-prone, so to be on the safe side we'll just always
    // block speculation.
    nv_speculation_barrier();

    semaphore_cpu_va = (char *) uvm_mem_get_cpu_addr_kernel(semaphore_mem) + semaphore_offset;

    WRITE_ONCE(*(NvU32 *)semaphore_cpu_va, semaphore_payload);
}

static NV_STATUS semaphore_release(NvU64 semaphore_address,
                                   NvU32 semaphore_payload,
                                   uvm_va_range_semaphore_pool_t *semaphore_pool,
                                   uvm_gpu_t *dest_gpu,
                                   uvm_tracker_t *tracker_ptr)
{
    uvm_gpu_t *gpu;
    uvm_gpu_t *gpu_owner = semaphore_pool->owner;

    // If there is a GPU owner, release the semaphore from it.
    if (gpu_owner != NULL)
        return semaphore_release_from_gpu(gpu_owner, semaphore_pool, semaphore_address, semaphore_payload, tracker_ptr);

    // Attempt eager release from CPU if the tracker is already completed.
    if (uvm_tracker_is_completed(tracker_ptr)) {
        semaphore_release_from_cpu(semaphore_pool->mem, semaphore_address, semaphore_payload);
        return NV_OK;
    }

    if (dest_gpu == NULL) {
        // The destination is the CPU, but we didn't do a CPU release above
        // because the previous work is not complete. This situation arises when
        // accessed_by mappings are being set up asynchronously, or the
        // test-only flag UVM_MIGRATE_FLAG_SKIP_CPU_MAP is used. So there should
        // be a registered GPU, since all CPU work is synchronous, and the
        // semaphore must be mapped on that GPU.
        //
        // Note that the GPU selected for the release may not be the same device
        // that prevented the tracker from being complete.
        gpu = uvm_processor_mask_find_first_gpu(&semaphore_pool->mem->kernel.mapped_on);

        UVM_ASSERT(gpu != NULL);
    }
    else {
        gpu = dest_gpu;
    }

    return semaphore_release_from_gpu(gpu, semaphore_pool, semaphore_address, semaphore_payload, tracker_ptr);
}

NV_STATUS uvm_migrate_init(void)
{
    NV_STATUS status = uvm_migrate_pageable_init();
    if (status != NV_OK)
        return status;

    g_uvm_perf_migrate_cpu_preunmap_enable = uvm_perf_migrate_cpu_preunmap_enable != 0;

    BUILD_BUG_ON((UVM_VA_BLOCK_SIZE) & (UVM_VA_BLOCK_SIZE - 1));

    if (g_uvm_perf_migrate_cpu_preunmap_enable) {
        if (uvm_perf_migrate_cpu_preunmap_block_order <= UVM_PERF_MIGRATE_CPU_PREUNMAP_BLOCK_ORDER_MAX) {
            g_uvm_perf_migrate_cpu_preunmap_size = UVM_VA_BLOCK_SIZE << uvm_perf_migrate_cpu_preunmap_block_order;
        }
        else {
            g_uvm_perf_migrate_cpu_preunmap_size = UVM_VA_BLOCK_SIZE << UVM_PERF_MIGRATE_CPU_PREUNMAP_BLOCK_ORDER_DEFAULT;

            UVM_INFO_PRINT("Invalid value %u for uvm_perf_migrate_cpu_preunmap_block_order. Using %u instead\n",
                           uvm_perf_migrate_cpu_preunmap_block_order,
                           UVM_PERF_MIGRATE_CPU_PREUNMAP_BLOCK_ORDER_DEFAULT);
        }
    }

    return NV_OK;
}

void uvm_migrate_exit(void)
{
    uvm_migrate_pageable_exit();
}

NV_STATUS uvm_api_migrate(UVM_MIGRATE_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_tracker_t tracker = UVM_TRACKER_INIT();
    uvm_tracker_t *tracker_ptr = NULL;
    uvm_gpu_t *dest_gpu = NULL;
    uvm_va_range_semaphore_pool_t *sema_va_range = NULL;
    struct mm_struct *mm;
    NV_STATUS status = NV_OK;
    bool flush_events = false;
    const bool synchronous = !(params->flags & UVM_MIGRATE_FLAG_ASYNC);
    int cpu_numa_node = (int)params->cpuNumaNode;
    uvm_processor_mask_t *gpus_to_check_for_nvlink_errors = NULL;

    // We temporarily allow 0 length in the IOCTL parameters as a signal to
    // only release the semaphore. This is because user-space is in charge of
    // migrating pageable memory in some cases.
    //
    // TODO: Bug 2419180: do not allow 0 length migrations when we fully switch
    // to migrate_vma for all types of vmas.
    if (params->length > 0 || synchronous || params->semaphoreAddress == 0) {
        if (uvm_api_range_invalid(params->base, params->length))
            return NV_ERR_INVALID_ADDRESS;
    }

    if (params->flags & ~UVM_MIGRATE_FLAGS_ALL)
        return NV_ERR_INVALID_ARGUMENT;

    if ((params->flags & UVM_MIGRATE_FLAGS_TEST_ALL) && !uvm_enable_builtin_tests) {
        UVM_INFO_PRINT("Test flag set for UVM_MIGRATE. Did you mean to insmod with uvm_enable_builtin_tests=1?\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    gpus_to_check_for_nvlink_errors = uvm_processor_mask_cache_alloc();
    if (!gpus_to_check_for_nvlink_errors)
        return NV_ERR_NO_MEMORY;

    uvm_processor_mask_zero(gpus_to_check_for_nvlink_errors);

    // mmap_lock will be needed if we have to create CPU mappings
    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_read(va_space);

    if (synchronous) {
        if (params->semaphoreAddress != 0) {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
    }
    else {
        if (params->semaphoreAddress == 0) {
            if (params->semaphorePayload != 0) {
                status = NV_ERR_INVALID_ARGUMENT;
                goto done;
            }
        }
        else {
            sema_va_range = uvm_va_range_semaphore_pool_find(va_space, params->semaphoreAddress);
            if (!IS_ALIGNED(params->semaphoreAddress, sizeof(params->semaphorePayload)) ||
                    !sema_va_range) {
                status = NV_ERR_INVALID_ADDRESS;
                goto done;
            }
        }
    }

    if (!uvm_uuid_is_cpu(&params->destinationUuid)) {
        if (params->flags & UVM_MIGRATE_FLAG_NO_GPU_VA_SPACE)
            dest_gpu = uvm_va_space_get_gpu_by_uuid(va_space, &params->destinationUuid);
        else
            dest_gpu = uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(va_space, &params->destinationUuid);

        if (!dest_gpu) {
            status = NV_ERR_INVALID_DEVICE;
            goto done;
        }

        if (params->length > 0 && !uvm_gpu_can_address(dest_gpu, params->base, params->length)) {
            status = NV_ERR_OUT_OF_RANGE;
            goto done;
        }
    }
    else {
        // If cpu_numa_node is not -1, we only check that it is a valid node in
        // the system, it has memory, and it doesn't correspond to a GPU node.
        //
        // For pageable memory, this is fine because alloc_pages_node will clamp
        // the allocation to cpuset_current_mems_allowed when uvm_migrate
        //_pageable is called from process context (uvm_migrate) when dst_id is
        // CPU. UVM bottom half calls uvm_migrate_pageable with CPU dst_id only
        // when the VMA memory policy is set to dst_node_id and dst_node_id is
        // not NUMA_NO_NODE.
        if (cpu_numa_node != -1 &&
            (!nv_numa_node_has_memory(cpu_numa_node) ||
             !node_isset(cpu_numa_node, node_possible_map) ||
             uvm_va_space_find_gpu_with_memory_node_id(va_space, cpu_numa_node))) {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
    }

    UVM_ASSERT(status == NV_OK);

    // If we're synchronous or if we need to release a semaphore, use a tracker.
    if (synchronous || params->semaphoreAddress)
        tracker_ptr = &tracker;

    if (params->length > 0) {
        uvm_api_range_type_t type;
        uvm_processor_id_t dest_id = dest_gpu ? dest_gpu->id : UVM_ID_CPU;

        // Migration to an integrated GPU is equivalent to migration to that
        // GPUs nearest NUMA node.
        if (dest_gpu && dest_gpu->parent->is_integrated_gpu) {
            dest_id = UVM_ID_CPU;
            cpu_numa_node = dest_gpu->parent->closest_cpu_numa_node;
        }

        type = uvm_api_range_type_check(va_space, mm, params->base, params->length);
        if (type == UVM_API_RANGE_TYPE_INVALID) {
            status = NV_ERR_INVALID_ADDRESS;
            goto done;
        }

        if (type == UVM_API_RANGE_TYPE_ATS) {
            uvm_migrate_args_t uvm_migrate_args =
            {
                .va_space                           = va_space,
                .mm                                 = mm,
                .start                              = params->base,
                .length                             = params->length,
                .dst_id                             = dest_id,
                .dst_node_id                        = cpu_numa_node,
                .populate_permissions               = UVM_POPULATE_PERMISSIONS_INHERIT,
                .populate_flags                     = UVM_POPULATE_PAGEABLE_FLAG_SKIP_PROT_CHECK,
                .skip_mapped                        = false,
                .populate_on_cpu_alloc_failures     = false,
                .populate_on_migrate_vma_failures   = true,
                .user_space_start                   = &params->userSpaceStart,
                .user_space_length                  = &params->userSpaceLength,
                .gpus_to_check_for_nvlink_errors    = gpus_to_check_for_nvlink_errors,
                .fail_on_unresolved_sto_errors      = false,
            };

            status = uvm_migrate_pageable(&uvm_migrate_args);
        }
        else {
            status = uvm_migrate(va_space,
                                 mm,
                                 params->base,
                                 params->length,
                                 dest_id,
                                 (UVM_ID_IS_CPU(dest_id) ? cpu_numa_node : NUMA_NO_NODE),
                                 params->flags,
                                 uvm_va_space_iter_managed_first(va_space, params->base, params->base),
                                 tracker_ptr,
                                 gpus_to_check_for_nvlink_errors);
        }
    }

done:
    uvm_global_gpu_retain(gpus_to_check_for_nvlink_errors);

    // We only need to hold mmap_lock to create new CPU mappings, so drop it if
    // we need to wait for the tracker to finish.
    //
    // TODO: Bug 1766650: For large migrations with destination CPU, try
    //       benchmarks to see if a two-pass approach would be faster (first
    //       pass pushes all GPU work asynchronously, second pass updates CPU
    //       mappings synchronously).
    if (mm)
        uvm_up_read_mmap_lock_out_of_order(mm);

    if (tracker_ptr) {
        // If requested, release semaphore
        if (params->semaphoreAddress && (status == NV_OK)) {
            status = semaphore_release(params->semaphoreAddress,
                                       params->semaphorePayload,
                                       sema_va_range,
                                       dest_gpu,
                                       tracker_ptr);
        }

        // Wait on the tracker if we are synchronous or there was an error. The
        // VA space lock must be held to prevent GPUs from being unregistered.
        if (synchronous || (status != NV_OK)) {
            NV_STATUS tracker_status = uvm_tracker_wait(tracker_ptr);

            // Only clobber status if we didn't hit an earlier error
            if (status == NV_OK)
                status = tracker_status;

            flush_events = true;
        }

        uvm_tracker_deinit(tracker_ptr);
    }

    uvm_va_space_up_read(va_space);
    uvm_va_space_mm_or_current_release(va_space, mm);

    // Check for STO errors in case there was no other error until now.
    if (status == NV_OK && !uvm_processor_mask_empty(gpus_to_check_for_nvlink_errors))
        status = uvm_global_gpu_check_nvlink_error(gpus_to_check_for_nvlink_errors);

    uvm_global_gpu_release(gpus_to_check_for_nvlink_errors);
    uvm_processor_mask_cache_free(gpus_to_check_for_nvlink_errors);

    // If the migration is known to be complete, eagerly dispatch the migration
    // events, instead of processing them on a later event flush. Note that an
    // asynchronous migration could be complete by now, but the flush would not
    // be triggered.
    if (flush_events)
        uvm_tools_flush_events();

    return status;
}

NV_STATUS uvm_api_migrate_range_group(UVM_MIGRATE_RANGE_GROUP_PARAMS *params, struct file *filp)
{
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status = NV_OK;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    struct mm_struct *mm;
    uvm_range_group_t *range_group;
    uvm_range_group_range_t *rgr;
    uvm_processor_id_t dest_id;
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    NvU32 migrate_flags = 0;
    uvm_gpu_t *gpu = NULL;
    uvm_processor_mask_t *gpus_to_check_for_nvlink_errors = NULL;

    gpus_to_check_for_nvlink_errors = uvm_processor_mask_cache_alloc();
    if (!gpus_to_check_for_nvlink_errors)
        return NV_ERR_NO_MEMORY;
 
    uvm_processor_mask_zero(gpus_to_check_for_nvlink_errors);

    // mmap_lock will be needed if we have to create CPU mappings
    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_read(va_space);

    if (uvm_uuid_is_cpu(&params->destinationUuid)) {
        dest_id = UVM_ID_CPU;
    }
    else {
        gpu = uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(va_space, &params->destinationUuid);
        if (!gpu) {
            status = NV_ERR_INVALID_DEVICE;
            goto done;
        }

        dest_id = gpu->id;
    }

    if (gpu && gpu->parent->is_integrated_gpu)
            dest_id = UVM_ID_CPU;

    range_group = radix_tree_lookup(&va_space->range_groups, params->rangeGroupId);
    if (!range_group) {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    // Migrate all VA ranges in the range group. uvm_migrate is used because it performs all
    // VA range validity checks.
    list_for_each_entry(rgr, &range_group->ranges, range_group_list_node) {
        NvU64 start = rgr->node.start;
        NvU64 length = rgr->node.end - rgr->node.start + 1;

        if (gpu && !uvm_gpu_can_address(gpu, start, length)) {
            status = NV_ERR_OUT_OF_RANGE;
        }
        else {
            uvm_va_range_managed_t *first_managed_range = uvm_va_space_iter_managed_first(va_space, start, start);

            if (!first_managed_range) {
                status = NV_ERR_INVALID_ADDRESS;
                goto done;
            }

            status = uvm_migrate(va_space,
                                 mm,
                                 start,
                                 length,
                                 dest_id,
                                 NUMA_NO_NODE,
                                 migrate_flags,
                                 first_managed_range,
                                 &local_tracker,
                                 gpus_to_check_for_nvlink_errors);
        }

        if (status != NV_OK)
            goto done;
    }

done:
    uvm_global_gpu_retain(gpus_to_check_for_nvlink_errors);

    // We only need to hold mmap_lock to create new CPU mappings, so drop it if
    // we need to wait for the tracker to finish.
    //
    // TODO: Bug 1766650: For large migrations with destination CPU, try
    //       benchmarks to see if a two-pass approach would be faster (first
    //       pass pushes all GPU work asynchronously, second pass updates CPU
    //       mappings synchronously).
    if (mm)
        uvm_up_read_mmap_lock_out_of_order(mm);

    tracker_status = uvm_tracker_wait_deinit(&local_tracker);
    uvm_va_space_up_read(va_space);
    uvm_va_space_mm_or_current_release(va_space, mm);

    // This API is synchronous, so wait for migrations to finish
    uvm_tools_flush_events();

    // Check for STO errors in case there was no other error until now.
    if (status == NV_OK && tracker_status == NV_OK)
        status = uvm_global_gpu_check_nvlink_error(gpus_to_check_for_nvlink_errors);

    uvm_global_gpu_release(gpus_to_check_for_nvlink_errors);
    uvm_processor_mask_cache_free(gpus_to_check_for_nvlink_errors);

    return status == NV_OK? tracker_status : status;
}
