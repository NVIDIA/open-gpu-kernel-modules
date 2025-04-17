
#ifndef _G_MIG_MONITOR_SESSION_NVOC_H_
#define _G_MIG_MONITOR_SESSION_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *      This file contains functions to grant MIG monitor capability
 *
 *   Key attributes of MIGMonitorSession class:
 *   - hClient is parent of MIGMonitorSession.
 *   - As the MIG monitor capability is world accessible by default,
 *     MIGMonitorSession allocation requires a privileged client only
 *     if the platform doesn't implement this capability
 *   - RmApi lock must be held.
 *****************************************************************************/

#pragma once
#include "g_mig_monitor_session_nvoc.h"

#ifndef MIG_MONITOR_SESSION_H
#define MIG_MONITOR_SESSION_H

#include "rmapi/resource.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MIG_MONITOR_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MIGMonitorSession;
struct NVOC_METADATA__RmResource;
struct NVOC_VTABLE__MIGMonitorSession;


struct MIGMonitorSession {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MIGMonitorSession *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct MIGMonitorSession *__nvoc_pbase_MIGMonitorSession;    // migmonitorsession

    // Data members
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};


// Vtable with 21 per-class function pointers
struct NVOC_VTABLE__MIGMonitorSession {
    NvBool (*__migmonitorsessionAccessCallback__)(struct MIGMonitorSession * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__migmonitorsessionShareCallback__)(struct MIGMonitorSession * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__migmonitorsessionGetMemInterMapParams__)(struct MIGMonitorSession * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__migmonitorsessionCheckMemInterUnmap__)(struct MIGMonitorSession * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__migmonitorsessionGetMemoryMappingDescriptor__)(struct MIGMonitorSession * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__migmonitorsessionControlSerialization_Prologue__)(struct MIGMonitorSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__migmonitorsessionControlSerialization_Epilogue__)(struct MIGMonitorSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__migmonitorsessionControl_Prologue__)(struct MIGMonitorSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__migmonitorsessionControl_Epilogue__)(struct MIGMonitorSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__migmonitorsessionCanCopy__)(struct MIGMonitorSession * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__migmonitorsessionIsDuplicate__)(struct MIGMonitorSession * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__migmonitorsessionPreDestruct__)(struct MIGMonitorSession * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__migmonitorsessionControl__)(struct MIGMonitorSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__migmonitorsessionControlFilter__)(struct MIGMonitorSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__migmonitorsessionMap__)(struct MIGMonitorSession * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__migmonitorsessionUnmap__)(struct MIGMonitorSession * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__migmonitorsessionIsPartialUnmapSupported__)(struct MIGMonitorSession * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__migmonitorsessionMapTo__)(struct MIGMonitorSession * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__migmonitorsessionUnmapFrom__)(struct MIGMonitorSession * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__migmonitorsessionGetRefCount__)(struct MIGMonitorSession * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__migmonitorsessionAddAdditionalDependants__)(struct RsClient *, struct MIGMonitorSession * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MIGMonitorSession {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_VTABLE__MIGMonitorSession vtable;
};

#ifndef __NVOC_CLASS_MIGMonitorSession_TYPEDEF__
#define __NVOC_CLASS_MIGMonitorSession_TYPEDEF__
typedef struct MIGMonitorSession MIGMonitorSession;
#endif /* __NVOC_CLASS_MIGMonitorSession_TYPEDEF__ */

#ifndef __nvoc_class_id_MIGMonitorSession
#define __nvoc_class_id_MIGMonitorSession 0x29e15c
#endif /* __nvoc_class_id_MIGMonitorSession */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MIGMonitorSession;

#define __staticCast_MIGMonitorSession(pThis) \
    ((pThis)->__nvoc_pbase_MIGMonitorSession)

#ifdef __nvoc_mig_monitor_session_h_disabled
#define __dynamicCast_MIGMonitorSession(pThis) ((MIGMonitorSession*) NULL)
#else //__nvoc_mig_monitor_session_h_disabled
#define __dynamicCast_MIGMonitorSession(pThis) \
    ((MIGMonitorSession*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MIGMonitorSession)))
#endif //__nvoc_mig_monitor_session_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MIGMonitorSession(MIGMonitorSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MIGMonitorSession(MIGMonitorSession**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_MIGMonitorSession(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MIGMonitorSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define migmonitorsessionAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define migmonitorsessionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) migmonitorsessionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define migmonitorsessionShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define migmonitorsessionShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) migmonitorsessionShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define migmonitorsessionGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define migmonitorsessionGetMemInterMapParams(pRmResource, pParams) migmonitorsessionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define migmonitorsessionCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define migmonitorsessionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) migmonitorsessionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define migmonitorsessionGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define migmonitorsessionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) migmonitorsessionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define migmonitorsessionControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define migmonitorsessionControlSerialization_Prologue(pResource, pCallContext, pParams) migmonitorsessionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define migmonitorsessionControlSerialization_Epilogue(pResource, pCallContext, pParams) migmonitorsessionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define migmonitorsessionControl_Prologue(pResource, pCallContext, pParams) migmonitorsessionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define migmonitorsessionControl_Epilogue(pResource, pCallContext, pParams) migmonitorsessionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define migmonitorsessionCanCopy(pResource) migmonitorsessionCanCopy_DISPATCH(pResource)
#define migmonitorsessionIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define migmonitorsessionIsDuplicate(pResource, hMemory, pDuplicate) migmonitorsessionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define migmonitorsessionPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define migmonitorsessionPreDestruct(pResource) migmonitorsessionPreDestruct_DISPATCH(pResource)
#define migmonitorsessionControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define migmonitorsessionControl(pResource, pCallContext, pParams) migmonitorsessionControl_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define migmonitorsessionControlFilter(pResource, pCallContext, pParams) migmonitorsessionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define migmonitorsessionMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define migmonitorsessionMap(pResource, pCallContext, pParams, pCpuMapping) migmonitorsessionMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define migmonitorsessionUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define migmonitorsessionUnmap(pResource, pCallContext, pCpuMapping) migmonitorsessionUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define migmonitorsessionIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define migmonitorsessionIsPartialUnmapSupported(pResource) migmonitorsessionIsPartialUnmapSupported_DISPATCH(pResource)
#define migmonitorsessionMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define migmonitorsessionMapTo(pResource, pParams) migmonitorsessionMapTo_DISPATCH(pResource, pParams)
#define migmonitorsessionUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define migmonitorsessionUnmapFrom(pResource, pParams) migmonitorsessionUnmapFrom_DISPATCH(pResource, pParams)
#define migmonitorsessionGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define migmonitorsessionGetRefCount(pResource) migmonitorsessionGetRefCount_DISPATCH(pResource)
#define migmonitorsessionAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define migmonitorsessionAddAdditionalDependants(pClient, pResource, pReference) migmonitorsessionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool migmonitorsessionAccessCallback_DISPATCH(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool migmonitorsessionShareCallback_DISPATCH(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS migmonitorsessionGetMemInterMapParams_DISPATCH(struct MIGMonitorSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS migmonitorsessionCheckMemInterUnmap_DISPATCH(struct MIGMonitorSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS migmonitorsessionGetMemoryMappingDescriptor_DISPATCH(struct MIGMonitorSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS migmonitorsessionControlSerialization_Prologue_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void migmonitorsessionControlSerialization_Epilogue_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS migmonitorsessionControl_Prologue_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void migmonitorsessionControl_Epilogue_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool migmonitorsessionCanCopy_DISPATCH(struct MIGMonitorSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionCanCopy__(pResource);
}

static inline NV_STATUS migmonitorsessionIsDuplicate_DISPATCH(struct MIGMonitorSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void migmonitorsessionPreDestruct_DISPATCH(struct MIGMonitorSession *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionPreDestruct__(pResource);
}

static inline NV_STATUS migmonitorsessionControl_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS migmonitorsessionControlFilter_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS migmonitorsessionMap_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS migmonitorsessionUnmap_DISPATCH(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool migmonitorsessionIsPartialUnmapSupported_DISPATCH(struct MIGMonitorSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS migmonitorsessionMapTo_DISPATCH(struct MIGMonitorSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionMapTo__(pResource, pParams);
}

static inline NV_STATUS migmonitorsessionUnmapFrom_DISPATCH(struct MIGMonitorSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionUnmapFrom__(pResource, pParams);
}

static inline NvU32 migmonitorsessionGetRefCount_DISPATCH(struct MIGMonitorSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionGetRefCount__(pResource);
}

static inline void migmonitorsessionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MIGMonitorSession *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__migmonitorsessionAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS migmonitorsessionConstruct_IMPL(struct MIGMonitorSession *arg_pMIGMonitorSession, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_migmonitorsessionConstruct(arg_pMIGMonitorSession, arg_pCallContext, arg_pParams) migmonitorsessionConstruct_IMPL(arg_pMIGMonitorSession, arg_pCallContext, arg_pParams)
void migmonitorsessionDestruct_IMPL(struct MIGMonitorSession *pMIGMonitorSession);

#define __nvoc_migmonitorsessionDestruct(pMIGMonitorSession) migmonitorsessionDestruct_IMPL(pMIGMonitorSession)
#undef PRIVATE_FIELD


#endif // MIG_MONITOR_SESSION_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MIG_MONITOR_SESSION_NVOC_H_
