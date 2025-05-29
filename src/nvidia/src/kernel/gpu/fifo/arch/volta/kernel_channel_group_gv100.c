/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_channel_group.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"
#include "kernel/virtualization/hypervisor/hypervisor.h"
#include "gpu/mem_mgr/mem_desc.h"

#include "nvrm_registry.h"

#include "gpu/bus/kern_bus.h"

/*!
 * @brief Allocate buffer to save/restore faulting engine methods
 */
NV_STATUS
kchangrpAllocFaultMethodBuffers_GV100
(
    OBJGPU                      *pGpu,
    KernelChannelGroup          *pKernelChannelGroup
)
{
    NV_STATUS                    status         = NV_OK;
    NvU32                        bufSizeInBytes = 0;
    KernelFifo                  *pKernelFifo    = GPU_GET_KERNEL_FIFO(pGpu);
    MemoryManager               *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32                        runQueues      = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);
    NvU32                        index          = 0;
    NvU32                        faultBufApert  = ADDR_SYSMEM;
    NvU32                        faultBufAttr   = NV_MEMORY_CACHED;
    NvU64                        memDescFlags   = MEMDESC_FLAGS_LOST_ON_SUSPEND;
    HW_ENG_FAULT_METHOD_BUFFER  *pFaultMthdBuf  = NULL;
    NvU32                        gfid           = pKernelChannelGroup->gfid;
    TRANSFER_SURFACE             surf           = {0};
    NvBool                       bReUseInitMem  = pGpu->getProperty(pGpu, PDB_PROP_GPU_REUSE_INIT_CONTING_MEM);

    //
    // Allocate method buffer if applicable
    // For SR-IOV, Guest RM allocates the mthd buffers, no work done by host-RM
    // For SR-IOV HEAVY and legacy vGpu, mthd buffers allocated by host RM,
    // For GSP config, method buffer allocation is done by CPU-RM
    //
    // Skip method buffer allocation for the rest
    //
    if ((IS_GFID_VF(gfid) && !IS_SRIOV_HEAVY(pGpu)) ||  // SRIOV guest on Host
        RMCFG_FEATURE_PLATFORM_GSP ||                       // GSP-RM
        IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||               // legacy vgpu
        IS_SRIOV_HEAVY_GUEST(pGpu))                     // SRIOV-heavy guest
    {
        return NV_OK;
    }

    // Pre-reqs
    NV_ASSERT_OR_RETURN((pKernelChannelGroup->pMthdBuffers != NULL), NV_ERR_INVALID_STATE);

    // Calculate size of buffer
    NV_ASSERT_OK_OR_RETURN(gpuGetCeFaultMethodBufferSize(pGpu, &bufSizeInBytes));
    NV_ASSERT((bufSizeInBytes > 0));

    if (IS_SRIOV_HEAVY_HOST(pGpu))
    {
        //
        // In case of SRIOV heavy mode host RM is allocating fault method buffers
        // on behalf of guest. As VF channels cannot use sysmem allocated in the
        // host, force fault buffer aperture to vid mem.
        //
        faultBufApert = ADDR_FBMEM;
        faultBufAttr  = NV_MEMORY_CACHED;
        memDescFlags  |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }
    else
    {
        // Get the right aperture/attribute
        faultBufApert = ADDR_SYSMEM;

        faultBufAttr  = NV_MEMORY_CACHED;
        memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC_3, _FAULT_METHOD_BUFFER, pGpu->instLocOverrides3),
                               "fault method buffer", &faultBufApert, &faultBufAttr);
        if (faultBufApert == ADDR_FBMEM)
            memDescFlags  |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
    }

    // Allocate buffer for each runqueue
    for (index = 0; index < runQueues; index++)
    {
        pFaultMthdBuf = &(pKernelChannelGroup->pMthdBuffers[index]);

retryInFB:
        // Allocate and initialize MEMDESC
        status = memdescCreate(&(pFaultMthdBuf->pMemDesc), pGpu, bufSizeInBytes, 0,
                               NV_TRUE, faultBufApert, faultBufAttr, memDescFlags);
        if (status != NV_OK)
        {
            DBG_BREAKPOINT();
            goto fail;
        }

        memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_34,
                    pFaultMthdBuf->pMemDesc);
        if (status != NV_OK)
        {
            memdescDestroy(pFaultMthdBuf->pMemDesc);
            pFaultMthdBuf->pMemDesc = NULL;
            if (bReUseInitMem && (faultBufApert == ADDR_SYSMEM))
            {
                 faultBufApert = ADDR_FBMEM;
                 memDescFlags  |= MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE;
                 goto retryInFB;
            }
            DBG_BREAKPOINT();
            goto fail;
        }

        memdescSetName(pGpu, pFaultMthdBuf->pMemDesc, NV_RM_SURF_NAME_CE_FAULT_METHOD_BUFFER, NULL);

        surf.pMemDesc = pFaultMthdBuf->pMemDesc;
        surf.offset = 0;

        NV_ASSERT_OK_OR_RETURN(
            memmgrMemSet(pMemoryManager, &surf, 0, bufSizeInBytes,
                         TRANSFER_FLAGS_NONE));

        pFaultMthdBuf->bar2Addr = 0;
    }

    return status;

fail:
    kchangrpFreeFaultMethodBuffers_HAL(pGpu, pKernelChannelGroup);
    return status;
}

/*!
 * @brief Free method buffers
 */
NV_STATUS
kchangrpFreeFaultMethodBuffers_GV100
(
    OBJGPU             *pGpu,
    KernelChannelGroup *pKernelChannelGroup
)
{
    NV_STATUS                   status         = NV_OK;
    HW_ENG_FAULT_METHOD_BUFFER *pFaultMthdBuf  = NULL;
    KernelFifo                 *pKernelFifo    = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32                       runQueues      = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);
    NvU32                       index          = 0;
    NvU32                       gfid           = pKernelChannelGroup->gfid;


    //
    // Free the method buffer if applicable
    // For SR-IOV, Guest RM allocates the mthd buffers but later RPCs into the
    // host, and populates the data structure, but it should be free-d only by
    // guest RM.
    // For SR-IOV HEAVY and legacy vGpu, mthd buffers should be free-d by host RM,
    // For GSP config, we need to free the method buffer in GSP-RM
    //
    // Skip free for the rest
    //
    if ((IS_GFID_VF(gfid) && !IS_SRIOV_HEAVY(pGpu)) ||  // SRIOV guest on Host
        IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||               // legacy vgpu
        IS_SRIOV_HEAVY_GUEST(pGpu))                     // SRIOV-heavy guest
    {
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN((pKernelChannelGroup->pMthdBuffers != NULL), NV_ERR_INVALID_STATE);

    // Free method buffer memdesc if allocated
    for (index = 0; index < runQueues; index++)
    {
        pFaultMthdBuf = &((pKernelChannelGroup->pMthdBuffers)[index]);
        if ((pFaultMthdBuf != NULL) && (pFaultMthdBuf->pMemDesc != NULL))
        {
            // Free the memory
            memdescFree(pFaultMthdBuf->pMemDesc);
            memdescDestroy(pFaultMthdBuf->pMemDesc);
            pFaultMthdBuf->pMemDesc = NULL;
        }
    }

    return status;
}

/*!
 * @brief Map method buffer to invisible BAR2 region
 */
NV_STATUS
kchangrpMapFaultMethodBuffers_GV100
(
    OBJGPU                      *pGpu,
    KernelChannelGroup          *pKernelChannelGroup,
    NvU32                        runqueue
)
{
    NvU32                        gfid           = pKernelChannelGroup->gfid;
    MemoryManager               *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelFifo                  *pKernelFifo    = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32                        runQueues      = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);
    KernelBus                   *pKernelBus     = GPU_GET_KERNEL_BUS(pGpu);
    HW_ENG_FAULT_METHOD_BUFFER  *pFaultMthdBuf  = NULL;

    //
    // Map method buffer to invisible BAR2 if applicable
    // For SR-IOV, Guest RM maps the mthd buffers, no work done by host-RM
    // For SR-IOV HEAVY and legacy vGpu, mthd buffers mapped to BAR2 by host RM,
    // For GSP config, method buffer BAR2 mapping is done by GSP-RM
    //
    // Skip method buffer allocation for the rest
    //
    if ((IS_GFID_VF(gfid) && !IS_SRIOV_HEAVY(pGpu)) ||  // SRIOV guest on Host
        IS_GSP_CLIENT(pGpu) ||                          // CPU-RM
        IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||               // legacy vgpu
        IS_SRIOV_HEAVY_GUEST(pGpu))                     // SRIOV-heavy guest
    {
        return NV_OK;
    }

    // Pre-reqs
    NV_ASSERT_OR_RETURN((pKernelChannelGroup->pMthdBuffers != NULL), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN((runqueue < runQueues), NV_ERR_INVALID_STATE);

    // Get method buffer handle
    pFaultMthdBuf = &(pKernelChannelGroup->pMthdBuffers[runqueue]);

    // Skip mapping if memdesc not allocated or if bar2 mapping has already been done
    if ((pFaultMthdBuf->pMemDesc == NULL) || (pFaultMthdBuf->bar2Addr != 0))
    {
        return NV_OK;
    }

    memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, pFaultMthdBuf->pMemDesc,
                                 AT_GPU, RM_ATTR_PAGE_SIZE_4KB);

    // Map the buffer to BAR2 invisible region
    NV_ASSERT_OK_OR_RETURN(kbusMapCpuInvisibleBar2Aperture_HAL(pGpu, pKernelBus,
                                                               pFaultMthdBuf->pMemDesc,
                                                               &(pFaultMthdBuf->bar2Addr),
                                                               pFaultMthdBuf->pMemDesc->Size,
                                                               0,
                                                               gfid));

    NV_PRINTF(LEVEL_INFO,
              "Allocating Method buffer with Bar2Addr LO 0x%08x Bar2Addr "
              "HI 0x%08x runqueue 0x%0x\n",
              NvU64_LO32(pFaultMthdBuf->bar2Addr),
              NvU64_HI32(pFaultMthdBuf->bar2Addr), runqueue);

    return NV_OK;
}

/*!
 * @brief Unmap method buffers from CPU invisible BAR2
 */
NV_STATUS
kchangrpUnmapFaultMethodBuffers_GV100
(
    OBJGPU             *pGpu,
    KernelChannelGroup *pKernelChannelGroup,
    NvU32               runqueue
)
{
    NvU32                       gfid           = pKernelChannelGroup->gfid;
    KernelBus                  *pKernelBus     = GPU_GET_KERNEL_BUS(pGpu);
    KernelFifo                 *pKernelFifo    = GPU_GET_KERNEL_FIFO(pGpu);
    NvU32                       runQueues      = kfifoGetNumRunqueues_HAL(pGpu, pKernelFifo);
    HW_ENG_FAULT_METHOD_BUFFER *pFaultMthdBuf  = NULL;

    //
    // Unmap the method buffer if applicable
    // For SR-IOV, Guest RM maps and is unmapped only by guest-RM
    // For SR-IOV HEAVY and legacy vGpu, mthd buffers should be unmapped by host RM,
    // For GSP config, method buffer BAR2 unmapping is done on GSP-RM
    //
    // Skip unmap for the rest
    //
    if ((IS_GFID_VF(gfid) && !IS_SRIOV_HEAVY(pGpu)) ||   // SRIOV guest on Host
        IS_GSP_CLIENT(pGpu) ||                           // CPU-RM
        IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||                // legacy vgpu
        IS_SRIOV_HEAVY_GUEST(pGpu))                      // SRIOV-heavy guest
    {
        return NV_OK;
    }

    // Pre-reqs
    NV_ASSERT_OR_RETURN((pKernelChannelGroup->pMthdBuffers != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((runqueue < runQueues), NV_ERR_INVALID_STATE);

    // Get method buffer handle
    pFaultMthdBuf = &(pKernelChannelGroup->pMthdBuffers[runqueue]);

    // TODO: Check to be removed as part of fix for Bug 200691429
    if (!RMCFG_FEATURE_PLATFORM_GSP)
        NV_ASSERT_OR_RETURN((pFaultMthdBuf->pMemDesc != NULL), NV_ERR_INVALID_STATE);

    // Unmap method buffer from bar2 invisible region and reset bar2addr
    if (pFaultMthdBuf->bar2Addr != 0)
    {
        kbusUnmapCpuInvisibleBar2Aperture_HAL(pGpu, pKernelBus, pFaultMthdBuf->pMemDesc,
                                              pFaultMthdBuf->bar2Addr, gfid);
        pFaultMthdBuf->bar2Addr = 0;
    }

    return NV_OK;
}
