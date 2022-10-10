/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl0000/ctrl0000base.finn
//

#include "ctrl/ctrlxxxx.h"
/* NV01_ROOT (client) control commands and parameters */

#define NV0000_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x0000,NV0000_CTRL_##cat,idx)

/* Client command categories (6bits) */
#define NV0000_CTRL_RESERVED       (0x00)
#define NV0000_CTRL_SYSTEM         (0x01)
#define NV0000_CTRL_GPU            (0x02)
#define NV0000_CTRL_GSYNC          (0x03)
#define NV0000_CTRL_DIAG           (0x04)
#define NV0000_CTRL_EVENT          (0x05)
#define NV0000_CTRL_NVD            (0x06)
#define NV0000_CTRL_SWINSTR        (0x07)
#define NV0000_CTRL_GSPC           (0x08)
#define NV0000_CTRL_PROC           (0x09)
#define NV0000_CTRL_SYNC_GPU_BOOST (0x0A)
#define NV0000_CTRL_GPUACCT        (0x0B)
#define NV0000_CTRL_VGPU           (0x0C)
#define NV0000_CTRL_CLIENT         (0x0D)

// per-OS categories start at highest category and work backwards
#define NV0000_CTRL_OS_WINDOWS     (0x3F)
#define NV0000_CTRL_OS_MACOS       (0x3E)
#define NV0000_CTRL_OS_UNIX        (0x3D)


/*
 * NV0000_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV0000_CTRL_CMD_NULL       (0x0) /* finn: Evaluated from "(FINN_NV01_ROOT_RESERVED_INTERFACE_ID << 8) | 0x0" */

/* _ctrl0000_base_h_ */
