/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *  Description:
 *      This file contains the functions managing the gpu database
 *
 ***************************************************************************/

#include "gpu_mgr/gpu_db.h"
#include "core/system.h"

#include "gpu/gpu.h" // for NBADDR

NV_STATUS
gpudbConstruct_IMPL
(
    GpuDb *pGpuDb
)
{
    listInit(&pGpuDb->gpuList, portMemAllocatorGetGlobalNonPaged());

    pGpuDb->pLock = portSyncMutexCreate(portMemAllocatorGetGlobalNonPaged());
    if (pGpuDb->pLock == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Gpu data base list lock init failed\n");
        listDestroy(&pGpuDb->gpuList);
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    return NV_OK;
}

void
gpudbDestruct_IMPL
(
    GpuDb *pGpuDb
)
{
    if (pGpuDb->pLock != NULL)
    {
        portSyncMutexDestroy(pGpuDb->pLock);
    }

    listDestroy(&pGpuDb->gpuList);
}

static PGPU_INFO_LIST_NODE
_gpudbFindGpuInfoByUuid
(
    const NvU8 *pUuid
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuDb *pGpuDb = SYS_GET_GPUDB(pSys);
    GPU_INFO_LIST_NODE *pNode = NULL;

    for (pNode = listHead(&pGpuDb->gpuList);
         pNode != NULL;
         pNode = listNext(&pGpuDb->gpuList, pNode))
    {
        if (portMemCmp(pNode->uuid, pUuid, RM_SHA1_GID_SIZE) == 0)
        {
            break;
        }
    }

    return pNode;
}

NV_STATUS
gpudbRegisterGpu(const NvU8 *pUuid, const NBADDR *pUpstreamPortPciInfo, NvU64 pciInfo)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuDb *pGpuDb = SYS_GET_GPUDB(pSys);
    GPU_INFO_LIST_NODE *pNode;
    NV_STATUS status = NV_OK;
    NvU32 i = 0;

    portSyncMutexAcquire(pGpuDb->pLock);

    pNode = _gpudbFindGpuInfoByUuid(pUuid);
    if (pNode != NULL)
    {
        pNode->bShutdownState = NV_FALSE;
        goto done;
    }

    pNode = listAppendNew(&pGpuDb->gpuList);
    if (pNode == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Append the list failed\n");
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto done;
    }

    portMemCopy(pNode->uuid, RM_SHA1_GID_SIZE, pUuid, RM_SHA1_GID_SIZE);

    pNode->pciPortInfo.domain = gpuDecodeDomain(pciInfo);
    pNode->pciPortInfo.bus = gpuDecodeBus(pciInfo);
    pNode->pciPortInfo.device = gpuDecodeDevice(pciInfo);
    pNode->pciPortInfo.function = 0;
    pNode->pciPortInfo.bValid = NV_TRUE;

    pNode->upstreamPciPortInfo.domain = pUpstreamPortPciInfo->domain;
    pNode->upstreamPciPortInfo.bus = pUpstreamPortPciInfo->bus;
    pNode->upstreamPciPortInfo.device = pUpstreamPortPciInfo->device;
    pNode->upstreamPciPortInfo.function = pUpstreamPortPciInfo->func;
    pNode->upstreamPciPortInfo.bValid = pUpstreamPortPciInfo->valid;

    pNode->bShutdownState = NV_FALSE;

    // Initialize all compute polcies with default values
    pNode->policyInfo.timeslice = NV2080_CTRL_CMD_GPU_COMPUTE_TIMESLICE_DEFAULT;

    // Initialize all choesnIdx to _INVALID
    for (i = 0; i < GPUDB_CLK_PROP_TOP_POLS_COUNT; ++i)
    {
        ct_assert(sizeof(pNode->clkPropTopPolsControl.chosenIdx[0]) == sizeof(NvU8));
        pNode->clkPropTopPolsControl.chosenIdx[i] = NV_U8_MAX;
    }

done:
    portSyncMutexRelease(pGpuDb->pLock);
    return status;
}

/*!
*  @brief Update/Set the compute policy config for a GPU
*
*  @param[in]   uuid        GPU uuid
*  @param[in]   policyType  Policy for which config has to be set
*  @param[in]   policyInfo  Requested policy config
*
*  @return NV_OK                           Config updated successfully
*  @return NV_ERR_INVALID_ARGUMENT         Invalid argument specified
*  @return NV_ERR_OBJECT_NOT_FOUND         GPU entry in db not found
*/
NV_STATUS
gpudbSetGpuComputePolicyConfig
(
    const NvU8              *pUuid,
    NvU32                   policyType,
    GPU_COMPUTE_POLICY_INFO *policyInfo
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuDb *pGpuDb = SYS_GET_GPUDB(pSys);
    GPU_INFO_LIST_NODE *pNode;
    NV_STATUS status = NV_ERR_OBJECT_NOT_FOUND;

    if (pUuid == NULL || policyInfo == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portSyncMutexAcquire(pGpuDb->pLock);

    pNode = _gpudbFindGpuInfoByUuid(pUuid);
    if (pNode == NULL)
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    // Store the policy specific data
    switch(policyType)
    {
        case NV2080_CTRL_GPU_COMPUTE_POLICY_TIMESLICE:
            pNode->policyInfo.timeslice = policyInfo->timeslice;
            status = NV_OK;
            break;
        default:
            status = NV_ERR_INVALID_ARGUMENT;
            break;

    }

done:
    portSyncMutexRelease(pGpuDb->pLock);
    return status;
}

/*!
*  @brief Get all compute policy configs for a GPU
*
*  @param[in]   uuid          GPU uuid
*  @param[in]   policyInfo    Pointer in which to retrieve all compute policies
*                             for the requested GPU
*
*  @return NV_OK                           Configs retrieved successfully
*  @return NV_ERR_INVALID_ARGUMENT         Invalid argument specified
*  @return NV_ERR_OBJECT_NOT_FOUND         GPU entry in db not found
*/
NV_STATUS
gpudbGetGpuComputePolicyConfigs
(
    const NvU8              *pUuid,
    GPU_COMPUTE_POLICY_INFO *policyInfo
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuDb *pGpuDb = SYS_GET_GPUDB(pSys);
    GPU_INFO_LIST_NODE *pNode;
    NV_STATUS status = NV_ERR_OBJECT_NOT_FOUND;

    if (pUuid == NULL || policyInfo == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portSyncMutexAcquire(pGpuDb->pLock);

    pNode = _gpudbFindGpuInfoByUuid(pUuid);
    if (pNode == NULL)
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    // Return the policy specific data
    portMemCopy(policyInfo, sizeof(GPU_COMPUTE_POLICY_INFO),
                &pNode->policyInfo, sizeof(GPU_COMPUTE_POLICY_INFO));
    status = NV_OK;

done:
    portSyncMutexRelease(pGpuDb->pLock);
    return status;
}

/*!
*  @brief Set clock policies control for a GPU
*
*  @param[in]   pUuid         Pointer to GPU uuid
*  @param[in]   pControl      Pointer to the control tuple
*
*  @return NV_OK                           Configs retrieved successfully
*  @return NV_ERR_INVALID_ARGUMENT         Invalid argument specified
*  @return NV_ERR_OBJECT_NOT_FOUND         GPU entry in db not found
*/
NV_STATUS
gpudbSetClockPoliciesControl
(
    const NvU8 *pUuid,
    GPU_CLK_PROP_TOP_POLS_CONTROL *pControl
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuDb *pGpuDb = SYS_GET_GPUDB(pSys);
    GPU_INFO_LIST_NODE *pNode;
    NV_STATUS status = NV_ERR_OBJECT_NOT_FOUND;

    if (pUuid == NULL || pControl == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portSyncMutexAcquire(pGpuDb->pLock);

    pNode = _gpudbFindGpuInfoByUuid(pUuid);
    if (pNode == NULL)
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    portMemCopy(&pNode->clkPropTopPolsControl,
                sizeof(GPU_CLK_PROP_TOP_POLS_CONTROL),
                pControl,
                sizeof(GPU_CLK_PROP_TOP_POLS_CONTROL));

    status = NV_OK;
done:
    portSyncMutexRelease(pGpuDb->pLock);
    return status;
}

/*!
*  @brief Get clock policies control for a GPU
*
*  @param[in]   pUuid         Pointer to GPU uuid
*  @param[out]  pControl      Pointer to the control tuple
*
*  @return NV_OK                           Configs retrieved successfully
*  @return NV_ERR_INVALID_ARGUMENT         Invalid argument specified
*  @return NV_ERR_OBJECT_NOT_FOUND         GPU entry in db not found
*/
NV_STATUS
gpudbGetClockPoliciesControl
(
    const NvU8 *pUuid,
    GPU_CLK_PROP_TOP_POLS_CONTROL *pControl
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuDb *pGpuDb = SYS_GET_GPUDB(pSys);
    GPU_INFO_LIST_NODE *pNode;
    NV_STATUS status = NV_ERR_OBJECT_NOT_FOUND;

    if (pUuid == NULL || pControl == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    portSyncMutexAcquire(pGpuDb->pLock);

    pNode = _gpudbFindGpuInfoByUuid(pUuid);
    if (pNode == NULL)
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    portMemCopy(pControl,
                sizeof(GPU_CLK_PROP_TOP_POLS_CONTROL),
                &pNode->clkPropTopPolsControl,
                sizeof(GPU_CLK_PROP_TOP_POLS_CONTROL));

    status = NV_OK;
done:
    portSyncMutexRelease(pGpuDb->pLock);
    return status;
}

NV_STATUS
gpudbSetShutdownState
(
    const NvU8 *pUuid
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuDb *pGpuDb = SYS_GET_GPUDB(pSys);
    GPU_INFO_LIST_NODE *pNode;
    NV_STATUS status = NV_OK;

    portSyncMutexAcquire(pGpuDb->pLock);
    pNode = _gpudbFindGpuInfoByUuid(pUuid);
    if (pNode == NULL)
    {
        status = NV_ERR_OBJECT_NOT_FOUND;
        goto done;
    }

    pNode->bShutdownState = NV_TRUE;

done:
    portSyncMutexRelease(pGpuDb->pLock);
    return status;
}
