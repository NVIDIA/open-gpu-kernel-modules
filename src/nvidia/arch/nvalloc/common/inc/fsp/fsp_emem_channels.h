/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES
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

#ifndef _FSP_EMEM_CHANNELS_H_
#define _FSP_EMEM_CHANNELS_H_

//
// NVDM (NVIDIA Data Model) overlayed on MCTP (Management Component Transport 
// Protocol) and sent over EMEM is the communication mechanism used between FSP 
// management partition and CPU-RM/other uprocs.
//

// RM uses channel 0 for FSP EMEM.
#define FSP_EMEM_CHANNEL_RM             0x0

// PMU/SOE use channel 4 for FSP EMEM.
#define FSP_EMEM_CHANNEL_PMU_SOE        0x4
#define FSP_EMEM_CHANNEL_MAX            0x8

// EMEM channel 0 (RM) is allocated 1K bytes.
#define FSP_EMEM_CHANNEL_RM_SIZE        1024

// EMEM channel 4 (PMU/SOE) is allocated 1K bytes.
#define FSP_EMEM_CHANNEL_PMU_SOE_SIZE   1024
#define FSP_EMEM_CHANNEL_PMU_SOE_OFFSET 4096

#endif // _FSP_EMEM_CHANNELS_H_
