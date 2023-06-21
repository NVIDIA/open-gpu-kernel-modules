/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// This file was generated with FINN, an NVIDIA coding tool.
// Source file:      ctrl/ctrlb0cc.finn
//




#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"  // NV2080_CTRL_GPU_REG_OP
#include "ctrlb0cc/ctrlb0ccprofiler.h"
#include "ctrlb0cc/ctrlb0ccinternal.h"
#include "ctrlb0cc/ctrlb0ccpower.h"

/*
 * NVB0CC_CTRL_CMD_PROFILER_REQUEST_CG_CONTROLS
 *
 * This command attempts to enable or disable various clock-gating features of
 * the GPU on behalf of the profiler. If this command is unable to set the
 * clock-gating feature state of any of the requested features, this command
 * will fail and none of the requested features will be modified. If this
 * command fails because one or more clock-gating feature requests were
 * rejected, it will return NV_ERR_STATE_IN_USE in the globalStatus
 * parameter and the fields in the statusMask parameter for the features for
 * which the requests were rejected will have the value
 * NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_REQUEST_REJECTED.
 * If a given feature is not supported on the GPU, the field for that clock-
 * gating feature will have the value
 * NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_REQUEST_NOT_SUPPORTED in the
 * statusMask parameter, but this condition by itself will not cause the
 * command to fail. Even if this command fails, the field for clock-gating
 * features which would have successfully changed will have the value
 * NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_FULFILLED in the statusMask
 * parameter.
 *
 * Each of the clock-gating features is reference-counted individually, so
 * that multiple MAXWELL_PROFILER objects may request and rely on the same
 * settings for the features simultaneously. Each clock-gating feature is
 * locked to the requested state until the MAXWELL_PROFILER object is freed or
 * the NVB0CC_CTRL_CMD_PROFILER_RELEASE_CG_CONTROLS command is called for that
 * feature.
 *
 * Currently, only MAXWELL_PROFILER requests for power features using this
 * command are reference counted. Changes to the power feature settings made
 * either by other control commands or the RM itself may interfere with the
 * settings requested by MAXWELL_PROFILER instances.
 *
 * This command will always return NV_OK when given valid
 * parameters. If there is any other failure that prevents the clock-gating
 * features from being set appropriately, the globalStatus parameter will
 * indicate this and the statusMask parameter will indicate which clock-gating
 * feature requests failed and why.
 *
 *   controlMask
 *     This parameter indicates which clock-gating features the request should
 *     apply to. This parameter has the following fields:
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG
 *         The value of this field indicates whether this request should apply
 *         to engine-level clock-gating of the GR engine. Valid values for
 *         this field are:
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_IGNORE
 *             This value indicates that the GR engine-level clock-gating
 *             should be ignored. This will not affect the reference count for
 *             this feature.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_DISABLE
 *             This value indicates that the GR engine-level clock-gating
 *             should be disabled.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_ENABLE
 *             This value indicates that the GR engine-level clock-gating
 *             should be enabled.
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG
 *         The value of this field indicates whether this request should apply
 *         to block-level clock-gating. Valid values for this field are:
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_IGNORE
 *             This value indicates that block-level clock-gating should be
 *             ignored. This will not affect the reference count for this
 *             feature.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_DISABLE
 *             This value indicates that block-level clock-gating should be
 *             disabled.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_ENABLE
 *             This value indicates that block-level clock-gating should be
 *             enabled.
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG
 *         The value of this field indicates whether this request should apply
 *         to second-level clock-gating. Valid values for this field are:
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_IGNORE
 *             This value indicates that second-level clock-gating should be
 *             ignored. This will not affect the reference count for this
 *             feature.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_DISABLE
 *             This value indicates that second-level clock-gating should be
 *             disabled.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_ENABLE
 *             This value indicates that second-level clock-gating should be
 *             enabled.
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG
 *         The value of this field indicates whether this request should apply
 *         to GR engine-level power-gating. Valid values for this field are:
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_IGNORE
 *             This value indicates that engine-level power-gating should be
 *             ignored. This will not affect the reference count for this
 *             feature.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_DISABLE
 *             This value indicates that engine-level power-gating should be
 *             disabled.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_ENABLE
 *             This value indicates that engine-level power-gating should be
 *             enabled.
 *         Note that this field is only temporary to allow reference counting
 *         restricted to MAXWELL_PROFILER instances, until the time when the
 *         existing controls for this power feature can be updated to support
 *         reference counting across all clients and the RM.
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN
 *         The value of this field indicates whether this request should apply
 *         to SM idle slowdown. Valid values for this field are:
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_IGNORE
 *             This value indicates that SM idle slowdown should be ignored.
 *             This will not affect the reference count for this feature.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_DISABLE
 *             This value indicates that SM idle slowdown should be disabled.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_ENABLE
 *             This value indicates that SM idle slowdown should be enabled.
 *         Note that this field is only temporary to allow reference counting
 *         restricted to MAXWELL_PROFILER instances, until the time when the
 *         existing controls for this power feature can be updated to support
 *         reference counting across all clients and the RM.
  *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT
 *         The value of this field indicates whether this request should apply
 *         to VAT. Valid values for this field are:
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_IGNORE
 *             This value indicates that VAT should be ignored.
 *             This will not affect the reference count for this feature.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_DISABLE
 *             This value indicates that VAT should be disabled.
 *           NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_ENABLE
 *             This value indicates that VAT should be enabled.
 *         Note that this field is only temporary to allow reference counting
 *         restricted to MAXWELL_PROFILER instances, until the time when the
 *         existing controls for this power feature can be updated to support
 *         reference counting across all clients and the RM.
 *   globalStatus
 *     This parameter returns the overall status of the requests for all
 *     clock-gating controls. If the value of this parameter is not
 *     NV_OK, none of the clock-gating controls will be set as
 *     requested. Possible values for this parameter are:
 *       NV_OK
 *         This value indicates that all of the clock-gating control requests
 *         were either fulfilled or not supported on the hardware.
 *       NV_ERR_INVALID_REQUEST
 *         This value indicates that at least one of the clock-gating control
 *         requests were invalid given the MAXWELL_PROFILER instance's
 *         outstanding requests.
 *       NV_ERR_STATE_IN_USE
 *         This value indicates that at least one of the clock-gating controls
 *         has already been locked to a conflicting state by another
 *         MAXWELL_PROFILER instance or the RM itself.
 *   statusMask
 *     This parameter returns the status of the request to set each clock-
 *     gating control specified by the controlMask parameter. The fields are
 *     identical to those of the controlMask parameter. For each field for
 *     which the corresponding field in the controlMask parameter has the
 *     value NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_IGNORE, the value is
 *     undefined. For each field for which the corresponding field in the
 *     controlMask parameter has the value
 *     NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_REQUEST, the value will be
 *     one of the following:
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_REQUEST_FULFILLED
 *         This value indicates that the clock-gating feature corresponding to
 *         the field in question was enabled or disabled according to the
 *         controlMask parameter, and the reference count for the feature was
 *         incremented accordingly.
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_REQUEST_REJECTED
 *         This value indicates that the clock-gating feature corresponding to
 *         the field in question was not set to the expected state according
 *         to the controlMask parameter because another conflicting request is
 *         currently outstanding for the clock-gating feature.
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_REQUEST_FAILED
 *         This value indicates that the clock-gating feature corresponding to
 *         the field in question was not set to the expected state according
 *         to the controlMask parameter because the attempt to do so failed
 *         with an error other than a conflicting request.
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_REQUEST_NOT_SUPPORTED
 *         This value indicates that the clock-gating feature corresponding to
 *         the field in question is not supported on this GPU. 
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 */
#define NVB0CC_CTRL_CMD_PROFILER_REQUEST_CG_CONTROLS NVB0CC_CTRL_CMD_POWER_REQUEST_FEATURES

typedef NVB0CC_CTRL_POWER_REQUEST_FEATURES_PARAMS NVB0CC_CTRL_PROFILER_REQUEST_CG_CONTROLS_PARAMS;

/* valid fields for the controlMask and statusMask parameters */
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG                 1:0
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG                 3:2
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG                 5:4

/* 
 * The following are temporary fields for the controlMask and statusMask
 * parameters. They are required to reference count their respective features
 * until the existing RM controls can be safely updated, and the definitions
 * for these features will be removed soon after that.
 */
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG                 7:6
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN        9:8
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT                  11:10

/* valid values for fields in the controlMask parameter */
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_IGNORE                                   (0x00000000)
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_DISABLE                                  (0x00000001)
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_ENABLE                                   (0x00000002)

#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_IGNORE                         NVB0CC_CTRL_PROFILER_CG_CONTROL_IGNORE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_DISABLE                        NVB0CC_CTRL_PROFILER_CG_CONTROL_DISABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_ENABLE                         NVB0CC_CTRL_PROFILER_CG_CONTROL_ENABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_IGNORE                         NVB0CC_CTRL_PROFILER_CG_CONTROL_IGNORE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_DISABLE                        NVB0CC_CTRL_PROFILER_CG_CONTROL_DISABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_ENABLE                         NVB0CC_CTRL_PROFILER_CG_CONTROL_ENABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_IGNORE                         NVB0CC_CTRL_PROFILER_CG_CONTROL_IGNORE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_DISABLE                        NVB0CC_CTRL_PROFILER_CG_CONTROL_DISABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_ENABLE                         NVB0CC_CTRL_PROFILER_CG_CONTROL_ENABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_IGNORE                         NVB0CC_CTRL_PROFILER_CG_CONTROL_IGNORE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_DISABLE                        NVB0CC_CTRL_PROFILER_CG_CONTROL_DISABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_ENABLE                         NVB0CC_CTRL_PROFILER_CG_CONTROL_ENABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_IGNORE                NVB0CC_CTRL_PROFILER_CG_CONTROL_IGNORE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_DISABLE               NVB0CC_CTRL_PROFILER_CG_CONTROL_DISABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_ENABLE                NVB0CC_CTRL_PROFILER_CG_CONTROL_ENABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_IGNORE                          NVB0CC_CTRL_PROFILER_CG_CONTROL_IGNORE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_DISABLE                         NVB0CC_CTRL_PROFILER_CG_CONTROL_DISABLE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_ENABLE                          NVB0CC_CTRL_PROFILER_CG_CONTROL_ENABLE
/* possible values for fields in the statusMask parameter */
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FULFILLED                        (0x00000000)
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_REJECTED                         (0x00000001)
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_NOT_SUPPORTED                    (0x00000002)
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FAILED                           (0x00000003)

#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_REQUEST_FULFILLED              NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FULFILLED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_REQUEST_REJECTED               NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_REJECTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_REQUEST_NOT_SUPPORTED          NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_NOT_SUPPORTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_REQUEST_FAILED                 NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FAILED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_REQUEST_FULFILLED              NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FULFILLED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_REQUEST_REJECTED               NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_REJECTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_REQUEST_NOT_SUPPORTED          NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_NOT_SUPPORTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_REQUEST_FAILED                 NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FAILED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_REQUEST_FULFILLED              NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FULFILLED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_REQUEST_REJECTED               NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_REJECTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_REQUEST_NOT_SUPPORTED          NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_NOT_SUPPORTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_REQUEST_FAILED                 NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FAILED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_REQUEST_FULFILLED              NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FULFILLED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_REQUEST_REJECTED               NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_REJECTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_REQUEST_NOT_SUPPORTED          NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_NOT_SUPPORTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_REQUEST_FAILED                 NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FAILED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_REQUEST_FULFILLED     NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FULFILLED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_REQUEST_REJECTED      NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_REJECTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_REQUEST_NOT_SUPPORTED NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_NOT_SUPPORTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_REQUEST_FAILED        NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FAILED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_REQUEST_FULFILLED               NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FULFILLED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_REQUEST_REJECTED                NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_REJECTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_REQUEST_NOT_SUPPORTED           NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_NOT_SUPPORTED
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_REQUEST_FAILED                  NVB0CC_CTRL_PROFILER_CG_CONTROL_REQUEST_FAILED

/* utility masks for the controlMask parameter for all controls */
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ALL                             \
    DRF_SHIFTMASK(NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG) |               \
    DRF_SHIFTMASK(NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG) |               \
    DRF_SHIFTMASK(NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG) |               \
    DRF_SHIFTMASK(NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG) |               \
    DRF_SHIFTMASK(NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN) |      \
    DRF_SHIFTMASK(NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT)
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ALL_IGNORE                      \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _ELCG, _IGNORE) |          \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _BLCG, _IGNORE) |          \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _SLCG, _IGNORE) |          \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _ELPG, _IGNORE) |          \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _IDLE_SLOWDOWN, _IGNORE) | \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _VAT, _IGNORE)
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ALL_DISABLE                     \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _ELCG, _DISABLE) |         \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _BLCG, _DISABLE) |         \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _SLCG, _DISABLE) |         \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _ELPG, _DISABLE) |         \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _IDLE_SLOWDOWN, _DISABLE)| \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _VAT, _DISABLE)
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ALL_ENABLE                      \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _ELCG, _ENABLE) |          \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _BLCG, _ENABLE) |          \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _SLCG, _ENABLE) |          \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _ELPG, _ENABLE) |          \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _IDLE_SLOWDOWN, _ENABLE) | \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _VAT, _ENABLE)

/*
 * NVB0CC_CTRL_CMD_PROFILER_RELEASE_CG_CONTROLS
 *
 * This command is deprecated; please us
 * NVB0CC_CTRL_CMD_POWER_RELEASE_FEATURES.
 *
 * This command releases the MAXWELL_PROFILER's request for the given clock-
 * gating features that was previously created by the
 * NVB0CC_CTRL_CMD_PROFILER_REQUEST_CG_CONTROLS command. If the MAXWELL_PROFILER
 * object does not have an outstanding request to one or more of the given
 * clock-gating features, those features will be ignored while the other
 * feature requests will be released.
 *
 * After calling this command, the calling client may not rely on the current
 * value of any of the released clock-gating features to remain, even if
 * several identical requests for the given clock-gating features were made
 * using NVB0CC_CTRL_CMD_PROFILER_REQUEST_CG_CONTROLS. The RM only reference-
 * counts the settings requested by MAXWELL_PROFILER instances - it does not
 * reference-count multiple identical requests made using the same
 * MAXWELL_PROFILER instance.
 *
 * All outstanding requests made using this MAXWELL_PROFILER object are
 * implicitly released when the MAXWELL_PROFILER is freed.
 *
 *   controlMask
 *     This parameter indicates which clock-gating features the RM should
 *     release the MAXWELL_PROFILER's reference to. See
 *     NVB0CC_CTRL_CMD_PROFILER_REQUEST_CG_CONTROLS for valid fields. Valid
 *     values for each field are:
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_IGNORE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_IGNORE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_IGNORE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_IGNORE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_IGNORE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_IGNORE
 *         This value indicates that the clock-gating feature associated with
 *         the field should not be released, even if the MAXWELL_PROFILER has an
 *         outstanding request for it. This will not affect the reference
 *         count for the feature.
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_RELEASE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_RELEASE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_RELEASE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_RELEASE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_RELEASE
 *       NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_RELEASE
 *         This value indicates that the MAXWELL_PROFILER's outstanding request
 *         for the clock-gating feature associated with the field should be
 *         released. This will decrement the reference count for the feature
 *         if the MAXWELL_PROFILER has an outstanding request for it.
 *
 * Possible status values returned are:
 *    NV_OK
 *    NV_ERR_INVALID_ARGUMENT
 */
#define NVB0CC_CTRL_CMD_PROFILER_RELEASE_CG_CONTROLS                             NVB0CC_CTRL_CMD_POWER_RELEASE_FEATURES

typedef NVB0CC_CTRL_POWER_RELEASE_FEATURES_PARAMS NVB0CC_CTRL_PROFILER_RELEASE_CG_CONTROLS_PARAMS;

/* 
 * valid values for the controlMask parameter in addition to
 * NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_*_IGNORE
 */
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_RELEASE                    (0x00000003)

#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELCG_RELEASE          NVB0CC_CTRL_PROFILER_CG_CONTROL_RELEASE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_BLCG_RELEASE          NVB0CC_CTRL_PROFILER_CG_CONTROL_RELEASE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_SLCG_RELEASE          NVB0CC_CTRL_PROFILER_CG_CONTROL_RELEASE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ELPG_RELEASE          NVB0CC_CTRL_PROFILER_CG_CONTROL_RELEASE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_IDLE_SLOWDOWN_RELEASE NVB0CC_CTRL_PROFILER_CG_CONTROL_RELEASE
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_VAT_RELEASE           NVB0CC_CTRL_PROFILER_CG_CONTROL_RELEASE

/* utility mask for the controlMask parameter for all fields */
#define NVB0CC_CTRL_PROFILER_CG_CONTROL_MASK_ALL_RELEASE                     \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _ELCG, _RELEASE) |         \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _BLCG, _RELEASE) |         \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _SLCG, _RELEASE) |         \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _ELPG, _RELEASE) |         \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _IDLE_SLOWDOWN, _RELEASE)| \
    DRF_DEF(B0CC, _CTRL_PROFILER_CG_CONTROL_MASK, _VAT, _RELEASE)
