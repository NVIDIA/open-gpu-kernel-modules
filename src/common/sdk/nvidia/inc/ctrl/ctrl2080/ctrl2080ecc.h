/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl2080/ctrl2080ecc.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"



#define NV2080_CTRL_CMD_ECC_GET_CLIENT_EXPOSED_COUNTERS (0x20803400U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_ECC_INTERFACE_ID << 8) | NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS
 *
 * sramLastClearedTimestamp [out]
 * dramLastClearedTimestamp [out]
 *      unix-epoch based timestamp. These fields indicate when the error counters
 *      were last cleared by the user.
 *
 * sramErrorCounts [out]
 * dramErrorCounts [out]
 *      Aggregate error counts for SRAM and DRAM
 */

#define NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS_MESSAGE_ID (0x0U)

typedef struct NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS {
    NvU32 sramLastClearedTimestamp;
    NvU32 dramLastClearedTimestamp;

    NV_DECLARE_ALIGNED(NvU64 sramCorrectedTotalCounts, 8);
    NV_DECLARE_ALIGNED(NvU64 sramUncorrectedTotalCounts, 8);
    NV_DECLARE_ALIGNED(NvU64 dramCorrectedTotalCounts, 8);
    NV_DECLARE_ALIGNED(NvU64 dramUncorrectedTotalCounts, 8);
} NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS;
/* _ctrl2080ecc_h_ */
