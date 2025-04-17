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
#include "ctrl/ctrl0000/ctrl0000system.h"

typedef struct
{
    PORT_RWLOCK *       pLock;
    NvU64               threadId;
    NvU64               timestamp;
    LOCK_TRACE_INFO     traceInfo;
    NvU64               tlsEntryId;
    volatile NvU32      contentionCount;
    NvU32               lowPriorityAging;
    volatile NvU64      totalWaitTime;
    volatile NvU64      totalRwHoldTime;
    volatile NvU64      totalRoHoldTime;
} RMAPI_LOCK;

RsServer          g_resServ;
static RM_API     g_RmApiList[RMAPI_TYPE_MAX];
static NvBool     g_bResServInit = NV_FALSE;
static RMAPI_LOCK g_RmApiLock;

static NvU64 g_rtd3PmPathThreadId = ~0ULL;

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
        goto failed;
    }

    status = rmapiControlCacheInit();
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "*** Cannot initialize rmapi cache\n");
        goto failed_free_lock;
    }

    RsResInfoInitialize();
    status = serverConstruct(&g_resServ, RS_PRIV_LEVEL_HOST, 0);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "*** Cannot initialize resource server\n");
        goto failed_free_cache;
    }

    serverSetClientHandleBase(&g_resServ, RS_CLIENT_HANDLE_BASE);

    listInit(&g_clientListBehindGpusLock, g_resServ.pAllocator);
    listInit(&g_userInfoList, g_resServ.pAllocator);
    multimapInit(&g_osInfoList, g_resServ.pAllocator);

    secInfo.privLevel         = RS_PRIV_LEVEL_KERNEL;
    secInfo.paramLocation     = PARAM_LOCATION_KERNEL;

    _rmapiInitInterface(&g_RmApiList[RMAPI_EXTERNAL],                NULL,     NV_FALSE /* bTlsInternal */,  NV_FALSE /* bApiLockInternal */, NV_FALSE /* bGpuLockInternal */);
    _rmapiInitInterface(&g_RmApiList[RMAPI_EXTERNAL_KERNEL],         &secInfo, NV_FALSE /* bTlsInternal */,  NV_FALSE /* bApiLockInternal */, NV_FALSE /* bGpuLockInternal */);
    _rmapiInitInterface(&g_RmApiList[RMAPI_MODS_LOCK_BYPASS],        &secInfo, NV_FALSE /* bTlsInternal */,  NV_TRUE  /* bApiLockInternal */, NV_TRUE  /* bGpuLockInternal */);
    _rmapiInitInterface(&g_RmApiList[RMAPI_API_LOCK_INTERNAL],       &secInfo, NV_TRUE  /* bTlsInternal */,  NV_TRUE  /* bApiLockInternal */, NV_FALSE /* bGpuLockInternal */);
    _rmapiInitInterface(&g_RmApiList[RMAPI_GPU_LOCK_INTERNAL],       &secInfo, NV_TRUE  /* bTlsInternal */,  NV_TRUE  /* bApiLockInternal */, NV_TRUE  /* bGpuLockInternal */);

    rmapiInitStubInterface(&g_RmApiList[RMAPI_STUBS]);

    g_bResServInit = NV_TRUE;

    return NV_OK;

failed_free_cache:
        rmapiControlCacheFree();
failed_free_lock:
        _rmapiLockFree();
failed:
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

    pRmApi->DisableClients = rmapiDisableClients;
    pRmApi->DisableClientsWithSecInfo = pRmApi->bTlsInternal ? rmapiDisableClientsWithSecInfo : rmapiDisableClientsWithSecInfoTls;

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

static void
_rmapiUnrefGpuAccessNeeded
(
    NvU32           gpuMask
)
{
    NvU32           gpuInstance = 0;
    OBJGPU         *pGpu = NULL;

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        osUnrefGpuAccessNeeded(pGpu->pOsGpuInfo);
    }
}

static NV_STATUS
_rmapiRefGpuAccessNeeded
(
    NvU32          *pGpuMask
)
{
    NV_STATUS       status = NV_OK;
    NvU32           mask = 0;
    NvU32           gpuInstance = 0;
    OBJGPU         *pGpu = NULL;

    status = gpumgrGetGpuAttachInfo(NULL, &mask);
    if (status != NV_OK)
    {
        return status;
    }

    while ((pGpu = gpumgrGetNextGpu(mask, &gpuInstance)) != NULL)
    {
        status = osRefGpuAccessNeeded(pGpu->pOsGpuInfo);
        if (status != NV_OK)
        {
            goto unref;
        }

       /*
        *_rmapiRefGpuAccessNeeded records the gpuMask
        * during ref up and this is used to unref exact same
        * GPUs in _rmapiUnrefGpuAccessNeeded. This is done
        * to protect against obtaining incorrect pGpu if the mask
        * changes due to a RM_API called between ref/unref
        * sequence.
        */
        *pGpuMask |= (1 << pGpu->gpuInstance);
    }

unref:
    if (status != NV_OK)
    {
        _rmapiUnrefGpuAccessNeeded(*pGpuMask);
    }
    return status;
}

NV_STATUS
rmapiPrologue
(
    RM_API         *pRmApi,
    RM_API_CONTEXT *pContext
)
{
    NV_STATUS       status = NV_OK;
    NvBool          bApiLockTaken = NV_FALSE;
    NvU32           mask;

    NV_ASSERT_OR_RETURN(pRmApi != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pContext != NULL, NV_ERR_INVALID_ARGUMENT);

    /*
     * Check for external clients. This condition is checked here
     * in order to avoid a check at all caller sites of
     * rmapiPrologue. Effectively rmapiprologue is a no-op for
     * internal clients.
     */
    if (!pRmApi->bTlsInternal)
    {
        mask = osGetDynamicPowerSupportMask();
        if (!mask)
            return status;
       /*
        * NOTE1: Callers of rmapiPro{Epi}logue function call may call
        * it with or without API lock taken. Hence, we check here
        * whether API lock has been taken. We take API lock if
        * it not taken already.
        * We obtain the pGPU by using the gpuMask in
        * _rmapiRef{Unref}GpuAccessNeeded. This needs API lock to be
        * safe against init/teardown of GPUs while we ref/unref
        * the GPUs. We release the lock after we have finished
        * with ref/unref, if we had taken it.
        */
       if (!rmapiLockIsOwner())
       {
           status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_CLIENT);
           if (status != NV_OK)
           {
               return status;
           }
           bApiLockTaken = NV_TRUE;
       }
       status = _rmapiRefGpuAccessNeeded(&pContext->gpuMask);
       if (bApiLockTaken == NV_TRUE)
       {
           rmapiLockRelease();
       }
    }
    return status;
}

void
rmapiEpilogue
(
    RM_API         *pRmApi,
    RM_API_CONTEXT *pContext
)
{
    NV_STATUS       status = NV_OK;
    NvBool          bApiLockTaken = NV_FALSE;
    NvU32           mask;

    NV_ASSERT_OR_RETURN_VOID(pRmApi != NULL);
    NV_ASSERT_OR_RETURN_VOID(pContext != NULL);

    /*
     * Check for external clients. This condition is checked here
     * in order to avoid a check at all caller sites of
     * rmapiEpilogue. Effectively rmapiEpilogue is a no-op for
     * internal clients.
     */
    if (!pRmApi->bTlsInternal)
    {
        mask = osGetDynamicPowerSupportMask();
        if (!mask)
            return;

       /* Please see NOTE1 */
       if (!rmapiLockIsOwner())
       {
           status = rmapiLockAcquire(RMAPI_LOCK_FLAGS_READ, RM_LOCK_MODULES_CLIENT);
           if (status != NV_OK)
           {
               return;
           }
           bApiLockTaken = NV_TRUE;
       }

       _rmapiUnrefGpuAccessNeeded(pContext->gpuMask);

       if (bApiLockTaken == NV_TRUE)
       {
           rmapiLockRelease();
       }
    }
}

NV_STATUS
rmapiInitLockInfo
(
    RM_API       *pRmApi,
    NvHandle      hClient,
    NvHandle      hSecondClient,
    RS_LOCK_INFO *pLockInfo
)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    NV_ASSERT_OR_RETURN(pLockInfo != NULL, NV_ERR_INVALID_STATE);
    pLockInfo->flags = 0;
    pLockInfo->state = 0;

    if (hClient != 0)
    {
        if ((pCallContext != NULL) && (pCallContext->pLockInfo != NULL))
        {
            pLockInfo->state = pCallContext->pLockInfo->state;

            if (!serverAllClientsLockIsOwner(&g_resServ))
            {
                // If no clients are locked, then we need to acquire client locks
                if (pCallContext->pLockInfo->pClient == NULL)
                    pLockInfo->state &= ~RM_LOCK_STATES_CLIENT_LOCK_ACQUIRED;

                // If we only need one client locked
                else if (hSecondClient == NV01_NULL_OBJECT)
                {
                    if (pCallContext->pLockInfo->pClient->hClient == hClient)
                        pLockInfo->pClient = pCallContext->pLockInfo->pClient;
                    else if ((pCallContext->pLockInfo->pSecondClient != NULL) &&
                             (pCallContext->pLockInfo->pSecondClient->hClient == hClient))
                    {
                        pLockInfo->pClient = pCallContext->pLockInfo->pSecondClient;
                    }
                    else
                        pLockInfo->state &= ~RM_LOCK_STATES_CLIENT_LOCK_ACQUIRED;
                }

                // If we only have one client locked, but we need two
                else if (pCallContext->pLockInfo->pSecondClient == NULL)
                {
                    if ((pCallContext->pLockInfo->pClient->hClient == hClient) ||
                         (pCallContext->pLockInfo->pClient->hClient == hSecondClient))
                    {
                        pLockInfo->pClient = pCallContext->pLockInfo->pClient;

                        //
                        // Special case: if both clients are the same -
                        // Set both pClient's so _serverLockDualClientWithLockInfo
                        // doesn't complain about the lock state being invalid.
                        //
                        if (hClient == hSecondClient)
                            pLockInfo->pSecondClient = pCallContext->pLockInfo->pClient;
                    }
                    else
                        pLockInfo->state &= ~RM_LOCK_STATES_CLIENT_LOCK_ACQUIRED;
                }

                // If we need two clients locked, and already have two
                else
                {
                    //
                    // Check whether both clients match, keep the original order of the
                    // clients (dual client locking always locks the lower numbered client
                    // handle first).
                    //
                    if (((pCallContext->pLockInfo->pClient->hClient == hClient) &&
                         (pCallContext->pLockInfo->pSecondClient->hClient ==
                          hSecondClient)) ||
                        ((pCallContext->pLockInfo->pClient->hClient == hSecondClient) &&
                         (pCallContext->pLockInfo->pSecondClient->hClient == hClient)))
                    {
                        pLockInfo->pClient = pCallContext->pLockInfo->pClient;
                        pLockInfo->pSecondClient = pCallContext->pLockInfo->pSecondClient;
                    }

                    // Check whether one client handle matches
                    else if ((pCallContext->pLockInfo->pClient->hClient == hClient) ||
                             (pCallContext->pLockInfo->pClient->hClient == hSecondClient))
                    {
                        pLockInfo->pClient = pCallContext->pLockInfo->pClient;
                        pLockInfo->state &= ~RM_LOCK_STATES_CLIENT_LOCK_ACQUIRED;
                    }
                    else if ((pCallContext->pLockInfo->pSecondClient->hClient ==
                              hClient) ||
                             (pCallContext->pLockInfo->pSecondClient->hClient ==
                              hSecondClient))
                    {
                        pLockInfo->pClient = pCallContext->pLockInfo->pSecondClient;
                        pLockInfo->state &= ~RM_LOCK_STATES_CLIENT_LOCK_ACQUIRED;
                    }
                    else
                        pLockInfo->state &= ~RM_LOCK_STATES_CLIENT_LOCK_ACQUIRED;
                }
            }
        }
    }

    if (!pRmApi->bRmSemaInternal)
        pLockInfo->flags |= RM_LOCK_FLAGS_RM_SEMA;

    if (pRmApi->bApiLockInternal)
    {
        pLockInfo->state |= RM_LOCK_STATES_API_LOCK_ACQUIRED;

        //
        // Don't acquire client locks if we already hold the API lock since we might've
        // already acquired RM locks that are ordered after client locks (such as higher numbered
        // client/GPU locks) and don't want to violate RM lock ordering.
        //
        if (rmapiLockIsOwner())
        {
            pLockInfo->flags |= RM_LOCK_FLAGS_NO_CLIENT_LOCK;
        }
    }

    if (pRmApi->bGpuLockInternal)
        pLockInfo->state |= RM_LOCK_STATES_ALLOW_RECURSIVE_LOCKS;

    return NV_OK;
}

static NV_STATUS
_rmapiLockAlloc(void)
{
    // Turn on by default for Linux to get some soak time
    // bug 2539044, bug 2536036: Enable by default.
    g_resServ.bUnlockedParamCopy = NV_TRUE;

    NvU32 val = 0;

    if ((osReadRegistryDword(NULL,
                            NV_REG_STR_RM_LOCKING_LOW_PRIORITY_AGING,
                            &val) == NV_OK))
    {
        g_RmApiLock.lowPriorityAging = val;
    }

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
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NV_STATUS rmStatus = NV_OK;
    NvU64 threadId = portThreadGetCurrentThreadId();

    NvU64 myPriority = 0;
    NvU64 startWaitTime = 0;

    // Make sure lock has been created
    NV_CHECK_OR_RETURN(LEVEL_ERROR, g_RmApiLock.pLock != NULL, NV_ERR_NOT_READY);

    NV_ASSERT_OR_RETURN(!rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    // Ensure that GPU locks are NEVER acquired before the API lock
    NV_ASSERT_OR_RETURN(rmGpuLocksGetOwnedMask() == 0, NV_ERR_INVALID_LOCK_STATE);

    //
    // If a read-only lock was requested, check to see if the module is allowed
    // to take read-only locks or the _FORCE flag was enabled and set.
    //
    if ((flags & RMAPI_LOCK_FLAGS_READ) && (module != RM_LOCK_MODULES_NONE))
    {
        if ((((flags & RMAPI_LOCK_FLAGS_READ_FORCE) == 0) ||
                !pSys->getProperty(pSys, PDB_PROP_SYS_ENABLE_FORCE_SHARED_LOCK)) &&
            ((pSys->apiLockModuleMask & RM_LOCK_MODULE_GRP(module)) == 0))
        {
            flags &= ~RMAPI_LOCK_FLAGS_READ;
        }
    }

    // Get start wait time measuring lock wait times
    if (pSys->getProperty(pSys, PDB_PROP_SYS_RM_LOCK_TIME_COLLECT))
        startWaitTime = osGetCurrentTick();

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
            // Conditional acquires don't care about contention or priority
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

            if (flags & RMAPI_LOCK_FLAGS_LOW_PRIORITY)
            {
                NvS32 age = g_RmApiLock.lowPriorityAging;

                portSyncRwLockAcquireWrite(g_RmApiLock.pLock);
                while ((g_RmApiLock.contentionCount > 0) && (age--))
                {
                    portSyncRwLockReleaseWrite(g_RmApiLock.pLock);
                    osDelay(10);
                    portSyncRwLockAcquireWrite(g_RmApiLock.pLock);
                }
            }
            else
            {
                portAtomicIncrementU32(&g_RmApiLock.contentionCount);
                portSyncRwLockAcquireWrite(g_RmApiLock.pLock);
                portAtomicDecrementU32(&g_RmApiLock.contentionCount);
            }
            g_RmApiLock.threadId = threadId;
        }
    }


    if (rmStatus == NV_OK)
    {
        NvU64 timestamp;
        timestamp = osGetCurrentTick();

        // Update total API lock wait time if measuring lock times
        if (pSys->getProperty(pSys, PDB_PROP_SYS_RM_LOCK_TIME_COLLECT))
            portAtomicExAddU64(&g_RmApiLock.totalWaitTime, timestamp - startWaitTime);

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

    NvP64 *pStartTime = tlsEntryAcquire(g_RmApiLock.tlsEntryId);
    if (pStartTime != NULL)
    {
        //
        // Store start time to track lock hold time. This is done
        // regardless of the value of PDB_PROP_SYS_RM_LOCK_TIME_COLLECT since
        // the API lock can be acquired before PDB properties are initialized
        // and released after they are which could lead to uninitialized memory
        // being present in TLS.
        //
        *(NvU64*)pStartTime = osGetCurrentTick();
    }

    return rmStatus;
}

void
rmapiLockRelease(void)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NvU64 threadId = portThreadGetCurrentThreadId();
    NvU64 timestamp;
    NvU64 startTime = 0;

    // Fetch start of hold time from TLS if measuring lock times
    if (pSys->getProperty(pSys, PDB_PROP_SYS_RM_LOCK_TIME_COLLECT))
        startTime = (NvU64) tlsEntryGet(g_RmApiLock.tlsEntryId);

    timestamp = osGetCurrentTick();

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

        // Update total RW API lock hold time if measuring lock times
        if (pSys->getProperty(pSys, PDB_PROP_SYS_RM_LOCK_TIME_COLLECT))
            portAtomicExAddU64(&g_RmApiLock.totalRwHoldTime, timestamp - startTime);

        portSyncRwLockReleaseWrite(g_RmApiLock.pLock);

    }
    else
    {
        // Update total RO API lock hold time if measuring lock times
        if (pSys->getProperty(pSys, PDB_PROP_SYS_RM_LOCK_TIME_COLLECT))
            portAtomicExAddU64(&g_RmApiLock.totalRoHoldTime, timestamp - startTime);

        portSyncRwLockReleaseRead(g_RmApiLock.pLock);
    }

    tlsEntryRelease(g_RmApiLock.tlsEntryId);
}

NvBool
rmapiLockIsOwner(void)
{
    return tlsEntryGet(g_RmApiLock.tlsEntryId) != 0;
}

NvBool
rmapiLockIsWriteOwner(void)
{
    NvU64 threadId = portThreadGetCurrentThreadId();

    return (rmapiLockIsOwner() && (threadId == g_RmApiLock.threadId));
}

//
// Retrieve total RM API lock wait and hold times
//
void
rmapiLockGetTimes(NV0000_CTRL_SYSTEM_GET_LOCK_TIMES_PARAMS *pParams)
{
    pParams->waitApiLock   = g_RmApiLock.totalWaitTime;
    pParams->holdRoApiLock = g_RmApiLock.totalRoHoldTime;
    pParams->holdRwApiLock = g_RmApiLock.totalRwHoldTime;
}

//
// Indicates current thread is in the RTD3 PM path (rm_transition_dynamic_power) which
// means that certain locking asserts/checks must be skipped due to inability to acquire
// the API lock in this path.
//
void rmapiEnterRtd3PmPath(void)
{
    // RTD3 path cannot be entered without the GPU lock
    NV_ASSERT(rmGpuLockIsOwner());

    NV_ASSERT(g_rtd3PmPathThreadId == ~0ULL);
    g_rtd3PmPathThreadId = portThreadGetCurrentThreadId();
}

//
// Signifies that current thread is leaving the RTD3 PM path, restoring lock
// asserting/checking behavior to normal.
//
void rmapiLeaveRtd3PmPath(void)
{
    NV_ASSERT(rmapiInRtd3PmPath());
    g_rtd3PmPathThreadId = ~0ULL;
}

//
// Checks if current thread is currently running in the RTD3 PM path.
//
NvBool rmapiInRtd3PmPath(void)
{
    return (g_rtd3PmPathThreadId == portThreadGetCurrentThreadId());
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

extern OsInfoMap g_osInfoList;

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

    RmClient  *pClient;
    RsClient  *pRsClient;

    OsInfoMapSubmap *pSubmap = NULL;
    OsInfoMapIter it;
    NvU64 key1 = (NvUPtr)pOSInfo;

    *pClientHandleListSize = 0;
    *ppClientHandleList = NULL;

    pSubmap = multimapFindSubmap(&g_osInfoList, key1);

    if (pSubmap == NULL)
        return NV_WARN_NOTHING_TO_DO;

    clientHandleListSize = multimapCountSubmapItems(&g_osInfoList, pSubmap);
    NV_PRINTF(LEVEL_INFO, "*** Found %d clients for %llx\n", clientHandleListSize, key1);

    if (clientHandleListSize == 0)
    {
        NV_ASSERT_FAILED("Empty client handle submap");
        return NV_ERR_INVALID_STATE;
    }

    pClientHandleList = portMemAllocNonPaged(clientHandleListSize * sizeof(NvU32));

    if (pClientHandleList == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    *pClientHandleListSize = clientHandleListSize;
    *ppClientHandleList = pClientHandleList;

    k = 0;
    it = multimapSubmapIterItems(&g_osInfoList, pSubmap);
    while(multimapItemIterNext(&it))
    {
        NV_ASSERT_OR_ELSE(clientHandleListSize > k, break);

        pClient = *it.pValue;
        pRsClient = staticCast(pClient, RsClient);

        NV_CHECK_OR_ELSE_STR(LEVEL_ERROR, pClient->pOSInfo == pOSInfo, "*** OS info mismatch", continue);

        pClientHandleList[k++] = pRsClient->hClient;
        NV_PRINTF(LEVEL_INFO, "*** Found: %x\n", pRsClient->hClient);
    }

    return NV_OK;
}

