/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl2080/ctrl2080perf.finn
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

#define NV2080_CTRL_PERF_BOOST_FLAGS_CUDA_PRIORITY          6:6
#define NV2080_CTRL_PERF_BOOST_FLAGS_CUDA_PRIORITY_DEFAULT  (0x00000000)
#define NV2080_CTRL_PERF_BOOST_FLAGS_CUDA_PRIORITY_HIGH     (0x00000001)

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

/*
 * NV2080_CTRL_PERF_POWERSTATE
 *
 * This structure describes power state information.
 *
 *    powerState
 *       This parameter specifies the type of power source.
 *       Legal values for this parameter include:
 *          NV2080_CTRL_PERF_POWER_SOURCE_AC
 *             This values indicates that the power state is AC.
 *          NV2080_CTRL_PERF_POWER_SOURCE_BATTERY
 *             This values indicates that the power state is battery.
 */
#define NV2080_CTRL_PERF_POWER_SOURCE_AC      (0x00000000)
#define NV2080_CTRL_PERF_POWER_SOURCE_BATTERY (0x00000001)

typedef struct NV2080_CTRL_PERF_POWERSTATE_PARAMS {
    NvU32 powerState;
} NV2080_CTRL_PERF_POWERSTATE_PARAMS;

/*
 * NV2080_CTRL_CMD_PERF_SET_POWERSTATE
 *
 * This command can be used to set the perf power state as AC or battery.
 *
 *    powerStateInfo
 *       This parameter specifies the power source type to set.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV2080_CTRL_CMD_PERF_SET_POWERSTATE (0x2080205b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS_MESSAGE_ID (0x5BU)

typedef struct NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS {
    NV2080_CTRL_PERF_POWERSTATE_PARAMS powerStateInfo;
} NV2080_CTRL_PERF_SET_POWERSTATE_PARAMS;

/*
 * NV2080_CTRL_CMD_PERF_SET_AUX_POWER_STATE
 *
 * This command allows the forcing of a performance level based on auxiliary
 * power-states.
 *
 *   powerState
 *     This parameter specifies the target auxiliary Power state. Legal aux
 *     power-states for this parameter are defined by the
 *     NV2080_CTRL_PERF_AUX_POWER_STATE_P* definitions that follow.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_PERF_SET_AUX_POWER_STATE (0x20802092) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS_MESSAGE_ID (0x92U)

typedef struct NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS {
    NvU32 powerState;
} NV2080_CTRL_PERF_SET_AUX_POWER_STATE_PARAMS;

#define NV2080_CTRL_PERF_AUX_POWER_STATE_P0    (0x00000000)
#define NV2080_CTRL_PERF_AUX_POWER_STATE_P1    (0x00000001)
#define NV2080_CTRL_PERF_AUX_POWER_STATE_P2    (0x00000002)
#define NV2080_CTRL_PERF_AUX_POWER_STATE_P3    (0x00000003)
#define NV2080_CTRL_PERF_AUX_POWER_STATE_P4    (0x00000004)
#define NV2080_CTRL_PERF_AUX_POWER_STATE_COUNT (0x00000005)

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
 * Enumeration VPstates - these are possible VPStates that clients can
 * request
 */
typedef enum NV2080_CTRL_PERF_RATED_TDP_VPSTATE_TYPE {
    /*!
     * Choise of the RATED TDP VPstate
     */
    NV2080_CTRL_PERF_VPSTATE_RATED_TDP = 0,
    /*!
     * Choise of the TURBO BOOST VPstate
     */
    NV2080_CTRL_PERF_VPSTATE_TURBO_BOOST = 1,
    /*!
     * Number of supported vpstates.
     *
     * @Note MUST ALWAYS BE LAST!
     */
    NV2080_CTRL_PERF_VPSTATE_NUM_VPSTATES = 2,
} NV2080_CTRL_PERF_RATED_TDP_VPSTATE_TYPE;

/*!
 * Enumeration VPstates - these are possible VPStates that clients can
 * request
 */
typedef struct NV2080_CTRL_PERF_RATED_TDP_CLIENT_REQUEST {
    /*!
     * [in] - Specified client for request.
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION       action;
    /*!
     * [in/out] - Client's requested action.
     */
    NV2080_CTRL_PERF_RATED_TDP_VPSTATE_TYPE vPstateType;
} NV2080_CTRL_PERF_RATED_TDP_CLIENT_REQUEST;

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
    NV2080_CTRL_PERF_RATED_TDP_ACTION       output;
    /*
     * [out] - Arbitrated output VPStates of all client requests (@ref inputs).
     * This is the current VPState of the RATED_TDP feature.
     */
    NV2080_CTRL_PERF_RATED_TDP_VPSTATE_TYPE outputVPstate;
    /*!
     * [out] - Array of input client request actions, indexed via @ref
     * NV2080_CTRL_PERF_RATED_TDP_CLIENT_<xyz>.  RM will arbitrate between these
     * requests, choosing the highest priority request != @ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_DEFAULT or fallback to choosing @ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_EXCEED.
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION       inputs[NV2080_CTRL_PERF_RATED_TDP_CLIENT_NUM_CLIENTS];
    /*!
     * [out] - Array of input client request VPstates, indexed via @ref
     * NV2080_CTRL_PERF_RATED_TDP_CLIENT_<xyz>.  RM will arbitrate between these
     * requests, choosing the highest priority request != @ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_DEFAULT or fallback to choosing @ref
     * NV2080_CTRL_PERF_RATED_TDP_ACTION_FORCE_EXCEED.
     */
    NV2080_CTRL_PERF_RATED_TDP_VPSTATE_TYPE vPstateTypes[NV2080_CTRL_PERF_RATED_TDP_CLIENT_NUM_CLIENTS];
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
#define NV2080_CTRL_CMD_PERF_RATED_TDP_GET_CONTROL (0x2080206e) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_RATED_TDP_GET_CONTROL_PARAMS_MESSAGE_ID" */

/*!
 * Structure containing the requested action for a RATED_TDP client (@ref
 * NV2080_CTRL_PERF_RATED_TDP_CLIENT).
 */
typedef struct NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS {
    /*!
     * [in] - Specified client for request.
     */
    NV2080_CTRL_PERF_RATED_TDP_CLIENT       client;
    /*!
     * [in/out] - Client's requested action.
     */
    NV2080_CTRL_PERF_RATED_TDP_ACTION       input;
    /*
     * [in] - Specified VPState of the request
     */
    NV2080_CTRL_PERF_RATED_TDP_VPSTATE_TYPE vPstateType;
} NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS;

#define NV2080_CTRL_PERF_RATED_TDP_GET_CONTROL_PARAMS_MESSAGE_ID (0x6EU)

typedef NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS NV2080_CTRL_PERF_RATED_TDP_GET_CONTROL_PARAMS;

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
#define NV2080_CTRL_CMD_PERF_RATED_TDP_SET_CONTROL (0x2080206f) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_RATED_TDP_SET_CONTROL_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_RATED_TDP_SET_CONTROL_PARAMS_MESSAGE_ID (0x6FU)

typedef NV2080_CTRL_PERF_RATED_TDP_CONTROL_PARAMS NV2080_CTRL_PERF_RATED_TDP_SET_CONTROL_PARAMS;

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
    /*!
     * PID struct pointer of the process that was active on the engine when the
     * the sample was taken. If no process is active then NULL pointer
     * will be returned
     */
    NV_DECLARE_ALIGNED(NvU64 pOsPidInfo, 8);
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
    NV_DECLARE_ALIGNED(NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE fb, 8);
    /*!
     * GR utilization sample.
     */
    NV_DECLARE_ALIGNED(NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE gr, 8);
    /*!
     * NV ENCODER utilization sample.
     */
    NV_DECLARE_ALIGNED(NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE nvenc, 8);
    /*!
     * NV DECODER utilization sample.
     */
    NV_DECLARE_ALIGNED(NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE nvdec, 8);
    /*!
     * NV JPEG utilization sample.
     */
    NV_DECLARE_ALIGNED(NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE nvjpg, 8);
    /*!
     * NV OFA utilization sample.
     */
    NV_DECLARE_ALIGNED(NV2080_CTRL_PERF_GPUMON_ENGINE_UTIL_SAMPLE nvofa, 8);
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
#define NV2080_CTRL_PERF_GPUMON_SAMPLE_COUNT_PERFMON_UTIL       72

#define NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_BUFFER_SIZE           \
    sizeof(NV2080_CTRL_PERF_GPUMON_PERFMON_UTIL_SAMPLE) *          \
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


typedef struct NV2080_CTRL_PERF_GET_CLK_INFO {
    NvU32 flags;
    NvU32 domain;
    NvU32 currentFreq;
    NvU32 defaultFreq;
    NvU32 minFreq;
    NvU32 maxFreq;
} NV2080_CTRL_PERF_GET_CLK_INFO;


#define NV2080_CTRL_PERF_CLK_MAX_DOMAINS    32U

#define NV2080_CTRL_CMD_PERF_GET_LEVEL_INFO (0x20802002) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_GET_LEVEL_INFO_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_GET_LEVEL_INFO_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV2080_CTRL_PERF_GET_LEVEL_INFO_PARAMS {
    NvU32 level;
    NvU32 flags;
    NV_DECLARE_ALIGNED(NvP64 perfGetClkInfoList, 8);
    NvU32 perfGetClkInfoListSize;
} NV2080_CTRL_PERF_GET_LEVEL_INFO_PARAMS;

#define NV2080_CTRL_CMD_PERF_GET_LEVEL_INFO_V2 (0x2080200b) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS {
    NvU32                         level;
    NvU32                         flags;
    NV2080_CTRL_PERF_GET_CLK_INFO perfGetClkInfoList[NV2080_CTRL_PERF_CLK_MAX_DOMAINS];
    NvU32                         perfGetClkInfoListSize;
} NV2080_CTRL_PERF_GET_LEVEL_INFO_V2_PARAMS;

#define NV2080_CTRL_PERF_GET_LEVEL_INFO_FLAGS_TYPE                 0:0
#define NV2080_CTRL_PERF_GET_LEVEL_INFO_FLAGS_TYPE_DEFAULT   (0x00000000)
#define NV2080_CTRL_PERF_GET_LEVEL_INFO_FLAGS_TYPE_OVERCLOCK (0x00000001)
#define NV2080_CTRL_PERF_GET_LEVEL_INFO_FLAGS_MODE                 2:1
#define NV2080_CTRL_PERF_GET_LEVEL_INFO_FLAGS_MODE_NONE      (0x00000000)
#define NV2080_CTRL_PERF_GET_LEVEL_INFO_FLAGS_MODE_DESKTOP   (0x00000001)
#define NV2080_CTRL_PERF_GET_LEVEL_INFO_FLAGS_MODE_MAXPERF   (0x00000002)

/*
 * NV2080_CTRL_CMD_PERF_GET_VID_ENG_PERFMON_SAMPLE
 *
 * This command can be used to obtain video decoder utilization of
 * the associated subdevice.
 * This command is not supported with SMC enabled.
 *
 *   engineType
 *     This parameter will allow clients to set type of video
 *     engine in question. It can be NVENC or NVDEC.
 *   clkPercentBusy
 *     This parameter contains the percentage during the sample that
 *     the clock remains busy.
 *   samplingPeriodUs
 *     This field returns the sampling period in microseconds.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV2080_CTRL_CMD_PERF_GET_VID_ENG_PERFMON_SAMPLE      (0x20802087) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS_MESSAGE_ID" */

typedef enum NV2080_CTRL_CMD_PERF_VID_ENG {
    /*!
     * GPU Video encoder engine.
     */
    NV2080_CTRL_CMD_PERF_VID_ENG_NVENC = 1,

    /*!
     * GPU video decoder engine.
     */
    NV2080_CTRL_CMD_PERF_VID_ENG_NVDEC = 2,

    /*!
     * GPU JPEG engine.
     */
    NV2080_CTRL_CMD_PERF_VID_ENG_NVJPG = 3,

    /*!
     * GPU OFA engine.
     */
    NV2080_CTRL_CMD_PERF_VID_ENG_NVOFA = 4,
} NV2080_CTRL_CMD_PERF_VID_ENG;

#define NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS_MESSAGE_ID (0x87U)

typedef struct NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS {
    NV2080_CTRL_CMD_PERF_VID_ENG engineType;
    NvU32                        clkPercentBusy;
    NvU32                        samplingPeriodUs;
} NV2080_CTRL_PERF_GET_VID_ENG_PERFMON_SAMPLE_PARAMS;

/*
 * NV2080_CTRL_CMD_PERF_GET_POWERSTATE
 *
 * This command can be used to find out whether the perf power state is AC/battery.
 *
 *    powerStateInfo
 *       This parameter specifies the power source type.
 *
 * Possible status values returned are:
 *   NV_OK
 */
#define NV2080_CTRL_CMD_PERF_GET_POWERSTATE (0x2080205a) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS_MESSAGE_ID (0x5AU)

typedef struct NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS {
    NV2080_CTRL_PERF_POWERSTATE_PARAMS powerStateInfo;
} NV2080_CTRL_PERF_GET_POWERSTATE_PARAMS;

/*
 * NV2080_CTRL_CMD_PERF_NOTIFY_VIDEOEVENT
 *
 * This command can be used by video driver to notify RM concerning
 * performance related events.
 *
 *    videoEvent
 *       This parameter specifies the video event to notify.
 *       Legal values for this parameter include:
 *          NV2080_CTRL_PERF_VIDEOEVENT_STREAM_HD_START
 *          NV2080_CTRL_PERF_VIDEOEVENT_STREAM_HD_STOP
 *             These values indicate that a HD video stream (less than 4K)
 *             has started/stopped.
 *          NV2080_CTRL_PERF_VIDEOEVENT_STREAM_SD_START
 *          NV2080_CTRL_PERF_VIDEOEVENT_STREAM_SD_STOP
 *             These are now obsolete in new products as we no longer
 *             need to differentiate between SD and HD.
 *          NV2080_CTRL_PERF_VIDEOEVENT_STREAM_4K_START
 *          NV2080_CTRL_PERF_VIDEOEVENT_STREAM_4K_STOP
 *             These value indicates that a 4K video stream (3840x2160 pixels
 *             or higher) has started/stopped.
 *          NV2080_CTRL_PERF_VIDEOEVENT_OFA_START
 *          NV2080_CTRL_PERF_VIDEOEVENT_OFA_STOP
 *             These value indicates that Optical Flow Accelerator usage has
 *             started/stopped.
 *       The following flags may be or'd into the event value:
 *          NV2080_CTRL_PERF_VIDEOEVENT_FLAG_LINEAR_MODE
 *              The stream operates BSP/VP2 or MSVLD/MSPDEC communication in
 *              linear mode (default is ring mode).
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_STATE
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_PARAM_STRUCT
 */
#define NV2080_CTRL_CMD_PERF_NOTIFY_VIDEOEVENT (0x2080205d) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS_MESSAGE_ID (0x5DU)

typedef struct NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS {
    NvU32 videoEvent;
} NV2080_CTRL_PERF_NOTIFY_VIDEOEVENT_PARAMS;

#define NV2080_CTRL_PERF_VIDEOEVENT_EVENT_MASK       (0x0000ffff)
#define NV2080_CTRL_PERF_VIDEOEVENT_STREAM_HD_START  (0x00000001)
#define NV2080_CTRL_PERF_VIDEOEVENT_STREAM_HD_STOP   (0x00000002)
#define NV2080_CTRL_PERF_VIDEOEVENT_STREAM_START     NV2080_CTRL_PERF_VIDEOEVENT_STREAM_HD_START
#define NV2080_CTRL_PERF_VIDEOEVENT_STREAM_STOP      NV2080_CTRL_PERF_VIDEOEVENT_STREAM_HD_STOP
#define NV2080_CTRL_PERF_VIDEOEVENT_STREAM_SD_START  (0x00000003)
#define NV2080_CTRL_PERF_VIDEOEVENT_STREAM_SD_STOP   (0x00000004)
#define NV2080_CTRL_PERF_VIDEOEVENT_STREAM_4K_START  (0x00000005)
#define NV2080_CTRL_PERF_VIDEOEVENT_STREAM_4K_STOP   (0x00000006)
#define NV2080_CTRL_PERF_VIDEOEVENT_OFA_START        (0x00000007)
#define NV2080_CTRL_PERF_VIDEOEVENT_OFA_STOP         (0x00000008)
#define NV2080_CTRL_PERF_VIDEOEVENT_FLAG_LINEAR_MODE (0x00010000)

/*!
 * @defgroup NV2080_CTRL_PERF_PSTATES
 *
 * These are definitions of performance states (P-states) values.
 * P0 has the maximum performance capability and consumes maximum
 * power. P1 has a lower perf and power than P0, and so on.
 * For NVIDIA GPUs, the following definitions are made:
 * P0    - maximum 3D performance
 * P1    - original P0 when active clocked
 * P2-P3 - balanced 3D performance-power
 * P8    - basic HD video playback
 * P10   - SD video playback
 * P12   - minimum idle power
 * P15   - max possible P-state under current scheme (currently not used)
 * Not all P-states are available on a given system.
 *
 * @note    The @ref NV2080_CTRL_PERF_PSTATES_ID was introduced after the
 *          original constants were added, so not all places that intend to use
 *          these values are using the type. They should be updated to do so.
 * @{
 */
typedef NvU32 NV2080_CTRL_PERF_PSTATES_ID;
#define NV2080_CTRL_PERF_PSTATES_UNDEFINED      (0x00000000U)
#define NV2080_CTRL_PERF_PSTATES_CLEAR_FORCED   (0x00000000U)
#define NV2080_CTRL_PERF_PSTATES_MIN            (0x00000001U)
#define NV2080_CTRL_PERF_PSTATES_P0             (0x00000001U)
#define NV2080_CTRL_PERF_PSTATES_P1             (0x00000002U)
#define NV2080_CTRL_PERF_PSTATES_P2             (0x00000004U)
#define NV2080_CTRL_PERF_PSTATES_P3             (0x00000008U)
#define NV2080_CTRL_PERF_PSTATES_P4             (0x00000010U)
#define NV2080_CTRL_PERF_PSTATES_P5             (0x00000020U)
#define NV2080_CTRL_PERF_PSTATES_P6             (0x00000040U)
#define NV2080_CTRL_PERF_PSTATES_P7             (0x00000080U)
#define NV2080_CTRL_PERF_PSTATES_P8             (0x00000100U)
#define NV2080_CTRL_PERF_PSTATES_P9             (0x00000200U)
#define NV2080_CTRL_PERF_PSTATES_P10            (0x00000400U)
#define NV2080_CTRL_PERF_PSTATES_P11            (0x00000800U)
#define NV2080_CTRL_PERF_PSTATES_P12            (0x00001000U)
#define NV2080_CTRL_PERF_PSTATES_P13            (0x00002000U)
#define NV2080_CTRL_PERF_PSTATES_P14            (0x00004000U)
#define NV2080_CTRL_PERF_PSTATES_P15            (0x00008000U)
#define NV2080_CTRL_PERF_PSTATES_MAX            NV2080_CTRL_PERF_PSTATES_P15
#define NV2080_CTRL_PERF_PSTATES_SKIP_ENTRY     (0x10000U) /* finn: Evaluated from "(NV2080_CTRL_PERF_PSTATES_MAX << 1)" */
#define NV2080_CTRL_PERF_PSTATES_ALL            (0xffffU) /* finn: Evaluated from "(NV2080_CTRL_PERF_PSTATES_MAX | (NV2080_CTRL_PERF_PSTATES_MAX - 1))" */
/*!@}*/

/*
 * NV2080_CTRL_CMD_PERF_GET_CURRENT_PSTATE
 *
 * This command returns the current performance state of the GPU.
 *
 *   currPstate
 *     This parameter returns the current P-state, as defined in
 *     NV2080_CTRL_PERF_PSTATES values.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_STATE
 */
#define NV2080_CTRL_CMD_PERF_GET_CURRENT_PSTATE (0x20802068) /* finn: Evaluated from "(FINN_NV20_SUBDEVICE_0_PERF_INTERFACE_ID << 8) | NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS_MESSAGE_ID" */

#define NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS_MESSAGE_ID (0x68U)

typedef struct NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS {
    NvU32 currPstate;
} NV2080_CTRL_PERF_GET_CURRENT_PSTATE_PARAMS;


/* _ctrl2080perf_h_ */

