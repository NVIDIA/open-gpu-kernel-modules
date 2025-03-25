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

#include "uvm_api.h"
#include "uvm_global.h"
#include "uvm_perf_events.h"
#include "uvm_perf_module.h"
#include "uvm_perf_thrashing.h"
#include "uvm_perf_utils.h"
#include "uvm_va_block.h"
#include "uvm_va_range.h"
#include "uvm_kvmalloc.h"
#include "uvm_tools.h"
#include "uvm_procfs.h"
#include "uvm_test.h"

// Number of bits for page-granularity time stamps. Currently we ignore the first 6 bits
// of the timestamp (i.e. we have 64ns resolution, which is good enough)
#define PAGE_THRASHING_LAST_TIME_STAMP_BITS 58
#define PAGE_THRASHING_NUM_EVENTS_BITS      3

#define PAGE_THRASHING_THROTTLING_END_TIME_STAMP_BITS 58
#define PAGE_THRASHING_THROTTLING_COUNT_BITS          8

// Per-page thrashing detection structure.
typedef struct
{
    struct
    {
        // Last time stamp when a thrashing-related event was recorded
        NvU64                        last_time_stamp : PAGE_THRASHING_LAST_TIME_STAMP_BITS;

        bool                    has_migration_events : 1;

        bool                   has_revocation_events : 1;

        // Number of consecutive "thrashing" events (within the configured
        // thrashing lapse)
        NvU8                    num_thrashing_events : PAGE_THRASHING_NUM_EVENTS_BITS;

        bool                                  pinned : 1;
    };

    struct
    {
        // Deadline for throttled processors to wake up
        NvU64              throttling_end_time_stamp : PAGE_THRASHING_THROTTLING_END_TIME_STAMP_BITS;

        // Number of times a processor has been throttled. This is used to
        // determine when the page needs to get pinned. After getting pinned
        // this field is always 0.
        NvU8                        throttling_count : PAGE_THRASHING_THROTTLING_COUNT_BITS;
    };

    // Processors accessing this page
    uvm_processor_mask_t                  processors;

    // Processors that have been throttled. This must be a subset of processors
    uvm_processor_mask_t        throttled_processors;

    // Memory residency for the page when in pinning phase
    uvm_processor_id_t           pinned_residency_id;

    // Processor not to be throttled in the current throttling period
    uvm_processor_id_t  do_not_throttle_processor_id;
} page_thrashing_info_t;

// Per-VA block thrashing detection structure. This state is protected by the
// VA block lock.
typedef struct
{
    page_thrashing_info_t                     *pages;

    NvU16                        num_thrashing_pages;

    NvU8                       thrashing_reset_count;

    uvm_processor_id_t                last_processor;

    NvU64                            last_time_stamp;

    NvU64                  last_thrashing_time_stamp;

    // Stats
    NvU32                           throttling_count;

    uvm_page_mask_t                  thrashing_pages;

    struct
    {
        NvU32                                  count;

        uvm_page_mask_t                         mask;

        // List of pinned pages. This list is only used if the pinning timeout
        // is not 0.
        struct list_head                        list;
    } pinned_pages;
} block_thrashing_info_t;

// Descriptor for a page that has been pinned due to thrashing. This structure
// is only used if the pinning timeout is not 0.
typedef struct
{
    uvm_va_block_t                         *va_block;

    // Page index within va_block
    uvm_page_index_t                      page_index;

    // Absolute timestamp after which the page will be unpinned
    NvU64                                   deadline;

    // Entry in the per-VA Space list of pinned pages. See
    // va_space_thrashing_info_t::pinned_pages::list.
    struct list_head             va_space_list_entry;

    // Entry in the per-VA Block list of pinned pages. See
    // block_thrashing_info_t::pinned_pages::list.
    struct list_head             va_block_list_entry;
} pinned_page_t;

// Per-VA space data structures and policy configuration
typedef struct
{
    // Per-VA space accounting of pinned pages that is used to speculatively
    // unpin pages after the configured timeout. This struct is only used if
    // the pinning timeout is not 0.
    struct
    {
        // Work descriptor that is executed asynchronously by a helper thread
        struct delayed_work                    dwork;

        // List of pinned pages. They are (mostly) ordered by unpin deadline.
        // New entries are inserted blindly at the tail since the expectation
        // is that they will have the largest deadline value. However, given
        // the drift between when multiple threads query their timestamps and
        // add those pages to the list under the lock, it might not be
        // strictly ordered. But this is OK since the difference will be very
        // small and they will be eventually removed from the list.
        //
        // Entries are removed when they reach the deadline by the function
        // configured in dwork. This list is protected by lock.
        struct list_head                        list;

        uvm_spinlock_t                          lock;

        uvm_va_block_context_t      *va_block_context;

        // Flag used to avoid scheduling delayed unpinning operations after
        // uvm_perf_thrashing_stop has been called.
        bool                    in_va_space_teardown;
    } pinned_pages;

    struct
    {
        // Whether thrashing mitigation is enabled on this VA space
        bool                                  enable;

        // true if the thrashing mitigation parameters have been modified using
        // test ioctls
        bool                          test_overrides;

        //
        // Fields below are the thrashing mitigation parameters on the VA space
        //
        unsigned                           threshold;

        unsigned                       pin_threshold;

        NvU64                               lapse_ns;

        NvU64                                 nap_ns;

        NvU64                               epoch_ns;

        unsigned                          max_resets;

        NvU64                                 pin_ns;
    } params;

    uvm_va_space_t                         *va_space;
} va_space_thrashing_info_t;

typedef struct
{
    // Entry for the per-processor thrashing_stats file in procfs
    struct proc_dir_entry *procfs_file;

    // Number of times thrashing is detected
    atomic64_t num_thrashing;

    // Number of times the processor was throttled while thrashing
    atomic64_t num_throttle;

    // Number of times a page was pinned on this processor while thrashing
    atomic64_t num_pin_local;

    // Number of times a page was pinned on a different processor while thrashing
    atomic64_t num_pin_remote;
} processor_thrashing_stats_t;

// Pre-allocated thrashing stats structure for the CPU. This is only valid if
// uvm_procfs_is_debug_enabled() returns true.
static processor_thrashing_stats_t g_cpu_thrashing_stats;

#define PROCESSOR_THRASHING_STATS_INC(proc, field)                                                   \
    do {                                                                                             \
        processor_thrashing_stats_t *_processor_stats = thrashing_stats_get_or_null(proc);           \
        if (_processor_stats)                                                                        \
            atomic64_inc(&_processor_stats->field);                                                  \
    } while (0)

// Global caches for the per-VA block thrashing detection structures
static struct kmem_cache *g_va_block_thrashing_info_cache __read_mostly;
static struct kmem_cache *g_pinned_page_cache __read_mostly;

//
// Tunables for thrashing detection/prevention (configurable via module parameters)
//

#define UVM_PERF_THRASHING_ENABLE_DEFAULT 1

// Enable/disable thrashing performance heuristics
static unsigned uvm_perf_thrashing_enable = UVM_PERF_THRASHING_ENABLE_DEFAULT;

#define UVM_PERF_THRASHING_THRESHOLD_DEFAULT 3
#define UVM_PERF_THRASHING_THRESHOLD_MAX     ((1 << PAGE_THRASHING_NUM_EVENTS_BITS) - 1)

// Number of consecutive thrashing events to initiate thrashing prevention
//
// Maximum value is UVM_PERF_THRASHING_THRESHOLD_MAX
static unsigned uvm_perf_thrashing_threshold = UVM_PERF_THRASHING_THRESHOLD_DEFAULT;

#define UVM_PERF_THRASHING_PIN_THRESHOLD_DEFAULT 10
#define UVM_PERF_THRASHING_PIN_THRESHOLD_MAX     ((1 << PAGE_THRASHING_THROTTLING_COUNT_BITS) - 1)

// Number of consecutive throttling operations before trying to map remotely
//
// Maximum value is UVM_PERF_THRASHING_PIN_THRESHOLD_MAX
static unsigned uvm_perf_thrashing_pin_threshold = UVM_PERF_THRASHING_PIN_THRESHOLD_DEFAULT;

// TODO: Bug 1768615: [uvm] Automatically tune default values for thrashing
// detection/prevention parameters
#define UVM_PERF_THRASHING_LAPSE_USEC_DEFAULT 500
#define UVM_PERF_THRASHING_LAPSE_USEC_DEFAULT_EMULATION (UVM_PERF_THRASHING_LAPSE_USEC_DEFAULT * 800)

// Lapse of time in microseconds that determines if two consecutive events on
// the same page can be considered thrashing
static unsigned uvm_perf_thrashing_lapse_usec = UVM_PERF_THRASHING_LAPSE_USEC_DEFAULT;

#define UVM_PERF_THRASHING_NAP_DEFAULT 1
#define UVM_PERF_THRASHING_NAP_MAX     100

// Time that the processor being throttled is forbidden to work on the thrashing
// page. This value is a multiplier of uvm_perf_thrashing_lapse_usec.
static unsigned uvm_perf_thrashing_nap = UVM_PERF_THRASHING_NAP_DEFAULT;

#define UVM_PERF_THRASHING_EPOCH_DEFAULT 2000

// Time lapse after which we consider thrashing is no longer happening. This
// value is a multiplier of uvm_perf_thrashing_lapse_usec.
static unsigned uvm_perf_thrashing_epoch = UVM_PERF_THRASHING_EPOCH_DEFAULT;

// When pages are pinned and the rest of thrashing processors are mapped
// remotely we lose track of who is accessing the page for the rest of
// program execution. This can lead to tremendous performance loss if the page
// is not thrashing anymore and it is always being accessed remotely.
// In order to avoid that scenario, we use a timer that unpins memory after
// some time. We use a per-VA space list of pinned pages, sorted by the
// deadline at which it will be unmapped from remote processors. Therefore,
// next remote access will trigger a fault that will migrate the page.
#define UVM_PERF_THRASHING_PIN_DEFAULT 300
#define UVM_PERF_THRASHING_PIN_DEFAULT_EMULATION 10

// Time for which a page remains pinned. This value is a multiplier of
// uvm_perf_thrashing_lapse_usec. 0 means that it is pinned forever.
static unsigned uvm_perf_thrashing_pin = UVM_PERF_THRASHING_PIN_DEFAULT;

// Number of times a VA block can be reset back to non-thrashing. This
// mechanism tries to avoid performing optimizations on a block that periodically
// causes thrashing
#define UVM_PERF_THRASHING_MAX_RESETS_DEFAULT 4

static unsigned uvm_perf_thrashing_max_resets = UVM_PERF_THRASHING_MAX_RESETS_DEFAULT;

// Module parameters for the tunables
module_param(uvm_perf_thrashing_enable,        uint, S_IRUGO);
module_param(uvm_perf_thrashing_threshold,     uint, S_IRUGO);
module_param(uvm_perf_thrashing_pin_threshold, uint, S_IRUGO);
module_param(uvm_perf_thrashing_lapse_usec,    uint, S_IRUGO);
module_param(uvm_perf_thrashing_nap,           uint, S_IRUGO);
module_param(uvm_perf_thrashing_epoch,         uint, S_IRUGO);
module_param(uvm_perf_thrashing_pin,           uint, S_IRUGO);
module_param(uvm_perf_thrashing_max_resets,    uint, S_IRUGO);

// See map_remote_on_atomic_fault uvm_va_block.c
unsigned uvm_perf_map_remote_on_native_atomics_fault = 0;
module_param(uvm_perf_map_remote_on_native_atomics_fault, uint, S_IRUGO);

// Global post-processed values of the module parameters. They can be overriden
// per VA-space.
static bool g_uvm_perf_thrashing_enable;
static unsigned g_uvm_perf_thrashing_threshold;
static unsigned g_uvm_perf_thrashing_pin_threshold;
static NvU64 g_uvm_perf_thrashing_lapse_usec;
static NvU64 g_uvm_perf_thrashing_nap;
static NvU64 g_uvm_perf_thrashing_epoch;
static NvU64 g_uvm_perf_thrashing_pin;
static unsigned g_uvm_perf_thrashing_max_resets;

// Helper macros to initialize thrashing parameters from module parameters
//
// This helper returns whether the type for the parameter is signed
#define THRASHING_PARAMETER_IS_SIGNED(v) (((typeof(v)) -1) < 0)

// Macro that initializes the given thrashing parameter and checks its validity
// (within [_mi:_ma]). Otherwise it is initialized with the given default
// parameter _d. The user value is read from _v, and the final value is stored
// in a variable named g_##_v, so it must be declared, too. Only unsigned
// parameters are supported.
#define INIT_THRASHING_PARAMETER_MIN_MAX(_v, _d, _mi, _ma)                            \
    do {                                                                              \
        unsigned v = (_v);                                                            \
        unsigned d = (_d);                                                            \
        unsigned mi = (_mi);                                                          \
        unsigned ma = (_ma);                                                          \
                                                                                      \
        BUILD_BUG_ON(sizeof(_v) > sizeof(unsigned));                                  \
        BUILD_BUG_ON(THRASHING_PARAMETER_IS_SIGNED(_v));                              \
                                                                                      \
        UVM_ASSERT(mi <= ma);                                                         \
        UVM_ASSERT(d >= mi);                                                          \
        UVM_ASSERT(d <= ma);                                                          \
                                                                                      \
        if (v >= mi && v <= ma) {                                                     \
            g_##_v = v;                                                               \
        }                                                                             \
        else {                                                                        \
            UVM_INFO_PRINT("Invalid value %u for " #_v ". Using %u instead\n", v, d); \
                                                                                      \
            g_##_v = d;                                                               \
        }                                                                             \
    } while (0)

#define INIT_THRASHING_PARAMETER(v, d)                 INIT_THRASHING_PARAMETER_MIN_MAX(v, d, 0u, UINT_MAX)

#define INIT_THRASHING_PARAMETER_MIN(v, d, mi)         INIT_THRASHING_PARAMETER_MIN_MAX(v, d, mi, UINT_MAX)
#define INIT_THRASHING_PARAMETER_MAX(v, d, ma)         INIT_THRASHING_PARAMETER_MIN_MAX(v, d, 0u, ma)

#define INIT_THRASHING_PARAMETER_NONZERO(v, d)         INIT_THRASHING_PARAMETER_MIN_MAX(v, d, 1u, UINT_MAX)
#define INIT_THRASHING_PARAMETER_NONZERO_MAX(v, d, ma) INIT_THRASHING_PARAMETER_MIN_MAX(v, d, 1u, ma)

#define INIT_THRASHING_PARAMETER_TOGGLE(v, d)          INIT_THRASHING_PARAMETER_MIN_MAX(v, d, 0u, 1u)

// Helpers to get/set the time stamp
static NvU64 page_thrashing_get_time_stamp(page_thrashing_info_t *entry)
{
    return entry->last_time_stamp << (64 - PAGE_THRASHING_LAST_TIME_STAMP_BITS);
}

static void page_thrashing_set_time_stamp(page_thrashing_info_t *entry, NvU64 time_stamp)
{
    entry->last_time_stamp = time_stamp >> (64 - PAGE_THRASHING_LAST_TIME_STAMP_BITS);
}

static NvU64 page_thrashing_get_throttling_end_time_stamp(page_thrashing_info_t *entry)
{
    return entry->throttling_end_time_stamp << (64 - PAGE_THRASHING_THROTTLING_END_TIME_STAMP_BITS);
}

static void page_thrashing_set_throttling_end_time_stamp(page_thrashing_info_t *entry, NvU64 time_stamp)
{
    entry->throttling_end_time_stamp = time_stamp >> (64 - PAGE_THRASHING_THROTTLING_END_TIME_STAMP_BITS);
}

// Performance heuristics module for thrashing
static uvm_perf_module_t g_module_thrashing;

// Callback declaration for the performance heuristics events
static void thrashing_event_cb(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data);
static void thrashing_block_destroy_cb(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data);
static void thrashing_block_munmap_cb(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data);

static uvm_perf_module_event_callback_desc_t g_callbacks_thrashing[] = {
    { UVM_PERF_EVENT_BLOCK_DESTROY, thrashing_block_destroy_cb },
    { UVM_PERF_EVENT_MODULE_UNLOAD, thrashing_block_destroy_cb },
    { UVM_PERF_EVENT_BLOCK_SHRINK , thrashing_block_destroy_cb },
    { UVM_PERF_EVENT_BLOCK_MUNMAP , thrashing_block_munmap_cb  },
    { UVM_PERF_EVENT_MIGRATION,     thrashing_event_cb         },
    { UVM_PERF_EVENT_REVOCATION,    thrashing_event_cb         }
};

static int nv_procfs_read_thrashing_stats(struct seq_file *s, void *v)
{
    processor_thrashing_stats_t *processor_stats = (processor_thrashing_stats_t *)s->private;

    UVM_ASSERT(processor_stats);

    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
            return -EAGAIN;

    UVM_SEQ_OR_DBG_PRINT(s, "thrashing     %llu\n", (NvU64)atomic64_read(&processor_stats->num_thrashing));
    UVM_SEQ_OR_DBG_PRINT(s, "throttle      %llu\n", (NvU64)atomic64_read(&processor_stats->num_throttle));
    UVM_SEQ_OR_DBG_PRINT(s, "pin_local     %llu\n", (NvU64)atomic64_read(&processor_stats->num_pin_local));
    UVM_SEQ_OR_DBG_PRINT(s, "pin_remote    %llu\n", (NvU64)atomic64_read(&processor_stats->num_pin_remote));

    uvm_up_read(&g_uvm_global.pm.lock);

    return 0;
}

static int nv_procfs_read_thrashing_stats_entry(struct seq_file *s, void *v)
{
    UVM_ENTRY_RET(nv_procfs_read_thrashing_stats(s, v));
}

UVM_DEFINE_SINGLE_PROCFS_FILE(thrashing_stats_entry);

#define THRASHING_STATS_FILE_NAME "thrashing_stats"

// Initialization/deinitialization of CPU thrashing stats
//
static NV_STATUS cpu_thrashing_stats_init(void)
{
    struct proc_dir_entry *cpu_base_dir_entry = uvm_procfs_get_cpu_base_dir();

    if (uvm_procfs_is_debug_enabled()) {
        UVM_ASSERT(!g_cpu_thrashing_stats.procfs_file);
        g_cpu_thrashing_stats.procfs_file = NV_CREATE_PROC_FILE(THRASHING_STATS_FILE_NAME,
                                                                cpu_base_dir_entry,
                                                                thrashing_stats_entry,
                                                                &g_cpu_thrashing_stats);
        if (!g_cpu_thrashing_stats.procfs_file)
            return NV_ERR_OPERATING_SYSTEM;
    }

    return NV_OK;
}

static void cpu_thrashing_stats_exit(void)
{
    if (g_cpu_thrashing_stats.procfs_file) {
        UVM_ASSERT(uvm_procfs_is_debug_enabled());
        proc_remove(g_cpu_thrashing_stats.procfs_file);
        g_cpu_thrashing_stats.procfs_file = NULL;
    }
}

// Get the thrashing stats struct for the given VA space if it exists
//
// No lock may be held. Therefore, the stats must be updated using atomics
static processor_thrashing_stats_t *gpu_thrashing_stats_get_or_null(uvm_gpu_t *gpu)
{
    return uvm_perf_module_type_data(gpu->perf_modules_data, UVM_PERF_MODULE_TYPE_THRASHING);
}

static processor_thrashing_stats_t *thrashing_stats_get_or_null(uvm_processor_id_t id)
{
    if (UVM_ID_IS_CPU(id)) {
        if (g_cpu_thrashing_stats.procfs_file)
            return &g_cpu_thrashing_stats;

        return NULL;
    }

    return gpu_thrashing_stats_get_or_null(uvm_gpu_get(id));
}

// Create the thrashing stats struct for the given GPU
//
// Global lock needs to be held
static NV_STATUS gpu_thrashing_stats_create(uvm_gpu_t *gpu)
{
    processor_thrashing_stats_t *gpu_thrashing;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);
    UVM_ASSERT(gpu_thrashing_stats_get_or_null(gpu) == NULL);
    UVM_ASSERT(uvm_procfs_is_debug_enabled());

    gpu_thrashing = uvm_kvmalloc_zero(sizeof(*gpu_thrashing));
    if (!gpu_thrashing)
        return NV_ERR_NO_MEMORY;

    gpu_thrashing->procfs_file = NV_CREATE_PROC_FILE(THRASHING_STATS_FILE_NAME,
                                                     gpu->procfs.dir,
                                                     thrashing_stats_entry,
                                                     gpu_thrashing);
    if (!gpu_thrashing->procfs_file) {
        uvm_kvfree(gpu_thrashing);
        return NV_ERR_OPERATING_SYSTEM;
    }

    uvm_perf_module_type_set_data(gpu->perf_modules_data, gpu_thrashing, UVM_PERF_MODULE_TYPE_THRASHING);

    return NV_OK;
}

static void gpu_thrashing_stats_destroy(uvm_gpu_t *gpu)
{
    processor_thrashing_stats_t *gpu_thrashing = gpu_thrashing_stats_get_or_null(gpu);

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    if (gpu_thrashing) {
        uvm_perf_module_type_unset_data(gpu->perf_modules_data, UVM_PERF_MODULE_TYPE_THRASHING);

        if (gpu_thrashing->procfs_file)
            proc_remove(gpu_thrashing->procfs_file);

        uvm_kvfree(gpu_thrashing);
    }
}

// Get the thrashing detection struct for the given VA space if it exists
//
// The caller must ensure that the va_space cannot be deleted, for the
// duration of this call. Holding either the va_block or va_space lock will do
// that.
static va_space_thrashing_info_t *va_space_thrashing_info_get_or_null(uvm_va_space_t *va_space)
{
    return uvm_perf_module_type_data(va_space->perf_modules_data, UVM_PERF_MODULE_TYPE_THRASHING);
}

// Get the thrashing detection struct for the given VA space. It asserts that
// the information has been previously created.
//
// The caller must ensure that the va_space cannot be deleted, for the
// duration of this call. Holding either the va_block or va_space lock will do
// that.
static va_space_thrashing_info_t *va_space_thrashing_info_get(uvm_va_space_t *va_space)
{
    va_space_thrashing_info_t *va_space_thrashing = va_space_thrashing_info_get_or_null(va_space);
    UVM_ASSERT(va_space_thrashing);

    return va_space_thrashing;
}

static void va_space_thrashing_info_init_params(va_space_thrashing_info_t *va_space_thrashing)
{
    UVM_ASSERT(!va_space_thrashing->params.test_overrides);

    va_space_thrashing->params.enable = g_uvm_perf_thrashing_enable;

    // Snap the thrashing parameters so that they can be tuned per VA space
    va_space_thrashing->params.threshold     = g_uvm_perf_thrashing_threshold;
    va_space_thrashing->params.pin_threshold = g_uvm_perf_thrashing_pin_threshold;

    // Default thrashing parameters are overriden for simulated/emulated GPUs
    if (g_uvm_global.num_simulated_devices > 0 &&
        (g_uvm_perf_thrashing_lapse_usec == UVM_PERF_THRASHING_LAPSE_USEC_DEFAULT)) {
        va_space_thrashing->params.lapse_ns  = UVM_PERF_THRASHING_LAPSE_USEC_DEFAULT_EMULATION * 1000;
    }
    else {
        va_space_thrashing->params.lapse_ns  = g_uvm_perf_thrashing_lapse_usec * 1000;
    }

    va_space_thrashing->params.nap_ns        = va_space_thrashing->params.lapse_ns * g_uvm_perf_thrashing_nap;
    va_space_thrashing->params.epoch_ns      = va_space_thrashing->params.lapse_ns * g_uvm_perf_thrashing_epoch;

    if (g_uvm_global.num_simulated_devices > 0 && (g_uvm_perf_thrashing_pin == UVM_PERF_THRASHING_PIN_DEFAULT)) {
        va_space_thrashing->params.pin_ns    = va_space_thrashing->params.lapse_ns
                                               * UVM_PERF_THRASHING_PIN_DEFAULT_EMULATION;
    }
    else {
        va_space_thrashing->params.pin_ns    = va_space_thrashing->params.lapse_ns * g_uvm_perf_thrashing_pin;
    }

    va_space_thrashing->params.max_resets    = g_uvm_perf_thrashing_max_resets;
}

// Create the thrashing detection struct for the given VA space
//
// VA space lock needs to be held in write mode
static va_space_thrashing_info_t *va_space_thrashing_info_create(uvm_va_space_t *va_space)
{
    va_space_thrashing_info_t *va_space_thrashing;
    uvm_assert_rwsem_locked_write(&va_space->lock);

    UVM_ASSERT(va_space_thrashing_info_get_or_null(va_space) == NULL);

    va_space_thrashing = uvm_kvmalloc_zero(sizeof(*va_space_thrashing));
    if (va_space_thrashing) {
        uvm_va_block_context_t *block_context = uvm_va_block_context_alloc(NULL);

        if (!block_context) {
            uvm_kvfree(va_space_thrashing);
            return NULL;
        }

        va_space_thrashing->pinned_pages.va_block_context = block_context;
        va_space_thrashing->va_space = va_space;

        va_space_thrashing_info_init_params(va_space_thrashing);

        uvm_perf_module_type_set_data(va_space->perf_modules_data, va_space_thrashing, UVM_PERF_MODULE_TYPE_THRASHING);
    }

    return va_space_thrashing;
}

// Destroy the thrashing detection struct for the given VA space
//
// VA space lock needs to be in write mode
static void va_space_thrashing_info_destroy(uvm_va_space_t *va_space)
{
    va_space_thrashing_info_t *va_space_thrashing = va_space_thrashing_info_get_or_null(va_space);
    uvm_assert_rwsem_locked_write(&va_space->lock);

    if (va_space_thrashing) {
        uvm_perf_module_type_unset_data(va_space->perf_modules_data, UVM_PERF_MODULE_TYPE_THRASHING);
        uvm_va_block_context_free(va_space_thrashing->pinned_pages.va_block_context);
        uvm_kvfree(va_space_thrashing);
    }
}

// Get the thrashing detection struct for the given block
static block_thrashing_info_t *thrashing_info_get(uvm_va_block_t *va_block)
{
    uvm_assert_mutex_locked(&va_block->lock);
    return uvm_perf_module_type_data(va_block->perf_modules_data, UVM_PERF_MODULE_TYPE_THRASHING);
}

// Get the thrashing detection struct for the given block or create it if it
// does not exist
static block_thrashing_info_t *thrashing_info_get_create(uvm_va_block_t *va_block)
{
    block_thrashing_info_t *block_thrashing = thrashing_info_get(va_block);

    BUILD_BUG_ON((1 << 8 * sizeof(block_thrashing->num_thrashing_pages)) < PAGES_PER_UVM_VA_BLOCK);
    BUILD_BUG_ON((1 << 16) < UVM_ID_MAX_PROCESSORS);

    if (!block_thrashing) {
        block_thrashing = nv_kmem_cache_zalloc(g_va_block_thrashing_info_cache, NV_UVM_GFP_FLAGS);
        if (!block_thrashing)
            goto done;

        block_thrashing->last_processor = UVM_ID_INVALID;
        INIT_LIST_HEAD(&block_thrashing->pinned_pages.list);

        uvm_perf_module_type_set_data(va_block->perf_modules_data, block_thrashing, UVM_PERF_MODULE_TYPE_THRASHING);
    }

done:
    return block_thrashing;
}

static void thrashing_reset_pages_in_region(uvm_va_block_t *va_block, NvU64 address, NvU64 bytes);

void uvm_perf_thrashing_info_destroy(uvm_va_block_t *va_block)
{
    block_thrashing_info_t *block_thrashing = thrashing_info_get(va_block);

    if (block_thrashing) {
        thrashing_reset_pages_in_region(va_block, va_block->start, uvm_va_block_size(va_block));

        uvm_perf_module_type_unset_data(va_block->perf_modules_data, UVM_PERF_MODULE_TYPE_THRASHING);

        uvm_kvfree(block_thrashing->pages);
        kmem_cache_free(g_va_block_thrashing_info_cache, block_thrashing);
    }
}

void thrashing_block_destroy_cb(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    uvm_va_block_t *va_block;

    UVM_ASSERT(g_uvm_perf_thrashing_enable);

    UVM_ASSERT(event_id == UVM_PERF_EVENT_BLOCK_DESTROY ||
               event_id == UVM_PERF_EVENT_BLOCK_SHRINK ||
               event_id == UVM_PERF_EVENT_MODULE_UNLOAD);

    if (event_id == UVM_PERF_EVENT_BLOCK_DESTROY)
        va_block = event_data->block_destroy.block;
    else if (event_id == UVM_PERF_EVENT_BLOCK_SHRINK)
        va_block = event_data->block_shrink.block;
    else
        va_block = event_data->module_unload.block;

    if (!va_block)
        return;

    uvm_perf_thrashing_info_destroy(va_block);
}

void thrashing_block_munmap_cb(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    uvm_va_block_t *va_block = event_data->block_munmap.block;
    uvm_va_block_region_t region = event_data->block_munmap.region;

    UVM_ASSERT(g_uvm_perf_thrashing_enable);
    UVM_ASSERT(event_id == UVM_PERF_EVENT_BLOCK_MUNMAP);
    UVM_ASSERT(va_block);

    thrashing_reset_pages_in_region(va_block,
                                    uvm_va_block_region_start(va_block, region),
                                    uvm_va_block_region_size(region));
}

// Sanity checks of the thrashing tracking state
static bool thrashing_state_checks(uvm_va_block_t *va_block,
                                   block_thrashing_info_t *block_thrashing,
                                   page_thrashing_info_t *page_thrashing,
                                   uvm_page_index_t page_index)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    va_space_thrashing_info_t *va_space_thrashing = va_space_thrashing_info_get(va_space);

    if (!block_thrashing) {
        UVM_ASSERT(!page_thrashing);
        return true;
    }

    UVM_ASSERT(uvm_page_mask_subset(&block_thrashing->pinned_pages.mask, &block_thrashing->thrashing_pages));

    if (page_thrashing) {
        UVM_ASSERT(block_thrashing->pages);
        UVM_ASSERT(page_thrashing == &block_thrashing->pages[page_index]);
    }
    else {
        UVM_ASSERT(!uvm_page_mask_test(&block_thrashing->thrashing_pages, page_index));
        return true;
    }

    UVM_ASSERT(uvm_processor_mask_subset(&page_thrashing->throttled_processors,
                                         &page_thrashing->processors));

    if (uvm_page_mask_test(&block_thrashing->thrashing_pages, page_index))
        UVM_ASSERT(page_thrashing->num_thrashing_events >= va_space_thrashing->params.threshold);

    if (page_thrashing->pinned) {
        UVM_ASSERT(uvm_page_mask_test(&block_thrashing->pinned_pages.mask, page_index));
        UVM_ASSERT(UVM_ID_IS_VALID(page_thrashing->pinned_residency_id));
        UVM_ASSERT(page_thrashing->throttling_count == 0);
    }
    else {
        UVM_ASSERT(!uvm_page_mask_test(&block_thrashing->pinned_pages.mask, page_index));
        UVM_ASSERT(UVM_ID_IS_INVALID(page_thrashing->pinned_residency_id));

        if (!uvm_processor_mask_empty(&page_thrashing->throttled_processors)) {
            UVM_ASSERT(page_thrashing->throttling_count > 0);
            UVM_ASSERT(uvm_page_mask_test(&block_thrashing->thrashing_pages, page_index));
        }
    }

    return true;
}

// Update throttling heuristics. Mainly check if a new throttling period has
// started and choose the next processor not to be throttled. This function
// is executed before the thrashing mitigation logic kicks in.
static void thrashing_throttle_update(va_space_thrashing_info_t *va_space_thrashing,
                                      uvm_va_block_t *va_block,
                                      page_thrashing_info_t *page_thrashing,
                                      uvm_processor_id_t processor,
                                      NvU64 time_stamp)
{
    NvU64 current_end_time_stamp = page_thrashing_get_throttling_end_time_stamp(page_thrashing);

    uvm_assert_mutex_locked(&va_block->lock);

    if (time_stamp > current_end_time_stamp) {
        NvU64 throttling_end_time_stamp = time_stamp + va_space_thrashing->params.nap_ns;
        page_thrashing_set_throttling_end_time_stamp(page_thrashing, throttling_end_time_stamp);

        // Avoid choosing the same processor in consecutive thrashing periods
        if (uvm_id_equal(page_thrashing->do_not_throttle_processor_id, processor))
            page_thrashing->do_not_throttle_processor_id = UVM_ID_INVALID;
        else
            page_thrashing->do_not_throttle_processor_id = processor;
    }
    else if (UVM_ID_IS_INVALID(page_thrashing->do_not_throttle_processor_id)) {
        page_thrashing->do_not_throttle_processor_id = processor;
    }
}

// Throttle the execution of a processor. If this is the first processor being
// throttled for a throttling period, compute the time stamp until which the
// rest of processors will be throttled on fault.
//
// - Page may be pinned (possible in thrashing due to revocation, such as
//   in system-wide atomics)
// - Requesting processor must not be throttled at this point.
//
static void thrashing_throttle_processor(uvm_va_block_t *va_block,
                                         block_thrashing_info_t *block_thrashing,
                                         page_thrashing_info_t *page_thrashing,
                                         uvm_page_index_t page_index,
                                         uvm_processor_id_t processor)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    NvU64 address = uvm_va_block_cpu_page_address(va_block, page_index);

    uvm_assert_mutex_locked(&va_block->lock);

    UVM_ASSERT(!uvm_id_equal(processor, page_thrashing->do_not_throttle_processor_id));

    if (!uvm_processor_mask_test_and_set(&page_thrashing->throttled_processors, processor)) {
        // CPU is throttled by sleeping. This is done in uvm_vm_fault so it
        // drops the VA block and VA space locks. Throttling start/end events
        // are recorded around the sleep calls.
        if (UVM_ID_IS_GPU(processor))
            uvm_tools_record_throttling_start(va_space, address, processor);

        if (!page_thrashing->pinned)
            UVM_PERF_SATURATING_INC(page_thrashing->throttling_count);

        UVM_PERF_SATURATING_INC(block_thrashing->throttling_count);
    }

    UVM_ASSERT(thrashing_state_checks(va_block, block_thrashing, page_thrashing, page_index));
}

// Stop throttling on the given processor. If this is the last processor being
// throttled for a throttling period, it will clear the throttling period.
//
// - Page may be pinned (possible in thrashing due to revocation, such as
//   in system-wide atomics)
// - Requesting processor must be throttled at this point.
//
static void thrashing_throttle_end_processor(uvm_va_block_t *va_block,
                                             block_thrashing_info_t *block_thrashing,
                                             page_thrashing_info_t *page_thrashing,
                                             uvm_page_index_t page_index,
                                             uvm_processor_id_t processor)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    NvU64 address = uvm_va_block_cpu_page_address(va_block, page_index);

    UVM_ASSERT(uvm_processor_mask_test(&page_thrashing->throttled_processors, processor));
    uvm_processor_mask_clear(&page_thrashing->throttled_processors, processor);
    if (uvm_processor_mask_empty(&page_thrashing->throttled_processors))
        page_thrashing_set_throttling_end_time_stamp(page_thrashing, 0);

    // See comment regarding throttling start/end events for CPU in
    // thrashing_throttle_processor
    if (UVM_ID_IS_GPU(processor))
        uvm_tools_record_throttling_end(va_space, address, processor);

    UVM_ASSERT(thrashing_state_checks(va_block, block_thrashing, page_thrashing, page_index));
}

// Clear the throttling state for all processors. This is used while
// transitioning to pinned state and during thrashing information reset.
static void thrashing_throttling_reset_page(uvm_va_block_t *va_block,
                                            block_thrashing_info_t *block_thrashing,
                                            page_thrashing_info_t *page_thrashing,
                                            uvm_page_index_t page_index)
{
    uvm_processor_id_t processor_id;

    for_each_id_in_mask(processor_id, &page_thrashing->throttled_processors) {
        thrashing_throttle_end_processor(va_block,
                                         block_thrashing,
                                         page_thrashing,
                                         page_index,
                                         processor_id);
    }

    UVM_ASSERT(uvm_processor_mask_empty(&page_thrashing->throttled_processors));
}

// Find the pinned page descriptor for the given page index. Return NULL if the
// page is not pinned.
static pinned_page_t *find_pinned_page(block_thrashing_info_t *block_thrashing, uvm_page_index_t page_index)
{
    pinned_page_t *pinned_page;

    list_for_each_entry(pinned_page, &block_thrashing->pinned_pages.list, va_block_list_entry) {
        if (pinned_page->page_index == page_index)
            return pinned_page;
    }

    return NULL;
}

// Pin a page on the specified processor. All thrashing processors will be
// mapped remotely on this location, when possible
//
// - Requesting processor cannot be throttled
//
static NV_STATUS thrashing_pin_page(va_space_thrashing_info_t *va_space_thrashing,
                                    uvm_va_block_t *va_block,
                                    uvm_va_block_context_t *va_block_context,
                                    block_thrashing_info_t *block_thrashing,
                                    page_thrashing_info_t *page_thrashing,
                                    uvm_page_index_t page_index,
                                    NvU64 time_stamp,
                                    uvm_processor_id_t residency,
                                    uvm_processor_id_t requester)
{
    uvm_processor_mask_t *current_residency = &va_block_context->scratch_processor_mask;

    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(!uvm_processor_mask_test(&page_thrashing->throttled_processors, requester));

    uvm_va_block_page_resident_processors(va_block, page_index, current_residency);

    // If we are pinning the page for the first time or we are pinning it on a
    // different location that the current location, reset the throttling state
    // to make sure that we flush any pending ThrottlingEnd events.
    if (!page_thrashing->pinned || !uvm_processor_mask_test(current_residency, residency))
        thrashing_throttling_reset_page(va_block, block_thrashing, page_thrashing, page_index);

    if (!page_thrashing->pinned) {
        if (va_space_thrashing->params.pin_ns > 0) {
            pinned_page_t *pinned_page = nv_kmem_cache_zalloc(g_pinned_page_cache, NV_UVM_GFP_FLAGS);
            if (!pinned_page)
                return NV_ERR_NO_MEMORY;

            pinned_page->va_block = va_block;
            pinned_page->page_index = page_index;
            pinned_page->deadline = time_stamp + va_space_thrashing->params.pin_ns;

            uvm_spin_lock(&va_space_thrashing->pinned_pages.lock);

            list_add_tail(&pinned_page->va_space_list_entry, &va_space_thrashing->pinned_pages.list);
            list_add_tail(&pinned_page->va_block_list_entry, &block_thrashing->pinned_pages.list);

            // We only schedule the delayed work if the list was empty before
            // adding this page. Otherwise, we just add it to the list. The
            // unpinning helper will remove from the list those pages with
            // deadline prior to its wakeup timestamp and will reschedule
            // itself if there are remaining pages in the list.
            if (list_is_singular(&va_space_thrashing->pinned_pages.list) &&
                !va_space_thrashing->pinned_pages.in_va_space_teardown) {
                int scheduled;
                scheduled = schedule_delayed_work(&va_space_thrashing->pinned_pages.dwork,
                                                  usecs_to_jiffies(va_space_thrashing->params.pin_ns / 1000));
                UVM_ASSERT(scheduled != 0);
            }

            uvm_spin_unlock(&va_space_thrashing->pinned_pages.lock);
        }

        page_thrashing->throttling_count = 0;
        page_thrashing->pinned = true;
        UVM_PERF_SATURATING_INC(block_thrashing->pinned_pages.count);
        uvm_page_mask_set(&block_thrashing->pinned_pages.mask, page_index);
    }

    page_thrashing->pinned_residency_id = residency;

    UVM_ASSERT(thrashing_state_checks(va_block, block_thrashing, page_thrashing, page_index));

    return NV_OK;
}

// Unpin a page. This function just clears the pinning tracking state, and does
// not remove remote mappings on the page. Callers will need to do it manually
// BEFORE calling this function, if so desired.
// - Page must be pinned
//
static void thrashing_unpin_page(va_space_thrashing_info_t *va_space_thrashing,
                                 uvm_va_block_t *va_block,
                                 block_thrashing_info_t *block_thrashing,
                                 page_thrashing_info_t *page_thrashing,
                                 uvm_page_index_t page_index)
{
    uvm_assert_mutex_locked(&va_block->lock);
    UVM_ASSERT(page_thrashing->pinned);

    if (va_space_thrashing->params.pin_ns > 0) {
        bool do_free = false;
        pinned_page_t *pinned_page = find_pinned_page(block_thrashing, page_index);

        UVM_ASSERT(pinned_page);
        UVM_ASSERT(pinned_page->page_index == page_index);
        UVM_ASSERT(pinned_page->va_block == va_block);

        // The va_space_list_entry and va_block_list_entry have special
        // meanings here:
        // - va_space_list_entry: when the delayed unpin worker removes the
        // pinned_page from this list, it takes the ownership of the page and
        // is in charge of freeing it.
        // - va_block_list_entry: by removing the page from this list,
        // thrashing_unpin_page tells the unpin delayed worker to skip
        // unpinning that page.
        uvm_spin_lock(&va_space_thrashing->pinned_pages.lock);
        list_del_init(&pinned_page->va_block_list_entry);

        if (!list_empty(&pinned_page->va_space_list_entry)) {
            do_free = true;
            list_del_init(&pinned_page->va_space_list_entry);

            if (list_empty(&va_space_thrashing->pinned_pages.list))
                cancel_delayed_work(&va_space_thrashing->pinned_pages.dwork);
        }

        uvm_spin_unlock(&va_space_thrashing->pinned_pages.lock);

        if (do_free)
            kmem_cache_free(g_pinned_page_cache, pinned_page);
    }

    page_thrashing->pinned_residency_id = UVM_ID_INVALID;
    page_thrashing->pinned = false;
    uvm_page_mask_clear(&block_thrashing->pinned_pages.mask, page_index);

    UVM_ASSERT(thrashing_state_checks(va_block, block_thrashing, page_thrashing, page_index));
}

static void thrashing_detected(uvm_va_block_t *va_block,
                               block_thrashing_info_t *block_thrashing,
                               page_thrashing_info_t *page_thrashing,
                               uvm_page_index_t page_index,
                               uvm_processor_id_t processor_id)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    NvU64 address = uvm_va_block_cpu_page_address(va_block, page_index);

    // Thrashing detected, record the event
    uvm_tools_record_thrashing(va_space, address, PAGE_SIZE, &page_thrashing->processors);
    if (!uvm_page_mask_test_and_set(&block_thrashing->thrashing_pages, page_index))
        ++block_thrashing->num_thrashing_pages;

    PROCESSOR_THRASHING_STATS_INC(processor_id, num_thrashing);

    UVM_ASSERT(thrashing_state_checks(va_block, block_thrashing, page_thrashing, page_index));
}

// Clear the thrashing information for the given page. This function does not
// unmap remote mappings on the page. Callers will need to do it BEFORE calling
// this function, if so desired
static void thrashing_reset_page(va_space_thrashing_info_t *va_space_thrashing,
                                 uvm_va_block_t *va_block,
                                 block_thrashing_info_t *block_thrashing,
                                 uvm_page_index_t page_index)
{
    page_thrashing_info_t *page_thrashing = &block_thrashing->pages[page_index];
    uvm_assert_mutex_locked(&va_block->lock);

    UVM_ASSERT(block_thrashing->num_thrashing_pages > 0);
    UVM_ASSERT(uvm_page_mask_test(&block_thrashing->thrashing_pages, page_index));
    UVM_ASSERT(page_thrashing->num_thrashing_events > 0);

    thrashing_throttling_reset_page(va_block, block_thrashing, page_thrashing, page_index);
    UVM_ASSERT(uvm_processor_mask_empty(&page_thrashing->throttled_processors));

    if (page_thrashing->pinned)
        thrashing_unpin_page(va_space_thrashing, va_block, block_thrashing, page_thrashing, page_index);

    page_thrashing->last_time_stamp       = 0;
    page_thrashing->has_migration_events  = 0;
    page_thrashing->has_revocation_events = 0;
    page_thrashing->num_thrashing_events  = 0;
    uvm_processor_mask_zero(&page_thrashing->processors);

    if (uvm_page_mask_test_and_clear(&block_thrashing->thrashing_pages, page_index))
        --block_thrashing->num_thrashing_pages;

    UVM_ASSERT(thrashing_state_checks(va_block, block_thrashing, page_thrashing, page_index));
}

// Call thrashing_reset_page for all the thrashing pages in the region
// described by address and bytes
static void thrashing_reset_pages_in_region(uvm_va_block_t *va_block, NvU64 address, NvU64 bytes)
{
    uvm_page_index_t page_index;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    va_space_thrashing_info_t *va_space_thrashing = va_space_thrashing_info_get(va_space);
    block_thrashing_info_t *block_thrashing = NULL;
    uvm_va_block_region_t region = uvm_va_block_region_from_start_size(va_block, address, bytes);

    block_thrashing = thrashing_info_get(va_block);
    if (!block_thrashing || !block_thrashing->pages)
        return;

    // Update all pages in the region
    for_each_va_block_page_in_region_mask(page_index, &block_thrashing->thrashing_pages, region)
        thrashing_reset_page(va_space_thrashing, va_block, block_thrashing, page_index);
}


// Unmap remote mappings from the given processors on the pinned pages
// described by region and block_thrashing->pinned pages.
static NV_STATUS unmap_remote_pinned_pages(uvm_va_block_t *va_block,
                                           uvm_va_block_context_t *va_block_context,
                                           block_thrashing_info_t *block_thrashing,
                                           uvm_va_block_region_t region,
                                           const uvm_processor_mask_t *unmap_processors)
{
    NV_STATUS status = NV_OK;
    NV_STATUS tracker_status;
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();
    uvm_processor_id_t processor_id;
    const uvm_va_policy_t *policy = uvm_va_policy_get(va_block, uvm_va_block_region_start(va_block, region));

    uvm_assert_mutex_locked(&va_block->lock);

    for_each_id_in_mask(processor_id, unmap_processors) {
        UVM_ASSERT(uvm_id_equal(processor_id, policy->preferred_location) ||
                   !uvm_processor_mask_test(&policy->accessed_by, processor_id));

        if (uvm_processor_mask_test(&va_block->resident, processor_id)) {
            const uvm_page_mask_t *resident_mask = uvm_va_block_resident_mask_get(va_block, processor_id, NUMA_NO_NODE);

            if (!uvm_page_mask_andnot(&va_block_context->caller_page_mask,
                                      &block_thrashing->pinned_pages.mask,
                                      resident_mask))
                continue;
        }
        else {
            uvm_page_mask_copy(&va_block_context->caller_page_mask, &block_thrashing->pinned_pages.mask);
        }

        status = uvm_va_block_unmap(va_block,
                                    va_block_context,
                                    processor_id,
                                    region,
                                    &va_block_context->caller_page_mask,
                                    &local_tracker);
        if (status != NV_OK)
            break;
    }

    tracker_status = uvm_tracker_add_tracker_safe(&va_block->tracker, &local_tracker);
    if (status == NV_OK)
        status = tracker_status;

    uvm_tracker_deinit(&local_tracker);

    return status;
}

NV_STATUS uvm_perf_thrashing_unmap_remote_pinned_pages_all(uvm_va_block_t *va_block,
                                                           uvm_va_block_context_t *va_block_context,
                                                           uvm_va_block_region_t region)
{
    block_thrashing_info_t *block_thrashing;
    uvm_processor_mask_t *unmap_processors = &va_block_context->unmap_processors_mask;
    const uvm_va_policy_t *policy = uvm_va_policy_get_region(va_block, region);

    uvm_assert_mutex_locked(&va_block->lock);

    block_thrashing = thrashing_info_get(va_block);
    if (!block_thrashing || !block_thrashing->pages)
        return NV_OK;

    if (uvm_page_mask_empty(&block_thrashing->pinned_pages.mask))
        return NV_OK;

    // Unmap all mapped processors (that are not SetAccessedBy) with
    // no copy of the page
    uvm_processor_mask_andnot(unmap_processors, &va_block->mapped, &policy->accessed_by);

    return unmap_remote_pinned_pages(va_block, va_block_context, block_thrashing, region, unmap_processors);
}

// Check that we are not migrating pages away from its pinned location and
// that we are not prefetching thrashing pages.
static bool migrating_wrong_pages(uvm_va_block_t *va_block,
                                  NvU64 address,
                                  NvU64 bytes,
                                  uvm_processor_id_t proc_id,
                                  uvm_make_resident_cause_t cause)
{
    uvm_page_index_t page_index;
    block_thrashing_info_t *block_thrashing = NULL;
    uvm_va_block_region_t region = uvm_va_block_region_from_start_size(va_block, address, bytes);

    block_thrashing = thrashing_info_get(va_block);
    if (!block_thrashing || !block_thrashing->pages)
        return false;

    for_each_va_block_page_in_region(page_index, region) {
        page_thrashing_info_t *page_thrashing = &block_thrashing->pages[page_index];
        UVM_ASSERT_MSG(!page_thrashing->pinned || uvm_id_equal(proc_id, page_thrashing->pinned_residency_id),
                       "Migrating to %u instead of %u\n",
                       uvm_id_value(proc_id), uvm_id_value(page_thrashing->pinned_residency_id));
        if (cause == UVM_MAKE_RESIDENT_CAUSE_PREFETCH)
            UVM_ASSERT(!uvm_page_mask_test(&block_thrashing->thrashing_pages, page_index));
    }

    return false;
}

static bool is_migration_pinned_pages_update(uvm_va_block_t *va_block,
                                             const uvm_perf_event_data_t *event_data,
                                             NvU64 address,
                                             NvU64 bytes)
{
    const block_thrashing_info_t *block_thrashing = NULL;
    uvm_va_block_region_t region = uvm_va_block_region_from_start_size(va_block, address, bytes);
    bool ret;

    if (event_data->migration.cause != UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT &&
        event_data->migration.cause != UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER) {
        return false;
    }

    block_thrashing = thrashing_info_get(va_block);
    if (!block_thrashing || !block_thrashing->pages)
        return false;

    ret = uvm_page_mask_region_full(&block_thrashing->pinned_pages.mask, region);
    if (ret) {
        uvm_page_index_t page_index;
        for_each_va_block_page_in_region(page_index, region) {
            page_thrashing_info_t *page_thrashing = &block_thrashing->pages[page_index];
            UVM_ASSERT(uvm_id_equal(page_thrashing->pinned_residency_id, event_data->migration.dst));
        }
    }

    return ret;
}

// This function processes migration/revocation events and determines if the
// affected pages are thrashing or not.
void thrashing_event_cb(uvm_perf_event_t event_id, uvm_perf_event_data_t *event_data)
{
    va_space_thrashing_info_t *va_space_thrashing;
    block_thrashing_info_t *block_thrashing = NULL;
    uvm_va_block_t *va_block;
    uvm_va_space_t *va_space;
    NvU64 address;
    NvU64 bytes;
    uvm_processor_id_t processor_id;
    uvm_page_index_t page_index;
    NvU64 time_stamp;
    uvm_va_block_region_t region;
    uvm_read_duplication_policy_t read_duplication;

    UVM_ASSERT(g_uvm_perf_thrashing_enable);

    UVM_ASSERT(event_id == UVM_PERF_EVENT_MIGRATION || event_id == UVM_PERF_EVENT_REVOCATION);

    if (event_id == UVM_PERF_EVENT_MIGRATION) {
        va_block     = event_data->migration.block;
        address      = event_data->migration.address;
        bytes        = event_data->migration.bytes;
        processor_id = event_data->migration.dst;

        // Skip the thrashing detection logic on eviction as we cannot take
        // the VA space lock
        if (event_data->migration.cause == UVM_MAKE_RESIDENT_CAUSE_EVICTION)
            return;

        // Do not perform checks during the first part of staging copies
        if (!uvm_id_equal(event_data->migration.dst, event_data->migration.make_resident_context->dest_id))
            return;

        va_space = uvm_va_block_get_va_space(va_block);
        va_space_thrashing = va_space_thrashing_info_get(va_space);
        if (!va_space_thrashing->params.enable)
            return;

        // TODO: Bug 3660922: HMM will need to look up the policy when
        // read duplication is supported.
        read_duplication = uvm_va_block_is_hmm(va_block) ?
                           UVM_READ_DUPLICATION_UNSET :
                           va_block->managed_range->policy.read_duplication;

        // We only care about migrations due to replayable faults, access
        // counters and page prefetching. For non-replayable faults, UVM will
        // try not to migrate memory since CE is transferring data anyway.
        // However, we can still see migration events due to initial
        // population. The rest of migrations are triggered due to user
        // commands or advice (such as read duplication) which takes precedence
        // over our heuristics. Therefore, we clear our internal tracking
        // state.
        if ((event_data->migration.cause != UVM_MAKE_RESIDENT_CAUSE_REPLAYABLE_FAULT &&
             event_data->migration.cause != UVM_MAKE_RESIDENT_CAUSE_ACCESS_COUNTER &&
             event_data->migration.cause != UVM_MAKE_RESIDENT_CAUSE_PREFETCH) ||
            (event_data->migration.transfer_mode != UVM_VA_BLOCK_TRANSFER_MODE_MOVE) ||
            (read_duplication == UVM_READ_DUPLICATION_ENABLED)) {
            thrashing_reset_pages_in_region(va_block, address, bytes);
            return;
        }

        // Assert that we are not migrating pages that are pinned away from
        // their pinning residency, or prefetching pages that are thrashing
        UVM_ASSERT(!migrating_wrong_pages(va_block, address, bytes, processor_id, event_data->migration.cause));

        // If we are being migrated due to pinning just return
        if (is_migration_pinned_pages_update(va_block, event_data, address, bytes))
            return;
    }
    else {
        va_block     = event_data->revocation.block;
        address      = event_data->revocation.address;
        bytes        = event_data->revocation.bytes;
        processor_id = event_data->revocation.proc_id;

        va_space = uvm_va_block_get_va_space(va_block);
        va_space_thrashing = va_space_thrashing_info_get(va_space);
        if (!va_space_thrashing->params.enable)
            return;
    }

    block_thrashing = thrashing_info_get_create(va_block);
    if (!block_thrashing)
        return;

    time_stamp = NV_GETTIME();

    if (!block_thrashing->pages) {
        // Don't create the per-page tracking structure unless there is some potential thrashing within the block
        NvU16 num_block_pages;

        if (block_thrashing->last_time_stamp == 0 ||
            uvm_id_equal(block_thrashing->last_processor, processor_id) ||
            time_stamp - block_thrashing->last_time_stamp > va_space_thrashing->params.lapse_ns)
            goto done;

        num_block_pages = uvm_va_block_size(va_block) / PAGE_SIZE;

        block_thrashing->pages = uvm_kvmalloc_zero(sizeof(*block_thrashing->pages) * num_block_pages);
        if (!block_thrashing->pages)
            goto done;

        for (page_index = 0; page_index < num_block_pages; ++page_index) {
            block_thrashing->pages[page_index].pinned_residency_id = UVM_ID_INVALID;
            block_thrashing->pages[page_index].do_not_throttle_processor_id = UVM_ID_INVALID;
        }
    }

    region = uvm_va_block_region_from_start_size(va_block, address, bytes);

    // Update all pages in the region
    for_each_va_block_page_in_region(page_index, region) {
        page_thrashing_info_t *page_thrashing = &block_thrashing->pages[page_index];
        NvU64 last_time_stamp = page_thrashing_get_time_stamp(page_thrashing);

        // It is not possible that a pinned page is migrated here, since the
        // fault that triggered the migration should have unpinned it in its
        // call to uvm_perf_thrashing_get_hint. Moreover page prefetching never
        // includes pages that are thrashing (including pinning)
        if (event_id == UVM_PERF_EVENT_MIGRATION)
            UVM_ASSERT(page_thrashing->pinned == 0);

        uvm_processor_mask_set(&page_thrashing->processors, processor_id);
        page_thrashing_set_time_stamp(page_thrashing, time_stamp);

        if (last_time_stamp == 0)
            continue;

        if (time_stamp - last_time_stamp <= va_space_thrashing->params.lapse_ns) {
            UVM_PERF_SATURATING_INC(page_thrashing->num_thrashing_events);
            if (page_thrashing->num_thrashing_events == va_space_thrashing->params.threshold)
                thrashing_detected(va_block, block_thrashing, page_thrashing, page_index, processor_id);

            if (page_thrashing->num_thrashing_events >= va_space_thrashing->params.threshold)
                block_thrashing->last_thrashing_time_stamp = time_stamp;

            if (event_id == UVM_PERF_EVENT_MIGRATION)
                page_thrashing->has_migration_events = true;
            else
                page_thrashing->has_revocation_events = true;
        }
        else if (page_thrashing->num_thrashing_events >= va_space_thrashing->params.threshold &&
                 !page_thrashing->pinned) {
            thrashing_reset_page(va_space_thrashing, va_block, block_thrashing, page_index);
        }
    }

done:
    block_thrashing->last_time_stamp = time_stamp;
    block_thrashing->last_processor  = processor_id;
}

static bool thrashing_processors_can_access(uvm_va_space_t *va_space,
                                            page_thrashing_info_t *page_thrashing,
                                            uvm_processor_id_t to)
{
    if (UVM_ID_IS_INVALID(to))
        return false;

    return uvm_processor_mask_subset(&page_thrashing->processors,
                                     &va_space->accessible_from[uvm_id_value(to)]);
}

static bool thrashing_processors_have_fast_access_to(uvm_va_space_t *va_space,
                                                     uvm_va_block_context_t *va_block_context,
                                                     page_thrashing_info_t *page_thrashing,
                                                     uvm_processor_id_t to)
{
    uvm_processor_mask_t *fast_to = &va_block_context->fast_access_mask;

    if (UVM_ID_IS_INVALID(to))
        return false;

    // Combine NVLINK/C2C and native atomics mask since we could have PCIe
    // atomics in the future
    uvm_processor_mask_and(fast_to,
                           &va_space->has_fast_link[uvm_id_value(to)],
                           &va_space->has_native_atomics[uvm_id_value(to)]);
    if (UVM_ID_IS_CPU(to)) {
        uvm_processor_mask_set(fast_to, to);
    }
    else {
        // Include all SMC peers and the processor 'to'.
        // This includes SMC peers that are not registered.
        // Since not-registered peers cannot be in page_thrashing->processors,
        // the value of their respective bits in "fast_to" doesn't matter.
        uvm_processor_mask_range_fill(fast_to,
                                      uvm_gpu_id_from_sub_processor(uvm_parent_gpu_id_from_gpu_id(to), 0),
                                      UVM_PARENT_ID_MAX_SUB_PROCESSORS);
    }

    return uvm_processor_mask_subset(&page_thrashing->processors, fast_to);
}

static void thrashing_processors_common_locations(uvm_va_space_t *va_space,
                                                  page_thrashing_info_t *page_thrashing,
                                                  uvm_processor_mask_t *common_locations)
{
    bool is_first = true;
    uvm_processor_id_t id;

    // Find processors that can be accessed from all thrashing processors. For
    // example: if A, B and C are thrashing, and A can access B and C can access
    // B, too, B would be the common location.
    uvm_processor_mask_zero(common_locations);

    for_each_id_in_mask(id, &page_thrashing->processors) {
        if (is_first)
            uvm_processor_mask_copy(common_locations, &va_space->can_access[uvm_id_value(id)]);
        else
            uvm_processor_mask_and(common_locations, common_locations, &va_space->can_access[uvm_id_value(id)]);

        is_first = false;
    }
}

static bool preferred_location_is_thrashing(uvm_processor_id_t preferred_location,
                                            page_thrashing_info_t *page_thrashing)
{
    if (UVM_ID_IS_INVALID(preferred_location))
        return false;

    return uvm_processor_mask_test(&page_thrashing->processors, preferred_location);
}

static uvm_perf_thrashing_hint_t get_hint_for_migration_thrashing(va_space_thrashing_info_t *va_space_thrashing,
                                                                  uvm_va_block_t *va_block,
                                                                  uvm_va_block_context_t *va_block_context,
                                                                  uvm_page_index_t page_index,
                                                                  page_thrashing_info_t *page_thrashing,
                                                                  uvm_processor_id_t requester)
{
    uvm_perf_thrashing_hint_t hint;
    uvm_processor_id_t closest_resident_id;
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    uvm_processor_id_t do_not_throttle_processor = page_thrashing->do_not_throttle_processor_id;
    uvm_processor_id_t pinned_residency = page_thrashing->pinned_residency_id;
    const uvm_va_policy_t *policy;
    uvm_processor_id_t preferred_location;

    policy = uvm_va_policy_get(va_block, uvm_va_block_cpu_page_address(va_block, page_index));

    preferred_location = policy->preferred_location;

    hint.type = UVM_PERF_THRASHING_HINT_TYPE_NONE;

    closest_resident_id = uvm_va_block_page_get_closest_resident(va_block, va_block_context, page_index, requester);
    if (uvm_va_block_is_hmm(va_block)) {
        // HMM pages always start out resident on the CPU but may not be
        // recorded in the va_block state because hmm_range_fault() or
        // similar functions haven't been called to get an accurate snapshot
        // of the Linux state. We can assume pages are CPU resident for the
        // purpose of deciding where to migrate to reduce thrashing.
        if (UVM_ID_IS_INVALID(closest_resident_id))
            closest_resident_id = UVM_ID_CPU;
    }
    else {
        UVM_ASSERT(UVM_ID_IS_VALID(closest_resident_id));
    }

    if (thrashing_processors_can_access(va_space, page_thrashing, preferred_location)) {
        // The logic in uvm_va_block_select_residency chooses the preferred
        // location if the requester can access it, so all processors should
        // naturally get mapped to the preferred without thrashing. However,
        // we can get here if preferred location was set after processors
        // started thrashing.
        //
        // TODO: Bug 2527408. Reset thrashing history when a user policy
        //       changes in a VA block.
        hint.type = UVM_PERF_THRASHING_HINT_TYPE_PIN;
        hint.pin.residency = preferred_location;
    }
    else if (!preferred_location_is_thrashing(preferred_location, page_thrashing) &&
             thrashing_processors_have_fast_access_to(va_space, va_block_context, page_thrashing, closest_resident_id)){
        // This is a fast path for those scenarios in which all thrashing
        // processors have fast access (NVLINK + native atomics or SMC peers)
        // to the current residency. This is skipped if the preferred location
        // is thrashing and not accessible by the rest of thrashing processors.
        // Otherwise, we would be in the condition above.
        if (UVM_ID_IS_CPU(closest_resident_id)) {
            // On P9 systems, we prefer the CPU to map vidmem (since it can
            // cache it), so don't map the GPU to sysmem.
            if (UVM_ID_IS_GPU(requester)) {
                hint.type = UVM_PERF_THRASHING_HINT_TYPE_PIN;
                hint.pin.residency = requester;
            }
        }
        else {
            hint.type = UVM_PERF_THRASHING_HINT_TYPE_PIN;
            hint.pin.residency = closest_resident_id;
        }
    }
    else if (uvm_id_equal(requester, preferred_location)) {
        if (page_thrashing->pinned) {
            // If the faulting processor is the preferred location, we can
            // only:
            // 1) Pin to the preferred location
            // 2) Throttle if it's pinned elsewhere and we are not the
            //    do_not_throttle_processor
            if (uvm_id_equal(preferred_location, pinned_residency) ||
                uvm_id_equal(preferred_location, do_not_throttle_processor)) {
                hint.type = UVM_PERF_THRASHING_HINT_TYPE_PIN;
                hint.pin.residency = preferred_location;
            }
            else {
                hint.type = UVM_PERF_THRASHING_HINT_TYPE_THROTTLE;
            }
        }
        else if (!uvm_id_equal(preferred_location, do_not_throttle_processor)) {
            hint.type = UVM_PERF_THRASHING_HINT_TYPE_THROTTLE;
        }
        else if (page_thrashing->throttling_count >= va_space_thrashing->params.pin_threshold) {
            hint.type = UVM_PERF_THRASHING_HINT_TYPE_PIN;
            hint.pin.residency = preferred_location;
        }
    }
    else if (page_thrashing->pinned) {
        // 1) If the requester is the do_not_throttle_processor pin it to the
        //    requester if all thrashing processors can access the requester,
        //    or to a common location, or to the requester anyway if no common
        //    location found.
        // 2) Try to map the current pinned residency.
        // 3) Throttle.
        if (uvm_id_equal(requester, do_not_throttle_processor)) {
            hint.type = UVM_PERF_THRASHING_HINT_TYPE_PIN;

            if (thrashing_processors_can_access(va_space, page_thrashing, requester)) {
                hint.pin.residency = requester;
            }
            else {
                uvm_processor_mask_t *common_locations = &va_block_context->scratch_processor_mask;

                thrashing_processors_common_locations(va_space, page_thrashing, common_locations);
                if (uvm_processor_mask_empty(common_locations)) {
                    hint.pin.residency = requester;
                }
                else {
                    // Find the common location that is closest to the requester
                    hint.pin.residency = uvm_processor_mask_find_closest_id(va_space, common_locations, requester);
                }
            }
        }
        else if (uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(pinned_residency)], requester)) {
            if (!uvm_va_block_is_hmm(va_block))
                UVM_ASSERT(uvm_id_equal(closest_resident_id, pinned_residency));

            hint.type = UVM_PERF_THRASHING_HINT_TYPE_PIN;
            hint.pin.residency = pinned_residency;
        }
        else {
            hint.type = UVM_PERF_THRASHING_HINT_TYPE_THROTTLE;
        }
    }
    else if (!uvm_id_equal(requester, do_not_throttle_processor)) {
        hint.type = UVM_PERF_THRASHING_HINT_TYPE_THROTTLE;
    }
    else if (page_thrashing->throttling_count >= va_space_thrashing->params.pin_threshold) {
        hint.type = UVM_PERF_THRASHING_HINT_TYPE_PIN;
        hint.pin.residency = requester;
    }

    if (hint.type == UVM_PERF_THRASHING_HINT_TYPE_PIN && !uvm_processor_has_memory(hint.pin.residency))
        hint.pin.residency = UVM_ID_CPU;

    return hint;
}

// Function called on fault that tells the fault handler if any operation
// should be performed to minimize thrashing. The logic is as follows:
//
// - Phase0: Block thrashing. If a number of consecutive thrashing events have
//   been detected on the VA block, per-page thrashing tracking information is
//   created.
// - Phase1: Throttling. When several processors fight over a page, we start a
//   "throttling period". During that period, only one processor will be able
//   to service faults on the page, and the rest will be throttled. All CPU
//   faults are considered to belong to the same device, even if they come from
//   different CPU threads.
// - Phase2: Pinning. After a number of consecutive throttling periods, the page
//   is pinned on a specific processor which all of the thrashing processors can
//   access.
// - Phase3: Revocation throttling. Even if the page is pinned, it can be still
//   thrashing due to revocation events (mainly due to system-wide atomics). In
//   that case we keep the page pinned while applying the same algorithm as in
//   Phase1.
uvm_perf_thrashing_hint_t uvm_perf_thrashing_get_hint(uvm_va_block_t *va_block,
                                                      uvm_va_block_context_t *va_block_context,
                                                      NvU64 address,
                                                      uvm_processor_id_t requester)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    va_space_thrashing_info_t *va_space_thrashing = va_space_thrashing_info_get(va_space);
    block_thrashing_info_t *block_thrashing = NULL;
    page_thrashing_info_t *page_thrashing = NULL;
    uvm_perf_thrashing_hint_t hint;
    uvm_page_index_t page_index = uvm_va_block_cpu_page_index(va_block, address);
    NvU64 time_stamp;
    NvU64 last_time_stamp;

    hint.type = UVM_PERF_THRASHING_HINT_TYPE_NONE;

    if (!va_space_thrashing->params.enable)
        return hint;

    // If we don't have enough memory to store thrashing information, we assume
    // no thrashing
    block_thrashing = thrashing_info_get(va_block);
    if (!block_thrashing)
        return hint;

    // If the per-page tracking structure has not been created yet, we assume
    // no thrashing
    if (!block_thrashing->pages)
        return hint;

    time_stamp = NV_GETTIME();

    if (block_thrashing->last_thrashing_time_stamp != 0 &&
        (time_stamp - block_thrashing->last_thrashing_time_stamp > va_space_thrashing->params.epoch_ns) &&
        block_thrashing->pinned_pages.count == 0 &&
        block_thrashing->thrashing_reset_count < va_space_thrashing->params.max_resets) {
        uvm_page_index_t reset_page_index;

        ++block_thrashing->thrashing_reset_count;

        // Clear the state of throttled processors to make sure that we flush
        // any pending ThrottlingEnd events
        for_each_va_block_page_in_mask(reset_page_index, &block_thrashing->thrashing_pages, va_block) {
            thrashing_throttling_reset_page(va_block,
                                            block_thrashing,
                                            &block_thrashing->pages[reset_page_index],
                                            reset_page_index);
        }

        // Reset per-page tracking structure
        // TODO: Bug 1769904 [uvm] Speculatively unpin pages that were pinned on a specific memory due to thrashing
        UVM_ASSERT(uvm_page_mask_empty(&block_thrashing->pinned_pages.mask));
        uvm_kvfree(block_thrashing->pages);
        block_thrashing->pages                     = NULL;
        block_thrashing->num_thrashing_pages       = 0;
        block_thrashing->last_processor            = UVM_ID_INVALID;
        block_thrashing->last_time_stamp           = 0;
        block_thrashing->last_thrashing_time_stamp = 0;
        uvm_page_mask_zero(&block_thrashing->thrashing_pages);
        goto done;
    }

    page_thrashing = &block_thrashing->pages[page_index];

    // Not enough thrashing events yet
    if (page_thrashing->num_thrashing_events < va_space_thrashing->params.threshold)
        goto done;

    // If the requesting processor is throttled, check the throttling end time
    // stamp
    if (uvm_processor_mask_test(&page_thrashing->throttled_processors, requester)) {
        NvU64 throttling_end_time_stamp = page_thrashing_get_throttling_end_time_stamp(page_thrashing);
        if (time_stamp < throttling_end_time_stamp &&
            !uvm_id_equal(requester, page_thrashing->do_not_throttle_processor_id)) {
            hint.type = UVM_PERF_THRASHING_HINT_TYPE_THROTTLE;
            goto done;
        }

        thrashing_throttle_end_processor(va_block, block_thrashing, page_thrashing, page_index, requester);
    }

    UVM_ASSERT(!uvm_processor_mask_test(&page_thrashing->throttled_processors, requester));

    last_time_stamp = page_thrashing_get_time_stamp(page_thrashing);

    // If the lapse since the last thrashing event is longer than a thrashing
    // lapse we are no longer thrashing
    if (time_stamp - last_time_stamp > va_space_thrashing->params.lapse_ns &&
        !page_thrashing->pinned) {
        goto done;
    }

    // Set the requesting processor in the thrashing processors mask
    uvm_processor_mask_set(&page_thrashing->processors, requester);

    UVM_ASSERT(page_thrashing->has_migration_events || page_thrashing->has_revocation_events);

    // Update throttling heuristics
    thrashing_throttle_update(va_space_thrashing, va_block, page_thrashing, requester, time_stamp);

    if (page_thrashing->pinned &&
        page_thrashing->has_revocation_events &&
        !uvm_id_equal(requester, page_thrashing->do_not_throttle_processor_id)) {

        // When we get revocation thrashing, this is due to system-wide atomics
        // downgrading the permissions of other processors. Revocations only
        // happen when several processors are mapping the same page and there
        // are no migrations. In this case, the only thing we can do is to
        // throttle the execution of the processors.
        hint.type = UVM_PERF_THRASHING_HINT_TYPE_THROTTLE;
    }
    else {
        hint = get_hint_for_migration_thrashing(va_space_thrashing,
                                                va_block,
                                                va_block_context,
                                                page_index,
                                                page_thrashing,
                                                requester);
    }

done:
    if (hint.type == UVM_PERF_THRASHING_HINT_TYPE_PIN) {
        NV_STATUS status = thrashing_pin_page(va_space_thrashing,
                                              va_block,
                                              va_block_context,
                                              block_thrashing,
                                              page_thrashing,
                                              page_index,
                                              time_stamp,
                                              hint.pin.residency,
                                              requester);

        // If there was some problem pinning the page (i.e. OOM), demote to
        // throttling)
        if (status != NV_OK) {
            hint.type = UVM_PERF_THRASHING_HINT_TYPE_THROTTLE;
        }
        else {
            if (uvm_id_equal(hint.pin.residency, requester))
                PROCESSOR_THRASHING_STATS_INC(requester, num_pin_local);
            else
                PROCESSOR_THRASHING_STATS_INC(requester, num_pin_remote);

            uvm_processor_mask_copy(&hint.pin.processors, &page_thrashing->processors);
        }
    }

    if (hint.type == UVM_PERF_THRASHING_HINT_TYPE_THROTTLE) {
        thrashing_throttle_processor(va_block,
                                     block_thrashing,
                                     page_thrashing,
                                     page_index,
                                     requester);

        PROCESSOR_THRASHING_STATS_INC(requester, num_throttle);

        hint.throttle.end_time_stamp = page_thrashing_get_throttling_end_time_stamp(page_thrashing);
    }
    else if (hint.type == UVM_PERF_THRASHING_HINT_TYPE_NONE && page_thrashing) {
        UVM_ASSERT(!uvm_processor_mask_test(&page_thrashing->throttled_processors, requester));
        UVM_ASSERT(!page_thrashing->pinned);
        UVM_ASSERT(UVM_ID_IS_INVALID(page_thrashing->pinned_residency_id));
    }

    return hint;
}

uvm_processor_mask_t *uvm_perf_thrashing_get_thrashing_processors(uvm_va_block_t *va_block, NvU64 address)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    va_space_thrashing_info_t *va_space_thrashing = va_space_thrashing_info_get(va_space);
    block_thrashing_info_t *block_thrashing = NULL;
    page_thrashing_info_t *page_thrashing = NULL;
    uvm_page_index_t page_index = uvm_va_block_cpu_page_index(va_block, address);

    UVM_ASSERT(g_uvm_perf_thrashing_enable);
    UVM_ASSERT(va_space_thrashing->params.enable);

    block_thrashing = thrashing_info_get(va_block);
    UVM_ASSERT(block_thrashing);

    UVM_ASSERT(block_thrashing->pages);

    page_thrashing = &block_thrashing->pages[page_index];

    return &page_thrashing->processors;
}

const uvm_page_mask_t *uvm_perf_thrashing_get_thrashing_pages(uvm_va_block_t *va_block)
{
    uvm_va_space_t *va_space = uvm_va_block_get_va_space(va_block);
    va_space_thrashing_info_t *va_space_thrashing = va_space_thrashing_info_get(va_space);
    block_thrashing_info_t *block_thrashing = NULL;

    if (!va_space_thrashing->params.enable)
        return NULL;

    block_thrashing = thrashing_info_get(va_block);
    if (!block_thrashing)
        return NULL;

    if (block_thrashing->num_thrashing_pages == 0)
        return NULL;

    return &block_thrashing->thrashing_pages;
}

#define TIMER_GRANULARITY_NS 20000ULL
static void thrashing_unpin_pages(struct work_struct *work)
{
    struct delayed_work *dwork = to_delayed_work(work);
    va_space_thrashing_info_t *va_space_thrashing = container_of(dwork, va_space_thrashing_info_t, pinned_pages.dwork);
    uvm_va_space_t *va_space = va_space_thrashing->va_space;
    uvm_va_block_context_t *va_block_context = va_space_thrashing->pinned_pages.va_block_context;

    // Take the VA space lock so that VA blocks don't go away during this
    // operation.
    uvm_va_space_down_read(va_space);

    if (va_space_thrashing->pinned_pages.in_va_space_teardown)
        goto exit_no_list_lock;

    while (1) {
        pinned_page_t *pinned_page;
        uvm_va_block_t *va_block;

        uvm_spin_lock(&va_space_thrashing->pinned_pages.lock);
        pinned_page = list_first_entry_or_null(&va_space_thrashing->pinned_pages.list,
                                               pinned_page_t,
                                               va_space_list_entry);

        if (pinned_page) {
            NvU64 now = NV_GETTIME();

            if (pinned_page->deadline <= (now + TIMER_GRANULARITY_NS)) {
                list_del_init(&pinned_page->va_space_list_entry);

                // Work cancellation is left to thrashing_unpin_page() as this
                // would only catch the following pattern:
                // - Worker thread A is in thrashing_unpin_pages but hasn't
                // looked at the list yet
                // - Thread B then removes the last entry
                // - Thread C then adds a new entry and re-schedules work
                // - Worker thread A removes the entry added by C because the
                // deadline has passed (unlikely), then cancels the work
                // scheduled by C.
            }
            else {
                NvU64 elapsed_us = (pinned_page->deadline - now) / 1000;

                schedule_delayed_work(&va_space_thrashing->pinned_pages.dwork, usecs_to_jiffies(elapsed_us));
                uvm_spin_unlock(&va_space_thrashing->pinned_pages.lock);
                break;
            }
        }

        uvm_spin_unlock(&va_space_thrashing->pinned_pages.lock);

        if (!pinned_page)
            break;

        va_block = pinned_page->va_block;
        if (uvm_va_block_is_hmm(va_block))
            uvm_hmm_migrate_begin_wait(va_block);
        uvm_mutex_lock(&va_block->lock);

        // Only operate if the pinned page's tracking state isn't already
        // cleared by thrashing_unpin_page()
        if (!list_empty(&pinned_page->va_block_list_entry)) {
            uvm_page_index_t page_index = pinned_page->page_index;
            block_thrashing_info_t *block_thrashing = thrashing_info_get(va_block);

            UVM_ASSERT(block_thrashing);
            UVM_ASSERT(uvm_page_mask_test(&block_thrashing->pinned_pages.mask, page_index));

            uvm_va_block_context_init(va_block_context, NULL);

            uvm_perf_thrashing_unmap_remote_pinned_pages_all(va_block,
                                                             va_block_context,
                                                             uvm_va_block_region_for_page(page_index));
            thrashing_reset_page(va_space_thrashing, va_block, block_thrashing, page_index);
        }

        uvm_mutex_unlock(&va_block->lock);
        if (uvm_va_block_is_hmm(va_block))
            uvm_hmm_migrate_finish(va_block);
        kmem_cache_free(g_pinned_page_cache, pinned_page);
    }

exit_no_list_lock:
    uvm_va_space_up_read(va_space);
}

static void thrashing_unpin_pages_entry(struct work_struct *work)
{
    UVM_ENTRY_VOID(thrashing_unpin_pages(work));
}

NV_STATUS uvm_perf_thrashing_load(uvm_va_space_t *va_space)
{
    va_space_thrashing_info_t *va_space_thrashing;
    NV_STATUS status;

    status = uvm_perf_module_load(&g_module_thrashing, va_space);
    if (status != NV_OK)
        return status;

    va_space_thrashing = va_space_thrashing_info_create(va_space);
    if (!va_space_thrashing)
        return NV_ERR_NO_MEMORY;

    uvm_spin_lock_init(&va_space_thrashing->pinned_pages.lock, UVM_LOCK_ORDER_LEAF);
    INIT_LIST_HEAD(&va_space_thrashing->pinned_pages.list);
    INIT_DELAYED_WORK(&va_space_thrashing->pinned_pages.dwork, thrashing_unpin_pages_entry);

    return NV_OK;
}

void uvm_perf_thrashing_stop(uvm_va_space_t *va_space)
{
    va_space_thrashing_info_t *va_space_thrashing;

    uvm_va_space_down_write(va_space);
    va_space_thrashing = va_space_thrashing_info_get_or_null(va_space);

    // Prevent further unpinning operations from being scheduled
    if (va_space_thrashing)
        va_space_thrashing->pinned_pages.in_va_space_teardown = true;

    uvm_va_space_up_write(va_space);

    // Cancel any pending work. We can safely access va_space_thrashing
    // because this function is called once from the VA space teardown path,
    // and the only function that frees it is uvm_perf_thrashing_unload,
    // which is called later in the teardown path.
    if (va_space_thrashing)
        (void)cancel_delayed_work_sync(&va_space_thrashing->pinned_pages.dwork);
}

void uvm_perf_thrashing_unload(uvm_va_space_t *va_space)
{
    va_space_thrashing_info_t *va_space_thrashing = va_space_thrashing_info_get_or_null(va_space);

    uvm_perf_module_unload(&g_module_thrashing, va_space);

    // Make sure that there are not pending work items
    if (va_space_thrashing) {
        UVM_ASSERT(list_empty(&va_space_thrashing->pinned_pages.list));

        va_space_thrashing_info_destroy(va_space);
    }
}

void uvm_perf_thrashing_register_gpu(uvm_va_space_t *va_space, uvm_gpu_t *gpu)
{
    va_space_thrashing_info_t *va_space_thrashing = va_space_thrashing_info_get(va_space);

    // If a simulated GPU is registered, re-initialize thrashing parameters in
    // case they need to be adjusted.
    if ((g_uvm_global.num_simulated_devices > 0) && !va_space_thrashing->params.test_overrides)
        va_space_thrashing_info_init_params(va_space_thrashing);
}

NV_STATUS uvm_perf_thrashing_init(void)
{
    NV_STATUS status;

    INIT_THRASHING_PARAMETER_TOGGLE(uvm_perf_thrashing_enable, UVM_PERF_THRASHING_ENABLE_DEFAULT);
    if (!g_uvm_perf_thrashing_enable)
        return NV_OK;

    uvm_perf_module_init("perf_thrashing",
                         UVM_PERF_MODULE_TYPE_THRASHING,
                         g_callbacks_thrashing,
                         ARRAY_SIZE(g_callbacks_thrashing),
                         &g_module_thrashing);

    INIT_THRASHING_PARAMETER_NONZERO_MAX(uvm_perf_thrashing_threshold,
                                         UVM_PERF_THRASHING_THRESHOLD_DEFAULT,
                                         UVM_PERF_THRASHING_THRESHOLD_MAX);

    INIT_THRASHING_PARAMETER_NONZERO_MAX(uvm_perf_thrashing_pin_threshold,
                                         UVM_PERF_THRASHING_PIN_THRESHOLD_DEFAULT,
                                         UVM_PERF_THRASHING_PIN_THRESHOLD_MAX);

    INIT_THRASHING_PARAMETER_NONZERO(uvm_perf_thrashing_lapse_usec, UVM_PERF_THRASHING_LAPSE_USEC_DEFAULT);

    INIT_THRASHING_PARAMETER_NONZERO_MAX(uvm_perf_thrashing_nap,
                                         UVM_PERF_THRASHING_NAP_DEFAULT,
                                         UVM_PERF_THRASHING_NAP_MAX);

    INIT_THRASHING_PARAMETER_NONZERO(uvm_perf_thrashing_epoch, UVM_PERF_THRASHING_EPOCH_DEFAULT);

    INIT_THRASHING_PARAMETER(uvm_perf_thrashing_pin, UVM_PERF_THRASHING_PIN_DEFAULT);

    INIT_THRASHING_PARAMETER(uvm_perf_thrashing_max_resets, UVM_PERF_THRASHING_MAX_RESETS_DEFAULT);

    g_va_block_thrashing_info_cache = NV_KMEM_CACHE_CREATE("uvm_block_thrashing_info_t", block_thrashing_info_t);
    if (!g_va_block_thrashing_info_cache) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    g_pinned_page_cache = NV_KMEM_CACHE_CREATE("uvm_pinned_page_t", pinned_page_t);
    if (!g_pinned_page_cache) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    status = cpu_thrashing_stats_init();
    if (status != NV_OK)
        goto error;

    return NV_OK;

error:
    uvm_perf_thrashing_exit();

    return status;
}

void uvm_perf_thrashing_exit(void)
{
    cpu_thrashing_stats_exit();

    kmem_cache_destroy_safe(&g_va_block_thrashing_info_cache);
    kmem_cache_destroy_safe(&g_pinned_page_cache);
}

NV_STATUS uvm_perf_thrashing_add_gpu(uvm_gpu_t *gpu)
{
    if (!uvm_procfs_is_debug_enabled())
        return NV_OK;

    return gpu_thrashing_stats_create(gpu);
}

void uvm_perf_thrashing_remove_gpu(uvm_gpu_t *gpu)
{
    gpu_thrashing_stats_destroy(gpu);
}

NV_STATUS uvm_test_get_page_thrashing_policy(UVM_TEST_GET_PAGE_THRASHING_POLICY_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    va_space_thrashing_info_t *va_space_thrashing;

    uvm_va_space_down_read(va_space);

    va_space_thrashing = va_space_thrashing_info_get(va_space);

    if (va_space_thrashing->params.enable) {
        params->policy = UVM_TEST_PAGE_THRASHING_POLICY_ENABLE;
        params->nap_ns = va_space_thrashing->params.nap_ns;
        params->pin_ns = va_space_thrashing->params.pin_ns;
        params->map_remote_on_native_atomics_fault = uvm_perf_map_remote_on_native_atomics_fault != 0;
    }
    else {
        params->policy = UVM_TEST_PAGE_THRASHING_POLICY_DISABLE;
    }

    uvm_va_space_up_read(va_space);

    return NV_OK;
}

NV_STATUS uvm_test_set_page_thrashing_policy(UVM_TEST_SET_PAGE_THRASHING_POLICY_PARAMS *params, struct file *filp)
{
    NV_STATUS status = NV_OK;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    va_space_thrashing_info_t *va_space_thrashing;

    if (params->policy >= UVM_TEST_PAGE_THRASHING_POLICY_MAX)
        return NV_ERR_INVALID_ARGUMENT;

    if (!g_uvm_perf_thrashing_enable)
        return NV_ERR_INVALID_STATE;

    uvm_va_space_down_write(va_space);

    va_space_thrashing = va_space_thrashing_info_get(va_space);
    va_space_thrashing->params.test_overrides = true;

    if (params->policy == UVM_TEST_PAGE_THRASHING_POLICY_ENABLE) {
        if (va_space_thrashing->params.enable)
            goto done_unlock_va_space;

        va_space_thrashing->params.pin_ns = params->pin_ns;
        va_space_thrashing->params.enable = true;
    }
    else {
        if (!va_space_thrashing->params.enable)
            goto done_unlock_va_space;

        va_space_thrashing->params.enable = false;
    }

    // When disabling thrashing detection, destroy the thrashing tracking
    // information for all VA blocks and unpin pages
    if (!va_space_thrashing->params.enable) {
        uvm_va_range_managed_t *managed_range;

        uvm_for_each_va_range_managed(managed_range, va_space) {
            uvm_va_block_t *va_block;

            for_each_va_block_in_va_range(managed_range, va_block) {
                uvm_va_block_region_t va_block_region = uvm_va_block_region_from_block(va_block);
                uvm_va_block_context_t *block_context = uvm_va_space_block_context(va_space, NULL);

                uvm_mutex_lock(&va_block->lock);

                // Unmap may split PTEs and require a retry. Needs to be called
                // before the pinned pages information is destroyed.
                status = UVM_VA_BLOCK_RETRY_LOCKED(va_block, NULL,
                             uvm_perf_thrashing_unmap_remote_pinned_pages_all(va_block,
                                                                              block_context,
                                                                              va_block_region));

                uvm_perf_thrashing_info_destroy(va_block);

                uvm_mutex_unlock(&va_block->lock);

                // Re-enable thrashing on failure to avoid getting asserts
                // about having state while thrashing is disabled
                if (status != NV_OK) {
                    va_space_thrashing->params.enable = true;
                    goto done_unlock_va_space;
                }
            }
        }

        status = uvm_hmm_clear_thrashing_policy(va_space);

        // Re-enable thrashing on failure to avoid getting asserts
        // about having state while thrashing is disabled
        if (status != NV_OK) {
            va_space_thrashing->params.enable = true;
            goto done_unlock_va_space;
        }
    }

done_unlock_va_space:
    uvm_va_space_up_write(va_space);

    return status;
}
