/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvstatus.h"
#include "nvtypes.h"

#include "containers/map.h"
#include "resserv/resserv.h"
#include "resserv/rs_resource.h"
#include "resserv/rs_client.h"
#include "resserv/rs_server.h"
#include "resserv/rs_access_rights.h"
#include "resserv/rs_access_map.h"

static NV_STATUS
_rsAccessGrantCallback
(
    RsResourceRef *pResourceRef,
    CALL_CONTEXT *pCallContext,
    RsClient *pInvokingClient,
    const RS_ACCESS_MASK *pParentRights,
    void *pAllocParams,
    RsAccessRight accessRight
);

/*!
 * @brief Checks which rights, if any, are being shared with the invoking client by a resource
 * This is a static helper function for rsAccessGrantRights.
 *
 * @param[in] pResourceRef
 * @param[in] pInvokingClient
 * @param[in] pCallContext May be NULL
 * @param[out] pRightsShared The set of access rights shared
 *
 * @return none
 */
static void
_rsAccessGetSharedRights
(
    RsResourceRef *pResourceRef,
    RsClient *pInvokingClient,
    CALL_CONTEXT *pCallContext,
    RS_ACCESS_MASK *pRightsShared
)
{
    RsShareList *pShareList;
    RsShareListIter it;

    RsServer *pServer = NULL;
    RsResourceRef *pParentRef = NULL;

    RS_ACCESS_MASK rightsGranted;
    RS_ACCESS_MASK rightsDenied;

    portMemSet(&rightsGranted, 0, sizeof(RS_ACCESS_MASK));
    portMemSet(&rightsDenied, 0, sizeof(RS_ACCESS_MASK));

    RS_ACCESS_MASK_CLEAR(pRightsShared);

    // No meaning to sharing rights with self, skip
    if (pInvokingClient == pResourceRef->pClient)
        return;

    if (pCallContext != NULL)
    {
        pServer = pCallContext->pServer;
        pParentRef = pCallContext->pContextRef;
    }

    pShareList = rsAccessGetActiveShareList(pResourceRef, pServer);

    if (pShareList != NULL)
    {
        it = listIterAll(pShareList);
        while (listIterNext(&it))
        {
            RS_SHARE_POLICY *pSharePolicy = it.pValue;

            if (resShareCallback(pResourceRef->pResource, pInvokingClient, pParentRef, pSharePolicy))
            {
                // Allow policies give rights on success
                if (!(pSharePolicy->action & RS_SHARE_ACTION_FLAG_REQUIRE))
                    RS_ACCESS_MASK_UNION(&rightsGranted, &pSharePolicy->accessMask);
            }
            else
            {
                // Require policies reject rights on failure
                if (pSharePolicy->action & RS_SHARE_ACTION_FLAG_REQUIRE)
                    RS_ACCESS_MASK_UNION(&rightsDenied, &pSharePolicy->accessMask);
            }
        }
    }

    if (pServer != NULL)
    {
        it = listIterAll(&pServer->globalInternalSharePolicyList);
        while (listIterNext(&it))
        {
            RS_SHARE_POLICY *pSharePolicy = it.pValue;

            if (resShareCallback(pResourceRef->pResource, pInvokingClient, pParentRef, pSharePolicy))
            {
                // Allow policies give rights on success
                if (!(pSharePolicy->action & RS_SHARE_ACTION_FLAG_REQUIRE))
                    RS_ACCESS_MASK_UNION(&rightsGranted, &pSharePolicy->accessMask);
            }
            else
            {
                // Require policies reject rights on failure
                if (pSharePolicy->action & RS_SHARE_ACTION_FLAG_REQUIRE)
                    RS_ACCESS_MASK_UNION(&rightsDenied, &pSharePolicy->accessMask);
            }
        }
    }

    RS_ACCESS_MASK_UNION(pRightsShared, &rightsGranted);
    RS_ACCESS_MASK_SUBTRACT(pRightsShared, &rightsDenied);
}

void rsAccessGetAvailableRights
(
    RsResourceRef *pResourceRef,
    RsClient *pClient,
    RS_ACCESS_MASK *pAvailableRights
)
{
    RS_ACCESS_MASK *pTargetRights;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    RS_ACCESS_MASK_CLEAR(pAvailableRights);

    // Look up rights client has on target resource
    pTargetRights = rsAccessLookup(pResourceRef, pClient);
    if (pTargetRights != NULL)
    {
        // Client owns the resource, use those rights directly
        portMemCopy(pAvailableRights, sizeof(*pAvailableRights),
                    pTargetRights, sizeof(*pTargetRights));
    }
    else
    {
        // Client does not own the resource, add any rights shared with this client
        _rsAccessGetSharedRights(pResourceRef, pClient, pCallContext, pAvailableRights);
    }
}

RS_ACCESS_MASK *
rsAccessLookup
(
    RsResourceRef *pResourceRef,
    RsClient *pClient
)
{
    if (pResourceRef->pClient == pClient)
        return &pResourceRef->accessMask;

    return NULL;
}

NV_STATUS
rsAccessCheckRights
(
    RsResourceRef *pResourceRef,
    RsClient *pInvokingClient,
    const RS_ACCESS_MASK *pRightsRequired
)
{
    RS_ACCESS_MASK ownedRights;

    NV_ASSERT_OR_RETURN(pRightsRequired != NULL, NV_ERR_INVALID_ARGUMENT);

    // Return if nothing to check
    if (rsAccessMaskIsEmpty(pRightsRequired))
        return NV_OK;

    // Uncached access rights require executing the callback every time
    rsAccessUpdateRights(pResourceRef, pInvokingClient, pRightsRequired);

    // Look up updated rights on target resource
    rsAccessGetAvailableRights(pResourceRef, pInvokingClient, &ownedRights);

    // Check that rights are sufficient
    if (!rsAccessMaskIsSubset(&ownedRights, pRightsRequired))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return NV_OK;
}

void rsAccessUpdateRights
(
    RsResourceRef *pResourceRef,
    RsClient *pInvokingClient,
    const RS_ACCESS_MASK *pRightsToUpdate
)
{
    RS_ACCESS_MASK *pTargetRights;
    RsAccessRight   accessRight;

    // Look up rights on target resource
    pTargetRights = rsAccessLookup(pResourceRef, pInvokingClient);

    //
    // Nothing to update if the resource is not owned by the client
    // (Uncached rights only have meaning for resources owned by the client)
    //
    if (pTargetRights == NULL)
        return;

    // Update access rights owned by the client for any uncached rights
    for (accessRight = 0; accessRight < RS_ACCESS_COUNT; accessRight++)
    {
        NV_STATUS status;
        const RS_ACCESS_INFO *pAccessRightInfo = &g_rsAccessMetadata[accessRight];
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

        if ((pRightsToUpdate != NULL &&
             !RS_ACCESS_MASK_TEST(pRightsToUpdate, accessRight)) ||
            !(pAccessRightInfo->flags & RS_ACCESS_FLAG_UNCACHED_CHECK))
        {
            continue;
        }

        status = _rsAccessGrantCallback(pResourceRef, pCallContext, pInvokingClient, NULL, NULL, accessRight);

        if (status != NV_OK)
        {
            RS_ACCESS_MASK_REMOVE(pTargetRights, accessRight);
        }
        else
        {
            RS_ACCESS_MASK_ADD(pTargetRights, accessRight);
        }
    }
}

/*!
 * @brief Checks whether two share policies are considered equal and can be merged
 *
 * This function uses the type and target of a share policy to determine whether
 * two share policy entries would match the same clients, in which case they could
 * be merged into one policy entry.
 *
 * @param[in] pSharePolicyA, pSharePolicyB the two policies to compare
 *
 * @return NV_TRUE if the two policies are equal,
 *         NV_FALSE otherwise
 */
static NvBool
rsSharePolicyEquals
(
    const RS_SHARE_POLICY *pSharePolicyA,
    const RS_SHARE_POLICY *pSharePolicyB
)
{
    if (pSharePolicyA == NULL || pSharePolicyB == NULL)
        return NV_FALSE;

    if (pSharePolicyA->type != pSharePolicyB->type)
        return NV_FALSE;

    if ((pSharePolicyA->action & RS_SHARE_ACTION_FLAG_REQUIRE) !=
        (pSharePolicyB->action & RS_SHARE_ACTION_FLAG_REQUIRE))
    {
        return NV_FALSE;
    }

    if (pSharePolicyA->type == RS_SHARE_TYPE_CLIENT)
    {
        return pSharePolicyA->target == pSharePolicyB->target;
    }

    // Otherwise, ignore target entirely
    return NV_TRUE;
}

RS_SHARE_POLICY *
rsShareListLookup
(
    RsShareList *pShareList,
    RS_SHARE_POLICY *pSharePolicy
)
{
    RsShareListIter it;

    //
    // Need to match a condition instead of just pValue,
    // can't just use listLookup directly
    //
    it = listIterAll(pShareList);
    while (listIterNext(&it))
    {
        if (rsSharePolicyEquals(it.pValue, pSharePolicy))
        {
            return it.pValue;
        }
    }

    return NULL;
}

NV_STATUS
rsShareListInsert
(
    RsShareList *pShareList,
    RS_SHARE_POLICY *pSharePolicy,
    RS_ACCESS_MASK *pAccessMask
)
{
    RS_ACCESS_MASK *pCurrentAccessMask;
    RS_SHARE_POLICY *pCurrentPolicy;
    RS_SHARE_POLICY *pNewPolicy;

    pCurrentPolicy = rsShareListLookup(pShareList, pSharePolicy);
    if (pCurrentPolicy == NULL)
    {
        // Allocate and insert a share policy entry
        pNewPolicy = listAppendValue(pShareList, pSharePolicy);
        if (pNewPolicy == NULL)
        {
            return NV_ERR_NO_MEMORY;
        }

        if (pAccessMask != NULL)
        {
            portMemCopy(pAccessMask, sizeof(*pAccessMask),
                        &pNewPolicy->accessMask, sizeof(pNewPolicy->accessMask));
        }
    }
    else
    {
        // Merge into existing share policy entry
        pCurrentAccessMask = &pCurrentPolicy->accessMask;
        RS_ACCESS_MASK_UNION(pCurrentAccessMask, &pSharePolicy->accessMask);

        if (pAccessMask != NULL)
        {
            portMemCopy(pAccessMask, sizeof(*pAccessMask),
                        pCurrentAccessMask, sizeof(*pCurrentAccessMask));
        }
    }

    return NV_OK;
}

void
rsShareListRemove
(
    RsShareList *pShareList,
    RS_SHARE_POLICY *pSharePolicy,
    RS_ACCESS_MASK *pAccessMask
)
{
    RS_SHARE_POLICY *pCurrentPolicy;
    RS_ACCESS_MASK *pCurrentAccessMask;

    pCurrentPolicy = rsShareListLookup(pShareList, pSharePolicy);
    if (pCurrentPolicy != NULL)
    {
        // Revoke specified rights from found mask
        pCurrentAccessMask = &pCurrentPolicy->accessMask;
        RS_ACCESS_MASK_SUBTRACT(pCurrentAccessMask, &pSharePolicy->accessMask);

        // pCurrentAccessMask may not exist afterwards, so copy output first
        if (pAccessMask != NULL)
        {
            portMemCopy(pAccessMask, sizeof(*pAccessMask),
                        pCurrentAccessMask, sizeof(*pCurrentAccessMask));
        }

        if (rsAccessMaskIsEmpty(pCurrentAccessMask))
        {
            // No more rights shared under this policy, erase it from the list
            listRemove(pShareList, pCurrentPolicy);
        }
    }
    else
    {
        // No match, no rights to revoke, output empty mask
        if (pAccessMask != NULL)
        {
            RS_ACCESS_MASK_CLEAR(pAccessMask);
        }
    }
}

NV_STATUS
rsShareListCopy
(
    RsShareList *pShareListDst,
    RsShareList *pShareListSrc
)
{
    RsShareListIter it;

    if (pShareListSrc == NULL)
        return NV_OK;

    it = listIterAll(pShareListSrc);
    while (listIterNext(&it))
    {
        if (NULL == listAppendValue(pShareListDst, it.pValue))
            return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

RsShareList *
rsAccessGetActiveShareList
(
    RsResourceRef *pResourceRef,
    RsServer *pServer
)
{
    RsResourceRef *pSearchRef = pResourceRef;

    // Search up the tree for a resource with an edited share list
    while (pSearchRef != NULL)
    {
        if (pSearchRef->bSharePolicyListModified)
            return &pSearchRef->sharePolicyList;

        pSearchRef = pSearchRef->pParentRef;
    }

    if (pServer != NULL)
        return &pServer->defaultInheritedSharePolicyList;

    return NULL;
}

/*!
 * @brief Checks whether one access right can be granted on a resource
 *
 * This is a static helper function for rsAccessGrantRights. The pParentRights
 * argument is not strictly necessary, but is used to avoid performing multiple
 * identical lookups in a map.
 *
 * @param[in] pResourceRef
 * @param[in] pCallContext
 * @param[in] pInvokingClient
 * @param[in] pParentRights The set of access rights held by the invoking client
 *            on the resource's parent
 * @param[in] accessRight The access right to try to grant
 *
 * @return NV_OK if the access right can be granted, or an error otherwise
 */
static NV_STATUS
_rsAccessGrantCallback
(
    RsResourceRef *pResourceRef,
    CALL_CONTEXT *pCallContext,
    RsClient *pInvokingClient,
    const RS_ACCESS_MASK *pParentRights,
    void *pAllocParams,
    RsAccessRight accessRight
)
{
    const RS_ACCESS_INFO   *pAccessRightInfo;
    API_SECURITY_INFO *pSecInfo = NULL;

    NV_ASSERT_OR_RETURN(accessRight < RS_ACCESS_COUNT, NV_ERR_INVALID_ARGUMENT);

    pAccessRightInfo = &g_rsAccessMetadata[accessRight];

    if (pCallContext != NULL)
    {
        pSecInfo = &pCallContext->secInfo;
    }
    else
    {
        NV_PRINTF(LEVEL_WARNING, "Called with NULL pCallContext, skipping permission checks\n");
    }

    //
    // If the parent object has this access right, then we should be able to
    // inherit it without doing any other checks
    //
    if ((pParentRights != NULL) && RS_ACCESS_MASK_TEST(pParentRights, accessRight))
    {
        return NV_OK;
    }

    if ((pSecInfo != NULL) && ((pAccessRightInfo->flags & RS_ACCESS_FLAG_ALLOW_PRIVILEGED) != 0))
    {
        // Allow admin-privileged contexts
        if (pSecInfo->privLevel >= RS_PRIV_LEVEL_USER_ROOT)
        {
            return NV_OK;
        }
    }

    if ((pSecInfo != NULL) && ((pAccessRightInfo->flags & RS_ACCESS_FLAG_ALLOW_KERNEL_PRIVILEGED) != 0))
    {
        // Allow kernel-privileged contexts
        if (pSecInfo->privLevel >= RS_PRIV_LEVEL_KERNEL)
        {
            return NV_OK;
        }
    }

    if ((pAccessRightInfo->flags & RS_ACCESS_FLAG_ALLOW_OWNER) != 0)
    {
        // Allow client this access right on itself
        if (pResourceRef->hResource == pInvokingClient->hClient)
        {
            return NV_OK;
        }
    }

    // Finally, invoke the resource's access callback
    if (resAccessCallback(pResourceRef->pResource, pInvokingClient, pAllocParams, accessRight))
    {
        return NV_OK;
    }

    // All attempts to grant access failed
    return NV_ERR_INSUFFICIENT_PERMISSIONS;
}


/*!
 * @brief Computes the list of access rights to attempt to grant on a resource
 *
 * This is a static helper function for rsAccessGrantRights.
 *
 * @param[in] pResourceRef
 * @param[in] pInvokingClient
 * @param[in] pRightsRequested The rights specified in the allocation parameters,
 *            or NULL if no access rights were explicitly requested
 * @param[in] pRightsRequired Rights required for the allocation of this object
 *            to succeed, not used if rights were explicitly requested
 * @param[out] pRightsToRequest The set of access rights that should be requested,
 *             based on input parameters provided
 *
 * @return NV_TRUE if access rights were explicitly requested, or
 *         NV_FALSE otherwise
 */
static NvBool
_rsAccessGetRightsToRequest
(
    RsResourceRef *pResourceRef,
    RsClient *pInvokingClient,
    const RS_ACCESS_MASK *pRightsRequested,
    const RS_ACCESS_MASK *pRightsRequired,
    RS_ACCESS_MASK *pRightsToRequest
)
{
    NvBool bExplicitlyRequested;

    NV_ASSERT(pRightsToRequest != NULL);
    RS_ACCESS_MASK_CLEAR(pRightsToRequest);

    if (pRightsRequested != NULL)
    {
        // A set of access rights was explicitly requested
        bExplicitlyRequested = NV_TRUE;

        portMemCopy(pRightsToRequest, sizeof(*pRightsToRequest),
                    pRightsRequested, sizeof(*pRightsRequested));
    }
    else
    {
        // No rights were explicitly requested
        bExplicitlyRequested = NV_FALSE;

        if (pResourceRef->pParentRef == NULL)
        {
            // Only client resources don't have a parent reference
            // Try to request all access rights for new clients
            RS_ACCESS_MASK_FILL(pRightsToRequest);
        }
        else
        {
            // Inherit access rights from parent reference
            RS_ACCESS_MASK *pParentRights = rsAccessLookup(pResourceRef->pParentRef, pInvokingClient);
            if (pParentRights != NULL)
            {
                portMemCopy(pRightsToRequest, sizeof(*pRightsToRequest),
                            pParentRights, sizeof(*pParentRights));
            }

            // Add any required rights as well
            if (pRightsRequired != NULL)
            {
                RS_ACCESS_MASK_UNION(pRightsToRequest, pRightsRequired);
            }
        }
    }

    return bExplicitlyRequested;
}

NV_STATUS
rsAccessGrantRights
(
    RsResourceRef *pResourceRef,
    CALL_CONTEXT *pCallContext,
    RsClient *pInvokingClient,
    const RS_ACCESS_MASK *pRightsRequested,
    const RS_ACCESS_MASK *pRightsRequired,
    void *pAllocParams
)
{
    NV_STATUS        status;
    NvBool           bExplicitlyRequested;
    RS_ACCESS_MASK   rightsToRequest;
    RS_ACCESS_MASK   rightsShared;
    RS_ACCESS_MASK  *pResourceRights;
    RS_ACCESS_MASK   resourceRights;
    RS_ACCESS_MASK  *pParentRights = NULL;
    RsAccessRight    accessRight;

    // Determine which rights to request based on pRightsRequested
    bExplicitlyRequested = _rsAccessGetRightsToRequest(pResourceRef, pInvokingClient,
                                                       pRightsRequested, pRightsRequired,
                                                       &rightsToRequest);

    // Return if nothing to grant
    if (rsAccessMaskIsEmpty(&rightsToRequest))
        return NV_OK;

    // Find rights on the current resource
    pResourceRights = rsAccessLookup(pResourceRef, pInvokingClient);
    if (pResourceRights == NULL)
    {
        //
        // When using grant for resources the client doesn't own, we don't modify the
        // resource's mask, we only use a local mask to record which rights were available
        //
        RS_ACCESS_MASK_CLEAR(&resourceRights);
        pResourceRights = &resourceRights;
    }

    // Explicitly requesting to not get all required rights, cannot possibly succeed
    if (bExplicitlyRequested &&
        (pRightsRequired != NULL) &&
        !rsAccessMaskIsSubset(&rightsToRequest, pRightsRequired))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    // Get rights on the parent resource to cache for _rsAccessGrantCallback
    if (pResourceRef->pParentRef != NULL)
    {
        pParentRights = rsAccessLookup(pResourceRef->pParentRef, pInvokingClient);
    }

    // Get any rights shared with this client
    _rsAccessGetSharedRights(pResourceRef, pInvokingClient, pCallContext, &rightsShared);

    // Grant each access right in rightsToRequest
    for (accessRight = 0; accessRight < RS_ACCESS_COUNT; accessRight++)
    {
        if (!RS_ACCESS_MASK_TEST(&rightsToRequest, accessRight))
            continue;

        if (RS_ACCESS_MASK_TEST(&rightsShared, accessRight))
        {
            status = NV_OK;
        }
        else
        {
            status = _rsAccessGrantCallback(pResourceRef, pCallContext, pInvokingClient,
                                            pParentRights, pAllocParams, accessRight);
        }

        if (status == NV_OK)
        {
            RS_ACCESS_MASK_ADD(pResourceRights, accessRight);
        }
        else
        {
            //
            // The default behavior is to silently ignore failure to grant an access right,
            // which happens when the requested access rights are not specified.
            //
            // In contrast, if access rights are explicitly requested (i.e. with
            // the NvRmAllocWithAccess API), we return an error code when we fail to
            // grant access rights.
            //
            if (bExplicitlyRequested)
                return status;
        }
    }

    // Fail if could not get all required rights
    if ((pRightsRequired != NULL) &&
        !rsAccessMaskIsSubset(pResourceRights, pRightsRequired))
    {
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    return NV_OK;
}
