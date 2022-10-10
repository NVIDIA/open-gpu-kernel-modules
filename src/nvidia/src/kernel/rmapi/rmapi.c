/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "nvrm_registry.h"
#include "rmapi/rmapi.h"
#include "entry_points.h"
#include "resserv/rs_server.h"
#include "rmapi/rs_utils.h"
#include "gpu/gpu_resource.h"
#include "gpu/device/device.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "diagnostics/tracer.h"
#include "tls/tls.h"
#include "core/thread_state.h"
#include "gpu_mgr/gpu_mgr.h"
#include "resource_desc.h"

typedef struct
{
    PORT_RWLOCK *       pLock;
    NvU64               threadId;
    NvU64               timestamp;
    LOCK_TRACE_INFO     traceInfo;
    NvU64               tlsEntryId;

} RMAPI_LOCK;

RsServer          g_resServ;
static RM_API     g_RmApiList[RMAPI_TYPE_MAX];
static NvBool     g_bResServInit = NV_FALSE;
static RMAPI_LOCK g_RmApiLock;

static void _rmapiInitInterface(RM_API *pRmApi, API_SECURITY_INFO *pDefaultSecurityInfo, NvBool bTlsInternal,
                                NvBool bApiLockInternal, NvBool bGpuLockInternal);
static NV_STATUS _rmapiLockAlloc(void);
static void _rmapiLockFree(void);

// from rmapi_stubs.c
void rmapiInitStubInterface(RM_API *pRmApi);

NV_STATUS
rmapiInitialize
(
    void
)
{
    NV_STATUS         status = NV_OK;
    API_SECURITY_INFO secInfo = {0};

    NV_ASSERT(!g_bResServInit);

    status = _rmapiLockAlloc();

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "*** Cannot allocate rmapi locks\n");
        return status;
    }

    RsResInfoInitialize();
    status = serverConstruct(&g_resServ, RS_PRIV_LEVEL_HOST, 0);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "*** Cannot initialize resource server\n");
        _rmapiLockFree();
        return status;
    }

    rmapiControlCacheInit();

    listInit(&g_clientListBehindGpusLock, g_resServ.pAllocator);
    listInit(&g_userInfoList, g_resServ.pAllocator);

    secInfo.privLevel         = RS_PRIV_LEVEL_KERNEL;
    secInfo.paramLocation     = PARAM_LOCATION_KERNEL;

    _rmapiInitInterface(&g_RmApiList[RMAPI_EXTERNAL],                NULL,     NV_FALSE /* bTlsInternal */,  NV_FALSE /* bApiLockInternal */, NV_FALSE /* bGpuLockInternal */);
    _rmapiInitInterface(&g_RmApiList[RMAPI_EXTERNAL_KERNEL],         &secInfo, NV_FALSE /* bTlsInternal */,  NV_FALSE /* bApiLockInternal */, NV_FALSE /* bGpuLockInternal */);
    _rmapiInitInterface(&g_RmApiList[RMAPI_MODS_LOCK_BYPASS],        &secInfo, NV_FALSE /* bTlsInternal */,  NV_TRUE  /* bApiLockInternal */, NV_TRUE  /* bGpuLockInternal */);
    _rmapiInitInterface(&g_RmApiList[RMAPI_API_LOCK_INTERNAL],       &secInfo, NV_TRUE  /* bTlsInternal */,  NV_TRUE  /* bApiLockInternal */, NV_FALSE /* bGpuLockInternal */);
    _rmapiInitInterface(&g_RmApiList[RMAPI_GPU_LOCK_INTERNAL],       &secInfo, NV_TRUE  /* bTlsInternal */,  NV_TRUE  /* bApiLockInternal */, NV_TRUE  /* bGpuLockInternal */);

    rmapiInitStubInterface(&g_RmApiList[RMAPI_STUBS]);

    g_bResServInit = NV_TRUE;

    return status;
}

void
rmapiShutdown
(
    void
)
{
    if (!g_bResServInit)
        return;

    serverFreeDomain(&g_resServ, 0);
    serverDestruct(&g_resServ);
    _rmapiLockFree();

    rmapiControlCacheFree();

    g_bResServInit = NV_FALSE;
}

static void
_rmapiInitInterface
(
    RM_API            *pRmApi,
    API_SECURITY_INFO *pDefaultSecInfo,
    NvBool             bTlsInternal,
    NvBool             bApiLockInternal,
    NvBool             bGpuLockInternal
)
{
    //
    // Initialize to all stubs first, so any APIs not explicitly set here
    // will return NV_ERR_NOT_SUPPORTED if called
    //
    rmapiInitStubInterface(pRmApi);

    //
    // Init members
    //
    if (pDefaultSecInfo)
        pRmApi->defaultSecInfo = *pDefaultSecInfo;

    pRmApi->bHasDefaultSecInfo = !!pDefaultSecInfo;
    pRmApi->bTlsInternal = bTlsInternal;
    pRmApi->bApiLockInternal = bApiLockInternal;
    pRmApi->bRmSemaInternal = bApiLockInternal;
    pRmApi->bGpuLockInternal = bGpuLockInternal;
    pRmApi->pPrivateContext = NULL;

    //
    // Init function pointers
    //
    pRmApi->Alloc = rmapiAlloc;
    pRmApi->AllocWithHandle = rmapiAllocWithHandle;
    pRmApi->AllocWithSecInfo = pRmApi->bTlsInternal ? rmapiAllocWithSecInfo : rmapiAllocWithSecInfoTls;

    pRmApi->FreeClientList = rmapiFreeClientList;
    pRmApi->FreeClientListWithSecInfo = pRmApi->bTlsInternal ? rmapiFreeClientListWithSecInfo : rmapiFreeClientListWithSecInfoTls;

    pRmApi->Free = rmapiFree;
    pRmApi->FreeWithSecInfo = pRmApi->bTlsInternal ? rmapiFreeWithSecInfo : rmapiFreeWithSecInfoTls;

    pRmApi->Control = rmapiControl;
    pRmApi->ControlWithSecInfo = pRmApi->bTlsInternal ? rmapiControlWithSecInfo : rmapiControlWithSecInfoTls;

    pRmApi->DupObject = rmapiDupObject;
    pRmApi->DupObjectWithSecInfo = pRmApi->bTlsInternal ? rmapiDupObjectWithSecInfo : rmapiDupObjectWithSecInfoTls;

    pRmApi->Share = rmapiShare;
    pRmApi->ShareWithSecInfo = pRmApi->bTlsInternal ? rmapiShareWithSecInfo : rmapiShareWithSecInfoTls;

    pRmApi->MapToCpu = rmapiMapToCpu;
    pRmApi->MapToCpuWithSecInfo = pRmApi->bTlsInternal ? rmapiMapToCpuWithSecInfo : rmapiMapToCpuWithSecInfoTls;
    pRmApi->MapToCpuWithSecInfoV2 = pRmApi->bTlsInternal ? rmapiMapToCpuWithSecInfoV2 : rmapiMapToCpuWithSecInfoTlsV2;

    pRmApi->UnmapFromCpu = rmapiUnmapFromCpu;
    pRmApi->UnmapFromCpuWithSecInfo = pRmApi->bTlsInternal ? rmapiUnmapFromCpuWithSecInfo : rmapiUnmapFromCpuWithSecInfoTls;

    pRmApi->Map = rmapiMap;
    pRmApi->MapWithSecInfo = pRmApi->bTlsInternal ? rmapiMapWithSecInfo : rmapiMapWithSecInfoTls;

    pRmApi->Unmap = rmapiUnmap;
    pRmApi->UnmapWithSecInfo = pRmApi->bTlsInternal ? rmapiUnmapWithSecInfo : rmapiUnmapWithSecInfoTls;
}

RM_API *
rmapiGetInterface
(
    RMAPI_TYPE rmapiType
)
{
    return &g_RmApiList[rmapiType];
}

NV_STATUS
rmapiPrologue
(
    RM_API         *pRmApi,
    RM_API_CONTEXT *pContext
)
{
    NV_STATUS       status = NV_OK;
    return status;
}

void
rmapiEpilogue
(
    RM_API         *pRmApi,
    RM_API_CONTEXT *pContext
)
{
}

void
rmapiInitLockInfo
(
    RM_API            *pRmApi,
    NvHandle           hClient,
    RS_LOCK_INFO      *pLockInfo
)
{
    NV_ASSERT_OR_RETURN_VOID(pLockInfo != NULL);
    pLockInfo->flags = 0;
    pLockInfo->state = 0;

    if (hClient != 0)
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        if ((pCallContext != NULL) && (pCallContext->pLockInfo != NULL))
        {
            pLockInfo->state = pCallContext->pLockInfo->state;

            if ((pCallContext->pLockInfo->pClient != NULL) &&
                (pCallContext->pLockInfo->pClient->hClient == hClient))
            {
                pLockInfo->pClient = pCallContext->pLockInfo->pClient;
            }
            else
            {
                pLockInfo->state &= ~RM_LOCK_STATES_CLIENT_LOCK_ACQUIRED;
            }
        }
    }

    if (!pRmApi->bRmSemaInternal)
        pLockInfo->flags |= RM_LOCK_FLAGS_RM_SEMA;

    if (pRmApi->bApiLockInternal)
    {
        pLockInfo->state |= RM_LOCK_STATES_API_LOCK_ACQUIRED;

        // RS-TODO: Assert that API rwlock is taken if no client is locked
        if (pLockInfo->pClient == NULL)
            pLockInfo->flags |= RM_LOCK_FLAGS_NO_CLIENT_LOCK;
    }

    if (pRmApi->bGpuLockInternal)
        pLockInfo->state |= RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS;
}

static NV_STATUS
_rmapiLockAlloc(void)
{
    // Turn on by default for Linux to get some soak time
    // bug 2539044, bug 2536036: Enable by default.
    g_resServ.bUnlockedParamCopy = NV_TRUE;

    NvU32 val = 0;
    if ((osReadRegistryDword(NULL,
                            NV_REG_STR_RM_PARAM_COPY_NO_LOCK,
                            &val) == NV_OK))
    {
        g_resServ.bUnlockedParamCopy = (val != 0);
    }

    portMemSet(&g_RmApiLock, 0, sizeof(g_RmApiLock));
    g_RmApiLock.threadId = ~((NvU64)(0));
    g_RmApiLock.pLock = portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());
    if (g_RmApiLock.pLock == NULL)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    g_RmApiLock.tlsEntryId = tlsEntryAlloc();

    return NV_OK;
}

static void
_rmapiLockFree(void)
{
    portSyncRwLockDestroy(g_RmApiLock.pLock);
}

NV_STATUS
rmapiLockAcquire(NvU32 flags, NvU32 module)
{
    NV_STATUS rmStatus = NV_OK;
    NvU64 threadId = portThreadGetCurrentThreadId();

    NvU64 myPriority = 0;

    LOCK_ASSERT_AND_RETURN(!rmapiLockIsOwner());

    //
    // If a read-only lock was requested, check to see if the module is allowed
    // to take read-only locks
    //
    if ((flags & RMAPI_LOCK_FLAGS_READ) && (module != RM_LOCK_MODULES_NONE))
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        if ((pSys->apiLockModuleMask & RM_LOCK_MODULE_GRP(module)) == 0)
        {
            flags &= ~RMAPI_LOCK_FLAGS_READ;
        }
    }

    //
    // For conditional acquires and DISPATCH_LEVEL we want to exit
    // immediately without waiting.
    //
    // If RM Locking V3 Lite is not enabled, *always* acquire the API
    // lock in WRITE mode to ensure compatibility with Locking model V2
    // behavior (providing exclusive access to the resource).
    //
    flags = osApiLockAcquireConfigureFlags(flags);
    if (flags & API_LOCK_FLAGS_COND_ACQUIRE)
    {
        if ((flags & RMAPI_LOCK_FLAGS_READ))
        {
            if (!portSyncRwLockAcquireReadConditional(g_RmApiLock.pLock))
                rmStatus = NV_ERR_TIMEOUT_RETRY;
        }
        else
        {
            if (portSyncRwLockAcquireWriteConditional(g_RmApiLock.pLock))
            {
                g_RmApiLock.threadId = threadId;
            }
            else
            {
                rmStatus = NV_ERR_TIMEOUT_RETRY;
            }
        }
    }
    else
    {
        if ((flags & RMAPI_LOCK_FLAGS_READ))
        {
            portSyncRwLockAcquireRead(g_RmApiLock.pLock);
        }
        else
        {

            portSyncRwLockAcquireWrite(g_RmApiLock.pLock);
            g_RmApiLock.threadId = threadId;
        }
    }


    if (rmStatus == NV_OK)
    {
        NvU64 timestamp;
        osGetCurrentTick(&timestamp);

        if (g_RmApiLock.threadId == threadId)
            g_RmApiLock.timestamp = timestamp;

        // save off owning thread
        RMTRACE_RMLOCK(_API_LOCK_ACQUIRE);

        // add api lock trace record
        INSERT_LOCK_TRACE(&g_RmApiLock.traceInfo,
                          NV_RETURN_ADDRESS(),
                          lockTraceAcquire,
                          flags, module,
                          threadId,
                          !portSyncExSafeToSleep(),
                          myPriority,
                          timestamp);

        //
        // If enabled, reset the timeout now that we are running and off
        // the Sleep Queue.
        //
        if (threadStateGetSetupFlags() &
            THREAD_STATE_SETUP_FLAGS_DO_NOT_INCLUDE_SLEEP_TIME_ENABLED)
        {
            threadStateResetTimeout(NULL);
        }
    }

    NvP64 *pAcquireAddress = tlsEntryAcquire(g_RmApiLock.tlsEntryId);
    if (pAcquireAddress != NULL)
    {
        *pAcquireAddress = (NvP64)(NvUPtr)NV_RETURN_ADDRESS();
    }

    return rmStatus;
}

void
rmapiLockRelease(void)
{
    NvU64 threadId = portThreadGetCurrentThreadId();
    NvU64 timestamp;

    osGetCurrentTick(&timestamp);

    RMTRACE_RMLOCK(_API_LOCK_RELEASE);

    // add api lock trace record
    INSERT_LOCK_TRACE(&g_RmApiLock.traceInfo,
                      NV_RETURN_ADDRESS(),
                      lockTraceRelease,
                      0, 0,
                      threadId,
                      !portSyncExSafeToSleep(),
                      0,
                      timestamp);

    if (g_RmApiLock.threadId == threadId)
    {
        //
        // If the threadId in the global is same as current thread id, then
        // we know that it was acquired in WRITE mode.
        //
        g_RmApiLock.threadId  = ~0ull;
        g_RmApiLock.timestamp = timestamp;
        portSyncRwLockReleaseWrite(g_RmApiLock.pLock);

    }
    else
    {
        portSyncRwLockReleaseRead(g_RmApiLock.pLock);
    }

    tlsEntryRelease(g_RmApiLock.tlsEntryId);
}

NvBool
rmapiLockIsOwner(void)
{
    return tlsEntryGet(g_RmApiLock.tlsEntryId) != NvP64_NULL;
}

//
// Mark for deletion the client resources from the data base, given a GPU mask
//
void
rmapiSetDelPendingClientResourcesFromGpuMask
(
    NvU32 gpuMask
)
{
    RS_ITERATOR             it;
    RmClient              **ppClient;
    RmClient               *pClient;
    RsClient               *pRsClient;
    Device                 *pDevice;
    NvBool                  bDevicesInMask = NV_FALSE;
    OBJGPU                 *pGpu;

    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);

        it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);

        // Check that one of the devices is in the GPU mask
        bDevicesInMask = NV_FALSE;
        while (clientRefIterNext(it.pClient, &it))
        {
            pDevice = dynamicCast(it.pResourceRef->pResource, Device);

            if (!pDevice)
            {
                continue;
            }

            pGpu = GPU_RES_GET_GPU(pDevice);
            if ((gpuMask & NVBIT(gpuGetInstance(pGpu))) != 0)
            {
                bDevicesInMask = NV_TRUE;
                break;
            }
        }

        if (bDevicesInMask == NV_FALSE)
        {
            continue;
        }

        pClient->Flags |= RMAPI_CLIENT_FLAG_DELETE_PENDING;
    }
}

void
rmapiDelPendingDevices
(
    NvU32 gpuMask
)
{
    RmClient **ppClient;
    RmClient  *pClient;
    RsClient  *pRsClient;
    RS_ITERATOR it;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    ppClient = serverutilGetFirstClientUnderLock();
    while (ppClient)
    {
        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);

        if (((pClient->Flags & RMAPI_CLIENT_FLAG_DELETE_PENDING) != 0) &&
            ((pClient->Flags & RMAPI_CLIENT_FLAG_RM_INTERNAL_CLIENT) == 0))
        {
            it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
            while(clientRefIterNext(pRsClient, &it))
            {
                RsResourceRef *pDeviceRef = it.pResourceRef;
                Device *pDevice = dynamicCast(pDeviceRef->pResource, Device);

                if ((gpuMask & NVBIT(gpuGetInstance(GPU_RES_GET_GPU(pDevice)))) != 0)
                {
                    pRmApi->Free(pRmApi, pRsClient->hClient, pDeviceRef->hResource);

                    // Client's resource map has been modified, re-snap iterator
                    it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
                }
            }

        }

        ppClient = serverutilGetNextClientUnderLock(ppClient);
    }
}

void
rmapiReportLeakedDevices
(
    NvU32 gpuMask
)
{
    RmClient **ppClient;
    RmClient  *pClient;
    RsClient  *pRsClient;
    RS_ITERATOR it;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    ppClient = serverutilGetFirstClientUnderLock();
    while (ppClient)
    {
        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);

        it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
        while(clientRefIterNext(pRsClient, &it))
        {
            RsResourceRef *pDeviceRef = it.pResourceRef;
            Device *pDevice = dynamicCast(pDeviceRef->pResource, Device);

            if ((gpuMask & NVBIT(gpuGetInstance(GPU_RES_GET_GPU(pDevice)))) != 0)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Device object leak: (0x%x, 0x%x). Please file a bug against RM-core.\n",
                          pRsClient->hClient, pDeviceRef->hResource);
                NV_ASSERT(0);

                // Delete leaked resource from database
                pRmApi->Free(pRmApi, pRsClient->hClient, pDeviceRef->hResource);

                // Client's resource map has been modified, re-snap iterator
                it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
            }
        }

        ppClient = serverutilGetNextClientUnderLock(ppClient);
    }
}

//
// Delete the marked client resources
//
void
rmapiDelPendingClients
(
    void
)
{
    RmClient **ppClient;
    RmClient  *pClient;
    RsClient  *pRsClient;
    RS_ITERATOR it;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    ppClient = serverutilGetFirstClientUnderLock();
    while (ppClient)
    {
        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);
        ppClient = serverutilGetNextClientUnderLock(ppClient);
        if ((pClient->Flags & RMAPI_CLIENT_FLAG_DELETE_PENDING) != 0)
        {
            // Only free clients that have no devices left
            it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
            if (!clientRefIterNext(pRsClient, &it))
                pRmApi->Free(pRmApi, pRsClient->hClient, pRsClient->hClient);
        }
    }
}

NV_STATUS
rmapiGetClientHandlesFromOSInfo
(
    void      *pOSInfo,
    NvHandle **ppClientHandleList,
    NvU32     *pClientHandleListSize
)
{
    NvHandle   *pClientHandleList;
    NvU32       clientHandleListSize = 0;
    NvU32       k;

    RmClient **ppClient;
    RmClient **ppFirstClient;
    RmClient  *pClient;
    RsClient  *pRsClient;

    ppFirstClient = NULL;
    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        pClient = *ppClient;
        if (pClient->pOSInfo != pOSInfo)
        {
            continue;
        }
        clientHandleListSize++;

        if (NULL == ppFirstClient)
            ppFirstClient = ppClient;
    }

    if (clientHandleListSize == 0)
    {
        *pClientHandleListSize = 0;
        *ppClientHandleList = NULL;
        return NV_ERR_INVALID_ARGUMENT;
    }

    pClientHandleList = portMemAllocNonPaged(clientHandleListSize * sizeof(NvU32));
    if (pClientHandleList == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    *pClientHandleListSize = clientHandleListSize;
    *ppClientHandleList = pClientHandleList;

    k = 0;
    for (ppClient = ppFirstClient;
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);
        if (pClient->pOSInfo != pOSInfo)
        {
            continue;
        }
        pClientHandleList[k++] = pRsClient->hClient;

        if (clientHandleListSize <= k)
            break;
    }

    return NV_OK;
}

