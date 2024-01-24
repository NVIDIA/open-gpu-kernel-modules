/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "vgpu/vgpu_events.h"

#include "published/turing/tu102/dev_bus.h"
#include "published/turing/tu102/dev_vm.h"

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

/*!
 * Previous bind sequence would just do a sysmembar after a flush.
 * Now the flushes don't guarantee anything for the BIND itself.
 * Just that previous reads/writes are complete.
 * We need to use the BIND_STATUS register now.
 * New procedure:
 * - Write NV_PBUS_BLOCK_(BAR1|BAR2|IFB)
 * - Poll NV_PBUS_BIND_STATUS to make sure the BIND completed.
 */
NV_STATUS
kbusBindBar2_TU102
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus,
    BAR2_MODE  bar2Mode
)
{
    NvU32             gfid;
    RMTIMEOUT         timeout;
    NvU32             temp;
    NvU32             value;
    NvU32             instBlkAperture = 0;
    NvU64             instBlkAddr     = 0;
    NV_STATUS         status          = NV_OK;
    NvBool            bIsModePhysical;
    MEMORY_DESCRIPTOR *pMemDesc;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    if (pKernelBus->bar2[gfid].bBootstrap &&
        (NULL != pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap) &&
        kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
    {
        pMemDesc = pKernelBus->bar2[gfid].pInstBlkMemDescForBootstrap;
    }
    else
    {
        pMemDesc = pKernelBus->bar2[gfid].pInstBlkMemDesc;
    }

    //
    // Bind BAR2 to virtual. Carefully.  We have not initialized PTEs yet. We will first
    // map the BAR2 PTEs into BAR2. This allows us to use the BAR2 interface to invalidate
    // the rest of the BAR2 PTEs.  WC memory writes are faster than single BAR0 writes
    // and this matters for RTL sim and emulation. DEBUG_CYA = OFF keeps the VBIOS
    // aperture in physical addressing.
    //
    bIsModePhysical = (BAR2_MODE_PHYSICAL == bar2Mode);

    if (!bIsModePhysical)
    {
        instBlkAperture = kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pMemDesc);
        instBlkAddr     = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
    }

    value = ((bIsModePhysical ? DRF_DEF(_PBUS, _BAR2_BLOCK, _TARGET, _VID_MEM) :
                                DRF_NUM(_PBUS, _BAR2_BLOCK, _TARGET, instBlkAperture)) |
             (bIsModePhysical ? DRF_DEF(_PBUS, _BAR2_BLOCK, _MODE, _PHYSICAL) :
                                DRF_DEF(_PBUS, _BAR2_BLOCK, _MODE, _VIRTUAL)) |
             (bIsModePhysical ? DRF_NUM(_PBUS, _BAR2_BLOCK, _PTR, 0x0) :
                                DRF_NUM(_PBUS, _BAR2_BLOCK, _PTR,
                                        NvU64_LO32(instBlkAddr >> GF100_BUS_INSTANCEBLOCK_SHIFT))) |
             DRF_DEF(_PBUS, _BAR2_BLOCK, _DEBUG_CYA, _OFF));

    {
        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_BAR2_BLOCK, value);
    }


    osFlushCpuWriteCombineBuffer();

    // Skip the wait if we are in the reset path (GPU most likely in a bad state)
    if (!IS_VIRTUAL(pGpu) && API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        return status;
    }

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    do
    {
        //
        // To avoid deadlocks and non-deterministic virtual address
        // translation behavior, after writing BAR2_BLOCK to bind BAR2 to a
        // virtual address space, SW must ensure that the bind has completed
        // prior to issuing any further BAR2 requests by polling for both
        // NV_PBUS_BIND_STATUS_BAR2_PENDING to return to EMPTY and
        // NV_PBUS_BIND_STATUS_BAR2_OUTSTANDING to return to FALSE.
        //
        // BAR2_PENDING indicates a Bar2 bind is waiting to be sent.
        // BAR2_OUTSTANDING indicates a Bar2 bind is outstanding to FB.
        //
        {
            temp = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_BIND_STATUS);
        }
        if (FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR2_PENDING, _EMPTY, temp) &&
            FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR2_OUTSTANDING, _FALSE, temp))
        {
            status = NV_OK;
            break;
        }

        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "timed out waiting for bar2 binding to complete\n");
            DBG_BREAKPOINT();
            break;
        }

        status = gpuCheckTimeout(pGpu, &timeout);
        osSpinLoop();
    } while (1);

    return status;
}

NvU64 kbusGetCpuInvisibleBar2BaseAdjust_TU102
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    const GMMU_FMT* pFmt = NULL;

    NV_ASSERT_OR_RETURN(pKernelGmmu != NULL, 0);
    pFmt = kgmmuFmtGetLatestSupportedFormat(pGpu, pKernelGmmu);
    NV_ASSERT_OR_RETURN(pFmt != NULL, 0);

    //
    // In RM-offload scenario, Kernel RM and Physical RM use their own GPU VA space respectively.
    //
    // The expectation is that the Physical RM base starts from the second PD entry of the topmost PD.
    //
    return mmuFmtEntryIndexVirtAddrLo(pFmt->pRoot, 0, 1 /* PD0[1] */);
}

/*!
 * Previous bind sequence would just do a sysmembar after a flush.
 * Now the flushes don't guarantee anything for the BIND itself.
 * Just that previous reads/writes are complete.
 * We need to use the BIND_STATUS register now.
 * New procedure:
 * - Write NV_PBUS_BLOCK_(BAR1|BAR2|IFB)
 * - Poll NV_PBUS_BIND_STATUS to make sure the BIND completed.
 */
NV_STATUS
kbusBar1InstBlkBind_TU102
(
    OBJGPU    *pGpu,
    KernelBus *pKernelBus
)
{
    KernelGmmu       *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32             gfid;
    NvU32             target;
    NvU32             temp;
    NvU32             ptr;
    RMTIMEOUT         timeout;
    NV_STATUS         status = NV_OK;
    NvBool            bIsModePhysical = NV_FALSE;
    NvBool            bBrokenFb = NV_FALSE;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    // Nothing to be done in guest in the paravirtualization case.
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    switch (kgmmuGetMemAperture(pKernelGmmu, pKernelBus->bar1[gfid].pInstBlkMemDesc))
    {
        case GMMU_APERTURE_VIDEO:
            target = NV_PBUS_BAR1_BLOCK_TARGET_VID_MEM;
            break;
        case GMMU_APERTURE_SYS_COH:
            target = NV_PBUS_BAR1_BLOCK_TARGET_SYS_MEM_COHERENT;
            break;
        case GMMU_APERTURE_SYS_NONCOH:
            target = NV_PBUS_BAR1_BLOCK_TARGET_SYS_MEM_NONCOHERENT;
            break;
        default:
            NV_ASSERT_OR_RETURN(0, NV_ERR_INVALID_STATE);
    }
    ptr = NvU64_LO32(pKernelBus->bar1[gfid].instBlockBase >> GF100_BUS_INSTANCEBLOCK_SHIFT);

    bIsModePhysical = kbusIsBar1PhysicalModeEnabled(pKernelBus);
    bBrokenFb = pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB);

    {
        GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_BAR1_BLOCK,
                (((bIsModePhysical && !bBrokenFb) ?
                                    DRF_DEF(_PBUS, _BAR1_BLOCK, _TARGET, _VID_MEM) :
                                    DRF_NUM(_PBUS, _BAR1_BLOCK, _TARGET, target)) |
                 (bIsModePhysical ? DRF_DEF(_PBUS, _BAR1_BLOCK, _MODE, _PHYSICAL) :
                                    DRF_DEF(_PBUS, _BAR1_BLOCK, _MODE, _VIRTUAL)) |
                 (bIsModePhysical ? DRF_NUM(_PBUS, _BAR1_BLOCK, _PTR, 0x0) :
                                    DRF_NUM(_PBUS, _BAR1_BLOCK, _PTR, ptr))));
    }

    osFlushCpuWriteCombineBuffer();

    // Skip the wait if we are in the reset path (GPU most likely in a bad state)
    if (!IS_VIRTUAL(pGpu) && API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        return status;
    }

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    do
    {
        //
        // To avoid deadlocks and non-deterministic virtual address
        // translation behavior, after writing BAR1_BLOCK to bind BAR1 to a
        // virtual address space, SW must ensure that the bind has completed
        // prior to issuing any further BAR1 requests by polling for both
        // NV_PBUS_BIND_STATUS_BAR1_PENDING to return to EMPTY and
        // NV_PBUS_BIND_STATUS_BAR1_OUTSTANDING to return to FALSE.
        //
        // BAR1_PENDING indicates a Bar1 bind is waiting to be sent.
        // BAR1_OUTSTANDING indicates a Bar1 bind is outstanding to FB.
        //
        {
            temp = GPU_VREG_RD32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_BIND_STATUS);
        }
        if (FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR1_PENDING, _EMPTY, temp) &&
            FLD_TEST_DRF(_PBUS, _BIND_STATUS, _BAR1_OUTSTANDING, _FALSE, temp))
        {
            status = NV_OK;
            break;
        }

        if (status == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "timed out waiting for bar1 binding to complete\n");
            DBG_BREAKPOINT();
            break;
        }
        status = gpuCheckTimeout(pGpu, &timeout);
        osSpinLoop();
    } while (1);

    return status;
}

/*!
 * @brief Returns BAR1 VA Size that needs to be reserved for BSOD Surface
 *
 *
 * @returns BAR1 VA Size that needs to be reserved
 */
NvU64
kbusGetBar1ResvdVA_TU102
(
    KernelBus *pKernelBus
)
{
    //
    // Turing+ supports 8K displays which needs at least 127MB of BAR1 VA with
    // 64KB/Big page size (used by KMD). Hence, doubling the prev size of 64MB
    //
    return NVBIT64(27); //128MB
}
