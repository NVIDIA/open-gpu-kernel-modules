
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


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MemoryFabricImportedRef;
struct NVOC_METADATA__RmResource;
struct NVOC_VTABLE__MemoryFabricImportedRef;


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

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__memoryfabricimportedrefCtrlValidate__)(struct MemoryFabricImportedRef * /*this*/, NV00FB_CTRL_VALIDATE_PARAMS *);  // exported (id=0xfb0101)

    // Data members
    NvU64 PRIVATE_FIELD(numUpdatedPfns);
    MEMORY_DESCRIPTOR *PRIVATE_FIELD(pTempMemDesc);
    MEM_FABRIC_IMPORT_DESCRIPTOR *PRIVATE_FIELD(pFabricImportDesc);
};


// Vtable with 21 per-class function pointers
struct NVOC_VTABLE__MemoryFabricImportedRef {
    NvBool (*__memoryfabricimportedrefCanCopy__)(struct MemoryFabricImportedRef * /*this*/);  // virtual override (res) base (rmres)
    NvBool (*__memoryfabricimportedrefAccessCallback__)(struct MemoryFabricImportedRef * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NvBool (*__memoryfabricimportedrefShareCallback__)(struct MemoryFabricImportedRef * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefGetMemInterMapParams__)(struct MemoryFabricImportedRef * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefCheckMemInterUnmap__)(struct MemoryFabricImportedRef * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefGetMemoryMappingDescriptor__)(struct MemoryFabricImportedRef * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefControlSerialization_Prologue__)(struct MemoryFabricImportedRef * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__memoryfabricimportedrefControlSerialization_Epilogue__)(struct MemoryFabricImportedRef * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefControl_Prologue__)(struct MemoryFabricImportedRef * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__memoryfabricimportedrefControl_Epilogue__)(struct MemoryFabricImportedRef * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefIsDuplicate__)(struct MemoryFabricImportedRef * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__memoryfabricimportedrefPreDestruct__)(struct MemoryFabricImportedRef * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefControl__)(struct MemoryFabricImportedRef * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefControlFilter__)(struct MemoryFabricImportedRef * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefMap__)(struct MemoryFabricImportedRef * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefUnmap__)(struct MemoryFabricImportedRef * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (rmres)
    NvBool (*__memoryfabricimportedrefIsPartialUnmapSupported__)(struct MemoryFabricImportedRef * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__memoryfabricimportedrefMapTo__)(struct MemoryFabricImportedRef * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__memoryfabricimportedrefUnmapFrom__)(struct MemoryFabricImportedRef * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__memoryfabricimportedrefGetRefCount__)(struct MemoryFabricImportedRef * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__memoryfabricimportedrefAddAdditionalDependants__)(struct RsClient *, struct MemoryFabricImportedRef * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__MemoryFabricImportedRef {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_VTABLE__MemoryFabricImportedRef vtable;
};

#ifndef __NVOC_CLASS_MemoryFabricImportedRef_TYPEDEF__
#define __NVOC_CLASS_MemoryFabricImportedRef_TYPEDEF__
typedef struct MemoryFabricImportedRef MemoryFabricImportedRef;
#endif /* __NVOC_CLASS_MemoryFabricImportedRef_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryFabricImportedRef
#define __nvoc_class_id_MemoryFabricImportedRef 0x189bad
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

NV_STATUS __nvoc_objCreateDynamic_MemoryFabricImportedRef(MemoryFabricImportedRef**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryFabricImportedRef(MemoryFabricImportedRef**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_MemoryFabricImportedRef(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryFabricImportedRef((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define memoryfabricimportedrefCanCopy_FNPTR(pMemoryFabricImportedRef) pMemoryFabricImportedRef->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefCanCopy__
#define memoryfabricimportedrefCanCopy(pMemoryFabricImportedRef) memoryfabricimportedrefCanCopy_DISPATCH(pMemoryFabricImportedRef)
#define memoryfabricimportedrefCtrlValidate_FNPTR(pMemoryFabricImportedRef) pMemoryFabricImportedRef->__memoryfabricimportedrefCtrlValidate__
#define memoryfabricimportedrefCtrlValidate(pMemoryFabricImportedRef, pParams) memoryfabricimportedrefCtrlValidate_DISPATCH(pMemoryFabricImportedRef, pParams)
#define memoryfabricimportedrefAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define memoryfabricimportedrefAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memoryfabricimportedrefAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define memoryfabricimportedrefShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define memoryfabricimportedrefShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memoryfabricimportedrefShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memoryfabricimportedrefGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define memoryfabricimportedrefGetMemInterMapParams(pRmResource, pParams) memoryfabricimportedrefGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define memoryfabricimportedrefCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define memoryfabricimportedrefCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) memoryfabricimportedrefCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define memoryfabricimportedrefGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define memoryfabricimportedrefGetMemoryMappingDescriptor(pRmResource, ppMemDesc) memoryfabricimportedrefGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define memoryfabricimportedrefControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define memoryfabricimportedrefControlSerialization_Prologue(pResource, pCallContext, pParams) memoryfabricimportedrefControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define memoryfabricimportedrefControlSerialization_Epilogue(pResource, pCallContext, pParams) memoryfabricimportedrefControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define memoryfabricimportedrefControl_Prologue(pResource, pCallContext, pParams) memoryfabricimportedrefControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
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
    return pMemoryFabricImportedRef->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefCanCopy__(pMemoryFabricImportedRef);
}

static inline NV_STATUS memoryfabricimportedrefCtrlValidate_DISPATCH(struct MemoryFabricImportedRef *pMemoryFabricImportedRef, NV00FB_CTRL_VALIDATE_PARAMS *pParams) {
    return pMemoryFabricImportedRef->__memoryfabricimportedrefCtrlValidate__(pMemoryFabricImportedRef, pParams);
}

static inline NvBool memoryfabricimportedrefAccessCallback_DISPATCH(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool memoryfabricimportedrefShareCallback_DISPATCH(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memoryfabricimportedrefGetMemInterMapParams_DISPATCH(struct MemoryFabricImportedRef *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS memoryfabricimportedrefCheckMemInterUnmap_DISPATCH(struct MemoryFabricImportedRef *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS memoryfabricimportedrefGetMemoryMappingDescriptor_DISPATCH(struct MemoryFabricImportedRef *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS memoryfabricimportedrefControlSerialization_Prologue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void memoryfabricimportedrefControlSerialization_Epilogue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefControl_Prologue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void memoryfabricimportedrefControl_Epilogue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefIsDuplicate_DISPATCH(struct MemoryFabricImportedRef *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void memoryfabricimportedrefPreDestruct_DISPATCH(struct MemoryFabricImportedRef *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefPreDestruct__(pResource);
}

static inline NV_STATUS memoryfabricimportedrefControl_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefControlFilter_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefMap_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS memoryfabricimportedrefUnmap_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool memoryfabricimportedrefIsPartialUnmapSupported_DISPATCH(struct MemoryFabricImportedRef *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS memoryfabricimportedrefMapTo_DISPATCH(struct MemoryFabricImportedRef *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefMapTo__(pResource, pParams);
}

static inline NV_STATUS memoryfabricimportedrefUnmapFrom_DISPATCH(struct MemoryFabricImportedRef *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefUnmapFrom__(pResource, pParams);
}

static inline NvU32 memoryfabricimportedrefGetRefCount_DISPATCH(struct MemoryFabricImportedRef *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefGetRefCount__(pResource);
}

static inline void memoryfabricimportedrefAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryFabricImportedRef *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__memoryfabricimportedrefAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool memoryfabricimportedrefCanCopy_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef);

NV_STATUS memoryfabricimportedrefCtrlValidate_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef, NV00FB_CTRL_VALIDATE_PARAMS *pParams);

NV_STATUS memoryfabricimportedrefConstruct_IMPL(struct MemoryFabricImportedRef *arg_pMemoryFabricImportedRef, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memoryfabricimportedrefConstruct(arg_pMemoryFabricImportedRef, arg_pCallContext, arg_pParams) memoryfabricimportedrefConstruct_IMPL(arg_pMemoryFabricImportedRef, arg_pCallContext, arg_pParams)
void memoryfabricimportedrefDestruct_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef);

#define __nvoc_memoryfabricimportedrefDestruct(pMemoryFabricImportedRef) memoryfabricimportedrefDestruct_IMPL(pMemoryFabricImportedRef)
#undef PRIVATE_FIELD


#endif /* _MEMORYFABRICIMPORTREF_H_ */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_FABRIC_IMPORT_REF_NVOC_H_
