/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208fbase.finn
//

#include "ctrl/ctrlxxxx.h"
/* NV20_SUBDEVICE_DIAG: diagnostic class control commands and parameters */

#define NV208F_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x208F, NV208F_CTRL_##cat, idx)

/* Subdevice diag command categories (6bits) */
#define NV208F_CTRL_RESERVED     (0x00)
#define NV208F_CTRL_POWER        (0x01)
#define NV208F_CTRL_THERMAL      (0x02)
#define NV208F_CTRL_SEQ          (0x03)
#define NV208F_CTRL_FIFO         (0x04)
#define NV208F_CTRL_FB           (0x05)
#define NV208F_CTRL_MC           (0x06)
#define NV208F_CTRL_BIF          (0x07)
#define NV208F_CTRL_CLK          (0x08)
#define NV208F_CTRL_PERF         (0x09)
#define NV208F_CTRL_FBIO         (0x0A)
#define NV208F_CTRL_MMU          (0x0B)
#define NV208F_CTRL_PMU          (0x0C)
#define NV208F_CTRL_EVENT        (0x10)
#define NV208F_CTRL_GPU          (0x11)
#define NV208F_CTRL_GR           (0x12)
#define NV208F_CTRL_PMGR         (0x13)
#define NV208F_CTRL_DMA          (0x14)
// const NV208F_CTRL_TMR = (0x15); // not supported
#define NV208F_CTRL_GSPMSGTIMING (0x16)
#define NV208F_CTRL_BUS          (0x17)

/*
 * NV208F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *     NV_OK
 */
#define NV208F_CTRL_CMD_NULL     (0x208f0000) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_RESERVED_INTERFACE_ID << 8) | 0x0" */

/* _ctrl208fbase_h_ */
