
#ifndef _G_SW_TEST_NVOC_H_
#define _G_SW_TEST_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_sw_test_nvoc.h"

#ifndef _SW_TEST_H_
#define _SW_TEST_H_

#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"

/*!
 * RM internal class representing NV04_SOFTWARE_TEST
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SW_TEST_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__SoftwareMethodTest;
struct NVOC_METADATA__ChannelDescendant;
struct NVOC_VTABLE__SoftwareMethodTest;


struct SoftwareMethodTest {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SoftwareMethodTest *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
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
    struct SoftwareMethodTest *__nvoc_pbase_SoftwareMethodTest;    // swtest
};


// Vtable with 32 per-class function pointers
struct NVOC_VTABLE__SoftwareMethodTest {
    NV_STATUS (*__swtestGetSwMethods__)(struct SoftwareMethodTest * /*this*/, const METHOD **, NvU32 *);  // virtual override (chandes) base (chandes)
    NvBool (*__swtestIsSwMethodStalling__)(struct SoftwareMethodTest * /*this*/, NvU32);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__swtestCheckMemInterUnmap__)(struct SoftwareMethodTest * /*this*/, NvBool);  // virtual inherited (chandes) base (chandes)
    NV_STATUS (*__swtestControl__)(struct SoftwareMethodTest * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__swtestMap__)(struct SoftwareMethodTest * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__swtestUnmap__)(struct SoftwareMethodTest * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (chandes)
    NvBool (*__swtestShareCallback__)(struct SoftwareMethodTest * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__swtestGetRegBaseOffsetAndSize__)(struct SoftwareMethodTest * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__swtestGetMapAddrSpace__)(struct SoftwareMethodTest * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (chandes)
    NV_STATUS (*__swtestInternalControlForward__)(struct SoftwareMethodTest * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (chandes)
    NvHandle (*__swtestGetInternalObjectHandle__)(struct SoftwareMethodTest * /*this*/);  // virtual inherited (gpures) base (chandes)
    NvBool (*__swtestAccessCallback__)(struct SoftwareMethodTest * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__swtestGetMemInterMapParams__)(struct SoftwareMethodTest * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__swtestGetMemoryMappingDescriptor__)(struct SoftwareMethodTest * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__swtestControlSerialization_Prologue__)(struct SoftwareMethodTest * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__swtestControlSerialization_Epilogue__)(struct SoftwareMethodTest * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NV_STATUS (*__swtestControl_Prologue__)(struct SoftwareMethodTest * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    void (*__swtestControl_Epilogue__)(struct SoftwareMethodTest * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (chandes)
    NvBool (*__swtestCanCopy__)(struct SoftwareMethodTest * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__swtestIsDuplicate__)(struct SoftwareMethodTest * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (chandes)
    void (*__swtestPreDestruct__)(struct SoftwareMethodTest * /*this*/);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__swtestControlFilter__)(struct SoftwareMethodTest * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (chandes)
    NvBool (*__swtestIsPartialUnmapSupported__)(struct SoftwareMethodTest * /*this*/);  // inline virtual inherited (res) base (chandes) body
    NV_STATUS (*__swtestMapTo__)(struct SoftwareMethodTest * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (chandes)
    NV_STATUS (*__swtestUnmapFrom__)(struct SoftwareMethodTest * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (chandes)
    NvU32 (*__swtestGetRefCount__)(struct SoftwareMethodTest * /*this*/);  // virtual inherited (res) base (chandes)
    void (*__swtestAddAdditionalDependants__)(struct RsClient *, struct SoftwareMethodTest * /*this*/, RsResourceRef *);  // virtual inherited (res) base (chandes)
    PEVENTNOTIFICATION * (*__swtestGetNotificationListPtr__)(struct SoftwareMethodTest * /*this*/);  // virtual inherited (notify) base (chandes)
    struct NotifShare * (*__swtestGetNotificationShare__)(struct SoftwareMethodTest * /*this*/);  // virtual inherited (notify) base (chandes)
    void (*__swtestSetNotificationShare__)(struct SoftwareMethodTest * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__swtestUnregisterEvent__)(struct SoftwareMethodTest * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (chandes)
    NV_STATUS (*__swtestGetOrAllocNotifShare__)(struct SoftwareMethodTest * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (chandes)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__SoftwareMethodTest {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__ChannelDescendant metadata__ChannelDescendant;
    const struct NVOC_VTABLE__SoftwareMethodTest vtable;
};

#ifndef __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__
#define __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__
typedef struct SoftwareMethodTest SoftwareMethodTest;
#endif /* __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__ */

#ifndef __nvoc_class_id_SoftwareMethodTest
#define __nvoc_class_id_SoftwareMethodTest 0xdea092
#endif /* __nvoc_class_id_SoftwareMethodTest */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SoftwareMethodTest;

#define __staticCast_SoftwareMethodTest(pThis) \
    ((pThis)->__nvoc_pbase_SoftwareMethodTest)

#ifdef __nvoc_sw_test_h_disabled
#define __dynamicCast_SoftwareMethodTest(pThis) ((SoftwareMethodTest*) NULL)
#else //__nvoc_sw_test_h_disabled
#define __dynamicCast_SoftwareMethodTest(pThis) \
    ((SoftwareMethodTest*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SoftwareMethodTest)))
#endif //__nvoc_sw_test_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SoftwareMethodTest(SoftwareMethodTest**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SoftwareMethodTest(SoftwareMethodTest**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_SoftwareMethodTest(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_SoftwareMethodTest((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define swtestGetSwMethods_FNPTR(pSwTest) pSwTest->__nvoc_metadata_ptr->vtable.__swtestGetSwMethods__
#define swtestGetSwMethods(pSwTest, ppMethods, pNumMethods) swtestGetSwMethods_DISPATCH(pSwTest, ppMethods, pNumMethods)
#define swtestIsSwMethodStalling_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesIsSwMethodStalling__
#define swtestIsSwMethodStalling(pChannelDescendant, hHandle) swtestIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define swtestCheckMemInterUnmap_FNPTR(pChannelDescendant) pChannelDescendant->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr->vtable.__chandesCheckMemInterUnmap__
#define swtestCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) swtestCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define swtestControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define swtestControl(pGpuResource, pCallContext, pParams) swtestControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define swtestMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define swtestMap(pGpuResource, pCallContext, pParams, pCpuMapping) swtestMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define swtestUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define swtestUnmap(pGpuResource, pCallContext, pCpuMapping) swtestUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define swtestShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define swtestShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) swtestShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define swtestGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define swtestGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) swtestGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define swtestGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define swtestGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) swtestGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define swtestInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define swtestInternalControlForward(pGpuResource, command, pParams, size) swtestInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define swtestGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define swtestGetInternalObjectHandle(pGpuResource) swtestGetInternalObjectHandle_DISPATCH(pGpuResource)
#define swtestAccessCallback_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define swtestAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) swtestAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define swtestGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define swtestGetMemInterMapParams(pRmResource, pParams) swtestGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define swtestGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define swtestGetMemoryMappingDescriptor(pRmResource, ppMemDesc) swtestGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define swtestControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define swtestControlSerialization_Prologue(pResource, pCallContext, pParams) swtestControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define swtestControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define swtestControlSerialization_Epilogue(pResource, pCallContext, pParams) swtestControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define swtestControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define swtestControl_Prologue(pResource, pCallContext, pParams) swtestControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define swtestControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define swtestControl_Epilogue(pResource, pCallContext, pParams) swtestControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define swtestCanCopy_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define swtestCanCopy(pResource) swtestCanCopy_DISPATCH(pResource)
#define swtestIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define swtestIsDuplicate(pResource, hMemory, pDuplicate) swtestIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define swtestPreDestruct_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define swtestPreDestruct(pResource) swtestPreDestruct_DISPATCH(pResource)
#define swtestControlFilter_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define swtestControlFilter(pResource, pCallContext, pParams) swtestControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define swtestIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define swtestIsPartialUnmapSupported(pResource) swtestIsPartialUnmapSupported_DISPATCH(pResource)
#define swtestMapTo_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define swtestMapTo(pResource, pParams) swtestMapTo_DISPATCH(pResource, pParams)
#define swtestUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define swtestUnmapFrom(pResource, pParams) swtestUnmapFrom_DISPATCH(pResource, pParams)
#define swtestGetRefCount_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define swtestGetRefCount(pResource) swtestGetRefCount_DISPATCH(pResource)
#define swtestAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define swtestAddAdditionalDependants(pClient, pResource, pReference) swtestAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define swtestGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__
#define swtestGetNotificationListPtr(pNotifier) swtestGetNotificationListPtr_DISPATCH(pNotifier)
#define swtestGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__
#define swtestGetNotificationShare(pNotifier) swtestGetNotificationShare_DISPATCH(pNotifier)
#define swtestSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__
#define swtestSetNotificationShare(pNotifier, pNotifShare) swtestSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define swtestUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__
#define swtestUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) swtestUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define swtestGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__
#define swtestGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) swtestGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS swtestGetSwMethods_DISPATCH(struct SoftwareMethodTest *pSwTest, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pSwTest->__nvoc_metadata_ptr->vtable.__swtestGetSwMethods__(pSwTest, ppMethods, pNumMethods);
}

static inline NvBool swtestIsSwMethodStalling_DISPATCH(struct SoftwareMethodTest *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__swtestIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS swtestCheckMemInterUnmap_DISPATCH(struct SoftwareMethodTest *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__nvoc_metadata_ptr->vtable.__swtestCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NV_STATUS swtestControl_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__swtestControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS swtestMap_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__swtestMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS swtestUnmap_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__swtestUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool swtestShareCallback_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__swtestShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS swtestGetRegBaseOffsetAndSize_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__swtestGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS swtestGetMapAddrSpace_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__swtestGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS swtestInternalControlForward_DISPATCH(struct SoftwareMethodTest *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__swtestInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle swtestGetInternalObjectHandle_DISPATCH(struct SoftwareMethodTest *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__swtestGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool swtestAccessCallback_DISPATCH(struct SoftwareMethodTest *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS swtestGetMemInterMapParams_DISPATCH(struct SoftwareMethodTest *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__swtestGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS swtestGetMemoryMappingDescriptor_DISPATCH(struct SoftwareMethodTest *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__swtestGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS swtestControlSerialization_Prologue_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void swtestControlSerialization_Epilogue_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__swtestControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS swtestControl_Prologue_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void swtestControl_Epilogue_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__swtestControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool swtestCanCopy_DISPATCH(struct SoftwareMethodTest *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestCanCopy__(pResource);
}

static inline NV_STATUS swtestIsDuplicate_DISPATCH(struct SoftwareMethodTest *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void swtestPreDestruct_DISPATCH(struct SoftwareMethodTest *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__swtestPreDestruct__(pResource);
}

static inline NV_STATUS swtestControlFilter_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool swtestIsPartialUnmapSupported_DISPATCH(struct SoftwareMethodTest *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS swtestMapTo_DISPATCH(struct SoftwareMethodTest *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestMapTo__(pResource, pParams);
}

static inline NV_STATUS swtestUnmapFrom_DISPATCH(struct SoftwareMethodTest *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestUnmapFrom__(pResource, pParams);
}

static inline NvU32 swtestGetRefCount_DISPATCH(struct SoftwareMethodTest *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__swtestGetRefCount__(pResource);
}

static inline void swtestAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SoftwareMethodTest *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__swtestAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * swtestGetNotificationListPtr_DISPATCH(struct SoftwareMethodTest *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__swtestGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * swtestGetNotificationShare_DISPATCH(struct SoftwareMethodTest *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__swtestGetNotificationShare__(pNotifier);
}

static inline void swtestSetNotificationShare_DISPATCH(struct SoftwareMethodTest *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__swtestSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS swtestUnregisterEvent_DISPATCH(struct SoftwareMethodTest *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__swtestUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS swtestGetOrAllocNotifShare_DISPATCH(struct SoftwareMethodTest *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__swtestGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS swtestGetSwMethods_IMPL(struct SoftwareMethodTest *pSwTest, const METHOD **ppMethods, NvU32 *pNumMethods);

NV_STATUS swtestConstruct_IMPL(struct SoftwareMethodTest *arg_pSwTest, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_swtestConstruct(arg_pSwTest, arg_pCallContext, arg_pParams) swtestConstruct_IMPL(arg_pSwTest, arg_pCallContext, arg_pParams)
void swtestDestruct_IMPL(struct SoftwareMethodTest *pSwTest);

#define __nvoc_swtestDestruct(pSwTest) swtestDestruct_IMPL(pSwTest)
#undef PRIVATE_FIELD


#endif // _SW_TEST_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SW_TEST_NVOC_H_
