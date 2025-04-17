/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/eng_desc.h"
#include "gpu/gpu_child_class_defs.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "gpu_mgr/gpu_mgr_sli.h"
#include "pascal/gp100/dev_bus_addendum.h"


/***************************** HW State Routines ***************************\
*                                                                           *
*         Pascal specific Descriptor List management functions              *
*                                                                           *
\***************************************************************************/


//
// List of GPU children that are sequenced through engine state transitions (StateInit,
// StateLoad, StateUnload and StateDestroy). This list controls only engine presence. Ordered is
// defined by gpuGetChildrenOrder_HAL.
//
// IMPORTANT: This function is to be deleted. Engine removal should instead handled by
// <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT FORK THIS LIST!
//
// List entries contain {CLASS-ID, # of instances} pairs.
//
static const GPUCHILDPRESENT gpuChildrenPresent_GP100[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 6),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 1),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GP100(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GP100);
    return gpuChildrenPresent_GP100;
}

// See gpuChildrenPresent_GM200 for documentation on GPUCHILDPRESENT
static const GPUCHILDPRESENT gpuChildrenPresent_GP102[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 6),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 1),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GP102(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GP102);
    return gpuChildrenPresent_GP102;
}

// See gpuChildrenPresent_GM200 for documentation on GPUCHILDPRESENT
static const GPUCHILDPRESENT gpuChildrenPresent_GP106[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 6),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 1),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GP106(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GP106);
    return gpuChildrenPresent_GP106;
}

// See gpuChildrenPresent_GM200 for documentation on GPUCHILDPRESENT
static const GPUCHILDPRESENT gpuChildrenPresent_GP108[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 6),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 1),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GP108(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GP108);
    return gpuChildrenPresent_GP108;
}


/*!
 * @brief   checks for NvLink connections between the GPUs specified in the gpuMaskArg,
 *          and updates the routing information as appropriate.
 *
 * @param[In]   gpuCount    The number of GPUs to be checked for NvLink links.
 * @param[In]   gpuMaskArg  A mask of the GPUs that are to be tested for  NvLink links.
 * @param[Out]  pSliLinkOutputMask  a mask of the GPUs that are attached to the NvLink link.
 * @param[Out]  pSliLinkCircular    a boolean indicating if the NvLink link is circular
 *                          (always FALSE).
 * @param[Out]  pSliLinkEndsMask    a mask indicating the endpoints of the NvLink link.
 *                          (always 0).
 */
void
gpuDetectNvlinkLinkFromGpus_GP100
(
    OBJGPU *pGpu,
    NvU32   gpuCount,
    NvU32   gpuMaskArg,
    NvU32  *pSliLinkOutputMask,
    NvBool *pSliLinkCircular,
    NvU32  *pSliLinkEndsMask,
    NvU32  *pVidLinkCount
)
{
    OBJGPU       *pGpuLoop            = NULL;
    KernelNvlink *pKernelNvlink       = NULL;
    KernelNvlink *pKernelNvlinkChild  = NULL;
    OBJGPU       *pGpuChild           = NULL;
    NvU32         gpuIndex            = 0;
    NvU32         gpuIndexChild       = 0;
    NvU32         parentPinsetIndex;
    NvU32         childPinsetIndex;
    NvU32         gpuMask             = gpuMaskArg;

    *pSliLinkOutputMask = 0;
    *pSliLinkCircular = NV_FALSE;
    *pSliLinkEndsMask = 0;
    *pVidLinkCount = 0;

    //
    // Link detection HAL should have same HAL implementation as HAL flag.
    // This checks for mismatched HAL implementation flag.
    //
    NV_ASSERT_OR_RETURN_VOID(gpuGetNvlinkLinkDetectionHalFlag_HAL(pGpu) == GPU_LINK_DETECTION_HAL_GP100);

    // Loop through GPU unique pairs
    gpuIndex = 0;
    pGpuLoop = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    while (pGpuLoop != NULL)
    {
        // determine if there is a connected NvLink on the GPU.
        pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpuLoop);
        if (NULL == pKernelNvlink)
        {
            pGpuLoop = gpumgrGetNextGpu(gpuMask, &gpuIndex);
            continue;
        }

        gpuIndexChild = gpuIndex;
        pGpuChild = gpumgrGetNextGpu(gpuMask, &gpuIndexChild);
        while (pGpuChild != NULL)
        {
            // determine if there is a connected NvLink on the GPU.
            pKernelNvlinkChild = GPU_GET_KERNEL_NVLINK(pGpuChild);
            if (NULL != pKernelNvlinkChild)
            {
                if (knvlinkIsNvlinkP2pSupported(pGpuLoop, pKernelNvlink, pGpuChild) == NV_FALSE)
                {
                    pGpuChild = gpumgrGetNextGpu(gpuMask, &gpuIndexChild);
                    continue;
                }
                if (0 == knvlinkGetConnectedLinksMask_HAL(pGpuChild, pKernelNvlinkChild))
                {
                    pGpuChild = gpumgrGetNextGpu(gpuMask, &gpuIndexChild);
                    continue;
                }
            }
            else
            {
                pGpuChild = gpumgrGetNextGpu(gpuMask, &gpuIndexChild);
                continue;
            }
            // Loop through the parent pinsets
            for (parentPinsetIndex = 0; parentPinsetIndex < DR_PINSET_COUNT; parentPinsetIndex++)
            {
                // Loop through the child pinsets
                for (childPinsetIndex = 0; childPinsetIndex < DR_PINSET_COUNT; childPinsetIndex++)
                {
                    if (gpumgrUpdateSliLinkRouting(pGpuLoop, pGpuChild, SLI_BT_NVLINK,
                                                   parentPinsetIndex, childPinsetIndex, NV_FALSE))
                    {
                        // update link Output Mask
                        *pSliLinkOutputMask |= (NVBIT(pGpuLoop->gpuInstance) | NVBIT(pGpuChild->gpuInstance));

                        // Since we found the connection, don't even test any
                        // other ChildPinset for this connected ParentPinset.
                        break;
                    }
                } // for (ChildPinset
            } // for (ParentPinset
            pGpuChild = gpumgrGetNextGpu(gpuMask, &gpuIndexChild);
        } // while (pGpuChild)
        pGpuLoop = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    } // while (pGpu)
}

/*!
 * @brief A bit is reserved in scratch 30 to indicate GPU Reset is pending.
 * This is useful for CSPs monitoring whether a GPU needs to be reset through out of band.
 * This function sets the bit to the requested value, indicating that a reset is pending or clearing the status.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] bResetRequired    The value to set
 *
 * @returns NV_OK if successful
 */
NV_STATUS
gpuSetResetScratchBit_GP100
(
    OBJGPU *pGpu,
    NvBool  bResetRequired
)
{
    NvU32 scratchRegVal = gpuReadPBusScratch_HAL(pGpu, NV_PBUS_SW_RESET_BITS_SCRATCH_REG);

    scratchRegVal = bResetRequired ?
                    FLD_SET_DRF(_PBUS, _SW_SCRATCH30, _GPU_RESET_REQUIRED, _ON, scratchRegVal) :
                    FLD_SET_DRF(_PBUS, _SW_SCRATCH30, _GPU_RESET_REQUIRED, _OFF, scratchRegVal);

    gpuWritePBusScratch_HAL(pGpu, NV_PBUS_SW_RESET_BITS_SCRATCH_REG, scratchRegVal);

    return NV_OK;
}

/*!
 * @brief A bit is reserved in scratch 30 to indicate GPU Reset is pending.
 * This is useful for CSPs monitoring whether a GPU needs to be reset through out of band.
 * This function reads the value of the bit.
 *
 * @param[in]  pGpu             OBJGPU pointer
 * @param[out] pbResetRequired  The value of the scratch bit
 *
 * @returns NV_OK if successful
 */
NV_STATUS
gpuGetResetScratchBit_GP100
(
    OBJGPU *pGpu,
    NvBool *pbResetRequired
)
{
    NvU32 scratchRegVal = gpuReadPBusScratch_HAL(pGpu, NV_PBUS_SW_RESET_BITS_SCRATCH_REG);

    *pbResetRequired = FLD_TEST_DRF(_PBUS, _SW_SCRATCH30, _GPU_RESET_REQUIRED, _ON, scratchRegVal);
    return NV_OK;
}
