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
#include "utils/nvassert.h"
#include "nvctassert.h"

#include "resserv/rs_access_rights.h"


// Ensure the number of declared access rights is within the capacity
// provided by the number of limbs used.
// Also, NVOC acces_right is NvU32 currently. It requires NVOC change to support 32+ bits
ct_assert(RS_ACCESS_COUNT <= SDK_RS_ACCESS_MAX_COUNT);


#if !(RS_STANDALONE_TEST)
const RS_ACCESS_INFO g_rsAccessMetadata[RS_ACCESS_COUNT] =
{
    // RS_ACCESS_DUP_OBJECT
    {
        RS_ACCESS_FLAG_ALLOW_OWNER
    },

    // RS_ACCESS_NICE
    {
        RS_ACCESS_FLAG_ALLOW_PRIVILEGED | RS_ACCESS_FLAG_UNCACHED_CHECK
    },

    // RS_ACCESS_DEBUG
    {
        RS_ACCESS_FLAG_ALLOW_OWNER
    },
};
#endif /* RS_STANDALONE_TEST */


NvBool
rsAccessMaskIsSubset
(
    const RS_ACCESS_MASK *pRightsPresent,
    const RS_ACCESS_MASK *pRightsRequired
)
{
    RsAccessRight accessRight;

    for (accessRight = 0; accessRight < RS_ACCESS_COUNT; accessRight++)
    {
        if (RS_ACCESS_MASK_TEST(pRightsRequired, accessRight) &&
            !RS_ACCESS_MASK_TEST(pRightsPresent, accessRight))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}


NvBool
rsAccessMaskIsEmpty
(
    const RS_ACCESS_MASK *pAccessMask
)
{
    RsAccessRight accessRight;

    for (accessRight = 0; accessRight < RS_ACCESS_COUNT; accessRight++)
    {
        if (RS_ACCESS_MASK_TEST(pAccessMask, accessRight))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}


void
rsAccessMaskFromArray
(
    RS_ACCESS_MASK *pAccessMask,
    const RsAccessRight *pRightsArray,
    NvLength length
)
{
    NvLength i;

    RS_ACCESS_MASK_CLEAR(pAccessMask);

    NV_ASSERT_OR_RETURN_VOID(pRightsArray != NULL);

    for (i = 0; i < length; i++)
    {
        RS_ACCESS_MASK_ADD(pAccessMask, pRightsArray[i]);
    }
}
