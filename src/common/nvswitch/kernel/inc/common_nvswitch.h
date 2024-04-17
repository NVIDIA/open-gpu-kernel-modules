/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _COMMON_NVSWITCH_H_
#define _COMMON_NVSWITCH_H_

#ifdef INCLUDE_NVLINK_LIB
#include "nvlink.h"
#endif

#include "export_nvswitch.h"
#include "error_nvswitch.h"
#include "io_nvswitch.h"
#include "rom_nvswitch.h"
#include "haldef_nvswitch.h"
#include "nvctassert.h"
#include "flcn/flcnable_nvswitch.h"
#include "inforom/inforom_nvswitch.h"
#include "spi_nvswitch.h"
#include "smbpbi_nvswitch.h"
#include "nvCpuUuid.h"
#include "fsprpc_nvswitch.h"

#include "soe/cci/cpld_machx03.h"

#define NVSWITCH_GET_BIT(v, p)       (((v) >> (p)) & 1)
#define NVSWITCH_SET_BIT(v, p)       ((v) |  NVBIT(p))
#define NVSWITCH_CLEAR_BIT(v, p)     ((v) & ~NVBIT(p))
#define NVSWITCH_MASK_BITS(n)        (~(0xFFFFFFFF << (n)))

static NV_INLINE NvBool nvswitch_test_flags(NvU32 val, NvU32 flags)
{
    return !!(val & flags);
}

static NV_INLINE void nvswitch_set_flags(NvU32 *val, NvU32 flags)
{
    *val |= flags;
}

static NV_INLINE void nvswitch_clear_flags(NvU32 *val, NvU32 flags)
{
    *val &= ~flags;
}

// Destructive operation to reverse bits in a mask
#define NVSWITCH_REVERSE_BITMASK_32(numBits, mask)  \
{                                                   \
    NvU32 i, reverse = 0;                           \
    FOR_EACH_INDEX_IN_MASK(32, i, mask)             \
    {                                               \
        reverse |= NVBIT((numBits - 1) - i);          \
    }                                               \
    FOR_EACH_INDEX_IN_MASK_END;                     \
                                                    \
    mask = reverse;                                 \
}

#define NVSWITCH_CHECK_STATUS(_d, _status)                  \
    if (_status != NVL_SUCCESS)                             \
    {                                                       \
        NVSWITCH_PRINT(_d, MMIO, "%s(%d): status=%d\n",     \
            __FUNCTION__, __LINE__,                         \
            _status);                                       \
    }

#define IS_RTLSIM(device)   (device->is_rtlsim)
#define IS_FMODEL(device)   (device->is_fmodel)
#define IS_EMULATION(device)   (device->is_emulation)

#define NVSWITCH_DEVICE_NAME                            "nvswitch"
#define NVSWITCH_LINK_NAME                              "link"

// Max size of sprintf("%d", valid_instance) compile time check
#if NVSWITCH_DEVICE_INSTANCE_MAX < 100
#define NVSWITCH_INSTANCE_LEN 2
#endif

#define NV_ARRAY_ELEMENTS(x)   ((sizeof(x)/sizeof((x)[0])))
 
#define NVSWITCH_DBG_LEVEL(_d) (_d == NULL ? NVSWITCH_DBG_LEVEL_INFO : ((nvswitch_device *)_d)->regkeys.debug_level)

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
#define NVSWITCH_PRINT(_d, _lvl, _fmt, ...)                 \
    ((NVSWITCH_DBG_LEVEL(_d) <= NVSWITCH_DBG_LEVEL_ ## _lvl) ?  \
        nvswitch_os_print(NVSWITCH_DBG_LEVEL_ ## _lvl,      \
            "%s[%-5s]: " _fmt,                              \
            ((_d == NULL) ?                                 \
                "nvswitchx" :                               \
                ((nvswitch_device *)_d)->name),             \
            #_lvl,                                          \
            ## __VA_ARGS__) :                               \
        ((void)(0))                                         \
    )
#else
    #define NVSWITCH_PRINT(_d, _lvl, _fmt, ...) ((void)0)
#endif

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
#define nvswitch_os_malloc(_size)                           \
    nvswitch_os_malloc_trace(_size, __FILE__, __LINE__)
#else
#define nvswitch_os_malloc(_size)                           \
    nvswitch_os_malloc_trace(_size, NULL, 0)
#endif

// LS10 Saved LED state
#define ACCESS_LINK_LED_STATE CPLD_MACHXO3_ACCESS_LINK_LED_CTL_NVL_CABLE_LED

//
// This macro should be used to check assertion statements and print Error messages.
//
#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
void nvswitch_assert_log
(
    const char *function,
    const char *file,
    NvU32 line
);

#define NVSWITCH_ASSERT(_cond)                                                       \
    ((void)((!(_cond)) ? nvswitch_assert_log(__FUNCTION__, __FILE__, __LINE__) : 0))

#else
void nvswitch_assert_log(void);

#define NVSWITCH_ASSERT(_cond)                                       \
    ((void)((!(_cond)) ? nvswitch_assert_log() : 0))
#endif

#define NVSWITCH_ASSERT_ERROR_INFO(errorCategory, errorInfo) NVSWITCH_ASSERT(0x0)
#define NVSWITCH_ASSERT_INFO(errCode, errLinkMask, errSubcode) NVSWITCH_ASSERT(0x0)

//
// This macro should be used cautiously as it prints information in the release
// drivers.
//
#define NVSWITCH_PRINT_SXID(_d, _sxid, _fmt, ...)                                         \
    do                                                                                    \
    {                                                                                     \
        NVSWITCH_ASSERT(nvswitch_translate_hw_error(_sxid) != NVSWITCH_NVLINK_HW_GENERIC); \
        nvswitch_os_print(NVSWITCH_DBG_LEVEL_ERROR,                                       \
            "nvidia-%s: SXid (PCI:" NVLINK_PCI_DEV_FMT "): %05d, " _fmt,                  \
            (_d)->name, NVLINK_PCI_DEV_FMT_ARGS(&(_d)->nvlink_device->pciInfo), _sxid,    \
            ##__VA_ARGS__);                                                               \
        nvswitch_lib_smbpbi_log_sxid(_d, _sxid, _fmt, ##__VA_ARGS__);                     \
        nvswitch_inforom_bbx_add_sxid(_d, _sxid, 0, 0, 0);                                \
    } while(0)

#define NVSWITCH_PRINT_SXID_NO_BBX(_d, _sxid, _fmt, ...)                                  \
    do                                                                                    \
    {                                                                                     \
        NVSWITCH_ASSERT(nvswitch_translate_hw_error(_sxid) != NVSWITCH_NVLINK_HW_GENERIC); \
        nvswitch_os_print(NVSWITCH_DBG_LEVEL_ERROR,                                       \
            "nvidia-%s: SXid (PCI:" NVLINK_PCI_DEV_FMT "): %05d, " _fmt,                  \
            (_d)->name, NVLINK_PCI_DEV_FMT_ARGS(&(_d)->nvlink_device->pciInfo), _sxid,    \
            ##__VA_ARGS__);                                                               \
        nvswitch_lib_smbpbi_log_sxid(_d, _sxid, _fmt, ##__VA_ARGS__);                     \
    } while(0)

#define NVSWITCH_DEV_CMD_DISPATCH_WITH_PRIVATE_DATA(cmd, function, type, private)\
    case cmd:                                                               \
    {                                                                       \
        if (sizeof(type) == size)                                           \
        {                                                                   \
            retval = function(device, params, private);                     \
        }                                                                   \
        else                                                                \
        {                                                                   \
            retval = -NVL_BAD_ARGS;                                         \
        }                                                                   \
        break;                                                              \
    }

#define NVSWITCH_DEV_CMD_DISPATCH_HELPER(cmd, supported, function, type) \
    case cmd:                                                            \
    {                                                                    \
        if (!supported)                                                  \
        {                                                                \
            retval = -NVL_ERR_NOT_SUPPORTED;                             \
        }                                                                \
        else if (sizeof(type) == size)                                   \
        {                                                                \
            retval = function(device, params);                           \
        }                                                                \
        else                                                             \
        {                                                                \
            retval = -NVL_BAD_ARGS;                                      \
        }                                                                \
        break;                                                           \
    }

#define NVSWITCH_DEV_CMD_DISPATCH(cmd, function, type) \
    NVSWITCH_DEV_CMD_DISPATCH_HELPER(cmd, NV_TRUE, function, type)

#define NVSWITCH_MODS_CMDS_SUPPORTED NV_FALSE

#define NVSWITCH_DEV_CMD_DISPATCH_MODS(cmd, function, type)   \
    NVSWITCH_DEV_CMD_DISPATCH_HELPER(cmd, NVSWITCH_MODS_CMDS_SUPPORTED, function, type)

#define NVSWITCH_MAX_NUM_LINKS 100
#if NVSWITCH_MAX_NUM_LINKS <= 100
#define NVSWITCH_LINK_INSTANCE_LEN 2
#endif

extern const nvlink_link_handlers nvswitch_link_handlers;

//
// link_info is used to store private link information
//
typedef struct
{
    char        name[sizeof(NVSWITCH_LINK_NAME) + NVSWITCH_LINK_INSTANCE_LEN];
} LINK_INFO;

typedef struct
{
    NvU32 external_fabric_mgmt;
    NvU32 txtrain_control;
    NvU32 crossbar_DBI;
    NvU32 link_DBI;
    NvU32 ac_coupled_mask;
    NvU32 ac_coupled_mask2;
    NvU32 swap_clk;
    NvU32 link_enable_mask;
    NvU32 link_enable_mask2;
    NvU32 bandwidth_shaper;
    NvU32 ssg_control;
    NvU32 skip_buffer_ready;
    NvU32 enable_pm;
    NvU32 chiplib_forced_config_link_mask;
    NvU32 chiplib_forced_config_link_mask2;
    NvU32 soe_dma_self_test;
    NvU32 soe_disable;
    NvU32 soe_enable;
    NvU32 soe_boot_core;
    NvU32 cci_control;
    NvU32 cci_link_train_disable_mask;
    NvU32 cci_link_train_disable_mask2;
    NvU32 cci_max_onboard_attempts;
    NvU32 cci_error_log_enable;
    NvU32 latency_counter;
    NvU32 nvlink_speed_control;
    NvU32 inforom_bbx_periodic_flush;
    NvU32 inforom_bbx_write_periodicity;
    NvU32 inforom_bbx_write_min_duration;
    NvU32 ato_control;
    NvU32 sto_control;
    NvU32 minion_disable;
    NvU32 set_ucode_target;
    NvU32 set_simmode;
    NvU32 set_smf_settings;
    NvU32 select_uphy_tables;
    NvU32 link_training_mode;
    NvU32 i2c_access_control;
    NvU32 force_kernel_i2c;
    NvU32 link_recal_settings;
    NvU32 crc_bit_error_rate_short;
    NvU32 crc_bit_error_rate_long;
    NvU32 lp_threshold;
    NvU32 minion_intr;
    NvU32 surpress_link_errors_for_gpu_reset;
    NvU32 block_code_mode;
    NvU32 reference_clock_mode;
    NvU32 debug_level;
} NVSWITCH_REGKEY_TYPE;

//
// Background tasks
//
typedef struct NVSWITCH_TASK
{
    struct NVSWITCH_TASK *prev;
    struct NVSWITCH_TASK *next;
    void (*task_fn_vdptr)(nvswitch_device *, void *);
    void (*task_fn_devptr)(nvswitch_device *);
    void *task_args;
    NvU64 period_nsec;
    NvU64 last_run_nsec;
    NvU32 flags;
} NVSWITCH_TASK_TYPE;

#define NVSWITCH_TASK_TYPE_FLAGS_RUN_EVEN_IF_DEVICE_NOT_INITIALIZED     0x1    // Run even the if not initialized
#define NVSWITCH_TASK_TYPE_FLAGS_RUN_ONCE                               0x2    // Only run the task once. Memory for task struct and args will be freed by dispatcher after running.
#define NVSWITCH_TASK_TYPE_FLAGS_VOID_PTR_ARGS                          0x4    // Function accepts args as void * args. 

//
// Wrapper struct for deffered SXID errors
//
typedef struct
{
    NvU32 nvlipt_instance;
    NvU32 link;
} NVSWITCH_DEFERRED_ERROR_REPORTING_ARGS;

//
// PLL
//
typedef struct
{
    NvU32   src_freq_khz;
    NvU32   M;
    NvU32   N;
    NvU32   PL;
    NvU32   dist_mode;
    NvU32   refclk_div;
    NvU32   vco_freq_khz;
    NvU32   freq_khz;
} NVSWITCH_PLL_INFO;

// Per-unit interrupt masks
typedef struct
{
    NvU32   fatal;
    NvU32   nonfatal;
    NvU32   correctable;
} NVSWITCH_INTERRUPT_MASK;

// BIOS Image
typedef struct
{
    // Size of the image.
    NvU32 size;

    // pointer to the BIOS image.
    NvU8* pImage;
} NVSWITCH_BIOS_IMAGE;

struct NVSWITCH_CLIENT_EVENT
{
    NVListRec entry;
    NvU32     eventId;
    void      *private_driver_data;
};

typedef struct 
{
    NvU8 port_event_type;        // 0 = port up, 1 = port down, 2 = error invalid entry
    NvU64 local_port_event_num;  // Count of preceding port events (local port event log)
    NvU64 global_port_event_num; // Count of preceding port events (globally)
    NvU32 link_id;               // Link that event occured on
    NvU64 time;                  // Platform time, in ns
} NVSWITCH_PORT_EVENT_TYPE;

#define NVSWITCH_PORT_EVENT_LOG_SIZE 1024

typedef struct
{
    NvU32 port_event_start;     // Start index within CB (circular buffer)
    NvU32 port_event_count;     // Count of current port events in CB
    NvU64 port_event_total;     // Count of total port events logged
    NvU32 port_event_log_size;  // CB size
    NVSWITCH_PORT_EVENT_TYPE *port_event_log; 
    NvBool overwritable;        // Old entries can be overwritten
    NvBool bOverflow;           // True when log has been overflowed and no
                                //  longer contains all port events that occurred
} NVSWITCH_PORT_EVENT_LOG_TYPE;


//
// common device information
//
struct nvswitch_device
{
#ifdef INCLUDE_NVLINK_LIB
    nvlink_device   *nvlink_device;
#endif

    char            name[sizeof(NVSWITCH_DEVICE_NAME) + NVSWITCH_INSTANCE_LEN];

    void            *os_handle;
    NvU32           os_instance;

    NvBool  is_emulation;
    NvBool  is_rtlsim;
    NvBool  is_fmodel;

    NVSWITCH_REGKEY_TYPE regkeys;

    // Tasks
    NVSWITCH_TASK_TYPE                  *tasks;

    // Errors
    NvU64                               error_total;    // Total errors recorded across all error logs
    NVSWITCH_ERROR_LOG_TYPE             log_FATAL_ERRORS;
    NVSWITCH_ERROR_LOG_TYPE             log_NONFATAL_ERRORS;

    // Port Events
    NVSWITCH_PORT_EVENT_LOG_TYPE        log_PORT_EVENTS;

    NVSWITCH_FIRMWARE                   firmware;

    // HAL connectivity
    nvswitch_hal hal;

    // SOE
    FLCNABLE *pSoe;
    // CCI
    struct CCI *pCci;

    NvU8   current_led_state;
    NvU8   next_led_state;
    NvU64  tp_counter_previous_sum[NVSWITCH_NVLINK_MAX_LINKS];

    // DMA
    NvU32 dma_addr_width;

    // InfoROM
    struct inforom                      *pInforom;

    // I2C
    struct NVSWITCH_OBJI2C              *pI2c;

    // SMBPBI
    struct smbpbi                       *pSmbpbi;

    // NVSWITCH_LINK_TYPE
    NvBool                              bModeContinuousALI;
    NVSWITCH_LINK_TYPE                  link[NVSWITCH_MAX_LINK_COUNT];

    // PLL
    NVSWITCH_PLL_INFO                   switch_pll;

    // Device specific information
    NvU32                               chip_arch;      // NVSWITCH_GET_INFO_INDEX_ARCH_*
    NvU32                               chip_impl;      // NVSWITCH_GET_INFO_INDEX_IMPL_*
                                                        //
    NvU32                               chip_id;        // NV_PSMC/PMC_BOOT_42_CHIP_ID_*
    void *                              chip_device;

    // UUID in big-endian format
    NvUuid uuid;

    // Fabric Manager timeout value for the heartbeat
    NvU32 fm_timeout;

    // Fabric State
    NVSWITCH_DRIVER_FABRIC_STATE driver_fabric_state;
    NVSWITCH_DEVICE_FABRIC_STATE device_fabric_state;
    NVSWITCH_DEVICE_BLACKLIST_REASON device_blacklist_reason;
    NvU64 fabric_state_timestamp;
    NvU32 fabric_state_sequence_number;

    // Full BIOS image
    NVSWITCH_BIOS_IMAGE                  biosImage;

    // List of client events
    NVListRec                            client_events_list;

    // To be removed once newer vbios is on TOT.
    NvBool  bIsNvlinkVbiosTableVersion2;

    // Trusted NVLink Mode
    NVSWITCH_DEVICE_TNVL_MODE tnvl_mode;
};

#define NVSWITCH_IS_DEVICE_VALID(device) \
    ((device != NULL) &&                 \
     (device->nvlink_device->type == NVLINK_DEVICE_TYPE_NVSWITCH))

#define NVSWITCH_IS_DEVICE_ACCESSIBLE(device) \
    (NVSWITCH_IS_DEVICE_VALID(device) &&      \
     (device->nvlink_device->pciInfo.bars[0].pBar != NULL))

#define NVSWITCH_IS_DEVICE_INITIALIZED(device) \
    (NVSWITCH_IS_DEVICE_ACCESSIBLE(device) &&  \
     (device->nvlink_device->initialized))

//
// Error Function defines
//

NvlStatus
nvswitch_construct_error_log
(
    NVSWITCH_ERROR_LOG_TYPE *errors,
    NvU32 error_log_size,
    NvBool overwritable
);

void
nvswitch_destroy_error_log
(
    nvswitch_device *device,
    NVSWITCH_ERROR_LOG_TYPE *errors
);

void
nvswitch_record_error
(
    nvswitch_device *device,
    NVSWITCH_ERROR_LOG_TYPE *errors,
    NvU32   error_type,                     // NVSWITCH_ERR_*
    NvU32   instance,
    NvU32   subinstance,
    NVSWITCH_ERROR_SRC_TYPE error_src,      // NVSWITCH_ERROR_SRC_*
    NVSWITCH_ERROR_SEVERITY_TYPE severity,  // NVSWITCH_ERROR_SEVERITY_*
    NvBool  error_resolved,
    void    *data,
    NvU32   data_size,
    NvU32   line,
    const char *description
);

void
nvswitch_discard_errors
(
    NVSWITCH_ERROR_LOG_TYPE *errors,
    NvU32 error_discard_count
);

void
nvswitch_get_error
(
    nvswitch_device *device,
    NVSWITCH_ERROR_LOG_TYPE *errors,
    NVSWITCH_ERROR_TYPE *error_entry,
    NvU32   error_idx,
    NvU32   *error_count
);

void
nvswitch_get_next_error
(
    nvswitch_device *device,
    NVSWITCH_ERROR_LOG_TYPE *errors,
    NVSWITCH_ERROR_TYPE *error_entry,
    NvU32   *error_count,
    NvBool  remove_from_list
);

void
nvswitch_get_link_handlers
(
    nvlink_link_handlers *nvswitch_link_handlers
);

//
// Timeout checking
//

typedef struct NVSWITCH_TIMEOUT
{
    NvU64   timeout_ns;
} NVSWITCH_TIMEOUT;

#define NVSWITCH_INTERVAL_1USEC_IN_NS     1000LL
#define NVSWITCH_INTERVAL_50USEC_IN_NS    50000LL
#define NVSWITCH_INTERVAL_1MSEC_IN_NS     1000000LL
#define NVSWITCH_INTERVAL_5MSEC_IN_NS     5000000LL
#define NVSWITCH_INTERVAL_750MSEC_IN_NS   750000000LL
#define NVSWITCH_INTERVAL_1SEC_IN_NS      1000000000LL
#define NVSWITCH_INTERVAL_4SEC_IN_NS      4000000000LL

#define NVSWITCH_HEARTBEAT_INTERVAL_NS    NVSWITCH_INTERVAL_1SEC_IN_NS

// This should only be used for short delays
#define NVSWITCH_NSEC_DELAY(nsec_delay)                         \
do                                                              \
{                                                               \
    if (!IS_FMODEL(device))                                     \
    {                                                           \
        NVSWITCH_TIMEOUT timeout;                               \
        nvswitch_timeout_create(nsec_delay, &timeout);          \
        do { }                                                  \
        while (!nvswitch_timeout_check(&timeout));              \
    }                                                           \
} while(0)

// Access link LED states on LS10 Systems
#define ACCESS_LINK_LED_STATE_FAULT      0U
#define ACCESS_LINK_LED_STATE_OFF        1U
#define ACCESS_LINK_LED_STATE_INITIALIZE 2U
#define ACCESS_LINK_LED_STATE_UP_WARM    3U
#define ACCESS_LINK_LED_STATE_UP_ACTIVE  4U
#define ACCESS_LINK_NUM_LED_STATES       5U

#define NVSWITCH_GET_CAP(tbl,cap,field) (((NvU8)tbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) & (0?cap##field))
#define NVSWITCH_SET_CAP(tbl,cap,field) ((tbl[((1?cap##field)>=cap##_TBL_SIZE) ? 0/0 : (1?cap##field)]) |= (0?cap##field))

NvBool nvswitch_is_lr10_device_id(NvU32 device_id);
NvBool nvswitch_is_ls10_device_id(NvU32 device_id);

NvU32 nvswitch_reg_read_32(nvswitch_device *device, NvU32 offset);
void nvswitch_reg_write_32(nvswitch_device *device, NvU32 offset, NvU32 data);
NvU64 nvswitch_read_64bit_counter(nvswitch_device *device, NvU32 lo_offset, NvU32 hi_offset);
void nvswitch_timeout_create(NvU64 timeout_ns, NVSWITCH_TIMEOUT *time);
NvBool nvswitch_timeout_check(NVSWITCH_TIMEOUT *time);
NvlStatus nvswitch_task_create(nvswitch_device *device,
                               void (*task_fn)(nvswitch_device *device), 
                               NvU64 period_nsec, NvU32 flags);
NvlStatus nvswitch_task_create_args(nvswitch_device* device, void *fn_args,
                               void (*task_fn)(nvswitch_device* device, void *fn_args), 
                               NvU64 period_nsec, NvU32 flags);
void nvswitch_tasks_destroy(nvswitch_device *device);

void nvswitch_free_chipdevice(nvswitch_device *device);
NvlStatus nvswitch_create_link(nvswitch_device *device, NvU32 link_number, nvlink_link **link);
nvlink_link* nvswitch_get_link(nvswitch_device *device, NvU8 link_id);
NvU64 nvswitch_get_enabled_link_mask(nvswitch_device *device);
void nvswitch_destroy_link(nvlink_link *link);
NvlStatus nvswitch_validate_pll_config(nvswitch_device *device,
                    NVSWITCH_PLL_INFO *switch_pll,
                    NVSWITCH_PLL_LIMITS default_pll_limits);

NvlStatus nvswitch_poll_sublink_state(nvswitch_device *device, nvlink_link *link);
void      nvswitch_setup_link_loopback_mode(nvswitch_device *device, NvU32 linkNumber);
void      nvswitch_reset_persistent_link_hw_state(nvswitch_device *device, NvU32 linkNumber);
void      nvswitch_store_topology_information(nvswitch_device *device, nvlink_link *link);

NvlStatus nvswitch_launch_ALI(nvswitch_device *device);
NvlStatus nvswitch_launch_ALI_link_training(nvswitch_device *device, nvlink_link *link, NvBool bSync);
NvlStatus nvswitch_inband_read_data(nvswitch_device *device, NvU8 *dest, NvU32 linkId, NvU32 *dataSize);
void      nvswitch_filter_messages(nvswitch_device *device, NvU32 linkId);
NvlStatus nvswitch_reset_and_train_link(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_set_training_mode(nvswitch_device *device);
NvBool    nvswitch_is_link_in_reset(nvswitch_device *device, nvlink_link *link);
void      nvswitch_apply_recal_settings(nvswitch_device *device, nvlink_link *link);
void nvswitch_init_buffer_ready(nvswitch_device *device, nvlink_link *link, NvBool bNportBufferReady);
NvBool    nvswitch_does_link_need_termination_enabled(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_link_termination_setup(nvswitch_device *device, nvlink_link* link);
void      nvswitch_record_port_event(nvswitch_device *device, NVSWITCH_PORT_EVENT_LOG_TYPE *port_events, NvU32 link_id, NvU8 port_event_type);
NvlStatus nvswitch_ctrl_get_port_events(nvswitch_device *device, NVSWITCH_GET_PORT_EVENTS_PARAMS *p);

#endif //_COMMON_NVSWITCH_H_
