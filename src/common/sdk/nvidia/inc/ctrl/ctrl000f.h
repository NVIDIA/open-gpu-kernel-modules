/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrl000f.finn
//

#include "ctrl/ctrlxxxx.h"

#define NV000F_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x000f, NV000F_CTRL_##cat, idx)

/* Client command categories (6bits) */
#define NV000F_CTRL_RESERVED  (0x00U)
#define NV000F_CTRL_FM        (0x01U)
#define NV000F_CTRL_RESERVED2 (0x02U)

/*
 * NV000f_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV000F_CTRL_CMD_NULL  (0xf0000U) /* finn: Evaluated from "(FINN_FABRIC_MANAGER_SESSION_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV000F_CTRL_CMD_SET_FM_STATE
 *
 * This command will notify RM that the fabric manager is initialized.
 *
 * RM would block P2P operations such as P2P capability reporting, NV50_P2P object
 * allocation etc. until the notification received.
 *
 * Possible status values returned are:
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_OBJECT_NOT_FOUND
 *    NV_ERR_NOT_SUPPORTED
 *    NV_OK
 */
#define NV000F_CTRL_CMD_SET_FM_STATE   (0xf0101U) /* finn: Evaluated from "(FINN_FABRIC_MANAGER_SESSION_FM_INTERFACE_ID << 8) | 0x1" */

/*
 * NV000F_CTRL_CMD_CLEAR_FM_STATE
 *
 * This command will notify RM that the fabric manager is uninitialized.
 *
 * RM would block P2P operations such as P2P capability reporting, NV50_P2P object
 * allocation etc. as soon as the notification received.
 *
 * Possible status values returned are:
 *    NV_ERR_INVALID_ARGUMENT
 *    NV_ERR_OBJECT_NOT_FOUND
 *    NV_ERR_NOT_SUPPORTED
 *    NV_OK
 */
#define NV000F_CTRL_CMD_CLEAR_FM_STATE (0xf0102U) /* finn: Evaluated from "(FINN_FABRIC_MANAGER_SESSION_FM_INTERFACE_ID << 8) | 0x2" */

/* _ctrl000f.h_ */
