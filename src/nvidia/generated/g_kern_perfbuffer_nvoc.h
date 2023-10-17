#ifndef _G_KERN_PERFBUFFER_NVOC_H_
#define _G_KERN_PERFBUFFER_NVOC_H_
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

#include "g_kern_perfbuffer_nvoc.h"

#ifndef KERN_PERFBUFFER_H
#define KERN_PERFBUFFER_H

#include "rmapi/client.h"
#include "gpu/gpu_resource.h"
#include "gpu/gpu_halspec.h"

/*!
 * Definition of PerfBuffer resource class
 */
#ifdef NVOC_KERN_PERFBUFFER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct PerfBuffer {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct PerfBuffer *__nvoc_pbase_PerfBuffer;
    NvBool (*__perfbufferShareCallback__)(struct PerfBuffer *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__perfbufferCheckMemInterUnmap__)(struct PerfBuffer *, NvBool);
    NV_STATUS (*__perfbufferMapTo__)(struct PerfBuffer *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__perfbufferGetMapAddrSpace__)(struct PerfBuffer *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__perfbufferGetRefCount__)(struct PerfBuffer *);
    void (*__perfbufferAddAdditionalDependants__)(struct RsClient *, struct PerfBuffer *, RsResourceRef *);
    NV_STATUS (*__perfbufferControl_Prologue__)(struct PerfBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__perfbufferGetRegBaseOffsetAndSize__)(struct PerfBuffer *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__perfbufferInternalControlForward__)(struct PerfBuffer *, NvU32, void *, NvU32);
    NV_STATUS (*__perfbufferUnmapFrom__)(struct PerfBuffer *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__perfbufferControl_Epilogue__)(struct PerfBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__perfbufferControlLookup__)(struct PerfBuffer *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__perfbufferGetInternalObjectHandle__)(struct PerfBuffer *);
    NV_STATUS (*__perfbufferControl__)(struct PerfBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__perfbufferUnmap__)(struct PerfBuffer *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__perfbufferGetMemInterMapParams__)(struct PerfBuffer *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__perfbufferGetMemoryMappingDescriptor__)(struct PerfBuffer *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__perfbufferControlFilter__)(struct PerfBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__perfbufferControlSerialization_Prologue__)(struct PerfBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__perfbufferCanCopy__)(struct PerfBuffer *);
    void (*__perfbufferPreDestruct__)(struct PerfBuffer *);
    NV_STATUS (*__perfbufferIsDuplicate__)(struct PerfBuffer *, NvHandle, NvBool *);
    void (*__perfbufferControlSerialization_Epilogue__)(struct PerfBuffer *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__perfbufferMap__)(struct PerfBuffer *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__perfbufferAccessCallback__)(struct PerfBuffer *, struct RsClient *, void *, RsAccessRight);
    void *pObject;
};

#ifndef __NVOC_CLASS_PerfBuffer_TYPEDEF__
#define __NVOC_CLASS_PerfBuffer_TYPEDEF__
typedef struct PerfBuffer PerfBuffer;
#endif /* __NVOC_CLASS_PerfBuffer_TYPEDEF__ */

#ifndef __nvoc_class_id_PerfBuffer
#define __nvoc_class_id_PerfBuffer 0x4bc43b
#endif /* __nvoc_class_id_PerfBuffer */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_PerfBuffer;

#define __staticCast_PerfBuffer(pThis) \
    ((pThis)->__nvoc_pbase_PerfBuffer)

#ifdef __nvoc_kern_perfbuffer_h_disabled
#define __dynamicCast_PerfBuffer(pThis) ((PerfBuffer*)NULL)
#else //__nvoc_kern_perfbuffer_h_disabled
#define __dynamicCast_PerfBuffer(pThis) \
    ((PerfBuffer*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PerfBuffer)))
#endif //__nvoc_kern_perfbuffer_h_disabled


NV_STATUS __nvoc_objCreateDynamic_PerfBuffer(PerfBuffer**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PerfBuffer(PerfBuffer**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_PerfBuffer(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_PerfBuffer((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define perfbufferShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) perfbufferShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define perfbufferCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) perfbufferCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define perfbufferMapTo(pResource, pParams) perfbufferMapTo_DISPATCH(pResource, pParams)
#define perfbufferGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) perfbufferGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define perfbufferGetRefCount(pResource) perfbufferGetRefCount_DISPATCH(pResource)
#define perfbufferAddAdditionalDependants(pClient, pResource, pReference) perfbufferAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define perfbufferControl_Prologue(pResource, pCallContext, pParams) perfbufferControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) perfbufferGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define perfbufferInternalControlForward(pGpuResource, command, pParams, size) perfbufferInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define perfbufferUnmapFrom(pResource, pParams) perfbufferUnmapFrom_DISPATCH(pResource, pParams)
#define perfbufferControl_Epilogue(pResource, pCallContext, pParams) perfbufferControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferControlLookup(pResource, pParams, ppEntry) perfbufferControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define perfbufferGetInternalObjectHandle(pGpuResource) perfbufferGetInternalObjectHandle_DISPATCH(pGpuResource)
#define perfbufferControl(pGpuResource, pCallContext, pParams) perfbufferControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define perfbufferUnmap(pGpuResource, pCallContext, pCpuMapping) perfbufferUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define perfbufferGetMemInterMapParams(pRmResource, pParams) perfbufferGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define perfbufferGetMemoryMappingDescriptor(pRmResource, ppMemDesc) perfbufferGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define perfbufferControlFilter(pResource, pCallContext, pParams) perfbufferControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferControlSerialization_Prologue(pResource, pCallContext, pParams) perfbufferControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferCanCopy(pResource) perfbufferCanCopy_DISPATCH(pResource)
#define perfbufferPreDestruct(pResource) perfbufferPreDestruct_DISPATCH(pResource)
#define perfbufferIsDuplicate(pResource, hMemory, pDuplicate) perfbufferIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define perfbufferControlSerialization_Epilogue(pResource, pCallContext, pParams) perfbufferControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define perfbufferMap(pGpuResource, pCallContext, pParams, pCpuMapping) perfbufferMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define perfbufferAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) perfbufferAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS perfbufferConstructHal_KERNEL(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);


#ifdef __nvoc_kern_perfbuffer_h_disabled
static inline NV_STATUS perfbufferConstructHal(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("PerfBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_perfbuffer_h_disabled
#define perfbufferConstructHal(pResource, pCallContext, pParams) perfbufferConstructHal_KERNEL(pResource, pCallContext, pParams)
#endif //__nvoc_kern_perfbuffer_h_disabled

#define perfbufferConstructHal_HAL(pResource, pCallContext, pParams) perfbufferConstructHal(pResource, pCallContext, pParams)

static inline void perfbufferDestruct_b3696a(struct PerfBuffer *pResource) {
    return;
}


#define __nvoc_perfbufferDestruct(pResource) perfbufferDestruct_b3696a(pResource)
static inline NvBool perfbufferShareCallback_DISPATCH(struct PerfBuffer *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__perfbufferShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS perfbufferCheckMemInterUnmap_DISPATCH(struct PerfBuffer *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__perfbufferCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS perfbufferMapTo_DISPATCH(struct PerfBuffer *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__perfbufferMapTo__(pResource, pParams);
}

static inline NV_STATUS perfbufferGetMapAddrSpace_DISPATCH(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__perfbufferGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 perfbufferGetRefCount_DISPATCH(struct PerfBuffer *pResource) {
    return pResource->__perfbufferGetRefCount__(pResource);
}

static inline void perfbufferAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct PerfBuffer *pResource, RsResourceRef *pReference) {
    pResource->__perfbufferAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS perfbufferControl_Prologue_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__perfbufferControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS perfbufferGetRegBaseOffsetAndSize_DISPATCH(struct PerfBuffer *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__perfbufferGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS perfbufferInternalControlForward_DISPATCH(struct PerfBuffer *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__perfbufferInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS perfbufferUnmapFrom_DISPATCH(struct PerfBuffer *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__perfbufferUnmapFrom__(pResource, pParams);
}

static inline void perfbufferControl_Epilogue_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__perfbufferControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS perfbufferControlLookup_DISPATCH(struct PerfBuffer *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__perfbufferControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle perfbufferGetInternalObjectHandle_DISPATCH(struct PerfBuffer *pGpuResource) {
    return pGpuResource->__perfbufferGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS perfbufferControl_DISPATCH(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__perfbufferControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS perfbufferUnmap_DISPATCH(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__perfbufferUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS perfbufferGetMemInterMapParams_DISPATCH(struct PerfBuffer *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__perfbufferGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS perfbufferGetMemoryMappingDescriptor_DISPATCH(struct PerfBuffer *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__perfbufferGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS perfbufferControlFilter_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__perfbufferControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS perfbufferControlSerialization_Prologue_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__perfbufferControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool perfbufferCanCopy_DISPATCH(struct PerfBuffer *pResource) {
    return pResource->__perfbufferCanCopy__(pResource);
}

static inline void perfbufferPreDestruct_DISPATCH(struct PerfBuffer *pResource) {
    pResource->__perfbufferPreDestruct__(pResource);
}

static inline NV_STATUS perfbufferIsDuplicate_DISPATCH(struct PerfBuffer *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__perfbufferIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void perfbufferControlSerialization_Epilogue_DISPATCH(struct PerfBuffer *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__perfbufferControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS perfbufferMap_DISPATCH(struct PerfBuffer *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__perfbufferMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool perfbufferAccessCallback_DISPATCH(struct PerfBuffer *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__perfbufferAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS __nvoc_perfbufferConstruct(struct PerfBuffer *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams) {
    return perfbufferConstructHal(arg_pResource, arg_pCallContext, arg_pParams);
}

NV_STATUS perfbufferPrivilegeCheck_IMPL(struct PerfBuffer *pPerfBuffer);

#ifdef __nvoc_kern_perfbuffer_h_disabled
static inline NV_STATUS perfbufferPrivilegeCheck(struct PerfBuffer *pPerfBuffer) {
    NV_ASSERT_FAILED_PRECOMP("PerfBuffer was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_perfbuffer_h_disabled
#define perfbufferPrivilegeCheck(pPerfBuffer) perfbufferPrivilegeCheck_IMPL(pPerfBuffer)
#endif //__nvoc_kern_perfbuffer_h_disabled

#undef PRIVATE_FIELD


#endif // KERN_PERFBUFFER_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_PERFBUFFER_NVOC_H_
