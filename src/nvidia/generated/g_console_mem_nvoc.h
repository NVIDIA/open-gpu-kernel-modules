#ifndef _G_CONSOLE_MEM_NVOC_H_
#define _G_CONSOLE_MEM_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_console_mem_nvoc.h"

#ifndef _CONSOLE_MEMORY_H_
#define _CONSOLE_MEMORY_H_

#include "mem_mgr/mem.h"

/*!
 * This class is used to create hMemory referencing reserved console memory
 */
#ifdef NVOC_CONSOLE_MEM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct ConsoleMemory {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Memory __nvoc_base_Memory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct ConsoleMemory *__nvoc_pbase_ConsoleMemory;
    NvBool (*__conmemCanCopy__)(struct ConsoleMemory *);
    NV_STATUS (*__conmemCheckMemInterUnmap__)(struct ConsoleMemory *, NvBool);
    NvBool (*__conmemShareCallback__)(struct ConsoleMemory *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__conmemMapTo__)(struct ConsoleMemory *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__conmemGetMapAddrSpace__)(struct ConsoleMemory *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__conmemIsExportAllowed__)(struct ConsoleMemory *);
    NvU32 (*__conmemGetRefCount__)(struct ConsoleMemory *);
    void (*__conmemAddAdditionalDependants__)(struct RsClient *, struct ConsoleMemory *, RsResourceRef *);
    NV_STATUS (*__conmemControl_Prologue__)(struct ConsoleMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__conmemIsGpuMapAllowed__)(struct ConsoleMemory *, struct OBJGPU *);
    NV_STATUS (*__conmemUnmapFrom__)(struct ConsoleMemory *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__conmemControl_Epilogue__)(struct ConsoleMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__conmemControlLookup__)(struct ConsoleMemory *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__conmemControl__)(struct ConsoleMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__conmemUnmap__)(struct ConsoleMemory *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__conmemGetMemInterMapParams__)(struct ConsoleMemory *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__conmemGetMemoryMappingDescriptor__)(struct ConsoleMemory *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__conmemControlFilter__)(struct ConsoleMemory *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__conmemControlSerialization_Prologue__)(struct ConsoleMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__conmemIsReady__)(struct ConsoleMemory *, NvBool);
    NV_STATUS (*__conmemCheckCopyPermissions__)(struct ConsoleMemory *, struct OBJGPU *, struct Device *);
    void (*__conmemPreDestruct__)(struct ConsoleMemory *);
    NV_STATUS (*__conmemIsDuplicate__)(struct ConsoleMemory *, NvHandle, NvBool *);
    void (*__conmemControlSerialization_Epilogue__)(struct ConsoleMemory *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__conmemMap__)(struct ConsoleMemory *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__conmemAccessCallback__)(struct ConsoleMemory *, struct RsClient *, void *, RsAccessRight);
};

#ifndef __NVOC_CLASS_ConsoleMemory_TYPEDEF__
#define __NVOC_CLASS_ConsoleMemory_TYPEDEF__
typedef struct ConsoleMemory ConsoleMemory;
#endif /* __NVOC_CLASS_ConsoleMemory_TYPEDEF__ */

#ifndef __nvoc_class_id_ConsoleMemory
#define __nvoc_class_id_ConsoleMemory 0xaac69e
#endif /* __nvoc_class_id_ConsoleMemory */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_ConsoleMemory;

#define __staticCast_ConsoleMemory(pThis) \
    ((pThis)->__nvoc_pbase_ConsoleMemory)

#ifdef __nvoc_console_mem_h_disabled
#define __dynamicCast_ConsoleMemory(pThis) ((ConsoleMemory*)NULL)
#else //__nvoc_console_mem_h_disabled
#define __dynamicCast_ConsoleMemory(pThis) \
    ((ConsoleMemory*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ConsoleMemory)))
#endif //__nvoc_console_mem_h_disabled


NV_STATUS __nvoc_objCreateDynamic_ConsoleMemory(ConsoleMemory**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ConsoleMemory(ConsoleMemory**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_ConsoleMemory(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_ConsoleMemory((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define conmemCanCopy(pConsoleMemory) conmemCanCopy_DISPATCH(pConsoleMemory)
#define conmemCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) conmemCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define conmemShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) conmemShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define conmemMapTo(pResource, pParams) conmemMapTo_DISPATCH(pResource, pParams)
#define conmemGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) conmemGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define conmemIsExportAllowed(pMemory) conmemIsExportAllowed_DISPATCH(pMemory)
#define conmemGetRefCount(pResource) conmemGetRefCount_DISPATCH(pResource)
#define conmemAddAdditionalDependants(pClient, pResource, pReference) conmemAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define conmemControl_Prologue(pResource, pCallContext, pParams) conmemControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define conmemIsGpuMapAllowed(pMemory, pGpu) conmemIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define conmemUnmapFrom(pResource, pParams) conmemUnmapFrom_DISPATCH(pResource, pParams)
#define conmemControl_Epilogue(pResource, pCallContext, pParams) conmemControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define conmemControlLookup(pResource, pParams, ppEntry) conmemControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define conmemControl(pMemory, pCallContext, pParams) conmemControl_DISPATCH(pMemory, pCallContext, pParams)
#define conmemUnmap(pMemory, pCallContext, pCpuMapping) conmemUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define conmemGetMemInterMapParams(pMemory, pParams) conmemGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define conmemGetMemoryMappingDescriptor(pMemory, ppMemDesc) conmemGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define conmemControlFilter(pResource, pCallContext, pParams) conmemControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define conmemControlSerialization_Prologue(pResource, pCallContext, pParams) conmemControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define conmemIsReady(pMemory, bCopyConstructorContext) conmemIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define conmemCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) conmemCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define conmemPreDestruct(pResource) conmemPreDestruct_DISPATCH(pResource)
#define conmemIsDuplicate(pMemory, hMemory, pDuplicate) conmemIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define conmemControlSerialization_Epilogue(pResource, pCallContext, pParams) conmemControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define conmemMap(pMemory, pCallContext, pParams, pCpuMapping) conmemMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define conmemAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) conmemAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool conmemCanCopy_IMPL(struct ConsoleMemory *pConsoleMemory);

static inline NvBool conmemCanCopy_DISPATCH(struct ConsoleMemory *pConsoleMemory) {
    return pConsoleMemory->__conmemCanCopy__(pConsoleMemory);
}

static inline NV_STATUS conmemCheckMemInterUnmap_DISPATCH(struct ConsoleMemory *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__conmemCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NvBool conmemShareCallback_DISPATCH(struct ConsoleMemory *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__conmemShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS conmemMapTo_DISPATCH(struct ConsoleMemory *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__conmemMapTo__(pResource, pParams);
}

static inline NV_STATUS conmemGetMapAddrSpace_DISPATCH(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__conmemGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool conmemIsExportAllowed_DISPATCH(struct ConsoleMemory *pMemory) {
    return pMemory->__conmemIsExportAllowed__(pMemory);
}

static inline NvU32 conmemGetRefCount_DISPATCH(struct ConsoleMemory *pResource) {
    return pResource->__conmemGetRefCount__(pResource);
}

static inline void conmemAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct ConsoleMemory *pResource, RsResourceRef *pReference) {
    pResource->__conmemAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS conmemControl_Prologue_DISPATCH(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__conmemControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool conmemIsGpuMapAllowed_DISPATCH(struct ConsoleMemory *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__conmemIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NV_STATUS conmemUnmapFrom_DISPATCH(struct ConsoleMemory *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__conmemUnmapFrom__(pResource, pParams);
}

static inline void conmemControl_Epilogue_DISPATCH(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__conmemControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS conmemControlLookup_DISPATCH(struct ConsoleMemory *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__conmemControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS conmemControl_DISPATCH(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__conmemControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS conmemUnmap_DISPATCH(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__conmemUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS conmemGetMemInterMapParams_DISPATCH(struct ConsoleMemory *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__conmemGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS conmemGetMemoryMappingDescriptor_DISPATCH(struct ConsoleMemory *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__conmemGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS conmemControlFilter_DISPATCH(struct ConsoleMemory *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__conmemControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS conmemControlSerialization_Prologue_DISPATCH(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__conmemControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS conmemIsReady_DISPATCH(struct ConsoleMemory *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__conmemIsReady__(pMemory, bCopyConstructorContext);
}

static inline NV_STATUS conmemCheckCopyPermissions_DISPATCH(struct ConsoleMemory *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__conmemCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline void conmemPreDestruct_DISPATCH(struct ConsoleMemory *pResource) {
    pResource->__conmemPreDestruct__(pResource);
}

static inline NV_STATUS conmemIsDuplicate_DISPATCH(struct ConsoleMemory *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__conmemIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline void conmemControlSerialization_Epilogue_DISPATCH(struct ConsoleMemory *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__conmemControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS conmemMap_DISPATCH(struct ConsoleMemory *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__conmemMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool conmemAccessCallback_DISPATCH(struct ConsoleMemory *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__conmemAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS conmemConstruct_IMPL(struct ConsoleMemory *arg_pConsoleMemory, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_conmemConstruct(arg_pConsoleMemory, arg_pCallContext, arg_pParams) conmemConstruct_IMPL(arg_pConsoleMemory, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CONSOLE_MEM_NVOC_H_
