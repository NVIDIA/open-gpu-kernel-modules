/*******************************************************************************
    Copyright (c) 2025-2026 NVIDIA Corporation

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


#include "uvm_perf_dynamic_threshold.h"

#include "uvm_api.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_gpu_access_counters.h"
#include "uvm_kvmalloc.h"
#include "uvm_perf_events.h"
#include "uvm_pmm_gpu.h"
#include "uvm_test.h"
#include "uvm_va_space.h"

// Target migration-rate range in MiB/s.
#define UVM_PERF_DYNAMIC_THRESHOLD_MIN_RATE_MIB_PER_SEC_DEFAULT     10U
#define UVM_PERF_DYNAMIC_THRESHOLD_MAX_RATE_MIB_PER_SEC_DEFAULT     1000U

// Largest accepted target rate, 1 TiB/s, above any interconnect.
#define UVM_PERF_DYNAMIC_THRESHOLD_MAX_RATE_MIB_PER_SEC_LIMIT       (1U << 20)

// Fixed PID controller gains in hundredths (20 means 0.20). The PID output
// divides by 100.
#define UVM_PERF_DYNAMIC_THRESHOLD_PID_KP                           20U
#define UVM_PERF_DYNAMIC_THRESHOLD_PID_KI                           5U
#define UVM_PERF_DYNAMIC_THRESHOLD_PID_KD                           10U

// Limit accumulated error to prevent integral windup.
#define UVM_PERF_DYNAMIC_THRESHOLD_PID_INTEGRAL_ERROR_LIMIT         10000LL

// Bound PID inputs and output to avoid overflow and large threshold changes.
#define UVM_PERF_DYNAMIC_THRESHOLD_PID_ERROR_LIMIT                  (1LL << 29)
#define UVM_PERF_DYNAMIC_THRESHOLD_PID_ADJUSTMENT_LIMIT             8192

// Lowest threshold the controller sets, unless a lower one was configured.
// It is below the default threshold on purpose: when migration is almost
// idle under memory pressure, the controller admits pages that the default
// threshold would leave remote, and 192 bounds that undershoot to 25%.
#define UVM_PERF_DYNAMIC_THRESHOLD_FLOOR                            192U

// Maximum threshold supported by the 16-bit hardware field.
#define UVM_PERF_DYNAMIC_THRESHOLD_MAX                              ((1U << 16) - 1)

static int uvm_perf_dynamic_threshold_enable = 1;
module_param(uvm_perf_dynamic_threshold_enable, int, S_IRUGO);
MODULE_PARM_DESC(uvm_perf_dynamic_threshold_enable,
                 "Enable Dynamic Migration Threshold (default: 1)");

static unsigned uvm_perf_dynamic_threshold_min_rate_mib_per_sec =
    UVM_PERF_DYNAMIC_THRESHOLD_MIN_RATE_MIB_PER_SEC_DEFAULT;
module_param(uvm_perf_dynamic_threshold_min_rate_mib_per_sec, uint, S_IRUGO);
MODULE_PARM_DESC(uvm_perf_dynamic_threshold_min_rate_mib_per_sec,
                 "Lower bound of the target migration-rate band in MiB/s (default: 10)");

static unsigned uvm_perf_dynamic_threshold_max_rate_mib_per_sec =
    UVM_PERF_DYNAMIC_THRESHOLD_MAX_RATE_MIB_PER_SEC_DEFAULT;
module_param(uvm_perf_dynamic_threshold_max_rate_mib_per_sec, uint, S_IRUGO);
MODULE_PARM_DESC(uvm_perf_dynamic_threshold_max_rate_mib_per_sec,
                 "Upper bound of the target migration-rate band in MiB/s (default: 1000)");

static int uvm_perf_dynamic_threshold_log_adjustments;
module_param(uvm_perf_dynamic_threshold_log_adjustments, int, S_IRUGO);
MODULE_PARM_DESC(uvm_perf_dynamic_threshold_log_adjustments,
                 "Log Dynamic Migration Threshold adjustments (default: 0)");

typedef struct
{
    NvS64 error;
    NvS64 integral_error;
    NvS64 proportional;
    NvS64 integral;
    NvS64 derivative;
    int adjustment;
    NvU32 threshold;
} dynamic_threshold_pid_result_t;

struct uvm_perf_dynamic_threshold_struct
{
    // GPU whose access counter threshold is adjusted.
    uvm_gpu_t *gpu;

    // Samples the migration rate once per second during an episode.
    struct delayed_work work;

    // Written by migration callbacks. An episode starts when an access counter
    // migration follows an eviction while HBM is under pressure.
    atomic64_t migration_bytes;
    atomic64_t eviction_count;
    atomic64_t eviction_bytes;
    atomic_t evicted;
    atomic_t active;

    // Owned by the work item. The callback that starts an episode also sets
    // last_sample_jiffies, before the work item is scheduled.
    unsigned long last_sample_jiffies;
    NvU32 initial_threshold;
    NvU32 threshold;
    NvS64 integral_error;
    NvS64 previous_error;
};

// Return free HBM reported by PMA.
static NvU64 gpu_free_memory_bytes(uvm_gpu_t *gpu)
{
    if (!gpu->pmm.pma_stats)
        return 0;

    return min(READ_ONCE(gpu->pmm.pma_stats->numFreePages64k) * UVM_PAGE_SIZE_64K, gpu->mem_info.size);
}

// Return whether HBM has recovered from pressure.
static bool gpu_memory_pressure_recovered(uvm_gpu_t *gpu)
{
    if (!gpu->pmm.pma_stats || gpu->mem_info.size == 0)
        return true;

    return gpu_free_memory_bytes(gpu) >= gpu->mem_info.size / 4;
}

// Calculate the threshold adjustment for the measured migration rate.
static void dynamic_threshold_pid(const uvm_perf_dynamic_threshold_t *dt,
                                  NvU64 rate_mib_per_sec,
                                  NvU64 elapsed_ms,
                                  dynamic_threshold_pid_result_t *result)
{
    NvU32 min_threshold = min(dt->initial_threshold, UVM_PERF_DYNAMIC_THRESHOLD_FLOOR);
    NvS64 dt_seconds = max_t(NvS64, elapsed_ms / MSEC_PER_SEC, 1);
    NvS64 output;

    memset(result, 0, sizeof(*result));
    result->threshold = dt->threshold;

    // The error is the distance from the target band, positive above it and
    // negative below it.
    if (rate_mib_per_sec > uvm_perf_dynamic_threshold_max_rate_mib_per_sec) {
        result->error = min_t(NvU64,
                              rate_mib_per_sec - uvm_perf_dynamic_threshold_max_rate_mib_per_sec,
                              UVM_PERF_DYNAMIC_THRESHOLD_PID_ERROR_LIMIT);
    }
    else if (rate_mib_per_sec < uvm_perf_dynamic_threshold_min_rate_mib_per_sec) {
        result->error = -(NvS64)min_t(NvU64,
                                      (NvU64)uvm_perf_dynamic_threshold_min_rate_mib_per_sec - rate_mib_per_sec,
                                      UVM_PERF_DYNAMIC_THRESHOLD_PID_ERROR_LIMIT);
    }
    else {
        // Inside the band: hold the threshold and forget the history.
        return;
    }

    // A sign change means the migration rate jumped over the entire target
    // band (the error is zero inside the band), from too high to too low or
    // vice versa, after an application phase change or a large overshoot.
    // Clear the integral so the old error does not wind up the threshold the
    // wrong way.
    result->integral_error = dt->integral_error;
    if ((result->error > 0 && dt->previous_error < 0) || (result->error < 0 && dt->previous_error > 0))
        result->integral_error = 0;

    // Integral windup happens when the threshold is already at its minimum or
    // maximum and the error persists. The integral keeps growing although no
    // adjustment can correct the error, and later overshoots while it unwinds.
    // To prevent integral windup, only accumulate while the threshold can
    // still move in the error's direction.
    if ((result->error > 0 && dt->threshold < UVM_PERF_DYNAMIC_THRESHOLD_MAX) ||
        (result->error < 0 && dt->threshold > min_threshold)) {
        result->integral_error = clamp_t(NvS64,
                                         result->integral_error + result->error * dt_seconds,
                                         -UVM_PERF_DYNAMIC_THRESHOLD_PID_INTEGRAL_ERROR_LIMIT,
                                         UVM_PERF_DYNAMIC_THRESHOLD_PID_INTEGRAL_ERROR_LIMIT);
    }

    // Combine the terms, then bound the step and the resulting threshold.
    result->proportional = result->error * UVM_PERF_DYNAMIC_THRESHOLD_PID_KP;
    result->integral = result->integral_error * UVM_PERF_DYNAMIC_THRESHOLD_PID_KI;
    result->derivative = ((result->error - dt->previous_error) / dt_seconds) * UVM_PERF_DYNAMIC_THRESHOLD_PID_KD;

    // The gains are in hundredths, so divide by 100.
    output = (result->proportional + result->integral + result->derivative) / 100;
    result->adjustment = (int)clamp_t(NvS64,
                                      output,
                                      -UVM_PERF_DYNAMIC_THRESHOLD_PID_ADJUSTMENT_LIMIT,
                                      UVM_PERF_DYNAMIC_THRESHOLD_PID_ADJUSTMENT_LIMIT);
    result->threshold = clamp_t(NvS64,
                                (NvS64)dt->threshold + result->adjustment,
                                min_threshold,
                                UVM_PERF_DYNAMIC_THRESHOLD_MAX);
}

// End the episode and forget its history. Clearing active last lets the next
// eviction under pressure start a new episode.
static void dynamic_threshold_stop(uvm_perf_dynamic_threshold_t *dt)
{
    dt->threshold = dt->initial_threshold;
    dt->integral_error = 0;
    dt->previous_error = 0;
    atomic64_set(&dt->migration_bytes, 0);
    atomic64_set(&dt->eviction_count, 0);
    atomic64_set(&dt->eviction_bytes, 0);
    atomic_set(&dt->evicted, 0);
    atomic_set(&dt->active, 0);
}

// Sample the migration rate and apply the next threshold. Restore the
// configured threshold and end the episode once HBM has recovered.
static void dynamic_threshold_work(struct work_struct *work)
{
    uvm_perf_dynamic_threshold_t *dt = container_of(to_delayed_work(work), uvm_perf_dynamic_threshold_t, work);
    dynamic_threshold_pid_result_t result = {};
    unsigned long now = jiffies;
    NvU64 elapsed_ms = max(jiffies_to_msecs(now - dt->last_sample_jiffies), 1U);
    NvU64 bytes = 0;
    NvU64 rate = 0;
    bool recovered;
    NV_STATUS status;

    // Skip this sample while the system is suspending or resuming.
    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock)) {
        schedule_delayed_work(&dt->work, HZ);
        return;
    }

    if (uvm_global_get_status() != NV_OK) {
        dynamic_threshold_stop(dt);
        goto unlock;
    }

    recovered = gpu_memory_pressure_recovered(dt->gpu);
    if (recovered) {
        result.threshold = dt->initial_threshold;
    }
    else {
        bytes = atomic64_xchg(&dt->migration_bytes, 0);
        rate = div64_u64(bytes * MSEC_PER_SEC, elapsed_ms * UVM_SIZE_1MB);
        dynamic_threshold_pid(dt, rate, elapsed_ms, &result);
    }

    status = uvm_gpu_access_counters_reconfigure_threshold_gpu(dt->gpu, result.threshold);

    if (status == NV_ERR_INVALID_STATE || (status == NV_OK && recovered)) {
        // The GPU no longer services access counters, or HBM recovered and
        // the configured threshold is back in place. The episode is over. A
        // later eviction under pressure starts a new one.
        if (uvm_perf_dynamic_threshold_log_adjustments) {
            UVM_INFO_PRINT("dynamic_threshold stop gpu=%s free_hbm_mib=%llu eviction_event_count=%llu "
                           "eviction_migration_mib=%llu threshold=%u\n",
                           uvm_gpu_name(dt->gpu), gpu_free_memory_bytes(dt->gpu) / UVM_SIZE_1MB,
                           (NvU64)atomic64_read(&dt->eviction_count),
                           (NvU64)atomic64_read(&dt->eviction_bytes) / UVM_SIZE_1MB,
                           dt->initial_threshold);
        }
        dynamic_threshold_stop(dt);
        goto unlock;
    }

    if (status == NV_OK) {
        // The new threshold is programmed. Commit the controller state.
        dt->threshold = result.threshold;
        dt->integral_error = result.integral_error;
        dt->previous_error = result.error;
        if (uvm_perf_dynamic_threshold_log_adjustments) {
            UVM_INFO_PRINT("dynamic_threshold gpu=%s elapsed_ms=%llu bytes=%llu rate_mib_s=%llu "
                           "threshold=%u error=%lld p=%lld i=%lld d=%lld requested_adjustment=%d "
                           "free_hbm_mib=%llu eviction_event_count=%llu eviction_migration_mib=%llu\n",
                           uvm_gpu_name(dt->gpu), elapsed_ms, bytes, rate, dt->threshold, result.error,
                           result.proportional, result.integral, result.derivative, result.adjustment,
                           gpu_free_memory_bytes(dt->gpu) / UVM_SIZE_1MB,
                           (NvU64)atomic64_read(&dt->eviction_count),
                           (NvU64)atomic64_read(&dt->eviction_bytes) / UVM_SIZE_1MB);
        }
    }
    else {
        // The threshold could not be programmed, for example while an access
        // counter test owns the configuration. Keep the controller history and
        // retry on the next sample. The traffic counted in this one is dropped.
    }

    dt->last_sample_jiffies = now;
    schedule_delayed_work(&dt->work, HZ);

unlock:
    uvm_up_read(&g_uvm_global.pm.lock);
}

// Enter UVM context for the workqueue callback.
static void dynamic_threshold_work_entry(struct work_struct *work)
{
    UVM_ENTRY_VOID(dynamic_threshold_work(work));
}

// Evictions signal memory pressure. The only form of eviction moves pages
// from GPU memory to the CPU.
static inline bool is_eviction(const uvm_perf_event_data_t *event_data)
{
    if (event_data->migration.cause != UVM_MAKE_RESIDENT_CAUSE_EVICTION)
        return false;

    UVM_ASSERT(UVM_ID_IS_GPU(event_data->migration.src));
    UVM_ASSERT(UVM_ID_IS_CPU(event_data->migration.dst));
    return true;
}

// Access counter migrations into the GPU, including prefetches issued while
// servicing access counters, are the traffic the threshold controls.
static inline bool is_access_counter_migration(const uvm_perf_event_data_t *event_data)
{
    const uvm_make_resident_context_t *context = event_data->migration.make_resident_context;
    uvm_make_resident_cause_t cause = event_data->migration.cause;

    if (!UVM_ID_IS_CPU(event_data->migration.src) || !UVM_ID_IS_GPU(event_data->migration.dst))
        return false;

    return cause == UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER ||
           (cause == UVM_MAKE_RESIDENT_CAUSE_PREFETCH && context &&
            context->cause == UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER);
}

// Start an episode on the first access counter migration that follows an
// eviction while HBM is under pressure. Return false if none was started.
static bool dynamic_threshold_start(uvm_perf_dynamic_threshold_t *dt)
{
    if (gpu_memory_pressure_recovered(dt->gpu)) {
        // HBM recovered since the eviction, so it no longer counts.
        atomic_set(&dt->evicted, 0);
        return false;
    }

    if (!atomic_read(&dt->evicted))
        return false;

    // Another callback may start the episode first.
    if (atomic_cmpxchg(&dt->active, 0, 1) != 0)
        return false;

    dt->last_sample_jiffies = jiffies;
    schedule_delayed_work(&dt->work, HZ);

    if (uvm_perf_dynamic_threshold_log_adjustments) {
        UVM_INFO_PRINT("dynamic_threshold start gpu=%s free_hbm_mib=%llu eviction_event_count=%llu "
                       "eviction_migration_mib=%llu\n",
                       uvm_gpu_name(dt->gpu), gpu_free_memory_bytes(dt->gpu) / UVM_SIZE_1MB,
                       (NvU64)atomic64_read(&dt->eviction_count),
                       (NvU64)atomic64_read(&dt->eviction_bytes) / UVM_SIZE_1MB);
    }

    return true;
}

// Remember evictions, count access counter traffic, and start an episode on
// the first traffic after an eviction under pressure.
static void dynamic_threshold_migration_callback(uvm_va_space_t *va_space,
                                                 uvm_perf_event_t event_id,
                                                 uvm_perf_event_data_t *event_data)
{
    NvU64 bytes = event_data->migration.bytes;
    uvm_perf_dynamic_threshold_t *dt;
    uvm_gpu_t *gpu;

    UVM_ASSERT(event_id == UVM_PERF_EVENT_MIGRATION);

    if (is_eviction(event_data)) {
        gpu = uvm_gpu_get(event_data->migration.src);
        dt = gpu ? gpu->dynamic_threshold : NULL;
        if (!dt)
            return;

        atomic64_inc(&dt->eviction_count);
        atomic64_add(bytes, &dt->eviction_bytes);
        if (atomic_xchg(&dt->evicted, 1) == 0 && uvm_perf_dynamic_threshold_log_adjustments) {
            UVM_INFO_PRINT("dynamic_threshold eviction gpu=%s free_hbm_mib=%llu\n",
                           uvm_gpu_name(gpu), gpu_free_memory_bytes(gpu) / UVM_SIZE_1MB);
        }
        return;
    }

    if (!is_access_counter_migration(event_data) || !uvm_va_space_has_access_counter_migrations(va_space))
        return;

    gpu = uvm_gpu_get(event_data->migration.dst);
    dt = gpu ? gpu->dynamic_threshold : NULL;
    if (!dt)
        return;

    if (!atomic_read(&dt->active) && !dynamic_threshold_start(dt))
        return;

    atomic64_add(bytes, &dt->migration_bytes);
}

NV_STATUS uvm_perf_dynamic_threshold_init(void)
{
    if (!uvm_perf_dynamic_threshold_enable)
        return NV_OK;

    if (uvm_perf_dynamic_threshold_max_rate_mib_per_sec == 0 ||
        uvm_perf_dynamic_threshold_max_rate_mib_per_sec > UVM_PERF_DYNAMIC_THRESHOLD_MAX_RATE_MIB_PER_SEC_LIMIT) {
        UVM_ERR_PRINT_ALWAYS("Invalid value %u for uvm_perf_dynamic_threshold_max_rate_mib_per_sec, "
                             "valid values: [1, %u]\n",
                             uvm_perf_dynamic_threshold_max_rate_mib_per_sec,
                             UVM_PERF_DYNAMIC_THRESHOLD_MAX_RATE_MIB_PER_SEC_LIMIT);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (uvm_perf_dynamic_threshold_min_rate_mib_per_sec > uvm_perf_dynamic_threshold_max_rate_mib_per_sec) {
        UVM_ERR_PRINT_ALWAYS("Invalid value %u for uvm_perf_dynamic_threshold_min_rate_mib_per_sec, "
                             "valid values: [0, %u]\n",
                             uvm_perf_dynamic_threshold_min_rate_mib_per_sec,
                             uvm_perf_dynamic_threshold_max_rate_mib_per_sec);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

NV_STATUS uvm_perf_dynamic_threshold_add_gpu(uvm_gpu_t *gpu)
{
    uvm_perf_dynamic_threshold_t *dt;

    if (!uvm_perf_dynamic_threshold_enable ||
        !uvm_parent_gpu_access_counters_required(gpu->parent) ||
        gpu->parent->smc.enabled)
        return NV_OK;

    dt = uvm_kvmalloc_zero(sizeof(*dt));
    if (!dt) {
        UVM_ERR_PRINT_ALWAYS("Failed to allocate dynamic threshold state for GPU %s\n", uvm_gpu_name(gpu));
        return NV_ERR_NO_MEMORY;
    }

    dt->gpu = gpu;
    dt->initial_threshold = uvm_gpu_access_counters_get_default_threshold();
    dt->threshold = dt->initial_threshold;
    INIT_DELAYED_WORK(&dt->work, dynamic_threshold_work_entry);
    gpu->dynamic_threshold = dt;

    return NV_OK;
}

void uvm_perf_dynamic_threshold_remove_gpu(uvm_gpu_t *gpu)
{
    uvm_perf_dynamic_threshold_t *dt = gpu->dynamic_threshold;

    if (!dt)
        return;

    // Migration callbacks for this GPU have stopped, so only the work item can
    // still requeue itself. cancel_delayed_work_sync() handles that.
    cancel_delayed_work_sync(&dt->work);
    gpu->dynamic_threshold = NULL;
    uvm_kvfree(dt);
}

NV_STATUS uvm_perf_dynamic_threshold_load(uvm_va_space_t *va_space)
{
    if (!uvm_perf_dynamic_threshold_enable)
        return NV_OK;

    return uvm_perf_register_event_callback(&va_space->perf_events,
                                            UVM_PERF_EVENT_MIGRATION,
                                            dynamic_threshold_migration_callback);
}

NV_STATUS uvm_test_inject_migration_event(UVM_TEST_INJECT_MIGRATION_EVENT_PARAMS *params,
                                          struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_perf_event_data_t event_data = {};
    uvm_make_resident_context_t *context;
    uvm_gpu_t *gpu;
    NV_STATUS status = NV_OK;

    // A prefetch is reported as issued while servicing access counters.
    switch (params->cause) {
        case UvmEventMigrationCauseAccessCounters:
            event_data.migration.cause = UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER;
            break;
        case UvmEventMigrationCausePrefetch:
            event_data.migration.cause = UVM_MAKE_RESIDENT_CAUSE_PREFETCH;
            break;
        case UvmEventMigrationCauseCoherence:
            event_data.migration.cause = UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    if (params->bytes == 0 || params->bytes > UVM_SIZE_1GB || !IS_ALIGNED(params->bytes, PAGE_SIZE))
        return NV_ERR_INVALID_ARGUMENT;

    context = uvm_kvmalloc_zero(sizeof(*context));
    if (!context)
        return NV_ERR_NO_MEMORY;

    context->cause = event_data.migration.cause == UVM_MAKE_RESIDENT_CAUSE_PREFETCH ?
                         UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER :
                         event_data.migration.cause;

    uvm_va_space_down_read(va_space);

    gpu = uvm_va_space_get_gpu_by_uuid(va_space, &params->gpu_uuid);
    if (!gpu) {
        status = NV_ERR_INVALID_DEVICE;
    }
    else if (!gpu->dynamic_threshold || !uvm_va_space_has_access_counter_migrations(va_space)) {
        status = NV_ERR_NOT_SUPPORTED;
    }
    else {
        event_data.migration.dst = gpu->id;
        event_data.migration.src = UVM_ID_CPU;
        event_data.migration.bytes = params->bytes;
        event_data.migration.transfer_mode = UVM_VA_BLOCK_TRANSFER_MODE_MOVE;
        event_data.migration.make_resident_context = context;
        dynamic_threshold_migration_callback(va_space, UVM_PERF_EVENT_MIGRATION, &event_data);
    }

    uvm_va_space_up_read(va_space);
    uvm_kvfree(context);

    return status;
}
