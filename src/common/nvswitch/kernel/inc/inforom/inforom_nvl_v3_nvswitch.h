/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 
#ifndef _INFOROM_NVL_V3_NVSWITCH_H_
#define _INFOROM_NVL_V3_NVSWITCH_H_

#include "inforom/inforom_nvswitch.h"

#define LUT_ELEMENT(block, dir, subtype, type, sev)                                 \
    { INFOROM_NVL_ERROR_TYPE ## type,                                               \
      FLD_SET_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _SEVERITY,  sev, 0) |    \
          FLD_SET_DRF(_INFOROM_NVL_OBJECT_V3, _ERROR_METADATA, _DIRECTION, dir, 0), \
      block ## dir ## subtype ## type,                                              \
      INFOROM_NVL_ERROR_BLOCK_TYPE_ ## block                                        \
    }

NvlStatus inforom_nvl_v3_map_error
(
    INFOROM_NVLINK_ERROR_TYPES error,
    NvU8  *pHeader,
    NvU16 *pMetadata,
    NvU8  *pErrorSubtype,
    INFOROM_NVL_ERROR_BLOCK_TYPE *pBlockType
);

NvlStatus
inforom_nvl_v3_encode_nvlipt_error_subtype
(
    NvU8 localLinkIdx,
    NvU8 *pSubtype
);

NvBool
inforom_nvl_v3_should_replace_error_rate_entry
(
    INFOROM_NVL_OBJECT_V3_CORRECTABLE_ERROR_RATE *pErrorRate,
    NvU32  flitCrcRate,
    NvU32 *pLaneCrcRates
);

void
inforom_nvl_v3_seconds_to_day_and_month
(
    NvU32 sec,
    NvU32 *pDay,
    NvU32 *pMonth
);

void
inforom_nvl_v3_update_error_rate_entry
(
    INFOROM_NVL_OBJECT_V3_CORRECTABLE_ERROR_RATE *pErrorRate,
    NvU32 newSec,
    NvU32 newFlitCrcRate,
    NvU32 *pNewLaneCrcRates
);

NvlStatus 
inforom_nvl_v3_map_error_to_userspace_error
(
    nvswitch_device *device,
    INFOROM_NVL_OBJECT_V3_ERROR_ENTRY *pErrorLog,
    NVSWITCH_NVLINK_ERROR_ENTRY *pNvlError
);

void
inforom_nvl_v3_update_correctable_error_rates
(
    INFOROM_NVL_CORRECTABLE_ERROR_RATE_STATE_V3S *pState,
    NvU8 link,
    INFOROM_NVLINK_CORRECTABLE_ERROR_COUNTS *pCounts
);

#endif //_INFOROM_NVL_V3_NVSWITCH_H_
