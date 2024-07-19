/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/rmapi.h"
#include "entry_points.h"
#include "core/thread_state.h"
#include "rmapi/rs_utils.h"
#include "resserv/rs_access_map.h"
#include "resource_desc.h"
#include "class/cl0071.h"

static NV_STATUS
_RmDupObject
(
    NvHandle  hClient,
    NvHandle  hParent,
    NvHandle *phObject,
    NvHandle  hClientSrc,
    NvHandle  hObjectSrc,
    NvU32     flags,
    API_SECURITY_INFO *pSecInfo,
    RS_LOCK_INFO      *pLockInfo
)
{
    NV_STATUS rmStatus;
    RS_RES_DUP_PARAMS params;

    NV_ASSERT_OR_RETURN(phObject != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pLockInfo != NULL, NV_ERR_INVALID_ARGUMENT);

    portMemSet(&params, 0, sizeof(params));
    params.hClientSrc = hClientSrc;
    params.hResourceSrc = hObjectSrc;
    params.hClientDst = hClient;
    params.hParentDst = hParent;
    params.hResourceDst = *phObject;
    params.pSecInfo = pSecInfo;
    params.flags   = flags;
    params.pLockInfo = pLockInfo;

    rmStatus = serverCopyResource(&g_resServ, &params);

    if (rmStatus == NV_OK)
        *phObject = params.hResourceDst;

    return rmStatus;
}

NV_STATUS
rmapiDupObject
(
    RM_API   *pRmApi,
    NvHandle  hClient,
    NvHandle  hParent,
    NvHandle *phObject,
    NvHandle  hClientSrc,
    NvHandle  hObjectSrc,
    NvU32     flags
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->DupObjectWithSecInfo(pRmApi, hClient, hParent, phObject, hClientSrc, hObjectSrc,
                                        flags, &pRmApi->defaultSecInfo);
}

NV_STATUS
rmapiDupObjectWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hParent,
    NvHandle          *phObject,
    NvHandle           hClientSrc,
    NvHandle           hObjectSrc,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS status;
    RM_API_CONTEXT rmApiContext = {0};
    RS_LOCK_INFO lockInfo;

    NV_PRINTF(LEVEL_INFO,
              "Nv04DupObject: hClient:0x%x hParent:0x%x hObject:0x%x\n",
              hClient, hParent, *phObject);
    NV_PRINTF(LEVEL_INFO,
              "Nv04DupObject:  hClientSrc:0x%x hObjectSrc:0x%x flags:0x%x\n",
              hClientSrc, hObjectSrc, flags);

    status = rmapiPrologue(pRmApi, &rmApiContext);
    if (status != NV_OK)
    {
        return status;
    }

    portMemSet(&lockInfo, 0, sizeof(lockInfo));
    status = rmapiInitLockInfo(pRmApi, hClient, hClientSrc, &lockInfo);
    if (status != NV_OK)
    {
        rmapiEpilogue(pRmApi, &rmApiContext);
        return NV_OK;
    }

    status = _RmDupObject(hClient, hParent, phObject, hClientSrc, hObjectSrc, flags, pSecInfo, &lockInfo);

    rmapiEpilogue(pRmApi, &rmApiContext);

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "...handle dup complete\n");
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Nv04DupObject: dup failed; status: %s (0x%08x)\n",
                  nvstatusToString(status), status);
        NV_PRINTF(LEVEL_WARNING,
                  "Nv04DupObject:  hClient:0x%x hParent:0x%x hObject:0x%x\n",
                  hClient, hParent, *phObject);
        NV_PRINTF(LEVEL_WARNING,
                  "Nv04DupObject:  hClientSrc:0x%x hObjectSrc:0x%x flags:0x%x\n",
                  hClientSrc, hObjectSrc, flags);
    }

    return status;
}

NV_STATUS
rmapiDupObjectWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hParent,
    NvHandle          *phObject,
    NvHandle           hClientSrc,
    NvHandle           hObjectSrc,
    NvU32              flags,
    API_SECURITY_INFO *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiDupObjectWithSecInfo(pRmApi, hClient, hParent, phObject, hClientSrc, hObjectSrc, flags, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}


static NV_STATUS
_RmShare
(
    NvHandle           hClient,
    NvHandle           hObject,
    RS_SHARE_POLICY   *pSharePolicy,
    API_SECURITY_INFO *pSecInfo,
    RS_LOCK_INFO      *pLockInfo
)
{
    RS_RES_SHARE_PARAMS params;
    portMemSet(&params, 0, sizeof(params));
    params.hClient = hClient;
    params.hResource = hObject;
    params.pSharePolicy = pSharePolicy;
    params.pSecInfo = pSecInfo;
    params.pLockInfo = pLockInfo;

    return serverShareResourceAccess(&g_resServ, &params);
}

NV_STATUS
rmapiShare
(
    RM_API          *pRmApi,
    NvHandle         hClient,
    NvHandle         hObject,
    RS_SHARE_POLICY *pSharePolicy
)
{
    if (!pRmApi->bHasDefaultSecInfo)
        return NV_ERR_NOT_SUPPORTED;

    return pRmApi->ShareWithSecInfo(pRmApi, hClient, hObject, pSharePolicy,
                                          &pRmApi->defaultSecInfo);
}

NV_STATUS
rmapiShareWithSecInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    RS_SHARE_POLICY   *pSharePolicy,
    API_SECURITY_INFO *pSecInfo
)
{
    NV_STATUS status;
    RM_API_CONTEXT rmApiContext = {0};
    RS_LOCK_INFO lockInfo;
    NvHandle hSecondClient = NV01_NULL_OBJECT;

    NV_PRINTF(LEVEL_INFO,
              "Nv04Share: hClient:0x%x hObject:0x%x pSharePolicy:%p\n",
              hClient, hObject, pSharePolicy);

    status = rmapiPrologue(pRmApi, &rmApiContext);
    if (status != NV_OK)
    {
        return status;
    }

    if ((pSecInfo->paramLocation == PARAM_LOCATION_KERNEL) &&
        (pSharePolicy->type == RS_SHARE_TYPE_CLIENT))
    {
        hSecondClient = pSharePolicy->target;
    }

    portMemSet(&lockInfo, 0, sizeof(lockInfo));
    status = rmapiInitLockInfo(pRmApi, hClient, hSecondClient, &lockInfo);
    if (status != NV_OK)
    {
        rmapiEpilogue(pRmApi, &rmApiContext);
        return NV_OK;
    }

    //
    // Currently, Share should have no internal callers.
    // If this changes and one takes a client lock, this could mess with
    // Share since it may require two clients when sharing with SHARE_TYPE_CLIENT.
    // Assert this for now, handle it properly if this ever changes (See DupObject)
    //
    NV_ASSERT (lockInfo.pClient == NULL);

    status = _RmShare(hClient, hObject, pSharePolicy, pSecInfo, &lockInfo);

    rmapiEpilogue(pRmApi, &rmApiContext);

    if (status == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "...resource share complete\n");
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Nv04Share: share failed; status: %s (0x%08x)\n",
                  nvstatusToString(status), status);
        NV_PRINTF(LEVEL_WARNING,
                  "Nv04Share:  hClient:0x%x hObject:0x%x pSharePolicy:%p\n",
                  hClient, hObject, pSharePolicy);
    }

    return status;
}

NV_STATUS
rmapiShareWithSecInfoTls
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    NvHandle           hObject,
    RS_SHARE_POLICY   *pSharePolicy,
    API_SECURITY_INFO *pSecInfo
)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS         status;

    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    status = rmapiShareWithSecInfo(pRmApi, hClient, hObject, pSharePolicy, pSecInfo);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);

    return status;
}

NV_STATUS
serverCopyResourceLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_RES_DUP_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    NV_ASSERT_OR_RETURN(pAccess != NULL, NV_ERR_INVALID_ARGUMENT);

    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_COPY))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;
    return NV_OK;
}

NV_STATUS
serverShareResourceLookupLockFlags
(
    RsServer *pServer,
    RS_LOCK_ENUM lock,
    RS_RES_SHARE_PARAMS *pParams,
    LOCK_ACCESS_TYPE *pAccess
)
{
    NV_ASSERT_OR_RETURN(pAccess != NULL, NV_ERR_INVALID_ARGUMENT);

    *pAccess = (serverSupportsReadOnlyLock(pServer, lock, RS_API_SHARE))
        ? LOCK_ACCESS_READ
        : LOCK_ACCESS_WRITE;
    return NV_OK;
}

NV_STATUS
serverInitGlobalSharePolicies
(
    RsServer *pServer
)
{
    RS_SHARE_POLICY sharePolicy;

    // Global default policies, these can be overridden by clients/objects
    portMemSet(&sharePolicy, 0, sizeof(sharePolicy));
    RS_ACCESS_MASK_ADD(&sharePolicy.accessMask, RS_ACCESS_DUP_OBJECT);
    sharePolicy.type = RS_SHARE_TYPE_PID;

    if (listAppendValue(&pServer->defaultInheritedSharePolicyList,
                        &sharePolicy) == NULL)
        return NV_ERR_NO_MEMORY;

    // Internal share policies, these can't be overridden

    // SMC dup policy: Do not allow duping across different SMC partition
    portMemSet(&sharePolicy, 0, sizeof(sharePolicy));
    sharePolicy.type = RS_SHARE_TYPE_SMC_PARTITION;
    sharePolicy.action = RS_SHARE_ACTION_FLAG_REQUIRE;
    RS_ACCESS_MASK_ADD(&sharePolicy.accessMask, RS_ACCESS_DUP_OBJECT);

    if (listAppendValue(&pServer->globalInternalSharePolicyList,
                        &sharePolicy) == NULL)
        return NV_ERR_NO_MEMORY;

    // FM dup policy: Allow FM to dup any user-mode client's resource.
    portMemSet(&sharePolicy, 0, sizeof(sharePolicy));
    sharePolicy.type = RS_SHARE_TYPE_FM_CLIENT;
    RS_ACCESS_MASK_ADD(&sharePolicy.accessMask, RS_ACCESS_DUP_OBJECT);

    if (listAppendValue(&pServer->globalInternalSharePolicyList,
                        &sharePolicy) == NULL)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

NV_STATUS serverUpdateLockFlagsForCopy(RsServer *pServer, RS_RES_DUP_PARAMS *pParams)
{
    RS_RESOURCE_DESC  *pResDesc;
    RS_LOCK_INFO *pLockInfo = pParams->pLockInfo;

    if (pParams->pSrcRef == NULL)
        return NV_ERR_INVALID_STATE;

    pResDesc = RsResInfoByExternalClassId(pParams->pSrcRef->externalClassId);
    if (pResDesc == NULL)
        return NV_ERR_INVALID_OBJECT;

    if (!(pResDesc->flags & RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_DUP))
    {
        pLockInfo->flags |= RM_LOCK_FLAGS_NO_GPUS_LOCK;
    }

    if (pResDesc->flags & RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_DUP)
    {
        pLockInfo->flags |= RM_LOCK_FLAGS_GPU_GROUP_LOCK;
    }

    pLockInfo->pContextRef = pParams->pSrcRef->pParentRef;

    return NV_OK;
}
