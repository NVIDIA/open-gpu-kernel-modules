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

/****************************************************************************
*
*   Kernel Ccu Module
*   This file contains functions managing CCU core on CPU RM
*
****************************************************************************/

#include "gpu/ccu/kernel_ccu.h"
#include "gpu/mig_mgr/kernel_mig_manager.h"
#include "gpu/timer/objtmr.h"
#include "ctrl/ctrl2080/ctrl2080perf_cf.h"
#include "utils/nvassert.h"
#include "gpu/bus/kern_bus.h"
#include "vgpu/rpc.h"

NV_STATUS kccuConstructEngine_IMPL(OBJGPU *pGpu,
                                   KernelCcu *pKernelCcu,
                                   ENGDESCRIPTOR engDesc)
{
    return NV_OK;
}

/*!
 * Allocate memory for class members, create & map shared buffer and initialize counter start
 * address and head/tail timestamp address
 *
 * @param[in]      pGpu       GPU object pointer
 * @param[in]      pKernelCcu KernelCcu object pointer
 * @param[in]      idx        MIG inst/for non-mig case idx is "0"
 * @param[in]      shrBufSize Shared buffer size
 * @param[in]      counterBlockSize   Counter block size
 *
 * @return  NV_OK on success, specific error code on failure.
 */
static NV_STATUS
_kccuAllocMemory
(
   OBJGPU     *pGpu,
   KernelCcu  *pKernelCcu,
   NvU32      idx,
   NvU32      shrBufSize,
   NvU32      counterBlockSize
)
{
    NV_STATUS status            = NV_OK;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;
    NvU32 aperture              = ADDR_SYSMEM;

    NV_PRINTF(LEVEL_INFO, "KernelCcu: Allocate memory for class members and shared buffer\n");

    if (IS_VIRTUAL(pGpu))
    {
        aperture = ADDR_FBMEM;
    }

    // Allocate memory & init the KernelCcu class members to store shared buffer info
    pKernelCcu->shrBuf[idx].pCounterDstInfo = portMemAllocNonPaged(sizeof(CCU_SHRBUF_INFO));
    pKernelCcu->shrBuf[idx].pKernelMapInfo  = portMemAllocNonPaged(sizeof(SHARED_BUFFER_MAP_INFO));

    if (pKernelCcu->shrBuf[idx].pCounterDstInfo == NULL || pKernelCcu->shrBuf[idx].pKernelMapInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR, "CCU port mem alloc failed for(%u) with status: 0x%x\n", idx, status);
        goto free_alloc;
    }
    portMemSet(pKernelCcu->shrBuf[idx].pCounterDstInfo, 0, sizeof(CCU_SHRBUF_INFO));
    portMemSet(pKernelCcu->shrBuf[idx].pKernelMapInfo, 0, sizeof(SHARED_BUFFER_MAP_INFO));

    // Create a memory descriptor data structure for the shared buffer
    status = memdescCreate(&pKernelCcu->pMemDesc[idx], pGpu, shrBufSize, 0, NV_MEMORY_CONTIGUOUS,
                           aperture, NV_MEMORY_CACHED, MEMDESC_FLAGS_USER_READ_ONLY);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdescCreate failed for(%u) with status: 0x%x\n", idx, status);
        goto free_alloc;
    }

    pMemDesc = pKernelCcu->pMemDesc[idx];
    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdescCreate failed. memdesc for(%u) is NULL\n", idx);
        goto free_alloc;
    }

    // Allocate physical storage for the memory descriptor
    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_55, pMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdescAlloc failed for(%u) with status: 0x%x\n", idx, status);
        memdescDestroy(pMemDesc);
        goto free_alloc;
    }

    if (IS_VIRTUAL(pGpu))
    {
        memdescSetPageSize(pMemDesc, AT_CPU, RM_PAGE_SIZE);

        pKernelCcu->shrBuf[idx].pKernelMapInfo->addr = kbusMapRmAperture_HAL(pGpu, pMemDesc);
        if (pKernelCcu->shrBuf[idx].pKernelMapInfo->addr == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }
    else
    {
        // Map memory to kernel VA space
        status = memdescMap(pMemDesc, 0, shrBufSize, NV_TRUE, NV_PROTECT_READ_WRITE,
                            &pKernelCcu->shrBuf[idx].pKernelMapInfo->addr,
                            &pKernelCcu->shrBuf[idx].pKernelMapInfo->priv);
    }
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdescMap failed for(%u)with status: 0x%x\n", idx, status);
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
        goto free_alloc;
    }

    portMemSet(pKernelCcu->shrBuf[idx].pKernelMapInfo->addr, 0, shrBufSize);

    // Init the counter start address, head & tail timestamp address and counter block size
    pKernelCcu->shrBuf[idx].pCounterDstInfo->pHeadTimeStamp =
        (NvU64 *)pKernelCcu->shrBuf[idx].pKernelMapInfo->addr;
    pKernelCcu->shrBuf[idx].pCounterDstInfo->pCounterBlock =
        (NvP64)((NvUPtr)pKernelCcu->shrBuf[idx].pKernelMapInfo->addr + CCU_TIMESTAMP_SIZE);
    pKernelCcu->shrBuf[idx].pCounterDstInfo->pTailTimeStamp =
        (NvU64 *)((NvUPtr)pKernelCcu->shrBuf[idx].pCounterDstInfo->pCounterBlock + counterBlockSize);
    pKernelCcu->shrBuf[idx].pCounterDstInfo->pSwizzId =
        (NvU8 *)((NvUPtr)pKernelCcu->shrBuf[idx].pCounterDstInfo->pTailTimeStamp + CCU_TIMESTAMP_SIZE);
    pKernelCcu->shrBuf[idx].pCounterDstInfo->pComputeId =
        (NvU8 *)((NvUPtr)pKernelCcu->shrBuf[idx].pCounterDstInfo->pSwizzId + CCU_MIG_SWIZZID_SIZE);
        pKernelCcu->shrBuf[idx].pCounterDstInfo->counterBlockSize = counterBlockSize;

    // Set mig swizz-id and compute-inst id to invalid
    if (!RMCFG_FEATURE_MODS_FEATURES)
    {
        portMemSet(pKernelCcu->shrBuf[idx].pCounterDstInfo->pCounterBlock, 0, counterBlockSize);
        *pKernelCcu->shrBuf[idx].pCounterDstInfo->pSwizzId = CCU_MIG_INVALID_SWIZZID;
        *pKernelCcu->shrBuf[idx].pCounterDstInfo->pComputeId = CCU_MIG_INVALID_COMPUTEID;
    }

    return NV_OK;

free_alloc:
    portMemFree(pKernelCcu->shrBuf[idx].pCounterDstInfo);
    portMemFree(pKernelCcu->shrBuf[idx].pKernelMapInfo);

    return status;
}

/*!
 * Cleanup shared buffer and class member memory for given shared buffer index
 *
 * @param[in]      pGpu       GPU object pointer
 * @param[in]      pKernelCcu KernelCcu object pointer
 * @param[in]      idx        MIG inst/for non-mig case idx is "0"
 *
 */
void
kccuShrBufIdxCleanup_IMPL
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu,
    NvU32      idx
)
{
    MEMORY_DESCRIPTOR *pMemDesc = pKernelCcu->pMemDesc[idx];

    NV_PRINTF(LEVEL_INFO, "Shared buffer unmap & free for idx(%u).\n", idx);

    if (IS_VIRTUAL(pGpu))
    {
        kbusUnmapRmAperture_HAL(pGpu, pMemDesc,
                                &pKernelCcu->shrBuf[idx].pKernelMapInfo->addr,
                                NV_TRUE);
    }
    else
    {
        memdescUnmap(pMemDesc, NV_TRUE, osGetCurrentProcess(),
                pKernelCcu->shrBuf[idx].pKernelMapInfo->addr,
                pKernelCcu->shrBuf[idx].pKernelMapInfo->priv);
    }
    memdescFree(pMemDesc);
    memdescDestroy(pMemDesc);

    // Free class member memory
    portMemFree(pKernelCcu->shrBuf[idx].pCounterDstInfo);
    portMemFree(pKernelCcu->shrBuf[idx].pKernelMapInfo);
    pKernelCcu->shrBuf[idx].pCounterDstInfo = NULL;
    pKernelCcu->shrBuf[idx].pKernelMapInfo  = NULL;
    pKernelCcu->pMemDesc[idx]               = NULL;
}

/*!
 * Unmap & free memory desc, shared buffer and class member memory
 *
 * @param[in]      pGpu       GPU object pointer
 * @param[in]      pKernelCcu KernelCcu object pointer
 *
 */
static void
_kccuUnmapAndFreeMemory
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu
)
{
    NvU32 i = 0;
    NV_STATUS status = NV_OK;
    NV2080_CTRL_INTERNAL_CCU_UNMAP_INFO_PARAMS params = { 0 };

    NV_PRINTF(LEVEL_INFO, "KernelCcu: Unmap and free shared buffer\n");

    // Internal RM api ctrl call to physical RM to unmap dev & mig shared buffer memdesc
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    params.bDevShrBuf = NV_TRUE;
    params.bMigShrBuf = NV_TRUE;

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_CCU_UNMAP,
                             &params,
                             sizeof(params));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdesc unmap request failed with status: 0x%x\n", status);
    }

    // Unmap & free mem desc
    for (i = 0; i < CCU_SHRBUF_COUNT_MAX; i++)
    {
        if (pKernelCcu->pMemDesc[i] != NULL)
        {
            kccuShrBufIdxCleanup(pGpu, pKernelCcu, i);
        }
    }
}

/*!
 * Send shared buffer info to phy-RM/gsp
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[in]  pKernelCcu     KernelCcu object pointer
 * @param[in]  shrBufStartIdx Shared buffer start idx
 *
 * @return  NV_OK on success, specific error code on failure.
 */
NV_STATUS
kccuShrBufInfoToCcu_IMPL
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu,
    NvU32      shrBufStartIdx
)
{
    NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS inParams = { 0 };
    NV_STATUS status = NV_OK;
    NvU32     idx    = 0;

    NV_PRINTF(LEVEL_INFO, "Send shared buffer info to phyRM/gsp to map.\n");

    for (idx = shrBufStartIdx; idx < CCU_SHRBUF_COUNT_MAX; idx++)
    {
        if (pKernelCcu->pMemDesc[idx] != NULL)
        {
            inParams.mapInfo[idx].phyAddr = memdescGetPhysAddr(pKernelCcu->pMemDesc[idx], AT_GPU, 0);

            if (idx == CCU_DEV_SHRBUF_ID)
            {
                inParams.mapInfo[idx].shrBufSize = pKernelCcu->devSharedBufSize;
                inParams.mapInfo[idx].cntBlkSize = pKernelCcu->devBufSize;
            }
            else
            {
                inParams.mapInfo[idx].shrBufSize = pKernelCcu->migSharedBufSize;
                inParams.mapInfo[idx].cntBlkSize = pKernelCcu->migBufSize;
            }
        }
    }

    // Internal RM api ctrl call to physical RM to map shared buffer memdesc
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_CCU_MAP,
                             &inParams,
                             sizeof(inParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdesc map request failed with status: 0x%x\n", status);
    }

    return status;
}

/*!
 * Create shared buffer for device counters
 *
 * @param[in]      pGpu       GPU object pointer
 * @param[in]      pKernelCcu KernelCcu object pointer
 *
 * @return  NV_OK on success, specific error code on failure.
 */
static NV_STATUS
_kccuInitDevSharedBuffer
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu
)
{
    NV_STATUS status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "Init shared buffer for device counters.\n");

    // Allocate shared buffer for device counters
    status = _kccuAllocMemory(pGpu, pKernelCcu, CCU_DEV_SHRBUF_ID, pKernelCcu->devSharedBufSize, pKernelCcu->devBufSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memory allocation failed with status: 0x%x\n", status);
    }

    return status;
}

/*!
 * Create shared buffer for mig counters
 *
 * @param[in]      pGpu       GPU object pointer
 * @param[in]      pKernelCcu KernelCcu object pointer
 *
 * @return  NV_OK on success, specific error code on failure.
 */
NV_STATUS
kccuInitMigSharedBuffer_IMPL
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu
)
{
    NV_STATUS status = NV_OK;
    NvU32 idx;

    NV_PRINTF(LEVEL_INFO, "Init shared buffer for mig counters.\n");

    // Allocate shared buffer for each mig gpu instance
    for (idx = CCU_MIG_SHRBUF_ID_START; idx < CCU_SHRBUF_COUNT_MAX; idx++)
    {
        status = _kccuAllocMemory(pGpu, pKernelCcu, idx, pKernelCcu->migSharedBufSize, pKernelCcu->migBufSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "CCU memory allocation failed for idx(%u) with status: 0x%x\n",
                    idx, status);

            // Free shared buffer & mem desc, for earlier mig inst & device
            _kccuUnmapAndFreeMemory(pGpu, pKernelCcu);
            break;
        }

    }

    return status;
}

/*!
 * State Load
 *
 * @param[in]      pGpu       GPU object pointer
 * @param[in]      pKernelCcu KernelCcu object pointer
 * @param[in]      flags      Type of transition
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_STATE
 */
NV_STATUS kccuStateLoad_IMPL
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu,
    NvU32      flags
)
{
    NV_STATUS status = NV_OK;

    // Skip for vGPU guest
    // Buffer allocation is done in FB via kccuInitVgpuMigSharedBuffer at CI creation
    if (IS_VIRTUAL(pGpu))
    {
        return status;
    }

    NV_PRINTF(LEVEL_INFO, "KernelCcu: State load \n");

    // Get the buffer size information
    status = kccuGetBufSize_HAL(pGpu, pKernelCcu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get the buffer size info(status: %u) \n", status);
        return status;
    }

    // Create device shared buffer
    status = _kccuInitDevSharedBuffer(pGpu, pKernelCcu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to init device shared buffer(status: %u) \n", status);
        return status;
    }

    // Create mig shared buffer
    if (IS_MIG_ENABLED(pGpu))
    {
        status = kccuInitMigSharedBuffer(pGpu, pKernelCcu);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to init mig shared buffer(status: %u) \n", status);
            return status;
        }
        pKernelCcu->bMigShrBufAllocated = NV_TRUE;
    }

    // Send shared buffer info to gsp
    return (kccuShrBufInfoToCcu(pGpu, pKernelCcu, CCU_DEV_SHRBUF_ID));
}

/*!
 * State Unload
 *
 * @param[in]      pGpu       GPU object pointer
 * @param[in]      pKernelCcu KernelCcu object pointer
 * @param[in]      flags      Type of transition
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_STATE
 */
NV_STATUS kccuStateUnload_IMPL
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu,
    NvU32      flags
)
{
    NV_STATUS status = NV_OK;
    NV_PRINTF(LEVEL_INFO, "KernelCcu: State unload \n");

    // Disable ccu stream state
    pKernelCcu->bStreamState = CCU_STREAM_STATE_DISABLE;

    // Free shared buffer & mem desc
    _kccuUnmapAndFreeMemory(pGpu, pKernelCcu);

    pKernelCcu->bMigShrBufAllocated = NV_FALSE;

    return status;
}

/*!
 * Get the shared buffer memory descriptor for buffer index
 *
 * @param[in]      pGpu                GPU object pointer
 * @param[in]      pKernelCcu          KernelCcu object pointer
 * @param[in]      idx                 Shared buffer index
 * @param[out]     MEMORY_DESCRIPTOR   Location of pMemDesc
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 */
NV_STATUS kccuMemDescGetForShrBufId_IMPL
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu,
    NvU32       idx,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    NV_PRINTF(LEVEL_INFO, "KernelCcu: Get memdesc for idx(%u) \n", idx);

    if (idx >= CCU_SHRBUF_COUNT_MAX)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdesc get failed for input idx(%u). Invalid index.\n",
                  idx);

        return NV_ERR_INVALID_ARGUMENT;
    }

    *ppMemDesc = pKernelCcu->pMemDesc[idx];

    return NV_OK;
}

/*!
 * Get counter block size
 *
 * @param[in]      pGpu                GPU object pointer
 * @param[in]      pKernelCcu          KernelCcu object pointer
 * @param[in]      bDevCounter         Device counter
 *
 * @return  counter block size
 */
NvU32 kccuCounterBlockSizeGet_IMPL
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu,
    NvBool     bDevCounter
)
{
    NV_PRINTF(LEVEL_INFO, "KernelCcu: Get counter block size \n");
    // For device counter block
    if (bDevCounter)
    {
        return pKernelCcu->devBufSize;
    }

    return pKernelCcu->migBufSize;
}

/*!
 * Set ccu stream state
 *
 * @param[in]      pGpu         GPU object pointer
 * @param[in]      pKernelCcu   KernelCcu object pointer
 * @param[in]      pParams      Ccu stream state param
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 */
NV_STATUS kccuStreamStateSet_IMPL
(
    OBJGPU                                         *pGpu,
    KernelCcu                                      *pKernelCcu,
    NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_INTERNAL_CCU_STREAM_STATE_PARAMS ccuParams = { 0 };

    NV_PRINTF(LEVEL_INFO, "KernelCcu: Set ccu stream \n");

    if (pParams == NULL || (pParams->bStreamState != CCU_STREAM_STATE_ENABLE &&
                            pParams->bStreamState != CCU_STREAM_STATE_DISABLE))
    {
        NV_PRINTF(LEVEL_ERROR, "KernelCcu: Invalid input params\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pKernelCcu->bStreamState == pParams->bStreamState)
    {
        NV_PRINTF(LEVEL_INFO, "KernelCcu: CCU stream state is already (%s)\n",
                  pKernelCcu->bStreamState ? "ENABLED" : "DISABLED");
        return NV_OK;
    }

    ccuParams.bStreamState = pParams->bStreamState;

    // RM api ctrl call to physical RM to set ccu stream state
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_CCU_SET_STREAM_STATE,
                             &ccuParams,
                             sizeof(ccuParams));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU stream state set failed with status: 0x%x\n", status);
        return status;
    }

    pKernelCcu->bStreamState = ccuParams.bStreamState;

    // Clear the shared buffer, when stream state is disabled
    if (pKernelCcu->bStreamState == CCU_STREAM_STATE_DISABLE)
    {
        NvU32 i;
        for (i = 0; i < CCU_SHRBUF_COUNT_MAX; i++)
        {
            MEMORY_DESCRIPTOR *pMemDesc = pKernelCcu->pMemDesc[i];

            if (pMemDesc == NULL)
                continue;

            portMemSet(pKernelCcu->shrBuf[i].pKernelMapInfo->addr, 0, memdescGetSize(pMemDesc));
        }
    }

    return NV_OK;
}

/*!
 * Get ccu stream state
 *
 * @param[in]      pGpu         GPU object pointer
 * @param[in]      pKernelCcu   KernelCcu object pointer
 *
 * @return  CCU stream state
 */
NvBool kccuStreamStateGet_IMPL
(
    OBJGPU      *pGpu,
    KernelCcu   *pKernelCcu
)
{
    NV_PRINTF(LEVEL_INFO, "KernelCcu: Get ccu stream \n");

    return pKernelCcu->bStreamState;
}

/*!
 * Get the shared buffer memory descriptor for compute inst with
 * swizz id and compute id
 *
 * @param[in]      pGpu                GPU object pointer
 * @param[in]      pKernelCcu          KernelCcu object pointer
 * @param[in]      swizzId             Mig inst swizz-id
 * @param[in]      computeId           Compute inst id
 * @param[out]     MEMORY_DESCRIPTOR   Location of pMemDesc
 *
 * @return  NV_OK
 * @return  NV_ERR_NOT_SUPPORTED
 * @return  NV_ERR_INVALID_ARGUMENT
 */
NV_STATUS kccuMemDescGetForComputeInst_IMPL
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu,
    NvU8       swizzId,
    NvU8       computeId,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    NvU32 idx;

    for (idx = CCU_MIG_SHRBUF_ID_START; idx < CCU_SHRBUF_COUNT_MAX; idx++)
    {
        if (pKernelCcu->pMemDesc[idx] == NULL)
        {
            continue;
        }
        if (*pKernelCcu->shrBuf[idx].pCounterDstInfo->pSwizzId == swizzId &&
            *pKernelCcu->shrBuf[idx].pCounterDstInfo->pComputeId == computeId)
        {
            *ppMemDesc = pKernelCcu->pMemDesc[idx];
            break;
        }
    }

    if (idx >= CCU_SHRBUF_COUNT_MAX)
    {
        NV_PRINTF(LEVEL_ERROR, "KernelCcu: memdesc get failed for input swizzId(%u), computeInst(%u)\n",
                  swizzId, computeId);

        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * Send vGPU guest shared buffer info to phy-RM/gsp
 *
 * @return  NV_OK on success, specific error code on failure.
 */
static NV_STATUS
_kccuVgpuShrBufInfoToCcu
(
    OBJGPU      *pGpu,
    KernelCcu   *pKernelCcu,
    NvU32       swizzId,
    NvU32       computeId,
    NvU32       idx,
    NvBool      bMap
)
{
    NV_STATUS   status = NV_OK;
    NvU64       gpfn;

    if (pKernelCcu->pMemDesc[idx] != NULL)
    {
        gpfn = memdescGetPte(pKernelCcu->pMemDesc[idx], AT_GPU, 0) >> RM_PAGE_SHIFT;

        NV_RM_RPC_UPDATE_GPM_GUEST_BUFFER_INFO(pGpu, status, gpfn, swizzId, computeId,
                                               pKernelCcu->migSharedBufSize, bMap);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "CCU vGPU memdesc map rpc request failed with status: 0x%x\n", status);
        }
    }

    return status;
}

/*!
 * Cleanup the shared buffer for vGPU guest mig counters
 *
 * @return  NV_OK on success, specific error code on failure
 */
NV_STATUS
kccuDeInitVgpuMigSharedBuffer_IMPL
(
    OBJGPU      *pGpu,
    KernelCcu   *pKernelCcu,
    NvU32       swizzId,
    NvU32       computeId
)
{
    NV_STATUS   status = NV_OK;
    NvU32       idx = computeId + 1;

    // Cleanup the buffer and member memory for given shared buffer
    if (pKernelCcu->pMemDesc[idx] != NULL)
    {
        NV_ASSERT_OK_OR_RETURN(_kccuVgpuShrBufInfoToCcu(pGpu, pKernelCcu, swizzId, computeId, idx, NV_FALSE));

        kccuShrBufIdxCleanup(pGpu, pKernelCcu, idx);
    }

    return status;
}

/*!
 * Create shared buffer for vGPU guest mig counters
 *
 * @return  NV_OK on success, specific error code on failure
 */
NV_STATUS
kccuInitVgpuMigSharedBuffer_IMPL
(
    OBJGPU      *pGpu,
    KernelCcu   *pKernelCcu,
    NvU32       swizzId,
    NvU32       computeId
)
{
    NV_STATUS   status = NV_OK;
    NvU32       idx = computeId + 1;

    if (!IS_VIRTUAL(pGpu) || !IS_MIG_ENABLED(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Get the buffer size information
    status = kccuGetBufSize_HAL(pGpu, pKernelCcu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get the buffer size info(status: %u) \n", status);
        return status;
    }

    // Allocate memory in vGPU guest FB
    status = _kccuAllocMemory(pGpu, pKernelCcu, idx, pKernelCcu->migSharedBufSize, pKernelCcu->migBufSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "vGPU memory allocation failed for idx(%u) with status: 0x%x\n", idx, status);

        // Cleanup the buffer and member memory for given shared buffer
        kccuShrBufIdxCleanup(pGpu, pKernelCcu, idx);
        return status;
    }

    return _kccuVgpuShrBufInfoToCcu(pGpu, pKernelCcu, swizzId, computeId, idx, NV_TRUE);
}
