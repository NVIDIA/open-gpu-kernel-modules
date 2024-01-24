/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _CCI_CDB_ASYNC_NVSWITCH_H_
#define _CCI_CDB_ASYNC_NVSWITCH_H_

#include "ls10/cci_ls10.h"

#include "nvlink_errors.h"
#include "nvtypes.h"

typedef enum cci_cdb_phase
{
    CCI_CDB_PHASE_IDLE = 0x0,
    CCI_CDB_PHASE_CHECK_READY,
    CCI_CDB_PHASE_SEND_COMMAND,
    CCI_CDB_PHASE_GET_RESPONSE,
    CCI_CDB_PHASE_CHECK_DONE
} CCI_CDB_PHASE;

typedef struct cci_cdb_state
{
    CCI_CDB_PHASE cdbPhase;
    NvU8   client;
    NvU8   laneMasksPending[NVSWITCH_CCI_NUM_LINKS_PER_OSFP_LS10]; 
    NvBool freeze_maintenance;
    NvBool restart_training;
    NvBool nvlink_mode;
} CCI_CDB_STATE;

// Should be called at some regular polling rate
void cciProcessCDBCallback(nvswitch_device *device);

NvlStatus cciConfigureNvlinkModeAsync(nvswitch_device *device, NvU32 client, NvU8 linkId, NvBool freeze_maintenance, NvBool restart_training, NvBool nvlink_mode);
#endif //_CCI_CDB_ASYNC_NVSWITCH_H_
