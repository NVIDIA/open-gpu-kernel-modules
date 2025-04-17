/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/gpu_child_class_defs.h"
#include "kernel/gpu/intr/intr.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "published/ampere/ga100/dev_bus_addendum.h"
#include "published/ampere/ga100/dev_fb.h"
#include "published/ampere/ga100/dev_vm.h"
#include "published/ampere/ga100/dev_fuse.h"
#include "published/ampere/ga100/dev_top.h"
#include "virtualization/hypervisor/hypervisor.h"

/*!
 * @brief Read fuse for display supported status.
 *        Some chips not marked displayless do not support display
 */
NvBool
gpuFuseSupportsDisplay_GA100
(
    OBJGPU *pGpu
)
{
    return GPU_FLD_TEST_DRF_DEF(pGpu, _FUSE, _STATUS_OPT_DISPLAY, _DATA, _ENABLE);
}

/*!
 * @brief Clear FBHUB POISON Interrupt state for Bug 2924523.
 * This HAL handles the CPU interrupt tree
 *
 * @param[in]      pGpu           OBJGPU pointer
 *
 * @return NV_OK if success, else appropriate NV_STATUS code
 */
NV_STATUS
gpuClearFbhubPoisonIntrForBug2924523_GA100
(
    OBJGPU *pGpu
)
{
    // INTR module is not stateloaded at gpuPostConstruct, so use HW default
    NvU32 intrVector = NV_PFB_FBHUB_POISON_INTR_VECTOR_HW_INIT;

    if (pGpu == NULL)
        return NV_OK;

    //
    // Check if FBHUB Poison interrupt got triggered before RM Init due
    // to VBIOS IFR on GA100. If yes, clear the FBHUB Interrupt. This WAR is
    // required for Bug 2924523 as VBIOS IFR causes FBHUB Poison intr.
    //
    if (intrIsVectorPending_HAL(pGpu, GPU_GET_INTR(pGpu), intrVector, NULL))
    {
        NV_PRINTF(LEVEL_ERROR, "FBHUB Interrupt detected. Clearing it.\n");
        intrClearLeafVector_HAL(pGpu, GPU_GET_INTR(pGpu), intrVector, NULL);
    }

    return NV_OK;
}

/*!
 * @brief Returns FLA VASpace Size for Ampere
 *
 * @param[in] pGpu                        OBJGPU pointer
 * @param[in] bNvSwitchVirtualization     boolean
 *
 * @returns NvU64 -> size of FLA VASpace
 */
NvU64
gpuGetFlaVasSize_GA100
(
    OBJGPU *pGpu,
    NvBool  bNvswitchVirtualization
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64          totalFbSize    = (pMemoryManager->Ram.fbTotalMemSizeMb << 20);

    if (bNvswitchVirtualization || totalFbSize <= NVBIT64(36))
    {
        return 0x2000000000;  // 128GB
    }
    else
    {
        return (totalFbSize * 2);
    }
}

/*!
 * @brief Is ctx buffer allocation in PMA supported
 */
NvBool
gpuIsCtxBufAllocInPmaSupported_GA100
(
    OBJGPU *pGpu
)
{
    //
    // This is supported by default on baremetal RM.
    // This has no impact in guest-RM since ctxBufPools are disabled on guest.
    // We leave this disabled on host-RM. TODO: Bug 4066846
    //
    if (!hypervisorIsVgxHyper())
        return NV_TRUE;
    return NV_FALSE;
}


//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//

static const GPUCHILDPRESENT gpuChildrenPresent_GA100[] =
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
    GPU_CHILD_PRESENT(KernelCE, 10),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 8),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GA100(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GA100);
    return gpuChildrenPresent_GA100;
}

//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//
static const GPUCHILDPRESENT gpuChildrenPresent_GA102[] =
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
    GPU_CHILD_PRESENT(KernelCE, 5),
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
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GA102(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GA102);
    return gpuChildrenPresent_GA102;
}


//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//
static const GPUCHILDPRESENT gpuChildrenPresent_GA10B[] =
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
    GPU_CHILD_PRESENT(KernelCE, 4),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 2),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GA10B(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GA10B);
    return gpuChildrenPresent_GA10B;
}


/*! @brief Returns if a P2P object is allocated in SRIOV mode.
 *
 *  @param[in]   pGpu     OBJGPU pointer
 *
 *  @returns for baremetal, this should just return NV_TRUE
             for SRIOV, return the SRIOV Info
 */
NvBool
gpuCheckIsP2PAllocated_GA100
(
    OBJGPU *pGpu
)
{
    if (!IS_VIRTUAL(pGpu) && !gpuIsSriovEnabled(pGpu))
        return NV_TRUE;

    return pGpu->sriovState.bP2PAllocated;
}

NvBool
gpuCheckIfFbhubPoisonIntrPending_GA100
(
    OBJGPU *pGpu
)
{
    return intrIsVectorPending_HAL(pGpu, GPU_GET_INTR(pGpu), NV_PFB_FBHUB_POISON_INTR_VECTOR_HW_INIT, NULL);
}


/*!
 * @brief A bit in scratch 30 is reserved to indicate GPU Drain and Reset is pending.
 * This is useful for CSPs monitoring whether a GPU needs to be drained and reset through out of band.
 * This function sets the bit to the requested value, indicating that a drain and reset is pending or clearing the status.
 *
 * @param[in] pGpu               OBJGPU pointer
 * @param[in] bDrainRecommended  Value to set
 *
 * @returns NV_OK
 */
NV_STATUS
gpuSetDrainAndResetScratchBit_GA100
(
    OBJGPU *pGpu,
    NvBool  bDrainRecommended
)
{
    NvU32 scratchRegVal = gpuReadPBusScratch_HAL(pGpu, NV_PBUS_SW_RESET_BITS_SCRATCH_REG);

    scratchRegVal = bDrainRecommended ?
                    FLD_SET_DRF(_PBUS, _SW_SCRATCH30, _GPU_DRAIN_AND_RESET_RECOMMENDED, _YES, scratchRegVal) :
                    FLD_SET_DRF(_PBUS, _SW_SCRATCH30, _GPU_DRAIN_AND_RESET_RECOMMENDED, _NO,  scratchRegVal);

    gpuWritePBusScratch_HAL(pGpu, NV_PBUS_SW_RESET_BITS_SCRATCH_REG, scratchRegVal);

    return NV_OK;
}

/*!
 * @brief A bit in scratch 30 is reserved to indicate GPU Drain and Reset is pending.
 * This is useful for CSPs monitoring whether a GPU needs to be drained and reset through out of band.
 * This function reads the value of the bit.
 *
 * @param[in]  pGpu                OBJGPU pointer
 * @param[out] pbDrainRecommended  The value of the scratch bit
 *
 * @returns NV_OK
 */
NV_STATUS
gpuGetDrainAndResetScratchBit_GA100
(
    OBJGPU *pGpu,
    NvBool *pbDrainRecommended
)
{
    NvU32 scratchRegVal = gpuReadPBusScratch_HAL(pGpu, NV_PBUS_SW_RESET_BITS_SCRATCH_REG);

    *pbDrainRecommended = FLD_TEST_DRF(_PBUS, _SW_SCRATCH30, _GPU_DRAIN_AND_RESET_RECOMMENDED, _YES, scratchRegVal);
    return NV_OK;
}
