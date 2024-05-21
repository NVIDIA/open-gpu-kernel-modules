
#ifndef _G_RS_RESOURCE_NVOC_H_
#define _G_RS_RESOURCE_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

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
#include "g_rs_resource_nvoc.h"

#ifndef _RS_RESOURCE_H_
#define _RS_RESOURCE_H_

#include "nvport/nvport.h"
#include "resserv/resserv.h"
#include "nvoc/object.h"
#include "resserv/rs_access_map.h"

#ifdef __cplusplus
extern "C" {
#endif


struct RsSession;

#ifndef __NVOC_CLASS_RsSession_TYPEDEF__
#define __NVOC_CLASS_RsSession_TYPEDEF__
typedef struct RsSession RsSession;
#endif /* __NVOC_CLASS_RsSession_TYPEDEF__ */

#ifndef __nvoc_class_id_RsSession
#define __nvoc_class_id_RsSession 0x830d90
#endif /* __nvoc_class_id_RsSession */



/**
 * @defgroup RsResource
 * @addtogroup RsResource
 * @{*/

#define ALLOC_STATE_INTERNAL_CLIENT_HANDLE   NVBIT(5)

/*
 * Locking operations for lock-metering
 */
#define RS_LOCK_TRACE_INVALID     1
#define RS_LOCK_TRACE_ACQUIRE     1
#define RS_LOCK_TRACE_RELEASE     2
#define RS_LOCK_TRACE_ALLOC       3
#define RS_LOCK_TRACE_FREE        4
#define RS_LOCK_TRACE_CTRL        5
#define RS_LOCK_TRACE_MAP         6
#define RS_LOCK_TRACE_UNMAP       7

/**
 * Context information for top-level, resource-level, and client-level locking
 * operations
 */
struct RS_LOCK_INFO
{
    struct RsClient *pClient;              ///< Pointer to client that was locked (if any)
    struct RsClient *pSecondClient;        ///< Pointer to second client, for dual-client locking
    RsResourceRef *pContextRef;     ///< User-defined reference
    struct RsSession *pSession;            ///< Session object to be locked, if any
    NvU32 flags;                    ///< RS_LOCK_FLAGS_*
    NvU32 state;                    ///< RS_LOCK_STATE_*
    NvU32 gpuMask;
    NvU8  traceOp;                  ///< RS_LOCK_TRACE_* operation for lock-metering
    NvU32 traceClassId;             ///< Class of initial resource that was locked for lock metering
};

struct RS_RES_ALLOC_PARAMS_INTERNAL
{
    NvHandle hClient;       ///< [in] The handle of the resource's client
    NvHandle hParent;       ///< [in] The handle of the resource's parent. This may be a client or another resource.
    NvHandle hResource;     ///< [inout] Server will assign a handle if this is 0, or else try the value provided
    NvU32 externalClassId;  ///< [in] External class ID of resource
    NvHandle hDomain;       ///< UNUSED

    // Internal use only
    RS_LOCK_INFO           *pLockInfo;        ///< [inout] Locking flags and state
    struct RsClient               *pClient;          ///< [out] Cached client
    RsResourceRef          *pResourceRef;     ///< [out] Cached resource reference
    NvU32                   allocFlags;       ///< [in] Allocation flags
    NvU32                   allocState;       ///< [inout] Allocation state
    API_SECURITY_INFO      *pSecInfo;

    void                   *pAllocParams;     ///< [in] Copied-in allocation parameters
    NvU32                   paramsSize;       ///< [in] Copied-in allocation parameters size

    // ... Dupe alloc
    struct RsClient               *pSrcClient;       ///< The client that is sharing the resource
    RsResourceRef          *pSrcRef;          ///< Reference to the resource that will be shared

    RS_ACCESS_MASK         *pRightsRequested; ///< [in] Access rights requested on the new resource
    // Buffer for storing contents of user mask. Do not use directly, use pRightsRequested instead.
    RS_ACCESS_MASK          rightsRequestedCopy;

    RS_ACCESS_MASK         *pRightsRequired;  ///< [in] Access rights required to alloc this object type
};

struct RS_RES_DUP_PARAMS_INTERNAL
{
    NvHandle            hClientSrc;            ///< [in] The handle of the source resource's client
    NvHandle            hResourceSrc;          ///< [in] The handle of the source resource.
    NvHandle            hClientDst;            ///< [in] The handle of the destination resource's client (may be different from source client)
    NvHandle            hParentDst;            ///< [in] The handle of the destination resource's parent.
    NvHandle            hResourceDst;          ///< [inout] The handle of the destination resource. Generated if 0.
    void               *pShareParams;          ///< [in] Copied-in sharing parameters
    NvU32               flags;                 ///< [in] Flags to denote special cases ( Bug: 2859347 to track removal)
    // Internal use only
    struct RsClient           *pSrcClient;
    RsResourceRef      *pSrcRef;
    API_SECURITY_INFO  *pSecInfo;              ///< [in] Security info
    RS_LOCK_INFO       *pLockInfo;             ///< [inout] Locking flags and state
};

struct RS_RES_SHARE_PARAMS_INTERNAL
{
    NvHandle            hClient;            ///< [in] The handle of the owner's client
    NvHandle            hResource;          ///< [in] The handle of the resource.
    RS_SHARE_POLICY    *pSharePolicy;       ///< [in] The policy to share with

    // Internal use only
    API_SECURITY_INFO  *pSecInfo;           ///< [in] Security info
    RS_LOCK_INFO       *pLockInfo;          ///< [inout] Locking flags and state
};

#define RS_IS_COPY_CTOR(pParams) ((pParams)->pSrcRef != NULL)

struct RS_RES_FREE_PARAMS_INTERNAL
{
    NvHandle           hClient;         ///< [in] The handle of the resource's client
    NvHandle           hResource;       ///< [in] The handle of the resource
    NvBool             bInvalidateOnly; ///< [in] Free the resource, but don't release its handle
    NvHandle           hDomain;         ///< UNUSED

    // Internal use only
    NvBool             bHiPriOnly;      ///< [in] Only free if this is a high priority resources
    NvBool             bDisableOnly;    ///< [in] Disable the target instead of freeing it (only applies to clients)
    RS_LOCK_INFO      *pLockInfo;       ///< [inout] Locking flags and state
    NvU32              freeFlags;       ///< [in] Flags for the free operation
    NvU32              freeState;       ///< [inout] Free state
    RsResourceRef     *pResourceRef;    ///< [inout] Cached RsResourceRef
    NV_STATUS          status;          ///< [out] Status of free operation
    API_SECURITY_INFO *pSecInfo;        ///< [in] Security info
};

struct NVOC_EXPORTED_METHOD_DEF;

struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



struct OBJGPUGRP;

#ifndef __NVOC_CLASS_OBJGPUGRP_TYPEDEF__
#define __NVOC_CLASS_OBJGPUGRP_TYPEDEF__
typedef struct OBJGPUGRP OBJGPUGRP;
#endif /* __NVOC_CLASS_OBJGPUGRP_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPUGRP
#define __nvoc_class_id_OBJGPUGRP 0xe40531
#endif /* __nvoc_class_id_OBJGPUGRP */



//
// RS_RES_CONTROL_PARAMS
//
// This structure encapsulates data sent to the cmd-specific rmctrl
// handlers.  Along with the arguments supplied by the requesting
// client (hClient, hObject, cmd, pParams, paramSize).
//
struct RS_RES_CONTROL_PARAMS_INTERNAL
{
    NvHandle                hClient;          // client-specified NV01_ROOT object handle
    NvHandle                hObject;          // client-specified object handle
    NvU32                   cmd;              // client-specified command #
    NvU32                   flags;            // flags related to control call execution
    void                   *pParams;          // client-specified params (in kernel space)
    NvU32                   paramsSize;       // client-specified size of pParams in bytes

    NvHandle                hParent;          // handle of hObject parent
    struct OBJGPU                 *pGpu;             // ptr to OBJGPU struct if applicable
    struct OBJGPUGRP              *pGpuGrp;          // ptr to OBJGPUGRP struct if applicable
    RsResourceRef          *pResourceRef;     // ptr to RsResourceRef if object is managed by
                                              // Resource Server
    API_SECURITY_INFO       secInfo;          // information on privilege level and pointer location (user/kernel)
    RS_LOCK_INFO           *pLockInfo;        ///< [inout] Locking flags and state
    RS_CONTROL_COOKIE      *pCookie;
    NvBool                  bInternal;        // True if control call was not issued from an external client
    NvBool                  bDeferredApi;     // Indicates ctrl is being dispatched via deferred API

    struct RS_RES_CONTROL_PARAMS_INTERNAL *pLegacyParams; // RS-TODO removeme
};

struct RS_RES_DTOR_PARAMS
{
    CALL_CONTEXT                 *pFreeContext;
    RS_RES_FREE_PARAMS_INTERNAL  *pFreeParams;
};

/**
 * Base class for all resources. Mostly a pure virtual interface which
 * should be overridden to implement resource specific behavior.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RS_RESOURCE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct RsResource {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct RsResource *__nvoc_pbase_RsResource;    // res

    // Vtable with 18 per-object function pointers
    NvBool (*__resCanCopy__)(struct RsResource * /*this*/);  // virtual
    NV_STATUS (*__resIsDuplicate__)(struct RsResource * /*this*/, NvHandle, NvBool *);  // virtual
    void (*__resPreDestruct__)(struct RsResource * /*this*/);  // virtual
    NV_STATUS (*__resControl__)(struct RsResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual
    NV_STATUS (*__resControlFilter__)(struct RsResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual
    NV_STATUS (*__resControlSerialization_Prologue__)(struct RsResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual
    void (*__resControlSerialization_Epilogue__)(struct RsResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual
    NV_STATUS (*__resControl_Prologue__)(struct RsResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual
    void (*__resControl_Epilogue__)(struct RsResource * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual
    NV_STATUS (*__resMap__)(struct RsResource * /*this*/, struct CALL_CONTEXT *, RS_CPU_MAP_PARAMS *, RsCpuMapping *);  // virtual
    NV_STATUS (*__resUnmap__)(struct RsResource * /*this*/, struct CALL_CONTEXT *, RsCpuMapping *);  // virtual
    NvBool (*__resIsPartialUnmapSupported__)(struct RsResource * /*this*/);  // inline virtual body
    NV_STATUS (*__resMapTo__)(struct RsResource * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual
    NV_STATUS (*__resUnmapFrom__)(struct RsResource * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual
    NvU32 (*__resGetRefCount__)(struct RsResource * /*this*/);  // virtual
    NvBool (*__resAccessCallback__)(struct RsResource * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual
    NvBool (*__resShareCallback__)(struct RsResource * /*this*/, struct RsClient *, RsResourceRef *, RS_SHARE_POLICY *);  // virtual
    void (*__resAddAdditionalDependants__)(struct RsClient *, struct RsResource * /*this*/, RsResourceRef *);  // virtual

    // Data members
    RsResourceRef *pResourceRef;
    struct RS_RES_DTOR_PARAMS dtorParams;
    NvBool bConstructed;
};

#ifndef __NVOC_CLASS_RsResource_TYPEDEF__
#define __NVOC_CLASS_RsResource_TYPEDEF__
typedef struct RsResource RsResource;
#endif /* __NVOC_CLASS_RsResource_TYPEDEF__ */

#ifndef __nvoc_class_id_RsResource
#define __nvoc_class_id_RsResource 0xd551cb
#endif /* __nvoc_class_id_RsResource */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

#define __staticCast_RsResource(pThis) \
    ((pThis)->__nvoc_pbase_RsResource)

#ifdef __nvoc_rs_resource_h_disabled
#define __dynamicCast_RsResource(pThis) ((RsResource*)NULL)
#else //__nvoc_rs_resource_h_disabled
#define __dynamicCast_RsResource(pThis) \
    ((RsResource*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RsResource)))
#endif //__nvoc_rs_resource_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RsResource(RsResource**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RsResource(RsResource**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_RsResource(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_RsResource((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define resCanCopy_FNPTR(pResource) pResource->__resCanCopy__
#define resCanCopy(pResource) resCanCopy_DISPATCH(pResource)
#define resIsDuplicate_FNPTR(pResource) pResource->__resIsDuplicate__
#define resIsDuplicate(pResource, hMemory, pDuplicate) resIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define resPreDestruct_FNPTR(pResource) pResource->__resPreDestruct__
#define resPreDestruct(pResource) resPreDestruct_DISPATCH(pResource)
#define resControl_FNPTR(pResource) pResource->__resControl__
#define resControl(pResource, pCallContext, pParams) resControl_DISPATCH(pResource, pCallContext, pParams)
#define resControlFilter_FNPTR(pResource) pResource->__resControlFilter__
#define resControlFilter(pResource, pCallContext, pParams) resControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define resControlSerialization_Prologue_FNPTR(pResource) pResource->__resControlSerialization_Prologue__
#define resControlSerialization_Prologue(pResource, pCallContext, pParams) resControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define resControlSerialization_Epilogue_FNPTR(pResource) pResource->__resControlSerialization_Epilogue__
#define resControlSerialization_Epilogue(pResource, pCallContext, pParams) resControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define resControl_Prologue_FNPTR(pResource) pResource->__resControl_Prologue__
#define resControl_Prologue(pResource, pCallContext, pParams) resControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define resControl_Epilogue_FNPTR(pResource) pResource->__resControl_Epilogue__
#define resControl_Epilogue(pResource, pCallContext, pParams) resControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define resMap_FNPTR(pResource) pResource->__resMap__
#define resMap(pResource, pCallContext, pParams, pCpuMapping) resMap_DISPATCH(pResource, pCallContext, pParams, pCpuMapping)
#define resUnmap_FNPTR(pResource) pResource->__resUnmap__
#define resUnmap(pResource, pCallContext, pCpuMapping) resUnmap_DISPATCH(pResource, pCallContext, pCpuMapping)
#define resIsPartialUnmapSupported_FNPTR(pResource) pResource->__resIsPartialUnmapSupported__
#define resIsPartialUnmapSupported(pResource) resIsPartialUnmapSupported_DISPATCH(pResource)
#define resMapTo_FNPTR(pResource) pResource->__resMapTo__
#define resMapTo(pResource, pParams) resMapTo_DISPATCH(pResource, pParams)
#define resUnmapFrom_FNPTR(pResource) pResource->__resUnmapFrom__
#define resUnmapFrom(pResource, pParams) resUnmapFrom_DISPATCH(pResource, pParams)
#define resGetRefCount_FNPTR(pResource) pResource->__resGetRefCount__
#define resGetRefCount(pResource) resGetRefCount_DISPATCH(pResource)
#define resAccessCallback_FNPTR(pResource) pResource->__resAccessCallback__
#define resAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) resAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define resShareCallback_FNPTR(pResource) pResource->__resShareCallback__
#define resShareCallback(pResource, pInvokingClient, pParentRef, pSharePolicy) resShareCallback_DISPATCH(pResource, pInvokingClient, pParentRef, pSharePolicy)
#define resAddAdditionalDependants_FNPTR(pResource) pResource->__resAddAdditionalDependants__
#define resAddAdditionalDependants(pClient, pResource, pReference) resAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)

// Dispatch functions
static inline NvBool resCanCopy_DISPATCH(struct RsResource *pResource) {
    return pResource->__resCanCopy__(pResource);
}

static inline NV_STATUS resIsDuplicate_DISPATCH(struct RsResource *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__resIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void resPreDestruct_DISPATCH(struct RsResource *pResource) {
    pResource->__resPreDestruct__(pResource);
}

static inline NV_STATUS resControl_DISPATCH(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__resControl__(pResource, pCallContext, pParams);
}

static inline NV_STATUS resControlFilter_DISPATCH(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__resControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS resControlSerialization_Prologue_DISPATCH(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__resControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void resControlSerialization_Epilogue_DISPATCH(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__resControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS resControl_Prologue_DISPATCH(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__resControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void resControl_Epilogue_DISPATCH(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__resControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS resMap_DISPATCH(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return pResource->__resMap__(pResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS resUnmap_DISPATCH(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return pResource->__resUnmap__(pResource, pCallContext, pCpuMapping);
}

static inline NvBool resIsPartialUnmapSupported_DISPATCH(struct RsResource *pResource) {
    return pResource->__resIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS resMapTo_DISPATCH(struct RsResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__resMapTo__(pResource, pParams);
}

static inline NV_STATUS resUnmapFrom_DISPATCH(struct RsResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__resUnmapFrom__(pResource, pParams);
}

static inline NvU32 resGetRefCount_DISPATCH(struct RsResource *pResource) {
    return pResource->__resGetRefCount__(pResource);
}

static inline NvBool resAccessCallback_DISPATCH(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__resAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NvBool resShareCallback_DISPATCH(struct RsResource *pResource, struct RsClient *pInvokingClient, RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pResource->__resShareCallback__(pResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline void resAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct RsResource *pResource, RsResourceRef *pReference) {
    pResource->__resAddAdditionalDependants__(pClient, pResource, pReference);
}

NvBool resCanCopy_IMPL(struct RsResource *pResource);

NV_STATUS resIsDuplicate_IMPL(struct RsResource *pResource, NvHandle hMemory, NvBool *pDuplicate);

void resPreDestruct_IMPL(struct RsResource *pResource);

NV_STATUS resControl_IMPL(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS resControlFilter_IMPL(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS resControlSerialization_Prologue_IMPL(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

void resControlSerialization_Epilogue_IMPL(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS resControl_Prologue_IMPL(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

void resControl_Epilogue_IMPL(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

NV_STATUS resMap_IMPL(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);

NV_STATUS resUnmap_IMPL(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);

static inline NvBool resIsPartialUnmapSupported_bf6dfa(struct RsResource *pResource) {
    return ((NvBool)(0 != 0));
}

NV_STATUS resMapTo_IMPL(struct RsResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);

NV_STATUS resUnmapFrom_IMPL(struct RsResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);

NvU32 resGetRefCount_IMPL(struct RsResource *pResource);

NvBool resAccessCallback_IMPL(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);

NvBool resShareCallback_IMPL(struct RsResource *pResource, struct RsClient *pInvokingClient, RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);

void resAddAdditionalDependants_IMPL(struct RsClient *pClient, struct RsResource *pResource, RsResourceRef *pReference);

NV_STATUS resConstruct_IMPL(struct RsResource *arg_pResource, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_resConstruct(arg_pResource, arg_pCallContext, arg_pParams) resConstruct_IMPL(arg_pResource, arg_pCallContext, arg_pParams)
void resDestruct_IMPL(struct RsResource *pResource);

#define __nvoc_resDestruct(pResource) resDestruct_IMPL(pResource)
NV_STATUS resSetFreeParams_IMPL(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_FREE_PARAMS_INTERNAL *pParams);

#ifdef __nvoc_rs_resource_h_disabled
static inline NV_STATUS resSetFreeParams(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_FREE_PARAMS_INTERNAL *pParams) {
    NV_ASSERT_FAILED_PRECOMP("RsResource was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_resource_h_disabled
#define resSetFreeParams(pResource, pCallContext, pParams) resSetFreeParams_IMPL(pResource, pCallContext, pParams)
#endif //__nvoc_rs_resource_h_disabled

NV_STATUS resGetFreeParams_IMPL(struct RsResource *pResource, struct CALL_CONTEXT **ppCallContext, struct RS_RES_FREE_PARAMS_INTERNAL **ppParams);

#ifdef __nvoc_rs_resource_h_disabled
static inline NV_STATUS resGetFreeParams(struct RsResource *pResource, struct CALL_CONTEXT **ppCallContext, struct RS_RES_FREE_PARAMS_INTERNAL **ppParams) {
    NV_ASSERT_FAILED_PRECOMP("RsResource was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_resource_h_disabled
#define resGetFreeParams(pResource, ppCallContext, ppParams) resGetFreeParams_IMPL(pResource, ppCallContext, ppParams)
#endif //__nvoc_rs_resource_h_disabled

NV_STATUS resControlLookup_IMPL(struct RsResource *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry);

#ifdef __nvoc_rs_resource_h_disabled
static inline NV_STATUS resControlLookup(struct RsResource *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    NV_ASSERT_FAILED_PRECOMP("RsResource was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_rs_resource_h_disabled
#define resControlLookup(pResource, pParams, ppEntry) resControlLookup_IMPL(pResource, pParams, ppEntry)
#endif //__nvoc_rs_resource_h_disabled

#undef PRIVATE_FIELD


/* @} */


struct OBJGPU;

#ifndef __NVOC_CLASS_OBJGPU_TYPEDEF__
#define __NVOC_CLASS_OBJGPU_TYPEDEF__
typedef struct OBJGPU OBJGPU;
#endif /* __NVOC_CLASS_OBJGPU_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cb
#endif /* __nvoc_class_id_OBJGPU */



/**
 * @defgroup RsCpuMapping
 * @addtogroup RsCpuMapping
 * @{*/
struct RsCpuMapping
{
    NvU64 offset;
    NvU64 length;
    NvU32 flags;
    NvP64 pLinearAddress;
    RsResourceRef *pContextRef;      ///< Context resource that may be needed for the mapping
    void *pContext;                  ///< Additional context data for the mapping
    NvU32 processId;

    RS_CPU_MAPPING_PRIVATE *pPrivate; ///< Opaque struct allocated and freed by resserv on behalf of the user
};
MAKE_LIST(RsCpuMappingList, RsCpuMapping);

/**
 * CPU mapping parameters
 */
struct RS_CPU_MAP_PARAMS
{
    NvHandle                hClient;
    NvHandle                hDevice;
    NvHandle                hMemory;
    NvU64                   offset;         ///< [in] Offset into the resource
    NvU64                   length;         ///< [in] Size of the region to map
    NvP64                  *ppCpuVirtAddr;
    NvU32                   flags;          ///< [in] Resource-specific flags

    // Passed from RM into CpuMapping
    NvU32                   protect;        ///< [in] Protection flags
    NvBool                  bKernel;

    /// [in] hContext Handle of resource that provides a context for the mapping (e.g., subdevice for channel map)
    NvHandle hContext;

    RS_LOCK_INFO   *pLockInfo;              ///< [inout] Locking flags and state
    API_SECURITY_INFO *pSecInfo;            ///< [in] Security Info
};

/**
 * CPU unmapping params for resource server tests
 */
struct RS_CPU_UNMAP_PARAMS
{
    NvHandle                hClient;
    NvHandle                hDevice;
    NvHandle                hMemory;
    NvP64                   pLinearAddress; ///< [in] Address of mapped memory
    NvU32                   flags;          ///< [in] Resource-specific flags
    NvU32                   processId;
    NvBool                  bTeardown;      ///< [in] Unmap operation is due to client teardown

    /// [in] hContext Handle of resource that provides a context for the mapping (e.g., subdevice for channel map)
    NvHandle                hContext;

    // RM-only
    void                   *pProcessHandle;

    NvBool        (*fnFilter)(RsCpuMapping*);   ///< [in] Mapping-filter function
    RS_LOCK_INFO   *pLockInfo;                  ///< [inout] Locking flags and state
    API_SECURITY_INFO *pSecInfo;                ///< [in] Security Info
};

/**
 * CPU mapping back-reference
 */
struct RS_CPU_MAPPING_BACK_REF
{
    RsCpuMapping *pCpuMapping;  ///< Mapping linked to this backref
    RsResourceRef *pBackRef;    ///< Resource reference with mapping
};
MAKE_LIST(RsCpuMappingBackRefList, RS_CPU_MAPPING_BACK_REF);
/* @} */

/**
 * @defgroup RsInterMapping
 * @addtogroup RsInterMapping
 * @{*/
struct RS_INTER_MAP_PARAMS
{
    NvHandle        hClient;
    NvHandle        hMapper;
    NvHandle        hMappable;
    NvHandle        hDevice;
    NvU64           offset;
    NvU64           length;
    NvU32           flags;
    NvU64           dmaOffset;              ///< [inout] RS-TODO rename this
    void           *pMemDesc;               ///< [out]

    // Internal use only
    RS_LOCK_INFO   *pLockInfo;              ///< [inout] Locking flags and state
    API_SECURITY_INFO *pSecInfo;            ///< [in] Security Info

    RS_INTER_MAP_PRIVATE *pPrivate;         ///< Opaque struct controlled by caller
};

struct RS_INTER_UNMAP_PARAMS
{
    NvHandle        hClient;
    NvHandle        hMapper;
    NvHandle        hDevice;
    NvU32           flags;
    NvU64           dmaOffset;              ///< [in] RS-TODO rename this
    NvU64           size;

    // Internal use only
    NvHandle        hMappable;
    void           *pMemDesc;               ///< MEMORY_DESCRIPTOR *
    RS_LOCK_INFO   *pLockInfo;              ///< [inout] Locking flags and state
    API_SECURITY_INFO *pSecInfo;            ///< [in] Security Info

    RS_INTER_UNMAP_PRIVATE *pPrivate;       ///< Opaque struct controlled by caller
};

/**
 * Inter-mapping information
 * Used to keep track of inter-mappings and unmap them on free
 */
struct RsInterMapping
{
    // RsResourceRef *pMapperRef     ///< (Implied) the resource that created and owns this mapping (this resource)
    RsResourceRef *pMappableRef;     ///< The resource being mapped by the mapper (e.g. hMemory)
    RsResourceRef *pContextRef;      ///< A resource used to provide additional context for the mapping (e.g. hDevice)
    NvU32 flags;                     ///< Flags passed when mapping, same flags also passed when unmapping
    NvU64 dmaOffset;
    NvU64 size;
    void *pMemDesc;
};
MAKE_LIST(RsInterMappingList, RsInterMapping);

/**
 * Inter-mapping back-reference
 */
struct RS_INTER_MAPPING_BACK_REF
{
    RsResourceRef *pMapperRef;       ///< Resource reference with mapping
    RsInterMapping *pMapping;        ///< Pointer to the inter-mapping linked to this backref
};
MAKE_LIST(RsInterMappingBackRefList, RS_INTER_MAPPING_BACK_REF);
/* @} */

typedef struct RS_RESOURCE_DESC RS_RESOURCE_DESC;
RS_RESOURCE_DESC *RsResInfoByExternalClassId(NvU32);
NvU32 RsResInfoGetInternalClassId(const RS_RESOURCE_DESC *);

/**
 * A reference to a resource that has been allocated in RM.
 */
struct RsResourceRef
{
    struct RsClient   *pClient;            ///< Pointer to the client that owns the ref
    struct RsResource *pResource;          ///< Pointer to the actual resource
    NvHandle    hResource;          ///< Resource handle
    struct RsResourceRef *pParentRef; ///< Parent resource reference
    RsIndex     childRefMap;        ///< Child reference multi-map: { internalClassId -> { handle -> RsResourceRef } }

    /**
     * Cached reference multi-map: { internalClassId -> { handle -> RsResourceRef } }
     *
     * The resource reference cache is a one-way association between this resource reference and
     * any other resource reference. Resource server does not populate the cache so it is up to the
     * resource implementation to manage it. clientRefIter can be used to iterate this cache.
     */
    RsIndex     cachedRefMap;

    /**
     * Dependants reference multi-map: { internalClassId -> { handle -> RsResourceRef } }
     *
     * A map of all resources that strongly depend on this resource.
     */
    RsIndex     depRefMap;

    /**
     * Dependants back-reference multi-map: { internalClassId -> { handle -> RsResourceRef } }
     *
     * AKA dependencies map
     *
     * A map of all resources that this resource strongly depends on.
     */
    RsIndex     depBackRefMap;

    /**
     * Policy under which this resource can be shared with other clients
     */
    RsShareList     sharePolicyList;
    NvBool          bSharePolicyListModified;

    /**
     * A mask of the access rights that the owner client has on this object.
     */
    RS_ACCESS_MASK accessMask;

    const RS_RESOURCE_DESC *pResourceDesc; ///< Cached pointer to the resource descriptor
    NvU32       internalClassId;    ///< Internal resource class id
    NvU32       externalClassId;    ///< External resource class id
    NvU32       depth;              ///< The depth of this reference in the resource graph
    NvBool      bInvalidated;       ///< Reference has been freed but not removed yet

    RsCpuMappingList cpuMappings;   ///< List of CPU mappings to the resource from this resource reference
    RsCpuMappingBackRefList backRefs;  ///< List of references that have this reference as a mapping context

    RsInterMappingList interMappings;        ///< List of inter-resource mappings created by this resource
    RsInterMappingBackRefList interBackRefs; ///< List of inter-resource mappings this resource has been mapped into

    struct RsSession *pSession;          ///< If set, this ref depends on a shared session
    struct RsSession *pDependantSession; ///< If set, this ref is depended on by a shared session

    ListNode   freeNode;        ///< Links to the client's pendingFreeList
};
MAKE_MAP(RsRefMap, RsResourceRef);
MAKE_INTRUSIVE_LIST(RsRefFreeList, RsResourceRef, freeNode);


// Iterator data structure to save state while walking through a list
struct RS_ITERATOR
{
    union
    {
        RsRefMapIter      mapIt; ///< Map iterator for all resource references under a client
        RsIndexIter idxIt; ///< Index iterator for child references of a resource reference
    };

    struct RsClient *pClient;
    RsResourceRef *pScopeRef;    ///< Reference to the resource that limits the scope of iteration
    NvU32 internalClassId;
    RsResourceRef *pResourceRef; ///< Resource ref that is being iterated over
    NvU8 type;                   ///< RS_ITERATE_*
    NvBool bExactMatch;          ///< If true, internalClassId must match exactly; if false, also match classes derived from the internalClassId
};

// Iterator data structure to save state while walking through a resource tree in pre-order
struct RS_ORDERED_ITERATOR
{
    NvS8 depth; ///< Depth of index stack; special value of -1 implies that the scope reference should be iterated over as well
    RsIndexIter idxIt[RS_MAX_RESOURCE_DEPTH+1]; ///< Stack of index iterators for child references of a resource reference

    struct RsClient *pClient;
    RsResourceRef *pScopeRef;    ///< Reference to the resource that limits the scope of iteration
    NvU32 internalClassId;
    NvBool bExactMatch;          ///< If true, internalClassId must match exactly; if false, also match classes derived from the internalClassId

    RsResourceRef *pResourceRef; ///< Resource ref that is being iterated over
};

/**
 * Macro for looking up a reference from a resource
 */
#define RES_GET_REF(pResource) (staticCast((pResource), RsResource)->pResourceRef)

/**
 * Macro for looking up a resource handle from a resource
 */
#define RES_GET_HANDLE(pResource) (RES_GET_REF(pResource)->hResource)

/**
 * Macro for looking up a resource's external class from a resource
 */
#define RES_GET_EXT_CLASS_ID(pResource) (RES_GET_REF(pResource)->externalClassId)

/**
 * Macro for looking up a resource's parent handle from a resource
 */
#define RES_GET_PARENT_HANDLE(pResource) (RES_GET_REF(pResource)->pParentRef->hResource)

/**
 * Macro for looking up a client from a resource
 */
#define RES_GET_CLIENT(pResource) (RES_GET_REF(pResource)->pClient)

/**
 * Macro for looking up a client handle from a resource
 */
#define RES_GET_CLIENT_HANDLE(pResource) (RES_GET_REF(pResource)->pClient->hClient)

/**
 * Find a CPU mapping owned by a resource reference
 *
 * @param[in]   pResourceRef
 * @param[in]   pAddress The CPU virtual address of the mapping to search for
 * @param[out]  ppMapping The returned mapping
 */
NV_STATUS refFindCpuMapping(RsResourceRef *pResourceRef, NvP64 pAddress, RsCpuMapping **ppMapping);

/**
 * Find a CPU mapping owned by a resource reference
 *
 * @param[in]   pResourceRef
 * @param[in]   pAddress The CPU virtual address of the mapping to search for
 * @param[in]   fnFilter A user-provided filtering function that determines which mappings to ignore.
 *                       If fnFilter is provided, then we will only return mappings for which fnFilter(mapping) returns NV_TRUE
 *                       All mappings will be searched over if fnFilter is NULL.
 * @param[out]  ppMapping The returned mapping
 * @param[in]   fnFilter A user-provided filtering function that determines which mappings to ignore.
 *                       If fnFilter is provided, then we will only return mappings for which fnFilter(mapping) returns NV_TRUE
 *                       All mappings will be searched over if fnFilter is NULL.
 */
NV_STATUS refFindCpuMappingWithFilter(RsResourceRef *pResourceRef, NvP64 pAddress, NvBool (*fnFilter)(RsCpuMapping*), RsCpuMapping **ppMapping);

/**
 * Find the first child object of given type
 *
 * @param[in]   pParentRef
 * @param[in]   internalClassId
 * @param[in]   bExactMatch If true, internalClassId must match exactly; if false, also match classes derived from the internalClassId
 * @param[out]  pResourceRef The returned RsResourceRef (Optional)
 */
NV_STATUS refFindChildOfType(RsResourceRef *pParentRef, NvU32 internalClassId, NvBool bExactMatch, RsResourceRef **ppResourceRef);

/**
 * Traverse up the reference parent-child hierarchy to find an ancestor reference of a given type
 *
 * @param[in]   pDescendantRef
 * @param[in]   internalClassId
 * @param[out]  ppAncestorRef The returned RsResourceRef (Optional)
 */
NV_STATUS refFindAncestorOfType(RsResourceRef *pDescendantRef, NvU32 internalClassId, RsResourceRef **ppAncestorRef);

/**
 * Traverse up the reference parent-child hierarchy to find if a ref is a descendant of a given ancestor ref
 *
 * @param[in]  pDescendantRef The node to start searching from (not included in the search)
 * @param[in]  pAncestorRef The node to search for in the parent-child hierarchy
 */
NvBool refHasAncestor(RsResourceRef *pDescendantRef, RsResourceRef *pAncestorRef);

/**
 * Add a new mapping to a reference's mapping list
 * @param[in] pResourceRef The reference to add a mapping to
 * @param[in] pMapParams The parameters used to initialize the mapping
 * @param[in] pContextRef A reference to a resource that provides a context for the mapping
 * @param[out] ppMapping Pointer to the allocated mapping [optional]
 */
NV_STATUS refAddMapping(RsResourceRef *pResourceRef, RS_CPU_MAP_PARAMS *pMapParams,
                        RsResourceRef *pContextRef, RsCpuMapping **ppMapping);

/**
 * Remove an existing mapping from a reference's mapping list and remove back-references to the mapping.
 * @param[in] pResourceRef The reference to add a mapping to
 * @param[in] pMapping Pointer to the allocated mapping
 */
void refRemoveMapping(RsResourceRef *pResourceRef, RsCpuMapping *pMapping);

/**
 * Allocate the user-controlled private pointer within the RsCpuMapping struct.
 * Resserv will call this function to alloc the private struct when the mapping is created
 * @param[in] pMapParams The parameters which were used to create the mapping
 * @param[inout] pMapping Pointer to the mapping whose private struct should be allocated
 */
NV_STATUS refAllocCpuMappingPrivate(RS_CPU_MAP_PARAMS *pMapParams, RsCpuMapping *pMapping);

/**
 * Free the user-controlled private pointer within the RsCpuMapping struct.
 * Resserv will call this function to free the private struct when the mapping is removed
 * @param[inout] pMapping Pointer to the mapping whose private struct should be freed
 */
void refFreeCpuMappingPrivate(RsCpuMapping *pMapping);

/**
 * Add a dependency between this resource reference and a dependent reference.
 * If this reference is freed, the dependent will be invalidated and torn down.
 *
 * @note Dependencies are implicit between a parent resource reference and child resource reference
 * @note No circular dependency checking is performed
 */
NV_STATUS refAddDependant(RsResourceRef *pResourceRef, RsResourceRef *pDependantRef);

/**
 * Remove the dependency between this resource reference and a dependent resource reference.
 */
void refRemoveDependant(RsResourceRef *pResourceRef, RsResourceRef *pDependantRef);

/**
 * Find, Add, or Remove an inter-mapping between two resources to the Mapper's list of inter-mappings
 * Inter-mappings are stored in the Mapper, and are matched by both the MappableRef and offset.
 *
 * @param[in] pMapperRef The reference which owns the inter-mapping
 * @param[in] pMappableRef The reference which was mapped from to create the inter-mapping
 *                         If NULL, will be ignored while matching inter-mappings
 * @param[in] dmaOffset The offset value assigned while mapping, used to identify mappings
 * @param[in] pContextRef A reference used during mapping and locking for additional context, used to identify mappings
 * @param[inout] ppMapping Writes the resulting inter-mapping, if successfully created (Add) or found (Find)
 * @param[in] pMapping The inter-mapping to remove (Remove)
 */
NV_STATUS refAddInterMapping(RsResourceRef *pMapperRef, RsResourceRef *pMappableRef, RsResourceRef *pContextRef, RsInterMapping **ppMapping);
void      refRemoveInterMapping(RsResourceRef *pMapperRef, RsInterMapping *pMapping);

/**
 * Store a resource reference in another reference's cache.
 * @param[in]   pParentRef The resource reference that owns the cache
 * @param[in]   pResourceRef The resource reference to store in the cache
 */
NV_STATUS refCacheRef(RsResourceRef *pParentRef, RsResourceRef *pResourceRef);

/**
 * Remove a resource reference from another reference's cache
 * @param[in]   pParentRef The resource reference that owns the cache
 * @param[in]   pResourceRef The resource reference to de-index
 */
NV_STATUS refUncacheRef(RsResourceRef *pParentRef, RsResourceRef *pResourceRef);

/**
 * Determine whether a reference is queued for removal
 * @param[in]   pResourceRef
 * @param[in]   pClient
 */
NvBool    refPendingFree(RsResourceRef *pResourceRef, struct RsClient *pClient);


#ifdef __cplusplus
}
#endif

#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_RS_RESOURCE_NVOC_H_
