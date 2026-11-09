
#ifndef _G_MEM_FABRIC_IMPORT_REF_NVOC_H_
#define _G_MEM_FABRIC_IMPORT_REF_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *       This file contains the functions managing the ref-counted imported
 *       fabric memory
 *
 *****************************************************************************/

#pragma once
#include "g_mem_fabric_import_ref_nvoc.h"

#ifndef _MEMORYFABRICIMPORTREF_H_
#define _MEMORYFABRICIMPORTREF_H_

#include "core/core.h"
#include "rmapi/resource.h"
#include "mem_mgr/mem_fabric_import_v2.h"

#include "ctrl/ctrl00fb.h"

// ****************************************************************************
//                          Type Definitions
// ****************************************************************************


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_MEM_FABRIC_IMPORT_REF_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemoryFabricImportedRef;
struct NVOC_METADATA__RmResource;


struct MemoryFabricImportedRef {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MemoryFabricImportedRef *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct MemoryFabricImportedRef *__nvoc_pbase_MemoryFabricImportedRef;    // memoryfabricimportedref

    // Data members
    NvU64 PRIVATE_FIELD(numUpdatedPfns);
    MEMORY_DESCRIPTOR *PRIVATE_FIELD(pTempMemDesc);
    MEM_FABRIC_IMPORT_DESCRIPTOR *PRIVATE_FIELD(pFabricImportDesc);
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemoryFabricImportedRef {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_MemoryFabricImportedRef
#define __nvoc_class_id_MemoryFabricImportedRef 0x189badu
typedef struct MemoryFabricImportedRef MemoryFabricImportedRef;
#endif /* __nvoc_class_id_MemoryFabricImportedRef */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportedRef;

#define __staticCast_MemoryFabricImportedRef(pThis) \
    ((pThis)->__nvoc_pbase_MemoryFabricImportedRef)

#ifdef __nvoc_mem_fabric_import_ref_h_disabled
#define __dynamicCast_MemoryFabricImportedRef(pThis) ((MemoryFabricImportedRef*) NULL)
#else //__nvoc_mem_fabric_import_ref_h_disabled
#define __dynamicCast_MemoryFabricImportedRef(pThis) \
    ((MemoryFabricImportedRef*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryFabricImportedRef)))
#endif //__nvoc_mem_fabric_import_ref_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MemoryFabricImportedRef(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryFabricImportedRef(MemoryFabricImportedRef**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_MemoryFabricImportedRef(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_MemoryFabricImportedRef((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS memoryfabricimportedrefConstruct_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_memoryfabricimportedrefConstruct(pMemoryFabricImportedRef, pCallContext, pParams) memoryfabricimportedrefConstruct_IMPL(pMemoryFabricImportedRef, pCallContext, pParams)

void memoryfabricimportedrefDestruct_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef);
#define __nvoc_memoryfabricimportedrefDestruct(pMemoryFabricImportedRef) memoryfabricimportedrefDestruct_IMPL(pMemoryFabricImportedRef)

NV_STATUS memoryfabricimportedrefCtrlValidate_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef, NV00FB_CTRL_VALIDATE_PARAMS *pParams);
#ifdef __nvoc_mem_fabric_import_ref_h_disabled
static inline NV_STATUS memoryfabricimportedrefCtrlValidate(struct MemoryFabricImportedRef *pMemoryFabricImportedRef, NV00FB_CTRL_VALIDATE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("MemoryFabricImportedRef was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_mem_fabric_import_ref_h_disabled
#define memoryfabricimportedrefCtrlValidate(pMemoryFabricImportedRef, pParams) memoryfabricimportedrefCtrlValidate_IMPL(pMemoryFabricImportedRef, pParams)
#endif // __nvoc_mem_fabric_import_ref_h_disabled


// Wrapper macros for halified functions
#define memoryfabricimportedrefCanCopy_FNPTR(pMemoryFabricImportedRef) pMemoryFabricImportedRef->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define memoryfabricimportedrefCanCopy(pMemoryFabricImportedRef) memoryfabricimportedrefCanCopy_DISPATCH(pMemoryFabricImportedRef)
#define memoryfabricimportedrefAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define memoryfabricimportedrefAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memoryfabricimportedrefAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define memoryfabricimportedrefShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define memoryfabricimportedrefShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memoryfabricimportedrefShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memoryfabricimportedrefGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define memoryfabricimportedrefGetMemInterMapParams(pRmResource, pParams) memoryfabricimportedrefGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define memoryfabricimportedrefCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define memoryfabricimportedrefCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) memoryfabricimportedrefCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define memoryfabricimportedrefGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define memoryfabricimportedrefGetMemoryMappingDescriptor(pRmResource, ppMemDesc) memoryfabricimportedrefGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define memoryfabricimportedrefControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define memoryfabricimportedrefControlSerialization_Prologue(pResource, pCallContext, pParams) memoryfabricimportedrefControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define memoryfabricimportedrefControlSerialization_Epilogue(pResource, pCallContext, pParams) memoryfabricimportedrefControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define memoryfabricimportedrefControl_Prologue(pResource, pCallContext, pParams) memoryfabricimportedrefControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define memoryfabricimportedrefControl_Epilogue(pResource, pCallContext, pParams) memoryfabricimportedrefControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define memoryfabricimportedrefIsDuplicate(pResource, hMemory, pDuplicate) memoryfabricimportedrefIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define memoryfabricimportedrefPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define memoryfabricimportedrefPreDestruct(pResource) memoryfabricimportedrefPreDestruct_DISPATCH(pResource)
#define memoryfabricimportedrefControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define memoryfabricimportedrefControl(pResource, pCallContext, pParams) memoryfabricimportedrefControl_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define memoryfabricimportedrefControlFilter(pResource, pCallContext, pParams) memoryfabricimportedrefControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define memoryfabricimportedrefMap(pResource, pCallContext, pParams, pCpuMapping) memoryfabricimportedrefMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define memoryfabricimportedrefUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define memoryfabricimportedrefUnmap(pResource, pCallContext, pCpuMapping) memoryfabricimportedrefUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define memoryfabricimportedrefIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define memoryfabricimportedrefIsPartialUnmapSupported(pResource) memoryfabricimportedrefIsPartialUnmapSupported_DISPATCH(pResource)
#define memoryfabricimportedrefMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define memoryfabricimportedrefMapTo(pResource, pParams) memoryfabricimportedrefMapTo_DISPATCH(pResource, pParams)
#define memoryfabricimportedrefUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define memoryfabricimportedrefUnmapFrom(pResource, pParams) memoryfabricimportedrefUnmapFrom_DISPATCH(pResource, pParams)
#define memoryfabricimportedrefGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define memoryfabricimportedrefGetRefCount(pResource) memoryfabricimportedrefGetRefCount_DISPATCH(pResource)
#define memoryfabricimportedrefAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define memoryfabricimportedrefAddAdditionalDependants(pClient, pResource, pReference) memoryfabricimportedrefAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool memoryfabricimportedrefCanCopy_DISPATCH(struct MemoryFabricImportedRef *pMemoryFabricImportedRef) {
    return pMemoryFabricImportedRef->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pMemoryFabricImportedRef, RsResource));
}

static inline NvBool memoryfabricimportedrefAccessCallback_DISPATCH(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool memoryfabricimportedrefShareCallback_DISPATCH(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memoryfabricimportedrefGetMemInterMapParams_DISPATCH(struct MemoryFabricImportedRef *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS memoryfabricimportedrefCheckMemInterUnmap_DISPATCH(struct MemoryFabricImportedRef *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS memoryfabricimportedrefGetMemoryMappingDescriptor_DISPATCH(struct MemoryFabricImportedRef *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS memoryfabricimportedrefControlSerialization_Prologue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void memoryfabricimportedrefControlSerialization_Epilogue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefControl_Prologue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void memoryfabricimportedrefControl_Epilogue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefIsDuplicate_DISPATCH(struct MemoryFabricImportedRef *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void memoryfabricimportedrefPreDestruct_DISPATCH(struct MemoryFabricImportedRef *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS memoryfabricimportedrefControl_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefControlFilter_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefMap_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS memoryfabricimportedrefUnmap_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool memoryfabricimportedrefIsPartialUnmapSupported_DISPATCH(struct MemoryFabricImportedRef *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS memoryfabricimportedrefMapTo_DISPATCH(struct MemoryFabricImportedRef *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS memoryfabricimportedrefUnmapFrom_DISPATCH(struct MemoryFabricImportedRef *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 memoryfabricimportedrefGetRefCount_DISPATCH(struct MemoryFabricImportedRef *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void memoryfabricimportedrefAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryFabricImportedRef *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NvBool memoryfabricimportedrefCanCopy_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef);

// Exported method declarations and/or inline definitions
NV_STATUS memoryfabricimportedrefCtrlValidate_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef, NV00FB_CTRL_VALIDATE_PARAMS *pParams);

// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif /* _MEMORYFABRICIMPORTREF_H_ */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_FABRIC_IMPORT_REF_NVOC_H_
