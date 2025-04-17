
#ifndef _G_DBGBUFFER_NVOC_H_
#define _G_DBGBUFFER_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_dbgbuffer_nvoc.h"

#ifndef _DBGBUFFER_H_
#define _DBGBUFFER_H_

#include "core/core.h"
#include "gpu/gpu_resource.h"
#include "class/cl00db.h"
#include "diagnostics/nv_debug_dump.h"

// ****************************************************************************
//                          Type definitions
// ****************************************************************************


/*!
 * RM internal class representing NV40_DEBUG_BUFFER
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_DBGBUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__DebugBufferApi;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__DebugBufferApi;


struct DebugBufferApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DebugBufferApi *__nvoc_metadata_ptr;
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
    struct DebugBufferApi *__nvoc_pbase_DebugBufferApi;    // dbgbuf

    // Data members
    MEMORY_DESCRIPTOR *pMemDesc;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__DebugBufferApi {
    NV_STATUS (*__dbgbufMap__)(struct DebugBufferApi * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__dbgbufUnmap__)(struct DebugBufferApi * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual override (res) base (gpures)
    NV_STATUS (*__dbgbufGetMapAddrSpace__)(struct DebugBufferApi * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__dbgbufGetMemoryMappingDescriptor__)(struct DebugBufferApi * /*this*/, MEMORY_DESCRIPTOR **);  // virtual override (rmres) base (gpures)
    NV_STATUS (*__dbgbufControl__)(struct DebugBufferApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__dbgbufShareCallback__)(struct DebugBufferApi * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dbgbufGetRegBaseOffsetAndSize__)(struct DebugBufferApi * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__dbgbufInternalControlForward__)(struct DebugBufferApi * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__dbgbufGetInternalObjectHandle__)(struct DebugBufferApi * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__dbgbufAccessCallback__)(struct DebugBufferApi * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dbgbufGetMemInterMapParams__)(struct DebugBufferApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dbgbufCheckMemInterUnmap__)(struct DebugBufferApi * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dbgbufControlSerialization_Prologue__)(struct DebugBufferApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__dbgbufControlSerialization_Epilogue__)(struct DebugBufferApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__dbgbufControl_Prologue__)(struct DebugBufferApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__dbgbufControl_Epilogue__)(struct DebugBufferApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__dbgbufCanCopy__)(struct DebugBufferApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dbgbufIsDuplicate__)(struct DebugBufferApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__dbgbufPreDestruct__)(struct DebugBufferApi * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dbgbufControlFilter__)(struct DebugBufferApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__dbgbufIsPartialUnmapSupported__)(struct DebugBufferApi * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__dbgbufMapTo__)(struct DebugBufferApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__dbgbufUnmapFrom__)(struct DebugBufferApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__dbgbufGetRefCount__)(struct DebugBufferApi * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__dbgbufAddAdditionalDependants__)(struct RsClient *, struct DebugBufferApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__DebugBufferApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__DebugBufferApi vtable;
};

#ifndef __NVOC_CLASS_DebugBufferApi_TYPEDEF__
#define __NVOC_CLASS_DebugBufferApi_TYPEDEF__
typedef struct DebugBufferApi DebugBufferApi;
#endif /* __NVOC_CLASS_DebugBufferApi_TYPEDEF__ */

#ifndef __nvoc_class_id_DebugBufferApi
#define __nvoc_class_id_DebugBufferApi 0x5e7a1b
#endif /* __nvoc_class_id_DebugBufferApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DebugBufferApi;

#define __staticCast_DebugBufferApi(pThis) \
    ((pThis)->__nvoc_pbase_DebugBufferApi)

#ifdef __nvoc_dbgbuffer_h_disabled
#define __dynamicCast_DebugBufferApi(pThis) ((DebugBufferApi*) NULL)
#else //__nvoc_dbgbuffer_h_disabled
#define __dynamicCast_DebugBufferApi(pThis) \
    ((DebugBufferApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DebugBufferApi)))
#endif //__nvoc_dbgbuffer_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DebugBufferApi(DebugBufferApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DebugBufferApi(DebugBufferApi**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_DebugBufferApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_DebugBufferApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define dbgbufMap_FNPTR(pDebugBufferApi) pDebugBufferApi->__nvoc_metadata_ptr->vtable.__dbgbufMap__
#define dbgbufMap(pDebugBufferApi, pCallContext, pParams, pCpuMapping) dbgbufMap_DISPATCH(pDebugBufferApi, pCallContext, pParams, pCpuMapping)
#define dbgbufUnmap_FNPTR(pDebugBufferApi) pDebugBufferApi->__nvoc_metadata_ptr->vtable.__dbgbufUnmap__
#define dbgbufUnmap(pDebugBufferApi, pCallContext, pCpuMapping) dbgbufUnmap_DISPATCH(pDebugBufferApi, pCallContext, pCpuMapping)
#define dbgbufGetMapAddrSpace_FNPTR(pDebugBufferApi) pDebugBufferApi->__nvoc_metadata_ptr->vtable.__dbgbufGetMapAddrSpace__
#define dbgbufGetMapAddrSpace(pDebugBufferApi, pCallContext, mapFlags, pAddrSpace) dbgbufGetMapAddrSpace_DISPATCH(pDebugBufferApi, pCallContext, mapFlags, pAddrSpace)
#define dbgbufGetMemoryMappingDescriptor_FNPTR(pDebugBufferApi) pDebugBufferApi->__nvoc_metadata_ptr->vtable.__dbgbufGetMemoryMappingDescriptor__
#define dbgbufGetMemoryMappingDescriptor(pDebugBufferApi, ppMemDesc) dbgbufGetMemoryMappingDescriptor_DISPATCH(pDebugBufferApi, ppMemDesc)
#define dbgbufControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define dbgbufControl(pGpuResource, pCallContext, pParams) dbgbufControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define dbgbufShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define dbgbufShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) dbgbufShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define dbgbufGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define dbgbufGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) dbgbufGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define dbgbufInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define dbgbufInternalControlForward(pGpuResource, command, pParams, size) dbgbufInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define dbgbufGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define dbgbufGetInternalObjectHandle(pGpuResource) dbgbufGetInternalObjectHandle_DISPATCH(pGpuResource)
#define dbgbufAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define dbgbufAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) dbgbufAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define dbgbufGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define dbgbufGetMemInterMapParams(pRmResource, pParams) dbgbufGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define dbgbufCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define dbgbufCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) dbgbufCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define dbgbufControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define dbgbufControlSerialization_Prologue(pResource, pCallContext, pParams) dbgbufControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define dbgbufControlSerialization_Epilogue(pResource, pCallContext, pParams) dbgbufControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define dbgbufControl_Prologue(pResource, pCallContext, pParams) dbgbufControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define dbgbufControl_Epilogue(pResource, pCallContext, pParams) dbgbufControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define dbgbufCanCopy(pResource) dbgbufCanCopy_DISPATCH(pResource)
#define dbgbufIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define dbgbufIsDuplicate(pResource, hMemory, pDuplicate) dbgbufIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define dbgbufPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define dbgbufPreDestruct(pResource) dbgbufPreDestruct_DISPATCH(pResource)
#define dbgbufControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define dbgbufControlFilter(pResource, pCallContext, pParams) dbgbufControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define dbgbufIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define dbgbufIsPartialUnmapSupported(pResource) dbgbufIsPartialUnmapSupported_DISPATCH(pResource)
#define dbgbufMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define dbgbufMapTo(pResource, pParams) dbgbufMapTo_DISPATCH(pResource, pParams)
#define dbgbufUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define dbgbufUnmapFrom(pResource, pParams) dbgbufUnmapFrom_DISPATCH(pResource, pParams)
#define dbgbufGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define dbgbufGetRefCount(pResource) dbgbufGetRefCount_DISPATCH(pResource)
#define dbgbufAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define dbgbufAddAdditionalDependants(pClient, pResource, pReference) dbgbufAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS dbgbufMap_DISPATCH(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pDebugBufferApi->__nvoc_metadata_ptr->vtable.__dbgbufMap__(pDebugBufferApi, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS dbgbufUnmap_DISPATCH(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pDebugBufferApi->__nvoc_metadata_ptr->vtable.__dbgbufUnmap__(pDebugBufferApi, pCallContext, pCpuMapping);
}

static inline NV_STATUS dbgbufGetMapAddrSpace_DISPATCH(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pDebugBufferApi->__nvoc_metadata_ptr->vtable.__dbgbufGetMapAddrSpace__(pDebugBufferApi, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS dbgbufGetMemoryMappingDescriptor_DISPATCH(struct DebugBufferApi *pDebugBufferApi, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pDebugBufferApi->__nvoc_metadata_ptr->vtable.__dbgbufGetMemoryMappingDescriptor__(pDebugBufferApi, ppMemDesc);
}

static inline NV_STATUS dbgbufControl_DISPATCH(struct DebugBufferApi *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__dbgbufControl__(pGpuResource, pCallContext, pParams);
}

static inline NvBool dbgbufShareCallback_DISPATCH(struct DebugBufferApi *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__dbgbufShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS dbgbufGetRegBaseOffsetAndSize_DISPATCH(struct DebugBufferApi *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__dbgbufGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS dbgbufInternalControlForward_DISPATCH(struct DebugBufferApi *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__dbgbufInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle dbgbufGetInternalObjectHandle_DISPATCH(struct DebugBufferApi *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__dbgbufGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool dbgbufAccessCallback_DISPATCH(struct DebugBufferApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS dbgbufGetMemInterMapParams_DISPATCH(struct DebugBufferApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__dbgbufGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS dbgbufCheckMemInterUnmap_DISPATCH(struct DebugBufferApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__dbgbufCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS dbgbufControlSerialization_Prologue_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void dbgbufControlSerialization_Epilogue_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__dbgbufControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS dbgbufControl_Prologue_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void dbgbufControl_Epilogue_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__dbgbufControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool dbgbufCanCopy_DISPATCH(struct DebugBufferApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufCanCopy__(pResource);
}

static inline NV_STATUS dbgbufIsDuplicate_DISPATCH(struct DebugBufferApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void dbgbufPreDestruct_DISPATCH(struct DebugBufferApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__dbgbufPreDestruct__(pResource);
}

static inline NV_STATUS dbgbufControlFilter_DISPATCH(struct DebugBufferApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool dbgbufIsPartialUnmapSupported_DISPATCH(struct DebugBufferApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS dbgbufMapTo_DISPATCH(struct DebugBufferApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufMapTo__(pResource, pParams);
}

static inline NV_STATUS dbgbufUnmapFrom_DISPATCH(struct DebugBufferApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufUnmapFrom__(pResource, pParams);
}

static inline NvU32 dbgbufGetRefCount_DISPATCH(struct DebugBufferApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__dbgbufGetRefCount__(pResource);
}

static inline void dbgbufAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct DebugBufferApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__dbgbufAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS dbgbufMap_IMPL(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);

NV_STATUS dbgbufUnmap_IMPL(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);

NV_STATUS dbgbufGetMapAddrSpace_IMPL(struct DebugBufferApi *pDebugBufferApi, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS dbgbufGetMemoryMappingDescriptor_IMPL(struct DebugBufferApi *pDebugBufferApi, MEMORY_DESCRIPTOR **ppMemDesc);

NV_STATUS dbgbufConstruct_IMPL(struct DebugBufferApi *arg_pDebugBufferApi, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_dbgbufConstruct(arg_pDebugBufferApi, arg_pCallContext, arg_pParams) dbgbufConstruct_IMPL(arg_pDebugBufferApi, arg_pCallContext, arg_pParams)
void dbgbufDestruct_IMPL(struct DebugBufferApi *pDebugBufferApi);

#define __nvoc_dbgbufDestruct(pDebugBufferApi) dbgbufDestruct_IMPL(pDebugBufferApi)
#undef PRIVATE_FIELD


#endif // _DBGBUFFER_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_DBGBUFFER_NVOC_H_
