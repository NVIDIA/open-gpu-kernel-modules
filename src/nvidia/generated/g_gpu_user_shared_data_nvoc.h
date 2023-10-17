#ifndef _G_GPU_USER_SHARED_DATA_NVOC_H_
#define _G_GPU_USER_SHARED_DATA_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_gpu_user_shared_data_nvoc.h"

#ifndef GPU_USER_SHARED_DATA_H
#define GPU_USER_SHARED_DATA_H

#include "core/core.h"
#include "mem_mgr/mem.h"
#include "gpu/gpu.h"
#include "nvoc/utility.h"
#include "ctrl/ctrl00de.h"

// ****************************************************************************
//                          Type definitions
// ****************************************************************************

/*!
 * RM internal class representing RM_USER_SHARED_DATA
 */
#ifdef NVOC_GPU_USER_SHARED_DATA_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct GpuUserSharedData {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Memory __nvoc_base_Memory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct GpuUserSharedData *__nvoc_pbase_GpuUserSharedData;
    NvBool (*__gpushareddataCanCopy__)(struct GpuUserSharedData *);
    NV_STATUS (*__gpushareddataCtrlCmdRequestDataPoll__)(struct GpuUserSharedData *, NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *);
    NV_STATUS (*__gpushareddataCheckMemInterUnmap__)(struct GpuUserSharedData *, NvBool);
    NvBool (*__gpushareddataShareCallback__)(struct GpuUserSharedData *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__gpushareddataMapTo__)(struct GpuUserSharedData *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__gpushareddataGetMapAddrSpace__)(struct GpuUserSharedData *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__gpushareddataIsExportAllowed__)(struct GpuUserSharedData *);
    NvU32 (*__gpushareddataGetRefCount__)(struct GpuUserSharedData *);
    void (*__gpushareddataAddAdditionalDependants__)(struct RsClient *, struct GpuUserSharedData *, RsResourceRef *);
    NV_STATUS (*__gpushareddataControl_Prologue__)(struct GpuUserSharedData *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__gpushareddataIsGpuMapAllowed__)(struct GpuUserSharedData *, struct OBJGPU *);
    NV_STATUS (*__gpushareddataUnmapFrom__)(struct GpuUserSharedData *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__gpushareddataControl_Epilogue__)(struct GpuUserSharedData *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpushareddataControlLookup__)(struct GpuUserSharedData *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__gpushareddataControl__)(struct GpuUserSharedData *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpushareddataUnmap__)(struct GpuUserSharedData *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__gpushareddataGetMemInterMapParams__)(struct GpuUserSharedData *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__gpushareddataGetMemoryMappingDescriptor__)(struct GpuUserSharedData *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__gpushareddataControlFilter__)(struct GpuUserSharedData *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpushareddataControlSerialization_Prologue__)(struct GpuUserSharedData *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpushareddataIsReady__)(struct GpuUserSharedData *, NvBool);
    NV_STATUS (*__gpushareddataCheckCopyPermissions__)(struct GpuUserSharedData *, struct OBJGPU *, struct Device *);
    void (*__gpushareddataPreDestruct__)(struct GpuUserSharedData *);
    NV_STATUS (*__gpushareddataIsDuplicate__)(struct GpuUserSharedData *, NvHandle, NvBool *);
    void (*__gpushareddataControlSerialization_Epilogue__)(struct GpuUserSharedData *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__gpushareddataMap__)(struct GpuUserSharedData *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__gpushareddataAccessCallback__)(struct GpuUserSharedData *, struct RsClient *, void *, RsAccessRight);
    NvU64 polledDataMask;
};

#ifndef __NVOC_CLASS_GpuUserSharedData_TYPEDEF__
#define __NVOC_CLASS_GpuUserSharedData_TYPEDEF__
typedef struct GpuUserSharedData GpuUserSharedData;
#endif /* __NVOC_CLASS_GpuUserSharedData_TYPEDEF__ */

#ifndef __nvoc_class_id_GpuUserSharedData
#define __nvoc_class_id_GpuUserSharedData 0x5e7d1f
#endif /* __nvoc_class_id_GpuUserSharedData */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuUserSharedData;

#define __staticCast_GpuUserSharedData(pThis) \
    ((pThis)->__nvoc_pbase_GpuUserSharedData)

#ifdef __nvoc_gpu_user_shared_data_h_disabled
#define __dynamicCast_GpuUserSharedData(pThis) ((GpuUserSharedData*)NULL)
#else //__nvoc_gpu_user_shared_data_h_disabled
#define __dynamicCast_GpuUserSharedData(pThis) \
    ((GpuUserSharedData*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpuUserSharedData)))
#endif //__nvoc_gpu_user_shared_data_h_disabled


NV_STATUS __nvoc_objCreateDynamic_GpuUserSharedData(GpuUserSharedData**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpuUserSharedData(GpuUserSharedData**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_GpuUserSharedData(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_GpuUserSharedData((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define gpushareddataCanCopy(pData) gpushareddataCanCopy_DISPATCH(pData)
#define gpushareddataCtrlCmdRequestDataPoll(pData, pParams) gpushareddataCtrlCmdRequestDataPoll_DISPATCH(pData, pParams)
#define gpushareddataCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) gpushareddataCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define gpushareddataShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) gpushareddataShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define gpushareddataMapTo(pResource, pParams) gpushareddataMapTo_DISPATCH(pResource, pParams)
#define gpushareddataGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) gpushareddataGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define gpushareddataIsExportAllowed(pMemory) gpushareddataIsExportAllowed_DISPATCH(pMemory)
#define gpushareddataGetRefCount(pResource) gpushareddataGetRefCount_DISPATCH(pResource)
#define gpushareddataAddAdditionalDependants(pClient, pResource, pReference) gpushareddataAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define gpushareddataControl_Prologue(pResource, pCallContext, pParams) gpushareddataControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataIsGpuMapAllowed(pMemory, pGpu) gpushareddataIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define gpushareddataUnmapFrom(pResource, pParams) gpushareddataUnmapFrom_DISPATCH(pResource, pParams)
#define gpushareddataControl_Epilogue(pResource, pCallContext, pParams) gpushareddataControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataControlLookup(pResource, pParams, ppEntry) gpushareddataControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define gpushareddataControl(pMemory, pCallContext, pParams) gpushareddataControl_DISPATCH(pMemory, pCallContext, pParams)
#define gpushareddataUnmap(pMemory, pCallContext, pCpuMapping) gpushareddataUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define gpushareddataGetMemInterMapParams(pMemory, pParams) gpushareddataGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define gpushareddataGetMemoryMappingDescriptor(pMemory, ppMemDesc) gpushareddataGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define gpushareddataControlFilter(pResource, pCallContext, pParams) gpushareddataControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataControlSerialization_Prologue(pResource, pCallContext, pParams) gpushareddataControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataIsReady(pMemory, bCopyConstructorContext) gpushareddataIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define gpushareddataCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) gpushareddataCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define gpushareddataPreDestruct(pResource) gpushareddataPreDestruct_DISPATCH(pResource)
#define gpushareddataIsDuplicate(pMemory, hMemory, pDuplicate) gpushareddataIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define gpushareddataControlSerialization_Epilogue(pResource, pCallContext, pParams) gpushareddataControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define gpushareddataMap(pMemory, pCallContext, pParams, pCpuMapping) gpushareddataMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define gpushareddataAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) gpushareddataAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool gpushareddataCanCopy_IMPL(struct GpuUserSharedData *pData);

static inline NvBool gpushareddataCanCopy_DISPATCH(struct GpuUserSharedData *pData) {
    return pData->__gpushareddataCanCopy__(pData);
}

NV_STATUS gpushareddataCtrlCmdRequestDataPoll_IMPL(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams);

static inline NV_STATUS gpushareddataCtrlCmdRequestDataPoll_DISPATCH(struct GpuUserSharedData *pData, NV00DE_CTRL_REQUEST_DATA_POLL_PARAMS *pParams) {
    return pData->__gpushareddataCtrlCmdRequestDataPoll__(pData, pParams);
}

static inline NV_STATUS gpushareddataCheckMemInterUnmap_DISPATCH(struct GpuUserSharedData *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__gpushareddataCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NvBool gpushareddataShareCallback_DISPATCH(struct GpuUserSharedData *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__gpushareddataShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS gpushareddataMapTo_DISPATCH(struct GpuUserSharedData *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__gpushareddataMapTo__(pResource, pParams);
}

static inline NV_STATUS gpushareddataGetMapAddrSpace_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__gpushareddataGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool gpushareddataIsExportAllowed_DISPATCH(struct GpuUserSharedData *pMemory) {
    return pMemory->__gpushareddataIsExportAllowed__(pMemory);
}

static inline NvU32 gpushareddataGetRefCount_DISPATCH(struct GpuUserSharedData *pResource) {
    return pResource->__gpushareddataGetRefCount__(pResource);
}

static inline void gpushareddataAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct GpuUserSharedData *pResource, RsResourceRef *pReference) {
    pResource->__gpushareddataAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS gpushareddataControl_Prologue_DISPATCH(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpushareddataControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool gpushareddataIsGpuMapAllowed_DISPATCH(struct GpuUserSharedData *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__gpushareddataIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NV_STATUS gpushareddataUnmapFrom_DISPATCH(struct GpuUserSharedData *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__gpushareddataUnmapFrom__(pResource, pParams);
}

static inline void gpushareddataControl_Epilogue_DISPATCH(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__gpushareddataControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataControlLookup_DISPATCH(struct GpuUserSharedData *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__gpushareddataControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS gpushareddataControl_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__gpushareddataControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataUnmap_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__gpushareddataUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS gpushareddataGetMemInterMapParams_DISPATCH(struct GpuUserSharedData *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__gpushareddataGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS gpushareddataGetMemoryMappingDescriptor_DISPATCH(struct GpuUserSharedData *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__gpushareddataGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS gpushareddataControlFilter_DISPATCH(struct GpuUserSharedData *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpushareddataControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataControlSerialization_Prologue_DISPATCH(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__gpushareddataControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataIsReady_DISPATCH(struct GpuUserSharedData *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__gpushareddataIsReady__(pMemory, bCopyConstructorContext);
}

static inline NV_STATUS gpushareddataCheckCopyPermissions_DISPATCH(struct GpuUserSharedData *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__gpushareddataCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline void gpushareddataPreDestruct_DISPATCH(struct GpuUserSharedData *pResource) {
    pResource->__gpushareddataPreDestruct__(pResource);
}

static inline NV_STATUS gpushareddataIsDuplicate_DISPATCH(struct GpuUserSharedData *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__gpushareddataIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline void gpushareddataControlSerialization_Epilogue_DISPATCH(struct GpuUserSharedData *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__gpushareddataControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS gpushareddataMap_DISPATCH(struct GpuUserSharedData *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__gpushareddataMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool gpushareddataAccessCallback_DISPATCH(struct GpuUserSharedData *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__gpushareddataAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS gpushareddataConstruct_IMPL(struct GpuUserSharedData *arg_pData, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_gpushareddataConstruct(arg_pData, arg_pCallContext, arg_pParams) gpushareddataConstruct_IMPL(arg_pData, arg_pCallContext, arg_pParams)
void gpushareddataDestruct_IMPL(struct GpuUserSharedData *pData);

#define __nvoc_gpushareddataDestruct(pData) gpushareddataDestruct_IMPL(pData)
#undef PRIVATE_FIELD


#endif // GPU_USER_SHARED_DATA_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_USER_SHARED_DATA_NVOC_H_
