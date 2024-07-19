/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*                                                                           *
*         SLI specific GPU Manager functions                                *
*                                                                           *
\***************************************************************************/

#include "core/system.h"
#include "core/thread_state.h"
#include "core/locks.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "kernel/gpu/intr/intr.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/heap.h"
#include "platform/sli/sli.h"
#include "nvpcie.h"
#include "nvhybridacpi.h"
#include "platform/platform.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu_mgr/gpu_mgr_sli.h"
#include "gpu_mgr/gpu_group.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/rc/kernel_rc.h"

#include "nvRmReg.h"

#include "ctrl/ctrl0000/ctrl0000gpu.h"
#include "nvlimits.h"

//
// gpumgrAreGpusInitialized
//
// Returns NV_TRUE if all GPUS in the given mask have been initialized.
//
NvBool gpumgrAreGpusInitialized(NvU32 gpuMask)
{
    NvU32 idx1 = 0;
    OBJGPU *pGpu;

    while ( (pGpu = gpumgrGetNextGpu(gpuMask, &idx1)) != NULL )
    {
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_STATE_INITIALIZED))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

void
gpumgrDetectSliLinkFromGpus
(
    NvBool  bForceDetection,
    NvU32   gpuCount,
    NvU32   gpuMaskArg,
    NvU32  *pSliLinkTestDone,
    NvU32  *pSliLinkOutputMask,
    NvBool *pSliLinkCircular,
    NvU32  *pSliLinkEndsMask,
    NvU32  *pVidLinkCount
)
{
    OBJGPU     *pGpu;
    OBJGPU     *pGpuSaved;
    NvU32       gpuIndex        = 0;
    NvU32       EndsMask        = 0;
    NvU32       gpuMask         = gpuMaskArg;
    NvU32       linkTestDone    = 0, linkOutputMask = 0;
    NvBool      bSliLinkCircular = NV_FALSE;
    NvU32       vidLinkCount    = 0;
    NvU32       linkHalImpl;
    NvBool      *pBcState;
    NvU32       gpuIndexInBcStateTable;

    *pSliLinkTestDone = *pSliLinkOutputMask = *pSliLinkEndsMask = 0;

    // Bail out early if all GPU'S haven't been loaded.
    if (!gpumgrAreGpusInitialized(gpuMask))
    {
        NV_PRINTF(LEVEL_WARNING,
                  "All GPUs do not have state initialized\n");
        return;
    }

    // If there's only 1 bit set or no bits set here then this call
    // is not going to help!
    if (gpuCount < 2)
    {
        // If we've gotten here, then that means the caller is calling
        // before more than one GPU is setup
        NV_PRINTF(LEVEL_INFO,
                  "gpumgrDetectSliLinkFromGpus: Need >=2 GPUs to test SliLink.\n          gpuMask = 0x%x\n",
                  gpuMask);
        return;
    }

    // If we've already done the detection on this set of gpus
    // return the output mask.

    if ((linkTestDone == gpuMask) && !bForceDetection)
    {
        *pSliLinkTestDone = linkTestDone;
        *pSliLinkOutputMask = linkOutputMask;
        *pSliLinkCircular = bSliLinkCircular;
        *pSliLinkEndsMask = EndsMask;
        *pVidLinkCount    = vidLinkCount;
        return;
    }

    // Bail early if one of the GPUs does not have a display engine
    gpuIndex = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL)
    {
        KernelDisplay *pKernelDisp = GPU_GET_KERNEL_DISPLAY(pGpu);
        if (
            pKernelDisp == NULL)
        {
            *pSliLinkTestDone = gpuMask;
            *pSliLinkOutputMask = 0;
            return;
        }
    }

    // clear out linkTestDone, linkOutputMask, circularVideoBridge
    linkTestDone = linkOutputMask = 0;
    bSliLinkCircular = NV_FALSE;

    // Check that all GPUs have the same HAL
    gpuIndex = 0;
    pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    if (pGpu != NULL)
    {
        pGpuSaved = pGpu;
        linkHalImpl = gpuGetSliLinkDetectionHalFlag_HAL(pGpu);
    }
    else
    {
         *pSliLinkTestDone = gpuMask;
         *pSliLinkOutputMask = 0;
         return;
    }

    pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    while (pGpu != NULL)
    {
        if (linkHalImpl != gpuGetSliLinkDetectionHalFlag_HAL(pGpu))
        {
            *pSliLinkTestDone = gpuMask;
            *pSliLinkOutputMask = 0;
            return;
        }
        pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    }

    // Make sure BC is disabled
    pBcState = portMemAllocNonPaged(gpuCount * sizeof(NvBool));
    if (pBcState == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "gpumgrDetectSliLinkFromGpus: Insufficient resources.\n");
        return;
    }

    gpuIndex = 0;
    gpuIndexInBcStateTable = 0;
    pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    while (pGpu != NULL)
    {
        if (IsSLIEnabled(pGpu))
        {
            pBcState[gpuIndexInBcStateTable++] = gpumgrGetBcEnabledStatus(pGpu);
            gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
        }
        pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    }

    // Call HAL function
    gpuDetectSliLinkFromGpus_HAL(pGpuSaved, gpuCount, gpuMaskArg,
                                 &linkOutputMask,
                                 &bSliLinkCircular,
                                 &EndsMask,
                                 &vidLinkCount);

    // Restore BC settings
    gpuIndex = 0;
    gpuIndexInBcStateTable = 0;
    pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    while (pGpu != NULL)
    {
        if (IsSLIEnabled(pGpu))
        {
            gpumgrSetBcEnabledStatus(pGpu, pBcState[gpuIndexInBcStateTable++]);
        }

        // Clear high speed bridge property here on all GPUs.
        // We set it in gpumgrDetectHighSpeedVideoBridges() for the GPUs in the video link.
        pGpu->setProperty(pGpu, PDB_PROP_GPU_HIGH_SPEED_BRIDGE_CONNECTED, NV_FALSE);

        pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    }

    portMemFree(pBcState);

    linkTestDone = gpuMaskArg;

    // See if we want to disable SLI Video Bridge - Bug 174307
    // Only need to read the regkey if we passed the detection code
    if (linkOutputMask)
    {
        gpuMask = linkOutputMask;

        gpuIndex = 0;
        for (pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
            NULL != pGpu;
            pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex))
        {
            if (gpuIsVideoLinkDisabled(pGpu))
            {
                // Remove all gpus if this regkey is found on any gpu
                linkOutputMask = 0;
            }
        }
    }

    gpumgrDetectHighSpeedVideoBridges(linkOutputMask);

    *pSliLinkOutputMask = linkOutputMask;
    *pSliLinkTestDone = linkTestDone;
    *pSliLinkCircular = bSliLinkCircular;
    *pSliLinkEndsMask = EndsMask;
    *pVidLinkCount = vidLinkCount;
}

/*!
 * @brief Updates the GPU routing information for the
 *          2 specified GPUs using the 2 Pinset Indices specified.
 *
 * @param[In]   pGpuParent      The 1st GPU to be tested for a connection.
 *              pGpuChild       The 2nd GPU to be tested for a connection with the 1st GPU.
 *              bridgeType      The type of bridge  whose routing information
 *                              should be updated withthe resiults of the testing.
 *              parentPinsetIndex   The pinset index to be used on the GpuParent
 *                              to test for an SLI connection.
 *              childPinsetIndex    The pinset index to be used on the GpuChild
 *                              to test for an SLI connection.
 *              bForceUpdate    Will be NV_TRUE if we want to force update the SLI link even
 *                              with no video link detected.
 *
 * @retun       a boolen indicating if the SLI connection specified
 *              by the pinset indices is valid between the 2 specified GPUs.
.*/
NvBool gpumgrUpdateSliLinkRouting
(
    OBJGPU *pGpuParent,
    OBJGPU *pGpuChild,
    NvU32 bridgeType,
    NvU32 parentPinsetIndex,
    NvU32 childPinsetIndex,
    NvBool bForceUpdate
)
{

    NvBool      linkFound   = NV_FALSE;
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJGPUMGR  *pGpuMgr     = SYS_GET_GPUMGR(pSys);
    KernelDisplay *pKernelDisp = GPU_GET_KERNEL_DISPLAY(pGpuParent);

    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpuParent) || pKernelDisp != NULL, NV_FALSE);

    // run the test. It is bidirectional
    if (!bForceUpdate)
    {
        if (pKernelDisp != NULL)
            linkFound = kdispDetectSliLink_HAL(pKernelDisp, pGpuParent, pGpuChild, NVBIT(parentPinsetIndex), NVBIT(childPinsetIndex)) == NV_OK;
    }

    if (linkFound || bForceUpdate)
    {
        // Store off the input connection between Parent and Child here
        pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex] =
            FLD_SET_DRF(_SLILINK, _ROUTE, _INPUT, _PASSED,
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex]);
        pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex] =
            FLD_SET_DRF(_SLILINK, _ROUTE, _INPUT, _PASSED,
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex]);

        // Store off the output connection between Parent and Child here
        pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex] =
            FLD_SET_DRF(_SLILINK, _ROUTE, _OUTPUT, _PASSED,
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex]);
        pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex] =
            FLD_SET_DRF(_SLILINK, _ROUTE, _OUTPUT, _PASSED,
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex]);

        // Store off the Child Port used by the Parent
        pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex] =
            FLD_SET_DRF_NUM(_SLILINK, _ROUTE, _INPUT_CHILD_PORT, (NvU8) NVBIT(childPinsetIndex),
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex]);

        // Store off the Parent Port used by the Child
        pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex] =
            FLD_SET_DRF_NUM(_SLILINK, _ROUTE, _INPUT_CHILD_PORT, (NvU8) NVBIT(parentPinsetIndex),
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex]);

        // Setup GPUs to reference each other
        pGpuParent->peer[parentPinsetIndex].pGpu       = pGpuChild;
        pGpuParent->peer[parentPinsetIndex].pinset     = NVBIT(childPinsetIndex);
        pGpuChild->peer[childPinsetIndex].pGpu   = pGpuParent;
        pGpuChild->peer[childPinsetIndex].pinset = NVBIT(parentPinsetIndex);
    }
    else
    {
         // Make sure to not override any passing cases with a failure
         // Store off the failed input connection between Parent & Child here
        if (!FLD_TEST_DRF(_SLILINK, _ROUTE, _INPUT, _PASSED,
            pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex]))
        {
            pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex] =
                FLD_SET_DRF(_SLILINK, _ROUTE, _INPUT, _FAILED,
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex]);
        }
        if (!FLD_TEST_DRF(_SLILINK, _ROUTE, _INPUT, _PASSED,
            pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex]))
        {
            pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex] =
                FLD_SET_DRF(_SLILINK, _ROUTE, _INPUT,  _FAILED,
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex ]);
        }
        // Store off the failed output connection between Child & Parent here
        if (!FLD_TEST_DRF(_SLILINK, _ROUTE, _OUTPUT, _PASSED,
            pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex]))
        {
            pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex] =
                FLD_SET_DRF(_SLILINK, _ROUTE, _OUTPUT, _FAILED,
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuParent->gpuInstance][pGpuChild->gpuInstance][parentPinsetIndex]);
        }
        if (!FLD_TEST_DRF(_SLILINK, _ROUTE, _OUTPUT, _PASSED,
            pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex]))
        {
            pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex]  =
                FLD_SET_DRF(_SLILINK, _ROUTE, _OUTPUT, _FAILED,
                pGpuMgr->gpuSliLinkRoute[bridgeType][pGpuChild->gpuInstance][pGpuParent->gpuInstance][childPinsetIndex ]);
        }
    }
    return linkFound;
}

NvBool
gpumgrDetectHighSpeedVideoBridges(NvU32 linkOutputMask)
{
    OBJGPU     *pGpu;
    KernelDisplay *pKernelDisp;
    NvU32       gpuIndex;
    NvBool      bHighSpeedBridgeConnected = NV_FALSE;

    gpuIndex = 0;
    while ((pGpu = gpumgrGetNextGpu(linkOutputMask, &gpuIndex)))
    {
        pKernelDisp = GPU_GET_KERNEL_DISPLAY(pGpu);

        if (
            pKernelDisp == NULL)
        {
            NV_ASSERT(pKernelDisp != NULL); // This should not happen
            continue;
        }

        //
        // Only one GPU in the link is able to detect a high speed bridge.
        // 3-way SLI, bridged 4-way SLI are not supported, so it is OK.
        //
        pGpu->setProperty(pGpu, PDB_PROP_GPU_HIGH_SPEED_BRIDGE_CONNECTED, NV_FALSE);

        NvU32 gpuLockMask = 0;
        if (IS_GSP_CLIENT(pGpu) && !rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
            NV_ASSERT_OK_OR_RETURN(rmGpuGroupLockAcquire(pGpu->gpuInstance,
                                                           GPU_LOCK_GRP_SUBDEVICE,
                                                           GPUS_LOCK_FLAGS_NONE,
                                                           RM_LOCK_MODULES_RPC,
                                                           &gpuLockMask));

        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV_STATUS status = pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_DETECT_HS_VIDEO_BRIDGE,
                                NULL,
                                0);

        bHighSpeedBridgeConnected |= (status == NV_OK);

        if (gpuLockMask != 0)
            rmGpuGroupLockRelease(gpuLockMask, GPUS_LOCK_FLAGS_NONE);
    }

    if (bHighSpeedBridgeConnected)
    {
        //
        // Update video bridge max pixelclock to the fast video bridge limit.
        // We are here because no slow video bridge ic\s connected to any connectors,
        // and a high speed video bridge is connected at least on one of the connectors.
        //
        gpuIndex = 0;
        while ((pGpu = gpumgrGetNextGpu(linkOutputMask, &gpuIndex)))
        {
             pKernelDisp = GPU_GET_KERNEL_DISPLAY(pGpu);

             if (
                pKernelDisp == NULL)
             {
                 NV_ASSERT(pKernelDisp != NULL); // This should not happen
                 continue;
             }

             // Set high speed bridge property on all GPUs.
             pGpu->setProperty(pGpu, PDB_PROP_GPU_HIGH_SPEED_BRIDGE_CONNECTED, NV_TRUE);
        }
    }
    return bHighSpeedBridgeConnected;
}

/*!
 * @brief Returns the pinset index to be used in an array from the pinset definition
 *
 * @param[In]  pinset           NvU32, Pinset value. Should have only one bit set.
 * @param[Out]  pPinsetIndex    NvU32*, Pinset index value to be used in an array.
.*/
NV_STATUS
gpumgrPinsetToPinsetTableIndex(NvU32 pinset, NvU32 *pPinsetIndex)
{
    NvU32 pinsetIndex;
    if (!ONEBITSET(pinset))
    {
        NV_PRINTF(LEVEL_ERROR, "More than one bit set: 0x%x\n", pinset);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (pinset)
    {
        case drPinSet_A:
        case drPinSet_B:
            break;
        case drPinSet_None:
        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown pin set value: 0x%x\n", pinset);
            DBG_BREAKPOINT();
            return NV_ERR_INVALID_ARGUMENT;
    }
    pinsetIndex = pinset - 1;
    NUMSETBITS_32(pinsetIndex);

    *pPinsetIndex = pinsetIndex;
    return NV_OK;
}

//
// gpumgrGetSliLinks
//
// This routine services the NV0000_CTRL_CMD_GPU_GET_VIDEO_LINKS
// command.  The passed in table is filled in with information
// about SLI connections (video bridge or NvLinks) between GPUs identified by their
// GPU IDs.  Each row represents the connections, if any, for a
// single GPU.  Peer GPUs are listed using their GPU IDs, invalid
// entries are filled with invalid ID values.
//
NV_STATUS
gpumgrGetSliLinks(NV0000_CTRL_GPU_GET_VIDEO_LINKS_PARAMS *pVideoLinksParams)
{
    NvU32 i, j;
    NvU32 gpuIndex, peerGpuIndex;
    NvU32 pinsetIndex, childDrPort, childDrPortIndex;
    NV0000_CTRL_GPU_VIDEO_LINKS *pSliLinks;
    OBJGPU *pGpu, *pPeerGpu;
    OBJSYS *pSys;
    OBJGPUMGR *pGpuMgr;
    NvU32 gpuAttachCnt, gpuAttachMask;
    NV_STATUS rmStatus = NV_OK;

    for (i = 0; i < NV0000_CTRL_GPU_MAX_ATTACHED_GPUS; i++)
    {
        pSliLinks = &pVideoLinksParams->links[i];

        for (j = 0; j < NV0000_CTRL_GPU_MAX_VIDEO_LINKS; j++)
        {
            pSliLinks->connectedGpuIds[j] = NV0000_CTRL_GPU_INVALID_ID;
        }
        pSliLinks->gpuId = NV0000_CTRL_GPU_INVALID_ID;
    }

    gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);
    if (gpuAttachCnt < 2)
    {
        return NV_OK;
    }

    pSys = SYS_GET_INSTANCE();
    pGpuMgr = SYS_GET_GPUMGR(pSys);

    i = gpuIndex = 0;
    while ((pGpu = gpumgrGetNextGpu(gpuAttachMask, &gpuIndex)) &&
           (i < NV0000_CTRL_GPU_MAX_ATTACHED_GPUS))
    {
        if (pGpu->gpuInstance >= NV2080_MAX_SUBDEVICES)
        {
            NV_ASSERT(pGpu->gpuInstance < NV2080_MAX_SUBDEVICES);
            continue;
        }

        pSliLinks = &pVideoLinksParams->links[i++];
        pSliLinks->gpuId = pGpu->gpuId;

        j = peerGpuIndex = 0;
        while ((pPeerGpu = gpumgrGetNextGpu(gpuAttachMask, &peerGpuIndex)) &&
               (j < NV0000_CTRL_GPU_MAX_VIDEO_LINKS))
        {
            if ((peerGpuIndex == gpuIndex) ||
                (pPeerGpu->gpuInstance >= NV2080_MAX_SUBDEVICES))
            {
                continue;
            }

            for (pinsetIndex = 0; pinsetIndex < DR_PINSET_COUNT; pinsetIndex++)
            {
                if (!FLD_TEST_DRF(_SLILINK, _ROUTE, _INPUT, _PASSED,
                    pGpuMgr->gpuSliLinkRoute[pGpuMgr->gpuBridgeType][pGpu->gpuInstance][pPeerGpu->gpuInstance][pinsetIndex]))
                {
                    continue;
                }

                childDrPort = DRF_VAL(_SLILINK, _ROUTE, _INPUT_CHILD_PORT,
                    pGpuMgr->gpuSliLinkRoute[pGpuMgr->gpuBridgeType][pGpu->gpuInstance][pPeerGpu->gpuInstance][pinsetIndex]);

                if (SLI_BT_VIDLINK == pGpuMgr->gpuBridgeType)
                {
                    rmStatus = gpumgrPinsetToPinsetTableIndex(childDrPort, &childDrPortIndex);
                    if (rmStatus != NV_OK)
                    {
                        return rmStatus;
                    }

                    if (!FLD_TEST_DRF(_SLILINK, _ROUTE, _OUTPUT, _PASSED,
                        pGpuMgr->gpuSliLinkRoute[pGpuMgr->gpuBridgeType][pPeerGpu->gpuInstance][pGpu->gpuInstance][childDrPortIndex]))
                    {
                        continue;
                    }
                }

                if (j < NV0000_CTRL_GPU_MAX_VIDEO_LINKS)
                {
                    pSliLinks->connectedGpuIds[j++] = pPeerGpu->gpuId;
                }
            }
        }
    }
    return NV_OK;
}
