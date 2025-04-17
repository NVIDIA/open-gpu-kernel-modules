/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlb06f.finn
//




/* MAXWELL_CHANNEL_GPFIFO_A control commands and parameters */

#include "ctrl/ctrlxxxx.h"
#define NVB06F_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0xB06F, NVB06F_CTRL_##cat, idx)

/* MAXWELL_CHANNEL_GPFIFO_A command categories (6bits) */
#define NVB06F_CTRL_RESERVED (0x00)
#define NVB06F_CTRL_GPFIFO   (0x01)

/*
 * NVB06F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NVB06F_CTRL_CMD_NULL (0xb06f0000) /* finn: Evaluated from "(FINN_MAXWELL_CHANNEL_GPFIFO_A_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NVB06F_CTRL_CMD_GET_ENGINE_CTX_SIZE
 *
 * This command returns the size of the engine context.
 *
 * engineID
 *   This parameter specifies the engine context size to be retrieved.
 *
 *   See the description of the NV2080_ENGINE_TYPE values in cl2080.h for more
 *   information
 *
 * size
 *   This parameter returns the size of the engine context
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NVB06F_CTRL_CMD_GET_ENGINE_CTX_SIZE (0xb06f010b) /* finn: Evaluated from "(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS_MESSAGE_ID" */

#define NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS_MESSAGE_ID (0xBU)

typedef struct NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS {
    NvU32 engineID;
    NvU32 size;
} NVB06F_CTRL_GET_ENGINE_CTX_SIZE_PARAMS;

/*
 * NVB06F_CTRL_CMD_GET_ENGINE_CTX_DATA
 *
 * This command returns the context buffer data for the given engine for vGPU motion.
 *
 * engineID
 *   This parameter specifies the engine context to be retrieved.
 *
 *   See the description of the NV2080_ENGINE_TYPE values in cl2080.h for
 *   more information
 *
 * size
 *   This parameter specifies the size of the context buffer.
 *
 * pEngineCtxBuff
 *   This parameter specifies the context buffer for motion operation to be filled in.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */

#define NVB06F_CTRL_CMD_GET_ENGINE_CTX_DATA (0xb06f010c) /* finn: Evaluated from "(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS_MESSAGE_ID" */

typedef struct SW_OBJECT_ENGINE_CTX {
    NvU32 hObject;
    NvU32 subCh;
} SW_OBJECT_ENGINE_CTX;

#define NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS_MESSAGE_ID (0xCU)

typedef struct NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS {
    NvU32 engineID;
    NvU32 size;
    NV_DECLARE_ALIGNED(NvP64 pEngineCtxBuff, 8);
} NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS;

/*
 * NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA
 *
 * This command restores the context buffer for the given engine for vGPU motion.
 *
 * engineID
 *   This parameter specifies the engine context to be restored.
 *
 *   See the description of the NV2080_ENGINE_TYPE values in cl2080.h for
 *   more information
 *
 * size
 *   This parameter specifies the size of the context buffer.
 *
 * pEngineCtxBuff
 *   This parameter specifies the context buffer for motion operation restore.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */

#define NVB06F_CTRL_CMD_MIGRATE_ENGINE_CTX_DATA (0xb06f010d) /* finn: Evaluated from "(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS_MESSAGE_ID" */

#define NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS_MESSAGE_ID (0xDU)

typedef NVB06F_CTRL_GET_ENGINE_CTX_DATA_PARAMS NVB06F_CTRL_MIGRATE_ENGINE_CTX_DATA_PARAMS;

/*
 * NVB06F_CTRL_CMD_GET_ENGINE_CTX_STATE
 *
 * This command returns the context buffer state of the given engine for vGPU motion.
 *
 * engineID
 *   This input parameter specifies the engine context to be restored.
 *
 *   See the description of the NV2080_ENGINE_TYPE values in cl2080.h for
 *   more information
 *
 * hObject
 *   This parameter specifies the channel object that is running on the SW engine.
 *
 * engineCtxState
 *   This parameter specifies the engine context state. For SW engine, the only meaningful
 *   field is INVALID, INITIALIZED and the subch.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */

#define NVB06F_CTRL_CMD_GET_ENGINE_CTX_STATE (0xb06f010e) /* finn: Evaluated from "(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS_MESSAGE_ID" */

#define NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS_MESSAGE_ID (0xEU)

typedef struct NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS {
    NvU32 engineID;
    NvU32 hObject;
    NvU32 engineCtxState;
} NVB06F_CTRL_GET_ENGINE_CTX_STATE_PARAMS;

#define NVB06F_CTRL_GET_ENGINE_CTX_VIRTUAL_CONTEXT              0:0
#define NVB06F_CTRL_GET_ENGINE_CTX_VIRTUAL_CONTEXT_DISABLED (0x00000000)
#define NVB06F_CTRL_GET_ENGINE_CTX_VIRTUAL_CONTEXT_ENABLED  (0x00000001)
#define NVB06F_CTRL_GET_ENGINE_CTX_STATE                        2:1
#define NVB06F_CTRL_GET_ENGINE_CTX_STATE_INVALID            (0x00000000)
#define NVB06F_CTRL_GET_ENGINE_CTX_STATE_INITIALIZED        (0x00000001)
#define NVB06F_CTRL_GET_ENGINE_CTX_STATE_PROMOTED           (0x00000002)
#define NVB06F_CTRL_GET_ENGINE_CTX_STATE_EVICTED            (0x00000003)
#define NVB06F_CTRL_GET_ENGINE_CTX_SUBCH                        6:4

/*
 * NVB06F_CTRL_CMD_GET_CHANNEL_HW_STATE
 *
 * This command returns the channel HW state.
 *
 * state
 *   This parameter stores single bit-fields corresponding to the following
 *   channel HW states:
 *      NEXT
 *          A value of NV_TRUE indicates that this channel should be scheduled
 *          first when GPU Host chooses this TSG to run next on the runlist.
 *
 *      CTX_RELOAD
 *          A value of NV_TRUE indicates that this channel's context was
 *          preempted and needs to be reloaded.
 *
 *      PENDING
 *          A value of NV_TRUE indicates that this channel is not loaded on the
 *          PBDMA but methods still remain. This includes the completion of
 *          semaphores acquires and WFI methods. This field is deprecated
 *          from Volta onwards, and can be ignored.
 *
 *      ENG_FAULTED
 *          A value of NV_TRUE indicates that the channel's engine has faulted,
 *          and the channel will not be rescheduled until the fault has been
 *          cleared. This bit should only be set as part of migration, and will
 *          not necessarily cause the channel to be prevented from being
 *          scheduled.
 *
 *      PBDMA_FAULTED
 *          A value of NV_TRUE indicates that the channel's PBDMA has faulted,
 *          and the channel will not be rescheduled until the fault has been
 *          cleared. This bit should only be set as part of migration, and will
 *          not necessarily cause the channel to be prevented from being
 *          scheduled.
 *
 *      ACQUIRE_FAIL
 *          A value of NV_TRUE indicates that the engine scheduler failed to
 *          acquire a semaphore for this channel.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */
#define NVB06F_CTRL_CMD_GET_CHANNEL_HW_STATE                (0xb06f010f) /* finn: Evaluated from "(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS_MESSAGE_ID" */

#define NVB06F_CTRL_CMD_CHANNEL_HW_STATE_NEXT               0:0
#define NVB06F_CTRL_CMD_CHANNEL_HW_STATE_CTX_RELOAD         1:1
#define NVB06F_CTRL_CMD_CHANNEL_HW_STATE_PENDING            2:2
#define NVB06F_CTRL_CMD_CHANNEL_HW_STATE_ENG_FAULTED        3:3
#define NVB06F_CTRL_CMD_CHANNEL_HW_STATE_PBDMA_FAULTED      4:4
#define NVB06F_CTRL_CMD_CHANNEL_HW_STATE_ACQUIRE_FAIL       5:5

typedef struct NVB06F_CTRL_CHANNEL_HW_STATE_PARAMS {
    NvU32 state;
} NVB06F_CTRL_CHANNEL_HW_STATE_PARAMS;

#define NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS_MESSAGE_ID (0xFU)

typedef NVB06F_CTRL_CHANNEL_HW_STATE_PARAMS NVB06F_CTRL_GET_CHANNEL_HW_STATE_PARAMS;

/*
 * NVB06F_CTRL_CMD_SET_CHANNEL_HW_STATE
 *
 * This command restores the channel HW state.
 *
 * state
 *   This parameter stores single bit-fields corresponding to the following
 *   channel HW states:
 *      NEXT
 *          A value of NV_TRUE indicates that this channel should be scheduled
 *          first when GPU Host chooses this TSG to run next on the runlist.
 *
 *      CTX_RELOAD
 *          A value of NV_TRUE indicates that this channel's context was
 *          preempted and needs to be reloaded.
 *
 *      PENDING
 *          A value of NV_TRUE indicates that this channel is not loaded on the
 *          PBDMA but methods still remain. This includes the completion of
 *          semaphores acquires and WFI methods. This field is deprecated
 *          from Volta onwards, and can be ignored.
 *
 *      ENG_FAULTED
 *          A value of NV_TRUE indicates that the channel's engine has faulted,
 *          and the channel will not be rescheduled until the fault has been
 *          cleared. This bit should only be set as part of migration, and will
 *          not necessarily cause the channel to be prevented from being
 *          scheduled.
 *
 *      PBDMA_FAULTED
 *          A value of NV_TRUE indicates that the channel's PBDMA has faulted,
 *          and the channel will not be rescheduled until the fault has been
 *          cleared. This bit should only be set as part of migration, and will
 *          not necessarily cause the channel to be prevented from being
 *          scheduled.
 *
 *      ACQUIRE_FAIL
 *          A value of NV_TRUE indicates that the engine scheduler failed to
 *          acquire a semaphore for this channel.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */
#define NVB06F_CTRL_CMD_SET_CHANNEL_HW_STATE (0xb06f0110) /* finn: Evaluated from "(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS_MESSAGE_ID" */

#define NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS_MESSAGE_ID (0x10U)

typedef NVB06F_CTRL_CHANNEL_HW_STATE_PARAMS NVB06F_CTRL_SET_CHANNEL_HW_STATE_PARAMS;

/*
 * NVB06F_CTRL_CMD_SAVE_ENGINE_CTX_DATA
 *
 * This command returns the context buffer data for the given engine for vGPU migration.
 *
 * engineID
 *   This parameter specifies the engine context to be retrieved.
 *
 *   See the description of the NV2080_ENGINE_TYPE values in cl2080.h for
 *   more information
 *
 * size
 *   This parameter specifies the size of the context buffer. The maximum size
 *   of engine context buffer is choosen to support only RAMFC and instance
 *   memory block. To use this RmCtrl for a higher sized buffer, like the GR
 *   context, the max size would have to be increased.
 *
 * engineCtxBuff
 *   This parameter specifies the context buffer data.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */

#define NVB06F_CTRL_CMD_SAVE_ENGINE_CTX_DATA   (0xb06f0111) /* finn: Evaluated from "(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS_MESSAGE_ID" */

#define NVB06F_CTRL_ENGINE_CTX_BUFFER_SIZE_MAX 4096

#define NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS_MESSAGE_ID (0x11U)

typedef struct NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS {
    NvU32 engineID;
    NvU32 size;
    NvU8  engineCtxBuff[NVB06F_CTRL_ENGINE_CTX_BUFFER_SIZE_MAX];
} NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS;

/*
 * NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA
 *
 * This command restores the context buffer for the given engine for vGPU migration.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 */

#define NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA (0xb06f0112) /* finn: Evaluated from "(FINN_MAXWELL_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS_MESSAGE_ID" */

typedef NVB06F_CTRL_SAVE_ENGINE_CTX_DATA_PARAMS NVB06F_CTRL_RESTORE_ENGINE_CTX_DATA_PARAMS;

// FINN PORT: The below type was generated by the FINN port to
// ensure that all API's have a unique structure associated
// with them!
#define NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS_MESSAGE_ID (0x12U)

typedef struct NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS {
    NVB06F_CTRL_RESTORE_ENGINE_CTX_DATA_PARAMS params;
} NVB06F_CTRL_CMD_RESTORE_ENGINE_CTX_DATA_FINN_PARAMS;

