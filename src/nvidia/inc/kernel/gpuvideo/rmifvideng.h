/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef RMIFVIDENG_H
#define RMIFVIDENG_H

/*!
 * @file   rmifvideng.h
 * @brief  RISC-V boot arguments / interface
 */

#include <nvtypes.h>

/*!
 * Video engine's frame-buffer interface block has several slots/indices which can
 * be bound to support DMA to various surfaces in memory. This is an
 * enumeration that gives name to each index based on type of memory-aperture
 * the index is used to access.
 *
 * Traditionally, video falcons have used the 6th index for ucode, and we will
 * continue to use that to allow legacy ucode to work seamlessly.
 *
 * Note: DO NOT CHANGE THE VALUE OF RM_VIDENG_DMAIDX_UCODE. That value is used by
 * both the legacy video ucode, which assumes that it will use index 6, and by
 * msdecos. Changing it will break legacy video ucode, unless it is updated to
 * reflect the new value.
 */
typedef enum _RM_VIDENG_DMAIDX_TYPE
{
    RM_VIDENG_DMAIDX_RSVD0         = 0,
    RM_VIDENG_DMAIDX_VIRT          = 1,
    RM_VIDENG_DMAIDX_PHYS_VID      = 2,
    RM_VIDENG_DMAIDX_PHYS_SYS_COH  = 3,
    RM_VIDENG_DMAIDX_PHYS_SYS_NCOH = 4,
    RM_VIDENG_DMAIDX_RSVD1         = 5,
    RM_VIDENG_DMAIDX_UCODE         = 6,
    RM_VIDENG_DMAIDX_END           = 7
} RM_VIDENG_DMAIDX_TYPE;

/*!
 * Configuration for riscv msdecos boot params
 */
#define NV_VIDENG_BOOT_PARAMS_VERSION                                       2

#define NV_VIDENG_BOOT_PARAMS_MEM_ADDR_LO                                31:0
#define NV_VIDENG_BOOT_PARAMS_MEM_ADDR_HI                                27:0
#define NV_VIDENG_BOOT_PARAMS_MEM_DMA_IDX                               31:28

#define NV_VIDENG_TRACESURF_PARAMS_SIZE                                  27:0
#define NV_VIDENG_TRACESURF_PARAMS_DMA_IDX                              31:28

typedef struct
{
    /*
     *                   *** WARNING ***
     * First 3 fields must be frozen like that always. Should never
     * be reordered or changed.
     */
    NvU8   version;        // version of boot params
    NvU8   rsvd;           // reserved byte
    NvU16  size;           // size of boot params
    /*
     * You can reorder or change below this point but update version.
     * Make sure to align it to 16B as ucode expect 16byte alignment to DMA efficiently.
     */
    NvU32  videoPgPmuHandshake;    // Handshake between PMU and Video Ucode for SW controlled IDLE signal.
    NvU64  rsvd2;                  // reserved field
} NV_VIDENG_BOOT_PARAMS, *PNV_VIDENG_BOOT_PARAMS;

#endif // RMIFVIDENG_H
