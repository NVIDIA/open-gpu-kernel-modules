/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * sramParityUncorrectedUnique  [out]
 * sramSecDedUncorrectedUnique  [out]
 * sramCorrectedUnique          [out]
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

#define NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS_MESSAGE_ID (0x0U)

typedef struct NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 sramParityUncorrectedUnique, 8);
    NV_DECLARE_ALIGNED(NvU64 sramSecDedUncorrectedUnique, 8);
    NV_DECLARE_ALIGNED(NvU64 sramCorrectedUnique, 8);
    NV_DECLARE_ALIGNED(NvU64 dramUncorrectedTotal, 8);
    NV_DECLARE_ALIGNED(NvU64 dramCorrectedTotal, 8);

    NvU32  lastClearedTimestamp;

    NV_DECLARE_ALIGNED(NvU64 sramBucketL2, 8);
    NV_DECLARE_ALIGNED(NvU64 sramBucketSM, 8);
    NV_DECLARE_ALIGNED(NvU64 sramBucketPcie, 8);
    NV_DECLARE_ALIGNED(NvU64 sramBucketFirmware, 8);
    NV_DECLARE_ALIGNED(NvU64 sramBucketOther, 8);

    NvBool sramErrorThresholdExceeded;
} NV2080_CTRL_ECC_GET_CLIENT_EXPOSED_COUNTERS_PARAMS;

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
#define NV2080_CTRL_CMD_ECC_GET_VOLATILE_COUNTS (0x20803401U) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_ECC_INTERFACE_ID << 8) | NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 sramCorUni, 8);
    NV_DECLARE_ALIGNED(NvU64 sramUncParityUni, 8);
    NV_DECLARE_ALIGNED(NvU64 sramUncSecDedUni, 8);
    NV_DECLARE_ALIGNED(NvU64 dramCorTot, 8);
    NV_DECLARE_ALIGNED(NvU64 dramUncTot, 8);
} NV2080_CTRL_ECC_GET_VOLATILE_COUNTS_PARAMS;

typedef struct eccLocation {
    NvU32 location;
    NvU32 sublocation;
    NvU32 extlocation;
} eccLocation;

/*
 * NV2080_CTRL_ECC_SRAM_UNIQUE_UNCORR_COUNTS_ENTRY
 *
 * unit
 *   The unit the error occurred in
 * location
 *   The location info for the error
 * address
 *   The address of the error
 * bIsParity
 *   True if error is parity error, false if error is SEC-DED error
 * count
 *   The number of uncorrectable unique error counts that occurred
 */

typedef struct NV2080_CTRL_ECC_SRAM_UNIQUE_UNCORR_COUNTS_ENTRY {
    NvU32       unit;
    eccLocation location;
    NvU32       address;
    NvBool      bIsParity;
    NvU32       count;
} NV2080_CTRL_ECC_SRAM_UNIQUE_UNCORR_COUNTS_ENTRY;

/*
 * NV2080_CTRL_CMD_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS
 *
 * This command is used to query the ECC inforom object and determine the number
 * of unique uncorrectable error counts that occurred at an address.
 *
 *   entryCount
 *     The number of entries
 *
 *   entries
 *     The array of NV2080_CTRL_ECC_SRAM_UNIQUE_UNCORR_COUNTS_ENTRY
 */

#define NV2080_CTRL_ECC_SRAM_UNIQUE_UNCORR_COUNTS_MAX_COUNT 600

#define NV2080_CTRL_CMD_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS   (0x20803402) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_ECC_INTERFACE_ID << 8) | NV2080_CTRL_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS_PARAMS {
    NvU32                                           entryCount;
    NV2080_CTRL_ECC_SRAM_UNIQUE_UNCORR_COUNTS_ENTRY entries[NV2080_CTRL_ECC_SRAM_UNIQUE_UNCORR_COUNTS_MAX_COUNT];
} NV2080_CTRL_ECC_GET_SRAM_UNIQUE_UNCORR_COUNTS_PARAMS;

#define NV2080_CTRL_ECC_ERROR_TYPE_CORRECTED    0
#define NV2080_CTRL_ECC_ERROR_TYPE_UNCORRECTED  1

/*
 * NV2080_CTRL_CMD_ECC_INJECT_ERROR
 *
 * This command is used to inject ECC errors.
 *
 *   unit
 *     The ECC unit
 *
 *   errorType
 *     The type of error to be injected
 *
 *   location
 *     The location within the ECC unit to be injected
 *
 *   flags
 *     Specific injection flags
 *
 *   address
 *     Specific injection address for DRAM
 */

#define NV2080_CTRL_CMD_ECC_INJECT_ERROR (0x20803403) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_ECC_INTERFACE_ID << 8) | NV2080_CTRL_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_ECC_INJECT_ERROR_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_ECC_INJECT_ERROR_PARAMS {
    NvU32       unit;
    NvU8        errorType;
    eccLocation location;
    NvU32       flags;
    NV_DECLARE_ALIGNED(NvU64 address, 8);
} NV2080_CTRL_ECC_INJECT_ERROR_PARAMS;

/*
 * NV2080_CTRL_CMD_ECC_GET_REPAIR_STATUS
 *
 * This command is used to query the status of TPC/Channel repair
 *
 * bTpcRepairPending
 *   Boolean indicating if TPC repair is pending
 * bChannelRepairPending
 *   Boolean indicating if Channel repair is pending
 */

#define NV2080_CTRL_CMD_ECC_GET_REPAIR_STATUS (0x20803404) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_ECC_INTERFACE_ID << 8) | NV2080_CTRL_ECC_GET_REPAIR_STATUS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_ECC_GET_REPAIR_STATUS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_ECC_GET_REPAIR_STATUS_PARAMS {
    NvBool bTpcRepairPending;
    NvBool bChannelRepairPending;
} NV2080_CTRL_ECC_GET_REPAIR_STATUS_PARAMS;

/* _ctrl2080ecc_h_ */
