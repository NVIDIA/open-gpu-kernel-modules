
#ifndef _G_GPU_RESOURCE_NVOC_H_
#define _G_GPU_RESOURCE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_gpu_resource_nvoc.h"

#ifndef _GPURESOURCE_H_
#define _GPURESOURCE_H_

#include "core/core.h"
#include "gpu/mem_mgr/mem_desc.h"

#include "rmapi/resource.h"


struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */



struct Subdevice;

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */



#define GPU_RES_GET_GPU(pRes) staticCastNoPtrCheck((pRes), GpuResource)->pGpu
#define GPU_RES_GET_GPUGRP(pRes) staticCastNoPtrCheck((pRes), GpuResource)->pGpuGrp
#define GPU_RES_GET_DEVICE(pRes) staticCastNoPtrCheck((pRes), GpuResource)->pDevice
#define GPU_RES_GET_SUBDEVICE(pRes) staticCastNoPtrCheck((pRes), GpuResource)->pSubdevice

#define GPU_RES_SET_THREAD_BC_STATE(pRes) PORT_UNREFERENCED_VARIABLE(pRes)

/*!
 * Abstract base class for common CPU mapping operations
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_GPU_RESOURCE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__RmResource;
struct NVOC_VTABLE__GpuResource;


struct GpuResource {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpuResource *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures

    // Data members
    struct OBJGPUGRP *pGpuGrp;
    struct OBJGPU *pGpu;
    struct Device *pDevice;
    struct Subdevice *pSubdevice;
    NvBool bBcResource;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__GpuResource {
    NV_STATUS (*__gpuresControl__)(struct GpuResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual override (res) base (rmres)
    NV_STATUS (*__gpuresMap__)(struct GpuResource * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual override (res) base (rmres)
    NV_STATUS (*__gpuresUnmap__)(struct GpuResource * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual override (res) base (rmres)
    NvBool (*__gpuresShareCallback__)(struct GpuResource * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual override (res) base (rmres)
    NV_STATUS (*__gpuresGetRegBaseOffsetAndSize__)(struct GpuResource * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual
    NV_STATUS (*__gpuresGetMapAddrSpace__)(struct GpuResource * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual
    NV_STATUS (*__gpuresInternalControlForward__)(struct GpuResource * /*this*/, NvU32, void *, NvU32);  // virtual
    NvHandle (*__gpuresGetInternalObjectHandle__)(struct GpuResource * /*this*/);  // virtual
    NvBool (*__gpuresAccessCallback__)(struct GpuResource * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpuresGetMemInterMapParams__)(struct GpuResource * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpuresCheckMemInterUnmap__)(struct GpuResource * /*this*/, NvBool);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpuresGetMemoryMappingDescriptor__)(struct GpuResource * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpuresControlSerialization_Prologue__)(struct GpuResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__gpuresControlSerialization_Epilogue__)(struct GpuResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NV_STATUS (*__gpuresControl_Prologue__)(struct GpuResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    void (*__gpuresControl_Epilogue__)(struct GpuResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (rmres)
    NvBool (*__gpuresCanCopy__)(struct GpuResource * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gpuresIsDuplicate__)(struct GpuResource * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (rmres)
    void (*__gpuresPreDestruct__)(struct GpuResource * /*this*/);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gpuresControlFilter__)(struct GpuResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (rmres)
    NvBool (*__gpuresIsPartialUnmapSupported__)(struct GpuResource * /*this*/);  // inline virtual inherited (res) base (rmres) body
    NV_STATUS (*__gpuresMapTo__)(struct GpuResource * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (rmres)
    NV_STATUS (*__gpuresUnmapFrom__)(struct GpuResource * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (rmres)
    NvU32 (*__gpuresGetRefCount__)(struct GpuResource * /*this*/);  // virtual inherited (res) base (rmres)
    void (*__gpuresAddAdditionalDependants__)(struct RsClient *, struct GpuResource * /*this*/, RsResourceRef *);  // virtual inherited (res) base (rmres)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__GpuResource {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
    const struct NVOC_VTABLE__GpuResource vtable;
};

#ifndef __NVOC_CLASS_GpuResource_TYPEDEF__
#define __NVOC_CLASS_GpuResource_TYPEDEF__
typedef struct GpuResource GpuResource;
#endif /* __NVOC_CLASS_GpuResource_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuResource
#define __nvoc_class_id_GpuResource 0x5d5d9f
#endif /* __nvoc_class_id_GpuResource */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

#define __staticCast_GpuResource(pThis) \
    ((pThis)->__nvoc_pbase_GpuResource)

#ifdef __nvoc_gpu_resource_h_disabled
#define __dynamicCast_GpuResource(pThis) ((GpuResource*) NULL)
#else //__nvoc_gpu_resource_h_disabled
#define __dynamicCast_GpuResource(pThis) \
    ((GpuResource*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuResource)))
#endif //__nvoc_gpu_resource_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpuResource(GpuResource**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuResource(GpuResource**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_GpuResource(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GpuResource((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define gpuresControl_FNPTR(pGpuResource) pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresControl__
#define gpuresControl(pGpuResource, pCallContext, pParams) gpuresControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define gpuresMap_FNPTR(pGpuResource) pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresMap__
#define gpuresMap(pGpuResource, pCallContext, pParams, pCpuMapping) gpuresMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define gpuresUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define gpuresUnmap(pGpuResource, pCallContext, pCpuMapping) gpuresUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define gpuresShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define gpuresShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) gpuresShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define gpuresGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define gpuresGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) gpuresGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define gpuresGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define gpuresGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) gpuresGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define gpuresInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define gpuresInternalControlForward(pGpuResource, command, pParams, size) gpuresInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define gpuresGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define gpuresGetInternalObjectHandle(pGpuResource) gpuresGetInternalObjectHandle_DISPATCH(pGpuResource)
#define gpuresAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define gpuresAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gpuresAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define gpuresGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define gpuresGetMemInterMapParams(pRmResource, pParams) gpuresGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define gpuresCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define gpuresCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) gpuresCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define gpuresGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define gpuresGetMemoryMappingDescriptor(pRmResource, ppMemDesc) gpuresGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define gpuresControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define gpuresControlSerialization_Prologue(pResource, pCallContext, pParams) gpuresControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpuresControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define gpuresControlSerialization_Epilogue(pResource, pCallContext, pParams) gpuresControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpuresControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define gpuresControl_Prologue(pResource, pCallContext, pParams) gpuresControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpuresControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define gpuresControl_Epilogue(pResource, pCallContext, pParams) gpuresControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpuresCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define gpuresCanCopy(pResource) gpuresCanCopy_DISPATCH(pResource)
#define gpuresIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define gpuresIsDuplicate(pResource, hMemory, pDuplicate) gpuresIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define gpuresPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define gpuresPreDestruct(pResource) gpuresPreDestruct_DISPATCH(pResource)
#define gpuresControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define gpuresControlFilter(pResource, pCallContext, pParams) gpuresControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gpuresIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define gpuresIsPartialUnmapSupported(pResource) gpuresIsPartialUnmapSupported_DISPATCH(pResource)
#define gpuresMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define gpuresMapTo(pResource, pParams) gpuresMapTo_DISPATCH(pResource, pParams)
#define gpuresUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define gpuresUnmapFrom(pResource, pParams) gpuresUnmapFrom_DISPATCH(pResource, pParams)
#define gpuresGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define gpuresGetRefCount(pResource) gpuresGetRefCount_DISPATCH(pResource)
#define gpuresAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define gpuresAddAdditionalDependants(pClient, pResource, pReference) gpuresAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS gpuresControl_DISPATCH(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS gpuresMap_DISPATCH(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS gpuresUnmap_DISPATCH(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool gpuresShareCallback_DISPATCH(struct GpuResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gpuresGetRegBaseOffsetAndSize_DISPATCH(struct GpuResource *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS gpuresGetMapAddrSpace_DISPATCH(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS gpuresInternalControlForward_DISPATCH(struct GpuResource *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle gpuresGetInternalObjectHandle_DISPATCH(struct GpuResource *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool gpuresAccessCallback_DISPATCH(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS gpuresGetMemInterMapParams_DISPATCH(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gpuresGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS gpuresCheckMemInterUnmap_DISPATCH(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gpuresCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS gpuresGetMemoryMappingDescriptor_DISPATCH(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__gpuresGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS gpuresControlSerialization_Prologue_DISPATCH(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void gpuresControlSerialization_Epilogue_DISPATCH(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gpuresControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpuresControl_Prologue_DISPATCH(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void gpuresControl_Epilogue_DISPATCH(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__gpuresControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool gpuresCanCopy_DISPATCH(struct GpuResource *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresCanCopy__(pResource);
}

static inline NV_STATUS gpuresIsDuplicate_DISPATCH(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void gpuresPreDestruct_DISPATCH(struct GpuResource *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__gpuresPreDestruct__(pResource);
}

static inline NV_STATUS gpuresControlFilter_DISPATCH(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool gpuresIsPartialUnmapSupported_DISPATCH(struct GpuResource *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS gpuresMapTo_DISPATCH(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresMapTo__(pResource, pParams);
}

static inline NV_STATUS gpuresUnmapFrom_DISPATCH(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresUnmapFrom__(pResource, pParams);
}

static inline NvU32 gpuresGetRefCount_DISPATCH(struct GpuResource *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__gpuresGetRefCount__(pResource);
}

static inline void gpuresAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__gpuresAddAdditionalDependants__(pClient, pResource, pReference);
}

NV_STATUS gpuresControl_IMPL(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS gpuresMap_IMPL(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

NV_STATUS gpuresUnmap_IMPL(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

NvBool gpuresShareCallback_IMPL(struct GpuResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);

NV_STATUS gpuresGetRegBaseOffsetAndSize_IMPL(struct GpuResource *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

NV_STATUS gpuresGetMapAddrSpace_IMPL(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

NV_STATUS gpuresInternalControlForward_IMPL(struct GpuResource *pGpuResource, NvU32 command, void *pParams, NvU32 size);

NvHandle gpuresGetInternalObjectHandle_IMPL(struct GpuResource *pGpuResource);

NV_STATUS gpuresConstruct_IMPL(struct GpuResource *arg_pGpuResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_gpuresConstruct(arg_pGpuResource, arg_pCallContext, arg_pParams) gpuresConstruct_IMPL(arg_pGpuResource, arg_pCallContext, arg_pParams)
NV_STATUS gpuresCopyConstruct_IMPL(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

#ifdef __nvoc_gpu_resource_h_disabled
static inline NV_STATUS gpuresCopyConstruct(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("GpuResource was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_resource_h_disabled
#define gpuresCopyConstruct(pGpuResource, pCallContext, pParams) gpuresCopyConstruct_IMPL(pGpuResource, pCallContext, pParams)
#endif //__nvoc_gpu_resource_h_disabled

void gpuresSetGpu_IMPL(struct GpuResource *pGpuResource, struct OBJGPU *pGpu, NvBool bBcResource);

#ifdef __nvoc_gpu_resource_h_disabled
static inline void gpuresSetGpu(struct GpuResource *pGpuResource, struct OBJGPU *pGpu, NvBool bBcResource) {
    NV_ASSERT_FAILED_PRECOMP("GpuResource was disabled!");
}
#else //__nvoc_gpu_resource_h_disabled
#define gpuresSetGpu(pGpuResource, pGpu, bBcResource) gpuresSetGpu_IMPL(pGpuResource, pGpu, bBcResource)
#endif //__nvoc_gpu_resource_h_disabled

void gpuresControlSetup_IMPL(struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, struct GpuResource *pGpuResource);

#ifdef __nvoc_gpu_resource_h_disabled
static inline void gpuresControlSetup(struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, struct GpuResource *pGpuResource) {
    NV_ASSERT_FAILED_PRECOMP("GpuResource was disabled!");
}
#else //__nvoc_gpu_resource_h_disabled
#define gpuresControlSetup(pParams, pGpuResource) gpuresControlSetup_IMPL(pParams, pGpuResource)
#endif //__nvoc_gpu_resource_h_disabled

NV_STATUS gpuresGetByHandle_IMPL(struct RsClient *pClient, NvHandle hResource, struct GpuResource **ppGpuResource);

#define gpuresGetByHandle(pClient, hResource, ppGpuResource) gpuresGetByHandle_IMPL(pClient, hResource, ppGpuResource)
NV_STATUS gpuresGetByDeviceOrSubdeviceHandle_IMPL(struct RsClient *pClient, NvHandle hResource, struct GpuResource **ppGpuResource);

#define gpuresGetByDeviceOrSubdeviceHandle(pClient, hResource, ppGpuResource) gpuresGetByDeviceOrSubdeviceHandle_IMPL(pClient, hResource, ppGpuResource)
#undef PRIVATE_FIELD


#endif // _GPURESOURCE_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_RESOURCE_NVOC_H_
