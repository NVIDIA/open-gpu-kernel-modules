/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nvtypes.h>
#if defined(_MSC_VER)
#pragma warning(disable:4324)
#endif

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      rs_access.finn
//




#include "nvtypes.h"
#include "nvmisc.h"


/****************************************************************************/
/*                       Access right definitions                           */
/****************************************************************************/

//
// The meaning of each access right is documented in
//   resman/docs/rmapi/resource_server/rm_capabilities.adoc
//
// RS_ACCESS_COUNT is the number of access rights that have been defined
// and are in use. All integers in the range [0, RS_ACCESS_COUNT) should
// represent valid access rights.
//
// When adding a new access right, don't forget to update
//   1) The descriptions in the resman/docs/rmapi/resource_server/rm_capabilities.adoc
//   2) RS_ACCESS_COUNT, defined below
//   3) The declaration of g_rsAccessMetadata in rs_access_rights.c
//   4) The list of access rights in drivers/common/chip-config/Chipcontrols.pm
//   5) Any relevant access right callbacks
//

#define RS_ACCESS_DUP_OBJECT 0U
#define RS_ACCESS_NICE       1U
#define RS_ACCESS_DEBUG      2U
#define RS_ACCESS_PERFMON    3U
#define RS_ACCESS_COUNT      4U


/****************************************************************************/
/*                     Access right data structures                         */
/****************************************************************************/

/*!
 * @brief A type that can be used to represent any access right.
 */
typedef NvU16 RsAccessRight;

/*!
 * @brief An internal type used to represent one limb in an access right mask.
 */
typedef NvU32 RsAccessLimb;
#define SDK_RS_ACCESS_LIMB_BITS 32

/*!
 * @brief The number of limbs in the RS_ACCESS_MASK struct.
 */
#define SDK_RS_ACCESS_MAX_LIMBS 1

/*!
 * @brief The maximum number of possible access rights supported by the
 *        current data structure definition.
 *
 * You probably want RS_ACCESS_COUNT instead, which is the number of actual
 * access rights defined.
 */
#define SDK_RS_ACCESS_MAX_COUNT (0x20) /* finn: Evaluated from "(SDK_RS_ACCESS_LIMB_BITS * SDK_RS_ACCESS_MAX_LIMBS)" */

/**
 * @brief A struct representing a set of access rights.
 *
 * Note that the values of bit positions larger than RS_ACCESS_COUNT is
 * undefined, and should not be assumed to be 0 (see RS_ACCESS_MASK_FILL).
 */
typedef struct RS_ACCESS_MASK {
    RsAccessLimb limbs[SDK_RS_ACCESS_MAX_LIMBS];
} RS_ACCESS_MASK;

/**
 * @brief A struct representing auxiliary information about each access right.
 */
typedef struct RS_ACCESS_INFO {
    NvU32 flags;
} RS_ACCESS_INFO;


/****************************************************************************/
/*                           Access right macros                            */
/****************************************************************************/

#define SDK_RS_ACCESS_LIMB_INDEX(index) ((index) / SDK_RS_ACCESS_LIMB_BITS)
#define SDK_RS_ACCESS_LIMB_POS(index)   ((index) % SDK_RS_ACCESS_LIMB_BITS)

#define SDK_RS_ACCESS_LIMB_ELT(pAccessMask, index) \
    ((pAccessMask)->limbs[SDK_RS_ACCESS_LIMB_INDEX(index)])
#define SDK_RS_ACCESS_OFFSET_MASK(index) \
    NVBIT_TYPE(SDK_RS_ACCESS_LIMB_POS(index), RsAccessLimb)

/*!
 * @brief Checks that accessRight represents a valid access right.
 *
 * The valid range of access rights is [0, RS_ACCESS_COUNT).
 *
 * @param[in] accessRight The access right value to check
 *
 * @return true if accessRight is valid
 * @return false otherwise
 */
#define RS_ACCESS_BOUNDS_CHECK(accessRight) \
    (accessRight < RS_ACCESS_COUNT)

/*!
 * @brief Test whether an access right is present in a set
 *
 * @param[in] pAccessMask The set of access rights to read
 * @param[in] index The access right to examine
 *
 * @return NV_TRUE if the access right specified by index was present in the set,
 *         and NV_FALSE otherwise
 */
#define RS_ACCESS_MASK_TEST(pAccessMask, index) \
    (RS_ACCESS_BOUNDS_CHECK(index) && \
        (SDK_RS_ACCESS_LIMB_ELT(pAccessMask, index) & SDK_RS_ACCESS_OFFSET_MASK(index)) != 0)

/*!
 * @brief Add an access right to a mask
 *
 * @param[in] pAccessMask The set of access rights to modify
 * @param[in] index The access right to set
 */
#define RS_ACCESS_MASK_ADD(pAccessMask, index) \
    do \
    { \
        if (RS_ACCESS_BOUNDS_CHECK(index)) { \
            SDK_RS_ACCESS_LIMB_ELT(pAccessMask, index) |= SDK_RS_ACCESS_OFFSET_MASK(index); \
        } \
    } while (NV_FALSE)

/*!
 * @brief Remove an access right from a mask
 *
 * @param[in] pAccessMask The set of access rights to modify
 * @param[in] index The access right to unset
 */
#define RS_ACCESS_MASK_REMOVE(pAccessMask, index) \
    do \
    { \
        if (RS_ACCESS_BOUNDS_CHECK(index)) { \
            SDK_RS_ACCESS_LIMB_ELT(pAccessMask, index) &= ~SDK_RS_ACCESS_OFFSET_MASK(index); \
        } \
    } while (NV_FALSE)

/*!
 * @brief Performs an in-place union between two access right masks
 *
 * @param[in,out] pMaskOut The access rights mask to be updated
 * @param[in] pMaskIn The set of access rights to be added to pMaskOut
 */
#define RS_ACCESS_MASK_UNION(pMaskOut, pMaskIn) \
    do \
    { \
        NvLength limb; \
        for (limb = 0; limb < SDK_RS_ACCESS_MAX_LIMBS; limb++) \
        { \
            SDK_RS_ACCESS_LIMB_ELT(pMaskOut, limb) |= SDK_RS_ACCESS_LIMB_ELT(pMaskIn, limb); \
        } \
    } while (NV_FALSE)

/*!
 * @brief Performs an in-place subtract of one mask's rights from another
 *
 * @param[in,out] pMaskOut The access rights mask to be updated
 * @param[in] pMaskIn The set of access rights to be removed from pMaskOut
 */
#define RS_ACCESS_MASK_SUBTRACT(pMaskOut, pMaskIn) \
    do \
    { \
        NvLength limb; \
        for (limb = 0; limb < SDK_RS_ACCESS_MAX_LIMBS; limb++) \
        { \
            SDK_RS_ACCESS_LIMB_ELT(pMaskOut, limb) &= ~SDK_RS_ACCESS_LIMB_ELT(pMaskIn, limb); \
        } \
    } while (NV_FALSE)

/*!
 * @brief Removes all rights from an access rights mask
 *
 * @param[in,out] pAccessMask The access rights mask to be updated
 */
#define RS_ACCESS_MASK_CLEAR(pAccessMask) \
    do \
    { \
        portMemSet(pAccessMask, 0, sizeof(*pAccessMask)); \
    } while (NV_FALSE)

/*!
 * @brief Adds all rights to an access rights mask
 *
 * @param[in,out] pAccessMask The access rights mask to be updated
 */
#define RS_ACCESS_MASK_FILL(pAccessMask) \
    do \
    { \
        portMemSet(pAccessMask, 0xff, sizeof(*pAccessMask)); \
    } while (NV_FALSE)


/****************************************************************************/
/*                           Share definitions                              */
/****************************************************************************/

//
// The usage of Share Policy and the meaning of each share type is documented in
//   resman/docs/rmapi/resource_server/rm_capabilities.adoc
//
#define RS_SHARE_TYPE_NONE              (0U)
#define RS_SHARE_TYPE_ALL               (1U)
#define RS_SHARE_TYPE_OS_SECURITY_TOKEN (2U)
#define RS_SHARE_TYPE_CLIENT            (3U)
#define RS_SHARE_TYPE_PID               (4U)
#define RS_SHARE_TYPE_SMC_PARTITION     (5U)
#define RS_SHARE_TYPE_GPU               (6U)
#define RS_SHARE_TYPE_FM_CLIENT         (7U)
// Must be last. Update when a new SHARE_TYPE is added
#define RS_SHARE_TYPE_MAX               (8U)


//
// Use Revoke to remove an existing policy from the list.
// Allow is based on OR logic, Require is based on AND logic.
// To share a right, at least one Allow (non-Require) must match, and all Require must pass.
// If Compose is specified, policies will be added to the list. Otherwise, they will replace the list.
//
#define RS_SHARE_ACTION_FLAG_REVOKE      NVBIT(0)
#define RS_SHARE_ACTION_FLAG_REQUIRE     NVBIT(1)
#define RS_SHARE_ACTION_FLAG_COMPOSE     NVBIT(2)

/****************************************************************************/
/*                       Share flag data structures                         */
/****************************************************************************/

typedef struct RS_SHARE_POLICY {
    NvU32          target;
    RS_ACCESS_MASK accessMask;
    NvU16          type;                         ///< RS_SHARE_TYPE_
    NvU8           action;                        ///< RS_SHARE_ACTION_
} RS_SHARE_POLICY;
