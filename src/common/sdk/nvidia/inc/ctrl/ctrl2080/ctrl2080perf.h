/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file: ctrl/ctrl2080/ctrl2080perf.finn
//

#include "nvfixedtypes.h"
#include "ctrl/ctrl2080/ctrl2080base.h"

#define NV_SUBPROC_NAME_MAX_LENGTH                    100

#include "nvmisc.h"

#include "ctrl/ctrl2080/ctrl2080clk.h"
#include "ctrl/ctrl2080/ctrl2080gpumon.h"
#include "ctrl/ctrl2080/ctrl2080volt.h"
#include "ctrl/ctrl2080/ctrl2080vfe.h"
#include "ctrl/ctrl2080/ctrl2080pmumon.h"
#include "ctrl/ctrl0080/ctrl0080perf.h"
//
// XAPICHK/XAPI_TEST chokes on the "static NVINLINE" defines in nvmisc.h.
// However, we don't need any of those definitions for those tests (XAPICHK is a
// syntactical check, not a functional test).  So, instead, just #define out the
// macros referenced below.
//

/*
 * NV2080_CTRL_CMD_PERF_BOOST
 *
 * This command can be used to boost P-State up one level or to the highest for a limited
 * duration for the associated subdevice. Boosts from different clients are being tracked
 * independently. Note that there are other factors that can limit P-States so the resulting
 * P-State may differ from expectation.
 *
 *   flags
 *     This parameter specifies the actual command. _CLEAR is to clear existing boost.
 *     _BOOST_1LEVEL is to boost P-State one level higher. _BOOST_TO_MAX is to boost
 *     to the highest P-State.
 *   duration
 *     This parameter specifies the duration of the boost in seconds. This has to be less
 *     than NV2080_CTRL_PERF_BOOST_DURATION_MAX.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */


#define NV2080_CTRL_PERF_BOOST_FLAGS_CMD                1:0
#define NV2080_CTRL_PERF_BOOST_FLAGS_CMD_CLEAR        (0x00000000)
#define NV2080_CTRL_PERF_BOOST_FLAGS_CMD_BOOST_1LEVEL (0x00000001)
#define NV2080_CTRL_PERF_BOOST_FLAGS_CMD_BOOST_TO_MAX (0x00000002)

#define NV2080_CTRL_PERF_BOOST_FLAGS_CUDA               4:4
#define NV2080_CTRL_PERF_BOOST_FLAGS_CUDA_NO          (0x00000000)
#define NV2080_CTRL_PERF_BOOST_FLAGS_CUDA_YES         (0x00000001)

#define NV2080_CTRL_PERF_BOOST_FLAGS_ASYNC              5:5
#define NV2080_CTRL_PERF_BOOST_FLAGS_ASYNC_NO         (0x00000000)
#define NV2080_CTRL_PERF_BOOST_FLAGS_ASYNC_YES        (0x00000001)

#define NV2080_CTRL_PERF_BOOST_DURATION_MAX           3600 //The duration can be specified up to 1 hour
#define NV2080_CTRL_PERF_BOOST_DURATION_INFINITE      0xffffffff // If set this way, the boost will last until cleared.

#define NV2080_CTRL_CMD_PERF_BOOST                    (0x2080200a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_BOOST_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_BOOST_PARAMS_MESSAGE_ID (0xAU)

typedef struct NV2080_CTRL_PERF_BOOST_PARAMS {
    NvU32 flags;
    NvU32 duration;
} NV2080_CTRL_PERF_BOOST_PARAMS;

/*
 * NV2080_CTRL_CMD_PERF_RESERVE_PERFMON_HW
 *
 *  This command reserves HW Performance Monitoring capabilities for exclusive
 *  use by the requester.  If the HW Performance Monitoring capabilities are
 *  currently in use then NVOS_STATUS_ERROR_STATE_IN_USE is returned.
 *
 *   bAcquire
 *     When set to TRUE this parameter indicates that the client wants to
 *     acquire the Performance Monitoring capabilities on the subdevice.
 *     When set to FALSE this parameter releases the Performance Monitoring
 *     capabilities on the subdevice.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NVOS_STATUS_ERROR_STATE_IN_USE
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_PERF_RESERVE_PERFMON_HW (0x20802093) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS_MESSAGE_ID (0x93U)

typedef struct NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS {
    NvBool bAcquire;
} NV2080_CTRL_PERF_RESERVE_PERFMON_HW_PARAMS;

/*!
 * Enumeration of the RATED_TDP arbitration clients which make requests to force
 * enable/disable VF points above the RATED_TDP point.
 *
 * These clients are sorted in descending priority - the RM will arbitrate
 * between all clients in order of priority, taking as output the first client
 * whose input action != @ref NV2080_CTRL_PERF_RATED_TDP_ACTION_DEFAULT.
 */
typedef enum NV2080_CTRL_PERF_RATED_TDP_CLIENT {
    /*!
     * Internal RM client corresponding to the RM's internal state and features.
     * The RM client will either allow default behavior (@ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_DEFAULT) or will limit to RATED_TDP
     * (@ref NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_LIMIT) when no power
     * controllers are active.
     */
    NV2080_CTRL_PERF_RATED_TDP_CLIENT_RM = 0,
    /*!
     * This Client is specifically for Bug 1785342 where we need to limit the TDP
     * to Min value on boot. And clear the Max TDP limit.
     */
    NV2080_CTRL_PERF_RATED_TDP_CLIENT_WAR_BUG_1785342 = 1,
    /*!
     * Global client request.  This client is expected to be used by a global
     * switch functionality in an end-user tool, such as EVGA Precision, to
     * either force enabling boost above RATED_TDP (@ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_EXCEED) or to force limiting to
     * RATED_TDP (@ref NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_LIMIT) across the
     * board, regardless of any app-profie settings.
     */
    NV2080_CTRL_PERF_RATED_TDP_CLIENT_GLOBAL = 2,
    /*!
     * Operating system request.  This client is expected to be used by the
     * operating system to set @ref NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_LOCK
     * for performance profiling.
     */
    NV2080_CTRL_PERF_RATED_TDP_CLIENT_OS = 3,
    /*!
     * App profile client requests.  This client is expected to be used by the
     * app-profile settings to either default to whatever was requested by
     * higher-priority clients (@ref NV2080_CTRL_PERF_RATED_TDP_ACTION_DEFAULT)
     * or to limit to RATED_TDP (@ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_LIMIT) for apps which have shown
     * bad behavior when boosting.
     */
    NV2080_CTRL_PERF_RATED_TDP_CLIENT_PROFILE = 4,
    /*!
     * Number of supported clients.
     *
     * @Note MUST ALWAYS BE LAST!
     */
    NV2080_CTRL_PERF_RATED_TDP_CLIENT_NUM_CLIENTS = 5,
} NV2080_CTRL_PERF_RATED_TDP_CLIENT;

/*!
 * Enumeration RATED_TDP actions - these are the requested actions clients can
 * make to change the behavior of the RATED_TDP functionality.
 */
typedef enum NV2080_CTRL_PERF_RATED_TDP_ACTION {
    /*!
     * The default action - meaning no explicit request from the client other
     * than to take the default behavior (allowing boosting above RATED_TDP) or
     * any explicit actions from lower priority clients.
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION_DEFAULT = 0,
    /*!
     * Force allow boosting above RATED_TDP - this action explicitly requests
     * boosting above RATED_TDP, preventing lower priority clients to limit to
     * RATED_TDP.
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_EXCEED = 1,
    /*!
     * Force to limit above RATED_TDP - this action explicitly requests to limit
     * to RATED_TDP.  This is the opposite of the default behavior to allow
     * boosting above RATED_TDP.  Clients specify this action when they
     * explicitly need boost to be disabled (e.g. eliminating perf variation,
     * special apps which exhibit bad behavior, etc.).
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_LIMIT = 2,
    /*!
     * Lock to RATED_TDP - this action requests the clocks to be fixed at the
     * RATED_TDP.  Used for achieving stable clocks required for profiling.
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_LOCK = 3,
    /*!
     * Lock to Min TDP - This requests min to be fixed at RATED_TDP but allow
     * boosting for max
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_FLOOR = 4,
} NV2080_CTRL_PERF_RATED_TDP_ACTION;

/*!
 * Structure describing dynamic state of the RATED_TDP feature.
 */
#define NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS_MESSAGE_ID (0x6DU)

typedef struct NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS {
    /*!
     * Structure of internal RM state - these values are used to determine the
     * behavior of NV2080_CTRL_PERF_RATED_TDP_CLIENT_RM per the RM's @ref
     * perfPwrRatedTdpLimitRegisterClientActive() interface.
     */
    struct {
        /*!
         * [out] - Mask of active client controllers (@ref
         * PERF_PWR_RATED_TDP_CLIENT) which are currently regulating TDP.  When
         * this mask is zero, NV2080_CTRL_PERF_RATED_TDP_CLIENT_RM will request
         * NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_LIMIT.
         */
        NvU32 clientActiveMask;
        /*!
         * [out] - Boolean indicating that user has requested locking to
         * RATED_TDP vPstate via corresponding regkey
         * NV_REG_STR_RM_PERF_RATED_TDP_LIMIT.  When the boolean value is true,
         * NV2080_CTRL_PERF_RATED_TDP_CLIENT_RM will request
         * NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_LIMIT.
         */
        NvU8  bRegkeyLimitRatedTdp;
    } rm;

    /*!
     * [out] - Arbitrated output action of all client requests (@ref inputs).
     * This is the current state of the RATED_TDP feature.  Will only be @ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_EXCEED or @ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_LIMIT.
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION output;
    /*!
     * [out] - Array of input client request actions, indexed via @ref
     * NV2080_CTRL_PERF_RATED_TDP_CLIENT_<xyz>.  RM will arbitrate between these
     * requests, choosing the highest priority request != @ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_DEFAULT or fallback to choosing @ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_EXCEED.
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION inputs[NV2080_CTRL_PERF_RATED_TDP_CLIENT_NUM_CLIENTS];
} NV2080_CTRL_PERF_RATED_TDP_STATUS_PARAMS;

/*!
 * NV2080_CTRL_CMD_PERF_RATED_TDP_GET_CONTROL
 *
 * This command retrieves the current requested RATED_TDP action corresponding
 * to the specified client.
 *
 * See @ref NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS for documentation of
 * parameters.
 *
 * Possible status values returned are
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_PERF_RATED_TDP_GET_CONTROL (0x2080206e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | 0x6E" */

/*!
 * NV2080_CTRL_CMD_PERF_RATED_TDP_SET_CONTROL
 *
 * This command sets the requested RATED_TDP action corresponding to the
 * specified client.  @Note, however, that this command is unable to set @ref
 * NV2080_CTRL_PERF_RATED_TDP_CLIENT_RM.
 *
 * See @ref NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS for documentation of
 * parameters.
 *
 * Possible status values returned are
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_PERF_RATED_TDP_SET_CONTROL (0x2080206f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | 0x6F" */

/*!
 * Structure containing the requested action for a RATED_TDP client (@ref
 * NV2080_CTRL_PERF_RATED_TDP_CLIENT).
 */
typedef struct NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS {
    /*!
     * [in] - Specified client for request.
     */
    NV2080_CTRL_PERF_RATED_TDP_CLIENT client;
    /*!
     * [in/out] - Client's requested action.
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION input;
} NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS;

/*!
 * This struct represents the GPU monitoring perfmon sample for an engine.
 */
typedef struct NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE {
    /*!
     * Percentage during the sample that the engine remains busy. This
     * is in units of pct*100.
     */
    NvU32 util;
    /*!
     * Scaling factor to convert utilization from full GPU to per vGPU.
     */
    NvU32 vgpuScale;
    /*!
     * Process ID of the process that was active on the engine when the
     * sample was taken. If no process is active then NV2080_GPUMON_PID_INVALID
     * will be returned.
     */
    NvU32 procId;
    /*!
     * Process ID of the process in the vGPU VM that was active on the engine when
     * the sample was taken. If no process is active then NV2080_GPUMON_PID_INVALID
     * will be returned.
     */
    NvU32 subProcessID;
     /*!
     * Process name of the process in the vGPU VM that was active on the engine when
     * the sample was taken. If no process is active then NULL will be returned.
     */
    char  subProcessName[NV_SUBPROC_NAME_MAX_LENGTH];
} NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE;

/*!
 * This struct represents the GPU monitoring perfmon sample.
 */
typedef struct NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE {
    /*!
     * Base GPU monitoring sample.
     */
    NV_DECLARE_ALIGNED(NV2080_CTRL_GPUMON_SAMPLE base, 8);
    /*!
     * FB bandwidth utilization sample.
     */
    NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE fb;
    /*!
     * GR utilization sample.
     */
    NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE gr;
    /*!
     * NV ENCODER utilization sample.
     */
    NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE nvenc;
    /*!
     * NV DECODER utilization sample.
     */
    NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE nvdec;
} NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE;

/*!
 * This struct represents the GPU monitoring samples of perfmon values that
 * client wants the access to.
 */
#define NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM_MESSAGE_ID (0x83U)

typedef NV2080_CTRL_GPUMON_SAMPLES NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM;

/*!
 * Number of GPU monitoring sample in their respective buffers.
 */
#define NV2080_CTRL_PERF_GPUMON_SAMPLE_COUNT_PERFMON_UTIL       100

#define NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_BUFFER_SIZE           \
    NV_SIZEOF32(NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE) *     \
    NV2080_CTRL_PERF_GPUMON_SAMPLE_COUNT_PERFMON_UTIL

/*!
 * NV2080_CTRL_CMD_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2
 *
 * This command returns perfmon gpu monitoring utilization samples.
 * This command is not supported with SMC enabled.
 *
 * See NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_PARAM_V2 for documentation
 * on the parameters.
 *
 * Possible status values returned are
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *
 * Note this is the same as NV2080_CTRL_CMD_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES
 * but without the embedded pointer.
 *
 */
#define NV2080_CTRL_CMD_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2 (0x20802096) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS_MESSAGE_ID" */

/*!
 *  This structure represents the GPU monitoring samples of utilization values that
 *  the client wants access to.
 */
#define NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS_MESSAGE_ID (0x96U)

typedef struct NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS {
    /*!
    * Type of the sample, see NV2080_CTRL_GPUMON_SAMPLE_TYPE_* for reference.
    */
    NvU8  type;
    /*!
    * Size of the buffer, this should be
    * bufSize ==  NV2080_CTRL_*_GPUMON_SAMPLE_COUNT_*
    *    sizeof(derived type of NV2080_CTRL_GPUMON_SAMPLE).
    */
    NvU32 bufSize;
    /*!
    * Number of samples in ring buffer.
    */
    NvU32 count;
    /*!
    * tracks the offset of the tail in the circular queue array pSamples.
    */
    NvU32 tracker;
    /*!
    * A circular queue with size == bufSize.
    *
    * @note This circular queue wraps around after 10 seconds of sampling,
    * and it is clients' responsibility to query within this time frame in
    * order to avoid losing samples.
    * @note With one exception, this queue contains last 10 seconds of samples
    * with tracker poiniting to oldest entry and entry before tracker as the
    * newest entry. Exception is when queue is not full (i.e. tracker is
    * pointing to a zeroed out entry), in that case valid entries are between 0
    * and tracker.
    * @note Clients can store tracker from previous query in order to provide
    * samples since last read.
    */
    NV_DECLARE_ALIGNED(NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE samples[NV2080_CTRL_PERF_GPUMON_SAMPLE_COUNT_PERFMON_UTIL], 8);
} NV2080_CTRL_PERF_GET_GPUMON_PERFMON_UTIL_SAMPLES_V2_PARAMS;

/*
 * NV2080_CTRL_CMD_PERF_GPU_IS_IDLE
 *
 * This command notifies RM to make p state switching aggressive by setting
 * required limiting factors to speed up GC6 Entry initiation.
 *
 *  prevPstate [out]
 *      This parameter will contain the pstate before the switch was initiated
 *
 * Possible status return values are:
 *   NV_OK : If P State Switch is successful
 *   NV_INVALID_STATE : If unable to access P State structure
 *   NVOS_STATUS_ERROR   : If P State Switch is unsuccessful
 */
#define NV2080_CTRL_CMD_PERF_GPU_IS_IDLE (0x20802089) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_GPU_IS_IDLE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_GPU_IS_IDLE_PARAMS_MESSAGE_ID (0x89U)

typedef struct NV2080_CTRL_PERF_GPU_IS_IDLE_PARAMS {
    NvU32 prevPstate;
    NvU32 action;
} NV2080_CTRL_PERF_GPU_IS_IDLE_PARAMS;

#define NV2080_CTRL_PERF_GPU_IS_IDLE_TRUE             (0x00000001)
#define NV2080_CTRL_PERF_GPU_IS_IDLE_FALSE            (0x00000002)

/*
 * NV2080_CTRL_CMD_PERF_AGGRESSIVE_PSTATE_NOTIFY
 *
 * This command is for the KMD Aggressive P-state feature.
 *
 *  bGpuIsIdle [in]
 *      When true, applies cap to lowest P-state/GPCCLK. When false, releases cap.
 *  idleTimeUs [in]
 *      The amount of time (in microseconds) the GPU was idle since previous
 *      call, part of the GPU utilization data from KMD.
 *  busyTimeUs [in]
 *      The amount of time (in microseconds) the GPU was not idle since
 *      previous call, part of the GPU utilization data from KMD.
 *
 * Possible status return values are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */


#define NV2080_CTRL_CMD_PERF_AGGRESSIVE_PSTATE_NOTIFY (0x2080208f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_AGGRESSIVE_PSTATE_NOTIFY_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_AGGRESSIVE_PSTATE_NOTIFY_PARAMS_MESSAGE_ID (0x8FU)

typedef struct NV2080_CTRL_PERF_AGGRESSIVE_PSTATE_NOTIFY_PARAMS {
    NvBool bGpuIsIdle;
    NvBool bRestoreToMax;
    NV_DECLARE_ALIGNED(NvU64 idleTimeUs, 8);
    NV_DECLARE_ALIGNED(NvU64 busyTimeUs, 8);
} NV2080_CTRL_PERF_AGGRESSIVE_PSTATE_NOTIFY_PARAMS;


/* _ctrl2080perf_h_ */

