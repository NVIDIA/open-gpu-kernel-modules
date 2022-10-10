/*
 * SPDX-FileCopyrightText: Copyright (c) 2006-2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl2080/ctrl2080fla.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_XX FLA control commands and parameters */

#include "ctrl2080common.h"

/*
 * NV2080_CTRL_CMD_FLA_RANGE
 *
 * This command is used to initialize/destroy FLA VAS for a GPU.This is intended
 * to be used by RM clients that manages the FLA VASpace range. The mode of the
 * command is decided based on the parameter passed by the client.
 *
 *    base
 *         This parameter specifies the base of the FLA VAS that needs to be allocated
 *         for this GPU
 *
 *    size
 *         This parameter specifies the size of the FLA VAS that needs to be allocated
 *         for this GPU
 *
 *    mode
 *         This parameter specifies the functionality of the command.
 *           MODE_INITIALIZE
 *             Setting this mode, will initialize the FLA VASpace for the gpu with
 *             base and size passed as arguments. FLA VASpace will be owned by RM.
 *             if the client calls the command more than once before destroying
 *             the FLA VAS, then this command will verify the range exported before and
 *             return success if it matches. If FLA is not supported for the platform,
 *             will return NV_ERR_NOT_SUPPORTED.
 *           MODE_DESTROY (deprecated)
 *             This command is NOP.
 *           MODE_HOST_MANAGED_VAS_INITIALIZE
 *             This mode will initialize the FLA VASpace for the gpu with hVASpace
 *             handle in addition to base and size arguments. FLA VASpace will be initiated 
 *             and owned by guest RM. Used only in virtualization platforms by internal clients.
 *           MODE_HOST_MANAGED_VAS_DESTROY
 *              This mode will destroy the FLA VAS associated with the device. It will destruct
 *              only the resources associated with host RM side. Used only in virtualization platforms
 *              by internal clients.
 *
 *    hVASpace 
 *         This paramete specifies the FLA VAspace that needs to be associated with 
 *         device. This parameter takes effect only for internal client in virtualization
 *         platforms. For any other platform and external clients, this parameter has no effect.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_IN_USE
 *   NV_ERR_INVALID_OWNER
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV2080_CTRL_CMD_FLA_RANGE (0x20803501) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLA_INTERFACE_ID << 8) | NV2080_CTRL_FLA_RANGE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FLA_RANGE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_FLA_RANGE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 base, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
    NvU32    mode;
    NvHandle hVASpace;
} NV2080_CTRL_FLA_RANGE_PARAMS;

#define NV2080_CTRL_FLA_RANGE_PARAMS_MODE_NONE 0x00000000
#define NV2080_CTRL_FLA_RANGE_PARAMS_MODE_INITIALIZE                         NVBIT(0)
#define NV2080_CTRL_FLA_RANGE_PARAMS_MODE_DESTROY                            NVBIT(1)
#define NV2080_CTRL_FLA_RANGE_PARAMS_MODE_HOST_MANAGED_VAS_INITIALIZE        NVBIT(2)
#define NV2080_CTRL_FLA_RANGE_PARAMS_MODE_HOST_MANAGED_VAS_DESTROY           NVBIT(3)


/*
 * NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK
 *
 * This command is used to (un)bind FLA Instance Memory Block(IMB) with MMU.
 * This control call is created for vGPU platform, when a FLA VAS is created/destroyed
 * by Guest RM. Guest RM doesn't have privilege to (un)bind the IMB with MMU, hence
 * need to be RPC-ed to Host RM to (un)bind
 * The mode of the command is decided based on the actionParam passed by the client.
 *
 *    imbPhysAddr
 *         This parameter specifies the FLA Instance Memory Block PA to be programmed
 *         to MMU. IMB address should be 4k aligned. This parameter is needed only
 *         for ACTION_BIND.
 *
 *    addrSpace
 *         This parameter specifies the address space of FLA Instance Memory Block. This
 *         parmater is needed only for ACTION_BIND.
 *         Available options are:
 *           NV2080_CTRL_FLA_ADDRSPACE_SYSMEM
 *               Clients need to use this address space if the IMB is located in sysmem
 *           NV2080_CTRL_FLA_ADDRSPACE_FBMEM
 *               Clients need to use this address space if the IMB is located in FB
 *
 *    actionParam
 *         This parameter specifies the functionality of the command.
 *           NV2080_CTRL_FLA_ACTION_BIND
 *             Setting this type, will call busBindFla helper HAL
 *           NV2080_CTRL_FLA_ACTION_UNBIND
 *             Setting this type, will call busUnbindFla helper HAL
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INSUFFICIENT_PERMISSIONS
 */
typedef enum NV2080_CTRL_FLA_ADDRSPACE {
    NV2080_CTRL_FLA_ADDRSPACE_SYSMEM = 0,
    NV2080_CTRL_FLA_ADDRSPACE_FBMEM = 1,
} NV2080_CTRL_FLA_ADDRSPACE;

typedef enum NV2080_CTRL_FLA_ACTION {
    NV2080_CTRL_FLA_ACTION_BIND = 0,
    NV2080_CTRL_FLA_ACTION_UNBIND = 1,
} NV2080_CTRL_FLA_ACTION;

#define NV2080_CTRL_CMD_FLA_SETUP_INSTANCE_MEM_BLOCK (0x20803502) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLA_INTERFACE_ID << 8) | NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 imbPhysAddr, 8);
    NV2080_CTRL_FLA_ADDRSPACE addrSpace;
    NV2080_CTRL_FLA_ACTION    flaAction;
} NV2080_CTRL_FLA_SETUP_INSTANCE_MEM_BLOCK_PARAMS;


/*
 * NV2080_CTRL_CMD_FLA_GET_RANGE
 *
 * This command is used to query the FLA base and size from plugin to return as static info to Guest RM. 
 *
 *    base
 *         This parameter returns the base address of FLA range registered to the subdevice. 
 *    size
 *         This parameter returns the size of FLA range registered to the subdevice. 
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FLA_GET_RANGE (0x20803503) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLA_INTERFACE_ID << 8) | NV2080_CTRL_FLA_GET_RANGE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FLA_GET_RANGE_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_FLA_GET_RANGE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 base, 8);
    NV_DECLARE_ALIGNED(NvU64 size, 8);
} NV2080_CTRL_FLA_GET_RANGE_PARAMS;

/*
 * NV2080_CTRL_CMD_FLA_GET_FABRIC_MEM_STATS
 *
 * This command returns the total size and the free size of the fabric vaspace.
 * Note: This returns the information for the FABRIC_VASPACE_A class.
 *
 *   totalSize[OUT]
 *      - Total fabric vaspace.
 *
 *   freeSize [OUT]
 *      - Available fabric vaspace.
 *
 * Possible status values returned are:
 *
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_FLA_GET_FABRIC_MEM_STATS (0x20803504) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_FLA_INTERFACE_ID << 8) | NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 totalSize, 8);
    NV_DECLARE_ALIGNED(NvU64 freeSize, 8);
} NV2080_CTRL_FLA_GET_FABRIC_MEM_STATS_PARAMS;

// _ctrl2080fla_h_
