/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl00f1.finn
//

#include "ctrl/ctrlxxxx.h"
#include "class/cl00e0.h"

/*
 * NV00F1_CTRL_CMD_GET_FABRIC_EVENTS
 *
 * Queries the fabric object for events.
 *
 *  eventArray [OUT]
 *    An array of import lifetime events.
 *
 *  numEvents [OUT]
 *    The number of valid events in eventArray.
 *
 *  bMoreEvents [OUT]
 *    Whether there are any remaining events to be queried.
 *
 * Possible status values returned are:
 *    NV_ERR_NOT_SUPPORTED
 *    NV_OK
 */
#define NV00F1_CTRL_CMD_GET_FABRIC_EVENTS               (0xf10001U) /* finn: Evaluated from "(FINN_IMEX_SESSION_INTERFACE_ID << 8) | NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS_MESSAGE_ID" */

/* Event types */
#define NV00F1_CTRL_FABRIC_EVENT_TYPE_MEM_IMPORT        0U
#define NV00F1_CTRL_FABRIC_EVENT_TYPE_MEM_UNIMPORT      1U
#define NV00F1_CTRL_FABRIC_EVENT_TYPE_REMOTE_GPU_ATTACH 2U
#define NV00F1_CTRL_FABRIC_EVENT_TYPE_CLIQUE_ID_CHANGE  3U

/*
 *  id
 *    Import ID. An import request can be uniquely identified using,
 *    <packet.uuid + index + ID>.
 *
 *  index
 *    Index of the export object to which the memory object is attached.
 *
 *  exportNodeId
 *    ID of the exporter node where memory will be imported.
 *
 *  exportUuid
 *    Universally unique identifier of the export object. This is extracted
 *    from a fabric packet.
 *

 */
typedef struct NV00F1_CTRL_FABRIC_MEM_IMPORT_EVENT_DATA {
    NV_DECLARE_ALIGNED(NvU64 id, 8);
    NvU16 index;
    NvU16 exportNodeId;
    NvU8  exportUuid[NV_MEM_EXPORT_UUID_LEN];
} NV00F1_CTRL_FABRIC_MEM_IMPORT_EVENT_DATA;

/*
 *  importEventId
 *    ID of the corresponding import event.
 *
 *  exportNodeId
 *    ID of the exporter node where memory will be unimported.
 */
typedef struct NV00F1_CTRL_FABRIC_MEM_UNIMPORT_EVENT_DATA {
    NV_DECLARE_ALIGNED(NvU64 importEventId, 8);
    NvU16 exportNodeId;
} NV00F1_CTRL_FABRIC_MEM_UNIMPORT_EVENT_DATA;

/*
 *  gpuFabricProbeHandle [IN]
 *    Fabric probe handle of the remote GPU
 *
 *  key [IN]
 *    Key is used by the GFM in the MCFLA team response as an ID to allow the
 *    RM to correlate it with the MCFLA team request.
 *
 *  cliqueId [IN]
 *    Clique ID of the remote GPU being attached.
 *
 *  index
 *    Index of the export object to which the memory object is attached.
 *
 *  exportNodeId
 *    ID of the exporter node where memory will be imported.
 *
 *  exportUuid
 *    Universally unique identifier of the export object. This is extracted
 *    from a fabric packet.
 */
typedef struct NV00F1_CTRL_ATTACH_REMOTE_GPU_EVENT_DATA {
    NV_DECLARE_ALIGNED(NvU64 gpuFabricProbeHandle, 8);
    NV_DECLARE_ALIGNED(NvU64 key, 8);
    NvU32 cliqueId;
    NvU16 index;
    NvU16 exportNodeId;
    NvU8  exportUuid[NV_MEM_EXPORT_UUID_LEN];
} NV00F1_CTRL_ATTACH_REMOTE_GPU_EVENT_DATA;

/*
 *  gpuId
 *    GPU ID of the GPU whose cliqueID has changed.
 *
 *  cliqueId
 *    Clique ID of the GPU.
 */
typedef struct NV00F1_CTRL_CLIQUE_ID_CHANGE_GPU_EVENT_DATA {
    NvU32 gpuId;
    NvU32 cliqueId;
} NV00F1_CTRL_CLIQUE_ID_CHANGE_GPU_EVENT_DATA;

/*
 *  type
 *    Event type, one of NV00F1_CTRL_FABRIC_EVENT_TYPE_*.
 *
 *  id
 *    A monotonically increasing event ID.
 *
 *  imexChannel
 *    imex channel subscribed by the caller.
 *
 *  data
 *    Event data
 */
typedef struct NV00F1_CTRL_FABRIC_EVENT {
    NvU8  type;
    NV_DECLARE_ALIGNED(NvU64 id, 8);
    NvU32 imexChannel;

    union {
        NV_DECLARE_ALIGNED(NV00F1_CTRL_FABRIC_MEM_IMPORT_EVENT_DATA import, 8);
        NV_DECLARE_ALIGNED(NV00F1_CTRL_FABRIC_MEM_UNIMPORT_EVENT_DATA unimport, 8);
        NV_DECLARE_ALIGNED(NV00F1_CTRL_ATTACH_REMOTE_GPU_EVENT_DATA attach, 8);
        NV00F1_CTRL_CLIQUE_ID_CHANGE_GPU_EVENT_DATA cliqueIdChange;
    } data;
} NV00F1_CTRL_FABRIC_EVENT;

#define NV00F1_CTRL_GET_FABRIC_EVENTS_ARRAY_SIZE 128U

#define NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS {
    NV_DECLARE_ALIGNED(NV00F1_CTRL_FABRIC_EVENT eventArray[NV00F1_CTRL_GET_FABRIC_EVENTS_ARRAY_SIZE], 8);
    NvU32  numEvents;
    NvBool bMoreEvents;
} NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS;

/*
 * NV00F1_CTRL_CMD_FINISH_MEM_UNIMPORT
 *
 * Notifies the unimport sequence is finished.
 *
 *  tokenArray [IN]
 *    An array of tokens that finished the unimport sequence.
 *
 *  numTokens [IN]
 *    The number of valid tokens in the tokenArray.
 *
 * Possible status values returned are:
 *    NV_ERR_OBJECT_NOT_FOUND
 *    NV_ERR_NOT_SUPPORTED
 *    NV_OK
 */
#define NV00F1_CTRL_CMD_FINISH_MEM_UNIMPORT (0xf10002U) /* finn: Evaluated from "(FINN_IMEX_SESSION_INTERFACE_ID << 8) | NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS_MESSAGE_ID" */

/*
 *  unimportEventId
 *    ID of the unimport event.
 */
typedef struct NV00F1_CTRL_FABRIC_UNIMPORT_TOKEN {
    NV_DECLARE_ALIGNED(NvU64 unimportEventId, 8);
} NV00F1_CTRL_FABRIC_UNIMPORT_TOKEN;

#define NV00F1_CTRL_FINISH_MEM_UNIMPORT_ARRAY_SIZE 256U

#define NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS {
    NV_DECLARE_ALIGNED(NV00F1_CTRL_FABRIC_UNIMPORT_TOKEN tokenArray[NV00F1_CTRL_FINISH_MEM_UNIMPORT_ARRAY_SIZE], 8);
    NvU32 numTokens;
} NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS;

/*
 * NV00F1_CTRL_CMD_DISABLE_IMPORTERS
 *
 *  Disables clients which imported memory from the given nodeId. Channel
 *  recovery (disablement) sequence is also invoked to halt fabric traffic
 *  from these clients.
 *
 *  nodeId [IN]
 *    An array of tokens that finished the unimport sequence.
 *
 * Possible status values returned are:
 *    NV_ERR_NOT_SUPPORTED
 *    NV_OK
 */
#define NV00F1_CTRL_CMD_DISABLE_IMPORTERS (0xf10003U) /* finn: Evaluated from "(FINN_IMEX_SESSION_INTERFACE_ID << 8) | NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS_MESSAGE_ID" */

#define NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS {
    NvU16 nodeId;
} NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS;

/* _ctrl00f1.h_ */
