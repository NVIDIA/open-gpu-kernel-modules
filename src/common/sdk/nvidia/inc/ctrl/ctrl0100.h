/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl0100.finn
//

#include "ctrl/ctrlxxxx.h"

#define NV0100_CTRL_CMD(cat,idx)          NVXXXX_CTRL_CMD(0x0100, NV0100_CTRL_##cat, idx)

/* Client command categories (6bits) */
#define NV0100_CTRL_RESERVED    (0x00U)
#define NV0100_CTRL_LOCK_STRESS (0x01U)

/*
 * NV0100_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_NULL    (0x1000000U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_RESERVED_INTERFACE_ID << 8) | 0x0" */



/*
 * NV0100_CTRL_CMD_RESET_LOCK_STRESS_STATE
 *
 * This command resets RM's lock stress counters to 0, allowing for a new lock stress
 * run to start.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_RESET_LOCK_STRESS_STATE (0x1000101U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | 0x1" */

/* 
 * Bit fields to indicate to the caller whether an action was performed on a counter 
 * Setting any of these fields to 1 means to increment/decrement the respective counter.
 * Setting any of these fields to 0 means to ignore the respective counter.
 */
#define NV0100_CTRL_GLOBAL_RMAPI_LOCK_STRESS_COUNTER_ACTION       0:0
#define NV0100_CTRL_GPU_LOCK_STRESS_COUNTER_ACTION                1:1
#define NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_ACTION             2:2
#define NV0100_CTRL_INTERNAL_CLIENT_LOCK_STRESS_COUNTER_ACTION    3:3
#define NV0100_CTRL_ALL_LOCK_STRESS_COUNTER_ACTION                3:0

/*
 * Bit fields to indicate to the caller what type of action was performed on a counter
 * Setting any of these fields to 1 means to increment the respective counter.
 * Setting any of these fields to 0 means to decrement the respective counter.
 */
#define NV0100_CTRL_GLOBAL_RMAPI_LOCK_STRESS_COUNTER_INCREMENT    4:4
#define NV0100_CTRL_GPU_LOCK_STRESS_COUNTER_INCREMENT             5:5
#define NV0100_CTRL_CLIENT_LOCK_STRESS_COUNTER_INCREMENT          6:6
#define NV0100_CTRL_INTERNAL_CLIENT_LOCK_STRESS_COUNTER_INCREMENT 7:7
#define NV0100_CTRL_ALL_LOCK_STRESS_COUNTER_INCREMENT             7:4

typedef struct NV0100_CTRL_LOCK_STRESS_OUTPUT {
    NvU8 action;
} NV0100_CTRL_LOCK_STRESS_OUTPUT;

/*
 * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_ALL_RM_LOCKS
 *
 * This command does a random increment/decrement on global counters in RM and reports
 * the operation performed back to the caller. This is done with all RM locks held.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_ALL_RM_LOCKS (0x1000102U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS_MESSAGE_ID" */

#define NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS_MESSAGE_ID (0x2U)

typedef NV0100_CTRL_LOCK_STRESS_OUTPUT NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS;

/*
 * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK
 *
 * This command does a random increment/decrement on global counters in RM and reports
 * the operation performed back to the caller. This is done with all RM locks held except
 * for the GPU locks.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK (0x1000103U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS_MESSAGE_ID" */

#define NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS_MESSAGE_ID (0x3U)

typedef NV0100_CTRL_LOCK_STRESS_OUTPUT NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS;

/*
 * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE
 *
 * This command does a random increment/decrement on global counters in RM and reports
 * the operation performed back to the caller. This is done with the API lock held in
 * read mode and GPU locks held.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE (0x1000104U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID" */

#define NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID (0x4U)

typedef NV0100_CTRL_LOCK_STRESS_OUTPUT NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS;

/*
 * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE
 *
 * This command does a random increment/decrement on global counters in RM and reports
 * the operation performed back to the caller. This is done with the API lock held in
 * read mode.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE (0x1000105U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID" */

#define NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID (0x5U)

typedef NV0100_CTRL_LOCK_STRESS_OUTPUT NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS;

/*
 * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS
 *
 * This command does a random increment/decrement on global counters in RM and reports
 * the operation performed back to the caller. This is done with all RM locks held in the
 * internal RM API path.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS (0x1000106U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS_MESSAGE_ID" */

#define NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS_MESSAGE_ID (0x6U)

typedef NV0100_CTRL_LOCK_STRESS_OUTPUT NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS;

/*
 * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK
 *
 * This command does a random increment/decrement on global counters in RM and reports
 * the operation performed back to the caller. This is done with all RM locks held except
 * for the GPU locks in the internal RM API path.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK (0x1000107U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS_MESSAGE_ID" */

#define NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS_MESSAGE_ID (0x7U)

typedef NV0100_CTRL_LOCK_STRESS_OUTPUT NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS;

/*
 * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE
 *
 * This command does a random increment/decrement on global counters in RM and reports
 * the operation performed back to the caller. This is done with the API lock taken in
 * read mode and GPU locks held in the internal RM API path.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE (0x1000108U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID" */

#define NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID (0x8U)

typedef NV0100_CTRL_LOCK_STRESS_OUTPUT NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS;

/*
 * NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE
 *
 * This command does a random increment/decrement on global counters in RM and reports
 * the operation performed back to the caller. This is done with the API lock held in read
 * mode in the internal RM API path.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE (0x1000109U) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID" */

#define NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS_MESSAGE_ID (0x9U)

typedef NV0100_CTRL_LOCK_STRESS_OUTPUT NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS;

/*
 * NV0100_CTRL_CMD_GET_LOCK_STRESS_COUNTERS
 *
 * This command gets the value of the global lock stress counters in RM at the end of
 * the lock stress test.
 *
 * Possible status values returned are:
 *    NV_OK
 */
#define NV0100_CTRL_CMD_GET_LOCK_STRESS_COUNTERS (0x100010aU) /* finn: Evaluated from "(FINN_LOCK_STRESS_OBJECT_LOCK_STRESS_INTERFACE_ID << 8) | NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS_MESSAGE_ID" */

#define NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS {
    NvS32 globalLockStressCounter;
    NvS32 gpuLockStressCounter;
    NvS32 clientLockStressCounter;
    NvS32 internalClientLockStressCounter;
} NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS;

