/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef KERNEL_CE_GV100_PRIVATE_H
#define KERNEL_CE_GV100_PRIVATE_H

#define MAX_CE_CNT 15

/*
 * sysmemLinks
 *    Represents the number of sysmem links detected
 *    This affects how many PCEs LCE0(sysmem read CE)
 *    and LCE1(sysmem write CE) should be mapped to
 * maxLinksPerPeer
 *    Represents the maximum number of peer links
 *    between this GPU and all its peers. This affects
 *    how many PCEs LCE3(P2P CE) should be mapped to
 * numPeers
 *    Represents the number of Peer GPUs discovered so far
 * bSymmetric
 *    Represents whether the topology detected so far
 *    is symmetric i.e. has same number of links to all
 *    peers connected through nvlink. This affects how
 *    many PCEs to assign to LCEs3-5 (nvlink P2P CEs)
 * bSwitchConfig
 *    Represents whether the config listed is intended
 *    for use with nvswitch systems
 * pceLceMap
 *    Value of NV_CE_PCE2LCE_CONFIG0 register with the
 *    above values for sysmemLinks, maxLinksPerPeer,
 *    numLinks and bSymmetric
 * grceConfig
 *    Value of NV_CE_GRCE_CONFIG register with the
 *    above values for sysmemLinks, maxLinksPerPeer,
 *    numLinks and bSymmetric
 * exposeCeMask
 *    Mask of CEs to expose to clients for the above
 *    above values for sysmemLinks, maxLinksPerPeer,
 *    numLinks and bSymmetric
 */
typedef struct NVLINK_CE_AUTO_CONFIG_TABLE
{
    NvU32  sysmemLinks;
    NvU32  maxLinksPerPeer;
    NvU32  numPeers;
    NvBool bSymmetric;
    NvBool bSwitchConfig;
    NvU32  pceLceMap[MAX_CE_CNT];
    NvU32  grceConfig[MAX_CE_CNT];
    NvU32  exposeCeMask;
} NVLINK_CE_AUTO_CONFIG_TABLE;

//
// General convention decided on between HW and SW:
//  - CE2 is for SYSMEM reads
//  - CE3 is for SYSMEM writes
//  - CE4-8 are for P2P
//
#define NVLINK_SYSMEM_READ_LCE  2
#define NVLINK_SYSMEM_WRITE_LCE 3
#define NVLINK_MIN_P2P_LCE      4
#define NVLINK_MAX_P2P_LCE      8

#endif
