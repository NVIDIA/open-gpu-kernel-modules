/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc638.finn
//

#include "ctrl/ctrlxxxx.h"
/* AMPERE_SMC_EXEC_PARTITION_REF commands and parameters */

#define NVC638_CTRL_CMD(cat,idx)             NVXXXX_CTRL_CMD(0xC638, NVC638_CTRL_##cat, idx)

/* Command categories (6bits) */
#define NVC638_CTRL_RESERVED       (0x00)
#define NVC638_CTRL_EXEC_PARTITION (0x01)

/*!
 * NVC638_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVC638_CTRL_CMD_NULL       (0xc6380000) /* finn: Evaluated from "(FINN_AMPERE_SMC_EXEC_PARTITION_REF_RESERVED_INTERFACE_ID << 8) | 0x0" */

/*!
 * NVC638_CTRL_CMD_GET_UUID
 *
 * This command returns SHA1 ASCII UUID string as well as the binary UUID for
 * the execution partition. The ASCII string format is,
 * "MIG-%16x-%08x-%08x-%08x-%024x" (the canonical format of a UUID)
 *
 * uuid[OUT]
 *      - Raw UUID bytes
 *
 * uuidStr[OUT]
 *      - ASCII UUID string
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 */

#define NVC638_UUID_LEN            16

/* 'M' 'I' 'G' '-'(x5), '\0x0', extra = 9 */
#define NVC638_UUID_STR_LEN        (0x29) /* finn: Evaluated from "((NVC638_UUID_LEN << 1) + 9)" */



#define NVC638_CTRL_CMD_GET_UUID (0xc6380101) /* finn: Evaluated from "(FINN_AMPERE_SMC_EXEC_PARTITION_REF_EXEC_PARTITION_INTERFACE_ID << 8) | NVC638_CTRL_GET_UUID_PARAMS_MESSAGE_ID" */



#define NVC638_CTRL_GET_UUID_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVC638_CTRL_GET_UUID_PARAMS {
    // C form: NvU8 uuid[NVC638_UUID_LEN];
    NvU8 uuid[NVC638_UUID_LEN];

    // C form: char uuidStr[NVC638_UUID_STR_LEN];
    char uuidStr[NVC638_UUID_STR_LEN];
} NVC638_CTRL_GET_UUID_PARAMS;//  _ctrlc638_h_
