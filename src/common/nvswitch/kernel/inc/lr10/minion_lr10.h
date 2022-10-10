/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _MINION_LR10_H_
#define _MINION_LR10_H_

#include "lr10.h"

// TODO modify these for LR10
#define FALCON_IMEM_BLK_SIZE_BYTES_LR10              256

#define FALCON_CODE_HDR_OS_CODE_OFFSET_LR10          0
#define FALCON_CODE_HDR_OS_CODE_SIZE_LR10            1
#define FALCON_CODE_HDR_OS_DATA_OFFSET_LR10          2
#define FALCON_CODE_HDR_OS_DATA_SIZE_LR10            3
#define FALCON_CODE_HDR_NUM_APPS_LR10                4
#define FALCON_CODE_HDR_APP_CODE_START_LR10          5
#define FALCON_CODE_HDR_APP_DATA_START_LR10          ( FALCON_CODE_HDR_APP_CODE_START_LR10 + (FALCON_CODE_HDR_NUM_APPS_LR10 * 2))
#define FALCON_CODE_HDR_CODE_OFFSET_LR10             0
#define FALCON_CODE_HDR_CODE_SIZE_LR10               1
#define FALCON_CODE_HDR_DATA_OFFSET_LR10             0
#define FALCON_CODE_HDR_DATA_SIZE_LR10               1

#define NV_MINION_NVLINK_DL_STAT_ARGS_LANEID  15:12
#define NV_MINION_NVLINK_DL_STAT_ARGS_ADDRS   11:0

typedef const struct
{
    NvU32 osCodeOffset;
    NvU32 osCodeSize;
    NvU32 osDataOffset;
    NvU32 osDataSize;
    NvU32 numApps;
    NvU32 appCodeStart;
    NvU32 appDataStart;
    NvU32 codeOffset;
    NvU32 codeSize;
    NvU32 dataOffset;
    NvU32 dataSize;
} FALCON_UCODE_HDR_INFO_LR10, *PFALCON_UCODE_HDR_INFO_LR10;

#define NVSWITCH_MINION_LINK_RD32_LR10(_d, _physlinknum, _dev, _reg) \
    NVSWITCH_LINK_RD32_LR10(_d, _physlinknum, MINION, _dev, _reg)

#define NVSWITCH_MINION_LINK_WR32_LR10(_d, _physlinknum, _dev, _reg, _data) \
    NVSWITCH_LINK_WR32_LR10(_d, _physlinknum, MINION, _dev, _reg, _data)

#define NVSWITCH_MINION_WR32_LR10(_d, _instance, _dev, _reg, _data)         \
       NVSWITCH_ENG_WR32_LR10(_d, MINION, , _instance, _dev, _reg, _data)

#define NVSWITCH_MINION_RD32_LR10(_d, _instance, _dev, _reg)                \
    NVSWITCH_ENG_RD32_LR10(_d, MINION, _instance, _dev, _reg)

#define NVSWITCH_MINION_WR32_BCAST_LR10(_d, _dev, _reg, _data)              \
    NVSWITCH_BCAST_WR32_LR10(_d, MINION, _dev, _reg, _data)

#define NVSWITCH_MINION_GET_LOCAL_LINK_ID(_physlinknum) \
    (_physlinknum%NVSWITCH_LINKS_PER_MINION)

//
// Internal function declarations
//
NvlStatus nvswitch_init_minion_lr10(nvswitch_device *device);
NvlStatus nvswitch_minion_send_command_lr10(nvswitch_device *device, NvU32 linkNumber, NvU32 command, NvU32 scratch0);
NvlStatus nvswitch_minion_get_dl_status_lr10(nvswitch_device *device, NvU32 linkId, NvU32 statusIdx, NvU32 statusArgs, NvU32 *statusData);
NvlStatus nvswitch_minion_get_initoptimize_status_lr10(nvswitch_device *device, NvU32 linkId);
NvlStatus nvswitch_minion_get_initnegotiate_status_lr10(nvswitch_device *device, NvU32 linkId);
NvlStatus nvswitch_minion_get_rxdet_status_lr10(nvswitch_device *device, NvU32 linkId);
NvlStatus nvswitch_minion_set_rx_term_lr10(nvswitch_device *device, NvU32 linkId);
NvU32     nvswitch_minion_get_line_rate_Mbps_lr10(nvswitch_device *device, NvU32 linkId);
NvU32     nvswitch_minion_get_data_rate_KiBps_lr10(nvswitch_device *device, NvU32 linkId);
NvlStatus nvswitch_set_minion_initialized_lr10(nvswitch_device *device, NvU32 idx_minion, NvBool initialized);
NvBool    nvswitch_is_minion_initialized_lr10(nvswitch_device *device, NvU32 idx_minion);
NvlStatus nvswitch_minion_clear_dl_error_counters_lr10(nvswitch_device *device, NvU32 linkId);

#endif //_MINION_LR10_H_
