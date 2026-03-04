/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the Software),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _GFW_LS10_H_
#define _GFW_LS10_H_



#include "nvswitch/ls10/dev_nvlsaw_ip.h"
#include "nvswitch/ls10/dev_therm.h"
#include "nvswitch/ls10/dev_fsp_pri.h"

//
// Transcribed from GFW ucode r5 v1 scratch definition for LS10
//

#define NV_NVLSAW_SW_SCRATCH(_index)             (NV_NVLSAW_SW_SCRATCH_0 + (_index)*4)
#define NV_NVLSAW_SW_BIOS_VERSION                 NV_NVLSAW_SW_SCRATCH(6)
#define NV_NVLSAW_SW_OEM_BIOS_VERSION             NV_NVLSAW_SW_SCRATCH(7)

//-----------------------------------------------------------------------------
#define NV_GFW_GLOBAL_BOOT_PARTITION_PROGRESS       NV_THERM_I2CS_SCRATCH
#define NV_GFW_GLOBAL_BOOT_PARTITION_PROGRESS_VALUE 7:0
#define NV_GFW_GLOBAL_BOOT_PARTITION_PROGRESS_VALUE_SUCCESS 0x000000FF

#define NV_NVLSAW_SW_OEM_BIOS_VERSION_BOARD_ID 31:16             

#endif //_GFW_LS10_H_

