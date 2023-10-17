#ifndef _G_SW_TEST_NVOC_H_
#define _G_SW_TEST_NVOC_H_
#include "nvoc/runtime.h"

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

#include "g_sw_test_nvoc.h"

#ifndef _SW_TEST_H_
#define _SW_TEST_H_

#include "core/core.h"
#include "kernel/gpu/fifo/channel_descendant.h"

/*!
 * RM internal class representing NV04_SOFTWARE_TEST
 */
#ifdef NVOC_SW_TEST_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct SoftwareMethodTest {
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
    struct SoftwareMethodTest *__nvoc_pbase_SoftwareMethodTest;
    NV_STATUS (*__swtestGetSwMethods__)(struct SoftwareMethodTest *, const METHOD **, NvU32 *);
    NV_STATUS (*__swtestCheckMemInterUnmap__)(struct SoftwareMethodTest *, NvBool);
    NvBool (*__swtestShareCallback__)(struct SoftwareMethodTest *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NvBool (*__swtestAccessCallback__)(struct SoftwareMethodTest *, struct RsClient *, void *, RsAccessRight);
    NV_STATUS (*__swtestMapTo__)(struct SoftwareMethodTest *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__swtestGetMapAddrSpace__)(struct SoftwareMethodTest *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__swtestSetNotificationShare__)(struct SoftwareMethodTest *, struct NotifShare *);
    NvU32 (*__swtestGetRefCount__)(struct SoftwareMethodTest *);
    void (*__swtestAddAdditionalDependants__)(struct RsClient *, struct SoftwareMethodTest *, RsResourceRef *);
    NV_STATUS (*__swtestControl_Prologue__)(struct SoftwareMethodTest *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__swtestGetRegBaseOffsetAndSize__)(struct SoftwareMethodTest *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__swtestInternalControlForward__)(struct SoftwareMethodTest *, NvU32, void *, NvU32);
    NV_STATUS (*__swtestUnmapFrom__)(struct SoftwareMethodTest *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__swtestControl_Epilogue__)(struct SoftwareMethodTest *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__swtestControlLookup__)(struct SoftwareMethodTest *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__swtestGetInternalObjectHandle__)(struct SoftwareMethodTest *);
    NV_STATUS (*__swtestControl__)(struct SoftwareMethodTest *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__swtestUnmap__)(struct SoftwareMethodTest *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__swtestGetMemInterMapParams__)(struct SoftwareMethodTest *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__swtestGetMemoryMappingDescriptor__)(struct SoftwareMethodTest *, struct MEMORY_DESCRIPTOR **);
    NvBool (*__swtestIsSwMethodStalling__)(struct SoftwareMethodTest *, NvU32);
    NV_STATUS (*__swtestControlFilter__)(struct SoftwareMethodTest *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__swtestUnregisterEvent__)(struct SoftwareMethodTest *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__swtestControlSerialization_Prologue__)(struct SoftwareMethodTest *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__swtestCanCopy__)(struct SoftwareMethodTest *);
    void (*__swtestPreDestruct__)(struct SoftwareMethodTest *);
    NV_STATUS (*__swtestIsDuplicate__)(struct SoftwareMethodTest *, NvHandle, NvBool *);
    void (*__swtestControlSerialization_Epilogue__)(struct SoftwareMethodTest *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__swtestGetNotificationListPtr__)(struct SoftwareMethodTest *);
    struct NotifShare *(*__swtestGetNotificationShare__)(struct SoftwareMethodTest *);
    NV_STATUS (*__swtestMap__)(struct SoftwareMethodTest *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__swtestGetOrAllocNotifShare__)(struct SoftwareMethodTest *, NvHandle, NvHandle, struct NotifShare **);
};

#ifndef __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__
#define __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__
typedef struct SoftwareMethodTest SoftwareMethodTest;
#endif /* __NVOC_CLASS_SoftwareMethodTest_TYPEDEF__ */

#ifndef __nvoc_class_id_SoftwareMethodTest
#define __nvoc_class_id_SoftwareMethodTest 0xdea092
#endif /* __nvoc_class_id_SoftwareMethodTest */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SoftwareMethodTest;

#define __staticCast_SoftwareMethodTest(pThis) \
    ((pThis)->__nvoc_pbase_SoftwareMethodTest)

#ifdef __nvoc_sw_test_h_disabled
#define __dynamicCast_SoftwareMethodTest(pThis) ((SoftwareMethodTest*)NULL)
#else //__nvoc_sw_test_h_disabled
#define __dynamicCast_SoftwareMethodTest(pThis) \
    ((SoftwareMethodTest*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SoftwareMethodTest)))
#endif //__nvoc_sw_test_h_disabled


NV_STATUS __nvoc_objCreateDynamic_SoftwareMethodTest(SoftwareMethodTest**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SoftwareMethodTest(SoftwareMethodTest**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_SoftwareMethodTest(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_SoftwareMethodTest((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define swtestGetSwMethods(pSwTest, ppMethods, pNumMethods) swtestGetSwMethods_DISPATCH(pSwTest, ppMethods, pNumMethods)
#define swtestCheckMemInterUnmap(pChannelDescendant, bSubdeviceHandleProvided) swtestCheckMemInterUnmap_DISPATCH(pChannelDescendant, bSubdeviceHandleProvided)
#define swtestShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) swtestShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define swtestAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) swtestAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define swtestMapTo(pResource, pParams) swtestMapTo_DISPATCH(pResource, pParams)
#define swtestGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) swtestGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define swtestSetNotificationShare(pNotifier, pNotifShare) swtestSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define swtestGetRefCount(pResource) swtestGetRefCount_DISPATCH(pResource)
#define swtestAddAdditionalDependants(pClient, pResource, pReference) swtestAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define swtestControl_Prologue(pResource, pCallContext, pParams) swtestControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define swtestGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) swtestGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define swtestInternalControlForward(pGpuResource, command, pParams, size) swtestInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define swtestUnmapFrom(pResource, pParams) swtestUnmapFrom_DISPATCH(pResource, pParams)
#define swtestControl_Epilogue(pResource, pCallContext, pParams) swtestControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define swtestControlLookup(pResource, pParams, ppEntry) swtestControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define swtestGetInternalObjectHandle(pGpuResource) swtestGetInternalObjectHandle_DISPATCH(pGpuResource)
#define swtestControl(pGpuResource, pCallContext, pParams) swtestControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define swtestUnmap(pGpuResource, pCallContext, pCpuMapping) swtestUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define swtestGetMemInterMapParams(pRmResource, pParams) swtestGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define swtestGetMemoryMappingDescriptor(pRmResource, ppMemDesc) swtestGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define swtestIsSwMethodStalling(pChannelDescendant, hHandle) swtestIsSwMethodStalling_DISPATCH(pChannelDescendant, hHandle)
#define swtestControlFilter(pResource, pCallContext, pParams) swtestControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define swtestUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) swtestUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define swtestControlSerialization_Prologue(pResource, pCallContext, pParams) swtestControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define swtestCanCopy(pResource) swtestCanCopy_DISPATCH(pResource)
#define swtestPreDestruct(pResource) swtestPreDestruct_DISPATCH(pResource)
#define swtestIsDuplicate(pResource, hMemory, pDuplicate) swtestIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define swtestControlSerialization_Epilogue(pResource, pCallContext, pParams) swtestControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define swtestGetNotificationListPtr(pNotifier) swtestGetNotificationListPtr_DISPATCH(pNotifier)
#define swtestGetNotificationShare(pNotifier) swtestGetNotificationShare_DISPATCH(pNotifier)
#define swtestMap(pGpuResource, pCallContext, pParams, pCpuMapping) swtestMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define swtestGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) swtestGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
NV_STATUS swtestGetSwMethods_IMPL(struct SoftwareMethodTest *pSwTest, const METHOD **ppMethods, NvU32 *pNumMethods);

static inline NV_STATUS swtestGetSwMethods_DISPATCH(struct SoftwareMethodTest *pSwTest, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return pSwTest->__swtestGetSwMethods__(pSwTest, ppMethods, pNumMethods);
}

static inline NV_STATUS swtestCheckMemInterUnmap_DISPATCH(struct SoftwareMethodTest *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return pChannelDescendant->__swtestCheckMemInterUnmap__(pChannelDescendant, bSubdeviceHandleProvided);
}

static inline NvBool swtestShareCallback_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__swtestShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NvBool swtestAccessCallback_DISPATCH(struct SoftwareMethodTest *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__swtestAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS swtestMapTo_DISPATCH(struct SoftwareMethodTest *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__swtestMapTo__(pResource, pParams);
}

static inline NV_STATUS swtestGetMapAddrSpace_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__swtestGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void swtestSetNotificationShare_DISPATCH(struct SoftwareMethodTest *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__swtestSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 swtestGetRefCount_DISPATCH(struct SoftwareMethodTest *pResource) {
    return pResource->__swtestGetRefCount__(pResource);
}

static inline void swtestAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct SoftwareMethodTest *pResource, RsResourceRef *pReference) {
    pResource->__swtestAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS swtestControl_Prologue_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__swtestControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS swtestGetRegBaseOffsetAndSize_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__swtestGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS swtestInternalControlForward_DISPATCH(struct SoftwareMethodTest *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__swtestInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS swtestUnmapFrom_DISPATCH(struct SoftwareMethodTest *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__swtestUnmapFrom__(pResource, pParams);
}

static inline void swtestControl_Epilogue_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__swtestControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS swtestControlLookup_DISPATCH(struct SoftwareMethodTest *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__swtestControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle swtestGetInternalObjectHandle_DISPATCH(struct SoftwareMethodTest *pGpuResource) {
    return pGpuResource->__swtestGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS swtestControl_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__swtestControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS swtestUnmap_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__swtestUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS swtestGetMemInterMapParams_DISPATCH(struct SoftwareMethodTest *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__swtestGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS swtestGetMemoryMappingDescriptor_DISPATCH(struct SoftwareMethodTest *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__swtestGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvBool swtestIsSwMethodStalling_DISPATCH(struct SoftwareMethodTest *pChannelDescendant, NvU32 hHandle) {
    return pChannelDescendant->__swtestIsSwMethodStalling__(pChannelDescendant, hHandle);
}

static inline NV_STATUS swtestControlFilter_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__swtestControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS swtestUnregisterEvent_DISPATCH(struct SoftwareMethodTest *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__swtestUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS swtestControlSerialization_Prologue_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__swtestControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool swtestCanCopy_DISPATCH(struct SoftwareMethodTest *pResource) {
    return pResource->__swtestCanCopy__(pResource);
}

static inline void swtestPreDestruct_DISPATCH(struct SoftwareMethodTest *pResource) {
    pResource->__swtestPreDestruct__(pResource);
}

static inline NV_STATUS swtestIsDuplicate_DISPATCH(struct SoftwareMethodTest *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__swtestIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void swtestControlSerialization_Epilogue_DISPATCH(struct SoftwareMethodTest *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__swtestControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *swtestGetNotificationListPtr_DISPATCH(struct SoftwareMethodTest *pNotifier) {
    return pNotifier->__swtestGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *swtestGetNotificationShare_DISPATCH(struct SoftwareMethodTest *pNotifier) {
    return pNotifier->__swtestGetNotificationShare__(pNotifier);
}

static inline NV_STATUS swtestMap_DISPATCH(struct SoftwareMethodTest *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__swtestMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS swtestGetOrAllocNotifShare_DISPATCH(struct SoftwareMethodTest *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__swtestGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

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
