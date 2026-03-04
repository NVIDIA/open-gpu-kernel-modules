/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0002.finn
//

#include "ctrl/ctrlxxxx.h"
#define NV0002_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x0002, NV0002_CTRL_##cat, idx)

/* Client command categories (6bits) */
#define NV0002_CTRL_RESERVED (0x00)
#define NV0002_CTRL_DMA      (0x01)


/*
 * NV0002_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 */

#define NV0002_CTRL_CMD_NULL (0x20000) /* finn: Evaluated from "(FINN_NV01_CONTEXT_DMA_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV0002_CTRL_CMD_UPDATE_CONTEXTDMA
 *
 * This command will update the parameters of the specified context dma.  The
 * context dma must be bound to a display channel.  The update is limited
 * to the display view of the context dma. Other use cases will continue to
 * use the original allocation parameters.
 *
 * This is used on platforms where memory may be moved by the operating
 * system after allocation.
 *
 * This control call supports the NVOS54_FLAGS_LOCK_BYPASS flag.
 *
 *   baseAddress
 *     This parameter, if selected by flags, indicates the new baseAddress for
 *     the ctxdma
 *   limit
 *     This parameter, if selected by flags, indicates the new limit of the
 *     ctxdma.
 *   hCtxDma
 *     ContextDma handle on which to operate. Must match the handle given to the control
 *     call.
 *   hChannel
 *     Display channel handle.  This field is ignored.
 *   hintHandle
 *     Hint value returned from HeapAllocHint which encodes information about
 *     the surface.  This is used by chips without generic kind.  Newer chips
 *     use the COMPR_INFO flag and the hintHandle must be zero.
 *   flags
 *     This parameter specifies flags which indicate which other parameters are
 *     valid.
 *       FLAGS_PAGESIZE updates the context DMA pagesize field, if not _DEFAULT
 *       FLAGS_USE_COMPR_INFO uses the surface format specified in the params, instead of hintHandle.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_OBJECT
 *    NV_ERR_INVALID_ARGUMENT
 *    NVOS_STATUS_NOT_SUPPORTED
 */
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA (0x20101) /* finn: Evaluated from "(FINN_NV01_CONTEXT_DMA_DMA_INTERFACE_ID << 8) | NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS_MESSAGE_ID" */

#define NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 baseAddress, 8);
    NV_DECLARE_ALIGNED(NvU64 limit, 8);
    NvHandle hSubDevice;
    NvHandle hCtxDma;
    NvHandle hChannel;
    NvHandle hintHandle;
    NvU32    flags;
} NV0002_CTRL_UPDATE_CONTEXTDMA_PARAMS;

#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_BASEADDRESS                        0:0
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_BASEADDRESS_INVALID                (0x00000000)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_BASEADDRESS_VALID                  (0x00000001)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_LIMIT                              1:1
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_LIMIT_INVALID                      (0x00000000)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_LIMIT_VALID                        (0x00000001)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_HINT                               2:2
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_HINT_INVALID                       (0x00000000)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_HINT_VALID                         (0x00000001)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_PAGESIZE                           4:3
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_PAGESIZE_DEFAULT                   (0x00000000)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_PAGESIZE_4K                        (0x00000001)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_PAGESIZE_BIG                       (0x00000002)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_USE_COMPR_INFO                     6:5
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_USE_COMPR_INFO_NONE                (0x00000000)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_USE_COMPR_INFO_FORMAT_PITCH        (0x00000001)
#define NV0002_CTRL_CMD_UPDATE_CONTEXTDMA_FLAGS_USE_COMPR_INFO_FORMAT_BLOCK_LINEAR (0x00000002)

/*
 * NV0002_CTRL_CMD_BIND_CONTEXTDMA
 *
 * Bind a context dma to a display channel.  Binding is no longer required for
 * Host channels, but does silently succeed.
 *
 * This control call supports the NVOS54_FLAGS_LOCK_BYPASS flag.
 *
 * This control replaces the obsolete RmBindContextDma() API.
 *
 * hChannel
 *     The channel for ctxdma bind
 *
 * Possible error codes include
 *   NV_OK
 *   NV_ERR_TOO_MANY_PRIMARIES          hash table is full
 *   NV_ERR_NO_MEMORY                   instance memory is full
 *   NV_ERR_INVALID_OFFSET              surface is not correctly aligned
 *   NV_ERR_STATE_IN_USE                context dma was already bound given channel
 */
#define NV0002_CTRL_CMD_BIND_CONTEXTDMA                                            (0x20102) /* finn: Evaluated from "(FINN_NV01_CONTEXT_DMA_DMA_INTERFACE_ID << 8) | NV0002_CTRL_BIND_CONTEXTDMA_PARAMS_MESSAGE_ID" */

#define NV0002_CTRL_BIND_CONTEXTDMA_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV0002_CTRL_BIND_CONTEXTDMA_PARAMS {
    NvHandle hChannel;
} NV0002_CTRL_BIND_CONTEXTDMA_PARAMS;

/*
 * NV0002_CTRL_CMD_UNBIND_CONTEXTDMA
 *
 * Unbind a context dma from a display channel.
 *
 * This control call supports the NVOS54_FLAGS_LOCK_BYPASS flag.
 *
 * hChannel
 *     The display channel to unbind from
 *
 * Possible error codes include
 *   NV_OK
 *   NV_ERR_INVALID_STATE               channel was not bound
 */
#define NV0002_CTRL_CMD_UNBIND_CONTEXTDMA (0x20103) /* finn: Evaluated from "(FINN_NV01_CONTEXT_DMA_DMA_INTERFACE_ID << 8) | NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS_MESSAGE_ID" */

#define NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS {
    NvHandle hChannel;
} NV0002_CTRL_UNBIND_CONTEXTDMA_PARAMS;

/* _ctrl0002.h_ */

