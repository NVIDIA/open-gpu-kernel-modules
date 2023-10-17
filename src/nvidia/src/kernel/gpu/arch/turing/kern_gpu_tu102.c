/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES
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
#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/hwproject.h"
#include "jt.h"

/*!
 * @brief Returns SR-IOV capabilities
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[out] pParams        Pointer for get_sriov_caps params
 *
 * @returns NV_OK always
 */
NV_STATUS
gpuGetSriovCaps_TU102
(
    OBJGPU *pGpu,
    NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *pParams
)
{
    if (!gpuIsSriovEnabled(pGpu))
    {
        pParams->bSriovEnabled = NV_FALSE;
        return NV_OK;
    }

    pParams->bSriovEnabled                         = NV_TRUE;
    pParams->totalVFs                              = pGpu->sriovState.totalVFs;
    pParams->firstVfOffset                         = pGpu->sriovState.firstVFOffset;
    pParams->FirstVFBar0Address                    = pGpu->sriovState.firstVFBarAddress[0];
    pParams->FirstVFBar1Address                    = pGpu->sriovState.firstVFBarAddress[1];
    pParams->FirstVFBar2Address                    = pGpu->sriovState.firstVFBarAddress[2];
    pParams->bar0Size                              = pGpu->sriovState.vfBarSize[0];
    pParams->bar1Size                              = pGpu->sriovState.vfBarSize[1];
    pParams->bar2Size                              = pGpu->sriovState.vfBarSize[2];
    pParams->b64bitBar0                            = pGpu->sriovState.b64bitVFBar0;
    pParams->b64bitBar1                            = pGpu->sriovState.b64bitVFBar1;
    pParams->b64bitBar2                            = pGpu->sriovState.b64bitVFBar2;
    pParams->bSriovHeavyEnabled                    = gpuIsWarBug200577889SriovHeavyEnabled(pGpu);
    pParams->bEmulateVFBar0TlbInvalidationRegister = pGpu->getProperty(pGpu, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE);
    pParams->bClientRmAllocatedCtxBuffer           = gpuIsClientRmAllocatedCtxBufferEnabled(pGpu);
    pParams->bNonPowerOf2ChannelCountSupported     = gpuIsNonPowerOf2ChannelCountSupported(pGpu);

    return NV_OK;
}

/*!
 * @brief determines whether this GPU mode needs to be initialized with an offset
 *        to access the registers defined in dev_vm.ref.
 *
 * @param[in]  pGpu          OBJGPU pointer
 *
 * @returns NvU32 - physical offset needed to access virtual registers in host
 */
NvU32
gpuGetVirtRegPhysOffset_TU102(OBJGPU *pGpu)
{
    // No offset is needed, only in case of VF in SR-IOV
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
        return 0;
    else
        return DRF_BASE(NV_VIRTUAL_FUNCTION_FULL_PHYS_OFFSET);
}

/*!
 * @brief Returns the physical address width for the given @ref NV_ADDRESS_SPACE
 */
NvU32 gpuGetPhysAddrWidth_TU102
(
    OBJGPU          *pGpu,
    NV_ADDRESS_SPACE addrSp
)
{
    // Currently this function supports only sysmem addresses
    NV_ASSERT_OR_RETURN(ADDR_SYSMEM == addrSp, 0);

    return NV_CHIP_EXTENDED_SYSTEM_PHYSICAL_ADDRESS_BITS;
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
static const GPUCHILDPRESENT gpuChildrenPresent_TU102[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJGPUMON, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 9),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
};

static const GPUCHILDPRESENT gpuChildrenPresent_TU104[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJGPUMON, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 9),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
};

static const GPUCHILDPRESENT gpuChildrenPresent_TU106[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJGPUMON, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 9),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_TU102(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_TU102);
    return gpuChildrenPresent_TU102;
}

const GPUCHILDPRESENT *
gpuGetChildrenPresent_TU104(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_TU104);
    return gpuChildrenPresent_TU104;
}

const GPUCHILDPRESENT *
gpuGetChildrenPresent_TU106(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_TU106);
    return gpuChildrenPresent_TU106;
}

/*!
 * @brief Check if the DSM JT version is correct
 *
 * @param  pGpu    OBJGPU pointer
 *
 * @return NV_OK if DSM version matches otherwise NV_ERR_INVALID_PARAMETER
 */
NV_STATUS
gpuJtVersionSanityCheck_TU102
(
    OBJGPU *pGpu
)
{
    NV_STATUS status = NV_OK;

    if (!IS_SILICON(pGpu) || NV_IS_MODS)
    {
        goto gpuJtVersionSanityCheck_TU102_EXIT;
    }

    if (FLD_TEST_DRF(_JT_FUNC, _CAPS, _JT_ENABLED, _TRUE, pGpu->acpiMethodData.jtMethodData.jtCaps))
    {
        if (!(pGpu->acpiMethodData.jtMethodData.jtRevId == NV_JT_FUNC_CAPS_REVISION_ID_2_00))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "JT Version mismatch 0x%x\n", pGpu->acpiMethodData.jtMethodData.jtRevId);
            DBG_BREAKPOINT();
            status = NV_ERR_INVALID_PARAMETER;
            goto gpuJtVersionSanityCheck_TU102_EXIT;
        }
    }

gpuJtVersionSanityCheck_TU102_EXIT:
    return status;
}
