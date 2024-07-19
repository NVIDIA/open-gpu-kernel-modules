
#ifndef _G_GPU_MGR_NVOC_H_
#define _G_GPU_MGR_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#pragma once
#include "g_gpu_mgr_nvoc.h"


#ifndef _GPUMGR_H_
#define _GPUMGR_H_

//
// GPU Manager Defines and Structures
//

struct OBJGPU;
#include "core/core.h"
#include "core/system.h"
#include "nvlimits.h"
#include "gpu_mgr/gpu_group.h"
#include "gpu/gpu_uuid.h"
#include "gpu/gpu_device_mapping.h"
#include "gpu/gpu_access.h"
#include "ctrl/ctrl0000/ctrl0000gpu.h"
#include "ctrl/ctrl2080/ctrl2080ce.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "ctrl/ctrlc637.h"
#include "nvoc/utility.h"

#include "gpu_mgr/gpu_mgr_sli.h"

#include "gpu/perf/kern_perf_gpuboostsync.h"

#include "utils/nvbitvector.h"
TYPEDEF_BITVECTOR(MC_ENGINE_BITVECTOR);

#define GPUMGR_MAX_GPU_INSTANCES        8
#define GPUMGR_MAX_COMPUTE_INSTANCES    8

//
// Terminology:
//    GPU         -> entity sitting on the bus
//    Device      -> broadcast semantics; maps to one or more GPUs
//    Subdevice   -> unicast semantics; maps to a single GPU
//


////////////////////////////////////////////////////////////////////////////////
//                         DO NOT ADD NEW STUBS HERE                          //
////////////////////////////////////////////////////////////////////////////////
#define gpumgrGetGpuLinkCount(deviceInstance)                   ((NvU32) 0)
#define gpumgrGetSliLinkOutputMaskFromGpu(pGpu)                 ((NvU32) 0)
#define gpumgrGetVidLinkOutputMaskFromGpu(pGpu)                 ((NvU32) 0)
#define gpumgrGetSliLinkOrderCount(pGpu)                        ((NvU32) 0)
#define gpumgrGetSliLinkConnectionCount(pGpu)                   ((NvU32) 0)
#define gpumgrGetSLIConfig(gpuInstance, onlyWithSliLink)        ((NvU32) 0)
#define gpumgrDisableVidLink(pGpu, head, max_dr_port)
#define gpumgrGetBcEnabledStatus(g)                             (NV_FALSE)
#define gpumgrGetBcEnabledStatusEx(g, t)                        (NV_FALSE)
#define gpumgrSetBcEnabledStatus(g, b)                          do { NvBool b2 = b; (void)b2; } while (0)
#define gpumgrSLILoopReentrancy(pGpu, l, r, i, pFuncStr)
#define gpumgrSLILoopReentrancyPop(pGpu)                        ((NvU32)0)
#define gpumgrSLILoopReentrancyPush(pGpu, sliLoopReentrancy)    do { NvU32 x = sliLoopReentrancy; (void)x; } while(0)

typedef struct
{
    NvU32     gpuId;
    NvU64     gpuDomainBusDevice;
    NvBool    bInitAttempted;
    NvBool    bDrainState;  // no new client connections to this GPU
    NvBool    bRemoveIdle;  // remove this GPU once it's idle (detached)
    NvBool    bExcluded;    // this gpu is marked as excluded; do not use
    NvBool    bUuidValid;   // cached uuid is valid
    NvBool    bSkipHwNvlinkDisable; //skip HW registers configuration for disabled links
    NvU32     initDisabledNvlinksMask;
    NV_STATUS initStatus;
    NvU8      uuid[RM_SHA1_GID_SIZE];
    OS_RM_CAPS *pOsRmCaps;    // "Opaque" pointer to os-specific capabilities
} PROBEDGPU;

#define NV_DEVICE_DISPLAY_FLAGS_AFR_FRAME_FLIPS                   11:4
#define NV_DEVICE_DISPLAY_FLAGS_AFR_FRAME_TIME                   12:12
#define NV_DEVICE_DISPLAY_FLAGS_AFR_FRAME_TIME_INVALID       0x0000000
#define NV_DEVICE_DISPLAY_FLAGS_AFR_FRAME_TIME_VALID         0x0000001

/*!
 * Structure for tracking resources allocated for saving primary GPU's VBIOS
 * state.  This is used for TDR/fullchip reset recovery.  The GPU object gets
 * destroyed, so the data belongs here.
 */
typedef struct _def_gpumgr_save_vbios_state
{
    RmPhysAddr vgaWorkspaceVidMemBase;        //<! Base address of the VGA workspace
    struct MEMORY_DESCRIPTOR *pSaveToMemDesc; //<! Where VGA workspace is saved to
    void *pSaveRegsOpaque;                    //<! Saved values of VGA registers
} GPUMGRSAVEVBIOSSTATE, *PGPUMGRSAVEVBIOSSTATE;

typedef struct CONF_COMPUTE_CAPS
{
    NvBool bApmFeatureCapable;
    NvBool bHccFeatureCapable;
    NvBool bCCFeatureEnabled;
    NvBool bDevToolsModeEnabled;
    NvBool bAcceptClientRequest;
    NvBool bMultiGpuProtectedPcieModeEnabled;
    NvBool bFatalFailure;
} CONF_COMPUTE_CAPS;

//
// types of bridges supported.
// These defines are inices for the types of bridges supported.
// preference for a given bridge type is determined by the lower value index.
// I.E. Video Link has the lower value index, so in the event that both NvLink & video link is
// detected, the video link will be used.
//
#define SLI_MAX_BRIDGE_TYPES    2
#define SLI_BT_VIDLINK          0
#define SLI_BT_NVLINK           1

//
// GPU NVLINK reduced bandwidth mode
//
#define GPU_NVLINK_BW_MODE_FULL     (0x0)
#define GPU_NVLINK_BW_MODE_OFF      (0x1)
#define GPU_NVLINK_BW_MODE_MIN      (0x2)
#define GPU_NVLINK_BW_MODE_HALF     (0x3)
#define GPU_NVLINK_BW_MODE_3QUARTER (0x4)

typedef struct NVLINK_TOPOLOGY_PARAMS
{
    NvU32   sysmemLinks;
    NvU32   maxLinksPerPeer;
    NvBool  bSymmetric;
    // Pascal only
    NvU32   numLinks;
    // Volta +
    NvU32   numPeers;
    NvBool  bSwitchConfig;
    // Ampere +
    NvU32   pceAvailableMaskPerConnectingHub[NV2080_CTRL_CE_MAX_HSHUBS];
    NvU32   fbhubPceMask;
    NvU32   maxPceLceMap[NV2080_CTRL_MAX_PCES];
    NvU32   maxGrceConfig[NV2080_CTRL_MAX_GRCES];
    NvU32   maxExposeCeMask;
    NvU32   maxTopoIdx;       // For table configs only; not applicable for algorithm
} NVLINK_TOPOLOGY_PARAMS, *PNVLINK_TOPOLOGY_PARAMS;

typedef struct _def_gpu_nvlink_topology_info
{
    NvBool  valid;
    NvU64   DomainBusDevice;
    NVLINK_TOPOLOGY_PARAMS params;
} NVLINK_TOPOLOGY_INFO, *PNVLINK_TOPOLOGY_INFO;

typedef struct
{
    OBJGPU     *pGpu;
    NvU32       gpuInstance;
} GPU_HANDLE_ID;

/*!
 * @brief Persistent compute instance storage
 *        Stores information required to recreate a compute instance which
 *        was created on an attached GPU, which was then detached.
 */
typedef struct GPUMGR_SAVE_COMPUTE_INSTANCE
{
    // NV_TRUE if a compute instance with the below resources should be restored
    NvBool bValid;
    // Exported CPU instance information for persistence
    NVC637_CTRL_EXEC_PARTITIONS_EXPORTED_INFO ciInfo;
    // ID of compute instance
    NvU32 id;
    // "Opaque" pointer to os-specific capabilities
    OS_RM_CAPS *pOsRmCaps;
} GPUMGR_SAVE_COMPUTE_INSTANCE;

/*!
 * @brief Persistent GPU instance storage
 *        Stores information required to recreate a GPU instance which
 *        was created on an attached GPU, which was then detached.
 */
typedef struct GPUMGR_SAVE_GPU_INSTANCE
{
    // NV_TRUE if a GPU instance with the below resources should be restored
    NvBool bValid;
    // Exported GPU instance information for persistence
    NV2080_CTRL_INTERNAL_KMIGMGR_EXPORTED_GPU_INSTANCE_INFO giInfo;
    // SwizzId of the saved GPU instance to restore to
    NvU32 swizzId;
    // "Opaque" pointer to os-specific capabilities
    OS_RM_CAPS *pOsRmCaps;
    // Saved compute instance information. Mar or may not have valid entries.
    GPUMGR_SAVE_COMPUTE_INSTANCE saveCI[GPUMGR_MAX_COMPUTE_INSTANCES];
} GPUMGR_SAVE_GPU_INSTANCE;

/*!
 * @brief Persistent MIG instance topology storage
 *        Stores information required to recreate all MIG instances which
 *        were created on an attached GPU, which was then detached.
 */
typedef struct GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY
{
    // NV_TRUE if a GPU was attached with associated DBDF.
    NvBool bValid;
    // The PCI Domain/Bus/Device/Function of the GPU for which this struct was saved.
    NvU64 domainBusDevice;
    // Flag checking whether we have restored from static info since boot
    NvBool bVgpuRestoredFromStaticInfo;
    // MIG repartitioning mode last registered for the GPU this struct was saved for.
    NvBool bMIGEnabled;
    // Saved instance information. May or may not have any valid entries.
    GPUMGR_SAVE_GPU_INSTANCE saveGI[GPUMGR_MAX_GPU_INSTANCES];
} GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY;

typedef struct GPUMGR_CACHED_MIG_GPU_INSTANCE
{
    NvBool bValid;
    NvU32 swizzId;
    NvBool bValidComputeInstances[GPUMGR_MAX_COMPUTE_INSTANCES];
} GPUMGR_CACHED_MIG_GPU_INSTANCE;

typedef struct GPUMGR_CACHED_MIG_STATE
{
    NvBool bValid;
    NvBool bMIGEnabled;
    NvU32 gpuId;
    GPUMGR_CACHED_MIG_GPU_INSTANCE gpuInstances[GPUMGR_MAX_GPU_INSTANCES];
} GPUMGR_CACHED_MIG_STATE;


#include "containers/list.h"
typedef struct PCIEP2PCAPSINFO
{
    NvU32    gpuId[GPUMGR_MAX_GPU_INSTANCES]; // Group of GPUs
    NvU32    gpuCount;                        // GPU count in gpuId[]
    NvU8     p2pWriteCapsStatus;              // PCIE P2P CAPS status for this group of GPUs
    NvU8     p2pReadCapsStatus;
    ListNode node;                            // For intrusive lists
} PCIEP2PCAPSINFO;
MAKE_INTRUSIVE_LIST(pcieP2PCapsInfoList, PCIEP2PCAPSINFO, node);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct OBJGPUMGR {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJGPUMGR *__nvoc_pbase_OBJGPUMGR;    // gpumgr

    // Data members
    PROBEDGPU probedGpus[32];
    void *probedGpusLock;
    NvU32 gpuAttachCount;
    NvU32 gpuAttachMask;
    NvU32 gpuMonolithicRmMask;
    NvU32 persistentSwStateGpuMask;
    NvU32 deviceCount;
    struct OBJGPUGRP *pGpuGrpTable[32];
    NvU32 gpuInstMaskTable[32];
    NvU8 gpuBridgeType;
    NvU8 gpuSliLinkRoute[2][32][32][2];
    SLI_GPU_BOOST_SYNC sliGpuBoostSync;
    GPUMGRSAVEVBIOSSTATE primaryVbiosState;
    NvU8 powerDisconnectedGpuCount;
    NvU8 powerDisconnectedGpuBus[32];
    NVLINK_TOPOLOGY_INFO nvlinkTopologyInfo[32];
    NvU8 nvlinkBwMode;
    GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY MIGTopologyInfo[32];
    void *cachedMIGInfoLock;
    GPUMGR_CACHED_MIG_STATE cachedMIGInfo[32];
    GPU_HANDLE_ID gpuHandleIDList[32];
    NvU32 numGpuHandles;
    CONF_COMPUTE_CAPS ccCaps;
    NvU64 ccAttackerAdvantage;
    pcieP2PCapsInfoList pcieP2PCapsInfoCache;
    void *pcieP2PCapsInfoLock;
};

#ifndef __NVOC_CLASS_OBJGPUMGR_TYPEDEF__
#define __NVOC_CLASS_OBJGPUMGR_TYPEDEF__
typedef struct OBJGPUMGR OBJGPUMGR;
#endif /* __NVOC_CLASS_OBJGPUMGR_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPUMGR
#define __nvoc_class_id_OBJGPUMGR 0xcf1b25
#endif /* __nvoc_class_id_OBJGPUMGR */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGPUMGR;

#define __staticCast_OBJGPUMGR(pThis) \
    ((pThis)->__nvoc_pbase_OBJGPUMGR)

#ifdef __nvoc_gpu_mgr_h_disabled
#define __dynamicCast_OBJGPUMGR(pThis) ((OBJGPUMGR*)NULL)
#else //__nvoc_gpu_mgr_h_disabled
#define __dynamicCast_OBJGPUMGR(pThis) \
    ((OBJGPUMGR*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJGPUMGR)))
#endif //__nvoc_gpu_mgr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJGPUMGR(OBJGPUMGR**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJGPUMGR(OBJGPUMGR**, Dynamic*, NvU32);
#define __objCreate_OBJGPUMGR(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJGPUMGR((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS gpumgrInitPcieP2PCapsCache_IMPL(struct OBJGPUMGR *pGpuMgr);


#define gpumgrInitPcieP2PCapsCache(pGpuMgr) gpumgrInitPcieP2PCapsCache_IMPL(pGpuMgr)
#define gpumgrInitPcieP2PCapsCache_HAL(pGpuMgr) gpumgrInitPcieP2PCapsCache(pGpuMgr)

void gpumgrDestroyPcieP2PCapsCache_IMPL(struct OBJGPUMGR *pGpuMgr);


#define gpumgrDestroyPcieP2PCapsCache(pGpuMgr) gpumgrDestroyPcieP2PCapsCache_IMPL(pGpuMgr)
#define gpumgrDestroyPcieP2PCapsCache_HAL(pGpuMgr) gpumgrDestroyPcieP2PCapsCache(pGpuMgr)

NV_STATUS gpumgrStorePcieP2PCapsCache_IMPL(NvU32 gpuMask, NvU8 p2pWriteCapStatus, NvU8 p2pReadCapStatus);


#define gpumgrStorePcieP2PCapsCache(gpuMask, p2pWriteCapStatus, p2pReadCapStatus) gpumgrStorePcieP2PCapsCache_IMPL(gpuMask, p2pWriteCapStatus, p2pReadCapStatus)
#define gpumgrStorePcieP2PCapsCache_HAL(gpuMask, p2pWriteCapStatus, p2pReadCapStatus) gpumgrStorePcieP2PCapsCache(gpuMask, p2pWriteCapStatus, p2pReadCapStatus)

void gpumgrRemovePcieP2PCapsFromCache_IMPL(NvU32 gpuId);


#define gpumgrRemovePcieP2PCapsFromCache(gpuId) gpumgrRemovePcieP2PCapsFromCache_IMPL(gpuId)
#define gpumgrRemovePcieP2PCapsFromCache_HAL(gpuId) gpumgrRemovePcieP2PCapsFromCache(gpuId)

NvBool gpumgrGetPcieP2PCapsFromCache_IMPL(NvU32 gpuMask, NvU8 *pP2PWriteCapStatus, NvU8 *pP2PReadCapStatus);


#define gpumgrGetPcieP2PCapsFromCache(gpuMask, pP2PWriteCapStatus, pP2PReadCapStatus) gpumgrGetPcieP2PCapsFromCache_IMPL(gpuMask, pP2PWriteCapStatus, pP2PReadCapStatus)
#define gpumgrGetPcieP2PCapsFromCache_HAL(gpuMask, pP2PWriteCapStatus, pP2PReadCapStatus) gpumgrGetPcieP2PCapsFromCache(gpuMask, pP2PWriteCapStatus, pP2PReadCapStatus)

NV_STATUS gpumgrConstruct_IMPL(struct OBJGPUMGR *arg_);

#define __nvoc_gpumgrConstruct(arg_) gpumgrConstruct_IMPL(arg_)
void gpumgrDestruct_IMPL(struct OBJGPUMGR *arg1);

#define __nvoc_gpumgrDestruct(arg1) gpumgrDestruct_IMPL(arg1)
void gpumgrAddSystemNvlinkTopo_IMPL(NvU64 DomainBusDevice);

#define gpumgrAddSystemNvlinkTopo(DomainBusDevice) gpumgrAddSystemNvlinkTopo_IMPL(DomainBusDevice)
NvBool gpumgrGetSystemNvlinkTopo_IMPL(NvU64 DomainBusDevice, struct NVLINK_TOPOLOGY_PARAMS *pTopoParams);

#define gpumgrGetSystemNvlinkTopo(DomainBusDevice, pTopoParams) gpumgrGetSystemNvlinkTopo_IMPL(DomainBusDevice, pTopoParams)
void gpumgrUpdateSystemNvlinkTopo_IMPL(NvU64 DomainBusDevice, struct NVLINK_TOPOLOGY_PARAMS *pTopoParams);

#define gpumgrUpdateSystemNvlinkTopo(DomainBusDevice, pTopoParams) gpumgrUpdateSystemNvlinkTopo_IMPL(DomainBusDevice, pTopoParams)
NV_STATUS gpumgrSetGpuInitDisabledNvlinks_IMPL(NvU32 gpuId, NvU32 mask, NvBool bSkipHwNvlinkDisable);

#define gpumgrSetGpuInitDisabledNvlinks(gpuId, mask, bSkipHwNvlinkDisable) gpumgrSetGpuInitDisabledNvlinks_IMPL(gpuId, mask, bSkipHwNvlinkDisable)
NV_STATUS gpumgrGetGpuInitDisabledNvlinks_IMPL(NvU32 gpuId, NvU32 *pMask, NvBool *pbSkipHwNvlinkDisable);

#define gpumgrGetGpuInitDisabledNvlinks(gpuId, pMask, pbSkipHwNvlinkDisable) gpumgrGetGpuInitDisabledNvlinks_IMPL(gpuId, pMask, pbSkipHwNvlinkDisable)
NvU8 gpumgrGetGpuNvlinkBwMode_IMPL(void);

#define gpumgrGetGpuNvlinkBwMode() gpumgrGetGpuNvlinkBwMode_IMPL()
void gpumgrSetGpuNvlinkBwModeFromRegistry_IMPL(struct OBJGPU *pGpu);

#define gpumgrSetGpuNvlinkBwModeFromRegistry(pGpu) gpumgrSetGpuNvlinkBwModeFromRegistry_IMPL(pGpu)
NV_STATUS gpumgrSetGpuNvlinkBwMode_IMPL(NvU8 mode);

#define gpumgrSetGpuNvlinkBwMode(mode) gpumgrSetGpuNvlinkBwMode_IMPL(mode)
NvBool gpumgrCheckIndirectPeer_IMPL(struct OBJGPU *pGpu, struct OBJGPU *pRemoteGpu);

#define gpumgrCheckIndirectPeer(pGpu, pRemoteGpu) gpumgrCheckIndirectPeer_IMPL(pGpu, pRemoteGpu)
void gpumgrAddSystemMIGInstanceTopo_IMPL(NvU64 domainBusDevice);

#define gpumgrAddSystemMIGInstanceTopo(domainBusDevice) gpumgrAddSystemMIGInstanceTopo_IMPL(domainBusDevice)
NvBool gpumgrGetSystemMIGInstanceTopo_IMPL(NvU64 domainBusDevice, struct GPUMGR_SAVE_MIG_INSTANCE_TOPOLOGY **ppTopoParams);

#define gpumgrGetSystemMIGInstanceTopo(domainBusDevice, ppTopoParams) gpumgrGetSystemMIGInstanceTopo_IMPL(domainBusDevice, ppTopoParams)
NvBool gpumgrIsSystemMIGEnabled_IMPL(NvU64 domainBusDevice);

#define gpumgrIsSystemMIGEnabled(domainBusDevice) gpumgrIsSystemMIGEnabled_IMPL(domainBusDevice)
void gpumgrSetSystemMIGEnabled_IMPL(NvU64 domainBusDevice, NvBool bMIGEnabled);

#define gpumgrSetSystemMIGEnabled(domainBusDevice, bMIGEnabled) gpumgrSetSystemMIGEnabled_IMPL(domainBusDevice, bMIGEnabled)
void gpumgrUnregisterRmCapsForMIGGI_IMPL(NvU64 gpuDomainBusDevice);

#define gpumgrUnregisterRmCapsForMIGGI(gpuDomainBusDevice) gpumgrUnregisterRmCapsForMIGGI_IMPL(gpuDomainBusDevice)
void gpumgrCacheCreateGpuInstance_IMPL(struct OBJGPU *pGpu, NvU32 swizzId);

#define gpumgrCacheCreateGpuInstance(pGpu, swizzId) gpumgrCacheCreateGpuInstance_IMPL(pGpu, swizzId)
void gpumgrCacheDestroyGpuInstance_IMPL(struct OBJGPU *pGpu, NvU32 swizzId);

#define gpumgrCacheDestroyGpuInstance(pGpu, swizzId) gpumgrCacheDestroyGpuInstance_IMPL(pGpu, swizzId)
void gpumgrCacheCreateComputeInstance_IMPL(struct OBJGPU *pGpu, NvU32 swizzId, NvU32 ciId);

#define gpumgrCacheCreateComputeInstance(pGpu, swizzId, ciId) gpumgrCacheCreateComputeInstance_IMPL(pGpu, swizzId, ciId)
void gpumgrCacheDestroyComputeInstance_IMPL(struct OBJGPU *pGpu, NvU32 swizzId, NvU32 ciId);

#define gpumgrCacheDestroyComputeInstance(pGpu, swizzId, ciId) gpumgrCacheDestroyComputeInstance_IMPL(pGpu, swizzId, ciId)
void gpumgrCacheSetMIGEnabled_IMPL(struct OBJGPU *pGpu, NvBool bMIGEnabled);

#define gpumgrCacheSetMIGEnabled(pGpu, bMIGEnabled) gpumgrCacheSetMIGEnabled_IMPL(pGpu, bMIGEnabled)
NV_STATUS gpumgrCacheGetActiveDeviceIds_IMPL(NV0000_CTRL_GPU_GET_ACTIVE_DEVICE_IDS_PARAMS *pActiveDeviceIdsParams);

#define gpumgrCacheGetActiveDeviceIds(pActiveDeviceIdsParams) gpumgrCacheGetActiveDeviceIds_IMPL(pActiveDeviceIdsParams)
void gpumgrUpdateBoardId_IMPL(struct OBJGPU *arg1);

#define gpumgrUpdateBoardId(arg1) gpumgrUpdateBoardId_IMPL(arg1)
void gpumgrServiceInterrupts_IMPL(NvU32 arg1, MC_ENGINE_BITVECTOR *arg2, NvBool arg3);

#define gpumgrServiceInterrupts(arg1, arg2, arg3) gpumgrServiceInterrupts_IMPL(arg1, arg2, arg3)
#undef PRIVATE_FIELD


typedef struct {
    NvBool         specified;                           // Set this flag when using this struct
    NvBool         bIsIGPU;                             // Set this flag for iGPU

    DEVICE_MAPPING deviceMapping[DEVICE_INDEX_MAX];  // Register Aperture mapping
    NvU32          socChipId0;                          // Chip ID used for HAL binding
    NvU32          iovaspaceId;                         // SMMU client ID
} SOCGPUATTACHARG;

//
// Packages up system/bus state for attach process.
//
typedef struct GPUATTACHARG
{
    GPUHWREG   *regBaseAddr;
    GPUHWREG   *fbBaseAddr;
    GPUHWREG   *instBaseAddr;
    RmPhysAddr  devPhysAddr;
    RmPhysAddr  fbPhysAddr;
    RmPhysAddr  instPhysAddr;
    RmPhysAddr  ioPhysAddr;
    NvU64       nvDomainBusDeviceFunc;
    NvU32       regLength;
    NvU64       fbLength;
    NvU32       instLength;
    NvU32       intLine;
    void        *pOsAttachArg;
    NvBool      bIsSOC;
    NvU32       socDeviceCount;
    DEVICE_MAPPING socDeviceMappings[GPU_MAX_DEVICE_MAPPINGS];
    NvU32       socId;
    NvU32       socSubId;
    NvU32       socChipId0;
    NvU32       iovaspaceId;
    NvBool      bRequestFwClientRm;
    NvS32       cpuNumaNodeId;

    //
    // The SOC-specific fields above are legacy fields that were added for
    // ARCH MODS iGPU verification. There is a plan to deprecate these fields as
    // part of an effort to clean up the existing iGPU code in RM.
    //
    // Starting with T234D+, the SOCGPUATTACHARG field below will be used to
    // pass the required attach info for a single SOC device from the RM OS
    // layer to core RM.
    //
    SOCGPUATTACHARG socDeviceArgs;
} GPUATTACHARG;

NV_STATUS   gpumgrThreadEnableExpandedGpuVisibility(void);
void        gpumgrThreadDisableExpandedGpuVisibility(void);
NvBool      gpumgrThreadHasExpandedGpuVisibility(void);

NV_STATUS   gpumgrGetGpuAttachInfo(NvU32 *pGpuCnt, NvU32 *pGpuMask);
NV_STATUS   gpumgrGetProbedGpuIds(NV0000_CTRL_GPU_GET_PROBED_IDS_PARAMS *);
NV_STATUS   gpumgrGetProbedGpuDomainBusDevice(NvU32 gpuId, NvU64 *gpuDomainBusDevice);
NV_STATUS   gpumgrGetAttachedGpuIds(NV0000_CTRL_GPU_GET_ATTACHED_IDS_PARAMS *);
NV_STATUS   gpumgrGetGpuIdInfo(NV0000_CTRL_GPU_GET_ID_INFO_PARAMS *);
NV_STATUS   gpumgrGetGpuIdInfoV2(NV0000_CTRL_GPU_GET_ID_INFO_V2_PARAMS *);
void        gpumgrSetGpuId(OBJGPU*, NvU32 gpuId);
NV_STATUS   gpumgrGetGpuInitStatus(NV0000_CTRL_GPU_GET_INIT_STATUS_PARAMS *);
void        gpumgrSetGpuInitStatus(NvU32 gpuId, NV_STATUS status);
OBJGPU*     gpumgrGetGpuFromId(NvU32 gpuId);
OBJGPU*     gpumgrGetGpuFromUuid(const NvU8 *pGpuUuid, NvU32 flags);
OBJGPU*     gpumgrGetGpuFromBusInfo(NvU32 domain, NvU8 bus, NvU8 device);
NvU32       gpumgrGetDefaultPrimaryGpu(NvU32 gpuMask);
NV_STATUS   gpumgrAllocGpuInstance(NvU32 *pDeviceInstance);
NV_STATUS   gpumgrRegisterGpuId(NvU32 gpuId, NvU64 gpuDomainBusDevice);
NV_STATUS   gpumgrUnregisterGpuId(NvU32 gpuId);
NV_STATUS   gpumgrExcludeGpuId(NvU32 gpuId);
NV_STATUS   gpumgrSetUuid(NvU32 gpuId, NvU8 *uuid);
NV_STATUS   gpumgrGetGpuUuidInfo(NvU32 gpuId, NvU8 **ppUuidStr, NvU32 *pUuidStrLen, NvU32 uuidFlags);
// gpumgrGetRmFirmwarePolicy() and  gpumgrGetRmFirmwareLogsEnabled() contain
// all logic for deciding the policies for loading firmwares, and so need to be
// compiled for all platforms besides those actually running the firmwares
void        gpumgrGetRmFirmwarePolicy(NvU32 pmcBoot42, NvBool bIsSoc,
                                      NvU32 enableFirmwareRegVal, NvBool *pbRequestFirmware,
                                      NvBool *pbAllowFallbackToMonolithicRm,
                                      NvBool bIsTccOrMcdm);
NvBool      gpumgrGetRmFirmwareLogsEnabled(NvU32 enableFirmwareLogsRegVal);
NvBool      gpumgrIsDeviceRmFirmwareCapable(NvU32 pmcBoot42,
                                            NvBool bIsSoc, NvBool *pbEnableByDefault, NvBool bIsTccOrMcdm);
NvBool      gpumgrIsVgxRmFirmwareCapableChip(NvU32 pmcBoot42);
NV_STATUS   gpumgrAttachGpu(NvU32 deviceInstance, GPUATTACHARG *);
NV_STATUS   gpumgrDetachGpu(NvU32 deviceInstance);
OBJGPU*     gpumgrGetNextGpu(NvU32 gpuMask, NvU32 *pStartIndex);
NV_STATUS   gpumgrStatePreInitGpu(OBJGPU*);
NV_STATUS   gpumgrStateInitGpu(OBJGPU*);
NV_STATUS   gpumgrStateLoadGpu(OBJGPU*, NvU32);
NV_STATUS   gpumgrAllocDeviceInstance(NvU32 *pDeviceInstance);
NV_STATUS   gpumgrCreateDevice(NvU32 *pDeviceInstance, NvU32 gpuMask, NvU32 *pGpuIdsOrdinal);
NV_STATUS   gpumgrDestroyDevice(NvU32 deviceInstance);
NvU32       gpumgrGetDeviceInstanceMask(void);
NvU32       gpumgrGetDeviceGpuMask(NvU32 deviceInstance);
NV_STATUS   gpumgrIsDeviceInstanceValid(NvU32 deviceInstance);
NvU32       gpumgrGetPrimaryForDevice(NvU32 deviceInstance);
NvBool      gpumgrIsSubDeviceInstanceValid(NvU32 subDeviceInstance);
NvBool      gpumgrIsDeviceEnabled(NvU32 deviceInstance);
NvU32       gpumgrGetGpuMask(OBJGPU *pGpu);
OBJGPU*     gpumgrGetGpu(NvU32 deviceInstance);
OBJGPU*     gpumgrGetSomeGpu(void);
NvU32       gpumgrGetSubDeviceCount(NvU32 gpuMask);
NvU32       gpumgrGetSubDeviceCountFromGpu(OBJGPU *pGpu);
NvU32       gpumgrGetSubDeviceMaxValuePlus1(OBJGPU *pGpu);
NvU32       gpumgrGetSubDeviceInstanceFromGpu(OBJGPU *pGpu);
OBJGPU*     gpumgrGetParentGPU(OBJGPU *pGpu);
void        gpumgrSetParentGPU(OBJGPU *pGpu, OBJGPU *pParentGpu);
NvBool      gpumgrIsGpuDisplayParent(OBJGPU*);
OBJGPU*     gpumgrGetDisplayParent(OBJGPU*);
NV_STATUS   gpumgrGetGpuLockAndDrPorts(OBJGPU*, OBJGPU*, NvU32 *, NvU32 *);
NV_STATUS   gpumgrGetBootPrimary(OBJGPU **ppGpu);
OBJGPU*     gpumgrGetMGpu(void);
RmPhysAddr  gpumgrGetGpuPhysFbAddr(OBJGPU*);
OBJGPU*     gpumgrGetGpuFromSubDeviceInst(NvU32, NvU32);
NV_STATUS   gpumgrAddDeviceInstanceToGpus(NvU32 gpuMask);
NV_STATUS   gpumgrRemoveDeviceInstanceFromGpus(NvU32 gpuMask);
NV_STATUS   gpumgrConstructGpuGrpObject(struct OBJGPUMGR *pGpuMgr, NvU32 gpuMask, struct OBJGPUGRP **ppGpuGrp);
struct OBJGPUGRP*  gpumgrGetGpuGrpFromGpu(OBJGPU *pGpu);
struct OBJGPUGRP*  gpumgrGetGpuGrpFromInstance(NvU32 gpugrpInstance);
NV_STATUS   gpumgrModifyGpuDrainState(NvU32 gpuId, NvBool bEnable, NvBool bRemove, NvBool bLinkDisable);
NV_STATUS   gpumgrQueryGpuDrainState(NvU32 gpuId, NvBool *pBEnable, NvBool *pBRemove);
NvBool      gpumgrIsGpuPointerValid(OBJGPU *pGpu);
NvBool      gpumgrIsGpuPointerAttached(OBJGPU *pGpu);
NvU32       gpumgrGetGrpMaskFromGpuInst(NvU32 gpuInst);
void        gpumgrAddDeviceMaskToGpuInstTable(NvU32 gpuMask);
void        gpumgrClearDeviceMaskFromGpuInstTable(NvU32 gpuMask);
NvBool      gpumgrSetGpuAcquire(OBJGPU *pGpu);
void        gpumgrSetGpuRelease(void);
NvU8        gpumgrGetGpuBridgeType(void);
NvBool      gpumgrAreAllGpusInOffloadMode(void);
NvBool      gpumgrIsSafeToReadGpuInfo(void);
NvBool      gpumgrIsDeviceMsixAllowed(RmPhysAddr bar0BaseAddr, NvU32 pmcBoot1, NvU32 pmcBoot42);

//
// gpumgrIsSubDeviceCountOne
//
static NV_INLINE NvBool
gpumgrIsSubDeviceCountOne(NvU32 gpuMask)
{
    //
    // A fast version of gpumgrGetSubDeviceCount(gpumask) == 1.
    // Make sure it returns 0 for gpuMask==0, just like gpumgrGetSubDeviceCount(0)!!!
    //
    return gpuMask != 0 && (gpuMask&(gpuMask-1)) == 0;
}

//
// gpumgrIsParentGPU
//
static NV_INLINE NvBool
gpumgrIsParentGPU(OBJGPU *pGpu)
{
    return gpumgrGetParentGPU(pGpu) == pGpu;
}

#endif // _GPUMGR_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_MGR_NVOC_H_
