/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc763.finn
//

#include "ctrl/ctrlxxxx.h"
/* Vidmem Access bit buffer control commands and parameters */

#define NVC763_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0xC763, NVC763_CTRL_##cat, idx)

/* MMU_VIDMEM_ACCESS_BIT_BUFFER command categories (6bits) */
#define NVC763_CTRL_RESERVED                     (0x00)
#define NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER     (0x01)

/*
 * SW def for number of range checkers. Current value taken from 
 * NV_PFB_PRI_MMU_VIDMEM_ACCESS_BIT_START_ADDR_LO__SIZE_1
 * on GA102. Compile time assert to check that the below
 * definition is consistent with HW manuals is included in
 * each gmmu HAL where this is relevant.
 */
#define NV_VIDMEM_ACCESS_BIT_BUFFER_NUM_CHECKERS 8

/*
 * NVC763_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVC763_CTRL_CMD_NULL                     (0xc7630000) /* finn: Evaluated from "(FINN_MMU_VIDMEM_ACCESS_BIT_BUFFER_RESERVED_INTERFACE_ID << 8) | 0x0" */

#define NVC763_CTRL_CMD_VIDMEM_ACCESS_BIT_ENABLE_LOGGING (0xc7630101) /* finn: Evaluated from "(FINN_MMU_VIDMEM_ACCESS_BIT_BUFFER_VIDMEM_ACCESS_BIT_BUFFER_INTERFACE_ID << 8) | NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_ENABLE_LOGGING_PARAMS_MESSAGE_ID" */

// Supported granularities for the vidmem access bit buffer logging
typedef enum NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY {
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_64KB = 0,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_128KB = 1,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_256KB = 2,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_512KB = 3,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_1MB = 4,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_2MB = 5,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_4MB = 6,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_8MB = 7,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_16MB = 8,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_32MB = 9,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_64MB = 10,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_128MB = 11,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_256MB = 12,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_512MB = 13,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_1GB = 14,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY_2GB = 15,
} NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY;

/**
 * enum of disable mode to be used when the MMU enters protected mode
 */
typedef enum NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DISABLE_MODE {
    /*!
     * Disable mode will set all the access/dirty bits as '0'
     */
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DISABLE_MODE_CLEAR = 0,
    /*!
     * Disable mode will set all the access/dirty bits as '1'
     */
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DISABLE_MODE_SET = 1,
} NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DISABLE_MODE;


// 
// If clients want to enable logging specifically for some MMU, clients need to
// do it in a loop
//
typedef enum NV_VIDMEM_ACCESS_BIT_BUFFER_MMU_TYPE {
    /*!
     * Read/Write Attrs only for HUBMMU registers
     */
    NV_VIDMEM_ACCESS_BIT_BUFFER_HUBMMU = 0,
    /*!
     * Read/Write Attrs only for GPCMMU registers
     */
    NV_VIDMEM_ACCESS_BIT_BUFFER_GPCMMU = 1,
    /*!
     * Read/Write Attrs only for HSHUBMMU registers
     */
    NV_VIDMEM_ACCESS_BIT_BUFFER_HSHUBMMU = 2,
    /*!
     * Default will enable for all MMU possible
     */
    NV_VIDMEM_ACCESS_BIT_BUFFER_DEFAULT = 3,
} NV_VIDMEM_ACCESS_BIT_BUFFER_MMU_TYPE;

typedef enum NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_TRACK_MODE {
    /*!
     Mode to track access bits
     */
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_TRACK_MODE_ACCESS = 0,
    /*!
     Mode to track dirty bits
     */
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_TRACK_MODE_DIRTY = 1,
} NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_TRACK_MODE;

/*
 * NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_ENABLE_LOGGING_PARAMS
 *
 * This structure is used to enable logging of the VAB and specifies
 * the requested configuration for the 8 independent range checkers.
 * The tracking mode and disable mode are the same for all range checkers.
 */
#define NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_ENABLE_LOGGING_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_ENABLE_LOGGING_PARAMS {
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_GRANULARITY  granularity[NV_VIDMEM_ACCESS_BIT_BUFFER_NUM_CHECKERS];
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_TRACK_MODE   trackMode;
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DISABLE_MODE disableMode;
    NV_DECLARE_ALIGNED(NvU64 startAddress[NV_VIDMEM_ACCESS_BIT_BUFFER_NUM_CHECKERS], 8);
    NvU8                                              rangeCount;
    NV_VIDMEM_ACCESS_BIT_BUFFER_MMU_TYPE              mmuType;
} NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_ENABLE_LOGGING_PARAMS;


#define NVC763_CTRL_CMD_VIDMEM_ACCESS_BIT_DISABLE_LOGGING (0xc7630102) /* finn: Evaluated from "(FINN_MMU_VIDMEM_ACCESS_BIT_BUFFER_VIDMEM_ACCESS_BIT_BUFFER_INTERFACE_ID << 8) | 0x2" */

/*
 * NVC763_CTRL_CMD_VIDMEM_ACCESS_BIT_BUFFER_DUMP
 *
 * This call initiates the dump request with the properties set using enable
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVC763_CTRL_CMD_VIDMEM_ACCESS_BIT_DUMP            (0xc7630103) /* finn: Evaluated from "(FINN_MMU_VIDMEM_ACCESS_BIT_BUFFER_VIDMEM_ACCESS_BIT_BUFFER_INTERFACE_ID << 8) | NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS_MESSAGE_ID" */

/*
 * NV_VAB_OP enumerates the types of dumps supported
 *
 * The options are:
 *
 *     AGGREGATE
 *     Collects access buffer bits over multiple dumps using a bitwise OR.
 *
 *     DIFF
 *     Sets a bit to 1 if it changed from 0 to 1 with this dump. If a bit was
 *     cleared since the last dump it will be 0. If a bit does not change
 *     with this dump it will be 0.
 *
 *     CURRENT
 *     Copies the current access bit buffer state as is from HW. This operation
 *     clears any underlying aggregation from previous dumps with the other
 *     two operations.
 *
 *     INVALID
 *     Should be unused and otherwise indicates error
 */
typedef enum NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_OP {
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_OP_AGGREGATE = 0,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_OP_DIFF = 1,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_OP_CURRENT = 2,
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_OP_INVALID = 3,
} NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_OP;

/*
 * NVC763_CTRL_VIDMEM_ACCESS_BIT_DUMP_PARAMS
 *
 * This structure records the dumped bits for the client
 * masked by the client's access bit mask determined
 * during VidmemAccessBitBuffer construction.
 *
 * bMetadata [IN]
 *      Whether or not clients want disable data.
 *
 * op_enum [IN]
 *      A member of NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_OP controlling the type of dump.
 *
 * accessBits [OUT]
 *      The client's access bits masked according to the client's access bit mask.
 *
 * gpcDisable [OUT]
 *      The GPC disable data from the VAB dump. See GPC_DISABLE in the Ampere-801 FD.
 *      
 * hubDisable [OUT]
 *      The HUB disable data from the VAB dump. See HUB_DISABLE in the Ampere-801 FD.
 *
 * hsceDisable [OUT]
 *      The HSCE disable data from the VAB dump. See HSCE_DIS in the Ampere-801 FD.
 *
 * linkDisable [OUT]
 *      The LINK disable data from the VAB dump. See LINK_DIS in the Ampere-801 FD.
 */
#define NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS {
    NvBool                                  bMetadata;
    NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_OP op_enum;
    NV_DECLARE_ALIGNED(NvU64 accessBits[64], 8);
    NV_DECLARE_ALIGNED(NvU64 gpcDisable, 8);
    NvU32                                   hubDisable;
    NvU16                                   hsceDisable;
    NvU8                                    linkDisable;
} NVC763_CTRL_VIDMEM_ACCESS_BIT_BUFFER_DUMP_PARAMS;


#define NVC763_CTRL_CMD_VIDMEM_ACCESS_BIT_PUT_OFFSET (0xc7630104) /* finn: Evaluated from "(FINN_MMU_VIDMEM_ACCESS_BIT_BUFFER_VIDMEM_ACCESS_BIT_BUFFER_INTERFACE_ID << 8) | NVC763_CTRL_VIDMEM_ACCESS_BIT_PUT_OFFSET_PARAMS_MESSAGE_ID" */

#define NVC763_CTRL_VIDMEM_ACCESS_BIT_PUT_OFFSET_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVC763_CTRL_VIDMEM_ACCESS_BIT_PUT_OFFSET_PARAMS {
    NvU32 vidmemAccessBitPutOffset;
} NVC763_CTRL_VIDMEM_ACCESS_BIT_PUT_OFFSET_PARAMS;
/* _ctrlc763_h_ */
