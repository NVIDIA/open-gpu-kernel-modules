#ifndef _G_MEM_MAPPER_NVOC_H_
#define _G_MEM_MAPPER_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_mem_mapper_nvoc.h"

#ifndef MEMORY_MAPPER_H
#define MEMORY_MAPPER_H

#include "core/core.h"
#include "rmapi/rmapi.h"
#include "rmapi/resource.h"
#include "gpu/gpu_resource.h"

#include "class/cl00fe.h"
#include "ctrl/ctrl00fe.h"

struct Subdevice;

#ifndef __NVOC_CLASS_Subdevice_TYPEDEF__
#define __NVOC_CLASS_Subdevice_TYPEDEF__
typedef struct Subdevice Subdevice;
#endif /* __NVOC_CLASS_Subdevice_TYPEDEF__ */

#ifndef __nvoc_class_id_Subdevice
#define __nvoc_class_id_Subdevice 0x4b01b3
#endif /* __nvoc_class_id_Subdevice */


struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



/*!
 * MemoryMapper provides paging operations channel interface to userspace clients.
 */
#ifdef NVOC_MEM_MAPPER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct MemoryMapper {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct MemoryMapper *__nvoc_pbase_MemoryMapper;
    NV_STATUS (*__memmapperCtrlCmdSubmitPagingOperations__)(struct MemoryMapper *, NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_PARAMS *);
    NvBool (*__memmapperShareCallback__)(struct MemoryMapper *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__memmapperCheckMemInterUnmap__)(struct MemoryMapper *, NvBool);
    NV_STATUS (*__memmapperMapTo__)(struct MemoryMapper *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__memmapperGetMapAddrSpace__)(struct MemoryMapper *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvU32 (*__memmapperGetRefCount__)(struct MemoryMapper *);
    void (*__memmapperAddAdditionalDependants__)(struct RsClient *, struct MemoryMapper *, RsResourceRef *);
    NV_STATUS (*__memmapperControl_Prologue__)(struct MemoryMapper *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memmapperGetRegBaseOffsetAndSize__)(struct MemoryMapper *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__memmapperInternalControlForward__)(struct MemoryMapper *, NvU32, void *, NvU32);
    NV_STATUS (*__memmapperUnmapFrom__)(struct MemoryMapper *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__memmapperControl_Epilogue__)(struct MemoryMapper *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memmapperControlLookup__)(struct MemoryMapper *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvHandle (*__memmapperGetInternalObjectHandle__)(struct MemoryMapper *);
    NV_STATUS (*__memmapperControl__)(struct MemoryMapper *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memmapperUnmap__)(struct MemoryMapper *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__memmapperGetMemInterMapParams__)(struct MemoryMapper *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__memmapperGetMemoryMappingDescriptor__)(struct MemoryMapper *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__memmapperControlFilter__)(struct MemoryMapper *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memmapperControlSerialization_Prologue__)(struct MemoryMapper *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__memmapperCanCopy__)(struct MemoryMapper *);
    void (*__memmapperPreDestruct__)(struct MemoryMapper *);
    NV_STATUS (*__memmapperIsDuplicate__)(struct MemoryMapper *, NvHandle, NvBool *);
    void (*__memmapperControlSerialization_Epilogue__)(struct MemoryMapper *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__memmapperMap__)(struct MemoryMapper *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__memmapperAccessCallback__)(struct MemoryMapper *, struct RsClient *, void *, RsAccessRight);
    struct Subdevice *pSubDevice;
};

#ifndef __NVOC_CLASS_MemoryMapper_TYPEDEF__
#define __NVOC_CLASS_MemoryMapper_TYPEDEF__
typedef struct MemoryMapper MemoryMapper;
#endif /* __NVOC_CLASS_MemoryMapper_TYPEDEF__ */

#ifndef __nvoc_class_id_MemoryMapper
#define __nvoc_class_id_MemoryMapper 0xb8e4a2
#endif /* __nvoc_class_id_MemoryMapper */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryMapper;

#define __staticCast_MemoryMapper(pThis) \
    ((pThis)->__nvoc_pbase_MemoryMapper)

#ifdef __nvoc_mem_mapper_h_disabled
#define __dynamicCast_MemoryMapper(pThis) ((MemoryMapper*)NULL)
#else //__nvoc_mem_mapper_h_disabled
#define __dynamicCast_MemoryMapper(pThis) \
    ((MemoryMapper*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryMapper)))
#endif //__nvoc_mem_mapper_h_disabled


NV_STATUS __nvoc_objCreateDynamic_MemoryMapper(MemoryMapper**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryMapper(MemoryMapper**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_MemoryMapper(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_MemoryMapper((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define memmapperCtrlCmdSubmitPagingOperations(pMemoryMapper, pParams) memmapperCtrlCmdSubmitPagingOperations_DISPATCH(pMemoryMapper, pParams)
#define memmapperShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) memmapperShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define memmapperCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) memmapperCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define memmapperMapTo(pResource, pParams) memmapperMapTo_DISPATCH(pResource, pParams)
#define memmapperGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) memmapperGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define memmapperGetRefCount(pResource) memmapperGetRefCount_DISPATCH(pResource)
#define memmapperAddAdditionalDependants(pClient, pResource, pReference) memmapperAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define memmapperControl_Prologue(pResource, pCallContext, pParams) memmapperControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) memmapperGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define memmapperInternalControlForward(pGpuResource, command, pParams, size) memmapperInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define memmapperUnmapFrom(pResource, pParams) memmapperUnmapFrom_DISPATCH(pResource, pParams)
#define memmapperControl_Epilogue(pResource, pCallContext, pParams) memmapperControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperControlLookup(pResource, pParams, ppEntry) memmapperControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define memmapperGetInternalObjectHandle(pGpuResource) memmapperGetInternalObjectHandle_DISPATCH(pGpuResource)
#define memmapperControl(pGpuResource, pCallContext, pParams) memmapperControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define memmapperUnmap(pGpuResource, pCallContext, pCpuMapping) memmapperUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define memmapperGetMemInterMapParams(pRmResource, pParams) memmapperGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define memmapperGetMemoryMappingDescriptor(pRmResource, ppMemDesc) memmapperGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define memmapperControlFilter(pResource, pCallContext, pParams) memmapperControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define memmapperControlSerialization_Prologue(pResource, pCallContext, pParams) memmapperControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperCanCopy(pResource) memmapperCanCopy_DISPATCH(pResource)
#define memmapperPreDestruct(pResource) memmapperPreDestruct_DISPATCH(pResource)
#define memmapperIsDuplicate(pResource, hMemory, pDuplicate) memmapperIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define memmapperControlSerialization_Epilogue(pResource, pCallContext, pParams) memmapperControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define memmapperMap(pGpuResource, pCallContext, pParams, pCpuMapping) memmapperMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define memmapperAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) memmapperAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS memmapperCtrlCmdSubmitPagingOperations_IMPL(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_PARAMS *pParams);

static inline NV_STATUS memmapperCtrlCmdSubmitPagingOperations_DISPATCH(struct MemoryMapper *pMemoryMapper, NV00FE_CTRL_SUBMIT_PAGING_OPERATIONS_PARAMS *pParams) {
    return pMemoryMapper->__memmapperCtrlCmdSubmitPagingOperations__(pMemoryMapper, pParams);
}

static inline NvBool memmapperShareCallback_DISPATCH(struct MemoryMapper *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__memmapperShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS memmapperCheckMemInterUnmap_DISPATCH(struct MemoryMapper *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__memmapperCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS memmapperMapTo_DISPATCH(struct MemoryMapper *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__memmapperMapTo__(pResource, pParams);
}

static inline NV_STATUS memmapperGetMapAddrSpace_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__memmapperGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvU32 memmapperGetRefCount_DISPATCH(struct MemoryMapper *pResource) {
    return pResource->__memmapperGetRefCount__(pResource);
}

static inline void memmapperAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct MemoryMapper *pResource, RsResourceRef *pReference) {
    pResource->__memmapperAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS memmapperControl_Prologue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memmapperControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memmapperGetRegBaseOffsetAndSize_DISPATCH(struct MemoryMapper *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__memmapperGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS memmapperInternalControlForward_DISPATCH(struct MemoryMapper *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__memmapperInternalControlForward__(pGpuResource, command, pParams, size);
}

static inline NV_STATUS memmapperUnmapFrom_DISPATCH(struct MemoryMapper *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__memmapperUnmapFrom__(pResource, pParams);
}

static inline void memmapperControl_Epilogue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memmapperControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memmapperControlLookup_DISPATCH(struct MemoryMapper *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__memmapperControlLookup__(pResource, pParams, ppEntry);
}

static inline NvHandle memmapperGetInternalObjectHandle_DISPATCH(struct MemoryMapper *pGpuResource) {
    return pGpuResource->__memmapperGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS memmapperControl_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__memmapperControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS memmapperUnmap_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__memmapperUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS memmapperGetMemInterMapParams_DISPATCH(struct MemoryMapper *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__memmapperGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS memmapperGetMemoryMappingDescriptor_DISPATCH(struct MemoryMapper *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__memmapperGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS memmapperControlFilter_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memmapperControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memmapperControlSerialization_Prologue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__memmapperControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool memmapperCanCopy_DISPATCH(struct MemoryMapper *pResource) {
    return pResource->__memmapperCanCopy__(pResource);
}

static inline void memmapperPreDestruct_DISPATCH(struct MemoryMapper *pResource) {
    pResource->__memmapperPreDestruct__(pResource);
}

static inline NV_STATUS memmapperIsDuplicate_DISPATCH(struct MemoryMapper *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__memmapperIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void memmapperControlSerialization_Epilogue_DISPATCH(struct MemoryMapper *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__memmapperControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS memmapperMap_DISPATCH(struct MemoryMapper *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__memmapperMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool memmapperAccessCallback_DISPATCH(struct MemoryMapper *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__memmapperAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS memmapperConstruct_IMPL(struct MemoryMapper *arg_pMemoryMapper, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_memmapperConstruct(arg_pMemoryMapper, arg_pCallContext, arg_pParams) memmapperConstruct_IMPL(arg_pMemoryMapper, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // MEMORY_MAPPER_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_MEM_MAPPER_NVOC_H_
