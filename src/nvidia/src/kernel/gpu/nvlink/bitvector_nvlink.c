/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED
#include "kernel/gpu/nvlink/bitvector_nvlink.h"

/*!
 * @copydoc convertMaskToBitVector
 */
NV_STATUS
convertMaskToBitVector(NvU64 inputLinkMask, NVLINK_BIT_VECTOR *pLocalLinkMask)
{
    NV_STATUS status;

    if (pLocalLinkMask == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = bitVectorClrAll(pLocalLinkMask);
    if (status != NV_OK)
    {
        return status;
    }

    status = bitVectorFromRaw(pLocalLinkMask, &inputLinkMask, sizeof(inputLinkMask));
    if (status != NV_OK)
    {
        return status;
    }

    return NV_OK;
}

/*!
 * @copydoc convertBitVectorToLinkMask32
 */
NV_STATUS
convertBitVectorToLinkMask32(NVLINK_BIT_VECTOR *pBitVector, NvU32 *linkMask)
{
    NV_STATUS status;
    NvU64 tmpLinkMask = 0;

    status = bitVectorToRaw(pBitVector, &tmpLinkMask, sizeof(tmpLinkMask));
    if (status != NV_OK)
    {
        return status;
    }

    *linkMask = (NvU32) tmpLinkMask;

    return NV_OK;
}

/*!
 * @copydoc convertBitVectorToLinkMasks
 */
NV_STATUS
convertBitVectorToLinkMasks(NVLINK_BIT_VECTOR *pLocalLinkMask,
                            void *pOutputLinkMask1, NvU32 outputLinkMask1Size,
                            NV2080_CTRL_NVLINK_LINK_MASK *pOutputLinkMask2)
{
    NV_STATUS status;

    if (pLocalLinkMask == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pOutputLinkMask1 != NULL)
    {
        if (outputLinkMask1Size == sizeof(NvU32))
        {
            status = convertBitVectorToLinkMask32(pLocalLinkMask, pOutputLinkMask1);
            if (status != NV_OK)
            {
                return status;
            }
        }
        else if (outputLinkMask1Size == sizeof(NvU64))
        {
            status = bitVectorToRaw(pLocalLinkMask, pOutputLinkMask1, outputLinkMask1Size);
            if (status != NV_OK)
            {
                return status;
            }
        }
        else
        {
            return NV_ERR_INVALID_ARGUMENT;
        }
    }

    if (pOutputLinkMask2 != NULL)
    {
        status = bitVectorToRaw(pLocalLinkMask, pOutputLinkMask2->masks, sizeof(pOutputLinkMask2->masks));
        if (status != NV_OK)
        {
            return status;
        }

        pOutputLinkMask2->lenMasks = sizeof(pOutputLinkMask2->masks)/sizeof(NvU64);
    }

    return NV_OK;
}

/*!
 * @copydoc convertLinkMasksToBitVector
 */
NV_STATUS
convertLinkMasksToBitVector(const void *pLinkMask1, NvU32 linkMask1Size,
                            const NV2080_CTRL_NVLINK_LINK_MASK *pLinkMask2,
                            NVLINK_BIT_VECTOR *pOutputBitVector)
{
    NV_STATUS status;
    NVLINK_BIT_VECTOR localLinkMask1;
    NVLINK_BIT_VECTOR localLinkMask2;

    // Return invalid args if output NVLINK_BIT_VECTOR is not specified
    if ((pOutputBitVector == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = bitVectorClrAll(pOutputBitVector);
    if (status != NV_OK)
    {
        return status;
    }

    status = bitVectorClrAll(&localLinkMask1);
    if (status != NV_OK)
    {
        return status;
    }

    status = bitVectorClrAll(&localLinkMask2);
    if (status != NV_OK)
    {
        return status;
    }

    // If pLinkMask1 is provided, convert it to bitvector
    if (pLinkMask1 != NULL)
    {
        // Validate size of the mask to be NvU32 or NvU64
        NV_CHECK_OR_RETURN(LEVEL_ERROR, (linkMask1Size == sizeof(NvU32) || linkMask1Size == sizeof(NvU64)), NV_ERR_INVALID_ARGUMENT);
        NvU64 tmpLinkMask = 0;
        portMemCopy(&tmpLinkMask, sizeof(tmpLinkMask), pLinkMask1, linkMask1Size);
        status = bitVectorFromRaw(&localLinkMask1, &tmpLinkMask, sizeof(tmpLinkMask));
        if (status != NV_OK)
        {
            return status;
        }
    }

    // If pLinkMask2 is provided, convert it to bitvector
    if (pLinkMask2 != NULL)
    {
        status = bitVectorFromRaw(&localLinkMask2, &pLinkMask2->masks, sizeof(pLinkMask2->masks));
        if (status != NV_OK)
        {
            return status;
        }
    }

    // Return the OR of the 2 converted bitvectors from pLinkMask1 and pLinkMask2
    return bitVectorOr(pOutputBitVector, &localLinkMask1, &localLinkMask2);
}
