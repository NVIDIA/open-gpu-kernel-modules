/*
 * SPDX-FileCopyrightText: Copyright (c) 2001-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrl5070/ctrl5070chnc.finn
//

#include "ctrl/ctrl5070/ctrl5070base.h"
#include "ctrl5070common.h"
#include "nvdisptypes.h"

#define NV5070_CTRL_CMD_NUM_DISPLAY_ID_DWORDS_PER_HEAD 2



#define NV5070_CTRL_IDLE_CHANNEL_ACCL_NONE             (0x00000000)
#define NV5070_CTRL_IDLE_CHANNEL_ACCL_IGNORE_PI                    (NVBIT(0))
#define NV5070_CTRL_IDLE_CHANNEL_ACCL_SKIP_NOTIF                   (NVBIT(1))
#define NV5070_CTRL_IDLE_CHANNEL_ACCL_SKIP_SEMA                    (NVBIT(2))
#define NV5070_CTRL_IDLE_CHANNEL_ACCL_IGNORE_INTERLOCK             (NVBIT(3))
#define NV5070_CTRL_IDLE_CHANNEL_ACCL_IGNORE_FLIPLOCK              (NVBIT(4))
#define NV5070_CTRL_IDLE_CHANNEL_ACCL_TRASH_ONLY                   (NVBIT(5))
#define NV5070_CTRL_IDLE_CHANNEL_ACCL_TRASH_AND_ABORT              (NVBIT(6))

#define NV5070_CTRL_IDLE_CHANNEL_PARAMS_MESSAGE_ID (0x1U)

typedef struct NV5070_CTRL_IDLE_CHANNEL_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       channelClass;
    NvU32                       channelInstance;

    NvU32                       desiredChannelStateMask;
    NvU32                       accelerators;        // For future expansion. Not yet implemented
    NvU32                       timeout;             // For future expansion. Not yet implemented
    NvBool                      restoreDebugMode;
} NV5070_CTRL_IDLE_CHANNEL_PARAMS;

/*
 * NV5070_CTRL_CMD_STOP_OVERLAY
 *
 * This command tries to turn the overlay off ASAP.
 *
 *      channelInstance
 *          This field indicates which of the two instances of the overlay
 *          channel the cmd is meant for.
 *
 *      notifyMode
 *          This field indicates the action RM should take once the overlay has
 *          been successfully stopped. The options are (1) Set a notifier
 *          (2) Set the notifier and generate and OS event
 *
 *      hNotifierCtxDma
 *          Handle to the ctx dma for the notifier that must be written once
 *          overlay is stopped. The standard NvNotification notifier structure
 *          is used.
 *
 *      offset
 *          Offset within the notifier context dma where the notifier begins
 *          Offset must be 16 byte aligned.
 *
 *      hEvent
 *          Handle to the event that RM must use to awaken the client when
 *          notifyMode is WRITE_AWAKEN.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT: Invalid notify mode
 *      NV_ERR_INVALID_CHANNEL: When the overlay is unallocated
 *      NV_ERR_INVALID_OWNER: Callee isn't the owner of the channel
 *      NV_ERR_INVALID_OBJECT_HANDLE: Notif ctx dma not found
 *      NV_ERR_INVALID_OFFSET: Bad offset within notif ctx dma
 *      NV_ERR_INSUFFICIENT_RESOURCES
 *      NV_ERR_TIMEOUT: RM timedout waiting to inject methods
 */
#define NV5070_CTRL_CMD_STOP_OVERLAY                          (0x50700102) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_CMD_STOP_OVERLAY_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_STOP_OVERLAY_NOTIFY_MODE_WRITE        (0x00000000)
#define NV5070_CTRL_CMD_STOP_OVERLAY_NOTIFY_MODE_WRITE_AWAKEN (0x00000001)

#define NV5070_CTRL_CMD_STOP_OVERLAY_PARAMS_MESSAGE_ID (0x2U)

typedef struct NV5070_CTRL_CMD_STOP_OVERLAY_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       channelInstance;
    NvU32                       notifyMode;
    NvHandle                    hNotifierCtxDma;
    NvU32                       offset;
    NV_DECLARE_ALIGNED(NvP64 hEvent, 8);
} NV5070_CTRL_CMD_STOP_OVERLAY_PARAMS;



/*
 * NV5070_CTRL_CMD_IS_MODE_POSSIBLE
 *
 * This command is used by DD to determine whether or not a given mode
 * is possible given the current nvclk, mclk, dispclk and potentially some
 * other parameters that are normally hidden from it. All the parameters
 * except IsPossible (output), Force422(output), MinPstate (input/output),
 * minPerfLevel (output), CriticalWatermark (output), worstCaseMargin (output),
 * and worstCaseDomain (output) params are supplied by the caller.
 *
 *      HeadActive
 *          Whether or not the params for this head are relevant.
 *
 *      PixelClock
 *          Frequency: Pixel clk frequency in KHz.
 *          Adj1000Div1001: 1000/1001 multiplier for pixel clock.
 *
 *      RasterSize
 *          Width: Total width of the raster. Also referred to as HTotal.
 *          Height: Total height of the raster. Also referred to as VTotal.
 *
 *      RasterBlankStart
 *          X: Start of horizontal blanking for the raster.
 *          Y: Start of vertical blanking for the raster.
 *
 *      RasterBlankEnd
 *          X: End of horizontal blanking for the raster.
 *          Y: End of vertical blanking for the raster.
 *
 *      RasterVertBlank2
 *          YStart: Start of second blanking for second field for an
 *              interlaced raster. This field is irrelevant when raster is
 *              progressive.
 *          YEnd: End of second blanking for second field for an
 *              interlaced raster. This field is irrelevant when raster is
 *              progressive.
 *
 *      Control
 *          RasterStructure: Whether the raster ir progressive or interlaced.
 *
 *      OutputScaler
 *          VerticalTaps: Vertical scaler taps.
 *          HorizontalTaps: Horizontal scaler taps.
 *          Force422: Whether OutputScaler is operating in 422 mode or not.
 *
 *      ViewportSizeOut
 *          Width: Width of output viewport.
 *          Height: Height of output viewport.
 *          Both the above fields are irrelevant for G80.
 *
 *      ViewportSizeOutMin
 *          Width: Minimum possible/expected width of output viewport.
 *          Height: Minimum possible/expected height of output viewport.
 *
 *      ViewportSizeIn
 *          Width: Width of input viewport.
 *          Height: Height of input viewport.
 *
 *      Params
 *          Format: Core channel's pixel format. See the enumerants following
 *              the variable declaration for possible options.
 *          SuperSample: Whether to use X1AA or X4AA in core channel.
 *              This parameter is ignored for G80.
 *
 *      BaseUsageBounds
 *          Usable: Whether or not the base channel is expected to be used.
 *          PixelDepth: Maximum pixel depth allowed in base channel.
 *          SuperSample: Whether or not X4AA is allowed in base channel.
 *          BaseLutUsage: Base LUT Size
 *          OutputLutUsage: Output LUT size
 *
 *      OverlayUsageBounds
 *          Usable: Whether or not the overlay channel is expected to be used.
 *          PixelDepth: Maximum pixel depth allowed in overlay channel.
 *          OverlayLutUsage: Overlay LUT Size
 *
 *      BaseLutLo
 *          Enable: Specifies Core Channel's Base LUT is enable or not.
 *          Mode: Specifies the LUT Mode.
 *          NeverYieldToBase: Specifies whether NEVER_YIELD_TO_BASE is enabled or not.
 *
 *      OutputLutLo
 *          Enable: Specifies Core Channel's Output LUT is enable or not.
 *          Mode: Specifies the LUT Mode.
 *          NeverYieldToBase: Specifies whether NEVER_YIELD_TO_BASE is enabled or not.
 *
 *      outputResourcePixelDepthBPP
 *          Specifies the output pixel depth with scaler mode.
 *
 *      CriticalWatermark
 *          If MinPState is set to _NEED_MIN_PSTATE, this will return the critical
 *          watermark level at the minimum Pstate.  Otherwise, this will return
 *          the critical watermark at the level that the IMP calculations are
 *          otherwise performed at.
 *
 *      pixelReplicateMode
 *          Specifies the replication mode whether it is X2 or X4. Need to set the parameter
 *          to OFF if there is no pixel replication.
 *
 *      numSSTLinks
 *          Number of Single Stream Transport links which will be used by the
 *          SOR.  "0" means to use the number indicated by the most recent
 *          NV0073_CTRL_CMD_DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST call.
 *
 *      RequestedOperation
 *          This parameter is used to determine whether
 *          1. DD is simplying querying whether or not the specified mode is
 *             possible (REQUESTED_OPER = _QUERY) or
 *          2. DD is about to set the specified mode and RM should make
 *             appropriate preparations to make the mode possible. DD should
 *             never pass in a mode that was never indicated by RM as possible
 *             when DD queried for the possibility of the mode. This
 *             corresponds to REQUESTED_OPER = _PRE_MODESET.
 *          3. DD just finished setting the specified mode. RM can go ahead
 *             and make changes like lowering the perf level if desired. This
 *             corresponds to REQUESTED_OPER = _POST_MODESET. This parameter is
 *             useful when we are at a higher perf level in a mode that's not
 *             possible at a lower perf level and want to go to a mode that is
 *             possible even at a lower perf level. In such cases, lowering
 *             perf level before modeset is complete is dangerous as it will
 *             cause underflow. RM will wait until the end of modeset to lower
 *             the perf level.
 *
 *      options
 *          Specifies a bitmask for options.
 *            NV5070_CTRL_IS_MODE_POSSIBLE_OPTIONS_GET_MARGIN
 *              Tells IMP to calculate worstCaseMargin and worstCaseDomain.
 *
 *      IsPossible
 *          This is the first OUT param for this call. It indicates whether
 *          or not the current mode is possible.
 *
 *      MinPState
 *          MinPState is an IO (in/out) variable; it gives the minimum p-state
 *          value at which the mode is possible on a PStates 2.0 system if the
 *          parameter is initialized by the caller with _NEED_MIN_PSTATE.  If
 *          _NEED_MIN_PSTATE is not specified, IMP query will just run at the
 *          max available perf level and return results for that pstate.
 *
 *          If the minimum pstate is required, then MasterLockMode,
 *          MasterLockPin, SlaveLockMode, and SlaveLockPin must all be
 *          initialized.
 *
 *          On a PStates 3.0 system, the return value for MinPState is
 *          undefined, but minPerfLevel can return the minimum IMP v-pstate.
 *
 *      minPerfLevel
 *          On a PStates 3.0 system, minPerfLevel returns the minimum IMP
 *          v-pstate at which the mode is possible.  On a PStates 2.0 system,
 *          minPerfLevel returns the minimum perf level at which the mode is
 *          possible.
 *
 *          minPerfLevel is valid only if MinPState is initialized to
 *          _NEED_MIN_PSTATE.
 *
 *      worstCaseMargin
 *          Returns the ratio of available bandwidth to required bandwidth,
 *          multiplied by NV5070_CTRL_IMP_MARGIN_MULTIPLIER.  Available
 *          bandwidth is calculated in the worst case bandwidth domain, i.e.,
 *          the domain with the least available margin.  Bandwidth domains
 *          include the IMP-relevant clock domains, and possibly other virtual
 *          bandwidth domains such as AWP.
 *
 *          Note that IMP checks additional parameters besides the bandwidth
 *          margins, but only the bandwidth margin is reported here, so it is
 *          possible for a mode to have a more restrictive domain that is not
 *          reflected in the reported margin result.
 *
 *          This result is not guaranteed to be valid if the mode is not
 *          possible.
 *
 *          Note also that the result is generally calculated for the highest
 *          pstate possible (usually P0).  But if _NEED_MIN_PSTATE is specified
 *          with the MinPState parameter, the result will be calculated for the
 *          min possible pstate (or the highest possible pstate, if the mode is
 *          not possible).
 *
 *          The result is valid only if
 *          NV5070_CTRL_IS_MODE_POSSIBLE_OPTIONS_GET_MARGIN is set in
 *          "options".
 *
 *      worstCaseDomain
 *          Returns a short text string naming the domain for the margin
 *          returned in "worstCaseMargin".  See "worstCaseMargin" for more
 *          information.
 *
 *      bUseCachedPerfState
 *          Indicates that RM should use cached values for the fastest
 *          available perf level (v-pstate for PStates 3.0 or pstate for
 *          PStates 2.0) and dispclk.  This feature allows the query call to
 *          execute faster, and is intended to be used, for example, during
 *          mode enumeration, when many IMP query calls are made in close
 *          succession, and perf conditions are not expected to change between
 *          query calls.  When IMP has not been queried recently, it is
 *          recommended to NOT use cached values, in case perf conditions have
 *          changed and the cached values no longer reflect the current
 *          conditions.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_GENERIC
 *
 * Assumptions/Limitations:
 *      - If the caller sends any methods to alter the State Cache, before calling of
 *          the following functions:
 *               NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_QUERY_USE_SC
 *               NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_PRE_MODESET_USE_SC
 *               NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_POST_MODESET_USE_SC
 *          the caller must repeatedly issue NV5070_CTRL_CMD_GET_CHANNEL_INFO, and delay until the
 *          returned channelState is either:
 *              NV5070_CTRL_CMD_GET_CHANNEL_INFO_STATE_IDLE,
 *              NV5070_CTRL_CMD_GET_CHANNEL_INFO_STATE_WRTIDLE, or
 *              NV5070_CTRL_CMD_GET_CHANNEL_INFO_STATE_EMPTY.
 *          This ensures that all commands have reached the State Cache before RM reads
 *              them.
 *
 *
 */
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE                                                  (0x50700109) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_QUERY                        (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_PRE_MODESET                  (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_POST_MODESET                 (0x00000002)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_QUERY_USE_SC                 (0x00000003)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_PRE_MODESET_USE_SC           (0x00000004)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_POST_MODESET_USE_SC          (0x00000005)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_REQUESTED_OPERATION_SUPERVISOR                   (0x00000007)

#define NV5070_CTRL_IS_MODE_POSSIBLE_OPTIONS_GET_MARGIN                                   (0x00000001)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_IS_POSSIBLE_NO                                   (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_IS_POSSIBLE_YES                                  (0x00000001)

#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_UNDEFINED                                    (0x00000000)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P0                                           (0x00000001)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P1                                           (0x00000002)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P2                                           (0x00000004)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P3                                           (0x00000008)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P8                                           (0x00000100)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P10                                          (0x00000400)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P12                                          (0x00001000)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P15                                          (0x00008000)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_MAX                                          NV5070_CTRL_IS_MODE_POSSIBLE_PSTATES_P15
#define NV5070_CTRL_IS_MODE_POSSIBLE_NEED_MIN_PSTATE                                      (0x10101010)
#define NV5070_CTRL_IS_MODE_POSSIBLE_NEED_MIN_PSTATE_DEFAULT                              (0x00000000)

#define NV5070_CTRL_IMP_MARGIN_MULTIPLIER                                                 (0x00000400)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_HEAD_ACTIVE_NO                                   (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_HEAD_ACTIVE_YES                                  (0x00000001)

#define NV5070_CTRL_IS_MODE_POSSIBLE_DISPLAY_ID_SKIP_IMP_OUTPUT_CHECK                     (0xAAAAAAAA)

#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_DEFAULT                  (0x00000000)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_16_422               (0x00000001)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_18_444               (0x00000002)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_20_422               (0x00000003)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_24_422               (0x00000004)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_24_444               (0x00000005)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_30_444               (0x00000006)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_32_422               (0x00000007)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_36_444               (0x00000008)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_48_444               (0x00000009)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PIXEL_CLOCK_ADJ1000DIV1001_NO                    (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PIXEL_CLOCK_ADJ1000DIV1001_YES                   (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_CONTROL_STRUCTURE_PROGRESSIVE                    (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_CONTROL_STRUCTURE_INTERLACED                     (0x00000001)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_SCALER_VERTICAL_TAPS_1                    (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_SCALER_VERTICAL_TAPS_2                    (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_SCALER_VERTICAL_TAPS_3                    (0x00000002)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_SCALER_VERTICAL_TAPS_3_ADAPTIVE           (0x00000003)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_SCALER_VERTICAL_TAPS_5                    (0x00000004)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_SCALER_HORIZONTAL_TAPS_1                  (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_SCALER_HORIZONTAL_TAPS_2                  (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_SCALER_HORIZONTAL_TAPS_8                  (0x00000002)

#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_SCALER_FORCE422_MODE_DISABLE                  (0x00000000)
#define NV5070_CTRL_IS_MODE_POSSIBLE_OUTPUT_SCALER_FORCE422_MODE_ENABLE                   (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_I8                                 (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_VOID16                             (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_VOID32                             (0x00000002)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_RF16_GF16_BF16_AF16                (0x00000003)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_A8R8G8B8                           (0x00000004)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_A2B10G10R10                        (0x00000005)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_A8B8G8R8                           (0x00000006)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_R5G6B5                             (0x00000007)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_FORMAT_A1R5G5B5                           (0x00000008)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_SUPER_SAMPLE_X1AA                         (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_SUPER_SAMPLE_X4AA                         (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_USABLE_USE_CURRENT             (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_USABLE_NO                      (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_USABLE_YES                     (0x00000002)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_USE_CURRENT        (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_8                  (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_16                 (0x00000002)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_32                 (0x00000003)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_PIXEL_DEPTH_64                 (0x00000004)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_SUPER_SAMPLE_USE_CURRENT       (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_SUPER_SAMPLE_X1AA              (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_SUPER_SAMPLE_X4AA              (0x00000002)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_BASE_LUT_USAGE_NONE            (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_BASE_LUT_USAGE_257             (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_BASE_LUT_USAGE_1025            (0x00000002)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_OUTPUT_LUT_USAGE_NONE          (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_OUTPUT_LUT_USAGE_257           (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_USAGE_BOUNDS_OUTPUT_LUT_USAGE_1025          (0x00000002)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_USABLE_USE_CURRENT          (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_USABLE_NO                   (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_USABLE_YES                  (0x00000002)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_PIXEL_DEPTH_USE_CURRENT     (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_PIXEL_DEPTH_16              (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_PIXEL_DEPTH_32              (0x00000002)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_PIXEL_DEPTH_64              (0x00000003)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_OVERLAY_LUT_USAGE_NONE      (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_OVERLAY_LUT_USAGE_257       (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OVERLAY_USAGE_BOUNDS_OVERLAY_LUT_USAGE_1025      (0x00000002)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_ENABLE_DISABLE                       (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_ENABLE_ENABLE                        (0x00000001)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_MODE_LORES                           (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_MODE_HIRES                           (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_MODE_INDEX_1025_UNITY_RANGE          (0x00000002)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_MODE_INTERPOLATE_1025_UNITY_RANGE    (0x00000003)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_MODE_INTERPOLATE_1025_XRBIAS_RANGE   (0x00000004)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_MODE_INTERPOLATE_1025_XVYCC_RANGE    (0x00000005)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_MODE_INTERPOLATE_257_UNITY_RANGE     (0x00000006)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_MODE_INTERPOLATE_257_LEGACY_RANGE    (0x00000007)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_NEVER_YIELD_TO_BASE_DISABLE          (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_BASE_LUT_LO_NEVER_YIELD_TO_BASE_ENABLE           (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_ENABLE_DISABLE                     (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_ENABLE_ENABLE                      (0x00000001)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_MODE_LORES                         (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_MODE_HIRES                         (0x00000001)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_MODE_INDEX_1025_UNITY_RANGE        (0x00000002)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_MODE_INTERPOLATE_1025_UNITY_RANGE  (0x00000003)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_MODE_INTERPOLATE_1025_XRBIAS_RANGE (0x00000004)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_MODE_INTERPOLATE_1025_XVYCC_RANGE  (0x00000005)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_MODE_INTERPOLATE_257_UNITY_RANGE   (0x00000006)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_MODE_INTERPOLATE_257_LEGACY_RANGE  (0x00000007)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_NEVER_YIELD_TO_BASE_DISABLE        (0x00000000)
#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_OUTPUT_LUT_LO_NEVER_YIELD_TO_BASE_ENABLE         (0x00000001)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PIXEL_REPLICATE_MODE_OFF                             (0x00000000)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PIXEL_REPLICATE_MODE_X2                              (0x00000001)
#define NV5070_CTRL_IS_MODE_POSSIBLE_PIXEL_REPLICATE_MODE_X4                              (0x00000002)

#define NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS_MESSAGE_ID (0x9U)

typedef struct NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    struct {
        NvU32 HeadActive;
        struct {
            NvU32 Frequency;

            NvU32 Adj1000Div1001;
        } PixelClock;

        struct {
            NvU32 Width;
            NvU32 Height;
        } RasterSize;

        struct {
            NvU32 X;
            NvU32 Y;
        } RasterBlankStart;

        struct {
            NvU32 X;
            NvU32 Y;
        } RasterBlankEnd;

        struct {
            NvU32 YStart;
            NvU32 YEnd;
        } RasterVertBlank2;

        struct {
            NvU32             Structure;
/*
 * Note: For query calls, the lock modes and lock pins are used only if the min
 * pstate is required (i.e., if MinPState is set to
 * NV5070_CTRL_IS_MODE_POSSIBLE_NEED_MIN_PSTATE).
 */
            NV_DISP_LOCK_MODE MasterLockMode;
            NV_DISP_LOCK_PIN  MasterLockPin;
            NV_DISP_LOCK_MODE SlaveLockMode;
            NV_DISP_LOCK_PIN  SlaveLockPin;
        } Control;

        struct {
            NvU32  VerticalTaps;
            NvU32  HorizontalTaps;
            NvBool Force422;
        } OutputScaler;

        struct {
            NvU32 Width;
            NvU32 Height;
        } ViewportSizeOut;

        struct {
            NvU32 Width;
            NvU32 Height;
        } ViewportSizeOutMin;

        struct {
            NvU32 Width;
            NvU32 Height;
        } ViewportSizeOutMax;

        struct {
            NvU32 Width;
            NvU32 Height;
        } ViewportSizeIn;

        struct {
            NvU32 Format;
            NvU32 SuperSample;
        } Params;

        struct {
            NvU32 Usable;
            NvU32 PixelDepth;
            NvU32 SuperSample;
            NvU32 BaseLutUsage;
            NvU32 OutputLutUsage;
        } BaseUsageBounds;

        struct {
            NvU32 Usable;
            NvU32 PixelDepth;
            NvU32 OverlayLutUsage;
        } OverlayUsageBounds;

        struct {
            NvBool Enable;
            NvU32  Mode;
            NvBool NeverYieldToBase;
        } BaseLutLo;

        struct {
            NvBool Enable;
            NvU32  Mode;
            NvBool NeverYieldToBase;
        } OutputLutLo;

        NvU32 displayId[NV5070_CTRL_CMD_NUM_DISPLAY_ID_DWORDS_PER_HEAD];
        NvU32 outputResourcePixelDepthBPP;

        NvU32 CriticalWatermark; // in pixels

    } Head[NV5070_CTRL_CMD_MAX_HEADS];

    struct {
        NvU32 owner;
        NvU32 protocol;
    } Dac[NV5070_CTRL_CMD_MAX_DACS];

    struct {
//
// owner field is deprecated. In the future, all client calls should set
// ownerMask and bUseSorOwnerMask. bUseSorOwnerMask must be set in order
// to use ownerMask.
//
        NvU32 owner;
        NvU32 ownerMask; // Head mask owned this sor

        NvU32 protocol;
        NvU32 pixelReplicateMode;

        NvU8  numSSTLinks;
    } Sor[NV5070_CTRL_CMD_MAX_SORS];

    NvBool bUseSorOwnerMask;

    struct {
        NvU32 owner;
        NvU32 protocol;
    } Pior[NV5070_CTRL_CMD_MAX_PIORS];


    NvU32  RequestedOperation;
// This argument is for VERIF and INTERNAL use only
    NvU32  options;
    NvU32  IsPossible;
    NvU32  MinPState;

    NvU32  minPerfLevel;
//
// Below are the possible Output values for MinPState variable.
// Lower the p-state value higher the power consumption; if no p-states are defined on chip
// then it will return as zero.
//

//
// Below are the possible input values for MinPstate Variable, by default it calculate
// mode is possible or not at max available p-state and return the same state in that variable.
//
    NvU32  worstCaseMargin;

//
// The calculated margin is multiplied by a constant, so that it can be
// represented as an integer with reasonable precision.  "0x400" was chosen
// because it is a power of two, which might allow some compilers/CPUs to
// simplify the calculation by doing a shift instead of a multiply/divide.
// (And 0x400 is 1024, which is close to 1000, so that may simplify visual
// interpretation of the raw margin value.)
//
    char   worstCaseDomain[8];

    NvBool bUseCachedPerfState;
} NV5070_CTRL_CMD_IS_MODE_POSSIBLE_PARAMS;



/*
 * NV5070_CTRL_CMD_GET_CHANNEL_INFO
 *
 * This command returns the current channel state.
 *
 *      channelClass
 *          This field indicates the hw class number (507A-507E)
 *
 *      channelInstance
 *          This field indicates which of the two instances of the channel
 *          (in case there are two. ex: base, overlay etc) the cmd is meant for.
 *          Note that core channel has only one instance and the field should
 *          be set to 0 for core channel.
 *
 *      channelState
 *          This field indicates the desired channel state in a mask form that
 *          is compatible with NV5070_CTRL_CMD_IDLE_CHANNEL. A mask format
 *          allows clients to check for one from a group of states.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC
 *
 * Display driver uses this call to ensure that all it's methods have
 * propagated through hardware's internal fifo
 * (NV5070_CTRL_GET_CHANNEL_INFO_STATE_NO_METHOD_PENDING) before it calls
 * RM to check whether or not the mode it set up in Assembly State Cache will
 * be possible. Note that display driver can not use completion notifier in
 * this case because completion notifier is associated with Update and Update
 * will propagate the state from Assembly to Armed and when checking the
 * possibility of a mode, display driver wouldn't want Armed state to be
 * affected.
 */
#define NV5070_CTRL_CMD_GET_CHANNEL_INFO (0x5070010b) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_CHANNEL_INFO_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_IDLE                 NV5070_CTRL_CMD_CHANNEL_STATE_IDLE
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_WRTIDLE              NV5070_CTRL_CMD_CHANNEL_STATE_WRTIDLE
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_EMPTY                NV5070_CTRL_CMD_CHANNEL_STATE_EMPTY
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_FLUSHED              NV5070_CTRL_CMD_CHANNEL_STATE_FLUSHED
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_BUSY                 NV5070_CTRL_CMD_CHANNEL_STATE_BUSY
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_DEALLOC              NV5070_CTRL_CMD_CHANNEL_STATE_DEALLOC
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_DEALLOC_LIMBO        NV5070_CTRL_CMD_CHANNEL_STATE_DEALLOC_LIMBO
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_LIMBO1               NV5070_CTRL_CMD_CHANNEL_STATE_LIMBO1
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_LIMBO2               NV5070_CTRL_CMD_CHANNEL_STATE_LIMBO2
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_FCODEINIT            NV5070_CTRL_CMD_CHANNEL_STATE_FCODEINIT
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_FCODE                NV5070_CTRL_CMD_CHANNEL_STATE_FCODE
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_VBIOSINIT            NV5070_CTRL_CMD_CHANNEL_STATE_VBIOSINIT
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_VBIOSOPER            NV5070_CTRL_CMD_CHANNEL_STATE_VBIOSOPER
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_UNCONNECTED          NV5070_CTRL_CMD_CHANNEL_STATE_UNCONNECTED
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_INITIALIZE           NV5070_CTRL_CMD_CHANNEL_STATE_INITIALIZE
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_SHUTDOWN1            NV5070_CTRL_CMD_CHANNEL_STATE_SHUTDOWN1
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_SHUTDOWN2            NV5070_CTRL_CMD_CHANNEL_STATE_SHUTDOWN2
#define NV5070_CTRL_GET_CHANNEL_INFO_STATE_NO_METHOD_PENDING    (NV5070_CTRL_GET_CHANNEL_INFO_STATE_EMPTY | NV5070_CTRL_GET_CHANNEL_INFO_STATE_WRTIDLE | NV5070_CTRL_GET_CHANNEL_INFO_STATE_IDLE)
#define NV5070_CTRL_CMD_GET_CHANNEL_INFO_PARAMS_MESSAGE_ID (0xBU)

typedef struct NV5070_CTRL_CMD_GET_CHANNEL_INFO_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       channelClass;
    NvU32                       channelInstance;
    NvBool                      IsChannelInDebugMode;

    NvU32                       channelState;
} NV5070_CTRL_CMD_GET_CHANNEL_INFO_PARAMS;



/*
 * NV5070_CTRL_CMD_SET_ACCL
 *
 *   This command turns accelerators on and off. The use of this command
 *   should be restricted as it may have undesirable effects. It's
 *   purpose is to provide a mechanism for clients to use the
 *   accelerator bits to get into states that are either not detectable
 *   by the RM or may take longer to reach than we think is reasonable
 *   to wait in the RM.
 *
 * NV5070_CTRL_CMD_GET_ACCL
 *
 *   This command queries the current state of the accelerators.
 *
 *      channelClass
 *          This field indicates the hw class number (507A-507E)
 *
 *      channelInstance
 *          This field indicates which of the two instances of the channel
 *          (in case there are two. ex: base, overlay etc) the cmd is meant for.
 *          Note that core channel has only one instance and the field should
 *          be set to 0 for core channel.
 *
 *      accelerators
 *          Accelerators to be set in the SET_ACCEL command. Returns the
 *          currently set accelerators on the GET_ACCEL command.
 *
 *      accelMask
 *          A mask to specify which accelerators to change with the
 *          SET_ACCEL command. This field does nothing in the GET_ACCEL
 *          command.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_CHANNEL
 *      NV_ERR_INVALID_OWNER
 *      NV_ERR_GENERIC
 *
 */

#define NV5070_CTRL_CMD_SET_ACCL (0x5070010c) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_SET_ACCL_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_ACCL (0x5070010d) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_GET_ACCL_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_ACCL_NONE    NV5070_CTRL_IDLE_CHANNEL_ACCL_NONE
#define NV5070_CTRL_ACCL_IGNORE_PI        NV5070_CTRL_IDLE_CHANNEL_ACCL_IGNORE_PI
#define NV5070_CTRL_ACCL_SKIP_NOTIF       NV5070_CTRL_IDLE_CHANNEL_ACCL_SKIP_NOTIF
#define NV5070_CTRL_ACCL_SKIP_SEMA        NV5070_CTRL_IDLE_CHANNEL_ACCL_SKIP_SEMA
#define NV5070_CTRL_ACCL_IGNORE_INTERLOCK NV5070_CTRL_IDLE_CHANNEL_ACCL_IGNORE_INTERLOCK
#define NV5070_CTRL_ACCL_IGNORE_FLIPLOCK  NV5070_CTRL_IDLE_CHANNEL_ACCL_IGNORE_FLIPLOCK
#define NV5070_CTRL_ACCL_TRASH_ONLY       NV5070_CTRL_IDLE_CHANNEL_ACCL_TRASH_ONLY
#define NV5070_CTRL_ACCL_TRASH_AND_ABORT  NV5070_CTRL_IDLE_CHANNEL_ACCL_TRASH_AND_ABORT
#define NV5070_CTRL_SET_ACCL_PARAMS_MESSAGE_ID (0xCU)

typedef struct NV5070_CTRL_SET_ACCL_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       channelClass;
    NvU32                       channelInstance;

    NvU32                       accelerators;
    NvU32                       accelMask;
} NV5070_CTRL_SET_ACCL_PARAMS;
#define NV5070_CTRL_GET_ACCL_PARAMS_MESSAGE_ID (0xDU)

typedef NV5070_CTRL_SET_ACCL_PARAMS NV5070_CTRL_GET_ACCL_PARAMS;

/*
 * NV5070_CTRL_CMD_STOP_BASE
 *
 * This command tries to turn the base channel off ASAP.
 *
 *      channelInstance
 *          This field indicates which of the two instances of the base
 *          channel the cmd is meant for.
 *
 *      notifyMode
 *          This field indicates the action RM should take once the base
 *          channel has been successfully stopped. The options are (1) Set a
 *          notifier (2) Set the notifier and generate and OS event
 *
 *      hNotifierCtxDma
 *          Handle to the ctx dma for the notifier that must be written once
 *          base channel is stopped. The standard NvNotification notifier
 *          structure is used.
 *
 *      offset
 *          Offset within the notifier context dma where the notifier begins
 *          Offset must be 16 byte aligned.
 *
 *      hEvent
 *          Handle to the event that RM must use to awaken the client when
 *          notifyMode is WRITE_AWAKEN.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT: Invalid notify mode
 *      NV_ERR_INVALID_CHANNEL: When the overlay is unallocated
 *      NV_ERR_INVALID_OWNER: Callee isn't the owner of the channel
 *      NV_ERR_INVALID_OBJECT_HANDLE: Notif ctx dma not found
 *      NV_ERR_INVALID_OFFSET: Bad offset within notif ctx dma
 *      NV_ERR_INSUFFICIENT_RESOURCES
 *      NV_ERR_TIMEOUT: RM timedout waiting to inject methods
 */
#define NV5070_CTRL_CMD_STOP_BASE                          (0x5070010e) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_CMD_STOP_BASE_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_STOP_BASE_NOTIFY_MODE_WRITE        (0x00000000)
#define NV5070_CTRL_CMD_STOP_BASE_NOTIFY_MODE_WRITE_AWAKEN (0x00000001)

#define NV5070_CTRL_CMD_STOP_BASE_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV5070_CTRL_CMD_STOP_BASE_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       channelInstance;
    NvU32                       notifyMode;
    NvHandle                    hNotifierCtxDma;
    NvU32                       offset;
    NV_DECLARE_ALIGNED(NvP64 hEvent, 8);
} NV5070_CTRL_CMD_STOP_BASE_PARAMS;



/*
 * NV5070_CTRL_CMD_GET_PINSET_COUNT
 *
 * Get number of pinsets on this GPU.
 *
 *   pinsetCount [out]
 *     Number of pinsets on this GPU is returned in this parameter.
 *     This count includes pinsets that are not connected.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV5070_CTRL_CMD_GET_PINSET_COUNT (0x50700115) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_GET_PINSET_COUNT_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_GET_PINSET_COUNT_PARAMS_MESSAGE_ID (0x15U)

typedef struct NV5070_CTRL_GET_PINSET_COUNT_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       pinsetCount;
} NV5070_CTRL_GET_PINSET_COUNT_PARAMS;

/*
 * NV5070_CTRL_CMD_GET_PINSET_PEER
 *
 * Retrieve the pinset/GPU that is connected to the specified pinset on
 * this GPU.
 *
 *   pinset [in]
 *     Pinset on this GPU for which peer info is to be returned must be
 *     specified in this parameter.
 *
 *   peerGpuId [out]
 *     Instance of the GPU on the other side of the connection is
 *     returned in this parameter.
 *
 *   peerPinset [out]
 *     Pinset on the other side of the connection is returned in this
 *     parameter.  If there is no connection then the value is
 *     NV5070_CTRL_CMD_GET_PINSET_PEER_PEER_PINSET_NONE.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV5070_CTRL_CMD_GET_PINSET_PEER                       (0x50700116) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_GET_PINSET_PEER_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_PINSET_PEER_PEER_GPUINSTANCE_NONE (0xffffffff)

#define NV5070_CTRL_CMD_GET_PINSET_PEER_PEER_PINSET_NONE      (0xffffffff)

#define NV5070_CTRL_GET_PINSET_PEER_PARAMS_MESSAGE_ID (0x16U)

typedef struct NV5070_CTRL_GET_PINSET_PEER_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       pinset;

    NvU32                       peerGpuInstance;
    NvU32                       peerPinset;
} NV5070_CTRL_GET_PINSET_PEER_PARAMS;

/*
 * NV5070_CTRL_CMD_SET_RMFREE_FLAGS
 *
 * This command sets the flags for an upcoming call to RmFree().
 * After the RmFree() API runs successfully or not, the flags are cleared.
 *
 *   flags
 *     This parameter holds the NV0000_CTRL_GPU_SET_RMFREE_FLAGS_*
 *     flags to be passed for the next RmFree() command only.
 *     The flags can be one of those:
 *     - NV0000_CTRL_GPU_SET_RMFREE_FLAGS_NONE:
 *       explicitly clears the flags
 *     - NV0000_CTRL_GPU_SET_RMFREE_FLAGS_FREE_PRESERVES_HW:
 *       instructs RmFree() to preserve the HW configuration. After
 *       RmFree() is run this flag is cleared.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV5070_CTRL_CMD_SET_RMFREE_FLAGS         (0x50700117) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_SET_RMFREE_FLAGS_NONE        0x00000000
#define NV5070_CTRL_SET_RMFREE_FLAGS_PRESERVE_HW 0x00000001
#define NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS_MESSAGE_ID (0x17U)

typedef struct NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       flags;
} NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS;


/*
 * NV5070_CTRL_CMD_IMP_SET_GET_PARAMETER
 *
 * This command allows to set or get certain IMP parameters. Change of
 * values take effect on next modeset and is persistent across modesets
 * until the driver is unloaded or user changes the override.
 *
 *   index
 *     One of NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_XXX defines -
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IMP_ENABLE
 *       Only supports "get" operation. If FALSE, IMP is being bypassed and
 *       all Is Mode Possible queries are answered with "mode is possible"
 *       and registers normally set by IMP are not changed from their defaults.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_ASR_ALLOWED
 *       Should IMP consider using ASR. ASR won't be allowed unless it is set to
 *       "allowed" through both _IS_ASR_ALLOWED and _IS_ASR_ALLOWED_PER_PSTATE.
 *       Note that IMP will not run ASR and MSCG at the same time.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_ASR_ALLOWED_PER_PSTATE
 *       Should IMP consider using ASR when this pstate is being used. ASR won't
 *       be allowed unless it is set to "allowed" through both
 *       NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_ASR_ALLOWED and
 *       NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_ASR_ALLOWED_PER_PSTATE.
 *       So when NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_ASR_ALLOWED
 *       returns FALSE, IMP won't consider ASR for any p-state. Note that IMP
 *       will not run ASR and MSCG at the same time. This function is valid
 *       only on PStates 2.0 systems.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_MSCG_ALLOWED_PER_PSTATE
 *       Should IMP consider using MSCG when this pstate is being used. MSCG
 *       won't be allowed if the MSCG feature isn't enabled even if we set to
 *       "allowed" through
 *       NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_MSCG_ALLOWED_PER_PSTATE.
 *       Use NV2080_CTRL_CMD_MC_QUERY_POWERGATING_PARAMETER to query if MSCG is
 *       supported and enabled. Note that IMP will not run ASR and MSCG at the
 *       same time. This function is valid only on PStates 2.0 systems.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_STUTTER_FEATURE_PER_PSTATE
 *       Only supports "get" operation. Returns which stutter feature is being
 *       engaged in hardware when running on the given pstate. Valid values are:
 *         NV5070_CTRL_IMP_STUTTER_FEATURE_NONE
 *           This value indicates no stutter feature is enabled.
 *         NV5070_CTRL_IMP_STUTTER_FEATURE_ASR
 *           This value indicates ASR is the current enabled stutter feature.
 *         NV5070_CTRL_IMP_STUTTER_FEATURE_MSCG
 *           This value indicates MSCG is the current enabled stutter feature.
 *       Note that system will not run ASR and MSCG at the same time. This
 *       function is valid only on PStates 2.0 systems.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_STUTTER_FEATURE_PREDICTED_EFFICIENCY_PER_PSTATE
 *       Only supports "get" operation. Returns the efficiency which IMP
 *       predicted for the engaged stutter feature (ASR or MSCG) when running
 *       on the given pstate. Normally, the actual efficiency should be higher
 *       than the calculated predicted efficiency. For MSCG, the predicted
 *       efficiency assumes no mempool compression. If compression is enabled
 *       with MSCG, the actual efficiency may be significantly higher. Returns
 *       0 if no stutter feature is running. On PStates 3.0 systems, the
 *       pstateApi parameter is ignored, and the result is returned for the min
 *       IMP v-pstate possible.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS
 *       Only supports "get" operation. Returns information about what the possible
 *       mclk switch is.  Valid fields are:
 *         NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_POSSIBLE
 *           This field is not head-specific and indicates if mclk switch is
 *           possible with the current mode.
 *         NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_OVERRIDE_MEMPOOL
 *           This field is not head-specific and indicates if mclk switch is
 *           possible with the nominal mempool settings (_NO) or if special
 *           settings are required in order for mclk switch to be possible (_YES).
 *         NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_MID_WATERMARK
 *           Each head has its own setting for this field.  If this field is
 *           set to _YES, then the specified head will allow mclk switch to
 *           begin if mempool occupancy exceeds the MID_WATERMARK setting.
 *         NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_DWCF
 *           Each head has its own setting for this field.  If this field is
 *           set to _YES, then the specified head will allow mclk switch to
 *           begin if the head is in its DWCF interval, and the mempool
 *           occupancy is greater than or equal to the DWCF watermark.
 *       Note:  If neither _MID_WATERMARK nor _DWCF is set to _YES, then the
 *       specified head is ignored when determining when it is OK to start an
 *       mclk switch.  Mclk switch must be allowed (or ignored) by all heads
 *       before an mclk switch will actually begin.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_FORCE_MIN_MEMPOOL
 *       Should min mempool be forced.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MEMPOOL_COMPRESSION
 *       Should mempool compression be enabled.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_CURSOR_SIZE
 *       The cursor size (in horizontal pixels) used by IMP (rather than the
 *       actual cursor size) for its computation.
 *       A maximum value is in place for what can be set. It can be queried
 *       after resetting the value - it gets reset to the maximum possible
 *       value.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_ISOFBLATENCY_TEST_ENABLE
 *       This is to Enable/Disable ISO FB Latency Test.
 *       The test records the max ISO FB latency for all heads during the test period (excluding modeset time).
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_ISOFBLATENCY_TEST_WC_TOTAL_LATENCY
 *       This is used to retrieve calculated wcTotalLatency of ISO FB Latency Test.
 *       wcTotalLatency is the worst case time for a request's data to come back after the request is issued.
 *       It is the sum of IMP calculated FbLatency and stream delay.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_ISOFBLATENCY_TEST_MAX_LATENCY
 *       This is used to retrieve the max latency among all heads during the whole ISO FB Latency Test.
 *       The max latency can be used to compare with the wcTotalLatency we calculated.
 *       It decides whether the ISO FB Latency Test is passed or not.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_ISOFBLATENCY_TEST_MAX_TEST_PERIOD
 *       This is used to retrieve the max test period during the whole ISO FB Latency Test.
 *       By experimental result, the test period should be at least 10 secs to approximate the
 *       worst case Fb latency in real situation.
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_GLITCHLESS_MODESET_ENABLE
 *       This enables or disables glitchless modesets.  Modesets can be
 *       glitchless if:
 *       (1) There are no raster timing changes, and
 *       (2) The resource requirements of all bandwidth clients are either not
 *           changing, or they are all changing in the same direction (all
 *           increasing or all decreasing).
 *       If glitchless modeset is disabled, or is not possible, heads will be
 *       blanked during the modeset transition.
 *   pstateApi
 *     NV2080_CTRL_PERF_PSTATES_PXXX value.
 *     Required for NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_ASR_ALLOWED_PER_PSTATE,
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_MSCG_ALLOWED_PER_PSTATE,
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_STUTTER_FEATURE_PER_PSTATE and
 *     NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_STUTTER_FEATURE_PREDICTED_EFFICIENCY_PER_PSTATE
 *     on PStates 2.0 systems. For other indices must be
 *     NV2080_CTRL_PERF_PSTATES_UNDEFINED.  Not used on PStates 3.0 systems.
 *   head
 *     Head index, which is required when querying Mclk switch feature.
 *     (index = NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS)
 *   operation
 *       NV5070_CTRL_IMP_SET_GET_PARAMETER_OPERATION_GET
 *         Indicates a "get" operation.
 *       NV5070_CTRL_IMP_SET_GET_PARAMETER_OPERATION_SET
 *         Indicates a "set" operation.
 *       NV5070_CTRL_IMP_SET_GET_PARAMETER_OPERATION_RESET
 *         Indicates a "reset" operation. This operation will reset the values for
 *         all indices to their RM defaults.
 *   value
 *     Value for new setting of a "set" operation, or the returned value of a
 *     "get" operation; for enable/disable operations, "enable" is non-zero,
 *     and "disable" is zero.
 *
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_INVALID_POINTER
 *   NV_ERR_INVALID_INDEX            specified index is not supported
 *   NV_ERR_INSUFFICIENT_RESOURCES   cannot handle any more overrides
 *   NV_ERR_INVALID_OBJECT     the struct needed to get the specified information
 *                                              is not marked as valid
 *   NV_ERR_INVALID_STATE            the parameter has been set but resetting will
 *                                              not be possible
 */
#define NV5070_CTRL_CMD_IMP_SET_GET_PARAMETER (0x50700118) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS_MESSAGE_ID (0x18U)

typedef struct NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       index;
    NvU32                       pstateApi;
    NvU32                       head;
    NvU32                       operation;
    NvU32                       value;
} NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS;

/* valid operation values */
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_OPERATION_GET                                                0
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_OPERATION_SET                                                1
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_OPERATION_RESET                                              2

/* valid index value */
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_NONE                                                   (0x00000000)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IMP_ENABLE                                             (0x00000001)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_ASR_ALLOWED                                         (0x00000002)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_ASR_ALLOWED_PER_PSTATE                              (0x00000003)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_IS_MSCG_ALLOWED_PER_PSTATE                             (0x00000004)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_STUTTER_FEATURE_PER_PSTATE                             (0x00000005)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_STUTTER_FEATURE_PREDICTED_EFFICIENCY_PER_PSTATE        (0x00000006)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS                            (0x00000007)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_FORCE_MIN_MEMPOOL                                      (0x00000008)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MEMPOOL_COMPRESSION                                    (0x00000009)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_CURSOR_SIZE                                            (0x0000000A)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_ISOFBLATENCY_TEST_ENABLE                               (0x0000000B)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_ISOFBLATENCY_TEST_WC_TOTAL_LATENCY                     (0x0000000C)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_ISOFBLATENCY_TEST_MAX_LATENCY                          (0x0000000D)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_ISOFBLATENCY_TEST_MAX_TEST_PERIOD                      (0x0000000E)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_GLITCHLESS_MODESET_ENABLE                              (0x0000000F)

/* valid NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_ISOHUB_STUTTER_FEATURE values */
#define NV5070_CTRL_IMP_STUTTER_FEATURE_NONE                                                           0
#define NV5070_CTRL_IMP_STUTTER_FEATURE_ASR                                                            1
#define NV5070_CTRL_IMP_STUTTER_FEATURE_MSCG                                                           2

/* valid NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE values */
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_POSSIBLE                      0:0
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_POSSIBLE_NO          (0x00000000)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_POSSIBLE_YES         (0x00000001)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_OVERRIDE_MEMPOOL              1:1
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_OVERRIDE_MEMPOOL_NO  (0x00000000)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_OVERRIDE_MEMPOOL_YES (0x00000001)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_MID_WATERMARK                 2:2
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_MID_WATERMARK_NO     (0x00000000)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_MID_WATERMARK_YES    (0x00000001)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_DWCF                          3:3
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_DWCF_NO              (0x00000000)
#define NV5070_CTRL_IMP_SET_GET_PARAMETER_INDEX_MCLK_SWITCH_FEATURE_OUTPUTS_VALUE_DWCF_YES             (0x00000001)

/*
 * NV5070_CTRL_CMD_SET_MEMPOOL_WAR_FOR_BLIT_TEARING
 *
 * This command engages the WAR for blit tearing caused by huge mempool size and
 * mempool compression. The EVR in aero off mode uses scanline info to predict
 * where the scanline will be at a later time. Since RG scanline is used to perform
 * front buffer blits and isohub buffers large amount of display data it may have
 * fetched several lines of data ahead of where the RG is scanning out leading to
 * video tearing. The WAR for this problem is to reduce the amount of data fetched.
 *
 *   base
 *     This struct must be the first member of all 5070 control calls containing
 *     the subdeviceIndex.
 *   bEngageWAR
 *     Indicates if mempool WAR has to be engaged or disengaged.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 */

#define NV5070_CTRL_CMD_SET_MEMPOOL_WAR_FOR_BLIT_TEARING                                               (0x50700119) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS_MESSAGE_ID (0x19U)

typedef struct NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvBool                      bEngageWAR;
} NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS;
typedef struct NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS *PNV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS;

#define NV5070_CTRL_CMD_GET_ACTIVE_VIEWPORT_BASE (0x50700120) /* finn: Evaluated from "(FINN_NV50_DISPLAY_CHNCTL_INTERFACE_ID << 8) | NV5070_CTRL_CMD_GET_ACTIVE_VIEWPORT_BASE_PARAMS_MESSAGE_ID" */

#define NV5070_CTRL_CMD_GET_ACTIVE_VIEWPORT_BASE_PARAMS_MESSAGE_ID (0x20U)

typedef struct NV5070_CTRL_CMD_GET_ACTIVE_VIEWPORT_BASE_PARAMS {
    NV5070_CTRL_CMD_BASE_PARAMS base;
    NvU32                       head;
    NvU32                       activeViewportBase;
} NV5070_CTRL_CMD_GET_ACTIVE_VIEWPORT_BASE_PARAMS;
typedef struct NV5070_CTRL_CMD_GET_ACTIVE_VIEWPORT_BASE_PARAMS *PNV5070_CTRL_CMD_GET_ACTIVE_VIEWPORT_BASE_PARAMS;

/* _ctrl5070chnc_h_ */
