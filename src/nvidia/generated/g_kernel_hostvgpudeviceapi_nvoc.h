
#ifndef _G_KERNEL_HOSTVGPUDEVICEAPI_NVOC_H_
#define _G_KERNEL_HOSTVGPUDEVICEAPI_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_hostvgpudeviceapi_nvoc.h"

#ifndef _KERNEL_HOSTVGPUDEVICEAPI_H_
#define _KERNEL_HOSTVGPUDEVICEAPI_H_

#include "core/core.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "rmapi/client.h"
#include "gpu/gpu_resource.h"

#include "class/cla084.h"

#include "ctrl/ctrla084.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_HOSTVGPUDEVICEAPI_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelHostVgpuDeviceShr;
struct NVOC_METADATA__RsShared;


struct KernelHostVgpuDeviceShr {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelHostVgpuDeviceShr *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RsShared __nvoc_base_RsShared;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsShared *__nvoc_pbase_RsShared;    // shr super
    struct KernelHostVgpuDeviceShr *__nvoc_pbase_KernelHostVgpuDeviceShr;    // kernelhostvgpudeviceshr

    // Data members
    KERNEL_HOST_VGPU_DEVICE *pDevice;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelHostVgpuDeviceShr {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RsShared metadata__RsShared;
};

#ifndef __NVOC_CLASS_KernelHostVgpuDeviceShr_TYPEDEF__
#define __NVOC_CLASS_KernelHostVgpuDeviceShr_TYPEDEF__
typedef struct KernelHostVgpuDeviceShr KernelHostVgpuDeviceShr;
#endif /* __NVOC_CLASS_KernelHostVgpuDeviceShr_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHostVgpuDeviceShr
#define __nvoc_class_id_KernelHostVgpuDeviceShr 0xe32156
#endif /* __nvoc_class_id_KernelHostVgpuDeviceShr */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceShr;

#define __staticCast_KernelHostVgpuDeviceShr(pThis) \
    ((pThis)->__nvoc_pbase_KernelHostVgpuDeviceShr)

#ifdef __nvoc_kernel_hostvgpudeviceapi_h_disabled
#define __dynamicCast_KernelHostVgpuDeviceShr(pThis) ((KernelHostVgpuDeviceShr*) NULL)
#else //__nvoc_kernel_hostvgpudeviceapi_h_disabled
#define __dynamicCast_KernelHostVgpuDeviceShr(pThis) \
    ((KernelHostVgpuDeviceShr*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelHostVgpuDeviceShr)))
#endif //__nvoc_kernel_hostvgpudeviceapi_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr**, Dynamic*, NvU32);
#define __objCreate_KernelHostVgpuDeviceShr(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelHostVgpuDeviceShr((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS kernelhostvgpudeviceshrConstruct_IMPL(struct KernelHostVgpuDeviceShr *arg_pKernelHostVgpuDeviceShr);

#define __nvoc_kernelhostvgpudeviceshrConstruct(arg_pKernelHostVgpuDeviceShr) kernelhostvgpudeviceshrConstruct_IMPL(arg_pKernelHostVgpuDeviceShr)
void kernelhostvgpudeviceshrDestruct_IMPL(struct KernelHostVgpuDeviceShr *pKernelHostVgpuDeviceShr);

#define __nvoc_kernelhostvgpudeviceshrDestruct(pKernelHostVgpuDeviceShr) kernelhostvgpudeviceshrDestruct_IMPL(pKernelHostVgpuDeviceShr)
#undef PRIVATE_FIELD


void destroyKernelHostVgpuDeviceShare(OBJGPU *pGpu, struct KernelHostVgpuDeviceShr* pShare);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_HOSTVGPUDEVICEAPI_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelHostVgpuDeviceApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;
struct NVOC_VTABLE__KernelHostVgpuDeviceApi;


struct KernelHostVgpuDeviceApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelHostVgpuDeviceApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^2
    struct Notifier *__nvoc_pbase_Notifier;    // notify super
    struct KernelHostVgpuDeviceApi *__nvoc_pbase_KernelHostVgpuDeviceApi;    // kernelhostvgpudeviceapi

    // Vtable with 12 per-object function pointers
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS *);  // exported (id=0xa0840101)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS *);  // exported (id=0xa0840102)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS *);  // exported (id=0xa0840103)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS *);  // exported (id=0xa0840104)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS *);  // exported (id=0xa0840105)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS *);  // exported (id=0xa0840106)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdEventSetNotification__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS *);  // exported (id=0xa0840107)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS *);  // exported (id=0xa084010a)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition__)(struct KernelHostVgpuDeviceApi * /*this*/);  // exported (id=0xa084010b)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdFreeStates__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_FREE_STATES_PARAMS *);  // exported (id=0xa084010c)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BOOTLOAD_VGPU_TASK_PARAMS *);  // exported (id=0xa084010d)
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdSetPlacementId__)(struct KernelHostVgpuDeviceApi * /*this*/, NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_PLACEMENT_ID_PARAMS *);  // exported (id=0xa084010e)

    // Data members
    struct KernelHostVgpuDeviceShr *pShared;
    NvU32 notifyActions[8];
};


// Vtable with 30 per-class function pointers
struct NVOC_VTABLE__KernelHostVgpuDeviceApi {
    NvBool (*__kernelhostvgpudeviceapiCanCopy__)(struct KernelHostVgpuDeviceApi * /*this*/);  // virtual override (res) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiControl__)(struct KernelHostVgpuDeviceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiMap__)(struct KernelHostVgpuDeviceApi * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiUnmap__)(struct KernelHostVgpuDeviceApi * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kernelhostvgpudeviceapiShareCallback__)(struct KernelHostVgpuDeviceApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiGetRegBaseOffsetAndSize__)(struct KernelHostVgpuDeviceApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiGetMapAddrSpace__)(struct KernelHostVgpuDeviceApi * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiInternalControlForward__)(struct KernelHostVgpuDeviceApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__kernelhostvgpudeviceapiGetInternalObjectHandle__)(struct KernelHostVgpuDeviceApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__kernelhostvgpudeviceapiAccessCallback__)(struct KernelHostVgpuDeviceApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiGetMemInterMapParams__)(struct KernelHostVgpuDeviceApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiCheckMemInterUnmap__)(struct KernelHostVgpuDeviceApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiGetMemoryMappingDescriptor__)(struct KernelHostVgpuDeviceApi * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiControlSerialization_Prologue__)(struct KernelHostVgpuDeviceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kernelhostvgpudeviceapiControlSerialization_Epilogue__)(struct KernelHostVgpuDeviceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiControl_Prologue__)(struct KernelHostVgpuDeviceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__kernelhostvgpudeviceapiControl_Epilogue__)(struct KernelHostVgpuDeviceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiIsDuplicate__)(struct KernelHostVgpuDeviceApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__kernelhostvgpudeviceapiPreDestruct__)(struct KernelHostVgpuDeviceApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiControlFilter__)(struct KernelHostVgpuDeviceApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__kernelhostvgpudeviceapiIsPartialUnmapSupported__)(struct KernelHostVgpuDeviceApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__kernelhostvgpudeviceapiMapTo__)(struct KernelHostVgpuDeviceApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__kernelhostvgpudeviceapiUnmapFrom__)(struct KernelHostVgpuDeviceApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__kernelhostvgpudeviceapiGetRefCount__)(struct KernelHostVgpuDeviceApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__kernelhostvgpudeviceapiAddAdditionalDependants__)(struct RsClient *, struct KernelHostVgpuDeviceApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__kernelhostvgpudeviceapiGetNotificationListPtr__)(struct KernelHostVgpuDeviceApi * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__kernelhostvgpudeviceapiGetNotificationShare__)(struct KernelHostVgpuDeviceApi * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__kernelhostvgpudeviceapiSetNotificationShare__)(struct KernelHostVgpuDeviceApi * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__kernelhostvgpudeviceapiUnregisterEvent__)(struct KernelHostVgpuDeviceApi * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__kernelhostvgpudeviceapiGetOrAllocNotifShare__)(struct KernelHostVgpuDeviceApi * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelHostVgpuDeviceApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
    const struct NVOC_VTABLE__KernelHostVgpuDeviceApi vtable;
};

#ifndef __NVOC_CLASS_KernelHostVgpuDeviceApi_TYPEDEF__
#define __NVOC_CLASS_KernelHostVgpuDeviceApi_TYPEDEF__
typedef struct KernelHostVgpuDeviceApi KernelHostVgpuDeviceApi;
#endif /* __NVOC_CLASS_KernelHostVgpuDeviceApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHostVgpuDeviceApi
#define __nvoc_class_id_KernelHostVgpuDeviceApi 0xb12d7d
#endif /* __nvoc_class_id_KernelHostVgpuDeviceApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceApi;

#define __staticCast_KernelHostVgpuDeviceApi(pThis) \
    ((pThis)->__nvoc_pbase_KernelHostVgpuDeviceApi)

#ifdef __nvoc_kernel_hostvgpudeviceapi_h_disabled
#define __dynamicCast_KernelHostVgpuDeviceApi(pThis) ((KernelHostVgpuDeviceApi*) NULL)
#else //__nvoc_kernel_hostvgpudeviceapi_h_disabled
#define __dynamicCast_KernelHostVgpuDeviceApi(pThis) \
    ((KernelHostVgpuDeviceApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelHostVgpuDeviceApi)))
#endif //__nvoc_kernel_hostvgpudeviceapi_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_KernelHostVgpuDeviceApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelHostVgpuDeviceApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define kernelhostvgpudeviceapiCanCopy_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiCanCopy__
#define kernelhostvgpudeviceapiCanCopy(pKernelHostVgpuDeviceApi) kernelhostvgpudeviceapiCanCopy_DISPATCH(pKernelHostVgpuDeviceApi)
#define kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo__
#define kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState__
#define kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo__
#define kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess__
#define kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf__
#define kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell__
#define kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdEventSetNotification_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdEventSetNotification__
#define kernelhostvgpudeviceapiCtrlCmdEventSetNotification(pKernelHostVgpuDeviceApi, pSetEventParams) kernelhostvgpudeviceapiCtrlCmdEventSetNotification_DISPATCH(pKernelHostVgpuDeviceApi, pSetEventParams)
#define kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges__
#define kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition__
#define kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition(pKernelHostVgpuDeviceApi) kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_DISPATCH(pKernelHostVgpuDeviceApi)
#define kernelhostvgpudeviceapiCtrlCmdFreeStates_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdFreeStates__
#define kernelhostvgpudeviceapiCtrlCmdFreeStates(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdFreeStates_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask__
#define kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdSetPlacementId_FNPTR(pKernelHostVgpuDeviceApi) pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetPlacementId__
#define kernelhostvgpudeviceapiCtrlCmdSetPlacementId(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdSetPlacementId_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define kernelhostvgpudeviceapiControl(pGpuResource, pCallContext, pParams) kernelhostvgpudeviceapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define kernelhostvgpudeviceapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) kernelhostvgpudeviceapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kernelhostvgpudeviceapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define kernelhostvgpudeviceapiUnmap(pGpuResource, pCallContext, pCpuMapping) kernelhostvgpudeviceapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kernelhostvgpudeviceapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define kernelhostvgpudeviceapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kernelhostvgpudeviceapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kernelhostvgpudeviceapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define kernelhostvgpudeviceapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kernelhostvgpudeviceapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kernelhostvgpudeviceapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define kernelhostvgpudeviceapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kernelhostvgpudeviceapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kernelhostvgpudeviceapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define kernelhostvgpudeviceapiInternalControlForward(pGpuResource, command, pParams, size) kernelhostvgpudeviceapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kernelhostvgpudeviceapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define kernelhostvgpudeviceapiGetInternalObjectHandle(pGpuResource) kernelhostvgpudeviceapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kernelhostvgpudeviceapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define kernelhostvgpudeviceapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kernelhostvgpudeviceapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define kernelhostvgpudeviceapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define kernelhostvgpudeviceapiGetMemInterMapParams(pRmResource, pParams) kernelhostvgpudeviceapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kernelhostvgpudeviceapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define kernelhostvgpudeviceapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kernelhostvgpudeviceapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kernelhostvgpudeviceapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define kernelhostvgpudeviceapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kernelhostvgpudeviceapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kernelhostvgpudeviceapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define kernelhostvgpudeviceapiControlSerialization_Prologue(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define kernelhostvgpudeviceapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define kernelhostvgpudeviceapiControl_Prologue(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define kernelhostvgpudeviceapiControl_Epilogue(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define kernelhostvgpudeviceapiIsDuplicate(pResource, hMemory, pDuplicate) kernelhostvgpudeviceapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kernelhostvgpudeviceapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define kernelhostvgpudeviceapiPreDestruct(pResource) kernelhostvgpudeviceapiPreDestruct_DISPATCH(pResource)
#define kernelhostvgpudeviceapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define kernelhostvgpudeviceapiControlFilter(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define kernelhostvgpudeviceapiIsPartialUnmapSupported(pResource) kernelhostvgpudeviceapiIsPartialUnmapSupported_DISPATCH(pResource)
#define kernelhostvgpudeviceapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define kernelhostvgpudeviceapiMapTo(pResource, pParams) kernelhostvgpudeviceapiMapTo_DISPATCH(pResource, pParams)
#define kernelhostvgpudeviceapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define kernelhostvgpudeviceapiUnmapFrom(pResource, pParams) kernelhostvgpudeviceapiUnmapFrom_DISPATCH(pResource, pParams)
#define kernelhostvgpudeviceapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define kernelhostvgpudeviceapiGetRefCount(pResource) kernelhostvgpudeviceapiGetRefCount_DISPATCH(pResource)
#define kernelhostvgpudeviceapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define kernelhostvgpudeviceapiAddAdditionalDependants(pClient, pResource, pReference) kernelhostvgpudeviceapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kernelhostvgpudeviceapiGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__
#define kernelhostvgpudeviceapiGetNotificationListPtr(pNotifier) kernelhostvgpudeviceapiGetNotificationListPtr_DISPATCH(pNotifier)
#define kernelhostvgpudeviceapiGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__
#define kernelhostvgpudeviceapiGetNotificationShare(pNotifier) kernelhostvgpudeviceapiGetNotificationShare_DISPATCH(pNotifier)
#define kernelhostvgpudeviceapiSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__
#define kernelhostvgpudeviceapiSetNotificationShare(pNotifier, pNotifShare) kernelhostvgpudeviceapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define kernelhostvgpudeviceapiUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__
#define kernelhostvgpudeviceapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) kernelhostvgpudeviceapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define kernelhostvgpudeviceapiGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__
#define kernelhostvgpudeviceapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) kernelhostvgpudeviceapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NvBool kernelhostvgpudeviceapiCanCopy_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi) {
    return pKernelHostVgpuDeviceApi->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiCanCopy__(pKernelHostVgpuDeviceApi);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdEventSetNotification_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdEventSetNotification__(pKernelHostVgpuDeviceApi, pSetEventParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition__(pKernelHostVgpuDeviceApi);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdFreeStates_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_FREE_STATES_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdFreeStates__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BOOTLOAD_VGPU_TASK_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetPlacementId_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_PLACEMENT_ID_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetPlacementId__(pKernelHostVgpuDeviceApi, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiControl_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiMap_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS kernelhostvgpudeviceapiUnmap_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool kernelhostvgpudeviceapiShareCallback_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetMapAddrSpace_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS kernelhostvgpudeviceapiInternalControlForward_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle kernelhostvgpudeviceapiGetInternalObjectHandle_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool kernelhostvgpudeviceapiAccessCallback_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetMemInterMapParams_DISPATCH(struct KernelHostVgpuDeviceApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiCheckMemInterUnmap_DISPATCH(struct KernelHostVgpuDeviceApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetMemoryMappingDescriptor_DISPATCH(struct KernelHostVgpuDeviceApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kernelhostvgpudeviceapiControlSerialization_Prologue_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kernelhostvgpudeviceapiControlSerialization_Epilogue_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiControl_Prologue_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void kernelhostvgpudeviceapiControl_Epilogue_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiIsDuplicate_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kernelhostvgpudeviceapiPreDestruct_DISPATCH(struct KernelHostVgpuDeviceApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiPreDestruct__(pResource);
}

static inline NV_STATUS kernelhostvgpudeviceapiControlFilter_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool kernelhostvgpudeviceapiIsPartialUnmapSupported_DISPATCH(struct KernelHostVgpuDeviceApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS kernelhostvgpudeviceapiMapTo_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiMapTo__(pResource, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiUnmapFrom_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiUnmapFrom__(pResource, pParams);
}

static inline NvU32 kernelhostvgpudeviceapiGetRefCount_DISPATCH(struct KernelHostVgpuDeviceApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiGetRefCount__(pResource);
}

static inline void kernelhostvgpudeviceapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelHostVgpuDeviceApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * kernelhostvgpudeviceapiGetNotificationListPtr_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * kernelhostvgpudeviceapiGetNotificationShare_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiGetNotificationShare__(pNotifier);
}

static inline void kernelhostvgpudeviceapiSetNotificationShare_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS kernelhostvgpudeviceapiUnregisterEvent_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetOrAllocNotifShare_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__kernelhostvgpudeviceapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NvBool kernelhostvgpudeviceapiCanCopy_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdEventSetNotification_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdFreeStates_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_FREE_STATES_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdBootloadVgpuTask_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BOOTLOAD_VGPU_TASK_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetPlacementId_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_CMD_KERNEL_HOST_VGPU_DEVICE_SET_PLACEMENT_ID_PARAMS *pParams);

NV_STATUS kernelhostvgpudeviceapiConstruct_IMPL(struct KernelHostVgpuDeviceApi *arg_pKernelHostVgpuDeviceApi, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_kernelhostvgpudeviceapiConstruct(arg_pKernelHostVgpuDeviceApi, arg_pCallContext, arg_pParams) kernelhostvgpudeviceapiConstruct_IMPL(arg_pKernelHostVgpuDeviceApi, arg_pCallContext, arg_pParams)
NV_STATUS kernelhostvgpudeviceapiCopyConstruct_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

#ifdef __nvoc_kernel_hostvgpudeviceapi_h_disabled
static inline NV_STATUS kernelhostvgpudeviceapiCopyConstruct(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelHostVgpuDeviceApi was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_hostvgpudeviceapi_h_disabled
#define kernelhostvgpudeviceapiCopyConstruct(pKernelHostVgpuDeviceApi, pCallContext, pParams) kernelhostvgpudeviceapiCopyConstruct_IMPL(pKernelHostVgpuDeviceApi, pCallContext, pParams)
#endif //__nvoc_kernel_hostvgpudeviceapi_h_disabled

void kernelhostvgpudeviceapiDestruct_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi);

#define __nvoc_kernelhostvgpudeviceapiDestruct(pKernelHostVgpuDeviceApi) kernelhostvgpudeviceapiDestruct_IMPL(pKernelHostVgpuDeviceApi)
#undef PRIVATE_FIELD


NV_STATUS
kernelhostvgpudeviceGetGuestFbInfo(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
                                   VGPU_DEVICE_GUEST_FB_INFO *pFbInfo);

NV_STATUS
kernelhostvgpudeviceSetGuestFbInfo(OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
                                   NvU64 offset, NvU64 length);

#endif // _KERNEL_HOSTVGPUDEVICEAPI_H_


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_HOSTVGPUDEVICEAPI_NVOC_H_
