 /*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef KERN_GPU_POWER_H
#define KERN_GPU_POWER_H

#include "ctrl/ctrl2080/ctrl2080power.h" // NV2080_CTRL_GC6_FLAVOR_ID_MAX
#include "diagnostics/profiler.h"

typedef enum
{
    GPU_GC6_STATE_POWERED_ON = 0                        ,
    GPU_GC6_STATE_EXITED     = GPU_GC6_STATE_POWERED_ON ,
    GPU_GC6_STATE_ENTERING                              ,
    GPU_GC6_STATE_ENTERING_FAILED                       ,
    GPU_GC6_STATE_ENTERED                               ,
    GPU_GC6_STATE_EXITING                               ,
    GPU_GC6_STATE_EXITING_FAILED                        ,
} GPU_GC6_STATE;

// TODO-SC use mask for the bool variables
typedef struct
{
    NvU32               refCount;
    NvU16               GC6PerstDelay;          // waiting time for Upstream Port of GPU,
                                                // before asserting perst# signal,
                                                // during RTD3/GC6 Entry.
    NvU16               GC6TotalBoardPower;     // Power required by GPU to sustain RTD3/GC6.
    GPU_GC6_STATE       currentState;
    NvU32               executedStepMask;       // step mask executed during entry sequence
    NvU32               stepMask[NV2080_CTRL_GC6_FLAVOR_ID_MAX];  // step mask cache
} _GPU_GC6_STATE;

// Macros for GPU_GC6_STATE
#define IS_GPU_GC6_STATE_POWERED_ON(obj) (obj->gc6State.currentState == GPU_GC6_STATE_POWERED_ON)
#define IS_GPU_GC6_STATE_EXITED(obj)     (obj->gc6State.currentState == GPU_GC6_STATE_EXITED)
#define IS_GPU_GC6_STATE_ENTERING(obj)   (obj->gc6State.currentState == GPU_GC6_STATE_ENTERING)
#define IS_GPU_GC6_STATE_ENTERED(obj)    (obj->gc6State.currentState == GPU_GC6_STATE_ENTERED)
#define IS_GPU_GC6_STATE_EXITING(obj)    (obj->gc6State.currentState == GPU_GC6_STATE_EXITING)

#define SET_GPU_GC6_STATE(obj, state)    (obj->gc6State.currentState = state)
#define SET_GPU_GC6_STATE_AND_LOG(obj, state)                         \
    do {                                                              \
        SET_GPU_GC6_STATE(obj, state);                                \
        RMTRACE_GPU(_GC6_STATE, obj->gpuId, state, 0, 0, 0, 0, 0, 0); \
    } while(0)

//Macro to check is a given GC6 step id is set
#define GPU_IS_GC6_STEP_ID_SET(stepId, stepMask)                      \
        ((NVBIT(NV2080_CTRL_GC6_STEP_ID_##stepId) & (stepMask)) != 0)


#endif // KERN_GPU_POWER_H
