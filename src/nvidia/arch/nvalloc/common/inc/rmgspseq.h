/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * Top-level header-file that defines the GSP sequencer structures
 */

#ifndef _RMGSPSEQ_H_
#define _RMGSPSEQ_H_

/*!
 * GSP sequencer structures and defines
 *
 * Here are the supported opcodes.
 * the sequencer buffer is a stream of commands composed of
 * an op-code directly followed by the exact number of arguments it needs.
 *
 * The supported op-codes are:
 * GSP_SEQUENCER_BUFFER_OPCODE_REG_WRITE:
 *  arg[0]: the register offset
 *  arg[1]: the register value
 *
 * GSP_SEQUENCER_BUFFER_OPCODE_REG_MODIFY:
 *  arg[0]: the register offset
 *  arg[1]: the mask where to apply the modification
 *  arg[2]: the value to apply. The value needs to be shifted to fit inside the mask,
 *
 * GSP_SEQUENCER_BUFFER_OPCODE_REG_POLL:
 *  arg[0]: the register offset
 *  arg[1]: the mask where to apply the modification
 *  arg[2]: the value to apply. The value needs to be shifted to fit inside the mask.
 *  arg[3]: the timeout in MS
 *  arg[4]: an unique error code from GSP_SEQUENCER_BUFFER_ERR. Helps map to the failing GSP code.
 *
 * GSP_SEQUENCER_BUFFER_OPCODE_DELAY_US
 *   arg[0]: the delay in micoseconds.
 *
 * GSP_SEQUENCER_REG_STORE
 *  This operation stores the specified register at the specified index in
 *  the sequencer buffer register storage area.
 *   arg[0]: the reg offset to store in the save area
 *   arg[1]: index in save area to store value of reg offset
 *
 * GSP_SEQUENCER_CORE_RESET
 *  This operation resets the core.  This operation takes no arguments.
 *
 * GSP_SEQUENCER_CORE_START
 *  This operation starts the core.  This operation takes no arguments.
 *
 * GSP_SEQUENCER_CORE_WAIT_FOR_HALT
 *  This operation waits for the core to halt after completing execution.
 *  This operation takes no arguments.
 *
 * GSP_SEQUENCER_CORE_RESUME
 *  This operation resumes the core in preparation for switching back to
 *  the GSP-RM.
 */
typedef enum GSP_SEQ_BUF_OPCODE
{
    GSP_SEQ_BUF_OPCODE_REG_WRITE = 0,
    GSP_SEQ_BUF_OPCODE_REG_MODIFY,
    GSP_SEQ_BUF_OPCODE_REG_POLL,
    GSP_SEQ_BUF_OPCODE_DELAY_US,
    GSP_SEQ_BUF_OPCODE_REG_STORE,
    GSP_SEQ_BUF_OPCODE_CORE_RESET,
    GSP_SEQ_BUF_OPCODE_CORE_START,
    GSP_SEQ_BUF_OPCODE_CORE_WAIT_FOR_HALT,
    GSP_SEQ_BUF_OPCODE_CORE_RESUME,
} GSP_SEQ_BUF_OPCODE;

#define GSP_SEQUENCER_PAYLOAD_SIZE_DWORDS(opcode)                       \
    ((opcode == GSP_SEQ_BUF_OPCODE_REG_WRITE)  ? (sizeof(GSP_SEQ_BUF_PAYLOAD_REG_WRITE)  / sizeof(NvU32)) : \
     (opcode == GSP_SEQ_BUF_OPCODE_REG_MODIFY) ? (sizeof(GSP_SEQ_BUF_PAYLOAD_REG_MODIFY) / sizeof(NvU32)) : \
     (opcode == GSP_SEQ_BUF_OPCODE_REG_POLL)   ? (sizeof(GSP_SEQ_BUF_PAYLOAD_REG_POLL)   / sizeof(NvU32)) : \
     (opcode == GSP_SEQ_BUF_OPCODE_DELAY_US)   ? (sizeof(GSP_SEQ_BUF_PAYLOAD_DELAY_US)   / sizeof(NvU32)) : \
     (opcode == GSP_SEQ_BUF_OPCODE_REG_STORE)  ? (sizeof(GSP_SEQ_BUF_PAYLOAD_REG_STORE)  / sizeof(NvU32)) : \
    /* GSP_SEQ_BUF_OPCODE_CORE_RESET */                                 \
    /* GSP_SEQ_BUF_OPCODE_CORE_START */                                 \
    /* GSP_SEQ_BUF_OPCODE_CORE_WAIT_FOR_HALT */                         \
    /* GSP_SEQ_BUF_OPCODE_CORE_RESUME */                                \
    0)

// The size of the structure must be DWord aligned!
typedef struct
{
    NvU32 addr;
    NvU32 val;
} GSP_SEQ_BUF_PAYLOAD_REG_WRITE;

// The size of the structure must be DWord aligned!
typedef struct
{
    NvU32 addr;
    NvU32 mask;
    NvU32 val;
} GSP_SEQ_BUF_PAYLOAD_REG_MODIFY;

// The size of the structure must be DWord aligned!
typedef struct
{
    NvU32 addr;
    NvU32 mask;
    NvU32 val;
    NvU32 timeout;
    NvU32 error;
} GSP_SEQ_BUF_PAYLOAD_REG_POLL;

// The size of the structure must be DWord aligned!
typedef struct
{
    NvU32 val;
} GSP_SEQ_BUF_PAYLOAD_DELAY_US;

// The size of the structure must be DWord aligned!
typedef struct
{
    NvU32 addr;
    NvU32 index;
} GSP_SEQ_BUF_PAYLOAD_REG_STORE;

typedef struct GSP_SEQUENCER_BUFFER_CMD
{
    GSP_SEQ_BUF_OPCODE opCode;
    union
    {
        GSP_SEQ_BUF_PAYLOAD_REG_WRITE regWrite;
        GSP_SEQ_BUF_PAYLOAD_REG_MODIFY regModify;
        GSP_SEQ_BUF_PAYLOAD_REG_POLL regPoll;
        GSP_SEQ_BUF_PAYLOAD_DELAY_US delayUs;
        GSP_SEQ_BUF_PAYLOAD_REG_STORE regStore;
    } payload;
} GSP_SEQUENCER_BUFFER_CMD;

//
// These error codes printed by the client help us
// see to which GSP uCode line it corresponds.
//
typedef enum GSP_SEQUENCER_BUFFER_ERR
{
    GSP_SEQUENCER_BUFFER_ERR_OK = 0,
    GSP_SEQUENCER_BUFFER_ERR_TIMEOUT1,
    GSP_SEQUENCER_BUFFER_ERR_TIMEOUT2,
    GSP_SEQUENCER_BUFFER_ERR_TIMEOUT3
} GSP_SEQUENCER_BUFFER_ERR;


// Sequencer implementation of FLD_WR_DRF_DEF()
#define GSP_SEQ_FLD_WR_DRF_DEF(gpu, gsp, d, r, f, c)                \
    {                                                               \
        GSP_SEQUENCER_BUFFER_CMD cmd;                               \
        cmd.opCode                 = GSP_SEQ_BUF_OPCODE_REG_MODIFY; \
        cmd.payload.regModify.addr = NV##d##r;                      \
        cmd.payload.regModify.mask = DRF_MASK(NV##d##r##f)          \
                                     << DRF_SHIFT(NV##d##r##f);     \
        cmd.payload.regModify.val  = DRF_DEF(d, r, f, c);           \
        (void)gspAppendToSequencerBuffer(gpu, gsp, &cmd);           \
    }

//
// Sequencer implementation similar to REG_FLD_WR_DRF_DEF() but with a base
// address specified instead of an aperture.
//
#define GSP_SEQ_BASE_FLD_WR_DRF_DEF(gpu, gsp, b, d, r, f, c)        \
    {                                                               \
        GSP_SEQUENCER_BUFFER_CMD cmd;                               \
        cmd.opCode                 = GSP_SEQ_BUF_OPCODE_REG_MODIFY; \
        cmd.payload.regModify.addr = (b) + NV##d##r;                \
        cmd.payload.regModify.mask = DRF_MASK(NV##d##r##f)          \
                                     << DRF_SHIFT(NV##d##r##f);     \
        cmd.payload.regModify.val = DRF_DEF(d, r, f, c);            \
        (void)gspAppendToSequencerBuffer(gpu, gsp, &cmd);           \
    }


/*!
 * Forward references
 */
struct rpc_run_cpu_sequencer_v17_00;

/*!
 * Structure tracking all information relevant to GSP sequencer bufferfor GSP-RM
 */
typedef struct
{
    /*! Pointer to RM-GSP CPU sequencer parameter block */
    struct rpc_run_cpu_sequencer_v17_00 *pRunCpuSeqParam;
} GSP_SEQUENCER_BUFFER;

/*!
 * RM-GSP sequencer buffer register macros.
 *   GSP_SEQ_BUF_REG_SAVE_SIZE : specifies size of save area in reg values
 *   GSP_SEQ_BUF_REG_SAVE_MAILBOX0 : index for saving of mailbox0 reg
 *   GSP_SEQ_BUF_REG_SAVE_MAILBOX1 : index for saving of mailbox1 reg
 */
#define GSP_SEQ_BUF_REG_SAVE_SIZE               (8)
#define GSP_SEQ_BUF_REG_SAVE_MAILBOX0           (0)
#define GSP_SEQ_BUF_REG_SAVE_MAILBOX1           (1)

#endif // _RMGSPSEQ_H_
