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

/***************************** HW State Routines ***************************\
*                                                                           *
*         System Object Function Definitions.                               *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "core/system.h"
#include "gpu/gpu.h"
#include "gpu_mgr/gpu_mgr.h"
#include "core/locks.h"
#include "os/os.h"
#include "nvrm_registry.h"
#include "core/thread_state.h"
#include "diagnostics/tracer.h"
#include "rmosxfac.h"
#include "tls/tls.h"
#include "rmapi/rmapi.h"
#include "rmapi/client.h"
#include "core/hal_mgr.h"
#include "nvoc/rtti.h"

#include "platform/chipset/chipset.h"
#include "platform/cpu.h"
#include "platform/platform.h"
#include "diagnostics/gpu_acct.h"
#include "platform/platform_request_handler.h"
#include "gpu/external_device/gsync.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "diagnostics/journal.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "power/gpu_boost_mgr.h"
#include "compute/fabric.h"
#include "gpu_mgr/gpu_db.h"
#include "core/bin_data.h"

// local static functions
static NV_STATUS    _sysCreateOs(OBJSYS *);
static NV_STATUS    _sysCreateChildObjects(OBJSYS *);
static void         _sysDeleteChildObjects(OBJSYS *);
static void         _sysNvSwitchDetection(OBJSYS *pSys);
static void         _sysInitStaticConfig(OBJSYS *pSys);;

// Global pointer to instance of OBJSYS
OBJSYS *g_pSys = NULL;

typedef struct
{
    NvLength               childOffset;
    const NVOC_CLASS_INFO *pClassInfo;
    NvBool                 bDynamicConstruct;
} sysChildObject;

static sysChildObject sysChildObjects[] =
{
    { NV_OFFSETOF(OBJSYS, pHalMgr),         classInfo(OBJHALMGR),       NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pPfm),            classInfo(OBJPFM),          NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pHypervisor),     classInfo(OBJHYPERVISOR),   NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pOS),             classInfo(OBJOS),           NV_FALSE }, //  OS: Wrapper macros must be enabled to use :CONSTRUCT.
    { NV_OFFSETOF(OBJSYS, pCl),             classInfo(OBJCL),           NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pGpuMgr),         classInfo(OBJGPUMGR),       NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pGsyncMgr),       classInfo(OBJGSYNCMGR),     NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pGpuAcct),        classInfo(GpuAccounting),   NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pPlatformRequestHandler), classInfo(PlatformRequestHandler),  NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pRcDB),           classInfo(OBJRCDB),         NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pVmm),            classInfo(OBJVMM),          NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pKernelVgpuMgr),  classInfo(KernelVgpuMgr),   NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pGpuBoostMgr),    classInfo(OBJGPUBOOSTMGR),  NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pFabric),         classInfo(Fabric),          NV_TRUE },
    { NV_OFFSETOF(OBJSYS, pGpuDb),          classInfo(GpuDb),           NV_TRUE },
};

static void
_sysDestroyMemExportCache(OBJSYS *pSys)
{
    if (pSys->pSysMemExportModuleLock != NULL)
    {
        portSyncRwLockDestroy(pSys->pSysMemExportModuleLock);
        pSys->pSysMemExportModuleLock = NULL;
    }

    NV_ASSERT(multimapCountItems(&pSys->sysMemExportCache) == 0);

    multimapDestroy(&pSys->sysMemExportCache);
}

static NV_STATUS
_sysInitMemExportCache(OBJSYS *pSys)
{
    multimapInit(&pSys->sysMemExportCache, portMemAllocatorGetGlobalNonPaged());

    pSys->pSysMemExportModuleLock =
                    portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());
    if (pSys->pSysMemExportModuleLock == NULL)
    {
        _sysDestroyMemExportCache(pSys);
        return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

static void
_sysDestroyMemExportClient(OBJSYS *pSys)
{
    RM_API *pRmApi;

    if (pSys->hSysMemExportClient == 0)
        return;

    // Acquire lock to keep rmapiGetInterface() happy.
    NV_ASSERT(rmapiLockAcquire(API_LOCK_FLAGS_NONE,
                               RM_LOCK_MODULES_DESTROY) == NV_OK);

    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    NV_ASSERT(pRmApi->Free(pRmApi, pSys->hSysMemExportClient,
                           pSys->hSysMemExportClient) == NV_OK);

    rmapiLockRelease();

    pSys->hSysMemExportClient = 0;
}

static NV_STATUS
_sysInitMemExportClient(OBJSYS *pSys)
{
    RM_API *pRmApi;
    NV_STATUS status;

    // Acquire lock to keep rmapiGetInterface() happy.
    status = rmapiLockAcquire(API_LOCK_FLAGS_NONE, RM_LOCK_MODULES_INIT);
    if (status != NV_OK)
        return status;

    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    status = pRmApi->AllocWithHandle(pRmApi, NV01_NULL_OBJECT,
                                     NV01_NULL_OBJECT, NV01_NULL_OBJECT,
                                     NV01_ROOT, &pSys->hSysMemExportClient,
                                     sizeof(pSys->hSysMemExportClient));

    rmapiLockRelease();

    return status;
}

NV_STATUS
sysConstruct_IMPL(OBJSYS *pSys)
{
    NV_STATUS          status;
    NvU32              sec = 0;
    NvU32              uSec = 0;

    g_pSys = pSys;

    RMTRACE_INIT();
    RMTRACE_INIT_NEW();

    _sysInitStaticConfig(pSys);

    status = _sysCreateChildObjects(pSys);
    if (status != NV_OK)
    {
        goto failed;
    }

    // Use the monotonic system clock for a unique value
    osGetCurrentTime(&sec, &uSec);
    pSys->rmInstanceId = (NvU64)sec * 1000000 + (NvU64)uSec;

    {
        // Using Hypervisor native interface to detect
        OBJOS         *pOS         = SYS_GET_OS(pSys);
        OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
        if (pHypervisor)
            hypervisorDetection(pHypervisor, pOS);
    }

    status = osRmInitRm();
    if (status != NV_OK)
        goto failed;

    _sysNvSwitchDetection(pSys);

    // init cpu state
    RmInitCpuInfo();

    // allocate locks, semaphores, whatever
    status = rmLocksAlloc(pSys);
    if (status != NV_OK)
        goto failed;

    status = threadStateGlobalAlloc();
    if (status != NV_OK)
        goto failed;

    status = rmapiInitialize();
    if (status != NV_OK)
        goto failed;

    status = _sysInitMemExportCache(pSys);
    if (status != NV_OK)
        goto failed;

    status = _sysInitMemExportClient(pSys);
    if (status != NV_OK)
        goto failed;

    bindataInitialize();

    return NV_OK;

failed:

    _sysDestroyMemExportCache(pSys);

    _sysDeleteChildObjects(pSys);

    g_pSys = NULL;

    threadStateGlobalFree();

    rmapiShutdown();
    rmLocksFree(pSys);

    return status;
}

void
sysDestruct_IMPL(OBJSYS *pSys)
{

    pSys->setProperty(pSys, PDB_PROP_SYS_DESTRUCTING, NV_TRUE);

    _sysDestroyMemExportCache(pSys);
    _sysDestroyMemExportClient(pSys);

    //
    // Any of these operations might fail but go ahead and
    // attempt to free remaining resources before complaining.
    //
    listDestroy(&g_clientListBehindGpusLock);
    listDestroy(&g_userInfoList);
    multimapDestroy(&g_osInfoList);

    rmapiShutdown();
    osSyncWithRmDestroy();
    threadStateGlobalFree();
    rmLocksFree(pSys);

    //
    // Free child objects
    //
    _sysDeleteChildObjects(pSys);

    g_pSys = NULL;

    RMTRACE_DESTROY();
    RMTRACE_DESTROY_NEW();

    bindataDestroy();
}

//
// Create static system object offspring.
//
static NV_STATUS
_sysCreateChildObjects(OBJSYS *pSys)
{
    NV_STATUS status = NV_OK;
    NvU32 i, n;

    n = NV_ARRAY_ELEMENTS(sysChildObjects);

    for (i = 0; i < n; i++)
    {
        if (sysChildObjects[i].bDynamicConstruct)
        {
            NvLength offset = sysChildObjects[i].childOffset;
            Dynamic **ppChild = reinterpretCast(reinterpretCast(pSys, NvU8*) + offset, Dynamic**);
            Dynamic *pNewObj;
            status = objCreateDynamic(&pNewObj, pSys, sysChildObjects[i].pClassInfo);

            if (status == NV_OK)
            {
                *ppChild = pNewObj;
            }
        }
        else
        {
            //
            // More cases should NOT be added to this list. OBJOS needs to be
            // cleaned up to use the bDynamicConstruct path then this hack can
            // be removed.
            //
            switch (sysChildObjects[i].pClassInfo->classId)
            {
                case classId(OBJOS):
                    status = _sysCreateOs(pSys);
                    break;
                default:
                    NV_ASSERT(0);
                    status = NV_ERR_INVALID_ARGUMENT;
                    break;
            }
        }

        // RMCONFIG:  Bail on errors unless the feature/object/engine/class
        //            is simply unsupported
        if (status == NV_ERR_NOT_SUPPORTED)
            status = NV_OK;
        if (status != NV_OK) break;
    }

    return status;
}

static void
_sysDeleteChildObjects(OBJSYS *pSys)
{
    int i;

    osRmCapUnregister(&pSys->pOsRmCaps);

    for (i = NV_ARRAY_ELEMENTS(sysChildObjects) - 1; i >= 0; i--)
    {
        NvLength offset = sysChildObjects[i].childOffset;
        Dynamic **ppChild = reinterpretCast(reinterpretCast(pSys, NvU8*) + offset, Dynamic**);
        objDelete(*ppChild);
        *ppChild = NULL;
    }
}

static void
_sysRegistryOverrideResourceServer
(
    OBJSYS *pSys,
    OBJGPU *pGpu
)
{
    NvU32 data32;

    // Set read-only API lock override
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_READONLY_API_LOCK,
                            &data32) == NV_OK)
    {
        NvU32 apiMask = 0;

        if (FLD_TEST_DRF(_REG_STR_RM, _READONLY_API_LOCK, _ALLOC_RESOURCE, _ENABLE, data32))
            apiMask |= NVBIT(RS_API_ALLOC_RESOURCE);

        if (FLD_TEST_DRF(_REG_STR_RM, _READONLY_API_LOCK, _FREE_RESOURCE, _ENABLE, data32))
            apiMask |= NVBIT(RS_API_FREE_RESOURCE);

        if (FLD_TEST_DRF(_REG_STR_RM, _READONLY_API_LOCK, _MAP, _ENABLE, data32))
            apiMask |= NVBIT(RS_API_MAP);

        if (FLD_TEST_DRF(_REG_STR_RM, _READONLY_API_LOCK, _UNMAP, _ENABLE, data32))
            apiMask |= NVBIT(RS_API_UNMAP);

        if (FLD_TEST_DRF(_REG_STR_RM, _READONLY_API_LOCK, _INTER_MAP, _ENABLE, data32))
            apiMask |= NVBIT(RS_API_INTER_MAP);

        if (FLD_TEST_DRF(_REG_STR_RM, _READONLY_API_LOCK, _INTER_UNMAP, _ENABLE, data32))
            apiMask |= NVBIT(RS_API_INTER_UNMAP);

        if (FLD_TEST_DRF(_REG_STR_RM, _READONLY_API_LOCK, _CTRL, _ENABLE, data32))
            apiMask |= NVBIT(RS_API_CTRL);

        if (FLD_TEST_DRF(_REG_STR_RM, _READONLY_API_LOCK, _COPY, _ENABLE, data32))
            apiMask |= NVBIT(RS_API_COPY);

        if (FLD_TEST_DRF(_REG_STR_RM, _READONLY_API_LOCK, _SHARE, _ENABLE, data32))
            apiMask |= NVBIT(RS_API_SHARE);

        pSys->apiLockMask = apiMask;
    }
    else
    {
        pSys->apiLockMask = NVBIT(RS_API_CTRL);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_READONLY_API_LOCK_MODULE,
                            &data32) == NV_OK)
    {
        pSys->apiLockModuleMask = data32;
    }
    else
    {
        pSys->apiLockModuleMask = RM_LOCK_MODULE_GRP(RM_LOCK_MODULES_CLIENT);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_LOCK_TIME_COLLECT,
                            &data32) == NV_OK)
    {
        pSys->setProperty(pSys, PDB_PROP_SYS_RM_LOCK_TIME_COLLECT, !!data32);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CLIENT_LIST_DEFERRED_FREE,
                            &data32) == NV_OK)
    {
        pSys->bUseDeferredClientListFree = !!data32;
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CLIENT_LIST_DEFERRED_FREE_LIMIT,
                            &data32) == NV_OK)
    {
        pSys->clientListDeferredFreeLimit = data32;
    }
}

static void
_sysRegistryOverrideExternalFabricMgmt
(
    OBJSYS *pSys,
    OBJGPU *pGpu
)
{
    NvU32 data32;

    // Set external fabric management property
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_EXTERNAL_FABRIC_MGMT,
                            &data32) == NV_OK)
    {
        if (FLD_TEST_DRF(_REG_STR_RM, _EXTERNAL_FABRIC_MGMT, _MODE, _ENABLE, data32))
        {
            NV_PRINTF(LEVEL_INFO,
                      "Enabling external fabric management.\n");

            pSys->setProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED, NV_TRUE);
        }

        if (FLD_TEST_DRF(_REG_STR_RM, _EXTERNAL_FABRIC_MGMT, _MODE, _DISABLE, data32))
        {
            NV_PRINTF(LEVEL_INFO,
                      "Disabling external fabric management.\n");

            pSys->setProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED, NV_FALSE);
        }
    }
}

void
sysEnableExternalFabricMgmt_IMPL
(
    OBJSYS *pSys
)
{
    pSys->setProperty(pSys, PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED, NV_TRUE);

    NV_PRINTF(LEVEL_INFO,
              "Enabling external fabric management for Proxy NvSwitch systems.\n");
}

void
sysForceInitFabricManagerState_IMPL
(
    OBJSYS *pSys
)
{
    //
    // We should only allow force init if there is not way to run fabric
    // manager. For example, HGX-2 virtualization use-case.
    //
    if (pSys->getProperty(pSys, PDB_PROP_SYS_NVSWITCH_IS_PRESENT) ||
        pSys->getProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_REGISTERED))
    {
        NV_ASSERT(0);
        return;
    }

    pSys->setProperty(pSys, PDB_PROP_SYS_FABRIC_MANAGER_IS_INITIALIZED, NV_TRUE);

    NV_PRINTF(LEVEL_INFO,
              "Forcing fabric manager's state as initialized to unblock clients.\n");
}

static void
_sysNvSwitchDetection
(
    OBJSYS *pSys
)
{

    if (osIsNvswitchPresent())
    {
        pSys->setProperty(pSys, PDB_PROP_SYS_NVSWITCH_IS_PRESENT, NV_TRUE);

        NV_PRINTF(LEVEL_INFO, "NvSwitch is found in the system\n");

        sysEnableExternalFabricMgmt(pSys);
    }
}

/*!
 * @brief Initialize static system configuration data.
 *
 * @param[in]   pSys    SYSTEM object pointer
 */
static void
_sysInitStaticConfig(OBJSYS *pSys)
{
    portMemSet(&pSys->staticConfig, 0, sizeof(pSys->staticConfig));
    osInitSystemStaticConfig(&pSys->staticConfig);
}

NV_STATUS
coreInitializeRm(void)
{
    NV_STATUS  status;
    OBJSYS    *pSys = NULL;

    //
    // Initialize libraries used by RM
    //

    // Portable runtime init
    status = portInitialize();
    if (status != NV_OK)
        return status;

    // Required before any NvLog (NV_PRINTF) calls
    NVLOG_INIT(NULL);

    // Required before any NV_PRINTF() calls
    if (!DBG_INIT())
    {
        status = NV_ERR_GENERIC;
        return status;
    }

    //
    // Initialize OBJSYS which spawns all the RM internal modules
    //
    status = objCreate(&pSys, NVOC_NULL_OBJECT, OBJSYS);

    nvAssertInit();

    return status;
 }

void
coreShutdownRm(void)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();

    //
    // Destruct OBJSYS which frees all the RM internal modules
    //
    objDelete(pSys);

    //
    // Deinitialize libraries used by RM
    //
    nvAssertDestroy();

    DBG_DESTROY();

    NVLOG_DESTROY();

    portShutdown();
}

// Obsolete RM init function -- code should migrate to new interfaces
NvS32
RmInitRm(void)
{
    return (coreInitializeRm() == NV_OK);
}

// Obsolete RM destroy function -- code should migrate to new interfaces
NvS32
RmDestroyRm(void)
{
    coreShutdownRm();
    return NV_TRUE;
}

static NV_STATUS
_sysCreateOs(OBJSYS *pSys)
{
    OBJOS      *pOS;
    NV_STATUS   status;

    // RMCONFIG: only if OS is enabled :-)
    RMCFG_MODULE_ENABLED_OR_BAIL(OS);

    status = objCreate(&pOS, pSys, OBJOS);
    if (status != NV_OK)
    {
        return status;
    }

    status = constructObjOS(pOS);
    if (status != NV_OK)
    {
        objDelete(pOS);
        return status;
    }

    status = osRmCapRegisterSys(&pSys->pOsRmCaps);
    if (status != NV_OK)
    {
        //
        // Device objects needed for some access rights failed
        // This is not system-critical since access rights are currently disabled,
        // so continue booting, just log error.
        //
        // RS-TODO make this fail once RM Capabilities are enabled (Bug 2549938)
        //
        NV_PRINTF(LEVEL_ERROR, "RM Access Sys Cap creation failed: 0x%x\n", status);
    }

    pSys->pOS = pOS;

    return NV_OK;
}

NV_STATUS
sysCaptureState_IMPL(OBJSYS *pSys)
{
    return NV_OK;
}

OBJOS*
sysGetOs_IMPL(OBJSYS *pSys)
{
    if (pSys->pOS)
        return pSys->pOS;

    //
    // A special case for any early 'get-object' calls for the OS
    // object before there is an OS object. Some RC code called on
    // DBG_BREAKPOINT assumes an OS object exists, and can cause a crash.
    //
    PORT_BREAKPOINT_ALWAYS();

    return NULL;
}

void
sysInitRegistryOverrides_IMPL
(
    OBJSYS         *pSys
)
{
    OBJGPU         *pGpu      = NULL;
    NvU32           data32    = 0;

    if (pSys->getProperty(pSys,
                PDB_PROP_SYS_REGISTRY_OVERRIDES_INITIALIZED))
    {
        // The registry overrides, if any, have already been applied.
        return;
    }

    // Get some GPU - as of now we need some gpu to read registry.
    pGpu = gpumgrGetSomeGpu();
    if (pGpu == NULL)
    {
        // Too early call ! we can not read the registry.
        return;
    }

    if ((osReadRegistryDword(pGpu,
            NV_REG_STR_RM_ENABLE_EVENT_TRACER, &data32) == NV_OK) && data32 )
    {
        RMTRACE_ENABLE(data32);
    }

    if (osReadRegistryDword(pGpu,
            NV_REG_STR_RM_CLIENT_DATA_VALIDATION, &data32) == NV_OK)
    {
        if (FLD_TEST_DRF(_REG_STR_RM, _CLIENT_DATA_VALIDATION, _KERNEL_BUFFERS, _ENABLED, data32))
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS, NV_TRUE);
        }
        else if (FLD_TEST_DRF(_REG_STR_RM, _CLIENT_DATA_VALIDATION, _KERNEL_BUFFERS, _DISABLED, data32))
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS, NV_FALSE);
        }

        if (FLD_TEST_DRF(_REG_STR_RM, _CLIENT_DATA_VALIDATION, _HANDLE, _ENABLED, data32))
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE, NV_TRUE);
        }
        else  if (FLD_TEST_DRF(_REG_STR_RM, _CLIENT_DATA_VALIDATION, _HANDLE, _DISABLED, data32))
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE, NV_FALSE);
        }

        if (FLD_TEST_DRF(_REG_STR_RM, _CLIENT_DATA_VALIDATION, _STRICT_CLIENT, _ENABLED, data32))
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT, NV_TRUE);
        }
        else if (FLD_TEST_DRF(_REG_STR_RM, _CLIENT_DATA_VALIDATION, _STRICT_CLIENT, _DISABLED, data32))
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT, NV_FALSE);
        }

        if (FLD_TEST_DRF(_REG_STR_RM, _CLIENT_DATA_VALIDATION, _ALL, _ENABLED, data32))
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE, NV_TRUE);
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS, NV_TRUE);
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT, NV_TRUE);
        }
        else  if (FLD_TEST_DRF(_REG_STR_RM, _CLIENT_DATA_VALIDATION, _ALL, _DISABLED, data32))
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE, NV_FALSE);
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_KERNEL_BUFFERS, NV_FALSE);
            pSys->setProperty(pSys, PDB_PROP_SYS_VALIDATE_CLIENT_HANDLE_STRICT, NV_FALSE);
        }
    }

    pSys->setProperty(pSys, PDB_PROP_SYS_REGISTRY_OVERRIDES_INITIALIZED, NV_TRUE);

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_STREAM_MEMOPS,
                            &data32) == NV_OK)
    {
        if (FLD_TEST_DRF(_REG_STR_RM, _STREAM_MEMOPS, _ENABLE, _YES, data32))
        {
            pSys->setProperty(pSys, PDB_PROP_SYS_ENABLE_STREAM_MEMOPS, NV_TRUE);
        }
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_PRIORITY_BOOST,
                            &data32) == NV_OK)
    {
        if (data32 == NV_REG_STR_RM_PRIORITY_BOOST_DISABLE)
            pSys->setProperty(pSys, PDB_PROP_SYS_PRIORITY_BOOST, NV_FALSE);
        else
            pSys->setProperty(pSys, PDB_PROP_SYS_PRIORITY_BOOST, NV_TRUE);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_PRIORITY_THROTTLE_DELAY,
                            &data32) == NV_OK)
    {
        pSys->setProperty(pSys, PDB_PROP_SYS_PRIORITY_THROTTLE_DELAY_US, data32);
    }

    _sysRegistryOverrideExternalFabricMgmt(pSys, pGpu);
    _sysRegistryOverrideResourceServer(pSys, pGpu);

    if (osBugCheckOnTimeoutEnabled())
    {
        pSys->setProperty(pSys, PDB_PROP_SYS_BUGCHECK_ON_TIMEOUT, NV_TRUE);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ENABLE_ROUTE_TO_PHYSICAL_LOCK_BYPASS,
                            &data32) == NV_OK)
    {
        pSys->setProperty(pSys, PDB_PROP_SYS_ROUTE_TO_PHYSICAL_LOCK_BYPASS, !!data32);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GPU_LOCK_MIDPATH, &data32) == NV_OK)
    {
        pSys->setProperty(pSys, PDB_PROP_SYS_GPU_LOCK_MIDPATH_ENABLED, !!data32);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ENABLE_RM_TEST_ONLY_CODE,
                            &data32) == NV_OK)
    {
        pSys->setProperty(pSys, PDB_PROP_SYS_ENABLE_RM_TEST_ONLY_CODE, !!data32);
    }

    gpumgrSetGpuNvlinkBwModeFromRegistry(pGpu);
}

void
sysApplyLockingPolicy_IMPL(OBJSYS *pSys)
{
    g_resServ.bRouteToPhysicalLockBypass = pSys->getProperty(pSys, PDB_PROP_SYS_ROUTE_TO_PHYSICAL_LOCK_BYPASS);
    g_resServ.roTopLockApiMask = pSys->apiLockMask;
}

NV_STATUS
sysSyncExternalFabricMgmtWAR_IMPL
(
    OBJSYS  *pSys,
    OBJGPU  *pGpu
)
{
    NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT_PARAMS params;
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS  status = NV_OK;

    params.bExternalFabricMgmt = pSys->getProperty(pSys,
                                     PDB_PROP_SYS_FABRIC_IS_EXTERNALLY_MANAGED);

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalClient,
                             NV0000_CTRL_CMD_SYSTEM_SYNC_EXTERNAL_FABRIC_MGMT,
                             &params, sizeof(params));

    return status;
}
