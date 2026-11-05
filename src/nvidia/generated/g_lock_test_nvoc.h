
#ifndef _G_LOCK_TEST_NVOC_H_
#define _G_LOCK_TEST_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_lock_test_nvoc.h"

#ifndef LOCK_TEST_H
#define LOCK_TEST_H

#include "gpu/gpu_resource.h"
#include "nvoc/prelude.h"
#include "nvstatus.h"
#include "resserv/resserv.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_LOCK_TEST_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LockTestRelaxedDupObject;
struct NVOC_METADATA__RmResource;


struct LockTestRelaxedDupObject {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LockTestRelaxedDupObject *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct LockTestRelaxedDupObject *__nvoc_pbase_LockTestRelaxedDupObject;    // lockTestRelaxedDupObj
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LockTestRelaxedDupObject {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_LockTestRelaxedDupObject
#define __nvoc_class_id_LockTestRelaxedDupObject 0x19e861u
typedef struct LockTestRelaxedDupObject LockTestRelaxedDupObject;
#endif /* __nvoc_class_id_LockTestRelaxedDupObject */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LockTestRelaxedDupObject;

#define __staticCast_LockTestRelaxedDupObject(pThis) \
    ((pThis)->__nvoc_pbase_LockTestRelaxedDupObject)

#ifdef __nvoc_lock_test_h_disabled
#define __dynamicCast_LockTestRelaxedDupObject(pThis) ((LockTestRelaxedDupObject*) NULL)
#else //__nvoc_lock_test_h_disabled
#define __dynamicCast_LockTestRelaxedDupObject(pThis) \
    ((LockTestRelaxedDupObject*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LockTestRelaxedDupObject)))
#endif //__nvoc_lock_test_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LockTestRelaxedDupObject(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LockTestRelaxedDupObject(LockTestRelaxedDupObject**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_LockTestRelaxedDupObject(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_LockTestRelaxedDupObject((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS lockTestRelaxedDupObjConstruct_IMPL(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_lockTestRelaxedDupObjConstruct(pResource, pCallContext, pParams) lockTestRelaxedDupObjConstruct_IMPL(pResource, pCallContext, pParams)

void lockTestRelaxedDupObjDestruct_IMPL(struct LockTestRelaxedDupObject *pResource);
#define __nvoc_lockTestRelaxedDupObjDestruct(pResource) lockTestRelaxedDupObjDestruct_IMPL(pResource)


// Wrapper macros for halified functions
#define lockTestRelaxedDupObjCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define lockTestRelaxedDupObjCanCopy(pResource) lockTestRelaxedDupObjCanCopy_DISPATCH(pResource)
#define lockTestRelaxedDupObjAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define lockTestRelaxedDupObjAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) lockTestRelaxedDupObjAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define lockTestRelaxedDupObjShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define lockTestRelaxedDupObjShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) lockTestRelaxedDupObjShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define lockTestRelaxedDupObjGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define lockTestRelaxedDupObjGetMemInterMapParams(pRmResource, pParams) lockTestRelaxedDupObjGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define lockTestRelaxedDupObjCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define lockTestRelaxedDupObjCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) lockTestRelaxedDupObjCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define lockTestRelaxedDupObjGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define lockTestRelaxedDupObjGetMemoryMappingDescriptor(pRmResource, ppMemDesc) lockTestRelaxedDupObjGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define lockTestRelaxedDupObjControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define lockTestRelaxedDupObjControlSerialization_Prologue(pResource, pCallContext, pParams) lockTestRelaxedDupObjControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define lockTestRelaxedDupObjControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define lockTestRelaxedDupObjControlSerialization_Epilogue(pResource, pCallContext, pParams) lockTestRelaxedDupObjControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define lockTestRelaxedDupObjControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define lockTestRelaxedDupObjControl_Prologue(pResource, pCallContext, pParams) lockTestRelaxedDupObjControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define lockTestRelaxedDupObjControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define lockTestRelaxedDupObjControl_Epilogue(pResource, pCallContext, pParams) lockTestRelaxedDupObjControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define lockTestRelaxedDupObjIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define lockTestRelaxedDupObjIsDuplicate(pResource, hMemory, pDuplicate) lockTestRelaxedDupObjIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define lockTestRelaxedDupObjPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define lockTestRelaxedDupObjPreDestruct(pResource) lockTestRelaxedDupObjPreDestruct_DISPATCH(pResource)
#define lockTestRelaxedDupObjControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define lockTestRelaxedDupObjControl(pResource, pCallContext, pParams) lockTestRelaxedDupObjControl_DISPATCH(pResource, pCallContext, pParams)
#define lockTestRelaxedDupObjControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define lockTestRelaxedDupObjControlFilter(pResource, pCallContext, pParams) lockTestRelaxedDupObjControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define lockTestRelaxedDupObjMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define lockTestRelaxedDupObjMap(pResource, pCallContext, pParams, pCpuMapping) lockTestRelaxedDupObjMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define lockTestRelaxedDupObjUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define lockTestRelaxedDupObjUnmap(pResource, pCallContext, pCpuMapping) lockTestRelaxedDupObjUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define lockTestRelaxedDupObjIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define lockTestRelaxedDupObjIsPartialUnmapSupported(pResource) lockTestRelaxedDupObjIsPartialUnmapSupported_DISPATCH(pResource)
#define lockTestRelaxedDupObjMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define lockTestRelaxedDupObjMapTo(pResource, pParams) lockTestRelaxedDupObjMapTo_DISPATCH(pResource, pParams)
#define lockTestRelaxedDupObjUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define lockTestRelaxedDupObjUnmapFrom(pResource, pParams) lockTestRelaxedDupObjUnmapFrom_DISPATCH(pResource, pParams)
#define lockTestRelaxedDupObjGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define lockTestRelaxedDupObjGetRefCount(pResource) lockTestRelaxedDupObjGetRefCount_DISPATCH(pResource)
#define lockTestRelaxedDupObjAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define lockTestRelaxedDupObjAddAdditionalDependants(pClient, pResource, pReference) lockTestRelaxedDupObjAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool lockTestRelaxedDupObjCanCopy_DISPATCH(struct LockTestRelaxedDupObject *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NvBool lockTestRelaxedDupObjAccessCallback_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool lockTestRelaxedDupObjShareCallback_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS lockTestRelaxedDupObjGetMemInterMapParams_DISPATCH(struct LockTestRelaxedDupObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS lockTestRelaxedDupObjCheckMemInterUnmap_DISPATCH(struct LockTestRelaxedDupObject *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS lockTestRelaxedDupObjGetMemoryMappingDescriptor_DISPATCH(struct LockTestRelaxedDupObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS lockTestRelaxedDupObjControlSerialization_Prologue_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void lockTestRelaxedDupObjControlSerialization_Epilogue_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS lockTestRelaxedDupObjControl_Prologue_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void lockTestRelaxedDupObjControl_Epilogue_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS lockTestRelaxedDupObjIsDuplicate_DISPATCH(struct LockTestRelaxedDupObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void lockTestRelaxedDupObjPreDestruct_DISPATCH(struct LockTestRelaxedDupObject *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS lockTestRelaxedDupObjControl_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS lockTestRelaxedDupObjControlFilter_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS lockTestRelaxedDupObjMap_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS lockTestRelaxedDupObjUnmap_DISPATCH(struct LockTestRelaxedDupObject *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool lockTestRelaxedDupObjIsPartialUnmapSupported_DISPATCH(struct LockTestRelaxedDupObject *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS lockTestRelaxedDupObjMapTo_DISPATCH(struct LockTestRelaxedDupObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS lockTestRelaxedDupObjUnmapFrom_DISPATCH(struct LockTestRelaxedDupObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 lockTestRelaxedDupObjGetRefCount_DISPATCH(struct LockTestRelaxedDupObject *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void lockTestRelaxedDupObjAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct LockTestRelaxedDupObject *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
static inline NvBool lockTestRelaxedDupObjCanCopy_e661f0(struct LockTestRelaxedDupObject *pResource){
    return NV_TRUE;
}

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // LOCK_TEST_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_LOCK_TEST_NVOC_H_
