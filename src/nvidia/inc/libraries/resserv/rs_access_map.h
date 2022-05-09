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
#ifndef RS_ACCESS_MAP_H
#define RS_ACCESS_MAP_H

#include "nvstatus.h"
#include "nvtypes.h"

#include "containers/map.h"
#include "resserv/resserv.h"
#include "resserv/rs_access_rights.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Returns the client's access mask for the resource, returning NULL if
 *        the resource is not owned by the client
 *
 * @param[in] pResourceRef The resource whose access mask is being checked
 * @param[in] pClient The client accessing the resource
 *
 * @return The resource's access rights mask, or
 * @return NULL if pClient does not own pResourceRef
 */
RS_ACCESS_MASK *rsAccessLookup(RsResourceRef *pResourceRef, RsClient *pClient);

/*!
 * @brief Fills a mask with all rights available to a client on a resource
 *        Gets both rights directly available through the access mask,
 *        as well as rights shared by the resource.
 *
 * @param[in] pResourceRef
 * @param[in] pClient
 * @param[out] pRightsShared The set of access rights available for this client on this resource
 *
 * @return none
 */
void rsAccessGetAvailableRights(RsResourceRef *pResourceRef, RsClient *pClient,
                                RS_ACCESS_MASK *pAvailableRights);

/*!
 * @brief Perform an access rights check on a target resource
 *
 * This function should be used to determine whether sufficient access
 * rights are already present, NOT whether access rights should be granted.
 * It will not update any state on its own.
 *
 * For each of the required access rights, the invoking client must hold
 * that access right on the target resource.
 *
 * @param[in] pResourceRef A reference to the target resource for which we are
 *            checking access rights
 * @param[in] pInvokingClient The client that is requesting access rights
 * @param[in] pRightsRequired The set of access rights that the invoking client
 *            should have on the target resource
 *
 * @return NV_OK if the invoking client has the required access rights on the
 *         target resource
 * @return NV_ERR_INSUFFICIENT_PERMISSIONS if the invoking client does not have
 *         the required access rights on the target resource
 */
NV_STATUS rsAccessCheckRights(RsResourceRef *pResourceRef, RsClient *pInvokingClient,
                              const RS_ACCESS_MASK *pRightsRequired);

/*!
 * @brief Update what access rights are currently owned on a target resource
 *        based on the target resource's current privilege.
 *
 * This function should be used to update the access rights currently owned
 * by the target resource. Most access rights are only obtained once and don't
 * disappear/reappear. However, the RS_ACCESS_FLAG_UNCACHED_CHECK flag can be
 * used to indicate access rights that are present/not present based on the target
 * resource's current level of privilege, NOT what the level of privilege was when
 * the access right was initially requested. This function is useful for updating the
 * which access rights are owned when accounting for uncached access rights.
 *
 * @param[in] pResourceRef A reference to the target resource for which we are
 *            checking access rights
 * @param[in] pInvokingClient The client to check level of access with
 * @param[in] pRightsToUpdate If non-NULL, only access rights set in this mask
 *                            will be updated
 *
 * @return none
 */
void rsAccessUpdateRights(RsResourceRef *pResourceRef, RsClient *pInvokingClient,
                          const RS_ACCESS_MASK *pRightsToUpdate);

/*!
 * @brief Searches a resource's share list for an entry equal to the
 *        passed in share policy, as defined by rsSharePolicyEquals
 *
 * @param[in] pShareList The RsShareList to be searched
 * @param[in] pSharePolicy The policy to be found
 *
 * @return A pointer to the corresponding policy, or
 * @return NULL if no matching entry is found
 */
RS_SHARE_POLICY *rsShareListLookup(RsShareList *pShareList, RS_SHARE_POLICY *pSharePolicy);

/*!
 * @brief Adds a new share policy to a resource's share list, or merges into
 *        an existing policy, if possible
 *
 * @param[in] pShareList The RsShareList to be searched
 * @param[in] pSharePolicy The policy to be added to the list, may be merged with
 *                         another policy with a matching pSharePolicy->type and ->target.
 *                         In this case, ->accessMask for the existing entry and the
 *                         new pSharePolicy will be merged with a union operation.
 * @param[out] pAccessMask The rights now shared for this share policy, may or
 *             may not match pSharePolicy->accessMask if merged with an existing policy.
 *             User may pass NULL, in which case nothing is written into this.
 *
 * @return NV_OK if the operation succeeded,
 * @return NV_ERR_NO_MEMORY if a new element needed to be added to the list, but
 *         insufficient memory was present to allocate one
 */
NV_STATUS rsShareListInsert(RsShareList *pShareList, RS_SHARE_POLICY *pSharePolicy,
                            RS_ACCESS_MASK *pAccessMask);

/*!
 * @brief Removes certain rights from being shared in a share policy entry
 *        from a resource's RsShareList.
 *
 * @param[in] pShareList The RsShareList to be searched
 * @param[in] pSharePolicy The policy to be removed from the list, matched using
 *                         pSharePolicy->type and ->target. Only rights specified in
 *                         pSharePolicy->accessMask are revoked, others will remain.
 *                         Use RS_ACCESS_MASK_FILL to for a full mask to revoke all rights.
 * @param[out] pAccessMask The rights still shared for this share policy, may or
 *             may not be empty.
 *             User may pass NULL, in which case nothing is written into this.
 *
 * @return none
 */
void rsShareListRemove(RsShareList *pShareList, RS_SHARE_POLICY *pSharePolicy,
                       RS_ACCESS_MASK *pAccessMask);

/*!
 * @brief Copy one share list into another
 *        Note that this does not replace the Dst list if it is not empty,
 *        elements will be appended onto any existing list.
 *
 * @param[in] pShareListDst The list to copy into
 * @param[in] pShareListSrc The list to copy from
 *
 * @return NV_ERR_NO_MEMORY, NV_OK
 */
NV_STATUS rsShareListCopy(RsShareList *pShareListDst, RsShareList *pShareListSrc);

/*!
 * @brief Returns the list which should be used for a resource's sharing
 *        In order, selects either the resource's own list, the client's inherited
 *        list, or the server's global default list.
 *
 * @param[in] pResourceRef
 * @param[in] pServer
 *
 * @return A pointer to the relevant share list
 * @return NULL if no list is available, and no pServer was provided.
 */
RsShareList * rsAccessGetActiveShareList(RsResourceRef *pResourceRef, RsServer *pServer);

/*!
 * @brief Attempts to grant a set of requested access rights on this resource.
 *
 * This function will attempt to grant the rights specified in pRightsRequested
 * to the client referred to by pClient. If successful, it will update the
 * access rights of the target resource referred to by pResourceRef.
 *
 * The resAccessCallback method on the target resource will be invoked to
 * perform checks. This requires that the target resource
 * pResourceRef->pResource already be initialized.
 *
 * If pRightsRequested is non-NULL, then the call will return an error code if
 * it is unable to grant any of the requested rights.
 *
 * If pRightsRequested is NULL, then the call will ignore any failure to
 * grant, taking a "best-effort" approach to granting access rights. The
 * rights requested will be determined as follows:
 *
 *   - If pResourceRef is a client resource, the function will attempt to
 *     request all possible access rights
 *   - For any other resource, the function will attempt to request the
 *     same set of access rights held by the invoking client on the parent
 *     resource
 *
 * @param[in] pResourceRef The target resource reference on which access
 *            rights will be granted
 * @param[in] pCallContext Information about the call context
 * @param[in] pInvokingClient The client requesting the access right
 * @param[in] pRightsRequested The set of access rights to attempt to grant,
 *            or NULL if no access rights were explicitly requested
 * @param[in] pRightsRequired Any rights additionally required for the operation,
 *            will be requested if pRightsRequested is not specified.
 *            If specified, all rights in this mask must be granted for the call to succeed.
 * @param[in] pAllocParams per-class allocation parameters passed into Alloc,
 *            NULL if this is not being called from the Alloc path.
 *
 * @return NV_OK if the access right should be granted
 * @return NV_ERR_INSUFFICIENT_PERMISSIONS if access rights were
 *         explicitly requested, and the function failed to grant all of the
 *         requested access rights
 */
NV_STATUS rsAccessGrantRights(RsResourceRef *pResourceRef, CALL_CONTEXT *pCallContext,
                              RsClient *pInvokingClient, const RS_ACCESS_MASK *pRightsRequested,
                              const RS_ACCESS_MASK *pRightsRequired, void *pAllocParams);

#ifdef __cplusplus
}
#endif

#endif /* RS_ACCESS_MAP_H */
