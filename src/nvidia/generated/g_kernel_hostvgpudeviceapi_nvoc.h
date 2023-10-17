#ifndef _G_KERNEL_HOSTVGPUDEVICEAPI_NVOC_H_
#define _G_KERNEL_HOSTVGPUDEVICEAPI_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_hostvgpudeviceapi_nvoc.h"

#ifndef _KERNEL_HOSTVGPUDEVICEAPI_H_
#define _KERNEL_HOSTVGPUDEVICEAPI_H_

#include "core/core.h"
#include "virtualization/kernel_vgpu_mgr.h"
#include "rmapi/client.h"
#include "gpu/gpu_resource.h"

#include "class/cla084.h"

#ifdef NVOC_KERNEL_HOSTVGPUDEVICEAPI_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelHostVgpuDeviceShr {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RsShared __nvoc_base_RsShared;
    struct Object *__nvoc_pbase_Object;
    struct RsShared *__nvoc_pbase_RsShared;
    struct KernelHostVgpuDeviceShr *__nvoc_pbase_KernelHostVgpuDeviceShr;
    KERNEL_HOST_VGPU_DEVICE *pDevice;
};

#ifndef __NVOC_CLASS_KernelHostVgpuDeviceShr_TYPEDEF__
#define __NVOC_CLASS_KernelHostVgpuDeviceShr_TYPEDEF__
typedef struct KernelHostVgpuDeviceShr KernelHostVgpuDeviceShr;
#endif /* __NVOC_CLASS_KernelHostVgpuDeviceShr_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHostVgpuDeviceShr
#define __nvoc_class_id_KernelHostVgpuDeviceShr 0xe32156
#endif /* __nvoc_class_id_KernelHostVgpuDeviceShr */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceShr;

#define __staticCast_KernelHostVgpuDeviceShr(pThis) \
    ((pThis)->__nvoc_pbase_KernelHostVgpuDeviceShr)

#ifdef __nvoc_kernel_hostvgpudeviceapi_h_disabled
#define __dynamicCast_KernelHostVgpuDeviceShr(pThis) ((KernelHostVgpuDeviceShr*)NULL)
#else //__nvoc_kernel_hostvgpudeviceapi_h_disabled
#define __dynamicCast_KernelHostVgpuDeviceShr(pThis) \
    ((KernelHostVgpuDeviceShr*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelHostVgpuDeviceShr)))
#endif //__nvoc_kernel_hostvgpudeviceapi_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelHostVgpuDeviceShr(KernelHostVgpuDeviceShr**, Dynamic*, NvU32);
#define __objCreate_KernelHostVgpuDeviceShr(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelHostVgpuDeviceShr((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

NV_STATUS kernelhostvgpudeviceshrConstruct_IMPL(struct KernelHostVgpuDeviceShr *arg_pKernelHostVgpuDeviceShr);

#define __nvoc_kernelhostvgpudeviceshrConstruct(arg_pKernelHostVgpuDeviceShr) kernelhostvgpudeviceshrConstruct_IMPL(arg_pKernelHostVgpuDeviceShr)
void kernelhostvgpudeviceshrDestruct_IMPL(struct KernelHostVgpuDeviceShr *pKernelHostVgpuDeviceShr);

#define __nvoc_kernelhostvgpudeviceshrDestruct(pKernelHostVgpuDeviceShr) kernelhostvgpudeviceshrDestruct_IMPL(pKernelHostVgpuDeviceShr)
#undef PRIVATE_FIELD


void destroyKernelHostVgpuDeviceShare(struct OBJGPU *pGpu, struct KernelHostVgpuDeviceShr* pShare);

#ifdef NVOC_KERNEL_HOSTVGPUDEVICEAPI_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelHostVgpuDeviceApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct KernelHostVgpuDeviceApi *__nvoc_pbase_KernelHostVgpuDeviceApi;
    NvBool (*__kernelhostvgpudeviceapiCanCopy__)(struct KernelHostVgpuDeviceApi *);
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo__)(struct KernelHostVgpuDeviceApi *, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState__)(struct KernelHostVgpuDeviceApi *, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo__)(struct KernelHostVgpuDeviceApi *, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess__)(struct KernelHostVgpuDeviceApi *, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf__)(struct KernelHostVgpuDeviceApi *, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell__)(struct KernelHostVgpuDeviceApi *, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdEventSetNotification__)(struct KernelHostVgpuDeviceApi *, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges__)(struct KernelHostVgpuDeviceApi *, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition__)(struct KernelHostVgpuDeviceApi *);
    NvBool (*__kernelhostvgpudeviceapiShareCallback__)(struct KernelHostVgpuDeviceApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__kernelhostvgpudeviceapiCheckMemInterUnmap__)(struct KernelHostVgpuDeviceApi *, NvBool);
    NV_STATUS (*__kernelhostvgpudeviceapiGetOrAllocNotifShare__)(struct KernelHostVgpuDeviceApi *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__kernelhostvgpudeviceapiMapTo__)(struct KernelHostVgpuDeviceApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiGetMapAddrSpace__)(struct KernelHostVgpuDeviceApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__kernelhostvgpudeviceapiSetNotificationShare__)(struct KernelHostVgpuDeviceApi *, struct NotifShare *);
    NvU32 (*__kernelhostvgpudeviceapiGetRefCount__)(struct KernelHostVgpuDeviceApi *);
    void (*__kernelhostvgpudeviceapiAddAdditionalDependants__)(struct RsClient *, struct KernelHostVgpuDeviceApi *, RsResourceRef *);
    NV_STATUS (*__kernelhostvgpudeviceapiControl_Prologue__)(struct KernelHostVgpuDeviceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kernelhostvgpudeviceapiGetRegBaseOffsetAndSize__)(struct KernelHostVgpuDeviceApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__kernelhostvgpudeviceapiInternalControlForward__)(struct KernelHostVgpuDeviceApi *, NvU32, void *, NvU32);
    NV_STATUS (*__kernelhostvgpudeviceapiUnmapFrom__)(struct KernelHostVgpuDeviceApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__kernelhostvgpudeviceapiControl_Epilogue__)(struct KernelHostVgpuDeviceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kernelhostvgpudeviceapiControlLookup__)(struct KernelHostVgpuDeviceApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__kernelhostvgpudeviceapiGetInternalObjectHandle__)(struct KernelHostVgpuDeviceApi *);
    NV_STATUS (*__kernelhostvgpudeviceapiControl__)(struct KernelHostVgpuDeviceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kernelhostvgpudeviceapiUnmap__)(struct KernelHostVgpuDeviceApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__kernelhostvgpudeviceapiGetMemInterMapParams__)(struct KernelHostVgpuDeviceApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__kernelhostvgpudeviceapiGetMemoryMappingDescriptor__)(struct KernelHostVgpuDeviceApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__kernelhostvgpudeviceapiControlFilter__)(struct KernelHostVgpuDeviceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__kernelhostvgpudeviceapiUnregisterEvent__)(struct KernelHostVgpuDeviceApi *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__kernelhostvgpudeviceapiControlSerialization_Prologue__)(struct KernelHostVgpuDeviceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__kernelhostvgpudeviceapiPreDestruct__)(struct KernelHostVgpuDeviceApi *);
    NV_STATUS (*__kernelhostvgpudeviceapiIsDuplicate__)(struct KernelHostVgpuDeviceApi *, NvHandle, NvBool *);
    void (*__kernelhostvgpudeviceapiControlSerialization_Epilogue__)(struct KernelHostVgpuDeviceApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__kernelhostvgpudeviceapiGetNotificationListPtr__)(struct KernelHostVgpuDeviceApi *);
    struct NotifShare *(*__kernelhostvgpudeviceapiGetNotificationShare__)(struct KernelHostVgpuDeviceApi *);
    NV_STATUS (*__kernelhostvgpudeviceapiMap__)(struct KernelHostVgpuDeviceApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__kernelhostvgpudeviceapiAccessCallback__)(struct KernelHostVgpuDeviceApi *, struct RsClient *, void *, RsAccessRight);
    struct KernelHostVgpuDeviceShr *pShared;
    NvU32 notifyActions[7];
};

#ifndef __NVOC_CLASS_KernelHostVgpuDeviceApi_TYPEDEF__
#define __NVOC_CLASS_KernelHostVgpuDeviceApi_TYPEDEF__
typedef struct KernelHostVgpuDeviceApi KernelHostVgpuDeviceApi;
#endif /* __NVOC_CLASS_KernelHostVgpuDeviceApi_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHostVgpuDeviceApi
#define __nvoc_class_id_KernelHostVgpuDeviceApi 0xb12d7d
#endif /* __nvoc_class_id_KernelHostVgpuDeviceApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHostVgpuDeviceApi;

#define __staticCast_KernelHostVgpuDeviceApi(pThis) \
    ((pThis)->__nvoc_pbase_KernelHostVgpuDeviceApi)

#ifdef __nvoc_kernel_hostvgpudeviceapi_h_disabled
#define __dynamicCast_KernelHostVgpuDeviceApi(pThis) ((KernelHostVgpuDeviceApi*)NULL)
#else //__nvoc_kernel_hostvgpudeviceapi_h_disabled
#define __dynamicCast_KernelHostVgpuDeviceApi(pThis) \
    ((KernelHostVgpuDeviceApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelHostVgpuDeviceApi)))
#endif //__nvoc_kernel_hostvgpudeviceapi_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelHostVgpuDeviceApi(KernelHostVgpuDeviceApi**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_KernelHostVgpuDeviceApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelHostVgpuDeviceApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define kernelhostvgpudeviceapiCanCopy(pKernelHostVgpuDeviceApi) kernelhostvgpudeviceapiCanCopy_DISPATCH(pKernelHostVgpuDeviceApi)
#define kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdEventSetNotification(pKernelHostVgpuDeviceApi, pSetEventParams) kernelhostvgpudeviceapiCtrlCmdEventSetNotification_DISPATCH(pKernelHostVgpuDeviceApi, pSetEventParams)
#define kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges(pKernelHostVgpuDeviceApi, pParams) kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_DISPATCH(pKernelHostVgpuDeviceApi, pParams)
#define kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition(pKernelHostVgpuDeviceApi) kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_DISPATCH(pKernelHostVgpuDeviceApi)
#define kernelhostvgpudeviceapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) kernelhostvgpudeviceapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define kernelhostvgpudeviceapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) kernelhostvgpudeviceapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define kernelhostvgpudeviceapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) kernelhostvgpudeviceapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define kernelhostvgpudeviceapiMapTo(pResource, pParams) kernelhostvgpudeviceapiMapTo_DISPATCH(pResource, pParams)
#define kernelhostvgpudeviceapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) kernelhostvgpudeviceapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define kernelhostvgpudeviceapiSetNotificationShare(pNotifier, pNotifShare) kernelhostvgpudeviceapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define kernelhostvgpudeviceapiGetRefCount(pResource) kernelhostvgpudeviceapiGetRefCount_DISPATCH(pResource)
#define kernelhostvgpudeviceapiAddAdditionalDependants(pClient, pResource, pReference) kernelhostvgpudeviceapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define kernelhostvgpudeviceapiControl_Prologue(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) kernelhostvgpudeviceapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define kernelhostvgpudeviceapiInternalControlForward(pGpuResource, command, pParams, size) kernelhostvgpudeviceapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define kernelhostvgpudeviceapiUnmapFrom(pResource, pParams) kernelhostvgpudeviceapiUnmapFrom_DISPATCH(pResource, pParams)
#define kernelhostvgpudeviceapiControl_Epilogue(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiControlLookup(pResource, pParams, ppEntry) kernelhostvgpudeviceapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define kernelhostvgpudeviceapiGetInternalObjectHandle(pGpuResource) kernelhostvgpudeviceapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define kernelhostvgpudeviceapiControl(pGpuResource, pCallContext, pParams) kernelhostvgpudeviceapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiUnmap(pGpuResource, pCallContext, pCpuMapping) kernelhostvgpudeviceapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define kernelhostvgpudeviceapiGetMemInterMapParams(pRmResource, pParams) kernelhostvgpudeviceapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define kernelhostvgpudeviceapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) kernelhostvgpudeviceapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define kernelhostvgpudeviceapiControlFilter(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) kernelhostvgpudeviceapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define kernelhostvgpudeviceapiControlSerialization_Prologue(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiPreDestruct(pResource) kernelhostvgpudeviceapiPreDestruct_DISPATCH(pResource)
#define kernelhostvgpudeviceapiIsDuplicate(pResource, hMemory, pDuplicate) kernelhostvgpudeviceapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define kernelhostvgpudeviceapiControlSerialization_Epilogue(pResource, pCallContext, pParams) kernelhostvgpudeviceapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define kernelhostvgpudeviceapiGetNotificationListPtr(pNotifier) kernelhostvgpudeviceapiGetNotificationListPtr_DISPATCH(pNotifier)
#define kernelhostvgpudeviceapiGetNotificationShare(pNotifier) kernelhostvgpudeviceapiGetNotificationShare_DISPATCH(pNotifier)
#define kernelhostvgpudeviceapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) kernelhostvgpudeviceapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define kernelhostvgpudeviceapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) kernelhostvgpudeviceapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool kernelhostvgpudeviceapiCanCopy_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi);

static inline NvBool kernelhostvgpudeviceapiCanCopy_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCanCopy__(pKernelHostVgpuDeviceApi);
}

NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS *pParams);

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_DEVICE_INFO_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetVgpuDeviceInfo__(pKernelHostVgpuDeviceApi, pParams);
}

NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS *pParams);

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_VGPU_GUEST_LIFE_CYCLE_STATE_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetVgpuGuestLifeCycleState__(pKernelHostVgpuDeviceApi, pParams);
}

NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS *pParams);

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_SET_OFFLINED_PAGE_PATCHINFO_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdSetOfflinedPagePatchInfo__(pKernelHostVgpuDeviceApi, pParams);
}

NV_STATUS kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS *pParams);

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_VF_CONFIG_SPACE_ACCESS_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdVfConfigSpaceAccess__(pKernelHostVgpuDeviceApi, pParams);
}

NV_STATUS kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS *pParams);

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_BIND_FECS_EVTBUF_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdBindFecsEvtbuf__(pKernelHostVgpuDeviceApi, pParams);
}

NV_STATUS kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS *pParams);

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_TRIGGER_PRIV_DOORBELL_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdTriggerPrivDoorbell__(pKernelHostVgpuDeviceApi, pParams);
}

NV_STATUS kernelhostvgpudeviceapiCtrlCmdEventSetNotification_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdEventSetNotification_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdEventSetNotification__(pKernelHostVgpuDeviceApi, pSetEventParams);
}

NV_STATUS kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS *pParams);

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi, NVA084_CTRL_KERNEL_HOST_VGPU_DEVICE_GET_BAR_MAPPING_RANGES_PARAMS *pParams) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdGetBarMappingRanges__(pKernelHostVgpuDeviceApi, pParams);
}

NV_STATUS kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_IMPL(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi);

static inline NV_STATUS kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition_DISPATCH(struct KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi) {
    return pKernelHostVgpuDeviceApi->__kernelhostvgpudeviceapiCtrlCmdRestoreDefaultExecPartition__(pKernelHostVgpuDeviceApi);
}

static inline NvBool kernelhostvgpudeviceapiShareCallback_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__kernelhostvgpudeviceapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS kernelhostvgpudeviceapiCheckMemInterUnmap_DISPATCH(struct KernelHostVgpuDeviceApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__kernelhostvgpudeviceapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetOrAllocNotifShare_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__kernelhostvgpudeviceapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS kernelhostvgpudeviceapiMapTo_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__kernelhostvgpudeviceapiMapTo__(pResource, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetMapAddrSpace_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__kernelhostvgpudeviceapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void kernelhostvgpudeviceapiSetNotificationShare_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__kernelhostvgpudeviceapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 kernelhostvgpudeviceapiGetRefCount_DISPATCH(struct KernelHostVgpuDeviceApi *pResource) {
    return pResource->__kernelhostvgpudeviceapiGetRefCount__(pResource);
}

static inline void kernelhostvgpudeviceapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelHostVgpuDeviceApi *pResource, RsResourceRef *pReference) {
    pResource->__kernelhostvgpudeviceapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS kernelhostvgpudeviceapiControl_Prologue_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kernelhostvgpudeviceapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetRegBaseOffsetAndSize_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__kernelhostvgpudeviceapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS kernelhostvgpudeviceapiInternalControlForward_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__kernelhostvgpudeviceapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS kernelhostvgpudeviceapiUnmapFrom_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__kernelhostvgpudeviceapiUnmapFrom__(pResource, pParams);
}

static inline void kernelhostvgpudeviceapiControl_Epilogue_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kernelhostvgpudeviceapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiControlLookup_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__kernelhostvgpudeviceapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle kernelhostvgpudeviceapiGetInternalObjectHandle_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource) {
    return pGpuResource->__kernelhostvgpudeviceapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS kernelhostvgpudeviceapiControl_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__kernelhostvgpudeviceapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiUnmap_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kernelhostvgpudeviceapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetMemInterMapParams_DISPATCH(struct KernelHostVgpuDeviceApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__kernelhostvgpudeviceapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiGetMemoryMappingDescriptor_DISPATCH(struct KernelHostVgpuDeviceApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__kernelhostvgpudeviceapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS kernelhostvgpudeviceapiControlFilter_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kernelhostvgpudeviceapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS kernelhostvgpudeviceapiUnregisterEvent_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__kernelhostvgpudeviceapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS kernelhostvgpudeviceapiControlSerialization_Prologue_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__kernelhostvgpudeviceapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void kernelhostvgpudeviceapiPreDestruct_DISPATCH(struct KernelHostVgpuDeviceApi *pResource) {
    pResource->__kernelhostvgpudeviceapiPreDestruct__(pResource);
}

static inline NV_STATUS kernelhostvgpudeviceapiIsDuplicate_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__kernelhostvgpudeviceapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void kernelhostvgpudeviceapiControlSerialization_Epilogue_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__kernelhostvgpudeviceapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *kernelhostvgpudeviceapiGetNotificationListPtr_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier) {
    return pNotifier->__kernelhostvgpudeviceapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *kernelhostvgpudeviceapiGetNotificationShare_DISPATCH(struct KernelHostVgpuDeviceApi *pNotifier) {
    return pNotifier->__kernelhostvgpudeviceapiGetNotificationShare__(pNotifier);
}

static inline NV_STATUS kernelhostvgpudeviceapiMap_DISPATCH(struct KernelHostVgpuDeviceApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__kernelhostvgpudeviceapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool kernelhostvgpudeviceapiAccessCallback_DISPATCH(struct KernelHostVgpuDeviceApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__kernelhostvgpudeviceapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
kernelhostvgpudeviceGetGuestFbInfo(struct OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
                                   VGPU_DEVICE_GUEST_FB_INFO *pFbInfo);

NV_STATUS
kernelhostvgpudeviceSetGuestFbInfo(struct OBJGPU *pGpu, KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice,
                                   NvU64 offset, NvU64 length);

#endif // _KERNEL_HOSTVGPUDEVICEAPI_H_


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_HOSTVGPUDEVICEAPI_NVOC_H_
