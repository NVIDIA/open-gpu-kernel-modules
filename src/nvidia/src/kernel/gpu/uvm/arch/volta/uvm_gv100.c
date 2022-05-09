/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvRmReg.h"
#include "gpu/gpu.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/uvm/uvm.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/bus/kern_bus.h"
#include "rmapi/event.h"

#include "class/clc365.h"
#include "ctrl/ctrlc365.h"
#include "published/volta/gv100/dev_fb.h"

NV_STATUS
uvmSetupAccessCntrBuffer_GV100
(
    OBJGPU *pGpu,
    OBJUVM *pUvm
)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32 accessCntrBufferHi = 0;
    NvU32 accessCntrBufferLo = 0;
    NvU64 vaddr;
    NV_STATUS status = NV_OK;

    // Return if guest RM is with no sriov
    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    if (!pUvm->accessCntrBuffer.pUvmAccessCntrMemDesc)
    {
        return NV_ERR_INVALID_OBJECT_BUFFER;
    }

    status = kbusMapCpuInvisibleBar2Aperture_HAL(pGpu, pKernelBus, pUvm->accessCntrBuffer.pUvmAccessCntrMemDesc,
                                                 &vaddr, pUvm->accessCntrBuffer.pUvmAccessCntrMemDesc->Size, 0, GPU_GFID_PF);
    if (status != NV_OK)
    {
        return status;
    }
    pUvm->accessCntrBuffer.bar2UvmAccessCntrBufferAddr = vaddr;

    accessCntrBufferHi = NvU64_HI32(pUvm->accessCntrBuffer.bar2UvmAccessCntrBufferAddr);
    accessCntrBufferLo = NvU64_LO32(pUvm->accessCntrBuffer.bar2UvmAccessCntrBufferAddr);

    uvmWriteAccessCntrBufferHiReg_HAL(pGpu, pUvm, accessCntrBufferHi);
    uvmWriteAccessCntrBufferLoReg_HAL(pGpu, pUvm, accessCntrBufferLo);

    return NV_OK;
}

NV_STATUS
uvmDisableAccessCntr_GV100
(
    OBJGPU *pGpu,
    OBJUVM *pUvm,
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

    uvmWriteAccessCntrBufferLoReg_HAL(pGpu, pUvm,
        FLD_SET_DRF( _PFB_NISO, _ACCESS_COUNTER_NOTIFY_BUFFER_LO, _EN, _FALSE,
                  uvmReadAccessCntrBufferLoReg_HAL(pGpu, pUvm)));

    //
    // Check for any pending notifications which might be pending in pipe to ensure
    // they don't show up later when the buffer is enabled again. To ensure that HW sets the
    // correct notifications PUSHED status in priv, perform a read to ensure that EN == FALSE.
    // If PUSHED == TRUE, RM will check the PUT pointer and if updated, it will wait for valid
    // bit to show up for all packets and then reset the buffer
    //
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    if (FLD_TEST_DRF(_PFB_NISO, _ACCESS_COUNTER_NOTIFY_BUFFER_LO, _EN, _FALSE,
                      uvmReadAccessCntrBufferLoReg_HAL(pGpu, pUvm)))
    {
        while (FLD_TEST_DRF(_PFB_NISO, _ACCESS_COUNTER_NOTIFY_BUFFER_INFO, _PUSHED, _FALSE,
                            uvmReadAccessCntrBufferInfoReg_HAL(pGpu, pUvm)))
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
        uvmReadAccessCntrBufferPutPtr_HAL(pGpu, pUvm, &putPtr);
        uvmReadAccessCntrBufferGetPtr_HAL(pGpu, pUvm, &getPtr);
        if (getPtr != putPtr)
        {
            MEMORY_DESCRIPTOR *pMemDesc = IS_GSP_CLIENT(pGpu) ?
                                          pUvm->accessCntrBuffer.pUvmAccessCntrAllocMemDesc :
                                          pUvm->accessCntrBuffer.pUvmAccessCntrMemDesc;
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
                NV_PRINTF(LEVEL_ERROR, "Failed to map access counter buffer while disabling it.\n");
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
                        NV_PRINTF(LEVEL_ERROR, "Failed to map access counter buffer while disabling it.\n");
                        return status;
                    }
                }
            }

            uvmWriteAccessCntrBufferGetPtr_HAL(pGpu, pUvm, getPtr);

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
uvmUnloadAccessCntrBuffer_GV100(OBJGPU *pGpu, OBJUVM *pUvm)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);

    // Return if guest RM is with no sriov
    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    uvmDisableAccessCntr_HAL(pGpu, pUvm, NV_FALSE);
    kbusUnmapCpuInvisibleBar2Aperture_HAL(pGpu, pKernelBus, pUvm->accessCntrBuffer.pUvmAccessCntrMemDesc,
                                          pUvm->accessCntrBuffer.bar2UvmAccessCntrBufferAddr, GPU_GFID_PF);
    pUvm->accessCntrBuffer.bar2UvmAccessCntrBufferAddr = 0;

    return NV_OK;
}

NV_STATUS
uvmDestroyAccessCntrBuffer_GV100(OBJGPU *pGpu, OBJUVM *pUvm)
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

    memdescFree(pUvm->accessCntrBuffer.pUvmAccessCntrAllocMemDesc);
    memdescDestroy(pUvm->accessCntrBuffer.pUvmAccessCntrAllocMemDesc);

    pUvm->accessCntrBuffer.pUvmAccessCntrAllocMemDesc = NULL;
    pUvm->accessCntrBuffer.accessCntrBufferSize = 0;

    return NV_OK;
}

NV_STATUS
uvmInitAccessCntrBuffer_GV100(OBJGPU *pGpu, OBJUVM *pUvm)
{
    NV_STATUS           status;
    MEMORY_DESCRIPTOR  *pUvmAccessCntrBufferDesc;
    NvP64               pAddr;
    NvP64               pPriv;
    MemoryManager      *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelGmmu         *pKernelGmmu    = GPU_GET_KERNEL_GMMU(pGpu);

    NvU32 accessCntrBufferAperture = 0;
    NvU32 accessCntrBufferAttr = 0;
    NV2080_CTRL_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE_PARAMS getSizeParams = {0};

    // Return if guest RM is with no sriov
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        return NV_OK;
    }

    // Issue control to fetch buffer size from physical
    status = pUvm->pRmApi->Control(pUvm->pRmApi,
                                   pUvm->hClient,
                                   pUvm->hSubdevice,
                                   NV2080_CTRL_CMD_INTERNAL_UVM_GET_ACCESS_CNTR_BUFFER_SIZE,
                                   &getSizeParams,
                                   sizeof(getSizeParams));
    if (status != NV_OK)
    {
        return status;
    }

    pUvm->accessCntrBuffer.accessCntrBufferSize = getSizeParams.bufferSize;

    accessCntrBufferAperture = ADDR_SYSMEM;
    accessCntrBufferAttr     = NV_MEMORY_CACHED;
    memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC_4, _UVM_FAULT_BUFFER_REPLAYABLE, pGpu->instLocOverrides4),
                           "UVM access counter", &accessCntrBufferAperture, &accessCntrBufferAttr);

    status = memdescCreate(&pUvmAccessCntrBufferDesc, pGpu, pUvm->accessCntrBuffer.accessCntrBufferSize, 0,
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

    status = memdescAlloc(pUvmAccessCntrBufferDesc);
    if (status != NV_OK)
    {
        memdescDestroy(pUvmAccessCntrBufferDesc);
        return status;
    }

    memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, pUvmAccessCntrBufferDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);

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

    pUvm->accessCntrBuffer.pUvmAccessCntrAllocMemDesc = pUvmAccessCntrBufferDesc;

    return status;
}

NV_STATUS
uvmResetAccessCntrBuffer_GV100(OBJGPU *pGpu, OBJUVM *pUvm, NvU32 counterType)
{
    switch(counterType)
    {
        case NVC365_CTRL_ACCESS_COUNTER_TYPE_ALL:
            GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_NOTIFY_BUFFER_CLR, _ALL_COUNTERS, _CLR);
            break;
        case NVC365_CTRL_ACCESS_COUNTER_TYPE_MIMC:
            GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_NOTIFY_BUFFER_CLR, _MIMC, _CLR);
            break;
        case NVC365_CTRL_ACCESS_COUNTER_TYPE_MOMC:
            GPU_FLD_WR_DRF_DEF(pGpu, _PFB_NISO, _ACCESS_COUNTER_NOTIFY_BUFFER_CLR, _MOMC, _CLR);
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }
    return NV_OK;
}

NV_STATUS
uvmAccessCntrSetCounterLimit_GV100(OBJGPU *pGpu, OBJUVM *pUvm, NvU32 type, NvU32 limit)
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
