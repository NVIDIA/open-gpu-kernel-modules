
#ifndef _G_KERNEL_VGPU_MGR_NVOC_H_
#define _G_KERNEL_VGPU_MGR_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_vgpu_mgr_nvoc.h"

#ifndef __kernel_vgpu_mgr_h__
#define __kernel_vgpu_mgr_h__

#include "ctrl/ctrl2080/ctrl2080vgpumgrinternal.h"
#include "ctrl/ctrl2080/ctrl2080fb.h"
#include "ctrl/ctrla081.h"
#include "ctrl/ctrlc637.h"

#include "gpu/gpu.h"
#include "nv-hypervisor.h"
#include "nvlimits.h"
#include "core/core.h"
#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "virtualization/common_vgpu_mgr.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"


struct PhysMemSubAlloc;

#ifndef __NVOC_CLASS_PhysMemSubAlloc_TYPEDEF__
#define __NVOC_CLASS_PhysMemSubAlloc_TYPEDEF__
typedef struct PhysMemSubAlloc PhysMemSubAlloc;
#endif /* __NVOC_CLASS_PhysMemSubAlloc_TYPEDEF__ */

#ifndef __nvoc_class_id_PhysMemSubAlloc
#define __nvoc_class_id_PhysMemSubAlloc 0x2351fc
#endif /* __nvoc_class_id_PhysMemSubAlloc */



struct VgpuConfigApi;

#ifndef __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
#define __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
typedef struct VgpuConfigApi VgpuConfigApi;
#endif /* __NVOC_CLASS_VgpuConfigApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VgpuConfigApi
#define __nvoc_class_id_VgpuConfigApi 0x4d560a
#endif /* __nvoc_class_id_VgpuConfigApi */


typedef struct KERNEL_MIG_GPU_INSTANCE KERNEL_MIG_GPU_INSTANCE;

/* vGPU events info lookup node*/
typedef struct VGPU_EVENT_INFO_NODE
{
    NvHandle                hClient;
    NvHandle                hVgpuConfig;
    struct VgpuConfigApi          *pVgpuConfigApi;
} VGPU_CONFIG_EVENT_INFO_NODE;

MAKE_LIST(VGPU_CONFIG_EVENT_INFO_NODE_LIST, VGPU_CONFIG_EVENT_INFO_NODE);

/* This structure represents vGPU guest's (VM's) information */
typedef struct
{
    VM_ID_TYPE          vmIdType;
    VM_ID               guestVmId;
    NvU8                vmName[NVA081_VM_NAME_SIZE]; /* Used only on KVM */
} KERNEL_GUEST_VM_INFO;

/* This structure represents vgpu device's FB information
 * which is visible to that guest. Here, 'offset' is offset
 * from start of physical FB addresses in host FB space.
 */
typedef struct VGPU_DEVICE_GUEST_FB_INFO
{
    NvU64               offset;
    NvU64               length;
    NvBool              bValid;
} VGPU_DEVICE_GUEST_FB_INFO;

/* This enum is used only on kvm/device-vm */
typedef enum VGPU_SYSFS_OP
{
    SET_GPU_INSTANCE_ID   = 0,
    SET_PLACEMENT_ID      = 1
} VGPU_SYSFS_OP;

typedef struct
{
    NvU64 hbmBaseAddr;
    NvU64 size;
} HBM_REGION_INFO;

/* This structure represents guest vgpu device's (assigned to VM) information.
   For VGPU-GSP, only KERNEL_HOST_VGPU_DEVICE is avaliable on kernel. */
typedef struct KERNEL_HOST_VGPU_DEVICE
{
    NvU32                            vgpuType;
    NvHandle                         hMigClient;        /*Internal RM client to dup smcPartition*/
    NvHandle                         hMigDevice;        /*Internal RM device to dup smcPartition*/
    struct KERNEL_VGPU_GUEST        *vgpuGuest;
    NvU32                            gfid;
    NvU32                            swizzId;
    NvU16                            placementId;
    NvU32                            numPluginChannels;
    NvU32                            chidOffset[RM_ENGINE_TYPE_LAST];
    NvU32                            channelCount[RM_ENGINE_TYPE_LAST]; /*Number of channels available to the VF*/
    NvU8                             vgpuUuid[RM_SHA1_GID_SIZE];
    struct REQUEST_VGPU_INFO_NODE   *pRequestVgpuInfoNode;
    struct PhysMemSubAlloc                 *pPhysMemSubAlloc;
    struct HOST_VGPU_DEVICE         *pHostVgpuDevice;
    HBM_REGION_INFO                 *hbmRegionList;
    NvU32                            numValidHbmRegions;
    // Legacy fields
    NvHandle                         hPluginFBAllocationClient;
    VGPU_DEVICE_GUEST_FB_INFO        vgpuDeviceGuestFbInfo;
    NvU32                           *pGuestFbSegment;
    NvU64                            guestFbSegmentPageSize;
    NvBool                           bOfflinedPageInfoValid;
    NvU32                            offlinedPageCount;       /* offlined page count */
    NvU64                            offlinedPageGpa[NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES];
    MEMORY_DESCRIPTOR                *pGspPluginHeapMemDesc;
    NvBool                            bDisableDefaultSmcExecPartRestore;
    struct GPUMGR_SAVE_COMPUTE_INSTANCE savedExecPartitions[NVC637_CTRL_MAX_EXEC_PARTITIONS];
    NvBool                            bGpupLiveMigrationEnabled; /* GPUP Live Migration Enabled status */
    NvBool                            bGspPluginTaskInitialized;
    NvU32                             vgpuDeviceInstanceId;
} KERNEL_HOST_VGPU_DEVICE;

MAKE_LIST(KERNEL_HOST_VGPU_DEVICE_LIST, KERNEL_HOST_VGPU_DEVICE);

/* vGPU guest definition */
typedef struct KERNEL_VGPU_GUEST
{
    KERNEL_GUEST_VM_INFO     guestVmInfo;                            // guest VM's information
    NvU32                    numVgpuDevices;
} KERNEL_VGPU_GUEST;

MAKE_LIST(KERNEL_VGPU_GUEST_LIST, KERNEL_VGPU_GUEST);

#define NV_VGPU_MAX_FB_SIZE_GB 512

MAKE_BITVECTOR(PLACEMENT_REGION_BIT_VECTOR, NV_VGPU_MAX_FB_SIZE_GB);

/* This structure stores per vGPU instance placement information */
typedef struct
{
    NvU16   creatablePlacementId;
    /* Used for Heterogeneous vGPU mode only */
    NvU32   heterogeneousSupportedVmmuOffset;
    NvU64   heterogeneousGspHeapOffset;
    /* Used for Homogeneous vGPU placement mode only */
    NvU32   homogeneousSupportedVmmuOffset;
} VGPU_INSTANCE_PLACEMENT_INFO;

/* These structures store per vGPU Type's placement information */
typedef struct
{
    VGPU_INSTANCE_PLACEMENT_INFO    vgpuInstancePlacementInfo[MAX_VGPU_DEVICES_PER_PGPU];
    /* Used for both Heterogeneous and Homogeneous vGPU placement mode */
    NvU32                           guestVmmuCount;
} VGPU_TYPE_PLACEMENT_INFO;

typedef struct
{
    VGPU_TYPE_PLACEMENT_INFO        vgpuTypePlacementInfo[MAX_VGPU_TYPES_PER_PGPU];
    NvU16                           placementRegionSize;
    /* Used for Heterogeneous vGPU mode only */
    PLACEMENT_REGION_BIT_VECTOR     usedPlacementRegionMap;
} KERNEL_VGPU_TYPE_PLACEMENT_INFO;

#define MAX_GI_PARTITIONS_PER_GPU 4  // 4 indices for storing 4 GI partitions which are supported on B40

typedef struct
{
    NvU32                             swizzId;
    NvBool                            bHeterogeneousModeEnabled;
    NvBool                            bUsed;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO   kernelVgpuTypePlacementInfo;
    // Used for heterogeneous placement
    NvU32                             supportedTypeIds[MAX_VGPU_TYPES_PER_PGPU];
} KERNEL_VGPU_TYPE_PLACEMENT_INFO_PER_GI;

/* pGPU information */
typedef struct
{
    NvU32                                   gpuPciId;
    NvU32                                   supportedTypeIds[MAX_VGPU_TYPES_PER_PGPU];
    NvU32                                   numActiveVgpu;
    NvU32                                   numVgpuTypes;
    NvU32                                   vgpuConfigState;
    KERNEL_HOST_VGPU_DEVICE_LIST            listHostVgpuDeviceHead;
    VGPU_TYPE                              *vgpuTypes[MAX_VGPU_TYPES_PER_PGPU];
    NvBool                                  isAttached;
    // Per VF per engine ChidOffset. Used for reserving guest channels per engine
    NvU32                                   chidOffset[VGPU_MAX_GFID][RM_ENGINE_TYPE_LAST];
    KERNEL_VGPU_TYPE_PLACEMENT_INFO         kernelVgpuTypePlacementInfo;
    KERNEL_VGPU_TYPE_PLACEMENT_INFO_PER_GI  kernelVgpuTypePlacementInfoGi[MAX_GI_PARTITIONS_PER_GPU];
    VGPU_CONFIG_EVENT_INFO_NODE_LIST        listVgpuConfigEventsHead;
    NvBool                                  sriovEnabled;
    NvBool                                  heterogeneousTimesliceSizesSupported;
    // Indicates the placement ID mode is supported on homogeneous vGPU profile
    NvBool                                  homogeneousPlacementSupported;
    NvU32                                   numCreatedVgpu;                     // Used only on KVM
    vgpu_vf_pci_info                        vfPciInfo[MAX_VF_COUNT_PER_GPU];    // Used only on KVM
    NvU64                                   createdVfMask;                      // Used only on KVM
    NvBool                                  miniQuarterEnabled;                 // Used only on ESXi (vGPU profile)
    NvBool                                  computeMediaEngineEnabled;          // Used only on ESXi (vGPU profile)

    /*!
     * SwizzId Map. HW currently uses only 14 swizzIds. Every bit position
     * in this mask represents swizzID and a "1" in bitMask states SwzzId
     * in already assigned to a vGPU device.
     */
    NvU64                                   assignedSwizzIdMask;
    NvU32                                   assignedSwizzIdVgpuCount[KMIGMGR_MAX_GPU_SWIZZID];
    NvU32                                   fractionalMultiVgpu;
    // Indicates MIG timeslicing mode enabled/disabled
    NvBool                                  migTimeslicingModeEnabled;
} KERNEL_PHYS_GPU_INFO;

/* vGPU info received from mdev kernel module for KVM */
typedef struct REQUEST_VGPU_INFO_NODE
{
    NvU32                    vgpuTypeId;
    NvU8                     vgpuDevName[VGPU_UUID_SIZE];
    NvU32                    gpuPciId;
    NvU32                    gpuPciBdf;
    NvU32                    swizzId;
    NvU32                    placementId;
    NvU16                    vgpuId;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
} REQUEST_VGPU_INFO_NODE;

MAKE_LIST(REQUEST_VGPU_INFO_NODE_LIST, REQUEST_VGPU_INFO_NODE);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_VGPU_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelVgpuMgr;
struct NVOC_METADATA__Object;


struct KernelVgpuMgr {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelVgpuMgr *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct KernelVgpuMgr *__nvoc_pbase_KernelVgpuMgr;    // kvgpumgr

    // Data members
    KERNEL_PHYS_GPU_INFO pgpuInfo[32];
    NvU32 pgpuCount;
    VGPU_TYPE_LIST listVgpuTypeHead;
    KERNEL_VGPU_GUEST_LIST listVgpuGuestHead;
    NvU32 user_min_supported_version;
    NvU32 user_max_supported_version;
    OBJEHEAP *pHeap;
    REQUEST_VGPU_INFO_NODE_LIST listRequestVgpuHead;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelVgpuMgr {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_KernelVgpuMgr_TYPEDEF__
#define __NVOC_CLASS_KernelVgpuMgr_TYPEDEF__
typedef struct KernelVgpuMgr KernelVgpuMgr;
#endif /* __NVOC_CLASS_KernelVgpuMgr_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelVgpuMgr
#define __nvoc_class_id_KernelVgpuMgr 0xa793dd
#endif /* __nvoc_class_id_KernelVgpuMgr */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelVgpuMgr;

#define __staticCast_KernelVgpuMgr(pThis) \
    ((pThis)->__nvoc_pbase_KernelVgpuMgr)

#ifdef __nvoc_kernel_vgpu_mgr_h_disabled
#define __dynamicCast_KernelVgpuMgr(pThis) ((KernelVgpuMgr*) NULL)
#else //__nvoc_kernel_vgpu_mgr_h_disabled
#define __dynamicCast_KernelVgpuMgr(pThis) \
    ((KernelVgpuMgr*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelVgpuMgr)))
#endif //__nvoc_kernel_vgpu_mgr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelVgpuMgr(KernelVgpuMgr**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelVgpuMgr(KernelVgpuMgr**, Dynamic*, NvU32);
#define __objCreate_KernelVgpuMgr(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelVgpuMgr((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS kvgpumgrConstruct_IMPL(struct KernelVgpuMgr *arg_pKernelVgpuMgr);

#define __nvoc_kvgpumgrConstruct(arg_pKernelVgpuMgr) kvgpumgrConstruct_IMPL(arg_pKernelVgpuMgr)
void kvgpumgrDestruct_IMPL(struct KernelVgpuMgr *pKernelVgpuMgr);

#define __nvoc_kvgpumgrDestruct(pKernelVgpuMgr) kvgpumgrDestruct_IMPL(pKernelVgpuMgr)
#undef PRIVATE_FIELD


NV_STATUS
kvgpumgrGetPgpuIndex(struct KernelVgpuMgr *pKernelVgpuMgr, NvU32 gpuPciId, NvU32* index);

NV_STATUS
kvgpumgrGetVgpuTypeInfo(NvU32 vgpuTypeId, VGPU_TYPE **vgpuType);

NV_STATUS
kvgpumgrSetSupportedPlacementIds(OBJGPU *pGpu);

NV_STATUS
kvgpumgrUpdateHeterogeneousInfo(OBJGPU *pGpu, Device *pDevice,
                                NvU32 vgpuTypeId, NvU16 *placementId,
                                NvU64 *guestFbLength, NvU64 *guestFbOffset,
                                NvU64 *gspHeapOffset, NvU64 *guestBar1PFOffset);

NV_STATUS
kvgpumgrUpdateHomogeneousInfo(OBJGPU *pGpu, Device *pDevice,
                              NvU32 vgpuTypeId, NvU16 *placementId,
                              NvU64 *guestFbLength, NvU64 *guestFbOffset);

NV_STATUS
kvgpumgrPgpuAddVgpuType(OBJGPU *pGpu, NvBool discardVgpuTypes, NVA081_CTRL_VGPU_INFO *pVgpuInfo);

NV_STATUS
kvgpumgrGetConfigEventInfoFromDb(NvHandle hClient, NvHandle hVgpuConfig,
                                 VGPU_CONFIG_EVENT_INFO_NODE **ppVgpuConfigEventInfoNode,
                                 NvU32 pgpuIndex);

NV_STATUS
kvgpumgrAttachGpu(NvU32 gpuPciId);

NV_STATUS
kvgpumgrDetachGpu(NvU32 gpuPciId);

NV_STATUS
kvgpumgrMigTimeslicingModeEnabled(OBJGPU *pGpu);
NvBool
kvgpumgrIsMigTimeslicingModeEnabled(OBJGPU *pGpu);

NV_STATUS
kvgpumgrGuestRegister(OBJGPU *pGpu,
                      NvU32 gfid,
                      NvU32 vgpuType,
                      NvU32 vmPid,
                      VM_ID_TYPE vmIdType,
                      VM_ID guestVmId,
                      NvHandle hPluginFBAllocationClient,
                      NvU32 numChannels,
                      NvU32 numPluginChannels,
                      NvU32 swizzId,
                      NvU32 vgpuDeviceInstanceId,
                      NvBool bDisableDefaultSmcExecPartRestore,
                      NvU16 placementId,
                      NvU8 *pVgpuDevName,
                      KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice);

NV_STATUS
kvgpumgrGuestUnregister(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice);

NV_STATUS
kvgpumgrGetMaxInstanceOfVgpu(NvU32 vgpuTypeId, NvU32 *maxInstanceVgpu);

NV_STATUS
kvgpumgrCheckVgpuTypeCreatable(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPhysGpuInfo, VGPU_TYPE *vgpuTypeInfo);

NV_STATUS
kvgpumgrEnumerateVgpuPerPgpu(OBJGPU *pGpu, NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams);

NV_STATUS
kvgpumgrClearGuestVmInfo(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice);

NV_STATUS
kvgpumgrGetSwizzId(OBJGPU *pGpu,
                   KERNEL_PHYS_GPU_INFO *pPhysGpuInfo,
                   NvU32 partitionFlag,
                   VGPU_TYPE *vgpuTypeInfo, 
                   NvU32 *swizzId);

NV_STATUS
kvgpumgrHeterogeneousGetChidOffset(NvU32 vgpuTypeId, NvU16 placementId,
                                   NvU32 numChannels, NvU64 *pChidOffset);

NV_STATUS
kvgpumgrHomogeneousGetChidOffset(NvU32 vgpuTypeId, NvU16 placementId,
                                 NvU32 numChannels, NvU64 *pChidOffset);

NV_STATUS
kvgpumgrValidateSwizzId(OBJGPU *pGpu,
                        NvU32 vgpuTypeId,
                        NvU32 swizzId);

NV_STATUS
kvgpumgrGetVgpuFbUsage(OBJGPU *pGpu, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams);

NV_STATUS
kvgpumgrSetVgpuEncoderCapacity(OBJGPU *pGpu, NvU8 *vgpuUuid, NvU32 encoderCapacity);

NV_STATUS
kvgpumgrCreateRequestVgpu(NvU32 gpuPciId, const NvU8 *pVgpuDevName,
                         NvU32 vgpuTypeId, NvU16 *vgpuId, NvU32 *gpuInstanceId, NvU32 *placementId, NvU32 gpuPciBdf);

NV_STATUS
kvgpumgrDeleteRequestVgpu(const NvU8 *pVgpuDevName, NvU16 vgpuId);

NV_STATUS
kvgpumgrGetAvailableInstances(NvU32 *avail_instances, OBJGPU *pGpu, VGPU_TYPE *vgpuTypeInfo,
                              NvU32 pgpuIndex, NvU8 devfn);

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromVgpuDevName(NvU32 gpuPciId, const NvU8 *pVgpuDevName,
                                      KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice);

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromVgpuUuid(NvU32 gpuPciId, NvU8 *vgpuUuid,
                                  KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice);

NV_STATUS
kvgpumgrGetCreatableVgpuTypes(OBJGPU *pGpu, struct KernelVgpuMgr *pKernelVgpuMgr, NvU32 pgpuIndex,
                              NvU32 gpuInstanceId, NvU32* numVgpuTypes, NvU32* vgpuTypes);

NvU64
kvgpumgrGetEccAndPrReservedFb(OBJGPU *pGpu);

NvU32
kvgpumgrGetPgpuDevIdEncoding(OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize);

NvU32
kvgpumgrGetPgpuSubdevIdEncoding(OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize);

NvU32
kvgpumgrGetPgpuFSEncoding(OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize);

NvU32
kvgpumgrGetPgpuCapEncoding(OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize);

NvBool
kvgpumgrCheckPgpuMigrationSupport(OBJGPU *pGpu);

NV_STATUS
kvgpumgrGetHostVgpuVersion(NvU32 *user_min_supported_version,
                           NvU32 *user_max_supported_version);

NV_STATUS
kvgpumgrSetHostVgpuVersion(NvU32 user_min_supported_version,
                           NvU32 user_max_supported_version);

NV_STATUS
kvgpumgrGetPartitionFlag(NvU32   vgpuTypeId,
                         NvU32   *partitionFlag);

NV_STATUS
kvgpumgrProcessVfInfo(NvU32 gpuPciId, NvU8 cmd, NvU32 domain, NvU32 bus, NvU32 slot, NvU32 function, NvBool isMdevAttached, vgpu_vf_pci_info *vf_info);

NV_STATUS
kvgpumgrSendAllVgpuTypesToGsp(OBJGPU *pGpu);

/*
*This function returns true if heterogeneous vgpu types is supported
*and doesn't inform whether heterogeneous vgpu sizes is supported.
*To figure out if heterogeneous vgpu sizes are suppported, 
*check "heterogeneousTimesliceSizesSupported" variable.
*/
NvBool
kvgpumgrIsHeterogeneousVgpuTypeSupported(void);

NV_STATUS
kvgpumgrCheckHomogeneousPlacementSupported(OBJGPU *pGpu, NvU32 swizzId);

NvBool
kvgpumgrIsVgpuWarmUpdateSupported(void);

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromGfid(NvU32 gpuPciId, NvU32 gfid,
                                  KERNEL_HOST_VGPU_DEVICE** ppHostVgpuDevice);
NV_STATUS
kvgpuMgrRestoreSmcExecPart(OBJGPU *pGpu,KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
                           KERNEL_MIG_GPU_INSTANCE *pKernelMIGGpuInstance);
NV_STATUS
kvgpumgrSetVgpuType(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPhysGpuInfo, NvU32 vgpuTypeId, NvU32 swizzId);

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromVmId(NvU32 gpuPciId, NvU64 vmId,
                                  KERNEL_HOST_VGPU_DEVICE** ppHostVgpuDevice);

NV_STATUS
kvgpumgrSetGpuInstanceId(REQUEST_VGPU_INFO_NODE *pRequestVgpu, NvU32 swizzId);

NV_STATUS
kvgpumgrSetPlacementId(REQUEST_VGPU_INFO_NODE *pRequestVgpu, NvU32 placementId);

NV_STATUS
kvgpuMgrGetPlacementResource(OBJGPU *pGpu, Device *pDevice,
                             const NvU8 *pVgpuDevName, NvU16 *placementId,
                             NvU64 *guestFbLength, NvU64 *guestFbOffset,
                             NvU64 *gspHeapOffset, NvU64 *guestBar1PFOffset);
KERNEL_VGPU_TYPE_PLACEMENT_INFO*
kvgpuMgrGetVgpuPlacementInfo(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPgpuInfo, NvU32 swizzId);
NvU32*
kvgpuMgrGetVgpuCreatableTypeIdFromSwizzId(OBJGPU *pGpu, KERNEL_PHYS_GPU_INFO *pPgpuInfo, NvU32 swizzId);
NvU32
kvgpuMgrGetSwizzIdFromDevice(OBJGPU *pGpu, Device *pDevice);
NV_STATUS
kvgpuMgrReserveVgpuPlacementInfoPerGI(OBJGPU *pGpu, NvU32 swizzId);
NV_STATUS
kvgpuMgrClearVgpuPlacementInfoPerGI(OBJGPU *pGpu, NvU32 swizzId);
NV_STATUS
kvgpuMgrSetHeterogeneousModePerGI(OBJGPU *pGpu, NvU32 swizzId, NvBool bHeterogeneousModeEnabled);
NV_STATUS
kvgpuMgrGetHeterogeneousModePerGI(OBJGPU *pGpu, NvU32 swizzId, NvBool *bHeterogeneousModeEnabled);
NV_STATUS
kvgpuMgrGetHeterogeneousMode(OBJGPU *pGpu, NvU32 swizzId, NvBool *bHeterogeneousModeEnabled);
#endif // __kernel_vgpu_mgr_h__

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_VGPU_MGR_NVOC_H_
