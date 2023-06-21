/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

#include "os/os.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"
#include "gpu/gpu.h"

/*!
 * Turing+ will use RXDET (receiver detect) feature to determine
 * if a link is connected. Check bRxDetected field in nvlink_link
 *
 * @param[in] pGpu           OBJGPU ptr
 * @param[in] pKernelNvlink  KernelNvlink ptr
 *
 * @return  returns NV_OK
 */
NV_STATUS
knvlinkFilterBridgeLinks_TU102
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    NV_STATUS status = NV_OK;
    NvU32     linkId;

    // All links from Turing+ are sensable by receiver detect
    pKernelNvlink->bridgeSensableLinks = pKernelNvlink->discoveredLinks;

    // If connections are forced through chiplib, return enabled links
    if (pKernelNvlink->bRegistryLinkOverride)
    {
        pKernelNvlink->connectedLinksMask = pKernelNvlink->enabledLinks;
        pKernelNvlink->bridgedLinks       = pKernelNvlink->enabledLinks;

        NV_PRINTF(LEVEL_INFO,
                  "Connections forced through chiplib. ConnectedLinksMask same as "
                  "enabledLinks = 0x%x\n", pKernelNvlink->connectedLinksMask);

        goto knvlinkFilterBridgeLinks_end;
    }

    // Mark the links as bridged if receiver detect has passed
    FOR_EACH_INDEX_IN_MASK(32, linkId, pKernelNvlink->discoveredLinks)
    {
#if defined(INCLUDE_NVLINK_LIB)

        // If the link has not been registered yet, continue
        if (pKernelNvlink->nvlinkLinks[linkId].core_link == NULL)
        {
            // Link is not registered yet. Connectivity is absent
            pKernelNvlink->connectedLinksMask &= ~NVBIT(linkId);
            pKernelNvlink->bridgedLinks       &= ~NVBIT(linkId);

            NV_PRINTF(LEVEL_INFO,
                      "GPU%d: Link%d not yet registered in core lib. Connectivity will be "
                      "established after RXDET\n", pGpu->gpuInstance, linkId);
            continue;
        }

        if (pKernelNvlink->nvlinkLinks[linkId].core_link->bRxDetected)
        {
            pKernelNvlink->connectedLinksMask |= NVBIT(linkId);
            pKernelNvlink->bridgedLinks       |= NVBIT(linkId);
        }
        else
        {
            pKernelNvlink->connectedLinksMask &= ~NVBIT(linkId);
            pKernelNvlink->bridgedLinks       &= ~NVBIT(linkId);
        }
#endif
    }
    FOR_EACH_INDEX_IN_MASK_END;

knvlinkFilterBridgeLinks_end:

    // For GSP-CLIENTs, the link masks and vbios info need to synchronize with GSP
    status = knvlinkSyncLinkMasksAndVbiosInfo(pGpu, pKernelNvlink);
    if (status != NV_OK)
    {
        return status;
    }

    return NV_OK;
}

/*!
 * @brief Return the mask of links that are connected
 *
 * @param[in] pGpu           OBJGPU ptr
 * @param[in] pKernelNvlink  KernelNvlink ptr
 */
NvU32
knvlinkGetConnectedLinksMask_TU102
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    //
    // On Turing, enabledLinks != connectedLinksMask
    // This is because the connection cannot be sensed till receiver detect
    // has reported whether or not the connection is present
    //
    return pKernelNvlink->connectedLinksMask;
}

/*!
 * @brief Is IOCTRL guaranteed to be powered up for D3
 *
 * @param[in] pGpu           OBJGPU ptr
 * @param[in] pKernelNvlink  KernelNvlink ptr
 *
 * return NV_TRUE if IOCTRL guaranteed to be powered up
 */
NvBool
knvlinkPoweredUpForD3_TU102
(
    OBJGPU       *pGpu,
    KernelNvlink *pKernelNvlink
)
{
    //
    // IOCTRL is guaranteed NOT to be reset if the D3 variant is
    // RTD3 or FGC6 and if NVLink L2 is supported on the chip
    //
    if ((pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_ACTIVE)  ||
         pGpu->getProperty(pGpu, PDB_PROP_GPU_FAST_GC6_ACTIVE)) &&
        pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_L2_POWER_STATE_ENABLED))
    {
        //
        // Bugs# 2274645, 2197144: On Turing, the NVLink clamps are broken. So,
        // IOCTRL unit will see the reset signal when the GPU enters RTD3/FGC6.
        //
        if (!pKernelNvlink->getProperty(pKernelNvlink,
                                        PDB_PROP_KNVLINK_BUG2274645_RESET_FOR_RTD3_FGC6))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}
