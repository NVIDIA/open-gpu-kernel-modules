/*
 * SPDX-FileCopyrightText: Copyright (c) 2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc365.finn
//

#include "ctrl/ctrlxxxx.h"
#define NVC365_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0xC365, NVC365_CTRL_##cat, idx)


#define NVC365_CTRL_RESERVED           (0x00)
#define NVC365_CTRL_ACCESS_CNTR_BUFFER (0x01)


/*
 * NVC365_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVC365_CTRL_CMD_NULL           (0xc3650000) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_RESERVED_INTERFACE_ID << 8) | 0x0" */






/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_READ_GET
 *
 * This command provides the value of the GET register 
 *
 *    accessCntrBufferGetOffset [OUT]
 *       This parameter returns the value of the GET register
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_READ_GET (0xc3650101) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS {
    NvU32 accessCntrBufferGetOffset;
} NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_GET_PARAMS;


/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_WRITE_GET
 *
 * This command writes a value into the GET register 
 *
 *    accessCntrBufferGetValue [IN]
 *       This parameter specifies the new value of the GET register
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_WRITE_GET (0xc3650102) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS {
    NvU32 accessCntrBufferGetValue;
} NVC365_CTRL_ACCESS_CNTR_BUFFER_WRITE_GET_PARAMS;


/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_READ_PUT
 *
 * This command provides the value of the PUT register 
 *
 *    accessCntrBufferPutOffset [OUT]
 *       This parameter returns the value of the PUT register
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_READ_PUT (0xc3650103) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS {
    NvU32 accessCntrBufferPutOffset;
} NVC365_CTRL_ACCESS_CNTR_BUFFER_READ_PUT_PARAMS;


/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_ENABLE
 *
 * This command enables/disables the access counters 
 * It also sets up RM to either service or ignore the Access Counter interrupts.
 *
 *    intrOwnership [IN]
 *       This parameter specifies whether RM should own the interrupt upon return
 *    enable [IN]
 *       NV_TRUE  = Access counters will be enabled
 *       NV_FALSE = Access counters will be disabled
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_ENABLE (0xc3650104) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS {
    NvU32  intrOwnership;
    NvBool enable;
} NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_PARAMS;

#define NVC365_CTRL_ACCESS_COUNTER_INTERRUPT_OWNERSHIP_NO_CHANGE (0x0)
#define NVC365_CTRL_ACCESS_COUNTER_INTERRUPT_OWNERSHIP_RM        (0x1)
#define NVC365_CTRL_ACCESS_COUNTER_INTERRUPT_OWNERSHIP_NOT_RM    (0x2)

/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_GET_SIZE
 *
 * This command provides the size of the notification buffer
 *
 *    accessCntrBufferSize [OUT]
 *       This parameter returns the size of the notification buffer
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_GET_SIZE              (0xc3650105) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS_MESSAGE_ID (0x5U)

typedef struct NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS {
    NvU32 accessCntrBufferSize;
} NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_SIZE_PARAMS;


/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS
 *
 * This command provides the access counter register mappings
 *
 *    pAccessCntrBufferGet [OUT]
 *       This parameter returns the pointer to the GET register
 *    pAccessCntrBufferPut [OUT]
 *       This parameter returns the pointer to the PUT register
 *    pAccessCntrlBufferFull [OUT]
 *       This parameter returns the pointer to the FULL register 
 *    pHubIntr [OUT]
 *       This parameter returns the pointer to the hub interrupt register
 *    pHubIntrEnSet [OUT]
 *       This parameter returns the pointer to the set register
 *    pHubIntrEnClear [OUT]
 *       This parameter returns the pointer to the clear register
 *    accessCntrMask [OUT]
 *       This parameter returns the interrupt mask
 * 
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS (0xc3650106) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS_MESSAGE_ID (0x6U)

typedef struct NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pAccessCntrBufferGet, 8);
    NV_DECLARE_ALIGNED(NvP64 pAccessCntrBufferPut, 8);
    NV_DECLARE_ALIGNED(NvP64 pAccessCntrBufferFull, 8);
    NV_DECLARE_ALIGNED(NvP64 pHubIntr, 8);
    NV_DECLARE_ALIGNED(NvP64 pHubIntrEnSet, 8);
    NV_DECLARE_ALIGNED(NvP64 pHubIntrEnClear, 8);
    NvU32 accessCntrMask;
} NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_REGISTER_MAPPINGS_PARAMS;


/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_GET_FULL_INFO
 *
 * This command gives information whether the buffer is full
 *
 *    fullFlag [OUT]
 *       This parameter specifies whether the buffer is full
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_GET_FULL_INFO (0xc3650107) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS_MESSAGE_ID (0x7U)

typedef struct NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS {
    NvBool fullFlag;
} NVC365_CTRL_ACCESS_CNTR_BUFFER_GET_FULL_INFO_PARAMS;


/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_RESET_COUNTERS
 *
 * This command resets access counters of specified type
 *
 *    resetFlag [OUT]
 *       This parameter specifies that counters have been reset
 *    counterType [IN]
 *       This parameter specifies the type of counters that should be reset (MIMC, MOMC or ALL)  
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_RESET_COUNTERS (0xc3650108) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS_MESSAGE_ID (0x8U)

typedef struct NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS {
    NvBool resetFlag;
    NvU32  counterType;
} NVC365_CTRL_ACCESS_CNTR_BUFFER_RESET_COUNTERS_PARAMS;

#define NVC365_CTRL_ACCESS_COUNTER_TYPE_MIMC   (0x0)
#define NVC365_CTRL_ACCESS_COUNTER_TYPE_MOMC   (0x1)
#define NVC365_CTRL_ACCESS_COUNTER_TYPE_ALL    (0x2)

/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_SET_CONFIG
 *
 * This command configures the access counters
 *
 *    mimcGranularity [IN]
 *       This parameter specifies the desired granularity for mimc (64K, 2M, 16M, 16G)
 *    momcGranularity [IN]
 *       This parameter specifies the desired granularity for momc (64K, 2M, 16M, 16G)
 *    mimcLimit [IN]
 *       This parameter specifies mimc limit (none, qtr, half, full)
 *    momcLimit [IN]
 *       This parameter specifies momc limit (none, qtr, half, full)
 *    threshold [IN]
 *       This parameter specifies the threshold
 *    flag [IN]
 *       This parameter is a bitmask denoting what configurations should be made
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_SET_CONFIG (0xc3650109) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS_MESSAGE_ID (0x9U)

typedef struct NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS {
    NvU32 mimcGranularity;
    NvU32 momcGranularity;
    NvU32 mimcLimit;
    NvU32 momcLimit;
    NvU32 threshold;
    NvU32 cmd;
} NVC365_CTRL_ACCESS_CNTR_SET_CONFIG_PARAMS;

#define NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_64K      (0x0)
#define NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_2M       (0x1)
#define NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16M      (0x2)
#define NVC365_CTRL_ACCESS_COUNTER_GRANULARITY_16G      (0x3)

#define NVC365_CTRL_ACCESS_COUNTER_MIMC_LIMIT           (0x0)
#define NVC365_CTRL_ACCESS_COUNTER_MOMC_LIMIT           (0x1)

#define NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_NONE       (0x0)
#define NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_QTR        (0x1)
#define NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_HALF       (0x2)
#define NVC365_CTRL_ACCESS_COUNTER_USE_LIMIT_FULL       (0x3)

#define NVC365_CTRL_ACCESS_COUNTER_SET_MIMC_GRANULARITY (0x1)
#define NVC365_CTRL_ACCESS_COUNTER_SET_MOMC_GRANULARITY (0x2)
#define NVC365_CTRL_ACCESS_COUNTER_SET_MIMC_LIMIT       (0x4)
#define NVC365_CTRL_ACCESS_COUNTER_SET_MOMC_LIMIT       (0x8)
#define NVC365_CTRL_ACCESS_COUNTER_SET_THRESHOLD        (0x10)

/*
 * NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_ENABLE_INTR
 *
 * This command enables the access counters interrupts 
 *
 *    enable [OUT]
 *       This parameter specifies that the access counters interrupts are enabled
 *
 * Possible status values returned are:
 *   NV_OK
 */

#define NVC365_CTRL_CMD_ACCESS_CNTR_BUFFER_ENABLE_INTR  (0xc365010b) /* finn: Evaluated from "(FINN_ACCESS_COUNTER_NOTIFY_BUFFER_ACCESS_CNTR_BUFFER_INTERFACE_ID << 8) | NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS_MESSAGE_ID" */

#define NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS_MESSAGE_ID (0xBU)

typedef struct NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS {
    NvBool enable;
} NVC365_CTRL_ACCESS_CNTR_BUFFER_ENABLE_INTR_PARAMS;
/* _ctrlc365_h_ */
