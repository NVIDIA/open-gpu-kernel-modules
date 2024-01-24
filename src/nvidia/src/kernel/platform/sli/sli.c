/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************* SLI Routines ******************************************\
*   SLI support                                                             *
****************************************************************************/

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "nvpcie.h"
#include "os/os.h"
#include "platform/chipset/chipset.h"
#include "platform/platform.h"
#include "platform/sli/sli.h"
#include "gpu_mgr/gpu_mgr_sli.h"

// Return smallest integer greater than mask with the same number of bits set.
static NvU32 rmSliNextMask(NvU32 mask)
{
    NvU32 r, l;

    r = mask & -(NvS32)mask; // lowest set bit

    mask += r;              // replace lowest block by a one left to it

    l = mask & -(NvS32)mask;// first zero beyond lowest block

    l -= r;                // lowest block

    while ( 0==(l&1) )  { l >>= 1; }  // move block to low end of word

    return  mask | (l>>1);            // need one bit less of low block
}

// rmSliSearchForSliCombination
//
// This function looks for all the nWay-SLI configurations with this
// GPU.  It as all the valid and non-valid SLI configurations with
// their status.  It returns the gpu specfic statuses, and the number
// of valid configs found.
//
static NV_INLINE NvU32
rmSliSearchForSliCombination(OBJGPU *pGpu, NvU32 nWay, NvU32 numFoundValidConfigs, NvU32 *pGpuSliStatus)
{
    NvU32                   gpuAttachCnt;
    NvU32                   gpuAttachMask;
    NvU32                   gpuMask;
    NvU32                   numValidConfigs     = 0;
    NvU32                   mask;
    NvU32                   smallestMask;
    NvU32                   largestMask;
    NvU32                   bit;
    NvU32                   firstBit            = ~0;
    NvU32                   lastBit             = ~0;
    NvBool                  bSliLinkCircular    = NV_FALSE;
    NvU32                   sliLinkTestDone     = 0;
    NvU32                   sliLinkOutputMask   = 0;
    NvU32                   sliLinkEndsMask     = 0;
    NvU32                   vidLinkCount        = 0;
    NvBool                  bAnyValid           = NV_FALSE;

    // get gpu attach info
    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);

    // return if we have less gpus than we request for
    if (gpuAttachCnt < nWay)
    {
        numValidConfigs = 0;
        goto rmSliSearchForSliCombination_exit;
    }

    gpuAttachMask &= ~NVBIT(pGpu->gpuInstance);

    mask = gpuAttachMask;
    // Get the first bit and the last bit set in the 32-bit mask.
    for (bit = 0; mask; bit++)
    {
        if ((firstBit == (NvU32)~0) && (mask & 1))
        {
            firstBit = lastBit = bit;
        }
        else if (mask & 1)
        {
            lastBit = bit;
        }
        mask >>= 1;
    }

    NV_ASSERT(firstBit != (NvU32)~0);
    NV_ASSERT(lastBit != (NvU32)~0);

    if ((firstBit == (NvU32)~0) || (lastBit == (NvU32)~0))
    {
        numValidConfigs = 0;
        goto rmSliSearchForSliCombination_exit;
    }

    smallestMask = ((1 << ((nWay - 1) + firstBit)) - 1) & ~((1 << firstBit) - 1);
    largestMask =  smallestMask << (lastBit - firstBit - (nWay - 1) +  1);
    mask = smallestMask;

    // Search for n-way configs with this gpu.
    do
    {

        gpuMask = NVBIT(pGpu->gpuInstance) | mask;

        //
        // Look for a mask contained in the GPU mask,
        // and check that GPUs are initialized
        //
        if (((mask & gpuAttachMask) != mask) ||
            (!gpumgrAreGpusInitialized(gpuMask)))
        {
            // Last mask. Exit loop.
            if (mask == largestMask)
            {
                break;
            }
            mask = rmSliNextMask(mask);
            NV_ASSERT(mask);
            continue;
        }

        // Do Sli link detection
        gpumgrDetectSliLinkFromGpus(NV_TRUE, nWay, gpuMask,
            &sliLinkTestDone,
            &sliLinkOutputMask,
            &bSliLinkCircular,
            &sliLinkEndsMask,
            &vidLinkCount);

        // Last mask. Exit loop.
        if (mask == largestMask)
        {
            break;
        }

        mask = rmSliNextMask(mask);
        NV_ASSERT(mask);
    } while (1);

    if ((numValidConfigs == 0) && bAnyValid)
    {
        *pGpuSliStatus = NV0000_CTRL_SLI_STATUS_OK;
    }

rmSliSearchForSliCombination_exit:

    return numValidConfigs;
}

//
//
// RmRunSLISupportCheck
//
// This function establishes the SLI possibilities for the specified GPU.
// The GPU is tested with each previously attached GPU to determine the
// set of valid SLI configurations to which it belongs.
//
static NvU32 RmRunSLISupportCheck
(
    OBJGPU *pGpu
)
{
    NvU32      gpuSliStatus         = NV0000_CTRL_SLI_STATUS_OK;
    NvU32      gpuAttachCnt;
    NvU32      gpuAttachMask;
    NvU32      numValidConfigs      = 0;
    NvU32      numXWayValidConfigs;
    NvU32      nWay                 = 0;

    // RMCONFIG: is SLI enabled in this build?

    OBJSYS    *pSys                 = SYS_GET_INSTANCE();

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        gpuSliStatus |= NV0000_CTRL_SLI_STATUS_GPU_NOT_SUPPORTED;

        // Detect SLI link before failing on GSP client RM
        if (!IS_GSP_CLIENT(pGpu))
            goto fail;
    }

    // get gpu attach info
    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);

    // Need more than one gpu to continue.
    if (gpuAttachCnt <= 1)
    {
        // Do not cache this status in pGpu->sliStatus as it is dependent on the GPU count
        gpuSliStatus |= NV0000_CTRL_SLI_STATUS_INVALID_GPU_COUNT;
        goto fail;
    }

    KernelNvlink *pKernelNvlink     = GPU_GET_KERNEL_NVLINK(pGpu);

    // Do not run the SLI check with NVSwitch
    if ((pSys->getProperty(pSys, PDB_PROP_SYS_NVSWITCH_IS_PRESENT) ||
         ((pKernelNvlink != NULL) &&
          ((GPU_IS_NVSWITCH_DETECTED(pGpu)) ||
           knvlinkIsNvswitchProxyPresent(pGpu, pKernelNvlink)))) &&
        pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED))
    {
        gpuSliStatus |= NV0000_CTRL_SLI_STATUS_GPU_NOT_SUPPORTED;
        goto fail;
    }

     // For 9 GPUs and above, we do not check the SLI configs
    if(gpuAttachCnt <= 8)
    {
        for(nWay=2; nWay<=gpuAttachCnt; nWay++)
        {
            numXWayValidConfigs = rmSliSearchForSliCombination(pGpu, nWay, numValidConfigs, &gpuSliStatus);
            numValidConfigs += numXWayValidConfigs;
        }

        //
        // Cache the gpu status in pGpu->sliStatus in case it has been cleared
        //
        // NOTE: rmSliSearchForSliCombination() clears gpuSliStatus in case of Base Mosaic and
        // Base Mosaic also requires gpu specific slistatus to be OK. Hence gpuSlistatus
        // is assigned to gpu specific status again.
        //
        if (gpuSliStatus == NV0000_CTRL_SLI_STATUS_OK)
        {
            pGpu->sliStatus = gpuSliStatus;
        }
    }
goto done;

fail:
    pGpu->sliStatus = gpuSliStatus;

done:
    NV_PRINTF(LEVEL_INFO, "gpuSliStatus 0x%x\n", gpuSliStatus);

    return numValidConfigs;
}

//
//
// RmInitScalability
//
// This function calls to establish the SLI possibilities for the specified GPU.
// Then it initalizes any additional SLI attributes.
//
void RmInitScalability
(
    OBJGPU *pGpu
)
{
    NvU32 numValidConfigs;
    NvU32 gpuMask;
    NvU32 idx = 0;
    NvU32 gpuLockMask = 0;

    if (IS_GSP_CLIENT(pGpu) && !rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
        NV_ASSERT_OR_RETURN_VOID(rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                       GPU_LOCK_GRP_SUBDEVICE,
                                                       GPUS_LOCK_FLAGS_NONE,
                                                       RM_LOCK_MODULES_RPC,
                                                       &gpuLockMask) == NV_OK);

    // determine the supported SLI configurations.
    numValidConfigs = RmRunSLISupportCheck(pGpu);

    if (0 < numValidConfigs)
    {
        gpuMask = gpumgrGetVidLinkOutputMaskFromGpu(pGpu);
        while ((pGpu = gpumgrGetNextGpu(gpuMask, &idx)) != NULL)
        {
            gpuInitSliIllumination(pGpu);
        }
    }

    if (gpuLockMask != 0)
        rmGpuGroupLockRelease(gpuLockMask, GPUS_LOCK_FLAGS_NONE);
}


NV_STATUS RmShutdownScalability
(
    OBJGPU *pGpu
)
{
    return NV_OK;
}
