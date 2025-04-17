/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "utils/nvbitvector.h"

/**
 * @brief   Returns the size, in bytes, of this bitvector.
 * @note    due to the compiler trick of storing the last index within a
 *          structure pointer in the data, the minimum size of an NV_BITEVECTOR
 *          will be the size of one pointer on a given architecture. If the
 *          storage size of the underlying data is changed to something less
 *          than the size of a pointer on a given architecture, then two
 *          libraries running on different architectures transferring bitvectors
 *          between them may disagree on the value of the direct sizeof operator
 *          on a struct of an NV_BITVECTOR derivative. This version of SizeOf
 *          should be agreeable to all architectures, and should be used instead
 *          of sizeof to marshall data between libraries running on different
 *          architectures.
 */
NvU32
bitVectorSizeOf_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
)
{
    NV_ASSERT_OR_RETURN(NULL != pBitVector, 0);

    return NV_BITVECTOR_BYTE_SIZE(bitVectorLast);
}

/**
 * @brief Clears all flags in pBitVector.
 */
NV_STATUS
bitVectorClrAll_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
)
{
    NvLength byteSize = NV_BITVECTOR_BYTE_SIZE(bitVectorLast);
    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);

    portMemSet(&pBitVector->qword, 0x0, byteSize);
    return NV_OK;
}

/**
 * @brief Clears the flag in pBitVector according to bit index idx
 */
NV_STATUS
bitVectorClr_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 idx
)
{
    NvU64 *qword;
    NvU32 qwordIdx = NV_BITVECTOR_IDX(idx);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(idx);

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(idx < bitVectorLast, NV_ERR_INVALID_ARGUMENT);

    qword = (NvU64 *)&pBitVector->qword;
    qword[qwordIdx] &= ~NVBIT64(qwordOffset);
    return NV_OK;
}

/**
 * @brief Clears all flags within a range in pBitVector
 */
NV_STATUS
bitVectorClrRange_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NV_RANGE range
)
{
    NvU64 *qword;
    NvU16 idx;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(rangeContains(rangeMake(0, bitVectorLast - 1), range),
                        NV_ERR_INVALID_ARGUMENT);

    qword = (NvU64 *)&pBitVector->qword;
    for (idx = (NvU16)range.lo; idx <= (NvU16)range.hi; ++idx)
    {
        if ((0 == NV_BITVECTOR_OFFSET(idx)) &&
            (rangeContains(range, rangeMake(idx + 63, idx + 63))))
        {
            qword[NV_BITVECTOR_IDX(idx)] = 0x0;
            idx += 63;
            continue;
        }

        status = bitVectorClr_IMPL(pBitVector, bitVectorLast, idx);
        if (NV_OK != status)
        {
            return status;
        }
    }

    return status;
}

/**
 * @brief Sets all flags in pBitVector
 */
NV_STATUS
bitVectorSetAll_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
)
{
    NvU64 *qword;
    NvLength byteSize = NV_BITVECTOR_BYTE_SIZE(bitVectorLast);
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorLast - 1);

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);

    qword = (NvU64 *)&pBitVector->qword;
    portMemSet(qword, NV_U8_MAX, byteSize);
    qword[arraySize - 1] &= (NV_U64_MAX >> (63 - qwordOffset));

    return NV_OK;
}

/**
 * @brief Sets the flag in pBitVector according to bit index idx
 */
NV_STATUS
bitVectorSet_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 idx
)
{
    NvU64 *qword;
    NvU32 qwordIdx = NV_BITVECTOR_IDX(idx);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(idx);

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);

    qword = (NvU64 *)&pBitVector->qword;
    qword[qwordIdx] |= NVBIT64(qwordOffset);

    return NV_OK;
}

/**
 * @brief Sets all flags within a range in pBitVector
 */
NV_STATUS
bitVectorSetRange_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NV_RANGE range
)
{
    NvU64 *qword;
    NvU16 idx;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(rangeContains(rangeMake(0, bitVectorLast - 1), range),
                        NV_ERR_INVALID_ARGUMENT);

    qword = (NvU64 *)&pBitVector->qword;
    for (idx = (NvU16)range.lo; idx <= (NvU16)range.hi; ++idx)
    {
        if ((0 == NV_BITVECTOR_OFFSET(idx)) &&
            (rangeContains(range, rangeMake(idx + 63, idx + 63))))
        {
            qword[NV_BITVECTOR_IDX(idx)] = (NV_U64_MAX);
            idx += 63;
            continue;
        }

        status = bitVectorSet_IMPL(pBitVector, bitVectorLast, idx);
        if (NV_OK != status)
        {
            return status;
        }
    }

    return status;
}

/**
 * @brief Toggles the flag in pBitVector according to bit index idx
 */
NV_STATUS
bitVectorInv_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 idx
)
{
    NvU64 *qword;
    NvU32 qwordIdx = NV_BITVECTOR_IDX(idx);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(idx);

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);

    qword = (NvU64 *)&pBitVector->qword;
    qword[qwordIdx] ^= NVBIT64(qwordOffset);

    return NV_OK;
}

/**
 * @brief Toggles all flags within a range in pBitVector
 */
NV_STATUS
bitVectorInvRange_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NV_RANGE range
)
{
    NvU64 *qword;
    NvU16 idx;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(rangeContains(rangeMake(0, bitVectorLast - 1), range),
                        NV_ERR_INVALID_ARGUMENT);

    qword = (NvU64 *)&pBitVector->qword;
    for (idx = (NvU16)range.lo; idx <= (NvU16)range.hi; ++idx)
    {
        if ((0 == NV_BITVECTOR_OFFSET(idx)) &&
            (rangeContains(range, rangeMake(idx + 63, idx + 63))))
        {
            qword[NV_BITVECTOR_IDX(idx)] = ~qword[NV_BITVECTOR_IDX(idx)];
            idx += 63;
            continue;
        }

        status = bitVectorInv_IMPL(pBitVector, bitVectorLast, idx);
        if (NV_OK != status)
        {
            return status;
        }
    }

    return status;
}

/**
 * @brief Initializes a NV_BITVECTOR with the bit indices contained within
 *        pIndices set.
 */
NV_STATUS
bitVectorFromArrayU16_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 *pIndices,
    NvU32 indicesSize
)
{
    NV_STATUS status = NV_OK;
    NvU32 i;

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pIndices, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(0 != indicesSize, NV_ERR_INVALID_ARGUMENT);

    status = bitVectorClrAll_IMPL(pBitVector, bitVectorLast);
    if (NV_OK != status)
    {
        return status;
    }

    for (i = 0; i < indicesSize; ++i)
    {
        status = bitVectorSet_IMPL(pBitVector, bitVectorLast, pIndices[i]);
        if (NV_OK != status)
        {
            return status;
        }
    }

    return status;
}

/**
 * @brief Checks if all flags in pBitVector are set
 */
NvBool
bitVectorTestAllSet_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
)
{
    const NvU64 *qword;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorLast - 1);
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_FALSE);

    qword = (const NvU64 *)&pBitVector->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        if (mask != (qword[idx] & mask))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

/**
 * @brief Checks if all flags in pBitVector are cleared
 */
NvBool
bitVectorTestAllCleared_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
)
{
    const NvU64 *qword;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorLast - 1);
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_FALSE);

    qword = (const NvU64 *)&pBitVector->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        if (0x0 != (qword[idx] & mask))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

/**
 * @brief Checks if two bitVectors are equivalent
 */
NvBool
bitVectorTestEqual_IMPL
(
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
)
{
    const NvU64 *qwordA;
    const NvU64 *qwordB;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorALast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorALast - 1);
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVectorA, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorB, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((bitVectorALast == bitVectorBLast), NV_ERR_INVALID_ARGUMENT);

    qwordA = (const NvU64 *)&pBitVectorA->qword;
    qwordB = (const NvU64 *)&pBitVectorB->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        if ((qwordA[idx] & mask) != (qwordB[idx] & mask))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

/**
 * @brief Checks if the set of set flags in bitVectorA is a subset of the set of
 *        set flags in bitVectorB.
 */
NvBool
bitVectorTestIsSubset_IMPL
(
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
)
{
    const NvU64 *qwordA;
    const NvU64 *qwordB;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorALast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorALast - 1);
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVectorA, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorB, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((bitVectorALast == bitVectorBLast), NV_ERR_INVALID_ARGUMENT);

    qwordA = (const NvU64 *)&pBitVectorA->qword;
    qwordB = (const NvU64 *)&pBitVectorB->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        if (((qwordA[idx] & mask) & (qwordB[idx] & mask)) != (qwordA[idx] & mask))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

/**
 * @brief Checks if the flag according to bit index idx in pBitVector is set
 */
NvBool
bitVectorTest_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NvU16 idx
)
{
    const NvU64 *qword;
    NvU32 qwordIdx = NV_BITVECTOR_IDX(idx);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(idx);

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_FALSE);
    NV_ASSERT_OR_RETURN(idx < bitVectorLast, NV_FALSE);

    qword = (const NvU64 *)&pBitVector->qword;
    return !!(qword[qwordIdx] & NVBIT64(qwordOffset));
}

/**
 * @brief Computes the intersection of flags in pBitVectorA and pBitVectorB, and
 *        stores the result in pBitVectorDst
 *
 * @param[out] pBitVectorDst Destination
 * @param[in]  pBitVectorA   First operand
 * @param[in]  pBitVectorB   Second operand
 *
 * @note it is valid for the same bitVector to be both destination and operand
 *       for this operation
 */
NV_STATUS
bitVectorAnd_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
)
{
    NvU64 *qwordDst;
    const NvU64 *qwordA;
    const NvU64 *qwordB;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorDstLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorDstLast - 1);
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVectorDst, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorA, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorB, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(((bitVectorDstLast == bitVectorALast) && (bitVectorALast ==
                        bitVectorBLast)), NV_ERR_INVALID_ARGUMENT);

    qwordDst = (NvU64 *)&pBitVectorDst->qword;
    qwordA   = (const NvU64 *)&pBitVectorA->qword;
    qwordB   = (const NvU64 *)&pBitVectorB->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        qwordDst[idx] = (qwordA[idx] & qwordB[idx]) & mask;
    }

    return NV_OK;
}

/**
 * @brief Computes the union of flags in pBitVectorA and pBitVectorB, and stores
 *        the result in pBitVectorDst
 *
 * @param[out] pBitVectorDst Destination
 * @param[in]  pBitVectorA   First operand
 * @param[in]  pBitVectorB   Second operand
 *
 * @note it is valid for the same bitVector to be both destination and operand
 *       for this operation
 */
NV_STATUS
bitVectorOr_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
)
{
    NvU64 *qwordDst;
    const NvU64 *qwordA;
    const NvU64 *qwordB;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorDstLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorDstLast - 1);
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVectorDst, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorA, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorB, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(((bitVectorDstLast == bitVectorALast) && (bitVectorALast ==
                        bitVectorBLast)), NV_ERR_INVALID_ARGUMENT);

    qwordDst = (NvU64 *)&pBitVectorDst->qword;
    qwordA   = (const NvU64 *)&pBitVectorA->qword;
    qwordB   = (const NvU64 *)&pBitVectorB->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        qwordDst[idx] = (qwordA[idx] | qwordB[idx]) & mask;
    }

    return NV_OK;
}

/**
 * @brief Computes the exclusive OR of flags in pBitVectorA and pBitVectorB, and stores
 *        the result in pBitVectorDst
 *
 * @param[out] pBitVectorDst Destination
 * @param[in]  pBitVectorA   First operand
 * @param[in]  pBitVectorB   Second operand
 *
 * @note it is valid for the same bitVector to be both destination and operand
 *       for this operation
 */
NV_STATUS
bitVectorXor_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorA,
    NvU16 bitVectorALast,
    const NV_BITVECTOR *pBitVectorB,
    NvU16 bitVectorBLast
)
{
    NvU64 *qwordDst;
    const NvU64 *qwordA;
    const NvU64 *qwordB;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorDstLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorDstLast - 1);
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVectorDst, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorA, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorB, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(((bitVectorDstLast == bitVectorALast) && (bitVectorALast ==
                        bitVectorBLast)), NV_ERR_INVALID_ARGUMENT);

    qwordDst = (NvU64 *)&pBitVectorDst->qword;
    qwordA   = (const NvU64 *)&pBitVectorA->qword;
    qwordB   = (const NvU64 *)&pBitVectorB->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        qwordDst[idx] = (qwordA[idx] ^ qwordB[idx]) & mask;
    }

    return NV_OK;
}

/**
 * @brief Causes the set of raised flags in pBitVectorDst to be equal to the
 *        complement of the set of raised flags in pBitVectorSrc.
 *
 * @param[out] pBitVectorDst Destination
 * @param[in]  pBitVectorSrc Source
 *
 * @note it is valid for the same bitVector to be both destination and
 *       source for this operation
 */
NV_STATUS
bitVectorComplement_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorSrc,
    NvU16 bitVectorSrcLast
)
{
    NvU64 *qwordDst;
    const NvU64 *qwordSrc;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorDstLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorDstLast - 1);
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVectorDst, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorSrc, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(((bitVectorDstLast == bitVectorSrcLast)), NV_ERR_INVALID_ARGUMENT);

    qwordDst = (NvU64 *)&pBitVectorDst->qword;
    qwordSrc = (const NvU64 *)&pBitVectorSrc->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        qwordDst[idx] = (~qwordSrc[idx]) & mask;
    }

    return NV_OK;
}

/**
 * @brief Causes the set of raised flags in pBitVectorDst to be equal to the set
 *        of raised flags in pBitVectorSrc.
 *
 * @param[out] pBitVectorDst Destination
 * @param[in]  pBitVectorSrc Source
 *
 * @note it is \b invalid for the same bitVector to be both destination and
 *       source for this operation
 */
NV_STATUS
bitVectorCopy_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorSrc,
    NvU16 bitVectorSrcLast
)
{
    NvLength byteSizeDst = NV_BITVECTOR_BYTE_SIZE(bitVectorDstLast);
    NvLength byteSizeSrc = NV_BITVECTOR_BYTE_SIZE(bitVectorSrcLast);

    NV_ASSERT_OR_RETURN(NULL != pBitVectorDst, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorSrc, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(bitVectorDstLast == bitVectorSrcLast, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBitVectorDst != pBitVectorSrc, NV_WARN_NOTHING_TO_DO);

    portMemCopy(&pBitVectorDst->qword, byteSizeDst, &pBitVectorSrc->qword, byteSizeSrc);
    return NV_OK;
}

/**
 * @brief Returns the bit index of the first set flag in pBitVector.
 *
 * @note in the absence of set flags in pBitVector, the index of the first
 *       invalid flag is returned.
 */
NvU32
bitVectorCountTrailingZeros_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
)
{
    const NvU64 *qword;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorLast - 1);
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVector, 0);

    qword = (const NvU64 *)&pBitVector->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        if (0x0 != (qword[idx] & mask))
        {
            return ((idx * (sizeof(NvU64) * 8)) +
                    portUtilCountTrailingZeros64(qword[idx] & mask));
        }
    }

    return bitVectorLast;
}

/**
 * @brief Returns the bit index of the last set flag in pBitVector.
 *
 * @note in the absence of set flags in pBitVector, the index of the first
 *       invalid flag is returned.
 */
NvU32
bitVectorCountLeadingZeros_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
)
{
    const NvU64 *qword;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorLast - 1);
    NvU32 qwordUnused = 63 - qwordOffset;
    NvU64 mask;

    NV_ASSERT_OR_RETURN(NULL != pBitVector, 0);

    qword = (const NvU64 *)&pBitVector->qword;
    for (idx = (arraySize - 1); idx != (NvU32)-1; idx--)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        if (0x0 != qword[idx])
        {
            //
            // We're counting from the MSB, and we have to subtract the unused
            // portion of the bitvector from the output
            //
            return (((arraySize - idx - 1) * (sizeof(NvU64) * 8)) +
                    portUtilCountLeadingZeros64(qword[idx] & mask)) -
                    qwordUnused;
        }
    }

    return bitVectorLast;
}

/**
 * @brief Returns the number of set bits in the bitvector.
 */
NvU32
bitVectorCountSetBits_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast
)
{
    const NvU64 *qword;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorLast - 1);
    NvU64 mask;
    NvU32 count;

    NV_ASSERT_OR_RETURN(NULL != pBitVector, 0);

    count = 0;
    qword = (const NvU64 *)&pBitVector->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));
        count += nvPopCount64(qword[idx] & mask);
    }

    return count;
}

/**
 * @brief Exports the bitVector data to an NvU64 raw bitmask array.
 */
NV_STATUS
bitVectorToRaw_IMPL
(
    const NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    void *pRawMask,
    NvU32 rawMaskSize
)
{
    const NvLength byteSize = NV_BITVECTOR_BYTE_SIZE(bitVectorLast);

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pRawMask, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(rawMaskSize >= byteSize, NV_ERR_BUFFER_TOO_SMALL);

    portMemCopy(pRawMask, byteSize, &pBitVector->qword, byteSize);
    return NV_OK;
}

/**
 * @brief Imports the bitVector data from an Nvu64 raw bitmask array.
 */
NV_STATUS
bitVectorFromRaw_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    const void *pRawMask,
    NvU32 rawMaskSize
)
{
    const NvLength byteSize = NV_BITVECTOR_BYTE_SIZE(bitVectorLast);

    NV_ASSERT_OR_RETURN(NULL != pBitVector, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pRawMask, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(rawMaskSize >= byteSize, NV_ERR_BUFFER_TOO_SMALL);

    portMemCopy(&pBitVector->qword, byteSize, pRawMask, byteSize);
    return NV_OK;
}


/**
 * @brief Gets slice for a range within pBitVector
 *
 * @note range length must be <=64, so the output slice can fit in a NvU64
 */
NV_STATUS
bitVectorGetSlice_IMPL
(
    NV_BITVECTOR *pBitVector,
    NvU16 bitVectorLast,
    NV_RANGE range,
    NvU64 *slice
)
{
    NvU64 *qword;
    NvU64 temp;
    NvU64 offsetLo = NV_BITVECTOR_OFFSET(range.lo);
    NvU64 offsetHi = NV_BITVECTOR_OFFSET(range.hi);
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pBitVector != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(rangeLength(range) <= 8 * sizeof(NvU64), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(range.hi != NV_U64_MAX, NV_ERR_INVALID_ARGUMENT); //detect underflow
    NV_ASSERT_OR_RETURN(rangeContains(rangeMake(0, bitVectorLast - 1), range),
                        NV_ERR_INVALID_ARGUMENT);

    qword = (NvU64 *)&pBitVector->qword;

    if(NV_BITVECTOR_IDX(range.lo) == NV_BITVECTOR_IDX(range.hi))
    {
        // range fits within a single qword index
        temp   = qword[NV_BITVECTOR_IDX(range.lo)];
        temp  &= DRF_SHIFTMASK64(offsetHi : offsetLo);
        temp >>= offsetLo;
        *slice = temp;
    }
    else
    {
        // range spreads across 2 qword indexes
        NV_ASSERT_OR_RETURN(NV_BITVECTOR_IDX(range.lo) == NV_BITVECTOR_IDX(range.hi) - 1,
                            NV_ERR_INVALID_ARGUMENT);

        temp   = qword[NV_BITVECTOR_IDX(range.lo)];
        temp  &= DRF_SHIFTMASK64(63 : offsetLo);
        temp >>= offsetLo;
        *slice = temp;

        temp   = qword[NV_BITVECTOR_IDX(range.hi)];
        temp  &= DRF_SHIFTMASK64(offsetHi : 0);
        temp <<= 64 - offsetLo;
        *slice |= temp;
    }

    return status;
}

/**
 * @brief Causes the least significant N raised bits in pBitVectorSrc to be
 *        raised in pBitVectorDst.
 *
 * @param[out] pBitVectorDst Destination
 * @param[in]  pBitVectorSrc Source
 * @param[in]  n             Count of bits to copy
 *
 * @note it is invalid for the same bitvector to be both dest and source
 * @note n cannot be larger than the size of the bitvector
 */
NV_STATUS
bitVectorLowestNBits_IMPL
(
    NV_BITVECTOR *pBitVectorDst,
    NvU16 bitVectorDstLast,
    const NV_BITVECTOR *pBitVectorSrc,
    NvU16 bitVectorSrcLast,
    NvU16 n
)
{
    NvU64 *qwordDst;
    const NvU64 *qwordSrc;
    NvU32 idx;
    NvU32 arraySize = NV_BITVECTOR_ARRAY_SIZE(bitVectorSrcLast);
    NvU32 qwordOffset = NV_BITVECTOR_OFFSET(bitVectorSrcLast - 1);
    NvU64 mask;
    NvU16 count;

    NV_ASSERT_OR_RETURN(NULL != pBitVectorSrc, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(NULL != pBitVectorDst, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((bitVectorSrcLast == bitVectorDstLast), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBitVectorDst != pBitVectorSrc, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(n < bitVectorSrcLast, NV_ERR_INVALID_ARGUMENT);

    if (n == bitVectorSrcLast)
        return bitVectorCopy_IMPL(pBitVectorDst, bitVectorDstLast, pBitVectorSrc, bitVectorSrcLast);

    bitVectorClrAll_IMPL(pBitVectorDst, bitVectorDstLast);

    if (n == 0)
        return NV_OK;

    count = 0;
    qwordSrc = (const NvU64 *)&pBitVectorSrc->qword;
    qwordDst = (NvU64 *)&pBitVectorDst->qword;
    for (idx = 0; idx < arraySize; idx++)
    {
        NvU64 bit;

        mask = (idx < arraySize - 1) ? NV_U64_MAX :
               (NV_U64_MAX >> (63 - qwordOffset));

        FOR_EACH_INDEX_IN_MASK(64, bit, qwordSrc[idx] & mask)
        {
            qwordDst[idx] |= NVBIT64(bit);

            count++;
            if (count == n)
                return NV_OK;
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }

    return NV_OK;
}

