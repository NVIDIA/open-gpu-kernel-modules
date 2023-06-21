/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl0080/ctrl0080base.finn
//

#include "ctrl/ctrlxxxx.h"
/* NV01_DEVICE_XX/NV03_DEVICE control commands and parameters */

#define NV0080_CTRL_CMD(cat,idx)                NVXXXX_CTRL_CMD(0x0080, NV0080_CTRL_##cat, idx)

/* GPU device command categories (6bits) */
#define NV0080_CTRL_RESERVED                   (0x00)
#define NV0080_CTRL_BIF                        (0x01)
#define NV0080_CTRL_GPU                        (0x02)
#define NV0080_CTRL_CLK                        (0x10)
#define NV0080_CTRL_GR                         (0x11)
#define NV0080_CTRL_CIPHER                     (0x12)
#define NV0080_CTRL_FB                         (0x13)
#define NV0080_CTRL_HOST                       (0x14)
#define NV0080_CTRL_VIDEO                      (0x15)
#define NV0080_CTRL_FIFO                       (0x17)
#define NV0080_CTRL_DMA                        (0x18)
#define NV0080_CTRL_PERF                       (0x19)
#define NV0080_CTRL_PERF_LEGACY_NON_PRIVILEGED (0x99) /* finn: Evaluated from "(NV0080_CTRL_PERF | NVxxxx_CTRL_LEGACY_NON_PRIVILEGED)" */
#define NV0080_CTRL_MSENC                      (0x1B)
#define NV0080_CTRL_BSP                        (0x1C)
#define NV0080_CTRL_RC                         (0x1D)
#define NV0080_CTRL_OS_UNIX                    (0x1E)
#define NV0080_CTRL_NVJPG                      (0x1F)
#define NV0080_CTRL_INTERNAL                   (0x20)
#define NV0080_CTRL_NVLINK                     (0x21)

/*
 * NV0080_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0080_CTRL_CMD_NULL                   (0x800000) /* finn: Evaluated from "(FINN_NV01_DEVICE_0_RESERVED_INTERFACE_ID << 8) | 0x0" */

/* _ctrl0080base_h_ */

/* extract device cap setting from specified category-specific caps table */
#define NV0080_CTRL_GET_CAP(cat,tbl,c)    \
    NV0080_CTRL_##cat##_GET_CAP(tbl, NV0080_CTRL_##cat##_CAPS_##c)
