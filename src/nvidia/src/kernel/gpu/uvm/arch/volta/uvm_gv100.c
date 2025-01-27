/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief VOLTA specific HALs for UVM routines reside in this file
 */

#include "core/core.h"
#include "nvrm_registry.h"
#include "gpu/gpu.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/uvm/uvm.h"
#include "gpu/uvm/access_cntr_buffer.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/bus/kern_bus.h"
#include "rmapi/event.h"

#include "class/clc365.h"
#include "ctrl/ctrlc365.h"
#include "published/volta/gv100/dev_fb.h"

NV_STATUS
uvmSetupAccessCntrBuffer_GV100
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU64 vaddr;
    NV_STATUS status = NV_OK;

    // Return if guest RM is with no sriov
    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    if (!pUvm->pAccessCounterBuffers[accessCounterIndex].pUvmAccessCntrMemDesc)
    {
        return NV_ERR_INVALID_OBJECT_BUFFER;
    }

    status = kbusMapCpuInvisibleBar2Aperture_HAL(pGpu, pKernelBus, pUvm->pAccessCounterBuffers[accessCounterIndex].pUvmAccessCntrMemDesc,
                                                 &vaddr, pUvm->pAccessCounterBuffers[accessCounterIndex].pUvmAccessCntrMemDesc->Size, 0, GPU_GFID_PF);
    if (status != NV_OK)
    {
        return status;
    }
    pUvm->pAccessCounterBuffers[accessCounterIndex].bar2UvmAccessCntrBufferAddr = vaddr;

    uvmProgramWriteAccessCntrBufferAddress_HAL(pGpu, pUvm, accessCounterIndex, vaddr);
    uvmProgramAccessCntrBufferEnabled_HAL(pGpu, pUvm, accessCounterIndex, NV_FALSE);

    return NV_OK;
}

NV_STATUS
uvmDisableAccessCntr_GV100
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvBool  bIsErrorRecovery
)
{
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    RMTIMEOUT   timeout;
    NvU32       putPtr;
    NvU32       getPtr;
    NV_STATUS   status      = NV_OK;

    status = gpuSanityCheckRegisterAccess(pGpu, 0, NULL);
    if (status != NV_OK)
        return status;

    if (!bIsErrorRecovery && kgmmuTestAccessCounterWriteNak_HAL(pGpu, pKernelGmmu))
    {
        NV_PRINTF(LEVEL_ERROR,
            "Forcing bIsErrorRecovery = NV_TRUE because of WRITE_NACK.\n");
        bIsErrorRecovery = NV_TRUE;
    }

    uvmProgramAccessCntrBufferEnabled_HAL(pGpu, pUvm, accessCounterIndex, NV_FALSE);

    //
    // Check for any pending notifications which might be pending in pipe to ensure
    // they don't show up later when the buffer is enabled again. To ensure that HW sets the
    // correct notifications PUSHED status in priv, perform a read to ensure that EN == FALSE.
    // If PUSHED == TRUE, RM will check the PUT pointer and if updated, it will wait for valid
    // bit to show up for all packets and then reset the buffer
    //
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    if (!uvmIsAccessCntrBufferEnabled_HAL(pGpu, pUvm, accessCounterIndex))
    {
        while (!uvmIsAccessCntrBufferPushed_HAL(pGpu, pUvm, accessCounterIndex))
        {
            if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Timeout waiting for HW to write notification buffer.\n");
                DBG_BREAKPOINT();
                return NV_ERR_TIMEOUT;
            }
        }

        //
        // If called from error recovery, we can't wait for packet to show up as notification packets
        // could be the source of error
        //
        if (bIsErrorRecovery)
            goto done;

        // If PUT pointer is updated, wait for VALID packets to show up and reset the packets
        uvmReadAccessCntrBufferPutPtr_HAL(pGpu, pUvm, accessCounterIndex, &putPtr);
        uvmReadAccessCntrBufferGetPtr_HAL(pGpu, pUvm, accessCounterIndex, &getPtr);
        if (getPtr != putPtr)
        {
            MEMORY_DESCRIPTOR *pMemDesc = RMCFG_FEATURE_PLATFORM_GSP ?
                                          pUvm->pAccessCounterBuffers[accessCounterIndex].pUvmAccessCntrMemDesc :
                                          pUvm->pAccessCounterBuffers[accessCounterIndex].pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc;
            NvU8 *pAccessCntrBufferPage;
            NvU32 entriesPerPage = RM_PAGE_SIZE / NVC365_NOTIFY_BUF_SIZE;
            NvU32 pageSizeModBufSize = RM_PAGE_SIZE % NVC365_NOTIFY_BUF_SIZE;
            NvU32 maxEntryCount = pMemDesc->Size / NVC365_NOTIFY_BUF_SIZE;
            NvU32 inPageGetPtr;
            NvP64 pAddr;
            NvP64 pPriv;

            NV_ASSERT_OR_RETURN(pageSizeModBufSize == 0, NV_ERR_INVALID_OPERATION);

            // Map one buffer page and wait for packets to become valid
            status = memdescMap(pMemDesc, (getPtr / entriesPerPage) * RM_PAGE_SIZE, RM_PAGE_SIZE,
                                NV_TRUE, NV_PROTECT_READ_WRITE, &pAddr, &pPriv);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Failed to map access counter buffer while disabling it: %d\n",
                          status);
                return status;
            }

            while (getPtr != putPtr)
            {
                pAccessCntrBufferPage = (NvU8 *)pAddr;
                inPageGetPtr = getPtr % entriesPerPage;

                // Wait for an entry to be come valid
                while (!DRF_VAL_MW(C365, _NOTIFY_BUF_ENTRY, _VALID, (NvU32 *)&pAccessCntrBufferPage[inPageGetPtr * NVC365_NOTIFY_BUF_SIZE]))
                    osSchedule();

                portMemSet((void *)(&pAccessCntrBufferPage[inPageGetPtr * NVC365_NOTIFY_BUF_SIZE]), 0, NVC365_NOTIFY_BUF_SIZE);
                getPtr = ((getPtr + 1 == maxEntryCount) ? 0 : (getPtr + 1));

                // Map another page with entries to clear
                if (getPtr % entriesPerPage == 0)
                {
                    memdescUnmap(pMemDesc, NV_TRUE, osGetCurrentProcess(), pAddr, pPriv);
                    status = memdescMap(pMemDesc, (getPtr / entriesPerPage) * RM_PAGE_SIZE, RM_PAGE_SIZE,
                                        NV_TRUE, NV_PROTECT_READ_WRITE, &pAddr, &pPriv);
                    if (status != NV_OK)
                    {
                        NV_PRINTF(LEVEL_ERROR, "Failed to map access counter buffer while disabling it: %d\n",
                                  status);

                        // Write get progress so far, all entries in [get, put)
                        // are valid or will become valid.
                        uvmWriteAccessCntrBufferGetPtr_HAL(pGpu, pUvm, accessCounterIndex, getPtr);
                        return status;
                    }
                }
            }

            uvmWriteAccessCntrBufferGetPtr_HAL(pGpu, pUvm, accessCounterIndex, getPtr);

            memdescUnmap(pMemDesc, NV_TRUE, osGetCurrentProcess(), pAddr, pPriv);
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Failed disabling notification buffer.\n");
        DBG_BREAKPOINT();
        return NV_ERR_TIMEOUT;
    }

done:
    return NV_OK;
}

NV_STATUS
uvmUnloadAccessCntrBuffer_GV100
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex
)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    // Return if guest RM is with no sriov
    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    uvmDisableAccessCntr_HAL(pGpu, pUvm, accessCounterIndex, NV_FALSE);
    kbusUnmapCpuInvisibleBar2Aperture_HAL(pGpu, pKernelBus, pUvm->pAccessCounterBuffers[accessCounterIndex].pUvmAccessCntrMemDesc,
                                          pUvm->pAccessCounterBuffers[accessCounterIndex].bar2UvmAccessCntrBufferAddr, GPU_GFID_PF);
    pUvm->pAccessCounterBuffers[accessCounterIndex].bar2UvmAccessCntrBufferAddr = 0;

    return NV_OK;
}

NV_STATUS
uvmDestroyAccessCntrBuffer_GV100
(
    OBJGPU              *pGpu,
    OBJUVM              *pUvm,
    AccessCounterBuffer *pAccessCounterBuffer
)
{
    if(pUvm == NULL)
    {
        return NV_WARN_NULL_OBJECT;
    }

    // Return if guest RM is with no sriov
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    memdescFree(pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc);
    memdescDestroy(pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc);

    pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc = NULL;

    return NV_OK;
}

NV_STATUS
uvmInitAccessCntrBuffer_GV100
(
    OBJGPU              *pGpu,
    OBJUVM              *pUvm,
    AccessCounterBuffer *pAccessCounterBuffer
)
{
    NV_STATUS           status;
    NvU32               accessCntrBufferSize;
    MEMORY_DESCRIPTOR  *pUvmAccessCntrBufferDesc;
    NvP64               pAddr;
    NvP64               pPriv;
    MemoryManager      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelGmmu         *pKernelGmmu    = GPU_GET_KERNEL_GMMU(pGpu);

    NvU32 accessCntrBufferAperture = 0;
    NvU32 accessCntrBufferAttr = 0;

    // Return if guest RM is with no sriov
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    accessCntrBufferSize = uvmGetAccessCounterBufferSize_HAL(pGpu, pUvm, pAccessCounterBuffer->accessCounterIndex);
    accessCntrBufferAperture = ADDR_SYSMEM;
    accessCntrBufferAttr     = NV_MEMORY_CACHED;
    memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC_4, _UVM_FAULT_BUFFER_REPLAYABLE, pGpu->instLocOverrides4),
                           "UVM access counter", &accessCntrBufferAperture, &accessCntrBufferAttr);

    status = memdescCreate(&pUvmAccessCntrBufferDesc, pGpu, accessCntrBufferSize, 0,
                           NV_FALSE, accessCntrBufferAperture, accessCntrBufferAttr, MEMDESC_FLAGS_LOST_ON_SUSPEND);
    if (status != NV_OK)
    {
        return status;
    }

    //
    // GPU doesn't read accessCounter notification buffer memory, so if buffer is in sysmem,
    // ensure that GpuCacheAttr is set to UNCACHED as having a vol bit set in PTEs will ensure HUB
    // uses L2Bypass mode and it will save extra cycles to cache in L2 while MMU will write notification packets.
    //
    if (accessCntrBufferAperture == ADDR_SYSMEM &&
        pKernelGmmu->getProperty(pKernelGmmu, PDB_PROP_KGMMU_SYSMEM_FAULT_BUFFER_GPU_UNCACHED))
    {
        memdescSetGpuCacheAttrib(pUvmAccessCntrBufferDesc, NV_MEMORY_UNCACHED);
    }

    memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, pUvmAccessCntrBufferDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);
    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_44,
                    pUvmAccessCntrBufferDesc);
    if (status != NV_OK)
    {
        memdescDestroy(pUvmAccessCntrBufferDesc);
        return status;
    }

    memdescSetName(pGpu, pUvmAccessCntrBufferDesc, NV_RM_SURF_NAME_ACCESS_COUNTER_BUFFER, NULL);

    status = memdescMap(pUvmAccessCntrBufferDesc, 0,
                        memdescGetSize(pUvmAccessCntrBufferDesc), NV_TRUE,
                        NV_PROTECT_READ_WRITE, &pAddr, &pPriv);
    if (status != NV_OK)
    {
        memdescFree(pUvmAccessCntrBufferDesc);
        memdescDestroy(pUvmAccessCntrBufferDesc);
        return status;
    }
    portMemSet(NvP64_VALUE(pAddr), 0, memdescGetSize(pUvmAccessCntrBufferDesc));

    memdescUnmap(pUvmAccessCntrBufferDesc, NV_TRUE, osGetCurrentProcess(), pAddr, pPriv);

    pAccessCounterBuffer->pUvmAccessCntrAllocMemDesc = pUvmAccessCntrBufferDesc;

    return status;
}

NV_STATUS
uvmAccessCntrSetCounterLimit_GV100
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
    NvU32   accessCounterIndex,
    NvU32   type,
    NvU32   limit
)
{
    if (type == NVC365_CTRL_ACCESS_COUNTER_MIMC_LIMIT)
    {
        switch(limit)
        {
            case NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_NONE:
                GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_CONFIG, _MIMC_USE_LIMIT, _NONE);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_QTR:
                GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_CONFIG, _MIMC_USE_LIMIT, _QTR);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_HALF:
                GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_CONFIG, _MIMC_USE_LIMIT, _HALF);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_FULL:
                GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_CONFIG, _MIMC_USE_LIMIT, _FULL);
                break;
            default:
                return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else if (type == NVC365_CTRL_ACCESS_COUNTER_MOMC_LIMIT)
    {
        switch(limit)
        {
            case NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_NONE:
                GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_CONFIG, _MOMC_USE_LIMIT, _NONE);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_QTR:
                GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_CONFIG, _MOMC_USE_LIMIT, _QTR);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_HALF:
                GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_CONFIG, _MOMC_USE_LIMIT, _HALF);
                break;
            case NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_FULL:
                GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_CONFIG, _MOMC_USE_LIMIT, _FULL);
                break;
            default:
                return NV_ERR_INVALID_ARGUMENT;
        }
    }
    else
        return NV_ERR_INVALID_ARGUMENT;

    return NV_OK;
}
