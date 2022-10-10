#ifndef _G_DISP_CHANNEL_NVOC_H_
#define _G_DISP_CHANNEL_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*   Description:
*       This file contains functions managing DispChannel and its derived classes.
*
******************************************************************************/

#include "g_disp_channel_nvoc.h"

#ifndef DISP_CHANNEL_H
#define DISP_CHANNEL_H

#include "gpu/gpu_resource.h"
#include "rmapi/event.h"

struct ContextDma;

#ifndef __NVOC_CLASS_ContextDma_TYPEDEF__
#define __NVOC_CLASS_ContextDma_TYPEDEF__
typedef struct ContextDma ContextDma;
#endif /* __NVOC_CLASS_ContextDma_TYPEDEF__ */

#ifndef __nvoc_class_id_ContextDma
#define __nvoc_class_id_ContextDma 0x88441b
#endif /* __nvoc_class_id_ContextDma */


struct DispObject;

#ifndef __NVOC_CLASS_DispObject_TYPEDEF__
#define __NVOC_CLASS_DispObject_TYPEDEF__
typedef struct DispObject DispObject;
#endif /* __NVOC_CLASS_DispObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DispObject
#define __nvoc_class_id_DispObject 0x999839
#endif /* __nvoc_class_id_DispObject */



/*!
 * Base class for display channels
 */
#ifdef NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DispChannel {
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
    struct DispChannel *__nvoc_pbase_DispChannel;
    NV_STATUS (*__dispchnGetRegBaseOffsetAndSize__)(struct DispChannel *, struct OBJGPU *, NvU32 *, NvU32 *);
    NvBool (*__dispchnShareCallback__)(struct DispChannel *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dispchnMapTo__)(struct DispChannel *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__dispchnGetOrAllocNotifShare__)(struct DispChannel *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__dispchnCheckMemInterUnmap__)(struct DispChannel *, NvBool);
    NV_STATUS (*__dispchnGetMapAddrSpace__)(struct DispChannel *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__dispchnSetNotificationShare__)(struct DispChannel *, struct NotifShare *);
    NvU32 (*__dispchnGetRefCount__)(struct DispChannel *);
    void (*__dispchnAddAdditionalDependants__)(struct RsClient *, struct DispChannel *, RsResourceRef *);
    NV_STATUS (*__dispchnControl_Prologue__)(struct DispChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchnInternalControlForward__)(struct DispChannel *, NvU32, void *, NvU32);
    NV_STATUS (*__dispchnUnmapFrom__)(struct DispChannel *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dispchnControl_Epilogue__)(struct DispChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchnControlLookup__)(struct DispChannel *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__dispchnGetInternalObjectHandle__)(struct DispChannel *);
    NV_STATUS (*__dispchnControl__)(struct DispChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchnUnmap__)(struct DispChannel *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__dispchnGetMemInterMapParams__)(struct DispChannel *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispchnGetMemoryMappingDescriptor__)(struct DispChannel *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__dispchnControlFilter__)(struct DispChannel *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchnUnregisterEvent__)(struct DispChannel *, NvHandle, NvHandle, NvHandle, NvHandle);
    NvBool (*__dispchnCanCopy__)(struct DispChannel *);
    void (*__dispchnPreDestruct__)(struct DispChannel *);
    PEVENTNOTIFICATION *(*__dispchnGetNotificationListPtr__)(struct DispChannel *);
    struct NotifShare *(*__dispchnGetNotificationShare__)(struct DispChannel *);
    NV_STATUS (*__dispchnMap__)(struct DispChannel *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__dispchnAccessCallback__)(struct DispChannel *, struct RsClient *, void *, RsAccessRight);
    struct DispObject *pDispObject;
    NvU32 DispClass;
    NvU32 InstanceNumber;
    NvP64 pControl;
    NvP64 pPriv;
    NvU32 ControlOffset;
    NvU32 ControlLength;
    NvBool bIsDma;
};

#ifndef __NVOC_CLASS_DispChannel_TYPEDEF__
#define __NVOC_CLASS_DispChannel_TYPEDEF__
typedef struct DispChannel DispChannel;
#endif /* __NVOC_CLASS_DispChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannel
#define __nvoc_class_id_DispChannel 0xbd2ff3
#endif /* __nvoc_class_id_DispChannel */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel;

#define __staticCast_DispChannel(pThis) \
    ((pThis)->__nvoc_pbase_DispChannel)

#ifdef __nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannel(pThis) ((DispChannel*)NULL)
#else //__nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannel(pThis) \
    ((DispChannel*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispChannel)))
#endif //__nvoc_disp_channel_h_disabled


NV_STATUS __nvoc_objCreateDynamic_DispChannel(DispChannel**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispChannel(DispChannel**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, NvU32 arg_isDma);
#define __objCreate_DispChannel(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams, arg_isDma) \
    __nvoc_objCreate_DispChannel((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams, arg_isDma)

#define dispchnGetRegBaseOffsetAndSize(pDispChannel, pGpu, pOffset, pSize) dispchnGetRegBaseOffsetAndSize_DISPATCH(pDispChannel, pGpu, pOffset, pSize)
#define dispchnShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispchnShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispchnMapTo(pResource, pParams) dispchnMapTo_DISPATCH(pResource, pParams)
#define dispchnGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispchnGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define dispchnCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispchnCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispchnGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispchnGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispchnSetNotificationShare(pNotifier, pNotifShare) dispchnSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispchnGetRefCount(pResource) dispchnGetRefCount_DISPATCH(pResource)
#define dispchnAddAdditionalDependants(pClient, pResource, pReference) dispchnAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispchnControl_Prologue(pResource, pCallContext, pParams) dispchnControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnInternalControlForward(pGpuResource, command, pParams, size) dispchnInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispchnUnmapFrom(pResource, pParams) dispchnUnmapFrom_DISPATCH(pResource, pParams)
#define dispchnControl_Epilogue(pResource, pCallContext, pParams) dispchnControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnControlLookup(pResource, pParams, ppEntry) dispchnControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispchnGetInternalObjectHandle(pGpuResource) dispchnGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispchnControl(pGpuResource, pCallContext, pParams) dispchnControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispchnUnmap(pGpuResource, pCallContext, pCpuMapping) dispchnUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispchnGetMemInterMapParams(pRmResource, pParams) dispchnGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispchnGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispchnGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispchnControlFilter(pResource, pCallContext, pParams) dispchnControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispchnUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispchnUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispchnCanCopy(pResource) dispchnCanCopy_DISPATCH(pResource)
#define dispchnPreDestruct(pResource) dispchnPreDestruct_DISPATCH(pResource)
#define dispchnGetNotificationListPtr(pNotifier) dispchnGetNotificationListPtr_DISPATCH(pNotifier)
#define dispchnGetNotificationShare(pNotifier) dispchnGetNotificationShare_DISPATCH(pNotifier)
#define dispchnMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispchnMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispchnAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispchnAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS dispchnGetRegBaseOffsetAndSize_IMPL(struct DispChannel *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

static inline NV_STATUS dispchnGetRegBaseOffsetAndSize_DISPATCH(struct DispChannel *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispChannel->__dispchnGetRegBaseOffsetAndSize__(pDispChannel, pGpu, pOffset, pSize);
}

static inline NvBool dispchnShareCallback_DISPATCH(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispchnShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispchnMapTo_DISPATCH(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispchnMapTo__(pResource, pParams);
}

static inline NV_STATUS dispchnGetOrAllocNotifShare_DISPATCH(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispchnGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS dispchnCheckMemInterUnmap_DISPATCH(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispchnCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispchnGetMapAddrSpace_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispchnGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void dispchnSetNotificationShare_DISPATCH(struct DispChannel *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispchnSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 dispchnGetRefCount_DISPATCH(struct DispChannel *pResource) {
    return pResource->__dispchnGetRefCount__(pResource);
}

static inline void dispchnAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference) {
    pResource->__dispchnAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS dispchnControl_Prologue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnInternalControlForward_DISPATCH(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispchnInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS dispchnUnmapFrom_DISPATCH(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispchnUnmapFrom__(pResource, pParams);
}

static inline void dispchnControl_Epilogue_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispchnControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnControlLookup_DISPATCH(struct DispChannel *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispchnControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle dispchnGetInternalObjectHandle_DISPATCH(struct DispChannel *pGpuResource) {
    return pGpuResource->__dispchnGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS dispchnControl_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispchnControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnUnmap_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchnUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispchnGetMemInterMapParams_DISPATCH(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispchnGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispchnGetMemoryMappingDescriptor_DISPATCH(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispchnGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispchnControlFilter_DISPATCH(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnUnregisterEvent_DISPATCH(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispchnUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NvBool dispchnCanCopy_DISPATCH(struct DispChannel *pResource) {
    return pResource->__dispchnCanCopy__(pResource);
}

static inline void dispchnPreDestruct_DISPATCH(struct DispChannel *pResource) {
    pResource->__dispchnPreDestruct__(pResource);
}

static inline PEVENTNOTIFICATION *dispchnGetNotificationListPtr_DISPATCH(struct DispChannel *pNotifier) {
    return pNotifier->__dispchnGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *dispchnGetNotificationShare_DISPATCH(struct DispChannel *pNotifier) {
    return pNotifier->__dispchnGetNotificationShare__(pNotifier);
}

static inline NV_STATUS dispchnMap_DISPATCH(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchnMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool dispchnAccessCallback_DISPATCH(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispchnAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS dispchnConstruct_IMPL(struct DispChannel *arg_pDispChannel, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams, NvU32 arg_isDma);
#define __nvoc_dispchnConstruct(arg_pDispChannel, arg_pCallContext, arg_pParams, arg_isDma) dispchnConstruct_IMPL(arg_pDispChannel, arg_pCallContext, arg_pParams, arg_isDma)
void dispchnDestruct_IMPL(struct DispChannel *pDispChannel);
#define __nvoc_dispchnDestruct(pDispChannel) dispchnDestruct_IMPL(pDispChannel)
void dispchnSetRegBaseOffsetAndSize_IMPL(struct DispChannel *pDispChannel, struct OBJGPU *pGpu);
#ifdef __nvoc_disp_channel_h_disabled
static inline void dispchnSetRegBaseOffsetAndSize(struct DispChannel *pDispChannel, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("DispChannel was disabled!");
}
#else //__nvoc_disp_channel_h_disabled
#define dispchnSetRegBaseOffsetAndSize(pDispChannel, pGpu) dispchnSetRegBaseOffsetAndSize_IMPL(pDispChannel, pGpu)
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS dispchnGrabChannel_IMPL(struct DispChannel *pDispChannel, NvHandle hClient, NvHandle hParent, NvHandle hChannel, NvU32 hClass, void *pAllocParms);
#ifdef __nvoc_disp_channel_h_disabled
static inline NV_STATUS dispchnGrabChannel(struct DispChannel *pDispChannel, NvHandle hClient, NvHandle hParent, NvHandle hChannel, NvU32 hClass, void *pAllocParms) {
    NV_ASSERT_FAILED_PRECOMP("DispChannel was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_disp_channel_h_disabled
#define dispchnGrabChannel(pDispChannel, hClient, hParent, hChannel, hClass, pAllocParms) dispchnGrabChannel_IMPL(pDispChannel, hClient, hParent, hChannel, hClass, pAllocParms)
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS dispchnBindCtx_IMPL(struct OBJGPU *pGpu, struct ContextDma *pContextDma, NvHandle hDispChannel);
#define dispchnBindCtx(pGpu, pContextDma, hDispChannel) dispchnBindCtx_IMPL(pGpu, pContextDma, hDispChannel)
NV_STATUS dispchnUnbindCtx_IMPL(struct OBJGPU *pGpu, struct ContextDma *pContextDma, NvHandle hDispChannel);
#define dispchnUnbindCtx(pGpu, pContextDma, hDispChannel) dispchnUnbindCtx_IMPL(pGpu, pContextDma, hDispChannel)
void dispchnUnbindCtxFromAllChannels_IMPL(struct OBJGPU *pGpu, struct ContextDma *pContextDma);
#define dispchnUnbindCtxFromAllChannels(pGpu, pContextDma) dispchnUnbindCtxFromAllChannels_IMPL(pGpu, pContextDma)
void dispchnUnbindAllCtx_IMPL(struct OBJGPU *pGpu, struct DispChannel *pDispChannel);
#ifdef __nvoc_disp_channel_h_disabled
static inline void dispchnUnbindAllCtx(struct OBJGPU *pGpu, struct DispChannel *pDispChannel) {
    NV_ASSERT_FAILED_PRECOMP("DispChannel was disabled!");
}
#else //__nvoc_disp_channel_h_disabled
#define dispchnUnbindAllCtx(pGpu, pDispChannel) dispchnUnbindAllCtx_IMPL(pGpu, pDispChannel)
#endif //__nvoc_disp_channel_h_disabled

NV_STATUS dispchnGetByHandle_IMPL(struct RsClient *pClient, NvHandle hDisplayChannel, struct DispChannel **ppDispChannel);
#define dispchnGetByHandle(pClient, hDisplayChannel, ppDispChannel) dispchnGetByHandle_IMPL(pClient, hDisplayChannel, ppDispChannel)
#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_XXX_CHANNEL_PIO
 */
#ifdef NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DispChannelPio {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct DispChannel __nvoc_base_DispChannel;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct DispChannel *__nvoc_pbase_DispChannel;
    struct DispChannelPio *__nvoc_pbase_DispChannelPio;
    NvBool (*__dispchnpioShareCallback__)(struct DispChannelPio *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dispchnpioMapTo__)(struct DispChannelPio *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__dispchnpioGetOrAllocNotifShare__)(struct DispChannelPio *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__dispchnpioCheckMemInterUnmap__)(struct DispChannelPio *, NvBool);
    NV_STATUS (*__dispchnpioGetMapAddrSpace__)(struct DispChannelPio *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__dispchnpioSetNotificationShare__)(struct DispChannelPio *, struct NotifShare *);
    NvU32 (*__dispchnpioGetRefCount__)(struct DispChannelPio *);
    void (*__dispchnpioAddAdditionalDependants__)(struct RsClient *, struct DispChannelPio *, RsResourceRef *);
    NV_STATUS (*__dispchnpioControl_Prologue__)(struct DispChannelPio *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchnpioGetRegBaseOffsetAndSize__)(struct DispChannelPio *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__dispchnpioInternalControlForward__)(struct DispChannelPio *, NvU32, void *, NvU32);
    NV_STATUS (*__dispchnpioUnmapFrom__)(struct DispChannelPio *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dispchnpioControl_Epilogue__)(struct DispChannelPio *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchnpioControlLookup__)(struct DispChannelPio *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__dispchnpioGetInternalObjectHandle__)(struct DispChannelPio *);
    NV_STATUS (*__dispchnpioControl__)(struct DispChannelPio *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchnpioUnmap__)(struct DispChannelPio *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__dispchnpioGetMemInterMapParams__)(struct DispChannelPio *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispchnpioGetMemoryMappingDescriptor__)(struct DispChannelPio *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__dispchnpioControlFilter__)(struct DispChannelPio *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchnpioUnregisterEvent__)(struct DispChannelPio *, NvHandle, NvHandle, NvHandle, NvHandle);
    NvBool (*__dispchnpioCanCopy__)(struct DispChannelPio *);
    void (*__dispchnpioPreDestruct__)(struct DispChannelPio *);
    PEVENTNOTIFICATION *(*__dispchnpioGetNotificationListPtr__)(struct DispChannelPio *);
    struct NotifShare *(*__dispchnpioGetNotificationShare__)(struct DispChannelPio *);
    NV_STATUS (*__dispchnpioMap__)(struct DispChannelPio *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__dispchnpioAccessCallback__)(struct DispChannelPio *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_DispChannelPio_TYPEDEF__
#define __NVOC_CLASS_DispChannelPio_TYPEDEF__
typedef struct DispChannelPio DispChannelPio;
#endif /* __NVOC_CLASS_DispChannelPio_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannelPio
#define __nvoc_class_id_DispChannelPio 0x10dec3
#endif /* __nvoc_class_id_DispChannelPio */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelPio;

#define __staticCast_DispChannelPio(pThis) \
    ((pThis)->__nvoc_pbase_DispChannelPio)

#ifdef __nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelPio(pThis) ((DispChannelPio*)NULL)
#else //__nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelPio(pThis) \
    ((DispChannelPio*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispChannelPio)))
#endif //__nvoc_disp_channel_h_disabled


NV_STATUS __nvoc_objCreateDynamic_DispChannelPio(DispChannelPio**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispChannelPio(DispChannelPio**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispChannelPio(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispChannelPio((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define dispchnpioShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispchnpioShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispchnpioMapTo(pResource, pParams) dispchnpioMapTo_DISPATCH(pResource, pParams)
#define dispchnpioGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispchnpioGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define dispchnpioCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispchnpioCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispchnpioGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispchnpioGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispchnpioSetNotificationShare(pNotifier, pNotifShare) dispchnpioSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispchnpioGetRefCount(pResource) dispchnpioGetRefCount_DISPATCH(pResource)
#define dispchnpioAddAdditionalDependants(pClient, pResource, pReference) dispchnpioAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispchnpioControl_Prologue(pResource, pCallContext, pParams) dispchnpioControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioGetRegBaseOffsetAndSize(pDispChannel, pGpu, pOffset, pSize) dispchnpioGetRegBaseOffsetAndSize_DISPATCH(pDispChannel, pGpu, pOffset, pSize)
#define dispchnpioInternalControlForward(pGpuResource, command, pParams, size) dispchnpioInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispchnpioUnmapFrom(pResource, pParams) dispchnpioUnmapFrom_DISPATCH(pResource, pParams)
#define dispchnpioControl_Epilogue(pResource, pCallContext, pParams) dispchnpioControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioControlLookup(pResource, pParams, ppEntry) dispchnpioControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispchnpioGetInternalObjectHandle(pGpuResource) dispchnpioGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispchnpioControl(pGpuResource, pCallContext, pParams) dispchnpioControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispchnpioUnmap(pGpuResource, pCallContext, pCpuMapping) dispchnpioUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispchnpioGetMemInterMapParams(pRmResource, pParams) dispchnpioGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispchnpioGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispchnpioGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispchnpioControlFilter(pResource, pCallContext, pParams) dispchnpioControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispchnpioUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispchnpioUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispchnpioCanCopy(pResource) dispchnpioCanCopy_DISPATCH(pResource)
#define dispchnpioPreDestruct(pResource) dispchnpioPreDestruct_DISPATCH(pResource)
#define dispchnpioGetNotificationListPtr(pNotifier) dispchnpioGetNotificationListPtr_DISPATCH(pNotifier)
#define dispchnpioGetNotificationShare(pNotifier) dispchnpioGetNotificationShare_DISPATCH(pNotifier)
#define dispchnpioMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispchnpioMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispchnpioAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispchnpioAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool dispchnpioShareCallback_DISPATCH(struct DispChannelPio *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispchnpioShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispchnpioMapTo_DISPATCH(struct DispChannelPio *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispchnpioMapTo__(pResource, pParams);
}

static inline NV_STATUS dispchnpioGetOrAllocNotifShare_DISPATCH(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispchnpioGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS dispchnpioCheckMemInterUnmap_DISPATCH(struct DispChannelPio *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispchnpioCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispchnpioGetMapAddrSpace_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispchnpioGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void dispchnpioSetNotificationShare_DISPATCH(struct DispChannelPio *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispchnpioSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 dispchnpioGetRefCount_DISPATCH(struct DispChannelPio *pResource) {
    return pResource->__dispchnpioGetRefCount__(pResource);
}

static inline void dispchnpioAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispChannelPio *pResource, RsResourceRef *pReference) {
    pResource->__dispchnpioAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS dispchnpioControl_Prologue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnpioControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnpioGetRegBaseOffsetAndSize_DISPATCH(struct DispChannelPio *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispChannel->__dispchnpioGetRegBaseOffsetAndSize__(pDispChannel, pGpu, pOffset, pSize);
}

static inline NV_STATUS dispchnpioInternalControlForward_DISPATCH(struct DispChannelPio *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispchnpioInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS dispchnpioUnmapFrom_DISPATCH(struct DispChannelPio *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispchnpioUnmapFrom__(pResource, pParams);
}

static inline void dispchnpioControl_Epilogue_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispchnpioControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnpioControlLookup_DISPATCH(struct DispChannelPio *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispchnpioControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle dispchnpioGetInternalObjectHandle_DISPATCH(struct DispChannelPio *pGpuResource) {
    return pGpuResource->__dispchnpioGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS dispchnpioControl_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispchnpioControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnpioUnmap_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchnpioUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispchnpioGetMemInterMapParams_DISPATCH(struct DispChannelPio *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispchnpioGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispchnpioGetMemoryMappingDescriptor_DISPATCH(struct DispChannelPio *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispchnpioGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispchnpioControlFilter_DISPATCH(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchnpioControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchnpioUnregisterEvent_DISPATCH(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispchnpioUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NvBool dispchnpioCanCopy_DISPATCH(struct DispChannelPio *pResource) {
    return pResource->__dispchnpioCanCopy__(pResource);
}

static inline void dispchnpioPreDestruct_DISPATCH(struct DispChannelPio *pResource) {
    pResource->__dispchnpioPreDestruct__(pResource);
}

static inline PEVENTNOTIFICATION *dispchnpioGetNotificationListPtr_DISPATCH(struct DispChannelPio *pNotifier) {
    return pNotifier->__dispchnpioGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *dispchnpioGetNotificationShare_DISPATCH(struct DispChannelPio *pNotifier) {
    return pNotifier->__dispchnpioGetNotificationShare__(pNotifier);
}

static inline NV_STATUS dispchnpioMap_DISPATCH(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchnpioMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool dispchnpioAccessCallback_DISPATCH(struct DispChannelPio *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispchnpioAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS dispchnpioConstruct_IMPL(struct DispChannelPio *arg_pDispChannelPio, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_dispchnpioConstruct(arg_pDispChannelPio, arg_pCallContext, arg_pParams) dispchnpioConstruct_IMPL(arg_pDispChannelPio, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


/*!
 * RM internal class representing XXX_XXX_CHANNEL_DMA
 */
#ifdef NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DispChannelDma {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct DispChannel __nvoc_base_DispChannel;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct DispChannel *__nvoc_pbase_DispChannel;
    struct DispChannelDma *__nvoc_pbase_DispChannelDma;
    NvBool (*__dispchndmaShareCallback__)(struct DispChannelDma *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__dispchndmaMapTo__)(struct DispChannelDma *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__dispchndmaGetOrAllocNotifShare__)(struct DispChannelDma *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__dispchndmaCheckMemInterUnmap__)(struct DispChannelDma *, NvBool);
    NV_STATUS (*__dispchndmaGetMapAddrSpace__)(struct DispChannelDma *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__dispchndmaSetNotificationShare__)(struct DispChannelDma *, struct NotifShare *);
    NvU32 (*__dispchndmaGetRefCount__)(struct DispChannelDma *);
    void (*__dispchndmaAddAdditionalDependants__)(struct RsClient *, struct DispChannelDma *, RsResourceRef *);
    NV_STATUS (*__dispchndmaControl_Prologue__)(struct DispChannelDma *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchndmaGetRegBaseOffsetAndSize__)(struct DispChannelDma *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__dispchndmaInternalControlForward__)(struct DispChannelDma *, NvU32, void *, NvU32);
    NV_STATUS (*__dispchndmaUnmapFrom__)(struct DispChannelDma *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dispchndmaControl_Epilogue__)(struct DispChannelDma *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchndmaControlLookup__)(struct DispChannelDma *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__dispchndmaGetInternalObjectHandle__)(struct DispChannelDma *);
    NV_STATUS (*__dispchndmaControl__)(struct DispChannelDma *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchndmaUnmap__)(struct DispChannelDma *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__dispchndmaGetMemInterMapParams__)(struct DispChannelDma *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispchndmaGetMemoryMappingDescriptor__)(struct DispChannelDma *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__dispchndmaControlFilter__)(struct DispChannelDma *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispchndmaUnregisterEvent__)(struct DispChannelDma *, NvHandle, NvHandle, NvHandle, NvHandle);
    NvBool (*__dispchndmaCanCopy__)(struct DispChannelDma *);
    void (*__dispchndmaPreDestruct__)(struct DispChannelDma *);
    PEVENTNOTIFICATION *(*__dispchndmaGetNotificationListPtr__)(struct DispChannelDma *);
    struct NotifShare *(*__dispchndmaGetNotificationShare__)(struct DispChannelDma *);
    NV_STATUS (*__dispchndmaMap__)(struct DispChannelDma *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__dispchndmaAccessCallback__)(struct DispChannelDma *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_DispChannelDma_TYPEDEF__
#define __NVOC_CLASS_DispChannelDma_TYPEDEF__
typedef struct DispChannelDma DispChannelDma;
#endif /* __NVOC_CLASS_DispChannelDma_TYPEDEF__ */

#ifndef __nvoc_class_id_DispChannelDma
#define __nvoc_class_id_DispChannelDma 0xfe3d2e
#endif /* __nvoc_class_id_DispChannelDma */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelDma;

#define __staticCast_DispChannelDma(pThis) \
    ((pThis)->__nvoc_pbase_DispChannelDma)

#ifdef __nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelDma(pThis) ((DispChannelDma*)NULL)
#else //__nvoc_disp_channel_h_disabled
#define __dynamicCast_DispChannelDma(pThis) \
    ((DispChannelDma*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispChannelDma)))
#endif //__nvoc_disp_channel_h_disabled


NV_STATUS __nvoc_objCreateDynamic_DispChannelDma(DispChannelDma**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispChannelDma(DispChannelDma**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispChannelDma(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispChannelDma((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define dispchndmaShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispchndmaShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispchndmaMapTo(pResource, pParams) dispchndmaMapTo_DISPATCH(pResource, pParams)
#define dispchndmaGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispchndmaGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define dispchndmaCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dispchndmaCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dispchndmaGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispchndmaGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispchndmaSetNotificationShare(pNotifier, pNotifShare) dispchndmaSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispchndmaGetRefCount(pResource) dispchndmaGetRefCount_DISPATCH(pResource)
#define dispchndmaAddAdditionalDependants(pClient, pResource, pReference) dispchndmaAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispchndmaControl_Prologue(pResource, pCallContext, pParams) dispchndmaControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaGetRegBaseOffsetAndSize(pDispChannel, pGpu, pOffset, pSize) dispchndmaGetRegBaseOffsetAndSize_DISPATCH(pDispChannel, pGpu, pOffset, pSize)
#define dispchndmaInternalControlForward(pGpuResource, command, pParams, size) dispchndmaInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispchndmaUnmapFrom(pResource, pParams) dispchndmaUnmapFrom_DISPATCH(pResource, pParams)
#define dispchndmaControl_Epilogue(pResource, pCallContext, pParams) dispchndmaControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaControlLookup(pResource, pParams, ppEntry) dispchndmaControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispchndmaGetInternalObjectHandle(pGpuResource) dispchndmaGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispchndmaControl(pGpuResource, pCallContext, pParams) dispchndmaControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispchndmaUnmap(pGpuResource, pCallContext, pCpuMapping) dispchndmaUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispchndmaGetMemInterMapParams(pRmResource, pParams) dispchndmaGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispchndmaGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispchndmaGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispchndmaControlFilter(pResource, pCallContext, pParams) dispchndmaControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispchndmaUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispchndmaUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispchndmaCanCopy(pResource) dispchndmaCanCopy_DISPATCH(pResource)
#define dispchndmaPreDestruct(pResource) dispchndmaPreDestruct_DISPATCH(pResource)
#define dispchndmaGetNotificationListPtr(pNotifier) dispchndmaGetNotificationListPtr_DISPATCH(pNotifier)
#define dispchndmaGetNotificationShare(pNotifier) dispchndmaGetNotificationShare_DISPATCH(pNotifier)
#define dispchndmaMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispchndmaMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispchndmaAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispchndmaAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
static inline NvBool dispchndmaShareCallback_DISPATCH(struct DispChannelDma *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispchndmaShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispchndmaMapTo_DISPATCH(struct DispChannelDma *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispchndmaMapTo__(pResource, pParams);
}

static inline NV_STATUS dispchndmaGetOrAllocNotifShare_DISPATCH(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispchndmaGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS dispchndmaCheckMemInterUnmap_DISPATCH(struct DispChannelDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__dispchndmaCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispchndmaGetMapAddrSpace_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispchndmaGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void dispchndmaSetNotificationShare_DISPATCH(struct DispChannelDma *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispchndmaSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 dispchndmaGetRefCount_DISPATCH(struct DispChannelDma *pResource) {
    return pResource->__dispchndmaGetRefCount__(pResource);
}

static inline void dispchndmaAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispChannelDma *pResource, RsResourceRef *pReference) {
    pResource->__dispchndmaAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS dispchndmaControl_Prologue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchndmaControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchndmaGetRegBaseOffsetAndSize_DISPATCH(struct DispChannelDma *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pDispChannel->__dispchndmaGetRegBaseOffsetAndSize__(pDispChannel, pGpu, pOffset, pSize);
}

static inline NV_STATUS dispchndmaInternalControlForward_DISPATCH(struct DispChannelDma *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispchndmaInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS dispchndmaUnmapFrom_DISPATCH(struct DispChannelDma *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispchndmaUnmapFrom__(pResource, pParams);
}

static inline void dispchndmaControl_Epilogue_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispchndmaControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchndmaControlLookup_DISPATCH(struct DispChannelDma *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispchndmaControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle dispchndmaGetInternalObjectHandle_DISPATCH(struct DispChannelDma *pGpuResource) {
    return pGpuResource->__dispchndmaGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS dispchndmaControl_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispchndmaControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispchndmaUnmap_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchndmaUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispchndmaGetMemInterMapParams_DISPATCH(struct DispChannelDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispchndmaGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispchndmaGetMemoryMappingDescriptor_DISPATCH(struct DispChannelDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispchndmaGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispchndmaControlFilter_DISPATCH(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispchndmaControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispchndmaUnregisterEvent_DISPATCH(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispchndmaUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NvBool dispchndmaCanCopy_DISPATCH(struct DispChannelDma *pResource) {
    return pResource->__dispchndmaCanCopy__(pResource);
}

static inline void dispchndmaPreDestruct_DISPATCH(struct DispChannelDma *pResource) {
    pResource->__dispchndmaPreDestruct__(pResource);
}

static inline PEVENTNOTIFICATION *dispchndmaGetNotificationListPtr_DISPATCH(struct DispChannelDma *pNotifier) {
    return pNotifier->__dispchndmaGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *dispchndmaGetNotificationShare_DISPATCH(struct DispChannelDma *pNotifier) {
    return pNotifier->__dispchndmaGetNotificationShare__(pNotifier);
}

static inline NV_STATUS dispchndmaMap_DISPATCH(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispchndmaMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool dispchndmaAccessCallback_DISPATCH(struct DispChannelDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispchndmaAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS dispchndmaConstruct_IMPL(struct DispChannelDma *arg_pDispChannelDma, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __nvoc_dispchndmaConstruct(arg_pDispChannelDma, arg_pCallContext, arg_pParams) dispchndmaConstruct_IMPL(arg_pDispChannelDma, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // DISP_CHANNEL_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_DISP_CHANNEL_NVOC_H_
