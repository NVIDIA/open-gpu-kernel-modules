/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2014 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl208f/ctrl208fdma.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

/*
 * NV208F_CTRL_CMD_DMA_IS_SUPPORTED_SPARSE_VIRTUAL
 *
 * This command checks whether or not "sparse" virtual address ranges are
 * supported for a given chip. This API is intended for debug-use only.
 *
 *   bIsSupported
 *     Whether or not "sparse" virtual address ranges are supported.
 */
#define NV208F_CTRL_CMD_DMA_IS_SUPPORTED_SPARSE_VIRTUAL (0x208f1401) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_DMA_INTERFACE_ID << 8) | NV208F_CTRL_DMA_IS_SUPPORTED_SPARSE_VIRTUAL_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_DMA_IS_SUPPORTED_SPARSE_VIRTUAL_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV208F_CTRL_DMA_IS_SUPPORTED_SPARSE_VIRTUAL_PARAMS {
    NvBool bIsSupported;
} NV208F_CTRL_DMA_IS_SUPPORTED_SPARSE_VIRTUAL_PARAMS;

/*
 * NV208F_CTRL_CMD_DMA_GET_VAS_BLOCK_DETAILS
 *
 * This command retrieves various details of the virtual address space block
 * allocated from the virtual address space heap for the given virtual address.
 *
 *   virtualAddress
 *     Virtual address to get information about.
 *
 *   beginAddress
 *     Start address of the corresponding virtual address space block.
 *
 *   endAddress
 *     End address (inclusive) of the corresponding virtual address space
 *     block.
 *
 *   alignedAddress
 *     Aligned address of the corresponding virtual address space block.
 *
 *   pageSize
 *     Page size of the virtual address space block.
 *
 *   hVASpace
 *     Handle to an allocated VA space. If 0, it is assumed that the device's
 *     VA space should be used.
 */
#define NV208F_CTRL_CMD_DMA_GET_VAS_BLOCK_DETAILS (0x208f1402) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_DMA_INTERFACE_ID << 8) | NV208F_CTRL_DMA_GET_VAS_BLOCK_DETAILS_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_DMA_GET_VAS_BLOCK_DETAILS_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV208F_CTRL_DMA_GET_VAS_BLOCK_DETAILS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 virtualAddress, 8);
    NV_DECLARE_ALIGNED(NvU64 beginAddress, 8);
    NV_DECLARE_ALIGNED(NvU64 endAddress, 8);
    NV_DECLARE_ALIGNED(NvU64 alignedAddress, 8);
    NvU32    pageSize;
    NvHandle hVASpace;
} NV208F_CTRL_DMA_GET_VAS_BLOCK_DETAILS_PARAMS;

/* _ctrl208fdma_h_ */

