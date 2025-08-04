
#ifndef _G_DEVICE_NVOC_H_
#define _G_DEVICE_NVOC_H_

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
#pragma once
#include "g_device_nvoc.h"

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "core/core.h"

#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "nvoc/utility.h"
#include "resserv/rs_resource.h"
#include "rmapi/control.h"
#include "containers/btree.h"

#include "gpu/gpu_halspec.h"
#include "gpu/gpu_resource.h"
#include "mem_mgr/vaspace.h"

#include "ctrl/ctrl0080.h" // rmcontrol params

// Forward declaration
struct KERNEL_HOST_VGPU_DEVICE;

struct OBJVASPACE;

#ifndef __NVOC_CLASS_OBJVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJVASPACE_TYPEDEF__
typedef struct OBJVASPACE OBJVASPACE;
#endif /* __NVOC_CLASS_OBJVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVASPACE
#define __nvoc_class_id_OBJVASPACE 0x6c347f
#endif /* __nvoc_class_id_OBJVASPACE */



/**
 *  A device consists of one or more GPUs. Devices provide broadcast
 *  semantics; that is, operations involving a device are applied to all GPUs
 *  in the device.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DEVICE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Device;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__Device;


struct Device {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Device *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct Device *__nvoc_pbase_Device;    // device

    // Vtable with 9 per-object function pointers
    NV_STATUS (*__deviceCtrlCmdDmaFlush__)(struct Device * /*this*/, NV0080_CTRL_DMA_FLUSH_PARAMS *);  // halified (2 hals) exported (id=0x801805) body
    NV_STATUS (*__deviceCtrlCmdFifoGetEngineContextProperties__)(struct Device * /*this*/, NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *);  // halified (2 hals) exported (id=0x801707) body
    NV_STATUS (*__deviceCtrlCmdFifoGetLatencyBufferSize__)(struct Device * /*this*/, NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS *);  // halified (2 hals) exported (id=0x80170e) body
    NV_STATUS (*__deviceCtrlCmdFifoIdleChannels__)(struct Device * /*this*/, NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS *);  // halified (2 hals) exported (id=0x801714) body
    NV_STATUS (*__deviceCtrlCmdHostGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_HOST_GET_CAPS_V2_PARAMS *);  // halified (2 hals) exported (id=0x801402)
    NV_STATUS (*__deviceCtrlCmdGpuGetBrandCaps__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x800294) body
    NV_STATUS (*__deviceCtrlCmdMsencGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS *);  // halified (2 hals) exported (id=0x801b02) body
    NV_STATUS (*__deviceCtrlCmdBspGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_BSP_GET_CAPS_PARAMS_V2 *);  // halified (2 hals) exported (id=0x801c02) body
    NV_STATUS (*__deviceCtrlCmdNvjpgGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS *);  // halified (2 hals) exported (id=0x801f02) body

    // Data members
    NvU32 deviceInst;
    NvU32 PerfReqCnt;
    PNODE DevMemoryTable;
    NvBool bSliGpuBoostSyncActivate;
    NvBool bPerfOptpActive;
    NvU32 nPerfOptpRefCnt;
    NvU32 nCudaLimitRefCnt;
    struct OBJVASPACE *pVASpace;
    NvHandle hClientShare;
    NvHandle hTargetClient;
    NvHandle hTargetDevice;
    NvU32 deviceAllocFlags;
    NvU32 deviceInternalAllocFlags;
    NvU64 vaStartInternal;
    NvU64 vaLimitInternal;
    NvU64 vaSize;
    NvU32 vaMode;
    NvU32 defaultVidmemPhysicalityOverride;
    struct KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__Device {
    NV_STATUS (*__deviceControl__)(struct Device * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (gpures)
    NV_STATUS (*__deviceInternalControlForward__)(struct Device * /*this*/, NvU32, void *, NvU32);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__deviceMap__)(struct Device * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__deviceUnmap__)(struct Device * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__deviceShareCallback__)(struct Device * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__deviceGetRegBaseOffsetAndSize__)(struct Device * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__deviceGetMapAddrSpace__)(struct Device * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__deviceGetInternalObjectHandle__)(struct Device * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__deviceAccessCallback__)(struct Device * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__deviceGetMemInterMapParams__)(struct Device * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__deviceCheckMemInterUnmap__)(struct Device * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__deviceGetMemoryMappingDescriptor__)(struct Device * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__deviceControlSerialization_Prologue__)(struct Device * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__deviceControlSerialization_Epilogue__)(struct Device * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__deviceControl_Prologue__)(struct Device * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__deviceControl_Epilogue__)(struct Device * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__deviceCanCopy__)(struct Device * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__deviceIsDuplicate__)(struct Device * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__devicePreDestruct__)(struct Device * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__deviceControlFilter__)(struct Device * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__deviceIsPartialUnmapSupported__)(struct Device * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__deviceMapTo__)(struct Device * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__deviceUnmapFrom__)(struct Device * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__deviceGetRefCount__)(struct Device * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__deviceAddAdditionalDependants__)(struct RsClient *, struct Device * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__Device {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__Device vtable;
};

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Device;

#define __staticCast_Device(pThis) \
    ((pThis)->__nvoc_pbase_Device)

#ifdef __nvoc_device_h_disabled
#define __dynamicCast_Device(pThis) ((Device*) NULL)
#else //__nvoc_device_h_disabled
#define __dynamicCast_Device(pThis) \
    ((Device*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Device)))
#endif //__nvoc_device_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Device(Device**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Device(Device**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_Device(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Device((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros for implementation functions
NV_STATUS deviceConstruct_IMPL(struct Device *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_deviceConstruct(arg_pResource, arg_pCallContext, arg_pParams) deviceConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)

void deviceDestruct_IMPL(struct Device *pResource);
#define __nvoc_deviceDestruct(pResource) deviceDestruct_IMPL(pResource)

NV_STATUS deviceGetByHandle_IMPL(struct RsClient *pClient, NvHandle hDevice, struct Device **ppDevice);
#define deviceGetByHandle(pClient, hDevice, ppDevice) deviceGetByHandle_IMPL(pClient, hDevice, ppDevice)

NV_STATUS deviceGetByInstance_IMPL(struct RsClient *pClient, NvU32 deviceInstance, struct Device **ppDevice);
#define deviceGetByInstance(pClient, deviceInstance, ppDevice) deviceGetByInstance_IMPL(pClient, deviceInstance, ppDevice)

NV_STATUS deviceGetByGpu_IMPL(struct RsClient *pClient, struct OBJGPU *pGpu, NvBool bAnyInGroup, struct Device **ppDevice);
#define deviceGetByGpu(pClient, pGpu, bAnyInGroup, ppDevice) deviceGetByGpu_IMPL(pClient, pGpu, bAnyInGroup, ppDevice)

NV_STATUS deviceGetDefaultVASpace_IMPL(struct Device *pDevice, struct OBJVASPACE **ppVAS);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceGetDefaultVASpace(struct Device *pDevice, struct OBJVASPACE **ppVAS) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceGetDefaultVASpace(pDevice, ppVAS) deviceGetDefaultVASpace_IMPL(pDevice, ppVAS)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceSetClientShare_IMPL(struct Device *pDevice, NvHandle hClientShare, NvU64 vaSize, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 deviceAllocFlags);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceSetClientShare(struct Device *pDevice, NvHandle hClientShare, NvU64 vaSize, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 deviceAllocFlags) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceSetClientShare(pDevice, hClientShare, vaSize, vaStartInternal, vaLimitInternal, deviceAllocFlags) deviceSetClientShare_IMPL(pDevice, hClientShare, vaSize, vaStartInternal, vaLimitInternal, deviceAllocFlags)
#endif // __nvoc_device_h_disabled

void deviceRemoveFromClientShare_IMPL(struct Device *pDevice);
#ifdef __nvoc_device_h_disabled
static inline void deviceRemoveFromClientShare(struct Device *pDevice) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
}
#else // __nvoc_device_h_disabled
#define deviceRemoveFromClientShare(pDevice) deviceRemoveFromClientShare_IMPL(pDevice)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceSetDefaultVASpace_IMPL(struct Device *pDevice, NvHandle hVASpace);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceSetDefaultVASpace(struct Device *pDevice, NvHandle hVASpace) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceSetDefaultVASpace(pDevice, hVASpace) deviceSetDefaultVASpace_IMPL(pDevice, hVASpace)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdBifAspmFeatureSupported_IMPL(struct Device *pDevice, NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS *pBifAspmParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdBifAspmFeatureSupported(struct Device *pDevice, NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS *pBifAspmParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdBifAspmFeatureSupported(pDevice, pBifAspmParams) deviceCtrlCmdBifAspmFeatureSupported_IMPL(pDevice, pBifAspmParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdBifAspmCyaUpdate_IMPL(struct Device *pDevice, NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS *pBifAspmCyaParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdBifAspmCyaUpdate(struct Device *pDevice, NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS *pBifAspmCyaParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdBifAspmCyaUpdate(pDevice, pBifAspmCyaParams) deviceCtrlCmdBifAspmCyaUpdate_IMPL(pDevice, pBifAspmCyaParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdBifGetPciePowerControlMask_IMPL(struct Device *pDevice, NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS *pBifPciePowerControlParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdBifGetPciePowerControlMask(struct Device *pDevice, NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS *pBifPciePowerControlParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdBifGetPciePowerControlMask(pDevice, pBifPciePowerControlParams) deviceCtrlCmdBifGetPciePowerControlMask_IMPL(pDevice, pBifPciePowerControlParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaGetPteInfo_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaGetPteInfo(struct Device *pDevice, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaGetPteInfo(pDevice, pParams) deviceCtrlCmdDmaGetPteInfo_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaUpdatePde2_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaUpdatePde2(struct Device *pDevice, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaUpdatePde2(pDevice, pParams) deviceCtrlCmdDmaUpdatePde2_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaSetPageDirectory_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaSetPageDirectory(struct Device *pDevice, NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaSetPageDirectory(pDevice, pParams) deviceCtrlCmdDmaSetPageDirectory_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaUnsetPageDirectory_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaUnsetPageDirectory(struct Device *pDevice, NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaUnsetPageDirectory(pDevice, pParams) deviceCtrlCmdDmaUnsetPageDirectory_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaAdvSchedGetVaCaps_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaAdvSchedGetVaCaps(struct Device *pDevice, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaAdvSchedGetVaCaps(pDevice, pParams) deviceCtrlCmdDmaAdvSchedGetVaCaps_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaGetPdeInfo_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaGetPdeInfo(struct Device *pDevice, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaGetPdeInfo(pDevice, pParams) deviceCtrlCmdDmaGetPdeInfo_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaSetPteInfo_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaSetPteInfo(struct Device *pDevice, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaSetPteInfo(pDevice, pParams) deviceCtrlCmdDmaSetPteInfo_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaInvalidateTLB_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaInvalidateTLB(struct Device *pDevice, NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaInvalidateTLB(pDevice, pParams) deviceCtrlCmdDmaInvalidateTLB_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_GET_CAPS_PARAMS *pDmaCapsParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaGetCaps(struct Device *pDevice, NV0080_CTRL_DMA_GET_CAPS_PARAMS *pDmaCapsParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaGetCaps(pDevice, pDmaCapsParams) deviceCtrlCmdDmaGetCaps_IMPL(pDevice, pDmaCapsParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaSetVASpaceSize_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaSetVASpaceSize(struct Device *pDevice, NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaSetVASpaceSize(pDevice, pParams) deviceCtrlCmdDmaSetVASpaceSize_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaEnablePrivilegedRange_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaEnablePrivilegedRange(struct Device *pDevice, NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaEnablePrivilegedRange(pDevice, pParams) deviceCtrlCmdDmaEnablePrivilegedRange_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdDmaSetDefaultVASpace_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdDmaSetDefaultVASpace(struct Device *pDevice, NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdDmaSetDefaultVASpace(pDevice, pParams) deviceCtrlCmdDmaSetDefaultVASpace_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdKGrGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_GR_GET_CAPS_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdKGrGetCaps(struct Device *pDevice, NV0080_CTRL_GR_GET_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdKGrGetCaps(pDevice, pParams) deviceCtrlCmdKGrGetCaps_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdKGrGetCapsV2_IMPL(struct Device *pDevice, NV0080_CTRL_GR_GET_CAPS_V2_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdKGrGetCapsV2(struct Device *pDevice, NV0080_CTRL_GR_GET_CAPS_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdKGrGetCapsV2(pDevice, pParams) deviceCtrlCmdKGrGetCapsV2_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdKGrGetInfo_IMPL(struct Device *pDevice, NV0080_CTRL_GR_GET_INFO_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdKGrGetInfo(struct Device *pDevice, NV0080_CTRL_GR_GET_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdKGrGetInfo(pDevice, pParams) deviceCtrlCmdKGrGetInfo_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdKGrGetInfoV2_IMPL(struct Device *pDevice, NV0080_CTRL_GR_GET_INFO_V2_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdKGrGetInfoV2(struct Device *pDevice, NV0080_CTRL_GR_GET_INFO_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdKGrGetInfoV2(pDevice, pParams) deviceCtrlCmdKGrGetInfoV2_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdKGrGetTpcPartitionMode_IMPL(struct Device *pDevice, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdKGrGetTpcPartitionMode(struct Device *pDevice, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdKGrGetTpcPartitionMode(pDevice, pParams) deviceCtrlCmdKGrGetTpcPartitionMode_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdKGrSetTpcPartitionMode_IMPL(struct Device *pDevice, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdKGrSetTpcPartitionMode(struct Device *pDevice, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdKGrSetTpcPartitionMode(pDevice, pParams) deviceCtrlCmdKGrSetTpcPartitionMode_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdKGrInternalInitBug4208224War_IMPL(struct Device *pDevice, NV0080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdKGrInternalInitBug4208224War(struct Device *pDevice, NV0080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdKGrInternalInitBug4208224War(pDevice, pParams) deviceCtrlCmdKGrInternalInitBug4208224War_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFbGetCompbitStoreInfo_IMPL(struct Device *pDevice, NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS *pCompbitStoreParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFbGetCompbitStoreInfo(struct Device *pDevice, NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS *pCompbitStoreParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFbGetCompbitStoreInfo(pDevice, pCompbitStoreParams) deviceCtrlCmdFbGetCompbitStoreInfo_IMPL(pDevice, pCompbitStoreParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFbSetZbcReferenced_IMPL(struct Device *pDevice, NV0080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFbSetZbcReferenced(struct Device *pDevice, NV0080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFbSetZbcReferenced(pDevice, pParams) deviceCtrlCmdFbSetZbcReferenced_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFbGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_PARAMS *pFbCapsParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFbGetCaps(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_PARAMS *pFbCapsParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFbGetCaps(pDevice, pFbCapsParams) deviceCtrlCmdFbGetCaps_IMPL(pDevice, pFbCapsParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFbGetCapsV2_IMPL(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_V2_PARAMS *pFbCapsParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFbGetCapsV2(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_V2_PARAMS *pFbCapsParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFbGetCapsV2(pDevice, pFbCapsParams) deviceCtrlCmdFbGetCapsV2_IMPL(pDevice, pFbCapsParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdSetDefaultVidmemPhysicality_IMPL(struct Device *pDevice, NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdSetDefaultVidmemPhysicality(struct Device *pDevice, NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdSetDefaultVidmemPhysicality(pDevice, pParams) deviceCtrlCmdSetDefaultVidmemPhysicality_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFifoGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *pFifoCapsParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFifoGetCaps(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *pFifoCapsParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFifoGetCaps(pDevice, pFifoCapsParams) deviceCtrlCmdFifoGetCaps_IMPL(pDevice, pFifoCapsParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFifoGetCapsV2_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS *pFifoCapsParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFifoGetCapsV2(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS *pFifoCapsParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFifoGetCapsV2(pDevice, pFifoCapsParams) deviceCtrlCmdFifoGetCapsV2_IMPL(pDevice, pFifoCapsParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFifoStopRunlist_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS *pStopRunlistParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFifoStopRunlist(struct Device *pDevice, NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS *pStopRunlistParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFifoStopRunlist(pDevice, pStopRunlistParams) deviceCtrlCmdFifoStopRunlist_IMPL(pDevice, pStopRunlistParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFifoStartRunlist_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_START_RUNLIST_PARAMS *pStartRunlistParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFifoStartRunlist(struct Device *pDevice, NV0080_CTRL_FIFO_START_RUNLIST_PARAMS *pStartRunlistParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFifoStartRunlist(pDevice, pStartRunlistParams) deviceCtrlCmdFifoStartRunlist_IMPL(pDevice, pStartRunlistParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFifoGetChannelList_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *pChannelParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFifoGetChannelList(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *pChannelParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFifoGetChannelList(pDevice, pChannelParams) deviceCtrlCmdFifoGetChannelList_IMPL(pDevice, pChannelParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdFifoSetChannelProperties_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS *pSetChannelPropertiesParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdFifoSetChannelProperties(struct Device *pDevice, NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS *pSetChannelPropertiesParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdFifoSetChannelProperties(pDevice, pSetChannelPropertiesParams) deviceCtrlCmdFifoSetChannelProperties_IMPL(pDevice, pSetChannelPropertiesParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdHostGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_HOST_GET_CAPS_PARAMS *pHostCapsParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdHostGetCaps(struct Device *pDevice, NV0080_CTRL_HOST_GET_CAPS_PARAMS *pHostCapsParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdHostGetCaps(pDevice, pHostCapsParams) deviceCtrlCmdHostGetCaps_IMPL(pDevice, pHostCapsParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdKPerfCudaLimitSetControl_IMPL(struct Device *pDevice, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdKPerfCudaLimitSetControl(struct Device *pDevice, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdKPerfCudaLimitSetControl(pDevice, pParams) deviceCtrlCmdKPerfCudaLimitSetControl_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdInternalPerfCudaLimitSetControl_IMPL(struct Device *pDevice, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdInternalPerfCudaLimitSetControl(struct Device *pDevice, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdInternalPerfCudaLimitSetControl(pDevice, pParams) deviceCtrlCmdInternalPerfCudaLimitSetControl_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdInternalPerfCudaLimitDisable_IMPL(struct Device *pDevice);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdInternalPerfCudaLimitDisable(struct Device *pDevice) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdInternalPerfCudaLimitDisable(pDevice) deviceCtrlCmdInternalPerfCudaLimitDisable_IMPL(pDevice)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuGetClasslist_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *pClassListParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuGetClasslist(struct Device *pDevice, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *pClassListParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuGetClasslist(pDevice, pClassListParams) deviceCtrlCmdGpuGetClasslist_IMPL(pDevice, pClassListParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuGetClasslistV2_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_CLASSLIST_V2_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuGetClasslistV2(struct Device *pDevice, NV0080_CTRL_GPU_GET_CLASSLIST_V2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuGetClasslistV2(pDevice, pParams) deviceCtrlCmdGpuGetClasslistV2_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuGetNumSubdevices_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS *pSubDeviceCountParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuGetNumSubdevices(struct Device *pDevice, NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS *pSubDeviceCountParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuGetNumSubdevices(pDevice, pSubDeviceCountParams) deviceCtrlCmdGpuGetNumSubdevices_IMPL(pDevice, pSubDeviceCountParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuModifyGpuSwStatePersistence_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_MODIFY_SW_STATE_PERSISTENCE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuModifyGpuSwStatePersistence(struct Device *pDevice, NV0080_CTRL_GPU_MODIFY_SW_STATE_PERSISTENCE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuModifyGpuSwStatePersistence(pDevice, pParams) deviceCtrlCmdGpuModifyGpuSwStatePersistence_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuQueryGpuSwStatePersistence_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_QUERY_SW_STATE_PERSISTENCE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuQueryGpuSwStatePersistence(struct Device *pDevice, NV0080_CTRL_GPU_QUERY_SW_STATE_PERSISTENCE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuQueryGpuSwStatePersistence(pDevice, pParams) deviceCtrlCmdGpuQueryGpuSwStatePersistence_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuGetVirtualizationMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_VIRTUALIZATION_MODE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuGetVirtualizationMode(struct Device *pDevice, NV0080_CTRL_GPU_GET_VIRTUALIZATION_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuGetVirtualizationMode(pDevice, pParams) deviceCtrlCmdGpuGetVirtualizationMode_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuSetVgpuVfBar1Size_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_SET_VGPU_VF_BAR1_SIZE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuSetVgpuVfBar1Size(struct Device *pDevice, NV0080_CTRL_GPU_SET_VGPU_VF_BAR1_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuSetVgpuVfBar1Size(pDevice, pParams) deviceCtrlCmdGpuSetVgpuVfBar1Size_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuGetSparseTextureComputeMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuGetSparseTextureComputeMode(struct Device *pDevice, NV0080_CTRL_GPU_GET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuGetSparseTextureComputeMode(pDevice, pModeParams) deviceCtrlCmdGpuGetSparseTextureComputeMode_IMPL(pDevice, pModeParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuSetSparseTextureComputeMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_SET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuSetSparseTextureComputeMode(struct Device *pDevice, NV0080_CTRL_GPU_SET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuSetSparseTextureComputeMode(pDevice, pModeParams) deviceCtrlCmdGpuSetSparseTextureComputeMode_IMPL(pDevice, pModeParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuGetVgxCaps_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_VGX_CAPS_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuGetVgxCaps(struct Device *pDevice, NV0080_CTRL_GPU_GET_VGX_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuGetVgxCaps(pDevice, pParams) deviceCtrlCmdGpuGetVgxCaps_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuVirtualizationSwitchToVga_IMPL(struct Device *pDevice);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuVirtualizationSwitchToVga(struct Device *pDevice) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuVirtualizationSwitchToVga(pDevice) deviceCtrlCmdGpuVirtualizationSwitchToVga_IMPL(pDevice)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuSetVgpuHeterogeneousMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuSetVgpuHeterogeneousMode(struct Device *pDevice, NV0080_CTRL_GPU_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuSetVgpuHeterogeneousMode(pDevice, pParams) deviceCtrlCmdGpuSetVgpuHeterogeneousMode_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuGetVgpuHeterogeneousMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuGetVgpuHeterogeneousMode(struct Device *pDevice, NV0080_CTRL_GPU_GET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuGetVgpuHeterogeneousMode(pDevice, pParams) deviceCtrlCmdGpuGetVgpuHeterogeneousMode_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuGetSriovCaps_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuGetSriovCaps(struct Device *pDevice, NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuGetSriovCaps(pDevice, pParams) deviceCtrlCmdGpuGetSriovCaps_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdGpuGetFindSubDeviceHandle_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_FIND_SUBDEVICE_HANDLE_PARAM *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdGpuGetFindSubDeviceHandle(struct Device *pDevice, NV0080_CTRL_GPU_FIND_SUBDEVICE_HANDLE_PARAM *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdGpuGetFindSubDeviceHandle(pDevice, pParams) deviceCtrlCmdGpuGetFindSubDeviceHandle_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdOsUnixVTSwitch_IMPL(struct Device *pDevice, NV0080_CTRL_OS_UNIX_VT_SWITCH_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdOsUnixVTSwitch(struct Device *pDevice, NV0080_CTRL_OS_UNIX_VT_SWITCH_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdOsUnixVTSwitch(pDevice, pParams) deviceCtrlCmdOsUnixVTSwitch_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled

NV_STATUS deviceCtrlCmdOsUnixVTGetFBInfo_IMPL(struct Device *pDevice, NV0080_CTRL_OS_UNIX_VT_GET_FB_INFO_PARAMS *pParams);
#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceCtrlCmdOsUnixVTGetFBInfo(struct Device *pDevice, NV0080_CTRL_OS_UNIX_VT_GET_FB_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_device_h_disabled
#define deviceCtrlCmdOsUnixVTGetFBInfo(pDevice, pParams) deviceCtrlCmdOsUnixVTGetFBInfo_IMPL(pDevice, pParams)
#endif // __nvoc_device_h_disabled


// Wrapper macros for halified functions
#define deviceControl_FNPTR(pResource) pResource->__nvoc_metadata_ptr->vtable.__deviceControl__
#define deviceControl(pResource, pCallContext, pParams) deviceControl_DISPATCH(pResource, pCallContext, pParams)
#define deviceInternalControlForward_FNPTR(pDevice) pDevice->__nvoc_metadata_ptr->vtable.__deviceInternalControlForward__
#define deviceInternalControlForward(pDevice, command, pParams, size) deviceInternalControlForward_DISPATCH(pDevice, command, pParams, size)
#define deviceCtrlCmdDmaFlush_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaFlush__
#define deviceCtrlCmdDmaFlush(pDevice, flushParams) deviceCtrlCmdDmaFlush_DISPATCH(pDevice, flushParams)
#define deviceCtrlCmdDmaFlush_HAL(pDevice, flushParams) deviceCtrlCmdDmaFlush_DISPATCH(pDevice, flushParams)
#define deviceCtrlCmdFifoGetEngineContextProperties_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoGetEngineContextProperties__
#define deviceCtrlCmdFifoGetEngineContextProperties(pDevice, pParams) deviceCtrlCmdFifoGetEngineContextProperties_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdFifoGetEngineContextProperties_HAL(pDevice, pParams) deviceCtrlCmdFifoGetEngineContextProperties_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdFifoGetLatencyBufferSize_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoGetLatencyBufferSize__
#define deviceCtrlCmdFifoGetLatencyBufferSize(pDevice, pGetLatencyBufferSizeParams) deviceCtrlCmdFifoGetLatencyBufferSize_DISPATCH(pDevice, pGetLatencyBufferSizeParams)
#define deviceCtrlCmdFifoGetLatencyBufferSize_HAL(pDevice, pGetLatencyBufferSizeParams) deviceCtrlCmdFifoGetLatencyBufferSize_DISPATCH(pDevice, pGetLatencyBufferSizeParams)
#define deviceCtrlCmdFifoIdleChannels_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoIdleChannels__
#define deviceCtrlCmdFifoIdleChannels(pDevice, pParams) deviceCtrlCmdFifoIdleChannels_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdFifoIdleChannels_HAL(pDevice, pParams) deviceCtrlCmdFifoIdleChannels_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdHostGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdHostGetCapsV2__
#define deviceCtrlCmdHostGetCapsV2(pDevice, pHostCapsParamsV2) deviceCtrlCmdHostGetCapsV2_DISPATCH(pDevice, pHostCapsParamsV2)
#define deviceCtrlCmdHostGetCapsV2_HAL(pDevice, pHostCapsParamsV2) deviceCtrlCmdHostGetCapsV2_DISPATCH(pDevice, pHostCapsParamsV2)
#define deviceCtrlCmdGpuGetBrandCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetBrandCaps__
#define deviceCtrlCmdGpuGetBrandCaps(pDevice, pParams) deviceCtrlCmdGpuGetBrandCaps_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuGetBrandCaps_HAL(pDevice, pParams) deviceCtrlCmdGpuGetBrandCaps_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdMsencGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdMsencGetCapsV2__
#define deviceCtrlCmdMsencGetCapsV2(pDevice, pMsencCapsParams) deviceCtrlCmdMsencGetCapsV2_DISPATCH(pDevice, pMsencCapsParams)
#define deviceCtrlCmdMsencGetCapsV2_HAL(pDevice, pMsencCapsParams) deviceCtrlCmdMsencGetCapsV2_DISPATCH(pDevice, pMsencCapsParams)
#define deviceCtrlCmdBspGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdBspGetCapsV2__
#define deviceCtrlCmdBspGetCapsV2(pDevice, pBspCapParams) deviceCtrlCmdBspGetCapsV2_DISPATCH(pDevice, pBspCapParams)
#define deviceCtrlCmdBspGetCapsV2_HAL(pDevice, pBspCapParams) deviceCtrlCmdBspGetCapsV2_DISPATCH(pDevice, pBspCapParams)
#define deviceCtrlCmdNvjpgGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdNvjpgGetCapsV2__
#define deviceCtrlCmdNvjpgGetCapsV2(pDevice, pNvjpgCapsParams) deviceCtrlCmdNvjpgGetCapsV2_DISPATCH(pDevice, pNvjpgCapsParams)
#define deviceCtrlCmdNvjpgGetCapsV2_HAL(pDevice, pNvjpgCapsParams) deviceCtrlCmdNvjpgGetCapsV2_DISPATCH(pDevice, pNvjpgCapsParams)
#define deviceMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define deviceMap(pGpuResource, pCallContext, pParams, pCpuMapping) deviceMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define deviceUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define deviceUnmap(pGpuResource, pCallContext, pCpuMapping) deviceUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define deviceShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define deviceShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) deviceShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define deviceGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define deviceGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) deviceGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define deviceGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define deviceGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) deviceGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define deviceGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define deviceGetInternalObjectHandle(pGpuResource) deviceGetInternalObjectHandle_DISPATCH(pGpuResource)
#define deviceAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define deviceAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) deviceAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define deviceGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define deviceGetMemInterMapParams(pRmResource, pParams) deviceGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define deviceCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define deviceCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) deviceCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define deviceGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define deviceGetMemoryMappingDescriptor(pRmResource, ppMemDesc) deviceGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define deviceControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define deviceControlSerialization_Prologue(pResource, pCallContext, pParams) deviceControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define deviceControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define deviceControlSerialization_Epilogue(pResource, pCallContext, pParams) deviceControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define deviceControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define deviceControl_Prologue(pResource, pCallContext, pParams) deviceControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define deviceControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define deviceControl_Epilogue(pResource, pCallContext, pParams) deviceControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define deviceCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define deviceCanCopy(pResource) deviceCanCopy_DISPATCH(pResource)
#define deviceIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define deviceIsDuplicate(pResource, hMemory, pDuplicate) deviceIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define devicePreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define devicePreDestruct(pResource) devicePreDestruct_DISPATCH(pResource)
#define deviceControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define deviceControlFilter(pResource, pCallContext, pParams) deviceControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define deviceIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define deviceIsPartialUnmapSupported(pResource) deviceIsPartialUnmapSupported_DISPATCH(pResource)
#define deviceMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define deviceMapTo(pResource, pParams) deviceMapTo_DISPATCH(pResource, pParams)
#define deviceUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define deviceUnmapFrom(pResource, pParams) deviceUnmapFrom_DISPATCH(pResource, pParams)
#define deviceGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define deviceGetRefCount(pResource) deviceGetRefCount_DISPATCH(pResource)
#define deviceAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define deviceAddAdditionalDependants(pClient, pResource, pReference) deviceAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS deviceControl_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS deviceInternalControlForward_DISPATCH(struct Device *pDevice, NvU32 command, void *pParams, NvU32 size) {
    return pDevice->__nvoc_metadata_ptr->vtable.__deviceInternalControlForward__(pDevice, command, pParams, size);
}

static inline NV_STATUS deviceCtrlCmdDmaFlush_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_FLUSH_PARAMS *flushParams) {
    return pDevice->__deviceCtrlCmdDmaFlush__(pDevice, flushParams);
}

static inline NV_STATUS deviceCtrlCmdFifoGetEngineContextProperties_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdFifoGetEngineContextProperties__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdFifoGetLatencyBufferSize_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS *pGetLatencyBufferSizeParams) {
    return pDevice->__deviceCtrlCmdFifoGetLatencyBufferSize__(pDevice, pGetLatencyBufferSizeParams);
}

static inline NV_STATUS deviceCtrlCmdFifoIdleChannels_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdFifoIdleChannels__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdHostGetCapsV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_HOST_GET_CAPS_V2_PARAMS *pHostCapsParamsV2) {
    return pDevice->__deviceCtrlCmdHostGetCapsV2__(pDevice, pHostCapsParamsV2);
}

static inline NV_STATUS deviceCtrlCmdGpuGetBrandCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuGetBrandCaps__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdMsencGetCapsV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS *pMsencCapsParams) {
    return pDevice->__deviceCtrlCmdMsencGetCapsV2__(pDevice, pMsencCapsParams);
}

static inline NV_STATUS deviceCtrlCmdBspGetCapsV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_BSP_GET_CAPS_PARAMS_V2 *pBspCapParams) {
    return pDevice->__deviceCtrlCmdBspGetCapsV2__(pDevice, pBspCapParams);
}

static inline NV_STATUS deviceCtrlCmdNvjpgGetCapsV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS *pNvjpgCapsParams) {
    return pDevice->__deviceCtrlCmdNvjpgGetCapsV2__(pDevice, pNvjpgCapsParams);
}

static inline NV_STATUS deviceMap_DISPATCH(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__deviceMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS deviceUnmap_DISPATCH(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__deviceUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool deviceShareCallback_DISPATCH(struct Device *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__deviceShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS deviceGetRegBaseOffsetAndSize_DISPATCH(struct Device *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__deviceGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS deviceGetMapAddrSpace_DISPATCH(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__deviceGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvHandle deviceGetInternalObjectHandle_DISPATCH(struct Device *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__deviceGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool deviceAccessCallback_DISPATCH(struct Device *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS deviceGetMemInterMapParams_DISPATCH(struct Device *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__deviceGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS deviceCheckMemInterUnmap_DISPATCH(struct Device *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__deviceCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS deviceGetMemoryMappingDescriptor_DISPATCH(struct Device *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__deviceGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS deviceControlSerialization_Prologue_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void deviceControlSerialization_Epilogue_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__deviceControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS deviceControl_Prologue_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void deviceControl_Epilogue_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__deviceControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool deviceCanCopy_DISPATCH(struct Device *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceCanCopy__(pResource);
}

static inline NV_STATUS deviceIsDuplicate_DISPATCH(struct Device *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void devicePreDestruct_DISPATCH(struct Device *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__devicePreDestruct__(pResource);
}

static inline NV_STATUS deviceControlFilter_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool deviceIsPartialUnmapSupported_DISPATCH(struct Device *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS deviceMapTo_DISPATCH(struct Device *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceMapTo__(pResource, pParams);
}

static inline NV_STATUS deviceUnmapFrom_DISPATCH(struct Device *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceUnmapFrom__(pResource, pParams);
}

static inline NvU32 deviceGetRefCount_DISPATCH(struct Device *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__deviceGetRefCount__(pResource);
}

static inline void deviceAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Device *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__deviceAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS deviceControl_IMPL(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS deviceInternalControlForward_IMPL(struct Device *pDevice, NvU32 command, void *pParams, NvU32 size);

NV_STATUS deviceCtrlCmdBifAspmFeatureSupported_IMPL(struct Device *pDevice, NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS *pBifAspmParams);

NV_STATUS deviceCtrlCmdBifAspmCyaUpdate_IMPL(struct Device *pDevice, NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS *pBifAspmCyaParams);

NV_STATUS deviceCtrlCmdBifGetPciePowerControlMask_IMPL(struct Device *pDevice, NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS *pBifPciePowerControlParams);

NV_STATUS deviceCtrlCmdDmaGetPteInfo_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaUpdatePde2_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaSetPageDirectory_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaUnsetPageDirectory_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaFlush_VF(struct Device *pDevice, NV0080_CTRL_DMA_FLUSH_PARAMS *flushParams);

static inline NV_STATUS deviceCtrlCmdDmaFlush_5baef9(struct Device *pDevice, NV0080_CTRL_DMA_FLUSH_PARAMS *flushParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS deviceCtrlCmdDmaAdvSchedGetVaCaps_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaGetPdeInfo_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaSetPteInfo_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaInvalidateTLB_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_GET_CAPS_PARAMS *pDmaCapsParams);

NV_STATUS deviceCtrlCmdDmaSetVASpaceSize_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaEnablePrivilegedRange_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdDmaSetDefaultVASpace_IMPL(struct Device *pDevice, NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdKGrGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_GR_GET_CAPS_PARAMS *pParams);

NV_STATUS deviceCtrlCmdKGrGetCapsV2_IMPL(struct Device *pDevice, NV0080_CTRL_GR_GET_CAPS_V2_PARAMS *pParams);

NV_STATUS deviceCtrlCmdKGrGetInfo_IMPL(struct Device *pDevice, NV0080_CTRL_GR_GET_INFO_PARAMS *pParams);

NV_STATUS deviceCtrlCmdKGrGetInfoV2_IMPL(struct Device *pDevice, NV0080_CTRL_GR_GET_INFO_V2_PARAMS *pParams);

NV_STATUS deviceCtrlCmdKGrGetTpcPartitionMode_IMPL(struct Device *pDevice, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdKGrSetTpcPartitionMode_IMPL(struct Device *pDevice, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdKGrInternalInitBug4208224War_IMPL(struct Device *pDevice, NV0080_CTRL_INTERNAL_KGR_INIT_BUG4208224_WAR_PARAMS *pParams);

NV_STATUS deviceCtrlCmdFbGetCompbitStoreInfo_IMPL(struct Device *pDevice, NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS *pCompbitStoreParams);

NV_STATUS deviceCtrlCmdFbSetZbcReferenced_IMPL(struct Device *pDevice, NV0080_CTRL_INTERNAL_MEMSYS_SET_ZBC_REFERENCED_PARAMS *pParams);

NV_STATUS deviceCtrlCmdFbGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_PARAMS *pFbCapsParams);

NV_STATUS deviceCtrlCmdFbGetCapsV2_IMPL(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_V2_PARAMS *pFbCapsParams);

NV_STATUS deviceCtrlCmdSetDefaultVidmemPhysicality_IMPL(struct Device *pDevice, NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS *pParams);

NV_STATUS deviceCtrlCmdFifoGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *pFifoCapsParams);

NV_STATUS deviceCtrlCmdFifoGetCapsV2_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS *pFifoCapsParams);

NV_STATUS deviceCtrlCmdFifoGetEngineContextProperties_VF(struct Device *pDevice, NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams);

static inline NV_STATUS deviceCtrlCmdFifoGetEngineContextProperties_92bfc3(struct Device *pDevice, NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS deviceCtrlCmdFifoStopRunlist_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS *pStopRunlistParams);

NV_STATUS deviceCtrlCmdFifoStartRunlist_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_START_RUNLIST_PARAMS *pStartRunlistParams);

NV_STATUS deviceCtrlCmdFifoGetChannelList_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *pChannelParams);

NV_STATUS deviceCtrlCmdFifoGetLatencyBufferSize_VF(struct Device *pDevice, NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS *pGetLatencyBufferSizeParams);

static inline NV_STATUS deviceCtrlCmdFifoGetLatencyBufferSize_92bfc3(struct Device *pDevice, NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS *pGetLatencyBufferSizeParams) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS deviceCtrlCmdFifoSetChannelProperties_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS *pSetChannelPropertiesParams);

static inline NV_STATUS deviceCtrlCmdFifoIdleChannels_46f6a7(struct Device *pDevice, NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS *pParams) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS deviceCtrlCmdFifoIdleChannels_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS *pParams);

NV_STATUS deviceCtrlCmdHostGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_HOST_GET_CAPS_PARAMS *pHostCapsParams);

NV_STATUS deviceCtrlCmdHostGetCapsV2_SOC(struct Device *pDevice, NV0080_CTRL_HOST_GET_CAPS_V2_PARAMS *pHostCapsParamsV2);

NV_STATUS deviceCtrlCmdHostGetCapsV2_IMPL(struct Device *pDevice, NV0080_CTRL_HOST_GET_CAPS_V2_PARAMS *pHostCapsParamsV2);

NV_STATUS deviceCtrlCmdKPerfCudaLimitSetControl_IMPL(struct Device *pDevice, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *pParams);

NV_STATUS deviceCtrlCmdInternalPerfCudaLimitSetControl_IMPL(struct Device *pDevice, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *pParams);

NV_STATUS deviceCtrlCmdInternalPerfCudaLimitDisable_IMPL(struct Device *pDevice);

NV_STATUS deviceCtrlCmdGpuGetClasslist_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *pClassListParams);

NV_STATUS deviceCtrlCmdGpuGetClasslistV2_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_CLASSLIST_V2_PARAMS *pParams);

NV_STATUS deviceCtrlCmdGpuGetNumSubdevices_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS *pSubDeviceCountParams);

NV_STATUS deviceCtrlCmdGpuModifyGpuSwStatePersistence_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_MODIFY_SW_STATE_PERSISTENCE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdGpuQueryGpuSwStatePersistence_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_QUERY_SW_STATE_PERSISTENCE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdGpuGetVirtualizationMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_VIRTUALIZATION_MODE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdGpuSetVgpuVfBar1Size_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_SET_VGPU_VF_BAR1_SIZE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdGpuGetSparseTextureComputeMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams);

NV_STATUS deviceCtrlCmdGpuSetSparseTextureComputeMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_SET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams);

NV_STATUS deviceCtrlCmdGpuGetVgxCaps_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_VGX_CAPS_PARAMS *pParams);

NV_STATUS deviceCtrlCmdGpuGetBrandCaps_VF(struct Device *pDevice, NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *pParams);

static inline NV_STATUS deviceCtrlCmdGpuGetBrandCaps_5baef9(struct Device *pDevice, NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *pParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS deviceCtrlCmdGpuVirtualizationSwitchToVga_IMPL(struct Device *pDevice);

NV_STATUS deviceCtrlCmdGpuSetVgpuHeterogeneousMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdGpuGetVgpuHeterogeneousMode_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams);

NV_STATUS deviceCtrlCmdGpuGetSriovCaps_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *pParams);

NV_STATUS deviceCtrlCmdGpuGetFindSubDeviceHandle_IMPL(struct Device *pDevice, NV0080_CTRL_GPU_FIND_SUBDEVICE_HANDLE_PARAM *pParams);

NV_STATUS deviceCtrlCmdMsencGetCapsV2_VF(struct Device *pDevice, NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS *pMsencCapsParams);

static inline NV_STATUS deviceCtrlCmdMsencGetCapsV2_5baef9(struct Device *pDevice, NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS *pMsencCapsParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS deviceCtrlCmdBspGetCapsV2_VF(struct Device *pDevice, NV0080_CTRL_BSP_GET_CAPS_PARAMS_V2 *pBspCapParams);

static inline NV_STATUS deviceCtrlCmdBspGetCapsV2_5baef9(struct Device *pDevice, NV0080_CTRL_BSP_GET_CAPS_PARAMS_V2 *pBspCapParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS deviceCtrlCmdNvjpgGetCapsV2_VF(struct Device *pDevice, NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS *pNvjpgCapsParams);

static inline NV_STATUS deviceCtrlCmdNvjpgGetCapsV2_c04480(struct Device *pDevice, NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS *pNvjpgCapsParams) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

NV_STATUS deviceCtrlCmdOsUnixVTSwitch_IMPL(struct Device *pDevice, NV0080_CTRL_OS_UNIX_VT_SWITCH_PARAMS *pParams);

NV_STATUS deviceCtrlCmdOsUnixVTGetFBInfo_IMPL(struct Device *pDevice, NV0080_CTRL_OS_UNIX_VT_GET_FB_INFO_PARAMS *pParams);

#undef PRIVATE_FIELD


// ****************************************************************************
//                            Deprecated Definitions
// ****************************************************************************

/**
 * WARNING: This function is deprecated and use is *strongly* discouraged
 * (especially for new code!)
 *
 * From the function name (CliSetGpuContext) it appears as a simple accessor but
 * violates expectations by modifying the SLI BC threadstate (calls to
 * GPU_RES_SET_THREAD_BC_STATE). This can be dangerous if not carefully managed
 * by the caller.
 *
 * Instead of using this routine, please use deviceGetByHandle then call
 * GPU_RES_GET_GPU, GPU_RES_GET_GPUGRP, GPU_RES_SET_THREAD_BC_STATE as needed.
 *
 * Note that GPU_RES_GET_GPU supports returning a pGpu for both pDevice,
 * pSubdevice, the base pResource type, and any resource that inherits from
 * GpuResource. That is, instead of using CliSetGpuContext or
 * CliSetSubDeviceContext, please use following pattern to look up the pGpu:
 *
 * OBJGPU *pGpu = GPU_RES_GET_GPU(pResource or pResourceRef->pResource)
 *
 * To set the threadstate, please use:
 *
 * GPU_RES_SET_THREAD_BC_STATE(pResource or pResourceRef->pResource);
 */
NV_STATUS CliSetGpuContext(NvHandle, NvHandle, OBJGPU **, struct OBJGPUGRP **);

/**
 * WARNING: This function is deprecated! Please use gpuGetByRef()
 */
OBJGPU *CliGetGpuFromContext(RsResourceRef *pContextRef, NvBool *pbBroadcast);

/**
 * WARNING: This function is deprecated! Please use gpuGetByHandle()
 */
OBJGPU *CliGetGpuFromHandle(NvHandle hClient, NvHandle hResource, NvBool *pbBroadcast);


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DEVICE_NVOC_H_
