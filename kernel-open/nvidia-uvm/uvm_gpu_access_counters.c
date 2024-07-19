/*******************************************************************************
    Copyright (c) 2017-2024 NVIDIA Corporation

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

#include "linux/sort.h"
#include "nv_uvm_interface.h"
#include "uvm_gpu_access_counters.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "uvm_kvmalloc.h"
#include "uvm_tools.h"
#include "uvm_va_block.h"
#include "uvm_va_range.h"
#include "uvm_va_space_mm.h"
#include "uvm_pmm_sysmem.h"
#include "uvm_perf_module.h"
#include "uvm_ats.h"
#include "uvm_ats_faults.h"

#define UVM_PERF_ACCESS_COUNTER_BATCH_COUNT_MIN     1
#define UVM_PERF_ACCESS_COUNTER_BATCH_COUNT_DEFAULT 256
#define UVM_PERF_ACCESS_COUNTER_GRANULARITY         UVM_ACCESS_COUNTER_GRANULARITY_2M
#define UVM_PERF_ACCESS_COUNTER_THRESHOLD_MIN       1
#define UVM_PERF_ACCESS_COUNTER_THRESHOLD_MAX       ((1 << 16) - 1)
#define UVM_PERF_ACCESS_COUNTER_THRESHOLD_DEFAULT   256

#define UVM_ACCESS_COUNTER_ACTION_CLEAR     0x1
#define UVM_ACCESS_COUNTER_PHYS_ON_MANAGED  0x2

// Each page in a tracked physical range may belong to a different VA Block. We
// preallocate an array of reverse map translations. However, access counter
// granularity can be set to up to 16G, which would require an array too large
// to hold all possible translations. Thus, we set an upper bound for reverse
// map translations, and we perform as many translation requests as needed to
// cover the whole tracked range.
#define UVM_MAX_TRANSLATION_SIZE (2 * 1024 * 1024ULL)
#define UVM_SUB_GRANULARITY_REGIONS 32

static unsigned g_uvm_access_counter_threshold;

// Per-VA space access counters information
typedef struct
{
    // VA space-specific configuration settings. These override the global
    // settings
    struct
    {
        atomic_t enable_mimc_migrations;

        atomic_t enable_momc_migrations;
    } params;

    uvm_va_space_t *va_space;
} va_space_access_counters_info_t;

// Enable/disable access-counter-guided migrations
//
static int uvm_perf_access_counter_mimc_migration_enable = -1;
static int uvm_perf_access_counter_momc_migration_enable = -1;

// Number of entries that are fetched from the GPU access counter notification
// buffer and serviced in batch
static unsigned uvm_perf_access_counter_batch_count = UVM_PERF_ACCESS_COUNTER_BATCH_COUNT_DEFAULT;

// See module param documentation below
static unsigned uvm_perf_access_counter_threshold = UVM_PERF_ACCESS_COUNTER_THRESHOLD_DEFAULT;

// Module parameters for the tunables
module_param(uvm_perf_access_counter_mimc_migration_enable, int, S_IRUGO);
MODULE_PARM_DESC(uvm_perf_access_counter_mimc_migration_enable,
                 "Whether MIMC access counters will trigger migrations."
                 "Valid values: <= -1 (default policy), 0 (off), >= 1 (on)");
module_param(uvm_perf_access_counter_momc_migration_enable, int, S_IRUGO);
MODULE_PARM_DESC(uvm_perf_access_counter_momc_migration_enable,
                 "Whether MOMC access counters will trigger migrations."
                 "Valid values: <= -1 (default policy), 0 (off), >= 1 (on)");
module_param(uvm_perf_access_counter_batch_count, uint, S_IRUGO);
module_param(uvm_perf_access_counter_threshold, uint, S_IRUGO);
MODULE_PARM_DESC(uvm_perf_access_counter_threshold,
                 "Number of remote accesses on a region required to trigger a notification."
                 "Valid values: [1, 65535]");

static void access_counter_buffer_flush_locked(uvm_parent_gpu_t *parent_gpu,
                                               uvm_gpu_buffer_flush_mode_t flush_mode);

static uvm_perf_module_event_callback_desc_t g_callbacks_access_counters[] = {};

// Performance heuristics module for access_counters
static uvm_perf_module_t g_module_access_counters;

// Get the access counters tracking struct for the given VA space if it exists.
// This information is allocated at VA space creation and freed during VA space
// destruction.
static va_space_access_counters_info_t *va_space_access_counters_info_get_or_null(uvm_va_space_t *va_space)
{
    return uvm_perf_module_type_data(va_space->perf_modules_data, UVM_PERF_MODULE_TYPE_ACCESS_COUNTERS);
}

// Get the access counters tracking struct for the given VA space. It asserts
// that the information has been previously created.
static va_space_access_counters_info_t *va_space_access_counters_info_get(uvm_va_space_t *va_space)
{
    va_space_access_counters_info_t *va_space_access_counters = va_space_access_counters_info_get_or_null(va_space);
    UVM_ASSERT(va_space_access_counters);

    return va_space_access_counters;
}

// Whether access counter migrations are enabled or not. The policy is as
// follows:
// - MIMC migrations are disabled by default on all non-ATS systems.
// - MOMC migrations are disabled by default on all systems
// - Users can override this policy by specifying on/off
static bool is_migration_enabled(uvm_access_counter_type_t type)
{
    int val;
    if (type == UVM_ACCESS_COUNTER_TYPE_MIMC) {
        val = uvm_perf_access_counter_mimc_migration_enable;
    }
    else {
        val = uvm_perf_access_counter_momc_migration_enable;

        UVM_ASSERT(type == UVM_ACCESS_COUNTER_TYPE_MOMC);
    }

    if (val == 0)
        return false;
    else if (val > 0)
        return true;

    if (type == UVM_ACCESS_COUNTER_TYPE_MOMC)
        return false;

    if (UVM_ATS_SUPPORTED())
        return g_uvm_global.ats.supported;

    return false;
}

// Create the access counters tracking struct for the given VA space
//
// VA space lock needs to be held in write mode
static va_space_access_counters_info_t *va_space_access_counters_info_create(uvm_va_space_t *va_space)
{
    va_space_access_counters_info_t *va_space_access_counters;
    uvm_assert_rwsem_locked_write(&va_space->lock);

    UVM_ASSERT(va_space_access_counters_info_get_or_null(va_space) == NULL);

    va_space_access_counters = uvm_kvmalloc_zero(sizeof(*va_space_access_counters));
    if (va_space_access_counters) {
        uvm_perf_module_type_set_data(va_space->perf_modules_data,
                                      va_space_access_counters,
                                      UVM_PERF_MODULE_TYPE_ACCESS_COUNTERS);

        // Snap the access_counters parameters so that they can be tuned per VA space
        atomic_set(&va_space_access_counters->params.enable_mimc_migrations,
                   is_migration_enabled(UVM_ACCESS_COUNTER_TYPE_MIMC));
        atomic_set(&va_space_access_counters->params.enable_momc_migrations,
                   is_migration_enabled(UVM_ACCESS_COUNTER_TYPE_MOMC));
        va_space_access_counters->va_space = va_space;
    }

    return va_space_access_counters;
}

// Destroy the access counters tracking struct for the given VA space
//
// VA space lock needs to be in write mode
static void va_space_access_counters_info_destroy(uvm_va_space_t *va_space)
{
    va_space_access_counters_info_t *va_space_access_counters = va_space_access_counters_info_get_or_null(va_space);
    uvm_assert_rwsem_locked_write(&va_space->lock);

    if (va_space_access_counters) {
        uvm_perf_module_type_unset_data(va_space->perf_modules_data, UVM_PERF_MODULE_TYPE_ACCESS_COUNTERS);
        uvm_kvfree(va_space_access_counters);
    }
}

static NV_STATUS config_granularity_to_bytes(UVM_ACCESS_COUNTER_GRANULARITY granularity, NvU64 *bytes)
{
    switch (granularity) {
        case UVM_ACCESS_COUNTER_GRANULARITY_64K:
            *bytes = 64 * 1024ULL;
            break;
        case UVM_ACCESS_COUNTER_GRANULARITY_2M:
            *bytes = 2 * UVM_SIZE_1MB;
            break;
        case UVM_ACCESS_COUNTER_GRANULARITY_16M:
            *bytes = 16 * UVM_SIZE_1MB;
            break;
        case UVM_ACCESS_COUNTER_GRANULARITY_16G:
            *bytes = 16 * UVM_SIZE_1GB;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

// Clear the access counter notifications and add it to the per-GPU clear
// tracker.
static NV_STATUS access_counter_clear_notifications(uvm_gpu_t *gpu,
                                                    uvm_access_counter_buffer_entry_t **notification_start,
                                                    NvU32 num_notifications)
{
    NvU32 i;
    NV_STATUS status;
    uvm_push_t push;
    uvm_access_counter_buffer_info_t *access_counters = &gpu->parent->access_counter_buffer_info;

    status = uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_MEMOPS, &push, "Clear access counter batch");
    if (status != NV_OK) {
        UVM_ERR_PRINT("Error creating push to clear access counters: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    for (i = 0; i < num_notifications; i++)
        gpu->parent->host_hal->access_counter_clear_targeted(&push, notification_start[i]);

    uvm_push_end(&push);

    uvm_tracker_remove_completed(&access_counters->clear_tracker);

    return uvm_tracker_add_push_safe(&access_counters->clear_tracker, &push);
}

// Clear all access counters and add the operation to the per-GPU clear tracker
static NV_STATUS access_counter_clear_all(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    uvm_push_t push;
    uvm_access_counter_buffer_info_t *access_counters = &gpu->parent->access_counter_buffer_info;

    status = uvm_push_begin(gpu->channel_manager,
                            UVM_CHANNEL_TYPE_MEMOPS,
                            &push,
                            "Clear access counter: all");
    if (status != NV_OK) {
        UVM_ERR_PRINT("Error creating push to clear access counters: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    gpu->parent->host_hal->access_counter_clear_all(&push);

    uvm_push_end(&push);

    uvm_tracker_remove_completed(&access_counters->clear_tracker);

    return uvm_tracker_add_push_safe(&access_counters->clear_tracker, &push);
}

static const uvm_gpu_access_counter_type_config_t *
get_config_for_type(const uvm_access_counter_buffer_info_t *access_counters, uvm_access_counter_type_t counter_type)
{
    return counter_type == UVM_ACCESS_COUNTER_TYPE_MIMC? &(access_counters)->current_config.mimc :
                                                         &(access_counters)->current_config.momc;
}

bool uvm_parent_gpu_access_counters_pending(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT(parent_gpu->access_counters_supported);

    // Fast path 1: we left some notifications unserviced in the buffer in the last pass
    if (parent_gpu->access_counter_buffer_info.cached_get != parent_gpu->access_counter_buffer_info.cached_put)
        return true;

    // Fast path 2: read the valid bit of the notification buffer entry pointed by the cached get pointer
    if (!parent_gpu->access_counter_buffer_hal->entry_is_valid(parent_gpu,
                                                               parent_gpu->access_counter_buffer_info.cached_get)) {
        // Slow path: read the put pointer from the GPU register via BAR0 over PCIe
        parent_gpu->access_counter_buffer_info.cached_put =
            UVM_GPU_READ_ONCE(*parent_gpu->access_counter_buffer_info.rm_info.pAccessCntrBufferPut);

        // No interrupt pending
        if (parent_gpu->access_counter_buffer_info.cached_get == parent_gpu->access_counter_buffer_info.cached_put)
            return false;
    }

    return true;
}

// Initialize the configuration and pre-compute some required values for the
// given access counter type
static void init_access_counter_types_config(const UvmGpuAccessCntrConfig *config,
                                             uvm_access_counter_type_t counter_type,
                                             uvm_gpu_access_counter_type_config_t *counter_type_config)
{
    NV_STATUS status;
    NvU64 tracking_size = 0;
    UVM_ACCESS_COUNTER_GRANULARITY granularity = counter_type == UVM_ACCESS_COUNTER_TYPE_MIMC? config->mimcGranularity:
                                                                                               config->momcGranularity;
    UVM_ACCESS_COUNTER_USE_LIMIT use_limit = counter_type == UVM_ACCESS_COUNTER_TYPE_MIMC? config->mimcUseLimit:
                                                                                           config->momcUseLimit;

    counter_type_config->rm.granularity = granularity;
    counter_type_config->rm.use_limit = use_limit;

    // Precompute the maximum size to use in reverse map translations and the
    // number of translations that are required per access counter notification.
    status = config_granularity_to_bytes(granularity, &tracking_size);
    UVM_ASSERT(status == NV_OK);

    // sub_granularity field is only filled for tracking granularities larger
    // than 64K
    if (granularity == UVM_ACCESS_COUNTER_GRANULARITY_64K)
        counter_type_config->sub_granularity_region_size = tracking_size;
    else
        counter_type_config->sub_granularity_region_size = tracking_size / UVM_SUB_GRANULARITY_REGIONS;

    counter_type_config->translation_size = min(UVM_MAX_TRANSLATION_SIZE, tracking_size);
    counter_type_config->translations_per_counter =
        max(counter_type_config->translation_size / UVM_MAX_TRANSLATION_SIZE, 1ULL);
    counter_type_config->sub_granularity_regions_per_translation =
        max(counter_type_config->translation_size / counter_type_config->sub_granularity_region_size, 1ULL);
    UVM_ASSERT(counter_type_config->sub_granularity_regions_per_translation <= UVM_SUB_GRANULARITY_REGIONS);
}

NV_STATUS uvm_parent_gpu_init_access_counters(uvm_parent_gpu_t *parent_gpu)
{
    NV_STATUS status = NV_OK;
    uvm_access_counter_buffer_info_t *access_counters = &parent_gpu->access_counter_buffer_info;
    uvm_access_counter_service_batch_context_t *batch_context = &access_counters->batch_service_context;
    NvU64 granularity_bytes = 0;

    if (uvm_perf_access_counter_threshold < UVM_PERF_ACCESS_COUNTER_THRESHOLD_MIN) {
        g_uvm_access_counter_threshold = UVM_PERF_ACCESS_COUNTER_THRESHOLD_MIN;
        pr_info("Value %u too small for uvm_perf_access_counter_threshold, using %u instead\n",
                uvm_perf_access_counter_threshold,
                g_uvm_access_counter_threshold);
    }
    else if (uvm_perf_access_counter_threshold > UVM_PERF_ACCESS_COUNTER_THRESHOLD_MAX) {
        g_uvm_access_counter_threshold = UVM_PERF_ACCESS_COUNTER_THRESHOLD_MAX;
        pr_info("Value %u too large for uvm_perf_access_counter_threshold, using %u instead\n",
                uvm_perf_access_counter_threshold,
                g_uvm_access_counter_threshold);
    }
    else {
        g_uvm_access_counter_threshold = uvm_perf_access_counter_threshold;
    }

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);
    UVM_ASSERT(parent_gpu->access_counter_buffer_hal != NULL);

    status = uvm_rm_locked_call(nvUvmInterfaceInitAccessCntrInfo(parent_gpu->rm_device,
                                                                 &access_counters->rm_info,
                                                                 0));
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to init notify buffer info from RM: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));

        // nvUvmInterfaceInitAccessCntrInfo may leave fields in rm_info
        // populated when it returns an error. Set the buffer handle to zero as
        // it is used by the deinitialization logic to determine if it was
        // correctly initialized.
        access_counters->rm_info.accessCntrBufferHandle = 0;
        goto fail;
    }

    UVM_ASSERT(access_counters->rm_info.bufferSize %
               parent_gpu->access_counter_buffer_hal->entry_size(parent_gpu) == 0);

    status = config_granularity_to_bytes(UVM_PERF_ACCESS_COUNTER_GRANULARITY, &granularity_bytes);
    UVM_ASSERT(status == NV_OK);
    if (granularity_bytes > UVM_MAX_TRANSLATION_SIZE)
        UVM_ASSERT(granularity_bytes % UVM_MAX_TRANSLATION_SIZE == 0);

    parent_gpu->access_counter_buffer_info.notifications_ignored_count = 0;
    parent_gpu->access_counter_buffer_info.reconfiguration_owner = NULL;

    uvm_tracker_init(&access_counters->clear_tracker);

    access_counters->max_notifications = parent_gpu->access_counter_buffer_info.rm_info.bufferSize /
                                         parent_gpu->access_counter_buffer_hal->entry_size(parent_gpu);

    // Check provided module parameter value
    access_counters->max_batch_size = max(uvm_perf_access_counter_batch_count,
                                          (NvU32)UVM_PERF_ACCESS_COUNTER_BATCH_COUNT_MIN);
    access_counters->max_batch_size = min(access_counters->max_batch_size,
                                          access_counters->max_notifications);

    if (access_counters->max_batch_size != uvm_perf_access_counter_batch_count) {
        pr_info("Invalid uvm_perf_access_counter_batch_count value on GPU %s: %u. Valid range [%u:%u] Using %u instead\n",
                uvm_parent_gpu_name(parent_gpu),
                uvm_perf_access_counter_batch_count,
                UVM_PERF_ACCESS_COUNTER_BATCH_COUNT_MIN,
                access_counters->max_notifications,
                access_counters->max_batch_size);
    }

    batch_context->notification_cache = uvm_kvmalloc_zero(access_counters->max_notifications *
                                                          sizeof(*batch_context->notification_cache));
    if (!batch_context->notification_cache) {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    batch_context->virt.notifications = uvm_kvmalloc_zero(access_counters->max_notifications *
                                                          sizeof(*batch_context->virt.notifications));
    if (!batch_context->virt.notifications) {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    batch_context->phys.notifications = uvm_kvmalloc_zero(access_counters->max_notifications *
                                                          sizeof(*batch_context->phys.notifications));
    if (!batch_context->phys.notifications) {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    batch_context->phys.translations = uvm_kvmalloc_zero((UVM_MAX_TRANSLATION_SIZE / PAGE_SIZE) *
                                                         sizeof(*batch_context->phys.translations));
    if (!batch_context->phys.translations) {
        status = NV_ERR_NO_MEMORY;
        goto fail;
    }

    return NV_OK;

fail:
    uvm_parent_gpu_deinit_access_counters(parent_gpu);

    return status;
}

void uvm_parent_gpu_deinit_access_counters(uvm_parent_gpu_t *parent_gpu)
{
    uvm_access_counter_buffer_info_t *access_counters = &parent_gpu->access_counter_buffer_info;
    uvm_access_counter_service_batch_context_t *batch_context = &access_counters->batch_service_context;

    UVM_ASSERT(parent_gpu->isr.access_counters.handling_ref_count == 0);

    if (access_counters->rm_info.accessCntrBufferHandle) {
        NV_STATUS status = uvm_rm_locked_call(nvUvmInterfaceDestroyAccessCntrInfo(parent_gpu->rm_device,
                                                                                  &access_counters->rm_info));
        UVM_ASSERT(status == NV_OK);

        access_counters->rm_info.accessCntrBufferHandle = 0;
        uvm_tracker_deinit(&access_counters->clear_tracker);
    }

    uvm_kvfree(batch_context->notification_cache);
    uvm_kvfree(batch_context->virt.notifications);
    uvm_kvfree(batch_context->phys.notifications);
    uvm_kvfree(batch_context->phys.translations);
    batch_context->notification_cache = NULL;
    batch_context->virt.notifications = NULL;
    batch_context->phys.notifications = NULL;
    batch_context->phys.translations = NULL;
}

bool uvm_parent_gpu_access_counters_required(const uvm_parent_gpu_t *parent_gpu)
{
    if (!parent_gpu->access_counters_supported)
        return false;

    if (parent_gpu->rm_info.isSimulated)
        return true;

    return is_migration_enabled(UVM_ACCESS_COUNTER_TYPE_MIMC) || is_migration_enabled(UVM_ACCESS_COUNTER_TYPE_MOMC);
}

// This function enables access counters with the given configuration and takes
// ownership from RM. The function also stores the new configuration within the
// uvm_gpu_t struct.
static NV_STATUS access_counters_take_ownership(uvm_gpu_t *gpu, UvmGpuAccessCntrConfig *config)
{
    NV_STATUS status, disable_status;
    uvm_access_counter_buffer_info_t *access_counters = &gpu->parent->access_counter_buffer_info;

    UVM_ASSERT(gpu->parent->access_counters_supported);
    UVM_ASSERT(uvm_sem_is_locked(&gpu->parent->isr.access_counters.service_lock));

    status = uvm_rm_locked_call(nvUvmInterfaceEnableAccessCntr(gpu->parent->rm_device,
                                                               &access_counters->rm_info,
                                                               config));
    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to enable access counter notification from RM: %s, GPU %s\n",
                      nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    status = access_counter_clear_all(gpu);
    if (status != NV_OK)
        goto error;

    status = uvm_tracker_wait(&access_counters->clear_tracker);
    if (status != NV_OK)
        goto error;

    // Read current get pointer as this might not be the first time we have
    // taken control of the notify buffer since the GPU was initialized. Then
    // flush old notifications. This will update the cached_put pointer.
    access_counters->cached_get = UVM_GPU_READ_ONCE(*access_counters->rm_info.pAccessCntrBufferGet);
    access_counter_buffer_flush_locked(gpu->parent, UVM_GPU_BUFFER_FLUSH_MODE_UPDATE_PUT);

    access_counters->current_config.threshold = config->threshold;

    init_access_counter_types_config(config, UVM_ACCESS_COUNTER_TYPE_MIMC, &access_counters->current_config.mimc);
    init_access_counter_types_config(config, UVM_ACCESS_COUNTER_TYPE_MOMC, &access_counters->current_config.momc);

    return NV_OK;

error:
    disable_status = uvm_rm_locked_call(nvUvmInterfaceDisableAccessCntr(gpu->parent->rm_device,
                                                                        &access_counters->rm_info));
    UVM_ASSERT(disable_status == NV_OK);

    return status;
}

// If ownership is yielded as part of reconfiguration, the access counters
// handling refcount may not be 0
static void access_counters_yield_ownership(uvm_parent_gpu_t *parent_gpu)
{
    NV_STATUS status;
    uvm_access_counter_buffer_info_t *access_counters = &parent_gpu->access_counter_buffer_info;

    UVM_ASSERT(parent_gpu->access_counters_supported);
    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.access_counters.service_lock));

    // Wait for any pending clear operation befor releasing ownership
    status = uvm_tracker_wait(&access_counters->clear_tracker);
    if (status != NV_OK)
        UVM_ASSERT(status == uvm_global_get_status());

    status = uvm_rm_locked_call(nvUvmInterfaceDisableAccessCntr(parent_gpu->rm_device,
                                                                &access_counters->rm_info));
    UVM_ASSERT(status == NV_OK);
}

// Increment the refcount of access counter enablement. If this is the first
// reference, enable the HW feature.
static NV_STATUS gpu_access_counters_enable(uvm_gpu_t *gpu, UvmGpuAccessCntrConfig *config)
{
    UVM_ASSERT(uvm_sem_is_locked(&gpu->parent->isr.access_counters.service_lock));
    UVM_ASSERT(gpu->parent->access_counters_supported);
    UVM_ASSERT(gpu->parent->access_counter_buffer_info.rm_info.accessCntrBufferHandle);

    // There cannot be a concurrent modification of the handling count, since
    // the only two writes of that field happen in the enable/disable functions
    // and those are protected by the access counters ISR lock.
    if (gpu->parent->isr.access_counters.handling_ref_count == 0) {
        NV_STATUS status = access_counters_take_ownership(gpu, config);

        if (status != NV_OK)
            return status;
    }

    ++gpu->parent->isr.access_counters.handling_ref_count;
    return NV_OK;
}

// Decrement the refcount of access counter enablement. If this is the last
// reference, disable the HW feature.
static void parent_gpu_access_counters_disable(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.access_counters.service_lock));
    UVM_ASSERT(parent_gpu->access_counters_supported);
    UVM_ASSERT(parent_gpu->isr.access_counters.handling_ref_count > 0);

    if (--parent_gpu->isr.access_counters.handling_ref_count == 0)
        access_counters_yield_ownership(parent_gpu);
}

// Invoked during registration of the GPU in the VA space
NV_STATUS uvm_gpu_access_counters_enable(uvm_gpu_t *gpu, uvm_va_space_t *va_space)
{
    NV_STATUS status;

    UVM_ASSERT(gpu->parent->access_counters_supported);

    uvm_parent_gpu_access_counters_isr_lock(gpu->parent);

    if (uvm_parent_processor_mask_test(&va_space->access_counters_enabled_processors, gpu->parent->id)) {
        status = NV_ERR_INVALID_DEVICE;
    }
    else {
        UvmGpuAccessCntrConfig default_config =
        {
            .mimcGranularity = UVM_PERF_ACCESS_COUNTER_GRANULARITY,
            .momcGranularity = UVM_PERF_ACCESS_COUNTER_GRANULARITY,
            .mimcUseLimit = UVM_ACCESS_COUNTER_USE_LIMIT_FULL,
            .momcUseLimit = UVM_ACCESS_COUNTER_USE_LIMIT_FULL,
            .threshold = g_uvm_access_counter_threshold,
        };
        status = gpu_access_counters_enable(gpu, &default_config);

        // No VA space lock is currently held, so the mask is atomically
        // modified to protect from concurrent enablement of access counters in
        // another GPU
        if (status == NV_OK)
            uvm_parent_processor_mask_set_atomic(&va_space->access_counters_enabled_processors, gpu->parent->id);
    }

    // If this is the first reference taken on access counters, dropping the
    // ISR lock will enable interrupts.
    uvm_parent_gpu_access_counters_isr_unlock(gpu->parent);

    return status;
}

void uvm_parent_gpu_access_counters_disable(uvm_parent_gpu_t *parent_gpu,
                                            uvm_va_space_t *va_space)
{
    UVM_ASSERT(parent_gpu->access_counters_supported);

    uvm_parent_gpu_access_counters_isr_lock(parent_gpu);

    if (uvm_parent_processor_mask_test_and_clear_atomic(&va_space->access_counters_enabled_processors,
                                                        parent_gpu->id)) {
        parent_gpu_access_counters_disable(parent_gpu);

        // If this is VA space reconfigured access counters, clear the
        // ownership to allow for other processes to invoke the reconfiguration
        if (parent_gpu->access_counter_buffer_info.reconfiguration_owner == va_space)
            parent_gpu->access_counter_buffer_info.reconfiguration_owner = NULL;
    }

    uvm_parent_gpu_access_counters_isr_unlock(parent_gpu);
}

static void write_get(uvm_parent_gpu_t *parent_gpu, NvU32 get)
{
    uvm_access_counter_buffer_info_t *access_counters = &parent_gpu->access_counter_buffer_info;

    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.access_counters.service_lock));

    // Write get on the GPU only if it's changed.
    if (access_counters->cached_get == get)
        return;

    access_counters->cached_get = get;

    // Update get pointer on the GPU
    UVM_GPU_WRITE_ONCE(*access_counters->rm_info.pAccessCntrBufferGet, get);
}

static void access_counter_buffer_flush_locked(uvm_parent_gpu_t *parent_gpu,
                                               uvm_gpu_buffer_flush_mode_t flush_mode)
{
    NvU32 get;
    NvU32 put;
    uvm_spin_loop_t spin;
    uvm_access_counter_buffer_info_t *access_counters = &parent_gpu->access_counter_buffer_info;

    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.access_counters.service_lock));
    UVM_ASSERT(parent_gpu->access_counters_supported);

    // Read PUT pointer from the GPU if requested
    UVM_ASSERT(flush_mode != UVM_GPU_BUFFER_FLUSH_MODE_WAIT_UPDATE_PUT);
    if (flush_mode == UVM_GPU_BUFFER_FLUSH_MODE_UPDATE_PUT)
        access_counters->cached_put = UVM_GPU_READ_ONCE(*access_counters->rm_info.pAccessCntrBufferPut);

    get = access_counters->cached_get;
    put = access_counters->cached_put;

    while (get != put) {
        // Wait until valid bit is set
        UVM_SPIN_WHILE(!parent_gpu->access_counter_buffer_hal->entry_is_valid(parent_gpu, get), &spin);

        parent_gpu->access_counter_buffer_hal->entry_clear_valid(parent_gpu, get);
        ++get;
        if (get == access_counters->max_notifications)
            get = 0;
    }

    write_get(parent_gpu, get);
}

void uvm_parent_gpu_access_counter_buffer_flush(uvm_parent_gpu_t *parent_gpu)
{
    UVM_ASSERT(parent_gpu->access_counters_supported);

    // Disables access counter interrupts and notification servicing
    uvm_parent_gpu_access_counters_isr_lock(parent_gpu);

    if (parent_gpu->isr.access_counters.handling_ref_count > 0)
        access_counter_buffer_flush_locked(parent_gpu, UVM_GPU_BUFFER_FLUSH_MODE_UPDATE_PUT);

    uvm_parent_gpu_access_counters_isr_unlock(parent_gpu);
}

static inline int cmp_access_counter_instance_ptr(const uvm_access_counter_buffer_entry_t *a,
                                                  const uvm_access_counter_buffer_entry_t *b)
{
    int result;

    result = uvm_gpu_phys_addr_cmp(a->virtual_info.instance_ptr, b->virtual_info.instance_ptr);
    // On Volta+ we need to sort by {instance_ptr + subctx_id} pair since it can
    // map to a different VA space
    if (result != 0)
        return result;
    return UVM_CMP_DEFAULT(a->virtual_info.ve_id, b->virtual_info.ve_id);
}

// Sort comparator for pointers to GVA access counter notification buffer
// entries that sorts by instance pointer
static int cmp_sort_virt_notifications_by_instance_ptr(const void *_a, const void *_b)
{
    const uvm_access_counter_buffer_entry_t *a = *(const uvm_access_counter_buffer_entry_t **)_a;
    const uvm_access_counter_buffer_entry_t *b = *(const uvm_access_counter_buffer_entry_t **)_b;

    UVM_ASSERT(a->address.is_virtual);
    UVM_ASSERT(b->address.is_virtual);

    return cmp_access_counter_instance_ptr(a, b);
}

// Compare two GPUs
static inline int cmp_gpu(const uvm_gpu_t *a, const uvm_gpu_t *b)
{
    NvU32 id_a = a ? uvm_id_value(a->id) : 0;
    NvU32 id_b = b ? uvm_id_value(b->id) : 0;

    return UVM_CMP_DEFAULT(id_a, id_b);
}

// Sort comparator for pointers to GVA access counter notification buffer
// entries that sorts by va_space, GPU ID, and fault address.
static int cmp_sort_virt_notifications_by_va_space_gpu_address(const void *_a, const void *_b)
{
    const uvm_access_counter_buffer_entry_t **a = (const uvm_access_counter_buffer_entry_t **)_a;
    const uvm_access_counter_buffer_entry_t **b = (const uvm_access_counter_buffer_entry_t **)_b;

    int result;

    result = UVM_CMP_DEFAULT((*a)->virtual_info.va_space, (*b)->virtual_info.va_space);
    if (result != 0)
        return result;

    result = cmp_gpu((*a)->gpu, (*b)->gpu);
    if (result != 0)
        return result;

    return UVM_CMP_DEFAULT((*a)->address.address, (*b)->address.address);
}

// Sort comparator for pointers to GPA access counter notification buffer
// entries that sorts by physical address' aperture
static int cmp_sort_phys_notifications_by_processor_id(const void *_a, const void *_b)
{
    const uvm_access_counter_buffer_entry_t *a = *(const uvm_access_counter_buffer_entry_t **)_a;
    const uvm_access_counter_buffer_entry_t *b = *(const uvm_access_counter_buffer_entry_t **)_b;

    UVM_ASSERT(!a->address.is_virtual);
    UVM_ASSERT(!b->address.is_virtual);

    return uvm_id_cmp(a->physical_info.resident_id, b->physical_info.resident_id);
}

typedef enum
{
    // Fetch a batch of notifications from the buffer. Stop at the first entry
    // that is not ready yet
    NOTIFICATION_FETCH_MODE_BATCH_READY,

    // Fetch all notifications in the buffer before PUT. Wait for all
    // notifications to become ready
    NOTIFICATION_FETCH_MODE_ALL,
} notification_fetch_mode_t;

static NvU32 fetch_access_counter_buffer_entries(uvm_parent_gpu_t *parent_gpu,
                                                 uvm_access_counter_service_batch_context_t *batch_context,
                                                 notification_fetch_mode_t fetch_mode)
{
    NvU32 get;
    NvU32 put;
    NvU32 notification_index;
    uvm_access_counter_buffer_entry_t *notification_cache;
    uvm_spin_loop_t spin;
    uvm_access_counter_buffer_info_t *access_counters = &parent_gpu->access_counter_buffer_info;
    NvU32 last_instance_ptr_idx = 0;
    uvm_aperture_t last_aperture = UVM_APERTURE_PEER_MAX;

    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.access_counters.service_lock));
    UVM_ASSERT(parent_gpu->access_counters_supported);

    notification_cache = batch_context->notification_cache;

    get = access_counters->cached_get;

    // Read put pointer from GPU and cache it
    if (get == access_counters->cached_put) {
        access_counters->cached_put = UVM_GPU_READ_ONCE(*access_counters->rm_info.pAccessCntrBufferPut);
    }

    put = access_counters->cached_put;

    if (get == put)
        return 0;

    batch_context->phys.num_notifications = 0;
    batch_context->virt.num_notifications = 0;

    batch_context->virt.is_single_instance_ptr = true;
    batch_context->phys.is_single_aperture = true;

    notification_index = 0;

    // Parse until get != put and have enough space to cache.
    while ((get != put) &&
           (fetch_mode == NOTIFICATION_FETCH_MODE_ALL || notification_index < access_counters->max_batch_size)) {
        uvm_access_counter_buffer_entry_t *current_entry = &notification_cache[notification_index];

        // We cannot just wait for the last entry (the one pointed by put) to become valid, we have to do it
        // individually since entries can be written out of order
        UVM_SPIN_WHILE(!parent_gpu->access_counter_buffer_hal->entry_is_valid(parent_gpu, get), &spin) {
            // We have some entry to work on. Let's do the rest later.
            if (fetch_mode != NOTIFICATION_FETCH_MODE_ALL && notification_index > 0)
                goto done;
        }

        // Prevent later accesses being moved above the read of the valid bit
        smp_mb__after_atomic();

        // Got valid bit set. Let's cache.
        parent_gpu->access_counter_buffer_hal->parse_entry(parent_gpu, get, current_entry);

        if (current_entry->address.is_virtual) {
            batch_context->virt.notifications[batch_context->virt.num_notifications++] = current_entry;

            if (batch_context->virt.is_single_instance_ptr) {
                if (batch_context->virt.num_notifications == 1) {
                    last_instance_ptr_idx = notification_index;
                }
                else if (cmp_access_counter_instance_ptr(&notification_cache[last_instance_ptr_idx],
                                                         current_entry) != 0) {
                    batch_context->virt.is_single_instance_ptr = false;
                }
            }
        }
        else {
            NvU64 translation_size;
            uvm_gpu_t *gpu;

            translation_size = get_config_for_type(access_counters,
                                                   current_entry->counter_type)->translation_size;
            current_entry->address.address = UVM_ALIGN_DOWN(current_entry->address.address, translation_size);

            batch_context->phys.notifications[batch_context->phys.num_notifications++] = current_entry;

            gpu = uvm_parent_gpu_find_first_valid_gpu(parent_gpu);
            if (!gpu) {
                current_entry->physical_info.resident_id = UVM_ID_INVALID;
                current_entry->gpu = NULL;
            }
            else {
                current_entry->gpu = gpu;
                current_entry->physical_info.resident_id =
                    uvm_gpu_get_processor_id_by_address(gpu, uvm_gpu_phys_address(current_entry->address.aperture,
                                                                                  current_entry->address.address));

                if (batch_context->phys.is_single_aperture) {
                    if (batch_context->phys.num_notifications == 1)
                        last_aperture = current_entry->address.aperture;
                    else if (current_entry->address.aperture != last_aperture)
                        batch_context->phys.is_single_aperture = false;
                }

                if (current_entry->counter_type == UVM_ACCESS_COUNTER_TYPE_MOMC)
                    UVM_ASSERT(uvm_id_equal(current_entry->physical_info.resident_id, gpu->id));
                else
                    UVM_ASSERT(!uvm_id_equal(current_entry->physical_info.resident_id, gpu->id));
            }
        }

        ++notification_index;
        ++get;
        if (get == access_counters->max_notifications)
            get = 0;
    }

done:
    write_get(parent_gpu, get);

    return notification_index;
}

static void translate_virt_notifications_instance_ptrs(uvm_parent_gpu_t *parent_gpu,
                                                       uvm_access_counter_service_batch_context_t *batch_context)
{
    NvU32 i;
    NV_STATUS status;

    for (i = 0; i < batch_context->virt.num_notifications; ++i) {
        uvm_access_counter_buffer_entry_t *current_entry = batch_context->virt.notifications[i];

        if (i == 0 ||
            cmp_access_counter_instance_ptr(current_entry, batch_context->virt.notifications[i - 1]) != 0) {
            // If instance_ptr is different, make a new translation. If the
            // translation fails then va_space will be NULL and the entry will
            // simply be ignored in subsequent processing.
            status = uvm_parent_gpu_access_counter_entry_to_va_space(parent_gpu,
                                                                     current_entry,
                                                                     &current_entry->virtual_info.va_space,
                                                                     &current_entry->gpu);
            if (status != NV_OK) {
                UVM_ASSERT(current_entry->virtual_info.va_space == NULL);
                UVM_ASSERT(current_entry->gpu == NULL);
            }
        }
        else {
            current_entry->virtual_info.va_space = batch_context->virt.notifications[i - 1]->virtual_info.va_space;
            current_entry->gpu = batch_context->virt.notifications[i - 1]->gpu;
        }
    }
}

// GVA notifications provide an instance_ptr and ve_id that can be directly
// translated to a VA space. In order to minimize translations, we sort the
// entries by instance_ptr, va_space and notification address in that order.
static void preprocess_virt_notifications(uvm_parent_gpu_t *parent_gpu,
                                          uvm_access_counter_service_batch_context_t *batch_context)
{
    if (!batch_context->virt.is_single_instance_ptr) {
        sort(batch_context->virt.notifications,
             batch_context->virt.num_notifications,
             sizeof(*batch_context->virt.notifications),
             cmp_sort_virt_notifications_by_instance_ptr,
             NULL);
    }

    translate_virt_notifications_instance_ptrs(parent_gpu, batch_context);

    sort(batch_context->virt.notifications,
         batch_context->virt.num_notifications,
         sizeof(*batch_context->virt.notifications),
         cmp_sort_virt_notifications_by_va_space_gpu_address,
         NULL);
}

// GPA notifications provide a physical address and an aperture. Sort
// accesses by aperture to try to coalesce operations on the same target
// processor.
static void preprocess_phys_notifications(uvm_access_counter_service_batch_context_t *batch_context)
{
    if (!batch_context->phys.is_single_aperture) {
        sort(batch_context->phys.notifications,
             batch_context->phys.num_notifications,
             sizeof(*batch_context->phys.notifications),
             cmp_sort_phys_notifications_by_processor_id,
             NULL);
    }
}

static NV_STATUS notify_tools_broadcast_and_process_flags(uvm_parent_gpu_t *parent_gpu,
                                                          uvm_access_counter_buffer_entry_t **notification_start,
                                                          NvU32 num_entries,
                                                          NvU32 flags)
{
    uvm_gpu_t *gpu = uvm_parent_gpu_find_first_valid_gpu(parent_gpu);
    NV_STATUS status = NV_OK;

    if (!gpu)
        return NV_OK;

    if (uvm_enable_builtin_tests) {
        // TODO: Bug 4310744: [UVM][TOOLS] Attribute access counter tools events
        //                    to va_space instead of broadcasting.
        NvU32 i;

        for (i = 0; i < num_entries; i++)
            uvm_tools_broadcast_access_counter(gpu, notification_start[i], flags & UVM_ACCESS_COUNTER_PHYS_ON_MANAGED);
    }

    if (flags & UVM_ACCESS_COUNTER_ACTION_CLEAR)
        status = access_counter_clear_notifications(gpu, notification_start, num_entries);

    return status;
}

static NV_STATUS notify_tools_and_process_flags(uvm_va_space_t *va_space,
                                                uvm_gpu_t *gpu,
                                                uvm_access_counter_buffer_entry_t **notification_start,
                                                NvU32 num_entries,
                                                NvU32 flags)
{
    NV_STATUS status = NV_OK;

    if (uvm_enable_builtin_tests) {
        NvU32 i;

        for (i = 0; i < num_entries; i++) {
            uvm_tools_record_access_counter(va_space,
                                            gpu->id,
                                            notification_start[i],
                                            flags & UVM_ACCESS_COUNTER_PHYS_ON_MANAGED);
        }
    }

    if (flags & UVM_ACCESS_COUNTER_ACTION_CLEAR)
        status = access_counter_clear_notifications(gpu, notification_start, num_entries);

    return status;
}

static NV_STATUS service_va_block_locked(uvm_processor_id_t processor,
                                         uvm_va_block_t *va_block,
                                         uvm_va_block_retry_t *va_block_retry,
                                         uvm_service_block_context_t *service_context,
                                         uvm_page_mask_t *accessed_pages)
{
    NV_STATUS status = NV_OK;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_range_group_range_iter_t iter;
    uvm_page_index_t page_index;
    uvm_page_index_t first_page_index;
    uvm_page_index_t last_page_index;
    NvU32 page_count = 0;
    const uvm_page_mask_t *residency_mask;
    const bool hmm_migratable = true;

    uvm_assert_mutex_locked(&va_block->lock);

    // GPU VA space could be gone since we received the notification. We handle
    // this case by skipping service if processor is not in the mapped mask.
    // Using this approach we also filter out notifications for pages that
    // moved since they were reported by the GPU. This is fine because:
    // - If the GPU is still accessing them, it should have faulted
    // - If the GPU gets remote mappings in the future, we will get new
    //   notifications and we will act accordingly
    // - If the GPU does not access the pages again, we do not want to migrate
    //   them
    if (!uvm_processor_mask_test(&va_block->mapped, processor))
        return NV_OK;

    if (uvm_processor_mask_test(&va_block->resident, processor))
        residency_mask = uvm_va_block_resident_mask_get(va_block, processor, NUMA_NO_NODE);
    else
        residency_mask = NULL;

    first_page_index = PAGES_PER_UVM_VA_BLOCK;
    last_page_index = 0;

    // Initialize fault service block context
    uvm_processor_mask_zero(&service_context->resident_processors);
    service_context->read_duplicate_count = 0;
    service_context->thrashing_pin_count = 0;

    // If the page is already resident on the accessing processor, the
    // notification for this page is stale. Skip it.
    if (residency_mask)
        uvm_page_mask_andnot(accessed_pages, accessed_pages, residency_mask);

    uvm_range_group_range_migratability_iter_first(va_space, va_block->start, va_block->end, &iter);

    for_each_va_block_page_in_mask(page_index, accessed_pages, va_block) {
        uvm_perf_thrashing_hint_t thrashing_hint;
        NvU64 address = uvm_va_block_cpu_page_address(va_block, page_index);
        bool read_duplicate = false;
        uvm_processor_id_t new_residency;
        const uvm_va_policy_t *policy;

        // Ensure that the migratability iterator covers the current address
        while (iter.end < address)
            uvm_range_group_range_migratability_iter_next(va_space, &iter, va_block->end);

        UVM_ASSERT(iter.start <= address && iter.end >= address);

        // If the range is not migratable, skip the page
        if (!iter.migratable)
            continue;

        thrashing_hint = uvm_perf_thrashing_get_hint(va_block, service_context->block_context, address, processor);
        if (thrashing_hint.type == UVM_PERF_THRASHING_HINT_TYPE_THROTTLE) {
            // If the page is throttling, ignore the access counter
            // notification
            continue;
        }
        else if (thrashing_hint.type == UVM_PERF_THRASHING_HINT_TYPE_PIN) {
            if (service_context->thrashing_pin_count++ == 0)
                uvm_page_mask_zero(&service_context->thrashing_pin_mask);

            uvm_page_mask_set(&service_context->thrashing_pin_mask, page_index);
        }

        // If the underlying VMA is gone, skip HMM migrations.
        if (uvm_va_block_is_hmm(va_block)) {
            status = uvm_hmm_find_vma(service_context->block_context->mm,
                                      &service_context->block_context->hmm.vma,
                                      address);
            if (status == NV_ERR_INVALID_ADDRESS)
                continue;

            UVM_ASSERT(status == NV_OK);
        }

        policy = uvm_va_policy_get(va_block, address);

        new_residency = uvm_va_block_select_residency(va_block,
                                                      service_context->block_context,
                                                      page_index,
                                                      processor,
                                                      uvm_fault_access_type_mask_bit(UVM_FAULT_ACCESS_TYPE_PREFETCH),
                                                      policy,
                                                      &thrashing_hint,
                                                      UVM_SERVICE_OPERATION_ACCESS_COUNTERS,
                                                      hmm_migratable,
                                                      &read_duplicate);

        if (!uvm_processor_mask_test_and_set(&service_context->resident_processors, new_residency))
            uvm_page_mask_zero(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency);

        uvm_page_mask_set(&service_context->per_processor_masks[uvm_id_value(new_residency)].new_residency, page_index);

        if (page_index < first_page_index)
            first_page_index = page_index;
        if (page_index > last_page_index)
            last_page_index = page_index;

        ++page_count;

        service_context->access_type[page_index] = UVM_FAULT_ACCESS_TYPE_PREFETCH;
    }

    // Apply the changes computed in the service block context, if there are
    // pages to be serviced
    if (page_count > 0) {
        uvm_processor_id_t id;
        uvm_processor_mask_t *update_processors = &service_context->update_processors;

        uvm_processor_mask_and(update_processors, &va_block->resident, &service_context->resident_processors);

        // Remove pages that are already resident in the destination processors
        for_each_id_in_mask(id, update_processors) {
            bool migrate_pages;
            uvm_page_mask_t *residency_mask = uvm_va_block_resident_mask_get(va_block, id, NUMA_NO_NODE);
            UVM_ASSERT(residency_mask);

            migrate_pages = uvm_page_mask_andnot(&service_context->per_processor_masks[uvm_id_value(id)].new_residency,
                                                 &service_context->per_processor_masks[uvm_id_value(id)].new_residency,
                                                 residency_mask);

            if (!migrate_pages)
                uvm_processor_mask_clear(&service_context->resident_processors, id);
        }

        if (!uvm_processor_mask_empty(&service_context->resident_processors)) {
            while (first_page_index <= last_page_index) {
                uvm_page_index_t outer = last_page_index + 1;
                const uvm_va_policy_t *policy;

                if (uvm_va_block_is_hmm(va_block)) {
                    status = NV_ERR_INVALID_ADDRESS;
                    if (service_context->block_context->mm) {
                        status = uvm_hmm_find_policy_vma_and_outer(va_block,
                                                                   &service_context->block_context->hmm.vma,
                                                                   first_page_index,
                                                                   &policy,
                                                                   &outer);
                    }
                    if (status != NV_OK)
                        break;
                }

                service_context->region = uvm_va_block_region(first_page_index, outer);
                first_page_index = outer;

                status = uvm_va_block_service_locked(processor, va_block, va_block_retry, service_context);
                if (status != NV_OK)
                    break;
            }
        }
    }

    ++service_context->num_retries;

    return status;
}

static void reverse_mappings_to_va_block_page_mask(uvm_va_block_t *va_block,
                                                   const uvm_reverse_map_t *reverse_mappings,
                                                   size_t num_reverse_mappings,
                                                   uvm_page_mask_t *page_mask)
{
    NvU32 index;

    UVM_ASSERT(page_mask);

    if (num_reverse_mappings > 0)
        UVM_ASSERT(reverse_mappings);

    uvm_page_mask_zero(page_mask);

    // Populate the mask of accessed pages within the VA Block
    for (index = 0; index < num_reverse_mappings; ++index) {
        const uvm_reverse_map_t *reverse_map = &reverse_mappings[index];
        uvm_va_block_region_t region = reverse_map->region;

        UVM_ASSERT(reverse_map->va_block == va_block);

        // The VA Block could have been split since we obtained the reverse
        // mappings. Clamp the region to the current VA block size, to handle
        // the case in which it was split.
        region.outer = min(region.outer, (uvm_page_index_t)uvm_va_block_num_cpu_pages(va_block));
        region.first = min(region.first, region.outer);

        uvm_page_mask_region_fill(page_mask, region);
    }
}

static NV_STATUS service_phys_single_va_block(uvm_access_counter_service_batch_context_t *batch_context,
                                              const uvm_access_counter_buffer_entry_t *current_entry,
                                              const uvm_reverse_map_t *reverse_mappings,
                                              size_t num_reverse_mappings,
                                              NvU32 *out_flags)
{
    uvm_gpu_t *gpu = current_entry->gpu;
    size_t index;
    uvm_va_block_t *va_block = reverse_mappings[0].va_block;
    uvm_va_space_t *va_space = NULL;
    struct mm_struct *mm = NULL;
    NV_STATUS status = NV_OK;
    const uvm_processor_id_t processor = current_entry->counter_type == UVM_ACCESS_COUNTER_TYPE_MIMC?
                                             gpu->id: UVM_ID_CPU;

    *out_flags &= ~UVM_ACCESS_COUNTER_ACTION_CLEAR;

    UVM_ASSERT(num_reverse_mappings > 0);

    uvm_mutex_lock(&va_block->lock);
    va_space = uvm_va_block_get_va_space_maybe_dead(va_block);
    uvm_mutex_unlock(&va_block->lock);

    if (va_space) {
        uvm_va_block_retry_t va_block_retry;
        va_space_access_counters_info_t *va_space_access_counters;
        uvm_service_block_context_t *service_context = &batch_context->block_service_context;
        uvm_page_mask_t *accessed_pages = &batch_context->accessed_pages;

        // If an mm is registered with the VA space, we have to retain it
        // in order to lock it before locking the VA space.
        mm = uvm_va_space_mm_retain_lock(va_space);
        uvm_va_space_down_read(va_space);

        // Re-check that the VA block is valid after taking the VA block lock.
        if (uvm_va_block_is_dead(va_block))
            goto done;

        va_space_access_counters = va_space_access_counters_info_get(va_space);
        if (UVM_ID_IS_CPU(processor) && !atomic_read(&va_space_access_counters->params.enable_momc_migrations))
            goto done;

        if (!UVM_ID_IS_CPU(processor) && !atomic_read(&va_space_access_counters->params.enable_mimc_migrations))
            goto done;

        service_context->operation = UVM_SERVICE_OPERATION_ACCESS_COUNTERS;
        service_context->num_retries = 0;

        uvm_va_block_context_init(service_context->block_context, mm);

        if (uvm_va_block_is_hmm(va_block))
            uvm_hmm_migrate_begin_wait(va_block);

        uvm_mutex_lock(&va_block->lock);

        reverse_mappings_to_va_block_page_mask(va_block, reverse_mappings, num_reverse_mappings, accessed_pages);

        status = UVM_VA_BLOCK_RETRY_LOCKED(va_block,
                                           &va_block_retry,
                                           service_va_block_locked(processor,
                                                                   va_block,
                                                                   &va_block_retry,
                                                                   service_context,
                                                                   accessed_pages));

        uvm_mutex_unlock(&va_block->lock);

        if (uvm_va_block_is_hmm(va_block)) {
            uvm_hmm_migrate_finish(va_block);

            // If the pages could not be migrated, no need to try again,
            // this is best effort only.
            if (status == NV_WARN_MORE_PROCESSING_REQUIRED || status == NV_WARN_MISMATCHED_TARGET)
                status = NV_OK;
        }

        if (status == NV_OK)
            *out_flags |= UVM_ACCESS_COUNTER_ACTION_CLEAR;
    }

done:
    if (va_space) {
        uvm_va_space_up_read(va_space);
        uvm_va_space_mm_release_unlock(va_space, mm);
    }

    // Drop the refcounts taken by the reverse map translation routines
    for (index = 0; index < num_reverse_mappings; ++index)
        uvm_va_block_release(va_block);

    return status;
}

static NV_STATUS service_phys_va_blocks(uvm_access_counter_service_batch_context_t *batch_context,
                                        const uvm_access_counter_buffer_entry_t *current_entry,
                                        const uvm_reverse_map_t *reverse_mappings,
                                        size_t num_reverse_mappings,
                                        NvU32 *out_flags)
{
    NV_STATUS status = NV_OK;
    size_t index;

    *out_flags &= ~UVM_ACCESS_COUNTER_ACTION_CLEAR;

    for (index = 0; index < num_reverse_mappings; ++index) {
        NvU32 out_flags_local = 0;
        status = service_phys_single_va_block(batch_context,
                                              current_entry,
                                              reverse_mappings + index,
                                              1,
                                              &out_flags_local);
        if (status != NV_OK)
            break;

        UVM_ASSERT((out_flags_local & ~UVM_ACCESS_COUNTER_ACTION_CLEAR) == 0);
        *out_flags |= out_flags_local;
    }

    // In the case of failure, drop the refcounts for the remaining reverse mappings
    while (++index < num_reverse_mappings)
        uvm_va_block_release(reverse_mappings[index].va_block);

    return status;
}

// Iterate over all regions set in the given sub_granularity mask
#define for_each_sub_granularity_region(region_start, region_end, sub_granularity, num_regions)      \
    for ((region_start) = find_first_bit(&(sub_granularity), (num_regions)),                         \
         (region_end) = find_next_zero_bit(&(sub_granularity), (num_regions), (region_start) + 1);   \
         (region_start) < (num_regions);                                                             \
         (region_start) = find_next_bit(&(sub_granularity), (num_regions), (region_end) + 1),        \
         (region_end) = find_next_zero_bit(&(sub_granularity), (num_regions), (region_start) + 1))


static bool are_reverse_mappings_on_single_block(const uvm_reverse_map_t *reverse_mappings, size_t num_reverse_mappings)
{
    size_t index;
    uvm_va_block_t *prev_va_block = NULL;

    for (index = 0; index < num_reverse_mappings; ++index) {
        uvm_va_block_t *va_block = reverse_mappings[index].va_block;
        UVM_ASSERT(va_block);

        if (prev_va_block && prev_va_block != va_block)
            return false;

        prev_va_block = va_block;
    }

    return true;
}

// Service the given translation range. It will return the count of the reverse
// mappings found during servicing in num_reverse_mappings, even if the function
// doesn't return NV_OK.
static NV_STATUS service_phys_notification_translation(uvm_gpu_t *resident_gpu,
                                                       uvm_access_counter_service_batch_context_t *batch_context,
                                                       const uvm_gpu_access_counter_type_config_t *config,
                                                       const uvm_access_counter_buffer_entry_t *current_entry,
                                                       NvU64 address,
                                                       unsigned long sub_granularity,
                                                       size_t *num_reverse_mappings,
                                                       NvU32 *out_flags)
{
    uvm_gpu_t *gpu = current_entry->gpu;
    NV_STATUS status;
    NvU32 region_start, region_end;

    *num_reverse_mappings = 0;

    // Get the reverse_map translations for all the regions set in the
    // sub_granularity field of the counter.
    for_each_sub_granularity_region(region_start,
                                    region_end,
                                    sub_granularity,
                                    config->sub_granularity_regions_per_translation) {
        NvU64 local_address = address + region_start * config->sub_granularity_region_size;
        NvU32 local_translation_size = (region_end - region_start) * config->sub_granularity_region_size;
        uvm_reverse_map_t *local_reverse_mappings = batch_context->phys.translations + *num_reverse_mappings;

        // Obtain the virtual addresses of the pages within the reported
        // DMA range
        if (resident_gpu) {
            *num_reverse_mappings += uvm_pmm_gpu_phys_to_virt(&resident_gpu->pmm,
                                                              local_address,
                                                              local_translation_size,
                                                              local_reverse_mappings);
        }
        else {
            *num_reverse_mappings += uvm_pmm_sysmem_mappings_dma_to_virt(&gpu->pmm_reverse_sysmem_mappings,
                                                                         local_address,
                                                                         local_translation_size,
                                                                         local_reverse_mappings,
                                                                         local_translation_size / PAGE_SIZE);
        }
    }

    if (*num_reverse_mappings == 0)
        return NV_OK;

    // Service all the translations
    if (are_reverse_mappings_on_single_block(batch_context->phys.translations, *num_reverse_mappings)) {
        status = service_phys_single_va_block(batch_context,
                                              current_entry,
                                              batch_context->phys.translations,
                                              *num_reverse_mappings,
                                              out_flags);
    }
    else {
        status = service_phys_va_blocks(batch_context,
                                        current_entry,
                                        batch_context->phys.translations,
                                        *num_reverse_mappings,
                                        out_flags);
    }

    return status;
}

static NV_STATUS service_phys_notification(uvm_access_counter_service_batch_context_t *batch_context,
                                           uvm_access_counter_buffer_entry_t *current_entry)
{
    NvU64 address;
    NvU64 translation_index;
    uvm_gpu_t *gpu = current_entry->gpu;
    uvm_parent_gpu_t *parent_gpu = gpu->parent;
    uvm_access_counter_buffer_info_t *access_counters = &parent_gpu->access_counter_buffer_info;
    uvm_access_counter_type_t counter_type = current_entry->counter_type;
    const uvm_gpu_access_counter_type_config_t *config = get_config_for_type(access_counters, counter_type);
    unsigned long sub_granularity;
    size_t total_reverse_mappings = 0;
    uvm_gpu_t *resident_gpu = NULL;
    NV_STATUS status = NV_OK;
    NvU32 flags = 0;

    address = current_entry->address.address;
    UVM_ASSERT(address % config->translation_size == 0);
    sub_granularity = current_entry->sub_granularity;

    if (config->rm.granularity == UVM_ACCESS_COUNTER_GRANULARITY_64K)
        sub_granularity = 1;

    if (UVM_ID_IS_GPU(current_entry->physical_info.resident_id)) {
        resident_gpu = uvm_gpu_get(current_entry->physical_info.resident_id);
        UVM_ASSERT(resident_gpu != NULL);

        if (gpu != resident_gpu && uvm_gpus_are_nvswitch_connected(gpu, resident_gpu)) {
            UVM_ASSERT(address >= resident_gpu->parent->nvswitch_info.fabric_memory_window_start);
            address -= resident_gpu->parent->nvswitch_info.fabric_memory_window_start;
        }

        // On P9 systems, the CPU accesses the reserved heap on vidmem via
        // coherent NVLINK mappings. This can trigger notifications that
        // fall outside of the allocatable address range. We just drop
        // them.
        if (address >= resident_gpu->mem_info.max_allocatable_address)
            goto out;
    }

    for (translation_index = 0; translation_index < config->translations_per_counter; ++translation_index) {
        size_t num_reverse_mappings;
        NvU32 out_flags_local = 0;
        status = service_phys_notification_translation(resident_gpu,
                                                       batch_context,
                                                       config,
                                                       current_entry,
                                                       address,
                                                       sub_granularity,
                                                       &num_reverse_mappings,
                                                       &out_flags_local);
        total_reverse_mappings += num_reverse_mappings;

        UVM_ASSERT((out_flags_local & ~UVM_ACCESS_COUNTER_ACTION_CLEAR) == 0);
        flags |= out_flags_local;

        if (status != NV_OK)
            break;

        address += config->translation_size;
        sub_granularity = sub_granularity >> config->sub_granularity_regions_per_translation;
    }

    if (uvm_enable_builtin_tests)
        flags |= ((total_reverse_mappings != 0) ? UVM_ACCESS_COUNTER_PHYS_ON_MANAGED : 0);

out:
    notify_tools_broadcast_and_process_flags(parent_gpu, &current_entry, 1, flags);
    return status;
}

// TODO: Bug 2018899: Add statistics for dropped access counter notifications
static NV_STATUS service_phys_notifications(uvm_parent_gpu_t *parent_gpu,
                                            uvm_access_counter_service_batch_context_t *batch_context)
{
    NvU32 i;
    uvm_access_counter_buffer_entry_t **notifications = batch_context->phys.notifications;

    UVM_ASSERT(parent_gpu->access_counters_can_use_physical_addresses);

    preprocess_phys_notifications(batch_context);

    for (i = 0; i < batch_context->phys.num_notifications; ++i) {
        NV_STATUS status;
        uvm_access_counter_buffer_entry_t *current_entry = notifications[i];

        if (!UVM_ID_IS_VALID(current_entry->physical_info.resident_id))
            continue;

        status = service_phys_notification(batch_context, current_entry);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static NV_STATUS service_notification_va_block_helper(struct mm_struct *mm,
                                                      uvm_va_block_t *va_block,
                                                      uvm_processor_id_t processor,
                                                      uvm_access_counter_service_batch_context_t *batch_context)
{
    uvm_va_block_retry_t va_block_retry;
    uvm_page_mask_t *accessed_pages = &batch_context->accessed_pages;
    uvm_service_block_context_t *service_context = &batch_context->block_service_context;

    if (uvm_page_mask_empty(accessed_pages))
        return NV_OK;

    uvm_assert_mutex_locked(&va_block->lock);

    service_context->operation = UVM_SERVICE_OPERATION_ACCESS_COUNTERS;
    service_context->num_retries = 0;

    return UVM_VA_BLOCK_RETRY_LOCKED(va_block,
                                     &va_block_retry,
                                     service_va_block_locked(processor,
                                                             va_block,
                                                             &va_block_retry,
                                                             service_context,
                                                             accessed_pages));
}

static void expand_notification_block(uvm_gpu_va_space_t *gpu_va_space,
                                      uvm_va_block_t *va_block,
                                      uvm_va_block_context_t *va_block_context,
                                      uvm_page_mask_t *accessed_pages,
                                      const uvm_access_counter_buffer_entry_t *current_entry)
{
    NvU64 addr;
    NvU64 granularity = 0;
    uvm_gpu_t *resident_gpu = NULL;
    uvm_processor_id_t resident_id;
    uvm_page_index_t page_index;
    uvm_gpu_t *gpu = gpu_va_space->gpu;
    const uvm_access_counter_buffer_info_t *access_counters = &gpu->parent->access_counter_buffer_info;
    const uvm_gpu_access_counter_type_config_t *config = get_config_for_type(access_counters,
                                                                             UVM_ACCESS_COUNTER_TYPE_MIMC);

    config_granularity_to_bytes(config->rm.granularity, &granularity);

    // Granularities other than 2MB can only be enabled by UVM tests. Do nothing
    // in that case.
    if (granularity != UVM_PAGE_SIZE_2M)
        return;

    addr = current_entry->address.address;

    uvm_assert_rwsem_locked(&gpu_va_space->va_space->lock);
    uvm_assert_mutex_locked(&va_block->lock);

    page_index = uvm_va_block_cpu_page_index(va_block, addr);

    resident_id = uvm_va_block_page_get_closest_resident(va_block, va_block_context, page_index, gpu->id);

    // resident_id might be invalid or might already be the same as the GPU
    // which received the notification if the memory was already migrated before
    // acquiring the locks either during the servicing of previous notifications
    // or during faults or because of explicit migrations or if the VA range was
    // freed after receiving the notification. Return NV_OK in such cases.
    if (!UVM_ID_IS_VALID(resident_id) || uvm_id_equal(resident_id, gpu->id))
        return;

    if (UVM_ID_IS_GPU(resident_id))
        resident_gpu = uvm_va_space_get_gpu(gpu_va_space->va_space, resident_id);

    if (uvm_va_block_get_physical_size(va_block, resident_id, page_index) != granularity) {
        uvm_page_mask_set(accessed_pages, page_index);
    }
    else {
        NvU32 region_start;
        NvU32 region_end;
        unsigned long sub_granularity = current_entry->sub_granularity;
        NvU32 num_regions = config->sub_granularity_regions_per_translation;
        NvU32 num_sub_pages = config->sub_granularity_region_size / PAGE_SIZE;
        uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(va_block, resident_id, NUMA_NO_NODE);

        UVM_ASSERT(num_sub_pages >= 1);

        // region_start and region_end refer to sub_granularity indices, not
        // page_indices.
        for_each_sub_granularity_region(region_start, region_end, sub_granularity, num_regions) {
            uvm_page_mask_region_fill(accessed_pages,
                                      uvm_va_block_region(region_start * num_sub_pages,
                                                          region_end * num_sub_pages));
        }

        // Remove pages in the va_block which are not resident on resident_id.
        // If the GPU is heavily accessing those pages, future access counter
        // migrations will migrate them to the GPU.
        uvm_page_mask_and(accessed_pages, accessed_pages, resident_mask);
    }
}

static NV_STATUS service_virt_notifications_in_block(uvm_gpu_va_space_t *gpu_va_space,
                                                     struct mm_struct *mm,
                                                     uvm_va_block_t *va_block,
                                                     uvm_access_counter_service_batch_context_t *batch_context,
                                                     NvU32 index,
                                                     NvU32 *out_index)
{
    NvU32 i;
    NvU32 flags = 0;
    NV_STATUS status = NV_OK;
    NV_STATUS flags_status;
    uvm_gpu_t *gpu = gpu_va_space->gpu;
    uvm_va_space_t *va_space = gpu_va_space->va_space;
    uvm_page_mask_t *accessed_pages = &batch_context->accessed_pages;
    uvm_access_counter_buffer_entry_t **notifications = batch_context->virt.notifications;
    uvm_service_block_context_t *service_context = &batch_context->block_service_context;

    UVM_ASSERT(va_block);
    UVM_ASSERT(index < batch_context->virt.num_notifications);

    uvm_assert_rwsem_locked(&va_space->lock);

    uvm_page_mask_zero(accessed_pages);

    uvm_va_block_context_init(service_context->block_context, mm);

    uvm_mutex_lock(&va_block->lock);

    for (i = index; i < batch_context->virt.num_notifications; i++) {
        uvm_access_counter_buffer_entry_t *current_entry = notifications[i];
        NvU64 address = current_entry->address.address;

        if (current_entry->virtual_info.va_space != va_space || current_entry->gpu != gpu || address > va_block->end)
            break;

        expand_notification_block(gpu_va_space,
                                  va_block,
                                  batch_context->block_service_context.block_context,
                                  accessed_pages,
                                  current_entry);
    }

    *out_index = i;

    // Atleast one notification should have been processed.
    UVM_ASSERT(index < *out_index);

    status = service_notification_va_block_helper(mm, va_block, gpu->id, batch_context);

    uvm_mutex_unlock(&va_block->lock);

    if (status == NV_OK)
        flags |= UVM_ACCESS_COUNTER_ACTION_CLEAR;

    flags_status = notify_tools_and_process_flags(va_space, gpu, &notifications[index], *out_index - index, flags);

    if ((status == NV_OK) && (flags_status != NV_OK))
        status = flags_status;

    return status;
}

static NV_STATUS service_virt_notification_ats(uvm_gpu_va_space_t *gpu_va_space,
                                               struct mm_struct *mm,
                                               uvm_access_counter_service_batch_context_t *batch_context,
                                               NvU32 index,
                                               NvU32 *out_index)
{

    NvU32 i;
    NvU64 base;
    NvU64 end;
    NvU64 address;
    NvU32 flags = UVM_ACCESS_COUNTER_ACTION_CLEAR;
    NV_STATUS status = NV_OK;
    NV_STATUS flags_status;
    struct vm_area_struct *vma = NULL;
    uvm_gpu_t *gpu = gpu_va_space->gpu;
    uvm_va_space_t *va_space = gpu_va_space->va_space;
    uvm_ats_fault_context_t *ats_context = &batch_context->ats_context;
    uvm_access_counter_buffer_entry_t **notifications = batch_context->virt.notifications;

    UVM_ASSERT(index < batch_context->virt.num_notifications);

    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_space->lock);

    address = notifications[index]->address.address;

    vma = find_vma_intersection(mm, address, address + 1);
    if (!vma) {
        // Clear the notification entry to continue receiving access counter
        // notifications when a new VMA is allocated in this range.
        status = notify_tools_and_process_flags(va_space, gpu, &notifications[index], 1, flags);
        *out_index = index + 1;
        return status;
    }

    base = UVM_VA_BLOCK_ALIGN_DOWN(address);
    end = min(base + UVM_VA_BLOCK_SIZE, (NvU64)vma->vm_end);

    uvm_page_mask_zero(&ats_context->accessed_mask);

    for (i = index; i < batch_context->virt.num_notifications; i++) {
        uvm_access_counter_buffer_entry_t *current_entry = notifications[i];
        address = current_entry->address.address;

        if (current_entry->virtual_info.va_space != va_space || current_entry->gpu != gpu || address >= end)
            break;

        uvm_page_mask_set(&ats_context->accessed_mask, (address - base) / PAGE_SIZE);
    }

    *out_index = i;

    // Atleast one notification should have been processed.
    UVM_ASSERT(index < *out_index);

    // TODO: Bug 2113632: [UVM] Don't clear access counters when the preferred
    //                    location is set
    // If no pages were actually migrated, don't clear the access counters.
    status = uvm_ats_service_access_counters(gpu_va_space, vma, base, ats_context);
    if (status != NV_OK)
        flags &= ~UVM_ACCESS_COUNTER_ACTION_CLEAR;

    flags_status = notify_tools_and_process_flags(va_space, gpu, &notifications[index], *out_index - index, flags);
    if ((status == NV_OK) && (flags_status != NV_OK))
        status = flags_status;

    return status;
}

static NV_STATUS service_virt_notifications_batch(uvm_gpu_va_space_t *gpu_va_space,
                                                  struct mm_struct *mm,
                                                  uvm_access_counter_service_batch_context_t *batch_context,
                                                  NvU32 index,
                                                  NvU32 *out_index)
{
    NV_STATUS status;
    uvm_va_range_t *va_range;
    uvm_va_space_t *va_space = gpu_va_space->va_space;
    uvm_access_counter_buffer_entry_t *current_entry = batch_context->virt.notifications[index];
    NvU64 address = current_entry->address.address;

    UVM_ASSERT(va_space);

    if (mm)
        uvm_assert_mmap_lock_locked(mm);

    uvm_assert_rwsem_locked(&va_space->lock);

    // Virtual address notifications are always 64K aligned
    UVM_ASSERT(IS_ALIGNED(address, UVM_PAGE_SIZE_64K));

    va_range = uvm_va_range_find(va_space, address);
    if (va_range) {
        // Avoid clearing the entry by default.
        NvU32 flags = 0;
        uvm_va_block_t *va_block = NULL;

        if (va_range->type == UVM_VA_RANGE_TYPE_MANAGED) {
            size_t index = uvm_va_range_block_index(va_range, address);

            va_block = uvm_va_range_block(va_range, index);

            // If the va_range is a managed range, the notification belongs to a
            // recently freed va_range if va_block is NULL. If va_block is not
            // NULL, service_virt_notifications_in_block will process flags.
            // Clear the notification entry to continue receiving notifications
            // when a new va_range is allocated in that region.
            flags = UVM_ACCESS_COUNTER_ACTION_CLEAR;
        }

        if (va_block) {
            status = service_virt_notifications_in_block(gpu_va_space, mm, va_block, batch_context, index, out_index);
        }
        else {
            status = notify_tools_and_process_flags(va_space, gpu_va_space->gpu, batch_context->virt.notifications, 1, flags);
            *out_index = index + 1;
        }
    }
    else if (uvm_ats_can_service_faults(gpu_va_space, mm)) {
        status = service_virt_notification_ats(gpu_va_space, mm, batch_context, index, out_index);
    }
    else {
        NvU32 flags;
        uvm_va_block_t *va_block = NULL;

        status = uvm_hmm_va_block_find(va_space, address, &va_block);

        // TODO: Bug 4309292: [UVM][HMM] Re-enable access counter HMM block
        //                    migrations for virtual notifications
        //
        // - If the va_block is HMM, don't clear the notification since HMM
        // migrations are currently disabled.
        //
        // - If the va_block isn't HMM, the notification belongs to a recently
        // freed va_range. Clear the notification entry to continue receiving
        // notifications when a new va_range is allocated in this region.
        flags = va_block ? 0 : UVM_ACCESS_COUNTER_ACTION_CLEAR;

        UVM_ASSERT((status == NV_ERR_OBJECT_NOT_FOUND) ||
                   (status == NV_ERR_INVALID_ADDRESS)  ||
                   uvm_va_block_is_hmm(va_block));

        // Clobber status to continue processing the rest of the notifications
        // in the batch.
        status = notify_tools_and_process_flags(va_space,
                                                gpu_va_space->gpu,
                                                batch_context->virt.notifications,
                                                1,
                                                flags);

        *out_index = index + 1;
    }

    return status;
}

static NV_STATUS service_virt_notifications(uvm_parent_gpu_t *parent_gpu,
                                            uvm_access_counter_service_batch_context_t *batch_context)
{
    NvU32 i = 0;
    NV_STATUS status = NV_OK;
    struct mm_struct *mm = NULL;
    uvm_va_space_t *va_space = NULL;
    uvm_va_space_t *prev_va_space = NULL;
    uvm_gpu_t *prev_gpu = NULL;
    uvm_gpu_va_space_t *gpu_va_space = NULL;

    // TODO: Bug 4299018 : Add support for virtual access counter migrations on
    //                     4K page sizes.
    if (PAGE_SIZE == UVM_PAGE_SIZE_4K) {
        return notify_tools_broadcast_and_process_flags(parent_gpu,
                                                        batch_context->virt.notifications,
                                                        batch_context->virt.num_notifications,
                                                        0);
    }

    preprocess_virt_notifications(parent_gpu, batch_context);

    while (i < batch_context->virt.num_notifications) {
        uvm_access_counter_buffer_entry_t *current_entry = batch_context->virt.notifications[i];
        va_space = current_entry->virtual_info.va_space;

        if (va_space != prev_va_space) {

            // New va_space detected, drop locks of the old va_space.
            if (prev_va_space) {
                uvm_va_space_up_read(prev_va_space);
                uvm_va_space_mm_release_unlock(prev_va_space, mm);

                mm = NULL;
                prev_gpu = NULL;
            }

            // Acquire locks for the new va_space.
            if (va_space) {
                mm = uvm_va_space_mm_retain_lock(va_space);
                uvm_va_space_down_read(va_space);
            }

            prev_va_space = va_space;
        }

        if (va_space) {
            if (prev_gpu != current_entry->gpu) {
                prev_gpu = current_entry->gpu;
                gpu_va_space = uvm_gpu_va_space_get(va_space, current_entry->gpu);
            }

            if (gpu_va_space && uvm_va_space_has_access_counter_migrations(va_space)) {
                status = service_virt_notifications_batch(gpu_va_space, mm, batch_context, i, &i);
            }
            else {
                status = notify_tools_and_process_flags(va_space,
                                                        current_entry->gpu,
                                                        &batch_context->virt.notifications[i],
                                                        1,
                                                        0);
                i++;
            }
        }
        else {
            status = notify_tools_broadcast_and_process_flags(parent_gpu, &batch_context->virt.notifications[i], 1, 0);
            i++;
        }

        if (status != NV_OK)
            break;
    }

    if (va_space) {
        uvm_va_space_up_read(va_space);
        uvm_va_space_mm_release_unlock(va_space, mm);
    }

    return status;
}

void uvm_parent_gpu_service_access_counters(uvm_parent_gpu_t *parent_gpu)
{
    NV_STATUS status = NV_OK;
    uvm_access_counter_service_batch_context_t *batch_context = &parent_gpu->access_counter_buffer_info.batch_service_context;

    UVM_ASSERT(parent_gpu->access_counters_supported);

    if (parent_gpu->access_counter_buffer_info.notifications_ignored_count > 0)
        return;

    while (1) {
        batch_context->num_cached_notifications = fetch_access_counter_buffer_entries(parent_gpu,
                                                                                      batch_context,
                                                                                      NOTIFICATION_FETCH_MODE_BATCH_READY);
        if (batch_context->num_cached_notifications == 0)
            break;

        ++batch_context->batch_id;

        if (batch_context->virt.num_notifications) {
            status = service_virt_notifications(parent_gpu, batch_context);
            if (status != NV_OK)
                break;
        }

        if (batch_context->phys.num_notifications) {
            status = service_phys_notifications(parent_gpu, batch_context);
            if (status != NV_OK)
                break;
        }
    }

    if (status != NV_OK) {
        UVM_DBG_PRINT("Error %s servicing access counter notifications on GPU: %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));
    }
}

static const NvU32 g_uvm_access_counters_threshold_max = (1 << 15) - 1;

static NV_STATUS access_counters_config_from_test_params(const UVM_TEST_RECONFIGURE_ACCESS_COUNTERS_PARAMS *params,
                                                         UvmGpuAccessCntrConfig *config)
{
    NvU64 tracking_size;
    memset(config, 0, sizeof(*config));

    if (params->threshold == 0 || params->threshold > g_uvm_access_counters_threshold_max)
        return NV_ERR_INVALID_ARGUMENT;

    if (config_granularity_to_bytes(params->mimc_granularity, &tracking_size) != NV_OK)
        return NV_ERR_INVALID_ARGUMENT;

    if (config_granularity_to_bytes(params->momc_granularity, &tracking_size) != NV_OK)
        return NV_ERR_INVALID_ARGUMENT;

    // Since values for granularity/use limit are shared between tests and
    // nv_uvm_types.h, the value will be checked in the call to
    // nvUvmInterfaceEnableAccessCntr
    config->mimcGranularity = params->mimc_granularity;
    config->momcGranularity = params->momc_granularity;

    config->mimcUseLimit = params->mimc_use_limit;
    config->momcUseLimit = params->momc_use_limit;

    config->threshold = params->threshold;

    return NV_OK;
}

bool uvm_va_space_has_access_counter_migrations(uvm_va_space_t *va_space)
{
    va_space_access_counters_info_t *va_space_access_counters = va_space_access_counters_info_get(va_space);

    return atomic_read(&va_space_access_counters->params.enable_mimc_migrations);
}

NV_STATUS uvm_perf_access_counters_init(void)
{
    uvm_perf_module_init("perf_access_counters",
                         UVM_PERF_MODULE_TYPE_ACCESS_COUNTERS,
                         g_callbacks_access_counters,
                         ARRAY_SIZE(g_callbacks_access_counters),
                         &g_module_access_counters);

    return NV_OK;
}

void uvm_perf_access_counters_exit(void)
{
}

NV_STATUS uvm_perf_access_counters_load(uvm_va_space_t *va_space)
{
    va_space_access_counters_info_t *va_space_access_counters;
    NV_STATUS status;

    status = uvm_perf_module_load(&g_module_access_counters, va_space);
    if (status != NV_OK)
        return status;

    va_space_access_counters = va_space_access_counters_info_create(va_space);
    if (!va_space_access_counters)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

void uvm_perf_access_counters_unload(uvm_va_space_t *va_space)
{
    uvm_perf_module_unload(&g_module_access_counters, va_space);

    va_space_access_counters_info_destroy(va_space);
}

NV_STATUS uvm_test_access_counters_enabled_by_default(UVM_TEST_ACCESS_COUNTERS_ENABLED_BY_DEFAULT_PARAMS *params,
                                                      struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_gpu_t *gpu = NULL;

    gpu = uvm_va_space_retain_gpu_by_uuid(va_space, &params->gpu_uuid);
    if (!gpu)
        return NV_ERR_INVALID_DEVICE;

    params->enabled = uvm_parent_gpu_access_counters_required(gpu->parent);

    uvm_gpu_release(gpu);

    return NV_OK;
}

NV_STATUS uvm_test_reconfigure_access_counters(UVM_TEST_RECONFIGURE_ACCESS_COUNTERS_PARAMS *params, struct file *filp)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu = NULL;
    UvmGpuAccessCntrConfig config = {0};
    va_space_access_counters_info_t *va_space_access_counters;
    uvm_va_space_t *va_space_reconfiguration_owner;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    status = access_counters_config_from_test_params(params, &config);
    if (status != NV_OK)
        return status;

    gpu = uvm_va_space_retain_gpu_by_uuid(va_space, &params->gpu_uuid);
    if (!gpu)
        return NV_ERR_INVALID_DEVICE;

    if (!gpu->parent->access_counters_supported) {
        status = NV_ERR_NOT_SUPPORTED;
        goto exit_release_gpu;
    }

    // ISR lock ensures that we own GET/PUT registers. It disables interrupts
    // and ensures that no other thread (nor the top half) will be able to
    // re-enable interrupts during reconfiguration.
    uvm_parent_gpu_access_counters_isr_lock(gpu->parent);

    uvm_va_space_down_read_rm(va_space);

    if (!uvm_processor_mask_test(&va_space->registered_gpus, gpu->id)) {
        status = NV_ERR_INVALID_STATE;
        goto exit_isr_unlock;
    }

    // Unregistration already started. Fail to avoid an interleaving in which
    // access counters end up been enabled on an unregistered GPU:
    // (thread 0) uvm_va_space_unregister_gpu disables access counters
    // (thread 1) assuming no VA space lock is held yet by the unregistration,
    //            this function enables access counters and runs to completion,
    //            returning NV_OK
    // (thread 0) uvm_va_space_unregister_gpu takes the VA space lock and
    //            completes the unregistration
    if (uvm_processor_mask_test(&va_space->gpu_unregister_in_progress, gpu->id)) {
        status = NV_ERR_INVALID_STATE;
        goto exit_isr_unlock;
    }

    va_space_access_counters = va_space_access_counters_info_get(va_space);

    va_space_reconfiguration_owner = gpu->parent->access_counter_buffer_info.reconfiguration_owner;

    // If any other VA space has reconfigured access counters on this GPU,
    // return error to avoid overwriting its configuration.
    if (va_space_reconfiguration_owner && (va_space_reconfiguration_owner != va_space)) {
        status = NV_ERR_INVALID_STATE;
        goto exit_isr_unlock;
    }

    if (!uvm_parent_processor_mask_test(&va_space->access_counters_enabled_processors, gpu->parent->id)) {
        status = gpu_access_counters_enable(gpu, &config);

        if (status == NV_OK)
            uvm_parent_processor_mask_set_atomic(&va_space->access_counters_enabled_processors, gpu->parent->id);
        else
            goto exit_isr_unlock;
    }

    UVM_ASSERT(gpu->parent->isr.access_counters.handling_ref_count > 0);

    // Disable counters, and renable with the new configuration.
    // Note that we are yielding ownership even when the access counters are
    // enabled in at least gpu. This inconsistent state is not visible to other
    // threads or VA spaces because of the ISR lock, and it is immediately
    // rectified by retaking ownership.
    access_counters_yield_ownership(gpu->parent);
    status = access_counters_take_ownership(gpu, &config);

    // Retaking ownership failed, so RM owns the interrupt.
    if (status != NV_OK) {
        // The state of any other VA space with access counters enabled is
        // corrupt
        // TODO: Bug 2419290: Fail reconfiguration if access
        // counters are enabled on a different VA space.
        if (gpu->parent->isr.access_counters.handling_ref_count > 1) {
            UVM_ASSERT_MSG(status == NV_OK,
                           "Access counters interrupt still owned by RM, other VA spaces may experience failures");
        }

        uvm_parent_processor_mask_clear_atomic(&va_space->access_counters_enabled_processors, gpu->parent->id);
        parent_gpu_access_counters_disable(gpu->parent);
        goto exit_isr_unlock;
    }

    gpu->parent->access_counter_buffer_info.reconfiguration_owner = va_space;

    uvm_va_space_up_read_rm(va_space);
    uvm_va_space_down_write(va_space);
    atomic_set(&va_space_access_counters->params.enable_mimc_migrations, !!params->enable_mimc_migrations);
    atomic_set(&va_space_access_counters->params.enable_momc_migrations, !!params->enable_momc_migrations);
    uvm_va_space_up_write(va_space);

exit_isr_unlock:
    if (status != NV_OK)
        uvm_va_space_up_read_rm(va_space);

    uvm_parent_gpu_access_counters_isr_unlock(gpu->parent);

exit_release_gpu:
    uvm_gpu_release(gpu);

    return status;
}

NV_STATUS uvm_test_reset_access_counters(UVM_TEST_RESET_ACCESS_COUNTERS_PARAMS *params, struct file *filp)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu = NULL;
    uvm_access_counter_buffer_info_t *access_counters;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    if (params->mode >= UVM_TEST_ACCESS_COUNTER_RESET_MODE_MAX)
        return NV_ERR_INVALID_ARGUMENT;

    if (params->mode == UVM_TEST_ACCESS_COUNTER_RESET_MODE_TARGETED &&
        params->counter_type >= UVM_TEST_ACCESS_COUNTER_TYPE_MAX) {
        return NV_ERR_INVALID_ARGUMENT;
    }

    gpu = uvm_va_space_retain_gpu_by_uuid(va_space, &params->gpu_uuid);
    if (!gpu)
        return NV_ERR_INVALID_DEVICE;

    if (!gpu->parent->access_counters_supported) {
        status = NV_ERR_NOT_SUPPORTED;
        goto exit_release_gpu;
    }

    uvm_parent_gpu_access_counters_isr_lock(gpu->parent);

    // Access counters not enabled. Nothing to reset
    if (gpu->parent->isr.access_counters.handling_ref_count == 0)
        goto exit_isr_unlock;

    access_counters = &gpu->parent->access_counter_buffer_info;

    if (params->mode == UVM_TEST_ACCESS_COUNTER_RESET_MODE_ALL) {
        status = access_counter_clear_all(gpu);
    }
    else {
        uvm_access_counter_buffer_entry_t entry = { 0 };
        uvm_access_counter_buffer_entry_t *notification = &entry;

        if (params->counter_type == UVM_TEST_ACCESS_COUNTER_TYPE_MIMC)
            entry.counter_type = UVM_ACCESS_COUNTER_TYPE_MIMC;
        else
            entry.counter_type = UVM_ACCESS_COUNTER_TYPE_MOMC;

        entry.bank = params->bank;
        entry.tag = params->tag;

        status = access_counter_clear_notifications(gpu, &notification, 1);
    }

    if (status == NV_OK)
        status = uvm_tracker_wait(&access_counters->clear_tracker);

exit_isr_unlock:
    uvm_parent_gpu_access_counters_isr_unlock(gpu->parent);

exit_release_gpu:
    uvm_gpu_release(gpu);

    return status;
}

void uvm_parent_gpu_access_counters_set_ignore(uvm_parent_gpu_t *parent_gpu, bool do_ignore)
{
    bool change_intr_state = false;

    if (!parent_gpu->access_counters_supported)
        return;

    uvm_parent_gpu_access_counters_isr_lock(parent_gpu);

    if (do_ignore) {
        if (parent_gpu->access_counter_buffer_info.notifications_ignored_count++ == 0)
            change_intr_state = true;
    }
    else {
        UVM_ASSERT(parent_gpu->access_counter_buffer_info.notifications_ignored_count >= 1);
        if (--parent_gpu->access_counter_buffer_info.notifications_ignored_count == 0)
            change_intr_state = true;
    }

    if (change_intr_state) {
        // We need to avoid an interrupt storm while ignoring notifications. We
        // just disable the interrupt.
        uvm_spin_lock_irqsave(&parent_gpu->isr.interrupts_lock);

        if (do_ignore)
            uvm_parent_gpu_access_counters_intr_disable(parent_gpu);
        else
            uvm_parent_gpu_access_counters_intr_enable(parent_gpu);

        uvm_spin_unlock_irqrestore(&parent_gpu->isr.interrupts_lock);

        if (!do_ignore)
            access_counter_buffer_flush_locked(parent_gpu, UVM_GPU_BUFFER_FLUSH_MODE_CACHED_PUT);
    }

    uvm_parent_gpu_access_counters_isr_unlock(parent_gpu);
}

NV_STATUS uvm_test_set_ignore_access_counters(UVM_TEST_SET_IGNORE_ACCESS_COUNTERS_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu = NULL;

    gpu = uvm_va_space_retain_gpu_by_uuid(va_space, &params->gpu_uuid);
    if (!gpu)
        return NV_ERR_INVALID_DEVICE;

    if (gpu->parent->access_counters_supported)
        uvm_parent_gpu_access_counters_set_ignore(gpu->parent, params->ignore);
    else
        status = NV_ERR_NOT_SUPPORTED;

    uvm_gpu_release(gpu);
    return status;
}
