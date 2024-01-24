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
// Source file:      class/cl00f9.finn
//

#include "cl00e0.h"

/*
 * Class definition for importing memory handles from a different RM client on
 * the same or another node (OS).
 */

#define NV_MEMORY_FABRIC_IMPORT_V2 (0xf9U) /* finn: Evaluated from "NV00F9_ALLOCATION_PARAMETERS_MESSAGE_ID" */

/*
 *  imexChannel [IN]
 *    IMEX channel to be used for import. (Currently ignored, driver picks the
 *    appropriate channel)
 *
 *  packet [DEPRECATED, use expPacket instead]
 *    Fabric packet which uniquely identifies this object universally.
 *
 *  expPacket [IN]
 *    Export object (see cl00e0.h) identifier from which memory will be imported.
 *
 *  index [IN]
 *    Index into the FLA handle array associated with the export UUID.
 *
 *  flags [IN]
 *    For future use.
 *    Set to zero for default behavior.
 *
 *  pOsEvent [IN]
 *    Optional OS event handle created with NvRmAllocOsEvent().
 *
 *  id [IN]
 *    A 48-bit ID for this import call. The caller has freedom to uniqueness
 *    of the ID. RM would track the object uniquely using
 *    <packet.uuid + index + ID>.
 */

#define NV00F9_IMPORT_ID_MAX (NVBIT64(48) - 1)
#define NV00F9_IMPORT_ID_SHIFT     16

#define NV00F9_ALLOCATION_PARAMETERS_MESSAGE_ID (0x00f9U)

typedef struct NV00F9_ALLOCATION_PARAMETERS {
    NvU32                imexChannel;
    NV_EXPORT_MEM_PACKET expPacket;
    NvU16                index;
    NvU32                flags;
    NV_DECLARE_ALIGNED(NvP64 pOsEvent, 8);
    NV_DECLARE_ALIGNED(NvU64 id, 8);
} NV00F9_ALLOCATION_PARAMETERS;

