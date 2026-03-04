/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
/*
 * This header file defines the types NVDpyId and NVDpyIdList, as well
 * as inline functions to manipulate these types.  NVDpyId and
 * NVDpyIdList should be treated as opaque by includers of this header
 * file.
 */

#ifndef __NV_DPY_ID_H__
#define __NV_DPY_ID_H__

#include "nvtypes.h"
#include "nvmisc.h"
#include "nv_common_utils.h"
#include <nvlimits.h> /* NV_MAX_SUBDEVICES */

typedef struct {
    NvU32 opaqueDpyId;
} NVDpyId;

typedef struct {
    NvU32 opaqueDpyIdList;
} NVDpyIdList;

#define NV_DPY_ID_MAX_SUBDEVICES NV_MAX_SUBDEVICES
#define NV_DPY_ID_MAX_DPYS_IN_LIST 32

/*
 * For use in combination with nvDpyIdToPrintFormat(); e.g.,
 *
 * printf("dpy id: " NV_DPY_ID_PRINT_FORMAT "\n",
 *        nvDpyIdToPrintFormat(dpyId));
 *
 * The includer should not make assumptions about the return type of
 * nvDpyIdToPrintFormat().
 */
#define NV_DPY_ID_PRINT_FORMAT "0x%08x"

/* functions to return an invalid DpyId and empty DpyIdList */

static inline NVDpyId nvInvalidDpyId(void)
{
    NVDpyId dpyId = { 0 };
    return dpyId;
}

static inline NVDpyIdList nvEmptyDpyIdList(void)
{
    NVDpyIdList dpyIdList = { 0 };
    return dpyIdList;
}

static inline NVDpyIdList nvAllDpyIdList(void)
{
    NVDpyIdList dpyIdList = { ~0U };
    return dpyIdList;
}

static inline void
nvEmptyDpyIdListSubDeviceArray(NVDpyIdList dpyIdList[NV_DPY_ID_MAX_SUBDEVICES])
{
    int dispIndex;
    for (dispIndex = 0; dispIndex < NV_DPY_ID_MAX_SUBDEVICES; dispIndex++) {
        dpyIdList[dispIndex] = nvEmptyDpyIdList();
    }
}

/* set operations on DpyIds and DpyIdLists: Add, Subtract, Intersect, Xor */

static inline __attribute__ ((warn_unused_result))
NVDpyIdList nvAddDpyIdToDpyIdList(NVDpyId dpyId, NVDpyIdList dpyIdList)
{
    NVDpyIdList tmpDpyIdList;
    tmpDpyIdList.opaqueDpyIdList = dpyIdList.opaqueDpyIdList |
        dpyId.opaqueDpyId;
    return tmpDpyIdList;
}

/* Passing an invalid display ID makes this function return an empty list. */
static inline __attribute__ ((warn_unused_result))
NVDpyIdList nvAddDpyIdToEmptyDpyIdList(NVDpyId dpyId)
{
    NVDpyIdList tmpDpyIdList;
    tmpDpyIdList.opaqueDpyIdList =  dpyId.opaqueDpyId;
    return tmpDpyIdList;
}

static inline __attribute__ ((warn_unused_result))
NVDpyIdList nvAddDpyIdListToDpyIdList(NVDpyIdList dpyIdListA,
                                      NVDpyIdList dpyIdListB)
{
    NVDpyIdList tmpDpyIdList;
    tmpDpyIdList.opaqueDpyIdList = dpyIdListB.opaqueDpyIdList |
        dpyIdListA.opaqueDpyIdList;
    return tmpDpyIdList;
}

/* Returns: dpyIdList - dpyId */
static inline __attribute__ ((warn_unused_result))
NVDpyIdList nvDpyIdListMinusDpyId(NVDpyIdList dpyIdList, NVDpyId dpyId)
{
    NVDpyIdList tmpDpyIdList;
    tmpDpyIdList.opaqueDpyIdList = dpyIdList.opaqueDpyIdList &
        (~dpyId.opaqueDpyId);
    return tmpDpyIdList;
}

/* Returns: dpyIdListA - dpyIdListB */
static inline __attribute__ ((warn_unused_result))
NVDpyIdList nvDpyIdListMinusDpyIdList(NVDpyIdList dpyIdListA,
                                      NVDpyIdList dpyIdListB)
{
    NVDpyIdList tmpDpyIdList;
    tmpDpyIdList.opaqueDpyIdList = dpyIdListA.opaqueDpyIdList &
        (~dpyIdListB.opaqueDpyIdList);
    return tmpDpyIdList;
}

static inline __attribute__ ((warn_unused_result))
NVDpyIdList nvIntersectDpyIdAndDpyIdList(NVDpyId dpyId, NVDpyIdList dpyIdList)
{
    NVDpyIdList tmpDpyIdList;
    tmpDpyIdList.opaqueDpyIdList = dpyIdList.opaqueDpyIdList &
        dpyId.opaqueDpyId;
    return tmpDpyIdList;
}

static inline __attribute__ ((warn_unused_result))
NVDpyIdList nvIntersectDpyIdListAndDpyIdList(NVDpyIdList dpyIdListA,
                                             NVDpyIdList dpyIdListB)
{
    NVDpyIdList tmpDpyIdList;
    tmpDpyIdList.opaqueDpyIdList = dpyIdListA.opaqueDpyIdList &
        dpyIdListB.opaqueDpyIdList;
    return tmpDpyIdList;
}

static inline __attribute__ ((warn_unused_result))
NVDpyIdList nvXorDpyIdAndDpyIdList(NVDpyId dpyId, NVDpyIdList dpyIdList)
{
    NVDpyIdList tmpDpyIdList;
    tmpDpyIdList.opaqueDpyIdList = dpyIdList.opaqueDpyIdList ^
        dpyId.opaqueDpyId;
    return tmpDpyIdList;
}

static inline __attribute__ ((warn_unused_result))
NVDpyIdList nvXorDpyIdListAndDpyIdList(NVDpyIdList dpyIdListA,
                                       NVDpyIdList dpyIdListB)
{
    NVDpyIdList tmpDpyIdList;
    tmpDpyIdList.opaqueDpyIdList = dpyIdListA.opaqueDpyIdList ^
        dpyIdListB.opaqueDpyIdList;
    return tmpDpyIdList;
}


/* boolean checks */

static inline NvBool nvDpyIdIsInDpyIdList(NVDpyId dpyId,
                                          NVDpyIdList dpyIdList)
{
    return !!(dpyIdList.opaqueDpyIdList & dpyId.opaqueDpyId);
}

static inline NvBool nvDpyIdIsInvalid(NVDpyId dpyId)
{
    return (dpyId.opaqueDpyId == 0);
}

static inline NvBool nvDpyIdListIsEmpty(NVDpyIdList dpyIdList)
{
    return (dpyIdList.opaqueDpyIdList == 0);
}

static inline NvBool
nvDpyIdListSubDeviceArrayIsEmpty(NVDpyIdList
                                 dpyIdList[NV_DPY_ID_MAX_SUBDEVICES])
{
    int dispIndex;
    for (dispIndex = 0; dispIndex < NV_DPY_ID_MAX_SUBDEVICES; dispIndex++) {
        if (!nvDpyIdListIsEmpty(dpyIdList[dispIndex])) {
            return NV_FALSE;
        }
    }
    return NV_TRUE;
}


static inline NvBool nvDpyIdsAreEqual(NVDpyId dpyIdA, NVDpyId dpyIdB)
{
    return (dpyIdA.opaqueDpyId == dpyIdB.opaqueDpyId);
}

static inline NvBool nvDpyIdListsAreEqual(NVDpyIdList dpyIdListA,
                                          NVDpyIdList dpyIdListB)
{
    return (dpyIdListA.opaqueDpyIdList == dpyIdListB.opaqueDpyIdList);
}

static inline NvBool nvDpyIdListIsASubSetofDpyIdList(NVDpyIdList dpyIdListA,
                                                     NVDpyIdList dpyIdListB)
{
    NVDpyIdList intersectedDpyIdList =
        nvIntersectDpyIdListAndDpyIdList(dpyIdListA, dpyIdListB);

    return nvDpyIdListsAreEqual(intersectedDpyIdList, dpyIdListA);
}


/*
 * retrieve the individual dpyIds from dpyIdList; if dpyId is invalid,
 * start at the beginning of the list; otherwise, start at the dpyId
 * after the specified dpyId
 */

static inline __attribute__ ((warn_unused_result))
NVDpyId nvNextDpyIdInDpyIdListUnsorted(NVDpyId dpyId, NVDpyIdList dpyIdList)
{
    if (nvDpyIdIsInvalid(dpyId)) {
        dpyId.opaqueDpyId = 1;
    } else {
        dpyId.opaqueDpyId <<= 1;
    }

    while (dpyId.opaqueDpyId) {

        if (nvDpyIdIsInDpyIdList(dpyId, dpyIdList)) {
            return dpyId;
        }

        dpyId.opaqueDpyId <<= 1;
    }

    /* no dpyIds left in dpyIdlist; return the invalid dpyId */

    return nvInvalidDpyId();
}

#define FOR_ALL_DPY_IDS(_dpyId, _dpyIdList)                           \
    for ((_dpyId) = nvNextDpyIdInDpyIdListUnsorted(nvInvalidDpyId(),  \
                                                   (_dpyIdList));     \
         !nvDpyIdIsInvalid(_dpyId);                                   \
         (_dpyId) = nvNextDpyIdInDpyIdListUnsorted((_dpyId),          \
                                                   (_dpyIdList)))

/* report how many dpyIds are in the dpyIdList */

static inline int nvCountDpyIdsInDpyIdList(NVDpyIdList dpyIdList)
{
    return nvPopCount32(dpyIdList.opaqueDpyIdList);
}

static inline int
nvCountDpyIdsInDpyIdListSubDeviceArray(NVDpyIdList
                                       dpyIdList[NV_DPY_ID_MAX_SUBDEVICES])
{
    int dispIndex, n = 0;

    for (dispIndex = 0; dispIndex < NV_DPY_ID_MAX_SUBDEVICES; dispIndex++) {
        n += nvCountDpyIdsInDpyIdList(dpyIdList[dispIndex]);
    }

    return n;
}

/* convert between dpyId/dpyIdList and NV-CONTROL values */

static inline int nvDpyIdToNvControlVal(NVDpyId dpyId)
{
    return (int) dpyId.opaqueDpyId;
}

static inline int nvDpyIdListToNvControlVal(NVDpyIdList dpyIdList)
{
    return (int) dpyIdList.opaqueDpyIdList;
}

static inline NVDpyId nvNvControlValToDpyId(int val)
{
    NVDpyId dpyId;
    dpyId.opaqueDpyId = (val == 0) ? 0 : 1 << (nv_ffs(val)-1);
    return dpyId;
}

static inline NVDpyIdList nvNvControlValToDpyIdList(int val)
{
    NVDpyIdList dpyIdList;
    dpyIdList.opaqueDpyIdList = val;
    return dpyIdList;
}


/* convert between dpyId and NvU32 */

static inline NVDpyId nvNvU32ToDpyId(NvU32 val)
{
    NVDpyId dpyId;
    dpyId.opaqueDpyId = (val == 0) ? 0 : 1 << (nv_ffs(val)-1);
    return dpyId;
}

static inline NVDpyIdList nvNvU32ToDpyIdList(NvU32 val)
{
    NVDpyIdList dpyIdList;
    dpyIdList.opaqueDpyIdList = val;
    return dpyIdList;
}

static inline NvU32 nvDpyIdToNvU32(NVDpyId dpyId)
{
    return dpyId.opaqueDpyId;
}

static inline NvU32 nvDpyIdListToNvU32(NVDpyIdList dpyIdList)
{
    return dpyIdList.opaqueDpyIdList;
}

/* Return the bit position of dpyId: a number in the range [0..31]. */
static inline NvU32 nvDpyIdToIndex(NVDpyId dpyId)
{
    return nv_ffs(dpyId.opaqueDpyId) - 1;
}

/* Return a display ID that is not in the list passed in. */

static inline NVDpyId nvNewDpyId(NVDpyIdList excludeList)
{
    NVDpyId dpyId;
    if (~excludeList.opaqueDpyIdList == 0) {
        return nvInvalidDpyId();
    }
    dpyId.opaqueDpyId =
        1U << (nv_ffs(~excludeList.opaqueDpyIdList) - 1);
    return dpyId;
}

/* See comment for NV_DPY_ID_PRINT_FORMAT. */
static inline NvU32 nvDpyIdToPrintFormat(NVDpyId dpyId)
{
    return nvDpyIdToNvU32(dpyId);
}

/* Prevent usage of opaque values. */
#define opaqueDpyId __ERROR_ACCESS_ME_VIA_NV_DPY_ID_H
#define opaqueDpyIdList __ERROR_ACCESS_ME_VIA_NV_DPY_ID_H

#endif /* __NV_DPY_ID_H__ */
