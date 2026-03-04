/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlb069.finn
//

#include "ctrl/ctrlxxxx.h"
/* MAXWELL_FAULT_BUFFER_A control commands and parameters */

#define NVB069_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0xB069, NVB069_CTRL_##cat, idx)

/* MAXWELL_FAULT_BUFFER_A command categories (6bits) */
#define NVB069_CTRL_RESERVED    (0x00)
#define NVB069_CTRL_FAULTBUFFER (0x01)

/*
 * NVB069_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVB069_CTRL_CMD_NULL    (0xb0690000) /* finn: Evaluated from "(FINN_MAXWELL_FAULT_BUFFER_A_RESERVED_INTERFACE_ID << 8) | 0x0" */






/*
 * NVB069_CTRL_CMD_FAULTBUFFER_READ_GET
 *
 * This command returns the current HW GET pointer for the requested type fault buffer
 *
 *    faultBufferGetOffset
 *      Value of current HW GET pointer
 *    faultBufferType
 *      Type of fault buffer. FAULT_BUFFER_REPLAYABLE or FAULT_BUFFER_NON_REPLAYABLE
 */
#define NVB069_CTRL_CMD_FAULTBUFFER_READ_GET (0xb0690101) /* finn: Evaluated from "(FINN_MAXWELL_FAULT_BUFFER_A_FAULTBUFFER_INTERFACE_ID << 8) | NVB069_CTRL_FAULTBUFFER_READ_GET_PARAMS_MESSAGE_ID" */

#define NVB069_CTRL_FAULTBUFFER_READ_GET_PARAMS_MESSAGE_ID (0x1U)

typedef struct NVB069_CTRL_FAULTBUFFER_READ_GET_PARAMS {
    NvU32 faultBufferGetOffset;
    NvU32 faultBufferType;
} NVB069_CTRL_FAULTBUFFER_READ_GET_PARAMS;

//
// Valid Fault buffer Types
// NON_REPLAYABLE is only supported in Volta+ GPUs.
//
#define NVB069_CTRL_FAULT_BUFFER_NON_REPLAYABLE (0x00000000)
#define NVB069_CTRL_FAULT_BUFFER_REPLAYABLE     (0x00000001)

/*
 * NVB069_CTRL_CMD_FAULTBUFFER_WRITE_GET
 *
 * This command writes the HW GET pointer for the requested type of fault buffer
 *
 * NOTE: The caller must issue a write barrier before this function to
 * ensure modifications to the current buffer entry are committed before
 * the GET pointer is updated.
 * 
 *    faultBufferGetOffset
 *      Value to be written to HW GET pointer
 *    faultBufferType
 *      Type of fault buffer. FAULT_BUFFER_REPLAYABLE or FAULT_BUFFER_NON_REPLAYABLE
 */
#define NVB069_CTRL_CMD_FAULTBUFFER_WRITE_GET   (0xb0690102) /* finn: Evaluated from "(FINN_MAXWELL_FAULT_BUFFER_A_FAULTBUFFER_INTERFACE_ID << 8) | NVB069_CTRL_FAULTBUFFER_WRITE_GET_PARAMS_MESSAGE_ID" */

#define NVB069_CTRL_FAULTBUFFER_WRITE_GET_PARAMS_MESSAGE_ID (0x2U)

typedef struct NVB069_CTRL_FAULTBUFFER_WRITE_GET_PARAMS {
    NvU32 faultBufferGetOffset;
    NvU32 faultBufferType;
} NVB069_CTRL_FAULTBUFFER_WRITE_GET_PARAMS;

/*
 * NVB069_CTRL_CMD_FAULTBUFFER_READ_PUT
 *
 * This command returns the current HW PUT pointer for the requested type fault buffer
 *
 *    faultBufferGetOffset
 *      Value of current HW PUT pointer
 *    faultBufferType
 *      Type of fault buffer. FAULT_BUFFER_REPLAYABLE or FAULT_BUFFER_NON_REPLAYABLE
 */
#define NVB069_CTRL_CMD_FAULTBUFFER_READ_PUT (0xb0690103) /* finn: Evaluated from "(FINN_MAXWELL_FAULT_BUFFER_A_FAULTBUFFER_INTERFACE_ID << 8) | NVB069_CTRL_FAULTBUFFER_READ_PUT_PARAMS_MESSAGE_ID" */

#define NVB069_CTRL_FAULTBUFFER_READ_PUT_PARAMS_MESSAGE_ID (0x3U)

typedef struct NVB069_CTRL_FAULTBUFFER_READ_PUT_PARAMS {
    NvU32 faultBufferPutOffset;
    NvU32 faultBufferType;
} NVB069_CTRL_FAULTBUFFER_READ_PUT_PARAMS;

#define NVB069_CTRL_CMD_FAULTBUFFER_ENABLE_NOTIFICATION (0xb0690104) /* finn: Evaluated from "(FINN_MAXWELL_FAULT_BUFFER_A_FAULTBUFFER_INTERFACE_ID << 8) | NVB069_CTRL_FAULTBUFFER_ENABLE_NOTIFICATION_PARAMS_MESSAGE_ID" */

#define NVB069_CTRL_FAULTBUFFER_ENABLE_NOTIFICATION_PARAMS_MESSAGE_ID (0x4U)

typedef struct NVB069_CTRL_FAULTBUFFER_ENABLE_NOTIFICATION_PARAMS {
    NvBool Enable;
} NVB069_CTRL_FAULTBUFFER_ENABLE_NOTIFICATION_PARAMS;

#define NVB069_CTRL_CMD_FAULTBUFFER_GET_SIZE (0xb0690105) /* finn: Evaluated from "(FINN_MAXWELL_FAULT_BUFFER_A_FAULTBUFFER_INTERFACE_ID << 8) | NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS_MESSAGE_ID" */

#define NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS_MESSAGE_ID (0x5U)

typedef struct NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS {
    NvU32 faultBufferSize;
} NVB069_CTRL_FAULTBUFFER_GET_SIZE_PARAMS;


/*
 * NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS
 *
 * This command provides kernel mapping to a few registers.
 * These mappings are needed by UVM driver to handle non fatal gpu faults
 *
 *    pFaultBufferGet
 *      Mapping for fault buffer's get pointer (NV_PFIFO_REPLAYABLE_FAULT_BUFFER_GET)
 *    pFaultBufferPut
 *      Mapping for fault buffer's put pointer (NV_PFIFO_REPLAYABLE_FAULT_BUFFER_PUT)
 *    pFaultBufferInfo
 *      Mapping for fault buffer's Info pointer (NV_PFIFO_REPLAYABLE_FAULT_BUFFER_INFO)
 *      Note: this variable is deprecated since buffer overflow is not a seperate register from Volta
 *    pPmcIntr
 *      Mapping for PMC intr register (NV_PMC_INTR(0))
 *    pPmcIntrEnSet
 *      Mapping for PMC intr set register - used to enable an intr (NV_PMC_INTR_EN_SET(0))
 *    pPmcIntrEnClear
 *      Mapping for PMC intr clear register - used to disable an intr (NV_PMC_INTR_EN_CLEAR(0))
 *    replayableFaultMask
 *      Mask for the replayable fault bit(NV_PMC_INTR_REPLAYABLE_FAULT)
 *    faultBufferType
 *      This is an input param denoting replayable/non-replayable fault buffer
 */
#define NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS (0xb0690106) /* finn: Evaluated from "(FINN_MAXWELL_FAULT_BUFFER_A_FAULTBUFFER_INTERFACE_ID << 8) | NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS_MESSAGE_ID" */

#define NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS_MESSAGE_ID (0x6U)

typedef struct NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS {
    NV_DECLARE_ALIGNED(NvP64 pFaultBufferGet, 8);
    NV_DECLARE_ALIGNED(NvP64 pFaultBufferPut, 8);
    NV_DECLARE_ALIGNED(NvP64 pFaultBufferInfo, 8);
    NV_DECLARE_ALIGNED(NvP64 pPmcIntr, 8);
    NV_DECLARE_ALIGNED(NvP64 pPmcIntrEnSet, 8);
    NV_DECLARE_ALIGNED(NvP64 pPmcIntrEnClear, 8);
    NvU32 replayableFaultMask;
    NV_DECLARE_ALIGNED(NvP64 pPrefetchCtrl, 8);
    NvU32 faultBufferType;
} NVB069_CTRL_CMD_FAULTBUFFER_GET_REGISTER_MAPPINGS_PARAMS;

/* _ctrlb069_h_ */
