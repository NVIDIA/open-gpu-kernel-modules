/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES
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

#ifndef FSP_NVDM_FORMAT_H
#define FSP_NVDM_FORMAT_H

#include "fsp/fsp_mctp_format.h"
#include "fsp/fsp_emem_channels.h"

//
// NVDM (NVIDIA Data Model) overlayed on MCTP (Management Component Transport 
// Protocol) and sent over EMEM is the communication mechanism used between FSP 
// management partition and CPU-RM/other uprocs.
//

#define NVDM_TYPE_RESET                 0x4
#define NVDM_TYPE_HULK                  0x11
#define NVDM_TYPE_FIRMWARE_UPDATE       0x12
#define NVDM_TYPE_PRC                   0x13
#define NVDM_TYPE_COT                   0x14
#define NVDM_TYPE_FSP_RESPONSE          0x15
#define NVDM_TYPE_CAPS_QUERY            0x16
#define NVDM_TYPE_INFOROM               0x17
#define NVDM_TYPE_SMBPBI                0x18
#define NVDM_TYPE_ROMREAD               0x1A
#define NVDM_TYPE_UEFI_RM               0x1C
#define NVDM_TYPE_UEFI_XTL_DEBUG_INTR   0x1D
#define NVDM_TYPE_TNVL                  0x1F
#define NVDM_TYPE_CLOCK_BOOST           0x20
#define NVDM_TYPE_FSP_GSP_COMM          0x21

#endif // FSP_NVDM_FORMAT_H
