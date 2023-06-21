/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl9067.finn
//

#include "ctrl/ctrlxxxx.h"
/* Subcontext control commands and parameters */
#define NV9067_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x9067, NV9067_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NV9067_CTRL_RESERVED      (0x00)
#define NV9067_CTRL_TPC_PARTITION (0x01)
#define NV9067_CTRL_CWD_WATERMARK (0x02)

/*!
 * Does nothing.
 */
#define NV9067_CTRL_CMD_NULL      (0x90670000) /* finn: Evaluated from "(FINN_FERMI_CONTEXT_SHARE_A_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*!
 * NV9067_CTRL_CMD_GET_TPC_PARTITION_TABLE
 *    This command gets the current partition table configuration of a subcontext
 *
 * NV9067_CTRL_CMD_SET_TPC_PARTITION_TABLE
 *    This command sets the partition table of a subcontext
 *
 * NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS
 *     This structure defines the parameters used for SET/GET per-subcontext TPC partitioning table configuration
 *
 *       numUsedTpc [in/out]
 *           Specifies the number of TPCs used by the subcontext
 *           While querying the enabled TPCs, this is an output paramter
 *           While configuring the TPCs, this is an input parameter
 *
 *       tpcList [in/out]
 *           Array containing the TPCs enabled for the subcontext.
 *           The first numUsedTpc in the array interpreted as the valid entries.
 *
 *           Only applicable for STATIC and DYNAMIC modes.
 *
 * NV9067_CTRL_TPC_PARTITION_TABLE_MAX_TPC_COUNT
 *     Max TPC count supported by this ctrl call
 *     
 * NV9067_CTRL_TPC_PARTITION_TABLE_TPC_INFO
 *   This structure defines the parameters for a TPC
 *
 *       globalTpcIndex
 *          Global logical index of the enabled TPC
 *
 *       lmemBlockIndex
 *          Block index of the Local memory backing store for the enabled TPC.
 *          For GET command, we will return the current lmem block assigment for STATIC & DYNAMIC modes.
 *          For SET command, this index is relevant only for STATIC mode.
 *          HW automatically assign it for other modes. So should be zeroed out for other modes.
 *
 */
#define NV9067_CTRL_CMD_GET_TPC_PARTITION_TABLE       (0x90670101) /* finn: Evaluated from "(FINN_FERMI_CONTEXT_SHARE_A_TPC_PARTITION_INTERFACE_ID << 8) | 0x1" */

#define NV9067_CTRL_CMD_SET_TPC_PARTITION_TABLE       (0x90670102) /* finn: Evaluated from "(FINN_FERMI_CONTEXT_SHARE_A_TPC_PARTITION_INTERFACE_ID << 8) | NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS_MESSAGE_ID" */

#define NV9067_CTRL_TPC_PARTITION_TABLE_TPC_COUNT_MAX 256

typedef struct NV9067_CTRL_TPC_PARTITION_TABLE_TPC_INFO {
    NvU16 globalTpcIndex;
    NvU16 lmemBlockIndex;
} NV9067_CTRL_TPC_PARTITION_TABLE_TPC_INFO;

#define NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS {
    NvU16                                    numUsedTpc;
    NV9067_CTRL_TPC_PARTITION_TABLE_TPC_INFO tpcList[NV9067_CTRL_TPC_PARTITION_TABLE_TPC_COUNT_MAX];
} NV9067_CTRL_TPC_PARTITION_TABLE_PARAMS;


/*!
 * NV9067_CTRL_CMD_GET_CWD_WATERMARK
 *    This command gets the cached watermark value for a subcontext
 *
 * NV9067_CTRL_CMD_SET_CWD_WATERMARK
 *    This command sets the watermark value for a subcontexts
 *
 * NV9067_CTRL_CWD_WATERMARK_PARAMS
 *     This structure defines the parameters used to SET/GET watermark value per-subcontext.
 *
 *        watermarkValue [in/out]
 *            Value of watermark per-subcontext
 *            Acts as a output parameter to get the current value of watermark for a subcontext.
 *            Acts as a input parameter to set the current value of watermark for a subcontexts.
 *
 * NV9067_CTRL_CWD_WATERMARK_VALUE_MIN
 *     Minimum value of watermark for any subcontext
 *     RM will throw an error if any value less than this value is specified
 *
 * NV9067_CTRL_CWD_WATERMARK_VALUE_DEFAULT
 *     Default value of watermark for any subcontext
 *     RM will set watermark of a subcontext to this value when the subcontext
 *     is created/initialized for the first time
 *
 * NV9067_CTRL_CWD_WATERMARK_VALUE_MAX
 *     Maximum value of watermark for any subcontext
 *     RM will throw an error if any value more than this value is specified
 *
 */

#define NV9067_CTRL_CMD_GET_CWD_WATERMARK       (0x90670201) /* finn: Evaluated from "(FINN_FERMI_CONTEXT_SHARE_A_CWD_WATERMARK_INTERFACE_ID << 8) | NV9067_CTRL_GET_CWD_WATERMARK_PARAMS_MESSAGE_ID" */

#define NV9067_CTRL_CWD_WATERMARK_VALUE_MIN     1
#define NV9067_CTRL_CWD_WATERMARK_VALUE_DEFAULT 2
#define NV9067_CTRL_CWD_WATERMARK_VALUE_MAX     256

typedef struct NV9067_CTRL_CWD_WATERMARK_PARAMS {
    NvU32 watermarkValue;
} NV9067_CTRL_CWD_WATERMARK_PARAMS;

#define NV9067_CTRL_GET_CWD_WATERMARK_PARAMS_MESSAGE_ID (0x1U)

typedef NV9067_CTRL_CWD_WATERMARK_PARAMS NV9067_CTRL_GET_CWD_WATERMARK_PARAMS;

#define NV9067_CTRL_CMD_SET_CWD_WATERMARK (0x90670202) /* finn: Evaluated from "(FINN_FERMI_CONTEXT_SHARE_A_CWD_WATERMARK_INTERFACE_ID << 8) | NV9067_CTRL_SET_CWD_WATERMARK_PARAMS_MESSAGE_ID" */

#define NV9067_CTRL_SET_CWD_WATERMARK_PARAMS_MESSAGE_ID (0x2U)

typedef NV9067_CTRL_CWD_WATERMARK_PARAMS NV9067_CTRL_SET_CWD_WATERMARK_PARAMS;

/* _ctrl9067_h_ */
