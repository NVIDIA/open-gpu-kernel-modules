
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

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_UVM_CHANNEL_RETAINER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__UvmChannelRetainer;
struct NVOC_METADATA__GpuResource;


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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__UvmChannelRetainer {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
};

#ifndef __nvoc_class_id_UvmChannelRetainer
#define __nvoc_class_id_UvmChannelRetainer 0xa3f03au
typedef struct UvmChannelRetainer UvmChannelRetainer;
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

NV_STATUS __nvoc_objCreateDynamic_UvmChannelRetainer(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_UvmChannelRetainer(UvmChannelRetainer**, Dynamic*, NvU32, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_UvmChannelRetainer(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_UvmChannelRetainer((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS uvmchanrtnrConstruct_IMPL(struct UvmChannelRetainer *pUvmChannelRetainer, CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_uvmchanrtnrConstruct(pUvmChannelRetainer, pCallContext, pParams) uvmchanrtnrConstruct_IMPL(pUvmChannelRetainer, pCallContext, pParams)

void uvmchanrtnrDestruct_IMPL(struct UvmChannelRetainer *pUvmChannelRetainer);
#define __nvoc_uvmchanrtnrDestruct(pUvmChannelRetainer) uvmchanrtnrDestruct_IMPL(pUvmChannelRetainer)

#ifdef __nvoc_uvm_channel_retainer_h_disabled
static inline NvBool uvmchanrtnrIsAllocationAllowed(struct UvmChannelRetainer *pUvmChannelRetainer, CALL_CONTEXT *pCallContext, struct KernelChannel *pKernelChannel) {
    NV_ASSERT_FAILED_PRECOMP("UvmChannelRetainer was disabled!");
    return NV_FALSE;
}
#else // __nvoc_uvm_channel_retainer_h_disabled
#define uvmchanrtnrIsAllocationAllowed(pUvmChannelRetainer, pCallContext, pKernelChannel) uvmchanrtnrIsAllocationAllowed_IMPL(pUvmChannelRetainer, pCallContext, pKernelChannel)
#endif // __nvoc_uvm_channel_retainer_h_disabled


// Wrapper macros for halified functions
#define uvmchanrtnrIsAllocationAllowed_HAL(pUvmChannelRetainer, pCallContext, pKernelChannel) uvmchanrtnrIsAllocationAllowed(pUvmChannelRetainer, pCallContext, pKernelChannel)
#define uvmchanrtnrControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define uvmchanrtnrControl(pGpuResource, pCallContext, pParams) uvmchanrtnrControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define uvmchanrtnrMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define uvmchanrtnrMap(pGpuResource, pCallContext, pParams, pCpuMapping) uvmchanrtnrMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define uvmchanrtnrUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define uvmchanrtnrUnmap(pGpuResource, pCallContext, pCpuMapping) uvmchanrtnrUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define uvmchanrtnrShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define uvmchanrtnrShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) uvmchanrtnrShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define uvmchanrtnrGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define uvmchanrtnrGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) uvmchanrtnrGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define uvmchanrtnrGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define uvmchanrtnrGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) uvmchanrtnrGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define uvmchanrtnrInternalControlForward_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__
#define uvmchanrtnrInternalControlForward(pGpuResource, command, pParams, size) uvmchanrtnrInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define uvmchanrtnrGetInternalObjectHandle_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__
#define uvmchanrtnrGetInternalObjectHandle(pGpuResource) uvmchanrtnrGetInternalObjectHandle_DISPATCH(pGpuResource)
#define uvmchanrtnrAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define uvmchanrtnrAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) uvmchanrtnrAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define uvmchanrtnrGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define uvmchanrtnrGetMemInterMapParams(pRmResource, pParams) uvmchanrtnrGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define uvmchanrtnrCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define uvmchanrtnrCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) uvmchanrtnrCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define uvmchanrtnrGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define uvmchanrtnrGetMemoryMappingDescriptor(pRmResource, ppMemDesc) uvmchanrtnrGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define uvmchanrtnrControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define uvmchanrtnrControlSerialization_Prologue(pResource, pCallContext, pParams) uvmchanrtnrControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define uvmchanrtnrControlSerialization_Epilogue(pResource, pCallContext, pParams) uvmchanrtnrControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define uvmchanrtnrControl_Prologue(pResource, pCallContext, pParams) uvmchanrtnrControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
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
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pGpuResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS uvmchanrtnrMap_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pGpuResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS uvmchanrtnrUnmap_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pGpuResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool uvmchanrtnrShareCallback_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pGpuResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS uvmchanrtnrGetRegBaseOffsetAndSize_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__(staticCast(pGpuResource, GpuResource), pGpu, pOffset, pSize);
}

static inline NV_STATUS uvmchanrtnrGetMapAddrSpace_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__(staticCast(pGpuResource, GpuResource), pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS uvmchanrtnrInternalControlForward_DISPATCH(struct UvmChannelRetainer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresInternalControlForward__(staticCast(pGpuResource, GpuResource), command, pParams, size);
}

static inline NvHandle uvmchanrtnrGetInternalObjectHandle_DISPATCH(struct UvmChannelRetainer *pGpuResource) {
    return pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetInternalObjectHandle__(staticCast(pGpuResource, GpuResource));
}

static inline NvBool uvmchanrtnrAccessCallback_DISPATCH(struct UvmChannelRetainer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS uvmchanrtnrGetMemInterMapParams_DISPATCH(struct UvmChannelRetainer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS uvmchanrtnrCheckMemInterUnmap_DISPATCH(struct UvmChannelRetainer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS uvmchanrtnrGetMemoryMappingDescriptor_DISPATCH(struct UvmChannelRetainer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS uvmchanrtnrControlSerialization_Prologue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void uvmchanrtnrControlSerialization_Epilogue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS uvmchanrtnrControl_Prologue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void uvmchanrtnrControl_Epilogue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool uvmchanrtnrCanCopy_DISPATCH(struct UvmChannelRetainer *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS uvmchanrtnrIsDuplicate_DISPATCH(struct UvmChannelRetainer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void uvmchanrtnrPreDestruct_DISPATCH(struct UvmChannelRetainer *pResource) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS uvmchanrtnrControlFilter_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool uvmchanrtnrIsPartialUnmapSupported_DISPATCH(struct UvmChannelRetainer *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS uvmchanrtnrMapTo_DISPATCH(struct UvmChannelRetainer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS uvmchanrtnrUnmapFrom_DISPATCH(struct UvmChannelRetainer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 uvmchanrtnrGetRefCount_DISPATCH(struct UvmChannelRetainer *pResource) {
    return pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void uvmchanrtnrAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct UvmChannelRetainer *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
NvBool uvmchanrtnrIsAllocationAllowed_IMPL(struct UvmChannelRetainer *pUvmChannelRetainer, CALL_CONTEXT *pCallContext, struct KernelChannel *pKernelChannel);

// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // UVM_CHANNEL_RETAINER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_UVM_CHANNEL_RETAINER_NVOC_H_
