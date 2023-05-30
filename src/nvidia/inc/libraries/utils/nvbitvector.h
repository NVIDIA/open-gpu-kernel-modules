/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _NV_UTILS_NV_BITVECTOR_H_
#define _NV_UTILS_NV_BITVECTOR_H_

#include "nvport/nvport.h"
#include "nvtypes.h"
#include "nvstatus.h"
#include "nvmisc.h"
#include "utils/nvassert.h"
#include "utils/nvrange.h"

#ifdef __cplusplus
extern "C" {
#endif

//
// Note: This will need to be recalculated if the data size changes
// IDX(i) = (index & ~(MASK(num bits)) >> log2(num bits)
//
#define NV_BITVECTOR_IDX(index)         (((index) & ~(0x3F)) >> 6)
#define NV_BITVECTOR_ARRAY_SIZE(last)   (NV_BITVECTOR_IDX((last) - 1) + 1)
#define NV_BITVECTOR_BYTE_SIZE(last)    (NV_BITVECTOR_ARRAY_SIZE((last)) * sizeof(NvU64))
#define NV_BITVECTOR_OFFSET(index)      ((index) & ((sizeof(NvU64) * 8) - 1))

/**
 * \anchor NV_BITVECTOR_1
 * @defgroup NV_BITVECTOR NV_BITVECTOR
 *
 * @brief NV_BITVECTOR is a collection of individual consecutive bit flags
 *        packed within an array of 64-bit integers. Each derivative of the
 *        NV_BITVECTOR type may specify the number of queryable flags, and the
 *        array will be sized according to the minimum number of 64-bit integers
 *        required to hold the flags.
 *
 * @details NV_BITVECTOR is a general purpose data structure utility.
 *          It consists of a single (real) field, named \b qword.
 *          Flags within a NV_BITVECTOR are represented beginning with the LSB of
 *          index 0 of \b qword, and are packed fully within a single qword
 *          before expanding into a new qword. Derivatives of NV_BITVECTOR must
 *          provide a type name for the new type, and the first index outside of
 *          the range of the new type (this value must be greater than 0.) A
 *          bitvector with bits 63 and 64 raised is represented in memory in a
 *          little-endian system as follows:
 *
 *             63                     NV_BITVECTOR_OFFSET(i)                 0
 *            .-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-.
 *          0 |1                                                              |
 *          1 |                                                              1|
 *            `-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-'
 *
 *          Thus, in order to conceptually model an NV_BITVECTOR horizontally as
 *          a continual ordered list of bits, one would have to write the
 *          bitvector from highest index to lowest, and read from right to left.
 *
 * @note The unused bits within a derivative type of NV_BITVECTOR are reserved,
 *       and must not be depended upon to contain any consistent value.
 *
 * @{
 */
typedef struct NV_BITVECTOR NV_BITVECTOR;
struct NV_BITVECTOR
{
    NvU64 qword;
};

#define TYPEDEF_BITVECTOR(bitvectTypeName)                  \
    union bitvectTypeName;                                  \
    typedef union bitvectTypeName bitvectTypeName;          \

#define IMPL_BITVECTOR(bitvectTypeName, last_val)           \
    union bitvectTypeName                                   \
    {                                                       \
        NV_BITVECTOR real;                                  \
        NvU64 qword[NV_BITVECTOR_ARRAY_SIZE(last_val)];     \
        struct                                              \
        {                                                   \
            char _[last_val];                               \
            char asrt[1 - 2 * !(last_val > 0)];             \
        } *last;                                            \
    }

#define MAKE_BITVECTOR(bitvectTypeName, last_val)           \
    TYPEDEF_BITVECTOR(bitvectTypeName)                      \
    IMPL_BITVECTOR(bitvectTypeName, last_val)

#define MAKE_ANON_BITVECTOR(last_val)                       \
    IMPL_BITVECTOR( , last_val)

#define bitVectorSizeOf(pBitVector)                                         \
    bitVectorSizeOf_IMPL(&((pBitVector)->real),                             \
                         sizeof(((pBitVector)->last->_)))

#define bitVectorClrAll(pBitVector)                                         \
    bitVectorClrAll_IMPL(&((pBitVector)->real),                             \
                         sizeof(((pBitVector)->last->_)))

#define bitVectorClr(pBitVector, idx)                                       \
    bitVectorClr_IMPL(&((pBitVector)->real),                                \
                      sizeof(((pBitVector)->last->_)), (idx))

#define bitVectorClrRange(pBitVector, range)                                \
    bitVectorClrRange_IMPL(&((pBitVector)->real),                           \
                      sizeof(((pBitVector)->last->_)), (range))

#define bitVectorSetAll(pBitVector)                                         \
    bitVectorSetAll_IMPL(&((pBitVector)->real),                             \
                         sizeof(((pBitVector)->last->_)))

#define bitVectorSet(pBitVector, idx)                                       \
    bitVectorSet_IMPL(&((pBitVector)->real),                                \
                      sizeof(((pBitVector)->last->_)), (idx))

#define bitVectorSetRange(pBitVector, range)                                \
    bitVectorSetRange_IMPL(&((pBitVector)->real),                           \
                      sizeof(((pBitVector)->last->_)), (range))

#define bitVectorFromArrayU16(pBitVector, pArr, sz)                         \
    bitVectorFromArrayU16_IMPL(&((pBitVector)->real),                       \
                            sizeof(((pBitVector)->last->_)),                \
                            (pArr),                                         \
                            (sz))

#define bitVectorTestAllSet(pBitVector)                                     \
     bitVectorTestAllSet_IMPL(&((pBitVector)->real),                        \
                              sizeof(((pBitVector)->last->_)))

#define bitVectorTestAllCleared(pBitVector)                                 \
    bitVectorTestAllCleared_IMPL(&((pBitVector)->real),                     \
                                 sizeof(((pBitVector)->last->_)))

#define bitVectorTestEqual(pBitVectorA, pBitVectorB)                        \
    bitVectorTestEqual_IMPL(&((pBitVectorA)->real),                         \
                            sizeof(((pBitVectorA)->last->_)),               \
                            &((pBitVectorB)->real),                         \
                            sizeof(((pBitVectorB)->last->_)))

#define bitVectorTestIsSubset(pBitVectorA, pBitVectorB)                     \
    bitVectorTestIsSubset_IMPL(&((pBitVectorA)->real),                      \
                               sizeof(((pBitVectorA)->last->_)),            \
                               &((pBitVectorB)->real),                      \
                               sizeof(((pBitVectorB)->last->_)))

#define bitVectorTest(pBitVector, idx)                                      \
    bitVectorTest_IMPL(&((pBitVector)->real),                               \
                       sizeof(((pBitVector)->last->_)),                     \
                       (idx))

#define bitVectorAnd(pBitVectorDst, pBitVectorA, pBitVectorB)               \
    bitVectorAnd_IMPL(&((pBitVectorDst)->real),                             \
                      sizeof(((pBitVectorDst)->last->_)),                   \
                      &((pBitVectorA)->real),                               \
                      sizeof(((pBitVectorA)->last->_)),                     \
                      &((pBitVectorB)->real),                               \
                      sizeof(((pBitVectorB)->last->_)))

#define bitVectorOr(pBitVectorDst, pBitVectorA, pBitVectorB)                \
    bitVectorOr_IMPL(&((pBitVectorDst)->real),                              \
                     sizeof(((pBitVectorDst)->last->_)),                    \
                     &((pBitVectorA)->real),                                \
                     sizeof(((pBitVectorA)->last->_)),                      \
                     &((pBitVectorB)->real),                                \
                     sizeof(((pBitVectorB)->last->_)))

#define bitVectorXor(pBitVectorDst, pBitVectorA, pBitVectorB)               \
    bitVectorXor_IMPL(&((pBitVectorDst)->real),                             \
                      sizeof(((pBitVectorDst)->last->_)),                   \
                      &((pBitVectorA)->real),                               \
                      sizeof(((pBitVectorA)->last->_)),                     \
                      &((pBitVectorB)->real),                               \
                      sizeof(((pBitVectorB)->last->_)))

#define bitVectorComplement(pBitVectorDst, pBitVectorSrc)                   \
    bitVectorComplement_IMPL(&((pBitVectorDst)->real),                      \
                             sizeof(((pBitVectorDst)->last->_)),            \
                             &((pBitVectorSrc)->real),                      \
                             sizeof(((pBitVectorSrc)->last->_)))

#define bitVectorCopy(pBitVectorDst, pBitVectorSrc)                         \
    bitVectorCopy_IMPL(&((pBitVectorDst)->real),                            \
                       sizeof(((pBitVectorDst)->last->_)),                  \
                       &((pBitVectorSrc)->real),                            \
                       sizeof(((pBitVectorSrc)->last->_)))

#define bitVectorCountTrailingZeros(pBitVector)                             \
    bitVectorCountTrailingZeros_IMPL(&((pBitVector)->real),                 \
                                     sizeof(((pBitVector)->last->_)))

#define bitVectorCountLeadingZeros(pBitVector)                              \
    bitVectorCountLeadingZeros_IMPL(&((pBitVector)->real),                  \
                                    sizeof(((pBitVector)->last->_)))

#define bitVectorCountSetBits(pBitVector)                                   \
    bitVectorCountSetBits_IMPL(&((pBitVector)->real),                       \
                               sizeof(((pBitVector)->last->_)))

#define bitVectorToRaw(pBitVector, pRawMask, rawMaskSize)                   \
    bitVectorToRaw_IMPL(&((pBitVector)->real),                              \
                        sizeof(((pBitVector)->last->_)),                    \
                        pRawMask,                                           \
                        rawMaskSize)

#define bitVectorFromRaw(pBitVector, pRawMask, rawMaskSize)                 \
    bitVectorFromRaw_IMPL(&((pBitVector)->real),                            \
                          sizeof(((pBitVector)->last->_)),                  \
                          pRawMask,                                         \
                          rawMaskSize)

#define FOR_EACH_IN_BITVECTOR(pBitVector, index)                            \
    {                                                                       \
        MAKE_ANON_BITVECTOR(sizeof(((pBitVector)->last->_))) localMask;     \
        bitVectorCopy(&localMask, (pBitVector));                            \
        for ((index) = bitVectorCountTrailingZeros(&localMask);             \
             !bitVectorTestAllCleared(&localMask);                          \
             bitVectorClr(&localMask, (index)),                             \
             (index) = bitVectorCountTrailingZeros(&localMask))             \
        {

#define FOR_EACH_IN_BITVECTOR_END()                                         \
        }                                                                   \
    }

#define FOR_EACH_IN_BITVECTOR_PAIR(pBitVectorA, indexA, pBitVectorB, indexB) \
    {                                                                        \
        MAKE_ANON_BITVECTOR(sizeof(((pBitVectorA)->last->_))) localMaskA;    \
        bitVectorCopy(&localMaskA, (pBitVectorA));                           \
        MAKE_ANON_BITVECTOR(sizeof(((pBitVectorB)->last->_))) localMaskB;    \
        bitVectorCopy(&localMaskB, (pBitVectorB));                           \
        for ((indexA) = bitVectorCountTrailingZeros(&localMaskA),            \
             (indexB) = bitVectorCountTrailingZeros(&localMaskB);            \
             !bitVectorTestAllCleared(&localMaskA) &&                        \
             !bitVectorTestAllCleared(&localMaskB);                          \
             bitVectorClr(&localMaskA, (indexA)),                            \
             bitVectorClr(&localMaskB, (indexB)),                            \
             (indexA) = bitVectorCountTrailingZeros(&localMaskA),            \
             (indexB) = bitVectorCountTrailingZeros(&localMaskB))            \
        {

#define FOR_EACH_IN_BITVECTOR_PAIR_END()                                    \
        }                                                                   \
    }

NvU32
bitVectorSizeOf_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
);

NV_STATUS
bitVectorClrAll_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
);

NV_STATUS
bitVectorClr_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 idx
);

NV_STATUS
bitVectorClrRange_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NV_RANGE range
);

NV_STATUS
bitVectorSetAll_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
);

NV_STATUS
bitVectorSet_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 idx
);

NV_STATUS
bitVectorSetRange_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NV_RANGE range
);

NV_STATUS
bitVectorInv_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 idx
);

NV_STATUS
bitVectorInvRange_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NV_RANGE range
);

NV_STATUS
bitVectorFromArrayU16_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 *pIndices,
    NvU32 indicesSize
);

NvBool
bitVectorTestAllSet_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
);

NvBool
bitVectorTestAllCleared_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
);

NvBool
bitVectorTestEqual_IMPL
(
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
);

NvBool
bitVectorTestIsSubset_IMPL
(
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
);

NvBool
bitVectorTest_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 idx
);

NV_STATUS
bitVectorAnd_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
);

NV_STATUS
bitVectorOr_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
);

NV_STATUS
bitVectorXor_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
);

NV_STATUS
bitVectorComplement_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorSrc,
    NvU16 bitVectorSrcLast
);

NV_STATUS
bitVectorCopy_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorSrc,
    NvU16 bitVectorSrcLast
);

NvU32
bitVectorCountTrailingZeros_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
);

NvU32
bitVectorCountLeadingZeros_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
);

NvU32
bitVectorCountSetBits_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
);

NV_STATUS
bitVectorToRaw_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    void *pRawMask,
    NvU32 rawMaskize
);

NV_STATUS
bitVectorFromRaw_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    const void *pRawMask,
    NvU32 rawMaskSize
);

#ifdef __cplusplus
}
#endif
///@}
///  NV_UTILS_BITVECTOR
#endif
