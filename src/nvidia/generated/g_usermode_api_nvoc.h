#ifndef _G_USERMODE_API_NVOC_H_
#define _G_USERMODE_API_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_usermode_api_nvoc.h"

#ifndef _USERMODE_API_H_
#define _USERMODE_API_H_

#include "core/core.h"
#include "mem_mgr/mem.h"
#include "gpu/gpu.h"
#include "nvoc/utility.h"

/*!
 * RM internal class representing USERMODE_A classes. We inherit Memory here instead of GpuResource
 * because GpuResource can only map regmem, whereas for HOPPER+, we will need to map sysmem with an
 * effective address space of fbmem if the user requests the GMMU/BAR1 mapping for the VF pages.
 */
#ifdef NVOC_USERMODE_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct UserModeApi {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Memory __nvoc_base_Memory;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct Memory *__nvoc_pbase_Memory;
    struct UserModeApi *__nvoc_pbase_UserModeApi;
    NvBool (*__usrmodeCanCopy__)(struct UserModeApi *);
    NV_STATUS (*__usrmodeGetMemInterMapParams__)(struct UserModeApi *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__usrmodeCheckMemInterUnmap__)(struct UserModeApi *, NvBool);
    NvBool (*__usrmodeShareCallback__)(struct UserModeApi *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__usrmodeMapTo__)(struct UserModeApi *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__usrmodeGetMapAddrSpace__)(struct UserModeApi *, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    NvBool (*__usrmodeIsExportAllowed__)(struct UserModeApi *);
    NvU32 (*__usrmodeGetRefCount__)(struct UserModeApi *);
    void (*__usrmodeAddAdditionalDependants__)(struct RsClient *, struct UserModeApi *, RsResourceRef *);
    NV_STATUS (*__usrmodeControl_Prologue__)(struct UserModeApi *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__usrmodeIsGpuMapAllowed__)(struct UserModeApi *, struct OBJGPU *);
    NV_STATUS (*__usrmodeUnmapFrom__)(struct UserModeApi *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__usrmodeControl_Epilogue__)(struct UserModeApi *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__usrmodeControlLookup__)(struct UserModeApi *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__usrmodeControl__)(struct UserModeApi *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__usrmodeUnmap__)(struct UserModeApi *, CALL_CONTEXT *, RsCpuMapping *);
    NV_STATUS (*__usrmodeGetMemoryMappingDescriptor__)(struct UserModeApi *, MEMORY_DESCRIPTOR **);
    NV_STATUS (*__usrmodeControlFilter__)(struct UserModeApi *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__usrmodeControlSerialization_Prologue__)(struct UserModeApi *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__usrmodeIsReady__)(struct UserModeApi *, NvBool);
    NV_STATUS (*__usrmodeCheckCopyPermissions__)(struct UserModeApi *, struct OBJGPU *, struct Device *);
    void (*__usrmodePreDestruct__)(struct UserModeApi *);
    NV_STATUS (*__usrmodeIsDuplicate__)(struct UserModeApi *, NvHandle, NvBool *);
    void (*__usrmodeControlSerialization_Epilogue__)(struct UserModeApi *, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__usrmodeMap__)(struct UserModeApi *, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);
    NvBool (*__usrmodeAccessCallback__)(struct UserModeApi *, struct RsClient *, void *, RsAccessRight);
    NvBool bPrivMapping;
    NvBool bInternalMmio;
};

#ifndef __NVOC_CLASS_UserModeApi_TYPEDEF__
#define __NVOC_CLASS_UserModeApi_TYPEDEF__
typedef struct UserModeApi UserModeApi;
#endif /* __NVOC_CLASS_UserModeApi_TYPEDEF__ */

#ifndef __nvoc_class_id_UserModeApi
#define __nvoc_class_id_UserModeApi 0x6f57ec
#endif /* __nvoc_class_id_UserModeApi */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_UserModeApi;

#define __staticCast_UserModeApi(pThis) \
    ((pThis)->__nvoc_pbase_UserModeApi)

#ifdef __nvoc_usermode_api_h_disabled
#define __dynamicCast_UserModeApi(pThis) ((UserModeApi*)NULL)
#else //__nvoc_usermode_api_h_disabled
#define __dynamicCast_UserModeApi(pThis) \
    ((UserModeApi*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(UserModeApi)))
#endif //__nvoc_usermode_api_h_disabled


NV_STATUS __nvoc_objCreateDynamic_UserModeApi(UserModeApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_UserModeApi(UserModeApi**, Dynamic*, NvU32, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_UserModeApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_UserModeApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define usrmodeCanCopy(pUserModeApi) usrmodeCanCopy_DISPATCH(pUserModeApi)
#define usrmodeGetMemInterMapParams(pMemory, pParams) usrmodeGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define usrmodeCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) usrmodeCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define usrmodeShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) usrmodeShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define usrmodeMapTo(pResource, pParams) usrmodeMapTo_DISPATCH(pResource, pParams)
#define usrmodeGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) usrmodeGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define usrmodeIsExportAllowed(pMemory) usrmodeIsExportAllowed_DISPATCH(pMemory)
#define usrmodeGetRefCount(pResource) usrmodeGetRefCount_DISPATCH(pResource)
#define usrmodeAddAdditionalDependants(pClient, pResource, pReference) usrmodeAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define usrmodeControl_Prologue(pResource, pCallContext, pParams) usrmodeControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define usrmodeIsGpuMapAllowed(pMemory, pGpu) usrmodeIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define usrmodeUnmapFrom(pResource, pParams) usrmodeUnmapFrom_DISPATCH(pResource, pParams)
#define usrmodeControl_Epilogue(pResource, pCallContext, pParams) usrmodeControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define usrmodeControlLookup(pResource, pParams, ppEntry) usrmodeControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define usrmodeControl(pMemory, pCallContext, pParams) usrmodeControl_DISPATCH(pMemory, pCallContext, pParams)
#define usrmodeUnmap(pMemory, pCallContext, pCpuMapping) usrmodeUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define usrmodeGetMemoryMappingDescriptor(pMemory, ppMemDesc) usrmodeGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define usrmodeControlFilter(pResource, pCallContext, pParams) usrmodeControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define usrmodeControlSerialization_Prologue(pResource, pCallContext, pParams) usrmodeControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define usrmodeIsReady(pMemory, bCopyConstructorContext) usrmodeIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define usrmodeCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) usrmodeCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define usrmodePreDestruct(pResource) usrmodePreDestruct_DISPATCH(pResource)
#define usrmodeIsDuplicate(pMemory, hMemory, pDuplicate) usrmodeIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define usrmodeControlSerialization_Epilogue(pResource, pCallContext, pParams) usrmodeControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define usrmodeMap(pMemory, pCallContext, pParams, pCpuMapping) usrmodeMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define usrmodeAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) usrmodeAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NvBool usrmodeCanCopy_IMPL(struct UserModeApi *pUserModeApi);

static inline NvBool usrmodeCanCopy_DISPATCH(struct UserModeApi *pUserModeApi) {
    return pUserModeApi->__usrmodeCanCopy__(pUserModeApi);
}

NV_STATUS usrmodeGetMemInterMapParams_IMPL(struct UserModeApi *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);

static inline NV_STATUS usrmodeGetMemInterMapParams_DISPATCH(struct UserModeApi *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__usrmodeGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS usrmodeCheckMemInterUnmap_DISPATCH(struct UserModeApi *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__usrmodeCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NvBool usrmodeShareCallback_DISPATCH(struct UserModeApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__usrmodeShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS usrmodeMapTo_DISPATCH(struct UserModeApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__usrmodeMapTo__(pResource, pParams);
}

static inline NV_STATUS usrmodeGetMapAddrSpace_DISPATCH(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__usrmodeGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool usrmodeIsExportAllowed_DISPATCH(struct UserModeApi *pMemory) {
    return pMemory->__usrmodeIsExportAllowed__(pMemory);
}

static inline NvU32 usrmodeGetRefCount_DISPATCH(struct UserModeApi *pResource) {
    return pResource->__usrmodeGetRefCount__(pResource);
}

static inline void usrmodeAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct UserModeApi *pResource, RsResourceRef *pReference) {
    pResource->__usrmodeAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS usrmodeControl_Prologue_DISPATCH(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__usrmodeControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool usrmodeIsGpuMapAllowed_DISPATCH(struct UserModeApi *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__usrmodeIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NV_STATUS usrmodeUnmapFrom_DISPATCH(struct UserModeApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__usrmodeUnmapFrom__(pResource, pParams);
}

static inline void usrmodeControl_Epilogue_DISPATCH(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__usrmodeControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS usrmodeControlLookup_DISPATCH(struct UserModeApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__usrmodeControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS usrmodeControl_DISPATCH(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__usrmodeControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS usrmodeUnmap_DISPATCH(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__usrmodeUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS usrmodeGetMemoryMappingDescriptor_DISPATCH(struct UserModeApi *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__usrmodeGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS usrmodeControlFilter_DISPATCH(struct UserModeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__usrmodeControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS usrmodeControlSerialization_Prologue_DISPATCH(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__usrmodeControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS usrmodeIsReady_DISPATCH(struct UserModeApi *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__usrmodeIsReady__(pMemory, bCopyConstructorContext);
}

static inline NV_STATUS usrmodeCheckCopyPermissions_DISPATCH(struct UserModeApi *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__usrmodeCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline void usrmodePreDestruct_DISPATCH(struct UserModeApi *pResource) {
    pResource->__usrmodePreDestruct__(pResource);
}

static inline NV_STATUS usrmodeIsDuplicate_DISPATCH(struct UserModeApi *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__usrmodeIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline void usrmodeControlSerialization_Epilogue_DISPATCH(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__usrmodeControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS usrmodeMap_DISPATCH(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__usrmodeMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NvBool usrmodeAccessCallback_DISPATCH(struct UserModeApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__usrmodeAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS usrmodeConstruct_IMPL(struct UserModeApi *arg_pUserModeApi, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_usrmodeConstruct(arg_pUserModeApi, arg_pCallContext, arg_pParams) usrmodeConstruct_IMPL(arg_pUserModeApi, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // _USERMODE_API_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_USERMODE_API_NVOC_H_
