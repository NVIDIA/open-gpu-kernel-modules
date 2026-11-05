
#ifndef _G_P2P_API_NVOC_H_
#define _G_P2P_API_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2009-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_p2p_api_nvoc.h"


#ifndef _P2P_API_H_
#define _P2P_API_H_

#include "core/core.h"
#include "rmapi/client.h"

//
// Definitions for P2PApi.attributes.
// Need to keep NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE sync with P2P_CONNECTIVITY in p2p_caps.h
//
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE                     3:0
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_UNKNOWN             0x0
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_PCIE                0x1
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_PCIE_BAR1           0x2
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_NVLINK              0x3
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_NVLINK_INDIRECT     0x4
#define NV_P2PAPI_ATTRIBUTES_CONNECTION_TYPE_C2C                 0x5
#define NV_P2PAPI_ATTRIBUTES_LINK_TYPE                           4:4
#define NV_P2PAPI_ATTRIBUTES_LINK_TYPE_GPA                       0x0
#define NV_P2PAPI_ATTRIBUTES_LINK_TYPE_SPA                       0x1
#define NV_P2PAPI_ATTRIBUTES_REMOTE_EGM                          5:5
#define NV_P2PAPI_ATTRIBUTES_REMOTE_EGM_NO                       0x0
#define NV_P2PAPI_ATTRIBUTES_REMOTE_EGM_YES                      0x1


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_P2P_API_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__P2PApi;
struct NVOC_METADATA__RmResource;


struct P2PApi {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__P2PApi *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RmResource __nvoc_base_RmResource;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsResource *__nvoc_pbase_RsResource;    // res super^2
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^2
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super
    struct P2PApi *__nvoc_pbase_P2PApi;    // p2papi

    // Data members
    struct OBJGPU *peer1;
    struct OBJGPU *peer2;
    NvU32 localGfid;
    NvU32 remoteGfid;
    NvU32 peerId1;
    NvU32 peerId2;
    NvU32 egmPeerId1;
    NvU32 egmPeerId2;
    NvU32 attributes;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__P2PApi {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RmResource metadata__RmResource;
};

#ifndef __nvoc_class_id_P2PApi
#define __nvoc_class_id_P2PApi 0x3982b7u
typedef struct P2PApi P2PApi;
#endif /* __nvoc_class_id_P2PApi */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_P2PApi;

#define __staticCast_P2PApi(pThis) \
    ((pThis)->__nvoc_pbase_P2PApi)

#ifdef __nvoc_p2p_api_h_disabled
#define __dynamicCast_P2PApi(pThis) ((P2PApi*) NULL)
#else //__nvoc_p2p_api_h_disabled
#define __dynamicCast_P2PApi(pThis) \
    ((P2PApi*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(P2PApi)))
#endif //__nvoc_p2p_api_h_disabled

NV_STATUS __nvoc_objCreateDynamic_P2PApi(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_P2PApi(P2PApi**, Dynamic*, NvU32, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __objCreate_P2PApi(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams) \
    __nvoc_objCreate_P2PApi((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pCallContext, pParams)


// Wrapper macros for implementation functions
NV_STATUS p2papiConstruct_IMPL(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
#define __nvoc_p2papiConstruct(pResource, pCallContext, pParams) p2papiConstruct_IMPL(pResource, pCallContext, pParams)

void p2papiDestruct_IMPL(struct P2PApi *pResource);
#define __nvoc_p2papiDestruct(pResource) p2papiDestruct_IMPL(pResource)


// Wrapper macros for halified functions
#define p2papiAccessCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__
#define p2papiAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) p2papiAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define p2papiShareCallback_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__
#define p2papiShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) p2papiShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define p2papiGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define p2papiGetMemInterMapParams(pRmResource, pParams) p2papiGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define p2papiCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define p2papiCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) p2papiCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define p2papiGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define p2papiGetMemoryMappingDescriptor(pRmResource, ppMemDesc) p2papiGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define p2papiControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__
#define p2papiControlSerialization_Prologue(pResource, pCallContext, pParams) p2papiControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define p2papiControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__
#define p2papiControlSerialization_Epilogue(pResource, pCallContext, pParams) p2papiControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define p2papiControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__
#define p2papiControl_Prologue(pResource, pCallContext, pParams) p2papiControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define p2papiControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__
#define p2papiControl_Epilogue(pResource, pCallContext, pParams) p2papiControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define p2papiCanCopy_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define p2papiCanCopy(pResource) p2papiCanCopy_DISPATCH(pResource)
#define p2papiIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define p2papiIsDuplicate(pResource, hMemory, pDuplicate) p2papiIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define p2papiPreDestruct_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define p2papiPreDestruct(pResource) p2papiPreDestruct_DISPATCH(pResource)
#define p2papiControl_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__
#define p2papiControl(pResource, pCallContext, pParams) p2papiControl_DISPATCH(pResource, pCallContext, pParams)
#define p2papiControlFilter_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define p2papiControlFilter(pResource, pCallContext, pParams) p2papiControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define p2papiMap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__
#define p2papiMap(pResource, pCallContext, pParams, pCpuMapping) p2papiMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define p2papiUnmap_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__
#define p2papiUnmap(pResource, pCallContext, pCpuMapping) p2papiUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define p2papiIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define p2papiIsPartialUnmapSupported(pResource) p2papiIsPartialUnmapSupported_DISPATCH(pResource)
#define p2papiMapTo_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define p2papiMapTo(pResource, pParams) p2papiMapTo_DISPATCH(pResource, pParams)
#define p2papiUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define p2papiUnmapFrom(pResource, pParams) p2papiUnmapFrom_DISPATCH(pResource, pParams)
#define p2papiGetRefCount_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define p2papiGetRefCount(pResource) p2papiGetRefCount_DISPATCH(pResource)
#define p2papiAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define p2papiAddAdditionalDependants(pClient, pResource, pReference) p2papiAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool p2papiAccessCallback_DISPATCH(struct P2PApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAccessCallback__(staticCast(pResource, RsResource), pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool p2papiShareCallback_DISPATCH(struct P2PApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resShareCallback__(staticCast(pResource, RsResource), pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS p2papiGetMemInterMapParams_DISPATCH(struct P2PApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__(staticCast(pRmResource, RmResource), pParams);
}

static inline NV_STATUS p2papiCheckMemInterUnmap_DISPATCH(struct P2PApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__(staticCast(pRmResource, RmResource), bSubdeviceHandleProvided);
}

static inline NV_STATUS p2papiGetMemoryMappingDescriptor_DISPATCH(struct P2PApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__(staticCast(pRmResource, RmResource), ppMemDesc);
}

static inline NV_STATUS p2papiControlSerialization_Prologue_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void p2papiControlSerialization_Epilogue_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlSerialization_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS p2papiControl_Prologue_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Prologue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline void p2papiControl_Epilogue_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl_Epilogue__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NvBool p2papiCanCopy_DISPATCH(struct P2PApi *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__(staticCast(pResource, RsResource));
}

static inline NV_STATUS p2papiIsDuplicate_DISPATCH(struct P2PApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__(staticCast(pResource, RsResource), hMemory, pDuplicate);
}

static inline void p2papiPreDestruct_DISPATCH(struct P2PApi *pResource) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__(staticCast(pResource, RsResource));
}

static inline NV_STATUS p2papiControl_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControl__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS p2papiControlFilter_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__(staticCast(pResource, RsResource), pCallContext, pParams);
}

static inline NV_STATUS p2papiMap_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMap__(staticCast(pResource, RsResource), pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS p2papiUnmap_DISPATCH(struct P2PApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmap__(staticCast(pResource, RsResource), pCallContext, pCpuMapping);
}

static inline NvBool p2papiIsPartialUnmapSupported_DISPATCH(struct P2PApi *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__(staticCast(pResource, RsResource));
}

static inline NV_STATUS p2papiMapTo_DISPATCH(struct P2PApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__(staticCast(pResource, RsResource), pParams);
}

static inline NV_STATUS p2papiUnmapFrom_DISPATCH(struct P2PApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__(staticCast(pResource, RsResource), pParams);
}

static inline NvU32 p2papiGetRefCount_DISPATCH(struct P2PApi *pResource) {
    return pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__(staticCast(pResource, RsResource));
}

static inline void p2papiAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct P2PApi *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__(pClient, staticCast(pResource, RsResource), pReference);
}

// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _P2P_API_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_P2P_API_NVOC_H_
