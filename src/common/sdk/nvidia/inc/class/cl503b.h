/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nvtypes.h>

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      class/cl503b.finn
//

#define NV50_P2P                  (0x503bU) /* finn: Evaluated from "NV503B_ALLOC_PARAMETERS_MESSAGE_ID" */

#define NV503B_FLAGS_P2P_TYPE            0:0
#define NV503B_FLAGS_P2P_TYPE_GPA 0
#define NV503B_FLAGS_P2P_TYPE_SPA 1

/*
 * NV503B_BAR1_P2P_DMA_INFO
 *
 *  The DMA information for BAR1 P2P.
 *  The default value for dma_address is NV_U64_MAX and dma_size is 0.
 *  This BAR1 P2P DMA information is only valid for the kernel-privileged clients.
 *  For unprivileged clients, all fields will be the default values.
 *
 */
typedef struct NV503B_BAR1_P2P_DMA_INFO {
    NV_DECLARE_ALIGNED(NvU64 dma_address, 8);
    NV_DECLARE_ALIGNED(NvU64 dma_size, 8);
} NV503B_BAR1_P2P_DMA_INFO;

/* NvRmAlloc parameters */
#define NV503B_ALLOC_PARAMETERS_MESSAGE_ID (0x503bU)

typedef struct NV503B_ALLOC_PARAMETERS {
    NvHandle hSubDevice;                /* subDevice handle of local GPU              */
    NvHandle hPeerSubDevice;            /* subDevice handle of peer GPU               */
    NvU32    subDevicePeerIdMask;       /* Bit mask of peer ID for SubDevice
                                         * A value of 0 defaults to RM selected
                                         * PeerIdMasks must match in loopback         */
    NvU32    peerSubDevicePeerIdMask;   /* Bit mask of peer ID for PeerSubDevice
                                         * A value of 0 defaults to RM selected
                                         * PeerIdMasks must match in loopback         */
    NV_DECLARE_ALIGNED(NvU64 mailboxBar1Addr, 8);           /* P2P Mailbox area base offset in BAR1
                                         * Must have the same value across the GPUs   */
    NvU32    mailboxTotalSize;          /* Size of the P2P Mailbox area
                                         * Must have the same value across the GPUs   */
    NvU32    flags;                     /* Flag to indicate types/attib of p2p   */
    NvU32    subDeviceEgmPeerIdMask;       /* Bit mask of EGM peer ID of SubDevice       */
    NvU32    peerSubDeviceEgmPeerIdMask;    /* Bit mask of EGM peer ID for PeerSubDevice  */
    NV_DECLARE_ALIGNED(NV503B_BAR1_P2P_DMA_INFO l2pBar1P2PDmaInfo, 8);  /* Bar1 DMA info from local GPU to peer GPU   */
    NV_DECLARE_ALIGNED(NV503B_BAR1_P2P_DMA_INFO p2lBar1P2PDmaInfo, 8);  /* Bar1 DMA info from peer GPU to local GPU   */
} NV503B_ALLOC_PARAMETERS;
