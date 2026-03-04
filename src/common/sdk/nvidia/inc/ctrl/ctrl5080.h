/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl5080.finn
//

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl0080/ctrl0080dma.h"  /* NV0080_CTRL_DMA_FILL_PTE_MEM_PARAMS */
#include "ctrl/ctrl2080/ctrl2080dma.h"  /* NV2080_CTRL_DMA_* */
#include "ctrl/ctrl2080/ctrl2080fifo.h" /* NV2080_CTRL_FIFO_* */
#include "ctrl/ctrl2080/ctrl2080gpu.h"  /* NV2080_CTRL_GPU_* */
#include "ctrl/ctrl2080/ctrl2080gr.h"   /* NV2080_CTRL_GR_* */
/* NV5080_DEFERRED_API - deferred RmControl commands */


#define NV5080_CTRL_CMD(cat,idx) NVXXXX_CTRL_CMD(0x5080, NV5080_CTRL_##cat,idx)

/* Command categories (6bits) */
#define NV5080_CTRL_RESERVED (0x00)
#define NV5080_CTRL_DEFERRED (0x01)

/*
 * NV5080_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV5080_CTRL_CMD_NULL (0x50800000) /* finn: Evaluated from "(FINN_NV50_DEFERRED_API_CLASS_RESERVED_INTERFACE_ID << 8) | 0x0" */






/*
 * NV5080_CTRL_CMD_DEFERRED_API (deprecated; use NV5080_CTRL_CMD_DEFERRED_API_V2 instead)
 *
 * This command sets up a deferred api call.
 *    hApiHandle
 *      Client Unique Handle which is used as the data of a sw method to invoke
 *      the api in the future
 *    cmd
 *      The rmcontrol cmd to invoke as a deferred api.
 *    flags_delete
 *      Indicates if an explicit delete is needed (default behavior is to
 *      auto delete after SW method has executed/completed).
 *    flags_wait_for_tlb_flush
 *      Indicates if the API should wait for an InvalidateTlb to also occur
 *      (not just that it's executed) before being considered completed and
 *      works in conjunction with flags_delete.
 *    hClientVA, hDeviceVA
 *      Client/Device handles of the owner of the virtual address space to
 *      to be updated (used with the FillPteMem API bundle)
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV5080_CTRL_CMD_DEFERRED_API (0x50800101) /* finn: Evaluated from "(FINN_NV50_DEFERRED_API_CLASS_DEFERRED_INTERFACE_ID << 8) | NV5080_CTRL_DEFERRED_API_PARAMS_MESSAGE_ID" */

#define NV5080_CTRL_DEFERRED_API_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV5080_CTRL_DEFERRED_API_PARAMS {
    NvHandle hApiHandle;
    NvU32    cmd;
    NvU32    flags;
    NvHandle hClientVA;
    NvHandle hDeviceVA;

    union {
        NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS InitCtx, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS PromoteCtx, 8);

        NV2080_CTRL_GPU_EVICT_CTX_PARAMS      EvictCtx;

        NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS InvalidateTlb;

        NV_DECLARE_ALIGNED(NV0080_CTRL_DMA_FILL_PTE_MEM_PARAMS FillPteMem, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS ZcullCtxsw, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS PmCtxsw, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS PreemptionCtxsw, 8);
    } api_bundle;
} NV5080_CTRL_DEFERRED_API_PARAMS;

#define NV5080_CTRL_CMD_DEFERRED_API_FLAGS_DELETE                       0:0
#define NV5080_CTRL_CMD_DEFERRED_API_FLAGS_DELETE_EXPLICIT          (0x00000001)
#define NV5080_CTRL_CMD_DEFERRED_API_FLAGS_DELETE_IMPLICIT          (0x00000000)

#define NV5080_CTRL_CMD_DEFERRED_API_FLAGS_WAIT_FOR_TLB_FLUSH           1:1
#define NV5080_CTRL_CMD_DEFERRED_API_FLAGS_WAIT_FOR_TLB_FLUSH_FALSE (0x00000000)
#define NV5080_CTRL_CMD_DEFERRED_API_FLAGS_WAIT_FOR_TLB_FLUSH_TRUE  (0x00000001)
/*
 * NV5080_CTRL_CMD_DEFERRED_API_V2
 *
 * This command sets up a deferred api call.
 *    hApiHandle
 *      Client Unique Handle which is used as the data of a sw method to invoke
 *      the api in the future
 *    cmd
 *      The rmcontrol cmd to invoke as a deferred api.
 *    flags_delete
 *      Indicates if an explicit delete is needed (default behavior is to
 *      auto delete after SW method has executed/completed).
 *    flags_wait_for_tlb_flush
 *      Indicates if the API should wait for an InvalidateTlb to also occur
 *      (not just that it's executed) before being considered completed and
 *      works in conjunction with flags_delete.
 *    hClientVA, hDeviceVA
 *      Client/Device handles of the owner of the virtual address space to
 *      to be updated (used with the FillPteMem API bundle)
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV5080_CTRL_CMD_DEFERRED_API_V2                             (0x50800103) /* finn: Evaluated from "(FINN_NV50_DEFERRED_API_CLASS_DEFERRED_INTERFACE_ID << 8) | NV5080_CTRL_DEFERRED_API_V2_PARAMS_MESSAGE_ID" */

#define NV5080_CTRL_DEFERRED_API_V2_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV5080_CTRL_DEFERRED_API_V2_PARAMS {
    NvHandle hApiHandle;
    NvU32    cmd;
    NvU32    flags;
    NvHandle hClientVA;
    NvHandle hDeviceVA;

    union {
        NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_INITIALIZE_CTX_PARAMS InitCtx, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_GPU_PROMOTE_CTX_PARAMS PromoteCtx, 8);

        NV2080_CTRL_GPU_EVICT_CTX_PARAMS      EvictCtx;

        NV2080_CTRL_DMA_INVALIDATE_TLB_PARAMS InvalidateTlb;

        NV_DECLARE_ALIGNED(NV0080_CTRL_DMA_FILL_PTE_MEM_PARAMS FillPteMem, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_GR_CTXSW_ZCULL_BIND_PARAMS ZcullCtxsw, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_GR_CTXSW_PM_BIND_PARAMS PmCtxsw, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_FIFO_DISABLE_CHANNELS_PARAMS DisableChannels, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_GR_CTXSW_PREEMPTION_BIND_PARAMS PreemptionCtxsw, 8);

        NV_DECLARE_ALIGNED(NV2080_CTRL_FIFO_UPDATE_CHANNEL_INFO_PARAMS ChannelInfoUpdate, 8);
    } api_bundle;
} NV5080_CTRL_DEFERRED_API_V2_PARAMS;

/* This is just used to forward NV5080_CTRL_CMD_DEFERRED_API_V2 from kernel RM
 * to physical RM.  The parameters are unchanged. */
#define NV5080_CTRL_CMD_DEFERRED_API_INTERNAL (0x50800104) /* finn: Evaluated from "(FINN_NV50_DEFERRED_API_CLASS_DEFERRED_INTERFACE_ID << 8) | NV5080_CTRL_DEFERRED_API_INTERNAL_PARAMS_MESSAGE_ID" */

#define NV5080_CTRL_DEFERRED_API_INTERNAL_PARAMS_MESSAGE_ID (0x4U)

typedef NV5080_CTRL_DEFERRED_API_V2_PARAMS NV5080_CTRL_DEFERRED_API_INTERNAL_PARAMS;

/*
 * NV5080_CTRL_CMD_REMOVE_API
 *
 * This command removes an explicit deferred api call.
 *    hApiHandle
 *      Client Unique Handle which is used as the data of a sw method to invoke
 *      the api in the future
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV5080_CTRL_CMD_REMOVE_API (0x50800102) /* finn: Evaluated from "(FINN_NV50_DEFERRED_API_CLASS_DEFERRED_INTERFACE_ID << 8) | NV5080_CTRL_REMOVE_API_PARAMS_MESSAGE_ID" */

#define NV5080_CTRL_REMOVE_API_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV5080_CTRL_REMOVE_API_PARAMS {
    NvHandle hApiHandle;
} NV5080_CTRL_REMOVE_API_PARAMS;

/* _ctrl5080_h_ */

