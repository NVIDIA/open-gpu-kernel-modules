/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "nvoc/object.h"
#include "resserv/rs_access_rights.h"
#include "resource_desc_flags.h"

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
