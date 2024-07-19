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

#include "common_nvswitch.h"
#include "rom_nvswitch.h"
#include "error_nvswitch.h"
#include "regkey_nvswitch.h"
#include "bios_nvswitch.h"
#include "haldef_nvswitch.h"
#include "flcn/haldefs_flcnable_nvswitch.h"
#include "flcn/flcn_nvswitch.h"
#include "soe/soe_nvswitch.h"
#include "soe/soeififr.h"
#include "nvVer.h"
#include "nvlink_inband_msg.h"
#include "cci/cci_nvswitch.h"

static NvlStatus _nvswitch_ctrl_inband_flush_data(nvswitch_device *device, NVSWITCH_INBAND_FLUSH_DATA_PARAMS *p);

#define NVSWITCH_DEV_CMD_CHECK_ADMIN    NVBIT64(0)
#define NVSWITCH_DEV_CMD_CHECK_FM       NVBIT64(1)

#define NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(cmd, function, type, private, flags)\
    case cmd:                                                                    \
    {                                                                            \
        if (sizeof(type) != size)                                                \
        {                                                                        \
            retval = -NVL_BAD_ARGS;                                              \
            break;                                                               \
        }                                                                        \
                                                                                 \
        retval = _nvswitch_lib_validate_privileged_ctrl(private, flags);         \
        if (retval != NVL_SUCCESS)                                               \
        {                                                                        \
            break;                                                               \
        }                                                                        \
                                                                                 \
        retval = function(device, params);                                       \
        break;                                                                   \
    }                                                                            \

#define NVSWITCH_DEV_CMD_DISPATCH_RESERVED(cmd)                                  \
    case cmd:                                                                    \
    {                                                                            \
        retval = -NVL_ERR_NOT_SUPPORTED;                                         \
        break;                                                                   \
    }                                                                            \

const static NvU32 nvswitch_lr10_device_ids[] =
{
    0x1AE8, 0x1AF0, 0x1AF1, 0x1AF2, 0x1AF3, 0x1AF4, 0x1AF5, 0x1AF6, 0x1AF7,
    0x1AF8, 0x1AF9, 0x1AFA, 0x1AFB, 0x1AFC, 0x1AFD, 0x1AFE, 0x1AFF
};

const static NvU32 nvswitch_ls10_device_ids[] =
{
    // PCIE endpoint to manage the NVLink switch HW
    0x22A0, 0x22A1, 0x22A2, 0x22A3, 0x22A4, 0x22A5, 0x22A6, 0x22A7,
    // PCI-PCI Bridge, Laguna Switch Function 0
    0x22A8, 0x22A9, 0x22AA, 0x22AB,
    // Non-Transparent Bridge, Laguna Switch Function 1
    0x22AC, 0x22AD, 0x22AE, 0x22AF
};

nvlink_link_handlers link_handlers;

static NvBool
_nvswitch_is_device_id_present
(
    const NvU32 *array,
    NvU32 array_len,
    NvU32 device_id
)
{
    NvU32 i = 0;

    for(i = 0; i < array_len; i++)
    {
        if (array[i] == device_id)
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

NvBool
nvswitch_is_lr10_device_id
(
    NvU32 device_id
)
{
    NvU32 count = (sizeof(nvswitch_lr10_device_ids) /
                        sizeof(nvswitch_lr10_device_ids[0]));

    return _nvswitch_is_device_id_present(nvswitch_lr10_device_ids, count, device_id);
}

NvBool
nvswitch_is_ls10_device_id
(
    NvU32 device_id
)
{
    NvU32 count = (sizeof(nvswitch_ls10_device_ids) /
                        sizeof(nvswitch_ls10_device_ids[0]));

    return _nvswitch_is_device_id_present(nvswitch_ls10_device_ids, count, device_id);
}

/*
 * NVLink corelib callbacks are used by the NVLink library separate from the
 * NVSwitch driver, therefore they do not take a device lock and can not modify
 * nvswitch_device state or use error logging.
 *
 * These NVSwitch functions modify link state outside of the corelib:
 *   _nvswitch_ctrl_inject_link_error - injects asynchronous link errors (MODS-only)
 */

static NV_API_CALL NvlStatus
_nvswitch_corelib_add_link
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_add_link(link);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_remove_link
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_remove_link(link);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_set_dl_link_mode
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_set_dl_link_mode(link, mode, flags);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_get_dl_link_mode
(
    nvlink_link *link,
    NvU64 *mode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_get_dl_link_mode(link, mode);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_set_tl_link_mode
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_set_tl_link_mode(link, mode, flags);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_get_tl_link_mode
(
    nvlink_link *link,
    NvU64 *mode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_get_tl_link_mode(link, mode);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_set_tx_mode
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_set_tx_mode(link, mode, flags);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_get_tx_mode
(
    nvlink_link *link,
    NvU64 *mode,
    NvU32 *subMode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_get_tx_mode(link, mode, subMode);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_set_rx_mode
(
    nvlink_link *link,
    NvU64 mode,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_set_rx_mode(link, mode, flags);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_get_rx_mode
(
    nvlink_link *link,
    NvU64 *mode,
    NvU32 *subMode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_get_rx_mode(link, mode, subMode);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_set_rx_detect
(
    nvlink_link *link,
    NvU32 flags
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_set_rx_detect(link, flags);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_get_rx_detect
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_corelib_get_rx_detect(link);
}

static NV_API_CALL void
_nvswitch_corelib_training_complete
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    device->hal.nvswitch_corelib_training_complete(link);
}

static NV_API_CALL void
_nvswitch_corelib_get_uphy_load
(
    nvlink_link *link,
    NvBool *bUnlocked
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    device->hal.nvswitch_corelib_get_uphy_load(link, bUnlocked);
}


static NV_API_CALL NvlStatus
_nvswitch_corelib_write_discovery_token
(
    nvlink_link *link,
    NvU64 token
)
{
    nvswitch_device *device = link->dev->pDevInfo;

    if (link->version >= NVLINK_DEVICE_VERSION_40)
    {
        nvswitch_store_topology_information(device, link);
        return NVL_SUCCESS;
    }

    return NVL_SUCCESS;
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_read_discovery_token
(
    nvlink_link *link,
    NvU64 *token
)
{
    if (link->version >= NVLINK_DEVICE_VERSION_40)
    {
        return NVL_SUCCESS;
    }
    return NVL_SUCCESS;
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_get_cci_link_mode
(
    nvlink_link *link,
    NvU64 *mode
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_cci_get_cci_link_mode(device, link->linkNumber, mode);
}

static NV_API_CALL NvlStatus
_nvswitch_corelib_ali_training
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;
    return device->hal.nvswitch_launch_ALI_link_training(device, link, NV_FALSE);
}

void
nvswitch_get_link_handlers
(
    nvlink_link_handlers *nvswitch_link_handlers
)
{
    if (!nvswitch_link_handlers)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    nvswitch_link_handlers->add = _nvswitch_corelib_add_link;
    nvswitch_link_handlers->remove = _nvswitch_corelib_remove_link;
    nvswitch_link_handlers->set_dl_link_mode = _nvswitch_corelib_set_dl_link_mode;
    nvswitch_link_handlers->get_dl_link_mode = _nvswitch_corelib_get_dl_link_mode;
    nvswitch_link_handlers->set_tl_link_mode = _nvswitch_corelib_set_tl_link_mode;
    nvswitch_link_handlers->get_tl_link_mode = _nvswitch_corelib_get_tl_link_mode;
    nvswitch_link_handlers->set_tx_mode = _nvswitch_corelib_set_tx_mode;
    nvswitch_link_handlers->get_tx_mode = _nvswitch_corelib_get_tx_mode;
    nvswitch_link_handlers->set_rx_mode = _nvswitch_corelib_set_rx_mode;
    nvswitch_link_handlers->get_rx_mode = _nvswitch_corelib_get_rx_mode;
    nvswitch_link_handlers->set_rx_detect = _nvswitch_corelib_set_rx_detect;
    nvswitch_link_handlers->get_rx_detect = _nvswitch_corelib_get_rx_detect;
    nvswitch_link_handlers->training_complete = _nvswitch_corelib_training_complete;
    nvswitch_link_handlers->get_uphy_load = _nvswitch_corelib_get_uphy_load;
    nvswitch_link_handlers->write_discovery_token = _nvswitch_corelib_write_discovery_token;
    nvswitch_link_handlers->read_discovery_token = _nvswitch_corelib_read_discovery_token;
    nvswitch_link_handlers->ali_training = _nvswitch_corelib_ali_training;
    nvswitch_link_handlers->get_cci_link_mode = _nvswitch_corelib_get_cci_link_mode;
}

#define NVSWITCH_INIT_REGKEY(_private, _regkey, _string, _default_val)          \
do                                                                              \
{                                                                               \
    NvU32 data;                                                                 \
                                                                                \
    device->regkeys._regkey = _default_val;                                     \
    if (NV_SWITCH_REGKEY_PRIVATE_ALLOWED || !NV_SWITCH_REGKEY##_private)        \
    {                                                                           \
        if (NVL_SUCCESS ==                                                      \
            nvswitch_os_read_registry_dword(device->os_handle, _string, &data)) \
        {                                                                       \
            NVSWITCH_PRINT(device, SETUP,                                       \
                "%s: Applying regkey %s=0x%x\n",                                \
                __FUNCTION__,                                                   \
                _string, data);                                                 \
            device->regkeys._regkey = data;                                     \
        }                                                                       \
    }                                                                           \
} while(0)

static void
_nvswitch_init_device_regkeys
(
    nvswitch_device *device
)
{
    //
    // Public external use regkeys
    //
    NVSWITCH_INIT_REGKEY(_PUBLIC, ato_control,
                         NV_SWITCH_REGKEY_ATO_CONTROL,
                         NV_SWITCH_REGKEY_ATO_CONTROL_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PUBLIC, sto_control,
                         NV_SWITCH_REGKEY_STO_CONTROL,
                         NV_SWITCH_REGKEY_STO_CONTROL_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PUBLIC, crc_bit_error_rate_short,
                         NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT,
                         NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PUBLIC, crc_bit_error_rate_long,
                         NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG,
                         NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PUBLIC, surpress_link_errors_for_gpu_reset,
                         NV_SWITCH_REGKEY_SURPRESS_LINK_ERRORS_FOR_GPU_RESET,
                         NV_SWITCH_REGKEY_SURPRESS_LINK_ERRORS_FOR_GPU_RESET_DISABLE);

    NVSWITCH_INIT_REGKEY(_PUBLIC, cci_control,
                         NV_SWITCH_REGKEY_CCI_CONTROL,
                         NV_SWITCH_REGKEY_CCI_CONTROL_DEFAULT);
    NVSWITCH_INIT_REGKEY(_PRIVATE, cci_link_train_disable_mask,
                         NV_SWITCH_REGKEY_CCI_DISABLE_LINK_TRAIN_MASK,
                         NV_SWITCH_REGKEY_CCI_DISABLE_LINK_TRAIN_MASK_DEFAULT);
    NVSWITCH_INIT_REGKEY(_PRIVATE, cci_link_train_disable_mask2,
                         NV_SWITCH_REGKEY_CCI_DISABLE_LINK_TRAIN_MASK2,
                         NV_SWITCH_REGKEY_CCI_DISABLE_LINK_TRAIN_MASK2_DEFAULT);
    NVSWITCH_INIT_REGKEY(_PUBLIC, cci_max_onboard_attempts,
                         NV_SWITCH_REGKEY_CCI_MAX_ONBOARD_ATTEMPTS,
                         NV_SWITCH_REGKEY_CCI_MAX_ONBOARD_ATTEMPTS_DEFAULT);
    NVSWITCH_INIT_REGKEY(_PUBLIC, cci_error_log_enable,
                         NV_SWITCH_REGKEY_CCI_ERROR_LOG_ENABLE,
                         NV_SWITCH_REGKEY_CCI_ERROR_LOG_ENABLE_DEFAULT);
    //
    // Debug use regkeys
    // Not available on release build kernel drivers
    //
    NVSWITCH_INIT_REGKEY(_PRIVATE, external_fabric_mgmt,
                         NV_SWITCH_REGKEY_EXTERNAL_FABRIC_MGMT,
                         NV_SWITCH_REGKEY_EXTERNAL_FABRIC_MGMT_ENABLE);

    NVSWITCH_INIT_REGKEY(_PRIVATE, txtrain_control,
                         NV_SWITCH_REGKEY_TXTRAIN_CONTROL,
                         NV_SWITCH_REGKEY_TXTRAIN_CONTROL_NOP);

    NVSWITCH_INIT_REGKEY(_PRIVATE, crossbar_DBI,
                         NV_SWITCH_REGKEY_CROSSBAR_DBI,
                         NV_SWITCH_REGKEY_CROSSBAR_DBI_ENABLE);

    NVSWITCH_INIT_REGKEY(_PRIVATE, link_DBI,
                         NV_SWITCH_REGKEY_LINK_DBI,
                         NV_SWITCH_REGKEY_LINK_DBI_ENABLE);

    NVSWITCH_INIT_REGKEY(_PRIVATE, ac_coupled_mask,
                         NV_SWITCH_REGKEY_AC_COUPLED_MASK,
                         0);

    NVSWITCH_INIT_REGKEY(_PRIVATE, ac_coupled_mask2,
                         NV_SWITCH_REGKEY_AC_COUPLED_MASK2,
                         0);

    NVSWITCH_INIT_REGKEY(_PRIVATE, swap_clk,
                         NV_SWITCH_REGKEY_SWAP_CLK_OVERRIDE,
                         nvswitch_get_swap_clk_default(device));

    NVSWITCH_INIT_REGKEY(_PRIVATE, link_enable_mask,
                         NV_SWITCH_REGKEY_ENABLE_LINK_MASK,
                         NV_U32_MAX);

    NVSWITCH_INIT_REGKEY(_PRIVATE, link_enable_mask2,
                         NV_SWITCH_REGKEY_ENABLE_LINK_MASK2,
                         NV_U32_MAX);

    NVSWITCH_INIT_REGKEY(_PRIVATE, bandwidth_shaper,
                         NV_SWITCH_REGKEY_BANDWIDTH_SHAPER,
                         NV_SWITCH_REGKEY_BANDWIDTH_SHAPER_PROD);

    NVSWITCH_INIT_REGKEY(_PRIVATE, ssg_control,
                         NV_SWITCH_REGKEY_SSG_CONTROL,
                         0);

    NVSWITCH_INIT_REGKEY(_PRIVATE, skip_buffer_ready,
                         NV_SWITCH_REGKEY_SKIP_BUFFER_READY,
                         0);

    NVSWITCH_INIT_REGKEY(_PUBLIC, enable_pm,
                         NV_SWITCH_REGKEY_ENABLE_PM,
                         NV_SWITCH_REGKEY_ENABLE_PM_YES);

    NVSWITCH_INIT_REGKEY(_PRIVATE, chiplib_forced_config_link_mask,
                         NV_SWITCH_REGKEY_CHIPLIB_FORCED_LINK_CONFIG_MASK,
                         0);

    NVSWITCH_INIT_REGKEY(_PRIVATE, chiplib_forced_config_link_mask2,
                         NV_SWITCH_REGKEY_CHIPLIB_FORCED_LINK_CONFIG_MASK2,
                         0);

    NVSWITCH_INIT_REGKEY(_PRIVATE, soe_dma_self_test,
                         NV_SWITCH_REGKEY_SOE_DMA_SELFTEST,
                         NV_SWITCH_REGKEY_SOE_DMA_SELFTEST_ENABLE);

    NVSWITCH_INIT_REGKEY(_PRIVATE, soe_disable,
                         NV_SWITCH_REGKEY_SOE_DISABLE,
                         NV_SWITCH_REGKEY_SOE_DISABLE_NO);

    NVSWITCH_INIT_REGKEY(_PRIVATE, latency_counter,
                         NV_SWITCH_REGKEY_LATENCY_COUNTER_LOGGING,
                         NV_SWITCH_REGKEY_LATENCY_COUNTER_LOGGING_ENABLE);

    NVSWITCH_INIT_REGKEY(_PRIVATE, nvlink_speed_control,
                         NV_SWITCH_REGKEY_SPEED_CONTROL,
                         NV_SWITCH_REGKEY_SPEED_CONTROL_SPEED_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, inforom_bbx_periodic_flush,
                         NV_SWITCH_REGKEY_INFOROM_BBX_ENABLE_PERIODIC_FLUSHING,
                         NV_SWITCH_REGKEY_INFOROM_BBX_ENABLE_PERIODIC_FLUSHING_DISABLE);

    NVSWITCH_INIT_REGKEY(_PRIVATE, inforom_bbx_write_periodicity,
                         NV_SWITCH_REGKEY_INFOROM_BBX_WRITE_PERIODICITY,
                         NV_SWITCH_REGKEY_INFOROM_BBX_WRITE_PERIODICITY_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, inforom_bbx_write_min_duration,
                         NV_SWITCH_REGKEY_INFOROM_BBX_WRITE_MIN_DURATION,
                         NV_SWITCH_REGKEY_INFOROM_BBX_WRITE_MIN_DURATION_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, minion_disable,
                         NV_SWITCH_REGKEY_MINION_DISABLE,
                         NV_SWITCH_REGKEY_MINION_DISABLE_NO);

    NVSWITCH_INIT_REGKEY(_PRIVATE, set_ucode_target,
                         NV_SWITCH_REGKEY_MINION_SET_UCODE_TARGET,
                         NV_SWITCH_REGKEY_MINION_SET_UCODE_TARGET_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, set_simmode,
                         NV_SWITCH_REGKEY_MINION_SET_SIMMODE,
                         NV_SWITCH_REGKEY_MINION_SET_SIMMODE_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, set_smf_settings,
                         NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS,
                         NV_SWITCH_REGKEY_MINION_SET_SMF_SETTINGS_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, select_uphy_tables,
                         NV_SWITCH_REGKEY_MINION_SELECT_UPHY_TABLES,
                         NV_SWITCH_REGKEY_MINION_SELECT_UPHY_TABLES_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, link_training_mode,
                         NV_SWITCH_REGKEY_LINK_TRAINING_SELECT,
                         NV_SWITCH_REGKEY_LINK_TRAINING_SELECT_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, i2c_access_control,
                         NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL,
                         NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, force_kernel_i2c,
                         NV_SWITCH_REGKEY_FORCE_KERNEL_I2C,
                         NV_SWITCH_REGKEY_FORCE_KERNEL_I2C_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, link_recal_settings,
                         NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS,
                         NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_NOP);

    NVSWITCH_INIT_REGKEY(_PRIVATE, lp_threshold,
                         NV_SWITCH_REGKEY_SET_LP_THRESHOLD,
                         NV_SWITCH_REGKEY_SET_LP_THRESHOLD_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PUBLIC, minion_intr,
                         NV_SWITCH_REGKEY_MINION_INTERRUPTS,
                         NV_SWITCH_REGKEY_MINION_INTERRUPTS_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, block_code_mode,
                         NV_SWITCH_REGKEY_BLOCK_CODE_MODE,
                         NV_SWITCH_REGKEY_BLOCK_CODE_MODE_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, reference_clock_mode,
                         NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE,
                         NV_SWITCH_REGKEY_REFERENCE_CLOCK_MODE_DEFAULT);
    
    NVSWITCH_INIT_REGKEY(_PRIVATE, debug_level,
                         NV_SWITCH_REGKEY_DBG_LEVEL,
                         NV_SWITCH_REGKEY_DBG_LEVEL_DEFAULT);
}

ct_assert(NVSWITCH_DBG_LEVEL_MMIO == NV_SWITCH_REGKEY_DBG_LEVEL_MMIO);
ct_assert(NVSWITCH_DBG_LEVEL_NOISY == NV_SWITCH_REGKEY_DBG_LEVEL_NOISY);
ct_assert(NVSWITCH_DBG_LEVEL_SETUP == NV_SWITCH_REGKEY_DBG_LEVEL_SETUP);
ct_assert(NVSWITCH_DBG_LEVEL_INFO == NV_SWITCH_REGKEY_DBG_LEVEL_INFO);
ct_assert(NVSWITCH_DBG_LEVEL_WARN == NV_SWITCH_REGKEY_DBG_LEVEL_WARN);
ct_assert(NVSWITCH_DBG_LEVEL_ERROR == NV_SWITCH_REGKEY_DBG_LEVEL_ERROR);

NvU64
nvswitch_lib_deferred_task_dispatcher
(
    nvswitch_device *device
)
{
    NvU64 time_nsec;
    NvU64 time_next_nsec = nvswitch_os_get_platform_time() + 100*NVSWITCH_INTERVAL_1MSEC_IN_NS;
    NVSWITCH_TASK_TYPE *task;
    NVSWITCH_TASK_TYPE *prev_task;

    if (!NVSWITCH_IS_DEVICE_VALID(device))
    {
        return NV_U64_MAX;
    }

    prev_task = NULL;
    task = device->tasks;

    // Walk the task list, executing those whose next execution interval is at hand
    while (task)
    {
        // Get current time (nsec) for scheduling
        time_nsec = nvswitch_os_get_platform_time();

        if (time_nsec >= task->last_run_nsec + task->period_nsec)
        {
            //
            // The task has never been run or it is time to run
            // Mark its last run time
            //
            task->last_run_nsec = time_nsec;
            // Run the task
            if (NVSWITCH_IS_DEVICE_INITIALIZED(device) ||
               (task->flags & NVSWITCH_TASK_TYPE_FLAGS_RUN_EVEN_IF_DEVICE_NOT_INITIALIZED))
            {
                if(task->flags & NVSWITCH_TASK_TYPE_FLAGS_VOID_PTR_ARGS)
                    (*task->task_fn_vdptr)(device, task->task_args);  // run task with provided args
                else
                    (*task->task_fn_devptr)(device);
            }
        }
        
        // Determine its next run time
        time_next_nsec = NV_MIN(task->last_run_nsec + task->period_nsec, time_next_nsec);

        // Advance pointer. If run once flag is set and task ran, remove task from list.
        if((task->flags & NVSWITCH_TASK_TYPE_FLAGS_RUN_ONCE) &&
           (task->last_run_nsec == time_nsec))
        {
            prev_task = task->prev;

            // Removing from list head
            if (prev_task == NULL)
            {
                device->tasks = task->next;
                if (device->tasks != NULL)
                {
                    device->tasks->prev = NULL;
                }
                nvswitch_os_free(task);
                task = device->tasks;
            }
            else
            {
                prev_task->next = task->next;
                if (prev_task->next != NULL)
                {
                    prev_task->next->prev = prev_task;
                }
                nvswitch_os_free(task);
                task = prev_task->next;
            }
        }
        else
        {
            task = task->next;
        }
    }

    time_nsec = nvswitch_os_get_platform_time();

    // Return to the OS layer how long to wait before calling again
    return(time_next_nsec >= time_nsec ? time_next_nsec - time_nsec : 0);
}

static NvlStatus
_nvswitch_setup_hal
(
    nvswitch_device *device,
    NvU32 pci_device_id
)
{
    if (nvswitch_is_lr10_device_id(pci_device_id))
    {
        nvswitch_setup_hal_lr10(device);
        return NVL_SUCCESS;
    }
    if (nvswitch_is_ls10_device_id(pci_device_id))
    {
        nvswitch_setup_hal_ls10(device);
        return NVL_SUCCESS;
    }
    NVSWITCH_PRINT(device, ERROR,
        "NVSwitch HAL setup failed - Unrecognized PCI Device ID\n");
    return -NVL_ERR_NOT_SUPPORTED;
}

NvlStatus
nvswitch_lib_check_api_version
(
    const char *user_version,
    char *kernel_version,
    NvU32 length
)
{
    const NvLength VERSION_LENGTH = nvswitch_os_strlen(NV_VERSION_STRING);

    if (kernel_version == NULL || user_version == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    if (length < VERSION_LENGTH)
    {
        return -NVL_NO_MEM;
    }

    nvswitch_os_memset(kernel_version, 0x0, length);
    nvswitch_os_strncpy(kernel_version, NV_VERSION_STRING, VERSION_LENGTH);

    kernel_version[length - 1] = '\0';

    if (nvswitch_os_strncmp(user_version, kernel_version, VERSION_LENGTH))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return NVL_SUCCESS;
}

NvBool
nvswitch_is_inforom_supported
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_inforom_supported(device);
}

NvBool
nvswitch_is_spi_supported
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_spi_supported(device);
}

NvBool
nvswitch_is_bios_supported
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_bios_supported(device);
}

NvBool
nvswitch_is_smbpbi_supported
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_smbpbi_supported(device);
}

NvBool
nvswitch_is_soe_supported
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_soe_supported(device);
}


NvlStatus
nvswitch_init_soe
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_init_soe(device);
}

void
nvswitch_soe_init_l2_state
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_soe_init_l2_state(device);
}

NvlStatus
nvswitch_soe_issue_ingress_stop
(
    nvswitch_device *device,
    NvU32 nport,
    NvBool bStop
)
{
    return device->hal.nvswitch_soe_issue_ingress_stop(device, nport, bStop);
}

void
nvswitch_fsp_update_cmdq_head_tail
(
    nvswitch_device *device,
    NvU32 queueHead,
    NvU32 queueTail
)
{
    device->hal.nvswitch_fsp_update_cmdq_head_tail(device, queueHead, queueTail);
}

void
nvswitch_fsp_get_cmdq_head_tail
(
    nvswitch_device *device,
    NvU32 *pQueueHead,
    NvU32 *pQueueTail
)
{
    device->hal.nvswitch_fsp_get_cmdq_head_tail(device, pQueueHead, pQueueTail);
}

void
nvswitch_fsp_update_msgq_head_tail
(
    nvswitch_device *device,
    NvU32 msgqHead,
    NvU32 msgqTail
)
{
    device->hal.nvswitch_fsp_update_msgq_head_tail(device, msgqHead, msgqTail);
}

void
nvswitch_fsp_get_msgq_head_tail
(
    nvswitch_device *device,
    NvU32 *pMsgqHead,
    NvU32 *pMsgqTail
)
{
   device->hal.nvswitch_fsp_get_msgq_head_tail(device, pMsgqHead, pMsgqTail);
}

NvU32
nvswitch_fsp_get_channel_size
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_fsp_get_channel_size(device);
}

NvU8
nvswitch_fsp_nvdm_to_seid
(
    nvswitch_device *device,
    NvU8 nvdmType
)
{
    return device->hal.nvswitch_fsp_nvdm_to_seid(device, nvdmType);
}

NvU32
nvswitch_fsp_create_mctp_header
(
    nvswitch_device *device,
    NvU8 som,
    NvU8 eom,
    NvU8 seid,
    NvU8 seq
)
{
    return device->hal.nvswitch_fsp_create_mctp_header(device, som, eom, seid, seq);
}

NvU32
nvswitch_fsp_create_nvdm_header
(
    nvswitch_device *device,
    NvU32 nvdmType
)
{
    return device->hal.nvswitch_fsp_create_nvdm_header(device, nvdmType);
}

NvlStatus
nvswitch_fsp_get_packet_info
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size,
    NvU8 *pPacketState,
    NvU8 *pTag
)
{
    return device->hal.nvswitch_fsp_get_packet_info(device, pBuffer, size, pPacketState, pTag);
}

NvlStatus
nvswitch_fsp_validate_mctp_payload_header
(
    nvswitch_device  *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return device->hal.nvswitch_fsp_validate_mctp_payload_header(device, pBuffer, size);
}

NvlStatus
nvswitch_fsp_process_nvdm_msg
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return device->hal.nvswitch_fsp_process_nvdm_msg(device, pBuffer, size);
}

NvlStatus
nvswitch_fsp_process_cmd_response
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return device->hal.nvswitch_fsp_process_cmd_response(device, pBuffer, size);
}

NvlStatus
nvswitch_fsp_config_ememc
(
    nvswitch_device *device,
    NvU32 offset,
    NvBool bAincw,
    NvBool bAincr
)
{
    return device->hal.nvswitch_fsp_config_ememc(device, offset, bAincw, bAincr);
}

NvlStatus
nvswitch_fsp_write_to_emem
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return device->hal.nvswitch_fsp_write_to_emem(device, pBuffer, size);
}

NvlStatus
nvswitch_fsp_read_from_emem
(
    nvswitch_device *device,
    NvU8 *pBuffer,
    NvU32 size
)
{
    return device->hal.nvswitch_fsp_read_from_emem(device, pBuffer, size);
}

NvlStatus
nvswitch_fsp_error_code_to_nvlstatus_map
(
    nvswitch_device *device,
    NvU32 errorCode
)
{
    return device->hal.nvswitch_fsp_error_code_to_nvlstatus_map(device, errorCode);
}

static NvlStatus
_nvswitch_ctrl_fsprpc_get_caps
(
    nvswitch_device *device,
    NVSWITCH_FSPRPC_GET_CAPS_PARAMS *params
)
{
    return device->hal.nvswitch_fsprpc_get_caps(device, params);
}

static NvlStatus
_nvswitch_ctrl_get_attestation_certificate_chain
(
    nvswitch_device *device,
    NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN_PARAMS *params
)
{
    return device->hal.nvswitch_tnvl_get_attestation_certificate_chain(device, params);
}

static NvlStatus
_nvswitch_ctrl_get_attestation_report
(
    nvswitch_device *device,
    NVSWITCH_GET_ATTESTATION_REPORT_PARAMS *params
)
{
    return device->hal.nvswitch_tnvl_get_attestation_report(device, params);
}

static NvlStatus
_nvswitch_ctrl_get_tnvl_status
(
    nvswitch_device *device,
    NVSWITCH_GET_TNVL_STATUS_PARAMS *params
)
{
    return device->hal.nvswitch_tnvl_get_status(device, params);
}

static NvlStatus
_nvswitch_construct_soe
(
    nvswitch_device *device
)
{
    FLCNABLE *pSoe = NULL;
    NvlStatus retval;

    device->pSoe = pSoe = (PFLCNABLE)soeAllocNew();
    if (pSoe == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "SOE allocation failed.\n");
        return -NVL_NO_MEM;
    }

    retval = soeInit(device, (PSOE)pSoe, device->nvlink_device->pciInfo.pciDeviceId);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "SOE init failed.\n");
        goto soe_init_failed;
    }

    if (flcnableConstruct_HAL(device, pSoe) != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "FALCON construct failed.\n");
        retval = -NVL_ERR_INVALID_STATE;
        goto flcn_construct_failed;
    }

    return NVL_SUCCESS;

flcn_construct_failed:
    soeDestroy(device, (PSOE)pSoe);

soe_init_failed:
    nvswitch_os_free(pSoe);
    device->pSoe = NULL;

    return retval;
}

static void
_nvswitch_destruct_soe
(
    nvswitch_device *device
)
{
    FLCNABLE *pSoe = device->pSoe;

    if (pSoe == NULL)
    {
        return;
    }

    flcnableDestruct_HAL(device, pSoe);
    soeDestroy(device, (PSOE)pSoe);

    nvswitch_os_free(pSoe);
    device->pSoe = NULL;
}

static NvlStatus
_nvswitch_construct_cci
(
    nvswitch_device *device
)
{
    CCI *pCci = NULL;
    NvlStatus retval;

    device->pCci = pCci = cciAllocNew();
    if (pCci == NULL)
    {
        NVSWITCH_PRINT(device, ERROR, "CCI allocation failed.\n");
        return -NVL_NO_MEM;
    }

    retval = cciInit(device, pCci, device->nvlink_device->pciInfo.pciDeviceId);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "CCI init failed.\n");
        goto cci_init_failed;
    }

    return NVL_SUCCESS;

cci_init_failed:
    nvswitch_os_free(pCci);
    device->pCci = NULL;

    return retval;
}

static void
_nvswitch_destruct_cci
(
    nvswitch_device *device
)
{
    CCI *pCci = device->pCci;

    if (pCci == NULL)
    {
        return;
    }

    cciDestroy(device, pCci);

    nvswitch_os_free(pCci);
    device->pCci = NULL;
}

static void
_nvswitch_update_link_state_led
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_update_link_state_led(device);
}

static void
_nvswitch_led_shutdown
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_led_shutdown(device);
}

static NvlStatus
_nvswitch_initialize_device_state
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_initialize_device_state(device);
}

static NvlStatus
_nvswitch_post_init_device_setup
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_post_init_device_setup(device);
}

static NvlStatus
_nvswitch_setup_system_registers
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_setup_system_registers(device);
}

static void
_nvswitch_post_init_blacklist_device_setup
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_post_init_blacklist_device_setup(device);
}

static void
_nvswitch_set_dma_mask
(
    nvswitch_device *device
)
{
    NvU32 hw_dma_width, retval;

    hw_dma_width = device->hal.nvswitch_get_device_dma_width(device);

    if (hw_dma_width == 0)
    {
        NVSWITCH_PRINT(device, INFO, "DMA is not supported on this device\n");
        return;
    }

    retval = nvswitch_os_set_dma_mask(device->os_handle, hw_dma_width);
    if (retval == NVL_SUCCESS)
    {
        device->dma_addr_width = hw_dma_width;
        return;
    }

    NVSWITCH_PRINT(device, SETUP,
                   "%s: Failed to set DMA mask, trying 32-bit fallback : %d\n",
                   __FUNCTION__, retval);

    retval = nvswitch_os_set_dma_mask(device->os_handle, 32);
    if (retval == NVL_SUCCESS)
    {
        device->dma_addr_width = 32;
        return;
    }

    // failure is not fatal, the driver will just restrict DMA functionality
    NVSWITCH_PRINT(device, ERROR, "Failed to set DMA mask : %d\n", retval);
}

NvlStatus
nvswitch_deassert_link_reset
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    return device->hal.nvswitch_deassert_link_reset(device, link);
}

NvU32
nvswitch_get_sublink_width
(
    nvswitch_device *device,
    NvU32 linkNumber
)
{
    return device->hal.nvswitch_get_sublink_width(device, linkNumber);
}

static void
_nvswitch_unregister_links
(
    nvswitch_device *device
)
{
    nvlink_link *link = NULL;
    NvU32 link_num;
    NvBool is_blacklisted;


    if (!NVSWITCH_IS_DEVICE_INITIALIZED(device))
        return;

    device->nvlink_device->initialized = 0;
    is_blacklisted = (device->device_fabric_state == NVSWITCH_DEVICE_FABRIC_STATE_BLACKLISTED);

    for (link_num = 0; link_num < nvswitch_get_num_links(device); link_num++)
    {
        if (nvlink_lib_get_link(device->nvlink_device, link_num, &link) == NVL_SUCCESS)
        {
            nvlink_lib_unregister_link(link);
            nvswitch_destroy_link(link);
        }
    }

    if (!is_blacklisted)
        nvswitch_inforom_nvlink_flush(device);
}

NvlStatus NV_API_CALL
nvswitch_lib_read_fabric_state
(
    nvswitch_device *device,
    NVSWITCH_DEVICE_FABRIC_STATE *device_fabric_state,
    NVSWITCH_DEVICE_BLACKLIST_REASON *device_blacklist_reason,
    NVSWITCH_DRIVER_FABRIC_STATE *driver_fabric_state
)
{
    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
        return -NVL_BAD_ARGS;

    if (device_fabric_state != NULL)
        *device_fabric_state = device->device_fabric_state;

    if (device_blacklist_reason != NULL)
        *device_blacklist_reason = device->device_blacklist_reason;

    if (driver_fabric_state != NULL)
        *driver_fabric_state = device->driver_fabric_state;

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_lib_blacklist_device
(
    nvswitch_device *device,
    NVSWITCH_DEVICE_BLACKLIST_REASON device_blacklist_reason
)
{
    NvlStatus status;

    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    if (device->device_fabric_state == NVSWITCH_DEVICE_FABRIC_STATE_BLACKLISTED)
    {
        NVSWITCH_PRINT(device, WARN, "Device is already blacklisted\n");
        return -NVL_ERR_NOT_SUPPORTED;
    }

    device->device_fabric_state = NVSWITCH_DEVICE_FABRIC_STATE_BLACKLISTED;
    device->device_blacklist_reason = device_blacklist_reason;

    status = device->hal.nvswitch_write_fabric_state(device);
    if (status != NVL_SUCCESS)
        NVSWITCH_PRINT(device, INFO, "Cannot send fabric state to SOE\n");

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_ctrl_blacklist_device(
    nvswitch_device *device,
    NVSWITCH_BLACKLIST_DEVICE_PARAMS *p
)
{
    NvlStatus status;

    status = nvswitch_lib_blacklist_device(device, p->deviceReason);
    if (status != NVL_SUCCESS)
        return status;

    if (nvswitch_is_tnvl_mode_locked(device))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s(%d): Security locked\n", __FUNCTION__, __LINE__);
    }
    else
    {
        nvswitch_lib_disable_interrupts(device);
    }

    // Unregister links from NVLinkCoreLib, so that link training is not
    // attempted
    _nvswitch_unregister_links(device);

    // Keep device registered for HAL access and Fabric State updates

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_ctrl_set_fm_driver_state(
    nvswitch_device *device,
    NVSWITCH_SET_FM_DRIVER_STATE_PARAMS *p
)
{
    NvU32 prev_fm_status;

    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    prev_fm_status = device->driver_fabric_state;
    device->driver_fabric_state = p->driverState;
    device->fabric_state_timestamp = nvswitch_os_get_platform_time();

    if (prev_fm_status != p->driverState)
    {
        if (nvswitch_lib_notify_client_events(device,
            NVSWITCH_DEVICE_EVENT_FABRIC_STATE) != NVL_SUCCESS)
        { 
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify event\n",
                           __FUNCTION__);
        }
    }

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_ctrl_set_device_fabric_state(
    nvswitch_device *device,
    NVSWITCH_SET_DEVICE_FABRIC_STATE_PARAMS *p
)
{
    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    if (device->device_fabric_state == NVSWITCH_DEVICE_FABRIC_STATE_BLACKLISTED)
        return -NVL_ERR_NOT_SUPPORTED;

    device->device_fabric_state = p->deviceState;
    device->fabric_state_timestamp = nvswitch_os_get_platform_time();

    // If FM had exceeded timeout, reset the status to not timed-out
    if (device->driver_fabric_state == NVSWITCH_DRIVER_FABRIC_STATE_MANAGER_TIMEOUT)
        device->driver_fabric_state = NVSWITCH_DRIVER_FABRIC_STATE_CONFIGURED;

    return NVL_SUCCESS;
}

static NvlStatus
nvswitch_ctrl_set_fm_timeout(
    nvswitch_device *device,
    NVSWITCH_SET_FM_HEARTBEAT_TIMEOUT_PARAMS *p
)
{
    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    device->fm_timeout = p->fmTimeout;

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_ctrl_register_events(
    nvswitch_device *device,
    NVSWITCH_REGISTER_EVENTS_PARAMS *p,
    void *osPrivate
)
{
    NvlStatus status = NVL_SUCCESS;
    NvU32 i;
    NvBool many_events, os_descriptor;
    void *osDescriptor = osPrivate;

    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    status = nvswitch_os_get_supported_register_events_params(&many_events,
                                                              &os_descriptor);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    if ((!many_events && (p->numEvents > 1)) ||
        (p->numEvents == 0))
    {
        return -NVL_BAD_ARGS;
    }

    if (os_descriptor)
    {
        osDescriptor = (void *) p->osDescriptor;
    }

    for (i = 0; i < p->numEvents; i++)
    {
        status = nvswitch_lib_add_client_event(device, osDescriptor, p->eventIds[i]);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to add client event.\n", __FUNCTION__);
            return status;
        }
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_ctrl_unregister_events(
    nvswitch_device *device,
    NVSWITCH_UNREGISTER_EVENTS_PARAMS *p,
    void *osPrivate
)
{
    NvlStatus status = NVL_SUCCESS;
    NvBool many_events, os_descriptor;
    void *osDescriptor = osPrivate;

    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    status = nvswitch_os_get_supported_register_events_params(&many_events,
                                                              &os_descriptor);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    if (os_descriptor)
    {
        osDescriptor = (void *) p->osDescriptor;
    }

    status = nvswitch_lib_remove_client_events(device, osDescriptor);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to remove client event.\n", __FUNCTION__);
        return status;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Sends NACK or drops given inband msg based on message type
 *
 * @Description :
 *
 * @param[in] device            NvSwitch device to contain this link
 * @param[in] linkId            Link ID
 * @param[in] msghdr            Header to the message
 *
 */
static void
nvswitch_send_nack_or_drop
(
    nvswitch_device            *device,
    NvU32                       linkId,
    nvlink_inband_msg_header_t *msghdr
)
{
    switch(msghdr->type)
    {
        case NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REQ:
            device->hal.nvswitch_send_inband_nack(device, (NvU32 *)msghdr, linkId);
            NVSWITCH_PRINT(device, ERROR,
                           "Sending NACK for message (type 0x%x)\n", msghdr->type);
            return;
        default:
            // TODO: Add SXid in future if needed.
            NVSWITCH_PRINT(device, ERROR,
                           "Dropping message (type 0x%x)\n", msghdr->type);
            return;
    }
}

/*
 * @Brief : Deletes all the entries in persistent or non-persistent lists.
 *          Send nacks if requested.
 *
 * @Description :
 *
 * @param[in] device              NVSwitch device to contain this link
 * @param[in] linkId              Link number
 * @param[in] bSendNack           Send nacks if true
 * @param[in] bNonPersistentOnly  Clear only non-persistent list
 */
static void
_nvswitch_inband_clear_lists
(
    nvswitch_device *device,
    NvU32            linkId,
    NvBool           bSendNack,
    NvBool           bNonPersistentOnly
)
{
    nvswitch_inband_data_list *curr = NULL;
    nvswitch_inband_data_list *next = NULL;
    nvlink_inband_msg_header_t *msghdr = NULL;

    nvListForEachEntry_safe(curr, next,
                    &device->link[linkId].inbandData.nonpersistent_list, entry)
    {
        if (bSendNack)
        {
            msghdr = (nvlink_inband_msg_header_t *)curr->data;
            nvswitch_send_nack_or_drop(device, linkId, msghdr);
        }

        nvListDel(&curr->entry);
        nvswitch_os_free(curr);
    }

    if (bNonPersistentOnly)
        return;

    nvListForEachEntry_safe(curr, next,
                    &device->link[linkId].inbandData.persistent_list, entry)
    {
        if (bSendNack)
        {
            msghdr = (nvlink_inband_msg_header_t *)curr->data;
            nvswitch_send_nack_or_drop(device, linkId, msghdr);
        }

        nvListDel(&curr->entry);
        nvswitch_os_free(curr);
    }
}

static void
nvswitch_fabric_state_heartbeat(
    nvswitch_device *device
)
{
    NvU64 age;
    NvU32 linkId;

    if (!NVSWITCH_IS_DEVICE_VALID(device))
        return;

    age = nvswitch_os_get_platform_time() - device->fabric_state_timestamp;

    // Check to see if we have exceeded the FM timeout
    if (device->driver_fabric_state == NVSWITCH_DRIVER_FABRIC_STATE_CONFIGURED &&
        age > (NvU64)device->fm_timeout * 1000ULL * 1000ULL)
         device->driver_fabric_state = NVSWITCH_DRIVER_FABRIC_STATE_MANAGER_TIMEOUT;

    //
    // If FM is not running, clear pending non-persistent messages. Persistent
    // messages can be processed by the FM when it restarts.
    //
    if (device->driver_fabric_state != NVSWITCH_DRIVER_FABRIC_STATE_CONFIGURED)
    {
        for (linkId = 0; linkId < nvswitch_get_num_links(device); linkId++)
            _nvswitch_inband_clear_lists(device, linkId,
                                         NV_TRUE /* Nack */,
                                         NV_TRUE /* Non-persistent only */);
    }

    (void)device->hal.nvswitch_write_fabric_state(device);
}

static NvlStatus
_nvswitch_ctrl_set_training_error_info
(
    nvswitch_device *device,
    NVSWITCH_SET_TRAINING_ERROR_INFO_PARAMS *p
)
{
    return device->hal.nvswitch_set_training_error_info(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_fatal_error_scope
(
    nvswitch_device *device,
    NVSWITCH_GET_FATAL_ERROR_SCOPE_PARAMS *pParams
)
{
    return device->hal.nvswitch_ctrl_get_fatal_error_scope(device, pParams);
}

static NvlStatus
_nvswitch_ctrl_therm_get_temperature_limit
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_TEMPERATURE_LIMIT_PARAMS *pParams
)
{
    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    return device->hal.nvswitch_ctrl_therm_get_temperature_limit(device, pParams);
}

//
// Construct an port event log
//
// If port_event_log_size > 0 a circular buffer is created to record port events
//
NvlStatus
_nvswitch_construct_port_event_log
(
    NVSWITCH_PORT_EVENT_LOG_TYPE *port_events,
    NvU32 port_event_log_size,
    NvBool overwritable
)
{
    NvlStatus retval = NVL_SUCCESS;

    NVSWITCH_ASSERT(port_events != NULL);

    port_events->port_event_start = 0;
    port_events->port_event_count = 0;
    port_events->port_event_total = 0;
    port_events->port_event_log_size = 0; 
    port_events->port_event_log = NULL;
    port_events->overwritable = overwritable;
    port_events->bOverflow = NV_FALSE;

    if (port_event_log_size > 0)
    {
        port_events->port_event_log = nvswitch_os_malloc(port_event_log_size * sizeof(NVSWITCH_PORT_EVENT_TYPE));
    }

    if (port_events->port_event_log != NULL)
    {
        port_events->port_event_log_size = port_event_log_size;
        nvswitch_os_memset(port_events->port_event_log, 0, port_events->port_event_log_size * sizeof(NVSWITCH_PORT_EVENT_TYPE));
    }

    if (port_event_log_size != port_events->port_event_log_size)
    {
        retval = -NVL_NO_MEM;
    }

    return retval;
}

//
// Destroy an error log
//
void
_nvswitch_destroy_port_event_log
(
    nvswitch_device *device,
    NVSWITCH_PORT_EVENT_LOG_TYPE *port_events
)
{
    if (port_events == NULL)
        return;

    port_events->port_event_start = 0;
    port_events->port_event_count = 0;
    port_events->port_event_log_size = 0;
    port_events->bOverflow = NV_FALSE;

    if (port_events->port_event_log != NULL)
    {
        nvswitch_os_free(port_events->port_event_log);
        port_events->port_event_log = NULL;
    }
}

NvlStatus
nvswitch_lib_initialize_device
(
    nvswitch_device *device
)
{
    NvlStatus retval = NVL_SUCCESS;
    NvU8 link_num;
    nvlink_link *link = NULL;
    NvBool is_blacklisted_by_os = NV_FALSE;

    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    if (NVSWITCH_IS_DEVICE_INITIALIZED(device))
    {
        NVSWITCH_PRINT(device, SETUP, "Device is already initialized!\n");
        return NVL_SUCCESS;
    }

    NVSWITCH_PRINT(device, SETUP,
        "Initializing nvswitch at (%04x:%02x:%02x.%02x)\n",
        device->nvlink_device->pciInfo.domain,
        device->nvlink_device->pciInfo.bus,
        device->nvlink_device->pciInfo.device,
        device->nvlink_device->pciInfo.function);

    nvListInit(&device->client_events_list);

    for (link_num=0; link_num < nvswitch_get_num_links(device); link_num++)
    {
        nvListInit(&device->link[link_num].inbandData.persistent_list);
        nvListInit(&device->link[link_num].inbandData.nonpersistent_list);
    }

    retval = nvswitch_lib_load_platform_info(device);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to load platform information\n");
        return retval;
    }

    if (nvswitch_is_soe_supported(device))
    {
        retval = _nvswitch_construct_soe(device);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "SOE is not supported, skipping construct\n");
    }
    if (nvswitch_is_cci_supported(device))
    {
        retval = _nvswitch_construct_cci(device);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "Failed to construct CCI: %d\n", retval);
            goto nvswitch_initialize_cci_fail;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "CCI is not supported, skipping construct\n");
    }

    _nvswitch_set_dma_mask(device);

    retval = _nvswitch_initialize_device_state(device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to initialize device state: %d!\n",
            retval);
        goto nvswitch_initialize_device_state_fail;
    }

    device->hal.nvswitch_load_uuid(device);

    /*
     * Check module parameters for blacklisted device
     */
    if (nvswitch_os_is_uuid_in_blacklist(&device->uuid) == NV_TRUE)
    {
        NVSWITCH_PRINT(device, SETUP,
            "Blacklisted nvswitch at (%04x:%02x:%02x.%02x)\n",
            device->nvlink_device->pciInfo.domain,
            device->nvlink_device->pciInfo.bus,
            device->nvlink_device->pciInfo.device,
            device->nvlink_device->pciInfo.function);
        is_blacklisted_by_os = NV_TRUE;
        // initialization continues until we have updated InfoROM...
    }

    if (nvswitch_is_inforom_supported(device))
    {
        retval = nvswitch_initialize_inforom(device);
        if (NVL_SUCCESS != retval)
        {
            NVSWITCH_PRINT(device, ERROR,
                    "Failed to initialize InfoROM rc: %d\n",
                    retval);
            goto nvswitch_initialize_device_state_fail;
        }

        retval = nvswitch_initialize_inforom_objects(device);
        if (NVL_SUCCESS != retval)
        {
            NVSWITCH_PRINT(device, ERROR,
                        "Failed to initialize InfoROM objects! rc:%d\n",
                        retval);
            goto nvswitch_initialize_inforom_fail;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, INFO,
                "InfoROM is not supported, skipping init\n");
    }

    (void)device->hal.nvswitch_read_oob_blacklist_state(device);
    (void)device->hal.nvswitch_write_fabric_state(device);

    nvswitch_task_create(device, &nvswitch_fabric_state_heartbeat,
                         NVSWITCH_HEARTBEAT_INTERVAL_NS,
                         NVSWITCH_TASK_TYPE_FLAGS_RUN_EVEN_IF_DEVICE_NOT_INITIALIZED);

    //
    // Blacklisted devices return successfully in order to preserve the fabric state heartbeat
    // and ensure OOB utilities don't think the driver has died
    //
    if (device->device_blacklist_reason == NVSWITCH_DEVICE_BLACKLIST_REASON_MANUAL_OUT_OF_BAND)
    {
        NVSWITCH_PRINT(device, SETUP,
            "Blacklisted nvswitch at (%04x:%02x:%02x.%02x)\n",
            device->nvlink_device->pciInfo.domain,
            device->nvlink_device->pciInfo.bus,
            device->nvlink_device->pciInfo.device,
            device->nvlink_device->pciInfo.function);
        return NVL_SUCCESS;
    }

    if (is_blacklisted_by_os)
    {
        (void)nvswitch_lib_blacklist_device(device, NVSWITCH_DEVICE_BLACKLIST_REASON_MANUAL_IN_BAND);
        return NVL_SUCCESS;
    }

    for (link_num=0; link_num < nvswitch_get_num_links(device); link_num++)
    {
        if (!nvswitch_is_link_valid(device, link_num))
        {
            continue;
        }

        retval = nvswitch_create_link(device, link_num, &link);
        if (NVL_SUCCESS != retval)
        {
            NVSWITCH_PRINT(device, ERROR,
                "Failed to create link %d : %d!\n",
                link_num,
                retval);
            goto nvswitch_link_fail;
        }

        retval = nvlink_lib_register_link(device->nvlink_device, link);
        if (NVL_SUCCESS != retval)
        {
            NVSWITCH_PRINT(device, ERROR,
                "Failed to register link %d with the nvlink core : %d!\n",
                link_num,
                retval);

            // Free the single dangling link.
            nvswitch_destroy_link(link);

            goto nvswitch_link_fail;
        }

        nvswitch_reset_persistent_link_hw_state(device, link_num);

        //
        // During Nvswitch initialization, the default L1 thresholds are programmed by the
        // BIOS from the BIOS tables. Save these L1 Threshold Values in scratch registers
        // for use when resetting the thresholds to default.
        //
        nvswitch_program_l1_scratch_reg(device, link_num);

        //
        // WAR : Initializing the L1 threshold registers at this point as a WAR for
        // Bug 3963639 where it was discussed that the L1 threshold register should have 
        // the default value for all available links and not just for active links.
        //
        nvswitch_init_lpwr_regs(link);
    }

    retval = nvswitch_set_training_mode(device);

    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to determine link training mode! rc: %d\n", retval);
        goto nvswitch_link_fail;
    }
    // Initialize select scratch registers to 0x0
    device->hal.nvswitch_init_scratch(device);

    retval = nvswitch_construct_error_log(&device->log_FATAL_ERRORS, 1024, NV_FALSE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to construct log_FATAL_ERRORS! rc: %d\n", retval);
        goto nvswitch_construct_error_log_fail;
    }

    retval = nvswitch_construct_error_log(&device->log_NONFATAL_ERRORS, 1024, NV_TRUE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to construct log_NONFATAL_ERRORS! rc: %d\n", retval);
        goto nvswitch_construct_error_log_fail;
    }

    retval = _nvswitch_construct_port_event_log(&device->log_PORT_EVENTS, NVSWITCH_PORT_EVENT_LOG_SIZE, NV_TRUE);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to construct log_PORT_EVENTS! rc: %d\n", retval);
        goto nvswitch_construct_port_event_log_fail;
    }

    if (device->regkeys.latency_counter == NV_SWITCH_REGKEY_LATENCY_COUNTER_LOGGING_ENABLE)
    {
        nvswitch_task_create(device, &nvswitch_internal_latency_bin_log,
            nvswitch_get_latency_sample_interval_msec(device) * NVSWITCH_INTERVAL_1MSEC_IN_NS * 9/10, 0);
    }

    nvswitch_task_create(device, &nvswitch_ecc_writeback_task,
        (60 * NVSWITCH_INTERVAL_1SEC_IN_NS), 0);

    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, WARN,
        "%s: Skipping setup of NvSwitch thermal alert monitoring\n",
            __FUNCTION__);
    }
    else
    {
        nvswitch_task_create(device, &nvswitch_monitor_thermal_alert,
            100*NVSWITCH_INTERVAL_1MSEC_IN_NS, 0);
    }

    device->nvlink_device->initialized = 1;

    return NVL_SUCCESS;

nvswitch_construct_error_log_fail:
    //free allocated memory to avoid leaking
    nvswitch_destroy_error_log(device, &device->log_FATAL_ERRORS);
    nvswitch_destroy_error_log(device, &device->log_NONFATAL_ERRORS);

nvswitch_construct_port_event_log_fail:
    //free allocated memory to avoid leaking
    _nvswitch_destroy_port_event_log(device, &device->log_PORT_EVENTS);

nvswitch_link_fail:
    // Track down all links that successfully registered.
    for (link_num = 0; link_num < nvswitch_get_num_links(device); link_num++)
    {
        if (nvlink_lib_get_link(device->nvlink_device, link_num, &link) == NVL_SUCCESS)
        {
            nvlink_lib_unregister_link(link);
            nvswitch_destroy_link(link);
        }
    }

    nvswitch_destroy_inforom_objects(device);

nvswitch_initialize_inforom_fail:
    nvswitch_destroy_inforom(device);

nvswitch_initialize_device_state_fail:
    _nvswitch_destruct_cci(device);
nvswitch_initialize_cci_fail:
    _nvswitch_destruct_soe(device);
    nvswitch_tasks_destroy(device);

    return retval;
}

NvBool
nvswitch_lib_validate_device_id
(
    NvU32 device_id
)
{
    if (nvswitch_is_lr10_device_id(device_id))
    {
        return NV_TRUE;
    }
    if (nvswitch_is_ls10_device_id(device_id))
    {
        return NV_TRUE;
    }
    return NV_FALSE;
}

NvlStatus
nvswitch_lib_post_init_device
(
    nvswitch_device *device
)
{
    NvlStatus retval;
    NvlStatus status;
    NvU32     link_num;
    NvU64     mode;
    nvlink_link *link;
    NvU64 enabledLinkMaskNonCci;

    if (!NVSWITCH_IS_DEVICE_INITIALIZED(device))
    {
        return -NVL_ERR_INVALID_STATE;
    }

    retval = _nvswitch_post_init_device_setup(device);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }
    
    if (nvswitch_is_bios_supported(device))
    {
        retval = nvswitch_bios_get_image(device);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }

        retval = nvswitch_parse_bios_image(device);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Skipping BIOS parsing since BIOS is unsupported.\n",
            __FUNCTION__);
    }

    if (nvswitch_is_cci_supported(device))
    {
        retval = cciLoad(device);
        if (NVL_SUCCESS != retval)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Init CCI failed\n",
                __FUNCTION__);
            return retval;
        }

        enabledLinkMaskNonCci = nvswitch_get_enabled_link_mask(device);

        FOR_EACH_INDEX_IN_MASK(64, link_num, enabledLinkMaskNonCci)
        {
            if (cciIsLinkManaged(device, link_num))
            {
                enabledLinkMaskNonCci = enabledLinkMaskNonCci & ~NVBIT64(link_num);
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;

        if (enabledLinkMaskNonCci != 0)
        {
            nvswitch_task_create(device, &_nvswitch_update_link_state_led,
                                NVSWITCH_INTERVAL_1SEC_IN_NS, 0);
        }
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "%s: Skipping CCI init.\n",
            __FUNCTION__);
    }

    retval = _nvswitch_setup_system_registers(device);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    nvswitch_smbpbi_post_init(device);

    // ALI launched by VBIOS on silicon
    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        (void)nvswitch_launch_ALI(device);
    }

    //
    // There is an edge case where a hyperisor may not send same number
    // of reset to switch and GPUs, so try to re-train links in fault
    // if possible
    //
    for (link_num=0; link_num < nvswitch_get_num_links(device); link_num++)
    {
        // Sanity check
        if (!nvswitch_is_link_valid(device, link_num))
        {
            continue;
        }

        // CCI links are trained and managed by SOE, skip any driver based training
        if (cciIsLinkManaged(device, link_num))
        {
            continue;
        }
        status = nvlink_lib_get_link(device->nvlink_device, link_num, &link);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to get link for LinkId %d\n",
                        __FUNCTION__, link_num);
            continue;
        }

        // If the link is in fault then re-train
        if(_nvswitch_corelib_get_dl_link_mode(link, &mode) != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: nvlipt_lnk_status: Failed to check link mode! LinkId %d\n",
                        __FUNCTION__, link_num);
        }
        else if(mode == NVLINK_LINKSTATE_FAULT)
        {
            NVSWITCH_PRINT(device, INFO, "%s: retraining LinkId %d\n",
                        __FUNCTION__, link_num);
            nvswitch_reset_and_train_link(device, link);
        }
    }

    return NVL_SUCCESS;
}

void
nvswitch_lib_post_init_blacklist_device
(
    nvswitch_device *device
)
{
    _nvswitch_post_init_blacklist_device_setup(device);
}

void
_nvswitch_check_pending_data_and_notify
(
    nvswitch_device       *device,
    NVSWITCH_CLIENT_EVENT *event
)
{
    switch (event->eventId)
    {
        case NVSWITCH_DEVICE_EVENT_INBAND_DATA:
        {
            NvU32 i;

            for (i = 0; i < nvswitch_get_num_links(device); i++)
            {
                if (!nvListIsEmpty(&device->link[i].inbandData.persistent_list) ||
                    !nvListIsEmpty(&device->link[i].inbandData.nonpersistent_list))
                {
                    (void)nvswitch_os_notify_client_event(device->os_handle,
                                                          event->private_driver_data,
                                                          event->eventId);
                }
            }
            break;
        }

        default:
            return;
    }
}

/*!
 * @brief: Gets the client event associated with the file descriptor
 *         if it already exists in the Device's client event list.
 *
 *         If found, and if there is pending data for the event,
 *         the event is triggered before returning to unblock the
 *         client right away.
 */
NvlStatus
nvswitch_lib_get_client_event
(
    nvswitch_device       *device,
    void                  *osPrivate,
    NVSWITCH_CLIENT_EVENT **ppClientEvent
)
{
    NVSWITCH_CLIENT_EVENT *curr = NULL;

    *ppClientEvent = NULL;

    if(!NVSWITCH_IS_DEVICE_VALID(device))
    {
        return -NVL_BAD_ARGS;
    }

    nvListForEachEntry(curr, &device->client_events_list, entry)
    {
        if (curr->private_driver_data == osPrivate)
        {
            *ppClientEvent = curr;
            _nvswitch_check_pending_data_and_notify(device, curr);
            return NVL_SUCCESS;
        }
    }

    return -NVL_NOT_FOUND;
}

/*!
 * @brief: Adds an event to the front of the
 *         Device's client event list.
 */
NvlStatus
nvswitch_lib_add_client_event
(
    nvswitch_device *device,
    void            *osPrivate,
    NvU32           eventId
)
{
    NVSWITCH_CLIENT_EVENT *newEvent;
    NvlStatus status = NVL_SUCCESS;

    if (!NVSWITCH_IS_DEVICE_VALID(device))
    {
        return -NVL_BAD_ARGS;
    }

    if (eventId >= NVSWITCH_DEVICE_EVENT_COUNT)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Invalid event Id.\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    // Invoke OS specific API to add event.
    status = nvswitch_os_add_client_event(device->os_handle,
                                          osPrivate,
                                          eventId);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to add client event.\n", __FUNCTION__);
        return status;
    }

    newEvent = nvswitch_os_malloc(sizeof(*newEvent));
    if (newEvent == NULL)
    {
        return -NVL_NO_MEM;
    }

    newEvent->eventId             = eventId;
    newEvent->private_driver_data = osPrivate;

    nvListAdd(&newEvent->entry, &device->client_events_list);

    return NVL_SUCCESS;
}

/*!
 * @brief: Removes all events corresponding to osPrivate,
 *         from the Device's client event list.
 */
NvlStatus
nvswitch_lib_remove_client_events
(
    nvswitch_device *device,
    void            *osPrivate
)
{
    NVSWITCH_CLIENT_EVENT *curr = NULL;
    NVSWITCH_CLIENT_EVENT *next = NULL;
    NvlStatus status = NVL_SUCCESS;

    //
    // Device shutdown may happen before this is called, so return
    // if device is gone
    //
    if (!NVSWITCH_IS_DEVICE_VALID(device))
    {
        return NVL_SUCCESS;
    }

    nvListForEachEntry_safe(curr, next, &device->client_events_list, entry)
    {
        if (curr->private_driver_data == osPrivate)
        {
            nvListDel(&curr->entry);
            nvswitch_os_free(curr);
        }
    }

    // Invoke OS specific API to remove event.
    status = nvswitch_os_remove_client_event(device->os_handle,
                                             osPrivate);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to remove client events.\n", __FUNCTION__);
        return status;
    }

    return NVL_SUCCESS;
}

/*!
 * @brief: Notifies all events with matching event id in the
 *         Device's client event list.
 */
NvlStatus
nvswitch_lib_notify_client_events
(
    nvswitch_device *device,
    NvU32            eventId
)
{
    NvlStatus status;
    NVSWITCH_CLIENT_EVENT *curr = NULL;

    if (!NVSWITCH_IS_DEVICE_VALID(device))
    {
        return -NVL_BAD_ARGS;
    }

    if (eventId >= NVSWITCH_DEVICE_EVENT_COUNT)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Invalid event Id.\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    nvListForEachEntry(curr, &device->client_events_list, entry)
    {
        if (curr->eventId == eventId)
        {
            // OS specific event notification.
            status = nvswitch_os_notify_client_event(device->os_handle,
                                                     curr->private_driver_data,
                                                     eventId);
            if (status != NVL_SUCCESS)
            {
                return status;
            }
        }
    }

    return NVL_SUCCESS;
}

void
nvswitch_record_port_event
(
    nvswitch_device *device, 
    NVSWITCH_PORT_EVENT_LOG_TYPE *port_events, 
    NvU32 link_id,
    NvU8 port_event_type
)
{
    NvU32 idx;

    NVSWITCH_ASSERT(port_events != NULL);

    // If no port events log has been created, then don't log it.
    if ((port_events->port_event_log_size != 0) && 
        (port_events->port_event_log != NULL))
    {
        idx = (port_events->port_event_start + port_events->port_event_count) 
                                        % port_events->port_event_log_size;

        if (port_events->port_event_count == port_events->port_event_log_size)
        {
            // Error: ring buffer is already full/
            if (port_events->overwritable)
            {
                port_events->port_event_start = (port_events->port_event_start + 1) 
                                                % port_events->port_event_log_size;
                port_events->bOverflow = NV_TRUE;
            }
            else
            {
                // No logging, ring buffer is full
                return;
            }
        }
        else
        {
            port_events->port_event_count++;
        }
        // Log port event info
        port_events->port_event_log[idx].link_id = link_id;
        port_events->port_event_log[idx].port_event_type = port_event_type;

        // Log tracking info
        port_events->port_event_log[idx].time = nvswitch_os_get_platform_time();
        port_events->port_event_log[idx].local_port_event_num = port_events->port_event_total;
    }
    port_events->port_event_total++;
}

/*
 * @Brief : Retrives a port event entry by index.
 *
 * @Description : Retrieves the port_event at index port_event_idx. If index is out 
 *                of range, returns an empty port event entry with port_event_type = 2
 *
 * @param[in] device              NVSwitch device to contain this link
 * @param[in] port_events         Log of all port events with metadata
 * @param[in] port_event_idx      Index of entry to retrieve (0 = oldest port event)
 * @param[out] port_event_count  Clear only non-persistent list
 */
void
nvswitch_get_port_event
(
    nvswitch_device *device,
    NVSWITCH_PORT_EVENT_LOG_TYPE *port_events,
    NVSWITCH_PORT_EVENT_TYPE *port_event_entry,
    NvU32 port_event_idx,
    NvU32 *port_event_count
)
{
    NvU32 idx;
    NVSWITCH_ASSERT(port_events != NULL);

    if (port_event_entry != NULL)
    {
        // Index is out of range
        if (port_event_idx >= port_events->port_event_count)
        {
            nvswitch_os_memset(port_event_entry, 0, sizeof(*port_event_entry));
            port_event_entry->port_event_type = NVSWITCH_PORT_EVENT_TYPE_INVALID;
            port_event_entry->time = nvswitch_os_get_platform_time();
        }
        else
        {
            idx = (port_events->port_event_start + port_event_idx) % port_events->port_event_log_size;
            *port_event_entry = port_events->port_event_log[idx];
        }
    }

    if (port_event_count)
    {
        *port_event_count = port_events->port_event_count;
    }
}

NvlStatus
nvswitch_ctrl_get_port_events
(
    nvswitch_device *device,
    NVSWITCH_GET_PORT_EVENTS_PARAMS *p
)
{
    NvU32 index = 0;
    NvU32 count = 0;
    NVSWITCH_PORT_EVENT_LOG_TYPE *port_events = &device->log_PORT_EVENTS;
    NVSWITCH_PORT_EVENT_TYPE port_event;

    nvswitch_os_memset(p->portEvent, 0, sizeof(NVSWITCH_PORT_EVENT)
                                    *NVSWITCH_PORT_EVENT_COUNT_SIZE);
    p->nextPortEventIndex = port_events->port_event_total;
    p->portEventCount = 0;
    p->bOverflow = port_events->bOverflow;

    // Return if there are no more port events to get
    nvswitch_get_port_event(device, port_events, &port_event, index, &count);
    if (count == 0)
    {
        return NVL_SUCCESS;
    }

    // If port event's local_port_Event_num is smaller than the portEventIndex
    // passed in by the client, fast-forward index by the difference.
    // This will skip over port events that were previously read by the client.
    if (port_event.local_port_event_num < p->portEventIndex)
    {
        index = (NvU32) (p->portEventIndex - port_event.local_port_event_num);
    }

    // Return if there are no more events after fast-forwarding.
    if (index >= count)
    {
        return NVL_SUCCESS;
    }

    while ((p->portEventCount < NVSWITCH_PORT_EVENT_COUNT_SIZE) && (index < count))
    {
        nvswitch_get_port_event(device, port_events, &port_event, index, NULL);

        p->portEvent[p->portEventCount].port_event_type = port_event.port_event_type;
        p->portEvent[p->portEventCount].link_id = port_event.link_id;
        p->portEvent[p->portEventCount].time = port_event.time;

        p->portEventCount++;
        index++;
    }

    p->portEventIndex = port_event.local_port_event_num + 1;

    return NVL_SUCCESS;
}

/*!
   @brief: Release ROM image from memory.
*/
void
_nvswitch_destroy_rom(nvswitch_device *device)
{
    if (device->biosImage.pImage != NULL)
    {
        nvswitch_os_free(device->biosImage.pImage);
        device->biosImage.pImage = NULL;
    }
}

/*!
 * @brief: Free the device's client event list
 */
static void
_nvswitch_destroy_event_list(nvswitch_device *device)
{
    NVSWITCH_CLIENT_EVENT *curr = NULL;
    NVSWITCH_CLIENT_EVENT *next = NULL;

    nvListForEachEntry_safe(curr, next, &device->client_events_list, entry)
    {
        nvListDel(&curr->entry);
        nvswitch_os_free(curr);
    }
}

NvlStatus
nvswitch_lib_shutdown_device
(
    nvswitch_device *device
)
{
    NVSWITCH_INBAND_FLUSH_DATA_PARAMS p;

    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    //
    // Set fabric state to offline
    //
    if (device->device_fabric_state != NVSWITCH_DEVICE_FABRIC_STATE_BLACKLISTED)
        device->device_fabric_state = NVSWITCH_DEVICE_FABRIC_STATE_OFFLINE;
    device->driver_fabric_state = NVSWITCH_DRIVER_FABRIC_STATE_OFFLINE;
    (void)device->hal.nvswitch_write_fabric_state(device);

    nvswitch_hw_counter_shutdown(device);

    // FLUSH any pending messages to avoid memory leaks
    p.linkMask = nvswitch_get_enabled_link_mask(device);
    _nvswitch_ctrl_inband_flush_data(device, &p);

    _nvswitch_destruct_cci(device);
    _nvswitch_led_shutdown(device);

    _nvswitch_unregister_links(device);

    nvswitch_destroy_error_log(device, &device->log_FATAL_ERRORS);
    nvswitch_destroy_error_log(device, &device->log_NONFATAL_ERRORS);

    _nvswitch_destroy_port_event_log(device, &device->log_PORT_EVENTS);

    nvswitch_smbpbi_unload(device);
    _nvswitch_destroy_event_list(device);

    nvswitch_destroy_inforom_objects(device);
    nvswitch_destroy_inforom(device);

    nvswitch_smbpbi_destroy(device);

    nvswitch_destroy_device_state(device);

    _nvswitch_destroy_rom(device);

    _nvswitch_destruct_soe(device);

    nvswitch_tasks_destroy(device);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_lib_get_log_count
(
    nvswitch_device *device,
    NvU32 *fatal, NvU32 *nonfatal, NvU32 *portEvent
)
{
    if (!NVSWITCH_IS_DEVICE_INITIALIZED(device) ||
        fatal == NULL || nonfatal == NULL || portEvent == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    *fatal = device->log_FATAL_ERRORS.error_count;
    *nonfatal = device->log_NONFATAL_ERRORS.error_count;
    *portEvent = device->log_PORT_EVENTS.port_event_count;
    // No report of log_INFO currently

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_lib_load_platform_info
(
    nvswitch_device *device
)
{
    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    device->hal.nvswitch_determine_platform(device);

    return NVL_SUCCESS;
}

void
nvswitch_lib_get_device_info
(
    nvswitch_device *device,
    struct nvlink_pci_info **pciInfo
)
{
    if (!NVSWITCH_IS_DEVICE_VALID(device) || pciInfo == NULL)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    *pciInfo = &device->nvlink_device->pciInfo;
}

NvlStatus
nvswitch_lib_get_bios_version
(
    nvswitch_device *device,
    NvU64 *version
)
{
    NVSWITCH_GET_BIOS_INFO_PARAMS p = { 0 };
    NvlStatus ret;

    if (!device)
        return -NVL_BAD_ARGS;

    ret = device->hal.nvswitch_ctrl_get_bios_info(device, &p);

    if (version != NULL)
    {
    *version = p.version;
    }

    return ret;
}

NvlStatus
nvswitch_lib_use_pin_irq
(
     nvswitch_device *device
)
{
    return IS_FMODEL(device);
}


NvlStatus
nvswitch_lib_register_device
(
    NvU16 pci_domain,
    NvU8 pci_bus,
    NvU8 pci_device,
    NvU8 pci_func,
    NvU16 pci_device_id,
    void *os_handle,
    NvU32 os_instance,
    nvswitch_device **return_device
)
{
    nvswitch_device *device  = NULL;
    nvlink_device   *coreDev = NULL;
    NvlStatus        retval  = NVL_SUCCESS;

    if (!nvlink_lib_is_initialized())
    {
        NVSWITCH_PRINT(device, ERROR,
            "NVLink core lib isn't initialized yet!\n");
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    if (return_device == NULL || os_handle == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    *return_device = NULL;

    device = nvswitch_os_malloc(sizeof(*device));
    if (NULL == device)
    {
        NVSWITCH_PRINT(device, ERROR,
            "nvswitch_os_malloc during device creation failed!\n");
        return -NVL_NO_MEM;
    }
    nvswitch_os_memset(device, 0, sizeof(*device));

    nvswitch_os_snprintf(device->name, sizeof(device->name),
         NVSWITCH_DEVICE_NAME "%d", os_instance);

    coreDev = nvswitch_os_malloc(sizeof(*coreDev));
    if (NULL == coreDev)
    {
        NVSWITCH_PRINT(device, ERROR,
            "nvswitch_os_malloc during device creation failed!\n");

        retval = -NVL_NO_MEM;
        goto nvlink_lib_register_device_fail;
    }
    nvswitch_os_memset(coreDev, 0, sizeof(*coreDev));

    coreDev->driverName =
        nvswitch_os_malloc(sizeof(NVSWITCH_DRIVER_NAME));
    if (coreDev->driverName == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "nvswitch_os_malloc during device creation failed!\n");

        retval = -NVL_NO_MEM;
        goto nvlink_lib_register_device_fail;
    }
    nvswitch_os_memcpy(coreDev->driverName, NVSWITCH_DRIVER_NAME,
                       sizeof(NVSWITCH_DRIVER_NAME));

    device->os_handle   = os_handle;
    device->os_instance = os_instance;

    device->nvlink_device             = coreDev;
    device->nvlink_device->deviceName = device->name;
    device->nvlink_device->uuid = NULL; // No UUID support for switch

    device->nvlink_device->pciInfo.domain      = pci_domain;
    device->nvlink_device->pciInfo.bus         = pci_bus;
    device->nvlink_device->pciInfo.device      = pci_device;
    device->nvlink_device->pciInfo.function    = pci_func;
    device->nvlink_device->pciInfo.pciDeviceId = pci_device_id;

    // nvlink_device has a back pointer to nvswitch_device
    device->nvlink_device->pDevInfo = device;
    device->nvlink_device->type = NVLINK_DEVICE_TYPE_NVSWITCH;

    //
    // Initialize the Fabric State
    //
    device->fm_timeout = NVSWITCH_DEFAULT_FM_HEARTBEAT_TIMEOUT_MSEC;
    device->fabric_state_sequence_number = 0;
    device->driver_fabric_state = NVSWITCH_DRIVER_FABRIC_STATE_STANDBY;
    device->device_fabric_state = NVSWITCH_DEVICE_FABRIC_STATE_STANDBY;
    device->device_blacklist_reason = NVSWITCH_DEVICE_BLACKLIST_REASON_NONE;

    //
    // Initialize TNVL Mode
    //
    device->tnvl_mode = NVSWITCH_DEVICE_TNVL_MODE_DISABLED;

    //
    // Initialize HAL connectivity as early as possible so that other lib
    // interfaces can work.
    //
    retval = _nvswitch_setup_hal(device, device->nvlink_device->pciInfo.pciDeviceId);
    if (retval != NVL_SUCCESS)
    {
        goto nvlink_lib_register_device_fail;
    }

    //
    // Initialize regkeys as early as possible so that most routines can take
    // advantage of them.
    //
    _nvswitch_init_device_regkeys(device);

    // After regkeys have been set then only set the enableALI field.
    device->nvlink_device->enableALI = (device->regkeys.link_training_mode ==
                        NV_SWITCH_REGKEY_LINK_TRAINING_SELECT_ALI) ? NV_TRUE:NV_FALSE;

    retval = nvlink_lib_register_device(device->nvlink_device);
    if (NVL_SUCCESS != retval)
    {
        NVSWITCH_PRINT(device, ERROR,
            "nvlinklib register device failed!\n");
        goto nvlink_lib_register_device_fail;
    }

    *return_device = device;

    NVSWITCH_PRINT(device, SETUP,
        "Successfully registered with nvlinkcore\n");

    return retval;

nvlink_lib_register_device_fail:

    if (NULL != coreDev)
    {
        nvswitch_os_free(coreDev->driverName);
        nvswitch_os_free(coreDev);
    }

    if (NULL != device)
        nvswitch_os_free(device);

    return retval;
}

void
nvswitch_lib_unregister_device
(
    nvswitch_device *device
)
{
    if (!NVSWITCH_IS_DEVICE_VALID(device))
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    nvlink_lib_unregister_device(device->nvlink_device);

    nvswitch_os_free(device->nvlink_device->driverName);
    nvswitch_os_free(device->nvlink_device);
    nvswitch_os_free(device);

    return;
}

/*!
 * @brief: Gets the mask of valid I2C ports on the
 *         Device.
 */
NvlStatus
nvswitch_lib_get_valid_ports_mask
(
    nvswitch_device *device,
    NvU32 *validPortsMask
)
{
    NvU32 port_info;
    NvU32 i;
    NvU32 ports_mask = 0;
    NvBool is_i2c_access_allowed;
    NvBool is_port_allowed;

    if (!NVSWITCH_IS_DEVICE_VALID(device) ||
        (validPortsMask == NULL))
    {
        return -NVL_BAD_ARGS;
    }

    is_i2c_access_allowed = (device->regkeys.i2c_access_control ==
                             NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL_ENABLE) ?
                                NV_TRUE : NV_FALSE;

    for (i = 0; i < NVSWITCH_MAX_I2C_PORTS; i++)
    {
        port_info = nvswitch_i2c_get_port_info(device, i);

        is_port_allowed = is_i2c_access_allowed ? NV_TRUE :
                              FLD_TEST_DRF(_I2C, _PORTINFO, _ACCESS_ALLOWED, _TRUE,
                                           port_info);

        if (is_port_allowed &&
            FLD_TEST_DRF(_I2C, _PORTINFO, _DEFINED, _PRESENT, port_info))
        {
            ports_mask |= NVBIT(i);
        }
    }

    *validPortsMask = ports_mask;
    return NVL_SUCCESS;
}

/*!
 * @brief: Returns if the I2C transactions are supported.
 */
NvBool
nvswitch_lib_is_i2c_supported
(
    nvswitch_device *device
)
{
    if (!NVSWITCH_IS_DEVICE_VALID(device))
    {
        NVSWITCH_ASSERT(0);
        return NV_FALSE;
    }

    return nvswitch_is_i2c_supported(device);
}

static NvlStatus
_nvswitch_perform_i2c_transfer
(
    nvswitch_device *device,
    NvU32 client,
    NvU8 type,
    NvU16 addr,
    NvU8 port,
    NvU8 cmd,
    NvU32 msgLength,
    NvU8 *pData
)
{
    NvlStatus status;
    NvU16 deviceAddr;
    NvU32 speedMode;
    NvBool bIsRead = NV_FALSE;
    NvU32 flags = 0;
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS i2c_params = {0};
    NvBool is_i2c_access_allowed;

    if (!nvswitch_os_is_admin())
    {
        return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
    }

    is_i2c_access_allowed = (device->regkeys.i2c_access_control ==
                             NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL_ENABLE) ?
                                NV_TRUE : NV_FALSE;

    //
    // The address needs to be shifted by 1,
    // See NVSWITCH_CTRL_I2C_INDEXED_PARAMS
    //
    deviceAddr = addr << 1;
    speedMode  = device->pI2c->Ports[port].defaultSpeedMode;
    flags      = DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _START, _SEND)              |
                 DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _STOP, _SEND)               |
                 DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _ADDRESS_MODE, _7BIT)       |
                 DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _FLAVOR, _HW)               |
                 DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _BLOCK_PROTOCOL, _DISABLED) |
                 DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _TRANSACTION_MODE, _NORMAL);

    switch (speedMode)
    {
        case NVSWITCH_I2C_SPEED_MODE_1000KHZ:
        {
            flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _1000KHZ, flags);
            break;
        }
        case NVSWITCH_I2C_SPEED_MODE_400KHZ:
        {
            flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _400KHZ, flags);
            break;
        }
        case NVSWITCH_I2C_SPEED_MODE_300KHZ:
        {
            flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _300KHZ, flags);
            break;
        }
        case NVSWITCH_I2C_SPEED_MODE_200KHZ:
        {
            flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _200KHZ, flags);
            break;
        }
        case NVSWITCH_I2C_SPEED_MODE_100KHZ:
        {
            flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _100KHZ, flags);
            break;
        }
        default:
        {
            NVSWITCH_PRINT(device, ERROR, "Invalid I2C speed!\n");
            status = -NVL_BAD_ARGS;
            goto end;
        }
    }

    switch (type)
    {
        case NVSWITCH_I2C_CMD_READ:
            bIsRead = NV_TRUE;
            // Fall through
        case NVSWITCH_I2C_CMD_WRITE:
        {
            flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ZERO, flags);
            break;
        }
        case NVSWITCH_I2C_CMD_SMBUS_READ:
        {
            bIsRead = NV_TRUE;
            flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _RESTART, _SEND, flags);
            // Fall through
        }
        case NVSWITCH_I2C_CMD_SMBUS_WRITE:
        {
            flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ONE, flags);
            break;
        }
        case NVSWITCH_I2C_CMD_SMBUS_QUICK_READ:
            bIsRead = NV_TRUE;
            // Fall through
        case NVSWITCH_I2C_CMD_SMBUS_QUICK_WRITE:
        {
            flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ZERO, flags);
            msgLength = 0;
            break;
        }
        default:
        {
            NVSWITCH_PRINT(device, ERROR, "Invalid SMBUS protocol! Protocol not supported.\n");
            status = -NVL_BAD_ARGS;
            goto end;
        }
    }

    if (!is_i2c_access_allowed &&
        !nvswitch_i2c_is_device_access_allowed(device, port, deviceAddr, bIsRead))
    {
        return -NVL_BAD_ARGS;
    }

    if (msgLength > NVSWITCH_CTRL_I2C_MESSAGE_LENGTH_MAX)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Length of buffer (0x%x bytes) provided larger than max (0x%x bytes)\n",
             msgLength, NVSWITCH_CTRL_I2C_MESSAGE_LENGTH_MAX);
        status = -NVL_BAD_ARGS;
        goto end;
    }

    if (bIsRead)
    {
        i2c_params.bIsRead = NV_TRUE;
    }
    else
    {
        flags = FLD_SET_DRF(SWITCH_CTRL, _I2C_FLAGS, _RESTART, _NONE, flags);
        nvswitch_os_memcpy(i2c_params.message, pData, msgLength);
    }

    if (FLD_TEST_DRF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ONE, flags))
    {
        i2c_params.index[0] = cmd;
    }

    i2c_params.port     = port;
    i2c_params.address  = deviceAddr;
    i2c_params.acquirer = client;
    i2c_params.flags    = flags;
    i2c_params.messageLength = msgLength;

    status = nvswitch_ctrl_i2c_indexed(device, &i2c_params);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "I2C transfer Failed!\n");
        goto end;
    }

    if (bIsRead)
    {
        nvswitch_os_memcpy(pData, i2c_params.message, msgLength);
    }

end:
    return status;
}

/*!
 * @brief: Performs an I2C transaction.
 */
NvlStatus
nvswitch_lib_i2c_transfer
(
    nvswitch_device *device,
    NvU32 port,
    NvU8 type,
    NvU8 addr,
    NvU8 command,
    NvU32 len,
    NvU8 *pData
)
{
    NvlStatus status;
    NvU32 port_info;
    NvBool is_i2c_access_allowed;
    NvBool is_port_allowed;

    if (!NVSWITCH_IS_DEVICE_VALID(device))
    {
        NVSWITCH_ASSERT(0);
        return -NVL_ERR_INVALID_STATE;
    }

    port_info = nvswitch_i2c_get_port_info(device, port);

    is_i2c_access_allowed = (device->regkeys.i2c_access_control ==
                             NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL_ENABLE) ?
                                NV_TRUE : NV_FALSE;
    is_port_allowed = is_i2c_access_allowed ? NV_TRUE :
                          FLD_TEST_DRF(_I2C, _PORTINFO, _ACCESS_ALLOWED, _TRUE,
                                       port_info);

    if (!is_port_allowed ||
        !FLD_TEST_DRF(_I2C, _PORTINFO, _DEFINED, _PRESENT, port_info))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Invalid port access %d.\n",
            __FUNCTION__, port);
        return (-NVL_BAD_ARGS);
    }

    status = _nvswitch_perform_i2c_transfer(device, NVSWITCH_I2C_ACQUIRER_EXTERNAL,
                                            type, (NvU16)addr, port, command, len, pData);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "I2C transaction failed!\n");
        return status;
    }

    return NVL_SUCCESS;
}

void
nvswitch_timeout_create
(
    NvU64   timeout_ns,
    NVSWITCH_TIMEOUT *time
)
{
    NvU64   time_current;

    time_current = nvswitch_os_get_platform_time();
    time->timeout_ns = time_current + timeout_ns;
}

NvBool
nvswitch_timeout_check
(
    NVSWITCH_TIMEOUT *time
)
{
    NvU64   time_current;

    time_current = nvswitch_os_get_platform_time();
    return (time->timeout_ns <= time_current);
}

NvlStatus
nvswitch_task_create
(
    nvswitch_device *device,
    void (*task_fn)(nvswitch_device *device),
    NvU64 period_nsec,
    NvU32 flags
)
{
    NVSWITCH_TASK_TYPE *task;
    task = nvswitch_os_malloc(sizeof(NVSWITCH_TASK_TYPE));

    if (task == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unable to allocate task.\n",
            __FUNCTION__);
        return -NVL_NO_MEM;
    }
    else
    {
        task->task_fn_devptr = task_fn;
        task->task_args = NULL;
        task->period_nsec = period_nsec;
        task->last_run_nsec = nvswitch_os_get_platform_time(); // Prevent deferred tasks from being run immediately
        task->flags = flags;
        task->prev = NULL;
        task->next = device->tasks;
        if (device->tasks != NULL)
        {
            device->tasks->prev = task;
        }
        device->tasks = task;
    }

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_task_create_args
(
    nvswitch_device *device,
    void *fn_args,
    void (*task_fn)(nvswitch_device* device, void *fn_args),
    NvU64 period_nsec,
    NvU32 flags
)
{
    NVSWITCH_TASK_TYPE *task;
    task = nvswitch_os_malloc(sizeof(NVSWITCH_TASK_TYPE));

    flags = flags | NVSWITCH_TASK_TYPE_FLAGS_VOID_PTR_ARGS;  // ensure dispatcher always executes tasks passed through this function with args
    
    if (task == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unable to allocate task.\n",
            __FUNCTION__);
        return -NVL_NO_MEM;
    }
    else
    {
        task->task_fn_vdptr = task_fn;
        task->task_args = fn_args;
        task->period_nsec = period_nsec;
        task->last_run_nsec = nvswitch_os_get_platform_time(); // Prevent deferred tasks from being run immediately
        task->flags = flags;
        task->prev = NULL;
        task->next = device->tasks;
        if (device->tasks != NULL)
        {
            device->tasks->prev = task;
        }
        device->tasks = task;
    }

    return NVL_SUCCESS;
}

void
nvswitch_tasks_destroy
(
    nvswitch_device *device
)
{
    NVSWITCH_TASK_TYPE *task = device->tasks;
    NVSWITCH_TASK_TYPE *next_task;

    device->tasks = NULL;

    while (task)
    {
        next_task = task->next;
        nvswitch_os_free(task);
        task = next_task;
    }
}

void
nvswitch_destroy_device_state
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_destroy_device_state(device);
}

static NvlStatus
_nvswitch_ctrl_get_info
(
    nvswitch_device *device,
    NVSWITCH_GET_INFO *p
)
{
    return device->hal.nvswitch_ctrl_get_info(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_nvlink_status
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_STATUS_PARAMS *ret
)
{
    return device->hal.nvswitch_ctrl_get_nvlink_status(device, ret);
}

static NvlStatus
_nvswitch_ctrl_get_counters
(
    nvswitch_device *device,
    NVSWITCH_NVLINK_GET_COUNTERS_PARAMS *ret
)
{
    return device->hal.nvswitch_ctrl_get_counters(device, ret);
}

NvlStatus
nvswitch_set_nport_port_config
(
    nvswitch_device *device,
    NVSWITCH_SET_SWITCH_PORT_CONFIG *p
)
{
    return device->hal.nvswitch_set_nport_port_config(device, p);
}

static NvlStatus
_nvswitch_ctrl_set_switch_port_config
(
    nvswitch_device *device,
    NVSWITCH_SET_SWITCH_PORT_CONFIG *p
)
{
    return device->hal.nvswitch_ctrl_set_switch_port_config(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_ingress_request_table
(
    nvswitch_device *device,
    NVSWITCH_GET_INGRESS_REQUEST_TABLE_PARAMS *params
)
{
    return device->hal.nvswitch_ctrl_get_ingress_request_table(device, params);
}

static NvlStatus
_nvswitch_ctrl_set_ingress_request_table
(
    nvswitch_device *device,
    NVSWITCH_SET_INGRESS_REQUEST_TABLE *p
)
{
    return device->hal.nvswitch_ctrl_set_ingress_request_table(device, p);
}

static NvlStatus
_nvswitch_ctrl_set_ingress_request_valid
(
    nvswitch_device *device,
    NVSWITCH_SET_INGRESS_REQUEST_VALID *p
)
{
    return device->hal.nvswitch_ctrl_set_ingress_request_valid(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_ingress_response_table
(
    nvswitch_device *device,
    NVSWITCH_GET_INGRESS_RESPONSE_TABLE_PARAMS *params
)
{
    return device->hal.nvswitch_ctrl_get_ingress_response_table(device, params);
}

static NvlStatus
_nvswitch_ctrl_set_ingress_response_table
(
    nvswitch_device *device,
    NVSWITCH_SET_INGRESS_RESPONSE_TABLE *p
)
{
    return device->hal.nvswitch_ctrl_set_ingress_response_table(device, p);
}

static NvlStatus
_nvswitch_ctrl_set_ganged_link_table
(
    nvswitch_device *device,
    NVSWITCH_SET_GANGED_LINK_TABLE *p
)
{
    return device->hal.nvswitch_ctrl_set_ganged_link_table(device, p);
}

void
nvswitch_init_npg_multicast
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_init_npg_multicast(device);
}

void
nvswitch_init_warm_reset
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_init_warm_reset(device);
}

static NvlStatus
_nvswitch_ctrl_set_remap_policy
(
    nvswitch_device *device,
    NVSWITCH_SET_REMAP_POLICY *p
)
{
    return device->hal.nvswitch_ctrl_set_remap_policy(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_remap_policy
(
    nvswitch_device *device,
    NVSWITCH_GET_REMAP_POLICY_PARAMS *params
)
{
    return device->hal.nvswitch_ctrl_get_remap_policy(device, params);
}

static NvlStatus
_nvswitch_ctrl_set_remap_policy_valid
(
    nvswitch_device *device,
    NVSWITCH_SET_REMAP_POLICY_VALID *p
)
{
    return device->hal.nvswitch_ctrl_set_remap_policy_valid(device, p);
}

static NvlStatus
_nvswitch_ctrl_set_routing_id
(
    nvswitch_device *device,
    NVSWITCH_SET_ROUTING_ID *p
)
{
    return device->hal.nvswitch_ctrl_set_routing_id(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_routing_id
(
    nvswitch_device *device,
    NVSWITCH_GET_ROUTING_ID_PARAMS *params
)
{
    return device->hal.nvswitch_ctrl_get_routing_id(device, params);
}

static NvlStatus
_nvswitch_ctrl_set_routing_id_valid
(
    nvswitch_device *device,
    NVSWITCH_SET_ROUTING_ID_VALID *p
)
{
    return device->hal.nvswitch_ctrl_set_routing_id_valid(device, p);
}

static NvlStatus
_nvswitch_ctrl_set_routing_lan
(
    nvswitch_device *device,
    NVSWITCH_SET_ROUTING_LAN *p
)
{
    return device->hal.nvswitch_ctrl_set_routing_lan(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_routing_lan
(
    nvswitch_device *device,
    NVSWITCH_GET_ROUTING_LAN_PARAMS *params
)
{
    return device->hal.nvswitch_ctrl_get_routing_lan(device, params);
}

static NvlStatus
_nvswitch_ctrl_set_routing_lan_valid
(
    nvswitch_device *device,
    NVSWITCH_SET_ROUTING_LAN_VALID *p
)
{
    return device->hal.nvswitch_ctrl_set_routing_lan_valid(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_internal_latency
(
    nvswitch_device *device,
    NVSWITCH_GET_INTERNAL_LATENCY *p
)
{
    return device->hal.nvswitch_ctrl_get_internal_latency(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_nvlipt_counters
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLIPT_COUNTERS *p
)
{
    //
    // This control call is now deprecated.
    // New control call to fetch throughput counters is:
    // nvswitch_ctrl_get_throughput_counters
    //
    return -NVL_ERR_NOT_SUPPORTED;
}

static NvlStatus
_nvswitch_ctrl_set_nvlipt_counter_config
(
    nvswitch_device *device,
    NVSWITCH_SET_NVLIPT_COUNTER_CONFIG *p
)
{
    //
    // This control call is now deprecated.
    // New control call to fetch throughput counters is:
    // nvswitch_ctrl_get_throughput_counters_lr10
    //
    // Setting counter config is not allowed on these
    // non-configurable counters. These counters are
    // expected to be used by monitoring clients.
    //
    return -NVL_ERR_NOT_SUPPORTED;
}

static NvlStatus
_nvswitch_ctrl_get_nvlipt_counter_config
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLIPT_COUNTER_CONFIG *p
)
{
    //
    // This control call is now deprecated.
    // New control call to fetch throughput counters is:
    // nvswitch_ctrl_get_throughput_counters_lr10
    //
    // Getting counter config is useful if counters are
    // configurable. These counters are not configurable
    // and are expected to be used by monitoring clients.
    //
    return -NVL_ERR_NOT_SUPPORTED;
}

static NvlStatus
_nvswitch_ctrl_register_read
(
    nvswitch_device *device,
    NVSWITCH_REGISTER_READ *p
)
{
    return device->hal.nvswitch_ctrl_register_read(device, p);
}

static NvlStatus
_nvswitch_ctrl_register_write
(
    nvswitch_device *device,
    NVSWITCH_REGISTER_WRITE *p
)
{
    return device->hal.nvswitch_ctrl_register_write(device, p);
}

NvU32
nvswitch_i2c_get_port_info
(
    nvswitch_device *device,
    NvU32 port
)
{
    return device->hal.nvswitch_i2c_get_port_info(device, port);
}

NvlStatus
nvswitch_ctrl_i2c_indexed
(
    nvswitch_device *device,
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS *pParams
)
{
    return device->hal.nvswitch_ctrl_i2c_indexed(device, pParams);
}

static NvlStatus
_nvswitch_ctrl_therm_read_temperature
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_TEMPERATURE_PARAMS *info
)
{
    return device->hal.nvswitch_ctrl_therm_read_temperature(device, info);
}

static NvlStatus
_nvswitch_ctrl_get_bios_info
(
    nvswitch_device *device,
    NVSWITCH_GET_BIOS_INFO_PARAMS *p
)
{
    return device->hal.nvswitch_ctrl_get_bios_info(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_inforom_version
(
    nvswitch_device *device,
    NVSWITCH_GET_INFOROM_VERSION_PARAMS *p
)
{
    return device->hal.nvswitch_ctrl_get_inforom_version(device, p);
}

NvlStatus
nvswitch_ctrl_set_latency_bins
(
    nvswitch_device *device,
    NVSWITCH_SET_LATENCY_BINS *p
)
{
    return device->hal.nvswitch_ctrl_set_latency_bins(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_ingress_reqlinkid
(
    nvswitch_device *device,
    NVSWITCH_GET_INGRESS_REQLINKID_PARAMS *params
)
{
    return device->hal.nvswitch_ctrl_get_ingress_reqlinkid(device, params);
}

NvlStatus
nvswitch_ctrl_get_throughput_counters
(
    nvswitch_device *device,
    NVSWITCH_GET_THROUGHPUT_COUNTERS_PARAMS *p
)
{
    return device->hal.nvswitch_ctrl_get_throughput_counters(device, p);
}

static NvlStatus
_nvswitch_ctrl_unregister_link
(
    nvswitch_device *device,
    NVSWITCH_UNREGISTER_LINK_PARAMS *params
)
{
    nvlink_link *link = nvswitch_get_link(device, (NvU8)params->portNum);

    if (link == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    // With ALI in FW, links can be unregistered while Active
    if (!device->nvlink_device->enableALI)
    {

    if (device->hal.nvswitch_is_link_in_use(device, params->portNum))
    {
        return -NVL_ERR_STATE_IN_USE;
    }

    }

    nvlink_lib_unregister_link(link);
    nvswitch_destroy_link(link);

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_ctrl_acquire_capability
(
    nvswitch_device *device,
    NVSWITCH_ACQUIRE_CAPABILITY_PARAMS *params,
    void *osPrivate
)
{
    return nvswitch_os_acquire_fabric_mgmt_cap(osPrivate,
                                               params->capDescriptor);
}

static NvlStatus
_nvswitch_ctrl_reset_and_drain_links
(
    nvswitch_device *device,
    NVSWITCH_RESET_AND_DRAIN_LINKS_PARAMS *params
)
{
    return device->hal.nvswitch_reset_and_drain_links(device, params->linkMask, NV_FALSE);
}

static NvlStatus
_nvswitch_ctrl_get_fom_values
(
    nvswitch_device *device,
    NVSWITCH_GET_FOM_VALUES_PARAMS *ret
)
{
    return device->hal.nvswitch_ctrl_get_fom_values(device, ret);
}

static NvlStatus
_nvswitch_ctrl_get_nvlink_ecc_errors
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ECC_ERRORS_PARAMS *params
)
{
    return device->hal.nvswitch_get_nvlink_ecc_errors(device, params);
}

static NvlStatus
_nvswitch_ctrl_set_mc_rid_table
(
    nvswitch_device *device,
    NVSWITCH_SET_MC_RID_TABLE_PARAMS *p
)
{
    return device->hal.nvswitch_ctrl_set_mc_rid_table(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_mc_rid_table
(
    nvswitch_device *device,
    NVSWITCH_GET_MC_RID_TABLE_PARAMS *p
)
{
    return device->hal.nvswitch_ctrl_get_mc_rid_table(device, p);
}

static NvlStatus
_nvswitch_ctrl_set_residency_bins
(
    nvswitch_device *device,
    NVSWITCH_SET_RESIDENCY_BINS *p
)
{
    return device->hal.nvswitch_ctrl_set_residency_bins(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_residency_bins
(
    nvswitch_device *device,
    NVSWITCH_GET_RESIDENCY_BINS *p
)
{
    return device->hal.nvswitch_ctrl_get_residency_bins(device, p);
}

static NvlStatus
_nvswitch_ctrl_get_rb_stall_busy
(
    nvswitch_device *device,
    NVSWITCH_GET_RB_STALL_BUSY *p
)
{
    return device->hal.nvswitch_ctrl_get_rb_stall_busy(device, p);
}

NvlStatus
nvswitch_ctrl_get_multicast_id_error_vector
(
    nvswitch_device *device,
    NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR *p
)
{
    return device->hal.nvswitch_ctrl_get_multicast_id_error_vector(device, p);
}

NvlStatus
nvswitch_ctrl_clear_multicast_id_error_vector
(
    nvswitch_device *device,
    NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR *p
)
{
    return device->hal.nvswitch_ctrl_clear_multicast_id_error_vector(device, p);
}

static NvlStatus
_nvswitch_ctrl_inband_send_data
(
    nvswitch_device *device,
    NVSWITCH_INBAND_SEND_DATA_PARAMS *p
)
{
    return device->hal.nvswitch_ctrl_inband_send_data(device, p);
}

static NvlStatus
_nvswitch_ctrl_inband_read_data
(
    nvswitch_device *device,
    NVSWITCH_INBAND_READ_DATA_PARAMS *p
)
{
    return device->hal.nvswitch_ctrl_inband_read_data(device, p);
}

static NvlStatus
_nvswitch_ctrl_inband_flush_data
(
    nvswitch_device *device,
    NVSWITCH_INBAND_FLUSH_DATA_PARAMS *p
)
{
    NvU32 i;
    NvU64 enabledLinkMask;

    if (p->linkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "Nothing to clear\n");
        return NVL_SUCCESS;
    }

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);

    FOR_EACH_INDEX_IN_MASK(64, i, p->linkMask)
    {
         if (nvswitch_is_link_valid(device, i) &&
             (enabledLinkMask & NVBIT64(i)))
         {
              //
              // Flush is expected to clear both persistent and non-persistent
              // list. FM does flush when it wants to drop (ignore) all pending
              // messages w/o any NACKs.
              //
              _nvswitch_inband_clear_lists(device, i,
                                           NV_FALSE /* Nack */,
                                           NV_FALSE /* Non-persistent only */);
         }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_ctrl_inband_pending_data_stats
(
    nvswitch_device *device,
    NVSWITCH_INBAND_PENDING_DATA_STATS_PARAMS *p
)
{
    NvU32 link_num;
    NvU64 enabledLinkMask, persistent_mask = 0, nonpersistent_mask = 0;

    enabledLinkMask = nvswitch_get_enabled_link_mask(device);

    for (link_num = 0; link_num < nvswitch_get_num_links(device); link_num++)
    {
         if (nvswitch_is_link_valid(device, link_num) &&
             (enabledLinkMask & NVBIT64(link_num)))
         {
              if (!nvListIsEmpty(&device->link[link_num].inbandData.persistent_list))
              {
                  persistent_mask |= NVBIT64(link_num);
              }

              if (!nvListIsEmpty(&device->link[link_num].inbandData.nonpersistent_list))
              {
                  nonpersistent_mask |= NVBIT64(link_num);
              }
         }
    }

    if (persistent_mask > 0)
    {
        p->linkMask = persistent_mask;
    }
    else
    {
        p->linkMask = nonpersistent_mask;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_ctrl_get_board_part_number
(
    nvswitch_device *device,
    NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR *p
)
{
    if (IS_RTLSIM(device) || IS_EMULATION(device) || IS_FMODEL(device))
    {
        NVSWITCH_PRINT(device, INFO,
        "%s: Skipping retrieval of board part number on FSF\n",
            __FUNCTION__);

        nvswitch_os_memset(p, 0, sizeof(NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR));

       return NVL_SUCCESS;
    }
    else
    {
        if (!nvswitch_is_inforom_supported(device))
        {
            NVSWITCH_PRINT(device, ERROR, "InfoROM is not supported\n");
            return -NVL_ERR_NOT_SUPPORTED;
        }

        return device->hal.nvswitch_ctrl_get_board_part_number(device, p);
    }
}

static NvlStatus
_nvswitch_ctrl_i2c_smbus_command
(
    nvswitch_device *device,
    NVSWITCH_I2C_SMBUS_COMMAND_PARAMS *pParams
)
{
    NvU32 port_info;
    NvU32 port = pParams->port;
    NvU8 msgLen;
    NvU8 cmd;
    NvU16 addr;
    NvU8 cmdType;
    NvU8 *pData;
    NvBool is_i2c_access_allowed;
    NvBool is_port_allowed;

    port_info = nvswitch_i2c_get_port_info(device, port);

    is_i2c_access_allowed = (device->regkeys.i2c_access_control ==
                             NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL_ENABLE) ?
                                NV_TRUE : NV_FALSE;
    is_port_allowed = is_i2c_access_allowed ? NV_TRUE :
                          FLD_TEST_DRF(_I2C, _PORTINFO, _ACCESS_ALLOWED, _TRUE,
                                       port_info);

    if (!is_port_allowed ||
        !FLD_TEST_DRF(_I2C, _PORTINFO, _DEFINED, _PRESENT, port_info))
    {
        NVSWITCH_PRINT(device, ERROR, "Invalid port access %d.\n", port);
        return NVL_BAD_ARGS;
    }

    addr = pParams->deviceAddr;

    switch (pParams->cmdType)
    {
        case NVSWITCH_I2C_SMBUS_CMD_QUICK:
        {
            cmd = 0;
            msgLen = 0;
            cmdType = pParams->bRead ?
                          NVSWITCH_I2C_CMD_SMBUS_QUICK_READ :
                          NVSWITCH_I2C_CMD_SMBUS_QUICK_WRITE;
            pData = NULL;
            break;
        }
        case NVSWITCH_I2C_SMBUS_CMD_BYTE:
        {
            cmd = 0;
            msgLen = 1;
            cmdType = pParams->bRead ?
                          NVSWITCH_I2C_CMD_READ :
                          NVSWITCH_I2C_CMD_WRITE;
            pData = (NvU8 *)&pParams->transactionData.smbusByte.message;
            break;
        }
        case NVSWITCH_I2C_SMBUS_CMD_BYTE_DATA:
        {
            msgLen = 1;
            cmd = pParams->transactionData.smbusByteData.cmd;
            cmdType = pParams->bRead ?
                          NVSWITCH_I2C_CMD_SMBUS_READ :
                          NVSWITCH_I2C_CMD_SMBUS_WRITE;
            pData = (NvU8 *)&pParams->transactionData.smbusByteData.message;
            break;
        }
        case NVSWITCH_I2C_SMBUS_CMD_WORD_DATA:
        {
            msgLen = 2;
            cmd = pParams->transactionData.smbusWordData.cmd;
            cmdType = pParams->bRead ?
                          NVSWITCH_I2C_CMD_SMBUS_READ :
                          NVSWITCH_I2C_CMD_SMBUS_WRITE;
            pData = (NvU8 *)&pParams->transactionData.smbusWordData.message;
            break;
        }
        default:
        {
            NVSWITCH_PRINT(device, ERROR, "Invalid Smbus command: %d.\n", port);
            return NVL_BAD_ARGS;
        }
    }

    return _nvswitch_perform_i2c_transfer(device, NVSWITCH_I2C_ACQUIRER_IOCTL,
                                          cmdType, addr, port, cmd, msgLen, pData);
}

NvBool
nvswitch_does_link_need_termination_enabled
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    return device->hal.nvswitch_does_link_need_termination_enabled(device, link);
}

NvlStatus
nvswitch_link_termination_setup
(
    nvswitch_device *device,
    nvlink_link* link
)
{
    return device->hal.nvswitch_link_termination_setup(device, link);
}

static NvlStatus
_nvswitch_ctrl_cci_cmis_presence
(
    nvswitch_device *device,
    NVSWITCH_CCI_CMIS_PRESENCE_PARAMS *pParams
)
{
    nvswitch_os_memset(pParams, 0, sizeof(NVSWITCH_CCI_CMIS_PRESENCE_PARAMS));
    if (device->pCci != NULL)
    {
        (void)cciGetXcvrMask(device, &pParams->cagesMask, &pParams->modulesMask);
    }

    // IOCTL will always succeed
    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_ctrl_cci_nvlink_mappings
(
    nvswitch_device *device,
    NVSWITCH_CCI_CMIS_NVLINK_MAPPING_PARAMS *pParams
)
{
    if (device->pCci == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return cciGetCageMapping(device, pParams->cageIndex, &pParams->linkMask, &pParams->encodedValue);
}

static NvlStatus
_nvswitch_ctrl_cci_memory_access_read
(
    nvswitch_device *device,
    NVSWITCH_CCI_CMIS_MEMORY_ACCESS_READ_PARAMS *pParams
)
{
    NvlStatus retVal;

    if (device->pCci == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (!cciCmisAccessTryLock(device, pParams->cageIndex))
    {
        return -NVL_ERR_STATE_IN_USE;
    }

    retVal = cciCmisRead(device, pParams->cageIndex, pParams->bank,
                       pParams->page, pParams->address, pParams->count,
                       pParams->data);

    if (!pParams->bSequenceLock)
    {
        cciCmisAccessReleaseLock(device, pParams->cageIndex);
    }

    return retVal;
}

static NvlStatus
_nvswitch_ctrl_cci_memory_access_write
(
    nvswitch_device *device,
    NVSWITCH_CCI_CMIS_MEMORY_ACCESS_WRITE_PARAMS *pParams
)
{
    NvlStatus retVal;

    if (device->pCci == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (!cciCmisAccessTryLock(device, pParams->cageIndex))
    {
        return -NVL_ERR_STATE_IN_USE;
    }

    retVal = cciCmisWrite(device, pParams->cageIndex, pParams->bank,
                        pParams->page, pParams->address, pParams->count,
                        pParams->data);

    if (!pParams->bSequenceLock)
    {
        cciCmisAccessReleaseLock(device, pParams->cageIndex);
    }

    return retVal;
}

static NvlStatus
_nvswitch_ctrl_cci_cage_bezel_marking
(
    nvswitch_device *device,
    NVSWITCH_CCI_CMIS_CAGE_BEZEL_MARKING_PARAMS *pParams
)
{
    if (device->pCci == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return cciCmisCageBezelMarking(device, pParams->cageIndex, pParams->bezelMarking);
}

static NvlStatus
_nvswitch_ctrl_get_soe_heartbeat
(
    nvswitch_device *device,
    NVSWITCH_GET_SOE_HEARTBEAT_PARAMS *pParams
)
{
    return device->hal.nvswitch_ctrl_get_soe_heartbeat(device, pParams);
}

static NvlStatus
_nvswitch_ctrl_set_continuous_ali
(
    nvswitch_device *device,
    NVSWITCH_SET_CONTINUOUS_ALI_PARAMS *pParams
)
{
    device->bModeContinuousALI = pParams->bEnable;
    NVSWITCH_PRINT(device, INFO,
            "%s: Continuous ALI 0x%x\n",
            __FUNCTION__, device->bModeContinuousALI);
    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_ctrl_request_ali
(
    nvswitch_device *device,
    NVSWITCH_REQUEST_ALI_PARAMS *pParams
)
{
    nvlink_link *link;
    NvU64 linkStateTl;
    NvU64 enabledLinkMask;
    NvU8 linkId;  

    if (device->bModeContinuousALI)
    {
        return -NVL_ERR_INVALID_STATE;
    }

    // Only process enabled links
    enabledLinkMask = nvswitch_get_enabled_link_mask(device);
    pParams->linkMaskTrain &= enabledLinkMask;

    NVSWITCH_PRINT(device, INFO,
            "%s: ALI requested for links 0x%llx\n",
            __FUNCTION__, pParams->linkMaskTrain);

    // Handle access links
    FOR_EACH_INDEX_IN_MASK(64, linkId, pParams->linkMaskTrain)
    {
        // Only directly launch ALI on non-CCI managed links
        if (cciIsLinkManaged(device, linkId))
        {
            continue;
        }

        link = nvswitch_get_link(device, linkId);
        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID(device, linkId, NVLIPT_LNK) ||
            (linkId >= NVSWITCH_NVLINK_MAX_LINKS) ||
            (device->hal.nvswitch_corelib_get_tl_link_mode(link, &linkStateTl) != NVL_SUCCESS))
        {
            continue;
        }

        if (linkStateTl == NVLINK_LINKSTATE_ACTIVE_PENDING)
        {
            continue;
        }

        // Forcibly reset and re-train access links
        device->hal.nvswitch_reset_and_drain_links(device, 
            NVBIT64(linkId), NV_TRUE);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Ask CCI to handle trunk links
    nvswitch_ctrl_cci_request_ali(device, pParams);

    return NVL_SUCCESS;
}

static NvlStatus
_nvswitch_ctrl_get_inforom_nvlink_max_correctable_error_rate
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS *params
)
{
    return nvswitch_inforom_nvlink_get_max_correctable_error_rate(device, params);
}

static NvlStatus
_nvswitch_ctrl_get_inforom_nvlink_errors
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS *params
)
{
    return nvswitch_inforom_nvlink_get_errors(device, params);
}

static NvlStatus
_nvswitch_ctrl_get_inforom_ecc_errors
(
    nvswitch_device *device,
    NVSWITCH_GET_ECC_ERROR_COUNTS_PARAMS *params
)
{
    return nvswitch_inforom_ecc_get_errors(device, params);
}

static NvlStatus
_nvswitch_ctrl_get_inforom_bbx_sxid
(
    nvswitch_device *device,
    NVSWITCH_GET_SXIDS_PARAMS *params
)
{
    return nvswitch_inforom_bbx_get_sxid(device, params);
}

static NvlStatus
_nvswitch_ctrl_get_inforom_bbx_sys_info
(
    nvswitch_device *device,
    NVSWITCH_GET_SYS_INFO_PARAMS *params
)
{
    return nvswitch_inforom_bbx_get_data(device, RM_SOE_IFR_BBX_GET_SYS_INFO, (void *)params);
}

static NvlStatus
_nvswitch_ctrl_get_inforom_bbx_time_info
(
    nvswitch_device *device,
    NVSWITCH_GET_TIME_INFO_PARAMS *params
)
{
    return nvswitch_inforom_bbx_get_data(device, RM_SOE_IFR_BBX_GET_TIME_INFO, (void *)params);
}

static NvlStatus
_nvswitch_ctrl_get_inforom_bbx_temp_data
(
    nvswitch_device *device,
    NVSWITCH_GET_TEMP_DATA_PARAMS *params
)
{
    return nvswitch_inforom_bbx_get_data(device, RM_SOE_IFR_BBX_GET_TEMP_DATA, (void *)params);
}

static NvlStatus
_nvswitch_ctrl_get_inforom_bbx_temp_samples
(
    nvswitch_device *device,
    NVSWITCH_GET_TEMP_SAMPLES_PARAMS *params
)
{
    return nvswitch_inforom_bbx_get_data(device, RM_SOE_IFR_BBX_GET_TEMP_SAMPLES, (void *)params);
}

static NvlStatus
_nvswitch_ctrl_get_nvlink_lp_counters
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_LP_COUNTERS_PARAMS *params
)
{
    return device->hal.nvswitch_ctrl_get_nvlink_lp_counters(device, params);
}

static NvlStatus
_nvswitch_ctrl_get_sw_info
(
    nvswitch_device *device,
    NVSWITCH_GET_SW_INFO_PARAMS *params
)
{
    return device->hal.nvswitch_ctrl_get_sw_info(device, params);
}

static NvlStatus
_nvswitch_lib_validate_privileged_ctrl
(
    void *osPrivate,
    NvU64 flags
)
{
    if (flags & NVSWITCH_DEV_CMD_CHECK_ADMIN)
    {
        if (nvswitch_os_is_admin())
        {
            return NVL_SUCCESS;
        }
    }

    if (flags & NVSWITCH_DEV_CMD_CHECK_FM)
    {
        if (nvswitch_os_is_fabric_manager(osPrivate))
        {
            return NVL_SUCCESS;
        }
    }

    return -NVL_ERR_INSUFFICIENT_PERMISSIONS;
}

/*
 * @Brief : Copy the data from the persistant or nonpersistant list
 *
 * @Description :
 *
 * @param[in] device            NvSwitch device to contain this link
 * @param[out] data             Destination Data
 * @param[in] linkId            link number of the link
 * @param[out] dataSize         Size of data copied
 *
 * @returns                     NVL_SUCCESS if action succeeded,
 *                              -NVL_NOT_FOUND if link doesnt have data
 */
NvlStatus
nvswitch_inband_read_data
(
    nvswitch_device *device,
    NvU8 *dest,
    NvU32 linkId,
    NvU32 *dataSize
)
{
    nvswitch_inband_data_list *curr = NULL;
    NVListRec *list;

    if (nvListIsEmpty(&device->link[linkId].inbandData.persistent_list) &&
        nvListIsEmpty(&device->link[linkId].inbandData.nonpersistent_list))
    {
        NVSWITCH_PRINT(device, ERROR, "%s: LinkId %d doesnt have any data to send\n",
                         __FUNCTION__, linkId);
        *dataSize = 0;
        return -NVL_NOT_FOUND;
    }

    list = nvListIsEmpty(&device->link[linkId].inbandData.persistent_list) ?
                         &device->link[linkId].inbandData.nonpersistent_list :
                         &device->link[linkId].inbandData.persistent_list;

    nvListForEachEntry(curr, list, entry)
    {
         *dataSize = curr->dataSize;
         nvswitch_os_memcpy(dest, curr->data, curr->dataSize);
         nvListDel(&curr->entry);
         nvswitch_os_free(curr);
         break;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Returns NV_TRUE if the given inband msg
 *          needs to go to persistant list
 *
 * @Description :
 *
 * @param[in] device            NvSwitch device to contain this link
 * @param[in] msghdr            Header to the message
 *
 */

static NvBool
nvswitch_is_message_persistent
(
    nvswitch_device *device,
    nvlink_inband_msg_header_t *msghdr
)
{
    // We expect only one message per received data 
    switch(msghdr->type)
    {
        case NVLINK_INBAND_MSG_TYPE_MC_TEAM_RELEASE_REQ:
            return NV_TRUE;
        default:
            return NV_FALSE;
    }
}

/*
 * @Brief : Moves the data into persistant or nonpersistant list
 *
 * @Description :
 *
 * @param[in] device            NvSwitch device to contain this link
 * @param[in] linkId            link number of the link
 *
 */
void
nvswitch_filter_messages
(
    nvswitch_device *device,
    NvU32           linkId
)
{
    NvlStatus status;
    nvlink_inband_msg_header_t *msghdr = NULL;
    nvswitch_inband_data_list *msg = device->link[linkId].inbandData.message;
    NvU8   *buffer = device->link[linkId].inbandData.message->data;
    NVSWITCH_DRIVER_FABRIC_STATE driver_fabric_state = 0;
    NvBool bSendNackOrDrop = NV_FALSE;

    NVSWITCH_ASSERT(nvswitch_lib_read_fabric_state(device, NULL, NULL,
                                        &driver_fabric_state) == NVL_SUCCESS);

    msghdr = (nvlink_inband_msg_header_t*)buffer;

    if (nvswitch_is_message_persistent(device, msghdr))
    {
        if (nvListCount(&device->link[linkId].inbandData.persistent_list) <
            device->hal.nvswitch_get_max_persistent_message_count(device))
        {
            nvListAdd(&msg->entry, &device->link[linkId].inbandData.persistent_list);
        }
        else
        {
            bSendNackOrDrop = NV_TRUE;
        }
    }
    else
    {
         if (driver_fabric_state == NVSWITCH_DRIVER_FABRIC_STATE_CONFIGURED)
         {
              nvListAdd(&msg->entry,
                        &device->link[linkId].inbandData.nonpersistent_list);
         }
         else
         {
              bSendNackOrDrop = NV_TRUE;
         }
    }

    if (bSendNackOrDrop)
    {
        nvswitch_send_nack_or_drop(device, linkId, msghdr);
        nvswitch_os_free(msg);
    }
    else
    {
        status = nvswitch_lib_notify_client_events(device,
                                                   NVSWITCH_DEVICE_EVENT_INBAND_DATA);
        if (status != NVL_SUCCESS)
        {
              NVSWITCH_PRINT(device, ERROR, "%s: Failed to notify INBAND_DATA event\n",
                             __FUNCTION__);
        }
    }

    device->link[linkId].inbandData.message = NULL;
}

/*
 * @Brief : Constructs an NVS link struct with the given data
 *
 * @Description :
 *
 * @param[in] device            NvSwitch device to contain this link
 * @param[in] link_num          link number of the link
 * @param[out] link             reference to store the created link into
 *
 * @returns                     NVL_SUCCESS if action succeeded,
 *                              -NVL_NO_MEM if memory allocation failed
 */
NvlStatus
nvswitch_create_link
(
    nvswitch_device *device,
    NvU32 link_number,
    nvlink_link **link
)
{
    NvlStatus   retval      = NVL_SUCCESS;
    nvlink_link *ret        = NULL;
    LINK_INFO   *link_info  = NULL;
    NvU64       ac_coupled_mask;

    NVSWITCH_ASSERT(nvswitch_get_num_links(device) <=  NVSWITCH_MAX_NUM_LINKS);

    ret = nvswitch_os_malloc(sizeof(*ret));
    if (NULL == ret)
    {
        NVSWITCH_PRINT(device, ERROR,
            "nvswitch_os_malloc during link creation failed!\n");
        retval = -NVL_NO_MEM;
        goto nvswitch_create_link_cleanup;
    }
    nvswitch_os_memset(ret, 0, sizeof(*ret));

    link_info = nvswitch_os_malloc(sizeof(*link_info));
    if (NULL == link_info)
    {
        NVSWITCH_PRINT(device, ERROR,
            "nvswitch_os_malloc during link creation failed!\n");
        retval = -NVL_NO_MEM;
        goto nvswitch_create_link_cleanup;
    }
    nvswitch_os_memset(link_info, 0, sizeof(*link_info));
    nvswitch_os_snprintf(link_info->name, sizeof(link_info->name), NVSWITCH_LINK_NAME "%d", link_number);

    ret->dev        = device->nvlink_device;
    ret->linkName   = link_info->name;
    ret->linkNumber = link_number;
    ret->state      = NVLINK_LINKSTATE_OFF;
    ret->ac_coupled = NV_FALSE;
    ret->version    = nvswitch_get_link_ip_version(device, link_number);

    ac_coupled_mask = ((NvU64)device->regkeys.ac_coupled_mask2 << 32 |
                       (NvU64)device->regkeys.ac_coupled_mask);

    if (ac_coupled_mask)
    {
        if (ac_coupled_mask & NVBIT64(link_number))
        {
            ret->ac_coupled = NV_TRUE;
        }
    }
    else if (device->firmware.nvlink.link_config_found)
    {
        if (device->firmware.nvlink.link_ac_coupled_mask & NVBIT64(link_number))
        {
            ret->ac_coupled = NV_TRUE;
        }
    }

    // Initialize NVLink corelib callbacks for switch
    nvswitch_get_link_handlers(&link_handlers);

    ret->link_handlers = &link_handlers;

    //
    // link_info is used to store private link information
    //

    ret->link_info = link_info;

    *link = ret;

    return retval;

nvswitch_create_link_cleanup:
    if (NULL != ret)
    {
        nvswitch_os_free(ret);
    }
    if (NULL != link_info)
    {
        nvswitch_os_free(link_info);
    }

    return retval;
}

void
nvswitch_destroy_link
(
    nvlink_link *link
)
{
    if (NULL != link->link_info)
    {
        nvswitch_os_free(link->link_info);
    }

    nvswitch_os_free(link);
}

NvU32
nvswitch_get_num_links
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_get_num_links(device);
}

NvBool
nvswitch_is_link_valid
(
    nvswitch_device *device,
    NvU32            link_id
)
{
    return device->hal.nvswitch_is_link_valid(device, link_id);
}

nvlink_link*
nvswitch_get_link(nvswitch_device *device, NvU8 link_id)
{
    nvlink_link *link = NULL;

    nvlink_lib_get_link(device->nvlink_device, link_id, &link);

    return link;
}

NvU64
nvswitch_get_enabled_link_mask
(
    nvswitch_device *device
)
{
    NvU64                    ret;
    nvlink_link             *link;
    NvU32 link_num;

    ret = 0x0;

    for (link_num = 0; link_num < nvswitch_get_num_links(device); link_num++)
    {
        if (nvlink_lib_get_link(device->nvlink_device, link_num, &link) == NVL_SUCCESS)
        {
            ret |= NVBIT64(link_num);
        }
    }

    return ret;
}

void
nvswitch_set_fatal_error
(
    nvswitch_device *device,
    NvBool           device_fatal,
    NvU32            link_id
)
{
    device->hal.nvswitch_set_fatal_error(device, device_fatal, link_id);
}

NvU32
nvswitch_get_swap_clk_default
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_get_swap_clk_default(device);
}

NvU32
nvswitch_get_latency_sample_interval_msec
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_get_latency_sample_interval_msec(device);
}

void
nvswitch_internal_latency_bin_log
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_internal_latency_bin_log(device);
}

void
nvswitch_ecc_writeback_task
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_ecc_writeback_task(device);
}

void
nvswitch_monitor_thermal_alert
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_monitor_thermal_alert(device);
}

void
nvswitch_hw_counter_shutdown
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_hw_counter_shutdown(device);
}

NvlStatus
nvswitch_get_rom_info
(
    nvswitch_device *device,
    NVSWITCH_EEPROM_TYPE *eeprom
)
{
    return device->hal.nvswitch_get_rom_info(device, eeprom);
}

void
nvswitch_lib_enable_interrupts
(
    nvswitch_device *device
)
{
    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    device->hal.nvswitch_lib_enable_interrupts(device);
}

void
nvswitch_lib_disable_interrupts
(
    nvswitch_device *device
)
{
    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    device->hal.nvswitch_lib_disable_interrupts(device);
}

NvlStatus
nvswitch_lib_check_interrupts
(
    nvswitch_device *device
)
{
    if (!NVSWITCH_IS_DEVICE_INITIALIZED(device))
    {
        return -NVL_BAD_ARGS;
    }

    return device->hal.nvswitch_lib_check_interrupts(device);
}

NvlStatus
nvswitch_lib_service_interrupts
(
    nvswitch_device *device
)
{
    if (!NVSWITCH_IS_DEVICE_INITIALIZED(device))
    {
        return -NVL_BAD_ARGS;
    }

    return device->hal.nvswitch_lib_service_interrupts(device);
}

NvU64
nvswitch_hw_counter_read_counter
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_hw_counter_read_counter(device);
}

NvU32
nvswitch_get_link_ip_version
(
    nvswitch_device *device,
    NvU32            link_id
)
{
    return device->hal.nvswitch_get_link_ip_version(device, link_id);
}

NvU32
nvswitch_reg_read_32
(
    nvswitch_device *device,
    NvU32 offset
)
{
    NvU32 val;

    if (device->nvlink_device->pciInfo.bars[0].pBar == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "register read failed at offset 0x%x\n", offset);

        return 0xFFFFFFFF;
    }

    val = nvswitch_os_mem_read32((NvU8 *)device->nvlink_device->pciInfo.bars[0].pBar + offset);

    if ((val & 0xFFFF0000) == 0xBADF0000)
    {
        NvU32 boot_0;
        NVSWITCH_PRINT(device, WARN,
            "Potential IO failure reading 0x%x (0x%x)\n", offset, val);
        boot_0 = nvswitch_os_mem_read32((NvU8 *)device->nvlink_device->pciInfo.bars[0].pBar + 0x0);

        if ((boot_0 & 0xFFFF0000) == 0xBADF0000)
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_HOST_IO_FAILURE,
                "IO failure\n");
            NVSWITCH_PRINT(device, ERROR,
                "IO failure reading 0x%x (0x%x)\n", offset, val);
        }
    }

#ifdef _VERBOSE_REG_ACCESS
    NVSWITCH_PRINT(device, SETUP,
        "NVSWITCH read 0x%6x+%6x = 0x%08x\n",
        device->nvlink_device->pciInfo.bars[0].baseAddr, offset, val);
#endif

    return val;
}

void
nvswitch_reg_write_32
(
    nvswitch_device *device,
    NvU32 offset,
    NvU32 data
)
{
    if (device->nvlink_device->pciInfo.bars[0].pBar == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "register write failed at offset 0x%x\n", offset);

        return;
    }

#ifdef _VERBOSE_REG_ACCESS
    NVSWITCH_PRINT(device, SETUP,
        "NVSWITCH write 0x%6x+%6x = 0x%08x\n",
        device->nvlink_device->pciInfo.bars[0].baseAddr, offset, data);
#endif

    // Write the register
    nvswitch_os_mem_write32((NvU8 *)device->nvlink_device->pciInfo.bars[0].pBar + offset, data);

    return;
}

NvU64
nvswitch_read_64bit_counter
(
    nvswitch_device *device,
    NvU32 lo_offset,
    NvU32 hi_offset
)
{
    NvU32   hi0;
    NvU32   hi1;
    NvU32   lo;

    hi0 = nvswitch_reg_read_32(device, hi_offset);
    do
    {
        hi1 = hi0;
        lo  = nvswitch_reg_read_32(device, lo_offset);
        hi0 = nvswitch_reg_read_32(device, hi_offset);
    } while (hi0 != hi1);

    return (lo | ((NvU64)hi0 << 32));
}

NvlStatus
nvswitch_validate_pll_config
(
    nvswitch_device *device,
    NVSWITCH_PLL_INFO *switch_pll,
    NVSWITCH_PLL_LIMITS default_pll_limits
)
{
    NvU32 update_rate_khz;
    NvU32 vco_freq_khz;
    NVSWITCH_PLL_LIMITS pll_limits;

    NVSWITCH_PRINT(device, SETUP,
        "%s: Validating PLL: %dkHz * %d / (%d * %d * (1 << %d))\n",
        __FUNCTION__,
        switch_pll->src_freq_khz,
        switch_pll->N,
        switch_pll->M,
        switch_pll->PL,
        switch_pll->dist_mode);

    //
    // These parameters could come from schmoo'ing API, settings file or a ROM.
    // For now, hard code with POR.
    //
    if (device->firmware.firmware_size > 0 &&
        device->firmware.clocks.clocks_found &&
        device->firmware.clocks.sys_pll.valid)
    {
        pll_limits = device->firmware.clocks.sys_pll;
    }
    else
    {
        pll_limits = default_pll_limits;
    }

    NVSWITCH_ASSERT(switch_pll->M != 0);
    NVSWITCH_ASSERT(switch_pll->PL != 0);

    if ((switch_pll->src_freq_khz < pll_limits.ref_min_mhz * 1000) ||
        (switch_pll->src_freq_khz > pll_limits.ref_max_mhz * 1000))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ERROR: Ref(%d) out-of-range\n",
            __FUNCTION__,
            switch_pll->src_freq_khz);
        return -NVL_ERR_INVALID_STATE;
    }

    if ((switch_pll->M < pll_limits.m_min) ||
        (switch_pll->M > pll_limits.m_max))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ERROR: M(%d) out-of-range\n",
            __FUNCTION__,
            switch_pll->M);
        return -NVL_ERR_INVALID_STATE;
    }

    if ((switch_pll->N < pll_limits.n_min) ||
        (switch_pll->N > pll_limits.n_max))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ERROR: N(%d) out-of-range\n",
            __FUNCTION__,
            switch_pll->N);
        return -NVL_ERR_INVALID_STATE;
    }

    if ((switch_pll->PL < pll_limits.pl_min) ||
        (switch_pll->PL > pll_limits.pl_max))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ERROR: PL(%d) out-of-range\n",
            __FUNCTION__,
            switch_pll->PL);
        return -NVL_ERR_INVALID_STATE;
    }

    vco_freq_khz = switch_pll->src_freq_khz * switch_pll->N
        / switch_pll->M;
    if ((vco_freq_khz < pll_limits.vco_min_mhz * 1000) ||
        (vco_freq_khz > pll_limits.vco_max_mhz * 1000))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ERROR: VCO(%d) freq out-of-range\n",
            __FUNCTION__,
            vco_freq_khz);
        return -NVL_ERR_INVALID_STATE;
    }

    update_rate_khz = switch_pll->src_freq_khz / switch_pll->M;
    if ((update_rate_khz < pll_limits.update_min_mhz * 1000) ||
        (update_rate_khz > pll_limits.update_max_mhz * 1000))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: ERROR: update rate(%d) out-of-range\n",
            __FUNCTION__,
            update_rate_khz);
        return -NVL_ERR_INVALID_STATE;
    }

    switch_pll->vco_freq_khz = vco_freq_khz;

    switch_pll->freq_khz =
        switch_pll->src_freq_khz * switch_pll->N /
        (switch_pll->M * switch_pll->PL * (1 << switch_pll->dist_mode));

    NVSWITCH_PRINT(device, SETUP,
        "%s: Validated PLL: %dkHz * %d / (%d * %d * (1 << %d)) = %dkHz\n",
        __FUNCTION__,
        switch_pll->src_freq_khz,
        switch_pll->N,
        switch_pll->M,
        switch_pll->PL,
        switch_pll->dist_mode,
        switch_pll->freq_khz);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_init_pll_config
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_init_pll_config(device);
}

NvlStatus
nvswitch_init_pll
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_init_pll(device);
}

void
nvswitch_init_clock_gating
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_init_clock_gating(device);
}

void
nvswitch_lib_get_uuid
(
    nvswitch_device *device,
    NvUuid *uuid
)
{
    if (!NVSWITCH_IS_DEVICE_INITIALIZED(device) || (uuid == NULL))
    {
        return;
    }

    nvswitch_os_memcpy(uuid, &device->uuid, sizeof(device->uuid));
}

NvlStatus
nvswitch_lib_get_physid
(
    nvswitch_device *device,
    NvU32 *phys_id
)
{
    NVSWITCH_GET_INFO get_info;
    NvlStatus ret;

    if (phys_id == NULL || !NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    get_info.count=1;
    get_info.index[0] = NVSWITCH_GET_INFO_INDEX_PHYSICAL_ID;

    ret = _nvswitch_ctrl_get_info(device, &get_info);
    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "Failed to get physical ID\n");
        return ret;
    }

    *phys_id = get_info.info[0];

    return NVL_SUCCESS;
}

void
nvswitch_i2c_set_hw_speed_mode
(
    nvswitch_device *device,
    NvU32 port,
    NvU32 speedMode
)
{
    device->hal.nvswitch_i2c_set_hw_speed_mode(device, port, speedMode);
    return;
}

void
nvswitch_lib_smbpbi_log_sxid
(
    nvswitch_device *device,
    NvU32           sxid,
    const char      *pFormat,
    ...
)
{
    va_list arglist;
    int     msglen;
    char    string[RM_SOE_SMBPBI_CMD_LOG_MESSAGE_MAX_STRING + 1];

    nvswitch_os_memset(string, 0, (NvLength)sizeof(string));

    va_start(arglist, pFormat);
    msglen = nvswitch_os_vsnprintf(string, sizeof(string), pFormat, arglist);
    va_end(arglist);

    if (!(msglen < 0))
    {
        //
        // HALs will know that the string is being truncated by seeing that the
        // last byte in the buffer is not nul.
        //
        msglen = NV_MIN(msglen + 1, (int)RM_SOE_SMBPBI_CMD_LOG_MESSAGE_MAX_STRING);
        device->hal.nvswitch_smbpbi_log_message(device, sxid, msglen, (NvU8 *) string);
    }
}

NvlStatus
nvswitch_set_minion_initialized
(
    nvswitch_device *device,
    NvU32 idx_minion,
    NvBool initialized
)
{
    return device->hal.nvswitch_set_minion_initialized(device, idx_minion, initialized);
}

NvBool
nvswitch_is_minion_initialized
(
    nvswitch_device *device,
    NvU32 idx_minion
)
{
    return device->hal.nvswitch_is_minion_initialized(device, idx_minion);
}

NvlStatus
nvswitch_device_discovery
(
    nvswitch_device *device,
    NvU32 discovery_offset
)
{
    return device->hal.nvswitch_device_discovery(device, discovery_offset);
}

void
nvswitch_filter_discovery
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_filter_discovery(device);
}

NvlStatus
nvswitch_process_discovery
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_process_discovery(device);
}

NvlStatus
nvswitch_init_minion
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_init_minion(device);
}

NvU32
nvswitch_get_link_eng_inst
(
    nvswitch_device *device,
    NvU32 link_id,
    NVSWITCH_ENGINE_ID eng_id
)
{
    return device->hal.nvswitch_get_link_eng_inst(device, link_id, eng_id);
}

void *
nvswitch_alloc_chipdevice
(
    nvswitch_device *device
)
{
    return(device->hal.nvswitch_alloc_chipdevice(device));
}

void
nvswitch_free_chipdevice
(
    nvswitch_device *device
)
{
    if (device->chip_device)
    {
        nvswitch_os_free(device->chip_device);
        device->chip_device = NULL;
    }
}

NvlStatus
nvswitch_init_thermal
(
    nvswitch_device *device
)
{
    return(device->hal.nvswitch_init_thermal(device));
}

NvU32
nvswitch_read_physical_id
(
    nvswitch_device *device
)
{
    return(device->hal.nvswitch_read_physical_id(device));
}

NvU32
nvswitch_get_caps_nvlink_version
(
    nvswitch_device *device
)
{
    return(device->hal.nvswitch_get_caps_nvlink_version(device));
}

void
nvswitch_initialize_interrupt_tree
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_initialize_interrupt_tree(device);
}

void
nvswitch_init_dlpl_interrupts
(
    nvlink_link *link
)
{
    nvswitch_device *device = link->dev->pDevInfo;

    device->hal.nvswitch_init_dlpl_interrupts(link);
}

NvlStatus
nvswitch_initialize_pmgr
(
    nvswitch_device *device
)
{
    return(device->hal.nvswitch_initialize_pmgr(device));
}

NvlStatus
nvswitch_initialize_ip_wrappers
(
    nvswitch_device *device
)
{
    return(device->hal.nvswitch_initialize_ip_wrappers(device));
}

NvlStatus
nvswitch_initialize_route
(
    nvswitch_device *device
)
{
    return(device->hal.nvswitch_initialize_route(device));
}

void
nvswitch_soe_unregister_events
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_soe_unregister_events(device);
}

NvlStatus
nvswitch_soe_register_event_callbacks
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_soe_register_event_callbacks(device);
}

NVSWITCH_BIOS_NVLINK_CONFIG *
nvswitch_get_bios_nvlink_config
(
    nvswitch_device *device
)
{
    return(device->hal.nvswitch_get_bios_nvlink_config(device));
}

NvlStatus
nvswitch_minion_send_command
(
    nvswitch_device *device,
    NvU32            linkNumber,
    NvU32            command,
    NvU32            scratch0
)
{
    return(device->hal.nvswitch_minion_send_command(device, linkNumber,
                                                    command, scratch0));
}

NvlStatus
nvswitch_init_nport
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_init_nport(device);
}

NvlStatus
nvswitch_init_nxbar
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_init_nxbar(device);
}

NvlStatus
nvswitch_clear_nport_rams
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_clear_nport_rams(device);
}

NvlStatus
nvswitch_pri_ring_init
(
    nvswitch_device *device
)
{
    return(device->hal.nvswitch_pri_ring_init(device));
}

NvlStatus
nvswitch_get_remap_table_selector
(
    nvswitch_device *device,
    NVSWITCH_TABLE_SELECT_REMAP table_selector,
    NvU32 *remap_ram_sel
)
{
    return device->hal.nvswitch_get_remap_table_selector(device, table_selector, remap_ram_sel);
}

NvU32
nvswitch_get_ingress_ram_size
(
    nvswitch_device *device,
    NvU32 ingress_ram_selector      // NV_INGRESS_REQRSPMAPADDR_RAM_ADDRESS_*
)
{
    return device->hal.nvswitch_get_ingress_ram_size(device, ingress_ram_selector);
}

NvlStatus
nvswitch_minion_get_dl_status
(
    nvswitch_device *device,
    NvU32            linkId,
    NvU32            statusIdx,
    NvU32            statusArgs,
    NvU32           *statusData
)
{
    return device->hal.nvswitch_minion_get_dl_status(device, linkId, statusIdx, statusArgs, statusData);
}

NvBool
nvswitch_is_i2c_supported
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_i2c_supported(device);
}


NvlStatus
nvswitch_poll_sublink_state
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    return device->hal.nvswitch_poll_sublink_state(device, link);
}

void
nvswitch_setup_link_loopback_mode
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    device->hal.nvswitch_setup_link_loopback_mode(device, linkNumber);
}

void
nvswitch_reset_persistent_link_hw_state
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    device->hal.nvswitch_reset_persistent_link_hw_state(device, linkNumber);
}

void
nvswitch_store_topology_information
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    device->hal.nvswitch_store_topology_information(device, link);
}

void
nvswitch_init_lpwr_regs
(
    nvlink_link *link
)
{
   nvswitch_device *device = link->dev->pDevInfo;
   device->hal.nvswitch_init_lpwr_regs(link);
}

void
nvswitch_program_l1_scratch_reg
(
    nvswitch_device *device,
    NvU32 linkNumber
)
{
   device->hal.nvswitch_program_l1_scratch_reg(device, linkNumber);
}

NvlStatus
nvswitch_check_io_sanity
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_check_io_sanity(device);
}

NvlStatus
nvswitch_launch_ALI
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_launch_ALI(device);
}

NvlStatus
nvswitch_set_training_mode
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_set_training_mode(device);
}

NvBool
nvswitch_is_link_in_reset
(
    nvswitch_device *device,
    nvlink_link     *link
)
{
    return device->hal.nvswitch_is_link_in_reset(device, link);
}

NvBool
nvswitch_i2c_is_device_access_allowed
(
    nvswitch_device *device,
    NvU32 port,
    NvU8 addr,
    NvBool bIsRead
)
{
    return device->hal.nvswitch_i2c_is_device_access_allowed(device, port, addr, bIsRead);
}

NvlStatus
nvswitch_parse_bios_image
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_parse_bios_image(device);
}

void
nvswitch_init_buffer_ready
(
    nvswitch_device *device,
    nvlink_link *link,
    NvBool bNportBufferReady
)
{
    device->hal.nvswitch_init_buffer_ready(device, link, bNportBufferReady);
}

void
nvswitch_apply_recal_settings
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    device->hal.nvswitch_apply_recal_settings(device, link);
}

NvlStatus
nvswitch_launch_ALI_link_training
(
    nvswitch_device *device,
    nvlink_link *link,
    NvBool bSync
)
{
    return device->hal.nvswitch_launch_ALI_link_training(device, link, bSync);
}

NvlStatus
nvswitch_reset_and_train_link
(
    nvswitch_device *device,
    nvlink_link     *link
)
{
    return device->hal.nvswitch_reset_and_train_link(device, link);
}

static NvlStatus
_nvswitch_ctrl_get_err_info
(
    nvswitch_device *device,
    NVSWITCH_NVLINK_GET_ERR_INFO_PARAMS *ret
)
{
    return device->hal.nvswitch_ctrl_get_err_info(device, ret);
}

static NvlStatus
_nvswitch_ctrl_clear_counters
(
    nvswitch_device *device,
    NVSWITCH_NVLINK_CLEAR_COUNTERS_PARAMS *ret
)
{
    return device->hal.nvswitch_ctrl_clear_counters(device, ret);
}

void      
nvswitch_setup_link_system_registers
(
    nvswitch_device *device, 
    nvlink_link *link
)
{
    device->hal.nvswitch_setup_link_system_registers(device, link);
}

void
nvswitch_load_link_disable_settings
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    device->hal.nvswitch_load_link_disable_settings(device, link);
}

static NvlStatus
_nvswitch_ctrl_set_nvlink_error_threshold
(
    nvswitch_device *device,
    NVSWITCH_SET_NVLINK_ERROR_THRESHOLD_PARAMS *pParams
)
{
    return device->hal.nvswitch_ctrl_set_nvlink_error_threshold(device, pParams);
}

static NvlStatus
_nvswitch_ctrl_get_nvlink_error_threshold
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_THRESHOLD_PARAMS *pParams
)
{
    return device->hal.nvswitch_ctrl_get_nvlink_error_threshold(device, pParams);
}

static NvlStatus
_nvswitch_ctrl_therm_read_voltage
(
    nvswitch_device *device,
    NVSWITCH_CTRL_GET_VOLTAGE_PARAMS *info
)
{
    return device->hal.nvswitch_ctrl_therm_read_voltage(device, info);
}

static NvlStatus
_nvswitch_ctrl_therm_read_power
(
    nvswitch_device *device,
    NVSWITCH_GET_POWER_PARAMS *info
)
{
    return device->hal.nvswitch_ctrl_therm_read_power(device, info);
}

NvlStatus
nvswitch_get_board_id
(
    nvswitch_device *device,
    NvU16 *boardId
)
{
    return device->hal.nvswitch_get_board_id(device, boardId);
}

NvlStatus
_nvswitch_ctrl_get_link_l1_capability
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_L1_CAPABILITY_PARAMS *p
)
{
    NvlStatus status;
    nvlink_link *link;
    NvU8 i;

    if (p->linkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: No links in linkMask\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    FOR_EACH_INDEX_IN_MASK(64, i, p->linkMask)
    {
        NvU32 linkNum;

        NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

        link = nvswitch_get_link(device, i);
        if ((link == NULL) ||
            (i >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Invalid input link %d set in linkMask\n",
                           __FUNCTION__, i);
            return -NVL_BAD_ARGS;
        }

        linkNum = link->linkNumber;

        status = device->hal.nvswitch_ctrl_get_link_l1_capability(device, linkNum, &(p->l1Capable[linkNum]));
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to get l1 capability for link %d\n",
                           __FUNCTION__, linkNum);
            return status;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

NvlStatus
_nvswitch_ctrl_get_link_l1_threshold
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_L1_THRESHOLD_PARAMS *p
)
{
    NvlStatus status;
    nvlink_link *link;
    NvBool isL1Capable;
    NvU8 i;

    if (device->regkeys.enable_pm == NV_SWITCH_REGKEY_ENABLE_PM_NO)
    {
        NVSWITCH_PRINT(device, INFO, "%s: L1 Threshold is disabled\n", __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (p->linkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: No links in linkMask\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    FOR_EACH_INDEX_IN_MASK(64, i, p->linkMask)
    {
        NvU32 linkNum;

        NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

        link = nvswitch_get_link(device, i);
        if ((link == NULL) ||
            (i >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Invalid input link %d set in linkMask\n",
                           __FUNCTION__, i);
            return -NVL_BAD_ARGS;
        }

        linkNum = link->linkNumber;

        status = device->hal.nvswitch_ctrl_get_link_l1_capability(device, linkNum, &isL1Capable);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to get l1 capability for link %d\n",
                           __FUNCTION__, linkNum);
            return status;
        }

        if (!isL1Capable)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Input link %d does not support L1\n",
                           __FUNCTION__, i);
            return -NVL_ERR_NOT_SUPPORTED;
        }

        // Get HAL 
        status = device->hal.nvswitch_ctrl_get_link_l1_threshold(device, linkNum, &(p->l1Threshold[linkNum]));
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to get L1 Threshold for link %d\n",
                           __FUNCTION__, linkNum);
            return status;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}

NvlStatus
_nvswitch_ctrl_set_link_l1_threshold
(
    nvswitch_device *device,
    NVSWITCH_SET_NVLINK_L1_THRESHOLD_PARAMS *p
)
{
    NvlStatus status;
    nvlink_link *link;
    NvBool isL1Capable;
    NvU8 i;

    if (device->regkeys.enable_pm == NV_SWITCH_REGKEY_ENABLE_PM_NO)
    {
        NVSWITCH_PRINT(device, INFO, "%s: L1 Threshold is disabled\n", __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (p->linkMask == 0)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: No links in linkMask\n", __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    FOR_EACH_INDEX_IN_MASK(64, i, p->linkMask)
    {
        NvU32 linkNum;
        NvU32 l1Threshold;

        NVSWITCH_ASSERT(i < NVSWITCH_LINK_COUNT(device));

        link = nvswitch_get_link(device, i);
        if ((link == NULL) ||
            (i >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Invalid input link %d set in linkMask\n",
                           __FUNCTION__, i);
            return -NVL_BAD_ARGS;
        }

        linkNum = link->linkNumber;
        l1Threshold = p->l1Threshold[linkNum];

        status = device->hal.nvswitch_ctrl_get_link_l1_capability(device, linkNum, &isL1Capable);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to get l1 capability for link %d\n",
                           __FUNCTION__, linkNum);
            return status;
        }

        if (!isL1Capable)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Input link %d does not support L1\n",
                           __FUNCTION__, i);
            return -NVL_ERR_NOT_SUPPORTED;
        }

        if (((l1Threshold < NVSWITCH_SET_NVLINK_L1_THRESHOLD_MIN) ||
             (l1Threshold > NVSWITCH_SET_NVLINK_L1_THRESHOLD_MAX)) &&
            (l1Threshold != NVSWITCH_SET_NVLINK_L1_THRESHOLD_DEFAULT))
        {
            return -NVL_BAD_ARGS;
        }

        status = device->hal.nvswitch_ctrl_set_link_l1_threshold(link,
                                                                 p->l1Threshold[linkNum]);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR, "%s: Failed to set L1 Threshold for link %d\n",
                           __FUNCTION__, linkNum);
            return status;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_detect_tnvl_mode
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_detect_tnvl_mode(device);
}

NvBool
nvswitch_is_tnvl_mode_enabled
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_tnvl_mode_enabled(device);
}

NvBool
nvswitch_is_tnvl_mode_locked
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_tnvl_mode_locked(device);
}

NvBool NV_API_CALL
nvswitch_lib_is_tnvl_enabled
(
    nvswitch_device *device
)
{
    return nvswitch_is_tnvl_mode_enabled(device);
}

NvlStatus
nvswitch_tnvl_send_fsp_lock_config
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_tnvl_send_fsp_lock_config(device);
}

static NvlStatus
_nvswitch_ctrl_set_device_tnvl_lock
(
    nvswitch_device *device,
    NVSWITCH_SET_DEVICE_TNVL_LOCK_PARAMS *p
)
{
    NvlStatus status = NVL_SUCCESS;

    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        NVSWITCH_PRINT(device, ERROR,
           "%s: TNVL is not enabled\n",
           __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    // Return failure if FM is not yet configured
    if (device->device_fabric_state != NVSWITCH_DEVICE_FABRIC_STATE_CONFIGURED)
    {
        NVSWITCH_PRINT(device, ERROR,
           "%s: FM is not configured yet\n",
           __FUNCTION__);
        return -NVL_ERR_INVALID_STATE;
    }

    //
    // Disable non-fatal and legacy interrupts
    // Disable commands to SOE
    //

    // Send lock-config command to FSP
    status = nvswitch_tnvl_send_fsp_lock_config(device);
    if (status == NVL_SUCCESS)
    {
        device->tnvl_mode = NVSWITCH_DEVICE_TNVL_MODE_LOCKED;
    }
    else
    {
        device->tnvl_mode = NVSWITCH_DEVICE_TNVL_MODE_FAILURE;
    }

    return status;
}

NvlStatus
nvswitch_lib_ctrl
(
    nvswitch_device *device,
    NvU32 cmd,
    void *params,
    NvU64 size,
    void *osPrivate
)
{
    NvlStatus retval;
    NvU64 flags = 0;

    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device) || params == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    flags = NVSWITCH_DEV_CMD_CHECK_ADMIN | NVSWITCH_DEV_CMD_CHECK_FM;
    switch (cmd)
    {
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_INFO,
                _nvswitch_ctrl_get_info,
                NVSWITCH_GET_INFO);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_INTERNAL_LATENCY,
                _nvswitch_ctrl_get_internal_latency,
                NVSWITCH_GET_INTERNAL_LATENCY);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_NVLIPT_COUNTERS,
                _nvswitch_ctrl_get_nvlipt_counters,
                NVSWITCH_GET_NVLIPT_COUNTERS);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_ERRORS,
                nvswitch_ctrl_get_errors,
                NVSWITCH_GET_ERRORS_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_PORT_EVENTS,
                nvswitch_ctrl_get_port_events,
                NVSWITCH_GET_PORT_EVENTS_PARAMS);        
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_NVLINK_STATUS,
                _nvswitch_ctrl_get_nvlink_status,
                NVSWITCH_GET_NVLINK_STATUS_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH_WITH_PRIVATE_DATA(
                CTRL_NVSWITCH_ACQUIRE_CAPABILITY,
                _nvswitch_ctrl_acquire_capability,
                NVSWITCH_ACQUIRE_CAPABILITY_PARAMS,
                osPrivate);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_TEMPERATURE,
                _nvswitch_ctrl_therm_read_temperature,
                NVSWITCH_CTRL_GET_TEMPERATURE_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_THROUGHPUT_COUNTERS,
                nvswitch_ctrl_get_throughput_counters,
                NVSWITCH_GET_THROUGHPUT_COUNTERS_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_FATAL_ERROR_SCOPE,
                _nvswitch_ctrl_get_fatal_error_scope,
                NVSWITCH_GET_FATAL_ERROR_SCOPE_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_SWITCH_PORT_CONFIG,
                _nvswitch_ctrl_set_switch_port_config,
                NVSWITCH_SET_SWITCH_PORT_CONFIG,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_INGRESS_REQUEST_TABLE,
                _nvswitch_ctrl_get_ingress_request_table,
                NVSWITCH_GET_INGRESS_REQUEST_TABLE_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_INGRESS_REQUEST_TABLE,
                _nvswitch_ctrl_set_ingress_request_table,
                NVSWITCH_SET_INGRESS_REQUEST_TABLE,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_INGRESS_REQUEST_VALID,
                _nvswitch_ctrl_set_ingress_request_valid,
                NVSWITCH_SET_INGRESS_REQUEST_VALID,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_INGRESS_RESPONSE_TABLE,
                _nvswitch_ctrl_get_ingress_response_table,
                NVSWITCH_GET_INGRESS_RESPONSE_TABLE_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_INGRESS_RESPONSE_TABLE,
                _nvswitch_ctrl_set_ingress_response_table,
                NVSWITCH_SET_INGRESS_RESPONSE_TABLE,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_GANGED_LINK_TABLE,
                _nvswitch_ctrl_set_ganged_link_table,
                NVSWITCH_SET_GANGED_LINK_TABLE,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_SET_LATENCY_BINS,
                nvswitch_ctrl_set_latency_bins,
                NVSWITCH_SET_LATENCY_BINS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_NVLIPT_COUNTER_CONFIG,
                _nvswitch_ctrl_set_nvlipt_counter_config,
                NVSWITCH_SET_NVLIPT_COUNTER_CONFIG,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_NVLIPT_COUNTER_CONFIG,
                _nvswitch_ctrl_get_nvlipt_counter_config,
                NVSWITCH_GET_NVLIPT_COUNTER_CONFIG,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_SET_REMAP_POLICY,
                _nvswitch_ctrl_set_remap_policy,
                NVSWITCH_SET_REMAP_POLICY,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_GET_REMAP_POLICY,
                _nvswitch_ctrl_get_remap_policy,
                NVSWITCH_GET_REMAP_POLICY_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_REMAP_POLICY_VALID,
                _nvswitch_ctrl_set_remap_policy_valid,
                NVSWITCH_SET_REMAP_POLICY_VALID,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_SET_ROUTING_ID,
                _nvswitch_ctrl_set_routing_id,
                NVSWITCH_SET_ROUTING_ID,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_GET_ROUTING_ID,
                _nvswitch_ctrl_get_routing_id,
                NVSWITCH_GET_ROUTING_ID_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_SET_ROUTING_ID_VALID,
                _nvswitch_ctrl_set_routing_id_valid,
                NVSWITCH_SET_ROUTING_LAN_VALID,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_SET_ROUTING_LAN,
                _nvswitch_ctrl_set_routing_lan,
                NVSWITCH_SET_ROUTING_LAN,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_GET_ROUTING_LAN,
                _nvswitch_ctrl_get_routing_lan,
                NVSWITCH_GET_ROUTING_LAN_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_ROUTING_LAN_VALID,
                _nvswitch_ctrl_set_routing_lan_valid,
                NVSWITCH_SET_ROUTING_LAN_VALID,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_INGRESS_REQLINKID,
                _nvswitch_ctrl_get_ingress_reqlinkid,
                NVSWITCH_GET_INGRESS_REQLINKID_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_UNREGISTER_LINK,
                _nvswitch_ctrl_unregister_link,
                NVSWITCH_UNREGISTER_LINK_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_RESET_AND_DRAIN_LINKS,
                _nvswitch_ctrl_reset_and_drain_links,
                NVSWITCH_RESET_AND_DRAIN_LINKS_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_BIOS_INFO,
                _nvswitch_ctrl_get_bios_info,
                NVSWITCH_GET_BIOS_INFO_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_INFOROM_VERSION,
                _nvswitch_ctrl_get_inforom_version,
                NVSWITCH_GET_INFOROM_VERSION_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_BLACKLIST_DEVICE,
                nvswitch_ctrl_blacklist_device,
                NVSWITCH_BLACKLIST_DEVICE_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_FM_DRIVER_STATE,
                nvswitch_ctrl_set_fm_driver_state,
                NVSWITCH_SET_FM_DRIVER_STATE_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_DEVICE_FABRIC_STATE,
                nvswitch_ctrl_set_device_fabric_state,
                NVSWITCH_SET_DEVICE_FABRIC_STATE_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_FM_HEARTBEAT_TIMEOUT,
                nvswitch_ctrl_set_fm_timeout,
                NVSWITCH_SET_FM_HEARTBEAT_TIMEOUT_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_WITH_PRIVATE_DATA(
                CTRL_NVSWITCH_REGISTER_EVENTS,
                _nvswitch_ctrl_register_events,
                NVSWITCH_REGISTER_EVENTS_PARAMS,
                osPrivate);
        NVSWITCH_DEV_CMD_DISPATCH_WITH_PRIVATE_DATA(
                CTRL_NVSWITCH_UNREGISTER_EVENTS,
                _nvswitch_ctrl_unregister_events,
                NVSWITCH_UNREGISTER_EVENTS_PARAMS,
                osPrivate);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_TRAINING_ERROR_INFO,
                _nvswitch_ctrl_set_training_error_info,
                NVSWITCH_SET_TRAINING_ERROR_INFO_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_MC_RID_TABLE,
                _nvswitch_ctrl_set_mc_rid_table,
                NVSWITCH_SET_MC_RID_TABLE_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_MC_RID_TABLE,
                _nvswitch_ctrl_get_mc_rid_table,
                NVSWITCH_GET_MC_RID_TABLE_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_COUNTERS,
                _nvswitch_ctrl_get_counters,
                NVSWITCH_NVLINK_GET_COUNTERS_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_NVLINK_ECC_ERRORS,
                _nvswitch_ctrl_get_nvlink_ecc_errors,
                NVSWITCH_GET_NVLINK_ECC_ERRORS_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_I2C_SMBUS_COMMAND,
                _nvswitch_ctrl_i2c_smbus_command,
                NVSWITCH_I2C_SMBUS_COMMAND_PARAMS,
                osPrivate, NVSWITCH_DEV_CMD_CHECK_ADMIN);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_CCI_CMIS_PRESENCE,
                _nvswitch_ctrl_cci_cmis_presence,
                NVSWITCH_CCI_CMIS_PRESENCE_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_CCI_CMIS_NVLINK_MAPPING,
                _nvswitch_ctrl_cci_nvlink_mappings,
                NVSWITCH_CCI_CMIS_NVLINK_MAPPING_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_CCI_CMIS_MEMORY_ACCESS_READ,
                _nvswitch_ctrl_cci_memory_access_read,
                NVSWITCH_CCI_CMIS_MEMORY_ACCESS_READ_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_CCI_CMIS_MEMORY_ACCESS_WRITE,
                _nvswitch_ctrl_cci_memory_access_write,
                NVSWITCH_CCI_CMIS_MEMORY_ACCESS_WRITE_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_CCI_CMIS_CAGE_BEZEL_MARKING,
                _nvswitch_ctrl_cci_cage_bezel_marking,
                NVSWITCH_CCI_CMIS_CAGE_BEZEL_MARKING_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_CCI_GET_GRADING_VALUES,
                nvswitch_ctrl_get_grading_values,
                NVSWITCH_CCI_GET_GRADING_VALUES_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_CCI_GET_PORTS_CPLD_INFO,
                nvswitch_ctrl_get_ports_cpld_info,
                NVSWITCH_CCI_GET_PORTS_CPLD_INFO_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_CCI_GET_FW_REVISIONS,
                nvswitch_ctrl_get_cci_fw_revisions,
                NVSWITCH_CCI_GET_FW_REVISION_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_CCI_SET_LOCATE_LED,
                nvswitch_ctrl_set_locate_led,
                NVSWITCH_CCI_SET_LOCATE_LED_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_SOE_HEARTBEAT,
                _nvswitch_ctrl_get_soe_heartbeat,
                NVSWITCH_GET_SOE_HEARTBEAT_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_CONTINUOUS_ALI,
                _nvswitch_ctrl_set_continuous_ali,
                NVSWITCH_SET_CONTINUOUS_ALI_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_REQUEST_ALI,
                _nvswitch_ctrl_request_ali,
                NVSWITCH_REQUEST_ALI_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH(
                CTRL_NVSWITCH_GET_TEMPERATURE_LIMIT,
                _nvswitch_ctrl_therm_get_temperature_limit,
                NVSWITCH_CTRL_GET_TEMPERATURE_LIMIT_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_NVLINK_MAX_ERROR_RATES,
                _nvswitch_ctrl_get_inforom_nvlink_max_correctable_error_rate,
                NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_NVLINK_ERROR_COUNTS,
                _nvswitch_ctrl_get_inforom_nvlink_errors,
                NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_ECC_ERROR_COUNTS,
                _nvswitch_ctrl_get_inforom_ecc_errors,
                NVSWITCH_GET_ECC_ERROR_COUNTS_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_SXIDS,
                _nvswitch_ctrl_get_inforom_bbx_sxid,
                NVSWITCH_GET_SXIDS_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_FOM_VALUES,
                _nvswitch_ctrl_get_fom_values,
                NVSWITCH_GET_FOM_VALUES_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_NVLINK_LP_COUNTERS,
                _nvswitch_ctrl_get_nvlink_lp_counters,
                NVSWITCH_GET_NVLINK_LP_COUNTERS_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_RESIDENCY_BINS,
                _nvswitch_ctrl_get_residency_bins,
                NVSWITCH_GET_RESIDENCY_BINS);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_SET_RESIDENCY_BINS,
                _nvswitch_ctrl_set_residency_bins,
                NVSWITCH_SET_RESIDENCY_BINS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_RB_STALL_BUSY,
                _nvswitch_ctrl_get_rb_stall_busy,
                NVSWITCH_GET_RB_STALL_BUSY);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR,
                nvswitch_ctrl_get_multicast_id_error_vector,
                NVSWITCH_GET_MULTICAST_ID_ERROR_VECTOR);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR,
                nvswitch_ctrl_clear_multicast_id_error_vector,
                NVSWITCH_CLEAR_MULTICAST_ID_ERROR_VECTOR);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_INBAND_SEND_DATA,
                _nvswitch_ctrl_inband_send_data,
                NVSWITCH_INBAND_SEND_DATA_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_INBAND_READ_DATA,
                _nvswitch_ctrl_inband_read_data,
                 NVSWITCH_INBAND_READ_DATA_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_INBAND_FLUSH_DATA,
                _nvswitch_ctrl_inband_flush_data,
                NVSWITCH_INBAND_FLUSH_DATA_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_INBAND_PENDING_DATA_STATS,
                _nvswitch_ctrl_inband_pending_data_stats,
                NVSWITCH_INBAND_PENDING_DATA_STATS_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_BOARD_PART_NUMBER,
                _nvswitch_ctrl_get_board_part_number,
                NVSWITCH_GET_BOARD_PART_NUMBER_VECTOR);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_SW_INFO,
                _nvswitch_ctrl_get_sw_info,
                NVSWITCH_GET_SW_INFO_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_REGISTER_READ,
                _nvswitch_ctrl_register_read,
                NVSWITCH_REGISTER_READ,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_REGISTER_WRITE,
                _nvswitch_ctrl_register_write,
                NVSWITCH_REGISTER_WRITE,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_GET_ERR_INFO,
                _nvswitch_ctrl_get_err_info,
                NVSWITCH_NVLINK_GET_ERR_INFO_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_CLEAR_COUNTERS,
                _nvswitch_ctrl_clear_counters,
                NVSWITCH_NVLINK_CLEAR_COUNTERS_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_SET_NVLINK_ERROR_THRESHOLD,
                _nvswitch_ctrl_set_nvlink_error_threshold,
                NVSWITCH_SET_NVLINK_ERROR_THRESHOLD_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_NVLINK_ERROR_THRESHOLD,
                _nvswitch_ctrl_get_nvlink_error_threshold,
                NVSWITCH_GET_NVLINK_ERROR_THRESHOLD_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_VOLTAGE,
                _nvswitch_ctrl_therm_read_voltage,
                NVSWITCH_CTRL_GET_VOLTAGE_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_POWER,
                _nvswitch_ctrl_therm_read_power,
                NVSWITCH_GET_POWER_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_SYS_INFO,
                _nvswitch_ctrl_get_inforom_bbx_sys_info,
                NVSWITCH_GET_SYS_INFO_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_TIME_INFO,
                _nvswitch_ctrl_get_inforom_bbx_time_info,
                NVSWITCH_GET_TIME_INFO_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_TEMP_DATA,
                _nvswitch_ctrl_get_inforom_bbx_temp_data,
                NVSWITCH_GET_TEMP_DATA_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_TEMP_SAMPLES,
                _nvswitch_ctrl_get_inforom_bbx_temp_samples,
                NVSWITCH_GET_TEMP_SAMPLES_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_NVLINK_L1_CAPABILITY,
                _nvswitch_ctrl_get_link_l1_capability,
                NVSWITCH_GET_NVLINK_L1_CAPABILITY_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_GET_NVLINK_L1_THRESHOLD,
                _nvswitch_ctrl_get_link_l1_threshold,
                NVSWITCH_GET_NVLINK_L1_THRESHOLD_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(CTRL_NVSWITCH_SET_NVLINK_L1_THRESHOLD,
                _nvswitch_ctrl_set_link_l1_threshold,
                NVSWITCH_SET_NVLINK_L1_THRESHOLD_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH(CTRL_NVSWITCH_FSPRPC_GET_CAPS,
                _nvswitch_ctrl_fsprpc_get_caps,
                NVSWITCH_FSPRPC_GET_CAPS_PARAMS);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_SET_DEVICE_TNVL_LOCK,
                _nvswitch_ctrl_set_device_tnvl_lock,
                NVSWITCH_SET_DEVICE_TNVL_LOCK_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN,
                _nvswitch_ctrl_get_attestation_certificate_chain,
                NVSWITCH_GET_ATTESTATION_CERTIFICATE_CHAIN_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_ATTESTATION_REPORT,
                _nvswitch_ctrl_get_attestation_report,
                NVSWITCH_GET_ATTESTATION_REPORT_PARAMS,
                osPrivate, flags);
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_TNVL_STATUS,
                _nvswitch_ctrl_get_tnvl_status,
                NVSWITCH_GET_TNVL_STATUS_PARAMS,
                osPrivate, flags);
        default:
            nvswitch_os_print(NVSWITCH_DBG_LEVEL_INFO, "unknown ioctl %x\n", cmd);
            retval = -NVL_BAD_ARGS;
            break;
    }

    return retval;
}

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
void nvswitch_assert_log
(
    const char *function,
    const char *file,
    NvU32 line
)
{
    nvswitch_os_assert_log("NVSwitch: Assertion failed in %s() at %s:%d\n",
                           function, file, line);
}
#else
void nvswitch_assert_log(void)
{
    nvswitch_os_assert_log("NVSwitch: Assertion failed\n");
}
#endif
