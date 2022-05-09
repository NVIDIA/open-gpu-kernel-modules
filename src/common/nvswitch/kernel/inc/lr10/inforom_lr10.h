/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _INFOROM_LR10_H_
#define _INFOROM_LR10_H_

NvlStatus nvswitch_inforom_nvl_log_error_event_lr10
(
    nvswitch_device            *device,
    void                       *pNvlGeneric,
    void                       *pNvlErrorEvent,
    NvBool                     *bDirty
);

NvlStatus nvswitch_inforom_nvl_update_link_correctable_error_info_lr10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    void *pData,
    NvU8 linkId,
    NvU8 nvliptInstance,
    NvU8 localLinkIdx,
    void *pNvlErrorCounts,
    NvBool *bDirty
);

NvlStatus
nvswitch_inforom_nvl_get_max_correctable_error_rate_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS *params
);

NvlStatus
nvswitch_inforom_nvl_get_errors_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS *params
);

NvlStatus
nvswitch_inforom_ecc_log_error_event_lr10
(
    nvswitch_device *device,
    INFOROM_ECC_OBJECT *pEccGeneric,
    INFOROM_NVS_ECC_ERROR_EVENT *err_event
);

void
nvswitch_inforom_ecc_get_total_errors_lr10
(
    nvswitch_device     *device,
    INFOROM_ECC_OBJECT  *pEccGeneric,
    NvU64               *pCorrectedTotal,
    NvU64               *pUncorrectedTotal
);

NvlStatus
nvswitch_inforom_ecc_get_errors_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_ECC_ERROR_COUNTS_PARAMS *params
);

void
nvswitch_initialize_oms_state_lr10
(
    nvswitch_device *device,
    INFOROM_OMS_STATE *pOmsState
);

NvBool
nvswitch_oms_get_device_disable_lr10
(
    INFOROM_OMS_STATE *pOmsState
);

void
nvswitch_oms_set_device_disable_lr10
(
    INFOROM_OMS_STATE *pOmsState,
    NvBool bForceDeviceDisable
);

NvlStatus
nvswitch_oms_inforom_flush_lr10
(
    struct nvswitch_device *device
);

NvlStatus
nvswitch_bbx_setup_prologue_lr10
(
    nvswitch_device    *device,
    void *pInforomBbxState
);

NvlStatus
nvswitch_bbx_setup_epilogue_lr10
(
    nvswitch_device    *device,
    void *pInforomBbxState
);

NvlStatus
nvswitch_bbx_add_data_time_lr10
(
    nvswitch_device *device,
    void *pInforomBbxState,
    void *pInforomBbxData
);

NvlStatus
nvswitch_bbx_add_sxid_lr10
(
    nvswitch_device *device,
    void *pInforomBbxState,
    void *pInforomBbxData
);

NvlStatus
nvswitch_bbx_add_temperature_lr10
(
    nvswitch_device *device,
    void *pInforomBbxState,
    void *pInforomBbxData
);

void
nvswitch_bbx_set_initial_temperature_lr10
(
    nvswitch_device *device,
    void *pInforomBbxState,
    void *pInforomBbxData
);

NvlStatus
nvswitch_inforom_bbx_get_sxid_lr10
(
    nvswitch_device *device,
    NVSWITCH_GET_SXIDS_PARAMS *params
);
#endif //_INFOROM_LR10_H_
