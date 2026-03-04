/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      class/cl00da.finn
//



/*
* NV_SEMAPHORE_SURFACE
*   A semaphore surface object contains a GPU-and-CPU-accessible memory region
*   containing semaphores, associated monitored fence values, and any other
*   related data necessary to implement a circular 32-bit or monotonic 64-bit
*   incrementing semaphore primitive and associated event delivery.
*/
#define NV_SEMAPHORE_SURFACE (0xdaU) /* finn: Evaluated from "NV_SEMAPHORE_SURFACE_ALLOC_PARAMETERS_MESSAGE_ID" */

 /*
  * NV_SEMAPHORE_SURFACE_WAIT_VALUE
  *   The notification index used when registering events with the RM event
  *   subsystem.  RM clients should not need to use this value, as they don't
  *   allocate events against this class themselves.  Rather, they specify an
  *   OS event when registering a CPU waiter, and semaphore surface takes care
  *   of creating event objects internally as necessary, similar to the event
  *   buffer record notification OS event mechanism.
  */
#define NV_SEMAPHORE_SURFACE_WAIT_VALUE             (0x00000000)

/*
* NV_SEMAPHORE_SURFACE_ALLOC_PARAMETERS
*
*   hSemaphoreMem [IN]
*       The handle to the memory used for the semaphore value and, when
*       supported, the monitored fence/conditional interrupt value. Must be
*       accessible by the GPU corresponding to the parent of the semaphore
*       surface, as well as the CPU.
*
*   hMaxSubmittedMem [IN]
*       The handle to the memory used to track the maximum signal value
*       submitted to the GPU for processing for a given semaphore slot in
*       hSemaphoreMem. This memory is only accessed by the CPU, but must
*       support a CPU mapping that allows the use of 64-bit atomic exchange
*       operations. This may be the same memory object as hSemaphoreMem if it is
*       possible to create one memory object that supports all the requirements
*       for a given GPU and CPU. This handle may be omitted/set to zero on GPUs
*       that do not require maximum submitted value tracking.
*
*   flags [IN]
*       Flags affecting the semaphore surface allocation. Currently, there are
*       not valid flags, so this value must be set to zero.
*/
#define NV_SEMAPHORE_SURFACE_ALLOC_PARAMETERS_MESSAGE_ID (0x00DAU)

typedef struct NV_SEMAPHORE_SURFACE_ALLOC_PARAMETERS {
    NvHandle hSemaphoreMem;
    NvHandle hMaxSubmittedMem;
    NV_DECLARE_ALIGNED(NvU64 flags, 8);
} NV_SEMAPHORE_SURFACE_ALLOC_PARAMETERS;

