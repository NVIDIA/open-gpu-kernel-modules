/*
 * SPDX-FileCopyrightText: Copyright (c) 2009-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "core/locks.h"
#include "gpu/gpu.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/bus/third_party_p2p.h"
#include "platform/p2p/p2p_caps.h"
#include "gpu/bus/kern_bus.h"
#include "mem_mgr/mem.h"
#include "rmapi/rs_utils.h"
#include "vgpu/rpc.h"
#include "os/os.h"

#include "class/cl503c.h"

//
// A monotonic counter as ID that's assigned to every new VidmemInfo.
// This is used to get internal VidmemInfo for persistent mappings.
//
static volatile NvU64 vidmemInfoId = 0;

//
// A monotonic counter as ID that's assigned to every new 3rd party p2p class.
//
static volatile NvU64 p2pTokenId = 0;

//
// We make sure that only one instance of NV50_THIRD_PARTY_P2P can be active at
// a time per client per GPU. It simplifies tuple(VA,size) tracking/validation
// in SW. For example, detecting duplicate/overlapping tuples.
//
NV_STATUS
thirdpartyp2pConstruct_IMPL
(
    ThirdPartyP2P *pThirdPartyP2P,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV503C_ALLOC_PARAMETERS    *pNv503cAllocParams = pParams->pAllocParams;
    NvHandle                    hClient            = pCallContext->pClient->hClient;
    NvHandle                    hThirdPartyP2P     = pParams->hResource;
    NvU32                       flags              = 0;
    OBJGPU                     *pGpu;
    CLI_THIRD_PARTY_P2P_TYPE    type;
    RsResourceRef              *pSubdeviceRef = pCallContext->pResourceRef->pParentRef;
    Subdevice                  *pSubdevice;
    NvU64                       p2pToken         = 0;
    NvU32                       peerIndex        = 0;
    NvU32                       pidIndex         = 0;
    NV_STATUS                   status           = NV_OK;
    NvU32                       pid = osGetCurrentProcess();
    RsShared                   *pShare;
    P2PTokenShare              *pP2PTokenShare;

    pSubdevice = dynamicCast(pSubdeviceRef->pResource, Subdevice);
    if (pSubdevice == NULL)
        return NV_ERR_INVALID_OBJECT_PARENT;

    pGpu = GPU_RES_GET_GPU(pThirdPartyP2P);
    if (pGpu == NULL)
        return NV_ERR_INVALID_OBJECT_PARENT;

    if (gpuIsApmFeatureEnabled(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    API_GPU_FULL_POWER_SANITY_CHECK(pGpu, NV_TRUE, NV_FALSE);

    if (pNv503cAllocParams != NULL)
    {
        flags              = pNv503cAllocParams->flags;
    }

    switch(DRF_VAL(503C, _ALLOC_PARAMETERS_FLAGS, _TYPE, flags))
    {
        case NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_BAR1:
            type = CLI_THIRD_PARTY_P2P_TYPE_BAR1;
            break;
        case NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_NVLINK:
            type = CLI_THIRD_PARTY_P2P_TYPE_NVLINK;
            break;
        case NV503C_ALLOC_PARAMETERS_FLAGS_TYPE_PROPRIETARY:
            type = CLI_THIRD_PARTY_P2P_TYPE_PROPRIETARY;
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    if (type == CLI_THIRD_PARTY_P2P_TYPE_BAR1)
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        p2pToken = portAtomicExIncrementU64(&p2pTokenId);;
    }
    else if (type == CLI_THIRD_PARTY_P2P_TYPE_NVLINK)
    {
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_COHERENT_CPU_MAPPING))
        {
            return NV_ERR_INVALID_STATE;
        }

        p2pToken = portAtomicExIncrementU64(&p2pTokenId);;
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pThirdPartyP2P->hClient          = hClient;
    pThirdPartyP2P->hThirdPartyP2P   = hThirdPartyP2P;
    pThirdPartyP2P->type             = type;
    pThirdPartyP2P->pSubdevice       = pSubdevice;
    pThirdPartyP2P->peerIndex        = peerIndex;
    pThirdPartyP2P->p2pToken         = p2pToken;
    pThirdPartyP2P->pDestroyCallback = NULL;
    pThirdPartyP2P->pData            = NULL;
    pThirdPartyP2P->pAddressRangeTree = NULL;
    pThirdPartyP2P->Node.keyStart    = hThirdPartyP2P;
    pThirdPartyP2P->Node.keyEnd      = hThirdPartyP2P;
    pThirdPartyP2P->Node.Data        = (void*)pThirdPartyP2P;
    portMemSet(pThirdPartyP2P->pidClientList, 0, sizeof(pThirdPartyP2P->pidClientList));
    mapInit(&pThirdPartyP2P->vaSpaceInfoMap, portMemAllocatorGetGlobalNonPaged());
    mapInitIntrusive(&pThirdPartyP2P->vidmemInfoMap);

    status = NV_ERR_OBJECT_NOT_FOUND;
    for (pidIndex = 0; pidIndex < CLI_THIRD_PARTY_P2P_MAX_CLIENT; pidIndex++)
    {
        if (0 == pThirdPartyP2P->pidClientList[pidIndex].pid)
        {
            pThirdPartyP2P->pidClientList[pidIndex].pid = pid;
            pThirdPartyP2P->pidClientList[pidIndex].hClient = hClient;
            status = NV_OK;
            break;
        }
    }

    status = serverAllocShare(&g_resServ, classInfo(P2PTokenShare), &pShare);

    if (status != NV_OK)
        return status;

    pP2PTokenShare = dynamicCast(pShare, P2PTokenShare);
    pP2PTokenShare->pThirdPartyP2P = pThirdPartyP2P;
    pThirdPartyP2P->pTokenShare = pP2PTokenShare;

    NV_ASSERT(status == NV_OK);
    return status;
}

static inline
NV_STATUS gpuFullPowerSanityCheck(OBJGPU *pGpu, NvBool bGpuAccess)
{
    API_GPU_FULL_POWER_SANITY_CHECK(pGpu, bGpuAccess, NV_FALSE);
    return NV_OK;
}

void
thirdpartyp2pDestruct_IMPL
(
    ThirdPartyP2P *pThirdPartyP2P
)
{
    NV_STATUS                               status = NV_OK;
    OBJGPU                                 *pGpu = GPU_RES_GET_GPU(pThirdPartyP2P);
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO        pVidmemInfo;
    CALL_CONTEXT                           *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL            *pParams;

    resGetFreeParams(staticCast(pThirdPartyP2P, RsResource), &pCallContext, &pParams);

    if (pThirdPartyP2P->pTokenShare)
        serverFreeShare(&g_resServ, staticCast(pThirdPartyP2P->pTokenShare, RsShared));

    pParams->status = gpuFullPowerSanityCheck(pGpu, NV_TRUE);
    if (pParams->status != NV_OK)
    {
        return;
    }

    mapDestroy(&pThirdPartyP2P->vaSpaceInfoMap);

    pVidmemInfo = mapFindGEQ(&pThirdPartyP2P->vidmemInfoMap, 0);
    while (pVidmemInfo != NULL)
    {
        status = CliDelThirdPartyP2PVidmemInfo(pThirdPartyP2P,
                pVidmemInfo->hMemory);
        NV_ASSERT(status == NV_OK);
        pVidmemInfo = mapFindGEQ(&pThirdPartyP2P->vidmemInfoMap, 0);
    }

    //
    // After destroying all of the vidmem info entries, there shouldn't remain
    // any entries in the address range tree.
    //
    NV_ASSERT(pThirdPartyP2P->pAddressRangeTree == NULL);

    if (pThirdPartyP2P->pDestroyCallback != NULL)
    {
        pThirdPartyP2P->pDestroyCallback(pThirdPartyP2P->pData);
    }

    pParams->status = status;
}

NV_STATUS CliGetThirdPartyP2PInfoFromToken
(
    NvU64  p2pToken,
    ThirdPartyP2P            **ppThirdPartyP2P
)
{
    ThirdPartyP2P *pThirdPartyP2P;
    RS_SHARE_ITERATOR it;

    NV_ASSERT_OR_RETURN((ppThirdPartyP2P != NULL), NV_ERR_INVALID_ARGUMENT);

    it = serverutilShareIter(classId(P2PTokenShare));

    while(serverutilShareIterNext(&it))
    {
        RsShared *pShared = it.pShared;
        P2PTokenShare *pP2PTokenShare = dynamicCast(pShared, P2PTokenShare);
        if (pP2PTokenShare == NULL)
            continue;
        pThirdPartyP2P = pP2PTokenShare->pThirdPartyP2P;

        if (pThirdPartyP2P->p2pToken == p2pToken)
        {
            *ppThirdPartyP2P = pThirdPartyP2P;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

static
NV_STATUS CliGetPlatformDataMatchFromVidMem
(
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo,
    void *pPlatformData
)
{
    NV_STATUS status;
    PNODE pNode;

    status = btreeSearch((NvU64)(NvUPtr)pPlatformData,
                         &pNode, pVidmemInfo->pMappingInfoList);
    return status;
}

NV_STATUS CliGetThirdPartyP2PPlatformData
(
    ThirdPartyP2P *pThirdPartyP2P,
    void *platformData
)
{
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO        pVidmemInfo;
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO_MAPIter vidMemMapIter = mapIterAll(&pThirdPartyP2P->vidmemInfoMap);

    while (mapIterNext(&vidMemMapIter))
    {
        pVidmemInfo = vidMemMapIter.pValue;
        if (CliGetPlatformDataMatchFromVidMem(pVidmemInfo, platformData) == NV_OK)
        {
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS CliNextThirdPartyP2PInfoWithPid
(
    OBJGPU         *pGpu,
    NvU32           pid,
    NvHandle        hClient,
    RmClient      **ppClientOut,
    ThirdPartyP2P **ppThirdPartyP2P
)
{
    ThirdPartyP2P *pThirdPartyP2P;
    RS_SHARE_ITERATOR it;

    it = serverutilShareIter(classId(P2PTokenShare));

    while(serverutilShareIterNext(&it))
    {
        RsShared *pShared = it.pShared;
        P2PTokenShare *pP2PTokenShare = dynamicCast(pShared, P2PTokenShare);
        if (pP2PTokenShare == NULL)
            continue;

        pThirdPartyP2P = pP2PTokenShare->pThirdPartyP2P;

        if (NULL == *ppThirdPartyP2P)
        {
            if (thirdpartyp2pIsValidClientPid(pThirdPartyP2P, pid, hClient))
            {
                RsClient *pClient = RES_GET_CLIENT(pThirdPartyP2P);
                *ppClientOut = dynamicCast(pClient, RmClient);
                *ppThirdPartyP2P = pThirdPartyP2P;
                return NV_OK;
            }
        }
        else if (pThirdPartyP2P->p2pToken ==
                                         (*ppThirdPartyP2P)->p2pToken)
        {
            *ppClientOut = NULL;
            *ppThirdPartyP2P = NULL;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS CliAddThirdPartyP2PVASpace
(
    ThirdPartyP2P *pThirdPartyP2P,
    NvHandle       hVASpace,
    NvU32         *pVASpaceToken
)
{
    NvHandle hThirdPartyP2P = RES_GET_HANDLE(pThirdPartyP2P);
    RsClient *pClient = RES_GET_CLIENT(pThirdPartyP2P);
    CLI_THIRD_PARTY_P2P_VASPACE_INFO vaSpaceInfo;
    NvU32 vaSpaceToken;

    NV_ASSERT_OR_RETURN((pVASpaceToken != NULL), NV_ERR_INVALID_ARGUMENT);

    portMemSet(&vaSpaceInfo, 0, sizeof(CLI_THIRD_PARTY_P2P_VASPACE_INFO));

    for (vaSpaceToken = 0xfe00; vaSpaceToken < 0xff00; vaSpaceToken++)
    {
        if (mapFind(&pThirdPartyP2P->vaSpaceInfoMap, vaSpaceToken) == NULL)
            break;
    }
    if (vaSpaceToken == 0xff00)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    vaSpaceInfo.hClient = pClient->hClient;
    vaSpaceInfo.hThirdPartyP2P = hThirdPartyP2P;
    vaSpaceInfo.hVASpace = hVASpace;
    vaSpaceInfo.vaSpaceToken = vaSpaceToken;

    if (mapInsertValue(&pThirdPartyP2P->vaSpaceInfoMap, hVASpace, &vaSpaceInfo) == NULL)
    {
        if (mapFind(&pThirdPartyP2P->vaSpaceInfoMap, hVASpace) != NULL)
        {
            return NV_ERR_INSERT_DUPLICATE_NAME;
        }
        else
        {
            return NV_ERR_NO_MEMORY;
        }
    }

    if (hVASpace != 0)
    {
        RsResourceRef *pP2PRef;
        RsResourceRef *pVASpaceRef;
        if ((clientGetResourceRef(pClient, hThirdPartyP2P, &pP2PRef) == NV_OK) &&
            (clientGetResourceRef(pClient, hVASpace, &pVASpaceRef) == NV_OK))
        {
            refAddDependant(pVASpaceRef, pP2PRef);
        }
    }

    *pVASpaceToken = vaSpaceToken;

    return NV_OK;
}

NV_STATUS CliDelThirdPartyP2PVASpace
(
    ThirdPartyP2P *pThirdPartyP2P,
    NvHandle hVASpace
)
{
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO pVASpaceInfo;

    pVASpaceInfo = mapFind(&pThirdPartyP2P->vaSpaceInfoMap, hVASpace);

    if (pVASpaceInfo == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    mapRemove(&pThirdPartyP2P->vaSpaceInfoMap, pVASpaceInfo);

    return NV_OK;
}

NV_STATUS thirdpartyp2pGetNextVASpaceInfo_IMPL
(
    ThirdPartyP2P *pThirdPartyP2P,
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO *ppVASpaceInfo
)
{
    if (*ppVASpaceInfo == NULL)
    {
        *ppVASpaceInfo = mapFindGEQ(&pThirdPartyP2P->vaSpaceInfoMap, 0);
    }
    else
    {
        *ppVASpaceInfo = mapNext(&pThirdPartyP2P->vaSpaceInfoMap, *ppVASpaceInfo);
    }

    if (*ppVASpaceInfo == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }
    else
    {
        return NV_OK;
    }
}

NV_STATUS thirdpartyp2pGetVASpaceInfoFromToken_IMPL
(
    ThirdPartyP2P                     *pThirdPartyP2P,
    NvU32                              vaSpaceToken,
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO *ppVASpaceInfo
)
{
    PCLI_THIRD_PARTY_P2P_VASPACE_INFO        pVASpaceInfo;
    CLI_THIRD_PARTY_P2P_VASPACE_INFO_MAPIter vaSpaceInfoIter = mapIterAll(&pThirdPartyP2P->vaSpaceInfoMap);

    NV_ASSERT_OR_RETURN((ppVASpaceInfo != NULL), NV_ERR_INVALID_ARGUMENT);

    while(mapIterNext(&vaSpaceInfoIter))
    {
        pVASpaceInfo = vaSpaceInfoIter.pValue;

        if (pVASpaceInfo->vaSpaceToken == vaSpaceToken)
        {
            *ppVASpaceInfo = pVASpaceInfo;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS CliAddThirdPartyP2PVidmemInfo
(
    ThirdPartyP2P *pThirdPartyP2P,
    NvHandle       hMemory,
    NvU64          address,
    NvU64          size,
    NvU64          offset,
    Memory        *pMemory
)
{
    NV_STATUS status;
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo;

    NV_ASSERT_OR_RETURN((pMemory != NULL), NV_ERR_INVALID_ARGUMENT);

    pVidmemInfo = portMemAllocNonPaged(sizeof(CLI_THIRD_PARTY_P2P_VIDMEM_INFO));
    if (pVidmemInfo == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pVidmemInfo, 0, sizeof(CLI_THIRD_PARTY_P2P_VIDMEM_INFO));

    listInitIntrusive(&pVidmemInfo->mappingExtentList);

    if (!mapInsertExisting(&pThirdPartyP2P->vidmemInfoMap, hMemory, pVidmemInfo))
    {
        if (mapFind(&pThirdPartyP2P->vidmemInfoMap, hMemory) != NULL)
        {
            status = NV_ERR_INSERT_DUPLICATE_NAME;
        }
        else
        {
            status = NV_ERR_INVALID_STATE;
        }
        portMemFree(pVidmemInfo);
        return status;
    }

    pVidmemInfo->offset = offset;

    pVidmemInfo->addressRangeNode.keyStart = address;
    pVidmemInfo->addressRangeNode.keyEnd = address + size - 1;
    pVidmemInfo->addressRangeNode.Data = pVidmemInfo;

    status = btreeInsert(&pVidmemInfo->addressRangeNode,
                         &pThirdPartyP2P->pAddressRangeTree);
    if (status != NV_OK)
    {
        mapRemove(&pThirdPartyP2P->vidmemInfoMap, pVidmemInfo);
        portMemFree(pVidmemInfo);
        return status;
    }

    pVidmemInfo->hClient = RES_GET_CLIENT_HANDLE(pThirdPartyP2P);
    pVidmemInfo->hThirdPartyP2P = RES_GET_HANDLE(pThirdPartyP2P);
    pVidmemInfo->hMemory = hMemory;
    pVidmemInfo->pMemDesc = pMemory->pMemDesc;
    pVidmemInfo->id = portAtomicExIncrementU64(&vidmemInfoId);

    return NV_OK;
}

// For persistent mappings, free VidmemInfo if it's not used by any clients.
void CliDelThirdPartyP2PVidmemInfoPersistent
(
    ThirdPartyP2P                   *pThirdPartyP2P,
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO *pVidmemInfo
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    NODE *pNode = NULL;

    NV_ASSERT((pVidmemInfo != NULL) && (pThirdPartyP2P != NULL));

    btreeEnumStart(0, &pNode, pVidmemInfo->pMappingInfoList);
    if (pNode == NULL)
    {
        pRmApi->Free(pRmApi, pThirdPartyP2P->hClient, pVidmemInfo->hMemory);
    }
}

NV_STATUS CliDelThirdPartyP2PVidmemInfo
(
    ThirdPartyP2P *pThirdPartyP2P,
    NvHandle hMemory
)
{
    NV_STATUS status;
    PNODE pNode;
    PCLI_THIRD_PARTY_P2P_MAPPING_INFO pMappingInfo;
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo;
    void *pKey;
    NvBool bPendingMappings = NV_FALSE;

    pVidmemInfo = mapFind(&pThirdPartyP2P->vidmemInfoMap, hMemory);
    if (pVidmemInfo == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pNode = pVidmemInfo->pMappingInfoList;
    while (pNode != NULL)
    {
        pMappingInfo = pNode->Data;
        pKey = (void *)(NvUPtr)pNode->keyStart;

        if (pMappingInfo->pFreeCallback != NULL)
        {
            pMappingInfo->pFreeCallback(pMappingInfo->pData);
        }

        status = thirdpartyp2pDelMappingInfoByKey(pThirdPartyP2P, pKey, NV_FALSE);
        NV_ASSERT(status == NV_OK);

        pNode = pVidmemInfo->pMappingInfoList;

        bPendingMappings = NV_TRUE;
    }

    // RSYNC is needed only if there are outstanding mappings.
    if (bPendingMappings)
    {
        osWaitForIbmnpuRsync(pVidmemInfo->pMemDesc->pGpu->pOsGpuInfo);
    }

    mapRemove(&pThirdPartyP2P->vidmemInfoMap, pVidmemInfo);

    status = btreeUnlink(&pVidmemInfo->addressRangeNode,
                         &pThirdPartyP2P->pAddressRangeTree);
    NV_ASSERT(status == NV_OK);

    portMemFree(pVidmemInfo);

    return NV_OK;
}

NV_STATUS CliGetThirdPartyP2PVidmemInfoFromAddress
(
    ThirdPartyP2P                    *pThirdPartyP2P,
    NvU64                             address,
    NvU64                             length,
    NvU64                            *pOffset,
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO *ppVidmemInfo
)
{
    NV_STATUS status;
    PNODE pNode;
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo;

    NV_ASSERT_OR_RETURN((pOffset != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((ppVidmemInfo != NULL), NV_ERR_INVALID_ARGUMENT);

    status = btreeSearch(address, &pNode,
                         pThirdPartyP2P->pAddressRangeTree);
    if (status != NV_OK)
    {
        return status;
    }

    pVidmemInfo = pNode->Data;

    if (address + length - 1 > pVidmemInfo->addressRangeNode.keyEnd)
        return NV_ERR_INVALID_ARGUMENT;

    *ppVidmemInfo = pVidmemInfo;

    //
    // Adjust offset w.r.t. the memdesc associated with PCLI_THIRD_PARTY_P2P_VIDMEM_INFO,
    // so that it can be safely consumed by memdescGetPhysAddr.
    //
    *pOffset = pVidmemInfo->offset +
               (address - pVidmemInfo->addressRangeNode.keyStart);

    return NV_OK;
}

NV_STATUS CliGetThirdPartyP2PVidmemInfoFromId
(
    ThirdPartyP2P                    *pThirdPartyP2P,
    NvU64                             id,
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO **ppVidmemInfo
)
{
    NV_STATUS status;
    PNODE pNode;

    NV_ASSERT_OR_RETURN((ppVidmemInfo != NULL), NV_ERR_INVALID_ARGUMENT);

    status = btreeSearch(id, &pNode, pThirdPartyP2P->pAddressRangeTree);
    if (status != NV_OK)
    {
        return status;
    }

    *ppVidmemInfo = pNode->Data;

    return NV_OK;
}

NV_STATUS CliRegisterThirdPartyP2PMappingCallback
(
    ThirdPartyP2P                        *pThirdPartyP2P,
    NvHandle                              hMemory,
    void                                 *pKey,
    THIRD_PARTY_P2P_VIDMEM_FREE_CALLBACK *pFreeCallback,
    void                                 *pData
)
{
    NV_STATUS status;
    PCLI_THIRD_PARTY_P2P_MAPPING_INFO pMappingInfo;

    NV_ASSERT_OR_RETURN((pFreeCallback != NULL), NV_ERR_INVALID_ARGUMENT);

    status = CliGetThirdPartyP2PMappingInfoFromKey(pThirdPartyP2P, hMemory, pKey,
                                                   &pMappingInfo);
    if (status != NV_OK)
    {
        return status;
    }

    NV_ASSERT_OR_RETURN((pMappingInfo->pFreeCallback == NULL), NV_ERR_INVALID_STATE);

    pMappingInfo->pFreeCallback = pFreeCallback;
    pMappingInfo->pData         = pData;

    return NV_OK;
}

NV_STATUS CliAddThirdPartyP2PMappingInfo
(
    ThirdPartyP2P                        *pThirdPartyP2P,
    NvHandle                              hMemory,
    void                                 *pKey,
    THIRD_PARTY_P2P_VIDMEM_FREE_CALLBACK *pFreeCallback,
    void                                 *pData,
    PCLI_THIRD_PARTY_P2P_MAPPING_INFO    *ppMappingInfo
)
{
    NV_STATUS status;
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo;
    PCLI_THIRD_PARTY_P2P_MAPPING_INFO pMappingInfo;

    NV_ASSERT_OR_RETURN((pKey != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((ppMappingInfo != NULL), NV_ERR_INVALID_ARGUMENT);

    pVidmemInfo = mapFind(&pThirdPartyP2P->vidmemInfoMap, hMemory);
    if (pVidmemInfo == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    pMappingInfo = portMemAllocNonPaged(sizeof(CLI_THIRD_PARTY_P2P_MAPPING_INFO));
    if (pMappingInfo == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemSet(pMappingInfo, 0, sizeof(CLI_THIRD_PARTY_P2P_MAPPING_INFO));

    pMappingInfo->Node.keyStart = (NvU64)(NvUPtr)pKey;
    pMappingInfo->Node.keyEnd = (NvU64)(NvUPtr)pKey;
    pMappingInfo->Node.Data = pMappingInfo;

    status = btreeInsert(&pMappingInfo->Node, &pVidmemInfo->pMappingInfoList);
    if (status != NV_OK)
    {
        portMemFree(pMappingInfo);
        return status;
    }

    pMappingInfo->pFreeCallback = pFreeCallback;
    pMappingInfo->pData = pData;

    *ppMappingInfo = pMappingInfo;

    return NV_OK;
}

NV_STATUS CliGetThirdPartyP2PMappingInfoFromKey
(
    ThirdPartyP2P                     *pThirdPartyP2P,
    NvHandle                           hMemory,
    void                              *pKey,
    PCLI_THIRD_PARTY_P2P_MAPPING_INFO *ppMappingInfo
)
{
    NV_STATUS status;
    PNODE pNode;
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO pVidmemInfo;

    NV_ASSERT_OR_RETURN((pKey != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((ppMappingInfo != NULL), NV_ERR_INVALID_ARGUMENT);

    pVidmemInfo = mapFind(&pThirdPartyP2P->vidmemInfoMap, hMemory);
    if (pVidmemInfo == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    status = btreeSearch((NvU64)(NvUPtr)pKey,
                         &pNode, pVidmemInfo->pMappingInfoList);
    if (status != NV_OK)
    {
        return status;
    }

    *ppMappingInfo = pNode->Data;

    return NV_OK;
}

static NV_STATUS _thirdpartyp2pDelMappingInfoByKey
(
    ThirdPartyP2P                    *pThirdPartyP2P,
    void                             *pKey,
    NvBool                            bIsRsyncNeeded,
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO **ppVidmemInfo
)
{
    NV_STATUS                                status;
    PNODE                                    pNode;
    PCLI_THIRD_PARTY_P2P_VIDMEM_INFO         pVidmemInfo;
    PCLI_THIRD_PARTY_P2P_MAPPING_INFO        pMappingInfo;
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfo;
    PCLI_THIRD_PARTY_P2P_MAPPING_EXTENT_INFO pExtentInfoNext;
    Subdevice                               *pSubdevice;
    OBJGPU                                  *pGpu = GPU_RES_GET_GPU(pThirdPartyP2P);
    KernelBus                               *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU64                                    length;
    NvU64                                    mappingLength;
    NvU64                                    address;
    NvU64                                    startOffset;
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO_MAPIter  vidMemMapIter;
    NvBool                                   bGpuLockTaken;
    NvBool                                   bVgpuRpc;

    NV_ASSERT_OR_RETURN((pKey != NULL), NV_ERR_INVALID_ARGUMENT);

    bGpuLockTaken = (rmDeviceGpuLockIsOwner(gpuGetInstance(pGpu)) ||
                     rmGpuLockIsOwner());

    bVgpuRpc = IS_VIRTUAL(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu);

    pSubdevice = pThirdPartyP2P->pSubdevice;

    GPU_RES_SET_THREAD_BC_STATE(pThirdPartyP2P);

    //
    // vGPU RPC is being called without GPU lock held.
    // So acquire the lock only for non-vGPU case and if
    // no locks are held.
    //
    if (!bVgpuRpc && !bGpuLockTaken)
    {
        status = rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                                         RM_LOCK_MODULES_P2P);
        NV_ASSERT_OK_OR_RETURN(status);
    }

    vidMemMapIter = mapIterAll(&pThirdPartyP2P->vidmemInfoMap);
    while (mapIterNext(&vidMemMapIter))
    {
        pVidmemInfo = vidMemMapIter.pValue;

        status = btreeSearch((NvU64)(NvUPtr)pKey,
                             &pNode, pVidmemInfo->pMappingInfoList);
        if (status == NV_OK)
        {
            pMappingInfo = pNode->Data;
            length = pMappingInfo->length;
            address = pMappingInfo->address;

            for(pExtentInfo = pMappingInfo->pStart; (pExtentInfo != NULL) && (length != 0);
                pExtentInfo = pExtentInfoNext)
            {
                pExtentInfoNext = listNext(&pVidmemInfo->mappingExtentList, pExtentInfo);
                startOffset = address - pExtentInfo->address;
                mappingLength = NV_MIN(length, (pExtentInfo->length - startOffset));

                address += mappingLength;
                length -= mappingLength;
                pExtentInfo->refCount--;
                if (pExtentInfo->refCount == 0)
                {
                    NV_PRINTF(LEVEL_INFO,
                              "Freeing P2P mapping for gpu VA: 0x%llx, length: 0x%llx\n",
                              pExtentInfo->address, pExtentInfo->length);

                    if (bVgpuRpc)
                    {
                        NV_RM_RPC_UNMAP_MEMORY(pGpu, pThirdPartyP2P->hClient,
                                               RES_GET_PARENT_HANDLE(pSubdevice),
                                               pVidmemInfo->hMemory,
                                               0,
                                               pExtentInfo->fbApertureOffset, status);
                    }
                    else
                    {
                        status = kbusUnmapFbApertureSingle(pGpu, pKernelBus,
                                                           pExtentInfo->pMemDesc,
                                                           pExtentInfo->fbApertureOffset,
                                                           pExtentInfo->length,
                                                           BUS_MAP_FB_FLAGS_MAP_UNICAST);
                    }
                    NV_ASSERT(status == NV_OK);

                    listRemove(&pVidmemInfo->mappingExtentList, pExtentInfo);

                    pSubdevice->P2PfbMappedBytes -= pExtentInfo->length;
                    memdescDestroy(pExtentInfo->pMemDesc);
                    portMemFree(pExtentInfo);
                }
            }
            NV_ASSERT(length == 0);

            status = btreeUnlink(&pMappingInfo->Node,
                                 &pVidmemInfo->pMappingInfoList);
            if (status == NV_OK)
            {
                portMemFree(pMappingInfo);
            }

            if (bIsRsyncNeeded)
            {
                osWaitForIbmnpuRsync(pVidmemInfo->pMemDesc->pGpu->pOsGpuInfo);
            }

            //
            // For persistent mappings, we return the VidmemInfo and clean up
            // the internal ThirdPartyP2P object and duped memory handle.
            //
            if (ppVidmemInfo != NULL)
            {
                *ppVidmemInfo = pVidmemInfo;
                break;
            }
        }
    }

    if (!bVgpuRpc && !bGpuLockTaken)
    {
        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
    }

    return NV_OK;
}

NV_STATUS thirdpartyp2pDelMappingInfoByKey_IMPL
(
    ThirdPartyP2P                    *pThirdPartyP2P,
    void                             *pKey,
    NvBool                            bIsRsyncNeeded
)
{
    return _thirdpartyp2pDelMappingInfoByKey(pThirdPartyP2P,
                                             pKey,
                                             bIsRsyncNeeded,
                                             NULL);
}

NV_STATUS thirdpartyp2pDelPersistentMappingInfoByKey_IMPL
(
    ThirdPartyP2P                    *pThirdPartyP2P,
    void                             *pKey,
    NvBool                            bIsRsyncNeeded
)
{
    CLI_THIRD_PARTY_P2P_VIDMEM_INFO *pVidmemInfo = NULL;

    NV_ASSERT_OK_OR_RETURN(
        _thirdpartyp2pDelMappingInfoByKey(pThirdPartyP2P, pKey,
                                          bIsRsyncNeeded, &pVidmemInfo));

    CliDelThirdPartyP2PVidmemInfoPersistent(pThirdPartyP2P, pVidmemInfo);

    return NV_OK;
}

NV_STATUS
CliAddThirdPartyP2PClientPid
(
    NvHandle hClient,
    NvHandle hThirdPartyP2P,
    NvU32    pid,
    NvU32    client
)
{
    RsResourceRef *pThirdPartyP2PRef;
    ThirdPartyP2P *pThirdPartyP2P;
    NvU32          pidIndex;
    RsClient      *pClient;

    NV_ASSERT_OK_OR_RETURN(
        serverGetClientUnderLock(&g_resServ, hClient, &pClient));

    NV_ASSERT_OK_OR_RETURN(
        clientGetResourceRef(pClient,
                                  hThirdPartyP2P,
                                  &pThirdPartyP2PRef));

    pThirdPartyP2P = dynamicCast(pThirdPartyP2PRef->pResource, ThirdPartyP2P);
    if (pThirdPartyP2P == NULL)
    {
        return NV_ERR_INVALID_OBJECT;
    }

    // Do not register another client if one already exists for this PID
    if (thirdpartyp2pIsValidClientPid(pThirdPartyP2P, pid, client))
    {
        return NV_OK;
    }
    NV_ASSERT_OR_RETURN(!thirdpartyp2pIsValidClientPid(pThirdPartyP2P, pid, 0), NV_ERR_STATE_IN_USE);


    for (pidIndex = 0; pidIndex < CLI_THIRD_PARTY_P2P_MAX_CLIENT; pidIndex++)
    {
        if (0 == pThirdPartyP2P->pidClientList[pidIndex].pid)
        {
            pThirdPartyP2P->pidClientList[pidIndex].pid = pid;
            pThirdPartyP2P->pidClientList[pidIndex].hClient = client;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NvBool
thirdpartyp2pIsValidClientPid_IMPL
(
    ThirdPartyP2P* pThirdPartyP2P,
    NvU32    pid,
    NvHandle hClient
)
{
    NvU32 pidIndex;

    for (pidIndex = 0; pidIndex < CLI_THIRD_PARTY_P2P_MAX_CLIENT; pidIndex++)
    {
        if (pid == pThirdPartyP2P->pidClientList[pidIndex].pid &&
           (hClient == 0 || hClient == pThirdPartyP2P->pidClientList[pidIndex].hClient))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

NV_STATUS
CliDelThirdPartyP2PClientPid
(
    RmClient *pClient,
    NvHandle hThirdPartyP2P,
    NvU32    pid,
    NvU32    client
)
{
    NvU32 pidIndex;
    RsResource               *pRes;
    ThirdPartyP2P            *pThirdPartyP2P;

    pThirdPartyP2P = serverutilGetDerived(pClient, hThirdPartyP2P, &pRes, ThirdPartyP2P);
    if (pThirdPartyP2P == NULL)
    {
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    for (pidIndex = 0; pidIndex < CLI_THIRD_PARTY_P2P_MAX_CLIENT; pidIndex++)
    {
        if (pid == pThirdPartyP2P->pidClientList[pidIndex].pid &&
            client == pThirdPartyP2P->pidClientList[pidIndex].hClient)
        {
            pThirdPartyP2P->pidClientList[pidIndex].pid = 0;
            pThirdPartyP2P->pidClientList[pidIndex].hClient = 0;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
CliUnregisterFromThirdPartyP2P
(
    RmClient *pClient
)
{
    NV_STATUS status = NV_OK;
    ThirdPartyP2P *pThirdPartyP2P = NULL;
    NvU32 pid;
    NvHandle hClient = staticCast(pClient, RsClient)->hClient;

    pid = pClient->ProcID;

    while (1)
    {
        RmClient *pThirdPartyP2PClient;
        status = CliNextThirdPartyP2PInfoWithPid(NULL, pid, hClient, &pThirdPartyP2PClient, &pThirdPartyP2P);
        if (status != NV_OK)
        {
            return NV_OK;
        }

        CliDelThirdPartyP2PClientPid(pThirdPartyP2PClient,
                               pThirdPartyP2P->hThirdPartyP2P,
                               pid,
                               hClient);
    }

    return status;
}

NV_STATUS
shrp2pConstruct_IMPL(P2PTokenShare *pP2PTokenShare)
{
    return NV_OK;
}

void shrp2pDestruct_IMPL(P2PTokenShare *pP2PTokenShare)
{
}

void
CliUnregisterMemoryFromThirdPartyP2P
(
    Memory   *pMemory
)
{
    RsClient *pRsClient = RES_GET_CLIENT(pMemory);
    NvHandle hMemory = RES_GET_HANDLE(pMemory);
    ThirdPartyP2P *pThirdPartyP2P;
    Device *pDevice = pMemory->pDevice;
    RsResourceRef *pDeviceRef = RES_GET_REF(pDevice);
    RS_ITERATOR subDevIt;
    RS_ITERATOR it;

    subDevIt = clientRefIter(pRsClient, pDeviceRef, classId(Subdevice),
                             RS_ITERATE_CHILDREN, NV_TRUE);
    while (clientRefIterNext(pRsClient, &subDevIt))
    {
        it = clientRefIter(pRsClient, subDevIt.pResourceRef, classId(ThirdPartyP2P), \
                           RS_ITERATE_CHILDREN, NV_TRUE);

        while (clientRefIterNext(pRsClient, &it))
        {
            pThirdPartyP2P = dynamicCast(it.pResourceRef->pResource, ThirdPartyP2P);
            if (pThirdPartyP2P == NULL)
                continue;

            (void)CliDelThirdPartyP2PVidmemInfo(pThirdPartyP2P, hMemory);
        }
    }
}
