/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef NVLINK_INBAND_DRV_HDR_H
#define NVLINK_INBAND_DRV_HDR_H

/*
 * This header file defines the header that should be used by RM and NVSwitch
 * driver to sync minions on both the sides before an actual inband message
 * transfer is initiated.
 *
 * Modifying the existing header structure is not allowed. A versioning
 * policy must be enforced if such changes are needed in the future.
 *
 * - Avoid use of enums or bit fields. Always use fixed types.
 * - Avoid conditional fields in the structs
 * - Avoid nested and complex structs. Keep them simple and flat for ease of
 *   encoding and decoding.
 * - Avoid embedded pointers. Flexible arrays at the end of the struct are allowed.
 * - Always use the packed struct to typecast inband messages. More details: 
 * - Always have reserved flags or fields to CYA given the stable ABI conditions.
 */

/* Align to byte boundaries */
#pragma pack(push, 1)

#include "nvtypes.h"

#define NVLINK_INBAND_MAX_XFER_SIZE                      0x100
#define NVLINK_INBAND_MAX_XFER_AT_ONCE                       4

#define NVLINK_INBAND_DRV_HDR_TYPE_START          NVBIT(0)
#define NVLINK_INBAND_DRV_HDR_TYPE_MID            NVBIT(1)
#define NVLINK_INBAND_DRV_HDR_TYPE_END            NVBIT(2)

/* Rest of the bits are reserved for future use and must be always set zero. */

typedef struct
{
    NvU8  data;
} nvlink_inband_drv_hdr_t;

#pragma pack(pop)

/* Don't add any code after this line */

#endif
