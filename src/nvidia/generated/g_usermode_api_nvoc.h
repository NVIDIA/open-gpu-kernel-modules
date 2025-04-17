
#ifndef _G_USERMODE_API_NVOC_H_
#define _G_USERMODE_API_NVOC_H_

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

#pragma once
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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_USERMODE_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__UserModeApi;
struct NVOC_METADATA__Memory;
struct NVOC_VTABLE__UserModeApi;


struct UserModeApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__UserModeApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Memory __nvoc_base_Memory;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct Memory *__nvoc_pbase_Memory;    // mem super
    struct UserModeApi *__nvoc_pbase_UserModeApi;    // usrmode

    // Data members
    NvBool bPrivMapping;
    NvBool bInternalMmio;
};


// Vtable with 26 per-class function pointers
struct NVOC_VTABLE__UserModeApi {
    NvBool (*__usrmodeCanCopy__)(struct UserModeApi * /*this*/);  // virtual override (res) base (mem)
    NV_STATUS (*__usrmodeGetMemInterMapParams__)(struct UserModeApi * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual override (rmres) base (mem)
    NV_STATUS (*__usrmodeIsDuplicate__)(struct UserModeApi * /*this*/, NvHandle, NvBool *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__usrmodeGetMapAddrSpace__)(struct UserModeApi * /*this*/, CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__usrmodeControl__)(struct UserModeApi * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__usrmodeMap__)(struct UserModeApi * /*this*/, CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__usrmodeUnmap__)(struct UserModeApi * /*this*/, CALL_CONTEXT *, RsCpuMapping *);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__usrmodeCheckMemInterUnmap__)(struct UserModeApi * /*this*/, NvBool);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__usrmodeGetMemoryMappingDescriptor__)(struct UserModeApi * /*this*/, MEMORY_DESCRIPTOR **);  // virtual inherited (mem) base (mem)
    NV_STATUS (*__usrmodeCheckCopyPermissions__)(struct UserModeApi * /*this*/, struct OBJGPU *, struct Device *);  // inline virtual inherited (mem) base (mem) body
    NV_STATUS (*__usrmodeIsReady__)(struct UserModeApi * /*this*/, NvBool);  // virtual inherited (mem) base (mem)
    NvBool (*__usrmodeIsGpuMapAllowed__)(struct UserModeApi * /*this*/, struct OBJGPU *);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__usrmodeIsExportAllowed__)(struct UserModeApi * /*this*/);  // inline virtual inherited (mem) base (mem) body
    NvBool (*__usrmodeAccessCallback__)(struct UserModeApi * /*this*/, RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (mem)
    NvBool (*__usrmodeShareCallback__)(struct UserModeApi * /*this*/, RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__usrmodeControlSerialization_Prologue__)(struct UserModeApi * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__usrmodeControlSerialization_Epilogue__)(struct UserModeApi * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    NV_STATUS (*__usrmodeControl_Prologue__)(struct UserModeApi * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__usrmodeControl_Epilogue__)(struct UserModeApi * /*this*/, CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (mem)
    void (*__usrmodePreDestruct__)(struct UserModeApi * /*this*/);  // virtual inherited (res) base (mem)
    NV_STATUS (*__usrmodeControlFilter__)(struct UserModeApi * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (mem)
    NvBool (*__usrmodeIsPartialUnmapSupported__)(struct UserModeApi * /*this*/);  // inline virtual inherited (res) base (mem) body
    NV_STATUS (*__usrmodeMapTo__)(struct UserModeApi * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (mem)
    NV_STATUS (*__usrmodeUnmapFrom__)(struct UserModeApi * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (mem)
    NvU32 (*__usrmodeGetRefCount__)(struct UserModeApi * /*this*/);  // virtual inherited (res) base (mem)
    void (*__usrmodeAddAdditionalDependants__)(struct RsClient *, struct UserModeApi * /*this*/, RsResourceRef *);  // virtual inherited (res) base (mem)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__UserModeApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Memory metadata__Memory;
    const struct NVOC_VTABLE__UserModeApi vtable;
};

#ifndef __NVOC_CLASS_UserModeApi_TYPEDEF__
#define __NVOC_CLASS_UserModeApi_TYPEDEF__
typedef struct UserModeApi UserModeApi;
#endif /* __NVOC_CLASS_UserModeApi_TYPEDEF__ */

#ifndef __nvoc_class_id_UserModeApi
#define __nvoc_class_id_UserModeApi 0x6f57ec
#endif /* __nvoc_class_id_UserModeApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_UserModeApi;

#define __staticCast_UserModeApi(pThis) \
    ((pThis)->__nvoc_pbase_UserModeApi)

#ifdef __nvoc_usermode_api_h_disabled
#define __dynamicCast_UserModeApi(pThis) ((UserModeApi*) NULL)
#else //__nvoc_usermode_api_h_disabled
#define __dynamicCast_UserModeApi(pThis) \
    ((UserModeApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(UserModeApi)))
#endif //__nvoc_usermode_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_UserModeApi(UserModeApi**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_UserModeApi(UserModeApi**, Dynamic*, NvU32, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_UserModeApi(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_UserModeApi((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define usrmodeCanCopy_FNPTR(pUserModeApi) pUserModeApi->__nvoc_metadata_ptr->vtable.__usrmodeCanCopy__
#define usrmodeCanCopy(pUserModeApi) usrmodeCanCopy_DISPATCH(pUserModeApi)
#define usrmodeGetMemInterMapParams_FNPTR(pMemory) pMemory->__nvoc_metadata_ptr->vtable.__usrmodeGetMemInterMapParams__
#define usrmodeGetMemInterMapParams(pMemory, pParams) usrmodeGetMemInterMapParams_DISPATCH(pMemory, pParams)
#define usrmodeIsDuplicate_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsDuplicate__
#define usrmodeIsDuplicate(pMemory, hMemory, pDuplicate) usrmodeIsDuplicate_DISPATCH(pMemory, hMemory, pDuplicate)
#define usrmodeGetMapAddrSpace_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMapAddrSpace__
#define usrmodeGetMapAddrSpace(pMemory, pCallContext, mapFlags, pAddrSpace) usrmodeGetMapAddrSpace_DISPATCH(pMemory, pCallContext, mapFlags, pAddrSpace)
#define usrmodeControl_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memControl__
#define usrmodeControl(pMemory, pCallContext, pParams) usrmodeControl_DISPATCH(pMemory, pCallContext, pParams)
#define usrmodeMap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memMap__
#define usrmodeMap(pMemory, pCallContext, pParams, pCpuMapping) usrmodeMap_DISPATCH(pMemory, pCallContext, pParams, pCpuMapping)
#define usrmodeUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memUnmap__
#define usrmodeUnmap(pMemory, pCallContext, pCpuMapping) usrmodeUnmap_DISPATCH(pMemory, pCallContext, pCpuMapping)
#define usrmodeCheckMemInterUnmap_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckMemInterUnmap__
#define usrmodeCheckMemInterUnmap(pMemory, bSubdeviceHandleProvided) usrmodeCheckMemInterUnmap_DISPATCH(pMemory, bSubdeviceHandleProvided)
#define usrmodeGetMemoryMappingDescriptor_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memGetMemoryMappingDescriptor__
#define usrmodeGetMemoryMappingDescriptor(pMemory, ppMemDesc) usrmodeGetMemoryMappingDescriptor_DISPATCH(pMemory, ppMemDesc)
#define usrmodeCheckCopyPermissions_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memCheckCopyPermissions__
#define usrmodeCheckCopyPermissions(pMemory, pDstGpu, pDstDevice) usrmodeCheckCopyPermissions_DISPATCH(pMemory, pDstGpu, pDstDevice)
#define usrmodeIsReady_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsReady__
#define usrmodeIsReady(pMemory, bCopyConstructorContext) usrmodeIsReady_DISPATCH(pMemory, bCopyConstructorContext)
#define usrmodeIsGpuMapAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsGpuMapAllowed__
#define usrmodeIsGpuMapAllowed(pMemory, pGpu) usrmodeIsGpuMapAllowed_DISPATCH(pMemory, pGpu)
#define usrmodeIsExportAllowed_FNPTR(pMemory) pMemory->__nvoc_base_Memory.__nvoc_metadata_ptr->vtable.__memIsExportAllowed__
#define usrmodeIsExportAllowed(pMemory) usrmodeIsExportAllowed_DISPATCH(pMemory)
#define usrmodeAccessCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define usrmodeAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) usrmodeAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define usrmodeShareCallback_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresShareCallback__
#define usrmodeShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) usrmodeShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define usrmodeControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define usrmodeControlSerialization_Prologue(pResource, pCallContext, pParams) usrmodeControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define usrmodeControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define usrmodeControlSerialization_Epilogue(pResource, pCallContext, pParams) usrmodeControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define usrmodeControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define usrmodeControl_Prologue(pResource, pCallContext, pParams) usrmodeControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define usrmodeControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define usrmodeControl_Epilogue(pResource, pCallContext, pParams) usrmodeControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define usrmodePreDestruct_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define usrmodePreDestruct(pResource) usrmodePreDestruct_DISPATCH(pResource)
#define usrmodeControlFilter_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define usrmodeControlFilter(pResource, pCallContext, pParams) usrmodeControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define usrmodeIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define usrmodeIsPartialUnmapSupported(pResource) usrmodeIsPartialUnmapSupported_DISPATCH(pResource)
#define usrmodeMapTo_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define usrmodeMapTo(pResource, pParams) usrmodeMapTo_DISPATCH(pResource, pParams)
#define usrmodeUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define usrmodeUnmapFrom(pResource, pParams) usrmodeUnmapFrom_DISPATCH(pResource, pParams)
#define usrmodeGetRefCount_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define usrmodeGetRefCount(pResource) usrmodeGetRefCount_DISPATCH(pResource)
#define usrmodeAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_Memory.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define usrmodeAddAdditionalDependants(pClient, pResource, pReference) usrmodeAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool usrmodeCanCopy_DISPATCH(struct UserModeApi *pUserModeApi) {
    return pUserModeApi->__nvoc_metadata_ptr->vtable.__usrmodeCanCopy__(pUserModeApi);
}

static inline NV_STATUS usrmodeGetMemInterMapParams_DISPATCH(struct UserModeApi *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeGetMemInterMapParams__(pMemory, pParams);
}

static inline NV_STATUS usrmodeIsDuplicate_DISPATCH(struct UserModeApi *pMemory, NvHandle hMemory, NvBool *pDuplicate) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeIsDuplicate__(pMemory, hMemory, pDuplicate);
}

static inline NV_STATUS usrmodeGetMapAddrSpace_DISPATCH(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeGetMapAddrSpace__(pMemory, pCallContext, mapFlags, pAddrSpace);
}

static inline NV_STATUS usrmodeControl_DISPATCH(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeControl__(pMemory, pCallContext, pParams);
}

static inline NV_STATUS usrmodeMap_DISPATCH(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeMap__(pMemory, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS usrmodeUnmap_DISPATCH(struct UserModeApi *pMemory, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeUnmap__(pMemory, pCallContext, pCpuMapping);
}

static inline NV_STATUS usrmodeCheckMemInterUnmap_DISPATCH(struct UserModeApi *pMemory, NvBool bSubdeviceHandleProvided) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeCheckMemInterUnmap__(pMemory, bSubdeviceHandleProvided);
}

static inline NV_STATUS usrmodeGetMemoryMappingDescriptor_DISPATCH(struct UserModeApi *pMemory, MEMORY_DESCRIPTOR **ppMemDesc) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeGetMemoryMappingDescriptor__(pMemory, ppMemDesc);
}

static inline NV_STATUS usrmodeCheckCopyPermissions_DISPATCH(struct UserModeApi *pMemory, struct OBJGPU *pDstGpu, struct Device *pDstDevice) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeCheckCopyPermissions__(pMemory, pDstGpu, pDstDevice);
}

static inline NV_STATUS usrmodeIsReady_DISPATCH(struct UserModeApi *pMemory, NvBool bCopyConstructorContext) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeIsReady__(pMemory, bCopyConstructorContext);
}

static inline NvBool usrmodeIsGpuMapAllowed_DISPATCH(struct UserModeApi *pMemory, struct OBJGPU *pGpu) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeIsGpuMapAllowed__(pMemory, pGpu);
}

static inline NvBool usrmodeIsExportAllowed_DISPATCH(struct UserModeApi *pMemory) {
    return pMemory->__nvoc_metadata_ptr->vtable.__usrmodeIsExportAllowed__(pMemory);
}

static inline NvBool usrmodeAccessCallback_DISPATCH(struct UserModeApi *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__usrmodeAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool usrmodeShareCallback_DISPATCH(struct UserModeApi *pResource, RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_metadata_ptr->vtable.__usrmodeShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS usrmodeControlSerialization_Prologue_DISPATCH(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__usrmodeControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void usrmodeControlSerialization_Epilogue_DISPATCH(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__usrmodeControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS usrmodeControl_Prologue_DISPATCH(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__usrmodeControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void usrmodeControl_Epilogue_DISPATCH(struct UserModeApi *pResource, CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__usrmodeControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline void usrmodePreDestruct_DISPATCH(struct UserModeApi *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__usrmodePreDestruct__(pResource);
}

static inline NV_STATUS usrmodeControlFilter_DISPATCH(struct UserModeApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__usrmodeControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool usrmodeIsPartialUnmapSupported_DISPATCH(struct UserModeApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__usrmodeIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS usrmodeMapTo_DISPATCH(struct UserModeApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__usrmodeMapTo__(pResource, pParams);
}

static inline NV_STATUS usrmodeUnmapFrom_DISPATCH(struct UserModeApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__usrmodeUnmapFrom__(pResource, pParams);
}

static inline NvU32 usrmodeGetRefCount_DISPATCH(struct UserModeApi *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__usrmodeGetRefCount__(pResource);
}

static inline void usrmodeAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct UserModeApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__usrmodeAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool usrmodeCanCopy_IMPL(struct UserModeApi *pUserModeApi);

NV_STATUS usrmodeGetMemInterMapParams_IMPL(struct UserModeApi *pMemory, RMRES_MEM_INTER_MAP_PARAMS *pParams);

NV_STATUS usrmodeConstruct_IMPL(struct UserModeApi *arg_pUserModeApi, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_usrmodeConstruct(arg_pUserModeApi, arg_pCallContext, arg_pParams) usrmodeConstruct_IMPL(arg_pUserModeApi, arg_pCallContext, arg_pParams)
#undef PRIVATE_FIELD


#endif // _USERMODE_API_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_USERMODE_API_NVOC_H_
