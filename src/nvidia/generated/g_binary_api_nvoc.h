
#ifndef _G_BINARY_API_NVOC_H_
#define _G_BINARY_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_binary_api_nvoc.h"

#ifndef BINARY_API_H
#define BINARY_API_H

#include "core/core.h"
#include "rmapi/resource.h"
#include "gpu/gpu_resource.h"
#include "resserv/rs_resource.h"
#include "rmapi/control.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_BINARY_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__BinaryApi;
struct NVOC_METADATA__GpuResource;


struct BinaryApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__BinaryApi *__nvoc_metadata_ptr;
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
    struct BinaryApi *__nvoc_pbase_BinaryApi;    // binapi
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__BinaryApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_BinaryApi
#define __nvoc_class_id_BinaryApi 0xb7a47cu
typedef struct BinaryApi BinaryApi;
#endif /* __nvoc_class_id_BinaryApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApi;

#define __staticCast_BinaryApi(pThis) \
    ((pThis)->__nvoc_pbase_BinaryApi)

#ifdef __nvoc_binary_api_h_disabled
#define __dynamicCast_BinaryApi(pThis) ((BinaryApi*) NULL)
#else //__nvoc_binary_api_h_disabled
#define __dynamicCast_BinaryApi(pThis) \
    ((BinaryApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(BinaryApi)))
#endif //__nvoc_binary_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_BinaryApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_BinaryApi(BinaryApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_BinaryApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_BinaryApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS binapiConstruct_IMPL(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_binapiConstruct(pResource, pCallContext, pParams) binapiConstruct_IMPL(pResource, pCallContext, pParams)


// Wrapper macros for halified functions
#define binapiControl_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define binapiControl(pResource, pCallContext, pParams) binapiControl_DISPATCH(pResource, pCallContext, pParams)
#define binapiMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define binapiMap(pGpuResource, pCallContext, pParams, pCpuMapping) binapiMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define binapiUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define binapiUnmap(pGpuResource, pCallContext, pCpuMapping) binapiUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define binapiShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define binapiShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) binapiShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define binapiGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define binapiGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) binapiGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define binapiGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define binapiGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) binapiGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define binapiInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define binapiInternalControlForward(pGpuResource, command, pParams, size) binapiInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define binapiGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define binapiGetInternalObjectHandle(pGpuResource) binapiGetInternalObjectHandle_DISPATCH(pGpuResource)
#define binapiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define binapiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) binapiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define binapiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define binapiGetMemInterMapParams(pRmResource, pParams) binapiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define binapiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define binapiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) binapiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define binapiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define binapiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) binapiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define binapiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define binapiControlSerialization_Prologue(pResource, pCallContext, pParams) binapiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define binapiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define binapiControlSerialization_Epilogue(pResource, pCallContext, pParams) binapiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define binapiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define binapiControl_Prologue(pResource, pCallContext, pParams) binapiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define binapiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define binapiControl_Epilogue(pResource, pCallContext, pParams) binapiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define binapiCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define binapiCanCopy(pResource) binapiCanCopy_DISPATCH(pResource)
#define binapiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define binapiIsDuplicate(pResource, hMemory, pDuplicate) binapiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define binapiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define binapiPreDestruct(pResource) binapiPreDestruct_DISPATCH(pResource)
#define binapiControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define binapiControlFilter(pResource, pCallContext, pParams) binapiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define binapiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define binapiIsPartialUnmapSupported(pResource) binapiIsPartialUnmapSupported_DISPATCH(pResource)
#define binapiMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define binapiMapTo(pResource, pParams) binapiMapTo_DISPATCH(pResource, pParams)
#define binapiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define binapiUnmapFrom(pResource, pParams) binapiUnmapFrom_DISPATCH(pResource, pParams)
#define binapiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define binapiGetRefCount(pResource) binapiGetRefCount_DISPATCH(pResource)
#define binapiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define binapiAddAdditionalDependants(pClient, pResource, pReference) binapiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS binapiControl_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS binapiMap_DISPATCH(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS binapiUnmap_DISPATCH(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool binapiShareCallback_DISPATCH(struct BinaryApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS binapiGetRegBaseOffsetAndSize_DISPATCH(struct BinaryApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS binapiGetMapAddrSpace_DISPATCH(struct BinaryApi *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS binapiInternalControlForward_DISPATCH(struct BinaryApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle binapiGetInternalObjectHandle_DISPATCH(struct BinaryApi *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool binapiAccessCallback_DISPATCH(struct BinaryApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS binapiGetMemInterMapParams_DISPATCH(struct BinaryApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS binapiCheckMemInterUnmap_DISPATCH(struct BinaryApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS binapiGetMemoryMappingDescriptor_DISPATCH(struct BinaryApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS binapiControlSerialization_Prologue_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void binapiControlSerialization_Epilogue_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS binapiControl_Prologue_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void binapiControl_Epilogue_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool binapiCanCopy_DISPATCH(struct BinaryApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS binapiIsDuplicate_DISPATCH(struct BinaryApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void binapiPreDestruct_DISPATCH(struct BinaryApi *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS binapiControlFilter_DISPATCH(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool binapiIsPartialUnmapSupported_DISPATCH(struct BinaryApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS binapiMapTo_DISPATCH(struct BinaryApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS binapiUnmapFrom_DISPATCH(struct BinaryApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 binapiGetRefCount_DISPATCH(struct BinaryApi *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void binapiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct BinaryApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NV_STATUS binapiControl_IMPL(struct BinaryApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_BINARY_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__BinaryApiPrivileged;
struct NVOC_METADATA__BinaryApi;


struct BinaryApiPrivileged {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__BinaryApiPrivileged *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct BinaryApi __nvoc_base_BinaryApi;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct RsResource *__nvoc_pbase_RsResource;    // res super^4
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^4
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^3
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super^2
    struct BinaryApi *__nvoc_pbase_BinaryApi;    // binapi super
    struct BinaryApiPrivileged *__nvoc_pbase_BinaryApiPrivileged;    // binapipriv
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__BinaryApiPrivileged {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__BinaryApi metadata__BinaryApi;
};

#ifndef __nvoc_class_id_BinaryApiPrivileged
#define __nvoc_class_id_BinaryApiPrivileged 0x1c0579u
typedef struct BinaryApiPrivileged BinaryApiPrivileged;
#endif /* __nvoc_class_id_BinaryApiPrivileged */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_BinaryApiPrivileged;

#define __staticCast_BinaryApiPrivileged(pThis) \
    ((pThis)->__nvoc_pbase_BinaryApiPrivileged)

#ifdef __nvoc_binary_api_h_disabled
#define __dynamicCast_BinaryApiPrivileged(pThis) ((BinaryApiPrivileged*) NULL)
#else //__nvoc_binary_api_h_disabled
#define __dynamicCast_BinaryApiPrivileged(pThis) \
    ((BinaryApiPrivileged*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(BinaryApiPrivileged)))
#endif //__nvoc_binary_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_BinaryApiPrivileged(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_BinaryApiPrivileged(BinaryApiPrivileged**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_BinaryApiPrivileged(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_BinaryApiPrivileged((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS binapiprivConstruct_IMPL(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_binapiprivConstruct(pResource, pCallContext, pParams) binapiprivConstruct_IMPL(pResource, pCallContext, pParams)


// Wrapper macros for halified functions
#define binapiprivControl_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define binapiprivControl(pResource, pCallContext, pParams) binapiprivControl_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define binapiprivMap(pGpuResource, pCallContext, pParams, pCpuMapping) binapiprivMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define binapiprivUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define binapiprivUnmap(pGpuResource, pCallContext, pCpuMapping) binapiprivUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define binapiprivShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define binapiprivShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) binapiprivShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define binapiprivGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define binapiprivGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) binapiprivGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define binapiprivGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define binapiprivGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) binapiprivGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define binapiprivInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define binapiprivInternalControlForward(pGpuResource, command, pParams, size) binapiprivInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define binapiprivGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define binapiprivGetInternalObjectHandle(pGpuResource) binapiprivGetInternalObjectHandle_DISPATCH(pGpuResource)
#define binapiprivAccessCallback_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define binapiprivAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) binapiprivAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define binapiprivGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define binapiprivGetMemInterMapParams(pRmResource, pParams) binapiprivGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define binapiprivCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define binapiprivCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) binapiprivCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define binapiprivGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define binapiprivGetMemoryMappingDescriptor(pRmResource, ppMemDesc) binapiprivGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define binapiprivControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define binapiprivControlSerialization_Prologue(pResource, pCallContext, pParams) binapiprivControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define binapiprivControlSerialization_Epilogue(pResource, pCallContext, pParams) binapiprivControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define binapiprivControl_Prologue(pResource, pCallContext, pParams) binapiprivControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define binapiprivControl_Epilogue(pResource, pCallContext, pParams) binapiprivControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivCanCopy_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define binapiprivCanCopy(pResource) binapiprivCanCopy_DISPATCH(pResource)
#define binapiprivIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define binapiprivIsDuplicate(pResource, hMemory, pDuplicate) binapiprivIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define binapiprivPreDestruct_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define binapiprivPreDestruct(pResource) binapiprivPreDestruct_DISPATCH(pResource)
#define binapiprivControlFilter_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define binapiprivControlFilter(pResource, pCallContext, pParams) binapiprivControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define binapiprivIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define binapiprivIsPartialUnmapSupported(pResource) binapiprivIsPartialUnmapSupported_DISPATCH(pResource)
#define binapiprivMapTo_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define binapiprivMapTo(pResource, pParams) binapiprivMapTo_DISPATCH(pResource, pParams)
#define binapiprivUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define binapiprivUnmapFrom(pResource, pParams) binapiprivUnmapFrom_DISPATCH(pResource, pParams)
#define binapiprivGetRefCount_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define binapiprivGetRefCount(pResource) binapiprivGetRefCount_DISPATCH(pResource)
#define binapiprivAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define binapiprivAddAdditionalDependants(pClient, pResource, pReference) binapiprivAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS binapiprivControl_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS binapiprivMap_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS binapiprivUnmap_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool binapiprivShareCallback_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS binapiprivGetRegBaseOffsetAndSize_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS binapiprivGetMapAddrSpace_DISPATCH(struct BinaryApiPrivileged *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS binapiprivInternalControlForward_DISPATCH(struct BinaryApiPrivileged *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle binapiprivGetInternalObjectHandle_DISPATCH(struct BinaryApiPrivileged *pGpuResource) {
    return pGpuResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool binapiprivAccessCallback_DISPATCH(struct BinaryApiPrivileged *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS binapiprivGetMemInterMapParams_DISPATCH(struct BinaryApiPrivileged *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS binapiprivCheckMemInterUnmap_DISPATCH(struct BinaryApiPrivileged *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS binapiprivGetMemoryMappingDescriptor_DISPATCH(struct BinaryApiPrivileged *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS binapiprivControlSerialization_Prologue_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void binapiprivControlSerialization_Epilogue_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS binapiprivControl_Prologue_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void binapiprivControl_Epilogue_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool binapiprivCanCopy_DISPATCH(struct BinaryApiPrivileged *pResource) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS binapiprivIsDuplicate_DISPATCH(struct BinaryApiPrivileged *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void binapiprivPreDestruct_DISPATCH(struct BinaryApiPrivileged *pResource) {
    pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS binapiprivControlFilter_DISPATCH(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool binapiprivIsPartialUnmapSupported_DISPATCH(struct BinaryApiPrivileged *pResource) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS binapiprivMapTo_DISPATCH(struct BinaryApiPrivileged *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS binapiprivUnmapFrom_DISPATCH(struct BinaryApiPrivileged *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 binapiprivGetRefCount_DISPATCH(struct BinaryApiPrivileged *pResource) {
    return pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void binapiprivAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct BinaryApiPrivileged *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_BinaryApi.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
NV_STATUS binapiprivControl_IMPL(struct BinaryApiPrivileged *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_BINARY_API_NVOC_H_
