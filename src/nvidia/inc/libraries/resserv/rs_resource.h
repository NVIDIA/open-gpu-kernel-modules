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

class RsSession;

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
    RsClient *pClient;              ///< Pointer to client that was locked (if any)
    RsClient *pSecondClient;        ///< Pointer to second client, for dual-client locking
    RsResourceRef *pContextRef;     ///< User-defined reference
    RsSession *pSession;            ///< Session object to be locked, if any
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
    RsClient               *pClient;          ///< [out] Cached client
    RsResourceRef          *pResourceRef;     ///< [out] Cached resource reference
    NvU32                   allocFlags;       ///< [in] Allocation flags
    NvU32                   allocState;       ///< [inout] Allocation state
    API_SECURITY_INFO      *pSecInfo;

    void                   *pAllocParams;     ///< [in] Copied-in allocation parameters
    NvU32                   paramsSize;       ///< [in] Copied-in allocation parameters size

    // ... Dupe alloc
    RsClient               *pSrcClient;       ///< The client that is sharing the resource
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
    RsClient           *pSrcClient;
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
class OBJGPU;
class OBJGPUGRP;

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
    OBJGPU                 *pGpu;             // ptr to OBJGPU struct if applicable
    OBJGPUGRP              *pGpuGrp;          // ptr to OBJGPUGRP struct if applicable
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
NVOC_PREFIX(res) class RsResource : Object
{
public:
// private:

    /**
     * Back-reference to the RsResourceRef that owns this object
     */
    RsResourceRef *pResourceRef;

    /**
     * Params for dtor
     */
    RS_RES_DTOR_PARAMS dtorParams;

    /**
     * Flag that indicates whether the RsResource was constructed. If params to
     * resConstruct are null the Resource ctor and dtor will be skipped. This is
     * only added for migration where the entire class hierarchy can't be
     * converted at once.
     *
     * RS-TODO: Remove once migrations are finished (added initially for
     * DmaObject)
     */
    NvBool bConstructed;

public:

    /**
     * Resource initializer
     * @param[in]   pResource Resource object to init
     * @param[in]   pCallContext
     * @param[in]   params Resource create parameters
     */
    NV_STATUS resConstruct(RsResource *pResource, CALL_CONTEXT *pCallContext, RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

    /**
     * Returns TRUE if the resource can be copied
     */
    virtual NvBool resCanCopy(RsResource *pResource);

    /**
     * Returns TRUE if the resources are duplicates
     */
    virtual NV_STATUS resIsDuplicate(RsResource *pResource, NvHandle hMemory, NvBool *pDuplicate);

    /**
     * Resource destructor
     * @param[in]   pResource Resource object to destruct
     */
    void resDestruct(RsResource *pResource);

    /**
     * Resource destructor prologue (occurs before mappings are torn-down)
     * @param[in]   pResource Resource object to destruct
     */
    virtual void resPreDestruct(RsResource *pResource);

    /**
     * Resource dtors take no parameters, so set them here.
     * @param[in]   pResource
     * @param[in]   pCallContext
     * @param[in]   params Resource destroy parameters
     */
    NV_STATUS resSetFreeParams(RsResource *pResource, CALL_CONTEXT *pCallContext, RS_RES_FREE_PARAMS_INTERNAL *pParams);

    /**
     * Resource dtors take no parameters, so get them here.
     * @param[in]   pResource
     * @param[out]  ppCallContext
     * @param[out]  ppParams Resource destroy parameters
     */
    NV_STATUS resGetFreeParams(RsResource *pResource, CALL_CONTEXT **ppCallContext, RS_RES_FREE_PARAMS_INTERNAL **ppParams);

    /**
     * Lookup a control call entry from a NVOC export table
     *
     * @param[in]       pResource
     * @param[in]       pParams
     * @param[out]      ppEntry
     */
    NV_STATUS resControlLookup(RsResource *pResource,
                               RS_RES_CONTROL_PARAMS_INTERNAL *pParams,
                               const struct NVOC_EXPORTED_METHOD_DEF **ppEntry);

    /**
     * Dispatch resource control call
     * @param[in]   pResource
     * @param[in]   pCallContext
     * @param[in]   pParams
     */
    virtual NV_STATUS resControl(RsResource *pResource, CALL_CONTEXT *pCallContext,
                                 RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

    /**
     * Early filter for control calls we don't want to service on a particular platform
     *
     * @param[in]   pResource
     * @param[in]   pCallContext
     * @param[in]   pParams
     */
    virtual NV_STATUS resControlFilter(RsResource *pResource, CALL_CONTEXT *pCallContext,
                                       RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

    /**
     * Serialize the control parameters if they are going to GSP/Host, not serialized, and support serialization
     * Or
     * Deserialize the control parameters if necessary and replace the inner params pointer with the deserialized params
     *
     * @param[in]   pResource
     * @param[in]   pCallContext
     * @param[in]   pParams
     */
    virtual NV_STATUS resControlSerialization_Prologue(RsResource *pResource, CALL_CONTEXT *pCallContext,
                                                       RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

    /**
     * Deserialize the parameters returned from GSP if client did not pass serialized params
     * Or
     * Serialize the control parameters if client expects it and restore the original inner params pointer
     *
     * @param[in]   pResource
     * @param[in]   pCallContext
     * @param[in]   pParams
     */
    virtual void resControlSerialization_Epilogue(RsResource *pResource, CALL_CONTEXT *pCallContext,
                                                  RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

    /**
     * Operations performed right before the control call is executed. Default stubbed.
     *
     * @param[in]   pResource
     * @param[in]   pCallContext
     * @param[in]   pParams
     */
    virtual NV_STATUS resControl_Prologue(RsResource *pResource, CALL_CONTEXT *pCallContext,
                                 RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

    /**
     * Operations performed right after the control call is executed. No return value. (void)
     *
     * @param[in]   pResource
     * @param[in]   pCallContext
     * @param[in]   pParams
     */
    virtual void resControl_Epilogue(RsResource *pResource, CALL_CONTEXT *pCallContext,
                                 RS_RES_CONTROL_PARAMS_INTERNAL *pParams);

    /**
     * Creates a mapping of the underlying resource in the physical address space of the requested process.
     *
     * The difference between serverResMap and resMap is that resMap provides a locked physical address
     * and serverResMap creates a virtual mapping to the physical address. For virtualization, the
     * tandem resource servers should be able to map a host physical address in a guest user space
     * VA without any resource-specific VA mapping code.
     *
     * Not all resources support mapping.
     *
     * @param[in]       pResource Resource to map
     * @param[in]       pCallContext
     * @param[inout]    pCpuMapping
     */
    virtual NV_STATUS resMap(RsResource *pResource,
                             CALL_CONTEXT *pCallContext,
                             RS_CPU_MAP_PARAMS *pParams,
                             RsCpuMapping *pCpuMapping);

    /**
     * Release a virtual address mapping
     * @param[in]   pResource Resource to map
     * @param[in]   pCallContext
     * @param[in]   pCpuMapping
     */
    virtual NV_STATUS resUnmap(RsResource *pResource, CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);

    /**
     * Returns true if partial unmap is supported by the resource
     * If true, resUnmapFrom() can be called to unmap a mapping partially
     */
    virtual NvBool resIsPartialUnmapSupported(RsResource *pResource) { return NV_FALSE; }

     /**
     * Maps to this resource from another resource
     * Not all resources can be mapped to, in such a case returns NV_ERR_INVALID_OBJECT_HANDLE
     *
     * @param[in] pResource
     * @param[inout] pParams
     */
    virtual NV_STATUS resMapTo(RsResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);

    /**
     * Unmaps a resource mapped to this resource
     * Not all resources can be unmapped, in such a case returns NV_ERR_INVALID_OBJECT_HANDLE
     *
     * @param[in] pResource
     * @param[in] pParams
     */
    virtual NV_STATUS resUnmapFrom(RsResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);

    /**
     * Gets a refcount for any underlying shared resource
     * @returns refcount
     */
    virtual NvU32 resGetRefCount(RsResource *pResource);

    /**
     * Decides whether the invoking client should be granted an access right on this resource.
     *
     * The purpose of providing this function is to provide subclassed resources the ability
     * to set custom policies for granting access rights. These policies can be implemented
     * based on the ambient privilege of the caller, such as the PID.
     *
     * @param[in]  pResource The resource for which the access right will be granted
     * @param[in]  pInvokingClient The client requesting the access right
     * @param[in]  pAllocParams The alloc params struct passed into the alloc call,
     *             NULL if called from outside the Alloc path
     * @param[in]  accessRight The access right to be granted
     * @returns NV_TRUE if the access right should be granted, and NV_FALSE otherwise
     */
    virtual NvBool resAccessCallback(RsResource *pResource, RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);

    /**
     * Decides whether rights can be shared with a client under a certain policy.
     *
     * The purpose of this function is to provide subclasses the ability to set custom definitions
     * for how certain policies will share. Certain share types can then be created to work based
     * on components not stored directly in resserv, such as PID.
     *
     * @param[in]    pResource The resource being shared
     * @param[in]    pInvokingClient The client being shared with
     * @param[in]    pParentRef dstParent if calling from DupObject, NULL otherwise
     * @param[in]    pSharePolicy The policy under which to share
     * @returns NV_TRUE if the share policy applies and rights should be shared, NV_FALSE otherwise
     */
    virtual NvBool resShareCallback(RsResource *pResource, RsClient *pInvokingClient, RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);

    /**
     * Adds dependants that aren't in childRefMap or depRefMap to the pending free list.
     *
     * Due to RAM constraints, some classes can add more dependants that aren't
     * represented in childRefMap or depRefMap. They can override this function
     * to put them in the pending free list while we are updating it.
     *
     * @param[in]    pClient
     * @param[in]    pResource      The RsResource with potential additional dependants
     * @param[in]    pReference     The pReference to pass in to
     *                              clientUpdatePendingFreeList()
     */
    virtual void resAddAdditionalDependants(RsClient *pClient, RsResource *pResource, RsResourceRef *pReference);
};

/* @} */

class OBJGPU;

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
    RsClient   *pClient;            ///< Pointer to the client that owns the ref
    RsResource *pResource;          ///< Pointer to the actual resource
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

    RsSession *pSession;          ///< If set, this ref depends on a shared session
    RsSession *pDependantSession; ///< If set, this ref is depended on by a shared session

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

    RsClient *pClient;
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

    RsClient *pClient;
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
NvBool    refPendingFree(RsResourceRef *pResourceRef, RsClient *pClient);


#ifdef __cplusplus
}
#endif

#endif
