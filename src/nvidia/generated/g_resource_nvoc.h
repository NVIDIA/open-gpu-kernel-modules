
#ifndef _G_RESOURCE_NVOC_H_
#define _G_RESOURCE_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_resource_nvoc.h"

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include "core/core.h"
#include "resserv/rs_resource.h"
#include "rmapi/control.h"

/* Forward declarations */
struct MEMORY_DESCRIPTOR;
struct OBJVASPACE;

struct RMRES_MEM_INTER_MAP_PARAMS
{
    /// [in]
    OBJGPU                   *pGpu;
    RsResourceRef            *pMemoryRef;
    NvBool                    bSubdeviceHandleProvided;

    /// [out]
    OBJGPU                   *pSrcGpu;
    struct MEMORY_DESCRIPTOR *pSrcMemDesc;
    NvHandle                  hMemoryDevice;
    // This flag will be set when this is FLA mapping
    NvBool                    bFlaMapping;
};

struct RS_RES_MAP_TO_PARAMS
{
    OBJGPU    *pGpu;                       ///< [in]
    OBJGPU    *pSrcGpu;                    ///< [in]
    struct MEMORY_DESCRIPTOR *pSrcMemDesc; ///< [in]
    struct MEMORY_DESCRIPTOR **ppMemDesc;  ///< [out]
    RsResourceRef *pMemoryRef;             ///< [in]
    NvHandle   hBroadcastDevice;           ///< [in]
    NvHandle   hMemoryDevice;              ///< [in]
    NvU32      gpuMask;                    ///< [in]
    NvU64      offset;                     ///< [in]
    NvU64      length;                     ///< [in]
    NvU32      flags;                      ///< [in]
    NvU64     *pDmaOffset;                 ///< [inout]
    NvBool     bSubdeviceHandleProvided;   ///< [in]
    NvBool     bFlaMapping;                ///< [in]
};

struct RS_RES_UNMAP_FROM_PARAMS
{
    OBJGPU    *pGpu;                      ///< [in]
    NvHandle   hMemory;                   ///< [in]
    NvHandle   hBroadcastDevice;          ///< [in]
    NvU32      gpuMask;                   ///< [in]
    NvU32      flags;                     ///< [in]
    NvU64      dmaOffset;                 ///< [in]
    NvU64      size;                      ///< [in]
    struct MEMORY_DESCRIPTOR *pMemDesc;   ///< [in]
    NvBool     bSubdeviceHandleProvided;  ///< [in]
};

struct RS_INTER_MAP_PRIVATE
{
    OBJGPU    *pGpu;
    OBJGPU    *pSrcGpu;
    struct MEMORY_DESCRIPTOR *pSrcMemDesc;
    NvHandle   hBroadcastDevice;
    NvHandle   hMemoryDevice;
    NvU32      gpuMask;
    NvBool     bSubdeviceHandleProvided;
    NvBool     bFlaMapping;
};

struct RS_INTER_UNMAP_PRIVATE
{
    OBJGPU    *pGpu;
    NvHandle   hBroadcastDevice;
    NvU32      gpuMask;
    NvBool     bSubdeviceHandleProvided;
    NvBool     bcState;
    NvBool     bAllocated;                ///< This struct has been allocated and must be freed
};

struct RS_CPU_MAPPING_PRIVATE
{
    NvU64 gpuAddress;
    NvU64 gpuMapLength;
    OBJGPU *pGpu;
    NvP64 pPriv;
    NvU32 protect;
    NvBool bKernel;
};

typedef struct RMRES_MEM_INTER_MAP_PARAMS RMRES_MEM_INTER_MAP_PARAMS;

/*!
 * All RsResource subclasses in RM must inherit from this class
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RESOURCE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct RmResourceCommon {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Ancestor object pointers for `staticCast` feature
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn
};

#ifndef __NVOC_CLASS_RmResourceCommon_TYPEDEF__
#define __NVOC_CLASS_RmResourceCommon_TYPEDEF__
typedef struct RmResourceCommon RmResourceCommon;
#endif /* __NVOC_CLASS_RmResourceCommon_TYPEDEF__ */

#ifndef __nvoc_class_id_RmResourceCommon
#define __nvoc_class_id_RmResourceCommon 0x8ef259
#endif /* __nvoc_class_id_RmResourceCommon */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

#define __staticCast_RmResourceCommon(pThis) \
    ((pThis)->__nvoc_pbase_RmResourceCommon)

#ifdef __nvoc_resource_h_disabled
#define __dynamicCast_RmResourceCommon(pThis) ((RmResourceCommon*)NULL)
#else //__nvoc_resource_h_disabled
#define __dynamicCast_RmResourceCommon(pThis) \
    ((RmResourceCommon*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RmResourceCommon)))
#endif //__nvoc_resource_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RmResourceCommon(RmResourceCommon**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RmResourceCommon(RmResourceCommon**, Dynamic*, NvU32);
#define __objCreate_RmResourceCommon(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_RmResourceCommon((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS rmrescmnConstruct_IMPL(struct RmResourceCommon *arg_pResourceCommmon);

#define __nvoc_rmrescmnConstruct(arg_pResourceCommmon) rmrescmnConstruct_IMPL(arg_pResourceCommmon)
#undef PRIVATE_FIELD


/*!
 * Utility base class for all RsResource subclasses in by RM. Doesn't have to be
 * used but if it isn't used RmResourceCommon must be inherited manually
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RESOURCE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct RmResource {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct RsResource __nvoc_base_RsResource;
    struct RmResourceCommon __nvoc_base_RmResourceCommon;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct RsResource *__nvoc_pbase_RsResource;    // res super
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super
    struct RmResource *__nvoc_pbase_RmResource;    // rmres

    // Vtable with 21 per-object function pointers
    NvBool (*__rmresAccessCallback__)(struct RmResource * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual override (res) base (res)
    NvBool (*__rmresShareCallback__)(struct RmResource * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual override (res) base (res)
    NV_STATUS (*__rmresGetMemInterMapParams__)(struct RmResource * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual
    NV_STATUS (*__rmresCheckMemInterUnmap__)(struct RmResource * /*this*/, NvBool);  // virtual
    NV_STATUS (*__rmresGetMemoryMappingDescriptor__)(struct RmResource * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual
    NV_STATUS (*__rmresControlSerialization_Prologue__)(struct RmResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (res)
    void (*__rmresControlSerialization_Epilogue__)(struct RmResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (res)
    NV_STATUS (*__rmresControl_Prologue__)(struct RmResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (res)
    void (*__rmresControl_Epilogue__)(struct RmResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (res)
    NvBool (*__rmresCanCopy__)(struct RmResource * /*this*/);  // virtual inherited (res) base (res)
    NV_STATUS (*__rmresIsDuplicate__)(struct RmResource * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (res)
    void (*__rmresPreDestruct__)(struct RmResource * /*this*/);  // virtual inherited (res) base (res)
    NV_STATUS (*__rmresControl__)(struct RmResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (res)
    NV_STATUS (*__rmresControlFilter__)(struct RmResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (res)
    NV_STATUS (*__rmresMap__)(struct RmResource * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (res) base (res)
    NV_STATUS (*__rmresUnmap__)(struct RmResource * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (res) base (res)
    NvBool (*__rmresIsPartialUnmapSupported__)(struct RmResource * /*this*/);  // inline virtual inherited (res) base (res) body
    NV_STATUS (*__rmresMapTo__)(struct RmResource * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (res)
    NV_STATUS (*__rmresUnmapFrom__)(struct RmResource * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (res)
    NvU32 (*__rmresGetRefCount__)(struct RmResource * /*this*/);  // virtual inherited (res) base (res)
    void (*__rmresAddAdditionalDependants__)(struct RsClient *, struct RmResource * /*this*/, RsResourceRef *);  // virtual inherited (res) base (res)

    // Data members
    NvU32 rpcGpuInstance;
    NvBool bRpcFree;
};

#ifndef __NVOC_CLASS_RmResource_TYPEDEF__
#define __NVOC_CLASS_RmResource_TYPEDEF__
typedef struct RmResource RmResource;
#endif /* __NVOC_CLASS_RmResource_TYPEDEF__ */

#ifndef __nvoc_class_id_RmResource
#define __nvoc_class_id_RmResource 0x03610d
#endif /* __nvoc_class_id_RmResource */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

#define __staticCast_RmResource(pThis) \
    ((pThis)->__nvoc_pbase_RmResource)

#ifdef __nvoc_resource_h_disabled
#define __dynamicCast_RmResource(pThis) ((RmResource*)NULL)
#else //__nvoc_resource_h_disabled
#define __dynamicCast_RmResource(pThis) \
    ((RmResource*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RmResource)))
#endif //__nvoc_resource_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RmResource(RmResource**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RmResource(RmResource**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_RmResource(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_RmResource((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define rmresAccessCallback_FNPTR(pResource) pResource->__rmresAccessCallback__
#define rmresAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) rmresAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define rmresShareCallback_FNPTR(pResource) pResource->__rmresShareCallback__
#define rmresShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) rmresShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define rmresGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__rmresGetMemInterMapParams__
#define rmresGetMemInterMapParams(pRmResource, pParams) rmresGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define rmresCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__rmresCheckMemInterUnmap__
#define rmresCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) rmresCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define rmresGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__rmresGetMemoryMappingDescriptor__
#define rmresGetMemoryMappingDescriptor(pRmResource, ppMemDesc) rmresGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define rmresControlSerialization_Prologue_FNPTR(pResource) pResource->__rmresControlSerialization_Prologue__
#define rmresControlSerialization_Prologue(pResource, pCallContext, pParams) rmresControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define rmresControlSerialization_Epilogue_FNPTR(pResource) pResource->__rmresControlSerialization_Epilogue__
#define rmresControlSerialization_Epilogue(pResource, pCallContext, pParams) rmresControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define rmresControl_Prologue_FNPTR(pResource) pResource->__rmresControl_Prologue__
#define rmresControl_Prologue(pResource, pCallContext, pParams) rmresControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define rmresControl_Epilogue_FNPTR(pResource) pResource->__rmresControl_Epilogue__
#define rmresControl_Epilogue(pResource, pCallContext, pParams) rmresControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define rmresCanCopy_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resCanCopy__
#define rmresCanCopy(pResource) rmresCanCopy_DISPATCH(pResource)
#define rmresIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resIsDuplicate__
#define rmresIsDuplicate(pResource, hMemory, pDuplicate) rmresIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define rmresPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resPreDestruct__
#define rmresPreDestruct(pResource) rmresPreDestruct_DISPATCH(pResource)
#define rmresControl_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resControl__
#define rmresControl(pResource, pCallContext, pParams) rmresControl_DISPATCH(pResource, pCallContext, pParams)
#define rmresControlFilter_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resControlFilter__
#define rmresControlFilter(pResource, pCallContext, pParams) rmresControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define rmresMap_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resMap__
#define rmresMap(pResource, pCallContext, pParams, pCpuMapping) rmresMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define rmresUnmap_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resUnmap__
#define rmresUnmap(pResource, pCallContext, pCpuMapping) rmresUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define rmresIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resIsPartialUnmapSupported__
#define rmresIsPartialUnmapSupported(pResource) rmresIsPartialUnmapSupported_DISPATCH(pResource)
#define rmresMapTo_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resMapTo__
#define rmresMapTo(pResource, pParams) rmresMapTo_DISPATCH(pResource, pParams)
#define rmresUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resUnmapFrom__
#define rmresUnmapFrom(pResource, pParams) rmresUnmapFrom_DISPATCH(pResource, pParams)
#define rmresGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resGetRefCount__
#define rmresGetRefCount(pResource) rmresGetRefCount_DISPATCH(pResource)
#define rmresAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RsResource.__resAddAdditionalDependants__
#define rmresAddAdditionalDependants(pClient, pResource, pReference) rmresAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool rmresAccessCallback_DISPATCH(struct RmResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__rmresAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool rmresShareCallback_DISPATCH(struct RmResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__rmresShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS rmresGetMemInterMapParams_DISPATCH(struct RmResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__rmresGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS rmresCheckMemInterUnmap_DISPATCH(struct RmResource *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__rmresCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS rmresGetMemoryMappingDescriptor_DISPATCH(struct RmResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__rmresGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS rmresControlSerialization_Prologue_DISPATCH(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__rmresControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void rmresControlSerialization_Epilogue_DISPATCH(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__rmresControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS rmresControl_Prologue_DISPATCH(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__rmresControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void rmresControl_Epilogue_DISPATCH(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__rmresControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool rmresCanCopy_DISPATCH(struct RmResource *pResource) {
    return pResource->__rmresCanCopy__(pResource);
}

static inline NV_STATUS rmresIsDuplicate_DISPATCH(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__rmresIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void rmresPreDestruct_DISPATCH(struct RmResource *pResource) {
    pResource->__rmresPreDestruct__(pResource);
}

static inline NV_STATUS rmresControl_DISPATCH(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__rmresControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS rmresControlFilter_DISPATCH(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__rmresControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS rmresMap_DISPATCH(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__rmresMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS rmresUnmap_DISPATCH(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__rmresUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool rmresIsPartialUnmapSupported_DISPATCH(struct RmResource *pResource) {
    return pResource->__rmresIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS rmresMapTo_DISPATCH(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__rmresMapTo__(pResource, pParams);
}

static inline NV_STATUS rmresUnmapFrom_DISPATCH(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__rmresUnmapFrom__(pResource, pParams);
}

static inline NvU32 rmresGetRefCount_DISPATCH(struct RmResource *pResource) {
    return pResource->__rmresGetRefCount__(pResource);
}

static inline void rmresAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference) {
    pResource->__rmresAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool rmresAccessCallback_IMPL(struct RmResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);

NvBool rmresShareCallback_IMPL(struct RmResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);

NV_STATUS rmresGetMemInterMapParams_IMPL(struct RmResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);

NV_STATUS rmresCheckMemInterUnmap_IMPL(struct RmResource *pRmResource, NvBool bSubdeviceHandleProvided);

NV_STATUS rmresGetMemoryMappingDescriptor_IMPL(struct RmResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);

NV_STATUS rmresControlSerialization_Prologue_IMPL(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

void rmresControlSerialization_Epilogue_IMPL(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS rmresControl_Prologue_IMPL(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

void rmresControl_Epilogue_IMPL(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS rmresConstruct_IMPL(struct RmResource *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_rmresConstruct(arg_pResource, arg_pCallContext, arg_pParams) rmresConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // _RESOURCE_H_


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_RESOURCE_NVOC_H_
