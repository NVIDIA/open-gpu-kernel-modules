/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _INFOROM_LS10_H_
#define _INFOROM_LS10_H_

NvlStatus nvswitch_inforom_nvl_log_error_event_ls10
(
    nvswitch_device            *device,
    void                       *pNvlGeneric,
    void                       *pNvlErrorEvent,
    NvBool                     *bDirty
);

NvlStatus nvswitch_inforom_nvl_update_link_correctable_error_info_ls10
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
nvswitch_inforom_nvl_get_max_correctable_error_rate_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_MAX_CORRECTABLE_ERROR_RATES_PARAMS *params
);

NvlStatus
nvswitch_inforom_nvl_get_errors_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_NVLINK_ERROR_COUNTS_PARAMS *params
);

NvlStatus nvswitch_inforom_nvl_setL1Threshold_ls10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    NvU32 word1,
    NvU32 word2
);

NvlStatus nvswitch_inforom_nvl_getL1Threshold_ls10
(
    nvswitch_device *device,
    void *pNvlGeneric,
    NvU32 *word1,
    NvU32 *word2
);

NvlStatus nvswitch_inforom_nvl_setup_nvlink_state_ls10
(
    nvswitch_device *device,
    INFOROM_NVLINK_STATE *pNvlinkState,
    NvU8 version
);

void
nvswitch_initialize_oms_state_ls10
(
    nvswitch_device *device,
    INFOROM_OMS_STATE *pOmsState
);

NvBool
nvswitch_oms_get_device_disable_ls10
(
    INFOROM_OMS_STATE *pOmsState
);

void
nvswitch_oms_set_device_disable_ls10
(
    INFOROM_OMS_STATE *pOmsState,
    NvBool bForceDeviceDisable
);

NvlStatus
nvswitch_oms_inforom_flush_ls10
(
    struct nvswitch_device *device
);

void
nvswitch_inforom_ecc_get_total_errors_ls10
(
    nvswitch_device     *device,
    INFOROM_ECC_OBJECT  *pEccGeneric,
    NvU64               *pCorrectedTotal,
    NvU64               *pUncorrectedTotal
);

NvlStatus
nvswitch_inforom_load_obd_ls10
(
    nvswitch_device *device
);

NvlStatus
nvswitch_bbx_add_sxid_ls10
(
    nvswitch_device *device,
    NvU32 exceptionType,
    NvU32 data0,
    NvU32 data1,
    NvU32 data2
);

NvlStatus
nvswitch_bbx_unload_ls10
(
    nvswitch_device *device
);

NvlStatus
nvswitch_bbx_load_ls10
(
    nvswitch_device *device,
    NvU64 time_ns,
    NvU8 osType,
    NvU32 osVersion
);

NvlStatus
nvswitch_bbx_get_sxid_ls10
(
    nvswitch_device *device,
    NVSWITCH_GET_SXIDS_PARAMS * params
);

NvlStatus
nvswitch_bbx_get_data_ls10
(
    nvswitch_device *device,
    NvU8 dataType,
    void *params
);
#endif //_INFOROM_LS10_H_
