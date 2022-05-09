/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvVer.h"

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
        retval = -NVL_ERR_NOT_IMPLEMENTED;                                       \
        break;                                                                   \
    }                                                                            \

//
// HW's device id list can be found here -
// P4hw:2001: hw\doc\engr\Dev_ID\DeviceID_master_list.txt
//

const static NvU32 nvswitch_lr10_device_ids[] =
{
    0x1AE8, 0x1AF0, 0x1AF1, 0x1AF2, 0x1AF3, 0x1AF4, 0x1AF5, 0x1AF6, 0x1AF7,
    0x1AF8, 0x1AF9, 0x1AFA, 0x1AFB, 0x1AFC, 0x1AFD, 0x1AFE, 0x1AFF
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
    return device->hal.nvswitch_corelib_get_uphy_load(link, bUnlocked);
}


static NV_API_CALL NvlStatus
_nvswitch_corelib_write_discovery_token
(
    nvlink_link *link,
    NvU64 token
)
{
    return NVL_SUCCESS;
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
                         NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_SHORT_OFF);

    NVSWITCH_INIT_REGKEY(_PUBLIC, crc_bit_error_rate_long,
                         NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG,
                         NV_SWITCH_REGKEY_CRC_BIT_ERROR_RATE_LONG_OFF);

    //
    // Private internal use regkeys
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

    NVSWITCH_INIT_REGKEY(_PRIVATE, enable_pm,
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

    NVSWITCH_INIT_REGKEY(_PUBLIC, soe_boot_core,
                         NV_SWITCH_REGKEY_SOE_BOOT_CORE,
                         NV_SWITCH_REGKEY_SOE_BOOT_CORE_DEFAULT);
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

    NVSWITCH_INIT_REGKEY(_PRIVATE, i2c_access_control,
                         NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL,
                         NV_SWITCH_REGKEY_I2C_ACCESS_CONTROL_DEFAULT);

    NVSWITCH_INIT_REGKEY(_PRIVATE, link_recal_settings,
                         NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS,
                         NV_SWITCH_REGKEY_LINK_RECAL_SETTINGS_NOP);
}
NvU64
nvswitch_lib_deferred_task_dispatcher
(
    nvswitch_device *device
)
{
    NvU64 time_nsec;
    NvU64 time_next_nsec = nvswitch_os_get_platform_time() + 100*NVSWITCH_INTERVAL_1MSEC_IN_NS;
    NVSWITCH_TASK_TYPE *task;

    if (!NVSWITCH_IS_DEVICE_VALID(device))
    {
        return NV_U64_MAX;
    }

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
               (task->flags & NVSWITCH_TASK_TYPE_FLAGS_ALWAYS_RUN))
                (*task->task_fn)(device);
        }

        // Determine its next run time
        time_next_nsec = NV_MIN(task->last_run_nsec + task->period_nsec, time_next_nsec);
        task = task->next;
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
nvswitch_is_smbpbi_supported
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_smbpbi_supported(device);
}

NvlStatus
nvswitch_soe_prepare_for_reset
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_soe_prepare_for_reset(device);
}

NvBool
nvswitch_is_soe_supported
(
    nvswitch_device *device
)
{
    if (device->regkeys.soe_disable == NV_SWITCH_REGKEY_SOE_DISABLE_YES)
    {
        NVSWITCH_PRINT(device, INFO, "SOE is disabled via regkey.\n");
        return NV_FALSE;
    }

    return device->hal.nvswitch_is_soe_supported(device);
}

NvlStatus
nvswitch_soe_set_ucode_core
(
    nvswitch_device *device,
    NvBool bFalcon
)
{
    return device->hal.nvswitch_soe_set_ucode_core(device, bFalcon);
}

NvlStatus
nvswitch_init_soe
(
    nvswitch_device *device
)
{
    if (device->regkeys.soe_disable == NV_SWITCH_REGKEY_SOE_DISABLE_YES)
    {
        NVSWITCH_PRINT(device, INFO, "SOE is disabled via regkey.\n");
        return NV_FALSE;
    }

    return device->hal.nvswitch_init_soe(device);
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
_nvswitch_setup_link_system_registers
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_setup_link_system_registers(device);
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

    nvswitch_lib_disable_interrupts(device);

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
    if (!NVSWITCH_IS_DEVICE_ACCESSIBLE(device))
    {
        return -NVL_BAD_ARGS;
    }

    device->driver_fabric_state = p->driverState;
    device->fabric_state_timestamp = nvswitch_os_get_platform_time();

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

static void
nvswitch_fabric_state_heartbeat(
    nvswitch_device *device
)
{
    NvU64 age;

    if (!NVSWITCH_IS_DEVICE_VALID(device))
        return;

    age = nvswitch_os_get_platform_time() - device->fabric_state_timestamp;

    // Check to see if we have exceeded the FM timeout
    if (device->driver_fabric_state == NVSWITCH_DRIVER_FABRIC_STATE_CONFIGURED &&
        age > (NvU64)device->fm_timeout * 1000ULL * 1000ULL)
         device->driver_fabric_state = NVSWITCH_DRIVER_FABRIC_STATE_MANAGER_TIMEOUT;

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
                         NVSWITCH_TASK_TYPE_FLAGS_ALWAYS_RUN);

    if (device->device_blacklist_reason == NVSWITCH_DEVICE_BLACKLIST_REASON_MANUAL_OUT_OF_BAND)
    {
        NVSWITCH_PRINT(device, SETUP,
            "Blacklisted nvswitch at (%04x:%02x:%02x.%02x)\n",
            device->nvlink_device->pciInfo.domain,
            device->nvlink_device->pciInfo.bus,
            device->nvlink_device->pciInfo.device,
            device->nvlink_device->pciInfo.function);
        return retval;
    }

    if (is_blacklisted_by_os)
    {
        (void)nvswitch_lib_blacklist_device(device, NVSWITCH_DEVICE_BLACKLIST_REASON_MANUAL_IN_BAND);
        return retval;
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
    _nvswitch_destruct_soe(device);

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
    return NV_FALSE;
}

NvlStatus
nvswitch_lib_post_init_device
(
    nvswitch_device *device
)
{
    NvlStatus retval;

    if (!NVSWITCH_IS_DEVICE_INITIALIZED(device))
    {
        return -NVL_ERR_INVALID_STATE;
    }

    retval = _nvswitch_post_init_device_setup(device);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }
    
    if (nvswitch_is_spi_supported(device))
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
            "%s: Skipping BIOS parsing since SPI is unsupported.\n",
            __FUNCTION__);
    }

    retval = _nvswitch_setup_link_system_registers(device);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    nvswitch_smbpbi_post_init(device);

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

/*!
 * @brief: Gets the client event associated with the file descriptor
 *         if it already exists in the Device's client event list.
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

    _nvswitch_unregister_links(device);

    nvswitch_destroy_error_log(device, &device->log_FATAL_ERRORS);
    nvswitch_destroy_error_log(device, &device->log_NONFATAL_ERRORS);

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
    NvU32 *fatal, NvU32 *nonfatal
)
{
    if (!NVSWITCH_IS_DEVICE_INITIALIZED(device) ||
        fatal == NULL || nonfatal == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    *fatal = device->log_FATAL_ERRORS.error_count;
    *nonfatal = device->log_NONFATAL_ERRORS.error_count;
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
    *version = p.version;

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

void
nvswitch_task_create
(
    nvswitch_device *device,
    void (*task_fn)(nvswitch_device *device),
    NvU64 period_nsec,
    NvU32 flags
)
{
    NVSWITCH_TASK_TYPE *task = nvswitch_os_malloc(sizeof(*task));

    if (task == NULL)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unable to allocate task.\n",
            __FUNCTION__);
    }
    else
    {
        task->task_fn = task_fn;
        task->period_nsec = period_nsec;
        task->last_run_nsec = 0;
        task->flags = flags;
        task->next = device->tasks;
        device->tasks = task;
    }
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
    // _nvswitch_ctrl_get_throughput_counters
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
    // _nvswitch_ctrl_get_throughput_counters_lr10
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
    // _nvswitch_ctrl_get_throughput_counters_lr10
    //
    // Getting counter config is useful if counters are
    // configurable. These counters are not configurable
    // and are expected to be used by monitoring clients.
    //
    return -NVL_ERR_NOT_SUPPORTED;
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

static NvlStatus
_nvswitch_ctrl_get_throughput_counters
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

    if (device->hal.nvswitch_is_link_in_use(device, params->portNum))
    {
        return -NVL_ERR_STATE_IN_USE;
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
    return device->hal.nvswitch_reset_and_drain_links(device, params->linkMask);
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
    return device->hal.nvswitch_init_clock_gating(device);
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
    char    string[80];

    va_start(arglist, pFormat);
    msglen = nvswitch_os_vsnprintf(string, sizeof(string), pFormat, arglist);
    va_end(arglist);

    if (!(msglen < 0))
    {
        msglen = NV_MIN(msglen + 1, (int) sizeof(string));
        nvswitch_smbpbi_log_message(device, sxid, msglen, (NvU8 *) string);
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
nvswitch_get_soe_ucode_binaries
(
    nvswitch_device *device,
    const NvU32 **soe_ucode_data,
    const NvU32 **soe_ucode_header
)
{
    return device->hal.nvswitch_get_soe_ucode_binaries(device, soe_ucode_data, soe_ucode_header);
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
    return device->hal.nvswitch_setup_link_loopback_mode(device, linkNumber);
}

void
nvswitch_reset_persistent_link_hw_state
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    return device->hal.nvswitch_reset_persistent_link_hw_state(device, linkNumber);
}

void
nvswitch_store_topology_information
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    return device->hal.nvswitch_store_topology_information(device, link);
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
    return device->hal.nvswitch_init_buffer_ready(device, link, bNportBufferReady);
}

void
nvswitch_apply_recal_settings
(
    nvswitch_device *device,
    nvlink_link *link
)
{
    return device->hal.nvswitch_apply_recal_settings(device, link);
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
                _nvswitch_ctrl_get_throughput_counters,
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
        NVSWITCH_DEV_CMD_DISPATCH_RESERVED(
                CTRL_NVSWITCH_RESERVED_0);
        NVSWITCH_DEV_CMD_DISPATCH_RESERVED(
                CTRL_NVSWITCH_RESERVED_1);
        NVSWITCH_DEV_CMD_DISPATCH_RESERVED(
                CTRL_NVSWITCH_RESERVED_2);
        NVSWITCH_DEV_CMD_DISPATCH_RESERVED(
                CTRL_NVSWITCH_RESERVED_3);
	    NVSWITCH_DEV_CMD_DISPATCH_RESERVED(
                CTRL_NVSWITCH_RESERVED_4);
        NVSWITCH_DEV_CMD_DISPATCH_RESERVED(
                CTRL_NVSWITCH_RESERVED_5);
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
        NVSWITCH_DEV_CMD_DISPATCH_PRIVILEGED(
                CTRL_NVSWITCH_GET_SW_INFO,
                _nvswitch_ctrl_get_sw_info,
                NVSWITCH_GET_SW_INFO_PARAMS,
                osPrivate, flags);
        default:
            nvswitch_os_print(NVSWITCH_DBG_LEVEL_INFO, "unknown ioctl %x\n", cmd);
            retval = -NVL_BAD_ARGS;
            break;
    }

    return retval;
}
