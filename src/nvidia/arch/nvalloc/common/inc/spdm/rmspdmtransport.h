/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _RM_SPDM_TRANSPORT_H_
#define _RM_SPDM_TRANSPORT_H_

/* ------------------------- Macros and Defines ----------------------------- */
// TODO CONFCOMP-1277: All these defines should be reviewed and simplified or removed.
//
// The following defines and macros are shared for any message-related constants
// shared between RM and endpoint.
//
#pragma pack(1)

#define NV_SPDM_MESSAGE_TYPE_NORMAL             (0)
#define NV_SPDM_MESSAGE_TYPE_SECURED            (1)

// SPDM Command Types
#define CC_CTRL_CODE_UNDEFINED                       (0)
#define CC_CTRL_CODE_SPDM_MESSAGE_PROCESS            (1)
#define CC_CTRL_CODE_SESSION_MESSAGE_PROCESS         (2)
#define CC_CTRL_CODE_APPLICATION_MESSAGE_PROCESS     (3)

#define CC_SPDM_ENDPOINT_ID_INVALID (0xFFFFFFFF)
#define CC_SPDM_GUEST_ID_INVALID    (0xFFFFFFFF)

#define NV_SPDM_DESC_HEADER_SIZE_IN_BYTE        (NvU32)sizeof(NV_SPDM_DESC_HEADER)
#define NV_SPDM_DESC_HEADER_VERSION_1_0         (0x10)
#define NV_SPDM_DESC_HEADER_VERSION_CURRENT     NV_SPDM_DESC_HEADER_VERSION_1_0
#define NV_SPDM_DESC_HEADER_ALIGNMENT           (256)
#define NV_SPDM_RM_SURFACE_SIZE_IN_BYTE         (0x2000)
#define NV_RM_BUFFER_SIZE_IN_BYTE               (NV_SPDM_RM_SURFACE_SIZE_IN_BYTE - NV_SPDM_DESC_HEADER_SIZE_IN_BYTE)
#define NV_SPDM_UNPROTECTED_REGION_ID           (0x0U)

typedef struct _NV_SPDM_DESC_HEADER
{
    NvU32 msgType;
    NvU32 msgSizeByte;
} NV_SPDM_DESC_HEADER, *PNV_SPDM_DESC_HEADER;

#pragma pack()

#endif // _RM_SPDM_TRANSPORT_H_
