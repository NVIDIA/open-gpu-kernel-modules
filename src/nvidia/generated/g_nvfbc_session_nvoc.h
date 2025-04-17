
#ifndef _G_NVFBC_SESSION_NVOC_H_
#define _G_NVFBC_SESSION_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_nvfbc_session_nvoc.h"

#ifndef _NVFBCSESSION_H_
#define _NVFBCSESSION_H_

#include "core/core.h"

#include "rmapi/client.h"
#include "gpu/gpu_resource.h"
#include "class/cla0bd.h"
#include "ctrl/ctrla0bd.h"

#include "rmapi/control.h"

#define SESSION_UPDATE_TIMESTAMP_UNINITIALIZED  0

typedef struct NvfbcSession NvfbcSession;
typedef struct NVFBC_SESSION_LIST_ITEM
{
    NvHandle      hClient;
    NvfbcSession *sessionPtr;
} NVFBC_SESSION_LIST_ITEM, *PNVFBC_SESSION_LIST_ITEM;

MAKE_LIST(NVFBC_SESSION_LIST, NVFBC_SESSION_LIST_ITEM);

typedef struct NVFBC_SESSION_ENTRY
{
    NvU32               processId;
    NvU32               vgpuInstanceId;
    NvU32               sessionId;
    NvU32               displayOrdinal;
    NvU32               sessionType;
    NvU32               sessionFlags;
    NvU32               hMaxResolution;
    NvU32               vMaxResolution;
    NvU32               hResolution;
    NvU32               vResolution;
    NvU32               totalGrabCalls;
    NvU32               averageFPS;
    NvU32               averageLatency;             // average over last 60 samples if FPS exceeds 60
    NvU64               lastUpdateTimeStamp;        // timestamp of last session update call
} NVFBC_SESSION_ENTRY, *PNVFBC_SESSION_ENTRY;



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVFBC_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__NvfbcSession;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__NvfbcSession;


struct NvfbcSession {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvfbcSession *__nvoc_metadata_ptr;
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
    struct NvfbcSession *__nvoc_pbase_NvfbcSession;    // nvfbcsession

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo__)(struct NvfbcSession * /*this*/, NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS *);  // exported (id=0xa0bd0101)

    // Data members
    NVFBC_SESSION_ENTRY nvfbcSessionEntry;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__NvfbcSession {
    NV_STATUS (*__nvfbcsessionControl__)(struct NvfbcSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvfbcsessionMap__)(struct NvfbcSession * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvfbcsessionUnmap__)(struct NvfbcSession * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__nvfbcsessionShareCallback__)(struct NvfbcSession * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvfbcsessionGetRegBaseOffsetAndSize__)(struct NvfbcSession * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvfbcsessionGetMapAddrSpace__)(struct NvfbcSession * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__nvfbcsessionInternalControlForward__)(struct NvfbcSession * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__nvfbcsessionGetInternalObjectHandle__)(struct NvfbcSession * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__nvfbcsessionAccessCallback__)(struct NvfbcSession * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvfbcsessionGetMemInterMapParams__)(struct NvfbcSession * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvfbcsessionCheckMemInterUnmap__)(struct NvfbcSession * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvfbcsessionGetMemoryMappingDescriptor__)(struct NvfbcSession * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvfbcsessionControlSerialization_Prologue__)(struct NvfbcSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__nvfbcsessionControlSerialization_Epilogue__)(struct NvfbcSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__nvfbcsessionControl_Prologue__)(struct NvfbcSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__nvfbcsessionControl_Epilogue__)(struct NvfbcSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__nvfbcsessionCanCopy__)(struct NvfbcSession * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__nvfbcsessionIsDuplicate__)(struct NvfbcSession * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__nvfbcsessionPreDestruct__)(struct NvfbcSession * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__nvfbcsessionControlFilter__)(struct NvfbcSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__nvfbcsessionIsPartialUnmapSupported__)(struct NvfbcSession * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__nvfbcsessionMapTo__)(struct NvfbcSession * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__nvfbcsessionUnmapFrom__)(struct NvfbcSession * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__nvfbcsessionGetRefCount__)(struct NvfbcSession * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__nvfbcsessionAddAdditionalDependants__)(struct RsClient *, struct NvfbcSession * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__NvfbcSession {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__NvfbcSession vtable;
};

#ifndef __NVOC_CLASS_NvfbcSession_TYPEDEF__
#define __NVOC_CLASS_NvfbcSession_TYPEDEF__
typedef struct NvfbcSession NvfbcSession;
#endif /* __NVOC_CLASS_NvfbcSession_TYPEDEF__ */

#ifndef __nvoc_class_id_NvfbcSession
#define __nvoc_class_id_NvfbcSession 0xcbde75
#endif /* __nvoc_class_id_NvfbcSession */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvfbcSession;

#define __staticCast_NvfbcSession(pThis) \
    ((pThis)->__nvoc_pbase_NvfbcSession)

#ifdef __nvoc_nvfbc_session_h_disabled
#define __dynamicCast_NvfbcSession(pThis) ((NvfbcSession*) NULL)
#else //__nvoc_nvfbc_session_h_disabled
#define __dynamicCast_NvfbcSession(pThis) \
    ((NvfbcSession*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvfbcSession)))
#endif //__nvoc_nvfbc_session_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvfbcSession(NvfbcSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvfbcSession(NvfbcSession**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_NvfbcSession(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_NvfbcSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo_FNPTR(pNvfbcSession) pNvfbcSession->__nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo__
#define nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo(pNvfbcSession, pParams) nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo_DISPATCH(pNvfbcSession, pParams)
#define nvfbcsessionControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define nvfbcsessionControl(pGpuResource, pCallContext, pParams) nvfbcsessionControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define nvfbcsessionMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define nvfbcsessionMap(pGpuResource, pCallContext, pParams, pCpuMapping) nvfbcsessionMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define nvfbcsessionUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define nvfbcsessionUnmap(pGpuResource, pCallContext, pCpuMapping) nvfbcsessionUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define nvfbcsessionShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define nvfbcsessionShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) nvfbcsessionShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define nvfbcsessionGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define nvfbcsessionGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) nvfbcsessionGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define nvfbcsessionGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define nvfbcsessionGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) nvfbcsessionGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define nvfbcsessionInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define nvfbcsessionInternalControlForward(pGpuResource, command, pParams, size) nvfbcsessionInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define nvfbcsessionGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define nvfbcsessionGetInternalObjectHandle(pGpuResource) nvfbcsessionGetInternalObjectHandle_DISPATCH(pGpuResource)
#define nvfbcsessionAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define nvfbcsessionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) nvfbcsessionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define nvfbcsessionGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define nvfbcsessionGetMemInterMapParams(pRmResource, pParams) nvfbcsessionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define nvfbcsessionCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define nvfbcsessionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) nvfbcsessionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define nvfbcsessionGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define nvfbcsessionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) nvfbcsessionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define nvfbcsessionControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define nvfbcsessionControlSerialization_Prologue(pResource, pCallContext, pParams) nvfbcsessionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvfbcsessionControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define nvfbcsessionControlSerialization_Epilogue(pResource, pCallContext, pParams) nvfbcsessionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvfbcsessionControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define nvfbcsessionControl_Prologue(pResource, pCallContext, pParams) nvfbcsessionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define nvfbcsessionControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define nvfbcsessionControl_Epilogue(pResource, pCallContext, pParams) nvfbcsessionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define nvfbcsessionCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define nvfbcsessionCanCopy(pResource) nvfbcsessionCanCopy_DISPATCH(pResource)
#define nvfbcsessionIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define nvfbcsessionIsDuplicate(pResource, hMemory, pDuplicate) nvfbcsessionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define nvfbcsessionPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define nvfbcsessionPreDestruct(pResource) nvfbcsessionPreDestruct_DISPATCH(pResource)
#define nvfbcsessionControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define nvfbcsessionControlFilter(pResource, pCallContext, pParams) nvfbcsessionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define nvfbcsessionIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define nvfbcsessionIsPartialUnmapSupported(pResource) nvfbcsessionIsPartialUnmapSupported_DISPATCH(pResource)
#define nvfbcsessionMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define nvfbcsessionMapTo(pResource, pParams) nvfbcsessionMapTo_DISPATCH(pResource, pParams)
#define nvfbcsessionUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define nvfbcsessionUnmapFrom(pResource, pParams) nvfbcsessionUnmapFrom_DISPATCH(pResource, pParams)
#define nvfbcsessionGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define nvfbcsessionGetRefCount(pResource) nvfbcsessionGetRefCount_DISPATCH(pResource)
#define nvfbcsessionAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define nvfbcsessionAddAdditionalDependants(pClient, pResource, pReference) nvfbcsessionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo_DISPATCH(struct NvfbcSession *pNvfbcSession, NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS *pParams) {
    return pNvfbcSession->__nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo__(pNvfbcSession, pParams);
}

static inline NV_STATUS nvfbcsessionControl_DISPATCH(struct NvfbcSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS nvfbcsessionMap_DISPATCH(struct NvfbcSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS nvfbcsessionUnmap_DISPATCH(struct NvfbcSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool nvfbcsessionShareCallback_DISPATCH(struct NvfbcSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS nvfbcsessionGetRegBaseOffsetAndSize_DISPATCH(struct NvfbcSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS nvfbcsessionGetMapAddrSpace_DISPATCH(struct NvfbcSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS nvfbcsessionInternalControlForward_DISPATCH(struct NvfbcSession *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle nvfbcsessionGetInternalObjectHandle_DISPATCH(struct NvfbcSession *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool nvfbcsessionAccessCallback_DISPATCH(struct NvfbcSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS nvfbcsessionGetMemInterMapParams_DISPATCH(struct NvfbcSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS nvfbcsessionCheckMemInterUnmap_DISPATCH(struct NvfbcSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS nvfbcsessionGetMemoryMappingDescriptor_DISPATCH(struct NvfbcSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS nvfbcsessionControlSerialization_Prologue_DISPATCH(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void nvfbcsessionControlSerialization_Epilogue_DISPATCH(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS nvfbcsessionControl_Prologue_DISPATCH(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void nvfbcsessionControl_Epilogue_DISPATCH(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool nvfbcsessionCanCopy_DISPATCH(struct NvfbcSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionCanCopy__(pResource);
}

static inline NV_STATUS nvfbcsessionIsDuplicate_DISPATCH(struct NvfbcSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void nvfbcsessionPreDestruct_DISPATCH(struct NvfbcSession *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionPreDestruct__(pResource);
}

static inline NV_STATUS nvfbcsessionControlFilter_DISPATCH(struct NvfbcSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool nvfbcsessionIsPartialUnmapSupported_DISPATCH(struct NvfbcSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS nvfbcsessionMapTo_DISPATCH(struct NvfbcSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionMapTo__(pResource, pParams);
}

static inline NV_STATUS nvfbcsessionUnmapFrom_DISPATCH(struct NvfbcSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionUnmapFrom__(pResource, pParams);
}

static inline NvU32 nvfbcsessionGetRefCount_DISPATCH(struct NvfbcSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionGetRefCount__(pResource);
}

static inline void nvfbcsessionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct NvfbcSession *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__nvfbcsessionAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS nvfbcsessionCtrlCmdNvFBCSwSessionUpdateInfo_IMPL(struct NvfbcSession *pNvfbcSession, NVA0BD_CTRL_NVFBC_SW_SESSION_UPDATE_INFO_PARAMS *pParams);

NV_STATUS nvfbcsessionConstruct_IMPL(struct NvfbcSession *arg_pNvfbcSession, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_nvfbcsessionConstruct(arg_pNvfbcSession, arg_pCallContext, arg_pParams) nvfbcsessionConstruct_IMPL(arg_pNvfbcSession, arg_pCallContext, arg_pParams)
void nvfbcsessionDestruct_IMPL(struct NvfbcSession *pNvfbcSession);

#define __nvoc_nvfbcsessionDestruct(pNvfbcSession) nvfbcsessionDestruct_IMPL(pNvfbcSession)
#undef PRIVATE_FIELD


NV_STATUS nvA0BDCtrlCmdNvFBCSwSessionUpdateInfo(RmCtrlParams *pRmCtrlParams);
NvBool    nvfbcIsSessionDataStale(NvU64 lastUpdateTimeStamp);

#endif // _NVFBCSESSION_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_NVFBC_SESSION_NVOC_H_
