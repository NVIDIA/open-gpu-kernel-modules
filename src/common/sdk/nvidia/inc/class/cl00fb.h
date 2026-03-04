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
// Source file:      class/cl00fb.finn
//

#include "cl00e0.h"

/*
 * Class definition for referencing the imported fabric memory,
 * NV_MEMORY_FABRIC_IMPORT_V2, using the export object UUID.
 *
 * A privileged fabric manager only class, parented by client.
 *
 * No fabric events will be generated during creation or destruction of
 * this class.
 */

#define NV_MEMORY_FABRIC_IMPORTED_REF (0xfbU) /* finn: Evaluated from "NV00FB_ALLOCATION_PARAMETERS_MESSAGE_ID" */

/*
 *  exportUuid [IN]
 *    Universally unique identifier of the export object. This is extracted
 *    from a fabric packet.
 *
 *  index [IN]
 *    Index of the export object to which the memory object is attached.
 *
 *  flags [IN]
 *     Currently unused. Must be zero for now.
 *
 *  id [IN]
 *    Import ID. An import request is uniquely identified using,
 *    <packet.uuid + index + ID>.
 */

#define NV00FB_ALLOCATION_PARAMETERS_MESSAGE_ID (0x00fbU)

typedef struct NV00FB_ALLOCATION_PARAMETERS {
    NvU8  exportUuid[NV_MEM_EXPORT_UUID_LEN];
    NvU16 index;
    NvU32 flags;
    NV_DECLARE_ALIGNED(NvU64 id, 8);
} NV00FB_ALLOCATION_PARAMETERS;

