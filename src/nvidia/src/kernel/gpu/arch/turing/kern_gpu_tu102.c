/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES
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
#include "gpu/bif/kernel_bif.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/mem_mgr/rm_page_size.h"
#include "jt.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gsp/kernel_gsp.h"

#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/hwproject.h"
#include "published/turing/tu102/dev_nv_xve.h"
#include "published/turing/tu102/dev_nv_xve3g_vf.h"
#include "published/turing/tu102/dev_gc6_island.h"
#include "published/turing/tu102/dev_gc6_island_addendum.h"

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
    pParams->bVfResizableBAR1Supported             = gpuIsVfResizableBAR1Supported(pGpu);

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
    GPU_CHILD_PRESENT(KernelHwpm, 1),
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
    GPU_CHILD_PRESENT(KernelHwpm, 1),
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
    GPU_CHILD_PRESENT(KernelHwpm, 1),
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

/*!
 * @brief: Return base offset of register group
 *
 * @param[in]  pGpu          OBJGPU pointer
 *             regBase       NvU32 value for requested register group
 * @param[out] offset        NvU32 pointer to return base offset
 *
 * @returns NV_OK if the register group is found.
 */
NV_STATUS
gpuGetRegBaseOffset_TU102(OBJGPU *pGpu, NvU32 regBase, NvU32 *pOffset)
{
    switch (regBase)
    {
        case NV_REG_BASE_USERMODE:
        {
            //
            // Baremetal from Turing will not use the NV_USERMODE_*
            // as - "NV_USERMODE is going away long term, and
            // doesn't support any of the new remap
            // type features". Since we would need the new features
            // such as worksubmission doorbell register interrupting GSP,
            // we will start using the physical registers through NV_VIRTUAL_FUNCTION
            // even on host.
            //
            *pOffset = GPU_GET_VREG_OFFSET(pGpu, DRF_BASE(NV_VIRTUAL_FUNCTION));

            return NV_OK;
        }
        default:
        {

            return NV_ERR_NOT_SUPPORTED;
        }
    }
}

/*!
 * @brief Check if register being accessed is within guest BAR0 space.
 *
 * @param[in] pGpu   OBJGPU pointer
 * @param[in] addr   Address being validated
 */
NV_STATUS
gpuSanityCheckVirtRegAccess_TU102
(
    OBJGPU *pGpu,
    NvU32   addr
)
{
    // Not applicable in PV mode
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_OK;
    }

    if ((addr >= DEVICE_BASE(NV_PCFG)) &&
        (addr < DEVICE_BASE(NV_PCFG) + PCIE_CONFIG_SPACE_SIZE))
    {
        return NV_OK;
    }

    // Check if address in NV_VIRTUAL_FUNCTION range, if not error out.
    if ((addr < DRF_EXTENT(NV_VIRTUAL_FUNCTION_PRIV)) ||
        ((addr >= DRF_BASE(NV_VIRTUAL_FUNCTION)) && (addr < DRF_EXTENT(NV_VIRTUAL_FUNCTION))))
    {
        return NV_OK;
    }

    return NV_ERR_INVALID_ADDRESS;
}

/*
 * @brief Function that checks if ECC error occurred by reading various count
 * registers/interrupt registers. This function is not floorsweeping-aware so
 * PRI errors are ignored
 */
NvBool
gpuCheckEccCounts_TU102
(
    OBJGPU *pGpu
)
{
    NvU32 dramCount = 0;
    NvU32 ltcCount = 0;
    NvU32 mmuCount = 0;
    NvU32 pcieCount = 0;

    kmemsysGetEccCounts_HAL(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu), &dramCount, &ltcCount);
    mmuCount += kgmmuGetEccCounts_HAL(pGpu, GPU_GET_KERNEL_GMMU(pGpu));
    pcieCount += kbifGetEccCounts_HAL(pGpu, GPU_GET_KERNEL_BIF(pGpu));
    pcieCount += kbusGetEccCounts_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu));

    // If counts > 0 or if poison interrupt pending, ECC error has occurred.
    if (((dramCount + ltcCount + mmuCount + pcieCount) != 0) || gpuCheckIfFbhubPoisonIntrPending_HAL(pGpu))
    {
        nvErrorLog_va((void *)pGpu, UNRECOVERABLE_ECC_ERROR_ESCAPE,
                      "An uncorrectable ECC error detected "
                      "(possible firmware handling failure) "
                      "DRAM:%d, LTC:%d, MMU:%d, PCIE:%d", dramCount, ltcCount, mmuCount, pcieCount);
        return NV_TRUE;
    }

    return NV_FALSE;
}

//
// This function checks for GFW boot completion status by reading
// NV_AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT_PROGRESS_COMPLETED bits and
// return true if GFW boot has completed.
//
// Either pGpu or pgc6VirtAddr should be not null.
// This function needs to be called in early init code-path where OBJGPU
// has not created. For that case, the NV_PGC6 base address will be mapped
// and pgc6VirtAddr will contain the virtual address for NV_PGC6.
// If pgc6VirtAddr is not null, then read the register with MEM_RD32,
// otherwise use the GPU_REG_RD32.
//
// The current GFW boot progress value will be returned in gfwBootProgressVal.
//
static NvBool
_gpuIsGfwBootCompleted_TU102
(
    OBJGPU  *pGpu,
    NvU8    *pgc6VirtAddr,
    NvU32   *gfwBootProgressVal
)
{
    NvU32 regVal;

    if (pgc6VirtAddr != NULL)
    {
        regVal = MEM_RD32(pgc6VirtAddr +
                          (NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK -
                           DEVICE_BASE(NV_PGC6)));
    }
    else
    {
        regVal = GPU_REG_RD32(pGpu, NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK);
    }

    //
    // Before reading the actual GFW_BOOT status register,
    // we want to check that FWSEC has lowered its PLM first.
    // If not then obviously it has not completed.
    //
    if (!FLD_TEST_DRF(_PGC6, _AON_SECURE_SCRATCH_GROUP_05_PRIV_LEVEL_MASK,
                      _READ_PROTECTION_LEVEL0, _ENABLE, regVal))
    {
        *gfwBootProgressVal = 0x0;
        return NV_FALSE;
    }

    if (pgc6VirtAddr != NULL)
    {
        regVal = MEM_RD32(pgc6VirtAddr +
                          (NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT -
                           DEVICE_BASE(NV_PGC6)));
    }
    else
    {
        regVal = GPU_REG_RD32(pGpu, NV_PGC6_AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT);
    }

    *gfwBootProgressVal = DRF_VAL(_PGC6, _AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT,
                                  _PROGRESS, regVal);

    return FLD_TEST_DRF(_PGC6, _AON_SECURE_SCRATCH_GROUP_05_0_GFW_BOOT,
                        _PROGRESS, _COMPLETED, regVal);
}

#define FWSECLIC_PROG_START_TIMEOUT             50000    // 50ms
#define FWSECLIC_PROG_COMPLETE_TIMEOUT          2000000  // 2s
#define GPU_GFW_BOOT_COMPLETION_TIMEOUT_US      (FWSECLIC_PROG_START_TIMEOUT + \
                                                 FWSECLIC_PROG_COMPLETE_TIMEOUT)
NV_STATUS
gpuWaitForGfwBootComplete_TU102
(
    OBJGPU    *pGpu
)
{
    NvU32        timeoutUs = GPU_GFW_BOOT_COMPLETION_TIMEOUT_US;
    NvU32        gfwBootProgressVal = 0;
    NV_STATUS    status = NV_OK;
    KernelGsp    *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);
    NvBool       bGfwBootCompleted = NV_FALSE;

    timeoutUs = gpuScaleTimeout(pGpu, timeoutUs);

    status = kflcnWaitForHalt_HAL(pGpu, pKernelFalcon, timeoutUs, GPU_TIMEOUT_FLAGS_OSTIMER);

    // Get GFW boot progress irrespective of falcon halt status
    bGfwBootCompleted = _gpuIsGfwBootCompleted_TU102(pGpu, NULL, &gfwBootProgressVal);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "GSP failed to halt with GFW_BOOT: (progress 0x%x)\n", gfwBootProgressVal);
        return status;
    }

    if (!bGfwBootCompleted)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to wait for GFW_BOOT: (progress 0x%x)\n", gfwBootProgressVal);
        return NV_ERR_NOT_READY;
    }

    return NV_OK;
}

//
// Workaround for Bug 3809777.
//
// This function is not created through HAL infrastructure. It needs to be
// called when OBJGPU is not created. HAL infrastructure can't be used for
// this case, so it has been added manually. It will be invoked directly by
// gpumgrIsDeviceMsixAllowed() after checking the GPU architecture.
//
// When driver is running inside guest in pass-through mode, check if MSI-X
// is enabled by reading NV_XVE_PRIV_MISC_1_CYA_HIDE_MSIX_CAP. The devinit
// can disable MSI-X capability, if configured. The hypervisor issues reset
// before launching VM. After reset, the MSI-X capability will be visible
// for some duration and then devinit hides the MSI-X capability. This
// devinit will run in the background. During this time, the hypervisor can
// assume that MSI-X capability is present in the GPU and configure the guest
// GPU PCIe device instance with MSI-X capability. When GPU tries to use the
// MSI-X interrupts later, then interrupt won't be triggered. To identify
// this case, wait for GPU devinit completion and check if MSI-X capability
// is not hidden.
//
NvBool gpuIsMsixAllowed_TU102
(
    RmPhysAddr bar0BaseAddr
)
{
    NvU8    *vAddr;
    NvU32    regVal;
    NvU32    timeUs = 0;
    NvU32    gfwBootProgressVal = 0;
    NvBool   bGfwBootCompleted = NV_FALSE;

    ct_assert(DRF_SIZE(NV_PGC6) <= RM_PAGE_SIZE);
    vAddr = osMapKernelSpace(bar0BaseAddr + DEVICE_BASE(NV_PGC6),
                             RM_PAGE_SIZE, NV_MEMORY_UNCACHED,
                             NV_PROTECT_READABLE);
    if (vAddr == NULL)
    {
        return NV_FALSE;
    }

    while (timeUs < GPU_GFW_BOOT_COMPLETION_TIMEOUT_US)
    {
        bGfwBootCompleted = _gpuIsGfwBootCompleted_TU102(NULL, vAddr, &gfwBootProgressVal);
        if (bGfwBootCompleted)
        {
            break;
        }

        osDelayUs(1000);
        timeUs += 1000;
    }

    osUnmapKernelSpace(vAddr, RM_PAGE_SIZE);
    if (!bGfwBootCompleted)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to wait for GFW_BOOT: (progress 0x%x)\n",
                  gfwBootProgressVal);
        return NV_FALSE;
    }

    vAddr = osMapKernelSpace(bar0BaseAddr + DEVICE_BASE(NV_PCFG) +
                             NV_XVE_PRIV_MISC_1, 4, NV_MEMORY_UNCACHED,
                             NV_PROTECT_READABLE);
    if (vAddr == NULL)
    {
        return NV_FALSE;
    }

    regVal = MEM_RD32(vAddr);
    osUnmapKernelSpace(vAddr, 4);

    return FLD_TEST_DRF(_XVE, _PRIV_MISC_1, _CYA_HIDE_MSIX_CAP, _FALSE, regVal);
}
