/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "objtmr.h"
#include "ctrl/ctrl2080/ctrl2080perf_cf.h"
#include "utils/nvassert.h"

/*!
 * Constrcutor for kccu class.
 *
 * @param[in] pGpu          GPU object pointer.
 * @param[in] pKernelCcu    KernelCcu object pointer
 * @param[in] engDesc       KernelCcu Engine descriptor
 *
 * @return  NV_OK  If successfully constructed.
 */
NV_STATUS
kccuConstructEngine_IMPL
(
   OBJGPU        *pGpu,
   KernelCcu     *pKernelCcu,
   ENGDESCRIPTOR engDesc
)
{
    NV_PRINTF(LEVEL_INFO, "KernelCcu: Constructor\n");

    return NV_OK;
}

/*!
 * Destructor
 *
 * @param[in]  pKernelCcu  KernelCcu object pointer
 */
void
kccuDestruct_IMPL
(
    KernelCcu *pKernelCcu
)
{
    NV_PRINTF(LEVEL_INFO, "KernelCcu: Destructor\n");

    return;
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
   NvU64      counterBlockSize
)
{
    NV_STATUS status            = NV_OK;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;

    NV_PRINTF(LEVEL_INFO, "KernelCcu: Allocate memory for class members and shared buffer\n");

    // Allocate memory & init the KernelCcu class members to store shared buffer info
    pKernelCcu->shrBuf[idx].pCounterDstInfo = portMemAllocNonPaged(sizeof(CCU_SHRBUF_INFO));
    pKernelCcu->shrBuf[idx].pKernelMapInfo  = portMemAllocNonPaged(sizeof(SHARED_BUFFER_MAP_INFO));

    if (pKernelCcu->shrBuf[idx].pCounterDstInfo == NULL || pKernelCcu->shrBuf[idx].pKernelMapInfo == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        NV_PRINTF(LEVEL_ERROR, "CCU port mem alloc failed for(%u) with status: 0x%x \r\n", idx, status);
        goto free_alloc;
    }
    portMemSet(pKernelCcu->shrBuf[idx].pCounterDstInfo, 0, sizeof(CCU_SHRBUF_INFO));
    portMemSet(pKernelCcu->shrBuf[idx].pKernelMapInfo, 0, sizeof(SHARED_BUFFER_MAP_INFO));

    // Create a memory descriptor data structure for the shared buffer
    status = memdescCreate(&pKernelCcu->pMemDesc[idx], pGpu, shrBufSize, 0, NV_MEMORY_CONTIGUOUS,
                           ADDR_SYSMEM, NV_MEMORY_CACHED,
                           MEMDESC_FLAGS_USER_READ_ONLY);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdescCreate failed for(%u) with status: 0x%x \r\n", idx, status);
        goto free_alloc;
    }

    pMemDesc = pKernelCcu->pMemDesc[idx];
    if (pMemDesc == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdescCreate failed. memdesc for(%u) is NULL\r\n", idx);
        goto free_alloc;
    }

    // Allocate physical storage for the memory descriptor
    status = memdescAlloc(pMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdescAlloc failed for(%u) with status: 0x%x \r\n", idx, status);
        memdescDestroy(pMemDesc);
        goto free_alloc;
    }

    // Map memory to kernel VA space
    status = memdescMap(pMemDesc, 0, shrBufSize, NV_TRUE, NV_PROTECT_READ_WRITE,
                        &pKernelCcu->shrBuf[idx].pKernelMapInfo->addr,
                        &pKernelCcu->shrBuf[idx].pKernelMapInfo->priv);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdescMap failed for(%u)with status: 0x%x \r\n", idx, status);
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
        pKernelCcu->shrBuf[idx].pCounterDstInfo->counterBlockSize = counterBlockSize;

    // Set mig swizz-id to invalid
    if (!NV_IS_MODS)
    {
        *pKernelCcu->shrBuf[idx].pCounterDstInfo->pSwizzId = CCU_MIG_INVALID_SWIZZID;
    }

    return NV_OK;

free_alloc:
    if (pKernelCcu->shrBuf[idx].pCounterDstInfo)
    {
        portMemFree(pKernelCcu->shrBuf[idx].pCounterDstInfo);
    }
    if (pKernelCcu->shrBuf[idx].pKernelMapInfo)
    {
        portMemFree(pKernelCcu->shrBuf[idx].pKernelMapInfo);
    }
    return status;
}

/*!
 * Free memory desc, shared buffer and class member memory
 *
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
    
    NV_PRINTF(LEVEL_INFO, "KernelCcu: Unmap and free shared buffer\n");

    // Internal RM api ctrl call to physical RM to unmap shared buffer memdesc
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_CCU_UNMAP,
                             NULL,
                             0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdesc unmap request failed with status: 0x%x \r\n", status);
    }

    // Unmap & free mem desc
    for (i = 0; i < CCU_SHRBUF_COUNT_MAX; i++)
    {
        MEMORY_DESCRIPTOR *pMemDesc = pKernelCcu->pMemDesc[i];

        if (pMemDesc == NULL)
            continue;

        memdescUnmap(pMemDesc, NV_TRUE, osGetCurrentProcess(),
                pKernelCcu->shrBuf[i].pKernelMapInfo->addr,
                pKernelCcu->shrBuf[i].pKernelMapInfo->priv);
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);

        // Free class member memory
        portMemFree(pKernelCcu->shrBuf[i].pCounterDstInfo);
        portMemFree(pKernelCcu->shrBuf[i].pKernelMapInfo);
        pKernelCcu->shrBuf[i].pCounterDstInfo = NULL;
        pKernelCcu->shrBuf[i].pKernelMapInfo  = NULL;
    }

    return;
}

/*!
 * Create shared buffer for counter data
 *
 * @param[in]      pGpu       GPU object pointer
 * @param[in]      pKernelCcu KernelCcu object pointer
 *
 * @return  NV_OK on success, specific error code on failure.
 */
static NV_STATUS
_kccuInitSharedBuffer
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu
)
{
    NV_STATUS status = NV_OK;
    NvU32 idx;

    if (IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Allocate shared buffer for device counters

    status = _kccuAllocMemory(pGpu, pKernelCcu, CCU_DEV_SHRBUF_ID, CCU_GPU_SHARED_BUFFER_SIZE_MAX,
                              CCU_PER_GPU_COUNTER_Q_SIZE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memory allocation failed with status: 0x%x \r\n", status);
        return status;
    }

    // Allocate shared buffer for each mig gpu instance
    for (idx = CCU_MIG_SHRBUF_ID_START; idx < CCU_SHRBUF_COUNT_MAX; idx++)
    {
        status = _kccuAllocMemory(pGpu, pKernelCcu, idx, CCU_MIG_INST_SHARED_BUFFER_SIZE_MAX,
                CCU_MIG_INST_COUNTER_Q_SIZE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "CCU memory allocation failed for idx(%u) with status: 0x%x \r\n",
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
    NvU32 idx = 0;
    NV_STATUS status = NV_OK;
    NV2080_CTRL_INTERNAL_CCU_MAP_INFO_PARAMS inParams = { 0 };

    NV_PRINTF(LEVEL_INFO, "KernelCcu: State load \n");

    // Create shared buffer
    status = _kccuInitSharedBuffer(pGpu, pKernelCcu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to init shared buffer(status: %u) \n", status);
        return status;
    }

    for (idx = 0; idx < CCU_SHRBUF_COUNT_MAX; idx++)
    {
        if (pKernelCcu->pMemDesc[idx] != NULL)
        {
            inParams.phyAddr[idx] = memdescGetPhysAddr(pKernelCcu->pMemDesc[idx], AT_GPU, 0);
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
        NV_PRINTF(LEVEL_ERROR, "CCU memdesc map request failed with status: 0x%x \r\n", status);
        return status;
    }

    return status;
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

    // Free shared buffer & mem desc
    _kccuUnmapAndFreeMemory(pGpu, pKernelCcu);

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

    if (IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (idx >= CCU_SHRBUF_COUNT_MAX)
    {
        NV_PRINTF(LEVEL_ERROR, "CCU memdesc get failed for input idx(%u). Invalid index.\r\n",
                  idx);

        return NV_ERR_INVALID_ARGUMENT;
    }

    *ppMemDesc = pKernelCcu->pMemDesc[idx];

    return NV_OK;
}

/*!
 * Get the shared buffer memory descriptor for swizz id
 *
 * @param[in]      pGpu                GPU object pointer
 * @param[in]      pKernelCcu          KernelCcu object pointer
 * @param[in]      swizzId             Mig inst swizz-id
 * @param[out]     MEMORY_DESCRIPTOR   Location of pMemDesc
 *
 * @return  NV_OK
 * @return  NV_ERR_INVALID_ARGUMENT
 */
NV_STATUS kccuMemDescGetForSwizzId_IMPL
(
    OBJGPU     *pGpu,
    KernelCcu  *pKernelCcu,
    NvU8       swizzId,
    MEMORY_DESCRIPTOR **ppMemDesc
)
{
    NvU32 idx = 0;

    if (IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    for (idx = CCU_MIG_SHRBUF_ID_START; idx < CCU_SHRBUF_COUNT_MAX; idx++)
    {
        if (*pKernelCcu->shrBuf[idx].pCounterDstInfo->pSwizzId == swizzId)
        {
            *ppMemDesc = pKernelCcu->pMemDesc[idx];
            break;
        }
    }

    if (idx >= CCU_SHRBUF_COUNT_MAX)
    {
        NV_PRINTF(LEVEL_ERROR, "KernelCcu: memdesc get failed for input swizzId(%u) \r\n",
                swizzId);

        return NV_ERR_INVALID_ARGUMENT;
    }

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
        return CCU_PER_GPU_COUNTER_Q_SIZE;
    }

    return CCU_MIG_INST_COUNTER_Q_SIZE;
}
