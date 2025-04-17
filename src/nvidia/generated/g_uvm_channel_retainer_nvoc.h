
#ifndef _G_UVM_CHANNEL_RETAINER_NVOC_H_
#define _G_UVM_CHANNEL_RETAINER_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2020-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_uvm_channel_retainer_nvoc.h"

#ifndef UVM_CHANNEL_RETAINER_H
#define UVM_CHANNEL_RETAINER_H

#include "os/os.h"
#include "resserv/resserv.h"
#include "nvoc/prelude.h"
#include "gpu/gpu_resource.h"
#include "nvos.h"
#include "kernel/gpu/fifo/kernel_channel.h"


struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_UVM_CHANNEL_RETAINER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__UvmChannelRetainer;
struct NVOC_METADATA__GpuResource;
struct NVOC_VTABLE__UvmChannelRetainer;


struct UvmChannelRetainer {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__UvmChannelRetainer *__nvoc_metadata_ptr;
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
    struct UvmChannelRetainer *__nvoc_pbase_UvmChannelRetainer;    // uvmchanrtnr

    // Data members
    MEMORY_DESCRIPTOR *pInstMemDesc;
    NvU32 chId;
    NvU32 runlistId;
};


// Vtable with 25 per-class function pointers
struct NVOC_VTABLE__UvmChannelRetainer {
    NV_STATUS (*__uvmchanrtnrControl__)(struct UvmChannelRetainer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__uvmchanrtnrMap__)(struct UvmChannelRetainer * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__uvmchanrtnrUnmap__)(struct UvmChannelRetainer * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__uvmchanrtnrShareCallback__)(struct UvmChannelRetainer * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__uvmchanrtnrGetRegBaseOffsetAndSize__)(struct UvmChannelRetainer * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__uvmchanrtnrGetMapAddrSpace__)(struct UvmChannelRetainer * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__uvmchanrtnrInternalControlForward__)(struct UvmChannelRetainer * /*this*/, NvU32, void *, NvU32);  // virtual inherited (gpures) base (gpures)
    NvHandle (*__uvmchanrtnrGetInternalObjectHandle__)(struct UvmChannelRetainer * /*this*/);  // virtual inherited (gpures) base (gpures)
    NvBool (*__uvmchanrtnrAccessCallback__)(struct UvmChannelRetainer * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__uvmchanrtnrGetMemInterMapParams__)(struct UvmChannelRetainer * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__uvmchanrtnrCheckMemInterUnmap__)(struct UvmChannelRetainer * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__uvmchanrtnrGetMemoryMappingDescriptor__)(struct UvmChannelRetainer * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__uvmchanrtnrControlSerialization_Prologue__)(struct UvmChannelRetainer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__uvmchanrtnrControlSerialization_Epilogue__)(struct UvmChannelRetainer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__uvmchanrtnrControl_Prologue__)(struct UvmChannelRetainer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__uvmchanrtnrControl_Epilogue__)(struct UvmChannelRetainer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__uvmchanrtnrCanCopy__)(struct UvmChannelRetainer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__uvmchanrtnrIsDuplicate__)(struct UvmChannelRetainer * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__uvmchanrtnrPreDestruct__)(struct UvmChannelRetainer * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__uvmchanrtnrControlFilter__)(struct UvmChannelRetainer * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__uvmchanrtnrIsPartialUnmapSupported__)(struct UvmChannelRetainer * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__uvmchanrtnrMapTo__)(struct UvmChannelRetainer * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__uvmchanrtnrUnmapFrom__)(struct UvmChannelRetainer * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__uvmchanrtnrGetRefCount__)(struct UvmChannelRetainer * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__uvmchanrtnrAddAdditionalDependants__)(struct RsClient *, struct UvmChannelRetainer * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__UvmChannelRetainer {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_VTABLE__UvmChannelRetainer vtable;
};

#ifndef __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__
#define __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__
typedef struct UvmChannelRetainer UvmChannelRetainer;
#endif /* __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__ */

#ifndef __nvoc_class_id_UvmChannelRetainer
#define __nvoc_class_id_UvmChannelRetainer 0xa3f03a
#endif /* __nvoc_class_id_UvmChannelRetainer */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_UvmChannelRetainer;

#define __staticCast_UvmChannelRetainer(pThis) \
    ((pThis)->__nvoc_pbase_UvmChannelRetainer)

#ifdef __nvoc_uvm_channel_retainer_h_disabled
#define __dynamicCast_UvmChannelRetainer(pThis) ((UvmChannelRetainer*) NULL)
#else //__nvoc_uvm_channel_retainer_h_disabled
#define __dynamicCast_UvmChannelRetainer(pThis) \
    ((UvmChannelRetainer*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(UvmChannelRetainer)))
#endif //__nvoc_uvm_channel_retainer_h_disabled

NV_STATUS __nvoc_objCreateDynamic_UvmChannelRetainer(UvmChannelRetainer**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_UvmChannelRetainer(UvmChannelRetainer**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_UvmChannelRetainer(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_UvmChannelRetainer((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define uvmchanrtnrControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define uvmchanrtnrControl(pGpuResource, pCallContext, pParams) uvmchanrtnrControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define uvmchanrtnrMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define uvmchanrtnrMap(pGpuResource, pCallContext, pParams, pCpuMapping) uvmchanrtnrMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define uvmchanrtnrUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define uvmchanrtnrUnmap(pGpuResource, pCallContext, pCpuMapping) uvmchanrtnrUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define uvmchanrtnrShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define uvmchanrtnrShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) uvmchanrtnrShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define uvmchanrtnrGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define uvmchanrtnrGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) uvmchanrtnrGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define uvmchanrtnrGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define uvmchanrtnrGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) uvmchanrtnrGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define uvmchanrtnrInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define uvmchanrtnrInternalControlForward(pGpuResource, command, pParams, size) uvmchanrtnrInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define uvmchanrtnrGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define uvmchanrtnrGetInternalObjectHandle(pGpuResource) uvmchanrtnrGetInternalObjectHandle_DISPATCH(pGpuResource)
#define uvmchanrtnrAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define uvmchanrtnrAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) uvmchanrtnrAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define uvmchanrtnrGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define uvmchanrtnrGetMemInterMapParams(pRmResource, pParams) uvmchanrtnrGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define uvmchanrtnrCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define uvmchanrtnrCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) uvmchanrtnrCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define uvmchanrtnrGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define uvmchanrtnrGetMemoryMappingDescriptor(pRmResource, ppMemDesc) uvmchanrtnrGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define uvmchanrtnrControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define uvmchanrtnrControlSerialization_Prologue(pResource, pCallContext, pParams) uvmchanrtnrControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define uvmchanrtnrControlSerialization_Epilogue(pResource, pCallContext, pParams) uvmchanrtnrControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define uvmchanrtnrControl_Prologue(pResource, pCallContext, pParams) uvmchanrtnrControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define uvmchanrtnrControl_Epilogue(pResource, pCallContext, pParams) uvmchanrtnrControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define uvmchanrtnrCanCopy(pResource) uvmchanrtnrCanCopy_DISPATCH(pResource)
#define uvmchanrtnrIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define uvmchanrtnrIsDuplicate(pResource, hMemory, pDuplicate) uvmchanrtnrIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define uvmchanrtnrPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define uvmchanrtnrPreDestruct(pResource) uvmchanrtnrPreDestruct_DISPATCH(pResource)
#define uvmchanrtnrControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define uvmchanrtnrControlFilter(pResource, pCallContext, pParams) uvmchanrtnrControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define uvmchanrtnrIsPartialUnmapSupported(pResource) uvmchanrtnrIsPartialUnmapSupported_DISPATCH(pResource)
#define uvmchanrtnrMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define uvmchanrtnrMapTo(pResource, pParams) uvmchanrtnrMapTo_DISPATCH(pResource, pParams)
#define uvmchanrtnrUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define uvmchanrtnrUnmapFrom(pResource, pParams) uvmchanrtnrUnmapFrom_DISPATCH(pResource, pParams)
#define uvmchanrtnrGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define uvmchanrtnrGetRefCount(pResource) uvmchanrtnrGetRefCount_DISPATCH(pResource)
#define uvmchanrtnrAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define uvmchanrtnrAddAdditionalDependants(pClient, pResource, pReference) uvmchanrtnrAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NV_STATUS uvmchanrtnrControl_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS uvmchanrtnrMap_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS uvmchanrtnrUnmap_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool uvmchanrtnrShareCallback_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS uvmchanrtnrGetRegBaseOffsetAndSize_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS uvmchanrtnrGetMapAddrSpace_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS uvmchanrtnrInternalControlForward_DISPATCH(struct UvmChannelRetainer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NvHandle uvmchanrtnrGetInternalObjectHandle_DISPATCH(struct UvmChannelRetainer *pGpuResource) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrGetInternalObjectHandle__(pGpuResource);
}

static inline NvBool uvmchanrtnrAccessCallback_DISPATCH(struct UvmChannelRetainer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS uvmchanrtnrGetMemInterMapParams_DISPATCH(struct UvmChannelRetainer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS uvmchanrtnrCheckMemInterUnmap_DISPATCH(struct UvmChannelRetainer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS uvmchanrtnrGetMemoryMappingDescriptor_DISPATCH(struct UvmChannelRetainer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS uvmchanrtnrControlSerialization_Prologue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void uvmchanrtnrControlSerialization_Epilogue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS uvmchanrtnrControl_Prologue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void uvmchanrtnrControl_Epilogue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool uvmchanrtnrCanCopy_DISPATCH(struct UvmChannelRetainer *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrCanCopy__(pResource);
}

static inline NV_STATUS uvmchanrtnrIsDuplicate_DISPATCH(struct UvmChannelRetainer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void uvmchanrtnrPreDestruct_DISPATCH(struct UvmChannelRetainer *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrPreDestruct__(pResource);
}

static inline NV_STATUS uvmchanrtnrControlFilter_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool uvmchanrtnrIsPartialUnmapSupported_DISPATCH(struct UvmChannelRetainer *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS uvmchanrtnrMapTo_DISPATCH(struct UvmChannelRetainer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrMapTo__(pResource, pParams);
}

static inline NV_STATUS uvmchanrtnrUnmapFrom_DISPATCH(struct UvmChannelRetainer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrUnmapFrom__(pResource, pParams);
}

static inline NvU32 uvmchanrtnrGetRefCount_DISPATCH(struct UvmChannelRetainer *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrGetRefCount__(pResource);
}

static inline void uvmchanrtnrAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct UvmChannelRetainer *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__uvmchanrtnrAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool uvmchanrtnrIsAllocationAllowed_IMPL(struct UvmChannelRetainer *pUvmChannelRetainer, CALL_CONTEXT *pCallContext, struct KernelChannel *pKernelChannel);


#ifdef __nvoc_uvm_channel_retainer_h_disabled
static inline NvBool uvmchanrtnrIsAllocationAllowed(struct UvmChannelRetainer *pUvmChannelRetainer, CALL_CONTEXT *pCallContext, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("UvmChannelRetainer was disabled!");
    return NV_FALSE;
}
#else //__nvoc_uvm_channel_retainer_h_disabled
#define uvmchanrtnrIsAllocationAllowed(pUvmChannelRetainer, pCallContext, pKernelChannel) uvmchanrtnrIsAllocationAllowed_IMPL(pUvmChannelRetainer, pCallContext, pKernelChannel)
#endif //__nvoc_uvm_channel_retainer_h_disabled

#define uvmchanrtnrIsAllocationAllowed_HAL(pUvmChannelRetainer, pCallContext, pKernelChannel) uvmchanrtnrIsAllocationAllowed(pUvmChannelRetainer, pCallContext, pKernelChannel)

NV_STATUS uvmchanrtnrConstruct_IMPL(struct UvmChannelRetainer *arg_pUvmChannelRetainer, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_uvmchanrtnrConstruct(arg_pUvmChannelRetainer, arg_pCallContext, arg_pParams) uvmchanrtnrConstruct_IMPL(arg_pUvmChannelRetainer, arg_pCallContext, arg_pParams)
void uvmchanrtnrDestruct_IMPL(struct UvmChannelRetainer *pUvmChannelRetainer);

#define __nvoc_uvmchanrtnrDestruct(pUvmChannelRetainer) uvmchanrtnrDestruct_IMPL(pUvmChannelRetainer)
#undef PRIVATE_FIELD


#endif // UVM_CHANNEL_RETAINER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_UVM_CHANNEL_RETAINER_NVOC_H_
