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

#ifndef BITVECTOR_NVLINK_H
#define BITVECTOR_NVLINK_H

/* ------------------------ System Includes --------------------------------- */
/* ------------------------ Application Includes ---------------------------- */
#include "utils/nvbitvector.h"
#include "ctrl/ctrl2080/ctrl2080nvlink_common.h" // rmcontrol mask structures

/* ------------------------ Global Variables -------------------------------- */
// NVLINK_BIT_VECTOR is the global bitvector type used in nvlink code
MAKE_BITVECTOR(NVLINK_BIT_VECTOR, NV2080_CTRL_NVLINK_MAX_LINKS);

/* ------------------------ Macros and Defines ------------------------------ */
/* ------------------------ Function Prototypes ----------------------------- */

/*!
 * @brief Function takes in a NvU64 input mask and converts it to a usable BITVECTOR
 * @param[in]   inputLinkMask    a NvU64 bitmask
 * @param[out]  pLocalLinkMask   pointer to a NVLINK_BIT_VECTOR to copy the passed in bitmask to
*/
NV_STATUS
convertMaskToBitVector(NvU64 inputLinkMask, NVLINK_BIT_VECTOR *pLocalLinkMask);

/*!
 * @brief Function takes in a NVLINK_BIT_VECTOR and returns a NvU32 bitmask
 * @param[in]   pBitVector pointer to a NVLINK_BIT_VECTOR to convert
 * @param[out]  linkMask   pointer to a NvU32 which hold the converted bitmask
*/
NV_STATUS
convertBitVectorToLinkMask32(NVLINK_BIT_VECTOR *pBitVector, NvU32 *linkMask);

/*!
 * @brief Function takes in a NVLINK_BIT_VECTOR and converts it to mask structures
 * @param[in]   pBitVector           pointer to a NVLINK_BIT_VECTOR to convert
 * @param[out]  pOutputLinkMask1     pointer to a NvU64 which hold the converted bitmask
 *                                      Can be null which will skip the conversion
 * @param[in]   outputLinkMask1Size  Details the number of bits in pOutputLinkMask1
 *                                      Size must be either 32 or 64
 * @param[out]  pOutputLinkMask2     Pointer to NV2080_CTRL_NVLINK_LINK_MASK to convert to
 *                                      Can be null which will skip the conversion
*/
NV_STATUS
convertBitVectorToLinkMasks(NVLINK_BIT_VECTOR *pBitVector,
                            void *pOutputLinkMask1, NvU32 outputLinkMask1Size,
                            NV2080_CTRL_NVLINK_LINK_MASK *pOutputLinkMask2);

/*!
 * @brief Function that can take 2 masks, a primitive and NV2080_CTRL_NVLINK_LINK_MASK,
 *        and returns the OR of the 2 as a native NVLINK_BIT_VECTOR. One mask must
 *        be provided
 * @param[in]  pLinkMask2        pointer to a NV2080_CTRL_NVLINK_LINK_MASK to convert
 *                                   Can be null which will skip the conversion
 * @param[in]  pLinkMask1        pointer to a NvU64 which hold the converted bitmask
 *                                  Can be null which will skip the conversion
 * @param[in]  linkMask1Size     Details the number of bits in pOutputLinkMask1
 *                                  Size must be either 32 or 64
 * @param[out] pOutputBitVector  Pointer to NVLINK_BIT_VECTOR to store conversion
*/
NV_STATUS
convertLinkMasksToBitVector(const void *pLinkMask1, NvU32 linkMask1Size,
                            const NV2080_CTRL_NVLINK_LINK_MASK *pLinkMask2,
                            NVLINK_BIT_VECTOR *pOutputBitVector);

#endif // BITVECTOR_NVLINK_H
