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

#pragma once

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl2080/ctrl2080ecc.finn
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

#define NV2080_CTRL_CMD_ECC_GET_ECI_COUNTERS (0x20803401U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_ECC_INTERFACE_ID << 8) | NV2080_CTRL_ECC_GET_ECI_COUNTERS_PARAMS_MESSAGE_ID" */

/*
 * NV2080_CTRL_ECC_GET_ECI_COUNTERS_PARAMS
 *
 * sramParityUncorrectedUnique  [out]
 * sramSecDedUncorrectedUnique  [out]
 * sramCorrectedTotal           [out]
 * dramUncorrectedTotal         [out]
 * dramCorrectedTotal           [out]
 *      Aggregate error counts for SRAM and DRAM.
 *
 * lastClearedTimestamp         [out]
 *      unix-epoch based timestamp. These fields indicate when the error counters
 *      were last cleared by the user.
 *
 * sramBucketL2                 [out]
 * sramBucketSM                 [out]
 * sramBucketPcie               [out]
 * sramBucketFirmware           [out]
 * sramBucketOther              [out]
 *      Aggregate unique uncorrctable error counts for SRAM buckets.
 *
 * sramErrorThresholdExceeded   [out]
 *      Boolean flag which is set if SRAM error threshold was exceeded
 */

#define NV2080_CTRL_ECC_GET_ECI_COUNTERS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_ECC_GET_ECI_COUNTERS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 sramParityUncorrectedUnique, 8);
    NV_DECLARE_ALIGNED(NvU64 sramSecDedUncorrectedUnique, 8);
    NV_DECLARE_ALIGNED(NvU64 sramCorrectedTotal, 8);
    NV_DECLARE_ALIGNED(NvU64 dramUncorrectedTotal, 8);
    NV_DECLARE_ALIGNED(NvU64 dramCorrectedTotal, 8);

    NvU32  lastClearedTimestamp;

    NV_DECLARE_ALIGNED(NvU64 sramBucketL2, 8);
    NV_DECLARE_ALIGNED(NvU64 sramBucketSM, 8);
    NV_DECLARE_ALIGNED(NvU64 sramBucketPcie, 8);
    NV_DECLARE_ALIGNED(NvU64 sramBucketFirmware, 8);
    NV_DECLARE_ALIGNED(NvU64 sramBucketOther, 8);

    NvBool sramErrorThresholdExceeded;
} NV2080_CTRL_ECC_GET_ECI_COUNTERS_PARAMS;

/*
 * NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS
 *
 * Reports count of volatile errors
 *
 * sramCorUni                   [out]:
 *      Unique correctable SRAM error count
 * sramUncParityUni             [out]:
 *      Unique uncorrectable SRAM parity error count
 * sramUncSecDedUni             [out]:
 *      Unique uncorrectable SRAM SEC-DED error count
 * dramCorTot                   [out]:
 *      Total correctable DRAM error count
 * dramUncTot                   [out]:
 *      total uncorrectable DRAM error count
 */
#define NV2080_CTRL_CMD_ECC_GET_VOLATILE_COUNTS (0x20803402U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_ECC_INTERFACE_ID << 8) | NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 sramCorUni, 8);
    NV_DECLARE_ALIGNED(NvU64 sramUncParityUni, 8);
    NV_DECLARE_ALIGNED(NvU64 sramUncSecDedUni, 8);
    NV_DECLARE_ALIGNED(NvU64 dramCorTot, 8);
    NV_DECLARE_ALIGNED(NvU64 dramUncTot, 8);
} NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS;

/* _ctrl2080ecc_h_ */
