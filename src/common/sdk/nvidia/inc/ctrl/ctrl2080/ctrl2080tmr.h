/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2015 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080tmr.finn
//

#include "ctrl/ctrl2080/ctrl2080base.h"

/* NV20_SUBDEVICE_TIMER related control commands and parameters */

/*
 * NV2080_CTRL_CMD_TIMER_SCHEDULE
 *
 * This command schedules a GPU timer event to fire at the specified time interval.
 * Can be called without API & GPU locks if NVOS54_FLAGS_IRQL_RAISED and
 * NVOS54_FLAGS_LOCK_BYPASS are set in NVOS54_PARAMETERS.flags
 *
 *   time_nsec
 *     This parameter specifies the time in nanoseconds at which the GPU timer
 *     event is to fire.
 *   flags
 *     This parameter determines the interpretation of the value specified by
 *     the time_nsec parameter:
 *       NV2080_CTRL_TIMER_SCHEDULE_FLAGS_TIME_ABS
 *         This flag indicates that time_nsec is in absolute time.
 *       NV2080_CTRL_TIMER_SCHEDULE_FLAGS_TIME_REL
 *         This flag indicates that time_nsec is in relative time.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_STATE
 */
#define NV2080_CTRL_CMD_TIMER_SCHEDULE (0x20800401) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_TIMER_INTERFACE_ID << 8) | NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 time_nsec, 8);
    NvU32 flags;
} NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS;

/* valid flag values */
#define NV2080_CTRL_TIMER_SCHEDULE_FLAGS_TIME               0:0
#define NV2080_CTRL_TIMER_SCHEDULE_FLAGS_TIME_ABS (0x00000000)
#define NV2080_CTRL_TIMER_SCHEDULE_FLAGS_TIME_REL (0x00000001)

/*
 * NV2080_CTRL_CMD_TIMER_CANCEL
 *
 * This command cancels any pending timer events initiated with the
 * NV2080_CTRL_CMD_TIMER_SCHEDULE command.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_TIMER_CANCEL              (0x20800402) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_TIMER_INTERFACE_ID << 8) | 0x2" */

/* 
 * NV2080_CTRL_CMD_TIMER_GET_TIME 
 * 
 * This command returns the current GPU timer value.  The current time is 
 * expressed in elapsed nanoseconds since 00:00 GMT, January 1, 1970 
 * (zero hour) with a resolution of 32 nanoseconds. 
 *
 * Can be called without API & GPU locks if NVOS54_FLAGS_IRQL_RAISED and
 * NVOS54_FLAGS_LOCK_BYPASS are set in NVOS54_PARAMETERS.flags
 * 
 * Possible status values returned are: 
 *   NV_OK 
 *   NV_ERR_INVALID_ARGUMENT 
 */
#define NV2080_CTRL_CMD_TIMER_GET_TIME            (0x20800403) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_TIMER_INTERFACE_ID << 8) | NV2080_CTRL_TIMER_GET_TIME_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_TIMER_GET_TIME_PARAMS_MESSAGE_ID (0x3U)

typedef struct NV2080_CTRL_TIMER_GET_TIME_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 time_nsec, 8);
} NV2080_CTRL_TIMER_GET_TIME_PARAMS;

/*
 * NV2080_CTRL_CMD_TIMER_GET_REGISTER_OFFSET
 *
 * The command returns the offset of the timer registers, so that clients may
 * map them directly. 
 *
 * Possible status values returned are: 
 *   NV_OK 
 */

#define NV2080_CTRL_CMD_TIMER_GET_REGISTER_OFFSET (0x20800404) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_TIMER_INTERFACE_ID << 8) | NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS_MESSAGE_ID (0x4U)

typedef struct NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS {
    NvU32 tmr_offset;
} NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS;

/*
 * NV2080_CTRL_TIMER_GPU_CPU_TIME_SAMPLE
 *
 * This structure describes the information obtained with
 * NV2080_CTRL_CMD_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO command.
 *
 *   gpuTime
 *     GPU time is the value of GPU global timer (PTIMER) with a resolution
 *     of 32 nano seconds.
 *   cpuTime
 *     CPU time. Resolution of the cpu time depends on its source. Refer to
 *     NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_*  for more information.

 */
typedef struct NV2080_CTRL_TIMER_GPU_CPU_TIME_SAMPLE {
    NV_DECLARE_ALIGNED(NvU64 cpuTime, 8);
    NV_DECLARE_ALIGNED(NvU64 gpuTime, 8);
} NV2080_CTRL_TIMER_GPU_CPU_TIME_SAMPLE;


/*
 * NV2080_CTRL_CMD_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO
 *
 * This command returns correlation information between GPU time and CPU time
 * for a given CPU clock type.
 *
 *   cpuClkId
 *     This parameter specifies the source of the CPU clock. This parameter is
 *     composed of two fields:
 *     NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_SOURCE
 *       This field specifies source ID of the CPU clock in question. Legal
 *       values for this parameter include:
 *         NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_OSTIME
 *           This clock id will provide real time in microseconds since
 *           00:00:00 UTC on January 1, 1970, as reported by the host OS.
 *         NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PLATFORM_API
 *           This clock id will provide time stamp that is constant-rate, high
 *           precision using platform API that is also available in the user
 *           mode.
 *         NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_TSC
 *           This clock id will provide time stamp using CPU's time stamp
 *           counter.
 *     NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PROCESSOR
 *       This field specifies the processor whose clock should be used for the
 *       source. The control call and cpuClkId parameter remain named for the
 *       CPU specifically for legacy reasons. Not all processors will support
 *       all clock sources. Legal values for this parameter include:
 *         NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PROCESSOR_CPU
 *           The clock information will be fulfilled by the CPU. This value
 *           is defined to be 0 so that it is the default for backwards
 *           compatibility.
 *         NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PROCESSOR_GSP
 *           The clock information will be fulfilled by the GSP.
 *
 *   sampleCount
 *     This field specifies the number of clock samples to be taken.
 *     This value cannot exceed NV2080_CTRL_TIMER_GPU_CPU_TIME_MAX_SAMPLES.
 *
 *   samples
 *     This field returns an array of requested samples. Refer to
 *     NV2080_CTRL_TIMER_GPU_CPU_TIME_SAMPLE to get details about each entry
 *     in the array.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO (0x20800406) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_TIMER_INTERFACE_ID << 8) | NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_TIMER_GPU_CPU_TIME_MAX_SAMPLES              16

#define NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS_MESSAGE_ID (0x6U)

typedef struct NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS {
    NvU8 cpuClkId;
    NvU8 sampleCount;
    NV_DECLARE_ALIGNED(NV2080_CTRL_TIMER_GPU_CPU_TIME_SAMPLE samples[NV2080_CTRL_TIMER_GPU_CPU_TIME_MAX_SAMPLES], 8);
} NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS;

#define NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_SOURCE    3:0

/* Legal NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_SOURCE values */
#define NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_OSTIME        (0x00000001)
#define NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_TSC           (0x00000002)
#define NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PLATFORM_API  (0x00000003)
#define NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_GSP_OS        (0x00000004)

#define NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PROCESSOR 7:4

/* Legal NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PROCESSOR values */
#define NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PROCESSOR_CPU (0x00000000)
#define NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PROCESSOR_GSP (0x00000001)


/*!
 * NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ
 *
 * This command changes the frequency at which Graphics Engine time stamp is
 * updated. Frequency can either be set to max or restored to default.
 * Clients can independently use this call to increase the timer frequency 
 * as a global reference count is maintained for requests to Max frequency.
 * Client is assured that the system stays in this state till the requested
 * client releases the state or is killed. Timer frequency will automatically
 * be restored to default when there is no pending request to increase.
 *
 * Note that recursive requests for the same state from the same client
 * are considered invalid.
 *
 * bSetMaxFreq
 *      Set to NV_TRUE if GR tick frequency needs to be set to Max.
 *
 * See @ref NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS for
 * documentation of parameters.
 *
 * Possible status values returned are
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_STATE_IN_USE
 *   NV_ERR_INVALID_OPERATION
 *   NV_ERR_INVALID_STATE
 */
#define NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ             (0x20800407) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_TIMER_INTERFACE_ID << 8) | NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS_MESSAGE_ID" */

/*!
 * This struct contains bSetMaxFreq flag.
 */
#define NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS_MESSAGE_ID (0x7U)

typedef struct NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS {
    NvBool bSetMaxFreq;
} NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS;

/* _ctrl2080tmr_h_ */
