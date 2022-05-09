/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _RESOURCE_DESC_H_
#define _RESOURCE_DESC_H_

#include "nvtypes.h"
#include "nvoc/runtime.h"
#include "resserv/rs_access_rights.h"

// Flags for RS_ENTRY
#define RS_FLAGS_NONE                             0

#define RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_ALLOC       NVBIT(0)  ///< GPUs Lock is acquired on allocation
#define RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE        NVBIT(1)  ///< GPUs Lock is acquired for free
#define RS_FLAGS_ACQUIRE_GPUS_LOCK                (RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPUS_LOCK_ON_FREE)

#define RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_ALLOC  NVBIT(2)  ///< GPU Group Lock is acquired on allocation
#define RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_FREE   NVBIT(3)  ///< GPU Group Lock is acquired for free
#define RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK           (RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_ALLOC | RS_FLAGS_ACQUIRE_GPU_GROUP_LOCK_ON_FREE)

#define RS_FLAGS_ALLOC_RPC_TO_VGPU_HOST           NVBIT(4)  ///< Issue RPC to host to allocate resource for virtual GPUs

#define RS_FLAGS_ACQUIRE_RO_API_LOCK_ON_ALLOC     NVBIT(5)  ///< Acquire the RO API lock for allocation, default is RW API lock

#define RS_FLAGS_ALLOC_RPC_TO_PHYS_RM             NVBIT(6)  ///< Issue RPC to allocate resource in physical RM

#define RS_FLAGS_ALLOC_RPC_TO_ALL                 (RS_FLAGS_ALLOC_RPC_TO_VGPU_HOST | RS_FLAGS_ALLOC_RPC_TO_PHYS_RM)

#define RS_FLAGS_INTERNAL_ONLY                    NVBIT(7)  ///< Class cannot be allocated outside of RM

#define RS_FLAGS_CHANNEL_DESCENDANT_COMMON        (RS_FLAGS_ACQUIRE_GPUS_LOCK | RS_FLAGS_ALLOC_RPC_TO_ALL)

#define RS_FREE_PRIORITY_DEFAULT                  0
#define RS_FREE_PRIORITY_HIGH                     1       ///< Resources with this priority will be freed ahead of others

/**
 * Information about a RsResource subclass.
 */
typedef struct RS_RESOURCE_DESC
{
    NvU32                  externalClassId;       ///< Id of the class as seen by the client
    NvU32                  internalClassId;       ///< NVOC class ID, mirrored from pClassInfo->classId
    const NVOC_CLASS_INFO *pClassInfo;            ///< RTTI information for internal class
    NvU32                  allocParamSize;        ///< Size of allocation param structure
    NvBool                 bParamRequired;        ///< If not required, param size can be 0 or allocParamSize
    NvBool                 bMultiInstance;        ///< Multiple instances of this object under a parent
    NvBool                 bAnyParent;            ///< Resource can be allocated under any parent
    NvU32                 *pParentList;           ///< NULL terminated list of internalClassId of parents
    NvU32                  freePriority;          ///< RS_FREE_PRIORITY_*
    NvU32                  flags;                 ///< Flags
    const RsAccessRight   *pRightsRequiredArray;  ///< Access rights required to allocate this resource
    NvLength               rightsRequiredLength;  ///< Length of pRightsRequiredArray
} RS_RESOURCE_DESC;

/** Initialize the global resource info table */
void RsResInfoInitialize(void);

/**
 * Look up RS_RESOURCE_DESC using the externalClassId. The id of the class as
 * seen by clients.
 */
RS_RESOURCE_DESC *RsResInfoByExternalClassId(NvU32 externalClassId);
NvU32 RsResInfoGetInternalClassId(const RS_RESOURCE_DESC *);

/** Get the global resource info table */
void RsResInfoGetResourceList(const RS_RESOURCE_DESC **ppResourceList, NvU32 *numResources);

#endif // _RESOURCE_DESC_H_
