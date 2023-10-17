#ifndef _G_REG_MEM_NVOC_H_
#define _G_REG_MEM_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_reg_mem_nvoc.h"

#ifndef _REGISTER_MEMORY_H_
#define _REGISTER_MEMORY_H_

#include "mem_mgr/mem.h"

/*!
 * Memory allocation representing GPU register space
 *
 * For security and maintenance reasons we want to phase this class out.
 */
#ifdef NVOC_REG_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct RegisterMemory {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Memory __nvoc_base_Memory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct RegisterMemory *__nvoc_pbase_RegisterMemory;
    NvBool (*__regmemCanCopy__)(struct RegisterMemory *);
    NV_STATUS (*__regmemCheckMemInterUnmap__)(struct RegisterMemory *, NvBool);
    NvBool (*__regmemShareCallback__)(struct RegisterMemory *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__regmemMapTo__)(struct RegisterMemory *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__regmemGetMapAddrSpace__)(struct RegisterMemory *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__regmemIsExportAllowed__)(struct RegisterMemory *);
    NvU32 (*__regmemGetRefCount__)(struct RegisterMemory *);
    void (*__regmemAddAdditionalDependants__)(struct RsClient *, struct RegisterMemory *, RsResourceRef *);
    NV_STATUS (*__regmemControl_Prologue__)(struct RegisterMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__regmemIsGpuMapAllowed__)(struct RegisterMemory *, struct OBJGPU *);
    NV_STATUS (*__regmemUnmapFrom__)(struct RegisterMemory *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__regmemControl_Epilogue__)(struct RegisterMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__regmemControlLookup__)(struct RegisterMemory *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__regmemControl__)(struct RegisterMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__regmemUnmap__)(struct RegisterMemory *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__regmemGetMemInterMapParams__)(struct RegisterMemory *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__regmemGetMemoryMappingDescriptor__)(struct RegisterMemory *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__regmemControlFilter__)(struct RegisterMemory *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__regmemControlSerialization_Prologue__)(struct RegisterMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__regmemIsReady__)(struct RegisterMemory *, NvBool);
    NV_STATUS (*__regmemCheckCopyPermissions__)(struct RegisterMemory *, struct OBJGPU *, struct Device *);
    void (*__regmemPreDestruct__)(struct RegisterMemory *);
    NV_STATUS (*__regmemIsDuplicate__)(struct RegisterMemory *, NvHandle, NvBool *);
    void (*__regmemControlSerialization_Epilogue__)(struct RegisterMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__regmemMap__)(struct RegisterMemory *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__regmemAccessCallback__)(struct RegisterMemory *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_RegisterMemory_TYPEDEF__
#define __NVOC_CLASS_RegisterMemory_TYPEDEF__
typedef struct RegisterMemory RegisterMemory;
#endif /* __NVOC_CLASS_RegisterMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_RegisterMemory
#define __nvoc_class_id_RegisterMemory 0x40d457
#endif /* __nvoc_class_id_RegisterMemory */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RegisterMemory;

#define __staticCast_RegisterMemory(pThis) \
    ((pThis)->__nvoc_pbase_RegisterMemory)

#ifdef __nvoc_reg_mem_h_disabled
#define __dynamicCast_RegisterMemory(pThis) ((RegisterMemory*)NULL)
#else //__nvoc_reg_mem_h_disabled
#define __dynamicCast_RegisterMemory(pThis) \
    ((RegisterMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RegisterMemory)))
#endif //__nvoc_reg_mem_h_disabled


NV_STATUS __nvoc_objCreateDynamic_RegisterMemory(RegisterMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RegisterMemory(RegisterMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_RegisterMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_RegisterMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define regmemCanCopy(pRegisterMemory) regmemCanCopy_DISPATCH(pRegisterMemory)
#define regmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) regmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define regmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) regmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define regmemMapTo(pResource, pParams) regmemMapTo_DISPATCH(pResource, pParams)
#define regmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) regmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define regmemIsExportAllowed(pMemory) regmemIsExportAllowed_DISPATCH(pMemory)
#define regmemGetRefCount(pResource) regmemGetRefCount_DISPATCH(pResource)
#define regmemAddAdditionalDependants(pClient, pResource, pReference) regmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define regmemControl_Prologue(pResource, pCallContext, pParams) regmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define regmemIsGpuMapAllowed(pMemory, pGpu) regmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define regmemUnmapFrom(pResource, pParams) regmemUnmapFrom_DISPATCH(pResource, pParams)
#define regmemControl_Epilogue(pResource, pCallContext, pParams) regmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define regmemControlLookup(pResource, pParams, ppEntry) regmemControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define regmemControl(pMemory, pCallContext, pParams) regmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define regmemUnmap(pMemory, pCallContext, pCpuMapping) regmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define regmemGetMemInterMapParams(pMemory, pParams) regmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define regmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) regmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define regmemControlFilter(pResource, pCallContext, pParams) regmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define regmemControlSerialization_Prologue(pResource, pCallContext, pParams) regmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define regmemIsReady(pMemory, bCopyConstructorContext) regmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define regmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) regmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define regmemPreDestruct(pResource) regmemPreDestruct_DISPATCH(pResource)
#define regmemIsDuplicate(pMemory, hMemory, pDuplicate) regmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define regmemControlSerialization_Epilogue(pResource, pCallContext, pParams) regmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define regmemMap(pMemory, pCallContext, pParams, pCpuMapping) regmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define regmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) regmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool regmemCanCopy_IMPL(struct RegisterMemory *pRegisterMemory);

static inline NvBool regmemCanCopy_DISPATCH(struct RegisterMemory *pRegisterMemory) {
    return pRegisterMemory->__regmemCanCopy__(pRegisterMemory);
}

static inline NV_STATUS regmemCheckMemInterUnmap_DISPATCH(struct RegisterMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__regmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NvBool regmemShareCallback_DISPATCH(struct RegisterMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__regmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS regmemMapTo_DISPATCH(struct RegisterMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__regmemMapTo__(pResource, pParams);
}

static inline NV_STATUS regmemGetMapAddrSpace_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__regmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool regmemIsExportAllowed_DISPATCH(struct RegisterMemory *pMemory) {
    return pMemory->__regmemIsExportAllowed__(pMemory);
}

static inline NvU32 regmemGetRefCount_DISPATCH(struct RegisterMemory *pResource) {
    return pResource->__regmemGetRefCount__(pResource);
}

static inline void regmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct RegisterMemory *pResource, RsResourceRef *pReference) {
    pResource->__regmemAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS regmemControl_Prologue_DISPATCH(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__regmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool regmemIsGpuMapAllowed_DISPATCH(struct RegisterMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__regmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NV_STATUS regmemUnmapFrom_DISPATCH(struct RegisterMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__regmemUnmapFrom__(pResource, pParams);
}

static inline void regmemControl_Epilogue_DISPATCH(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__regmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS regmemControlLookup_DISPATCH(struct RegisterMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__regmemControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS regmemControl_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__regmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS regmemUnmap_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__regmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS regmemGetMemInterMapParams_DISPATCH(struct RegisterMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__regmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS regmemGetMemoryMappingDescriptor_DISPATCH(struct RegisterMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__regmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS regmemControlFilter_DISPATCH(struct RegisterMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__regmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS regmemControlSerialization_Prologue_DISPATCH(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__regmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS regmemIsReady_DISPATCH(struct RegisterMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__regmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NV_STATUS regmemCheckCopyPermissions_DISPATCH(struct RegisterMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__regmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline void regmemPreDestruct_DISPATCH(struct RegisterMemory *pResource) {
    pResource->__regmemPreDestruct__(pResource);
}

static inline NV_STATUS regmemIsDuplicate_DISPATCH(struct RegisterMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__regmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline void regmemControlSerialization_Epilogue_DISPATCH(struct RegisterMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__regmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS regmemMap_DISPATCH(struct RegisterMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__regmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool regmemAccessCallback_DISPATCH(struct RegisterMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__regmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS regmemConstruct_IMPL(struct RegisterMemory *arg_pRegisterMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_regmemConstruct(arg_pRegisterMemory, arg_pCallContext, arg_pParams) regmemConstruct_IMPL(arg_pRegisterMemory, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_REG_MEM_NVOC_H_
