/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl90e6.finn
//

#include "ctrl/ctrlxxxx.h"
#define NV90E6_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0x90E6, NV90E6_CTRL_##cat, idx)


/* NV90E6 command categories (6bits) */
#define NV90E6_CTRL_RESERVED (0x00)
#define NV90E6_CTRL_MASTER   (0x01)


/*
 * NV90E6_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NV90E6_CTRL_CMD_NULL (0x90e60000) /* finn: Evaluated from "(FINN_GF100_SUBDEVICE_MASTER_RESERVED_INTERFACE_ID << 8) | 0x0" */





/*
 * NV90E6_CTRL_CMD_MASTER_GET_ERROR_INTR_OFFSET_MASK
 *
 * This command is used to query the offset and mask within the object mapping
 * that can be used to query for ECC and NVLINK interrupts.
 *
 * If a read of the given offset+mask is non-zero then it is possible an ECC or
 * an NVLINK error has been reported and not yet handled. If this is true then
 * the caller must either wait until the read returns zero or call into the
 * corresponding count reporting APIs to get updated counts.
 *
 * offset
 *   The offset into a GF100_SUBDEVICE_MASTSER's mapping where the top level
 *   interrupt register can be found.
 * mask
 *   Compatibility field that contains the same bits as eccMask. This field is
 *   deprecated and will be removed.
 * eccMask
 *   The mask to AND with the value found at offset to determine if any ECC
 *   interrupts are pending.
 * nvlinkMask
 *   The mask to AND with the value found at offset to determine if any NVLINK
 *   interrupts are pending.
 *
 *   Possible return status values are
 *     NV_OK
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */
#define NV90E6_CTRL_CMD_MASTER_GET_ERROR_INTR_OFFSET_MASK (0x90e60101) /* finn: Evaluated from "(FINN_GF100_SUBDEVICE_MASTER_MASTER_INTERFACE_ID << 8) | NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS_MESSAGE_ID" */

#define NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS {
    NvU32 offset;
    NvU32 mask;     // TODO: remove after all users have switched to use eccMask
    NvU32 eccMask;
    NvU32 nvlinkMask;
} NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS;

// TODO: remove once users of this interface have switched to the new name.
#define NV90E6_CTRL_CMD_MASTER_GET_ECC_INTR_OFFSET_MASK NV90E6_CTRL_CMD_MASTER_GET_ERROR_INTR_OFFSET_MASK

typedef NV90E6_CTRL_MASTER_GET_ERROR_INTR_OFFSET_MASK_PARAMS NV90E6_CTRL_MASTER_GET_ECC_INTR_OFFSET_MASK_PARAMS;

/*
 * NV90E6_CTRL_CMD_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK
 *
 * This command is used to query the mask within the fastpath register
 * (VIRTUAL_FUNCTION_ERR_CONT) that can be used to query for ECC and NVLINK interrupts.
 *
 * If a read of the given mask is non-zero then it is possible an ECC or
 * an NVLINK error has been reported and not yet handled. If this is true then
 * the caller must either wait until the read returns zero or call into the
 * corresponding count reporting APIs to get updated counts.
 *
 * [out] eccMask
 *   The mask to AND with the value found at offset to determine if any ECC
 *   interrupts are possibly pending.
 * [out] nvlinkMask
 *   The mask to AND with the value found at offset to determine if any NVLINK
 *   interrupts are possibly pending.
 *
 *   Possible return status values are
 *     NV_OK
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */
#define NV90E6_CTRL_CMD_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK (0x90e60102) /* finn: Evaluated from "(FINN_GF100_SUBDEVICE_MASTER_MASTER_INTERFACE_ID << 8) | NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS_MESSAGE_ID" */

#define NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS {
    NvU32 eccMask;
    NvU32 nvlinkMask;
} NV90E6_CTRL_MASTER_GET_VIRTUAL_FUNCTION_ERROR_CONT_INTR_MASK_PARAMS;

/* _ctrl90e6_h_ */

