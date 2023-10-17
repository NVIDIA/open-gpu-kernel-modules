#ifndef _G_UVM_CHANNEL_RETAINER_NVOC_H_
#define _G_UVM_CHANNEL_RETAINER_NVOC_H_
#include "nvoc/runtime.h"

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



#ifdef NVOC_UVM_CHANNEL_RETAINER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct UvmChannelRetainer {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct UvmChannelRetainer *__nvoc_pbase_UvmChannelRetainer;
    NvBool (*__uvmchanrtnrShareCallback__)(struct UvmChannelRetainer *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__uvmchanrtnrCheckMemInterUnmap__)(struct UvmChannelRetainer *, NvBool);
    NV_STATUS (*__uvmchanrtnrMapTo__)(struct UvmChannelRetainer *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__uvmchanrtnrGetMapAddrSpace__)(struct UvmChannelRetainer *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__uvmchanrtnrGetRefCount__)(struct UvmChannelRetainer *);
    void (*__uvmchanrtnrAddAdditionalDependants__)(struct RsClient *, struct UvmChannelRetainer *, RsResourceRef *);
    NV_STATUS (*__uvmchanrtnrControl_Prologue__)(struct UvmChannelRetainer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__uvmchanrtnrGetRegBaseOffsetAndSize__)(struct UvmChannelRetainer *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__uvmchanrtnrInternalControlForward__)(struct UvmChannelRetainer *, NvU32, void *, NvU32);
    NV_STATUS (*__uvmchanrtnrUnmapFrom__)(struct UvmChannelRetainer *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__uvmchanrtnrControl_Epilogue__)(struct UvmChannelRetainer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__uvmchanrtnrControlLookup__)(struct UvmChannelRetainer *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__uvmchanrtnrGetInternalObjectHandle__)(struct UvmChannelRetainer *);
    NV_STATUS (*__uvmchanrtnrControl__)(struct UvmChannelRetainer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__uvmchanrtnrUnmap__)(struct UvmChannelRetainer *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__uvmchanrtnrGetMemInterMapParams__)(struct UvmChannelRetainer *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__uvmchanrtnrGetMemoryMappingDescriptor__)(struct UvmChannelRetainer *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__uvmchanrtnrControlFilter__)(struct UvmChannelRetainer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__uvmchanrtnrControlSerialization_Prologue__)(struct UvmChannelRetainer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__uvmchanrtnrCanCopy__)(struct UvmChannelRetainer *);
    void (*__uvmchanrtnrPreDestruct__)(struct UvmChannelRetainer *);
    NV_STATUS (*__uvmchanrtnrIsDuplicate__)(struct UvmChannelRetainer *, NvHandle, NvBool *);
    void (*__uvmchanrtnrControlSerialization_Epilogue__)(struct UvmChannelRetainer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__uvmchanrtnrMap__)(struct UvmChannelRetainer *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__uvmchanrtnrAccessCallback__)(struct UvmChannelRetainer *, struct RsClient *, void *, RsAccessRight);
    MEMORY_DESCRIPTOR *pInstMemDesc;
    NvU32 chId;
    NvU32 runlistId;
};

#ifndef __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__
#define __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__
typedef struct UvmChannelRetainer UvmChannelRetainer;
#endif /* __NVOC_CLASS_UvmChannelRetainer_TYPEDEF__ */

#ifndef __nvoc_class_id_UvmChannelRetainer
#define __nvoc_class_id_UvmChannelRetainer 0xa3f03a
#endif /* __nvoc_class_id_UvmChannelRetainer */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_UvmChannelRetainer;

#define __staticCast_UvmChannelRetainer(pThis) \
    ((pThis)->__nvoc_pbase_UvmChannelRetainer)

#ifdef __nvoc_uvm_channel_retainer_h_disabled
#define __dynamicCast_UvmChannelRetainer(pThis) ((UvmChannelRetainer*)NULL)
#else //__nvoc_uvm_channel_retainer_h_disabled
#define __dynamicCast_UvmChannelRetainer(pThis) \
    ((UvmChannelRetainer*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(UvmChannelRetainer)))
#endif //__nvoc_uvm_channel_retainer_h_disabled


NV_STATUS __nvoc_objCreateDynamic_UvmChannelRetainer(UvmChannelRetainer**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_UvmChannelRetainer(UvmChannelRetainer**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_UvmChannelRetainer(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_UvmChannelRetainer((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define uvmchanrtnrShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) uvmchanrtnrShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define uvmchanrtnrCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) uvmchanrtnrCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define uvmchanrtnrMapTo(pResource, pParams) uvmchanrtnrMapTo_DISPATCH(pResource, pParams)
#define uvmchanrtnrGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) uvmchanrtnrGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define uvmchanrtnrGetRefCount(pResource) uvmchanrtnrGetRefCount_DISPATCH(pResource)
#define uvmchanrtnrAddAdditionalDependants(pClient, pResource, pReference) uvmchanrtnrAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define uvmchanrtnrControl_Prologue(pResource, pCallContext, pParams) uvmchanrtnrControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) uvmchanrtnrGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define uvmchanrtnrInternalControlForward(pGpuResource, command, pParams, size) uvmchanrtnrInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define uvmchanrtnrUnmapFrom(pResource, pParams) uvmchanrtnrUnmapFrom_DISPATCH(pResource, pParams)
#define uvmchanrtnrControl_Epilogue(pResource, pCallContext, pParams) uvmchanrtnrControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrControlLookup(pResource, pParams, ppEntry) uvmchanrtnrControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define uvmchanrtnrGetInternalObjectHandle(pGpuResource) uvmchanrtnrGetInternalObjectHandle_DISPATCH(pGpuResource)
#define uvmchanrtnrControl(pGpuResource, pCallContext, pParams) uvmchanrtnrControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define uvmchanrtnrUnmap(pGpuResource, pCallContext, pCpuMapping) uvmchanrtnrUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define uvmchanrtnrGetMemInterMapParams(pRmResource, pParams) uvmchanrtnrGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define uvmchanrtnrGetMemoryMappingDescriptor(pRmResource, ppMemDesc) uvmchanrtnrGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define uvmchanrtnrControlFilter(pResource, pCallContext, pParams) uvmchanrtnrControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrControlSerialization_Prologue(pResource, pCallContext, pParams) uvmchanrtnrControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrCanCopy(pResource) uvmchanrtnrCanCopy_DISPATCH(pResource)
#define uvmchanrtnrPreDestruct(pResource) uvmchanrtnrPreDestruct_DISPATCH(pResource)
#define uvmchanrtnrIsDuplicate(pResource, hMemory, pDuplicate) uvmchanrtnrIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define uvmchanrtnrControlSerialization_Epilogue(pResource, pCallContext, pParams) uvmchanrtnrControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define uvmchanrtnrMap(pGpuResource, pCallContext, pParams, pCpuMapping) uvmchanrtnrMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define uvmchanrtnrAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) uvmchanrtnrAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
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

static inline NvBool uvmchanrtnrShareCallback_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__uvmchanrtnrShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS uvmchanrtnrCheckMemInterUnmap_DISPATCH(struct UvmChannelRetainer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__uvmchanrtnrCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS uvmchanrtnrMapTo_DISPATCH(struct UvmChannelRetainer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__uvmchanrtnrMapTo__(pResource, pParams);
}

static inline NV_STATUS uvmchanrtnrGetMapAddrSpace_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__uvmchanrtnrGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 uvmchanrtnrGetRefCount_DISPATCH(struct UvmChannelRetainer *pResource) {
    return pResource->__uvmchanrtnrGetRefCount__(pResource);
}

static inline void uvmchanrtnrAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct UvmChannelRetainer *pResource, RsResourceRef *pReference) {
    pResource->__uvmchanrtnrAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS uvmchanrtnrControl_Prologue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__uvmchanrtnrControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS uvmchanrtnrGetRegBaseOffsetAndSize_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__uvmchanrtnrGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS uvmchanrtnrInternalControlForward_DISPATCH(struct UvmChannelRetainer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__uvmchanrtnrInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS uvmchanrtnrUnmapFrom_DISPATCH(struct UvmChannelRetainer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__uvmchanrtnrUnmapFrom__(pResource, pParams);
}

static inline void uvmchanrtnrControl_Epilogue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__uvmchanrtnrControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS uvmchanrtnrControlLookup_DISPATCH(struct UvmChannelRetainer *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__uvmchanrtnrControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle uvmchanrtnrGetInternalObjectHandle_DISPATCH(struct UvmChannelRetainer *pGpuResource) {
    return pGpuResource->__uvmchanrtnrGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS uvmchanrtnrControl_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__uvmchanrtnrControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS uvmchanrtnrUnmap_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__uvmchanrtnrUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS uvmchanrtnrGetMemInterMapParams_DISPATCH(struct UvmChannelRetainer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__uvmchanrtnrGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS uvmchanrtnrGetMemoryMappingDescriptor_DISPATCH(struct UvmChannelRetainer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__uvmchanrtnrGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS uvmchanrtnrControlFilter_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__uvmchanrtnrControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS uvmchanrtnrControlSerialization_Prologue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__uvmchanrtnrControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool uvmchanrtnrCanCopy_DISPATCH(struct UvmChannelRetainer *pResource) {
    return pResource->__uvmchanrtnrCanCopy__(pResource);
}

static inline void uvmchanrtnrPreDestruct_DISPATCH(struct UvmChannelRetainer *pResource) {
    pResource->__uvmchanrtnrPreDestruct__(pResource);
}

static inline NV_STATUS uvmchanrtnrIsDuplicate_DISPATCH(struct UvmChannelRetainer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__uvmchanrtnrIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void uvmchanrtnrControlSerialization_Epilogue_DISPATCH(struct UvmChannelRetainer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__uvmchanrtnrControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS uvmchanrtnrMap_DISPATCH(struct UvmChannelRetainer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__uvmchanrtnrMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool uvmchanrtnrAccessCallback_DISPATCH(struct UvmChannelRetainer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__uvmchanrtnrAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
