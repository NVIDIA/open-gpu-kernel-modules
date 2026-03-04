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

#ifndef _MINION_LS10_H_
#define _MINION_LS10_H_

#include "ls10.h"
#include "nvlink_inband_drv_header.h"

#define FALCON_IMEM_BLK_SIZE_BYTES_LS10              256

#define FALCON_CODE_HDR_OS_CODE_OFFSET_LS10          0
#define FALCON_CODE_HDR_OS_CODE_SIZE_LS10            1
#define FALCON_CODE_HDR_OS_DATA_OFFSET_LS10          2
#define FALCON_CODE_HDR_OS_DATA_SIZE_LS10            3
#define FALCON_CODE_HDR_NUM_APPS_LS10                4
#define FALCON_CODE_HDR_APP_CODE_START_LS10          5
#define FALCON_CODE_HDR_APP_DATA_START_LS10          ( FALCON_CODE_HDR_APP_CODE_START_LS10 + (FALCON_CODE_HDR_NUM_APPS_LS10 * 2))
#define FALCON_CODE_HDR_CODE_OFFSET_LS10             0
#define FALCON_CODE_HDR_CODE_SIZE_LS10               1
#define FALCON_CODE_HDR_DATA_OFFSET_LS10             0
#define FALCON_CODE_HDR_DATA_SIZE_LS10               1

#define NV_MINION_NVLINK_DL_STAT_ARGS_LANEID  15:12
#define NV_MINION_NVLINK_DL_STAT_ARGS_ADDRS   11:0

//
// Inband data structure
//
typedef struct inband_send_data
{
    // Inband buffer at sender Minion
    NvU8  *sendBuffer;

    // Number of bytes of data to be sent
    NvU32  bufferSize;

    // Header
    nvlink_inband_drv_hdr_t hdr;
} nvswitch_inband_send_data;

//
// Internal function declarations
//
NvlStatus nvswitch_minion_get_dl_status_ls10(nvswitch_device *device, NvU32 linkId, NvU32 statusIdx, NvU32 statusArgs, NvU32 *statusData);
NvlStatus nvswitch_set_minion_initialized_ls10(nvswitch_device *device, NvU32 idx_minion, NvBool initialized);
NvBool    nvswitch_is_minion_initialized_ls10(nvswitch_device *device, NvU32 idx_minion);
NvlStatus nvswitch_init_minion_ls10(nvswitch_device *device);
NvlStatus nvswitch_minion_send_command_ls10(nvswitch_device *device, NvU32 linkNumber, NvU32 command, NvU32 scratch0);
NvlStatus nvswitch_minion_riscv_get_physical_address_ls10(nvswitch_device *device,NvU32 idx_minion, NvU32 target, NvLength offset, NvU64 *pRiscvPa);
NvlStatus nvswitch_minion_set_sim_mode_ls10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_minion_set_smf_settings_ls10(nvswitch_device *device, nvlink_link *link);
NvlStatus nvswitch_minion_select_uphy_tables_ls10(nvswitch_device *device, nvlink_link *link);
NvBool    nvswitch_minion_is_riscv_active_ls10(nvswitch_device *device, NvU32 idx_minion);
NvlStatus nvswitch_minion_clear_dl_error_counters_ls10(nvswitch_device *device, NvU32 linkId);
NvlStatus nvswitch_minion_send_inband_data_ls10(nvswitch_device *device, NvU32 linkId, nvswitch_inband_send_data *inBandData);
void nvswitch_minion_receive_inband_data_ls10(nvswitch_device *device, NvU32 linkId);
NvlStatus nvswitch_minion_log_ali_debug_registers_ls10(nvswitch_device *device, nvlink_link *link);

#endif //_MINION_LS10_H_
