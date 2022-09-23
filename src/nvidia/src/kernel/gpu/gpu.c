/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief HW State Routines: System Object Function Definitions.
 */


#include "lib/base_utils.h"
#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/inst_mem/disp_inst_mem.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/eng_desc.h"
#include "nv_ref.h"
#include "os/os.h"
#include "nvrm_registry.h"
#include "gpu_mgr/gpu_mgr.h"
#include "core/thread_state.h"
#include "core/locks.h"
#include "diagnostics/tracer.h"
#include "rmapi/client_resource.h"
#include "diagnostics/journal.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi_utils.h"
#include "core/hal_mgr.h"
#include "vgpu/rpc.h"

#include <nverror.h>

#include "gpu/nvdec/kernel_nvdec.h"
#include "gpu/sec2/kernel_sec2.h"
#include "gpu/gsp/kernel_gsp.h"
#include "platform/platform.h"
#include "platform/chipset/chipset.h"
#include "kernel/gpu/host_eng/host_eng.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/intr/intr.h"

#include "diagnostics/gpu_acct.h"

#include "g_odb.h"

typedef struct GPUCHILDINFO *PGPUCHILDINFO;
typedef struct GPUCHILDINFO  GPUCHILDINFO;

typedef struct GPUCHILDTYPE *PGPUCHILDTYPE;
typedef struct GPUCHILDTYPE  GPUCHILDTYPE;

#define  RMTRACE_ENGINE_PROFILE_EVENT(EventName, EngineId, ReadCount, WriteCount)           \
{                                                                                           \
        RMTRACE_PROBE4(generic, marker,                                                     \
                       NvU32, EngineId, sizeof(NvU32),                                      \
                       char*, EventName, sizeof(EventName),                                 \
                       NvU32, ReadCount, sizeof(NvU32),                                      \
                       NvU32, WriteCount, sizeof(NvU32));                                   \
    }

// Public interface functions

static NV_STATUS gpuRemoveMissingEngines(OBJGPU *);

// local static function
static NV_STATUS gpuCreateChildObjects(OBJGPU *, NvBool);
static NV_STATUS gpuStatePreLoad(OBJGPU *, NvU32);
static NV_STATUS gpuStatePostLoad(OBJGPU *, NvU32);
static NV_STATUS gpuStatePreUnload(OBJGPU *, NvU32);
static NV_STATUS gpuStatePostUnload(OBJGPU *, NvU32);
static void      gpuXlateHalImplToArchImpl(OBJGPU *, HAL_IMPLEMENTATION, NvU32 *, NvU32 *);
static NvBool    gpuSatisfiesTemporalOrder(OBJGPU *, HAL_IMPLEMENTATION, NvU32, NvU32);
static NvBool    gpuSatisfiesTemporalOrderMaskRev(OBJGPU *, HAL_IMPLEMENTATION, NvU32, NvU32, NvU32);
static NvBool    gpuIsT124ImplementationOrBetter(OBJGPU *);
static NvBool    gpuShouldCreateObject(PGPUCHILDINFO, PENGDESCRIPTOR, NvU32);

static void gpuDestroyMissingEngine(OBJGPU *, OBJENGSTATE *);
static void gpuRemoveMissingEngineClasses(OBJGPU *, NvU32);

static NV_STATUS _gpuCreateEngineOrderList(OBJGPU *pGpu);
static void _gpuFreeEngineOrderList(OBJGPU *pGpu);


static void _gpuInitPciHandle(OBJGPU *pGpu);
static void _gpuInitPhysicalRmApi(OBJGPU *pGpu);
static NV_STATUS _gpuAllocateInternalObjects(OBJGPU *pGpu);
static void _gpuFreeInternalObjects(OBJGPU *pGpu);

typedef struct
{
    NvS32   childOrderIndex;
    NvS32   instanceID;
    NvU32   flags;
    NvBool  bStarted;
} ENGLIST_ITER, *PENGLIST_ITER;

static ENGLIST_ITER gpuGetEngineOrderListIter(OBJGPU *pGpu, NvU32 flags);
static NvBool gpuGetNextInEngineOrderList(OBJGPU *pGpu, ENGLIST_ITER *pIt, PENGDESCRIPTOR pEngDesc);

static inline void _setPlatformNoHostbridgeDetect(NvBool bValue)
{
    OBJPFM *pPfm = SYS_GET_PFM(SYS_GET_INSTANCE());
    pPfm->setProperty(pPfm, PDB_PROP_PFM_NO_HOSTBRIDGE_DETECT, bValue);
}

// Forward declare all the class definitions so that we don't need to pull in all the headers
#define GPU_CHILD(className, accessorName, numInstances, bConstructEarly, bAlwaysCreate, gpuField) \
    extern const struct NVOC_CLASS_DEF NV_CONCATENATE(__nvoc_class_def_, className);

#include "gpu/gpu_child_list.h"


// Describes a child type (e.g.: classId(OBJCE))
struct GPUCHILDTYPE
{
    NvBool           bConstructEarly;    // bConstructEarly objects are created in a separate step. FUSE must be created
                                         // before BIF since we need to know the OPSB fuse value for enabling/disabling
                                         // certain features in bifInitRegistryOverrides
    NvBool           bAlwaysCreate;
    NvU32            instances;
    NvU32            gpuChildPtrOffset;
    const NVOC_CLASS_INFO *pClassInfo;   // NULL if engine is disabled by chip-config
};

// List of all possible GPU offspring
static GPUCHILDTYPE gpuChildTypeList[] =
{
    #define GPU_CHILD(className, accessorName, numInstances, bConstructEarly, bAlwaysCreate, gpuField) \
        { bConstructEarly, bAlwaysCreate, numInstances, NV_OFFSETOF(OBJGPU, gpuField), classInfo(className) },

    #include "gpu/gpu_child_list.h"
};

// Describes a child instance (e.g.: classId(OBJCE) instanceID #1)
struct GPUCHILDINFO
{
    NvBool           bAlwaysCreate;
    NvBool           bConstructEarly;
    ENGDESCRIPTOR    engDesc;
    NvU32            gpuChildPtrOffset;
    const NVOC_CLASS_INFO *pClassInfo;
    GPUCHILDTYPE    *pChildType;
};

static PGPUCHILDTYPE   gpuGetChildType(NVOC_CLASS_ID classId);
static NV_STATUS       gpuGetChildInfo(NVOC_CLASS_ID classId, NvU32 instanceID, PGPUCHILDINFO pChildInfoOut);
static Dynamic       **gpuGetChildPtr(OBJGPU *pGpu, NvU32 gpuChildPtrOffset);

#define GPU_NUM_CHILD_TYPES \
    ((sizeof(gpuChildTypeList) / sizeof(GPUCHILDTYPE)))

/*!
 * GFID allocation state
 */
typedef enum _gfid_alloc_state
{
    GFID_FREE = 0,
    GFID_ALLOCATED = 1
} GFID_ALLOC_STATUS;

//
// Generate a 32-bit id from domain, bus and device tuple.
//
// This is a one way function that is not guaranteed to generate a unique id for
// each domain, bus, device tuple as domain alone can be 32-bit. Historically,
// we have been assuming that the domain can only be 16-bit, but that has never
// been true on Linux and Hyper-V virtualization has exposed that by using
// arbitrary 32-bit domains for passthrough GPUs. This is the only known case
// today that requires immediate support. The domains on Hyper-V come from
// hashing some system and GPU information and are claimed to be unique even if
// we consider the lower 16-bits only. Hence, as a temporary solution, only the
// lower 16-bits are used and it's asserted that top 16-bits are only non-0 on
// Hyper-V.
//
// Long term the 32-bit ids should be changed to 64-bit or the generation scheme
// should be changed to guarantee uniqueness. Both of these are impactful as the
// biggest user of this is the commonly used 32-bit OBJGPU::gpuId.
//
NvU32 gpuGenerate32BitId(NvU32 domain, NvU8 bus, NvU8 device)
{
    NvU32 id = gpuEncodeBusDevice(bus, device);

    // Include only the lower 16-bits to match the old gpuId scheme
    id |= (domain & 0xffff) << 16;

    return id;
}

void gpuChangeComputeModeRefCount_IMPL(OBJGPU *pGpu, NvU32 command)
{
    switch(command)
    {
        case NV_GPU_COMPUTE_REFCOUNT_COMMAND_INCREMENT:
            NV_ASSERT(pGpu->computeModeRefCount >= 0);
            ++pGpu->computeModeRefCount;

            if (1 == pGpu->computeModeRefCount)
            {
                NV_PRINTF(LEVEL_INFO, "GPU (ID: 0x%x): new mode: COMPUTE\n",
                          pGpu->gpuId);

                timeoutInitializeGpuDefault(&pGpu->timeoutData, pGpu);
            }
            break;

        case NV_GPU_COMPUTE_REFCOUNT_COMMAND_DECREMENT:
            --pGpu->computeModeRefCount;
            NV_ASSERT(pGpu->computeModeRefCount >= 0);

            if (pGpu->computeModeRefCount < 0)
            {
                pGpu->computeModeRefCount = 0;
            }

            if (0 == pGpu->computeModeRefCount)
            {
                NV_PRINTF(LEVEL_INFO, "GPU (ID: 0x%x): new mode: GRAPHICS\n",
                          pGpu->gpuId);

                timeoutInitializeGpuDefault(&pGpu->timeoutData, pGpu);
            }
            break;

        default:
            NV_PRINTF(LEVEL_ERROR, "Bad command: 0x%x\n", command);
            NV_ASSERT(0);
            break;
    }
}

//
// gpuPostConstruct
//
// Called by the gpu manager to finish OBJGPU construction phase.
// Tasks handled here include binding a HAL module to the gpu
// and the construction of engine object offspring.
//
NV_STATUS
gpuPostConstruct_IMPL
(
    OBJGPU       *pGpu,
    GPUATTACHARG *pAttachArg
)
{
    NV_STATUS rmStatus;

    gpumgrAddDeviceInstanceToGpus(NVBIT(pGpu->gpuInstance));

    rmStatus = regAccessConstruct(&pGpu->registerAccess, pGpu);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to construct IO Apertures for attached devices \n");
        return rmStatus;
    }

    gpuInitChipInfo(pGpu);

    //
    // gpuInitRegistryOverrides() must be called before the child engines
    // are being created. This would ensure that the child engines
    // pick up any overrides from the GPU. We need to keep this here since the
    // gpuDetermineVirtualMode will be using these overrides
    //
    gpuInitRegistryOverrides_HAL(pGpu);
    gpuInitInstLocOverrides_HAL(pGpu);

    NV_ASSERT_OK_OR_RETURN(gpuPrivSecInitRegistryOverrides(pGpu));

    //
    // gpuDetermineVirtualMode inits hPci but only for virtualization case. So if
    // it does not init it, do here for using it for non-virtualization as well
    //
    if (pGpu->hPci == NULL)
    {
        //
        // We don't check the return status. Even if PCI handle is not obtained
        // it should not block rest of the gpu init sequence.
        //
        _gpuInitPciHandle(pGpu);
    }

    //
    // Initialize the base offset for the virtual registers for physical function
    // or baremetal
    //
    pGpu->sriovState.virtualRegPhysOffset = gpuGetVirtRegPhysOffset_HAL(pGpu);

    //
    // Check if FBHUB Poison interrupt is triggered before RM Init due
    // to VBIOS IFR on GA100. If yes, clear the FBHUB Interrupt. This WAR is
    // required for Bug 2924523 where VBIOS IFR causes FBHUB Poison intr.
    // We need to clear this before RM Init begins, as an FBHUB Poison as part of
    // RM Init is a valid interrupt
    //
    // Additional details which might be of interest exist in bug 200620015
    // comments 43-45 pertaining to the necessity of the WAR so close to the
    // register write enablement.
    //
    gpuClearFbhubPoisonIntrForBug2924523_HAL(pGpu);

    //
    // Initialize engine order before engine init/load/etc
    //
    rmStatus = _gpuCreateEngineOrderList(pGpu);
    if ( rmStatus != NV_OK )
        return rmStatus;

    gpuBuildClassDB(pGpu);

    // The first time the emulation setting is checked is in timeoutInitializeGpuDefault.
    pGpu->computeModeRefCount = 0;
    pGpu->hComputeModeReservation = NV01_NULL_OBJECT;

    // Setting default timeout values
    timeoutInitializeGpuDefault(&pGpu->timeoutData, pGpu);

    // Set 2 stage error recovery if Vista or Unix or GSP-RM.
    if (!IsAMODEL(pGpu))
    {
        pGpu->bTwoStageRcRecoveryEnabled = NV_TRUE;
    }

    // create core objects (i.e. bif)
    rmStatus = gpuCreateChildObjects(pGpu, /* bConstructEarly */ NV_TRUE);
    if (rmStatus != NV_OK)
        return rmStatus;

    gpuGetIdInfo_HAL(pGpu);
    gpuUpdateIdInfo_HAL(pGpu);

    _gpuInitPhysicalRmApi(pGpu);

    // need to get illumination values after the GPU Id
    // has been setup to allow for GPU specific settings
    gpuDeterminePersistantIllumSettings(pGpu);

    // Construct and update the engine database
    rmStatus = gpuConstructEngineTable(pGpu);
    if (rmStatus != NV_OK)
        return rmStatus;
    rmStatus = gpuUpdateEngineTable(pGpu);
    if (rmStatus != NV_OK)
        return rmStatus;

    // create remaining gpu offspring
    rmStatus = gpuCreateChildObjects(pGpu, /* bConstructEarly */ NV_FALSE);
    if (rmStatus != NV_OK)
        return rmStatus;

    gpuGetHwDefaults(pGpu);

    // Handle per-device core logic registry settings
    OBJCL *pCl  = SYS_GET_CL(SYS_GET_INSTANCE());
    if (pCl != NULL)
    {
        clInitPropertiesFromRegistry(pGpu, pCl);
    }

    // Set any state overrides required for L2 cache only mode
    if (gpuIsCacheOnlyModeEnabled(pGpu))
    {
        gpuSetCacheOnlyModeOverrides_HAL(pGpu);
    }

    // Register the OCA dump callback function.
    gpuDumpCallbackRegister(pGpu);

    // Initialize reference count for external kernel clients
    pGpu->externalKernelClientCount = 0;

    return NV_OK;
}

NV_STATUS gpuConstruct_IMPL
(
    OBJGPU *pGpu,
    NvU32   gpuInstance
)
{

    pGpu->gpuInstance = gpuInstance;

    // allocate OS-specific GPU extension area
    osInitOSHwInfo(pGpu);

    return gpuConstructPhysical(pGpu);
}

// NVOC-TODO : delete this after all Rmconfig modules migrated to NVOC
NV_STATUS
gpuBindHalLegacy_IMPL
(
    OBJGPU *pGpu,
    NvU32   chipId0,
    NvU32   chipId1
)
{
    OBJSYS    *pSys = SYS_GET_INSTANCE();
    OBJHALMGR *pHalMgr = SYS_GET_HALMGR(pSys);
    NV_STATUS  status;

    // chipId0 and chipId1 needs to be function parameter since GPU Reg read
    // is not ready at this point.
    pGpu->chipId0 = chipId0;
    pGpu->chipId1 = chipId1;

    //
    // The system object will pass PMC_BOOT_0 and PMC_BOOT_42 to all the HAL's and return the
    // one that claims it supports this chip arch/implementation
    //
    status = halmgrGetHalForGpu(pHalMgr, pGpu->chipId0, pGpu->chipId1, &pGpu->halImpl);
    if (status != NV_OK)
        return status;

    pGpu->pHal = halmgrGetHal(pHalMgr, pGpu->halImpl);

    return status;
}

static void
_gpuInitPciHandle
(
    OBJGPU *pGpu
)
{
    NvU32 domain   = gpuGetDomain(pGpu);
    NvU8  bus      = gpuGetBus(pGpu);
    NvU8  device   = gpuGetDevice(pGpu);
    NvU8  function = 0;

    pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
}

static NV_STATUS _gpuRmApiControl
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hObject,
    NvU32 cmd,
    void *pParams,
    NvU32 paramsSize
)
{
    RmCtrlParams rmCtrlParams;
    CALL_CONTEXT callCtx, *oldCtx = NULL;
    RS_LOCK_INFO lockInfo = {0};
    NV_STATUS status = NV_OK;

    OBJGPU *pGpu = (OBJGPU*)pRmApi->pPrivateContext;

    // This API is only used to route locally on monolithic or UCODE
    NV_ASSERT_OR_RETURN(!IS_GSP_CLIENT(pGpu), NV_ERR_INVALID_STATE);

    //
    // The physical API can be used on any controls and any handles and it is
    // expected to be routed correctly. However, if the caller is using the GPU
    // internal handles, we can skip the resource server overhead and make a
    // direct function call instead.
    //
    if (hClient == pGpu->hInternalClient && hObject == pGpu->hInternalSubdevice)
    {
        NV_ASSERT_OR_RETURN(pGpu->pCachedSubdevice && pGpu->pCachedRsClient, NV_ERR_INVALID_STATE);

        const struct NVOC_EXPORTED_METHOD_DEF *pEntry;
        pEntry = objGetExportedMethodDef((void*)pGpu->pCachedSubdevice, cmd);

        NV_ASSERT_OR_RETURN(pEntry != NULL, NV_ERR_NOT_SUPPORTED);

        NV_ASSERT_OR_RETURN(pEntry->paramSize == paramsSize, NV_ERR_INVALID_PARAM_STRUCT);
        NV_PRINTF(LEVEL_INFO, "GPU Internal RM control 0x%08x on gpuInst:%x hClient:0x%08x hSubdevice:0x%08x\n",
                  cmd, pGpu->gpuInstance, hClient, hObject);

        portMemSet(&rmCtrlParams, 0, sizeof(rmCtrlParams));
        rmCtrlParams.hClient    = hClient;
        rmCtrlParams.hObject    = hObject;
        rmCtrlParams.pGpu       = pGpu;
        rmCtrlParams.cmd        = cmd;
        rmCtrlParams.flags      = NVOS54_FLAGS_LOCK_BYPASS;
        rmCtrlParams.pParams    = pParams;
        rmCtrlParams.paramsSize = paramsSize;
        rmCtrlParams.secInfo.privLevel = RS_PRIV_LEVEL_KERNEL;
        rmCtrlParams.secInfo.paramLocation = PARAM_LOCATION_KERNEL;
        rmCtrlParams.bInternal  = NV_TRUE;

        lockInfo.flags = RM_LOCK_FLAGS_NO_GPUS_LOCK | RM_LOCK_FLAGS_NO_CLIENT_LOCK;
        rmCtrlParams.pLockInfo = &lockInfo;

        portMemSet(&callCtx, 0, sizeof(callCtx));
        callCtx.pResourceRef   = RES_GET_REF(pGpu->pCachedSubdevice);
        callCtx.pClient        = pGpu->pCachedRsClient;
        callCtx.secInfo        = rmCtrlParams.secInfo;
        callCtx.pServer        = &g_resServ;
        callCtx.pControlParams = &rmCtrlParams;
        callCtx.pLockInfo      = rmCtrlParams.pLockInfo;

        resservSwapTlsCallContext(&oldCtx, &callCtx);

        if (pEntry->paramSize == 0)
        {
            status = ((NV_STATUS(*)(void*))pEntry->pFunc)(pGpu->pCachedSubdevice);
        }
        else
        {
            status = ((NV_STATUS(*)(void*,void*))pEntry->pFunc)(pGpu->pCachedSubdevice, pParams);
        }

        resservRestoreTlsCallContext(oldCtx);
    }
    else
    {
        RM_API *pInternalRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pGpu->gpuInstance), NV_ERR_INVALID_LOCK_STATE);

        status = pInternalRmApi->Control(pInternalRmApi, hClient, hObject, cmd, pParams, paramsSize);
    }

    return status;
}

static NV_STATUS _gpuRmApiAllocWithHandle
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hParent,
    NvHandle hObject,
    NvU32 hClass,
    void *pAllocParams
)
{
    // Simple forwarder for now
    RM_API *pInternalRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    return pInternalRmApi->AllocWithHandle(pInternalRmApi, hClient, hParent, hObject, hClass, pAllocParams);
}
static NV_STATUS _gpuRmApiFree
(
    RM_API *pRmApi,
    NvHandle hClient,
    NvHandle hObject
)
{
    // Simple forwarder for now
    RM_API *pInternalRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    return pInternalRmApi->Free(pInternalRmApi, hClient, hObject);
}

static void
_gpuInitPhysicalRmApi
(
    OBJGPU *pGpu
)
{
    // Populate all unused APIs with stubs
    pGpu->physicalRmApi = *rmapiGetInterface(RMAPI_STUBS);
    pGpu->physicalRmApi.pPrivateContext = pGpu;

    portMemSet(&pGpu->physicalRmApi.defaultSecInfo, 0,
               sizeof(pGpu->physicalRmApi.defaultSecInfo));
    pGpu->physicalRmApi.defaultSecInfo.privLevel     = RS_PRIV_LEVEL_KERNEL;
    pGpu->physicalRmApi.defaultSecInfo.paramLocation = PARAM_LOCATION_KERNEL;
    pGpu->physicalRmApi.bHasDefaultSecInfo           = NV_TRUE;
    pGpu->physicalRmApi.bTlsInternal                 = NV_TRUE;
    pGpu->physicalRmApi.bApiLockInternal             = NV_TRUE;
    pGpu->physicalRmApi.bRmSemaInternal              = NV_TRUE;
    pGpu->physicalRmApi.bGpuLockInternal             = NV_TRUE;

    // Only initialize the methods that exist on GSP/DCE as well
    pGpu->physicalRmApi.Control                      = _gpuRmApiControl;
    pGpu->physicalRmApi.AllocWithHandle              = _gpuRmApiAllocWithHandle;
    pGpu->physicalRmApi.Free                         = _gpuRmApiFree;
}

static NV_STATUS
_gpuInitChipInfo
(
    OBJGPU *pGpu
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    const NvU32 paramSize = sizeof(NV2080_CTRL_INTERNAL_GPU_GET_CHIP_INFO_PARAMS);
    NV_STATUS status;

    pGpu->pChipInfo = portMemAllocNonPaged(paramSize);
    NV_ASSERT_OR_RETURN(pGpu->pChipInfo != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pGpu->pChipInfo, 0, paramSize);

    NV_ASSERT_OK_OR_GOTO(status, pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                                 NV2080_CTRL_CMD_INTERNAL_GPU_GET_CHIP_INFO,
                                                 pGpu->pChipInfo, paramSize), done);

    pGpu->chipInfo.subRevision  = pGpu->pChipInfo->chipSubRev;
    pGpu->idInfo.PCIDeviceID    = pGpu->pChipInfo->pciDeviceId;
    pGpu->idInfo.PCISubDeviceID = pGpu->pChipInfo->pciSubDeviceId;
    pGpu->idInfo.PCIRevisionID  = pGpu->pChipInfo->pciRevisionId;

done:
    if (status != NV_OK)
    {
        portMemFree(pGpu->pChipInfo);
        pGpu->pChipInfo = NULL;
    }

    return status;
}

static NV_STATUS
gpuInitVmmuInfo
(
    OBJGPU *pGpu
)
{
    NV_STATUS  status;
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS params;

    pGpu->vmmuSegmentSize = 0;

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_GPU_GET_VMMU_SEGMENT_SIZE,
                             &params, sizeof(params));

    if (status == NV_ERR_NOT_SUPPORTED)
    {
        // Leave segment size initialized to zero to signal no VMMU present on physical
        return NV_OK;
    }
    else if (status != NV_OK)
    {
        return status;
    }

    pGpu->vmmuSegmentSize = params.vmmuSegmentSize;

    return status;
}

static NV_STATUS _gpuAllocateInternalObjects
(
    OBJGPU *pGpu
)
{
    NV_STATUS status = NV_OK;

    if (IS_GSP_CLIENT(pGpu))
    {
        if (IsT234D(pGpu))
        {
            //
            // NOTE: We add +1 to the client base because DCE-RM will also
            // allocate internal objects, taking the !IS_GSP_CLIENT path below.
            //
            pGpu->hInternalClient = RS_CLIENT_INTERNAL_HANDLE_BASE + 1;
            pGpu->hInternalDevice = NV_GPU_INTERNAL_DEVICE_HANDLE;
            pGpu->hInternalSubdevice = NV_GPU_INTERNAL_SUBDEVICE_HANDLE;
        }
        else
        {
            GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
            NV_ASSERT_OR_RETURN(pGSCI != NULL, NV_ERR_INVALID_STATE);

            pGpu->hInternalClient = pGSCI->hInternalClient;
            pGpu->hInternalDevice = pGSCI->hInternalDevice;
            pGpu->hInternalSubdevice = pGSCI->hInternalSubdevice;
        }
    }
    else
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        NV_ASSERT_OK_OR_RETURN(rmapiutilAllocClientAndDeviceHandles(
            pRmApi, pGpu, &pGpu->hInternalClient, &pGpu->hInternalDevice, &pGpu->hInternalSubdevice));

        NV_ASSERT_OK_OR_GOTO(status, serverGetClientUnderLock(&g_resServ, pGpu->hInternalClient,
            &pGpu->pCachedRsClient), done);
        NV_ASSERT_OK_OR_GOTO(status, subdeviceGetByGpu(pGpu->pCachedRsClient, pGpu,
            &pGpu->pCachedSubdevice), done);
    }

done:
    if (status != NV_OK)
    {
        _gpuFreeInternalObjects(pGpu);
    }

    return status;
}

static void _gpuFreeInternalObjects
(
    OBJGPU *pGpu
)
{
    if (!IS_GSP_CLIENT(pGpu))
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

        rmapiutilFreeClientAndDeviceHandles(pRmApi,
            &pGpu->hInternalClient, &pGpu->hInternalDevice, &pGpu->hInternalSubdevice);
    }
}

static NV_STATUS
_gpuCreateEngineOrderList
(
     OBJGPU *pGpu
)
{
    NvU32                  i;
    NvU32                  numClassDesc;
    NvU32                  numLists;
    NV_STATUS              status = NV_OK;
    PGPU_ENGINE_ORDER      pEngineOrder = &pGpu->engineOrder;
    NvU32                  numEngineDesc, curEngineDesc;
    NvU32                  listTypes[] = {GCO_LIST_INIT, GCO_LIST_LOAD, GCO_LIST_UNLOAD, GCO_LIST_DESTROY};
    PENGDESCRIPTOR        *ppEngDescriptors[4];
    ENGLIST_ITER           it;
    ENGDESCRIPTOR          engDesc;

    ct_assert(NV_ARRAY_ELEMENTS32(ppEngDescriptors) == NV_ARRAY_ELEMENTS32(listTypes));

#define GPU_CHILD(a, b, numInstances, c, d, e) +numInstances

    struct ChildList {
        char children[ 0 +
            #include "gpu/gpu_child_list.h"
        ];
    };

    //
    // The maximum number of engines known to RM controls
    // must be at least the number of actual OBJGPU children.
    //
    ct_assert(NV2080_CTRL_GPU_MAX_ENGINE_OBJECTS >=
        sizeof(((struct ChildList*)(NULL))->children) /* sizeof(ChildList::children) */);

    numLists = NV_ARRAY_ELEMENTS32(listTypes);

    ppEngDescriptors[0] = &pEngineOrder->pEngineInitDescriptors;
    ppEngDescriptors[1] = &pEngineOrder->pEngineLoadDescriptors;
    ppEngDescriptors[2] = &pEngineOrder->pEngineUnloadDescriptors;
    ppEngDescriptors[3] = &pEngineOrder->pEngineDestroyDescriptors;

    //
    // Find the size of the engine descriptor list. The sizes of all lists
    // are checked for consistency to catch mistakes.
    //
    // The list is copied into OBJGPU storage as it's modified during
    // dynamic engine removal (e.g.: gpuMissingEngDescriptor).
    //
    numEngineDesc = 0;

    for (i = 0; i < numLists; i++)
    {
        curEngineDesc = 0;

        it = gpuGetEngineOrderListIter(pGpu, listTypes[i]);

        while (gpuGetNextInEngineOrderList(pGpu, &it, &engDesc))
        {
            curEngineDesc++;
        }

        if ((numEngineDesc != 0) && (numEngineDesc != curEngineDesc))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Sizes of all engine order lists do not match!\n");
            NV_ASSERT(0);
            status = NV_ERR_INVALID_STATE;
            goto done;
        }

        numEngineDesc = curEngineDesc;
    }

    pEngineOrder->numEngineDescriptors = numEngineDesc;


    for (i = 0; i < numLists; i++)
    {
        curEngineDesc = 0;

        *ppEngDescriptors[i] = portMemAllocNonPaged(sizeof(ENGDESCRIPTOR) * numEngineDesc);
        if ( NULL == *ppEngDescriptors[i])
        {
             NV_ASSERT(0);
             status = NV_ERR_NO_MEMORY;
             goto done;
        }

        it = gpuGetEngineOrderListIter(pGpu, listTypes[i]);

        while (gpuGetNextInEngineOrderList(pGpu, &it, &engDesc))
        {
            (*ppEngDescriptors[i])[curEngineDesc] = engDesc;
            curEngineDesc++;
        }
    }

    pEngineOrder->pClassDescriptors = gpuGetClassDescriptorList_HAL(pGpu, &numClassDesc);
    pEngineOrder->numClassDescriptors = numClassDesc;

    return NV_OK;

done:
    portMemFree(pEngineOrder->pEngineInitDescriptors);
    pEngineOrder->pEngineInitDescriptors = NULL;

    portMemFree(pEngineOrder->pEngineDestroyDescriptors);
    pEngineOrder->pEngineDestroyDescriptors = NULL;

    portMemFree(pEngineOrder->pEngineLoadDescriptors);
    pEngineOrder->pEngineLoadDescriptors = NULL;

    portMemFree(pEngineOrder->pEngineUnloadDescriptors);
    pEngineOrder->pEngineUnloadDescriptors = NULL;

    return status;
}

static void
_gpuFreeEngineOrderList
(
    OBJGPU *pGpu
)
{
    PGPU_ENGINE_ORDER pEngineOrder = &pGpu->engineOrder;

    if (!pEngineOrder->pEngineInitDescriptors)
        return;

    portMemFree(pEngineOrder->pEngineInitDescriptors);
    portMemFree(pEngineOrder->pEngineDestroyDescriptors);
    portMemFree(pEngineOrder->pEngineLoadDescriptors);
    portMemFree(pEngineOrder->pEngineUnloadDescriptors);

    pEngineOrder->pEngineInitDescriptors    = NULL;
    pEngineOrder->pEngineDestroyDescriptors = NULL;
    pEngineOrder->pEngineLoadDescriptors    = NULL;
    pEngineOrder->pEngineUnloadDescriptors  = NULL;
    pEngineOrder->pClassDescriptors         = NULL;
}

/*!
 * @brief Returns a pointer to the GPU's pointer to a child specified by its childInfo
 *
 * @param[in] pGpu            OBJPGU pointer
 * @param[in] pChildInfo      Pointer to table entry
 */
static Dynamic**
gpuGetChildPtr(OBJGPU *pGpu, NvU32 gpuChildPtrOffset)
{
     return (Dynamic**)((NvU8*)pGpu + gpuChildPtrOffset);
}

/*!
 * @brief Looks up for an instance of engine
 *
 * @param[in]  classId
 * @param[in]  instanceID
 * @param[out] pChildInfoOut
 */
static NV_STATUS
gpuGetChildInfo(NVOC_CLASS_ID classId, NvU32 instanceID, PGPUCHILDINFO pChildInfoOut)
{
    PGPUCHILDTYPE pChildType;

    NV_ASSERT_OR_RETURN(pChildInfoOut, NV_ERR_INVALID_STATE);

    pChildType = gpuGetChildType(classId);

    NV_ASSERT_OR_RETURN(pChildType && (instanceID < pChildType->instances), NV_ERR_INVALID_OBJECT);

    pChildInfoOut->engDesc = MKENGDESC(classId, instanceID);
    pChildInfoOut->bAlwaysCreate = pChildType->bAlwaysCreate;
    pChildInfoOut->bConstructEarly = pChildType->bConstructEarly;
    pChildInfoOut->pClassInfo = pChildType->pClassInfo;
    pChildInfoOut->pChildType = pChildType;

    pChildInfoOut->gpuChildPtrOffset = pChildType->gpuChildPtrOffset + sizeof(void *) * instanceID;

    return NV_OK;
}

/*!
 * @brief Looks up for a class in the table based on class id
 *
 * All classes are uniquely identified by their classId.
 *
 * @param[in] classId         NVOC_CLASS_ID
 */
static PGPUCHILDTYPE
gpuGetChildType(NVOC_CLASS_ID classId)
{
    NvU32 i;

    for (i = 0; i < GPU_NUM_CHILD_TYPES; i++)
    {
        if (gpuChildTypeList[i].pClassInfo && gpuChildTypeList[i].pClassInfo->classId == classId)
        {
            return &gpuChildTypeList[i];
        }
    }

    return NULL;
}

/**
 * @brief Initializes iterator for all possible ENGDESCRIPTORs that could be GPU
 *        children.
 *
 * @return        GPU_CHILD_ITER
 */
GPU_CHILD_ITER
gpuGetPossibleEngDescriptorIter(void)
{
    GPU_CHILD_ITER it = {0};
    return it;
}

/**
 * @brief Iterator over all possible ENGDESCRIPTORs that could be GPU children.
 *
 * @param[in,out] pIt          Iterator
 * @param[out]    pEngDesc     The next engine descriptor
 *
 * @return        NV_TRUE if *pEngDesc is valid, NV_FALSE if there are no more engines
 */
NvBool
gpuGetNextPossibleEngDescriptor(GPU_CHILD_ITER *pIt, ENGDESCRIPTOR *pEngDesc)
{
    PGPUCHILDTYPE pChildType;
    GPUCHILDINFO childInfo;

    if (pIt->childTypeIdx >= GPU_NUM_CHILD_TYPES)
        return NV_FALSE;

    pChildType = &gpuChildTypeList[pIt->childTypeIdx];

    // Advance instance #
    if (pIt->childInst < pChildType->instances && pChildType->pClassInfo)
    {
           NV_STATUS status = gpuGetChildInfo(pChildType->pClassInfo->classId, pIt->childInst, &childInfo);

           NV_ASSERT(status == NV_OK);

           pIt->gpuChildPtrOffset = childInfo.gpuChildPtrOffset;
           pIt->childInst++;

           *pEngDesc = childInfo.engDesc;
           return NV_TRUE;
    }

    pIt->childTypeIdx++;
    pIt->childInst = 0;

    // Recurse (max depth is 1)
    return gpuGetNextPossibleEngDescriptor(pIt, pEngDesc);
}

/*!
 * @brief Returns the unshared engstate for the child object with the given engine
 * descriptor (i.e.: the ENGSTATE without any of the SLI sharing hacks).
 *
 * All engines are uniquely identified by their engine descriptor.
 *
 * @param[in] pGpu     OBJGPU pointer
 * @param[in] engDesc  ENGDESCRIPTOR
 */
POBJENGSTATE
gpuGetEngstateNoShare_IMPL(OBJGPU *pGpu, ENGDESCRIPTOR engDesc)
{
    ENGSTATE_ITER it = gpuGetEngstateIter(pGpu);
    OBJENGSTATE *pEngstate;

    while (gpuGetNextEngstate(pGpu, &it, &pEngstate))
    {
        if (engstateGetDescriptor(pEngstate) == engDesc)
        {
            return pEngstate;
        }
    }

    return NULL;
}

/*!
 * @brief Returns the engstate for the child object with the given engine descriptor
 *
 * All engines are uniquely identified by their engine descriptor.
 *
 * @param[in] pGpu     OBJGPU pointer
 * @param[in] engDesc  ENGDESCRIPTOR
 */
POBJENGSTATE
gpuGetEngstate_IMPL(OBJGPU *pGpu, ENGDESCRIPTOR engDesc)
{
    if (ENGDESC_FIELD(engDesc, _CLASS) == classId(KernelFifo))
    {
        return staticCast(gpuGetKernelFifoShared(pGpu), OBJENGSTATE);
    }

    // Everything else is unshared
    return gpuGetEngstateNoShare(pGpu, engDesc);
}


/*!
 * @brief Gets the shared, aka the dominate KernelFifo, for a linked group of GPUs.
 *
 * Much of the SW state for KernelFifo is stored in the pKernelFifo hanging off
 * the parent GPU when in SLI.
 *
 * @param[in] pGpu     OBJGPU pointer
 */
KernelFifo*
gpuGetKernelFifoShared_IMPL(OBJGPU *pGpu)
{
    // If SLI is not active use parent GPU
    pGpu = (!gpumgrIsParentGPU(pGpu) &&
            !pGpu->getProperty(pGpu, PDB_PROP_GPU_SLI_LINK_ACTIVE)) ?
        gpumgrGetParentGPU(pGpu) : pGpu;

    return pGpu->pKernelFifo;
}

/*!
 * @brief Iterates over pGpu's child engstates. Returns NV_FALSE when there are
 *        no more.
 *
 * @param[in]      pGpu         OBJGPU pointer
 * @param[in,out]  pIt          Iterator
 * @param[out]     ppEngState   The next engstate
 *
 * @return         NV_TRUE if ppEngstate is valid, NV_FALSE if no more found
 */
NvBool
gpuGetNextEngstate_IMPL(OBJGPU *pGpu, ENGSTATE_ITER *pIt, OBJENGSTATE **ppEngstate)
{
    ENGDESCRIPTOR   engDesc;
    OBJENGSTATE    *pEngstate;
    Dynamic       **ppChild;

    while (gpuGetNextPossibleEngDescriptor(pIt, &engDesc))
    {
        ppChild = gpuGetChildPtr(pGpu, pIt->gpuChildPtrOffset);
        if (*ppChild != NULL)
        {
            pEngstate = dynamicCast(*ppChild, OBJENGSTATE);
            if (pEngstate != NULL)
            {
                *ppEngstate = pEngstate;
                return NV_TRUE;
            }
        }
    }

    return NV_FALSE;
}

/*!
 * @brief Iterates over pGpu's child engstates that implement INTRABLE.
 * Returns NV_FALSE when there are no more.
 *
 * @param[in]      pGpu         OBJGPU pointer
 * @param[in,out]  pIt          Iterator
 * @param[out]     ppPmuclient  The next PMU client
 *
 * @return         NV_TRUE if ppPmuclient is valid, NV_FALSE if no more found
 */
NvBool
gpuGetNextStaticIntrable_IMPL(OBJGPU *pGpu, GPU_CHILD_ITER *pIt, OBJINTRABLE **ppIntrable)
{
    ENGDESCRIPTOR engDesc;
    OBJHOSTENG   *pHostEng;
    OBJINTRABLE  *pIntrable;
    Dynamic     **ppChild;

    while (gpuGetNextPossibleEngDescriptor(pIt, &engDesc))
    {
        ppChild = gpuGetChildPtr(pGpu, pIt->gpuChildPtrOffset);
        if (*ppChild != NULL)
        {
            pHostEng = dynamicCast(*ppChild, OBJHOSTENG);

            // Exclude host engines for now, as we only want static units
            if (pHostEng == NULL)
            {
                pIntrable = dynamicCast(*ppChild, OBJINTRABLE);
                if (pIntrable != NULL)
                {
                    *ppIntrable = pIntrable;
                    return NV_TRUE;
                }
            }
        }
    }

    return NV_FALSE;
}

/*!
 * @brief Returns the hosteng for the child object with the given engine descriptor
 *
 * All engines are uniquely identified by their engine descriptor.
 *
 * @param[in] pGpu     OBJGPU pointer
 * @param[in] engDesc  ENGDESCRIPTOR
 */
POBJHOSTENG
gpuGetHosteng_IMPL(OBJGPU *pGpu, ENGDESCRIPTOR engDesc)
{
    OBJENGSTATE *pEngstate = gpuGetEngstate(pGpu, engDesc);
    OBJHOSTENG *pHosteng;

    if (pEngstate)
        pHosteng = dynamicCast(pEngstate, OBJHOSTENG);
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to get hosteng.\n");
        return NULL;
    }

    return pHosteng;
}

/*!
 * @brief The generic object constructor
 *
 * @param[in] pGpu         POBJGPU
 * @param[in] classId      NVOC_CLASS_ID
 * @param[in] instanceID   NvU32
 *
 */
NV_STATUS
gpuCreateObject_IMPL
(
    OBJGPU       *pGpu,
    NVOC_CLASS_ID classId,
    NvU32         instanceID
)
{
    NV_STATUS      status;
    OBJENGSTATE   *pEngstate;
    GPUCHILDINFO   childInfo;
    Dynamic      **ppChildPtr;
    ENGSTATE_TRANSITION_DATA engTransitionData;

    status       = gpuGetChildInfo(classId, instanceID, &childInfo);

    NV_ASSERT_OR_RETURN(status == NV_OK, status);

    ppChildPtr    = gpuGetChildPtr(pGpu, childInfo.gpuChildPtrOffset);

    // Ask the object database utility to create a child object.
    status = objCreateDynamic(ppChildPtr, pGpu, childInfo.pClassInfo);

    if (status != NV_OK)
    {
        return status;
    }
    NV_ASSERT_OR_RETURN(*ppChildPtr, NV_ERR_INVALID_STATE);

    pEngstate = dynamicCast(*ppChildPtr, OBJENGSTATE);

    if (pEngstate == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        goto gpuCreateObject_exit;
    }

    status = engstateConstructBase(pEngstate, pGpu, childInfo.engDesc);
    NV_ASSERT_OR_GOTO(status == NV_OK, gpuCreateObject_exit);

    engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_CONSTRUCT, &engTransitionData);
    status = engstateConstructEngine(pGpu, pEngstate, childInfo.engDesc);
    engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_CONSTRUCT, &engTransitionData);

    // If engine is missing, free it immediately
    if (pEngstate->getProperty(pEngstate, PDB_PROP_ENGSTATE_IS_MISSING))
    {
        status = NV_ERR_NOT_SUPPORTED;
    }

gpuCreateObject_exit:
    if (status != NV_OK)
    {
        objDelete(*ppChildPtr);
        *ppChildPtr    = NULL;
    }

    return status;
}


void
gpuDestruct_IMPL
(
    OBJGPU *pGpu
)
{
    HWBC_LIST           *pGpuHWBCList = NULL;
    int                  typeNum;
    int                  instNum;
    GPUCHILDTYPE        *pChildTypeCur;
    GPUCHILDINFO         childInfoCur;
    Dynamic            **pChildPtr;

    // Call gpuacctDisableAccounting if accounting is enabled since it does some memory deallocation
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON))
    {
        GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(SYS_GET_INSTANCE());
        NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS params;
        NV_STATUS status;

        /*
         * On VGX host, users are not allowed to disable accounting. But we still
         * need to do that while cleaning up (destroy timer part of this cleanup)
         * in gpuDestruct_IMPL() path. If PDB_PROP_GPU_ACCOUNTING_ON is NV_TRUE and
         * we call gpuacctDisableAccounting_IMPL() in gpuDestruct_IMPL() path,
         * it throws not supported error. To bypass the not supported case in
         * the gpuacctDisableAccounting_IMPL(), we are setting
         * PDB_PROP_GPU_ACCOUNTING_ON to NV_FALSE here in gpuDestruct_IMPL(), so
         * that execution goes forward in gpuacctDisableAccounting_IMPL() and
         * timer gets destroyed properly.
         */
        pGpu->setProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON, NV_FALSE);

        params.gpuId = pGpu->gpuId;
        params.pid = 0;
        params.newState = NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED;

        status = gpuacctDisableAccounting(pGpuAcct, pGpu->gpuInstance, &params);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "gpuacctDisableAccounting failed with error %d on GPU ID %d\n",
                      status, pGpu->gpuId);
        }
    }

    // Free children in reverse order from construction
    for (typeNum = GPU_NUM_CHILD_TYPES - 1; typeNum >= 0; typeNum--)
    {
        pChildTypeCur = &gpuChildTypeList[typeNum];

        if (!pChildTypeCur->pClassInfo)
        {
            continue;
        }

        for (instNum = pChildTypeCur->instances - 1; instNum >= 0; instNum--)
        {
            NV_STATUS status;

            status = gpuGetChildInfo(pChildTypeCur->pClassInfo->classId, instNum, &childInfoCur);

            NV_ASSERT(status == NV_OK);

            pChildPtr = gpuGetChildPtr(pGpu, childInfoCur.gpuChildPtrOffset);

            if (*pChildPtr)
            {
                objDelete(*pChildPtr);
                *pChildPtr = NULL;
            }
        }
    }

    //
    // If device instance is unassigned, we haven't initialized far enough to
    // do any accounting with it
    //
    if (gpuGetDeviceInstance(pGpu) != NV_MAX_DEVICES)
    {
        rmapiReportLeakedDevices(gpuGetGpuMask(pGpu));
    }

    _gpuFreeEngineOrderList(pGpu);

    portMemFree(pGpu->pUserRegisterAccessMap);
    pGpu->pUserRegisterAccessMap = NULL;

    portMemFree(pGpu->pUnrestrictedRegisterAccessMap);
    pGpu->pUnrestrictedRegisterAccessMap = NULL;

    portMemFree(pGpu->pDeviceInfoTable);
    pGpu->pDeviceInfoTable = NULL;
    pGpu->numDeviceInfoEntries = 0;

    pGpu->userRegisterAccessMapSize = 0;

    gpuDestroyEngineTable(pGpu);
    gpuDestroyClassDB(pGpu);
    osDestroyOSHwInfo(pGpu);

    while(pGpu->pHWBCList)
    {
        pGpuHWBCList = pGpu->pHWBCList;
        pGpu->pHWBCList = pGpuHWBCList->pNext;
        portMemFree(pGpuHWBCList);
    }

    //
    // Destroy and free the RegisterAccess object linked to this GPU
    // This should be moved out to gpu_mgr in the future to line up with
    // the construction, but currently depends on pGpu still existing
    //
    regAccessDestruct(&pGpu->registerAccess);

    NV_ASSERT(pGpu->numConstructedFalcons == 0);

    portMemFree(pGpu->pRegopOffsetScratchBuffer);
    pGpu->pRegopOffsetScratchBuffer = NULL;

    portMemFree(pGpu->pRegopOffsetAddrScratchBuffer);
    pGpu->pRegopOffsetAddrScratchBuffer = NULL;

    pGpu->regopScratchBufferMaxOffsets = 0;

    NV_ASSERT(pGpu->numSubdeviceBackReferences == 0);
    portMemFree(pGpu->pSubdeviceBackReferences);
    pGpu->pSubdeviceBackReferences = NULL;
    pGpu->numSubdeviceBackReferences = 0;
    pGpu->maxSubdeviceBackReferences = 0;

    gpuDestructPhysical(pGpu);
}

static NV_STATUS
gpuCreateChildObjects
(
    OBJGPU *pGpu,
    NvBool  bConstructEarly
)
{
    PENGDESCRIPTOR pEngDescriptors;
    NvU32          numEngDescriptors;
    PGPUCHILDTYPE  pChildTypeCur;
    GPUCHILDINFO   childInfoCur;
    NvU32          t, i;
    NV_STATUS      rmStatus = NV_OK;

    pEngDescriptors = gpuGetInitEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    for (t = 0; t < GPU_NUM_CHILD_TYPES; t++)
    {
        pChildTypeCur = &gpuChildTypeList[t];

        if (!pChildTypeCur->pClassInfo)
        {
            continue;
        }

        for (i = 0; i < pChildTypeCur->instances; i++)
        {
            NVOC_CLASS_ID classId = pChildTypeCur->pClassInfo->classId;

            rmStatus = gpuGetChildInfo(classId, i, &childInfoCur);

            NV_ASSERT(rmStatus == NV_OK);

            if ((bConstructEarly == childInfoCur.bConstructEarly) &&
                gpuShouldCreateObject(&childInfoCur,
                                      pEngDescriptors,
                                      numEngDescriptors))
            {
                rmStatus = gpuCreateObject(pGpu, classId, i);

                // RMCONFIG:  Bail on errors unless the feature/object/engine/class
                //            is simply unsupported
                if (rmStatus == NV_ERR_NOT_SUPPORTED)
                {
                    rmStatus = NV_OK;
                }
                else if (rmStatus != NV_OK)
                {
                    return rmStatus;
                }
            }
        }

        // Bail out of both loops.
        if (rmStatus != NV_OK)
        {
            break;
        }
    }

    return rmStatus;
}

static NvBool
gpuShouldCreateObject
(
    PGPUCHILDINFO pChildInfo,
    PENGDESCRIPTOR pEngDescriptors,
    NvU32 numEngDescriptors
)
{
    NvBool retVal = NV_FALSE;
    NvU32 curEngDescIdx;

    if (pChildInfo->bAlwaysCreate)
    {
        // For now all SW engines get created
        retVal = NV_TRUE;
    }
    else
    {
        // Let the HAL confirm that we should create an object for this engine.
        for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
        {
            if (pChildInfo->engDesc == pEngDescriptors[curEngDescIdx])
            {
                retVal = NV_TRUE;
                break;
            }
        }
    }

    return retVal;
}

NvU32
gpuGetGpuMask_IMPL
(
    OBJGPU *pGpu
)
{
    if (IsSLIEnabled(pGpu))
    {
        return 1 << (gpumgrGetSubDeviceInstanceFromGpu(pGpu));
    }
    else
    {
        return 1 << (pGpu->gpuInstance);
    }
}

static NV_STATUS gspSupportsEngine(OBJGPU *pGpu, ENGDESCRIPTOR engdesc, NvBool *supports)
{
    if (!IS_GSP_CLIENT(pGpu))
        return NV_WARN_NOTHING_TO_DO;

    NvU32 clientEngineId = 0;

    if (gpuXlateEngDescToClientEngineId(pGpu, engdesc, &clientEngineId) != NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "Failed to xlate engdesc 0x%x\n", engdesc);
        return NV_WARN_NOTHING_TO_DO;
    }

    if (pGpu->gspSupportedEngines == NULL)
    {
        pGpu->gspSupportedEngines = portMemAllocNonPaged(sizeof(*pGpu->gspSupportedEngines));
        NV_ASSERT_OR_RETURN(pGpu->gspSupportedEngines != NULL, NV_ERR_NO_MEMORY);

        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        NV_STATUS status = pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_GPU_GET_ENGINES_V2,
                        pGpu->gspSupportedEngines,
                        sizeof(*pGpu->gspSupportedEngines));

        if (status != NV_OK)
        {
            portMemFree(pGpu->gspSupportedEngines);
            return status;
        }
    }

    NvU32 i;
    for (i = 0; i < pGpu->gspSupportedEngines->engineCount; i++)
    {
        if (pGpu->gspSupportedEngines->engineList[i] == clientEngineId)
        {
            *supports = NV_TRUE;
            return NV_OK;
        }
    }

    *supports = NV_FALSE;
    return NV_OK;
}

/*
 *   The engine removal protocol is as follows:
 *   -   engines returning an error code from ConstructEngine will be immediately
 *       removed (this happens in gpuCreateObject)
 *   -   engines may set ENGSTATE_IS_MISSING at any time before gpuStatePreInit
 *   -   engines with ENGSTATE_IS_MISSING set at gpuStatePreInit will be removed
 *   -   engines that return NV_FALSE from engstateIsPresent at gpuStatePreInit
 *       will be removed
 *
 *   gpuRemoveMissingEngines takes place before the main loop in gpuStatePreInit
 *   and is responsible for removing engines satisfying the last two bullets
 *   above.
 */
static NV_STATUS
gpuRemoveMissingEngines
(
    OBJGPU        *pGpu
)
{
    NvU32          curEngDescIdx;
    PENGDESCRIPTOR engDescriptorList = gpuGetInitEngineDescriptors(pGpu);
    NvU32          numEngDescriptors = gpuGetNumEngDescriptors(pGpu);
    NV_STATUS      rmStatus = NV_OK;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        OBJENGSTATE  *pEngstate;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];
        NVOC_CLASS_ID curClassId = ENGDESC_FIELD(curEngDescriptor, _CLASS);

        if (curClassId == classId(OBJINVALID))
        {
            continue;
        }

        pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate != NULL)
        {
            if (!pEngstate->getProperty(pEngstate, PDB_PROP_ENGSTATE_IS_MISSING) &&
                engstateIsPresent(pGpu, pEngstate))
            {
                continue;
            }

            gpuDestroyMissingEngine(pGpu, pEngstate);
            pEngstate = NULL;
        }

        //
        // pEngstate is NULL or missing, so we must be sure to unregister
        // all associated API classes and remove the stale engine descriptors
        // from the GPU HAL engine lists.
        //
        NV_PRINTF(LEVEL_INFO, "engine %d:%d is missing, removing\n",
                  ENGDESC_FIELD(curEngDescriptor, _CLASS),
                  ENGDESC_FIELD(curEngDescriptor, _INST));

        rmStatus = gpuDeleteEngineOnPreInit(pGpu, curEngDescriptor);
        NV_ASSERT(rmStatus == NV_OK || !"Error while trying to remove missing engine");
    }

    return rmStatus;
}

/*
 * Removing classes from classDB of a missing engine
 */
static void
gpuRemoveMissingEngineClasses
(
    OBJGPU      *pGpu,
    NvU32       missingEngDescriptor
)
{
    NvU32   numClasses, i;
    NvU32  *pClassList = NULL;
    if (gpuGetClassList(pGpu, &numClasses, NULL, missingEngDescriptor) == NV_OK)
    {
        pClassList = portMemAllocNonPaged(sizeof(NvU32) * numClasses);
        if (NV_OK == gpuGetClassList(pGpu, &numClasses, pClassList, missingEngDescriptor))
        {
            for (i = 0; i < numClasses; i++)
            {
                gpuDeleteClassFromClassDBByClassId(pGpu, pClassList[i]);
            }
        }
        portMemFree(pClassList);
        pClassList = NULL;
    }
}

/*
 *  Destroy and unregister engine object of a missing engine
 */
static void
gpuDestroyMissingEngine
(
    OBJGPU      *pGpu,
    OBJENGSTATE *pEngstate
)
{
    GPUCHILDINFO  childInfo;
    Dynamic     **pChildPtr;
    NV_STATUS     status;
    ENGDESCRIPTOR engDesc;

    engstateInitMissing(pGpu, pEngstate);

    engDesc = engstateGetDescriptor(pEngstate);

    status = gpuGetChildInfo(ENGDESC_FIELD(engDesc, _CLASS), ENGDESC_FIELD(engDesc, _INST), &childInfo);

    NV_ASSERT_OR_RETURN_VOID(status == NV_OK);

    pChildPtr = gpuGetChildPtr(pGpu, childInfo.gpuChildPtrOffset);

    objDelete(*pChildPtr);
    *pChildPtr = NULL;
}

/*
 * @brief Find if given engine descriptor is supported by GPU
 *
 * @param[in] pGpu              OBJGPU pointer
 * @param[in] descriptor        engine descriptor to search for
 *
 * @returns NV_TRUE if given engine descriptor was found in a
 * given engine descriptor list, NV_FALSE otherwise.
 *
 */
NvBool
gpuIsEngDescSupported_IMPL
(
        OBJGPU *pGpu,
        NvU32 descriptor
)
{
    NvU32           numEngDescriptors   = gpuGetNumEngDescriptors(pGpu);
    PENGDESCRIPTOR  pEngDescriptor      = gpuGetInitEngineDescriptors(pGpu);
    NvU32           counter             = 0;
    NvBool          engDescriptorFound  = NV_FALSE;

    for (counter = 0; counter < numEngDescriptors; counter++)
    {
        if (pEngDescriptor[counter] == descriptor)
        {
            engDescriptorFound = NV_TRUE;
            break;
        }
    }

    return engDescriptorFound;
}
/*!
 * @brief Mark given Engine Descriptor with ENG_INVALID engine descriptor.
 *
 * Note: It is legal to have more than one entry with equal Descriptor
 * in the Engine Descriptor list.
  *
 * @param[in] pEngDescriptor    Pointer to array of engine descriptors
 * @param[in] maxDescriptors    Size of engine descriptor array
 * @param[in] descriptor        Engine descriptor to be changed to ENG_INVALID engine descriptor
 *
 * @returns void
 */
static void
gpuMissingEngDescriptor(PENGDESCRIPTOR pEngDescriptor, NvU32 maxDescriptors,
                        ENGDESCRIPTOR descriptor)
{
    NvU32 counter;

    for (counter = 0; counter < maxDescriptors; counter++)
    {
        if (pEngDescriptor[counter] == descriptor)
        {
            pEngDescriptor[counter] = ENG_INVALID;
        }
    }
}


/*!
 * @brief Delete an engine from class DB.
 *
 * WARNING! Function doesn't remove INIT/DESTROY engines from HAL lists.
 * gpuInitEng and gpuDestroyEng won't be no-ops for relevant engine.
 *
 * Use case:
 *  If an engine needs to be removed, but StateInit/Destroy are required.
 *  It's better to use gpuDeleteEngineOnPreInit instead.
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] engDesc       Engine ID to search and remove
 *
 * @returns NV_STATUS - NV_OK always.
 */
NV_STATUS
gpuDeleteEngineFromClassDB_IMPL(OBJGPU *pGpu, NvU32 engDesc)
{
    PENGDESCRIPTOR pEngDesc          = NULL;
    NvU32          numEngDescriptors = gpuGetNumEngDescriptors(pGpu);
    NvU32          engDescriptor     = engDesc;

    // remove Class tagged with engDesc from Class Database
    gpuDeleteClassFromClassDBByEngTag(pGpu, engDesc);

    //
    // Bug 370327
    // Q: Why remove load/unload?
    // A: Since this engine does not exist, we should prevent hw accesses to it
    //    which should ideally only take place in load/unload ( not init/destroy )
    //
    // Q: Why not remove init/destroy, the engines gone right?
    // A: If init does some alloc and loadhw does the probe then removing destroy
    //    will leak.
    //

    // Remove load
    pEngDesc = gpuGetLoadEngineDescriptors(pGpu);
    gpuMissingEngDescriptor(pEngDesc, numEngDescriptors,
                            engDescriptor);

    // Remove unload
    pEngDesc = gpuGetUnloadEngineDescriptors(pGpu);
    gpuMissingEngDescriptor(pEngDesc, numEngDescriptors,
                            engDescriptor);

    pGpu->engineDB.bValid = NV_FALSE;

    return NV_OK;
}

/*!
 * @brief Delete an engine from class DB only prior or on gpuPreInit stage.
 *
 * WARNING! Function must be used only before INIT stage, to avoid leaks.
 * See gpuDeleteEngineFromClassDB for more information.
 *
 * Function removes Classes with given Engine Tag from class DB
 * and removes Engines from HAL lists with equal Engine Tags.
 * Function doesn't remove Engines from HAL Sync list,
 * see gpuDeleteEngineFromClassDB for more information.
 *
 * Use case:
 * Any platform where an engine is absent and it is required to
 * prevent engine's load/unload and init/destroy calls from getting executed.
 * In other words, this function is used when it is OK to remove/STUB all
 * of the HALs of an engine without jeopardizing the initialization and
 * operation of other engines.
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] engDesc       Engine ID to search and remove
 *
 * @returns NV_STATUS - NV_OK on success, error otherwise.
 *
 */
NV_STATUS
gpuDeleteEngineOnPreInit_IMPL(OBJGPU *pGpu, NvU32 engDesc)
{
    PENGDESCRIPTOR pEngDesc          = NULL;
    NvU32          numEngDescriptors = gpuGetNumEngDescriptors(pGpu);
    ENGDESCRIPTOR  engDescriptor     = engDesc;
    NV_STATUS      rmStatus = NV_OK;
    NvBool bGspSupported = NV_FALSE;

    rmStatus = gspSupportsEngine(pGpu, engDesc, &bGspSupported);
    if (rmStatus == NV_WARN_NOTHING_TO_DO)
        rmStatus = NV_OK;

    NV_ASSERT_OK_OR_RETURN(rmStatus);

    // remove Class tagged with engDesc from Class Database.
    if (!bGspSupported)
        gpuDeleteClassFromClassDBByEngTag(pGpu, engDesc);

    // Remove Load Engine Descriptors
    pEngDesc = gpuGetLoadEngineDescriptors(pGpu);
    gpuMissingEngDescriptor(pEngDesc, numEngDescriptors,
                            engDescriptor);

    // Remove Unload Engine Descriptors
    pEngDesc = gpuGetUnloadEngineDescriptors(pGpu);
    gpuMissingEngDescriptor(pEngDesc, numEngDescriptors,
                            engDescriptor);

    // Remove Init Engine Descriptors
    pEngDesc = gpuGetInitEngineDescriptors(pGpu);
    gpuMissingEngDescriptor(pEngDesc, numEngDescriptors,
                            engDescriptor);

    // Remove Destroy Engine Descriptors
    pEngDesc = gpuGetDestroyEngineDescriptors(pGpu);
    gpuMissingEngDescriptor(pEngDesc, numEngDescriptors,
                            engDescriptor);

    if (!bGspSupported)
    {
        rmStatus = gpuUpdateEngineTable(pGpu);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Update engine table operation failed!\n");
            DBG_BREAKPOINT();
        }
    }

    return rmStatus;
}

/*!
 * @brief Perform GPU pre init tasks
 *
 * Function tries to pre-init all engines from HAL Init Engine Descriptor list.
 * If engine is not present, or its engine pre-init function reports it is unsupported
 * then engine will be deleted from Class DB and HAL lists.
 *
 * TODO: Merge structurally equivalent code with other gpuState* functions.
 * TODO: Fix "init missing" concept to not create unsupported objects at all.
 *
 * @param[in] pGpu  OBJGPU pointer
 *
 * @returns NV_OK upon successful pre-initialization
 */
NV_STATUS
gpuStatePreInit_IMPL
(
    OBJGPU *pGpu
)
{
    PENGDESCRIPTOR engDescriptorList;
    NvU32          numEngDescriptors;
    NvU32          curEngDescIdx;
    NV_STATUS      rmStatus = NV_OK;

    // Quadro, Geforce SMB, Tesla, VGX, Titan GPU detection
    gpuInitBranding(pGpu);

    LOCK_ASSERT_AND_RETURN(rmGpuLockIsOwner());

    NV_ASSERT_OK_OR_RETURN(_gpuAllocateInternalObjects(pGpu));
    NV_ASSERT_OK_OR_RETURN(_gpuInitChipInfo(pGpu));
    NV_ASSERT_OK_OR_RETURN(gpuConstructUserRegisterAccessMap(pGpu));
    NV_ASSERT_OK_OR_RETURN(gpuBuildGenericKernelFalconList(pGpu));

    rmStatus = gpuRemoveMissingEngines(pGpu);
    NV_ASSERT(rmStatus == NV_OK);

    pGpu->bFullyConstructed = NV_TRUE;

    engDescriptorList = gpuGetInitEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];
        OBJENGSTATE *pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_PRE_INIT, &engTransitionData);
        rmStatus = engstateStatePreInit(pGpu, pEngstate);
        engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_PRE_INIT, &engTransitionData);

        if (rmStatus == NV_ERR_NOT_SUPPORTED)
        {
            switch (curEngDescriptor)
            {
                //
                // Allow removing kernel engines in StatePreInit if their
                // physical counterpart is absent.
                //
                case ENG_KERNEL_DISPLAY:
                    // On Displayless GPU's, Display Engine is not present. So, RM should not keep the display
                    // classes in GET_CLASSLIST. Hence removing the Display classes from the ClassDB
                    gpuRemoveMissingEngineClasses(pGpu, ENG_KERNEL_DISPLAY);
                    break;
                //
                // Explicitly track engines that trigger this block
                // so that we can verify they function properly
                // after they are no longer removed here.
                //
                case ENG_INFOROM:
                    // TODO: try to remove this special case
                    NV_PRINTF(LEVEL_WARNING,
                        "engine removal in PreInit with NV_ERR_NOT_SUPPORTED is deprecated (%s)\n",
                        engstateGetName(pEngstate));
                    break;
                default:
                    NV_PRINTF(LEVEL_ERROR,
                        "disallowing NV_ERR_NOT_SUPPORTED PreInit removal of untracked engine (%s)\n",
                        engstateGetName(pEngstate));
                    DBG_BREAKPOINT();
                    NV_ASSERT(0);
                    break;
            }

            gpuDestroyMissingEngine(pGpu, pEngstate);
            pEngstate = NULL;

            rmStatus = gpuDeleteEngineOnPreInit(pGpu, curEngDescriptor);
            // TODO: destruct engine here after MISSING support is removed
            NV_ASSERT(rmStatus == NV_OK || !"Error while trying to remove missing engine");
        }
        else if (rmStatus != NV_OK)
        {
            break;
        }
    }

    return rmStatus;
}

// TODO: Merge structurally equivalent code with other gpuState* functions.
NV_STATUS
gpuStateInit_IMPL
(
    OBJGPU *pGpu
)
{
    PENGDESCRIPTOR engDescriptorList;
    NvU32          numEngDescriptors;
    NvU32          curEngDescIdx;
    NV_STATUS      rmStatus = NV_OK;

    // Initialize numaNodeId to invalid node ID as "0" can be considered valid node
    pGpu->numaNodeId = NV0000_CTRL_NO_NUMA_NODE;

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Move the below code into appropriate ENGSTATE objects.
    //       DO NOT ADD MORE SPECIAL CASES HERE!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    engDescriptorList = gpuGetInitEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    LOCK_ASSERT_AND_RETURN(rmGpuLockIsOwner());

    // Do this before calling stateInit() of child engines.
    objCreate(&pGpu->pPrereqTracker, pGpu, PrereqTracker, pGpu);

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Move the above code into appropriate ENGSTATE objects.
    //       DO NOT ADD MORE SPECIAL CASES HERE!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];
        OBJENGSTATE *pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_INIT, &engTransitionData);
        rmStatus = engstateStateInit(pGpu, pEngstate);
        engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_INIT, &engTransitionData);

        // RMCONFIG:  Bail on errors unless the feature/object/engine/class
        //            is simply unsupported

        if (rmStatus == NV_ERR_NOT_SUPPORTED)
            rmStatus = NV_OK;
        if (rmStatus != NV_OK)
            goto gpuStateInit_exit;
    }

    // Set a property indicating that the state initialization has been done
    pGpu->setProperty(pGpu, PDB_PROP_GPU_STATE_INITIALIZED, NV_TRUE);

gpuStateInit_exit:
    return rmStatus;
}

/*!
 * @brief Top level pre-load routine
 *
 * Provides a mechanism to resolve cyclic dependencies between engines.
 *
 * StatePreLoad() is called before StateLoad() likewise StatePostUnload() is
 * called after StateUnload().
 *
 * Dependencies which are DAGs should continue to be resolved by reordering the
 * engine descriptor lists. Reordering the descriptor lists won't solve cyclic
 * dependencies as at least one constraint would always be violated.
 *
 * TODO: Merge structurally equivalent code with other gpuState* functions.
 *
 * @param[in] pGpu       OBJPGU pointer
 * @param[in] flags      Type of transition
 */
static NV_STATUS
gpuStatePreLoad
(
    OBJGPU *pGpu,
    NvU32   flags
)
{
    PENGDESCRIPTOR engDescriptorList;
    NvU32          numEngDescriptors;
    NvU32          curEngDescIdx;
    NV_STATUS      rmStatus = NV_OK;

    engDescriptorList = gpuGetLoadEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];
        OBJENGSTATE *pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        RMTRACE_ENGINE_PROFILE_EVENT("gpuStatePreLoadEngStart", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

        engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_PRE_LOAD, &engTransitionData);
        rmStatus = engstateStatePreLoad(pGpu, pEngstate, flags);
        engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_PRE_LOAD, &engTransitionData);

        RMTRACE_ENGINE_PROFILE_EVENT("gpuStatePreLoadEngEnd", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

        //
        // An engine load leaving the broadcast status to NV_TRUE
        // will most likely mess up the pre-load of the next engines
        //
        NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

        // RMCONFIG:  Bail on errors unless the feature/object/engine/class
        //            is simply unsupported
        if (rmStatus == NV_ERR_NOT_SUPPORTED)
            rmStatus = NV_OK;
        if (rmStatus != NV_OK)
            break;

        //
        // Release and re-acquire the lock to allow interrupts
        //
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

        rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                     RM_LOCK_MODULES_STATE_LOAD);
        if (rmStatus != NV_OK)
            break;
    }

    return rmStatus;
}

// TODO: Merge structurally equivalent code with other gpuState* functions.
NV_STATUS
gpuStateLoad_IMPL
(
    OBJGPU *pGpu,
    NvU32   flags
)
{
    PENGDESCRIPTOR engDescriptorList;
    NvU32          numEngDescriptors;
    NvU32          curEngDescIdx;
    NV_STATUS      rmStatus = NV_OK;
    NvU32          status   = NV_OK;

    pGpu->registerAccess.regReadCount = pGpu->registerAccess.regWriteCount = 0;
    RMTRACE_ENGINE_PROFILE_EVENT("gpuStateLoadStart", pGpu->gpuId, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

    // Initialize SRIOV specific members of OBJGPU
    status = gpuInitSriov_HAL(pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error initializing SRIOV: 0x%0x\n", status);
        return status;
    }

    // It is a no-op on baremetal and inside non SRIOV guest.
    rmStatus = gpuCreateDefaultClientShare_HAL(pGpu);
    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    rmStatus = gpuStatePreLoad(pGpu, flags);
    if (rmStatus != NV_OK)
    {
        //
        // return early if we broke out of the preLoad sequence with
        // rmStatus != NV_OK
        //
        return rmStatus;
    }

    engDescriptorList = gpuGetLoadEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    // Set indicator that we are running state load
    pGpu->bStateLoading = NV_TRUE;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];
        OBJENGSTATE *pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        RMTRACE_ENGINE_PROFILE_EVENT("gpuStateLoadEngStart", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

        engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_LOAD, &engTransitionData);
        rmStatus = engstateStateLoad(pGpu, pEngstate, flags);
        engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_LOAD, &engTransitionData);


        // TODO: This is temporary and may be dead with TESLA
        if (rmStatus == NV_ERR_INVALID_ADDRESS)
        {
            NV_PRINTF(LEVEL_ERROR, "NV_ERR_INVALID_ADDRESS is no longer supported in StateLoad (%s)\n",
                engstateGetName(pEngstate));
            DBG_BREAKPOINT();
        }

        //
        // An engine load leaving the broadcast status to NV_TRUE
        // will most likely mess up the load of the next engines
        //
        NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

        // RMCONFIG:  Bail on errors unless the feature/object/engine/class
        //            is simply unsupported
        if (rmStatus == NV_ERR_NOT_SUPPORTED)
            rmStatus = NV_OK;
        if (rmStatus != NV_OK)
            goto gpuStateLoad_exit;

        //
        // Release and re-acquire the lock to allow interrupts
        //
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

        rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                     RM_LOCK_MODULES_STATE_LOAD);
        if (rmStatus != NV_OK)
            goto gpuStateLoad_exit;

        RMTRACE_ENGINE_PROFILE_EVENT("gpuStateLoadEngEnd", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);
    }

    rmStatus = gpuInitVmmuInfo(pGpu);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error initializing VMMU info: 0x%0x\n", status);
        goto gpuStateLoad_exit;
    }

    {
        // Perform post load operations
        rmStatus = gpuStatePostLoad(pGpu, flags);
        if (rmStatus != NV_OK)
            goto gpuStateLoad_exit;

    }

    // Clear indicator that we are running state load
    pGpu->bStateLoading = NV_FALSE;

    // Set a property indicating that the state load has been done
    pGpu->bStateLoaded = NV_TRUE;

    RMTRACE_ENGINE_PROFILE_EVENT("gpuStateLoadEnd", pGpu->gpuId, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

gpuStateLoad_exit:
    return rmStatus;
}

/*!
 * @brief Top level post-load routine
 *
 * Provides a mechanism to resolve cyclic dependencies between engines. For
 * example, OBJFB depends on OBJCE on Fermi (for memory scrubbing), likewise
 * OBJCE also depends on OBJFB (for instance memory).
 *
 * StatePostLoad() is called after StateLoad() likewise StatePreUnload() is
 * called prior to StateUnload().
 *
 * Dependencies which are DAGs should continue to be resolved by reordering the
 * engine descriptor lists. Reordering the descriptor lists won't solve cyclic
 * dependencies as at least one constraint would always be violated.
 *
 * TODO: Merge structurally equivalent code with other gpuState* functions.
 *
 * @param[in] pGpu       OBJPGU pointer
 * @param[in] flags      Type of transition
 */
static NV_STATUS
gpuStatePostLoad
(
    OBJGPU *pGpu,
    NvU32   flags
)
{
    PENGDESCRIPTOR engDescriptorList;
    NvU32          numEngDescriptors;
    NvU32          curEngDescIdx;
    NV_STATUS      rmStatus = NV_OK;

    engDescriptorList = gpuGetLoadEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];
        OBJENGSTATE *pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        RMTRACE_ENGINE_PROFILE_EVENT("gpuStatePostLoadEngStart", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);
        engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_POST_LOAD, &engTransitionData);
        rmStatus = engstateStatePostLoad(pGpu, pEngstate, flags);
        engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_POST_LOAD, &engTransitionData);
        RMTRACE_ENGINE_PROFILE_EVENT("gpuStatePostLoadEngEnd", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

        // RMCONFIG:  Bail on errors unless the feature/object/engine/class
        //            is simply unsupported
        if (rmStatus == NV_ERR_NOT_SUPPORTED)
            rmStatus = NV_OK;
        if (rmStatus != NV_OK)
            goto gpuStatePostLoad_exit;

        //
        // Release and re-acquire the lock to allow interrupts
        //
        rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

        rmStatus = rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                     RM_LOCK_MODULES_STATE_LOAD);

        if (rmStatus != NV_OK)
            goto gpuStatePostLoad_exit;
    }

    pGpu->boardInfo = portMemAllocNonPaged(sizeof(*pGpu->boardInfo));
    if (pGpu->boardInfo)
    {
        // To avoid potential race of xid reporting with the control, zero it out
        portMemSet(pGpu->boardInfo, '\0', sizeof(*pGpu->boardInfo));

        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        if(pRmApi->Control(pRmApi,
                           pGpu->hInternalClient,
                           pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_GPU_GET_OEM_BOARD_INFO,
                           pGpu->boardInfo,
                           sizeof(*pGpu->boardInfo)) != NV_OK)
        {
            portMemFree(pGpu->boardInfo);
            pGpu->boardInfo = NULL;
        }
    }

gpuStatePostLoad_exit:
    return rmStatus;
}

/*!
 * @brief Top level pre-unload routine
 *
 * Provides a mechanism to resolve cyclic dependencies between engines. For
 * example, OBJFB depends on OBJCE on Fermi (for memory scrubbing), likewise
 * OBJCE also depends on OBJFB (for instance memory).
 *
 * StatePostLoad() is called after StateLoad() likewise StatePreUnload() is
 * called prior to StateUnload().
 *
 * Dependencies which are DAGs should continue to be resolved by reordering the
 * engine descriptor lists. Reordering the descriptor lists won't solve cyclic
 * dependencies as at least one constraint would always be violated.
 *
 * TODO: Merge structurally equivalent code with other gpuState* functions.
 *
 * @param[in] pGpu       OBJPGU pointer
 * @param[in] flags      Type of transition
 */
static NV_STATUS
gpuStatePreUnload
(
    OBJGPU *pGpu,
    NvU32   flags
)
{
    PENGDESCRIPTOR      engDescriptorList;
    NvU32               numEngDescriptors;
    NvU32               curEngDescIdx;
    NV_STATUS           rmStatus = NV_OK;

    portMemFree(pGpu->boardInfo);
    pGpu->boardInfo = NULL;

    engDescriptorList = gpuGetUnloadEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];
        OBJENGSTATE *pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_PRE_UNLOAD, &engTransitionData);
        rmStatus = engstateStatePreUnload(pGpu, pEngstate, flags);
        engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_PRE_UNLOAD, &engTransitionData);

        //
        // During unload, failure of a single engine may not be fatal.
        // ASSERT if there is a failure, but ignore the status and continue
        // unloading other engines to prevent (worse) memory leaks.
        //
        if (rmStatus != NV_OK)
        {
            if (rmStatus != NV_ERR_NOT_SUPPORTED)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to pre unload engine with descriptor index: 0x%x and descriptor: 0x%x\n",
                          curEngDescIdx, curEngDescriptor);
                if (!IS_FMODEL(pGpu))
                {
                    NV_ASSERT(0);
                }
            }
            rmStatus = NV_OK;
        }

        // Ensure that intr on other GPUs are serviced
        gpuServiceInterruptsAllGpus(pGpu);
    }

    return rmStatus;
}

NV_STATUS
gpuEnterShutdown_IMPL
(
    OBJGPU *pGpu
)
{
    NV_STATUS rmStatus = gpuStateUnload(pGpu, GPU_STATE_DEFAULT);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to unload the device with error 0x%x\n", rmStatus);
    }

    return rmStatus;
}

// TODO: Merge structurally equivalent code with other gpuState* functions.
NV_STATUS
gpuStateUnload_IMPL
(
    OBJGPU *pGpu,
    NvU32   flags
)
{
    PENGDESCRIPTOR engDescriptorList;
    NvU32          numEngDescriptors;
    NvU32          curEngDescIdx;
    OBJENGSTATE   *pEngstate;
    NV_STATUS      rmStatus = NV_OK;
    NV_STATUS      fatalErrorStatus = NV_OK;

    NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

    // Set indicator that state is currently unloading.
    pGpu->bStateUnloading = NV_TRUE;

    {
        rmStatus = gpuStatePreUnload(pGpu, flags);
    }

    if (rmStatus != NV_OK)
        return rmStatus;

    engDescriptorList = gpuGetUnloadEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];

        pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_UNLOAD, &engTransitionData);
        rmStatus = engstateStateUnload(pGpu, pEngstate, flags);
        engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_UNLOAD, &engTransitionData);

        //
        // An engine unload leaving the broadcast status to NV_TRUE
        // will most likely mess up the unload of the next engines
        //
        NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

        //
        // During unload, failure of a single engine may not be fatal.
        // ASSERT if there is a failure, but ignore the status and continue
        // unloading other engines to prevent (worse) memory leaks.
        //
        if (rmStatus != NV_OK)
        {
            if (rmStatus != NV_ERR_NOT_SUPPORTED)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to unload engine with descriptor index: 0x%x and descriptor: 0x%x\n",
                          curEngDescIdx, curEngDescriptor);
                if (!IS_FMODEL(pGpu))
                {
                    NV_ASSERT(0);

                    if (flags & GPU_STATE_FLAGS_PRESERVING)
                    {
                        //
                        // FBSR can fail due to low sysmem.
                        // So return error.
                        // See bugs 2051056, 2049141
                        //
                        if (objDynamicCastById(pEngstate, classId(MemorySystem)))
                        {
                            fatalErrorStatus = rmStatus;
                        }
                    }
                }
            }
            rmStatus = NV_OK;
        }
        // Ensure that intr on other GPUs are serviced
        gpuServiceInterruptsAllGpus(pGpu);
    }

    // Call the gpuStatePostUnload routine
    rmStatus = gpuStatePostUnload(pGpu, flags);
    NV_ASSERT_OK(rmStatus);

    gpuDestroyDefaultClientShare_HAL(pGpu);

    // De-init SRIOV
    gpuDeinitSriov_HAL(pGpu);

    // Set indicator that state unload finished.
    pGpu->bStateUnloading = NV_FALSE;

    // Set a property indicating that the state unload has been done
    if (rmStatus == NV_OK)
    {
        pGpu->bStateLoaded = NV_FALSE;
    }

    if (fatalErrorStatus != NV_OK)
    {
        rmStatus = fatalErrorStatus;
    }

    return rmStatus;
}

/*!
 * @brief Top level post-unload routine
 *
 * Provides a mechanism to resolve cyclic dependencies between engines.
 *
 *
 * Dependencies which are DAGs should continue to be resolved by reordering the
 * engine descriptor lists. Reordering the descriptor lists won't solve cyclic
 * dependencies as at least one constraint would always be violated.
 *
 * TODO: Merge structurally equivalent code with other gpuState* functions.
 *
 * @param[in] pGpu       OBJPGU pointer
 * @param[in] flags      Type of transition
 */
static NV_STATUS
gpuStatePostUnload
(
    OBJGPU *pGpu,
    NvU32   flags
)
{
    PENGDESCRIPTOR      engDescriptorList;
    NvU32               numEngDescriptors;
    NvU32               curEngDescIdx;
    OBJENGSTATE        *pEngstate;
    NV_STATUS           rmStatus = NV_OK;

    engDescriptorList = gpuGetUnloadEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];

        pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_POST_UNLOAD, &engTransitionData);
        rmStatus = engstateStatePostUnload(pGpu, pEngstate, flags);
        engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_POST_UNLOAD, &engTransitionData);

        //
        // An engine post-unload leaving the broadcast status to NV_TRUE
        // will most likely mess up the post-unload of the next engines
        //
        NV_ASSERT(!gpumgrGetBcEnabledStatus(pGpu));

        //
        // During unload, failure of a single engine may not be fatal.
        // ASSERT if there is a failure, but ignore the status and continue
        // unloading other engines to prevent (worse) memory leaks.
        //
        if (rmStatus != NV_OK)
        {
            if (rmStatus != NV_ERR_NOT_SUPPORTED)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to post unload engine with descriptor index: 0x%x and descriptor: 0x%x\n",
                          curEngDescIdx, curEngDescriptor);
                if (!IS_FMODEL(pGpu))
                {
                    NV_ASSERT(0);
                }
            }
            rmStatus = NV_OK;
        }

        // Ensure that intr on other GPUs are serviced
        gpuServiceInterruptsAllGpus(pGpu);
    }

    return rmStatus;
}

NV_STATUS
gpuStateDestroy_IMPL
(
    OBJGPU *pGpu
)
{
    PENGDESCRIPTOR engDescriptorList;
    NvU32          numEngDescriptors;
    NvU32          curEngDescIdx;
    OBJENGSTATE   *pEngstate;
    NV_STATUS      rmStatus = NV_OK;

    engDescriptorList = gpuGetDestroyEngineDescriptors(pGpu);
    numEngDescriptors = gpuGetNumEngDescriptors(pGpu);

    NV_RM_RPC_SIM_FREE_INFRA(pGpu, rmStatus);

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = engDescriptorList[curEngDescIdx];

        pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_DESTROY, &engTransitionData);
        engstateStateDestroy(pGpu, pEngstate);
        engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_DESTROY, &engTransitionData);
    }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Move the below code into appropriate ENGSTATE objects.
    //       DO NOT ADD MORE SPECIAL CASES HERE!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // Do this after calling stateDestroy() of child engines.
    objDelete(pGpu->pPrereqTracker);
    pGpu->pPrereqTracker = NULL;

    // Clear the property indicating that the state initialization has been done
    if (rmStatus == NV_OK)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_STATE_INITIALIZED, NV_FALSE);
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
        rmStatus = kgspUnloadRm(pGpu, pKernelGsp);
    }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Move the above code into appropriate ENGSTATE objects.
    //       DO NOT ADD MORE SPECIAL CASES HERE!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    _gpuFreeInternalObjects(pGpu);
    gpuDestroyGenericKernelFalconList(pGpu);

    portMemFree(pGpu->gspSupportedEngines);
    pGpu->gspSupportedEngines = NULL;

    portMemFree(pGpu->pChipInfo);
    pGpu->pChipInfo = NULL;

    pGpu->bFullyConstructed = NV_FALSE;

    return rmStatus;
}

//
// Logic: If arch = requested AND impl = requested --> NV_TRUE
//        OR If arch = requested AND impl = requested AND maskRev = requested --> NV_TRUE
//        OR If arch = requested AND impl = requested AND rev = requested --> NV_TRUE
//
NvBool
gpuIsImplementation_IMPL
(
    OBJGPU *pGpu,
    HAL_IMPLEMENTATION halImpl,
    NvU32 maskRevision,
    NvU32 revision
)
{
    NvU32 gpuArch, gpuImpl;
    NvBool result = NV_FALSE;

    NV_ASSERT(revision == GPU_NO_REVISION);

    gpuXlateHalImplToArchImpl(pGpu, halImpl, &gpuArch, &gpuImpl);

    result = ((gpuGetChipArch(pGpu) == gpuArch) &&
              (gpuGetChipImpl(pGpu) == gpuImpl));

    if (maskRevision != GPU_NO_MASK_REVISION)
    {
        result = result && (GPU_GET_MASKREVISION(pGpu) == maskRevision);
    }

    return result;
}

// Check the software state to decide if we are in full power mode or not.
NvBool
gpuIsGpuFullPower_IMPL
(
    OBJGPU *pGpu
)
{
    NvBool retVal = NV_TRUE;

    //
    // SW may have indicated that the GPU ins in standby, hibernate, or powered off,
    // indicating a logical power state.
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_STANDBY) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_HIBERNATE))
    {
        retVal = NV_FALSE;
    }

    return retVal;
}

// Check the software state to decide if we are in full power mode or not.
NvBool
gpuIsGpuFullPowerForPmResume_IMPL
(
    OBJGPU *pGpu
)
{
    NvBool retVal = NV_TRUE;
    //
    // SW may have indicated that the GPU ins in standby, resume, hibernate, or powered off,
    // indicating a logical power state.
    //
    if ((!pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH)) &&
        (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_STANDBY) ||
         pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_HIBERNATE)))
    {
        retVal = NV_FALSE;
    }
    return retVal;
}

NvBool
gpuIsImplementationOrBetter_IMPL
(
    OBJGPU *pGpu,
    HAL_IMPLEMENTATION halImpl,
    NvU32 maskRevision,
    NvU32 revision
)
{
    NvU32 gpuArch, gpuImpl;
    NvU32 chipArch;
    NvBool result = NV_FALSE;

    NV_ASSERT(revision == GPU_NO_REVISION);

    gpuXlateHalImplToArchImpl(pGpu, halImpl, &gpuArch, &gpuImpl);

    // "is implementation or better" is only defined between 2 gpus within
    // the same "gpu series" as defined in config/Gpus.pm and gpuarch.h
    chipArch = gpuGetChipArch(pGpu);

    if (DRF_VAL(GPU, _ARCHITECTURE, _SERIES, chipArch) == DRF_VAL(GPU, _ARCHITECTURE, _SERIES, gpuArch))
    {
        if (maskRevision != GPU_NO_MASK_REVISION)
        {
            result = gpuSatisfiesTemporalOrderMaskRev(pGpu, halImpl, gpuArch,
                                                      gpuImpl, maskRevision);
        }
        else
        {
            // In case there is a temporal ordering we need to account for
            result = gpuSatisfiesTemporalOrder(pGpu, halImpl, gpuArch, gpuImpl);
        }
    }

    return result;
}

static void
gpuXlateHalImplToArchImpl
(
    OBJGPU *pGpu,
    HAL_IMPLEMENTATION halImpl,
    NvU32 *gpuArch,
    NvU32 *gpuImpl
)
{
    switch (halImpl)
    {
        case HAL_IMPL_GM107:
        {
            *gpuArch = GPU_ARCHITECTURE_MAXWELL;
            *gpuImpl = GPU_IMPLEMENTATION_GM107;
            break;
        }

        case HAL_IMPL_GM108:
        {
            *gpuArch = GPU_ARCHITECTURE_MAXWELL;
            *gpuImpl = GPU_IMPLEMENTATION_GM108;
            break;
        }

        case HAL_IMPL_GM200:
        {
            *gpuArch = GPU_ARCHITECTURE_MAXWELL2;
            *gpuImpl = GPU_IMPLEMENTATION_GM200;
            break;
        }

        case HAL_IMPL_GM204:
        {
            *gpuArch = GPU_ARCHITECTURE_MAXWELL2;
            *gpuImpl = GPU_IMPLEMENTATION_GM204;
            break;
        }

        case HAL_IMPL_GM206:
        {
            *gpuArch = GPU_ARCHITECTURE_MAXWELL2;
            *gpuImpl = GPU_IMPLEMENTATION_GM206;
            break;
        }

        case HAL_IMPL_GP100:
        {
            *gpuArch = GPU_ARCHITECTURE_PASCAL;
            *gpuImpl = GPU_IMPLEMENTATION_GP100;
            break;
        }

        case HAL_IMPL_GP102:
        {
            *gpuArch = GPU_ARCHITECTURE_PASCAL;
            *gpuImpl = GPU_IMPLEMENTATION_GP102;
            break;
        }

        case HAL_IMPL_GP104:
        {
            *gpuArch = GPU_ARCHITECTURE_PASCAL;
            *gpuImpl = GPU_IMPLEMENTATION_GP104;
            break;
        }

        case HAL_IMPL_GP106:
        {
            *gpuArch = GPU_ARCHITECTURE_PASCAL;
            *gpuImpl = GPU_IMPLEMENTATION_GP106;
            break;
        }

        case HAL_IMPL_GP107:
        {
            *gpuArch = GPU_ARCHITECTURE_PASCAL;
            *gpuImpl = GPU_IMPLEMENTATION_GP107;
            break;
        }

        case HAL_IMPL_GP108:
        {
            *gpuArch = GPU_ARCHITECTURE_PASCAL;
            *gpuImpl = GPU_IMPLEMENTATION_GP108;
            break;
        }

        case HAL_IMPL_GV100:
        {
            *gpuArch = GPU_ARCHITECTURE_VOLTA;
            *gpuImpl = GPU_IMPLEMENTATION_GV100;
            break;
        }

        case HAL_IMPL_GV11B:
        {
            *gpuArch = GPU_ARCHITECTURE_VOLTA2;
            *gpuImpl = GPU_IMPLEMENTATION_GV11B;
            break;
        }

        case HAL_IMPL_TU102:
        {
            *gpuArch = GPU_ARCHITECTURE_TURING;
            *gpuImpl = GPU_IMPLEMENTATION_TU102;
            break;
        }

        case HAL_IMPL_TU104:
        {
            *gpuArch = GPU_ARCHITECTURE_TURING;
            *gpuImpl = GPU_IMPLEMENTATION_TU104;
            break;
        }

        case HAL_IMPL_TU106:
        {
            *gpuArch = GPU_ARCHITECTURE_TURING;
            *gpuImpl = GPU_IMPLEMENTATION_TU106;
            break;
        }

        case HAL_IMPL_TU116:
        {
            *gpuArch = GPU_ARCHITECTURE_TURING;
            *gpuImpl = GPU_IMPLEMENTATION_TU116;
            break;
        }

        case HAL_IMPL_TU117:
        {
            *gpuArch = GPU_ARCHITECTURE_TURING;
            *gpuImpl = GPU_IMPLEMENTATION_TU117;
            break;
        }

        case HAL_IMPL_AMODEL:
        {
            *gpuArch = GPU_ARCHITECTURE_SIMS;
            *gpuImpl = GPU_IMPLEMENTATION_AMODEL;
            break;
        }

        case HAL_IMPL_T124:
        {
            *gpuArch = GPU_ARCHITECTURE_T12X;
            *gpuImpl = GPU_IMPLEMENTATION_T124;
            break;
        }

        case HAL_IMPL_T132:
        {
            *gpuArch = GPU_ARCHITECTURE_T13X;
            *gpuImpl = GPU_IMPLEMENTATION_T132;
            break;
        }

        case HAL_IMPL_T210:
        {
            *gpuArch = GPU_ARCHITECTURE_T21X;
            *gpuImpl = GPU_IMPLEMENTATION_T210;
            break;
        }

        case HAL_IMPL_T186:
        {
            *gpuArch = GPU_ARCHITECTURE_T18X;
            *gpuImpl = GPU_IMPLEMENTATION_T186;
            break;
        }

        case HAL_IMPL_T194:
        {
            *gpuArch = GPU_ARCHITECTURE_T19X;
            *gpuImpl = GPU_IMPLEMENTATION_T194;
            break;
        }

        case HAL_IMPL_GA100:
        {
            *gpuArch = GPU_ARCHITECTURE_AMPERE;
            *gpuImpl = GPU_IMPLEMENTATION_GA100;
            break;
        }

        case HAL_IMPL_GA102:
        {
            *gpuArch = GPU_ARCHITECTURE_AMPERE;
            *gpuImpl = GPU_IMPLEMENTATION_GA102;
            break;
        }

        case HAL_IMPL_GA102F:
        {
            *gpuArch = GPU_ARCHITECTURE_AMPERE;
            *gpuImpl = GPU_IMPLEMENTATION_GA102F;
            break;
        }

        case HAL_IMPL_GA103:
        {
            *gpuArch = GPU_ARCHITECTURE_AMPERE;
            *gpuImpl = GPU_IMPLEMENTATION_GA103;
            break;
        }

        case HAL_IMPL_GA104:
        {
            *gpuArch = GPU_ARCHITECTURE_AMPERE;
            *gpuImpl = GPU_IMPLEMENTATION_GA104;
            break;
        }

        case HAL_IMPL_GA106:
        {
            *gpuArch = GPU_ARCHITECTURE_AMPERE;
            *gpuImpl = GPU_IMPLEMENTATION_GA106;
            break;
        }

        case HAL_IMPL_GA107:
        {
            *gpuArch = GPU_ARCHITECTURE_AMPERE;
            *gpuImpl = GPU_IMPLEMENTATION_GA107;
            break;
        }


        default:
        {
            *gpuArch = 0;
            *gpuImpl = 0;
            NV_PRINTF(LEVEL_ERROR, "Invalid halimpl\n");
            DBG_BREAKPOINT();
            break;
        }
    }
}

//
// default Logic: If arch is greater than requested --> NV_TRUE
//                OR If arch is = requested AND impl is >= requested --> NV_TRUE
//
// NOTE: only defined for gpus within same gpu series
//
static NvBool
gpuSatisfiesTemporalOrder
(
    OBJGPU *pGpu,
    HAL_IMPLEMENTATION halImpl,
    NvU32 gpuArch,
    NvU32 gpuImpl
)
{
    NvBool result = NV_FALSE;

    switch (halImpl)
    {
        //
        // Comparison of Tegra series isn't straightforward with the chip ids
        // following different formats and so we can't use them
        // to figure out the relative ordering of chips.
        // T12X, T13X use 0x40, 0x13.
        //
        case HAL_IMPL_T124:
        {
            result = gpuIsT124ImplementationOrBetter(pGpu);
            break;
        }
        default:
        {
            NvU32 chipArch = gpuGetChipArch(pGpu);
            NvU32 chipImpl = gpuGetChipImpl(pGpu);

            result = ((chipArch > gpuArch) ||
                      ((chipArch == gpuArch) &&
                      (chipImpl >= gpuImpl)));
            break;
        }
    }

    return result;
}

/*!
 * @brief Checks if current GPU is T124OrBetter
 *
 * T124+ corresponds to BIG-GPU tegra chips that
 * are either T124 or beyond.
 * ChipArch which the generic implementation relies
 * on doesn't give the hierarchy of chips
 * accurately. Hence the explicit check for chips
 * below.
 *
 * @param[in]     pGpu          GPU object pointer
 *
 * @returns  NV_TRUE if T124 or any later big-gpu tegra chip,
 *           NV_FALSE otherwise
 */
static NvBool
gpuIsT124ImplementationOrBetter
(
    OBJGPU *pGpu
)
{
    NvU32 chipArch = gpuGetChipArch(pGpu);
    NvU32 chipImpl = gpuGetChipImpl(pGpu);

    //
    // All Big-gpu chips like T124, T132 or later satisy the condition.
    // This makes the assumption that starting from T186, there are no
    // AURORA chips.
    //
    return (((chipArch == GPU_ARCHITECTURE_T12X) && (chipImpl == GPU_IMPLEMENTATION_T124)) ||
            ((chipArch == GPU_ARCHITECTURE_T13X) && (chipImpl == GPU_IMPLEMENTATION_T132)) ||
            ((chipArch == GPU_ARCHITECTURE_T21X) && (chipImpl == GPU_IMPLEMENTATION_T210)) ||
            ((chipArch == GPU_ARCHITECTURE_T19X) && (chipImpl == GPU_IMPLEMENTATION_T194)) ||
            ((chipArch == GPU_ARCHITECTURE_T23X) && (chipImpl == GPU_IMPLEMENTATION_T234D)) ||
            ((chipArch == GPU_ARCHITECTURE_T23X) && (chipImpl == GPU_IMPLEMENTATION_T234)) ||
            ((chipArch >= GPU_ARCHITECTURE_T18X) && (chipImpl == GPU_IMPLEMENTATION_T186)));
}


//
// default Logic: If arch = requested AND impl = requested AND
//                 maskRev is >= requested --> NV_TRUE
//
static NvBool
gpuSatisfiesTemporalOrderMaskRev
(
    OBJGPU *pGpu,
    HAL_IMPLEMENTATION halImpl,
    NvU32 gpuArch,
    NvU32 gpuImpl,
    NvU32 maskRevision
)
{
    NvBool result = NV_FALSE;

    result = ((gpuGetChipArch(pGpu)== gpuArch) &&
              (gpuGetChipImpl(pGpu) == gpuImpl) &&
              (GPU_GET_MASKREVISION(pGpu) >= maskRevision));

    return result;
}

// =============== Engine Database ==============================

typedef struct {
    NvU32         clientEngineId;
    NVOC_CLASS_ID class;
    NvU32         instance;
    NvBool        bHostEngine;
} EXTERN_TO_INTERNAL_ENGINE_ID;

static const EXTERN_TO_INTERNAL_ENGINE_ID rmClientEngineTable[] =
{
    { NV2080_ENGINE_TYPE_GR0,        classId(Graphics)   , 0,  NV_TRUE },
    { NV2080_ENGINE_TYPE_GR1,        classId(Graphics)   , 1,  NV_TRUE },
    { NV2080_ENGINE_TYPE_GR2,        classId(Graphics)   , 2,  NV_TRUE },
    { NV2080_ENGINE_TYPE_GR3,        classId(Graphics)   , 3,  NV_TRUE },
    { NV2080_ENGINE_TYPE_GR4,        classId(Graphics)   , 4,  NV_TRUE },
    { NV2080_ENGINE_TYPE_GR5,        classId(Graphics)   , 5,  NV_TRUE },
    { NV2080_ENGINE_TYPE_GR6,        classId(Graphics)   , 6,  NV_TRUE },
    { NV2080_ENGINE_TYPE_GR7,        classId(Graphics)   , 7,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY0,      classId(OBJCE)      , 0,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY1,      classId(OBJCE)      , 1,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY2,      classId(OBJCE)      , 2,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY3,      classId(OBJCE)      , 3,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY4,      classId(OBJCE)      , 4,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY5,      classId(OBJCE)      , 5,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY6,      classId(OBJCE)      , 6,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY7,      classId(OBJCE)      , 7,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY8,      classId(OBJCE)      , 8,  NV_TRUE },
    { NV2080_ENGINE_TYPE_COPY9,      classId(OBJCE)      , 9,  NV_TRUE },
    { NV2080_ENGINE_TYPE_NVDEC0,     classId(OBJBSP)     , 0,  NV_TRUE },
    { NV2080_ENGINE_TYPE_NVDEC1,     classId(OBJBSP)     , 1,  NV_TRUE },
    { NV2080_ENGINE_TYPE_NVDEC2,     classId(OBJBSP)     , 2,  NV_TRUE },
    { NV2080_ENGINE_TYPE_NVDEC3,     classId(OBJBSP)     , 3,  NV_TRUE },
    { NV2080_ENGINE_TYPE_NVDEC4,     classId(OBJBSP)     , 4,  NV_TRUE },
    { NV2080_ENGINE_TYPE_CIPHER,     classId(OBJCIPHER)  , 0,  NV_TRUE },
    { NV2080_ENGINE_TYPE_NVENC0,     classId(OBJMSENC)   , 0,  NV_TRUE },
    { NV2080_ENGINE_TYPE_NVENC1,     classId(OBJMSENC)   , 1,  NV_TRUE },
    { NV2080_ENGINE_TYPE_NVENC2,     classId(OBJMSENC)   , 2,  NV_TRUE },
    { NV2080_ENGINE_TYPE_SW,         classId(OBJSWENG)   , 0,  NV_TRUE },
    { NV2080_ENGINE_TYPE_SEC2,       classId(OBJSEC2)    , 0,  NV_TRUE },
    { NV2080_ENGINE_TYPE_NVJPEG0,    classId(OBJNVJPG)   , 0,  NV_TRUE },
    { NV2080_ENGINE_TYPE_OFA,        classId(OBJOFA)     , 0,  NV_TRUE },
    { NV2080_ENGINE_TYPE_DPU,        classId(OBJDPU)     , 0,  NV_FALSE },
    { NV2080_ENGINE_TYPE_PMU,        classId(Pmu)        , 0,  NV_FALSE },
    { NV2080_ENGINE_TYPE_FBFLCN,     classId(OBJFBFLCN)  , 0,  NV_FALSE },
    { NV2080_ENGINE_TYPE_HOST,       classId(KernelFifo) , 0,  NV_FALSE },
};

NV_STATUS gpuConstructEngineTable_IMPL
(
    OBJGPU *pGpu
)
{
    NvU32        engineId    = 0;

    // Alloc engine DB
    pGpu->engineDB.bValid = NV_FALSE;
    pGpu->engineDB.pType = portMemAllocNonPaged(
                        NV_ARRAY_ELEMENTS(rmClientEngineTable) * sizeof(*pGpu->engineDB.pType));
    if (pGpu->engineDB.pType == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "gpuConstructEngineTable: Could not allocate engine DB\n");
        DBG_BREAKPOINT();
        return NV_ERR_NO_MEMORY;
    }
    pGpu->engineDB.size = 0; // That's right, its the size not the capacity
                             // of the engineDB

    // Initialize per-GPU per-engine list of non-stall interrupt event nodes.
    for (engineId = 0; engineId < NV2080_ENGINE_TYPE_LAST; engineId++)
    {
        pGpu->engineNonstallIntr[engineId].pEventNode = NULL;
        pGpu->engineNonstallIntr[engineId].pSpinlock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
        if (pGpu->engineNonstallIntr[engineId].pSpinlock == NULL)
            return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    return NV_OK;
}

NV_STATUS gpuUpdateEngineTable_IMPL
(
    OBJGPU *pGpu
)
{
    NV_STATUS status       = NV_OK;
    NvU32     counter      = 0;
    NvU32     numClasses   = 0;

    if (pGpu->engineDB.pType == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "gpuUpdateEngineTable: EngineDB has not been created yet\n");
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    if (pGpu->engineDB.bValid)
    {
        return NV_OK;
    }

    // Read through the classDB and populate engineDB
    pGpu->engineDB.size = 0;
    for (counter = 0; counter < NV_ARRAY_ELEMENTS(rmClientEngineTable); counter++)
    {
        // There are tests such as ClassA06fTest that attempt to bind all engines reported
        if (!rmClientEngineTable[counter].bHostEngine)
        {
            continue;
        }

        status = gpuGetClassList(pGpu, &numClasses, NULL,
                                       MKENGDESC(rmClientEngineTable[counter].class, rmClientEngineTable[counter].instance));
        if ((status != NV_OK) || ( numClasses == 0))
        {
            continue;
        }
        pGpu->engineDB.pType[pGpu->engineDB.size++] =
            rmClientEngineTable[counter].clientEngineId;
    }

    pGpu->engineDB.bValid = NV_TRUE;

    return NV_OK;
}
void gpuDestroyEngineTable_IMPL(OBJGPU *pGpu)
{
    NvU32     engineId      = 0;

    if (pGpu->engineDB.pType)
    {
        pGpu->engineDB.size  = 0;
        portMemFree(pGpu->engineDB.pType);
        pGpu->engineDB.pType = NULL;
        pGpu->engineDB.bValid = NV_FALSE;
    }

    for (engineId = 0; engineId < NV2080_ENGINE_TYPE_LAST; engineId++)
    {
        NV_ASSERT(pGpu->engineNonstallIntr[engineId].pEventNode == NULL);

        if (pGpu->engineNonstallIntr[engineId].pSpinlock != NULL)
        {
            portSyncSpinlockDestroy(pGpu->engineNonstallIntr[engineId].pSpinlock);
        }
    }
}

NvBool gpuCheckEngineTable_IMPL
(
    OBJGPU *pGpu,
    NvU32 engType
)
{
    NvU32 engineId;

    if (!IS_MODS_AMODEL(pGpu))
    {
        NV_ASSERT_OR_RETURN(pGpu->engineDB.bValid, NV_FALSE);
    }

    NV_ASSERT_OR_RETURN(engType < NV2080_ENGINE_TYPE_LAST, NV_FALSE);

    for (engineId = 0; engineId < pGpu->engineDB.size; engineId++)
    {
        if (engType ==  pGpu->engineDB.pType[engineId])
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

NV_STATUS
gpuXlateClientEngineIdToEngDesc_IMPL
(
    OBJGPU        *pGpu,
    NvU32          clientEngineID,
    ENGDESCRIPTOR *pEngDesc

)
{
    NvU32    counter;

    for (counter = 0; counter < NV_ARRAY_ELEMENTS(rmClientEngineTable); counter++)
    {
        if (rmClientEngineTable[counter].clientEngineId == clientEngineID)
        {
            *pEngDesc = MKENGDESC(rmClientEngineTable[counter].class, rmClientEngineTable[counter].instance);
            return NV_OK;
        }
    }

    return NV_ERR_INVALID_ARGUMENT;
}

NV_STATUS
gpuXlateEngDescToClientEngineId_IMPL
(
    OBJGPU       *pGpu,
    ENGDESCRIPTOR engDesc,
    NvU32        *pClientEngineID
)
{
    NvU32    counter;

    for (counter = 0; counter < NV_ARRAY_ELEMENTS(rmClientEngineTable); counter++)
    {
        if (MKENGDESC(rmClientEngineTable[counter].class, rmClientEngineTable[counter].instance) == engDesc)
        {
            *pClientEngineID = rmClientEngineTable[counter].clientEngineId;
            return NV_OK;
        }
    }

    return NV_ERR_INVALID_ARGUMENT;
}

NV_STATUS
gpuGetFlcnFromClientEngineId_IMPL
(
    OBJGPU       *pGpu,
    NvU32         clientEngineId,
    Falcon      **ppFlcn
)
{
    *ppFlcn = NULL;
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
gpuGetGidInfo_IMPL
(
    OBJGPU  *pGpu,
    NvU8   **ppGidString,
    NvU32   *pGidStrlen,
    NvU32    gidFlags
)
{
    NV_STATUS rmStatus = NV_OK;
    NvU8      gidData[RM_SHA1_GID_SIZE];
    NvU32     gidSize = RM_SHA1_GID_SIZE;

    if (!FLD_TEST_DRF(2080_GPU_CMD,_GPU_GET_GID_FLAGS,_TYPE,_SHA1,gidFlags))
    {
        return NV_ERR_INVALID_FLAGS;
    }

    if (pGpu->gpuUuid.isInitialized)
    {
        portMemCopy(gidData, gidSize, &pGpu->gpuUuid.uuid[0], gidSize);
        goto fillGidData;
    }

    rmStatus = gpuGenGidData_HAL(pGpu, gidData, gidSize, gidFlags);

    if (rmStatus != NV_OK)
    {
        return rmStatus;
    }

    // if not cached, cache it here
    portMemCopy(&pGpu->gpuUuid.uuid[0], gidSize, gidData, gidSize);
    pGpu->gpuUuid.isInitialized = NV_TRUE;

fillGidData:
    if (ppGidString != NULL)
    {
        if (FLD_TEST_DRF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY,
                         gidFlags))
        {
            //
            // Instead of transforming the Gid into a string, just use it in its
            // original binary form. The allocation rules are the same as those
            // followed by the transformGidToUserFriendlyString routine: we
            // allocate ppGidString here, and the caller frees ppGidString.
            //
            *ppGidString = portMemAllocNonPaged(gidSize);
            if (*ppGidString == NULL)
            {
                return NV_ERR_NO_MEMORY;
            }

            portMemCopy(*ppGidString, gidSize, gidData, gidSize);
            *pGidStrlen = gidSize;
        }
        else
        {
            NV_ASSERT_OR_RETURN(pGidStrlen != NULL, NV_ERR_INVALID_ARGUMENT);
            rmStatus = transformGidToUserFriendlyString(gidData, gidSize,
                ppGidString, pGidStrlen, gidFlags);
        }
    }

    return rmStatus;
}

void
gpuSetDisconnectedProperties_IMPL
(
    OBJGPU *pGpu
)
{
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_LOST, NV_TRUE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_PM_CODEPATH, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_STANDBY, NV_FALSE);
    pGpu->bInD3Cold = NV_FALSE;
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_HIBERNATE, NV_FALSE);

}

/*!
 * @brief: Get the GPU's sparse texture compute mode setting information.
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[out]  pDefault    The default sparse texture compute mode setting
 *                          for this GPU on this platform.
 * @param[out]  pCurrent    The current sparse texture compute mode setting
 *                          that was applied on the last driver load.
 * @param[out]  pPending    The sparse texture compute mode setting that will
 *                          be applied on the next driver load.
 *
 * @returns NV_OK if the setting information is available
 *          NV_ERR_INVALID_ARGUMENT if any of the pointers are invalid
 *          NV_ERR_NOT_SUPPORTED if the sparse texture RMCFG is not enabled,
 *              or the GPU cannot support optimizing sparse texture vs compute.
 */
NV_STATUS
gpuGetSparseTextureComputeMode_IMPL
(
    OBJGPU *pGpu,
    NvU32  *pDefault,
    NvU32  *pCurrent,
    NvU32  *pPending
)
{
    NV_STATUS status;
    NvU32 data;

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) &&
         !pGpu->getProperty(pGpu, PDB_PROP_GPU_VGPU_BIG_PAGE_SIZE_64K))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((pDefault == NULL) || (pCurrent == NULL) || (pPending == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    *pDefault = pGpu->getProperty(pGpu, PDB_PROP_GPU_OPTIMIZE_SPARSE_TEXTURE_BY_DEFAULT) ?
                    NV0080_CTRL_GPU_SPARSE_TEXTURE_COMPUTE_MODE_OPTIMIZE_SPARSE_TEXTURE :
                    NV0080_CTRL_GPU_SPARSE_TEXTURE_COMPUTE_MODE_OPTIMIZE_COMPUTE;

    *pCurrent = *pDefault;
    if (pGpu->optimizeUseCaseOverride != NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_DEFAULT)
    {
        switch (pGpu->optimizeUseCaseOverride)
        {
            case NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_COMPUTE:
                *pCurrent = NV0080_CTRL_GPU_SPARSE_TEXTURE_COMPUTE_MODE_OPTIMIZE_COMPUTE;
                break;
            case NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_SPARSE_TEX:
                *pCurrent = NV0080_CTRL_GPU_SPARSE_TEXTURE_COMPUTE_MODE_OPTIMIZE_SPARSE_TEXTURE;
                break;
            default:
                break;
        }
    }

    //
    // The pending starts out as the default value; we will only attempt an
    // override if the regkey is set and has a valid value.
    //
    *pPending = *pDefault;
    status = osReadRegistryDword(pGpu,
                                 NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX,
                                 &data);
    if (status == NV_OK)
    {
        switch (data)
        {
            case NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_COMPUTE:
                *pPending = NV0080_CTRL_GPU_SPARSE_TEXTURE_COMPUTE_MODE_OPTIMIZE_COMPUTE;
                break;
            case NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_SPARSE_TEX:
                *pPending = NV0080_CTRL_GPU_SPARSE_TEXTURE_COMPUTE_MODE_OPTIMIZE_SPARSE_TEXTURE;
                break;
            default:
                break;
        }
    }

    return NV_OK;
}

/*!
 * @brief: Set the GPU's sparse texture compute mode setting to apply on the
 *         next driver load.
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[out]  setting     The sparse texture compute mode setting that should
 *                          be applied on the next driver load.
 *
 * @returns NV_OK if the setting is saved in the registry
 *          NV_ERR_INVALID_ARGUMENT if pGpu or the setting is invalid
 *          NV_ERR_NOT_SUPPORTED if the sparse texture RMCFG is not enabled,
 *              or the GPU cannot support optimizing sparse texture vs compute.
 */
NV_STATUS
gpuSetSparseTextureComputeMode_IMPL
(
    OBJGPU *pGpu,
    NvU32 setting
)
{
    NV_STATUS status;
    NvU32 data = NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_DEFAULT;

    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)) &&
         !pGpu->getProperty(pGpu, PDB_PROP_GPU_VGPU_BIG_PAGE_SIZE_64K))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_CAN_OPTIMIZE_COMPUTE_USE_CASE))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (setting == NV0080_CTRL_GPU_SPARSE_TEXTURE_COMPUTE_MODE_OPTIMIZE_COMPUTE)
    {
        data = NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_COMPUTE;
    }
    else if (setting == NV0080_CTRL_GPU_SPARSE_TEXTURE_COMPUTE_MODE_OPTIMIZE_SPARSE_TEXTURE)
    {
        data = NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_SPARSE_TEX;
    }
    else if (setting != NV0080_CTRL_GPU_SPARSE_TEXTURE_COMPUTE_MODE_DEFAULT)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = osWriteRegistryDword(pGpu,
                                  NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX,
                                  data);
    return status;
}

NV_STATUS
gpuAddConstructedFalcon_IMPL
(
    OBJGPU  *pGpu,
    Falcon *pFlcn
)
{
    NV_ASSERT_OR_RETURN(pFlcn, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(
        pGpu->numConstructedFalcons < NV_ARRAY_ELEMENTS(pGpu->constructedFalcons),
        NV_ERR_BUFFER_TOO_SMALL);

    pGpu->constructedFalcons[pGpu->numConstructedFalcons++] = pFlcn;
    return NV_OK;
}

NV_STATUS
gpuRemoveConstructedFalcon_IMPL
(
    OBJGPU  *pGpu,
    Falcon *pFlcn
)
{
    NvU32 i, j;
    for (i = 0; i < pGpu->numConstructedFalcons; i++)
    {
        if (pGpu->constructedFalcons[i] == pFlcn)
        {
            for (j = i+1; j < pGpu->numConstructedFalcons; j++)
            {
                pGpu->constructedFalcons[j-1] = pGpu->constructedFalcons[j];
            }
            pGpu->numConstructedFalcons--;
            pGpu->constructedFalcons[pGpu->numConstructedFalcons] = NULL;
            return NV_OK;
        }
    }
    NV_ASSERT_FAILED("Attempted to remove a non-existent initialized Falcon!");
    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
gpuGetConstructedFalcon_IMPL
(
    OBJGPU  *pGpu,
    NvU32 index,
    Falcon  **ppFlcn
)
{
    if (index >= pGpu->numConstructedFalcons)
        return NV_ERR_OUT_OF_RANGE;

    *ppFlcn = pGpu->constructedFalcons[index];
    NV_ASSERT(*ppFlcn != NULL);
    return NV_OK;
}

NV_STATUS gpuBuildGenericKernelFalconList_IMPL(OBJGPU *pGpu)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status;
    NvU32 i;

    NV2080_CTRL_INTERNAL_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams;

    pParams = portMemAllocNonPaged(sizeof(*pParams));
    NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pParams, 0, sizeof(*pParams));

    NV_ASSERT_OK_OR_GOTO(status,
        pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_GET_CONSTRUCTED_FALCON_INFO,
                        pParams, sizeof(*pParams)),
        done);

    NV_ASSERT_TRUE_OR_GOTO(status,
        pParams->numConstructedFalcons <= NV_ARRAY_ELEMENTS(pGpu->genericKernelFalcons),
        NV_ERR_BUFFER_TOO_SMALL, done);

    for (i = 0; i < pParams->numConstructedFalcons; i++)
    {
        KernelFalconEngineConfig config = {0};

        config.physEngDesc   = pParams->constructedFalconsTable[i].engDesc;
        config.ctxAttr       = pParams->constructedFalconsTable[i].ctxAttr;
        config.ctxBufferSize = pParams->constructedFalconsTable[i].ctxBufferSize;
        config.addrSpaceList = pParams->constructedFalconsTable[i].addrSpaceList;
        config.registerBase  = pParams->constructedFalconsTable[i].registerBase;

        status = objCreate(&pGpu->genericKernelFalcons[i], pGpu, GenericKernelFalcon, pGpu, &config);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to create a GenericKernelFalcon object %d\n", i);
            goto done;
        }
    }

    pGpu->numGenericKernelFalcons = pParams->numConstructedFalcons;

done:
    portMemFree(pParams);
    if (status != NV_OK)
    {
        gpuDestroyGenericKernelFalconList(pGpu);
    }
    return status;
}

void gpuDestroyGenericKernelFalconList_IMPL(OBJGPU *pGpu)
{
    NvU32 i;
    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpu->genericKernelFalcons); i++)
    {
        if (pGpu->genericKernelFalcons[i] != NULL)
        {
            objDelete(pGpu->genericKernelFalcons[i]);
            pGpu->genericKernelFalcons[i] = NULL;
        }
    }
    pGpu->numGenericKernelFalcons = 0;
}


GenericKernelFalcon *
gpuGetGenericKernelFalconForEngine_IMPL
(
    OBJGPU  *pGpu,
    ENGDESCRIPTOR engDesc
)
{
    NvU32 i;
    for (i = 0; i < pGpu->numGenericKernelFalcons; i++)
    {
        KernelFalcon *pKernelFalcon = staticCast(pGpu->genericKernelFalcons[i], KernelFalcon);
        if (pKernelFalcon->physEngDesc == engDesc)
            return pGpu->genericKernelFalcons[i];
    }
    return NULL;
}

void gpuRegisterGenericKernelFalconIntrService_IMPL(OBJGPU *pGpu, void *pRecords)
{
    NvU32 i;
    for (i = 0; i < pGpu->numGenericKernelFalcons; i++)
    {
        IntrService *pIntrService = staticCast(pGpu->genericKernelFalcons[i], IntrService);
        if (pIntrService != NULL)
            intrservRegisterIntrService(pGpu, pIntrService, pRecords);
    }
}

/**
 * @brief Initializes iterator for ENGDESCRIPTOR load order
 *
 * @return        GPU_CHILD_ITER
 */
static ENGLIST_ITER
gpuGetEngineOrderListIter(OBJGPU *pGpu, NvU32 flags)
{
    ENGLIST_ITER it = { 0 };
    it.flags = flags;
    return it;
}


static const GPUCHILDPRESENT *
gpuFindChildPresent(const GPUCHILDPRESENT *pChildPresentList, NvU32 numChildPresent, NvU32 classId)
{
    NvU32 i;

    for (i = 0; i < numChildPresent; i++)
    {
        if (pChildPresentList[i].classId == classId)
            return &pChildPresentList[i];
    }

    return NULL;
}

/*!
 * @brief Sanity checks on given gfid
 *
 * @param[in] pGpu   OBJGPU pointer
 * @param[in] gfid   GFID to be validated
 * @param[in] bInUse NV_TRUE if GFID is being set for use
 */
NV_STATUS
gpuSanityCheckGfid_IMPL(OBJGPU *pGpu, NvU32 gfid, NvBool bInUse)
{
    // Error if pAllocatedGfids
    if (pGpu->sriovState.pAllocatedGfids == NULL)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    // Sanity check on GFID
    if (gfid > pGpu->sriovState.maxGfid)
    {
        return NV_ERR_OUT_OF_RANGE;
    }
    else if((bInUse == NV_TRUE) && (pGpu->sriovState.pAllocatedGfids[gfid] == GFID_ALLOCATED))
    {
        return NV_ERR_IN_USE;
    }
    else if((bInUse == NV_FALSE) && (pGpu->sriovState.pAllocatedGfids[gfid] == GFID_FREE))
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    return NV_OK;
}

/*!
 * @brief Set/Unset bit in pAllocatedGfids
 *
 * @param[in] pGpu   OBJGPU pointer
 * @param[in] gfid   GFID to be set/unset (Assumes GFID is sanity checked before calling this function)
 * @param[in] bInUse NV_TRUE if GFID in use
 */
void
gpuSetGfidUsage_IMPL(OBJGPU *pGpu, NvU32 gfid, NvBool bInUse)
{
    NV_ASSERT_OR_RETURN_VOID(pGpu->sriovState.pAllocatedGfids != NULL);

    if (bInUse == NV_TRUE)
        pGpu->sriovState.pAllocatedGfids[gfid] = GFID_ALLOCATED;
    else
        pGpu->sriovState.pAllocatedGfids[gfid] = GFID_FREE;
}

/**
 * @brief Iterates over the engine ordering list
 *
 * @param[in,out] pIt          Iterator
 * @param[out]    pEngDesc     The next engine descriptor
 *
 * @return        NV_TRUE if *pEngDesc is valid, NV_FALSE if there are no more engines
 */
NvBool
gpuGetNextInEngineOrderList(OBJGPU *pGpu, ENGLIST_ITER *pIt, PENGDESCRIPTOR pEngDesc)
{
    NvBool                 bReverse = !!(pIt->flags & (GCO_LIST_UNLOAD | GCO_LIST_DESTROY));
    const GPUCHILDORDER   *pChildOrderList;
    NvU32                  numChildOrder;
    const GPUCHILDPRESENT *pChildPresentList;
    NvU32                  numChildPresent;
    const GPUCHILDPRESENT *pCurChildPresent;
    const GPUCHILDORDER   *pCurChildOrder;
    NvBool                 bAdvance = NV_FALSE;

    pChildOrderList = gpuGetChildrenOrder_HAL(pGpu, &numChildOrder);
    pChildPresentList = gpuGetChildrenPresent_HAL(pGpu, &numChildPresent);

    if (!pIt->bStarted)
    {
        pIt->bStarted = NV_TRUE;
        pIt->childOrderIndex = bReverse ? (NvS32)numChildOrder - 1 : 0;
    }

    while (1)
    {
        if (bAdvance)
            pIt->childOrderIndex += bReverse ? -1 : 1;

        if ((pIt->childOrderIndex >= (NvS32)numChildOrder) || (pIt->childOrderIndex < 0))
            return NV_FALSE;

        pCurChildOrder = &pChildOrderList[pIt->childOrderIndex];

        if ((pCurChildOrder->flags & pIt->flags) != pIt->flags)
        {
            bAdvance = NV_TRUE;
            continue;
        }

        pCurChildPresent = gpuFindChildPresent(pChildPresentList, numChildPresent, pCurChildOrder->classId);

        if (!pCurChildPresent)
        {
            bAdvance = NV_TRUE;
            continue;
        }

        if (bAdvance)
        {
            pIt->instanceID = bReverse ? pCurChildPresent->instances - 1 : 0;
        }

        if ((pIt->instanceID < (NvS32)pCurChildPresent->instances) && (pIt->instanceID >= 0))
        {
            *pEngDesc = MKENGDESC(pCurChildOrder->classId, pIt->instanceID);

            pIt->instanceID += bReverse ? -1 : 1;

            return NV_TRUE;
        }

        bAdvance = NV_TRUE;
    }

    return NV_FALSE;
}

/**
 * Set SLI broadcast state in threadstate if SLI is enabled for the GPU
 */
void
gpuSetThreadBcState_IMPL(OBJGPU *pGpu, NvBool bcState)
{
    {
        gpumgrSetBcEnabledStatus(pGpu, bcState);
    }
}


NV_STATUS
gpuInitDispIpHal_IMPL
(
    OBJGPU *pGpu,
    NvU32   ipver
)
{
    RmHalspecOwner *pRmHalspecOwner = staticCast(pGpu, RmHalspecOwner);
    DispIpHal *pDispIpHal = &pRmHalspecOwner->dispIpHal;
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    //
    // 0xFFFFFFFF ipver value happens when Display engines is disabled.  NVOC Disp IP
    // halspec doesn't have a hal variant maps to this value.  Convert it to DISPv0000.
    //
    if (ipver == 0xFFFFFFFF)
    {
        ipver = 0;
    }
    else if (ipver == 0x03010000)
    {
        //
        // Display on GV100 has 0x0301 IP ver while it uses v0300 manuals.  It is listed
        // in disp.def IP_VERSIONS table as v03_00 since we added the chip.  This wasn't a
        // problem in chip-config as there it maps a range of IP ver to an implementation.
        // Versions in "v0300 <= ipver < 0400" map to _v03_00 or lower IP version function.
        // NVOC maps exact number but not range, thus we need to override the value when
        // initializing halspec.
        //
        ipver = 0x03000000;
    }

    __nvoc_init_halspec_DispIpHal(pDispIpHal, ipver & 0xFFFF0000);

    if ((ipver & 0xFFFF0000) != 0)
    {
        DispIpHal dispIpHalv00;
        __nvoc_init_halspec_DispIpHal(&dispIpHalv00, 0);

        //
        // At GPU creation time, dispIpHal.__nvoc_HalVarIdx is initialized with DISPv0000.
        // Any valid non-zero IP version listed in halspec DispIpHal assigns __nvoc_HalVarIdx
        // to different value.
        //
        // If dispIpHal.__nvoc_HalVarIdx keeps same idx of DISPv0000 for a non-zero ipver,
        // this means the IP ver is not listed in halspec DispIpHal and should be fixed.
        //
        // NVOC-TODO : make __nvoc_init_halspec_DispIpHal return error code and remove the check
        if (pDispIpHal->__nvoc_HalVarIdx == dispIpHalv00.__nvoc_HalVarIdx)
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid dispIpHal.__nvoc_HalVarIdx %d for Disp IP Vertion 0x%08x\n",
                pDispIpHal->__nvoc_HalVarIdx, ipver);

            NV_ASSERT(0);
            return NV_ERR_INVALID_STATE;
        }
    }

    void __nvoc_init_funcTable_KernelDisplay(KernelDisplay *, RmHalspecOwner *);
    __nvoc_init_funcTable_KernelDisplay(pKernelDisplay, pRmHalspecOwner);

    void __nvoc_init_funcTable_DisplayInstanceMemory(DisplayInstanceMemory *, RmHalspecOwner *);
    __nvoc_init_funcTable_DisplayInstanceMemory(KERNEL_DISPLAY_GET_INST_MEM(pKernelDisplay),
                                                pRmHalspecOwner);

    return NV_OK;
}

/*!
 * @brief: Initialize chip related info
 *  This function fills up the chip info structure of OBJGPU.
 *
 * @param[in]  pGpu          OBJGPU pointer
 *
 * @returns void
 */

void
gpuInitChipInfo_IMPL
(
    OBJGPU *pGpu
)
{
    //
    // NOTE: Register access and DRF field splitting should generally always
    // go in HAL functions, but PMC_BOOT_0 and PMC_BOOT_42 are an exception
    // as these are guaranteed to remain the same across chips, since we use
    // them to figure out which chip it is and how to wire up the HALs.
    //
    pGpu->chipInfo.pmcBoot0.impl          = DRF_VAL(_PMC, _BOOT_0, _IMPLEMENTATION, pGpu->chipId0);
    pGpu->chipInfo.pmcBoot0.arch          = DRF_VAL(_PMC, _BOOT_0, _ARCHITECTURE, pGpu->chipId0) << GPU_ARCH_SHIFT;
    pGpu->chipInfo.pmcBoot0.majorRev      = DRF_VAL(_PMC, _BOOT_0, _MAJOR_REVISION, pGpu->chipId0);
    pGpu->chipInfo.pmcBoot0.minorRev      = DRF_VAL(_PMC, _BOOT_0, _MINOR_REVISION, pGpu->chipId0);
    pGpu->chipInfo.pmcBoot0.minorExtRev   = NV2080_CTRL_GPU_INFO_MINOR_REVISION_EXT_NONE;
    pGpu->chipInfo.pmcBoot42.impl         = DRF_VAL(_PMC, _BOOT_42, _IMPLEMENTATION, pGpu->chipId1);
    pGpu->chipInfo.pmcBoot42.arch         = DRF_VAL(_PMC, _BOOT_42, _ARCHITECTURE, pGpu->chipId1) << GPU_ARCH_SHIFT;
    pGpu->chipInfo.pmcBoot42.majorRev     = DRF_VAL(_PMC, _BOOT_42, _MAJOR_REVISION, pGpu->chipId1);
    pGpu->chipInfo.pmcBoot42.minorRev     = DRF_VAL(_PMC, _BOOT_42, _MINOR_REVISION, pGpu->chipId1);
    pGpu->chipInfo.pmcBoot42.minorExtRev  = DRF_VAL(_PMC, _BOOT_42, _MINOR_EXTENDED_REVISION, pGpu->chipId1);

    //
    // SOC do not use pmcBoot0/pmcBoot42 and instead write the impl details to
    // these top level chipInfo fields, which is what the getters return.
    //
    pGpu->chipInfo.implementationId = pGpu->chipInfo.pmcBoot42.impl;
    pGpu->chipInfo.platformId       = pGpu->chipInfo.pmcBoot42.arch;
}

/*!
 * @brief: Returns physical address of end of DMA accessible range.
 *
 * @param[in]  pGpu  GPU object pointer
 *
 * @returns physical address of end of DMA accessible range
 */
RmPhysAddr
gpuGetDmaEndAddress_IMPL(OBJGPU *pGpu)
{
    NvU32 numPhysAddrBits = gpuGetPhysAddrWidth_HAL(pGpu, ADDR_SYSMEM);
    RmPhysAddr dmaWindowStartAddr = gpuGetDmaStartAddress(pGpu);

    return dmaWindowStartAddr + (1ULL << numPhysAddrBits) - 1;
}

void *gpuGetStaticInfo(OBJGPU *pGpu)
{

    return NULL;
}

void *gpuGetGspStaticInfo(OBJGPU *pGpu)
{
    return &(GPU_GET_KERNEL_GSP(pGpu)->gspStaticInfo);
}

OBJRPC *gpuGetGspClientRpc(OBJGPU *pGpu)
{
    if (IS_GSP_CLIENT(pGpu))
    {
        return GPU_GET_KERNEL_GSP(pGpu)->pRpc;
    }
    return NULL;
}

OBJRPC *gpuGetVgpuRpc(OBJGPU *pGpu)
{
    return NULL;
}

OBJRPC *gpuGetRpc(OBJGPU *pGpu)
{
    if (IS_VIRTUAL(pGpu))
        return gpuGetVgpuRpc(pGpu);

    if (IS_GSP_CLIENT(pGpu))
        return gpuGetGspClientRpc(pGpu);

    return NULL;
}

/*!
 * @brief: Check if system memory is accessible by GPU
 *         Dependent on NV2080_CTRL_CMD_BUS_SYSMEM_ACCESS only exercised on Windows.
 *
 * @param[in]  pGpu          OBJGPU pointer
 *
 * @returns NvBool  NV_TRUE is system memory is accessible,
 *                  NV_FALSE otherwise
 */
NvBool
gpuCheckSysmemAccess_IMPL(OBJGPU* pGpu)
{
    return NV_TRUE;
}

/*!
 * @brief Read the pcie spec registers using config cycles
 *
 * @param[in] pGpu     GPU object pointer
 * @param[in] index    Register offset in PCIe config space
 * @param[out] pData    Value of the register
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuReadBusConfigCycle_IMPL
(
    OBJGPU *pGpu,
    NvU32   index,
    NvU32   *pData
)
{
    NvU32 domain   = gpuGetDomain(pGpu);
    NvU8  bus      = gpuGetBus(pGpu);
    NvU8  device   = gpuGetDevice(pGpu);
    NvU8  function = 0;

    if (pGpu->hPci == NULL)
    {
        pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
    }

    *pData = osPciReadDword(pGpu->hPci, index);

    return NV_OK;
}

/*!
 * @brief Write to pcie spec registers using config cycles
 *
 * @param[in] pGpu     GPU object pointer
 * @param[in] index    Register offset in PCIe config space
 * @param[in] value    Write this value to the register
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuWriteBusConfigCycle_IMPL
(
    OBJGPU *pGpu,
    NvU32   index,
    NvU32   value
)
{
    NvU32 domain   = gpuGetDomain(pGpu);
    NvU8  bus      = gpuGetBus(pGpu);
    NvU8  device   = gpuGetDevice(pGpu);
    NvU8  function = 0;

    if (pGpu->hPci == NULL)
    {
        pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
    }

    osPciWriteDword(pGpu->hPci, index, value);

    return NV_OK;
}

NV_STATUS gpuGetCeFaultMethodBufferSize_KERNEL(OBJGPU *pGpu, NvU32 *size)
{
    NvU32 sz = pGpu->ceFaultMethodBufferSize;
    NV_STATUS status = NV_OK;

    if (sz == 0)
        status = kceGetFaultMethodBufferSize(pGpu, &sz);

    if (status == NV_OK)
        *size = sz;

    return NV_OK;
}

void gpuServiceInterruptsAllGpus_IMPL
(
    OBJGPU *pGpu
)
{
    Intr *pIntr  = GPU_GET_INTR(pGpu);
    MC_ENGINE_BITVECTOR engines;
    if (pIntr != NULL)
    {
        bitVectorSetAll(&engines);
        intrServiceStallListAllGpusCond(pGpu, pIntr, &engines, NV_TRUE);
    }
}
