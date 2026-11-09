
#ifndef _G_MIG_CONFIG_SESSION_NVOC_H_
#define _G_MIG_CONFIG_SESSION_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2019-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing MIG configuration
 *
 *   Key attributes of MIGConfigSession class:
 *   - hClient is parent of MIGConfigSession.
 *   - MIGConfigSession can be allocated by privileged client.
 *   - RmApi lock must be held.
 *****************************************************************************/

#pragma once
#include "g_mig_config_session_nvoc.h"

#ifndef MIG_CONFIG_SESSION_H
#define MIG_CONFIG_SESSION_H

#include "rmapi/resource.h"
#include "class/clc639.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MIG_CONFIG_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MIGConfigSession;
struct NVOC_METADATA__RmResource;


struct MIGConfigSession {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MIGConfigSession *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct MIGConfigSession *__nvoc_pbase_MIGConfigSession;    // migconfigsession

    // Data members
    NvU64 PRIVATE_FIELD(dupedCapDescriptor);
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MIGConfigSession {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_MIGConfigSession
#define __nvoc_class_id_MIGConfigSession 0x36a941u
typedef struct MIGConfigSession MIGConfigSession;
#endif /* __nvoc_class_id_MIGConfigSession */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MIGConfigSession;

#define __staticCast_MIGConfigSession(pThis) \
    ((pThis)->__nvoc_pbase_MIGConfigSession)

#ifdef __nvoc_mig_config_session_h_disabled
#define __dynamicCast_MIGConfigSession(pThis) ((MIGConfigSession*) NULL)
#else //__nvoc_mig_config_session_h_disabled
#define __dynamicCast_MIGConfigSession(pThis) \
    ((MIGConfigSession*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MIGConfigSession)))
#endif //__nvoc_mig_config_session_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MIGConfigSession(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MIGConfigSession(MIGConfigSession**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_MIGConfigSession(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_MIGConfigSession((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS migconfigsessionConstruct_IMPL(struct MIGConfigSession *pMIGConfigSession, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_migconfigsessionConstruct(pMIGConfigSession, pCallContext, pParams) migconfigsessionConstruct_IMPL(pMIGConfigSession, pCallContext, pParams)

void migconfigsessionDestruct_IMPL(struct MIGConfigSession *pMIGConfigSession);
#define __nvoc_migconfigsessionDestruct(pMIGConfigSession) migconfigsessionDestruct_IMPL(pMIGConfigSession)


// Wrapper macros for halified functions
#define migconfigsessionAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define migconfigsessionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) migconfigsessionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define migconfigsessionShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define migconfigsessionShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) migconfigsessionShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define migconfigsessionGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define migconfigsessionGetMemInterMapParams(pRmResource, pParams) migconfigsessionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define migconfigsessionCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define migconfigsessionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) migconfigsessionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define migconfigsessionGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define migconfigsessionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) migconfigsessionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define migconfigsessionControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define migconfigsessionControlSerialization_Prologue(pResource, pCallContext, pParams) migconfigsessionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define migconfigsessionControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define migconfigsessionControlSerialization_Epilogue(pResource, pCallContext, pParams) migconfigsessionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define migconfigsessionControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define migconfigsessionControl_Prologue(pResource, pCallContext, pParams) migconfigsessionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define migconfigsessionControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define migconfigsessionControl_Epilogue(pResource, pCallContext, pParams) migconfigsessionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define migconfigsessionCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define migconfigsessionCanCopy(pResource) migconfigsessionCanCopy_DISPATCH(pResource)
#define migconfigsessionIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define migconfigsessionIsDuplicate(pResource, hMemory, pDuplicate) migconfigsessionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define migconfigsessionPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define migconfigsessionPreDestruct(pResource) migconfigsessionPreDestruct_DISPATCH(pResource)
#define migconfigsessionControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define migconfigsessionControl(pResource, pCallContext, pParams) migconfigsessionControl_DISPATCH(pResource, pCallContext, pParams)
#define migconfigsessionControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define migconfigsessionControlFilter(pResource, pCallContext, pParams) migconfigsessionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define migconfigsessionMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define migconfigsessionMap(pResource, pCallContext, pParams, pCpuMapping) migconfigsessionMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define migconfigsessionUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define migconfigsessionUnmap(pResource, pCallContext, pCpuMapping) migconfigsessionUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define migconfigsessionIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define migconfigsessionIsPartialUnmapSupported(pResource) migconfigsessionIsPartialUnmapSupported_DISPATCH(pResource)
#define migconfigsessionMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define migconfigsessionMapTo(pResource, pParams) migconfigsessionMapTo_DISPATCH(pResource, pParams)
#define migconfigsessionUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define migconfigsessionUnmapFrom(pResource, pParams) migconfigsessionUnmapFrom_DISPATCH(pResource, pParams)
#define migconfigsessionGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define migconfigsessionGetRefCount(pResource) migconfigsessionGetRefCount_DISPATCH(pResource)
#define migconfigsessionAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define migconfigsessionAddAdditionalDependants(pClient, pResource, pReference) migconfigsessionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool migconfigsessionAccessCallback_DISPATCH(struct MIGConfigSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool migconfigsessionShareCallback_DISPATCH(struct MIGConfigSession *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS migconfigsessionGetMemInterMapParams_DISPATCH(struct MIGConfigSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS migconfigsessionCheckMemInterUnmap_DISPATCH(struct MIGConfigSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS migconfigsessionGetMemoryMappingDescriptor_DISPATCH(struct MIGConfigSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS migconfigsessionControlSerialization_Prologue_DISPATCH(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void migconfigsessionControlSerialization_Epilogue_DISPATCH(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS migconfigsessionControl_Prologue_DISPATCH(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void migconfigsessionControl_Epilogue_DISPATCH(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool migconfigsessionCanCopy_DISPATCH(struct MIGConfigSession *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS migconfigsessionIsDuplicate_DISPATCH(struct MIGConfigSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void migconfigsessionPreDestruct_DISPATCH(struct MIGConfigSession *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS migconfigsessionControl_DISPATCH(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS migconfigsessionControlFilter_DISPATCH(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS migconfigsessionMap_DISPATCH(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS migconfigsessionUnmap_DISPATCH(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool migconfigsessionIsPartialUnmapSupported_DISPATCH(struct MIGConfigSession *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS migconfigsessionMapTo_DISPATCH(struct MIGConfigSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS migconfigsessionUnmapFrom_DISPATCH(struct MIGConfigSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 migconfigsessionGetRefCount_DISPATCH(struct MIGConfigSession *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void migconfigsessionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MIGConfigSession *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // MIG_CONFIG_SESSION_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MIG_CONFIG_SESSION_NVOC_H_
