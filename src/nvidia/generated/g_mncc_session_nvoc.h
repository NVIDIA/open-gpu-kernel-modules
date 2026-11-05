
#ifndef _G_MNCC_SESSION_NVOC_H_
#define _G_MNCC_SESSION_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *      MNCC Session - capability acquisition class for Multi-Node
 *      Confidential Computing clients.
 *
 *   A client which owns MnccSession is identified as the MNCC daemon in RM
 *   land.  Allocating this class validates NV_RM_CAP_SYS_MNCC_MGMT and holds
 *   onto the duped capability descriptor for the lifetime of the session.
 *
 *   Key attributes of the MnccSession class:
 *   - hClient is parent of MnccSession.
 *   - Non-admin callers without a valid capability FD are rejected at alloc.
 *   - RmApi lock must be held. This class should not communicate with a GPU.
 *
 *****************************************************************************/

#pragma once
#include "g_mncc_session_nvoc.h"

#ifndef MNCC_SESSION_H
#define MNCC_SESSION_H


#include "rmapi/resource.h"


// ****************************************************************************
//                          Type Definitions
// ****************************************************************************


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MNCC_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MnccSession;
struct NVOC_METADATA__RmResource;


struct MnccSession {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MnccSession *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct MnccSession *__nvoc_pbase_MnccSession;    // mnccSession

    // Data members
    NvU64 dupedCapDescriptor;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MnccSession {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_MnccSession
#define __nvoc_class_id_MnccSession 0x50aee7u
typedef struct MnccSession MnccSession;
#endif /* __nvoc_class_id_MnccSession */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MnccSession;

#define __staticCast_MnccSession(pThis) \
    ((pThis)->__nvoc_pbase_MnccSession)

#ifdef __nvoc_mncc_session_h_disabled
#define __dynamicCast_MnccSession(pThis) ((MnccSession*) NULL)
#else //__nvoc_mncc_session_h_disabled
#define __dynamicCast_MnccSession(pThis) \
    ((MnccSession*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MnccSession)))
#endif //__nvoc_mncc_session_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MnccSession(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MnccSession(MnccSession**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_MnccSession(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_MnccSession((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS mnccSessionConstruct_IMPL(struct MnccSession *pMnccSession, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_mnccSessionConstruct(pMnccSession, pCallContext, pParams) mnccSessionConstruct_IMPL(pMnccSession, pCallContext, pParams)

void mnccSessionDestruct_IMPL(struct MnccSession *pMnccSession);
#define __nvoc_mnccSessionDestruct(pMnccSession) mnccSessionDestruct_IMPL(pMnccSession)


// Wrapper macros for halified functions
#define mnccSessionAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define mnccSessionAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) mnccSessionAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define mnccSessionShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define mnccSessionShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) mnccSessionShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define mnccSessionGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define mnccSessionGetMemInterMapParams(pRmResource, pParams) mnccSessionGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define mnccSessionCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define mnccSessionCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) mnccSessionCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define mnccSessionGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define mnccSessionGetMemoryMappingDescriptor(pRmResource, ppMemDesc) mnccSessionGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define mnccSessionControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define mnccSessionControlSerialization_Prologue(pResource, pCallContext, pParams) mnccSessionControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define mnccSessionControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define mnccSessionControlSerialization_Epilogue(pResource, pCallContext, pParams) mnccSessionControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define mnccSessionControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define mnccSessionControl_Prologue(pResource, pCallContext, pParams) mnccSessionControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define mnccSessionControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define mnccSessionControl_Epilogue(pResource, pCallContext, pParams) mnccSessionControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define mnccSessionCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define mnccSessionCanCopy(pResource) mnccSessionCanCopy_DISPATCH(pResource)
#define mnccSessionIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define mnccSessionIsDuplicate(pResource, hMemory, pDuplicate) mnccSessionIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define mnccSessionPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define mnccSessionPreDestruct(pResource) mnccSessionPreDestruct_DISPATCH(pResource)
#define mnccSessionControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define mnccSessionControl(pResource, pCallContext, pParams) mnccSessionControl_DISPATCH(pResource, pCallContext, pParams)
#define mnccSessionControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define mnccSessionControlFilter(pResource, pCallContext, pParams) mnccSessionControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define mnccSessionMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define mnccSessionMap(pResource, pCallContext, pParams, pCpuMapping) mnccSessionMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define mnccSessionUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define mnccSessionUnmap(pResource, pCallContext, pCpuMapping) mnccSessionUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define mnccSessionIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define mnccSessionIsPartialUnmapSupported(pResource) mnccSessionIsPartialUnmapSupported_DISPATCH(pResource)
#define mnccSessionMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define mnccSessionMapTo(pResource, pParams) mnccSessionMapTo_DISPATCH(pResource, pParams)
#define mnccSessionUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define mnccSessionUnmapFrom(pResource, pParams) mnccSessionUnmapFrom_DISPATCH(pResource, pParams)
#define mnccSessionGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define mnccSessionGetRefCount(pResource) mnccSessionGetRefCount_DISPATCH(pResource)
#define mnccSessionAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define mnccSessionAddAdditionalDependants(pClient, pResource, pReference) mnccSessionAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool mnccSessionAccessCallback_DISPATCH(struct MnccSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool mnccSessionShareCallback_DISPATCH(struct MnccSession *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS mnccSessionGetMemInterMapParams_DISPATCH(struct MnccSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS mnccSessionCheckMemInterUnmap_DISPATCH(struct MnccSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS mnccSessionGetMemoryMappingDescriptor_DISPATCH(struct MnccSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS mnccSessionControlSerialization_Prologue_DISPATCH(struct MnccSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void mnccSessionControlSerialization_Epilogue_DISPATCH(struct MnccSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS mnccSessionControl_Prologue_DISPATCH(struct MnccSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void mnccSessionControl_Epilogue_DISPATCH(struct MnccSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool mnccSessionCanCopy_DISPATCH(struct MnccSession *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS mnccSessionIsDuplicate_DISPATCH(struct MnccSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void mnccSessionPreDestruct_DISPATCH(struct MnccSession *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS mnccSessionControl_DISPATCH(struct MnccSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS mnccSessionControlFilter_DISPATCH(struct MnccSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS mnccSessionMap_DISPATCH(struct MnccSession *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS mnccSessionUnmap_DISPATCH(struct MnccSession *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool mnccSessionIsPartialUnmapSupported_DISPATCH(struct MnccSession *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS mnccSessionMapTo_DISPATCH(struct MnccSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS mnccSessionUnmapFrom_DISPATCH(struct MnccSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 mnccSessionGetRefCount_DISPATCH(struct MnccSession *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void mnccSessionAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MnccSession *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



#endif // MNCC_SESSION_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MNCC_SESSION_NVOC_H_
