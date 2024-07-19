/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/********************************* VMA Manager *****************************\
*                                                                           *
*   The VirtMemAllocator is managed in this module.  All priviledged        *
*   state and object interaction is handled here.                           *
*                                                                           *
****************************************************************************/

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "rmapi/control.h"
#include "gpu/mem_mgr/virt_mem_allocator_common.h"
#include "os/os.h"
#include "core/system.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "diagnostics/profiler.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/gpu_vaspace.h"
#include "mem_mgr/virtual_mem.h"
#include "class/cl0000.h"
#include "class/cl90f1.h" // FERMI_VASPACE_A
#include "ctrl/ctrl0080/ctrl0080dma.h"
#include "ctrl/ctrl208f/ctrl208fdma.h"
#include "vgpu/rpc.h"
#include "core/locks.h"
#include "virtualization/kernel_hostvgpudeviceapi.h"
#include "gpu/subdevice/subdevice_diag.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/bus/kern_bus.h"
#include "platform/sli/sli.h"
#include "containers/eheap_old.h"

/*!
 * @brief Allocate mapping.
 *
* @todo Update function comment.
 * Please update function description and argument comments
 * if you do understand what function does and arguments mean.
 * Below is just a template for you.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] pDma              VirtMemAllocator pointer
 * @param[in] pVirtualMemory    VirtualMemory pointer
 * @param[in] pMemory           Memory object to map
 * @param[in] pDmaMappingInfo   CLI_DMA_MAPPING_INFO pointer
 *
 * @returns NV_STATUS status = NV_OK on success, or status upon failure.
 */
NV_STATUS
dmaAllocMap_IMPL
(
    OBJGPU               *pGpu,
    VirtMemAllocator     *pDma,
    OBJVASPACE           *pVAS,
    VirtualMemory        *pVirtualMemory,
    Memory               *pMemory,
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo
)
{
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    NV_STATUS         status = NV_OK;
    NvU32             p2p;
    NvU64             vaddr;
    NvU32             dmaAllocMapFlag;
    NvU64             baseVirtAddr;
    NvU64             virtSize;
    NvU32             swizzId = KMIGMGR_SWIZZID_INVALID;

    CLI_DMA_ALLOC_MAP_INFO  mapInfo;

    NV_ASSERT(pVirtualMemory != NULL);

    p2p = DRF_VAL(OS46, _FLAGS, _P2P_ENABLE, pDmaMappingInfo->Flags);

    //
    // By default any fabric memory should be mapped as peer memory. So, don't honor
    // any P2P flags.
    //
    if (
        (memdescGetAddressSpace(pDmaMappingInfo->pMemDesc) == ADDR_FABRIC_MC) ||
        (memdescGetAddressSpace(pDmaMappingInfo->pMemDesc) == ADDR_FABRIC_V2))
    {
        p2p  = 0;
    }

    if ((p2p == NVOS46_FLAGS_P2P_ENABLE_NOSLI) && IsSLIEnabled(pGpu))
    {
        NvU32 deviceInst  = gpuGetDeviceInstance(pGpu);
        NvU32 subDevIdTgt = DRF_VAL(OS46, _FLAGS, _P2P_SUBDEV_ID_TGT, pDmaMappingInfo->Flags);

        pGpu = gpumgrGetGpuFromSubDeviceInst(deviceInst, subDevIdTgt);
        gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
        pDma = GPU_GET_DMA(pGpu);
    }

    //
    // Temporarily set _DMA_UNICAST_REUSE_ALLOC for NV50_MEMORY_VIRTUAL since that
    // class has already assigned VA space and allocated PTEs.
    //
    dmaAllocMapFlag = pDmaMappingInfo->Flags;
    if (pVirtualMemory->bReserveVaOnAlloc)
        dmaAllocMapFlag = FLD_SET_DRF(OS46, _FLAGS, _DMA_UNICAST_REUSE_ALLOC, _TRUE, dmaAllocMapFlag);

    //
    // Calculate the virtual address of the mapping.
    //
    virtmemGetAddressAndSize(pVirtualMemory, &baseVirtAddr, &virtSize);
    if (FLD_TEST_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE, pDmaMappingInfo->Flags))
    {
        // Fixed offset indicates an absolute virtual address.
        vaddr = pDmaMappingInfo->DmaOffset;
    }
    else
    {
        // Otherwise the offset is relative to the target virtual allocation.
        vaddr = baseVirtAddr + pDmaMappingInfo->DmaOffset;
    }

    //
    // Check the result is within the bounds of the target virtual allocation.
    //
    // Only perform this check for mappings to existing virtual memory.
    // For CTXDMA case this check is meaningless since the [IN] dmaOffset will be garbage.
    //
    if (FLD_TEST_DRF(OS46, _FLAGS, _DMA_UNICAST_REUSE_ALLOC, _TRUE, dmaAllocMapFlag))
    {
        NV_ASSERT_OR_RETURN(vaddr >= baseVirtAddr, NV_ERR_INVALID_OFFSET);
        NV_ASSERT_OR_RETURN(vaddr < (baseVirtAddr + virtSize), NV_ERR_INVALID_OFFSET);
    }

    mapInfo.pVirtualMemory  = pVirtualMemory;
    mapInfo.pMemory         = pMemory;
    mapInfo.pDmaMappingInfo = pDmaMappingInfo;

    if ((pKernelMIGManager != NULL) && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager))
    {
        RsClient *pClient = RES_GET_CLIENT(pVirtualMemory);
        NvHandle hDevice = RES_GET_PARENT_HANDLE(pVirtualMemory);
        MIG_INSTANCE_REF ref;
        Device *pDevice;

        NV_ASSERT_OK_OR_RETURN(deviceGetByHandle(pClient, hDevice, &pDevice));

        NV_ASSERT_OK_OR_RETURN(kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref));
        swizzId = ref.pKernelMIGGpuInstance->swizzId;
    }

    status = dmaAllocMapping_HAL(pGpu,
                                 pDma,
                                 pVAS,
                                 pDmaMappingInfo->pMemDesc,
                                 &vaddr,
                                 dmaAllocMapFlag,  // Use locally updated dmaAllocMapFlag
                                 &mapInfo,
                                 swizzId);

    if (status == NV_OK)
    {
        pDmaMappingInfo->DmaOffset = vaddr;
    }

    if ((p2p == NVOS46_FLAGS_P2P_ENABLE_NOSLI) && IsSLIEnabled(pGpu))
    {
        pGpu = gpumgrGetParentGPU(pGpu);
        gpumgrSetBcEnabledStatus(pGpu, NV_TRUE);
    }

    return status;
}

/*!
 * @brief Free mapping.
 *
 * @todo Update function comment.
 * Please update function description and argument comments
 * if you do understand what function does and arguments mean.
 * Below is just a template for you.
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] pDma              VirtMemAllocator pointer
 * @param[in] pVirtualMemory    VirtualMemory pointer
 * @param[in] pDmaMappingInfo   CLI_DMA_MAPPING_INFO pointer
 *
 * @returns NV_STATUS status = NV_OK on success, or status upon failure.
 */
NV_STATUS
dmaFreeMap_IMPL
(
    OBJGPU               *pGpu,
    VirtMemAllocator     *pDma,
    OBJVASPACE           *pVAS,
    VirtualMemory        *pVirtualMemory,
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo,
    NvU32                 flags
)
{
    NV_STATUS status = NV_OK;
    NvU32     p2p, subDevIdTgt;
    NvU32     deviceInst = gpuGetDeviceInstance(pGpu);
    CLI_DMA_ALLOC_MAP_INFO mapInfo;

    p2p =  DRF_VAL(OS46, _FLAGS, _P2P_ENABLE, pDmaMappingInfo->Flags);
    subDevIdTgt = DRF_VAL(OS46, _FLAGS, _P2P_SUBDEV_ID_TGT, pDmaMappingInfo->Flags);

    if ((p2p == NVOS46_FLAGS_P2P_ENABLE_NOSLI) && IsSLIEnabled(pGpu))
    {
        pGpu = gpumgrGetGpuFromSubDeviceInst(deviceInst, subDevIdTgt);
        gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
        pDma = GPU_GET_DMA(pGpu);
    }

    mapInfo.pVirtualMemory  = pVirtualMemory;
    mapInfo.pMemory         = NULL;
    mapInfo.pDmaMappingInfo = pDmaMappingInfo;

    // free mapping in context dma
    status = dmaFreeMapping_HAL(pGpu, pDma, pVAS, pDmaMappingInfo->DmaOffset,
                                pDmaMappingInfo->pMemDesc, flags, &mapInfo);

    if ((p2p == NVOS46_FLAGS_P2P_ENABLE_NOSLI) && IsSLIEnabled(pGpu))
    {
        pGpu = gpumgrGetParentGPU(pGpu);
        gpumgrSetBcEnabledStatus(pGpu, NV_TRUE);
    }

    return status;
}

//
// deviceCtrlCmdDmaGetPteInfo_IMPL
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaGetPteInfo_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams
)
{
    OBJGPU         *pGpu = GPU_RES_GET_GPU(pDevice);
    OBJVASPACE     *pVAS = NULL;
    NV_STATUS       status = NV_OK;
    CALL_CONTEXT   *pCallContext = resservGetTlsCallContext();
    RmCtrlParams   *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDevice), pRmCtrlParams->hObject,
                                                            pParams->hVASpace, &pVAS));

    status = vaspaceGetPteInfo(pVAS, pGpu, pParams, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "vaspaceGetPteInfo failed\n");
    }

    return status;
}

//
// deviceCtrlCmdDmaUpdatePde2_IMPL
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaUpdatePde2_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *pParams
)
{
    OBJGPU      *pGpu       = GPU_RES_GET_GPU(pDevice);;
    OBJVASPACE  *pVAS       = NULL;
    NV_STATUS    status     = NV_OK;
    NvBool       bBcState   = NV_TRUE;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (pCallContext->secInfo.privLevel < RS_PRIV_LEVEL_KERNEL)
    {
            return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        NV_RM_RPC_UPDATE_PDE_2(pGpu, RES_GET_CLIENT_HANDLE(pDevice), RES_GET_HANDLE(pDevice), pParams, status);
        return status;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDevice), RES_GET_HANDLE(pDevice),
                                                            pParams->hVASpace, &pVAS));

    // Force to UC if client passed in sub-device handle.
    if (0 != pParams->subDeviceId)
    {
        bBcState = gpumgrGetBcEnabledStatus(pGpu);

        pGpu = gpumgrGetGpuFromSubDeviceInst(gpuGetDeviceInstance(pGpu),
                                             pParams->subDeviceId - 1);
        gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        OBJGVASPACE *pGVAS = dynamicCast(pVAS, OBJGVASPACE);
        if (pGVAS == NULL)
        {
            status = NV_ERR_NOT_SUPPORTED;
            SLI_LOOP_BREAK;
        }
        status = gvaspaceUpdatePde2(pGVAS, pGpu, pParams);
        if (status != NV_OK)
        {
            SLI_LOOP_BREAK;
        }
    }
    SLI_LOOP_END

    // Restore BC if required.
    if (0 != pParams->subDeviceId)
    {
        gpumgrSetBcEnabledStatus(pGpu, bBcState);
    }

    return status;
}

//
// deviceCtrlCmdDmaSetVASpaceSize_IMPL
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaSetVASpaceSize_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS *pParams
)
{
    OBJGPU          *pGpu    = GPU_RES_GET_GPU(pDevice);
    OBJVASPACE      *pVAS    = NULL;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDevice), RES_GET_HANDLE(pDevice),
                                                            pParams->hVASpace, &pVAS));

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in non SRIOV (legacy) guest RM, do a
    // RPC to the host to do the hardware update.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;
        NV_STATUS     status = NV_OK;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        if (status != NV_OK)
        {
            return status;
        }
    }

    OBJGVASPACE *pGVAS = dynamicCast(pVAS, OBJGVASPACE);
    NV_ASSERT_OR_RETURN(pGVAS != NULL, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OK_OR_RETURN(gvaspaceResize(pGVAS, pParams));

    return NV_OK;
}

//
// deviceCtrlCmdDmaSetPageDirectory_IMPL
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaSetPageDirectory_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams
)
{
    NvHandle        hClient     = RES_GET_CLIENT_HANDLE(pDevice);
    NvHandle        hDevice     = RES_GET_HANDLE(pDevice);
    OBJGPU         *pGpu        = GPU_RES_GET_GPU(pDevice);
    OBJVASPACE     *pVAS;
    NV_STATUS       status      = NV_OK;
    NvBool          bBcState    = NV_FALSE;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_SET_PAGE_DIRECTORY(pGpu, hClient, hDevice, pParams, status);
        if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) || status != NV_OK)
        {
            return status;
        }
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDevice), hDevice,
                                                            pParams->hVASpace, &pVAS));

    // Force to UC if client passed in sub-device handle.
    if (0 != pParams->subDeviceId)
    {
        bBcState = gpumgrGetBcEnabledStatus(pGpu);

        pGpu = gpumgrGetGpuFromSubDeviceInst(gpuGetDeviceInstance(pGpu),
                                             pParams->subDeviceId - 1);
        gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
    }


    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        OBJGVASPACE *pGVAS = dynamicCast(pVAS, OBJGVASPACE);
        if (pGVAS == NULL)
        {
            status = NV_ERR_NOT_SUPPORTED;
            SLI_LOOP_BREAK;
        }
        status = gvaspaceExternalRootDirCommit(pGVAS, hClient, pGpu, pParams);
        if (status != NV_OK)
        {
            SLI_LOOP_BREAK;
        }
    }
    SLI_LOOP_END

    // Restore BC if required.
    if (0 != pParams->subDeviceId)
    {
        gpumgrSetBcEnabledStatus(pGpu, bBcState);
    }

    if (status != NV_OK && IS_GSP_CLIENT(pGpu))
    {
        NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS params = {0};

        params.hVASpace    = pParams->hVASpace;
        params.subDeviceId = pParams->subDeviceId;

        NV_RM_RPC_UNSET_PAGE_DIRECTORY(pGpu, hClient, hDevice, &params, status);
    }

    if (status != NV_OK && IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS params = {0};

        params.hVASpace    = pParams->hVASpace;
        params.subDeviceId = pParams->subDeviceId;

        NV_RM_RPC_CONTROL(pGpu,
                          hClient,
                          hDevice,
                          NV0080_CTRL_CMD_DMA_UNSET_PAGE_DIRECTORY,
                          &params,
                          sizeof(NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS),
                          status);

    }

    return status;
}

//
// deviceCtrlCmdDmaUnsetPageDirectory_IMPL
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaUnsetPageDirectory_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams
)
{
    OBJGPU         *pGpu          = GPU_RES_GET_GPU(pDevice);
    OBJVASPACE     *pVAS          = NULL;
    CALL_CONTEXT   *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams   *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;
    NvBool          bBcState      = NV_FALSE;
    NV_STATUS       status        = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }


    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDevice), RES_GET_HANDLE(pDevice),
                                                            pParams->hVASpace, &pVAS));

    OBJGVASPACE *pGVAS = dynamicCast(pVAS, OBJGVASPACE);
    NV_ASSERT_OR_RETURN(pGVAS != NULL, NV_ERR_NOT_SUPPORTED);

    // Force to UC if client passed in sub-device handle.
    if (pParams->subDeviceId != 0)
    {
        bBcState = gpumgrGetBcEnabledStatus(pGpu);

        pGpu = gpumgrGetGpuFromSubDeviceInst(gpuGetDeviceInstance(pGpu),
                                             pParams->subDeviceId - 1);
        gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        status = gvaspaceExternalRootDirRevoke(pGVAS, pGpu, pParams);
        if (status != NV_OK)
        {
            SLI_LOOP_BREAK;
        }
    }
    SLI_LOOP_END

    //
    // Updating the instance block of all channels in the TSGs that's using
    // the VA space
    //
    status = gvaspaceUnregisterAllChanGrps(pGVAS, pGpu);

    // Restore BC if required.
    if (pParams->subDeviceId != 0)
    {
        gpumgrSetBcEnabledStatus(pGpu, bBcState);
    }

    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
    }

    return status;
}

//
// deviceCtrlCmdDmaSetPteInfo_IMPL
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaSetPteInfo_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams
)
{
    OBJGPU     *pGpu    = GPU_RES_GET_GPU(pDevice);
    OBJVASPACE *pVAS    = NULL;
    NV_STATUS   status  = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDevice), RES_GET_HANDLE(pDevice),
                                                            pParams->hVASpace, &pVAS));

    status = vaspaceSetPteInfo(pVAS, pGpu, pParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "vaspaceGetPteInfo failed\n");
        NV_ASSERT(0);
    }

    return status;
}

//
// deviceCtrlCmdDmaFlush_IMPL
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaFlush_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_FLUSH_PARAMS *flushParams
)
{
    OBJGPU             *pGpu        = GPU_RES_GET_GPU(pDevice);
    KernelBus          *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    FB_CACHE_MEMTYPE    targetMem   = FB_CACHE_MEM_UNDEFINED;
    FB_CACHE_OP         cacheOp     = FB_CACHE_OP_UNDEFINED;
    NV_STATUS           status      = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_PRINTF(LEVEL_INFO, "Flush op invoked with target Unit 0x%x\n",
              flushParams->targetUnit);

    if (FLD_TEST_DRF(0080, _CTRL_DMA_FLUSH_TARGET_UNIT, _L2_INVALIDATE,
                     _SYSMEM, flushParams->targetUnit))
    {
        targetMem = FB_CACHE_SYSTEM_MEMORY;
        cacheOp =  FB_CACHE_INVALIDATE;
    }
    if (FLD_TEST_DRF(0080, _CTRL_DMA_FLUSH_TARGET_UNIT, _L2_INVALIDATE,
                    _PEERMEM, flushParams->targetUnit))
    {
        targetMem = FB_CACHE_PEER_MEMORY;
        cacheOp =  FB_CACHE_INVALIDATE;
    }
    if (FLD_TEST_DRF(0080, _CTRL_DMA_FLUSH_TARGET_UNIT, _L2, _ENABLE,
                     flushParams->targetUnit))
    {
        targetMem = FB_CACHE_DIRTY;
        cacheOp =  FB_CACHE_WRITEBACK;
    }
    if (FLD_TEST_DRF(0080, _CTRL_DMA_FLUSH_TARGET_UNIT, _COMPTAG, _ENABLE,
                     flushParams->targetUnit))
    {
        targetMem = FB_CACHE_COMPTAG_MEMORY;
        cacheOp =  FB_CACHE_WRITEBACK;
    }

    if ((targetMem != FB_CACHE_MEM_UNDEFINED) && (cacheOp != FB_CACHE_OP_UNDEFINED))
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
        {
            KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
            status = kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, NULL, targetMem, cacheOp);
            if (status != NV_OK)
            {
                SLI_LOOP_RETURN(status);
            }
        }
        SLI_LOOP_END
    }

    if (FLD_TEST_DRF(0080, _CTRL_DMA_FLUSH_TARGET_UNIT, _FB, _ENABLE,
                     flushParams->targetUnit))
    {
        status = kbusSendSysmembar(pGpu, pKernelBus);
    }

    return status;
}

//
// deviceCtrlCmdDmaAdvSchedGetVaCaps_IMPL
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaAdvSchedGetVaCaps_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams
)
{
    OBJGPU        *pGpu           = GPU_RES_GET_GPU(pDevice);
    OBJVASPACE    *pVAS           = NULL;
    NV_STATUS      status         = NV_OK;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu));

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
        vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDevice), RES_GET_HANDLE(pDevice),
                                          pParams->hVASpace, &pVAS));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        vaspaceGetVasInfo(pVAS, pParams));

    pParams->compressionPageSize = pMemorySystemConfig->comprPageSize;
    pParams->vaSpaceId           = pVAS->vaspaceId;

    return status;
}

//
// deviceCtrlCmdDmaGetPdeInfo_IMPL
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaGetPdeInfo_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams
)
{
    OBJGPU         *pGpu            = GPU_RES_GET_GPU(pDevice);
    NV_STATUS       status          = NV_OK;
    OBJVASPACE     *pVAS            = NULL;
    CALL_CONTEXT   *pCallContext    = resservGetTlsCallContext();
    RmCtrlParams   *pRmCtrlParams   = pCallContext->pControlParams->pLegacyParams;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in non SRIOV (legacy) guest RM, do a
    // RPC to the host to do the hardware update.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        NV_STATUS status = NV_OK;
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDevice), RES_GET_HANDLE(pDevice),
                                                            pParams->hVASpace, &pVAS));

    if(vaspaceGetPageTableInfo(pVAS, pParams) != NV_OK)
    {
        status = NV_ERR_INVALID_EVENT;
        NV_PRINTF(LEVEL_ERROR, "vaspaceGetPageTableInfo failed\n");
        NV_ASSERT(0);
    }

    return status;
}
NV_STATUS
deviceCtrlCmdDmaSetDefaultVASpace_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *pParams
)
{
    OBJGPU   *pGpu   = GPU_RES_GET_GPU(pDevice);
    NV_STATUS status = NV_OK;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    NV_ASSERT_OK_OR_RETURN(
        deviceSetDefaultVASpace(
            pDevice,
            pParams->hVASpace));

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        CALL_CONTEXT   *pCallContext  = resservGetTlsCallContext();
        RmCtrlParams   *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;

        NV_RM_RPC_CONTROL(pRmCtrlParams->pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
    }

    return status;
}

//
// subdeviceCtrlCmdDmaInvalidateTLB
//
// Lock Requirements:
//      Assert that GPUs lock held on entry
//      Called from SW method w/o API lock
//
NV_STATUS
subdeviceCtrlCmdDmaInvalidateTLB_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams
)
{
    OBJGPU     *pGpu    = GPU_RES_GET_GPU(pSubdevice);
    OBJVASPACE *pVAS    = NULL;

    LOCK_ASSERT_AND_RETURN(rmGpuLockIsOwner());

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pSubdevice), RES_GET_PARENT_HANDLE(pSubdevice),
                                                            pParams->hVASpace, &pVAS));

    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NV_STATUS status = NV_OK;
        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    //
    // Although this function is used following PTE upgrades most of the time,
    // we cannot guarantee that, nor can we easily determine the update type.
    //
    vaspaceInvalidateTlb(pVAS, pGpu, PTE_DOWNGRADE);

    return NV_OK;
}

/*!
 * @brief subdeviceCtrlCmdDmaGetInfo
 *
 * Lock Requirements:
 *      Assert that both the GPUs lock and API lock are held on entry.
 */
NV_STATUS
subdeviceCtrlCmdDmaGetInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_DMA_GET_INFO_PARAMS *pDmaInfoParams
)
{
    OBJGPU     *pGpu   = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS   status = NV_OK;
    NvU32       i;
    NvU32       data;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    // error checck
    if (pDmaInfoParams->dmaInfoTblSize > NV2080_CTRL_DMA_GET_INFO_MAX_ENTRIES)
        return NV_ERR_INVALID_PARAM_STRUCT;

    // step thru list
    for (i = 0; i < pDmaInfoParams->dmaInfoTblSize; i++)
    {
        switch (pDmaInfoParams->dmaInfoTbl[i].index)
        {
            case NV2080_CTRL_DMA_INFO_INDEX_SYSTEM_ADDRESS_SIZE:
                data = gpuGetPhysAddrWidth_HAL(pGpu, ADDR_SYSMEM);
                break;
            default:
            {
                data = 0;
                status = NV_ERR_INVALID_ARGUMENT;
                break;
            }

        }

        if (status != NV_OK)
            break;

        // save off data value
        pDmaInfoParams->dmaInfoTbl[i].data = data;
    }

    return status;
}

/*!
 * @brief New TLB interface control call w/o engine masks.
 *
 * Lock Requirements:
 *      Assert that API lock and GPUs lock held on entry
 *
 */
NV_STATUS
deviceCtrlCmdDmaInvalidateTLB_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams
)
{
    OBJGPU      *pGpu    = GPU_RES_GET_GPU(pDevice);
    OBJVASPACE  *pVAS    = NULL;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to do the hardware update.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams->pLegacyParams;
        NV_STATUS status = NV_OK;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }


    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDevice), RES_GET_HANDLE(pDevice),
                                                            pParams->hVASpace, &pVAS));


    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    {
        //
        // Although this function is used following PTE upgrades most of the time,
        // we cannot guarantee that, nor can we easily determine the update type.
        //
        vaspaceInvalidateTlb(pVAS, pGpu, PTE_DOWNGRADE);
    }
    SLI_LOOP_END

    return NV_OK;
}

//
// deviceCtrlCmdDmaGetCaps_IMPL
//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
deviceCtrlCmdDmaGetCaps_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_GET_CAPS_PARAMS *pDmaCapsParams
)
{
    NV_STATUS   status = NV_OK;
    OBJGPU     *pGpu = GPU_RES_GET_GPU(pDevice);
    VirtMemAllocator *pDma = GPU_GET_DMA(pGpu);

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    // sanity check array size
    if (pDmaCapsParams->capsTblSize != NV0080_CTRL_DMA_CAPS_TBL_SIZE)
    {
        NV_PRINTF(LEVEL_ERROR, "size mismatch: client 0x%x rm 0x%x\n",
                  pDmaCapsParams->capsTblSize, NV0080_CTRL_DMA_CAPS_TBL_SIZE);
        return NV_ERR_INVALID_ARGUMENT;
    }

    portMemSet(pDmaCapsParams->capsTbl, 0, NV0080_CTRL_DMA_CAPS_TBL_SIZE);

    // Fill in caps
    if (pDma->getProperty(pDma, PDB_PROP_DMA_ENFORCE_32BIT_POINTER))
        RMCTRL_SET_CAP(pDmaCapsParams->capsTbl, NV0080_CTRL_DMA_CAPS, _32BIT_POINTER_ENFORCED);

    if (pDma->getProperty(pDma, PDB_PROP_DMA_SHADER_ACCESS_SUPPORTED))
        RMCTRL_SET_CAP(pDmaCapsParams->capsTbl, NV0080_CTRL_DMA_CAPS, _SHADER_ACCESS_SUPPORTED);

    if (pDma->getProperty(pDma, PDB_PROP_DMA_IS_SUPPORTED_SPARSE_VIRTUAL))
        RMCTRL_SET_CAP(pDmaCapsParams->capsTbl, NV0080_CTRL_DMA_CAPS, _SPARSE_VIRTUAL_SUPPORTED);

    // Supported on all platforms except the Maxwell amodel simulator
    if (pDma->getProperty(pDma, PDB_PROP_DMA_MULTIPLE_VASPACES_SUPPORTED))
        RMCTRL_SET_CAP(pDmaCapsParams->capsTbl, NV0080_CTRL_DMA_CAPS, _MULTIPLE_VA_SPACES_SUPPORTED);

    return status;
}

//
// deviceCtrlCmdDmaEnablePrivilegedRange_IMPL
//
// Lock Requirements:
//      Assert that both locks are held on entry
// Enables the privileged range assuming that the vaspace
// has not yet been created. If the vaspace has already been
// created that means we have already made allocations in this
// vaspace(lazy allocation). In this case this ctrl call should fail.
//
NV_STATUS
deviceCtrlCmdDmaEnablePrivilegedRange_IMPL
(
    Device *pDevice,
    NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS *pParams
)
{
    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner());

    if (pParams->hVASpace != NV01_NULL_OBJECT)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pDevice->pVASpace == NULL)
    {
        pDevice->deviceInternalAllocFlags |=
                                  NV_DEVICE_INTERNAL_ALLOCATION_FLAGS_ENABLE_PRIVILEGED_VASPACE;
        return NV_OK;
    }

    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
diagapiCtrlCmdDmaIsSupportedSparseVirtual_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_DMA_IS_SUPPORTED_SPARSE_VIRTUAL_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDiagApi);
    VirtMemAllocator *pDma = GPU_GET_DMA(pGpu);

    pParams->bIsSupported = pDma->getProperty(pDma, PDB_PROP_DMA_IS_SUPPORTED_SPARSE_VIRTUAL);
    return NV_OK;
}

NV_STATUS
diagapiCtrlCmdDmaGetVasBlockDetails_IMPL
(
    DiagApi *pDiagApi,
    NV208F_CTRL_DMA_GET_VAS_BLOCK_DETAILS_PARAMS *pParams
)
{
    OBJGPU             *pGpu        = GPU_RES_GET_GPU(pDiagApi);
    RsResourceRef      *pSubdevRef;
    Subdevice          *pGpuSubDevInfo;
    OBJVASPACE         *pVAS        = NULL;
    OBJEHEAP           *pHeap       = NULL;
    EMEMBLOCK          *pMemBlock   = NULL;

    if (NV_OK != refFindAncestorOfType(RES_GET_REF(pDiagApi), classId(Subdevice), &pSubdevRef))
        return NV_ERR_INVALID_OBJECT_PARENT;

    pGpuSubDevInfo = dynamicCast(pSubdevRef->pResource, Subdevice);

    NV_CHECK_OK_OR_RETURN(LEVEL_WARNING,
                          vaspaceGetByHandleOrDeviceDefault(RES_GET_CLIENT(pDiagApi), RES_GET_PARENT_HANDLE(pGpuSubDevInfo),
                                                            pParams->hVASpace, &pVAS));

    pHeap = vaspaceGetHeap(pVAS);
    NV_ASSERT_OR_RETURN(NULL != pHeap, NV_ERR_INVALID_ARGUMENT);
    pMemBlock = pHeap->eheapGetBlock(pHeap, pParams->virtualAddress, 0);
    NV_ASSERT_OR_RETURN(NULL != pMemBlock, NV_ERR_INVALID_ARGUMENT);

    pParams->beginAddress   = pMemBlock->begin;
    pParams->endAddress     = pMemBlock->end;
    pParams->alignedAddress = pMemBlock->align;
    pParams->pageSize = vaspaceGetMapPageSize(pVAS, pGpu, pMemBlock);

    NV_ASSERT_OR_RETURN(0 != pParams->pageSize, NV_ERR_INVALID_ARGUMENT);

    return NV_OK;
}

/*!
 * Initialize an abstracted page array with opaque page array data.
 *
 * By default, the page data is treated as an RmPhysAddr array.
 * If the data is an OS-specific format, the bOsFormat field must be
 * set to NV_TRUE.
 */
void
dmaPageArrayInit
(
    DMA_PAGE_ARRAY *pPageArray, //!< [out] Abstracted page array.
    void           *pPageData,  //!< [in] Opaque page array data.
    NvU32           pageCount   //!< [in] Number of pages represented.
)
{
    portMemSet(pPageArray, 0, sizeof(*pPageArray));
    pPageArray->pData = pPageData;
    pPageArray->count = pageCount;
}

/*!
 * Initialize an abstracted page array with opaque page array data.
 *
 *  TODO: Deprecate dmaPageArrayInit once moving all the use cases.
 */
void
dmaPageArrayInitWithFlags
(
    DMA_PAGE_ARRAY *pPageArray,    //!< [out] Abstracted page array.
    void           *pPageData,     //!< [in] Opaque page array data.
    NvU32           pageCount,     //!< [in] Number of pages represented
    NvU64           pageArrayFlags //!< [in] Flags of type DMA_PAGE_ARRARY_FLAGS
)
{
    dmaPageArrayInit(pPageArray, pPageData, pageCount);
}

/*!
 * Initialize an abstracted page array from a memory descriptor.
 */
void
dmaPageArrayInitFromMemDesc
(
    DMA_PAGE_ARRAY     *pPageArray,         //!< [out] Abstracted page array.
    MEMORY_DESCRIPTOR  *pMemDesc,           //!< [in] Memory descriptor.
    ADDRESS_TRANSLATION addressTranslation  //!< [in] Address translation for page array.
)
{
    dmaPageArrayInit(pPageArray,
        memdescGetPteArray(pMemDesc, addressTranslation),
        memdescGetPteArraySize(pMemDesc, addressTranslation));
}

/*!
 * Extract a physical page address from an abstracted page array.
 *
 * @returns The physical (byte) address of the requested page.
 * @returns ~0 if the index is out of bounds (fatal error).
 */
RmPhysAddr
dmaPageArrayGetPhysAddr
(
    DMA_PAGE_ARRAY *pPageArray, //!< [in] Abstracted page array.
    NvU32           pageIndex   //!< [in] Page index to retrieve.
)
{
    RmPhysAddr addr;

    NV_ASSERT_OR_RETURN(pPageArray->pData, ~0ULL);
    NV_ASSERT_OR_RETURN(pageIndex < pPageArray->count, ~0ULL);

    if (pPageArray->bDuplicate)
    {
        pageIndex = 0;
    }

    if (pPageArray->bOsFormat)
    {
        addr = osPageArrayGetPhysAddr(pPageArray->pOsGpuInfo,
            pPageArray->pData, pPageArray->startIndex + pageIndex);
    }
    else
    {
        RmPhysAddr *pPteArray = pPageArray->pData;
        {
            addr = pPteArray[pPageArray->startIndex + pageIndex];
        }
    }

    return addr;
}
