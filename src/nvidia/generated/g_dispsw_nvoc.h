
#ifndef _G_DISPSW_NVOC_H_
#define _G_DISPSW_NVOC_H_
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

#pragma once
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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DISPSW_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct DispSwObject {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct ChannelDescendant __nvoc_base_ChannelDescendant;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^3
    struct Notifier *__nvoc_pbase_Notifier;    // notify super^2
    struct ChannelDescendant *__nvoc_pbase_ChannelDescendant;    // chandes super
    struct DispSwObject *__nvoc_pbase_DispSwObject;    // dispsw

    // Vtable with 33 per-object function pointers
    NV_STATUS (*__dispswGetSwMethods__)(struct DispSwObject * /*this*/, const METHOD **, NvU32 *);  // virtual halified (singleton optimized) override (chandes) base (chandes) body
    NV_STATUS (*__dispswCtrlCmdNotifyOnVblank__)(struct DispSwObject * /*this*/, NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS *);  // exported (id=0x90720101)
    NvBool (*__dispswIsSwMethodStalling__)(struct DispSwObject * /*this*/, NvU32);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__dispswCheckMemInterUnmap__)(struct DispSwObject * /*this*/, NvBool);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__dispswControl__)(struct DispSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__dispswMap__)(struct DispSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__dispswUnmap__)(struct DispSwObject * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NvBool (*__dispswShareCallback__)(struct DispSwObject * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__dispswGetRegBaseOffsetAndSize__)(struct DispSwObject * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__dispswGetMapAddrSpace__)(struct DispSwObject * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__dispswInternalControlForward__)(struct DispSwObject * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (chandes)
    NvHandle (*__dispswGetInternalObjectHandle__)(struct DispSwObject * /*this*/);  // virtual inherited (gpures) base (chandes)
    NvBool (*__dispswAccessCallback__)(struct DispSwObject * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__dispswGetMemInterMapParams__)(struct DispSwObject * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__dispswGetMemoryMappingDescriptor__)(struct DispSwObject * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__dispswControlSerialization_Prologue__)(struct DispSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__dispswControlSerialization_Epilogue__)(struct DispSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__dispswControl_Prologue__)(struct DispSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__dispswControl_Epilogue__)(struct DispSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NvBool (*__dispswCanCopy__)(struct DispSwObject * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__dispswIsDuplicate__)(struct DispSwObject * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (chandes)
    void (*__dispswPreDestruct__)(struct DispSwObject * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__dispswControlFilter__)(struct DispSwObject * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (chandes)
    NvBool (*__dispswIsPartialUnmapSupported__)(struct DispSwObject * /*this*/);  // inline virtual inherited (res) base (chandes) body
    NV_STATUS (*__dispswMapTo__)(struct DispSwObject * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__dispswUnmapFrom__)(struct DispSwObject * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (chandes)
    NvU32 (*__dispswGetRefCount__)(struct DispSwObject * /*this*/);  // virtual inherited (res) base (chandes)
    void (*__dispswAddAdditionalDependants__)(struct RsClient *, struct DispSwObject * /*this*/, RsResourceRef *);  // virtual inherited (res) base (chandes)
    PEVENTNOTIFICATION * (*__dispswGetNotificationListPtr__)(struct DispSwObject * /*this*/);  // virtual inherited (notify) base (chandes)
    struct NotifShare * (*__dispswGetNotificationShare__)(struct DispSwObject * /*this*/);  // virtual inherited (notify) base (chandes)
    void (*__dispswSetNotificationShare__)(struct DispSwObject * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__dispswUnregisterEvent__)(struct DispSwObject * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__dispswGetOrAllocNotifShare__)(struct DispSwObject * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (chandes)

    // Data members
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

// Casting support
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


// Wrapper macros
#define dispswGetSwMethods_FNPTR(pDispSw) pDispSw->__dispswGetSwMethods__
#define dispswGetSwMethods(pDispSw, ppMethods, pNumMethods) dispswGetSwMethods_DISPATCH(pDispSw, ppMethods, pNumMethods)
#define dispswGetSwMethods_HAL(pDispSw, ppMethods, pNumMethods) dispswGetSwMethods_DISPATCH(pDispSw, ppMethods, pNumMethods)
#define dispswCtrlCmdNotifyOnVblank_FNPTR(pDispSwObject) pDispSwObject->__dispswCtrlCmdNotifyOnVblank__
#define dispswCtrlCmdNotifyOnVblank(pDispSwObject, pNotifyParams) dispswCtrlCmdNotifyOnVblank_DISPATCH(pDispSwObject, pNotifyParams)
#define dispswIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesIsSwMethodStalling__
#define dispswIsSwMethodStalling(pChannelDescendant, hHandle) dispswIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define dispswCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__chandesCheckMemInterUnmap__
#define dispswCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) dispswCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define dispswControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresControl__
#define dispswControl(pGpuResource, pCallContext, pParams) dispswControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dispswMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresMap__
#define dispswMap(pGpuResource, pCallContext, pParams, pCpuMapping) dispswMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define dispswUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresUnmap__
#define dispswUnmap(pGpuResource, pCallContext, pCpuMapping) dispswUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define dispswShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresShareCallback__
#define dispswShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dispswShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dispswGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetRegBaseOffsetAndSize__
#define dispswGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) dispswGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define dispswGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetMapAddrSpace__
#define dispswGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) dispswGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define dispswInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresInternalControlForward__
#define dispswInternalControlForward(pGpuResource, command, pParams, size) dispswInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dispswGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__
#define dispswGetInternalObjectHandle(pGpuResource) dispswGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dispswAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresAccessCallback__
#define dispswAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dispswAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dispswGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemInterMapParams__
#define dispswGetMemInterMapParams(pRmResource, pParams) dispswGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dispswGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresGetMemoryMappingDescriptor__
#define dispswGetMemoryMappingDescriptor(pRmResource, ppMemDesc) dispswGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define dispswControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Prologue__
#define dispswControlSerialization_Prologue(pResource, pCallContext, pParams) dispswControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispswControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControlSerialization_Epilogue__
#define dispswControlSerialization_Epilogue(pResource, pCallContext, pParams) dispswControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispswControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Prologue__
#define dispswControl_Prologue(pResource, pCallContext, pParams) dispswControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dispswControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__rmresControl_Epilogue__
#define dispswControl_Epilogue(pResource, pCallContext, pParams) dispswControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dispswCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resCanCopy__
#define dispswCanCopy(pResource) dispswCanCopy_DISPATCH(pResource)
#define dispswIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsDuplicate__
#define dispswIsDuplicate(pResource, hMemory, pDuplicate) dispswIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dispswPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resPreDestruct__
#define dispswPreDestruct(pResource) dispswPreDestruct_DISPATCH(pResource)
#define dispswControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resControlFilter__
#define dispswControlFilter(pResource, pCallContext, pParams) dispswControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dispswIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define dispswIsPartialUnmapSupported(pResource) dispswIsPartialUnmapSupported_DISPATCH(pResource)
#define dispswMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resMapTo__
#define dispswMapTo(pResource, pParams) dispswMapTo_DISPATCH(pResource, pParams)
#define dispswUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resUnmapFrom__
#define dispswUnmapFrom(pResource, pParams) dispswUnmapFrom_DISPATCH(pResource, pParams)
#define dispswGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resGetRefCount__
#define dispswGetRefCount(pResource) dispswGetRefCount_DISPATCH(pResource)
#define dispswAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__resAddAdditionalDependants__
#define dispswAddAdditionalDependants(pClient, pResource, pReference) dispswAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define dispswGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationListPtr__
#define dispswGetNotificationListPtr(pNotifier) dispswGetNotificationListPtr_DISPATCH(pNotifier)
#define dispswGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetNotificationShare__
#define dispswGetNotificationShare(pNotifier) dispswGetNotificationShare_DISPATCH(pNotifier)
#define dispswSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifySetNotificationShare__
#define dispswSetNotificationShare(pNotifier, pNotifShare) dispswSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define dispswUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyUnregisterEvent__
#define dispswUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) dispswUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define dispswGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__notifyGetOrAllocNotifShare__
#define dispswGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) dispswGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS dispswGetSwMethods_DISPATCH(struct DispSwObject *pDispSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pDispSw->__dispswGetSwMethods__(pDispSw, ppMethods, pNumMethods);
}

static inline NV_STATUS dispswCtrlCmdNotifyOnVblank_DISPATCH(struct DispSwObject *pDispSwObject, NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS *pNotifyParams) {
    return pDispSwObject->__dispswCtrlCmdNotifyOnVblank__(pDispSwObject, pNotifyParams);
}

static inline NvBool dispswIsSwMethodStalling_DISPATCH(struct DispSwObject *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__dispswIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS dispswCheckMemInterUnmap_DISPATCH(struct DispSwObject *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__dispswCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NV_STATUS dispswControl_DISPATCH(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__dispswControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS dispswMap_DISPATCH(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispswMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dispswUnmap_DISPATCH(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__dispswUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool dispswShareCallback_DISPATCH(struct DispSwObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__dispswShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dispswGetRegBaseOffsetAndSize_DISPATCH(struct DispSwObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__dispswGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS dispswGetMapAddrSpace_DISPATCH(struct DispSwObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__dispswGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dispswInternalControlForward_DISPATCH(struct DispSwObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__dispswInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle dispswGetInternalObjectHandle_DISPATCH(struct DispSwObject *pGpuResource) {
    return pGpuResource->__dispswGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool dispswAccessCallback_DISPATCH(struct DispSwObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__dispswAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dispswGetMemInterMapParams_DISPATCH(struct DispSwObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__dispswGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dispswGetMemoryMappingDescriptor_DISPATCH(struct DispSwObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__dispswGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS dispswControlSerialization_Prologue_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispswControlSerialization_Epilogue_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispswControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dispswControl_Prologue_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void dispswControl_Epilogue_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__dispswControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dispswCanCopy_DISPATCH(struct DispSwObject *pResource) {
    return pResource->__dispswCanCopy__(pResource);
}

static inline NV_STATUS dispswIsDuplicate_DISPATCH(struct DispSwObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__dispswIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dispswPreDestruct_DISPATCH(struct DispSwObject *pResource) {
    pResource->__dispswPreDestruct__(pResource);
}

static inline NV_STATUS dispswControlFilter_DISPATCH(struct DispSwObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__dispswControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool dispswIsPartialUnmapSupported_DISPATCH(struct DispSwObject *pResource) {
    return pResource->__dispswIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dispswMapTo_DISPATCH(struct DispSwObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__dispswMapTo__(pResource, pParams);
}

static inline NV_STATUS dispswUnmapFrom_DISPATCH(struct DispSwObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__dispswUnmapFrom__(pResource, pParams);
}

static inline NvU32 dispswGetRefCount_DISPATCH(struct DispSwObject *pResource) {
    return pResource->__dispswGetRefCount__(pResource);
}

static inline void dispswAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DispSwObject *pResource, RsResourceRef *pReference) {
    pResource->__dispswAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * dispswGetNotificationListPtr_DISPATCH(struct DispSwObject *pNotifier) {
    return pNotifier->__dispswGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * dispswGetNotificationShare_DISPATCH(struct DispSwObject *pNotifier) {
    return pNotifier->__dispswGetNotificationShare__(pNotifier);
}

static inline void dispswSetNotificationShare_DISPATCH(struct DispSwObject *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__dispswSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS dispswUnregisterEvent_DISPATCH(struct DispSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__dispswUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS dispswGetOrAllocNotifShare_DISPATCH(struct DispSwObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__dispswGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS dispswGetSwMethods_46f6a7(struct DispSwObject *pDispSw, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS dispswCtrlCmdNotifyOnVblank_IMPL(struct DispSwObject *pDispSwObject, NV9072_CTRL_CMD_NOTIFY_ON_VBLANK_PARAMS *pNotifyParams);

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
