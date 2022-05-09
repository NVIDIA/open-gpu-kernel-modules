#ifndef _G_GPU_RESOURCE_NVOC_H_
#define _G_GPU_RESOURCE_NVOC_H_
#include "nvoc/runtime.h"

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

#define GPU_RES_SET_THREAD_BC_STATE(pRes) do { \
    gpuSetThreadBcState(staticCastNoPtrCheck((pRes), GpuResource)->pGpu, \
                        staticCastNoPtrCheck((pRes), GpuResource)->bBcResource); \
    } while(0)

/*!
 * Abstract base class for common CPU mapping operations
 */
#ifdef NVOC_GPU_RESOURCE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct GpuResource {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RmResource __nvoc_base_RmResource;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    NV_STATUS (*__gpuresControl__)(struct GpuResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpuresMap__)(struct GpuResource *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NV_STATUS (*__gpuresUnmap__)(struct GpuResource *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NvBool (*__gpuresShareCallback__)(struct GpuResource *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__gpuresGetRegBaseOffsetAndSize__)(struct GpuResource *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__gpuresGetMapAddrSpace__)(struct GpuResource *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NV_STATUS (*__gpuresInternalControlForward__)(struct GpuResource *, NvU32, void *, NvU32);
    NvHandle (*__gpuresGetInternalObjectHandle__)(struct GpuResource *);
    NV_STATUS (*__gpuresCheckMemInterUnmap__)(struct GpuResource *, NvBool);
    NV_STATUS (*__gpuresGetMemInterMapParams__)(struct GpuResource *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__gpuresGetMemoryMappingDescriptor__)(struct GpuResource *, struct MEMORY_DESCRIPTOR **);
    NvU32 (*__gpuresGetRefCount__)(struct GpuResource *);
    NV_STATUS (*__gpuresControlFilter__)(struct GpuResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    void (*__gpuresAddAdditionalDependants__)(struct RsClient *, struct GpuResource *, RsResourceRef *);
    NV_STATUS (*__gpuresControl_Prologue__)(struct GpuResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__gpuresCanCopy__)(struct GpuResource *);
    NV_STATUS (*__gpuresMapTo__)(struct GpuResource *, RS_RES_MAP_TO_PARAMS *);
    void (*__gpuresPreDestruct__)(struct GpuResource *);
    NV_STATUS (*__gpuresUnmapFrom__)(struct GpuResource *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__gpuresControl_Epilogue__)(struct GpuResource *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpuresControlLookup__)(struct GpuResource *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NvBool (*__gpuresAccessCallback__)(struct GpuResource *, struct RsClient *, void *, RsAccessRight);
    struct OBJGPUGRP *pGpuGrp;
    struct OBJGPU *pGpu;
    struct Device *pDevice;
    struct Subdevice *pSubdevice;
    NvBool bBcResource;
};

#ifndef __NVOC_CLASS_GpuResource_TYPEDEF__
#define __NVOC_CLASS_GpuResource_TYPEDEF__
typedef struct GpuResource GpuResource;
#endif /* __NVOC_CLASS_GpuResource_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuResource
#define __nvoc_class_id_GpuResource 0x5d5d9f
#endif /* __nvoc_class_id_GpuResource */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

#define __staticCast_GpuResource(pThis) \
    ((pThis)->__nvoc_pbase_GpuResource)

#ifdef __nvoc_gpu_resource_h_disabled
#define __dynamicCast_GpuResource(pThis) ((GpuResource*)NULL)
#else //__nvoc_gpu_resource_h_disabled
#define __dynamicCast_GpuResource(pThis) \
    ((GpuResource*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuResource)))
#endif //__nvoc_gpu_resource_h_disabled


NV_STATUS __nvoc_objCreateDynamic_GpuResource(GpuResource**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuResource(GpuResource**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_GpuResource(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GpuResource((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define gpuresControl(pGpuResource, pCallContext, pParams) gpuresControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define gpuresMap(pGpuResource, pCallContext, pParams, pCpuMapping) gpuresMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define gpuresUnmap(pGpuResource, pCallContext, pCpuMapping) gpuresUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define gpuresShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) gpuresShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define gpuresGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) gpuresGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define gpuresGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) gpuresGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define gpuresInternalControlForward(pGpuResource, command, pParams, size) gpuresInternalControlForward_DISPATCH(pGpuResource, command, pParams, size)
#define gpuresGetInternalObjectHandle(pGpuResource) gpuresGetInternalObjectHandle_DISPATCH(pGpuResource)
#define gpuresCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) gpuresCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define gpuresGetMemInterMapParams(pRmResource, pParams) gpuresGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define gpuresGetMemoryMappingDescriptor(pRmResource, ppMemDesc) gpuresGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define gpuresGetRefCount(pResource) gpuresGetRefCount_DISPATCH(pResource)
#define gpuresControlFilter(pResource, pCallContext, pParams) gpuresControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gpuresAddAdditionalDependants(pClient, pResource, pReference) gpuresAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define gpuresControl_Prologue(pResource, pCallContext, pParams) gpuresControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpuresCanCopy(pResource) gpuresCanCopy_DISPATCH(pResource)
#define gpuresMapTo(pResource, pParams) gpuresMapTo_DISPATCH(pResource, pParams)
#define gpuresPreDestruct(pResource) gpuresPreDestruct_DISPATCH(pResource)
#define gpuresUnmapFrom(pResource, pParams) gpuresUnmapFrom_DISPATCH(pResource, pParams)
#define gpuresControl_Epilogue(pResource, pCallContext, pParams) gpuresControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpuresControlLookup(pResource, pParams, ppEntry) gpuresControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define gpuresAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gpuresAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS gpuresControl_IMPL(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

static inline NV_STATUS gpuresControl_DISPATCH(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__gpuresControl__(pGpuResource, pCallContext, pParams);
}

NV_STATUS gpuresMap_IMPL(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);

static inline NV_STATUS gpuresMap_DISPATCH(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__gpuresMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

NV_STATUS gpuresUnmap_IMPL(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);

static inline NV_STATUS gpuresUnmap_DISPATCH(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__gpuresUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

NvBool gpuresShareCallback_IMPL(struct GpuResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);

static inline NvBool gpuresShareCallback_DISPATCH(struct GpuResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__gpuresShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

NV_STATUS gpuresGetRegBaseOffsetAndSize_IMPL(struct GpuResource *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);

static inline NV_STATUS gpuresGetRegBaseOffsetAndSize_DISPATCH(struct GpuResource *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__gpuresGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

NV_STATUS gpuresGetMapAddrSpace_IMPL(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);

static inline NV_STATUS gpuresGetMapAddrSpace_DISPATCH(struct GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__gpuresGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

NV_STATUS gpuresInternalControlForward_IMPL(struct GpuResource *pGpuResource, NvU32 command, void *pParams, NvU32 size);

static inline NV_STATUS gpuresInternalControlForward_DISPATCH(struct GpuResource *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return pGpuResource->__gpuresInternalControlForward__(pGpuResource, command, pParams, size);
}

NvHandle gpuresGetInternalObjectHandle_IMPL(struct GpuResource *pGpuResource);

static inline NvHandle gpuresGetInternalObjectHandle_DISPATCH(struct GpuResource *pGpuResource) {
    return pGpuResource->__gpuresGetInternalObjectHandle__(pGpuResource);
}

static inline NV_STATUS gpuresCheckMemInterUnmap_DISPATCH(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__gpuresCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS gpuresGetMemInterMapParams_DISPATCH(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__gpuresGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS gpuresGetMemoryMappingDescriptor_DISPATCH(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__gpuresGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NvU32 gpuresGetRefCount_DISPATCH(struct GpuResource *pResource) {
    return pResource->__gpuresGetRefCount__(pResource);
}

static inline NV_STATUS gpuresControlFilter_DISPATCH(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpuresControlFilter__(pResource, pCallContext, pParams);
}

static inline void gpuresAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference) {
    pResource->__gpuresAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS gpuresControl_Prologue_DISPATCH(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpuresControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool gpuresCanCopy_DISPATCH(struct GpuResource *pResource) {
    return pResource->__gpuresCanCopy__(pResource);
}

static inline NV_STATUS gpuresMapTo_DISPATCH(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__gpuresMapTo__(pResource, pParams);
}

static inline void gpuresPreDestruct_DISPATCH(struct GpuResource *pResource) {
    pResource->__gpuresPreDestruct__(pResource);
}

static inline NV_STATUS gpuresUnmapFrom_DISPATCH(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__gpuresUnmapFrom__(pResource, pParams);
}

static inline void gpuresControl_Epilogue_DISPATCH(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__gpuresControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpuresControlLookup_DISPATCH(struct GpuResource *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__gpuresControlLookup__(pResource, pParams, ppEntry);
}

static inline NvBool gpuresAccessCallback_DISPATCH(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__gpuresAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

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
