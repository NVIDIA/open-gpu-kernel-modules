/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      class/cl00e0.finn
//

#include "cl0080.h"

/*
 * Class definition for exporting memory handles to a different RM client on the
 * same or another node (OS).
 *
 * No memory is allocated or mapped using this class.
 * Parented by hClient.
 * NvRmDupObject() is not supported. Use NV_MEM_EXPORT_FLAGS_DUP_BY_UUID instead.
 */

#define NV_MEMORY_EXPORT           (0xe0U) /* finn: Evaluated from "NV00E0_ALLOCATION_PARAMETERS_MESSAGE_ID" */

#define NV_MEM_EXPORT_UUID_LEN     16
#define NV_MEM_EXPORT_PACKET_LEN   32
#define NV_MEM_EXPORT_METADATA_LEN 64

typedef struct NV_EXPORT_MEM_PACKET {
    NvU8 uuid[NV_MEM_EXPORT_UUID_LEN];
    NvU8 opaque[16];
} NV_EXPORT_MEM_PACKET;

#define NV_MEM_EXPORT_FLAGS_DEFAULT     0x00000000
#define NV_MEM_EXPORT_FLAGS_DUP_BY_UUID 0x00000001

/*
 * imexChannel [IN]
 * Used to associated export object with an IMEX channel.
 * When duping,
 * - If the caller is not the privileged IMEX daemon, validate the caller
 *   is subscribed to the channel.
 * - Validate that the imexChannel matches with that of the export object being
 *   duped.
 *
 *  packet [IN/OUT]
 *    Bag of bits which uniquely identifies this object universally.
 *    - When a new object is allocated, "packet" is returned as output.
 *    - If NV_MEM_EXPORT_FLAGS_DUP_BY_UUID is provided, "packet" is used as
 *      input to duplicate the existing object.
 *
 *  numMaxHandles [IN/OUT]
 *    Max number of memory handles to be attached to the export object.
 *    - Input when a new object is allocated.
 *    - Output When duplicating.
 *
 *  flags [IN/OUT]
 *    One of NV_MEM_EXPORT_FLAGS_*
 *    - Input when a new object is allocated or duplicated.
 *    - Output when duplicating. The flags at the time of allocation are
 *      returned.
 *
 *  metadata [IN/OUT]
 *    Private data about the export object (opaque to kernel mode)
 *    - Input when a new object is allocated.
 *    - Output When duplicating.
 *
 *  deviceInstanceMask [OUT]
 *    Attached device instance mask
 *
 *  giIdMasks [OUT]
 *    Attached GPU instance ID mask (MIG is currently not supported)
 *    Must be indexed using deviceInstanceMask.
 *
 *  numCurHandles [OUT]
 *    Number of memory handles currently attached to the export object.
 */

#define NV00E0_ALLOCATION_PARAMETERS_MESSAGE_ID (0x00e0U)

typedef struct NV00E0_ALLOCATION_PARAMETERS {
    NvU32                imexChannel;
    NV_EXPORT_MEM_PACKET packet;
    NvU16                numMaxHandles;
    NvU32                flags;
    NvU8                 metadata[NV_MEM_EXPORT_METADATA_LEN];
    NvU32                deviceInstanceMask;
    NvU32                giIdMasks[NV_MAX_DEVICES];
    NvU16                numCurHandles;
} NV00E0_ALLOCATION_PARAMETERS;
