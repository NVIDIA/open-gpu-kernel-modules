/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gsp/gsp_trace_rats_macro.h"
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
#include "platform/sli/sli.h"
#include "core/hal_mgr.h"
#include "vgpu/rpc.h"
#include "jt.h"
#include "kernel/gpu/nvbitmask.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"

#include "nvmisc.h"

#include "ctrl/ctrl402c.h" // NV402C_CTRL_NUM_I2C_PORTS
#include "ctrl/ctrl5070/ctrl5070chnc.h" // NV5070_CTRL_CMD_GET_PINSET_PEER_PEER_PINSET_NONE

#include "gpu/gpu_fabric_probe.h"

#include "vgpu/vgpu_events.h"
#include "vgpu/sdk-structures.h"

#include <nverror.h>

#include "gpu/sec2/kernel_sec2.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/video/kernel_video_engine.h"
#include "platform/platform.h"
#include "platform/chipset/chipset.h"
#include "kernel/gpu/host_eng/host_eng.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/conf_compute/conf_compute.h"
#include "gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/intr/intr.h"

#include "diagnostics/gpu_acct.h"

#include "nvop.h"

#include "nvdevid.h" // for NV_PCI_DEVID_DEVICE

#include "virtualization/hypervisor/hypervisor.h"
#include "kernel/virtualization/kernel_vgpu_mgr.h"

#include "gpu/bus/kern_bus.h"

static void      gpuSetupVirtualGuestOwnedHW(OBJHYPERVISOR *, OBJGPU *);

static NV_STATUS gpuDetermineVirtualMode(OBJGPU *);

#include "gpu/fsp/kern_fsp.h"
#include "fsp/fsp_clock_boost_rpc.h"

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
#include "nv_sriov_defines.h"
#include "diagnostics/code_coverage_mgr.h"
#endif

#include "g_odb.h"

#define  RMTRACE_ENGINE_PROFILE_EVENT(EventName, EngineId, ReadCount, WriteCount)           \
{                                                                                           \
        RMTRACE_PROBE4(generic, marker,                                                     \
                       NvU32, EngineId, sizeof(NvU32),                                      \
                       char*, EventName, sizeof(EventName),                                 \
                       NvU32, ReadCount, sizeof(NvU32),                                      \
                       NvU32, WriteCount, sizeof(NvU32));                                   \
    }

static NV_STATUS gpuRemoveMissingEngines(OBJGPU *);

// local static function
static NV_STATUS _gpuChildrenPresentInit(OBJGPU *pGpu);
static NV_STATUS gpuCreateChildObjects(OBJGPU *, NvBool);
static NV_STATUS gpuCreateObject(OBJGPU *pGpu, NVOC_CLASS_ID, NvU64, Dynamic **);
static NV_STATUS gpuStatePreLoad(OBJGPU *, NvU32);
static NV_STATUS gpuStatePostLoad(OBJGPU *, NvU32);
static NV_STATUS gpuStatePreUnload(OBJGPU *, NvU32);
static NV_STATUS gpuStatePostUnload(OBJGPU *, NvU32);
static void      gpuXlateHalImplToArchImpl(HAL_IMPLEMENTATION, NvU32 *, NvU32 *);
static NvBool    gpuSatisfiesTemporalOrder(OBJGPU *, HAL_IMPLEMENTATION);
static NvBool    gpuShouldCreateObject(OBJGPU *pGpu, NvU32 classId, NvU32 instance);

static void gpuDestroyMissingEngine(OBJGPU *, OBJENGSTATE *);
static void gpuRemoveMissingEngineClasses(OBJGPU *, NvU32);

static NV_STATUS _gpuCreateEngineOrderList(OBJGPU *pGpu);
static void _gpuFreeEngineOrderList(OBJGPU *pGpu);


static void _gpuInitPciHandle(OBJGPU *pGpu);
static void _gpuInitPhysicalRmApi(OBJGPU *pGpu);
static NV_STATUS _gpuAllocateInternalObjects(OBJGPU *pGpu);
static void _gpuFreeInternalObjects(OBJGPU *pGpu);
static NV_STATUS _gpuSetResetRequiredState(OBJGPU *pGpu, NvBool newState);

typedef struct
{
    NvS32   childOrderIndex;
    NvS32   instanceID;
    NvU32   flags;
    NvBool  bStarted;
} ENGLIST_ITER, *PENGLIST_ITER;

static ENGLIST_ITER gpuGetEngineOrderListIter(OBJGPU *pGpu, NvU32 flags);
static NvBool gpuGetNextInEngineOrderList(OBJGPU *pGpu, ENGLIST_ITER *pIt, ENGDESCRIPTOR *pEngDesc);

static inline void _setPlatformNoHostbridgeDetect(NvBool bValue)
{
    OBJPFM *pPfm = SYS_GET_PFM(SYS_GET_INSTANCE());
    pPfm->setProperty(pPfm, PDB_PROP_PFM_NO_HOSTBRIDGE_DETECT, bValue);
}

static NV_STATUS       _gpuChildNvocClassInfoGet(OBJGPU *pGpu, NVOC_CLASS_ID classId, const NVOC_CLASS_INFO **ppClassInfo);

void
_gpuDetectNvswitchSupport
(
    OBJGPU *pGpu
)
{
    NvU32 val = pGpu->fabricProbeRegKeyOverride;

    //
    // Slowdown Threshold 0 leads to driver crash with DIVIDE ERROR
    // Hence setting it to 1
    //
    pGpu->fabricProbeRetryDelay = 0;
    pGpu->fabricProbeSlowdownThreshold = 1;
    pGpu->nvswitchSupport = NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_NOT_SUPPORTED;

    if (IS_VIRTUAL(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = gpuGetStaticInfo(pGpu);

        pGpu->nvswitchSupport = pVSI->p2pCaps.bGpuSupportsFabricProbe ?
                                    NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_SUPPORTED :
                                    NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_NOT_SUPPORTED;
    }
    else
    {
        NvU32 status = NV_OK;
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_PMGR_MODULE_INFO_PARAMS moduleInfoParams = {0};
        NV_CHECK_OK(status, LEVEL_ERROR,
                    pRmApi->Control(pRmApi,
                                    pGpu->hInternalClient,
                                    pGpu->hInternalSubdevice,
                                    NV2080_CTRL_CMD_PMGR_GET_MODULE_INFO,
                                    &moduleInfoParams,
                                    sizeof(moduleInfoParams)));

        if (status == NV_OK)
        {
            pGpu->moduleId = moduleInfoParams.moduleId;
            pGpu->nvswitchSupport = moduleInfoParams.nvswitchSupport;
        }
    }

    if (GPU_IS_NVSWITCH_DETECTED(pGpu))
    {
        pGpu->fabricProbeRetryDelay = GPU_FABRIC_PROBE_DEFAULT_DELAY;
        pGpu->fabricProbeSlowdownThreshold =
                    GPU_FABRIC_PROBE_DEFAULT_PROBE_SLOWDOWN_THRESHOLD;
    }

    if (val != 0)
    {
        pGpu->fabricProbeSlowdownThreshold = \
            DRF_VAL(_REG_STR, _RM_GPU_FABRIC_PROBE, _SLOWDOWN_THRESHOLD, val);

        pGpu->fabricProbeSlowdownThreshold = \
            NV_MAX(pGpu->fabricProbeSlowdownThreshold , 1);

        pGpu->fabricProbeRetryDelay = \
            DRF_VAL(_REG_STR, _RM_GPU_FABRIC_PROBE, _DELAY, val);

        if (pGpu->fabricProbeRetryDelay != 0)
        {
            pGpu->nvswitchSupport = \
                NV2080_CTRL_PMGR_MODULE_INFO_NVSWITCH_SUPPORTED;
        }
    }
}

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

    if ((domain >> 16) != 0) {
        NV_ASSERT(hypervisorIsType(OS_HYPERVISOR_HYPERV));
    }

    return id;
}

NvU32 gpuGenerate32BitIdFromPhysAddr(RmPhysAddr addr)
{
    NvU32 id = NvU64_LO32(addr>>RM_PAGE_SHIFT);
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
    NV_STATUS  rmStatus;

    gpumgrAddDeviceInstanceToGpus(NVBIT(pGpu->gpuInstance));

    rmStatus = regAccessConstruct(&pGpu->registerAccess, pGpu);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to construct IO Apertures for attached devices \n");
        return rmStatus;
    }

    if (IS_VIRTUAL(pGpu))
    {
        //
        // FIXME: gpumgrGetGpuHalFactorOfVirtual has already checked this to initialize halspec,
        // but the VF HAL matches both legacy and SRIOV.
        //
        NvU32 config = GPU_REG_RD32(pGpu, NV_PMC_BOOT_1);
        if (FLD_TEST_DRF(_PMC, _BOOT_1, _VGPU, _VF, config))
        {
            pGpu->bIsVirtualWithSriov = NV_TRUE;
        }
        else
        {
            pGpu->bIsVirtualWithSriov = NV_FALSE;
        }
    }

    pGpu->sriovState.virtualRegPhysOffset = gpuGetVirtRegPhysOffset_HAL(pGpu);
    pGpu->simMode = NV_SIM_MODE_INVALID;

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

    // check if RM is running in a virtualization mode
    // This function needs to be called before we use IS_VIRTUAL macro.
    // Because IS_VIRTUAL macro relies on this function to determine Virtual mode.
    // eg. gpuCreateChildObjects->...->xxxHalIfacesSetup_xxx relies on IS_VIRTUAL macro.
    rmStatus = gpuDetermineVirtualMode(pGpu);
    if (rmStatus != NV_OK)
        return rmStatus;

    pGpu->setProperty(pGpu, PDB_PROP_GPU_MOVE_CTX_BUFFERS_TO_PMA,
        gpuIsCtxBufAllocInPmaSupported_HAL(pGpu));
    //
    // gpuDetermineVirtualMode inits hPci but only for virtualization case. So if
    // it does not init it, do here for using it for non-virtualization as well
    // Don't bother initing SOC Pci handle since it's not on PCIE.
    //
    if (pGpu->hPci == NULL && !pGpu->bIsSOC)
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

    NV_ASSERT_OK_OR_RETURN(
        _gpuChildrenPresentInit(pGpu));

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

    if (hypervisorIsVgxHyper() || IS_VIRTUAL(pGpu))
    {
        vgpuInitRegistryOverWrite(pGpu);
    }

    if (hypervisorIsVgxHyper())
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_VIRTUALIZATION_MODE_HOST_VGPU, NV_TRUE);
    }

    gpuApplySchedulerPolicy(pGpu);

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

#if NVCPU_IS_PPC64LE
    // Skip PCI Express Host Bridge initialization on PPC64 platforms
    _setPlatformNoHostbridgeDetect(NV_TRUE);
#endif

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

    if (IS_SIMULATION(pGpu) && !IS_VIRTUAL(pGpu))
    {
        //
        // gpuDetermineSelfHostedMode must be called after gpuDetermineVirtualMode
        // and vgpuCreateObject(for VGPU static info) as the self hosted detection mechanism
        // in VF depends on them. But in SCSIM(SIM_BUILD) it is required to be called
        // before vgpuCreateObject(from where initRpcInfrastructure_VGPU is called) in PF
        // so that the correct setting NV_VGPU_SEND_RING_GP_IN_RING_NO is done
        // for the SIM escapes.
        //
        gpuDetermineSelfHostedMode_HAL(pGpu);
    }

    if (
        IS_VIRTUAL(pGpu))
    {
        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && IS_VIRTUAL_WITH_SRIOV(pGpu))
        {
            KernelBus *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);

#if defined(NV_UNIX)
            // For self hosted hopper, VF BAR2 is absent. So disable bUsePhysicalBar2InitPagetable
            // in case of self hosted SRIOV guest
            if (pAttachArg->instLength == 0)
                pKernelBus->bUsePhysicalBar2InitPagetable = NV_FALSE;
#endif

            if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
            {
                // setup BAR2 in physical mode
                NV_ASSERT_OK_OR_RETURN(kbusBar2BootStrapInPhysicalMode_HAL(pGpu, pKernelBus));
            }
        }

        rmStatus = vgpuCreateObject(pGpu);
        if (rmStatus != NV_OK)
        {
            if (NV_ERR_LIB_RM_VERSION_MISMATCH == rmStatus)
            {
                nvErrorLog_va(pGpu, VGPU_START_ERROR,
                              "Guest driver is incompatible with host driver");
            }
            return rmStatus;
        }
    }

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

#if RMCFG_FEATURE_GSPRM_BULLSEYE || defined(GSPRM_BULLSEYE_ENABLE)
    if (IS_GSP_CLIENT(pGpu))
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        codecovmgrRegisterCoverageBuffer(pSys->pCodeCovMgr, GFID_TASK_RM,
                                         pGpu->gpuInstance, BULLSEYE_TASK_RM_COVERAGE_SIZE);
        for (NvU32 gfid = 1; gfid <= MAX_PARTITIONS_WITH_GFID; gfid++)
        {
            codecovmgrRegisterCoverageBuffer(pSys->pCodeCovMgr, gfid,
                                             pGpu->gpuInstance, BULLSEYE_TASK_VGPU_COVERAGE_SIZE);
        }
    }
#endif

    // Initialize the GPU recovery action, if the OS is already in a bad state.
    pGpu->currentRecoveryAction = GPU_RECOVERY_ACTION_UNKNOWN;
    gpuRefreshRecoveryAction_KERNEL(pGpu, NV_TRUE);

    return NV_OK;
}

NV_STATUS gpuConstruct_IMPL
(
    OBJGPU *pGpu,
    NvU32   gpuInstance,
    NvU32   gpuId,
    NvUuid *pGpuUuid
)
{

    pGpu->gpuInstance = gpuInstance;
    pGpu->gpuId = pGpu->boardId = gpuId; // boardId may be updated later
    pGpu->gspRmInitialized = NV_FALSE;

    if (pGpuUuid != NULL)
    {
        portMemCopy(&pGpu->gpuUuid.uuid[0], sizeof(pGpu->gpuUuid.uuid),
                    &pGpuUuid->uuid[0], sizeof(pGpuUuid->uuid));
        pGpu->gpuUuid.isInitialized = NV_TRUE;
    }

    // allocate OS-specific GPU extension area
    osInitOSHwInfo(pGpu);

    // Initialize NvFBC session count and list.
    listInit(&(pGpu->nvfbcSessionList), portMemAllocatorGetGlobalNonPaged());

    // Initialize NvENC session count and list.
    listInit(&(pGpu->nvencSessionList), portMemAllocatorGetGlobalNonPaged());

    multimapInit(&pGpu->videoEventBufferBindingsUid, portMemAllocatorGetGlobalNonPaged());

#if KERNEL_GSP_TRACING_RATS_ENABLED
    multimapInit(&pGpu->gspTraceEventBufferBindingsUid, portMemAllocatorGetGlobalNonPaged());
#endif

    // Initialize the i2c port via which external devices will be connected.
    pGpu->i2cPortForExtdev = NV402C_CTRL_NUM_I2C_PORTS;

    // Assume no SLI peer connection until later
    for (NvU32 i = 0; i < DR_PINSET_COUNT; ++i)
    {
        pGpu->peer[i].pGpu        = NULL;
        pGpu->peer[i].pinset      = NV5070_CTRL_CMD_GET_PINSET_PEER_PEER_PINSET_NONE;
    }

    pGpu->pDpcThreadState = portMemAllocNonPaged(sizeof(THREAD_STATE_NODE));
    NV_ASSERT_OR_RETURN(pGpu->pDpcThreadState != NULL, NV_ERR_NO_MEMORY);

    return gpuConstructPhysical(pGpu);
}

// NVOC-TODO : delete this after all Rmconfig modules migrated to NVOC
NV_STATUS
gpuBindHalLegacy_IMPL
(
    OBJGPU *pGpu,
    NvU32   chipId0,
    NvU32   chipId1,
    NvU32   socChipId0
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
    status = halmgrGetHalForGpu(pHalMgr, socChipId0 ? socChipId0 : pGpu->chipId0, pGpu->chipId1, &pGpu->halImpl);
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
    // This optimization should be skipped on vGPU Guests as they should always
    // go through the resource server. The resource server is responsible for
    // deciding whether to RPC to the HOST (ROUTE_TO_VGPU_HOST) or call the
    // implementation on the guest.
    //
    if (!IS_VIRTUAL(pGpu) && hClient == pGpu->hInternalClient && hObject == pGpu->hInternalSubdevice)
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

        NV_ASSERT_OK_OR_RETURN(resservSwapTlsCallContext(&oldCtx, &callCtx));

        if (pEntry->paramSize == 0)
        {
            status = ((NV_STATUS(*)(void*))pEntry->pFunc)(pGpu->pCachedSubdevice);
        }
        else
        {
            status = ((NV_STATUS(*)(void*,void*))pEntry->pFunc)(pGpu->pCachedSubdevice, pParams);
        }

        NV_ASSERT_OK(resservRestoreTlsCallContext(oldCtx));
    }
    else
    {
        RM_API *pInternalRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
            NV_ERR_INVALID_LOCK_STATE);

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
    void *pAllocParams,
    NvU32 paramsSize
)
{
    // Simple forwarder for now
    RM_API *pInternalRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    return pInternalRmApi->AllocWithHandle(pInternalRmApi, hClient, hParent, hObject, hClass, pAllocParams, paramsSize);
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
    pGpu->vmmuSegmentSize = 0;

    if (!IS_VIRTUAL(pGpu))
    {
        NV_STATUS  status;
        RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_GPU_GET_VMMU_SEGMENT_SIZE_PARAMS params;

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
    }

    else if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);

        pGpu->vmmuSegmentSize =  pVSI->vgpuStaticProperties.vmmuSegmentSize;
    }

    return NV_OK;
}

static NvU32 gpuGetDceClientInternalClientHandle(OBJGPU *pGpu)
{
    NvU32 hClient = RS_CLIENT_INTERNAL_HANDLE_BASE;
    return hClient;
}
static NV_STATUS _gpuAllocateInternalObjects
(
    OBJGPU *pGpu
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    NV_STATUS status = NV_OK;

    if (IS_GSP_CLIENT(pGpu))
    {
        if (IS_DCE_CLIENT(pGpu))
        {
            pGpu->hInternalClient = gpuGetDceClientInternalClientHandle(pGpu);
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

        rmapiControlCacheSetGpuAttrForObject(pGpu->hInternalClient,
                                             pGpu->hInternalSubdevice,
                                             pGpu);

        rmapiControlCacheSetGpuAttrForObject(pGpu->hInternalClient,
                                             pGpu->hInternalDevice,
                                             pGpu);
    }
    else
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        NV_ASSERT_OK_OR_RETURN(rmapiutilAllocClientAndDeviceHandles(
            pRmApi, pGpu, &pGpu->hInternalClient, &pGpu->hInternalDevice, &pGpu->hInternalSubdevice));

        NV_ASSERT_OK_OR_GOTO(status, serverGetClientUnderLock(&g_resServ, pGpu->hInternalClient,
            &pGpu->pCachedRsClient), done);
        NV_ASSERT_OK_OR_GOTO(status, subdeviceGetByHandle(pGpu->pCachedRsClient, pGpu->hInternalSubdevice,
            &pGpu->pCachedSubdevice), done);
    }

    NV_PRINTF(LEVEL_INFO, "GPU-%d allocated hInternalClient=0x%08x\n", pGpu->gpuInstance, pGpu->hInternalClient);

    //
    // Allocate the internal client for lock stress testing if lock stress testing is
    // enabled through the registry.
    //
    if (pSys->getProperty(pSys, PDB_PROP_SYS_ENABLE_RM_TEST_ONLY_CODE))
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            pRmApi->AllocWithHandle(pRmApi,
                                    NV01_NULL_OBJECT,
                                    NV01_NULL_OBJECT,
                                    NV01_NULL_OBJECT,
                                    NV01_ROOT,
                                    &pGpu->hInternalLockStressClient,
                                    sizeof(pGpu->hInternalLockStressClient)),
                                    done);
    }
    else
        pGpu->hInternalLockStressClient = NV01_NULL_OBJECT;


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
    // Free internal lock stress client if it was allocated
    if (pGpu->hInternalLockStressClient != NV01_NULL_OBJECT)
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

        pRmApi->Free(pRmApi, pGpu->hInternalLockStressClient,
            pGpu->hInternalLockStressClient);
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        rmapiControlCacheFreeObjectEntry(pGpu->hInternalClient, pGpu->hInternalSubdevice);
        rmapiControlCacheFreeObjectEntry(pGpu->hInternalClient, pGpu->hInternalDevice);
    }
    else
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
    GpuEngineOrder        *pEngineOrder = &pGpu->engineOrder;
    NvU32                  numEngineDesc, curEngineDesc;
    NvU32                  listTypes[] = {GCO_LIST_INIT, GCO_LIST_LOAD, GCO_LIST_UNLOAD, GCO_LIST_DESTROY};
    ENGDESCRIPTOR        **ppEngDescriptors[4];
    ENGLIST_ITER           it;
    ENGDESCRIPTOR          engDesc;

    ct_assert(NV_ARRAY_ELEMENTS(ppEngDescriptors) == NV_ARRAY_ELEMENTS(listTypes));

#define GPU_CHILD(a, b, numInstances, c, d) +numInstances

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

    numLists = NV_ARRAY_ELEMENTS(listTypes);

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
    GpuEngineOrder *pEngineOrder = &pGpu->engineOrder;

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
 * @brief   For a given @ref OBJGPU, given the @ref NVOC_CLASS_ID for an object
 *          that is a @ref OBJGPU child, this function will return the
 *          @ref NVOC_CLASS_INFO for the concrete class type that should be
 *          instantiated for the field with that @ref NVOC_CLASS_ID
 *
 * @details This function helps support polymorphism of @ref OBJGPU children.
 *          The provided @ref NVOC_CLASS_ID is the class ID for the base class
 *          of the pointer field in @ref OBJGPU (e.g., @ref OBJGPU::pPmu), which
 *          can be made to point at different sub-classes at runtime. This
 *          function, given that @ref NVOC_CLASS_ID, provides the information
 *          about which concrete sub-class should actually be constructed.
 *
 * @param[in]   pGpu
 * @param[in]   classId
 *  @ref NVOC_CLASS_ID for the class type of the base class pointer in
 *  @ref OBJGPU
 * @param[out]  ppClassInfo
 *  Pointer to location into which to store pointer to the class info for the
 *  concrete class to be constructed
 *
 * @return  @ref NV_OK
 *  Success
 * @return  @ref NV_ERR_INVALID_STATE
 *  No match for classId found within this @ref OBJGPU's children
 */
static NV_STATUS
_gpuChildNvocClassInfoGet
(
    OBJGPU                 *pGpu,
    NVOC_CLASS_ID           classId,
    const NVOC_CLASS_INFO **ppClassInfo
)
{
    NvU32 i;

    for (i = 0U; i < pGpu->numChildrenPresent; i++)
    {
        if (classId == pGpu->pChildrenPresent[i].classId)
        {
            *ppClassInfo = pGpu->pChildrenPresent[i].pClassInfo;
            return NV_OK;
        }
    }

    DBG_BREAKPOINT();
    return NV_ERR_INVALID_STATE;
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
OBJENGSTATE *
gpuGetEngstateNoShare_IMPL(OBJGPU *pGpu, ENGDESCRIPTOR engDesc)
{
    switch (ENGDESC_FIELD(engDesc, _CLASS))
    {
#define GPU_CHILD_SINGLE_INST(className, accessorName, c, d, e) \
        case classId(className):                                \
            return dynamicCast((Dynamic*)accessorName(pGpu), OBJENGSTATE);
#define GPU_CHILD_MULTI_INST(className, accessorName, c, d, e)  \
        case classId(className):                                \
            return dynamicCast((Dynamic*)accessorName(pGpu, ENGDESC_FIELD(engDesc, _INST)), OBJENGSTATE);

        #include "gpu/gpu_child_list.h"
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
OBJENGSTATE *
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

    return GPU_GET_KERNEL_FIFO_UC(pGpu);
}

/*!
 * @brief Iterates over pGpu's children, returning those that inherit given classId
 *
 * @param[in]      pGpu           OBJGPU pointer
 * @param[in,out]  pIt            Iterator
 * @param[in]      classId        classId of class the given child has to inherit
 *
 * @return         The next matching child, already cast to given type or NULL
 */
void *
gpuGetNextChildOfTypeUnsafe_IMPL
(
    OBJGPU *pGpu,
    GPU_CHILD_ITER *pIt,
    NvU32 classId
)
{
    void *pDerivedChild;
    const NvU32 numChildren = gpuGetNumChildren(pGpu);

    while (pIt->childIndex < numChildren)
    {
        Dynamic *pDynamicChild = gpuGetChild(pGpu, pIt->childIndex++);
        if (pDynamicChild == NULL)
            continue;

        pDerivedChild = objDynamicCastById(pDynamicChild, classId);
        if (pDerivedChild != NULL)
            return pDerivedChild;
    }

    return NULL;
}

/*!
 * @brief Returns the hosteng for the child object with the given engine descriptor
 *
 * All engines are uniquely identified by their engine descriptor.
 *
 * @param[in] pGpu     OBJGPU pointer
 * @param[in] engDesc  ENGDESCRIPTOR
 */
OBJHOSTENG *
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
 */
static NV_STATUS
gpuCreateObject
(
    OBJGPU       *pGpu,
    NVOC_CLASS_ID classId,
    NvU64         instanceID,
    Dynamic     **ppChildPtr
)
{
    NV_STATUS                   status;
    OBJENGSTATE                *pEngstate;
    ENGDESCRIPTOR               engDesc = MKENGDESC(classId, instanceID);
    const NVOC_CLASS_INFO      *pClassInfo;
    Dynamic                    *pConcreteChild      = NULL;
    ENGSTATE_TRANSITION_DATA    engTransitionData;

    if (!gpuShouldCreateObject(pGpu, classId, instanceID))
        return NV_OK;

    NV_ASSERT_OK_OR_RETURN(
        _gpuChildNvocClassInfoGet(pGpu, classId, &pClassInfo));

    // Ask the object database utility to create a child object.
    status = objCreateDynamic(&pConcreteChild, pGpu, pClassInfo);

    if (status != NV_OK)
    {
        return status;
    }
    NV_ASSERT_OR_RETURN(pConcreteChild != NULL, NV_ERR_INVALID_STATE);

    //
    // Cast back to a pointer to the base class and assign it into the pointer
    // in OBJGPU
    //
    *ppChildPtr = objDynamicCastById(pConcreteChild, classId);
    NV_ASSERT_TRUE_OR_GOTO(status,
        (*ppChildPtr != NULL),
        NV_ERR_INVALID_STATE,
        gpuCreateObject_exit);

    pEngstate = dynamicCast(*ppChildPtr, OBJENGSTATE);

    if (pEngstate == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        goto gpuCreateObject_exit;
    }

    status = engstateConstructBase(pEngstate, pGpu, engDesc);
    NV_CHECK_OR_GOTO(LEVEL_INFO, status == NV_OK, gpuCreateObject_exit);

    engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_CONSTRUCT, &engTransitionData);
    status = engstateConstructEngine(pGpu, pEngstate, engDesc);
    engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_CONSTRUCT, &engTransitionData);

    // If engine is missing, free it immediately
    if (pEngstate->getProperty(pEngstate, PDB_PROP_ENGSTATE_IS_MISSING))
    {
        status = NV_ERR_NOT_SUPPORTED;
    }

gpuCreateObject_exit:
    if (status != NV_OK)
    {
        objDelete(pConcreteChild);
        *ppChildPtr    = NULL;
    }

    //
    // It's not an error if the engine is simply unsupported.
    // We correct the status here instead of propagating it to the caller,
    // to minimize the generated code size since it directly impacts GSP-RM perf
    //
    if (status == NV_ERR_NOT_SUPPORTED)
        status = NV_OK;

    return status;
}


void
gpuDestruct_IMPL
(
    OBJGPU *pGpu
)
{
    HWBC_LIST           *pGpuHWBCList = NULL;
    NvS32                i;

    // Call gpuacctDisableAccounting if accounting is enabled since it does some memory deallocation
    if (hypervisorIsVgxHyper() ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON))
    {
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

        NV_STATUS status = gpuDisableAccounting(pGpu, NV_TRUE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "gpuDisableAccounting failed with error %d on GPU ID %d\n",
                      status, pGpu->gpuId);
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

    if (
        IS_VIRTUAL(pGpu))
    {
        vgpuDestructObject(pGpu);
    }

    // Free children in reverse order from construction
    for (i = (NvS32)gpuGetNumChildren(pGpu) - 1; i >= 0; i--)
    {
        Dynamic *pChild = gpuGetChild(pGpu, i);
        if (pChild)
        {
            objDelete(pChild);
            pGpu->children.pChild[i] = NULL;
        }
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

    multimapDestroy(&pGpu->videoEventBufferBindingsUid);

#if KERNEL_GSP_TRACING_RATS_ENABLED
    multimapDestroy(&pGpu->gspTraceEventBufferBindingsUid);
#endif

    portMemFree(pGpu->pDpcThreadState);

    gpuDestructPhysical(pGpu);
}

/*!
 * @brief   Initializes @ref OBJGPU::pChildrenPresent data
 *
 * @param[in]   pGpu
 *
 * @return  @ref NV_OK
 *  Success
 */
static NV_STATUS
_gpuChildrenPresentInit
(
    OBJGPU *pGpu
)
{
    pGpu->pChildrenPresent =
        gpuGetChildrenPresent_HAL(pGpu, &pGpu->numChildrenPresent);
    return NV_OK;
}

static NV_STATUS
gpuCreateChildObjects
(
    OBJGPU *pGpu,
    NvBool  bConstructEarly
)
{
    NV_STATUS status;

#define GPU_CHILD_SINGLE_INST(className, accessorName, numInstances, bEarly, gpuField)  \
    if (bEarly == bConstructEarly)                                                      \
    {                                                                                   \
        status = gpuCreateObject(pGpu, classId(className), 0,                           \
                                 (Dynamic**)&pGpu->children.named.gpuField);            \
        if (status != NV_OK)                                                            \
            return status;                                                              \
    }

#define GPU_CHILD_MULTI_INST(className, accessorName, numInstances, bEarly, gpuField)   \
    if (bEarly == bConstructEarly)                                                      \
    {                                                                                   \
        NvU64 i;                                                                        \
        for (i = 0; i < numInstances; i++)                                              \
        {                                                                               \
            status = gpuCreateObject(pGpu, classId(className), i,                       \
                                     (Dynamic**)&pGpu->children.named.gpuField[i]);     \
            if (status != NV_OK)                                                        \
                return status;                                                          \
        }                                                                               \
    }

    #include "gpu/gpu_child_list.h"

    return NV_OK;
}

static NvBool
gpuShouldCreateObject
(
    OBJGPU *pGpu,
    NvU32 classId,
    NvU32 instance
)
{
    NvU32 childIdx;

    // Let the HAL confirm that we should create an object for this engine.
    for (childIdx = 0; childIdx < pGpu->numChildrenPresent; childIdx++)
    {
        if (classId == pGpu->pChildrenPresent[childIdx].classId)
        {
            return (instance < pGpu->pChildrenPresent[childIdx].instances);
        }
    }

    return NV_FALSE;
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

/*!
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
 *
 *   Additionally, note that this function handles engines that were never
 *   present according to gpuChildrenPresent_HAL; these engines' associated
 *   classes must be removed from the class DB, and that is handled via this
 *   function.
 */
static NV_STATUS
gpuRemoveMissingEngines
(
    OBJGPU        *pGpu
)
{
    NvU32          curEngDescIdx;
    ENGDESCRIPTOR *pEngDescriptorList = gpuGetInitEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NV_STATUS      rmStatus           = NV_OK;
    NvU32          curClassDescIdx;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        OBJENGSTATE  *pEngstate;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
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

    //
    // Check the rest of the class descriptors for engines that are not part of
    // the present list, and therefore not part of the init list, and ensure
    // that those get removed from the class DB.
    //
    for (curClassDescIdx = 0U;
         curClassDescIdx < pGpu->engineOrder.numClassDescriptors;
         curClassDescIdx++)
    {
        const GPU_RESOURCE_DESC *const pCurDesc     =
            &pGpu->engineOrder.pClassDescriptors[curClassDescIdx];
        NvBool bHostSupportsEngine = NV_FALSE;

        //
        // Skip any classes which:
        //  1.) Do not have an engine class ID
        //  2.) Have an engine of GPU. ENG_GPU does not correspond to an
        //      OBJENGSTATE, and it can never be missing.
        //  3.) Have an OBJENGSTATE that is present
        //
        if ((ENGDESC_FIELD(pCurDesc->engDesc, _CLASS) == classId(OBJINVALID)) ||
            (pCurDesc->engDesc == ENG_GPU) ||
            gpuGetEngstate(pGpu, pCurDesc->engDesc) != NULL)
        {
            continue;
        }

        //
        // If the engstate is NULL, the engine may still be supported on GSP or VGPU host. If
        // it is, we can skip removing it.
        //
        if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
        {
            bHostSupportsEngine = gpuCheckEngineWithOrderList_HAL(pGpu, pCurDesc->engDesc, NV_FALSE);
        }

        if (bHostSupportsEngine)
            continue;

        NV_ASSERT_OK_OR_RETURN(
            gpuDeleteClassFromClassDBByEngTag(pGpu, pCurDesc->engDesc));
    }

    // Update the engine table after deleting any classes from the class DB
    NV_ASSERT_OK_OR_RETURN(
        gpuUpdateEngineTable(pGpu));

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
    Dynamic      *pDynamic = objFullyDerive(pEngstate);
    NvU32         i;

    engstateInitMissing(pGpu, pEngstate);
    objDelete(pDynamic);

    for (i = 0; i < gpuGetNumChildren(pGpu); i++)
    {
        if (pGpu->children.pChild[i] == pDynamic)
        {
            pGpu->children.pChild[i] = NULL;
            return;
        }
    }
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
    ENGDESCRIPTOR  *pEngDescriptor      = gpuGetInitEngineDescriptors(pGpu);
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
gpuMissingEngDescriptor(ENGDESCRIPTOR *pEngDescriptor, NvU32 maxDescriptors,
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
    ENGDESCRIPTOR *pEngDesc          = NULL;
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
    ENGDESCRIPTOR *pEngDesc          = NULL;
    NvU32          numEngDescriptors = gpuGetNumEngDescriptors(pGpu);
    ENGDESCRIPTOR  engDescriptor     = engDesc;
    NV_STATUS      rmStatus = NV_OK;
    NvBool bHostSupported = NV_FALSE;

    if (IS_GSP_CLIENT(pGpu) || IS_VIRTUAL(pGpu))
        bHostSupported = gpuCheckEngineWithOrderList_HAL(pGpu, engDesc, NV_FALSE);

    // remove Class tagged with engDesc from Class Database.
    if (!bHostSupported)
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

    if (!bHostSupported)
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
    NV_STATUS      rmStatus = NV_OK;

    //
    // The prereq tracker must be kept track of in stateInit/Destroy because
    // it accumulates dependencies throughout stateInit, stateInit may happen
    // multiple times in SLI linking, and it and does not destroy the prereq list
    // until the entire object is destroyed
    //
    NV_ASSERT_OK_OR_RETURN(
        objCreate(&pGpu->pPrereqTracker, pGpu, PrereqTracker, pGpu));

    // Quadro, Geforce SMB, Tesla, VGX, Titan GPU detection
    NV_ASSERT_OK_OR_RETURN(gpuInitBranding(pGpu));

    // Set GC6 specific values in OBJGPU, as per static data from GSP.
    gpuGetRtd3GC6Data(pGpu);

    //
    // gpuDetermineSelfHostedMode must be called after gpuDetermineVirtualMode/kgspInitRm
    // where VGPU/GSP static info is populated as the self hosted detection mechanism
    // in VF and Kernel-RM depends on them respectively.
    //
    // For Kernel-RM, gpuDetermineSelfHostedMode_HAL should be called in
    // gpuStatePreInit because GSP static config gets populated only by
    // PreInit and also all the gpuIsSelfHosted callers are from PreInit and
    // onwards.
    //
    gpuDetermineSelfHostedMode_HAL(pGpu);

    NV_ASSERT_OR_RETURN(rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_ASSERT_OK_OR_RETURN(_gpuAllocateInternalObjects(pGpu));
    NV_ASSERT_OK_OR_RETURN(_gpuInitChipInfo(pGpu));
    NV_ASSERT_OK_OR_RETURN(gpuConstructUserRegisterAccessMap(pGpu));
    NV_ASSERT_OK_OR_RETURN(gpuBuildGenericKernelFalconList(pGpu));

    NV_ASSERT_OK_OR_RETURN(gpuBuildKernelVideoEngineList(pGpu));

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED))
    {
        NvBool bGpuSupportMig = gpuValidateMIGSupport_HAL(pGpu);

        pGpu->setProperty(pGpu, PDB_PROP_GPU_MIG_SUPPORTED, bGpuSupportMig);
        if (!bGpuSupportMig)
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_MIG_TIMESLICING_SUPPORTED, bGpuSupportMig);
        }
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvgpumgrMigTimeslicingModeEnabled(pGpu));
    }

    rmStatus = gpuRemoveMissingEngines(pGpu);
    NV_ASSERT(rmStatus == NV_OK);

    pGpu->bFullyConstructed = NV_TRUE;

    ENGDESCRIPTOR *pEngDescriptorList = gpuGetInitEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
        OBJENGSTATE  *pEngstate        = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        rmStatus = gpuLoadFailurePathTest(pGpu, NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_PREINIT, curEngDescIdx, NV_FALSE);
        if (rmStatus == NV_OK)
        {
            engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_PRE_INIT, &engTransitionData);
            rmStatus = engstateStatePreInit(pGpu, pEngstate);
            engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_PRE_INIT, &engTransitionData);
        }
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
                case ENG_HDACODEC:
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

    // RM User Shared Data is currently unable to support VGPU due to isolation requirements
    if (IS_VIRTUAL(pGpu))
    {
        gpuDeleteClassFromClassDBByClassId(pGpu, RM_USER_SHARED_DATA);
    }

    gpuInitOptimusSettings(pGpu);

    return rmStatus;
}

// TODO: Merge structurally equivalent code with other gpuState* functions.
NV_STATUS
gpuStateInit_IMPL
(
    OBJGPU *pGpu
)
{
    NV_STATUS rmStatus = NV_OK;

    // Initialize numaNodeId to invalid node ID as "0" can be considered valid node
    pGpu->numaNodeId = NV0000_CTRL_NO_NUMA_NODE;

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Move the below code into appropriate ENGSTATE objects.
    //       DO NOT ADD MORE SPECIAL CASES HERE!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    NV_ASSERT_OR_RETURN(rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_ASSERT_OK_OR_GOTO(rmStatus,
        gpuStateInitStartedSatisfy_HAL(pGpu, pGpu->pPrereqTracker),
        gpuStateInit_exit);

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Move the above code into appropriate ENGSTATE objects.
    //       DO NOT ADD MORE SPECIAL CASES HERE!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    ENGDESCRIPTOR *pEngDescriptorList = gpuGetInitEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
        OBJENGSTATE  *pEngstate        = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        rmStatus = gpuLoadFailurePathTest(pGpu, NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_INIT, curEngDescIdx, NV_FALSE);
        if (rmStatus == NV_OK)
        {
            engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_INIT, &engTransitionData);
            rmStatus = engstateStateInit(pGpu, pEngstate);
            engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_INIT, &engTransitionData);
        }

        // RMCONFIG:  Bail on errors unless the feature/object/engine/class
        //            is simply unsupported

        if (rmStatus == NV_ERR_NOT_SUPPORTED)
            rmStatus = NV_OK;
        if (rmStatus != NV_OK)
            goto gpuStateInit_exit;
    }

    // Set a property indicating that VF BAR0 MMU TLB Invalidation register emulation is required or not.
    if (hypervisorIsVgxHyper())
    {
        if (
            IsdADA(pGpu) ||
            0)
        {
            NvU32 data32 = NV_REG_STR_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE_DEFAULT;

            // Registry override to change default mode, i.e, emulate VF MMU TLB Invalidation register
            if ((osReadRegistryDword(pGpu, NV_REG_STR_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE, &data32) == NV_OK) &&
                (data32 == NV_REG_STR_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE_DISABLE))
            {
                pGpu->setProperty(pGpu, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE, NV_FALSE);
            }
        }
        else
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_BUG_3007008_EMULATE_VF_MMU_TLB_INVALIDATE, NV_FALSE);
        }
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
    NV_STATUS      rmStatus           = NV_OK;
    ENGDESCRIPTOR *pEngDescriptorList = gpuGetLoadEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
        OBJENGSTATE *pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        rmStatus = gpuLoadFailurePathTest(pGpu, NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_PRELOAD, curEngDescIdx, NV_FALSE);
        if (rmStatus == NV_OK)
        {
            RMTRACE_ENGINE_PROFILE_EVENT("gpuStatePreLoadEngStart", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

            engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_PRE_LOAD, &engTransitionData);
            rmStatus = engstateStatePreLoad(pGpu, pEngstate, flags);
            engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_PRE_LOAD, &engTransitionData);

            RMTRACE_ENGINE_PROFILE_EVENT("gpuStatePreLoadEngEnd", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);
        }
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
    NV_STATUS      rmStatus = NV_OK;
    NvU32          status   = NV_OK;

    pGpu->registerAccess.regReadCount = pGpu->registerAccess.regWriteCount = 0;
    RMTRACE_ENGINE_PROFILE_EVENT("gpuStateLoadStart", pGpu->gpuId, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

    _gpuDetectNvswitchSupport(pGpu);

    /* Hibernation RESTORE steps for vGPU GSP.
     * 1. Call vgpuReinitializeRpcInfraOnStateLoad() to re-init RPC infratructure.
     * 2. Call vgpuGspSetupBuffers() to allocate GSP buffer in SYSMEM.
     * 3. Restore hibernate data by calling NV_RM_RPC_RESTORE_HIBERNATION_DATA RPC.
     * 4. Initialize BAR2.
     * 5. Call vgpuGspTeardownBuffers() to teardown GSP buffers allocated in SYSMEM in step2.
     * 6. Call vgpuGspSetupBuffers() to allocate GSP buffer back in FBMEM.
     */
    if (IS_VIRTUAL_WITH_FULL_SRIOV(pGpu) && (flags & GPU_STATE_FLAGS_PRESERVING))
    {
        rmStatus = vgpuReinitializeRpcInfraOnStateLoad(pGpu);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to re-init RPC infrastructure on resume, status 0x%x\n", rmStatus);
            DBG_BREAKPOINT();
            return rmStatus;
        }
    }

    // Initialize SRIOV specific members of OBJGPU
    status = gpuInitSriov_HAL(pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error initializing SRIOV: 0x%0x\n", status);
        return status;
    }

    if (IS_VIRTUAL_WITH_FULL_SRIOV(pGpu) && (flags & GPU_STATE_FLAGS_PRESERVING))
    {
        NV_RM_RPC_RESTORE_HIBERNATION_DATA(pGpu, rmStatus);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "RPC to restore host hibernation data failed, status 0x%x\n", rmStatus);
            DBG_BREAKPOINT();
            return rmStatus;
        }
    }

    if (!(flags & GPU_STATE_FLAGS_PRESERVING))
    {
        // It is a no-op on baremetal and inside non SRIOV guest.
        rmStatus = gpuCreateDefaultClientShare_HAL(pGpu);
        if (rmStatus != NV_OK)
        {
            return rmStatus;
        }
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
    if ((flags & GPU_STATE_FLAGS_PRESERVING) && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
    {
        // Allocate vGPU GSP buffers to FB_MEM.
        NV_PRINTF(LEVEL_INFO, "RPC: Allocate vGPU GSP buffers to FB_MEM\n");
        vgpuGspTeardownBuffers(pGpu);
        rmStatus = vgpuGspSetupBuffers(pGpu);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "RPC buffers setup failed: 0x%x\n", rmStatus);
            return rmStatus;
        }
    }

    ENGDESCRIPTOR *pEngDescriptorList = gpuGetLoadEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    // Set indicator that we are running state load
    pGpu->bStateLoading = NV_TRUE;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
        OBJENGSTATE *pEngstate = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        rmStatus = gpuLoadFailurePathTest(pGpu, NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_LOAD, curEngDescIdx, NV_FALSE);
        if (rmStatus == NV_OK)
        {
            RMTRACE_ENGINE_PROFILE_EVENT("gpuStateLoadEngStart", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

            engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_LOAD, &engTransitionData);
            rmStatus = engstateStateLoad(pGpu, pEngstate, flags);
            engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_LOAD, &engTransitionData);
        }

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
        {
            goto gpuStateLoad_exit;
        }

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

    // Video logging is not a required feature, don't override existing status
    NV_CHECK(LEVEL_ERROR, gpuInitVideoLogging(pGpu) == NV_OK);

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

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH) &&
            pGpu->bRecheckSliSupportAtResume)
        {
            NvU32 gpuAttachCnt, gpuAttachMask;
            NvU32 gpuInstance = 0;
            OBJGPU *gpuLoop;
            NvU32 numPoweredOn = 1; // Include the current pGpu
            gpumgrGetGpuAttachInfo(&gpuAttachCnt, &gpuAttachMask);

            while ((gpuLoop = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)))
            {
                if (gpuIsGpuFullPower(gpuLoop))
                    numPoweredOn++;
            }
            if (numPoweredOn == gpuAttachCnt)
            {
                RmInitScalability(pGpu);
            }
        }
    }

{
    NvBool bVgpuOnGspEnabled = IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && RMCFG_FEATURE_PLATFORM_GSP;
    if ((hypervisorIsVgxHyper() || bVgpuOnGspEnabled) &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON) &&
        !IS_MIG_ENABLED(pGpu))
    {
        // If VGX host, enable per process accounting by default.
        NV_STATUS gpuacctStatus = gpuEnableAccounting(pGpu);

        // Don't return this error since GPU accounting is just a reporting feature, we don't
        // want to halt execution as a result of it failing
        if (gpuacctStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "gpuEnableAccounting failed with error %d on GPU ID %d\n",
                    gpuacctStatus, pGpu->gpuId);
        }
    }
}

    // Clear indicator that we are running state load
    pGpu->bStateLoading = NV_FALSE;

    // Set a property indicating that the state load has been done
    pGpu->bStateLoaded = NV_TRUE;

    RMTRACE_ENGINE_PROFILE_EVENT("gpuStateLoadEnd", pGpu->gpuId, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);

gpuStateLoad_exit:
    return rmStatus;
}

NV_STATUS gpuEnableAccounting_IMPL(OBJGPU *pGpu)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
    NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS params;

    params.gpuId = pGpu->gpuId;
    params.pid = 0;
    params.newState = NV0000_CTRL_GPU_ACCOUNTING_STATE_ENABLED;

    return gpuacctEnableAccounting(pGpuAcct, pGpu->gpuInstance, &params);
}

NV_STATUS gpuDisableAccounting_IMPL(OBJGPU *pGpu, NvBool bForce)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    GpuAccounting *pGpuAcct = SYS_GET_GPUACCT(pSys);
    NV0000_CTRL_GPUACCT_SET_ACCOUNTING_STATE_PARAMS params;

    params.gpuId = pGpu->gpuId;
    params.pid = 0;
    params.newState = NV0000_CTRL_GPU_ACCOUNTING_STATE_DISABLED;

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
    if (bForce)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_ACCOUNTING_ON, NV_FALSE);
    }

    return gpuacctDisableAccounting(pGpuAcct, pGpu->gpuInstance, &params);
}

static NV_STATUS
_gpuRemoveP2pCapsFromPeerGpus
(
    OBJGPU *pGpu
)
{
    OBJGPU *pPeerGpu = NULL;
    NV_STATUS status = NV_OK;
    NvU32 attachMask;
    NvU32 gpuCount;
    NvU32 gpuIndex;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          gpumgrGetGpuAttachInfo(&gpuCount, &attachMask));
    gpuIndex = 0;
    while ((pPeerGpu = gpumgrGetNextGpu(attachMask, &gpuIndex)) != NULL)
    {
        RM_API *pPeerRmApi = GPU_GET_PHYSICAL_RMAPI(pPeerGpu);
        NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS removeP2PCapsParams = {0};

        if (!gpuIsStateLoaded(pPeerGpu))
        {
            continue;
        }

        removeP2PCapsParams.peerGpuIdCount = 1;
        removeP2PCapsParams.peerGpuIds[0] = pGpu->gpuId;

        NV_CHECK_OK_OR_CAPTURE_FIRST_ERROR(status,
                                           LEVEL_ERROR,
                                           pPeerRmApi->Control(pPeerRmApi,
                                                               pPeerGpu->hInternalClient,
                                                               pPeerGpu->hInternalSubdevice,
                                                               NV2080_CTRL_CMD_INTERNAL_REMOVE_P2P_CAPS,
                                                               &removeP2PCapsParams,
                                                               sizeof(removeP2PCapsParams)));
    }

    return status;
}

static NV_STATUS
_gpuPropagateP2PCapsToAllGpus
(
    OBJGPU *pAttachedGpu
)
{
    NV_STATUS status = NV_OK;
    NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PARAMS *pSetP2PCapsParams = NULL;
    OBJGPU *pGpu = NULL;
    NvU32 *peerGpuIds = NULL;
    NvU32 *peerGpuInstances = NULL;
    NvU32 attachMask;
    NvU32 gpuCount;
    NvU32 gpuIndex;
    NvU32 failingGpuIndex;
    NvU32 i;

    pSetP2PCapsParams = portMemAllocNonPaged(sizeof(*pSetP2PCapsParams));
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
                       pSetP2PCapsParams != NULL,
                       NV_ERR_NO_MEMORY);

    peerGpuIds = portMemAllocNonPaged(sizeof(*peerGpuIds) *
                                      NV_ARRAY_ELEMENTS(pSetP2PCapsParams->peerGpuInfos));
    NV_CHECK_OR_ELSE(LEVEL_ERROR,
                     peerGpuIds != NULL,
                     status = NV_ERR_NO_MEMORY; goto exit);

    peerGpuInstances = portMemAllocNonPaged(sizeof(*peerGpuInstances) *
                                            NV_ARRAY_ELEMENTS(pSetP2PCapsParams->peerGpuInfos));
    NV_CHECK_OR_ELSE(LEVEL_ERROR,
                     peerGpuInstances != NULL,
                     status = NV_ERR_NO_MEMORY; goto exit);

    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                        gpumgrGetGpuAttachInfo(&gpuCount, &attachMask),
                        exit);

    gpuIndex = 0;
    while ((pGpu = gpumgrGetNextGpu(attachMask, &gpuIndex)) != NULL)
    {
        RM_API *pPeerRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        if ((!gpuIsStateLoaded(pGpu)) && (pGpu != pAttachedGpu))
        {
            continue;
        }

        portMemSet(pSetP2PCapsParams, 0, sizeof(*pSetP2PCapsParams));

        // The newly attached GPU needs to be informed of all of the current GPUs caps,
        // whereas all the others need to be informed of the attached GPU only.
        if (pGpu == pAttachedGpu)
        {
            NvU32 peerGpuIndex = 0;
            OBJGPU *pPeerGpu = NULL;

            i = 0;
            while ((pPeerGpu = gpumgrGetNextGpu(attachMask, &peerGpuIndex)) != NULL)
            {
                if ((!gpuIsStateLoaded(pPeerGpu)) && (pPeerGpu != pAttachedGpu))
                {
                    continue;
                }

                peerGpuIds[i] = pPeerGpu->gpuId;
                peerGpuInstances[i] = gpuGetInstance(pPeerGpu);
                i++;
            }

            pSetP2PCapsParams->peerGpuCount = i;
        }
        else
        {
            pSetP2PCapsParams->peerGpuCount = 1;

            peerGpuIds[0] = pAttachedGpu->gpuId;
            peerGpuInstances[0] = gpuGetInstance(pAttachedGpu);
        }

        for (i = 0; i < pSetP2PCapsParams->peerGpuCount; i++)
        {
            NV2080_CTRL_INTERNAL_SET_P2P_CAPS_PEER_INFO *pPeerInfo = NULL;

            pPeerInfo = &pSetP2PCapsParams->peerGpuInfos[i];
            pPeerInfo->gpuId = peerGpuIds[i];
            pPeerInfo->gpuInstance = peerGpuInstances[i];

            NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                                CliGetSystemP2pCaps((NvU32[]) { pGpu->gpuId, pPeerInfo->gpuId },
                                                    (pGpu->gpuId == pPeerInfo->gpuId) ? 1 : 2,
                                                   &pPeerInfo->p2pCaps,
                                                   &pPeerInfo->p2pOptimalReadCEs,
                                                   &pPeerInfo->p2pOptimalWriteCEs,
                                                    pPeerInfo->p2pCapsStatus,
                                                   &pPeerInfo->busPeerId,
                                                   &pPeerInfo->busEgmPeerId),
                                fail);
        }

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                            pPeerRmApi->Control(pPeerRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                                NV2080_CTRL_CMD_INTERNAL_SET_P2P_CAPS,
                                                pSetP2PCapsParams, sizeof(*pSetP2PCapsParams)),
                            fail);
    }

    goto exit;

fail:
    // Remove the caps from the successfully processed peer GPUs
    failingGpuIndex = gpuIndex;
    gpuIndex = 0;
    while (((pGpu = gpumgrGetNextGpu(attachMask, &gpuIndex)) != NULL) &&
           (gpuIndex != failingGpuIndex))
    {
        RM_API *pPeerRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV2080_CTRL_INTERNAL_REMOVE_P2P_CAPS_PARAMS removeP2PCapsParams = {0};
        NV_STATUS ignoredStatus;

        if ((!gpuIsStateLoaded(pGpu)) && (pGpu != pAttachedGpu))
        {
            continue;
        }

        if (pGpu == pAttachedGpu)
        {
            NvU32 peerGpuIndex = 0;
            OBJGPU *pPeerGpu = NULL;

            while ((pPeerGpu = gpumgrGetNextGpu(attachMask, &peerGpuIndex)) != NULL)
            {
                if ((!gpuIsStateLoaded(pPeerGpu)) && (pPeerGpu != pAttachedGpu))
                {
                    continue;
                }

                removeP2PCapsParams.peerGpuIds[removeP2PCapsParams.peerGpuIdCount] = pPeerGpu->gpuId;
                removeP2PCapsParams.peerGpuIdCount++;
            }
        }
        else
        {
            removeP2PCapsParams.peerGpuIdCount = 1;
            removeP2PCapsParams.peerGpuIds[0] = pAttachedGpu->gpuId;
        }

        NV_CHECK_OK(ignoredStatus,
                    LEVEL_ERROR,
                    pPeerRmApi->Control(pPeerRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                        NV2080_CTRL_CMD_INTERNAL_REMOVE_P2P_CAPS,
                                        &removeP2PCapsParams, sizeof(removeP2PCapsParams)));
    }

exit:
    portMemFree(pSetP2PCapsParams);
    pSetP2PCapsParams = NULL;

    portMemFree(peerGpuIds);
    peerGpuIds = NULL;

    portMemFree(peerGpuInstances);
    peerGpuInstances = NULL;

    return status;
}

static NV_STATUS
_gpuSetVgpuMgrConfig
(
    OBJGPU *pGpu
)
{
    RM_API *pPeerRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG_PARAMS params = {0};

    params.bSupportHeterogeneousTimeSlicedVgpuTypes = kvgpumgrIsHeterogeneousVgpuTypeSupported();

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                          pPeerRmApi->Control(pPeerRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                              NV2080_CTRL_CMD_VGPU_MGR_INTERNAL_SET_VGPU_MGR_CONFIG,
                                             &params, sizeof(params)));

    return NV_OK;
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
    NV_STATUS      rmStatus = NV_OK;
#if RMCFG_FEATURE_SLINEXT && defined(DEBUG)
    NvU32          numPoweredOn = 0;

    SLI_LOOP_START(SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        numPoweredOn++;
    SLI_LOOP_END

    // Ensure all GPUs are available
    NV_ASSERT(gpumgrGetSubDeviceCountFromGpu(pGpu) == numPoweredOn);
#endif

    ENGDESCRIPTOR *pEngDescriptorList = gpuGetLoadEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
        OBJENGSTATE  *pEngstate        = gpuGetEngstate(pGpu, curEngDescriptor);

        if (pEngstate == NULL)
        {
            continue;
        }

        rmStatus = gpuLoadFailurePathTest(pGpu, NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_POSTLOAD, curEngDescIdx, NV_FALSE);
        if (rmStatus == NV_OK)
        {
            RMTRACE_ENGINE_PROFILE_EVENT("gpuStatePostLoadEngStart", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);
            engstateLogStateTransitionPre(pEngstate, ENGSTATE_STATE_POST_LOAD, &engTransitionData);
            rmStatus = engstateStatePostLoad(pGpu, pEngstate, flags);
            engstateLogStateTransitionPost(pEngstate, ENGSTATE_STATE_POST_LOAD, &engTransitionData);
            RMTRACE_ENGINE_PROFILE_EVENT("gpuStatePostLoadEngEnd", curEngDescriptor, pGpu->registerAccess.regReadCount, pGpu->registerAccess.regWriteCount);
        }
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

    // Caching GID data, the GID is generated by PMU and passed to RM during PMU INIT message.
    //NV_ASSERT_OK(gpuGetGidInfo(pGpu, NULL, NULL, DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1)));

    if (hypervisorIsVgxHyper())
    {
        NV_CHECK_OK_OR_GOTO(rmStatus,
                            LEVEL_ERROR,
                            _gpuSetVgpuMgrConfig(pGpu),
                            gpuStatePostLoad_exit);
    }

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && IS_GSP_CLIENT(pGpu))
    {
        NV_CHECK_OK_OR_GOTO(rmStatus,
                            LEVEL_ERROR,
                            _gpuPropagateP2PCapsToAllGpus(pGpu),
                            gpuStatePostLoad_exit);

        NV_CHECK_OK_OR_GOTO(rmStatus,
                            LEVEL_ERROR,
                            kvgpumgrSendAllVgpuTypesToGsp(pGpu),
                            gpuStatePostLoad_exit);
    }

    if (!IS_VIRTUAL(pGpu))
    {
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
    }
    if (!hypervisorIsVgxHyper())
    {
        NV_ASSERT_OR_GOTO(gpuFabricProbeStart(pGpu, &pGpu->pGpuFabricProbeInfoKernel) == NV_OK,
                          gpuStatePostLoad_exit);
    }

    // terminate the load failure test
    if (rmStatus == NV_OK)
        gpuLoadFailurePathTest(pGpu, NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_POSTLOAD, 0, NV_TRUE);

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
    NV_STATUS           rmStatus = NV_OK;

    rmapiControlCacheFreeNonPersistentCacheForGpu(pGpu->gpuInstance);

    if (!hypervisorIsVgxHyper())
    {
        gpuFabricProbeStop(pGpu->pGpuFabricProbeInfoKernel);
    }

    portMemFree(pGpu->boardInfo);
    pGpu->boardInfo = NULL;

    ENGDESCRIPTOR *pEngDescriptorList = gpuGetUnloadEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
        OBJENGSTATE  *pEngstate        = gpuGetEngstate(pGpu, curEngDescriptor);

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

    if(IS_GSP_CLIENT(pGpu))
    {
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

        rmStatus = kgspUnloadRm(pGpu, pKernelGsp, KGSP_UNLOAD_MODE_NORMAL, GPU_STATE_FLAGS_FAST_UNLOAD);
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

    gpuFreeVideoLogging(pGpu);

    ENGDESCRIPTOR *pEngDescriptorList = gpuGetUnloadEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
        OBJENGSTATE  *pEngstate        = gpuGetEngstate(pGpu, curEngDescriptor);

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

    /* Hibernation SAVE steps for vGPU GSP.
    * 1. Initial GSP buffers in FBMEM during RPC init.
    * 2. Save hibernate data by calling NV_RM_RPC_SAVE_HIBERNATION_DATA RPC.
    * 3. Call vgpuGspTeardownBuffers() to teardown GSP buffers.
    * 4. Destroy BAR2.
    */
    if (IS_VIRTUAL_WITH_FULL_SRIOV(pGpu) && (flags & GPU_STATE_FLAGS_PRESERVING))
    {
        // Save hibernate data by calling NV_RM_RPC_SAVE_HIBERNATION_DATA RPC.
        NV_RM_RPC_SAVE_HIBERNATION_DATA(pGpu, rmStatus);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "RPC to save host hibernation data failed, status 0x%x\n", rmStatus);
            DBG_BREAKPOINT();
            return rmStatus;
        }
        // Call vgpuGspTeardownBuffers() to teardown GSP buffers.
        if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu))
        {
            vgpuGspTeardownBuffers(pGpu);
        }
    }

    // Call the gpuStatePostUnload routine
    rmStatus = gpuStatePostUnload(pGpu, flags);
    NV_ASSERT_OK(rmStatus);

    if (!(flags & GPU_STATE_FLAGS_PRESERVING))
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
    NV_STATUS rmStatus = NV_OK;

    ENGDESCRIPTOR *pEngDescriptorList = gpuGetUnloadEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
        OBJENGSTATE  *pEngstate        = gpuGetEngstate(pGpu, curEngDescriptor);

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

    if (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && IS_GSP_CLIENT(pGpu))
    {
        NV_ASSERT_OK(_gpuRemoveP2pCapsFromPeerGpus(pGpu));
    }

    return rmStatus;
}

NV_STATUS
gpuStateDestroy_IMPL
(
    OBJGPU *pGpu
)
{
    NV_STATUS      rmStatus = NV_OK;

    rmapiControlCacheFreeAllCacheForGpu(pGpu->gpuInstance);

    // remove all video event bind points before destroying gsp engine state below
    videoRemoveAllBindpointsForGpu(pGpu);

    ENGDESCRIPTOR *pEngDescriptorList = gpuGetDestroyEngineDescriptors(pGpu);
    NvU32          numEngDescriptors  = gpuGetNumEngDescriptors(pGpu);
    NvU32          curEngDescIdx;

    // Order is determined by gpuGetChildrenOrder_HAL pulling gpuChildOrderList array
    for (curEngDescIdx = 0; curEngDescIdx < numEngDescriptors; curEngDescIdx++)
    {
        ENGSTATE_TRANSITION_DATA engTransitionData;
        ENGDESCRIPTOR curEngDescriptor = pEngDescriptorList[curEngDescIdx];
        OBJENGSTATE  *pEngstate        = gpuGetEngstate(pGpu, curEngDescriptor);

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

    gpuStateInitStartedRetract_HAL(pGpu, pGpu->pPrereqTracker);

    // Clear the property indicating that the state initialization has been done
    if (rmStatus == NV_OK)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_STATE_INITIALIZED, NV_FALSE);
    }

    if (IS_GSP_CLIENT(pGpu))
    {
        KernelGsp *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);
        rmStatus = kgspUnloadRm(pGpu, pKernelGsp,
                                KGSP_UNLOAD_MODE_NORMAL, NV2080_CTRL_GPU_SET_POWER_STATE_GPU_LEVEL_0);
    }

    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: Move the above code into appropriate ENGSTATE objects.
    //       DO NOT ADD MORE SPECIAL CASES HERE!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    _gpuFreeInternalObjects(pGpu);
    gpuDestroyGenericKernelFalconList(pGpu);

    gpuDestroyKernelVideoEngineList(pGpu);

    objDelete(pGpu->pPrereqTracker);
    pGpu->pPrereqTracker = NULL;

    portMemFree(pGpu->pChipInfo);
    pGpu->pChipInfo = NULL;

    pGpu->bFullyConstructed = NV_FALSE;

    gpuDeinitOptimusSettings(pGpu);

    return rmStatus;
}

//
// Logic: If arch = requested AND impl = requested --> NV_TRUE
//
NvBool
gpuIsImplementation_IMPL
(
    OBJGPU *pGpu,
    HAL_IMPLEMENTATION halImpl
)
{
    NvU32 gpuArch, gpuImpl;

    gpuXlateHalImplToArchImpl(halImpl, &gpuArch, &gpuImpl);

    return ((gpuGetChipArch(pGpu) == gpuArch) &&
            (gpuGetChipImpl(pGpu) == gpuImpl));
}

/*!
 * @brief Initialize SBIOS settings for Optimus GOLD to driver loaded state.
 *
 * @param[in]  pGpu    GPU object pointer
 *
 * @return NV_OK
 */
NV_STATUS
gpuInitOptimusSettings_IMPL(OBJGPU *pGpu)
{
    NV_STATUS   status;
    NvU32       inOut;
    NvU32       data32;
    NvU16       rtnSize;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_MOBILE))
    {
        return NV_OK;
    }

    // Check regkey settings
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_D3_FEATURE,
                                                    &data32) == NV_OK)
    {
        // Enabled by default
        if (FLD_TEST_DRF(_REG_STR_RM, _D3_FEATURE, _DRIVER_CFG_SPACE_RESTORE, _DISABLED, data32))
        {
            pGpu->setProperty(pGpu, PDB_PROP_GPU_OPTIMUS_GOLD_CFG_SPACE_RESTORE, NV_FALSE);

            // return early so we do not change SBIOS behaviour
            return NV_OK;
        }
    }

    //
    // For Optimus GOLD + GC6 systems, the _OFF and _ON ACPI methods are shared
    // between normal GOLD entry and exit and GC6 TDR recovery paths.
    //
    // Previously SBIOS has saved and restored the GPU config space within
    // the _OFF and _ON methods, but for GC6 TDR recovery this
    // will not work and we need to restore config space in driver instead.
    //
    // To support both old and new configurations we query the SBIOS
    // capabilities to indicate to SBIOS that driver supports the config
    // space restore. If SBIOS also supports this, then it will indicate
    // to driver that driver has the ownership. Otherwise SBIOS will fallback
    // to owning the save and restore itself.
    //
    // This behavior is summarized by the following table:
    // +============+=======+==========+========+========+=================+
    // | DRIVER     | SBIOS | WR_EN    | TARGET | ACTUAL | GC6 TDR Support |
    // +============+=======+==========+========+========+=================+
    // | X          |  OLD  | X        | X      | SBIOS  | NO              |
    // | OLD        |  NEW  | FALSE    | X      | SBIOS  | NO              |
    // | NEW-LOAD   |  NEW  | TRUE     | DRIVER | DRIVER | YES             |
    // | NEW-UNLOAD |  NEW  | TRUE     | SBIOS  | SBIOS  | NO DRIVER       |
    // +------------+------------------+--------+--------+-----------------+
    //
    // NEW-LOAD and NEW-UNLOAD represent the states when a new driver loads
    // and unloads. It is important for new drivers to restore the ownership back
    // to SBIOS in case an old driver is loaded without a cold reboot.
    //
    rtnSize = (NvU16)sizeof(inOut);
    inOut = 0;
    inOut = FLD_SET_DRF(OP_FUNC, _OPTIMUSCAPS, _CFG_SPACE_OWNER_WR_EN,  _TRUE,   inOut);
    inOut = FLD_SET_DRF(OP_FUNC, _OPTIMUSCAPS, _CFG_SPACE_OWNER_TARGET, _DRIVER, inOut);

    //
    // It doesn't matter if this was not successful as we will enable the driver side
    // save restore anyway. This call is to save some work for the SBIOS if possible.
    //
    status = osCallACPI_DSM(pGpu,
                            ACPI_DSM_FUNCTION_NVOP,
                            NVOP_FUNC_OPTIMUSCAPS,
                            &inOut,
                            &rtnSize);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_NOTICE,
                  "SBIOS did not acknowledge cfg space owner change\n");
    }

    return NV_OK;
}

/*!
 * @brief Restore SBIOS settings for Optimus GOLD to driver unloaded state.
 *
 * @param[in]  pGpu    GPU object pointer
 *
 * @return NV_OK if successful
 * @return NV_ERR_INVALID_STATE if SBIOS failed to acknowledge the restore request
 * @return Bubbles up error codes on ACPI call failure
 */
NV_STATUS
gpuDeinitOptimusSettings_IMPL(OBJGPU *pGpu)
{
    NV_STATUS   status;
    NvU32       inOut;
    NvU16       rtnSize;

    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_MOBILE))
    {
        return NV_OK;
    }

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_OPTIMUS_GOLD_CFG_SPACE_RESTORE))
    {
        rtnSize = (NvU16)sizeof(inOut);
        inOut = 0;
        inOut = FLD_SET_DRF(OP_FUNC, _OPTIMUSCAPS, _CFG_SPACE_OWNER_WR_EN,  _TRUE,  inOut);
        inOut = FLD_SET_DRF(OP_FUNC, _OPTIMUSCAPS, _CFG_SPACE_OWNER_TARGET, _SBIOS, inOut);

        status = osCallACPI_DSM(pGpu,
                                ACPI_DSM_FUNCTION_NVOP,
                                NVOP_FUNC_OPTIMUSCAPS,
                                &inOut,
                                &rtnSize);
        // NV_ASSERT_OR_RETURN(status == NV_OK, status);
        // NV_ASSERT_OR_RETURN(FLD_TEST_DRF(OP_FUNC, _OPTIMUSCAPS, _CFG_SPACE_OWNER_ACTUAL,
        //                                _SBIOS, inOut),
        //                   NV_ERR_INVALID_STATE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_INFO,
                      "SBIOS did not acknowledge cfg space owner change\n");
        }
    }

    return NV_OK;
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

static NV_STATUS
gpuDetermineVirtualMode
(
    OBJGPU *pGpu
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJOS  *pOS = SYS_GET_OS(pSys);
    OBJGPU *pGpuTemp;
    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);
    NvU32 gpuAttachMask, gpuInstance;
    NvBool bIsVirtual = NV_FALSE;
    NvU32 config = 0;
    NvBool bNoHostBridgeDetected = NV_TRUE;

    if (pGpu->bIsSOC || pGpu->getProperty(pGpu, PDB_PROP_GPU_TEGRA_SOC_NVDISPLAY))
    {
        return NV_OK;
    }

    // Using Hypervisor native interface to detect
    if (pHypervisor && (!pHypervisor->bDetected))
        hypervisorDetection(pHypervisor, pOS);

    gpumgrGetGpuAttachInfo(NULL, &gpuAttachMask);
    gpuInstance = 0;

    config = GPU_REG_RD32(pGpu, NV_PMC_BOOT_1);

    if (FLD_TEST_DRF(_PMC, _BOOT_1, _VGPU, _VF, config))
    {
        NV_ASSERT(pGpu->bIsVirtualWithSriov);

        bIsVirtual = NV_TRUE;
        pGpu->bPipelinedPteMemEnabled = NV_TRUE;
    }
    else
    {
        bNoHostBridgeDetected = NV_FALSE;
    }

    _setPlatformNoHostbridgeDetect(bNoHostBridgeDetected);

    if (!pGpu->bSriovEnabled && !IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        pGpu->bWarBug200577889SriovHeavyEnabled = NV_FALSE;
    }

    gpuSetupVirtualGuestOwnedHW(pHypervisor, pGpu);

    // Checking if the VM is already vGPU enabled.
    // NMOS and vGPU cannot be simultaneously enabled on a VM.
    if (pGpu->bIsPassthru)
    {
        while ((pGpuTemp = gpumgrGetNextGpu(gpuAttachMask, &gpuInstance)) != NULL)
        {
            if (IS_VIRTUAL(pGpuTemp))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "vGPU and Passthrough not supported simultaneously on the same VM.\n");
                pGpu->bIsPassthru = NV_FALSE;
                return NV_ERR_NOT_SUPPORTED;
            }
        }
    }

    // Early detection at GPU creation time should be consistent to the real detection
    NV_ASSERT_OR_RETURN(pGpu->isVirtual == bIsVirtual, NV_ERR_INVALID_STATE);

    return NV_OK;
}

NvBool
gpuIsImplementationOrBetter_IMPL
(
    OBJGPU *pGpu,
    HAL_IMPLEMENTATION halImpl
)
{
    NvU32 gpuArch, gpuImpl;
    NvU32 chipArch;

    gpuXlateHalImplToArchImpl(halImpl, &gpuArch, &gpuImpl);

    // "is implementation or better" is only defined between 2 gpus within
    // the same "gpu series" as defined in config/Chips.pm and nv_arch.h
    chipArch = gpuGetChipArch(pGpu);

    if (DRF_VAL(GPU, _ARCHITECTURE, _SERIES, chipArch) != DRF_VAL(GPU, _ARCHITECTURE, _SERIES, gpuArch))
    {
        return NV_FALSE;
    }

    // In case there is a temporal ordering we need to account for
    return gpuSatisfiesTemporalOrder(pGpu, halImpl);
}

static void
gpuXlateHalImplToArchImpl
(
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


        case HAL_IMPL_AD102:
        {
            *gpuArch = GPU_ARCHITECTURE_ADA;
            *gpuImpl = GPU_IMPLEMENTATION_AD102;
            break;
        }

        case HAL_IMPL_AD103:
        {
            *gpuArch = GPU_ARCHITECTURE_ADA;
            *gpuImpl = GPU_IMPLEMENTATION_AD103;
            break;
        }

        case HAL_IMPL_AD104:
        {
            *gpuArch = GPU_ARCHITECTURE_ADA;
            *gpuImpl = GPU_IMPLEMENTATION_AD104;
            break;
        }

        case HAL_IMPL_AD106:
        {
            *gpuArch = GPU_ARCHITECTURE_ADA;
            *gpuImpl = GPU_IMPLEMENTATION_AD106;
            break;
        }

        case HAL_IMPL_AD107:
        {
            *gpuArch = GPU_ARCHITECTURE_ADA;
            *gpuImpl = GPU_IMPLEMENTATION_AD107;
            break;
        }

        case HAL_IMPL_GH100:
        {
            *gpuArch = GPU_ARCHITECTURE_HOPPER;
            *gpuImpl = GPU_IMPLEMENTATION_GH100;
            break;
        }

        case HAL_IMPL_GB100:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB1XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB100;
            break;
        }

        case HAL_IMPL_GB102:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB1XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB102;
            break;
        }

        case HAL_IMPL_GB10B:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB1XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB10B;
            break;
        }

        case HAL_IMPL_GB110:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB1XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB110;
            break;
        }

        case HAL_IMPL_GB112:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB1XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB112;
            break;
        }

        case HAL_IMPL_GB202:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB2XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB202;
            break;
        }

        case HAL_IMPL_GB203:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB2XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB203;
            break;
        }

        case HAL_IMPL_GB205:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB2XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB205;
            break;
        }

        case HAL_IMPL_GB206:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB2XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB206;
            break;
        }

        case HAL_IMPL_GB207:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB2XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB207;
            break;
        }

        case HAL_IMPL_GB20B:
        {
            *gpuArch = GPU_ARCHITECTURE_BLACKWELL_GB2XX;
            *gpuImpl = GPU_IMPLEMENTATION_GB20B;
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
// default Logic: If halImpl is equal or greater than requested --> NV_TRUE
//
// Arch and impl IDs are not guaranteed to be ordered.
// "halImpl" is used here to match the ordering in chip-config/NVOC
//
// NOTE: only defined for gpus within same gpu series
//
static NvBool
gpuSatisfiesTemporalOrder
(
    OBJGPU *pGpu,
    HAL_IMPLEMENTATION halImpl
)
{
    NvBool result = NV_FALSE;

    switch (halImpl)
    {
        default:
        {
            HAL_IMPLEMENTATION chipImpl = pGpu->halImpl;
            NV_ASSERT(chipImpl < HAL_IMPL_MAXIMUM);

            result = (chipImpl >= halImpl);

            break;
        }
    }

    return result;
}

static void
gpuSetupVirtualGuestOwnedHW
(
    OBJHYPERVISOR *pHypervisor,
    OBJGPU *pGpu
)
{
    if (RMCFG_FEATURE_PLATFORM_MODS || IS_SIMULATION(pGpu))
    {
        // platform lacks method to issue PCI config read cycles (so, not detected).
        return;
    }

    NvU32 cfgBaseAddressLow, domain;
    NvU8 bus = 0, device = 0;
    void *handle = NULL;

    if ((pHypervisor == NULL) || !pHypervisor->bDetected || !pHypervisor->bIsHVMGuest)
        return;

    domain = gpuGetDomain(pGpu);
    bus = gpuGetBus(pGpu);
    device = gpuGetDevice(pGpu);

    handle = osPciInitHandle(domain, bus, device, 0, NULL, NULL);

    _setPlatformNoHostbridgeDetect((handle == NULL) ? NV_FALSE : NV_TRUE);

    if (handle == NULL)
    {
        return;
    }

    pGpu->hPci = handle;

    if (!IS_VIRTUAL_WITH_SRIOV(pGpu) && !IS_VIRTUAL(pGpu))
    {
        cfgBaseAddressLow = osPciReadDword(handle, NV_CONFIG_PCI_NV_5);

        if (!IS_BAR_64(cfgBaseAddressLow))
        {
            if (osPciReadDword(handle, NV_CONFIG_PCI_NV_6) != 0)
            {
                // We can access the BAR2 from 0x18, so the BAR2 is moved by host.
                pGpu->bBar2MovedByVtd = NV_TRUE;
                NV_PRINTF(LEVEL_INFO, "VT-d moved BAR2 to 0x18.\n");
            }
            else
            {
                NV_PRINTF(LEVEL_INFO, "VT-d still keeps BAR2 at 0x1C.\n");
            }
        }
        else
        {
            pGpu->bBar1Is64Bit = NV_TRUE;
            NV_PRINTF(LEVEL_INFO, "VT-d is using a 64bit BAR1.\n");
        }

        pGpu->bIsPassthru = NV_TRUE;

        NV_PRINTF(LEVEL_INFO,
                  "GPU at domain: %d bus: %d, device: %d is virtual (HW passthrough mode).\n",
                  domain, bus, device);
    }
}

// =============== Engine Database ==============================

typedef struct {
    RM_ENGINE_TYPE clientEngineId;
    NVOC_CLASS_ID class;
    NvU32         instance;
    NvBool        bHostEngine;
} EXTERN_TO_INTERNAL_ENGINE_ID;

static const EXTERN_TO_INTERNAL_ENGINE_ID rmClientEngineTable[] =
{
    { RM_ENGINE_TYPE_GR0,        classId(Graphics)   , 0,  NV_TRUE },
    { RM_ENGINE_TYPE_GR1,        classId(Graphics)   , 1,  NV_TRUE },
    { RM_ENGINE_TYPE_GR2,        classId(Graphics)   , 2,  NV_TRUE },
    { RM_ENGINE_TYPE_GR3,        classId(Graphics)   , 3,  NV_TRUE },
    { RM_ENGINE_TYPE_GR4,        classId(Graphics)   , 4,  NV_TRUE },
    { RM_ENGINE_TYPE_GR5,        classId(Graphics)   , 5,  NV_TRUE },
    { RM_ENGINE_TYPE_GR6,        classId(Graphics)   , 6,  NV_TRUE },
    { RM_ENGINE_TYPE_GR7,        classId(Graphics)   , 7,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY0,      classId(OBJCE)      , 0,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY1,      classId(OBJCE)      , 1,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY2,      classId(OBJCE)      , 2,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY3,      classId(OBJCE)      , 3,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY4,      classId(OBJCE)      , 4,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY5,      classId(OBJCE)      , 5,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY6,      classId(OBJCE)      , 6,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY7,      classId(OBJCE)      , 7,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY8,      classId(OBJCE)      , 8,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY9,      classId(OBJCE)      , 9,  NV_TRUE },
    { RM_ENGINE_TYPE_COPY10,     classId(OBJCE)      , 10, NV_TRUE },
    { RM_ENGINE_TYPE_COPY11,     classId(OBJCE)      , 11, NV_TRUE },
    { RM_ENGINE_TYPE_COPY12,     classId(OBJCE)      , 12, NV_TRUE },
    { RM_ENGINE_TYPE_COPY13,     classId(OBJCE)      , 13, NV_TRUE },
    { RM_ENGINE_TYPE_COPY14,     classId(OBJCE)      , 14, NV_TRUE },
    { RM_ENGINE_TYPE_COPY15,     classId(OBJCE)      , 15, NV_TRUE },
    { RM_ENGINE_TYPE_COPY16,     classId(OBJCE)      , 16, NV_TRUE },
    { RM_ENGINE_TYPE_COPY17,     classId(OBJCE)      , 17, NV_TRUE },
    { RM_ENGINE_TYPE_COPY18,     classId(OBJCE)      , 18, NV_TRUE },
    { RM_ENGINE_TYPE_COPY19,     classId(OBJCE)      , 19, NV_TRUE },
    { RM_ENGINE_TYPE_NVDEC0,     classId(OBJBSP)     , 0,  NV_TRUE },
    { RM_ENGINE_TYPE_NVDEC1,     classId(OBJBSP)     , 1,  NV_TRUE },
    { RM_ENGINE_TYPE_NVDEC2,     classId(OBJBSP)     , 2,  NV_TRUE },
    { RM_ENGINE_TYPE_NVDEC3,     classId(OBJBSP)     , 3,  NV_TRUE },
    { RM_ENGINE_TYPE_NVDEC4,     classId(OBJBSP)     , 4,  NV_TRUE },
    { RM_ENGINE_TYPE_NVDEC5,     classId(OBJBSP)     , 5,  NV_TRUE },
    { RM_ENGINE_TYPE_NVDEC6,     classId(OBJBSP)     , 6,  NV_TRUE },
    { RM_ENGINE_TYPE_NVDEC7,     classId(OBJBSP)     , 7,  NV_TRUE },
    { RM_ENGINE_TYPE_CIPHER,     classId(OBJCIPHER)  , 0,  NV_TRUE },
    { RM_ENGINE_TYPE_NVENC0,     classId(OBJMSENC)   , 0,  NV_TRUE },
    { RM_ENGINE_TYPE_NVENC1,     classId(OBJMSENC)   , 1,  NV_TRUE },
    { RM_ENGINE_TYPE_NVENC2,     classId(OBJMSENC)   , 2,  NV_TRUE },
// Bug 4175886 - Use this new value for all chips once GB20X is released
    { RM_ENGINE_TYPE_NVENC3,     classId(OBJMSENC)   , 3,  NV_TRUE },
    { RM_ENGINE_TYPE_SW,         classId(OBJSWENG)   , 0,  NV_TRUE },
    { RM_ENGINE_TYPE_SEC2,       classId(OBJSEC2)    , 0,  NV_TRUE },
    { RM_ENGINE_TYPE_NVJPEG0,    classId(OBJNVJPG)   , 0,  NV_TRUE },
    { RM_ENGINE_TYPE_NVJPEG1,    classId(OBJNVJPG)   , 1,  NV_TRUE },
    { RM_ENGINE_TYPE_NVJPEG2,    classId(OBJNVJPG)   , 2,  NV_TRUE },
    { RM_ENGINE_TYPE_NVJPEG3,    classId(OBJNVJPG)   , 3,  NV_TRUE },
    { RM_ENGINE_TYPE_NVJPEG4,    classId(OBJNVJPG)   , 4,  NV_TRUE },
    { RM_ENGINE_TYPE_NVJPEG5,    classId(OBJNVJPG)   , 5,  NV_TRUE },
    { RM_ENGINE_TYPE_NVJPEG6,    classId(OBJNVJPG)   , 6,  NV_TRUE },
    { RM_ENGINE_TYPE_NVJPEG7,    classId(OBJNVJPG)   , 7,  NV_TRUE },
    { RM_ENGINE_TYPE_OFA0,       classId(OBJOFA)     , 0,  NV_TRUE },
    { RM_ENGINE_TYPE_OFA1,       classId(OBJOFA)     , 1,  NV_TRUE },
    { RM_ENGINE_TYPE_DPU,        classId(OBJDPU)     , 0,  NV_FALSE },
    { RM_ENGINE_TYPE_PMU,        classId(Pmu)        , 0,  NV_FALSE },
    { RM_ENGINE_TYPE_FBFLCN,     classId(OBJFBFLCN)  , 0,  NV_FALSE },
    { RM_ENGINE_TYPE_HOST,       classId(KernelFifo) , 0,  NV_FALSE },
};

NV_STATUS gpuConstructEngineTable_IMPL
(
    OBJGPU *pGpu
)
{
    NvU32        engineIdx    = 0;

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
    for (engineIdx = 0; engineIdx < (NvU32)RM_ENGINE_TYPE_LAST; engineIdx++)
    {
        NV_STATUS status = gpuEngineEventNotificationListCreate(pGpu,
            &pGpu->engineNonstallIntrEventNotifications[engineIdx]);
        if (status != NV_OK)
        {
            gpuDestroyEngineTable(pGpu);
            return status;
        }
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
    for (NvU32 engineIdx = 0; engineIdx < (NvU32)RM_ENGINE_TYPE_LAST; engineIdx++)
        gpuEngineEventNotificationListDestroy(pGpu,
            pGpu->engineNonstallIntrEventNotifications[engineIdx]);

    if (pGpu->engineDB.pType)
    {
        pGpu->engineDB.size  = 0;
        portMemFree(pGpu->engineDB.pType);
        pGpu->engineDB.pType = NULL;
        pGpu->engineDB.bValid = NV_FALSE;
    }
}

NvBool gpuCheckEngineTable_IMPL
(
    OBJGPU *pGpu,
    RM_ENGINE_TYPE engType
)
{
    NvU32 engineIdx;

    if (!IS_MODS_AMODEL(pGpu))
    {
        NV_ASSERT_OR_RETURN(pGpu->engineDB.bValid, NV_FALSE);
    }

    NV_ASSERT_OR_RETURN(engType < RM_ENGINE_TYPE_LAST, NV_FALSE);

    for (engineIdx = 0; engineIdx < pGpu->engineDB.size; engineIdx++)
    {
        if (engType ==  pGpu->engineDB.pType[engineIdx])
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
    RM_ENGINE_TYPE clientEngineID,
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
    RM_ENGINE_TYPE *pClientEngineID
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
    RM_ENGINE_TYPE clientEngineId,
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
                ppGidString, pGidStrlen, gidFlags, RM_UUID_PREFIX_GPU);
        }
    }

    return rmStatus;
}

static void
_gpuSetDisconnectedPropertiesWorker
(
    NvU32  gpuInstance,
    void  *pArg
)
{
    OBJGPU *pGpu = gpumgrGetGpu(gpuInstance);

    gpuRefreshRecoveryAction_HAL(pGpu, NV_FALSE);
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
    SET_GPU_GC6_STATE(pGpu, GPU_GC6_STATE_POWERED_ON);

    //
    // Queue a work item to refresh recovery action, as
    // gpuSetDisconnectedProperties can be called at raised (device) IRQL.
    //
    NV_ASSERT_OK(osQueueWorkItemWithFlags(pGpu,
                                          _gpuSetDisconnectedPropertiesWorker,
                                          NULL,
                                          OS_QUEUE_WORKITEM_FLAGS_FALLBACK_TO_DPC
                                          | OS_QUEUE_WORKITEM_FLAGS_LOCK_GPU_GROUP_DEVICE));
}

#include "ctrl/ctrl0080/ctrl0080gpu.h"

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
    VGPU_STATIC_INFO *pVSI = gpuGetStaticInfo(pGpu);
    NV_STATUS status;
    NvU32 srcFalconIdx, tgtFalconIdx;

    NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams = NULL;
    NvBool bAllocatedParams = NV_FALSE;

    if (IS_VIRTUAL(pGpu))
    {
        NV_ASSERT(pVSI != NULL);
        pParams = &pVSI->constructedFalconInfo;
    }
    else
    {
        pParams = portMemAllocNonPaged(sizeof(*pParams));
        NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_NO_MEMORY);

        bAllocatedParams = NV_TRUE;

        portMemSet(pParams, 0, sizeof(*pParams));

        NV_ASSERT_OK_OR_GOTO(status,
            pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_GPU_GET_CONSTRUCTED_FALCON_INFO,
                            pParams, sizeof(*pParams)),
            done);
    }

    NV_ASSERT_TRUE_OR_GOTO(status,
        pParams->numConstructedFalcons <= NV_ARRAY_ELEMENTS(pGpu->genericKernelFalcons),
        NV_ERR_BUFFER_TOO_SMALL, done);

    tgtFalconIdx = 0;
    for (srcFalconIdx = 0; srcFalconIdx < pParams->numConstructedFalcons; srcFalconIdx++)
    {
        KernelFalconEngineConfig config = {0};

        if (pParams->constructedFalconsTable[srcFalconIdx].engDesc == ENG_SEC2 &&
            GPU_GET_KERNEL_SEC2(pGpu) != NULL)
        {
            //
            // Do not create a GenericKernelFalcon for SEC2 if a KernelSec2 instance exists.
            // This is needed for pre-Turing VGPU guests as KernelSec2 is not supported there.
            //
            continue;
        }

        config.physEngDesc   = pParams->constructedFalconsTable[srcFalconIdx].engDesc;
        config.ctxAttr       = pParams->constructedFalconsTable[srcFalconIdx].ctxAttr;
        config.ctxBufferSize = pParams->constructedFalconsTable[srcFalconIdx].ctxBufferSize;
        config.addrSpaceList = pParams->constructedFalconsTable[srcFalconIdx].addrSpaceList;
        config.registerBase  = pParams->constructedFalconsTable[srcFalconIdx].registerBase;

        status = objCreate(&pGpu->genericKernelFalcons[tgtFalconIdx], pGpu, GenericKernelFalcon, pGpu, &config);
        tgtFalconIdx++;

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to create a GenericKernelFalcon object with engdesc %u\n",
                config.physEngDesc);
            goto done;
        }
    }

    pGpu->numGenericKernelFalcons = tgtFalconIdx;

done:
    if (bAllocatedParams)
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
        objDelete(pGpu->genericKernelFalcons[i]);
        pGpu->genericKernelFalcons[i] = NULL;
    }
    pGpu->numGenericKernelFalcons = 0;
}

NV_STATUS gpuBuildKernelVideoEngineList_IMPL(OBJGPU *pGpu)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS status = NV_OK;
    NvU32 numKernelVideoEngines = 0;
    VGPU_STATIC_INFO *pVSI = gpuGetStaticInfo(pGpu);
    NV2080_CTRL_GPU_GET_CONSTRUCTED_FALCON_INFO_PARAMS *pParams;
    NvU32 i;
    NvU32 data;

    // when regkey is not available, set default eventbuffer size to 32K
    data = DRF_NUM(_REG_STR, _RM_VIDEO_EVENT_TRACE, _EVENT_BUFFER_SIZE_IN_4k, 0x8);

    osReadRegistryDword(pGpu, NV_REG_STR_RM_VIDEO_EVENT_TRACE, &data);

    if (IS_VIRTUAL(pGpu))
    {
        NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);
        pParams = &pVSI->constructedFalconInfo;
    }
    else
    {
        pParams = portMemAllocNonPaged(sizeof(*pParams));
        NV_ASSERT_OR_RETURN(pParams != NULL, NV_ERR_NO_MEMORY);

        portMemSet(pParams, 0, sizeof(*pParams));

        NV_ASSERT_OK_OR_GOTO(
            status,
            pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_GPU_GET_CONSTRUCTED_FALCON_INFO,
                            pParams, sizeof(*pParams)),
            done);
    }

    for (i = 0; i < pParams->numConstructedFalcons; i++)
    {
        ENGDESCRIPTOR physEngDesc = pParams->constructedFalconsTable[i].engDesc;

        if (!IS_VIDEO_ENGINE(physEngDesc))
        {
            continue;
        }

        NV_ASSERT_OR_ELSE(numKernelVideoEngines < NV_ARRAY_ELEMENTS(pGpu->kernelVideoEngines),
            status = NV_ERR_INVALID_STATE; goto done;);

        NV_ASSERT_OK_OR_GOTO(
            status,
            objCreate(&pGpu->kernelVideoEngines[numKernelVideoEngines], pGpu, KernelVideoEngine, pGpu, physEngDesc),
            done);

        pGpu->kernelVideoEngines[numKernelVideoEngines]->videoTraceInfo.eventTraceRegkeyData = data;
        numKernelVideoEngines++;
    }

    pGpu->numKernelVideoEngines = numKernelVideoEngines;

done:
    if (!IS_VIRTUAL(pGpu))
    {
        portMemFree(pParams);
    }
    if (status != NV_OK)
    {
        gpuDestroyKernelVideoEngineList(pGpu);
    }
    return status;
}

NV_STATUS gpuInitVideoLogging_IMPL(OBJGPU *pGpu)
{
    NvU32 i;
    for (i = 0; i < pGpu->numKernelVideoEngines; i++)
    {
        if (pGpu->kernelVideoEngines[i] != NULL)
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kvidengInitLogging(pGpu, pGpu->kernelVideoEngines[i]));
        }
    }
    return NV_OK;
}

void gpuFreeVideoLogging_IMPL(OBJGPU *pGpu)
{
    NvU32 i;
    for (i = 0; i < pGpu->numKernelVideoEngines; i++)
    {
        if (pGpu->kernelVideoEngines[i] != NULL)
        {
            kvidengFreeLogging(pGpu, pGpu->kernelVideoEngines[i]);
        }
    }
}

void gpuDestroyKernelVideoEngineList_IMPL(OBJGPU *pGpu)
{
    NvU32 i;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGpu->kernelVideoEngines); i++)
    {
        objDelete(pGpu->kernelVideoEngines[i]);
        pGpu->kernelVideoEngines[i] = NULL;
    }
    pGpu->numKernelVideoEngines = 0;
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
 * @brief Get GFID State
 *
 * @param[in] pGpu   OBJGPU pointer
 * @param[in] gfid   GFID to be validated
 * @param[out] Current state of GFID
 */
NV_STATUS
gpuGetGfidState_IMPL(OBJGPU *pGpu, NvU32 gfid, GFID_ALLOC_STATUS *pState)
{
    if (!gpuIsSriovEnabled(pGpu))
        return NV_OK;

    if (pGpu->sriovState.pAllocatedGfids == NULL || pState == NULL)
    {
        return NV_ERR_INVALID_ADDRESS;
    }

    // Sanity check on GFID
    if (gfid > pGpu->sriovState.maxGfid)
    {
        return NV_ERR_OUT_OF_RANGE;
    }

    *pState = (GFID_ALLOC_STATUS)pGpu->sriovState.pAllocatedGfids[gfid];

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

/*!
 * @brief Set pAllocatedGfids allocated status as being invalidated
 *
 * @param[in] pGpu   OBJGPU pointer
 * @param[in] gfid   GFID to be set/unset (Assumes GFID is sanity checked before calling this function)
 */
void
gpuSetGfidInvalidated_IMPL(OBJGPU *pGpu, NvU32 gfid)
{
    NV_ASSERT_OR_RETURN_VOID(pGpu->sriovState.pAllocatedGfids != NULL);

    pGpu->sriovState.pAllocatedGfids[gfid] = GFID_INVALIDATED;
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
gpuGetNextInEngineOrderList(OBJGPU *pGpu, ENGLIST_ITER *pIt, ENGDESCRIPTOR *pEngDesc)
{
    NvBool                 bReverse = !!(pIt->flags & (GCO_LIST_UNLOAD | GCO_LIST_DESTROY));
    const GPUCHILDORDER   *pChildOrderList;
    NvU32                  numChildOrder;
    const GPUCHILDPRESENT *pCurChildPresent;
    const GPUCHILDORDER   *pCurChildOrder;
    NvBool                 bAdvance = NV_FALSE;
    NvBool                 bFirstIteration = NV_FALSE;

    pChildOrderList = gpuGetChildrenOrder_HAL(pGpu, &numChildOrder);

    if (!pIt->bStarted)
    {
        bFirstIteration = NV_TRUE;
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

        pCurChildPresent = gpuFindChildPresent(pGpu->pChildrenPresent,
            pGpu->numChildrenPresent, pCurChildOrder->classId);

        if (!pCurChildPresent)
        {
            bAdvance = NV_TRUE;
            continue;
        }

        if (bAdvance || bFirstIteration)
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
    void __nvoc_init_dataField_KernelDisplay(KernelDisplay*, RmHalspecOwner* );
    __nvoc_init_funcTable_KernelDisplay(pKernelDisplay, pRmHalspecOwner);
    __nvoc_init_dataField_KernelDisplay(pKernelDisplay, pRmHalspecOwner);

    void __nvoc_init_funcTable_DisplayInstanceMemory(DisplayInstanceMemory *, RmHalspecOwner *);
    void __nvoc_init_dataField_DisplayInstanceMemory(DisplayInstanceMemory *, RmHalspecOwner *);
    __nvoc_init_funcTable_DisplayInstanceMemory(KERNEL_DISPLAY_GET_INST_MEM(pKernelDisplay),
                                                pRmHalspecOwner);
    __nvoc_init_dataField_DisplayInstanceMemory(KERNEL_DISPLAY_GET_INST_MEM(pKernelDisplay),
                                                pRmHalspecOwner);

    void __nvoc_init_funcTable_KernelHead(KernelHead *, RmHalspecOwner *);
    void __nvoc_init_dataField_KernelHead(KernelHead *, RmHalspecOwner *);
    NvU32 headIdx;

    for (headIdx = 0; headIdx < OBJ_MAX_HEADS; headIdx++)
    {
        __nvoc_init_funcTable_KernelHead(KDISP_GET_HEAD(pKernelDisplay, headIdx),
                                         pRmHalspecOwner);
        __nvoc_init_dataField_KernelHead(KDISP_GET_HEAD(pKernelDisplay, headIdx),
                                         pRmHalspecOwner);
    }

    return NV_OK;
}

/*!
 *@brief Check if CC feature is enabled
 */
NvBool
gpuIsCCFeatureEnabled_IMPL(OBJGPU *pGpu)
{
    ConfidentialCompute  *pCC = GPU_GET_CONF_COMPUTE(pGpu);

    if (pCC != NULL)
    {
        return pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED);
    }
    return NV_FALSE;
}

/*!
 *@brief Check if APM feature is enabled
 */
NvBool
gpuIsApmFeatureEnabled_IMPL(OBJGPU *pGpu)
{
    ConfidentialCompute  *pCC = GPU_GET_CONF_COMPUTE(pGpu);

    if (pCC != NULL)
    {
        return pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_APM_FEATURE_ENABLED);
    }
    return NV_FALSE;
}

/*!
 *@brief Check if either CC or APM feature is enabled
 */
NvBool
gpuIsCCorApmFeatureEnabled_IMPL(OBJGPU *pGpu)
{
    return gpuIsCCFeatureEnabled(pGpu) || gpuIsApmFeatureEnabled(pGpu);
}

/*!
 *@brief Check if CC devtools mode is enabled
 */
NvBool
gpuIsCCDevToolsModeEnabled_IMPL(OBJGPU *pGpu)
{
    ConfidentialCompute  *pCC = GPU_GET_CONF_COMPUTE(pGpu);

    if ((pCC != NULL) && gpuIsCCFeatureEnabled(pGpu))
    {
        return pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_DEVTOOLS_MODE_ENABLED);
    }
    return NV_FALSE;
}

/*!
 *@brief Check if MultiGPU nvlink mode is enabled
 */
NvBool
gpuIsCCMultiGpuNvleModeEnabled_IMPL(OBJGPU *pGpu)
{
    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);

    if ((pCC != NULL) && gpuIsCCFeatureEnabled(pGpu))
    {
        return pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_MULTI_GPU_NVLE_MODE_ENABLED);
    }
    return NV_FALSE;
}

/*!
 *@brief Check if protected PCIe is enabled
 */
NvBool
gpuIsCCMultiGpuProtectedPcieModeEnabled_IMPL(OBJGPU *pGpu)
{
    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);

    if ((pCC != NULL) && gpuIsCCFeatureEnabled(pGpu))
    {
        return pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_MULTI_GPU_PROTECTED_PCIE_MODE_ENABLED);
    }
    return NV_FALSE;
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
    pGpu->chipInfo.pmcBoot0.arch          = decodePmcBoot0Architecture(pGpu->chipId0) << GPU_ARCH_SHIFT;
    pGpu->chipInfo.pmcBoot0.majorRev      = DRF_VAL(_PMC, _BOOT_0, _MAJOR_REVISION, pGpu->chipId0);
    pGpu->chipInfo.pmcBoot0.minorRev      = DRF_VAL(_PMC, _BOOT_0, _MINOR_REVISION, pGpu->chipId0);
    pGpu->chipInfo.pmcBoot0.minorExtRev   = NV2080_CTRL_GPU_INFO_MINOR_REVISION_EXT_NONE;
    pGpu->chipInfo.pmcBoot42.impl         = DRF_VAL(_PMC, _BOOT_42, _IMPLEMENTATION, pGpu->chipId1);
    pGpu->chipInfo.pmcBoot42.arch         = decodePmcBoot42Architecture(pGpu->chipId1) << GPU_ARCH_SHIFT;
    pGpu->chipInfo.pmcBoot42.majorRev     = DRF_VAL(_PMC, _BOOT_42, _MAJOR_REVISION, pGpu->chipId1);
    pGpu->chipInfo.pmcBoot42.minorRev     = DRF_VAL(_PMC, _BOOT_42, _MINOR_REVISION, pGpu->chipId1);
    pGpu->chipInfo.pmcBoot42.minorExtRev  = DRF_VAL(_PMC, _BOOT_42, _MINOR_EXTENDED_REVISION, pGpu->chipId1);

    //
    // SOC do not use pmcBoot0/pmcBoot42 and instead write the impl details to
    // these top level chipInfo fields, which is what the getters return.
    //
    pGpu->chipInfo.implementationId = pGpu->chipInfo.pmcBoot42.impl;
    pGpu->chipInfo.platformId       = pGpu->chipInfo.pmcBoot42.arch;
    pGpu->chipInfo.chipId           = decodePmcBoot42ChipId(pGpu->chipId1);
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

    return (1ULL << numPhysAddrBits) - 1;
}

VGPU_STATIC_INFO *gpuGetStaticInfo(OBJGPU *pGpu)
{
    if (IS_VIRTUAL(pGpu))
    {
        NV_ASSERT_OR_RETURN(GPU_GET_VGPU(pGpu) != NULL, NULL);
        return &GPU_GET_VGPU(pGpu)->_vgpuStaticInfo;
    }

    return NULL;
}

GspStaticConfigInfo *gpuGetGspStaticInfo(OBJGPU *pGpu)
{
    NV_ASSERT_OR_RETURN(GPU_GET_KERNEL_GSP(pGpu) != NULL, NULL);
    return &(GPU_GET_KERNEL_GSP(pGpu)->gspStaticInfo);
}

OBJRPC *gpuGetGspClientRpc(OBJGPU *pGpu)
{
    if (IS_GSP_CLIENT(pGpu))
    {
        NV_ASSERT_OR_RETURN(GPU_GET_KERNEL_GSP(pGpu) != NULL, NULL);
        return GPU_GET_KERNEL_GSP(pGpu)->pRpc;
    }
    return NULL;
}

OBJRPC *gpuGetVgpuRpc(OBJGPU *pGpu)
{
    if (IS_VIRTUAL(pGpu))
    {
        NV_ASSERT_OR_RETURN(GPU_GET_VGPU(pGpu) != NULL, NULL);
        return GPU_GET_VGPU(pGpu)->pRpc;
    }
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


/*!
 * @brief Sets the GC6/JT SBIOS capability
 *
 * The capabilities are retrieved from the SBIOS through JT_FUNC_CAPS subfunction
 *
 * @param[in]  pGpu GPU object pointer
 *
 * @return status   bubble up the return status from osCallACPI_DSM
 */
NV_STATUS
gpuSetGC6SBIOSCapabilities_IMPL(OBJGPU *pGpu)
{
    NV_STATUS   status;

    pGpu->acpiMethodData.jtMethodData.bSBIOSCaps = NV_FALSE;

    if ((!pGpu->acpiMethodData.bValid) ||
        (pGpu->acpiMethodData.jtMethodData.status != NV_OK))
    {
        RMTRACE_SBIOS (_ACPI_DSM_ERROR, pGpu->gpuId, ACPI_DSM_FUNCTION_JT, JT_FUNC_CAPS, 0, 0, 0, 0, 0);
        return NV_ERR_NOT_SUPPORTED;
    }

    status = gpuJtVersionSanityCheck_HAL(pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unsupported JT revision ID. GC6 is being disabled.\n");
        RMTRACE_SBIOS (_ACPI_DSM_ERROR, pGpu->gpuId, ACPI_DSM_FUNCTION_JT, JT_FUNC_CAPS, 0, 0, 0, 0, 0);
        return status;
    }

    if (FLD_TEST_DRF(_JT_FUNC, _CAPS, _JT_ENABLED, _TRUE, pGpu->acpiMethodData.jtMethodData.jtCaps))
    {
        pGpu->acpiMethodData.jtMethodData.bSBIOSCaps = NV_TRUE;

        switch (pGpu->acpiMethodData.jtMethodData.jtRevId)
        {
            case NV_JT_FUNC_CAPS_REVISION_ID_1_03:
                // GC6 2.0 production
                break;
            case NV_JT_FUNC_CAPS_REVISION_ID_2_00:
                // GC6 3.0 and FGC6 production
                break;
            default:
                NV_PRINTF(LEVEL_ERROR,
                          "Unsupported JT revision ID. GC6 is being disabled. Update the "
                          "board EC PIC FW. On Windows, update the SBIOS GC6 AML as well.\n");
                DBG_BREAKPOINT();
                pGpu->acpiMethodData.jtMethodData.bSBIOSCaps = NV_FALSE;
                break;
        }

    }

    RMTRACE_GPU(_GC6_SBIOS_CAP, pGpu->gpuId, pGpu->acpiMethodData.jtMethodData.jtCaps, pGpu->acpiMethodData.jtMethodData.jtRevId, pGpu->acpiMethodData.jtMethodData.bSBIOSCaps, 0, 0, 0, 0);
    RMTRACE_SBIOS (_ACPI_DSM_METHOD, pGpu->gpuId, ACPI_DSM_FUNCTION_JT, JT_FUNC_CAPS, pGpu->acpiMethodData.jtMethodData.jtCaps, 0, 0, 0, 0);

    return NV_OK;
}

NV_STATUS gpuSimEscapeWrite(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, NvU32 Value)
{
    NV_ASSERT_OR_RETURN(Size <= (sizeof Value), NV_ERR_INVALID_ARGUMENT);

    if (pGpu->bUseRpcSimEscapes)
    {
        return RmRpcSimEscapeWrite(pGpu, path, Index, Size, Value);
    }
    return osSimEscapeWrite(pGpu, path, Index, Size, Value);
}

NV_STATUS gpuSimEscapeWriteBuffer(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, void* pBuffer)
{
    return osSimEscapeWriteBuffer(pGpu, path, Index, Size, pBuffer);
}

NV_STATUS gpuSimEscapeRead(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, NvU32 *Value)
{
    NV_ASSERT_OR_RETURN(Size <= (sizeof *Value), NV_ERR_INVALID_ARGUMENT);

    //
    // Zero-initialize because the functions below don't fill in entire Value
    // for Size < 4
    //
    NvU32 readValue = 0;
    NV_STATUS status;

    if (pGpu->bUseRpcSimEscapes)
    {
        status = RmRpcSimEscapeRead(pGpu, path, Index, Size, &readValue);
    }
    else
    {
        status = osSimEscapeRead(pGpu, path, Index, Size, &readValue);
    }

    *Value = readValue;
    return status;
}

NV_STATUS gpuSimEscapeReadBuffer(OBJGPU *pGpu, const char *path, NvU32 Index, NvU32 Size, void* pBuffer)
{
    return osSimEscapeReadBuffer(pGpu, path, Index, Size, pBuffer);
}

//
// Only supported with Windows, Debug or Develop driver, or with Release drivers instrumented builds.
// Instrumented build: add RMCFG_OPTIONS="--enable=RMTEST" to nvmake command line
// Todo: add OpenRM/GSP support
//
#if defined(GPU_LOAD_FAILURE_TEST_SUPPORTED)
NV_STATUS
gpuLoadFailurePathTest_IMPL
(
    OBJGPU *pGpu,
    NvU32   engStage,
    NvU32   engDescIdx,
    NvBool  bStopTest
)
{
    NV_STATUS rmStatus = NV_OK;

    // Check that test is enabled and in the stating stage
    if (!FLD_TEST_DRF(_REG_STR, _GPU_LOAD_FAILURE_TEST, _STATUS, _START, pGpu->loadFailurePathTestControl))
    {
        return NV_OK;
    }

    // Not supported for suspend/resume
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IN_PM_RESUME_CODEPATH))
    {
        return NV_OK;
    }

    // Stop the test
    if (bStopTest)
    {
        pGpu->loadFailurePathTestControl = FLD_SET_DRF(_REG_STR, _GPU_LOAD_FAILURE_TEST, _STATUS, _FINISHED, pGpu->loadFailurePathTestControl);
        goto writeRegistryAndExit;
    }

    NvU32 regEngStage = DRF_VAL(_REG_STR, _GPU_LOAD_FAILURE_TEST, _STAGE, pGpu->loadFailurePathTestControl);
    NvU32 regEngDescIdx = DRF_VAL(_REG_STR, _GPU_LOAD_FAILURE_TEST, _ENGINEINDEX, pGpu->loadFailurePathTestControl);

    // Check that we are not yet in the targeted stage (preinit, init, preload, load or postload)
    if (engStage < regEngStage)
    {
        return NV_OK;
    }

    // Check that we are in the next stage
    if (engStage > regEngStage)
    {
        // Reset engine index to 0 and move to next index
        regEngDescIdx = 0;
        regEngStage = engStage;
    }

    // Check that the engine is the next one to be tested
    if (engDescIdx < regEngDescIdx)
    {
        return NV_OK;
    }

    // At this point we fail the engine
    rmStatus = NV_ERR_GENERIC;

    // Update registry key for next step
    pGpu->loadFailurePathTestControl = FLD_SET_DRF(_REG_STR, _GPU_LOAD_FAILURE_TEST, _STATUS, _RUNNING, pGpu->loadFailurePathTestControl);
    pGpu->loadFailurePathTestControl = FLD_SET_DRF_NUM(_REG_STR, _GPU_LOAD_FAILURE_TEST, _STAGE, regEngStage, pGpu->loadFailurePathTestControl);
    pGpu->loadFailurePathTestControl = FLD_SET_DRF_NUM(_REG_STR, _GPU_LOAD_FAILURE_TEST, _ENGINEINDEX, regEngDescIdx + 1, pGpu->loadFailurePathTestControl);

writeRegistryAndExit:
    osWriteRegistryDword(pGpu,
                         NV_REG_STR_GPU_LOAD_FAILURE_TEST,
                         pGpu->loadFailurePathTestControl);

    switch (engStage)
    {
        case NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_PREINIT:
            NV_PRINTF(LEVEL_ERROR, "Failing GPU PreInit for Engine ID 0x%x (%d)\n", engDescIdx, engDescIdx);
            break;
        case NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_INIT:
            NV_PRINTF(LEVEL_ERROR, "Failing GPU Init for Engine ID 0x%x (%d)\n", engDescIdx, engDescIdx);
            break;
        case NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_PRELOAD:
            NV_PRINTF(LEVEL_ERROR, "Failing GPU PreLoad for Engine ID 0x%x (%d)\n", engDescIdx, engDescIdx);
            break;
        case NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_LOAD:
            NV_PRINTF(LEVEL_ERROR, "Failing GPU Load for Engine ID 0x%x (%d)\n", engDescIdx, engDescIdx);
            break;
        case NV_REG_STR_GPU_LOAD_FAILURE_TEST_STAGE_POSTLOAD:
            NV_PRINTF(LEVEL_ERROR, "Failing GPU PostLoad for Engine ID 0x%x (%d)\n", engDescIdx, engDescIdx);
            break;
    }


    return rmStatus;
}
#endif

NvU32
gpuGetLitterValues_KERNEL
(
    OBJGPU *pGpu,
    NvU32 index
)
{
    KernelGraphicsManager *pKernelGraphicsManager = GPU_GET_KERNEL_GRAPHICS_MANAGER(pGpu);
    const NV2080_CTRL_INTERNAL_STATIC_GR_INFO *pGrInfo;
    NvU32 i;

    NV_ASSERT_OR_RETURN(kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->bInitialized, 0);
    pGrInfo = kgrmgrGetLegacyKGraphicsStaticInfo(pGpu, pKernelGraphicsManager)->pGrInfo;
    NV_ASSERT_OR_RETURN(pGrInfo != NULL, 0);

    for (i = 0; i < NV_ARRAY_ELEMENTS(pGrInfo->infoList); i++)
    {
        if (pGrInfo->infoList[i].index == index)
            return pGrInfo->infoList[i].data;
    }
    return 0;
}

NV_STATUS gpuGetChipDetails_IMPL
(
    OBJGPU *pGpu,
    NV2080_CTRL_GPU_GET_CHIP_DETAILS_PARAMS *pParams
)
{
    NV2080_CTRL_BIOS_GET_SKU_INFO_PARAMS biosGetSKUInfoParams;

    NV_ASSERT_OK_OR_RETURN(gpuGetSkuInfo_HAL(pGpu, &biosGetSKUInfoParams));

    //
    // GPU chip name (PCI device ID)
    // Upper half of pGpu->idInfo.PCIDeviceID is devid
    //
    pParams->pciDevId = (NvU16)DRF_VAL(_PCI, _SUBID, _DEVICE, pGpu->idInfo.PCIDeviceID);

    // GPU chip SKU
    portStringCopy((char *) pParams->chipSku,
                   sizeof(pParams->chipSku),
                   (char *) biosGetSKUInfoParams.chipSKU,
                   sizeof(biosGetSKUInfoParams.chipSKU));

    // GPU revision
    pParams->chipMajor = gpuGetChipMajRev(pGpu);
    pParams->chipMinor = gpuGetChipMinRev(pGpu);

    return NV_OK;
}

BRANDING_TYPE
gpuDetectBranding_IMPL
(
    OBJGPU *pGpu
)
{
    if (pGpu->bIsNvidiaNvs)
        return BRANDING_TYPE_NVS_NVIDIA;
    else if (pGpu->bIsQuadroAD)
        return BRANDING_TYPE_QUADRO_AD;
    else if (pGpu->bIsQuadro)
        return BRANDING_TYPE_QUADRO_GENERIC;

    return BRANDING_TYPE_NONE;
}

COMPUTE_BRANDING_TYPE
gpuDetectComputeBranding_IMPL(OBJGPU *pGpu)
{
    if (pGpu->bIsTesla)
        return COMPUTE_BRANDING_TYPE_TESLA;

    return COMPUTE_BRANDING_TYPE_NONE;
}

/*!
 * @brief Determine the VGX brand of the board
 * @returns the VGX brand
 */
BRANDING_TYPE
gpuDetectVgxBranding_IMPL(OBJGPU *pGpu)
{
    if (pGpu->bIsVgx)
        return BRANDING_TYPE_VGX;

    return BRANDING_TYPE_NONE;
}

/*!
 * @brief Returns the value of bAmpereErrorContainmentXidEnabled
 *
 * @param[In] pGpu  OBJGPU pointer
 *
 * @return the value of bAmpereErrorContainmentXidEnabled
.*/
NvBool
gpuIsAmpereErrorContainmentXidEnabled_KERNEL
(
    OBJGPU *pGpu
)
{
    //
    // When the poison fuse is enabled, both bGlobalPoisonFuseEnabled
    // and bAmpereErrorContainmentXidEnabled are set to NV_TRUE so just
    // use the existing function
    //
    return gpuIsGlobalPoisonFuseEnabled_HAL(pGpu);
}


/*!
 * @brief returns a boolean indicating if an SLI bridge is supported by the specified GPU.
 *
 * @param[In]   pGpu    The GPU to check for SLI bridge support
 *
 * @return      a boolean indicating if the specified GPU supports an SLI bridge.
 *              the bridge may be a Video bridge or NvLink.
.*/
NvBool
gpuIsSliLinkSupported_IMPL
(
    OBJGPU  *pGpu
)
{
    NvBool   bIsSupported = NV_FALSE;

    if (!bIsSupported)
    {
        KernelNvlink * pKernelNvLink =  GPU_GET_KERNEL_NVLINK(pGpu);
        if (pKernelNvLink != NULL)
        {
           bIsSupported = (knvlinkGetConnectedLinksMask_HAL(pGpu, pKernelNvLink) != 0U);
        }
    }

    return bIsSupported;
}

NvBool
gpuCheckEngine_KERNEL
(
    OBJGPU        *pGpu,
    ENGDESCRIPTOR  engDesc
)
{
    return gpuCheckEngineWithOrderList_KERNEL(pGpu, engDesc, NV_TRUE);
}

/*!
 * @brief Checks whether an engine is available or not.
 *
 * The 'engine' is an engine descriptor
 * This function is different from busProbeRegister in a sense that it doesn't
 * rely on timeouts after a read of a register in the reg space for engine.
 * Instead, it
 *  - Return TRUE for all engines which are must present in GPU.
 *  - Get information about CE, MSENC, NVJPG and OFA engines from plugin or GSP-RM.
 *  - If bCheckEngineOrder is true, the remaining engines are searched for in gpuChildOrderList_HAL.
 *
 * @param[in] pGpu       OBJGPU pointer
 * @param[in] engDesc    ENGDESCRIPTOR pointer used to check Engine presence
 * @param[in] bCheckEngineOrder If true, check gpuChildOrderList_HAL for engDesc as well
 *
 * @returns NV_TRUE if engine is available.
 *          NV_FALSE if engine is not available or floorswept.
 *
 */
NvBool
gpuCheckEngineWithOrderList_KERNEL
(
    OBJGPU        *pGpu,
    ENGDESCRIPTOR  engDesc,
    NvBool         bCheckEngineOrder
)
{
    NvU32     rmEngineCaps[NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX] = {0};
    NvU32     nv2080EngineCaps[NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX] = {0};
    NvBool    bSupported;
    NV_STATUS status;

    if (IS_VIRTUAL(pGpu))
    {
        NvU32             i     = 0;
        NvU32             j     = 0;
        VGPU_STATIC_INFO *pVSI  = GPU_GET_STATIC_INFO(pGpu);

        if (pVSI == NULL)
        {
            return NV_FALSE;
        }

        //
        // vGPU Enginelist can accommodate maximum of NVGPU_VGPU_ENGINE_LIST_LAST engine mask.
        // Currently, vGPU plugin advertises the same in NvU64 bitmask.
        //
        ct_assert(RM_ENGINE_TYPE_LAST <= NVGPU_VGPU_ENGINE_LIST_LAST);
        ct_assert(NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX <= (NVGPU_VGPU_ENGINE_LIST_MASK_ARRAY_MAX * 2));

        for (i = 0; i < (NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX/2); i++)
        {
            nv2080EngineCaps[j++] = NvU64_LO32(pVSI->engineList[i]);
            nv2080EngineCaps[j++] = NvU64_HI32(pVSI->engineList[i]);
        }
    }
    else
    {
        NvU32 i;
        GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);
        if (pGSCI == NULL)
        {
            return NV_FALSE;
        }

        for (i = 0; i < NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX; i++)
        {
            nv2080EngineCaps[i] = pGSCI->engineCaps[i];
        }
    }

    NV_CHECK_OK_OR_ELSE(status, LEVEL_ERROR,
        gpuGetRmEngineTypeCapMask(nv2080EngineCaps,
                                  NVGPU_ENGINE_CAPS_MASK_ARRAY_MAX,
                                  rmEngineCaps),
        return NV_FALSE);

    switch (ENGDESC_FIELD(engDesc, _CLASS))
    {
        case ENG_CLASS_LSFM:
        case ENG_CLASS_PMU:
        case ENG_CLASS_CLK:
        case ENG_CLASS_ACR:
        case ENG_CLASS_DISP:
            return NV_FALSE;
        //
        // This function is used in two environments:
        // (a) vGPU where display is not yet supported.
        // (b) RM offload (Kernel RM) where display is supported.
        //
        case ENG_CLASS_KERNEL_DISPLAY:
            return IS_GSP_CLIENT(pGpu);

        case ENG_CLASS_BIF:
        case ENG_CLASS_KERNEL_BIF:
        case ENG_CLASS_MC:
        case ENG_CLASS_KERNEL_MC:
        case ENG_CLASS_PRIV_RING:
        case ENG_CLASS_SW_INTR:
        case ENG_CLASS_TMR:
        case ENG_CLASS_DMA:
        case ENG_CLASS_BUS:
        case ENG_CLASS_CIPHER:
        case ENG_CLASS_INTR:
        case ENG_CLASS_GPULOG:
        case ENG_CLASS_GPUMON:
        case ENG_CLASS_FIFO:
            return NV_TRUE;

        case ENG_CLASS_CE:
        {
            NvBool isEnginePresent = !!NVGPU_GET_ENGINE_CAPS_MASK(rmEngineCaps,
                RM_ENGINE_TYPE_COPY(GET_CE_IDX(engDesc)));

            // This is a temporary WAR
            if(!isEnginePresent)
            {
                // Check if the CE actually exists and if it is required, mark it as available
                isEnginePresent = gpuRequireGrCePresence_HAL(pGpu, engDesc);
            }
            return isEnginePresent;
        }

        case ENG_CLASS_NVENC:
            return !!NVGPU_GET_ENGINE_CAPS_MASK(rmEngineCaps,
                RM_ENGINE_TYPE_NVENC(GET_MSENC_IDX(engDesc)));
        case ENG_CLASS_SEC2:
            return !!NVGPU_GET_ENGINE_CAPS_MASK(rmEngineCaps,
                                                RM_ENGINE_TYPE_SEC2);
        case ENG_CLASS_NVDEC:
            return !!NVGPU_GET_ENGINE_CAPS_MASK(rmEngineCaps,
                RM_ENGINE_TYPE_NVDEC(GET_NVDEC_IDX(engDesc)));

        case ENG_CLASS_OFA:
            return !!NVGPU_GET_ENGINE_CAPS_MASK(rmEngineCaps,
                RM_ENGINE_TYPE_OFA(GET_OFA_IDX(engDesc)));

        case ENG_CLASS_NVJPEG:
            return !!NVGPU_GET_ENGINE_CAPS_MASK(rmEngineCaps,
                RM_ENGINE_TYPE_NVJPEG(GET_NVJPEG_IDX(engDesc)));

        case ENG_CLASS_GR:
        {
            if (engDesc == ENG_GR(0))
            {
                return NV_TRUE;
            }

            KernelFifo *pKernelFifo  = GPU_GET_KERNEL_FIFO(pGpu);

            NV_ASSERT_OR_RETURN(pKernelFifo != NULL, NV_FALSE);

            return (kfifoCheckEngine_HAL(pGpu, pKernelFifo,
                                         engDesc,
                                         &bSupported) == NV_OK &&
                    bSupported);
        }

        case ENG_CLASS_INVALID:
            NV_PRINTF(LEVEL_ERROR,
                      "Query for ENG_INVALID considered erroneous: %d\n",
                      engDesc);
            return NV_TRUE;
        //
        // Check if engine descriptor is supported by current GPU.
        // Callee must not send engine descriptor which are not on
        // HAL lists of GPU. So Add ASSERT there.
        //
        default:
        {
            if (bCheckEngineOrder)
            {
                bSupported = gpuIsEngDescSupported(pGpu, engDesc);

                if (!bSupported)
                {
                    NV_PRINTF(LEVEL_ERROR, "Unable to check engine ID: 0x%x\n",
                              engDesc);
                    NV_ASSERT(bSupported);
                }
            }
            else
                bSupported = NV_FALSE;

            return bSupported;
        }
    }
}

/*!
 *  Obtains the valid scheduling policy for the current platform.
 *  Use: Determine whether software scheduling is required.
 */
const char *
gpuGetSchedulerPolicy_IMPL
(
    OBJGPU          *pGpu,
    NvU32           *pSchedPolicy
)
{
    NvU32   schedPolicy         = SCHED_POLICY_DEFAULT;
    NvU32   regkey              = 0;

    if (hypervisorIsVgxHyper() || (RMCFG_FEATURE_PLATFORM_GSP && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)))
    {
        //  Disable OBJSCHED_SW_ENABLE when GPU is older than Pascal.
        //  This is true for WDDM and vGPU scheduling
        if (!IsPASCALorBetter(pGpu))
        {
            schedPolicy = SCHED_POLICY_DEFAULT;
        }
        // for pre GB20x, when MIG is enabled, only allow fixed share policy
        else if (IS_MIG_IN_USE(pGpu) &&
                (hypervisorIsVgxHyper() || (IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu) && !IS_VIRTUAL(pGpu)))
                && (!IsGB20XorBetter(pGpu))
                )
        {
            schedPolicy = SCHED_POLICY_VGPU_FIXED_SHARE;
        }
        // Check the PVMRL regkey
        else if (osReadRegistryDword(pGpu, NV_REG_STR_RM_PVMRL, &regkey) == NV_OK &&
                 FLD_TEST_REF(NV_REG_STR_RM_PVMRL_ENABLE, _YES, regkey) )
        {
            NvU32 configSchedPolicy = REF_VAL(NV_REG_STR_RM_PVMRL_SCHED_POLICY, regkey);

            switch (configSchedPolicy)
            {
                case NV_REG_STR_RM_PVMRL_SCHED_POLICY_VGPU_EQUAL_SHARE:
                    schedPolicy = SCHED_POLICY_VGPU_EQUAL_SHARE;
                    break;
                case NV_REG_STR_RM_PVMRL_SCHED_POLICY_VGPU_FIXED_SHARE:
                    schedPolicy = SCHED_POLICY_VGPU_FIXED_SHARE;
                    break;
                default:
                    NV_PRINTF(LEVEL_INFO,
                              "Invalid scheduling policy %d specified by regkey.\n",
                              configSchedPolicy);
                    break;
            }
        }
        else
        {
            schedPolicy = SCHED_POLICY_BEST_EFFORT;
        }
    }

    *pSchedPolicy = schedPolicy;

    switch (schedPolicy)
    {
        case SCHED_POLICY_BEST_EFFORT:
            return MAKE_NV_PRINTF_STR("BEST_EFFORT");
        case SCHED_POLICY_VGPU_EQUAL_SHARE:
            return MAKE_NV_PRINTF_STR("EQUAL_SHARE");
        case SCHED_POLICY_VGPU_FIXED_SHARE:
            return MAKE_NV_PRINTF_STR("FIXED_SHARE");
        case SCHED_POLICY_DEFAULT:
        default:
            // For baremetal and PT
            return MAKE_NV_PRINTF_STR("NONE");
    }
}

void
gpuApplySchedulerPolicy_IMPL
(
    OBJGPU          *pGpu
)
{
    const char         *schedPolicyName;
    NvU32               configSchedPolicy;
    NvU32               domain             = gpuGetDomain(pGpu);
    NvU32               bus                = gpuGetBus(pGpu);
    NvU32               device             = gpuGetDevice(pGpu);
    NvBool              bIsSchedSwEnabled  = NV_FALSE;

    schedPolicyName = gpuGetSchedulerPolicy(pGpu, &configSchedPolicy);

    // PVMRL is disabled when GPU is older than Pascal
    if (((RMCFG_FEATURE_PLATFORM_GSP && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)) || hypervisorIsVgxHyper()) && IsPASCALorBetter(pGpu))
    {
        bIsSchedSwEnabled = (configSchedPolicy != SCHED_POLICY_DEFAULT);

        portDbgPrintf("NVRM: GPU at %04x:%02x:%02x.0 has software scheduler %s with policy %s.\n",
                      domain, bus, device,
                      bIsSchedSwEnabled ? MAKE_NV_PRINTF_STR("ENABLED") : MAKE_NV_PRINTF_STR("DISABLED"),
                      schedPolicyName);
    }
    else
    {
        // RM is not yet ready to print this message in release builds on baremetal.
        NV_PRINTF(LEVEL_INFO,
                  "GPU at %04x:%02x:%02x.0 has software scheduler %s with policy %s.\n",
                  domain, bus, device,
                  bIsSchedSwEnabled ? MAKE_NV_PRINTF_STR("ENABLED") : MAKE_NV_PRINTF_STR("DISABLED"),
                  schedPolicyName);
    }

    // Enabled SWRL Granular locking only if SWRL is enabled on hypervisor or VGPU_GSP_PLUGIN_OFFLOAD is enabled
    if (((RMCFG_FEATURE_PLATFORM_GSP && IS_VGPU_GSP_PLUGIN_OFFLOAD_ENABLED(pGpu)) || hypervisorIsVgxHyper()) && bIsSchedSwEnabled)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_SWRL_GRANULAR_LOCKING, NV_TRUE);
    }
}

static NV_STATUS
_gpuSetResetRequiredState
(
    OBJGPU *pGpu,
    NvBool  newState
)
{
    NvBool      prevState;
    NV_STATUS   status;

    status = gpuIsDeviceMarkedForReset(pGpu, &prevState);
    if (status != NV_OK)
    {
        goto _gpuSetResetRequiredState_exit;
    }

    status = gpuSetResetScratchBit_HAL(pGpu, newState);
    if (status != NV_OK)
    {
        goto _gpuSetResetRequiredState_exit;
    }

    if (prevState != newState)
    {
        status = gpuResetRequiredStateChanged_HAL(pGpu, newState);
    }

_gpuSetResetRequiredState_exit:
    return status;
}

/**
 * @brief Interface which allows GPU to be marked for pending reset.
 *
 * @param[in] pGpu          OBJGPU pointer
 *
 * @return                  NV_OK if successful
 */
NV_STATUS
gpuMarkDeviceForReset_IMPL
(
    OBJGPU *pGpu
)
{
    return _gpuSetResetRequiredState(pGpu, NV_TRUE);
}

/**
 * @brief Interface which allows GPU to be unmarked for pending reset.
 *
 * This should be used only for debug or testing through
 * NV2080_CTRL_CMD_GPU_UNMARK_DEVICE_FOR_RESET and never called directly.
 *
 * @param[in] pGpu          OBJGPU pointer
 *
 * @return                  NV_OK if successful
 */
NV_STATUS
gpuUnmarkDeviceForReset_IMPL
(
    OBJGPU *pGpu
)
{
    return _gpuSetResetRequiredState(pGpu, NV_FALSE);
}

/**
 * @brief Interface which returns if a GPU reset is needed.
 *
 * @param[in] pGpu                    OBJGPU pointer
 * @param[out] pbResetRequired        NvBool pointer indicating if the GPU needs a reset
 *
 * @return                            NV_OK if successful
 */
NV_STATUS
gpuIsDeviceMarkedForReset_IMPL
(
    OBJGPU *pGpu,
    NvBool *pbResetRequired
)
{
    return gpuGetResetScratchBit_HAL(pGpu, pbResetRequired);
}

static NV_STATUS
_gpuSetDrainAndResetState
(
    OBJGPU *pGpu,
    NvBool  newState
)
{
    NvBool      prevState;
    NV_STATUS   status;

    status = gpuIsDeviceMarkedForDrainAndReset(pGpu, &prevState);
    if (status != NV_OK)
    {
        goto _gpuSetDrainAndResetState_exit;
    }

    status = gpuSetDrainAndResetScratchBit_HAL(pGpu, newState);
    if (status != NV_OK)
    {
        goto _gpuSetDrainAndResetState_exit;
    }

    if (prevState != newState)
    {
        gpuRefreshRecoveryAction_HAL(pGpu, NV_FALSE);
    }

_gpuSetDrainAndResetState_exit:
    return status;
}

/*!
 * @brief Interface which allows GPU to be marked for pending drain and reset. This means,
 * applications should be drained from the GPU and the GPU reset to regain full operability.
 *
 * How is this state exposed:
 * - PMU SMBPBI mailbox will provide a query to expose this flag. So presumably a BMC queries
 *   this flag and resets GPU after all workload running on the GPU has completed.
 * - Ampere Error containment XID prints also exposes this state
 *
 * @param[in] pGpu  OBJGPU pointer
 *
 * @return NV_OK if successful
 */
NV_STATUS
gpuMarkDeviceForDrainAndReset_IMPL
(
    OBJGPU *pGpu
)
{
    return _gpuSetDrainAndResetState(pGpu, NV_TRUE);
}

/*!
 * @brief Interface which allows GPU to be unmarked for pending drain and reset.
 *
 * This should be used only for debug or testing through
 * NV2080_CTRL_CMD_GPU_UNMARK_DEVICE_FOR_DRAIN_AND_RESET and never called directly.
 *
 * How is this state exposed:
 * - PMU SMBPBI mailbox will provide a query to expose this flag. So presumably a BMC queries
 *   this flag and resets GPU after all workload running on the GPU has completed.
 * - Ampere Error containment XID prints also exposes this state
 *
 * @param[in] pGpu  OBJGPU pointer
 *
 * @return NV_OK if successful
 */
NV_STATUS
gpuUnmarkDeviceForDrainAndReset_IMPL
(
    OBJGPU *pGpu
)
{
    return _gpuSetDrainAndResetState(pGpu, NV_FALSE);
}

/*!
 * @brief Interface which returns if a GPU drain and reset is recommended.
 *
 * @param[in]  pGpu                 OBJGPU pointer
 * @param[out] pbDrainRecommended   NvBool pointer indicating if the GPU needs a
 *                                  drain and reset
 *
 * @return NV_OK if successful
 */
NV_STATUS
gpuIsDeviceMarkedForDrainAndReset_IMPL
(
    OBJGPU *pGpu,
    NvBool *pbDrainRecommended
)
{
    return gpuGetDrainAndResetScratchBit_HAL(pGpu, pbDrainRecommended);
}

const char*
_gpuRecoveryActionName
(
    NV2080_CTRL_GPU_RECOVERY_ACTION action
)
{
    switch(action)
    {
        case NV2080_CTRL_GPU_RECOVERY_ACTION_NONE:
            return "None";
        case NV2080_CTRL_GPU_RECOVERY_ACTION_GPU_RESET:
            return "GPU Reset Required";
        case NV2080_CTRL_GPU_RECOVERY_ACTION_NODE_REBOOT:
            return "Node Reboot Required";
        case NV2080_CTRL_GPU_RECOVERY_ACTION_DRAIN_P2P:
            return "Drain P2P";
        case NV2080_CTRL_GPU_RECOVERY_ACTION_DRAIN_AND_RESET:
            return "Drain and Reset";
        default:
            NV_ASSERT_FAILED("Unknown recovery action!");
            return "Unknown";
    }
}

/*!
 * @brief This function queries the action required to recover from a GPU fault.
 *
 * @param[In]   pGpu     The GPU to query
 * @param[Out]  pParams  The structure to store the recovery action
 *
 * @return      None.
 */
void
gpuGetRecoveryAction_IMPL
(
    OBJGPU                                      *pGpu,
    NV2080_CTRL_GPU_GET_RECOVERY_ACTION_PARAMS  *pParams
)
{
    NV_PRINTF(LEVEL_INFO,
              "GetRecoveryAction: 0x%x (%s)\n",
              pGpu->currentRecoveryAction,
              _gpuRecoveryActionName(pGpu->currentRecoveryAction));

    pParams->action = pGpu->currentRecoveryAction;
}

/*!
 * @brief This function refreshes the GPU fault recovery action, accounting for
 * recent changes. This function runs in a work item with GPUS_LOCK_ALL already
 * acquired.
 *
 * @param[In]   pGpu     The GPU object
 *
 * @return      None.
 */
static void
_gpuRefreshRecoveryActionInLock
(
    NvU32  gpuInstance,
    void  *pParams
)
{
    OBJSYS                          *pSys = SYS_GET_INSTANCE();
    OBJGPU                          *pGpu = gpumgrGetGpu(gpuInstance);
    NV_STATUS                        rmStatus;
    NvBool                           bResetRequired;
    NvBool                           bDrainAndReset;
    NV2080_CTRL_GPU_RECOVERY_ACTION  newAction;
    NV2080_CTRL_GPU_RECOVERY_ACTION  oldAction;
    
    if (pGpu == NULL)
    {
        // Call-back is too late. pGpu is already NULL
        return;
    }

    // Decide the new recovery action
    if (pSys->getProperty(pSys, PDB_PROP_SYS_RECOVERY_REBOOT_REQUIRED)
        || pGpu->getProperty(pGpu, PDB_PROP_GPU_RECOVERY_REBOOT_REQUIRED))
    {
        newAction = NV2080_CTRL_GPU_RECOVERY_ACTION_NODE_REBOOT;
    }
    else
    {
        rmStatus = gpuIsDeviceMarkedForReset(pGpu, &bResetRequired);
        if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED) ||
            ((rmStatus == NV_OK) && bResetRequired))
        {
            newAction = NV2080_CTRL_GPU_RECOVERY_ACTION_GPU_RESET;
        }
        else
        {
            rmStatus = gpuIsDeviceMarkedForDrainAndReset(pGpu, &bDrainAndReset);
            if ((rmStatus == NV_OK) && bDrainAndReset)
            {
                newAction = NV2080_CTRL_GPU_RECOVERY_ACTION_DRAIN_AND_RESET;
            }
            else if (pGpu->getProperty(pGpu, PDB_PROP_GPU_RECOVERY_DRAIN_P2P_REQUIRED))
            {
                newAction = NV2080_CTRL_GPU_RECOVERY_ACTION_DRAIN_P2P;
            }
            else
            {
                newAction = NV2080_CTRL_GPU_RECOVERY_ACTION_NONE;
            }
        }
    }

    if (newAction != pGpu->currentRecoveryAction)
    {
        oldAction = pGpu->currentRecoveryAction;
        pGpu->currentRecoveryAction = newAction;

        if (oldAction != GPU_RECOVERY_ACTION_UNKNOWN)
        {
            // Trigger NVML event for the new recovery action.
            gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_GPU_RECOVERY_ACTION, NULL, 0, 0, newAction);

            // Log XID 154 to indicate new recovery action.
            nvErrorLog_va(pGpu, GPU_RECOVERY_ACTION_CHANGED, "GPU recovery action changed from 0x%x (%s) to 0x%x (%s)",
                oldAction, _gpuRecoveryActionName(oldAction), newAction, _gpuRecoveryActionName(newAction));
        }
    }
}

/*!
 * @brief This function refreshes the GPU fault recovery action, accounting for
 * recent changes.
 *
 * @param[In]   pGpu    The GPU object
 * @param[In]   inLock  Specifies whether the calling thread has already
 *                      acquired all GPU locks.
 *
 * @return      None.
 */
void
gpuRefreshRecoveryAction_KERNEL
(
    OBJGPU *pGpu,
    NvBool  inLock
)
{
    if (!inLock)
    {
        //
        // Schedule a workitem to acquire GPUS_LOCK_ALL and perform the refresh
        // as the current thread could be in any IRQL / lock context.
        //
        NV_ASSERT_OK(osQueueWorkItemWithFlags(pGpu,
                                              _gpuRefreshRecoveryActionInLock,
                                              NULL,
                                              OS_QUEUE_WORKITEM_FLAGS_LOCK_GPUS));
    }
    else
    {
        // Lock requirement is already satisfied, perform the refresh directly.
        _gpuRefreshRecoveryActionInLock(pGpu->gpuInstance, NULL);
    }
}

/*!
 * @brief This function specifies whether the GPU needs all its P2P traffic
 * drained before it can be used.
 *
 * @param[In]   pGpu      The GPU object
 * @param[In]   bDrainP2P Specifies whether the GPU needs P2P draining
 *
 * @return      None.
 */
void
gpuSetRecoveryDrainP2P_KERNEL
(
    OBJGPU *pGpu,
    NvBool  bDrainP2P
)
{
    if (!!pGpu->getProperty(pGpu, PDB_PROP_GPU_RECOVERY_DRAIN_P2P_REQUIRED) != !!bDrainP2P)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_DRAIN_P2P_REQUIRED, bDrainP2P);
        gpuRefreshRecoveryAction_KERNEL(pGpu, NV_FALSE);
    }
}
/*!
 * @brief Set partition error attribution
 *
 * @param[in] pGpu         OBJGPU pointer
 * @param[in] errorCode    Error Containment error code
 * @param[in] loc          Location, SubLocation information
 * @param[in] rcErrorCode  RC error code
 *
 * @returns NV_OK
 *          NV_ERR_INVALID_ARGUMENT if errorCode is invalid or
            if partition attribution isn't supported for the error
 */
NV_STATUS
gpuSetPartitionErrorAttribution_KERNEL
(
    OBJGPU                *pGpu,
    NV_ERROR_CONT_ERR_ID   errorCode,
    NV_ERROR_CONT_LOCATION loc,
    NvU32                  rcErrorCode
)
{
    switch (errorCode)
    {
        case NV_ERROR_CONT_ERR_ID_E24_GSP_POISON:
        {
            NV_PRINTF(LEVEL_ERROR, "MIG_INSTANCE_REF determining is not supported for error ID 0x%x.\n", errorCode);
            NV_ASSERT(0);
            return NV_ERR_INVALID_ARGUMENT;
        }
        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Invalid error ID: 0x%x\n", errorCode);
            NV_ASSERT(0);
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    return NV_OK;
}

/*!
 * @brief This function logs an XID message to OOB by sending an RPC message to the GSP.
 *
 * @param[in] pGpu         OBJGPU pointer
 * @param[in] xid          The XID number
 * @param[in] message      The text message associated with the XID
 * @param[in] len          Length, in bytes, of the text message, excluding the null terminator
 *
 * @returns NV_OK
 *          NV_ERR_INVALID_ARGUMENT if errorCode is invalid or
            if partition attribution isn't supported for the error
 */
void
gpuLogOobXidMessage_KERNEL
(
    OBJGPU      *pGpu,
    NvU32        xid,
    const char  *message,
    NvU32        len
)
{
    RM_API                                 *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS                               status;
    NvBool                                  bGspFatalError = NV_FALSE;
    NV2080_CTRL_INTERNAL_LOG_OOB_XID_PARAMS params = {0};
    KernelGsp                              *pKernelGsp = GPU_GET_KERNEL_GSP(pGpu);

    bGspFatalError = pKernelGsp->bFatalError;

    // Exclude conditions that indicate issues with GSP communication.
    if ((xid == GSP_ERROR) ||
        (xid == GSP_RPC_TIMEOUT) ||
        (xid == ROBUST_CHANNEL_GPU_HAS_FALLEN_OFF_THE_BUS) ||
        API_GPU_IN_RESET_SANITY_CHECK(pGpu) ||
        !pGpu->gspRmInitialized ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_PREPARING_FULLCHIP_RESET) ||
        pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST) ||
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_CONNECTED) ||
        bGspFatalError)
    {
        return;
    }

    // Copy the message into the RPC params, truncated to max RPC size, but
    // always include a tailing NULL terminator.
    len = NV_MIN(len, NV2080_INTERNAL_OOB_XID_MESSAGE_BUFFER_SIZE - 1);
    params.message[len] = '\0';
    params.xid = xid;
    params.len = len;
    portMemCopy(params.message, NV2080_INTERNAL_OOB_XID_MESSAGE_BUFFER_SIZE, message, len);

    NV_CHECK_OK(status, LEVEL_ERROR,
                pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_LOG_OOB_XID,
                                &params,
                                sizeof(params)));

}

NvBool
gpuValidateMIGSupport_KERNEL
(
    OBJGPU *pGpu
)
{
    GspStaticConfigInfo *pGSCI = GPU_GET_GSP_STATIC_INFO(pGpu);

    NV_ASSERT_OR_RETURN(pGSCI != NULL, NV_FALSE);

    return pGSCI->bIsMigSupported;
}

void
gpuSetRecoveryRebootRequired_IMPL
(
    OBJGPU  *pGpu,
    NvBool   bRebootRequired,
    NvBool   bBlockNewWorkload
)
{
    if (!!pGpu->getProperty(pGpu, PDB_PROP_GPU_RECOVERY_REBOOT_REQUIRED) != !!bRebootRequired)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_RECOVERY_REBOOT_REQUIRED, bRebootRequired);

        gpuRefreshRecoveryAction_KERNEL(pGpu, NV_FALSE);
    }

    pGpu->bBlockNewWorkload = bBlockNewWorkload;
}
