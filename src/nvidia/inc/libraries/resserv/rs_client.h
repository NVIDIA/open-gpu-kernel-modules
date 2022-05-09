/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_rs_client_nvoc.h"

#ifndef _RS_CLIENT_H_
#define _RS_CLIENT_H_


#include "resserv/resserv.h"
#include "nvport/nvport.h"
#include "resserv/rs_resource.h"
#include "containers/list.h"
#include "utils/nvrange.h"

#define RS_UNIQUE_HANDLE_BASE  (0xcaf00000)
#define RS_UNIQUE_HANDLE_RANGE (0x00080000)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RsClient
 * @addtogroup RsClient
 * @{*/

typedef enum {
    CLIENT_TYPE_USER,
    CLIENT_TYPE_KERNEL
} CLIENT_TYPE;

typedef struct AccessBackRef
{
    NvHandle hClient;
    NvHandle hResource;
} AccessBackRef;

MAKE_LIST(AccessBackRefList, AccessBackRef);

/**
 * Information about a client
 */
NVOC_PREFIX(client) class RsClient : Object
{
public:
    /**
     * The handle of this client
     */
    NvHandle hClient;

    /**
     * Kernel or user client
     */
    CLIENT_TYPE type;

    /**
     * Client is in a state where it can allocate new objects
     */
    NvBool bActive;

    /**
     * True if client tripped the resource count warning threshold
     */
    NvBool bResourceWarning;

    /**
     * Maps resource handle -> RsResourceRef
     */
    RsRefMap resourceMap;

    /**
     * Access right back reference list of <hClient, hResource> pairs
     *
     * A map of all hResource's (with hClient to scope the handle) that have
     * shared access rights with us.
     */
    AccessBackRefList accessBackRefList;

    /**
     * The first generated handle in the generated resource handle space
     *
     * It is an error for the handleRangeStart to be 0 because that is a
     * reserved handle.
     *
     * The first generated handle is not necessarily the lowest possible handle
     * because the handle generator may overflow. The lowest possible resource
     * handle is 0x1.
     *
     * Generated handles will be of the form: handleRangeStart + [0, handleRangeSize)
     */
    NvHandle handleRangeStart;

    /**
     * The size of the generated resource handle space.
     *
     * It is an error for the handleRangeSize to be 0.
     *
     * Generated handles will be of the form: handleRangeStart + [0, handleRangeSize)
     */
    NvHandle handleRangeSize;

    /**
     * The handles in the restricted resource handle space.
     */
    NV_RANGE handleRestrictRange;

    /**
     * Index used to generate the next handle in the resource handle space
     */
    NvHandle handleGenIdx;

    /**
     * Ordered list of resources that are to be freed
     */
    RsRefFreeList pendingFreeList;

    /**
     * Information about recursive resource free calls is stored here
     */
    RS_FREE_STACK *pFreeStack;

    /**
     * Construct a client instance
     * @param[in]   pClient This client
     * @param[in]   pAllocator NvPort memory allocation interface for client memory allocations
     * @param[in]   pParams The allocation params
     */
    NV_STATUS clientConstruct(RsClient *pClient, PORT_MEM_ALLOCATOR *pAllocator, RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

    /**
     * Destruct a client instance and free all allocated resources
     * @param[in]   pClient This client
     */
    void clientDestruct(RsClient *pClient);

    /**
     * Get a resource pointer from a resource reference. No resource locks are taken.
     * @param[in]   pClient This client
     * @param[in]   pResourceRef The reference to the resource
     * @param[out]  ppResource Pointer to the resource
     */
    NV_STATUS clientGetResourceByRef(RsClient *pClient, RsResourceRef *pResourceRef, RsResource **ppResource);

    /**
     * Get a resource pointer from a resource handle. No resource locks are taken.
     * @param[in]   pClient This client
     * @param[in]   hResource Resource handle
     * @param[in]   internalClassId Expected internal class ID of object. Must match.
     * @param[out]  ppResource Pointer to the resource
     */
    NV_STATUS clientGetResource(RsClient *pClient, NvHandle hResource, NvU32 internalClassId, RsResource **ppResource);

    /**
     * Get the reference to a resource
     * @param[in]   pClient This client
     * @param[in]   hResource The resource to lookup
     * @param[out]  ppResourceRef The reference to the resource
     */
    NV_STATUS clientGetResourceRef(RsClient *pClient, NvHandle hResource, RsResourceRef **ppResourceRef);

    /**
     * Get the reference to a resource, but only if the passed in access rights are
     * possessed by the invoking client.
     *
     * @param[in]   pClient This client
     * @param[in]   hResource The resource to lookup
     * @param[in]   pRightsRequired The rights required for success
     * @param[out]  ppResourceRef The reference to the resource
     */
    NV_STATUS clientGetResourceRefWithAccess(RsClient *pClient, NvHandle hResource, const RS_ACCESS_MASK *pRightsRequired, RsResourceRef **ppResourceRef);

    /**
     * Get the reference to a resource (with a type check)
     * @param[in]   pClient This client
     * @param[in]   hResource The resource to lookup
     * @param[in]   internalClassId The internal resource class id
     * @param[out]  ppResourceRef The reference to the resource
     */
    NV_STATUS clientGetResourceRefByType(RsClient *pClient, NvHandle hResource, NvU32 internalClassId, RsResourceRef **ppResourceRef);

    /**
     * Validate that current process is allowed to use this client
     * @param[in]   pClient This client
     * @param[in]   pSecInfo Security info of the current API call
     */
    virtual NV_STATUS clientValidate(RsClient *pClient, const API_SECURITY_INFO * pSecInfo);

    /**
     * Allocate a resource in RM for this client
     * @param[in]       pClient This client
     * @param[in]       pServer
     * @param[inout]    pParams Resource allocation parameters
     */
    NV_STATUS clientAllocResource(RsClient *pClient, RsServer *pServer, RS_RES_ALLOC_PARAMS_INTERNAL *pParams);

    /**
     * Duplicate a resource reference into this client
     * @param[in]       pClient This client
     * @param[in]       pServer The resource server instance
     * @param[inout]    pParams Resource sharing parameters
     */
    NV_STATUS clientCopyResource(RsClient *pClient, RsServer *pServer, RS_RES_DUP_PARAMS_INTERNAL *pParams);

    /**
     * Free a resource for this client and updates resource reference book-keeping.
     * If the resource has a non-zero reference count, only book-keeping will be updated.
     * Resources should never be freed in control calls.
     *
     * @param[in]       pClient This client
     * @param[in]       pServer
     * @param[in]       pParams Resource destruction parameters
     */
    virtual NV_STATUS clientFreeResource(RsClient *pClient, RsServer *pServer, RS_RES_FREE_PARAMS_INTERNAL *pParams);

    /**
     * Remove a resource reference to the client's resource hashmap
     * @param[in] pClient This client
     * @param[in] pResourceRef The reference to free
     */
    virtual NV_STATUS clientDestructResourceRef(RsClient *pClient, RsServer *pServer, RsResourceRef *pResourceRef);

   /**
     * Unmap a mapping that belongs to a resource reference in this client.
     * @param[in]       pClient This client
     * @param[in]       pResourceRef The reference that owns the mapping
     * @param[inout]    ppCpuMapping The mapping to unmap
     */
    virtual NV_STATUS clientUnmapMemory(RsClient *pClient, RsResourceRef *pResourceRef,
                                        RS_LOCK_INFO *pLockInfo, RsCpuMapping **ppCpuMapping,
                                        API_SECURITY_INFO *pSecInfo);
   /**
     * Create an inter-mapping between two resources owned by this client
     * Resserv only implements a stub, users should override this to fill their own MapTo params struct
     *
     * @param[in]  pClient This client
     * @param[in]  pMapperRef The resource that can be used to create the mapping
     * @param[in]  pMappableRef The resource that can be mapped
     * @param[in]  pParams parameters describing the unmapping
     */
    virtual NV_STATUS clientInterMap(RsClient *pClient, RsResourceRef *pMapperRef, RsResourceRef *pMappableRef, RS_INTER_MAP_PARAMS *pParams);

   /**
     * Unmap an inter-mapping between two resources owned by this client
     * Resserv only implements a stub, users should override this to fill their own UnmapFrom params struct
     *
     * @param[in]  pClient This client
     * @param[in]  pMapperRef The reference that was was used to create the mapping
     * @param[in]  pParams parameters describing the unmapping
     */
    virtual void clientInterUnmap(RsClient *pClient, RsResourceRef *pMapperRef, RS_INTER_UNMAP_PARAMS *pParams);

    /**
     * Generate an unused handle for a resource. The handle will be generated in the white-listed range that was
     * specified when the client was allocated.
     *
     * The handle generator will wrap-around when the number of handles generated is greater than handleRangeSize, and
     * the generator will start at handle 0x1 if it overflows (0x0 is a reserved handle).
     *
     * The handle generator can generate up to 2^32-2 unique handles if handleRangeStart + handleRangeSize overflows
     * (because 0x0 is a reserved handle). Otherwise, the handle generator can generate up to 2^32-1 unique handles.
     *
     * @param[in]  pClient This client
     * @param[out] pHandle The generated handle
     *
     */
    NV_STATUS clientGenResourceHandle(RsClient *pClient, NvHandle *pHandle);

    /**
     * Validate that a given resource handle is well-formed and does not already
     * exist under a given client.
     *
     * @param[in]   pClient
     * @param[in]   hResource
     * @param[in]   bRestrict If true, fail validation for handles in the client's restricted range
     */
    virtual NV_STATUS clientValidateNewResourceHandle(RsClient *pClient, NvHandle hResource, NvBool bRestrict);

    /**
     * Wrapper that generates a resource handle if a handle of 0 is provided, or otherwise
     * validates a handle that was provided.
     *
     * @param[in]       pClient
     * @param[inout]    phResource
     */
    NV_STATUS clientAssignResourceHandle(RsClient *pClient, NvHandle *phResource);

    /**
     * Recursively generate a client's list of resources to free
     * @param[in]   pClient
     * @param[in]   pTarget     The resource ref currently being processed
     * @param[in]   pReference  The resource ref that this function was initially called on
     * @param[in]   bMove       If NV_TRUE:  Add/move the target to the front of the list
     *                          If NV_FALSE: Add the target to the front of the list if it isn't already in the list
     */
    NV_STATUS clientUpdatePendingFreeList(RsClient *pClient, RsResourceRef *pTarget,
                                          RsResourceRef *pReference, NvBool bMove);

    /**
     * Allow derived client classes to modify the generated list of resources to free
     * before they are freed.
     * @param[in]   pClient
     * @param[out]  ppFirstLowPriRef A pointer to the first reference that is low priority
     */
    virtual NV_STATUS clientPostProcessPendingFreeList(RsClient *pClient, RsResourceRef **ppFirstLowPriRef);

    /**
     * Add a back reference to a client/resource pair that shared access with our client
     * so we can remove that access entry on client destruction.
     * @param[in]   pClient This client
     * @param[in]   pResourceRef Resource reference that decided to share access with us
     */
    NV_STATUS clientAddAccessBackRef(RsClient* pClient, RsResourceRef* pResourceRef);

    /**
     * Remove all access map entries for all back references we stored so other clients
     * reusing the same client handle won't get unauthorized access. Intended to be called
     * during client destruction.
     * @param[in]   pClient This client
     * @param[in]   pServer Resource Server instance
     */
    void clientFreeAccessBackRefs(RsClient *pClient, RsServer *pServer);

    /*
     * Set the start handle and range for this client's handle generator.
     *
     * @note Supplying a range and size of 0 will set the generator to the default start handle and range
     * @note The handle generator can only be set before any handle has been generated
     *
     * @param[in]       pClient
     * @param[in]       handleRangeStart
     * @param[in]       handleRangeSize
     */
    NV_STATUS clientSetHandleGenerator(RsClient *pClient, NvHandle handleRangeStart, NvHandle handleRangeSize);

    /**
     * Verify whether a client is able to share a resource under a certain share policy
     *
     * @param[in] pClient Client attempting to share the resource
     * @param[in] pReousrceRef The resource being shared
     * @param[in] pSharePolicy The policy under which the resource is to be shared
     * @param[in] pCallContext The context of the call intending to perform the share
     */
    NV_STATUS clientCanShareResource(RsClient *pClient, RsResourceRef *pResourceRef,
                                     RS_SHARE_POLICY *pSharePolicy, CALL_CONTEXT *pCallContext);

    /**
     * Share access to a resource with other clients under the specified share policy.
     *
     * @param[in]   pClient This client
     * @param[in]   pResourceRef Resource reference which is sharing access
     * @param[in]   pSharePolicy The policy under which the resource is sharing access
     */
    virtual NV_STATUS clientShareResource(RsClient *pClient, RsResourceRef *pResourceRef,
                                          RS_SHARE_POLICY *pSharePolicy,
                                          CALL_CONTEXT *pCallContext);

    /**
     * Share access to a resource with other clients under the specified share policy.
     *
     * @param[in]   pClient This client
     * @param[in]   pResourceRef Resource reference which is sharing access
     * @param[in]   pSharePolicy The policy under which the resource is sharing access
     */
    NV_STATUS clientShareResourceTargetClient(RsClient *pClient, RsResourceRef *pResourceRef,
                                              RS_SHARE_POLICY *pSharePolicy, CALL_CONTEXT *pCallContext);
    /*
     * Set the start handle and range for this client's restricted handle
     * range. This range of handles cannot be explicitly requested. Any
     * restricted handles that are in the client's resource handle generator
     * range can still be generated.
     *
     * @note Supplying a range and size of 0 will disable the restricted range
     * @note The handle generator can only be set before any handle has been generated
     *
     * @param[in]       pClient
     * @param[in]       handleRangeStart
     * @param[in]       handleRangeSize
     */
    NV_STATUS clientSetRestrictedRange(RsClient *pClient, NvHandle handleRangeStart, NvU32 handleRangeSize);
};

/**
 * Get an iterator to the elements in the client's resource map
 * @param[in] pClient
 * @param[in] pScopeRef Restrict the iteration based on this reference [optional]
 * @param[in] internalClassId Only iterate over resources with this class id [optional]
 * @param[in] type RS_ITERATE_CHILDREN, RS_ITERATE_DESCENDANTS, RS_ITERATE_CACHED, RS_ITERATE_DEPENDANTS
 * @param[in] bExactMatch If true, internalClassId must match exactly; if false, also match classes derived from the internalClassId
 *
 * @note If type=RS_ITERATE_CHILDREN, pScopeRef will restrict iteration to children of the scope ref
 * @note If type=RS_ITERATE_DESCENDANTS, pScopeRef will restrict iteration to descendants of the scope ref
 * @note If type=RS_ITERATE_CACHED, pScopeRef will restrict iteration to references cached by the scope ref
 */
RS_ITERATOR clientRefIter(RsClient *pClient, RsResourceRef *pScopeRef, NvU32 internalClassId, RS_ITER_TYPE type, NvBool bExactMatch);

/**
 * Get the next iterator to the elements in the client's resource map
 * @param[in] pClient
 * @param[inout] pIt The iterator
 */
NvBool clientRefIterNext(RsClient *pClient, RS_ITERATOR *pIt);

/**
 * Get an iterator to the elements in the client's resource map.
 *
 * This iterator will visit all descendants in pre-order according to the parent-child
 * resource hierarchy.
 *
 * @param[in] pClient
 * @param[in] pScopeRef Restrict the iteration based on this reference [optional]
 * @param[in] internalClassId Only iterate over resources with this class id [optional]
 * @param[in] bExactMatch If true, internalClassId must match exactly; if false, also match classes derived from the internalClassId
 */
RS_ORDERED_ITERATOR clientRefOrderedIter(RsClient *pClient, RsResourceRef *pScopeRef, NvU32 internalClassId, NvBool bExactMatch);

/**
 * Get the next ordered iterator to the elements in the client's resource map
 * @param[in] pClient
 * @param[inout] pIt The iterator
 */
NvBool clientRefOrderedIterNext(RsClient *pClient, RS_ORDERED_ITERATOR *pIt);


/**
 * RsResource interface to a RsClient
 *
 * This allows clients to be interfaced with as-if they were resources (e.g.,
 * to perform a control call on a client).
 *
 * An RsClientResource is automatically allocated under a client as a top-level
 * object when that client is allocated and cannot be explicitly freed. Only
 * one RsClientResource is permitted per-client.
 *
 * Any resource allocated under a client will be a descendant of the client
 * proxy resource.
 *
 */
NVOC_PREFIX(clientres) class RsClientResource : RsResource
{
public:
    NV_STATUS clientresConstruct(RsClientResource* pClientRes, CALL_CONTEXT *pCallContext, RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
              : RsResource(pCallContext, pParams);
    void clientresDestruct(RsClientResource* pClientRes);

// private:
    RsClient* pClient;
};

/**
 * Client destruction parameters
 */
struct RS_CLIENT_FREE_PARAMS_INTERNAL
{
    NvHandle hDomain;           ///< [in] The parent domain
    NvHandle hClient;           ///< [in] The client handle
    NvBool   bHiPriOnly;        ///< [in] Only free high priority resources
    NvU32    state;             ///< [in] User-defined state

    RS_RES_FREE_PARAMS_INTERNAL *pResFreeParams; ///< [in] Necessary for locking state
    API_SECURITY_INFO *pSecInfo;                 ///< [in] Security Info
};

/**
 * Return an iterator to a resource reference multi-map
 * @param[in] pIndex The multi-map to iterate
 * @param[in] index Return only the references belonging to this index
 */
RsIndexIter indexRefIter(RsIndex *pIndex, NvU32 index);

/**
 * Return an iterator to all resource references in a multi-map
 * @param[in] pIndex The multi-map to iterate
 */
RsIndexIter indexRefIterAll(RsIndex *pIndex);

/**
 * Get the next iterator in a resource reference multi-map
 * @param[in] pIt Iterator
 */
NvBool indexRefIterNext(RsIndexIter *pIt);

/* @} */

#ifdef __cplusplus
}
#endif

#endif
