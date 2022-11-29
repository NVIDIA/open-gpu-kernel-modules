#ifndef _G_KERNEL_VGPU_MGR_NVOC_H_
#define _G_KERNEL_VGPU_MGR_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_vgpu_mgr_nvoc.h"

#ifndef __kernel_vgpu_mgr_h__
#define __kernel_vgpu_mgr_h__

#include "ctrl/ctrl2080/ctrl2080vgpumgrinternal.h"
#include "ctrl/ctrla081.h"
#include "ctrl/ctrla084.h"

#include "gpu/gpu.h"
#include "nv-hypervisor.h"
#include "nvlimits.h"
#include "core/core.h"
#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "virtualization/common_vgpu_mgr.h"

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

/* This structure represents guest vgpu device's (assigned to VM) information.
   For VGPU-GSP, only KERNEL_HOST_VGPU_DEVICE is avaliable on kernel. */
typedef struct KERNEL_HOST_VGPU_DEVICE
{
    NvU32                            vgpuType;
    NvHandle                         hClient;        /*Internal RM client to dup smcPartition*/
    struct KERNEL_VGPU_GUEST        *vgpuGuest;
    NvU32                            gfid;
    NvU32                            swizzId;
    NvU32                            numPluginChannels;
    NvU32                            chidOffset[RM_ENGINE_TYPE_LAST];
    NvU8                             vgpuUuid[RM_SHA1_GID_SIZE];
    void                            *pVgpuVfioRef;
    struct REQUEST_VGPU_INFO_NODE   *pRequestVgpuInfoNode;
    struct PhysMemSubAlloc                 *pPhysMemSubAlloc;
    NvU32                            gpuInstance;
    struct HOST_VGPU_DEVICE         *pHostVgpuDevice;
    // Legacy fields
    NvHandle                         hPluginFBAllocationClient;
    VGPU_DEVICE_GUEST_FB_INFO        vgpuDeviceGuestFbInfo;
    NvU32                           *pGuestFbSegment;
    NvU32                            guestFbSegmentPageSize;
    NvBool                           bOfflinedPageInfoValid;
    NvU32                            offlinedPageCount;       /* offlined page count */
    NvU64                            offlinedPageGpa[NV2080_CTRL_FB_OFFLINED_PAGES_MAX_PAGES];
} KERNEL_HOST_VGPU_DEVICE;

MAKE_LIST(KERNEL_HOST_VGPU_DEVICE_LIST, KERNEL_HOST_VGPU_DEVICE);

/* vGPU guest definition */
typedef struct KERNEL_VGPU_GUEST
{
    KERNEL_GUEST_VM_INFO     guestVmInfo;                            // guest VM's information
    NvU32                    numVgpuDevices;
} KERNEL_VGPU_GUEST;

MAKE_LIST(KERNEL_VGPU_GUEST_LIST, KERNEL_VGPU_GUEST);

/* pGPU information */
typedef struct
{
    NvU32                            gpuPciId;
    NvU32                            supportedTypeIds[MAX_VGPU_TYPES_PER_PGPU];
    NvU32                            numActiveVgpu;
    NvU32                            numVgpuTypes;
    NvU32                            vgpuConfigState;
    KERNEL_HOST_VGPU_DEVICE_LIST     listHostVgpuDeviceHead;
    VGPU_TYPE                       *vgpuTypes[MAX_VGPU_TYPES_PER_PGPU];
    NvBool                           isAttached;
    // Per VF per engine ChidOffset. Used for reserving guest channels per engine
    NvU32                            chidOffset[VGPU_MAX_GFID][RM_ENGINE_TYPE_LAST];
    VGPU_CONFIG_EVENT_INFO_NODE_LIST listVgpuConfigEventsHead;
    NvBool                           sriovEnabled;
    NvU32                            numCreatedVgpu;                     // Used only on KVM
    vgpu_vf_pci_info                 vfPciInfo[MAX_VF_COUNT_PER_GPU];    // Used only on KVM
    NvU64                            createdVfMask;                      // Used only on KVM

    /*!
     * SwizzId Map. HW currently uses only 14 swizzIds. Every bit position
     * in this mask represents swizzID and a "1" in bitMask states SwzzId
     * in already assigned to a vGPU device.
     */
    NvU64                            assignedSwizzIdMask;
    NvU32                            fractionalMultiVgpu;
} KERNEL_PHYS_GPU_INFO;

/* vGPU info received from mdev kernel module for KVM */
typedef struct REQUEST_VGPU_INFO_NODE
{
    char                     configParams[VGPU_CONFIG_PARAMS_MAX_LENGTH];
    NvU8                     mdevUuid[VGPU_UUID_SIZE];
    void                    *waitQueue;
    NvU8                    *vmName;
    NvS32                   *returnStatus;
    NvU32                    gpuPciId;
    NvU32                    qemuPid;
    NvU16                    vgpuId;
    VGPU_DEVICE_STATE        deviceState;
    NvU32                    gpuPciBdf;
    NvU32                    swizzId;
    KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
} REQUEST_VGPU_INFO_NODE;

MAKE_LIST(REQUEST_VGPU_INFO_NODE_LIST, REQUEST_VGPU_INFO_NODE);

#ifdef NVOC_KERNEL_VGPU_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelVgpuMgr {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct KernelVgpuMgr *__nvoc_pbase_KernelVgpuMgr;
    KERNEL_PHYS_GPU_INFO pgpuInfo[32];
    NvU32 pgpuCount;
    VGPU_TYPE_LIST listVgpuTypeHead;
    KERNEL_VGPU_GUEST_LIST listVgpuGuestHead;
    NvU32 user_min_supported_version;
    NvU32 user_max_supported_version;
    struct OBJEHEAP *pHeap;
    REQUEST_VGPU_INFO_NODE_LIST listRequestVgpuHead;
    MEMORY_DESCRIPTOR *pGspPluginHeapMemDesc;
};

#ifndef __NVOC_CLASS_KernelVgpuMgr_TYPEDEF__
#define __NVOC_CLASS_KernelVgpuMgr_TYPEDEF__
typedef struct KernelVgpuMgr KernelVgpuMgr;
#endif /* __NVOC_CLASS_KernelVgpuMgr_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelVgpuMgr
#define __nvoc_class_id_KernelVgpuMgr 0xa793dd
#endif /* __nvoc_class_id_KernelVgpuMgr */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelVgpuMgr;

#define __staticCast_KernelVgpuMgr(pThis) \
    ((pThis)->__nvoc_pbase_KernelVgpuMgr)

#ifdef __nvoc_kernel_vgpu_mgr_h_disabled
#define __dynamicCast_KernelVgpuMgr(pThis) ((KernelVgpuMgr*)NULL)
#else //__nvoc_kernel_vgpu_mgr_h_disabled
#define __dynamicCast_KernelVgpuMgr(pThis) \
    ((KernelVgpuMgr*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelVgpuMgr)))
#endif //__nvoc_kernel_vgpu_mgr_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelVgpuMgr(KernelVgpuMgr**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelVgpuMgr(KernelVgpuMgr**, Dynamic*, NvU32);
#define __objCreate_KernelVgpuMgr(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelVgpuMgr((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

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
kvgpumgrPgpuAddVgpuType(struct OBJGPU *pGpu, NvBool discardVgpuTypes, NVA081_CTRL_VGPU_INFO *pVgpuInfo);

NV_STATUS
kvgpumgrGetConfigEventInfoFromDb(NvHandle hClient, NvHandle hVgpuConfig,
                                 VGPU_CONFIG_EVENT_INFO_NODE **ppVgpuConfigEventInfoNode,
                                 NvU32 pgpuIndex);

NV_STATUS
kvgpumgrAttachGpu(NvU32 gpuPciId);

NV_STATUS
kvgpumgrDetachGpu(NvU32 gpuPciId);

NV_STATUS
kvgpumgrRegisterGuestId(NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_GUEST_ID_PARAMS *pParams,
                        KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice, struct OBJGPU *pGpu);

NV_STATUS
kvgpumgrGuestRegister(struct OBJGPU *pGpu,
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
                      KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice);

NV_STATUS
kvgpumgrGuestUnregister(struct OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice);

NV_STATUS
kvgpumgrGetMaxInstanceOfVgpu(NvU32 vgpuTypeId, NvU32 *maxInstanceVgpu);

NV_STATUS
kvgpumgrCheckVgpuTypeCreatable(KERNEL_PHYS_GPU_INFO *pPhysGpuInfo, VGPU_TYPE *vgpuTypeInfo);

NV_STATUS
kvgpumgrEnumerateVgpuPerPgpu(struct OBJGPU *pGpu, NV2080_CTRL_VGPU_MGR_INTERNAL_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams);

NV_STATUS
kvgpumgrClearGuestVmInfo(struct OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice);

NV_STATUS
kvgpumgrGetSwizzId(struct OBJGPU *pGpu,
                   KERNEL_PHYS_GPU_INFO *pPhysGpuInfo,
                   NvU32 partitionFlag,
                   NvU32 *swizzId);

NV_STATUS
kvgpumgrGetVgpuFbUsage(struct OBJGPU *pGpu, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams);

NV_STATUS
kvgpumgrSetVgpuEncoderCapacity(struct OBJGPU *pGpu, NvU8 *vgpuUuid, NvU32 encoderCapacity);

NV_STATUS
kvgpumgrStart(const NvU8 *pMdevUuid, void *waitQueue, NvS32 *returnStatus,
              NvU8 *vmName, NvU32 qemuPid);

NV_STATUS
kvgpumgrCreateRequestVgpu(NvU32 gpuPciId, const NvU8 *pMdevUuid,
                         NvU32 vgpuTypeId, NvU16 *vgpuId, NvU32 gpuPciBdf);

NV_STATUS
kvgpumgrDeleteRequestVgpu(const NvU8 *pMdevUuid, NvU16 vgpuId);

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromMdevUuid(NvU32 gpuPciId, const NvU8 *pMdevUuid,
                                      KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice);

NV_STATUS
kvgpumgrGetHostVgpuDeviceFromVmId(NvU32 gpuPciId, VM_ID guestVmId,
                                  KERNEL_HOST_VGPU_DEVICE **ppKernelHostVgpuDevice,
                                  VM_ID_TYPE vmIdType);

NV_STATUS
kvgpumgrGetCreatableVgpuTypes(struct OBJGPU *pGpu, struct KernelVgpuMgr *pKernelVgpuMgr, NvU32 pgpuIndex, NvU32* numVgpuTypes, NvU32* vgpuTypes);

NvU32
kvgpumgrGetPgpuDevIdEncoding(struct OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize);

NvU32
kvgpumgrGetPgpuSubdevIdEncoding(struct OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize);

NvU32
kvgpumgrGetPgpuFSEncoding(struct OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize);

NvU32
kvgpumgrGetPgpuCapEncoding(struct OBJGPU *pGpu, NvU8 *pgpuString, NvU32 strSize);

NvBool
kvgpumgrCheckPgpuMigrationSupport(struct OBJGPU *pGpu);

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
kvgpumgrSendAllVgpuTypesToGsp(struct OBJGPU *pGpu);

NvBool
kvgpumgrIsHeterogeneousVgpuSupported(void);

#endif // __kernel_vgpu_mgr_h__

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERNEL_VGPU_MGR_NVOC_H_
