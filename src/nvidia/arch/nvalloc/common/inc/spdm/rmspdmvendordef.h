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

#ifndef _RMSPDMVENDORDEF_H_
#define _RMSPDMVENDORDEF_H_
#include <nvtypes.h>

/*!
 * Defines structures and interfaces for any SPDM Vendor-defined behavior.
 */

/* ------------------------- NVIDIA Export Secrets -------------------------- */


/* ------------------------- Macros ----------------------------------------- */
#define SPDM_MAX_MEASUREMENT_BLOCK_COUNT     (64)
#define SPDM_CERT_DEFAULT_SLOT_ID            (0)
#define SPDM_CAPABILITIES_CT_EXPONENT_MAX    (0x1F)
#define SPDM_DEFAULT_HEARTBEAT_PERIOD_IN_SEC (4)

#define NV_BYTE_TO_BIT_OVERFLOW_MASK_UINT32 (0xE0000000)
#define IS_BYTE_TO_BIT_OVERFLOW_UINT32(a) \
        ((a & NV_BYTE_TO_BIT_OVERFLOW_MASK_UINT32) != 0)

/* ---------------------- Public defines ------------------------------------- */
#define NV_KEY_UPDATE_TRIGGER_THRESHOLD                 (0x10000000)

#define NV_KEY_UPDATE_TRIGGER_ID_INVALID                (0)
#define NV_KEY_UPDATE_TRIGGER_ID_GET_MEASUREMENTS       (1)
#define NV_KEY_UPDATE_TRIGGER_ID_HEARTBEAT              (2)
#define NV_KEY_UPDATE_TRIGGER_ID_LAST                   (3)

#define NV_SPDM_ENCAP_CERT_SIZE_IN_BYTE                 (578)

#endif // _RMSPDMVENDORDEF_H_
