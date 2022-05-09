/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "core/core.h"
#include "resource_desc.h"

// Need the full header for the class allocation param structure.
#define SDK_ALL_CLASSES_INCLUDE_FULL_HEADER
#include "g_allclasses.h"
// Not a class header, but contains an allocation struct used by several classes
#include "class/clb0b5sw.h"
#include "nvos.h"

#include "rmapi/alloc_size.h"
#include "rmapi/resource_fwd_decls.h"
#include "resserv/rs_access_rights.h"

//
// Macros to transform list into static table
//

// NULL terminated list
#define RS_LIST(...) {__VA_ARGS__, 0}
#define RS_ROOT_OBJECT {0}
#define RS_ANY_PARENT {0}

// Populate parents
#define RS_ENTRY(cls, internalClass, bMultiInstance, parentList, allocParam, freePriority, flags, rightsRequired) \
    NvU32 cls##ParentList[] = parentList;

#include "resource_list.h"

#undef RS_LIST
#undef RS_ROOT_OBJECT
#undef RS_ANY_PARENT


#define RS_ACCESS_NONE {-1}
#define RS_ACCESS_LIST(...) {__VA_ARGS__}

// Populate rights required
#define RS_ENTRY(cls, internalClass, bMultiInstance, parentList, allocParam, freePriority, flags, rightsRequired) \
    static const RsAccessRight cls##_RightsRequiredArray[] = rightsRequired;

#include "resource_list.h"

#undef RS_ACCESS_NONE
#undef RS_ACCESS_LIST

// Populate forward declarations
#define RS_ENTRY(cls, internalClass, bMultiInstance, parentList, allocParam, freePriority, flags, rightsRequired) \
    extern const struct NVOC_CLASS_DEF __nvoc_class_def_##internalClass; /* defn here to keep POPULATE_STRUCT happy if the class is disabled */

#include "resource_list.h"


#define RS_REQUIRED(allocParam) sizeof(allocParam), NV_TRUE
#define RS_OPTIONAL(allocParam) sizeof(allocParam), NV_FALSE
#define RS_NONE                 0, NV_FALSE
#define RS_ENTRY(cls, internalClass, bMultiInstance, bAnyParent, allocParam, freePriority, flags, bRightsRequired) \
{ \
    cls, \
    classId(internalClass), \
    classInfo(internalClass), \
    allocParam, \
    bMultiInstance, \
    bAnyParent, \
    cls##ParentList, \
    freePriority, \
    flags, \
    cls##_RightsRequiredArray, \
    bRightsRequired ? NV_ARRAY_ELEMENTS(cls##_RightsRequiredArray) : 0, \
},

#define RS_LIST(...) NV_FALSE
#define RS_ROOT_OBJECT NV_FALSE
#define RS_ANY_PARENT NV_TRUE
#define RS_ACCESS_NONE NV_FALSE
#define RS_ACCESS_LIST(...) NV_TRUE
static RS_RESOURCE_DESC
g_RsResourceDescList[] =
{
#include "resource_list.h"
};
#undef RS_LIST
#undef RS_ROOT_OBJECT
#undef RS_ANY_PARENT
#undef RS_ACCESS_NONE
#undef RS_ACCESS_LIST
#undef RS_REQUIRED
#undef RS_OPTIONAL
#undef RS_NONE

#define NUM_ENTRIES_DESC_LIST NV_ARRAY_ELEMENTS32(g_RsResourceDescList)

void RsResInfoInitialize(void)
{
    //
    // Keep the array sorted by externalClassId, so we can binary search it
    // Simple bubble-sort is fine here as the number of elements is below 300,
    // and we only call this once on boot anyway.
    //
    NvU32 i, j;
    for (i = 0; i < NUM_ENTRIES_DESC_LIST - 1; i++)
    {
        for (j = i + 1; j < NUM_ENTRIES_DESC_LIST; j++)
        {
            RS_RESOURCE_DESC *a = &g_RsResourceDescList[i];
            RS_RESOURCE_DESC *b = &g_RsResourceDescList[j];

            if (a->externalClassId > b->externalClassId)
            {
                RS_RESOURCE_DESC tmp;
                portMemCopy(&tmp, sizeof(tmp), a,    sizeof(*a));
                portMemCopy(a,    sizeof(*a),  b,    sizeof(*b));
                portMemCopy(b,    sizeof(*b),  &tmp, sizeof(tmp));
            }
        }
    }
}

RS_RESOURCE_DESC *
RsResInfoByExternalClassId
(
    NvU32 externalClassId
)
{
    NvU32 low  = 0;
    NvU32 high = NUM_ENTRIES_DESC_LIST;

    // Binary search the array; If not found, the break in the middle will be hit
    while (1)
    {
        NvU32 mid  = (low + high) / 2;

        if (g_RsResourceDescList[mid].externalClassId == externalClassId)
            return &g_RsResourceDescList[mid];

        if (high == mid || low == mid)
            break;

        if (g_RsResourceDescList[mid].externalClassId > externalClassId)
            high = mid;
        else
            low = mid;
    }

    return NULL;
}

NvU32 RsResInfoGetInternalClassId(const RS_RESOURCE_DESC *pResDesc)
{
    return pResDesc ? pResDesc->internalClassId : 0;
}

void RsResInfoGetResourceList(const RS_RESOURCE_DESC **ppResourceList, NvU32 *numResources)
{
    *ppResourceList = g_RsResourceDescList;
    *numResources = NV_ARRAY_ELEMENTS(g_RsResourceDescList);
}

NV_STATUS
rmapiGetClassAllocParamSize
(
    NvU32   *pAllocParamSizeBytes,
    NvP64    pUserParams,
    NvBool  *pBAllowNull,
    NvU32    hClass
)
{
    RS_RESOURCE_DESC *pResDesc;

    *pAllocParamSizeBytes = 0;
    *pBAllowNull = NV_FALSE;

    pResDesc = RsResInfoByExternalClassId(hClass);

    if (!pResDesc)
        return NV_ERR_INVALID_CLASS;

    if (pResDesc->bParamRequired)
    {
        // params are required
        *pAllocParamSizeBytes = pResDesc->allocParamSize;
    }
    else if (pResDesc->allocParamSize)
    {
        // params are *optional*
        *pBAllowNull = NV_TRUE;
        if (pUserParams != (NvP64) 0)
            *pAllocParamSizeBytes = pResDesc->allocParamSize;
    }
    else
    {
        // no params
        *pBAllowNull = NV_TRUE;
    }

    return NV_OK;
}
