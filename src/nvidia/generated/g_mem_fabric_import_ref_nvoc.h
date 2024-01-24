#ifndef _G_MEM_FABRIC_IMPORT_REF_NVOC_H_
#define _G_MEM_FABRIC_IMPORT_REF_NVOC_H_
#include "nvoc/runtime.h"

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

struct MemoryFabricImportedRef {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct MemoryFabricImportedRef *__nvoc_pbase_MemoryFabricImportedRef;
    NvBool (*__memoryfabricimportedrefCanCopy__)(struct MemoryFabricImportedRef *);
    NV_STATUS (*__memoryfabricimportedrefCtrlValidate__)(struct MemoryFabricImportedRef *, NV00FB_CTRL_VALIDATE_PARAMS *);
    NvBool (*__memoryfabricimportedrefShareCallback__)(struct MemoryFabricImportedRef *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__memoryfabricimportedrefCheckMemInterUnmap__)(struct MemoryFabricImportedRef *, NvBool);
    NV_STATUS (*__memoryfabricimportedrefControl__)(struct MemoryFabricImportedRef *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricimportedrefGetMemInterMapParams__)(struct MemoryFabricImportedRef *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__memoryfabricimportedrefGetMemoryMappingDescriptor__)(struct MemoryFabricImportedRef *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__memoryfabricimportedrefGetRefCount__)(struct MemoryFabricImportedRef *);
    NV_STATUS (*__memoryfabricimportedrefControlFilter__)(struct MemoryFabricImportedRef *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__memoryfabricimportedrefAddAdditionalDependants__)(struct RsClient *, struct MemoryFabricImportedRef *, RsResourceRef *);
    NV_STATUS (*__memoryfabricimportedrefControlSerialization_Prologue__)(struct MemoryFabricImportedRef *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricimportedrefControl_Prologue__)(struct MemoryFabricImportedRef *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricimportedrefUnmap__)(struct MemoryFabricImportedRef *, struct CALL_CONTEXT *, RsCpuMapping *);
    NvBool (*__memoryfabricimportedrefIsPartialUnmapSupported__)(struct MemoryFabricImportedRef *);
    void (*__memoryfabricimportedrefPreDestruct__)(struct MemoryFabricImportedRef *);
    NV_STATUS (*__memoryfabricimportedrefMapTo__)(struct MemoryFabricImportedRef *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__memoryfabricimportedrefIsDuplicate__)(struct MemoryFabricImportedRef *, NvHandle, NvBool *);
    void (*__memoryfabricimportedrefControlSerialization_Epilogue__)(struct MemoryFabricImportedRef *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__memoryfabricimportedrefControl_Epilogue__)(struct MemoryFabricImportedRef *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memoryfabricimportedrefUnmapFrom__)(struct MemoryFabricImportedRef *, RS_RES_UNMAP_FROM_PARAMS *);
    NV_STATUS (*__memoryfabricimportedrefMap__)(struct MemoryFabricImportedRef *, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__memoryfabricimportedrefAccessCallback__)(struct MemoryFabricImportedRef *, struct RsClient *, void *, RsAccessRight);
    NvU64 PRIVATE_FIELD(numUpdatedPfns);
    MEMORY_DESCRIPTOR *PRIVATE_FIELD(pTempMemDesc);
    MEM_FABRIC_IMPORT_DESCRIPTOR *PRIVATE_FIELD(pFabricImportDesc);
};

#ifndef __NVOC_CLASS_MemoryFabricImportedRef_TYPEDEF__
#define __NVOC_CLASS_MemoryFabricImportedRef_TYPEDEF__
typedef struct MemoryFabricImportedRef MemoryFabricImportedRef;
#endif /* __NVOC_CLASS_MemoryFabricImportedRef_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryFabricImportedRef
#define __nvoc_class_id_MemoryFabricImportedRef 0x189bad
#endif /* __nvoc_class_id_MemoryFabricImportedRef */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryFabricImportedRef;

#define __staticCast_MemoryFabricImportedRef(pThis) \
    ((pThis)->__nvoc_pbase_MemoryFabricImportedRef)

#ifdef __nvoc_mem_fabric_import_ref_h_disabled
#define __dynamicCast_MemoryFabricImportedRef(pThis) ((MemoryFabricImportedRef*)NULL)
#else //__nvoc_mem_fabric_import_ref_h_disabled
#define __dynamicCast_MemoryFabricImportedRef(pThis) \
    ((MemoryFabricImportedRef*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryFabricImportedRef)))
#endif //__nvoc_mem_fabric_import_ref_h_disabled


NV_STATUS __nvoc_objCreateDynamic_MemoryFabricImportedRef(MemoryFabricImportedRef**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryFabricImportedRef(MemoryFabricImportedRef**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MemoryFabricImportedRef(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryFabricImportedRef((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define memoryfabricimportedrefCanCopy(pMemoryFabricImportedRef) memoryfabricimportedrefCanCopy_DISPATCH(pMemoryFabricImportedRef)
#define memoryfabricimportedrefCtrlValidate(pMemoryFabricImportedRef, pParams) memoryfabricimportedrefCtrlValidate_DISPATCH(pMemoryFabricImportedRef, pParams)
#define memoryfabricimportedrefShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) memoryfabricimportedrefShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define memoryfabricimportedrefCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) memoryfabricimportedrefCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define memoryfabricimportedrefControl(pResource, pCallContext, pParams) memoryfabricimportedrefControl_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefGetMemInterMapParams(pRmResource, pParams) memoryfabricimportedrefGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define memoryfabricimportedrefGetMemoryMappingDescriptor(pRmResource, ppMemDesc) memoryfabricimportedrefGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define memoryfabricimportedrefGetRefCount(pResource) memoryfabricimportedrefGetRefCount_DISPATCH(pResource)
#define memoryfabricimportedrefControlFilter(pResource, pCallContext, pParams) memoryfabricimportedrefControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefAddAdditionalDependants(pClient, pResource, pReference) memoryfabricimportedrefAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define memoryfabricimportedrefControlSerialization_Prologue(pResource, pCallContext, pParams) memoryfabricimportedrefControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefControl_Prologue(pResource, pCallContext, pParams) memoryfabricimportedrefControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefUnmap(pResource, pCallContext, pCpuMapping) memoryfabricimportedrefUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define memoryfabricimportedrefIsPartialUnmapSupported(pResource) memoryfabricimportedrefIsPartialUnmapSupported_DISPATCH(pResource)
#define memoryfabricimportedrefPreDestruct(pResource) memoryfabricimportedrefPreDestruct_DISPATCH(pResource)
#define memoryfabricimportedrefMapTo(pResource, pParams) memoryfabricimportedrefMapTo_DISPATCH(pResource, pParams)
#define memoryfabricimportedrefIsDuplicate(pResource, hMemory, pDuplicate) memoryfabricimportedrefIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define memoryfabricimportedrefControlSerialization_Epilogue(pResource, pCallContext, pParams) memoryfabricimportedrefControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefControl_Epilogue(pResource, pCallContext, pParams) memoryfabricimportedrefControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memoryfabricimportedrefUnmapFrom(pResource, pParams) memoryfabricimportedrefUnmapFrom_DISPATCH(pResource, pParams)
#define memoryfabricimportedrefMap(pResource, pCallContext, pParams, pCpuMapping) memoryfabricimportedrefMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define memoryfabricimportedrefAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memoryfabricimportedrefAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool memoryfabricimportedrefCanCopy_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef);

static inline NvBool memoryfabricimportedrefCanCopy_DISPATCH(struct MemoryFabricImportedRef *pMemoryFabricImportedRef) {
    return pMemoryFabricImportedRef->__memoryfabricimportedrefCanCopy__(pMemoryFabricImportedRef);
}

NV_STATUS memoryfabricimportedrefCtrlValidate_IMPL(struct MemoryFabricImportedRef *pMemoryFabricImportedRef, NV00FB_CTRL_VALIDATE_PARAMS *pParams);

static inline NV_STATUS memoryfabricimportedrefCtrlValidate_DISPATCH(struct MemoryFabricImportedRef *pMemoryFabricImportedRef, NV00FB_CTRL_VALIDATE_PARAMS *pParams) {
    return pMemoryFabricImportedRef->__memoryfabricimportedrefCtrlValidate__(pMemoryFabricImportedRef, pParams);
}

static inline NvBool memoryfabricimportedrefShareCallback_DISPATCH(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__memoryfabricimportedrefShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memoryfabricimportedrefCheckMemInterUnmap_DISPATCH(struct MemoryFabricImportedRef *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__memoryfabricimportedrefCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS memoryfabricimportedrefControl_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryfabricimportedrefControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefGetMemInterMapParams_DISPATCH(struct MemoryFabricImportedRef *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__memoryfabricimportedrefGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS memoryfabricimportedrefGetMemoryMappingDescriptor_DISPATCH(struct MemoryFabricImportedRef *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__memoryfabricimportedrefGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 memoryfabricimportedrefGetRefCount_DISPATCH(struct MemoryFabricImportedRef *pResource) {
    return pResource->__memoryfabricimportedrefGetRefCount__(pResource);
}

static inline NV_STATUS memoryfabricimportedrefControlFilter_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryfabricimportedrefControlFilter__(pResource, pCallContext, pParams);
}

static inline void memoryfabricimportedrefAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryFabricImportedRef *pResource, RsResourceRef *pReference) {
    pResource->__memoryfabricimportedrefAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS memoryfabricimportedrefControlSerialization_Prologue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryfabricimportedrefControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefControl_Prologue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memoryfabricimportedrefControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefUnmap_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__memoryfabricimportedrefUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool memoryfabricimportedrefIsPartialUnmapSupported_DISPATCH(struct MemoryFabricImportedRef *pResource) {
    return pResource->__memoryfabricimportedrefIsPartialUnmapSupported__(pResource);
}

static inline void memoryfabricimportedrefPreDestruct_DISPATCH(struct MemoryFabricImportedRef *pResource) {
    pResource->__memoryfabricimportedrefPreDestruct__(pResource);
}

static inline NV_STATUS memoryfabricimportedrefMapTo_DISPATCH(struct MemoryFabricImportedRef *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__memoryfabricimportedrefMapTo__(pResource, pParams);
}

static inline NV_STATUS memoryfabricimportedrefIsDuplicate_DISPATCH(struct MemoryFabricImportedRef *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__memoryfabricimportedrefIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void memoryfabricimportedrefControlSerialization_Epilogue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memoryfabricimportedrefControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline void memoryfabricimportedrefControl_Epilogue_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memoryfabricimportedrefControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memoryfabricimportedrefUnmapFrom_DISPATCH(struct MemoryFabricImportedRef *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__memoryfabricimportedrefUnmapFrom__(pResource, pParams);
}

static inline NV_STATUS memoryfabricimportedrefMap_DISPATCH(struct MemoryFabricImportedRef *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__memoryfabricimportedrefMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool memoryfabricimportedrefAccessCallback_DISPATCH(struct MemoryFabricImportedRef *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__memoryfabricimportedrefAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
