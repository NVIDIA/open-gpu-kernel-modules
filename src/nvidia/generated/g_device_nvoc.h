
#ifndef _G_DEVICE_NVOC_H_
#define _G_DEVICE_NVOC_H_
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


struct Device {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct GpuResource __nvoc_base_GpuResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct Device *__nvoc_pbase_Device;    // device

    // Vtable with 88 per-object function pointers
    NV_STATUS (*__deviceControl__)(struct Device * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (gpures)
    NV_STATUS (*__deviceInternalControlForward__)(struct Device * /*this*/, NvU32, void *, NvU32);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__deviceCtrlCmdBifGetDmaBaseSysmemAddr__)(struct Device * /*this*/, NV0080_CTRL_BIF_GET_DMA_BASE_SYSMEM_ADDR_PARAMS *);  // exported (id=0x800103)
    NV_STATUS (*__deviceCtrlCmdBifAspmFeatureSupported__)(struct Device * /*this*/, NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS *);  // exported (id=0x800104)
    NV_STATUS (*__deviceCtrlCmdBifAspmCyaUpdate__)(struct Device * /*this*/, NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS *);  // exported (id=0x800105)
    NV_STATUS (*__deviceCtrlCmdBifGetPciePowerControlMask__)(struct Device * /*this*/, NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS *);  // exported (id=0x800106)
    NV_STATUS (*__deviceCtrlCmdDmaGetPteInfo__)(struct Device * /*this*/, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *);  // exported (id=0x801801)
    NV_STATUS (*__deviceCtrlCmdDmaUpdatePde2__)(struct Device * /*this*/, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *);  // exported (id=0x80180f)
    NV_STATUS (*__deviceCtrlCmdDmaSetPageDirectory__)(struct Device * /*this*/, NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *);  // exported (id=0x801813)
    NV_STATUS (*__deviceCtrlCmdDmaUnsetPageDirectory__)(struct Device * /*this*/, NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *);  // exported (id=0x801814)
    NV_STATUS (*__deviceCtrlCmdDmaFlush__)(struct Device * /*this*/, NV0080_CTRL_DMA_FLUSH_PARAMS *);  // halified (2 hals) exported (id=0x801805) body
    NV_STATUS (*__deviceCtrlCmdDmaAdvSchedGetVaCaps__)(struct Device * /*this*/, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *);  // exported (id=0x801806)
    NV_STATUS (*__deviceCtrlCmdDmaGetPdeInfo__)(struct Device * /*this*/, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *);  // exported (id=0x801809)
    NV_STATUS (*__deviceCtrlCmdDmaSetPteInfo__)(struct Device * /*this*/, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *);  // exported (id=0x80180a)
    NV_STATUS (*__deviceCtrlCmdDmaInvalidateTLB__)(struct Device * /*this*/, NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS *);  // exported (id=0x80180c)
    NV_STATUS (*__deviceCtrlCmdDmaGetCaps__)(struct Device * /*this*/, NV0080_CTRL_DMA_GET_CAPS_PARAMS *);  // exported (id=0x80180d)
    NV_STATUS (*__deviceCtrlCmdDmaSetVASpaceSize__)(struct Device * /*this*/, NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS *);  // exported (id=0x80180e)
    NV_STATUS (*__deviceCtrlCmdDmaEnablePrivilegedRange__)(struct Device * /*this*/, NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS *);  // exported (id=0x801810)
    NV_STATUS (*__deviceCtrlCmdDmaSetDefaultVASpace__)(struct Device * /*this*/, NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *);  // exported (id=0x801812)
    NV_STATUS (*__deviceCtrlCmdKGrGetCaps__)(struct Device * /*this*/, NV0080_CTRL_GR_GET_CAPS_PARAMS *);  // exported (id=0x801102)
    NV_STATUS (*__deviceCtrlCmdKGrGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_GR_GET_CAPS_V2_PARAMS *);  // exported (id=0x801109)
    NV_STATUS (*__deviceCtrlCmdKGrGetInfo__)(struct Device * /*this*/, NV0080_CTRL_GR_GET_INFO_PARAMS *);  // exported (id=0x801104)
    NV_STATUS (*__deviceCtrlCmdKGrGetInfoV2__)(struct Device * /*this*/, NV0080_CTRL_GR_GET_INFO_V2_PARAMS *);  // exported (id=0x801110)
    NV_STATUS (*__deviceCtrlCmdKGrGetTpcPartitionMode__)(struct Device * /*this*/, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *);  // exported (id=0x801107)
    NV_STATUS (*__deviceCtrlCmdKGrSetTpcPartitionMode__)(struct Device * /*this*/, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *);  // exported (id=0x801108)
    NV_STATUS (*__deviceCtrlCmdFbGetCompbitStoreInfo__)(struct Device * /*this*/, NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS *);  // exported (id=0x801306)
    NV_STATUS (*__deviceCtrlCmdFbGetCaps__)(struct Device * /*this*/, NV0080_CTRL_FB_GET_CAPS_PARAMS *);  // exported (id=0x801301)
    NV_STATUS (*__deviceCtrlCmdFbGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_FB_GET_CAPS_V2_PARAMS *);  // exported (id=0x801307)
    NV_STATUS (*__deviceCtrlCmdSetDefaultVidmemPhysicality__)(struct Device * /*this*/, NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS *);  // exported (id=0x801308)
    NV_STATUS (*__deviceCtrlCmdFifoGetCaps__)(struct Device * /*this*/, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *);  // exported (id=0x801701)
    NV_STATUS (*__deviceCtrlCmdFifoGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS *);  // exported (id=0x801713)
    NV_STATUS (*__deviceCtrlCmdFifoStartSelectedChannels__)(struct Device * /*this*/, NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *);  // exported (id=0x801705)
    NV_STATUS (*__deviceCtrlCmdFifoGetEngineContextProperties__)(struct Device * /*this*/, NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *);  // halified (2 hals) exported (id=0x801707) body
    NV_STATUS (*__deviceCtrlCmdFifoStopRunlist__)(struct Device * /*this*/, NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS *);  // exported (id=0x801711)
    NV_STATUS (*__deviceCtrlCmdFifoStartRunlist__)(struct Device * /*this*/, NV0080_CTRL_FIFO_START_RUNLIST_PARAMS *);  // exported (id=0x801712)
    NV_STATUS (*__deviceCtrlCmdFifoGetChannelList__)(struct Device * /*this*/, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *);  // exported (id=0x80170d)
    NV_STATUS (*__deviceCtrlCmdFifoGetLatencyBufferSize__)(struct Device * /*this*/, NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS *);  // halified (2 hals) exported (id=0x80170e) body
    NV_STATUS (*__deviceCtrlCmdFifoSetChannelProperties__)(struct Device * /*this*/, NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS *);  // exported (id=0x80170f)
    NV_STATUS (*__deviceCtrlCmdFifoIdleChannels__)(struct Device * /*this*/, NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS *);  // halified (2 hals) exported (id=0x801714) body
    NV_STATUS (*__deviceCtrlCmdHostGetCaps__)(struct Device * /*this*/, NV0080_CTRL_HOST_GET_CAPS_PARAMS *);  // exported (id=0x801401)
    NV_STATUS (*__deviceCtrlCmdHostGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_HOST_GET_CAPS_V2_PARAMS *);  // exported (id=0x801402)
    NV_STATUS (*__deviceCtrlCmdKPerfCudaLimitSetControl__)(struct Device * /*this*/, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *);  // exported (id=0x801909)
    NV_STATUS (*__deviceCtrlCmdInternalPerfCudaLimitSetControl__)(struct Device * /*this*/, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *);  // exported (id=0x802009)
    NV_STATUS (*__deviceCtrlCmdInternalPerfCudaLimitDisable__)(struct Device * /*this*/);  // exported (id=0x802004)
    NV_STATUS (*__deviceCtrlCmdGpuGetClasslist__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *);  // exported (id=0x800201)
    NV_STATUS (*__deviceCtrlCmdGpuGetClasslistV2__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_CLASSLIST_V2_PARAMS *);  // exported (id=0x800292)
    NV_STATUS (*__deviceCtrlCmdGpuGetNumSubdevices__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS *);  // exported (id=0x800280)
    NV_STATUS (*__deviceCtrlCmdGpuModifyGpuSwStatePersistence__)(struct Device * /*this*/, NV0080_CTRL_GPU_MODIFY_SW_STATE_PERSISTENCE_PARAMS *);  // exported (id=0x800287)
    NV_STATUS (*__deviceCtrlCmdGpuQueryGpuSwStatePersistence__)(struct Device * /*this*/, NV0080_CTRL_GPU_QUERY_SW_STATE_PERSISTENCE_PARAMS *);  // exported (id=0x800288)
    NV_STATUS (*__deviceCtrlCmdGpuGetVirtualizationMode__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_VIRTUALIZATION_MODE_PARAMS *);  // exported (id=0x800289)
    NV_STATUS (*__deviceCtrlCmdGpuSetVgpuVfBar1Size__)(struct Device * /*this*/, NV0080_CTRL_GPU_SET_VGPU_VF_BAR1_SIZE_PARAMS *);  // exported (id=0x800296)
    NV_STATUS (*__deviceCtrlCmdGpuGetSparseTextureComputeMode__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *);  // exported (id=0x80028c)
    NV_STATUS (*__deviceCtrlCmdGpuSetSparseTextureComputeMode__)(struct Device * /*this*/, NV0080_CTRL_GPU_SET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *);  // exported (id=0x80028d)
    NV_STATUS (*__deviceCtrlCmdGpuGetVgxCaps__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_VGX_CAPS_PARAMS *);  // exported (id=0x80028e)
    NV_STATUS (*__deviceCtrlCmdGpuGetBrandCaps__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *);  // halified (2 hals) exported (id=0x800294) body
    NV_STATUS (*__deviceCtrlCmdGpuVirtualizationSwitchToVga__)(struct Device * /*this*/);  // exported (id=0x800290)
    NV_STATUS (*__deviceCtrlCmdGpuSetVgpuHeterogeneousMode__)(struct Device * /*this*/, NV0080_CTRL_GPU_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *);  // exported (id=0x800297)
    NV_STATUS (*__deviceCtrlCmdGpuGetVgpuHeterogeneousMode__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_VGPU_HETEROGENEOUS_MODE_PARAMS *);  // exported (id=0x800298)
    NV_STATUS (*__deviceCtrlCmdGpuGetSriovCaps__)(struct Device * /*this*/, NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *);  // exported (id=0x800291)
    NV_STATUS (*__deviceCtrlCmdGpuGetFindSubDeviceHandle__)(struct Device * /*this*/, NV0080_CTRL_GPU_FIND_SUBDEVICE_HANDLE_PARAM *);  // exported (id=0x800293)
    NV_STATUS (*__deviceCtrlCmdMsencGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_MSENC_GET_CAPS_V2_PARAMS *);  // halified (2 hals) exported (id=0x801b02) body
    NV_STATUS (*__deviceCtrlCmdBspGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_BSP_GET_CAPS_PARAMS_V2 *);  // halified (2 hals) exported (id=0x801c02) body
    NV_STATUS (*__deviceCtrlCmdNvjpgGetCapsV2__)(struct Device * /*this*/, NV0080_CTRL_NVJPG_GET_CAPS_V2_PARAMS *);  // halified (2 hals) exported (id=0x801f02) body
    NV_STATUS (*__deviceCtrlCmdOsUnixVTSwitch__)(struct Device * /*this*/, NV0080_CTRL_OS_UNIX_VT_SWITCH_PARAMS *);  // exported (id=0x801e01)
    NV_STATUS (*__deviceCtrlCmdOsUnixVTGetFBInfo__)(struct Device * /*this*/, NV0080_CTRL_OS_UNIX_VT_GET_FB_INFO_PARAMS *);  // exported (id=0x801e02)
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
#define __dynamicCast_Device(pThis) ((Device*)NULL)
#else //__nvoc_device_h_disabled
#define __dynamicCast_Device(pThis) \
    ((Device*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Device)))
#endif //__nvoc_device_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Device(Device**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Device(Device**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_Device(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_Device((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define deviceControl_FNPTR(pResource) pResource->__deviceControl__
#define deviceControl(pResource, pCallContext, pParams) deviceControl_DISPATCH(pResource, pCallContext, pParams)
#define deviceInternalControlForward_FNPTR(pDevice) pDevice->__deviceInternalControlForward__
#define deviceInternalControlForward(pDevice, command, pParams, size) deviceInternalControlForward_DISPATCH(pDevice, command, pParams, size)
#define deviceCtrlCmdBifGetDmaBaseSysmemAddr_FNPTR(pDevice) pDevice->__deviceCtrlCmdBifGetDmaBaseSysmemAddr__
#define deviceCtrlCmdBifGetDmaBaseSysmemAddr(pDevice, pBifDmaBaseSysmemParams) deviceCtrlCmdBifGetDmaBaseSysmemAddr_DISPATCH(pDevice, pBifDmaBaseSysmemParams)
#define deviceCtrlCmdBifAspmFeatureSupported_FNPTR(pDevice) pDevice->__deviceCtrlCmdBifAspmFeatureSupported__
#define deviceCtrlCmdBifAspmFeatureSupported(pDevice, pBifAspmParams) deviceCtrlCmdBifAspmFeatureSupported_DISPATCH(pDevice, pBifAspmParams)
#define deviceCtrlCmdBifAspmCyaUpdate_FNPTR(pDevice) pDevice->__deviceCtrlCmdBifAspmCyaUpdate__
#define deviceCtrlCmdBifAspmCyaUpdate(pDevice, pBifAspmCyaParams) deviceCtrlCmdBifAspmCyaUpdate_DISPATCH(pDevice, pBifAspmCyaParams)
#define deviceCtrlCmdBifGetPciePowerControlMask_FNPTR(pDevice) pDevice->__deviceCtrlCmdBifGetPciePowerControlMask__
#define deviceCtrlCmdBifGetPciePowerControlMask(pDevice, pBifPciePowerControlParams) deviceCtrlCmdBifGetPciePowerControlMask_DISPATCH(pDevice, pBifPciePowerControlParams)
#define deviceCtrlCmdDmaGetPteInfo_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaGetPteInfo__
#define deviceCtrlCmdDmaGetPteInfo(pDevice, pParams) deviceCtrlCmdDmaGetPteInfo_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaUpdatePde2_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaUpdatePde2__
#define deviceCtrlCmdDmaUpdatePde2(pDevice, pParams) deviceCtrlCmdDmaUpdatePde2_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaSetPageDirectory_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaSetPageDirectory__
#define deviceCtrlCmdDmaSetPageDirectory(pDevice, pParams) deviceCtrlCmdDmaSetPageDirectory_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaUnsetPageDirectory_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaUnsetPageDirectory__
#define deviceCtrlCmdDmaUnsetPageDirectory(pDevice, pParams) deviceCtrlCmdDmaUnsetPageDirectory_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaFlush_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaFlush__
#define deviceCtrlCmdDmaFlush(pDevice, flushParams) deviceCtrlCmdDmaFlush_DISPATCH(pDevice, flushParams)
#define deviceCtrlCmdDmaFlush_HAL(pDevice, flushParams) deviceCtrlCmdDmaFlush_DISPATCH(pDevice, flushParams)
#define deviceCtrlCmdDmaAdvSchedGetVaCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaAdvSchedGetVaCaps__
#define deviceCtrlCmdDmaAdvSchedGetVaCaps(pDevice, pParams) deviceCtrlCmdDmaAdvSchedGetVaCaps_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaGetPdeInfo_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaGetPdeInfo__
#define deviceCtrlCmdDmaGetPdeInfo(pDevice, pParams) deviceCtrlCmdDmaGetPdeInfo_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaSetPteInfo_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaSetPteInfo__
#define deviceCtrlCmdDmaSetPteInfo(pDevice, pParams) deviceCtrlCmdDmaSetPteInfo_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaInvalidateTLB_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaInvalidateTLB__
#define deviceCtrlCmdDmaInvalidateTLB(pDevice, pParams) deviceCtrlCmdDmaInvalidateTLB_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaGetCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaGetCaps__
#define deviceCtrlCmdDmaGetCaps(pDevice, pDmaCapsParams) deviceCtrlCmdDmaGetCaps_DISPATCH(pDevice, pDmaCapsParams)
#define deviceCtrlCmdDmaSetVASpaceSize_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaSetVASpaceSize__
#define deviceCtrlCmdDmaSetVASpaceSize(pDevice, pParams) deviceCtrlCmdDmaSetVASpaceSize_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaEnablePrivilegedRange_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaEnablePrivilegedRange__
#define deviceCtrlCmdDmaEnablePrivilegedRange(pDevice, pParams) deviceCtrlCmdDmaEnablePrivilegedRange_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdDmaSetDefaultVASpace_FNPTR(pDevice) pDevice->__deviceCtrlCmdDmaSetDefaultVASpace__
#define deviceCtrlCmdDmaSetDefaultVASpace(pDevice, pParams) deviceCtrlCmdDmaSetDefaultVASpace_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdKGrGetCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdKGrGetCaps__
#define deviceCtrlCmdKGrGetCaps(pDevice, pParams) deviceCtrlCmdKGrGetCaps_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdKGrGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdKGrGetCapsV2__
#define deviceCtrlCmdKGrGetCapsV2(pDevice, pParams) deviceCtrlCmdKGrGetCapsV2_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdKGrGetInfo_FNPTR(pDevice) pDevice->__deviceCtrlCmdKGrGetInfo__
#define deviceCtrlCmdKGrGetInfo(pDevice, pParams) deviceCtrlCmdKGrGetInfo_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdKGrGetInfoV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdKGrGetInfoV2__
#define deviceCtrlCmdKGrGetInfoV2(pDevice, pParams) deviceCtrlCmdKGrGetInfoV2_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdKGrGetTpcPartitionMode_FNPTR(pDevice) pDevice->__deviceCtrlCmdKGrGetTpcPartitionMode__
#define deviceCtrlCmdKGrGetTpcPartitionMode(pDevice, pParams) deviceCtrlCmdKGrGetTpcPartitionMode_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdKGrSetTpcPartitionMode_FNPTR(pDevice) pDevice->__deviceCtrlCmdKGrSetTpcPartitionMode__
#define deviceCtrlCmdKGrSetTpcPartitionMode(pDevice, pParams) deviceCtrlCmdKGrSetTpcPartitionMode_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdFbGetCompbitStoreInfo_FNPTR(pDevice) pDevice->__deviceCtrlCmdFbGetCompbitStoreInfo__
#define deviceCtrlCmdFbGetCompbitStoreInfo(pDevice, pCompbitStoreParams) deviceCtrlCmdFbGetCompbitStoreInfo_DISPATCH(pDevice, pCompbitStoreParams)
#define deviceCtrlCmdFbGetCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdFbGetCaps__
#define deviceCtrlCmdFbGetCaps(pDevice, pFbCapsParams) deviceCtrlCmdFbGetCaps_DISPATCH(pDevice, pFbCapsParams)
#define deviceCtrlCmdFbGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdFbGetCapsV2__
#define deviceCtrlCmdFbGetCapsV2(pDevice, pFbCapsParams) deviceCtrlCmdFbGetCapsV2_DISPATCH(pDevice, pFbCapsParams)
#define deviceCtrlCmdSetDefaultVidmemPhysicality_FNPTR(pDevice) pDevice->__deviceCtrlCmdSetDefaultVidmemPhysicality__
#define deviceCtrlCmdSetDefaultVidmemPhysicality(pDevice, pParams) deviceCtrlCmdSetDefaultVidmemPhysicality_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdFifoGetCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoGetCaps__
#define deviceCtrlCmdFifoGetCaps(pDevice, pFifoCapsParams) deviceCtrlCmdFifoGetCaps_DISPATCH(pDevice, pFifoCapsParams)
#define deviceCtrlCmdFifoGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoGetCapsV2__
#define deviceCtrlCmdFifoGetCapsV2(pDevice, pFifoCapsParams) deviceCtrlCmdFifoGetCapsV2_DISPATCH(pDevice, pFifoCapsParams)
#define deviceCtrlCmdFifoStartSelectedChannels_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoStartSelectedChannels__
#define deviceCtrlCmdFifoStartSelectedChannels(pDevice, pStartSel) deviceCtrlCmdFifoStartSelectedChannels_DISPATCH(pDevice, pStartSel)
#define deviceCtrlCmdFifoGetEngineContextProperties_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoGetEngineContextProperties__
#define deviceCtrlCmdFifoGetEngineContextProperties(pDevice, pParams) deviceCtrlCmdFifoGetEngineContextProperties_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdFifoGetEngineContextProperties_HAL(pDevice, pParams) deviceCtrlCmdFifoGetEngineContextProperties_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdFifoStopRunlist_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoStopRunlist__
#define deviceCtrlCmdFifoStopRunlist(pDevice, pStopRunlistParams) deviceCtrlCmdFifoStopRunlist_DISPATCH(pDevice, pStopRunlistParams)
#define deviceCtrlCmdFifoStartRunlist_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoStartRunlist__
#define deviceCtrlCmdFifoStartRunlist(pDevice, pStartRunlistParams) deviceCtrlCmdFifoStartRunlist_DISPATCH(pDevice, pStartRunlistParams)
#define deviceCtrlCmdFifoGetChannelList_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoGetChannelList__
#define deviceCtrlCmdFifoGetChannelList(pDevice, pChannelParams) deviceCtrlCmdFifoGetChannelList_DISPATCH(pDevice, pChannelParams)
#define deviceCtrlCmdFifoGetLatencyBufferSize_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoGetLatencyBufferSize__
#define deviceCtrlCmdFifoGetLatencyBufferSize(pDevice, pGetLatencyBufferSizeParams) deviceCtrlCmdFifoGetLatencyBufferSize_DISPATCH(pDevice, pGetLatencyBufferSizeParams)
#define deviceCtrlCmdFifoGetLatencyBufferSize_HAL(pDevice, pGetLatencyBufferSizeParams) deviceCtrlCmdFifoGetLatencyBufferSize_DISPATCH(pDevice, pGetLatencyBufferSizeParams)
#define deviceCtrlCmdFifoSetChannelProperties_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoSetChannelProperties__
#define deviceCtrlCmdFifoSetChannelProperties(pDevice, pSetChannelPropertiesParams) deviceCtrlCmdFifoSetChannelProperties_DISPATCH(pDevice, pSetChannelPropertiesParams)
#define deviceCtrlCmdFifoIdleChannels_FNPTR(pDevice) pDevice->__deviceCtrlCmdFifoIdleChannels__
#define deviceCtrlCmdFifoIdleChannels(pDevice, pParams) deviceCtrlCmdFifoIdleChannels_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdFifoIdleChannels_HAL(pDevice, pParams) deviceCtrlCmdFifoIdleChannels_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdHostGetCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdHostGetCaps__
#define deviceCtrlCmdHostGetCaps(pDevice, pHostCapsParams) deviceCtrlCmdHostGetCaps_DISPATCH(pDevice, pHostCapsParams)
#define deviceCtrlCmdHostGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdHostGetCapsV2__
#define deviceCtrlCmdHostGetCapsV2(pDevice, pHostCapsParamsV2) deviceCtrlCmdHostGetCapsV2_DISPATCH(pDevice, pHostCapsParamsV2)
#define deviceCtrlCmdKPerfCudaLimitSetControl_FNPTR(pDevice) pDevice->__deviceCtrlCmdKPerfCudaLimitSetControl__
#define deviceCtrlCmdKPerfCudaLimitSetControl(pDevice, pParams) deviceCtrlCmdKPerfCudaLimitSetControl_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdInternalPerfCudaLimitSetControl_FNPTR(pDevice) pDevice->__deviceCtrlCmdInternalPerfCudaLimitSetControl__
#define deviceCtrlCmdInternalPerfCudaLimitSetControl(pDevice, pParams) deviceCtrlCmdInternalPerfCudaLimitSetControl_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdInternalPerfCudaLimitDisable_FNPTR(pDevice) pDevice->__deviceCtrlCmdInternalPerfCudaLimitDisable__
#define deviceCtrlCmdInternalPerfCudaLimitDisable(pDevice) deviceCtrlCmdInternalPerfCudaLimitDisable_DISPATCH(pDevice)
#define deviceCtrlCmdGpuGetClasslist_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetClasslist__
#define deviceCtrlCmdGpuGetClasslist(pDevice, pClassListParams) deviceCtrlCmdGpuGetClasslist_DISPATCH(pDevice, pClassListParams)
#define deviceCtrlCmdGpuGetClasslistV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetClasslistV2__
#define deviceCtrlCmdGpuGetClasslistV2(pDevice, pParams) deviceCtrlCmdGpuGetClasslistV2_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuGetNumSubdevices_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetNumSubdevices__
#define deviceCtrlCmdGpuGetNumSubdevices(pDevice, pSubDeviceCountParams) deviceCtrlCmdGpuGetNumSubdevices_DISPATCH(pDevice, pSubDeviceCountParams)
#define deviceCtrlCmdGpuModifyGpuSwStatePersistence_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuModifyGpuSwStatePersistence__
#define deviceCtrlCmdGpuModifyGpuSwStatePersistence(pDevice, pParams) deviceCtrlCmdGpuModifyGpuSwStatePersistence_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuQueryGpuSwStatePersistence_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuQueryGpuSwStatePersistence__
#define deviceCtrlCmdGpuQueryGpuSwStatePersistence(pDevice, pParams) deviceCtrlCmdGpuQueryGpuSwStatePersistence_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuGetVirtualizationMode_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetVirtualizationMode__
#define deviceCtrlCmdGpuGetVirtualizationMode(pDevice, pParams) deviceCtrlCmdGpuGetVirtualizationMode_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuSetVgpuVfBar1Size_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuSetVgpuVfBar1Size__
#define deviceCtrlCmdGpuSetVgpuVfBar1Size(pDevice, pParams) deviceCtrlCmdGpuSetVgpuVfBar1Size_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuGetSparseTextureComputeMode_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetSparseTextureComputeMode__
#define deviceCtrlCmdGpuGetSparseTextureComputeMode(pDevice, pModeParams) deviceCtrlCmdGpuGetSparseTextureComputeMode_DISPATCH(pDevice, pModeParams)
#define deviceCtrlCmdGpuSetSparseTextureComputeMode_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuSetSparseTextureComputeMode__
#define deviceCtrlCmdGpuSetSparseTextureComputeMode(pDevice, pModeParams) deviceCtrlCmdGpuSetSparseTextureComputeMode_DISPATCH(pDevice, pModeParams)
#define deviceCtrlCmdGpuGetVgxCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetVgxCaps__
#define deviceCtrlCmdGpuGetVgxCaps(pDevice, pParams) deviceCtrlCmdGpuGetVgxCaps_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuGetBrandCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetBrandCaps__
#define deviceCtrlCmdGpuGetBrandCaps(pDevice, pParams) deviceCtrlCmdGpuGetBrandCaps_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuGetBrandCaps_HAL(pDevice, pParams) deviceCtrlCmdGpuGetBrandCaps_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuVirtualizationSwitchToVga_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuVirtualizationSwitchToVga__
#define deviceCtrlCmdGpuVirtualizationSwitchToVga(pDevice) deviceCtrlCmdGpuVirtualizationSwitchToVga_DISPATCH(pDevice)
#define deviceCtrlCmdGpuSetVgpuHeterogeneousMode_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuSetVgpuHeterogeneousMode__
#define deviceCtrlCmdGpuSetVgpuHeterogeneousMode(pDevice, pParams) deviceCtrlCmdGpuSetVgpuHeterogeneousMode_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuGetVgpuHeterogeneousMode_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetVgpuHeterogeneousMode__
#define deviceCtrlCmdGpuGetVgpuHeterogeneousMode(pDevice, pParams) deviceCtrlCmdGpuGetVgpuHeterogeneousMode_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuGetSriovCaps_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetSriovCaps__
#define deviceCtrlCmdGpuGetSriovCaps(pDevice, pParams) deviceCtrlCmdGpuGetSriovCaps_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdGpuGetFindSubDeviceHandle_FNPTR(pDevice) pDevice->__deviceCtrlCmdGpuGetFindSubDeviceHandle__
#define deviceCtrlCmdGpuGetFindSubDeviceHandle(pDevice, pParams) deviceCtrlCmdGpuGetFindSubDeviceHandle_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdMsencGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdMsencGetCapsV2__
#define deviceCtrlCmdMsencGetCapsV2(pDevice, pMsencCapsParams) deviceCtrlCmdMsencGetCapsV2_DISPATCH(pDevice, pMsencCapsParams)
#define deviceCtrlCmdMsencGetCapsV2_HAL(pDevice, pMsencCapsParams) deviceCtrlCmdMsencGetCapsV2_DISPATCH(pDevice, pMsencCapsParams)
#define deviceCtrlCmdBspGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdBspGetCapsV2__
#define deviceCtrlCmdBspGetCapsV2(pDevice, pBspCapParams) deviceCtrlCmdBspGetCapsV2_DISPATCH(pDevice, pBspCapParams)
#define deviceCtrlCmdBspGetCapsV2_HAL(pDevice, pBspCapParams) deviceCtrlCmdBspGetCapsV2_DISPATCH(pDevice, pBspCapParams)
#define deviceCtrlCmdNvjpgGetCapsV2_FNPTR(pDevice) pDevice->__deviceCtrlCmdNvjpgGetCapsV2__
#define deviceCtrlCmdNvjpgGetCapsV2(pDevice, pNvjpgCapsParams) deviceCtrlCmdNvjpgGetCapsV2_DISPATCH(pDevice, pNvjpgCapsParams)
#define deviceCtrlCmdNvjpgGetCapsV2_HAL(pDevice, pNvjpgCapsParams) deviceCtrlCmdNvjpgGetCapsV2_DISPATCH(pDevice, pNvjpgCapsParams)
#define deviceCtrlCmdOsUnixVTSwitch_FNPTR(pDevice) pDevice->__deviceCtrlCmdOsUnixVTSwitch__
#define deviceCtrlCmdOsUnixVTSwitch(pDevice, pParams) deviceCtrlCmdOsUnixVTSwitch_DISPATCH(pDevice, pParams)
#define deviceCtrlCmdOsUnixVTGetFBInfo_FNPTR(pDevice) pDevice->__deviceCtrlCmdOsUnixVTGetFBInfo__
#define deviceCtrlCmdOsUnixVTGetFBInfo(pDevice, pParams) deviceCtrlCmdOsUnixVTGetFBInfo_DISPATCH(pDevice, pParams)
#define deviceMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresMap__
#define deviceMap(pGpuResource, pCallContext, pParams, pCpuMapping) deviceMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define deviceUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresUnmap__
#define deviceUnmap(pGpuResource, pCallContext, pCpuMapping) deviceUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define deviceShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresShareCallback__
#define deviceShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) deviceShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define deviceGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define deviceGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) deviceGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define deviceGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define deviceGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) deviceGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define deviceGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define deviceGetInternalObjectHandle(pGpuResource) deviceGetInternalObjectHandle_DISPATCH(pGpuResource)
#define deviceAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define deviceAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) deviceAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define deviceGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define deviceGetMemInterMapParams(pRmResource, pParams) deviceGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define deviceCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresCheckMemInterUnmap__
#define deviceCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) deviceCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define deviceGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define deviceGetMemoryMappingDescriptor(pRmResource, ppMemDesc) deviceGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define deviceControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define deviceControlSerialization_Prologue(pResource, pCallContext, pParams) deviceControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define deviceControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define deviceControlSerialization_Epilogue(pResource, pCallContext, pParams) deviceControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define deviceControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define deviceControl_Prologue(pResource, pCallContext, pParams) deviceControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define deviceControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define deviceControl_Epilogue(pResource, pCallContext, pParams) deviceControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define deviceCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define deviceCanCopy(pResource) deviceCanCopy_DISPATCH(pResource)
#define deviceIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define deviceIsDuplicate(pResource, hMemory, pDuplicate) deviceIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define devicePreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define devicePreDestruct(pResource) devicePreDestruct_DISPATCH(pResource)
#define deviceControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define deviceControlFilter(pResource, pCallContext, pParams) deviceControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define deviceIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define deviceIsPartialUnmapSupported(pResource) deviceIsPartialUnmapSupported_DISPATCH(pResource)
#define deviceMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define deviceMapTo(pResource, pParams) deviceMapTo_DISPATCH(pResource, pParams)
#define deviceUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define deviceUnmapFrom(pResource, pParams) deviceUnmapFrom_DISPATCH(pResource, pParams)
#define deviceGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define deviceGetRefCount(pResource) deviceGetRefCount_DISPATCH(pResource)
#define deviceAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define deviceAddAdditionalDependants(pClient, pResource, pReference) deviceAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS deviceControl_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__deviceControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS deviceInternalControlForward_DISPATCH(struct Device *pDevice, NvU32 command, void *pParams, NvU32 size) {
    return pDevice->__deviceInternalControlForward__(pDevice, command, pParams, size);
}

static inline NV_STATUS deviceCtrlCmdBifGetDmaBaseSysmemAddr_DISPATCH(struct Device *pDevice, NV0080_CTRL_BIF_GET_DMA_BASE_SYSMEM_ADDR_PARAMS *pBifDmaBaseSysmemParams) {
    return pDevice->__deviceCtrlCmdBifGetDmaBaseSysmemAddr__(pDevice, pBifDmaBaseSysmemParams);
}

static inline NV_STATUS deviceCtrlCmdBifAspmFeatureSupported_DISPATCH(struct Device *pDevice, NV0080_CTRL_BIF_SET_ASPM_FEATURE_PARAMS *pBifAspmParams) {
    return pDevice->__deviceCtrlCmdBifAspmFeatureSupported__(pDevice, pBifAspmParams);
}

static inline NV_STATUS deviceCtrlCmdBifAspmCyaUpdate_DISPATCH(struct Device *pDevice, NV0080_CTRL_BIF_ASPM_CYA_UPDATE_PARAMS *pBifAspmCyaParams) {
    return pDevice->__deviceCtrlCmdBifAspmCyaUpdate__(pDevice, pBifAspmCyaParams);
}

static inline NV_STATUS deviceCtrlCmdBifGetPciePowerControlMask_DISPATCH(struct Device *pDevice, NV0080_CTRL_CMD_BIF_GET_PCIE_POWER_CONTROL_MASK_PARAMS *pBifPciePowerControlParams) {
    return pDevice->__deviceCtrlCmdBifGetPciePowerControlMask__(pDevice, pBifPciePowerControlParams);
}

static inline NV_STATUS deviceCtrlCmdDmaGetPteInfo_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaGetPteInfo__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaUpdatePde2_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaUpdatePde2__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaSetPageDirectory_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaSetPageDirectory__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaUnsetPageDirectory_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaUnsetPageDirectory__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaFlush_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_FLUSH_PARAMS *flushParams) {
    return pDevice->__deviceCtrlCmdDmaFlush__(pDevice, flushParams);
}

static inline NV_STATUS deviceCtrlCmdDmaAdvSchedGetVaCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaAdvSchedGetVaCaps__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaGetPdeInfo_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaGetPdeInfo__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaSetPteInfo_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaSetPteInfo__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaInvalidateTLB_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_INVALIDATE_TLB_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaInvalidateTLB__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaGetCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_GET_CAPS_PARAMS *pDmaCapsParams) {
    return pDevice->__deviceCtrlCmdDmaGetCaps__(pDevice, pDmaCapsParams);
}

static inline NV_STATUS deviceCtrlCmdDmaSetVASpaceSize_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaSetVASpaceSize__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaEnablePrivilegedRange_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_ENABLE_PRIVILEGED_RANGE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaEnablePrivilegedRange__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdDmaSetDefaultVASpace_DISPATCH(struct Device *pDevice, NV0080_CTRL_DMA_SET_DEFAULT_VASPACE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdDmaSetDefaultVASpace__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdKGrGetCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_GR_GET_CAPS_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdKGrGetCaps__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdKGrGetCapsV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_GR_GET_CAPS_V2_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdKGrGetCapsV2__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdKGrGetInfo_DISPATCH(struct Device *pDevice, NV0080_CTRL_GR_GET_INFO_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdKGrGetInfo__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdKGrGetInfoV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_GR_GET_INFO_V2_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdKGrGetInfoV2__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdKGrGetTpcPartitionMode_DISPATCH(struct Device *pDevice, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdKGrGetTpcPartitionMode__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdKGrSetTpcPartitionMode_DISPATCH(struct Device *pDevice, NV0080_CTRL_GR_TPC_PARTITION_MODE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdKGrSetTpcPartitionMode__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdFbGetCompbitStoreInfo_DISPATCH(struct Device *pDevice, NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS *pCompbitStoreParams) {
    return pDevice->__deviceCtrlCmdFbGetCompbitStoreInfo__(pDevice, pCompbitStoreParams);
}

static inline NV_STATUS deviceCtrlCmdFbGetCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_PARAMS *pFbCapsParams) {
    return pDevice->__deviceCtrlCmdFbGetCaps__(pDevice, pFbCapsParams);
}

static inline NV_STATUS deviceCtrlCmdFbGetCapsV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_V2_PARAMS *pFbCapsParams) {
    return pDevice->__deviceCtrlCmdFbGetCapsV2__(pDevice, pFbCapsParams);
}

static inline NV_STATUS deviceCtrlCmdSetDefaultVidmemPhysicality_DISPATCH(struct Device *pDevice, NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdSetDefaultVidmemPhysicality__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdFifoGetCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *pFifoCapsParams) {
    return pDevice->__deviceCtrlCmdFifoGetCaps__(pDevice, pFifoCapsParams);
}

static inline NV_STATUS deviceCtrlCmdFifoGetCapsV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS *pFifoCapsParams) {
    return pDevice->__deviceCtrlCmdFifoGetCapsV2__(pDevice, pFifoCapsParams);
}

static inline NV_STATUS deviceCtrlCmdFifoStartSelectedChannels_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *pStartSel) {
    return pDevice->__deviceCtrlCmdFifoStartSelectedChannels__(pDevice, pStartSel);
}

static inline NV_STATUS deviceCtrlCmdFifoGetEngineContextProperties_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_GET_ENGINE_CONTEXT_PROPERTIES_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdFifoGetEngineContextProperties__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdFifoStopRunlist_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_STOP_RUNLIST_PARAMS *pStopRunlistParams) {
    return pDevice->__deviceCtrlCmdFifoStopRunlist__(pDevice, pStopRunlistParams);
}

static inline NV_STATUS deviceCtrlCmdFifoStartRunlist_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_START_RUNLIST_PARAMS *pStartRunlistParams) {
    return pDevice->__deviceCtrlCmdFifoStartRunlist__(pDevice, pStartRunlistParams);
}

static inline NV_STATUS deviceCtrlCmdFifoGetChannelList_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CHANNELLIST_PARAMS *pChannelParams) {
    return pDevice->__deviceCtrlCmdFifoGetChannelList__(pDevice, pChannelParams);
}

static inline NV_STATUS deviceCtrlCmdFifoGetLatencyBufferSize_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_GET_LATENCY_BUFFER_SIZE_PARAMS *pGetLatencyBufferSizeParams) {
    return pDevice->__deviceCtrlCmdFifoGetLatencyBufferSize__(pDevice, pGetLatencyBufferSizeParams);
}

static inline NV_STATUS deviceCtrlCmdFifoSetChannelProperties_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_SET_CHANNEL_PROPERTIES_PARAMS *pSetChannelPropertiesParams) {
    return pDevice->__deviceCtrlCmdFifoSetChannelProperties__(pDevice, pSetChannelPropertiesParams);
}

static inline NV_STATUS deviceCtrlCmdFifoIdleChannels_DISPATCH(struct Device *pDevice, NV0080_CTRL_FIFO_IDLE_CHANNELS_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdFifoIdleChannels__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdHostGetCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_HOST_GET_CAPS_PARAMS *pHostCapsParams) {
    return pDevice->__deviceCtrlCmdHostGetCaps__(pDevice, pHostCapsParams);
}

static inline NV_STATUS deviceCtrlCmdHostGetCapsV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_HOST_GET_CAPS_V2_PARAMS *pHostCapsParamsV2) {
    return pDevice->__deviceCtrlCmdHostGetCapsV2__(pDevice, pHostCapsParamsV2);
}

static inline NV_STATUS deviceCtrlCmdKPerfCudaLimitSetControl_DISPATCH(struct Device *pDevice, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdKPerfCudaLimitSetControl__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdInternalPerfCudaLimitSetControl_DISPATCH(struct Device *pDevice, NV0080_CTRL_PERF_CUDA_LIMIT_CONTROL_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdInternalPerfCudaLimitSetControl__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdInternalPerfCudaLimitDisable_DISPATCH(struct Device *pDevice) {
    return pDevice->__deviceCtrlCmdInternalPerfCudaLimitDisable__(pDevice);
}

static inline NV_STATUS deviceCtrlCmdGpuGetClasslist_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_CLASSLIST_PARAMS *pClassListParams) {
    return pDevice->__deviceCtrlCmdGpuGetClasslist__(pDevice, pClassListParams);
}

static inline NV_STATUS deviceCtrlCmdGpuGetClasslistV2_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_CLASSLIST_V2_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuGetClasslistV2__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuGetNumSubdevices_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_NUM_SUBDEVICES_PARAMS *pSubDeviceCountParams) {
    return pDevice->__deviceCtrlCmdGpuGetNumSubdevices__(pDevice, pSubDeviceCountParams);
}

static inline NV_STATUS deviceCtrlCmdGpuModifyGpuSwStatePersistence_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_MODIFY_SW_STATE_PERSISTENCE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuModifyGpuSwStatePersistence__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuQueryGpuSwStatePersistence_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_QUERY_SW_STATE_PERSISTENCE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuQueryGpuSwStatePersistence__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuGetVirtualizationMode_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_VIRTUALIZATION_MODE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuGetVirtualizationMode__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuSetVgpuVfBar1Size_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_SET_VGPU_VF_BAR1_SIZE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuSetVgpuVfBar1Size__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuGetSparseTextureComputeMode_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams) {
    return pDevice->__deviceCtrlCmdGpuGetSparseTextureComputeMode__(pDevice, pModeParams);
}

static inline NV_STATUS deviceCtrlCmdGpuSetSparseTextureComputeMode_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_SET_SPARSE_TEXTURE_COMPUTE_MODE_PARAMS *pModeParams) {
    return pDevice->__deviceCtrlCmdGpuSetSparseTextureComputeMode__(pDevice, pModeParams);
}

static inline NV_STATUS deviceCtrlCmdGpuGetVgxCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_VGX_CAPS_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuGetVgxCaps__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuGetBrandCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_BRAND_CAPS_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuGetBrandCaps__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuVirtualizationSwitchToVga_DISPATCH(struct Device *pDevice) {
    return pDevice->__deviceCtrlCmdGpuVirtualizationSwitchToVga__(pDevice);
}

static inline NV_STATUS deviceCtrlCmdGpuSetVgpuHeterogeneousMode_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_SET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuSetVgpuHeterogeneousMode__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuGetVgpuHeterogeneousMode_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_VGPU_HETEROGENEOUS_MODE_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuGetVgpuHeterogeneousMode__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuGetSriovCaps_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_GET_SRIOV_CAPS_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdGpuGetSriovCaps__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdGpuGetFindSubDeviceHandle_DISPATCH(struct Device *pDevice, NV0080_CTRL_GPU_FIND_SUBDEVICE_HANDLE_PARAM *pParams) {
    return pDevice->__deviceCtrlCmdGpuGetFindSubDeviceHandle__(pDevice, pParams);
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

static inline NV_STATUS deviceCtrlCmdOsUnixVTSwitch_DISPATCH(struct Device *pDevice, NV0080_CTRL_OS_UNIX_VT_SWITCH_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdOsUnixVTSwitch__(pDevice, pParams);
}

static inline NV_STATUS deviceCtrlCmdOsUnixVTGetFBInfo_DISPATCH(struct Device *pDevice, NV0080_CTRL_OS_UNIX_VT_GET_FB_INFO_PARAMS *pParams) {
    return pDevice->__deviceCtrlCmdOsUnixVTGetFBInfo__(pDevice, pParams);
}

static inline NV_STATUS deviceMap_DISPATCH(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__deviceMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS deviceUnmap_DISPATCH(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__deviceUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool deviceShareCallback_DISPATCH(struct Device *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__deviceShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS deviceGetRegBaseOffsetAndSize_DISPATCH(struct Device *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__deviceGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS deviceGetMapAddrSpace_DISPATCH(struct Device *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__deviceGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvHandle deviceGetInternalObjectHandle_DISPATCH(struct Device *pGpuResource) {
    return pGpuResource->__deviceGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool deviceAccessCallback_DISPATCH(struct Device *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__deviceAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS deviceGetMemInterMapParams_DISPATCH(struct Device *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__deviceGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS deviceCheckMemInterUnmap_DISPATCH(struct Device *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__deviceCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS deviceGetMemoryMappingDescriptor_DISPATCH(struct Device *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__deviceGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS deviceControlSerialization_Prologue_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__deviceControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void deviceControlSerialization_Epilogue_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__deviceControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS deviceControl_Prologue_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__deviceControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void deviceControl_Epilogue_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__deviceControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool deviceCanCopy_DISPATCH(struct Device *pResource) {
    return pResource->__deviceCanCopy__(pResource);
}

static inline NV_STATUS deviceIsDuplicate_DISPATCH(struct Device *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__deviceIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void devicePreDestruct_DISPATCH(struct Device *pResource) {
    pResource->__devicePreDestruct__(pResource);
}

static inline NV_STATUS deviceControlFilter_DISPATCH(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__deviceControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool deviceIsPartialUnmapSupported_DISPATCH(struct Device *pResource) {
    return pResource->__deviceIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS deviceMapTo_DISPATCH(struct Device *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__deviceMapTo__(pResource, pParams);
}

static inline NV_STATUS deviceUnmapFrom_DISPATCH(struct Device *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__deviceUnmapFrom__(pResource, pParams);
}

static inline NvU32 deviceGetRefCount_DISPATCH(struct Device *pResource) {
    return pResource->__deviceGetRefCount__(pResource);
}

static inline void deviceAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct Device *pResource, RsResourceRef *pReference) {
    pResource->__deviceAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS deviceControl_IMPL(struct Device *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS deviceInternalControlForward_IMPL(struct Device *pDevice, NvU32 command, void *pParams, NvU32 size);

NV_STATUS deviceCtrlCmdBifGetDmaBaseSysmemAddr_IMPL(struct Device *pDevice, NV0080_CTRL_BIF_GET_DMA_BASE_SYSMEM_ADDR_PARAMS *pBifDmaBaseSysmemParams);

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

NV_STATUS deviceCtrlCmdFbGetCompbitStoreInfo_IMPL(struct Device *pDevice, NV0080_CTRL_FB_GET_COMPBIT_STORE_INFO_PARAMS *pCompbitStoreParams);

NV_STATUS deviceCtrlCmdFbGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_PARAMS *pFbCapsParams);

NV_STATUS deviceCtrlCmdFbGetCapsV2_IMPL(struct Device *pDevice, NV0080_CTRL_FB_GET_CAPS_V2_PARAMS *pFbCapsParams);

NV_STATUS deviceCtrlCmdSetDefaultVidmemPhysicality_IMPL(struct Device *pDevice, NV0080_CTRL_FB_SET_DEFAULT_VIDMEM_PHYSICALITY_PARAMS *pParams);

NV_STATUS deviceCtrlCmdFifoGetCaps_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_PARAMS *pFifoCapsParams);

NV_STATUS deviceCtrlCmdFifoGetCapsV2_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_GET_CAPS_V2_PARAMS *pFifoCapsParams);

NV_STATUS deviceCtrlCmdFifoStartSelectedChannels_IMPL(struct Device *pDevice, NV0080_CTRL_FIFO_START_SELECTED_CHANNELS_PARAMS *pStartSel);

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
#else //__nvoc_device_h_disabled
#define deviceGetDefaultVASpace(pDevice, ppVAS) deviceGetDefaultVASpace_IMPL(pDevice, ppVAS)
#endif //__nvoc_device_h_disabled

NV_STATUS deviceSetClientShare_IMPL(struct Device *pDevice, NvHandle hClientShare, NvU64 vaSize, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 deviceAllocFlags);

#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceSetClientShare(struct Device *pDevice, NvHandle hClientShare, NvU64 vaSize, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 deviceAllocFlags) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_device_h_disabled
#define deviceSetClientShare(pDevice, hClientShare, vaSize, vaStartInternal, vaLimitInternal, deviceAllocFlags) deviceSetClientShare_IMPL(pDevice, hClientShare, vaSize, vaStartInternal, vaLimitInternal, deviceAllocFlags)
#endif //__nvoc_device_h_disabled

void deviceRemoveFromClientShare_IMPL(struct Device *pDevice);

#ifdef __nvoc_device_h_disabled
static inline void deviceRemoveFromClientShare(struct Device *pDevice) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
}
#else //__nvoc_device_h_disabled
#define deviceRemoveFromClientShare(pDevice) deviceRemoveFromClientShare_IMPL(pDevice)
#endif //__nvoc_device_h_disabled

NV_STATUS deviceSetDefaultVASpace_IMPL(struct Device *pDevice, NvHandle hVASpace);

#ifdef __nvoc_device_h_disabled
static inline NV_STATUS deviceSetDefaultVASpace(struct Device *pDevice, NvHandle hVASpace) {
    NV_ASSERT_FAILED_PRECOMP("Device was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_device_h_disabled
#define deviceSetDefaultVASpace(pDevice, hVASpace) deviceSetDefaultVASpace_IMPL(pDevice, hVASpace)
#endif //__nvoc_device_h_disabled

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
