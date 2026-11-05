/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _NVLINK_EVENT_DEFS_H_
#define _NVLINK_EVENT_DEFS_H_

#include "nvtypes.h"

typedef NvU16 NVLINK_OP_EVENT_CODE;
enum
{
    NVLINK_OP_EVENT_CODE_ALI_TRAINING_FAILURE 		 = 0x0001U,
    NVLINK_OP_EVENT_CODE_MSE_DEGRADED        		 = 0x0002U,
    NVLINK_OP_EVENT_CODE_MSE_WATCHDOG_TIMEOUT		 = 0x0003U,
    NVLINK_OP_EVENT_CODE_SW_LINK_DOWN        		 = 0x0004U,
    NVLINK_OP_EVENT_CODE_FATAL_ERROR_NVL3     		 = 0x0005U,
    NVLINK_OP_EVENT_CODE_PRIV_ERROR           		 = 0x0006U,
    NVLINK_OP_EVENT_CODE_SAW_ERROR_GPU_FATAL             = 0x0007U,
    NVLINK_OP_EVENT_CODE_SAW_ERROR_NON_FATAL_WARNING     = 0x0008U,
    NVLINK_OP_EVENT_CODE_SAW_ERROR_NON_FATAL_CONTAINED   = 0x0009U,
    NVLINK_OP_EVENT_CODE_RLW_ERROR_GPU_FATAL             = 0x000AU,
    NVLINK_OP_EVENT_CODE_RLW_ERROR_NON_FATAL_WARNING     = 0x000BU,
    NVLINK_OP_EVENT_CODE_RLW_ERROR_APPLICATION_FATAL     = 0x000CU,
    NVLINK_OP_EVENT_CODE_TLW_ERROR_GPU_FATAL             = 0x000DU,
    NVLINK_OP_EVENT_CODE_TLW_ERROR_NON_FATAL_WARNING     = 0x000EU,
    NVLINK_OP_EVENT_CODE_TLW_ERROR_NON_FATAL_CONTAINED   = 0x000FU,
    NVLINK_OP_EVENT_CODE_TREX_ERROR_NON_FATAL_WARNING    = 0x0010U,
    NVLINK_OP_EVENT_CODE_NVLPW_ERROR_NON_FATAL_WARNING   = 0x0011U,
    NVLINK_OP_EVENT_CODE_NETIR_ERROR_NON_FATAL_WARNING   = 0x0012U,
    NVLINK_OP_EVENT_CODE_NETIR_ERROR_GPU_FATAL           = 0x0013U,
    NVLINK_OP_EVENT_CODE_NETIR_ERROR_LINK_FATAL          = 0x0014U,
};

#endif // _NVLINK_EVENT_DEFS_H_
