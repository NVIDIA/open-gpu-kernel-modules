#ifndef _G_VGPUCONFIGAPI_NVOC_H_
#define _G_VGPUCONFIGAPI_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

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
#include "g_vgpuconfigapi_nvoc.h"

#ifndef _VGPUCONFIGAPI_H_
#define _VGPUCONFIGAPI_H_

#include "core/core.h"
#include "class/cla081.h"
#include "rmapi/client.h"
#include "gpu/gpu_resource.h"
#include "rmapi/event.h"

#include "ctrl/ctrla081.h" // rmcontrol params

//
// Virtual GPU configuration information
//

#ifdef NVOC_VGPUCONFIGAPI_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct VgpuConfigApi {
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
    struct VgpuConfigApi *__nvoc_pbase_VgpuConfigApi;
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigSetInfo__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigNotifyStart__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_NOTIFY_START_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigMdevRegister__)(struct VgpuConfigApi *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap__)(struct VgpuConfigApi *, NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo__)(struct VgpuConfigApi *, NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdGetVgpuDriversCaps__)(struct VgpuConfigApi *, NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *);
    NV_STATUS (*__vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId__)(struct VgpuConfigApi *, NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *);
    NvBool (*__vgpuconfigapiShareCallback__)(struct VgpuConfigApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__vgpuconfigapiCheckMemInterUnmap__)(struct VgpuConfigApi *, NvBool);
    NV_STATUS (*__vgpuconfigapiGetOrAllocNotifShare__)(struct VgpuConfigApi *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__vgpuconfigapiMapTo__)(struct VgpuConfigApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__vgpuconfigapiGetMapAddrSpace__)(struct VgpuConfigApi *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__vgpuconfigapiSetNotificationShare__)(struct VgpuConfigApi *, struct NotifShare *);
    NvU32 (*__vgpuconfigapiGetRefCount__)(struct VgpuConfigApi *);
    void (*__vgpuconfigapiAddAdditionalDependants__)(struct RsClient *, struct VgpuConfigApi *, RsResourceRef *);
    NV_STATUS (*__vgpuconfigapiControl_Prologue__)(struct VgpuConfigApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vgpuconfigapiGetRegBaseOffsetAndSize__)(struct VgpuConfigApi *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__vgpuconfigapiInternalControlForward__)(struct VgpuConfigApi *, NvU32, void *, NvU32);
    NV_STATUS (*__vgpuconfigapiUnmapFrom__)(struct VgpuConfigApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__vgpuconfigapiControl_Epilogue__)(struct VgpuConfigApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vgpuconfigapiControlLookup__)(struct VgpuConfigApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__vgpuconfigapiGetInternalObjectHandle__)(struct VgpuConfigApi *);
    NV_STATUS (*__vgpuconfigapiControl__)(struct VgpuConfigApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vgpuconfigapiUnmap__)(struct VgpuConfigApi *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__vgpuconfigapiGetMemInterMapParams__)(struct VgpuConfigApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__vgpuconfigapiGetMemoryMappingDescriptor__)(struct VgpuConfigApi *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__vgpuconfigapiControlFilter__)(struct VgpuConfigApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__vgpuconfigapiUnregisterEvent__)(struct VgpuConfigApi *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__vgpuconfigapiControlSerialization_Prologue__)(struct VgpuConfigApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__vgpuconfigapiCanCopy__)(struct VgpuConfigApi *);
    void (*__vgpuconfigapiPreDestruct__)(struct VgpuConfigApi *);
    NV_STATUS (*__vgpuconfigapiIsDuplicate__)(struct VgpuConfigApi *, NvHandle, NvBool *);
    void (*__vgpuconfigapiControlSerialization_Epilogue__)(struct VgpuConfigApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__vgpuconfigapiGetNotificationListPtr__)(struct VgpuConfigApi *);
    struct NotifShare *(*__vgpuconfigapiGetNotificationShare__)(struct VgpuConfigApi *);
    NV_STATUS (*__vgpuconfigapiMap__)(struct VgpuConfigApi *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__vgpuconfigapiAccessCallback__)(struct VgpuConfigApi *, struct RsClient *, void *, RsAccessRight);
    NvU32 notifyActions[5];
};

#ifndef __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
#define __NVOC_CLASS_VgpuConfigApi_TYPEDEF__
typedef struct VgpuConfigApi VgpuConfigApi;
#endif /* __NVOC_CLASS_VgpuConfigApi_TYPEDEF__ */

#ifndef __nvoc_class_id_VgpuConfigApi
#define __nvoc_class_id_VgpuConfigApi 0x4d560a
#endif /* __nvoc_class_id_VgpuConfigApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_VgpuConfigApi;

#define __staticCast_VgpuConfigApi(pThis) \
    ((pThis)->__nvoc_pbase_VgpuConfigApi)

#ifdef __nvoc_vgpuconfigapi_h_disabled
#define __dynamicCast_VgpuConfigApi(pThis) ((VgpuConfigApi*)NULL)
#else //__nvoc_vgpuconfigapi_h_disabled
#define __dynamicCast_VgpuConfigApi(pThis) \
    ((VgpuConfigApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VgpuConfigApi)))
#endif //__nvoc_vgpuconfigapi_h_disabled


NV_STATUS __nvoc_objCreateDynamic_VgpuConfigApi(VgpuConfigApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VgpuConfigApi(VgpuConfigApi**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_VgpuConfigApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_VgpuConfigApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define vgpuconfigapiCtrlCmdVgpuConfigSetInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigSetInfo_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification(pVgpuConfigApi, pSetEventParams) vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_DISPATCH(pVgpuConfigApi, pSetEventParams)
#define vgpuconfigapiCtrlCmdVgpuConfigNotifyStart(pVgpuConfigApi, pNotifyParams) vgpuconfigapiCtrlCmdVgpuConfigNotifyStart_DISPATCH(pVgpuConfigApi, pNotifyParams)
#define vgpuconfigapiCtrlCmdVgpuConfigMdevRegister(pVgpuConfigApi) vgpuconfigapiCtrlCmdVgpuConfigMdevRegister_DISPATCH(pVgpuConfigApi)
#define vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity(pVgpuConfigApi, pEncoderParams) vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_DISPATCH(pVgpuConfigApi, pEncoderParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString(pVgpuConfigApi, pGpuMetadataStringParams) vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_DISPATCH(pVgpuConfigApi, pGpuMetadataStringParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdGetVgpuDriversCaps(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdGetVgpuDriversCaps_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId(pVgpuConfigApi, pParams) vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_DISPATCH(pVgpuConfigApi, pParams)
#define vgpuconfigapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) vgpuconfigapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define vgpuconfigapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) vgpuconfigapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define vgpuconfigapiGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) vgpuconfigapiGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define vgpuconfigapiMapTo(pResource, pParams) vgpuconfigapiMapTo_DISPATCH(pResource, pParams)
#define vgpuconfigapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) vgpuconfigapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define vgpuconfigapiSetNotificationShare(pNotifier, pNotifShare) vgpuconfigapiSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define vgpuconfigapiGetRefCount(pResource) vgpuconfigapiGetRefCount_DISPATCH(pResource)
#define vgpuconfigapiAddAdditionalDependants(pClient, pResource, pReference) vgpuconfigapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define vgpuconfigapiControl_Prologue(pResource, pCallContext, pParams) vgpuconfigapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) vgpuconfigapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define vgpuconfigapiInternalControlForward(pGpuResource, command, pParams, size) vgpuconfigapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define vgpuconfigapiUnmapFrom(pResource, pParams) vgpuconfigapiUnmapFrom_DISPATCH(pResource, pParams)
#define vgpuconfigapiControl_Epilogue(pResource, pCallContext, pParams) vgpuconfigapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiControlLookup(pResource, pParams, ppEntry) vgpuconfigapiControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define vgpuconfigapiGetInternalObjectHandle(pGpuResource) vgpuconfigapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define vgpuconfigapiControl(pGpuResource, pCallContext, pParams) vgpuconfigapiControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define vgpuconfigapiUnmap(pGpuResource, pCallContext, pCpuMapping) vgpuconfigapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define vgpuconfigapiGetMemInterMapParams(pRmResource, pParams) vgpuconfigapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define vgpuconfigapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) vgpuconfigapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define vgpuconfigapiControlFilter(pResource, pCallContext, pParams) vgpuconfigapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) vgpuconfigapiUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define vgpuconfigapiControlSerialization_Prologue(pResource, pCallContext, pParams) vgpuconfigapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiCanCopy(pResource) vgpuconfigapiCanCopy_DISPATCH(pResource)
#define vgpuconfigapiPreDestruct(pResource) vgpuconfigapiPreDestruct_DISPATCH(pResource)
#define vgpuconfigapiIsDuplicate(pResource, hMemory, pDuplicate) vgpuconfigapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define vgpuconfigapiControlSerialization_Epilogue(pResource, pCallContext, pParams) vgpuconfigapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define vgpuconfigapiGetNotificationListPtr(pNotifier) vgpuconfigapiGetNotificationListPtr_DISPATCH(pNotifier)
#define vgpuconfigapiGetNotificationShare(pNotifier) vgpuconfigapiGetNotificationShare_DISPATCH(pNotifier)
#define vgpuconfigapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) vgpuconfigapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define vgpuconfigapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) vgpuconfigapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_INFO_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetInfo__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_ENUMERATE_VGPU_PER_PGPU_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigEnumerateVgpuPerPgpu__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPE_INFO_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuTypeInfo__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetSupportedVgpuTypes__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_TYPES_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetCreatableVgpuTypes__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_EVENT_SET_NOTIFICATION_PARAMS *pSetEventParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigEventSetNotification__(pVgpuConfigApi, pSetEventParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigNotifyStart_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_NOTIFY_START_PARAMS *pNotifyParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigNotifyStart_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_NOTIFY_START_PARAMS *pNotifyParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigNotifyStart__(pVgpuConfigApi, pNotifyParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigMdevRegister_IMPL(struct VgpuConfigApi *pVgpuConfigApi);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigMdevRegister_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigMdevRegister__(pVgpuConfigApi);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *pEncoderParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VGPU_INSTANCE_ENCODER_CAPACITY_PARAMS *pEncoderParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetVgpuInstanceEncoderCapacity__(pVgpuConfigApi, pEncoderParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_VGPU_FB_USAGE_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetVgpuFbUsage__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_CMD_VGPU_CONFIG_GET_MIGRATION_CAP_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetMigrationCap__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_HOST_FB_RESERVATION_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetHostFbReservation__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *pGpuMetadataStringParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_PGPU_METADATA_STRING_PARAMS *pGpuMetadataStringParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetPgpuMetadataString__(pVgpuConfigApi, pGpuMetadataStringParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_DOORBELL_EMULATION_SUPPORT_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetDoorbellEmulationSupport__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_GET_FREE_SWIZZID_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigGetFreeSwizzId__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_PGPU_GET_MULTI_VGPU_SUPPORT_INFO_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdPgpuGetMultiVgpuSupportInfo__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdGetVgpuDriversCaps_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdGetVgpuDriversCaps_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_GET_VGPU_DRIVER_CAPS_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdGetVgpuDriversCaps__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_SET_PGPU_INFO_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigSetPgpuInfo__(pVgpuConfigApi, pParams);
}

NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_IMPL(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *pParams);

static inline NV_STATUS vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId_DISPATCH(struct VgpuConfigApi *pVgpuConfigApi, NVA081_CTRL_VGPU_CONFIG_VALIDATE_SWIZZID_PARAMS *pParams) {
    return pVgpuConfigApi->__vgpuconfigapiCtrlCmdVgpuConfigValidateSwizzId__(pVgpuConfigApi, pParams);
}

static inline NvBool vgpuconfigapiShareCallback_DISPATCH(struct VgpuConfigApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__vgpuconfigapiShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS vgpuconfigapiCheckMemInterUnmap_DISPATCH(struct VgpuConfigApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__vgpuconfigapiCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS vgpuconfigapiGetOrAllocNotifShare_DISPATCH(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__vgpuconfigapiGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS vgpuconfigapiMapTo_DISPATCH(struct VgpuConfigApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__vgpuconfigapiMapTo__(pResource, pParams);
}

static inline NV_STATUS vgpuconfigapiGetMapAddrSpace_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__vgpuconfigapiGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void vgpuconfigapiSetNotificationShare_DISPATCH(struct VgpuConfigApi *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__vgpuconfigapiSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 vgpuconfigapiGetRefCount_DISPATCH(struct VgpuConfigApi *pResource) {
    return pResource->__vgpuconfigapiGetRefCount__(pResource);
}

static inline void vgpuconfigapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct VgpuConfigApi *pResource, RsResourceRef *pReference) {
    pResource->__vgpuconfigapiAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS vgpuconfigapiControl_Prologue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vgpuconfigapiControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuconfigapiGetRegBaseOffsetAndSize_DISPATCH(struct VgpuConfigApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__vgpuconfigapiGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS vgpuconfigapiInternalControlForward_DISPATCH(struct VgpuConfigApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__vgpuconfigapiInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS vgpuconfigapiUnmapFrom_DISPATCH(struct VgpuConfigApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__vgpuconfigapiUnmapFrom__(pResource, pParams);
}

static inline void vgpuconfigapiControl_Epilogue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vgpuconfigapiControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuconfigapiControlLookup_DISPATCH(struct VgpuConfigApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__vgpuconfigapiControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle vgpuconfigapiGetInternalObjectHandle_DISPATCH(struct VgpuConfigApi *pGpuResource) {
    return pGpuResource->__vgpuconfigapiGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS vgpuconfigapiControl_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__vgpuconfigapiControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuconfigapiUnmap_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vgpuconfigapiUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS vgpuconfigapiGetMemInterMapParams_DISPATCH(struct VgpuConfigApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__vgpuconfigapiGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS vgpuconfigapiGetMemoryMappingDescriptor_DISPATCH(struct VgpuConfigApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__vgpuconfigapiGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS vgpuconfigapiControlFilter_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vgpuconfigapiControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS vgpuconfigapiUnregisterEvent_DISPATCH(struct VgpuConfigApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__vgpuconfigapiUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS vgpuconfigapiControlSerialization_Prologue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__vgpuconfigapiControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool vgpuconfigapiCanCopy_DISPATCH(struct VgpuConfigApi *pResource) {
    return pResource->__vgpuconfigapiCanCopy__(pResource);
}

static inline void vgpuconfigapiPreDestruct_DISPATCH(struct VgpuConfigApi *pResource) {
    pResource->__vgpuconfigapiPreDestruct__(pResource);
}

static inline NV_STATUS vgpuconfigapiIsDuplicate_DISPATCH(struct VgpuConfigApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__vgpuconfigapiIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void vgpuconfigapiControlSerialization_Epilogue_DISPATCH(struct VgpuConfigApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__vgpuconfigapiControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *vgpuconfigapiGetNotificationListPtr_DISPATCH(struct VgpuConfigApi *pNotifier) {
    return pNotifier->__vgpuconfigapiGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *vgpuconfigapiGetNotificationShare_DISPATCH(struct VgpuConfigApi *pNotifier) {
    return pNotifier->__vgpuconfigapiGetNotificationShare__(pNotifier);
}

static inline NV_STATUS vgpuconfigapiMap_DISPATCH(struct VgpuConfigApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__vgpuconfigapiMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool vgpuconfigapiAccessCallback_DISPATCH(struct VgpuConfigApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__vgpuconfigapiAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS vgpuconfigapiConstruct_IMPL(struct VgpuConfigApi *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_vgpuconfigapiConstruct(arg_pResource, arg_pCallContext, arg_pParams) vgpuconfigapiConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void vgpuconfigapiDestruct_IMPL(struct VgpuConfigApi *pResource);

#define __nvoc_vgpuconfigapiDestruct(pResource) vgpuconfigapiDestruct_IMPL(pResource)
#undef PRIVATE_FIELD


void                CliNotifyVgpuConfigEvent    (OBJGPU *,  NvU32);

#endif // _VGPUCONFIGAPI_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VGPUCONFIGAPI_NVOC_H_
