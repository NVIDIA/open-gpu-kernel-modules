/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _RS_UTILS_H_
#define _RS_UTILS_H_

/**
 * @defgroup RsUtilities
 *
 * Provides convenience utilities for resserv. Utility functions provide
 * abstractions that take handles as inputs -- helpful for legacy code that
 * passes hClient or hResource handles and not underlying objects. Desire
 * is for pClient and RsResourceRef types to be used for new code instead of
 * passing handles around and this utility module phased out.
 *
 * @{
 */

#include "resserv/rs_server.h"
#include "resserv/rs_client.h"
#include "resserv/rs_resource.h"

#include "rmapi/client.h"

#include "containers/list.h"

MAKE_LIST(ClientHandlesList, NvHandle);

#define serverutilGetDerived(pRmClient, hResource, ppBaseRes, type) \
    (clientGetResource(staticCast((pRmClient), RsClient),           \
            (hResource),                                            \
            classId(type),                                          \
            (ppBaseRes)) != NV_OK)                                  \
            ? NULL                                                  \
            : dynamicCast(*(ppBaseRes), type)

/**
  * Get the reference to a resource
  * @param[in]   hClient Client handle
  * @param[in]   hResource The resource to lookup
  * @param[out]  ppResourceRef The reference to the resource
  */
NV_STATUS serverutilGetResourceRef(NvHandle hClient, NvHandle hObject,
                                  RsResourceRef **ppResourceRef);

/**
  * Get the reference to a resource (with a type check)
  * @param[in]   hClient Client handle
  * @param[in]   hResource The resource to lookup
  * @param[out]  ppResourceRef The reference to the resource
  */
NV_STATUS serverutilGetResourceRefWithType(NvHandle hClient, NvHandle hObject,
                                          NvU32 internalClassId, RsResourceRef **ppResourceRef);

/**
  * Get the reference to a resource (with a type and parent check)
  * @param[in]   hClient Client handle
  * @param[in]   hResource The resource to lookup
  * @param[out]  ppResourceRef The reference to the resource
  */
NV_STATUS serverutilGetResourceRefWithParent(NvHandle hClient, NvHandle hParent, NvHandle hObject,
                                            NvU32 internalClassId, RsResourceRef **ppResourceRef);

/**
 * Find the first child object of given type
 */
RsResourceRef *serverutilFindChildRefByType(NvHandle hClient, NvHandle hParent, NvU32 internalClassId, NvBool bExactMatch);


/**
 * Get an iterator to the elements in the client's resource map
 *
 * See clientRefIter for documentation on hScopedObject and iterType
 */
RS_ITERATOR serverutilRefIter(NvHandle hClient, NvHandle hScopedObject, NvU32 internalClassId, RS_ITER_TYPE iterType, NvBool bExactMatch);

/**
 * Get an iterator to the elements in the server's shared object map
 */
RS_SHARE_ITERATOR serverutilShareIter(NvU32 internalClassId);

/**
 * Get an iterator to the elements in the server's shared object map
 */
NvBool serverutilShareIterNext(RS_SHARE_ITERATOR* pIt);

/**
 * Validate that a given resource handle is well-formed and does not already
 * exist under a given client.
 */
NvBool serverutilValidateNewResourceHandle(NvHandle, NvHandle);

/**
 * Generate an unused handle for a resource. The handle will be generated in the white-listed range that was
 * specified when the client was allocated.
 */
NV_STATUS serverutilGenResourceHandle(NvHandle, NvHandle*);

/**
 * Get a client pointer from a client handle without taking any locks.
 *
 * @param[in]   hClient The client to acquire
 */
RmClient *serverutilGetClientUnderLock(NvHandle hClient);

/**
 * Get a client pointer from a client handle and lock it.
 *
 * @param[in]   hClient The client to acquire
 * @param[in]   access LOCK_ACCESS_*
 * @param[out]  ppClientEntry Pointer to the CLIENT_ENTRY
 * @param[out]  ppClient Pointer to the RmClient
 */
NV_STATUS serverutilAcquireClient(NvHandle hClient, LOCK_ACCESS_TYPE access, CLIENT_ENTRY **ppClientEntry, RmClient **ppClient);

/**
 * Unlock a client
 *
 * @param[in]   access LOCK_ACCESS_*
 * @param[in]   pClientEntry Pointer to the CLIENT_ENTRY
 */
void serverutilReleaseClient(LOCK_ACCESS_TYPE access, CLIENT_ENTRY *pClientEntry);

/**
 * Get the first valid client pointer in resource server without taking any locks.
 */
RmClient **serverutilGetFirstClientUnderLock(void);

/**
 * Get the next valid client pointer in resource server without taking any locks.
 *
 * @param[in]   ppClient Pointer returned by a previous call to
 *                       serverutilGetFirstClientUnderLock or
 *                       serverutilGetNextClientUnderLock
 */
RmClient **serverutilGetNextClientUnderLock(RmClient **pClient);

/*!
 * @brief Retrieve all hClients allocated for the given (ProcID, SubProcessID)
 *
 * This function iterates through all the clients in the resource server and finds
 * hClients allocated for the given (ProcID, SubProcessID) and returns them to
 * the caller.
 *
 * @param[in]  procID           Process ID
 * @param[in]  subProcessID     SubProcess ID
 * @param[out] pClientList      List in which the client handles are returned
 *
 * @return NV_STATUS
 */
NV_STATUS serverutilGetClientHandlesFromPid(NvU32 procID, NvU32 subProcessID, ClientHandlesList *pClientList);

/**
 * This is a filtering function intended to be used with refFindCpuMappingWithFilter.
 * This filter will only match user mappings belonging to the current process.
 *
 * @param[in] ppMapping The mapping that is being filtered
 */
NvBool serverutilMappingFilterCurrentUserProc(RsCpuMapping *ppMapping);

/**
 * This is a filtering function intended to be used with refFindCpuMappingWithFilter.
 * This filter will only match kernel mappings.
 *
 * @param[in] ppMapping The mapping that is being filtered
 */
NvBool serverutilMappingFilterKernel(RsCpuMapping *ppMapping);

#endif
