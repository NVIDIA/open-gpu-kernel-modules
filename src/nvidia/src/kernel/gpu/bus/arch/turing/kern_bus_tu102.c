/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/bus/kern_bus.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"

/*!
 * @brief Returns the first available peer Id excluding the nvlink peerIds
 *
 * @param[in] pGpu
 * @param[in] pKernelBus
 *
 * @returns NvU32 first free peer Id
 */
NvU32
kbusGetUnusedPciePeerId_TU102
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    NvU32         nvlinkIdMask  = 0;
    NvU32         peerId;

    if ((pKernelNvlink != NULL) &&
        (pKernelNvlink->getProperty(pKernelNvlink,
                        PDB_PROP_KNVLINK_WAR_BUG_3471679_PEERID_FILTERING)))
    {
        //
        // Get the mask of NvLink peerIds, to exclude them from the
        // peerIds PCIE P2P is using.
        // Pre-Ampere GPUs use a static peerId assignment reserved in
        // busGetNvlinkP2PPeerId_GP100() and we need to make sure the
        // PCIE and nvLink P2P assignments do not collide.
        // Make this Windows + Turing only until bug 3471679 is fixed.
        //
        nvlinkIdMask = knvlinkGetUniquePeerIdMask_HAL(pGpu, pKernelNvlink);
    }

    for (peerId = 0; peerId < pKernelBus->numPeers; peerId++)
    {
        if ((pKernelBus->p2pPcie.busPeer[peerId].refCount == 0) &&
            (!pKernelBus->p2pPcie.busPeer[peerId].bReserved) &&
            ((BIT(peerId) & nvlinkIdMask) == 0))
        {
            return peerId;
        }
    }
    return BUS_INVALID_PEER;
}
