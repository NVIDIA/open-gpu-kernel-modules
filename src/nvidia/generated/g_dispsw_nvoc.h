#ifndef _G_DISPSW_NVOC_H_
#define _G_DISPSW_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_dispsw_nvoc.h"

#ifndef DISPSW_H
#define DISPSW_H

#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"
#include "gpu/disp/vblank_callback/vblank.h"
#include "gpu/gpu_halspec.h"

#include "ctrl/ctrl9072.h"

/* ------------------------ Types definitions ------------------------------ */
#define F_SEMAPHORE_ADDR_HI_VALID      (NVBIT(0))
#define F_SEMAPHORE_ADDR_LO_VALID      (NVBIT(1))
#define F_SEMAPHORE_ADDR_VALID         (F_SEMAPHORE_ADDR_HI_VALID | F_SEMAPHORE_ADDR_LO_VALID)
#define F_SEMAPHORE_RELEASE            (NVBIT(2))
#define F_NOTIFIER_FILL                (NVBIT(3))
/* ------------------------ Types definitions ------------------------------ */
typedef struct DispSwObject *PDISP_EVENT_SW_OBJECT;

#ifndef __NVOC_CLASS_DispSwObject_TYPEDEF__
#define __NVOC_CLASS_DispSwObject_TYPEDEF__
typedef struct DispSwObject DispSwObject;
#endif /* __NVOC_CLASS_DispSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSwObject
#define __nvoc_class_id_DispSwObject 0x99ad6d
#endif /* __nvoc_class_id_DispSwObject */



/* ------------------------ Macros & Defines ------------------------------- */
typedef struct DISPCOMMONOBJECT
{
    struct {
        /*!
         * A semaphore release location is specified by GPU virtual address.
         */
        union {
            NvU64       GPUVA;
        } Addr;
        NvU32       ReleaseValue;
        NvU32       ReleaseTrigger;
        VBLANKCALLBACK ReleaseCallback;
    } Semaphore;

    NvU32           SwapReadyMode;
    NvU32           Head;

    /*!
    * a pointer back to the "child" (encapsulating) arch-specific object
    */
    PDISP_EVENT_SW_OBJECT DispObject;
} DISPCOMMONOBJECT, *PDISPCOMMONOBJECT;

struct DISP_SW_OBJECT_NOTIFY
{
    NvU64          NotifierGPUVA;
    NvU32          NotifyAction;
    NvU32          NotifyTrigger;
    VBLANKCALLBACK Callback;
};

NV_STATUS dispswReleaseSemaphoreAndNotifierFill(struct OBJGPU *pGpu,
                                                NvU64 gpuVA,
                                                NvU32 vaSpace,
                                                NvU32 releasevalue,
                                                NvU32 flags,
                                                NvU32 completionStatus,
                                                struct Device *pDevice);

/*!
 * RM internal class representing GF100_DISP_SW
 */
#ifdef NVOC_DISPSW_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct DispSwObject {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct ChannelDescendant __nvoc_base_ChannelDescendant;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct ChannelDescendant *__nvoc_pbase_ChannelDescendant;
    struct DispSwObject *__nvoc_pbase_DispSwObject;
    NV_STATUS (*__dispswGetSwMethods__)(struct DispSwObject *, const METHOD **, NvU32 *);
    NV_STATUS (*__dispswCtrlCmdNotifyOnVblank__)(struct DispSwObject *, NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS *);
    NV_STATUS (*__dispswCheckMemInterUnmap__)(struct DispSwObject *, NvBool);
    NvBool (*__dispswShareCallback__)(struct DispSwObject *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__dispswAccessCallback__)(struct DispSwObject *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__dispswMapTo__)(struct DispSwObject *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__dispswGetMapAddrSpace__)(struct DispSwObject *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__dispswSetNotificationShare__)(struct DispSwObject *, struct NotifShare *);
    NvU32 (*__dispswGetRefCount__)(struct DispSwObject *);
    void (*__dispswAddAdditionalDependants__)(struct RsClient *, struct DispSwObject *, RsResourceRef *);
    NV_STATUS (*__dispswControl_Prologue__)(struct DispSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispswGetRegBaseOffsetAndSize__)(struct DispSwObject *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__dispswInternalControlForward__)(struct DispSwObject *, NvU32, void *, NvU32);
    NV_STATUS (*__dispswUnmapFrom__)(struct DispSwObject *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__dispswControl_Epilogue__)(struct DispSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispswControlLookup__)(struct DispSwObject *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__dispswGetInternalObjectHandle__)(struct DispSwObject *);
    NV_STATUS (*__dispswControl__)(struct DispSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispswUnmap__)(struct DispSwObject *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__dispswGetMemInterMapParams__)(struct DispSwObject *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__dispswGetMemoryMappingDescriptor__)(struct DispSwObject *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__dispswIsSwMethodStalling__)(struct DispSwObject *, NvU32);
    NV_STATUS (*__dispswControlFilter__)(struct DispSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__dispswUnregisterEvent__)(struct DispSwObject *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__dispswControlSerialization_Prologue__)(struct DispSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__dispswCanCopy__)(struct DispSwObject *);
    void (*__dispswPreDestruct__)(struct DispSwObject *);
    NV_STATUS (*__dispswIsDuplicate__)(struct DispSwObject *, NvHandle, NvBool *);
    void (*__dispswControlSerialization_Epilogue__)(struct DispSwObject *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__dispswGetNotificationListPtr__)(struct DispSwObject *);
    struct NotifShare *(*__dispswGetNotificationShare__)(struct DispSwObject *);
    NV_STATUS (*__dispswMap__)(struct DispSwObject *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__dispswGetOrAllocNotifShare__)(struct DispSwObject *, NvHandle, NvHandle, struct NotifShare **);
    NvU32 Flags;
    DISPCOMMONOBJECT DispCommon;
    struct DISP_SW_OBJECT_NOTIFY NotifyOnVBlank;
    NvU32 PresentInterval;
};

#ifndef __NVOC_CLASS_DispSwObject_TYPEDEF__
#define __NVOC_CLASS_DispSwObject_TYPEDEF__
typedef struct DispSwObject DispSwObject;
#endif /* __NVOC_CLASS_DispSwObject_TYPEDEF__ */

#ifndef __nvoc_class_id_DispSwObject
#define __nvoc_class_id_DispSwObject 0x99ad6d
#endif /* __nvoc_class_id_DispSwObject */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispSwObject;

#define __staticCast_DispSwObject(pThis) \
    ((pThis)->__nvoc_pbase_DispSwObject)

#ifdef __nvoc_dispsw_h_disabled
#define __dynamicCast_DispSwObject(pThis) ((DispSwObject*)NULL)
#else //__nvoc_dispsw_h_disabled
#define __dynamicCast_DispSwObject(pThis) \
    ((DispSwObject*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DispSwObject)))
#endif //__nvoc_dispsw_h_disabled


NV_STATUS __nvoc_objCreateDynamic_DispSwObject(DispSwObject**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DispSwObject(DispSwObject**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_DispSwObject(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DispSwObject((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define dispswGetSwMethods(pDispSw, ppMethods, pNumMethods) dispswGetSwMethods_DISPATCH(pDispSw, ppMethods, pNumMethods)
#define dispswGetSwMethods_HAL(pDispSw, ppMethods, pNumMethods) dispswGetSwMethods_DISPATCH(pDispSw, ppMethods, pNumMethods)
#define dispswCtrlCmdNotifyOnVblank(pDispSwObject, pNotifyParams) dispswCtrlCmdNotifyOnVblank_DISPATCH(pDispSwObject, pNotifyParams)
#define dispswCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) dispswCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define dispswShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispswShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispswAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispswAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispswMapTo(pResource, pParams) dispswMapTo_DISPATCH(pResource, pParams)
#define dispswGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispswGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispswSetNotificationShare(pNotifier, pNotifShare) dispswSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispswGetRefCount(pResource) dispswGetRefCount_DISPATCH(pResource)
#define dispswAddAdditionalDependants(pClient, pResource, pReference) dispswAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispswControl_Prologue(pResource, pCallContext, pParams) dispswControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispswGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) dispswGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define dispswInternalControlForward(pGpuResource, command, pParams, size) dispswInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispswUnmapFrom(pResource, pParams) dispswUnmapFrom_DISPATCH(pResource, pParams)
#define dispswControl_Epilogue(pResource, pCallContext, pParams) dispswControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispswControlLookup(pResource, pParams, ppEntry) dispswControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define dispswGetInternalObjectHandle(pGpuResource) dispswGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispswControl(pGpuResource, pCallContext, pParams) dispswControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispswUnmap(pGpuResource, pCallContext, pCpuMapping) dispswUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispswGetMemInterMapParams(pRmResource, pParams) dispswGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispswGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispswGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispswIsSwMethodStalling(pChannelDescendant, hHandle) dispswIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define dispswControlFilter(pResource, pCallContext, pParams) dispswControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispswUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispswUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispswControlSerialization_Prologue(pResource, pCallContext, pParams) dispswControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispswCanCopy(pResource) dispswCanCopy_DISPATCH(pResource)
#define dispswPreDestruct(pResource) dispswPreDestruct_DISPATCH(pResource)
#define dispswIsDuplicate(pResource, hMemory, pDuplicate) dispswIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispswControlSerialization_Epilogue(pResource, pCallContext, pParams) dispswControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispswGetNotificationListPtr(pNotifier) dispswGetNotificationListPtr_DISPATCH(pNotifier)
#define dispswGetNotificationShare(pNotifier) dispswGetNotificationShare_DISPATCH(pNotifier)
#define dispswMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispswMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispswGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispswGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
static inline NV_STATUS dispswGetSwMethods_46f6a7(struct DispSwObject *pDispSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS dispswGetSwMethods_DISPATCH(struct DispSwObject *pDispSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pDispSw->__dispswGetSwMethods__(pDispSw, ppMethods, pNumMethods);
}

NV_STATUS dispswCtrlCmdNotifyOnVblank_IMPL(struct DispSwObject *pDispSwObject, NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS *pNotifyParams);

static inline NV_STATUS dispswCtrlCmdNotifyOnVblank_DISPATCH(struct DispSwObject *pDispSwObject, NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS *pNotifyParams) {
    return pDispSwObject->__dispswCtrlCmdNotifyOnVblank__(pDispSwObject, pNotifyParams);
}

static inline NV_STATUS dispswCheckMemInterUnmap_DISPATCH(struct DispSwObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__dispswCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool dispswShareCallback_DISPATCH(struct DispSwObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispswShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool dispswAccessCallback_DISPATCH(struct DispSwObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispswAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispswMapTo_DISPATCH(struct DispSwObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispswMapTo__(pResource, pParams);
}

static inline NV_STATUS dispswGetMapAddrSpace_DISPATCH(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispswGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void dispswSetNotificationShare_DISPATCH(struct DispSwObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispswSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 dispswGetRefCount_DISPATCH(struct DispSwObject *pResource) {
    return pResource->__dispswGetRefCount__(pResource);
}

static inline void dispswAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispSwObject *pResource, RsResourceRef *pReference) {
    pResource->__dispswAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS dispswControl_Prologue_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispswGetRegBaseOffsetAndSize_DISPATCH(struct DispSwObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__dispswGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS dispswInternalControlForward_DISPATCH(struct DispSwObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispswInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS dispswUnmapFrom_DISPATCH(struct DispSwObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispswUnmapFrom__(pResource, pParams);
}

static inline void dispswControl_Epilogue_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispswControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispswControlLookup_DISPATCH(struct DispSwObject *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__dispswControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle dispswGetInternalObjectHandle_DISPATCH(struct DispSwObject *pGpuResource) {
    return pGpuResource->__dispswGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS dispswControl_DISPATCH(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispswControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispswUnmap_DISPATCH(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispswUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS dispswGetMemInterMapParams_DISPATCH(struct DispSwObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispswGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispswGetMemoryMappingDescriptor_DISPATCH(struct DispSwObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispswGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvBool dispswIsSwMethodStalling_DISPATCH(struct DispSwObject *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__dispswIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS dispswControlFilter_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispswUnregisterEvent_DISPATCH(struct DispSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispswUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispswControlSerialization_Prologue_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool dispswCanCopy_DISPATCH(struct DispSwObject *pResource) {
    return pResource->__dispswCanCopy__(pResource);
}

static inline void dispswPreDestruct_DISPATCH(struct DispSwObject *pResource) {
    pResource->__dispswPreDestruct__(pResource);
}

static inline NV_STATUS dispswIsDuplicate_DISPATCH(struct DispSwObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispswIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispswControlSerialization_Epilogue_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispswControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *dispswGetNotificationListPtr_DISPATCH(struct DispSwObject *pNotifier) {
    return pNotifier->__dispswGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *dispswGetNotificationShare_DISPATCH(struct DispSwObject *pNotifier) {
    return pNotifier->__dispswGetNotificationShare__(pNotifier);
}

static inline NV_STATUS dispswMap_DISPATCH(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispswMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispswGetOrAllocNotifShare_DISPATCH(struct DispSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispswGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS dispswConstruct_IMPL(struct DispSwObject *arg_pDispSw, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dispswConstruct(arg_pDispSw, arg_pCallContext, arg_pParams) dispswConstruct_IMPL(arg_pDispSw, arg_pCallContext, arg_pParams)
void dispswDestruct_IMPL(struct DispSwObject *pDispSw);

#define __nvoc_dispswDestruct(pDispSw) dispswDestruct_IMPL(pDispSw)
#undef PRIVATE_FIELD


#endif // DISPSW_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DISPSW_NVOC_H_
