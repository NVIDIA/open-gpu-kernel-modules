/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl208f/ctrl208ffbio.finn
//

#include "ctrl/ctrl208f/ctrl208fbase.h"

/*
 * NV208F_CTRL_CMD_FBIO_SETUP_TRAINING_EXP
 *
 * This command defines an fbio training experiment for later use.
 *
 * This command has the form of a 'cmd' operation descriminant
 * followed by a union populated with an operand to match the 'cmd'.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_COMMAND
 *
 */
#define NV208F_CTRL_CMD_FBIO_SETUP_TRAINING_EXP                     (0x208f0a03) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FBIO_INTERFACE_ID << 8) | NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_PARAMS_MESSAGE_ID" */

/*
 * NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_NULL
 * 
 * This command has no effect when used.
 */
#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_NULL                    0

/*
 * NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_NUMBER_OF_MOD_SLOTS
 *
 * This command defines the number of register modification slots to be used
 * in the setup phase of the pass/fail training exercise.  Using this cmd
 * causes any previously defined modification table to be cleared/released.
 * The maximum size of the table is defined by
 *     _SETUP_FBIO_TRAINING_EXP_SET_NUMBER_OF_MOD_SLOTS__MAX
 * Using the value of zero for number of mod slots is valid.  "Mod slots"
 * are defined below.
 *
 * Possible status values returned are:
 *   NV_ERR_INVALID_LIMIT
 *     if modSlots is > __MAX
 *   NV_ERR_INSUFFICIENT_RESOURCES
 *     if we're out of memory setting up the mod slot table
 */
#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_NUMBER_OF_MOD_SLOTS 1
typedef struct NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_NUMBER_OF_MOD_SLOTS_OPERANDS {
    NvU32 modSlots;
} NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_NUMBER_OF_MOD_SLOTS_OPERANDS;

#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_NUMBER_OF_MOD_SLOTS__MAX 256

/*
 * NV208F_CTRL_FBIO_SETUPFBIO_TRAINING_EXP_SET_MOD_SLOT
 *
 * This command is used to define each individual element of the the sequence
 * of operations which will be performed before running the pass/fail training
 * exercise.  Note that this cmd is multi-cmd (with a null cmd all its own,
 * etc).
 *
 * Possible status values returned are:
 *   NV_ERR_INVALID_INDEX
 *     if seq is out of range for previously set SET_NUMBER_OF_MOD_SLOTS
 *     operation.
 *   NV_ERR_INVALID_COMMAND
 *     if cmd isn't recognized
 */
#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT                 2

#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_NULL            0

/*
 * NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_SET_REGISTER
 *
 * This cmd is used to define a register transaction to be applied in
 * sequence before running the pass/fail experiment.  This is where
 * NV_PFB_TRAINING_ADR, NV_PFB_TRAINING_CMD, NV_PFB_TRAINING_DP(i),
 * NV_PFB_TRAINING_THRESHOLD and NV_PFB_TRAINING_MASK, etc. should be 
 * configured before calling back with cmd
 * NV208F_CTRL_CMD_FBIO_RUN_TRAINING_EXP.
 *
 *   reg
 *     This parameter specifies the BAR0 register offset to affect.
 *   andMask
 *   orMask
 *     These parameter specify the RMW values to be used in the following:
 *       write32(reg, (read32(reg) & andMask) | orMask)
 */
#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_SET_REGISTER    1
typedef struct NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_SET_REGISTER_OPERANDS {
    NvU32 reg;
    NvU32 andMask;
    NvU32 orMask;
} NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_SET_REGISTER_OPERANDS;

/*
 * NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_DELAY
 *
 * This cmd is used to define a delay to be applied in the setup sequence
 * before running the pass/fail experiment.
 *
 *  usec
 *    Specifies delay to be used in microseconds.
 */

#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_DELAY 2
typedef struct NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_DELAY_OPERANDS {
    NvU32 usec;
} NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_DELAY_OPERANDS;

/*
 * NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_INIT_LT
 *
 * This cmd is used to define a point where normal link training initialization
 * may be executed in the sequence before running the pass/fail experiment.  
 * In general, this is not needed since it is done during normal
 * initialization, but does allow re-initialization during the sequence.
 */
#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_INIT_LT  3

/*
 * NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_RUN_LT
 *
 * This cmd is used to define a point where normal link training may be
 * executed in the sequence before running the pass/fail experiment.
 * In general, this is not needed since it is somewhat redundant.
 */
#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_RUN_LT   4

/*
 * NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_STOP_SEQ
 *
 * This cmd is used to define a point where the sequence stops without
 * running the pass/fail experiment.  
 */
#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_STOP_SEQ 5



typedef struct NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_PARAMS {
    NvU32 cmd;
    /* C form: NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_OPERANDS op; */
    union {

        NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_SET_REGISTER_OPERANDS setRegister;

        NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_DELAY_OPERANDS        delay;
    } op;
    NvU32 seq;
} NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_PARAMS;



#define NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_PARAMS {
    NvU32 cmd;

    /* C form: NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_OPERANDS op; */
    union {

        NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_NUMBER_OF_MOD_SLOTS_OPERANDS setNumberOfModSlots;

        NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_SET_MOD_SLOT_PARAMS              setModSlot;
    } op;
} NV208F_CTRL_FBIO_SETUP_TRAINING_EXP_PARAMS;

/*
 * NV208F_CTRL_CMD_FBIO_RUN_TRAINING_EXP
 *
 * This command runs the previously defined experiment and returns status on
 * pass/fail.  NV_OK is returned in the case of both pass
 * and fail.
 *
 * The risingPasses and fallingPasses outputs represents the results across
 * all partitions and bytelanes.  Each array entry corresponds to a partition
 * and the bits within each member represent the bytelane.  So e.g.:
 * risingPasses[4] represents the rising pass results for all of partition 4's
 * byte lanes.
 *
 * The partitionsValid bitmask represents the partitions for which the results
 * in risingPasses and fallingPasses are valid (not floorswept).
 *
 * The bytelanesValid bitmask represents the bytelanes for which the results
 * are valid (available bytelanes).
 *
 * The failingDebug[] represent debug data for why (if so) a test fails.
 * This is basically NV_PFB_TRAINING_DEBUG(i)
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_TIMEOUT
 *       if hardware doesn't respond to request in a timely manner.
 *   NV_ERR_INVALID_DATA
 *       if there was some sort of very weird data corruption issue.
 */
#define NV208F_CTRL_CMD_FBIO_RUN_TRAINING_EXP          (0x208f0a04) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FBIO_INTERFACE_ID << 8) | NV208F_CTRL_FBIO_RUN_TRAINING_EXP_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FBIO_RUN_TRAINING_EXP_RESULT__SIZE 8
#define NV208F_CTRL_FBIO_RUN_TRAINING_EXP_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV208F_CTRL_FBIO_RUN_TRAINING_EXP_PARAMS {NvU32 risingPasses[NV208F_CTRL_FBIO_RUN_TRAINING_EXP_RESULT__SIZE];
    NvU32 fallingPasses[NV208F_CTRL_FBIO_RUN_TRAINING_EXP_RESULT__SIZE];
    NvU32 failingDebug[NV208F_CTRL_FBIO_RUN_TRAINING_EXP_RESULT__SIZE];
    NvU32 partitionsValid;
    NvU32 bytelanesValid;
} NV208F_CTRL_FBIO_RUN_TRAINING_EXP_PARAMS;

#define NV208F_CTRL_CMD_FBIO_GET_TRAINING_CAPS (0x208f0a05) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_DIAG_FBIO_INTERFACE_ID << 8) | NV208F_CTRL_FBIO_GET_TRAINING_CAPS_PARAMS_MESSAGE_ID" */

#define NV208F_CTRL_FBIO_GET_TRAINING_CAPS_PARAMS_MESSAGE_ID (0x5U)

typedef struct NV208F_CTRL_FBIO_GET_TRAINING_CAPS_PARAMS {
    NvU32 supported;
} NV208F_CTRL_FBIO_GET_TRAINING_CAPS_PARAMS;

// _ctrl208ffbio_h_
