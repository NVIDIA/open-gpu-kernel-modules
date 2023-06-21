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
// Source file:      ctrl/ctrl0090.finn
//



// NV0080_CTRL_GR_TPC_PARTITION_MODE
#include "ctrl/ctrl0080/ctrl0080gr.h"

/*!
 * This file defines control call interfaces for the KernelGraphicsContext
 * objects Each channel running within a TSG contains a HW context represented
 * by the above object(s). GR permits multiple channels to share a single
 * context, and these APIs operate upon that context and may be issued from any
 * TSG, channel, or context handle.
 */

/*
 * NV0090_CTRL_CMD_NULL
 *
 * @brief This command does nothing.
 *
 * @return NV_OK
 */
#define NV0090_CTRL_CMD_NULL (0x900100) /* finn: Evaluated from "(FINN_NV0090_KERNEL_GRAPHICS_CONTEXT_INTERFACE_ID << 8) | 0x0" */

/*!
 * NV0090_CTRL_CMD_SET_TPC_PARTITION_MODE
 * NV0090_CTRL_CMD_GET_TPC_PARTITION_MODE
 *
 * @brief sets or gets the TPC partition mode for this context
 *
 * @param[in/out] mode              Partitioning mode enum value
 * @param[in/out] bEnableAllTpcs    Flag to enable all TPCs by default
 *
 * @return NV_OK on success
 *         NV_ERR_OBJECT_NOT_FOUND if issued on non-GR ctx
 */
typedef struct NV0090_CTRL_TPC_PARTITION_MODE_PARAMS {
    NV0080_CTRL_GR_TPC_PARTITION_MODE mode;
    NvBool                            bEnableAllTpcs;
} NV0090_CTRL_TPC_PARTITION_MODE_PARAMS;

#define NV0090_CTRL_CMD_SET_TPC_PARTITION_MODE          (0x900101) /* finn: Evaluated from "(FINN_NV0090_KERNEL_GRAPHICS_CONTEXT_INTERFACE_ID << 8) | 0x1" */
#define NV0090_CTRL_CMD_INTERNAL_SET_TPC_PARTITION_MODE (0x900102) /* finn: Evaluated from "(FINN_NV0090_KERNEL_GRAPHICS_CONTEXT_INTERFACE_ID << 8) | 0x2" */
#define NV0090_CTRL_CMD_GET_TPC_PARTITION_MODE          (0x900103) /* finn: Evaluated from "(FINN_NV0090_KERNEL_GRAPHICS_CONTEXT_INTERFACE_ID << 8) | 0x3" */
#define NV0090_CTRL_CMD_INTERNAL_GET_TPC_PARTITION_MODE (0x900104) /* finn: Evaluated from "(FINN_NV0090_KERNEL_GRAPHICS_CONTEXT_INTERFACE_ID << 8) | 0x4" */

/**
 * NV0090_CTRL_GET_MMU_DEBUG_MODE
 *
 * @brief retrieve the current MMU debug mode for the grctx according to the HW
 *
 * @param[out] bMode            current MMU debug mode
 */
typedef struct NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS {
    NvBool bMode;
} NV0090_CTRL_GET_MMU_DEBUG_MODE_PARAMS;

#define NV0090_CTRL_CMD_GET_MMU_DEBUG_MODE          (0x900105) /* finn: Evaluated from "(FINN_NV0090_KERNEL_GRAPHICS_CONTEXT_INTERFACE_ID << 8) | 0x5" */
#define NV0090_CTRL_CMD_INTERNAL_GET_MMU_DEBUG_MODE (0x900106) /* finn: Evaluated from "(FINN_NV0090_KERNEL_GRAPHICS_CONTEXT_INTERFACE_ID << 8) | 0x6" */

/**
 * NV0090_CTRL_PROGRAM_VIDEMEM_PROMOTE
 *
 * @brief This control call is used to set the promotion policies to vidmem through
 * per-TSG config
 *
 * If the request is for less bytes than the current PROMOTE setting, then
 * the request is promoted. For example if the request size is for 32B and
 * 64B promotion is turned on for that unit, then the request to FB will be
 * for all the bytes to the 64B aligned address.
 *
 * @param[in] l1
 *   An input parameter which represents VIDMEM_L1_PROMOTE[17:16].
 * @param[in] t1
 *   An input parameter which represents VIDMEM_T1_PROMOTE[19:18].
 *
 * @return NV_OK on success, or 
 *         NV_ERR_INVALID_ARGUMENT or
 *         NV_ERR_INVALID_STATE
 */
typedef enum NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE {
    NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE_NONE = 0,
    NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE_64B = 1,
    NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE_128B = 2,
} NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE;

typedef struct NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_FIELD {
    NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_SIZE size;
} NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_FIELD;

#define NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS {
    NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_FIELD l1;
    NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_FIELD t1;
} NV0090_CTRL_PROGRAM_VIDMEM_PROMOTE_PARAMS;

#define NV0090_CTRL_CMD_PROGRAM_VIDMEM_PROMOTE          (0x900107) /* finn: Evaluated from "(FINN_NV0090_KERNEL_GRAPHICS_CONTEXT_INTERFACE_ID << 8) | 0x7" */
#define NV0090_CTRL_CMD_INTERNAL_PROGRAM_VIDMEM_PROMOTE (0x900108) /* finn: Evaluated from "(FINN_NV0090_KERNEL_GRAPHICS_CONTEXT_INTERFACE_ID << 8) | 0x8" */

