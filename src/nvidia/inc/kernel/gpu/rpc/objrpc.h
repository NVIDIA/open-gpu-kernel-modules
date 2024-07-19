/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// #ifndef NVOC
// #include "g_objrpc_nvoc.h"
// #endif

#ifndef _OBJRPC_H_
#define _OBJRPC_H_

#include "vgpu/rpc_headers.h"
#include "diagnostics/nv_debug_dump.h"
#include "ctrl/ctrl2080/ctrl2080event.h" // rmcontrol params (from hal)
#include "ctrl/ctrl2080/ctrl2080gpu.h" // rmcontrol params (from hal)
#include "ctrl/ctrl2080/ctrl2080rc.h" // rmcontrol params (from hal)
#include "ctrl/ctrl2080/ctrl2080perf.h" // rmcontrol params (from hal)
#include "ctrl/ctrl0080/ctrl0080fb.h" // rmcontrol params (from hal)
#include "ctrl/ctrl0080/ctrl0080dma.h" // rmcontrol params (from hal)
#include "gpu/gsp/message_queue.h"
#include "libraries/utils/nvbitvector.h"

#include "gpu/rpc/objrpcstructurecopy.h"


typedef struct GSP_FIRMWARE GSP_FIRMWARE;
typedef struct _object_vgpu OBJVGPU, *POBJVGPU;
TYPEDEF_BITVECTOR(MC_ENGINE_BITVECTOR);

#include "g_rpc_hal.h" // For RPC_HAL_IFACES
#include "g_rpc_odb.h" // For RPC_HAL_IFACES

#define RPC_TIMEOUT_LIMIT_PRINT_RATE_THRESH 3  // rate limit after 3 prints
#define RPC_TIMEOUT_LIMIT_PRINT_RATE_SKIP   29 // skip 29 of 30 prints

#define RPC_HISTORY_DEPTH 128

typedef struct RpcHistoryEntry
{
    NvU32 function;
    NvU64 data[2];
    NvU64 ts_start;
    NvU64 ts_end;
} RpcHistoryEntry;

struct OBJRPC{
    OBJECT_BASE_DEFINITION(RPC);

    struct {
        NvU32 ipVersion;
    }__nvoc_pbase_Object[1]; // This nested structure mechanism is to bypass NVOC

    // Message buffer fields
    NvU32 *message_buffer;
    NvU32 *message_buffer_priv;
    MEMORY_DESCRIPTOR *pMemDesc_mesg;
    NvU32  maxRpcSize;
    NvU32  largeRpcSize;

    // UVM Message buffer fields
    NvU32 *message_buffer_uvm;
    NvU32 *message_buffer_priv_uvm;
    MEMORY_DESCRIPTOR *pMemDesc_mesg_uvm;

    /* Message Queue */
    struct _message_queue_info *pMessageQueueInfo;

    RpcHistoryEntry rpcHistory[RPC_HISTORY_DEPTH];
    NvU32 rpcHistoryCurrent;
    RpcHistoryEntry rpcEventHistory[RPC_HISTORY_DEPTH];
    NvU32 rpcEventHistoryCurrent;
    NvU32 timeoutCount;
    NvBool bQuietPrints;

    OBJRPCSTRUCTURECOPY rpcStructureCopy;
};

//
// Utility macros for composing RPC messages.
// See <vgpu/dev_vgpu.h> for message formats.
// A message has a fixed-format header and optionally a variable length
// parameter after the header.
//

#define vgpu_rpc_message_header_v  ((rpc_message_header_v*)(pRpc->message_buffer))
#define rpc_message                (vgpu_rpc_message_header_v->rpc_message_data)

static inline void _objrpcStructureCopyAssignIpVersion(struct OBJRPCSTRUCTURECOPY* pRpcStructureCopy, NvU32 ipVersion)
{
    pRpcStructureCopy->__nvoc_pbase_Object->ipVersion = ipVersion;
}

static inline void _objrpcAssignIpVersion(struct OBJRPC* pRpc, NvU32 ipVersion)
{
    pRpc->__nvoc_pbase_Object->ipVersion = ipVersion;
}

OBJRPC *initRpcObject(OBJGPU *pGpu);
void rpcSetIpVersion(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 ipVersion);
void rpcObjIfacesSetup(OBJRPC *pRpc);
NV_STATUS rpcWriteCommonHeader(OBJGPU *pGpu, OBJRPC *pRpc, NvU32 func, NvU32 paramLength);
NV_STATUS rpcWriteCommonHeaderSim(OBJGPU *pGpu);
NV_STATUS vgpuGspSetupBuffers(OBJGPU *pGpu);
void vgpuGspTeardownBuffers(OBJGPU *pGpu);
NV_STATUS vgpuReinitializeRpcInfraOnStateLoad(OBJGPU *pGpu);

// Initialize and free RPC infrastructure
NV_STATUS initRpcInfrastructure_VGPU(OBJGPU *pGpu);
NV_STATUS freeRpcInfrastructure_VGPU(OBJGPU *pGpu);

NV_STATUS _allocRpcMemDesc(OBJGPU *pGpu, NvU64 size, NvBool bContig, NV_ADDRESS_SPACE addrSpace, NvU32 memFlags,
                           MEMORY_DESCRIPTOR **ppMemDesc, void **ppMemBuffer, void **ppMemBufferPriv);
void _freeRpcMemDesc(OBJGPU *pGpu, MEMORY_DESCRIPTOR **ppMemDesc, void **ppMemBuffer, void **ppMemBufferPriv);

NV_STATUS rpcDmaControl_wrapper(OBJGPU *pGpu, OBJRPC *pRpc, NvHandle hClient, NvHandle hObject, NvU32 cmd,
                               void *pParamStructPtr, NvU32 paramSize);
//
// OBJGPU RPC member accessors.
// Historically, they have been defined inline by the following macros.
// These definitions were migrated to gpu.c in order to avoid having to include object headers in
// this file.
//

OBJRPC *gpuGetGspClientRpc(OBJGPU*);
OBJRPC *gpuGetVgpuRpc(OBJGPU*);
OBJRPC *gpuGetRpc(OBJGPU*);

#define GPU_GET_GSPCLIENT_RPC(u)   gpuGetGspClientRpc(u)
#define GPU_GET_VGPU_RPC(u)        gpuGetVgpuRpc(u)
#define GPU_GET_RPC(u)             gpuGetRpc(u)

#endif // _OBJRPC_H_
