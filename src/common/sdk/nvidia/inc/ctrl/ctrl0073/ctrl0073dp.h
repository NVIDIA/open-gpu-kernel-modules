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
// Source file:      ctrl/ctrl0073/ctrl0073dp.finn
//

#include "ctrl/ctrl0073/ctrl0073base.h"
#include "ctrl/ctrl0073/ctrl0073common.h"

#include "nvcfg_sdk.h"

/* NV04_DISPLAY_COMMON dfp-display-specific control commands and parameters */

/*
 * NV0073_CTRL_CMD_DP_AUXCH_CTRL
 *
 * This command can be used to perform an aux channel transaction to the
 * displayPort receiver.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the dfp
 *     caps should be returned.  The display ID must a dfp display.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   bAddrOnly
 *     If set to NV_TRUE, this parameter prompts an address-only
 *     i2c-over-AUX transaction to be issued, if supported.  Else the
 *     call fails with NVOS_STATUS_ERR_NOT_SUPPORTED.  The size parameter is
 *     expected to be 0 for address-only transactions.
 *   cmd
 *     This parameter is an input to this command.  The cmd parameter follows
 *     Section 2.4 AUX channel syntax in the DisplayPort spec.
 *     Here are the current defined fields:
 *       NV0073_CTRL_DP_AUXCH_CMD_TYPE
 *         This specifies the request command transaction
 *           NV0073_CTRL_DP_AUXCH_CMD_TYPE_I2C
 *             Set this value to indicate a I2C transaction.
 *           NV0073_CTRL_DP_AUXCH_CMD_TYPE_AUX
 *             Set this value to indicate a DisplayPort transaction.
 *       NV0073_CTRL_DP_AUXCH_CMD_I2C_MOT
 *         This field is dependent on NV0073_CTRL_DP_AUXCH_CMD_TYPE.
 *         It is only valid if NV0073_CTRL_DP_AUXCH_CMD_TYPE_I2C
 *         is specified above and indicates a middle of transaction.
 *         In the case of AUX, this field should be set to zero.  The valid
 *         values are:
 *           NV0073_CTRL_DP_AUXCH_CMD_I2C_MOT_FALSE
 *             The I2C transaction is not in the middle of a transaction.
 *           NV0073_CTRL_DP_AUXCH_CMD_I2C_MOT_TRUE
 *             The I2C transaction is in the middle of a transaction.
 *       NV0073_CTRL_DP_AUXCH_CMD_REQ_TYPE
 *         The request type specifies if we are doing a read/write or write
 *         status request:
 *           NV0073_CTRL_DP_AUXCH_CMD_REQ_TYPE_READ
 *             An I2C or AUX channel read is requested.
 *           NV0073_CTRL_DP_AUXCH_CMD_REQ_TYPE_WRITE
 *             An I2C or AUX channel write is requested.
 *           NV0073_CTRL_DP_AUXCH_CMD_REQ_TYPE_WRITE_STATUS
 *             An I2C write status request desired.  This value should
 *             not be set in the case of an AUX CH request and only applies
 *             to I2C write transaction command.
 *   addr
 *     This parameter is an input to this command.  The addr parameter follows
 *     Section 2.4 in DisplayPort spec and the client should refer to the valid
 *     address in DisplayPort spec.  Only the first 20 bits are valid.
 *   data[]
 *     In the case of a read transaction, this parameter returns the data from
 *     transaction request.  In the case of a write transaction, the client
 *     should write to this buffer for the data to send.  The max # of bytes
 *     allowed is NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE.
 *   size
 *     Specifies how many data bytes to read/write depending on the transaction type.
 *     The input size value should be indexed from 0.  That means if you want to read
 *     1 byte -> size = 0, 2 bytes -> size = 1, 3 bytes -> size = 2, up to 16 bytes
 *     where size = 15.  On return, this parameter returns total number of data bytes
 *     successfully read/written from/to the transaction (indexed from 1).  That is,
 *     if you successfully requested 1 byte, you would send down size = 0.  On return,
 *     you should expect size = 1 if all 1 byte were successfully read. (Note that
 *     it is valid for a display to reply with fewer than the requested number of
 *     bytes; in that case, it is up to the client to make a new request for the
 *     remaining bytes.)
 *   replyType
 *     This parameter is an output to this command.  It returns the auxChannel
 *     status after the end of the aux Ch transaction.  The valid values are
 *     based on the DisplayPort spec:
 *       NV0073_CTRL_DP_AUXCH_REPLYTYPE_ACK
 *         In the case of a write,
 *         AUX: write transaction completed and all data bytes written.
 *         I2C: return size bytes has been written to i2c slave.
 *         In the case of a read, return of ACK indicates ready to reply
 *         another read request.
 *       NV0073_CTRL_DP_AUXCH_REPLYTYPE_NACK
 *         In the case of a write, first return size bytes have been written.
 *         In the case of a read, implies that does not have requested data
 *         for the read request transaction.
 *       NV0073_CTRL_DP_AUXCH_REPLYTYPE_DEFER
 *         Not ready for the write/read request and client should retry later.
 *       NV0073_CTRL_DP_DISPLAYPORT_AUXCH_REPLYTYPE_I2CNACK
 *         Applies to I2C transactions only.  For I2C write transaction:
 *         has written the first return size bytes to I2C slave before getting
 *         NACK.  For a read I2C transaction, the I2C slave has NACKED the I2C
 *         address.
 *       NV0073_CTRL_DP_AUXCH_REPLYTYPE_I2CDEFER
 *         Applicable to I2C transactions.  For I2C write and read
 *         transactions, I2C slave has yet to ACK or NACK the I2C transaction.
 *       NV0073_CTRL_DP_AUXCH_REPLYTYPE_TIMEOUT
 *         The receiver did not respond within the timeout period defined in
 *         the DisplayPort 1.1a specification.
 *   retryTimeMs
 *     This parameter is an output to this command.  In case of
 *     NVOS_STATUS_ERROR_RETRY return status, this parameter returns the time
 *     duration in milli-seconds after which client should retry this command.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NVOS_STATUS_ERROR_RETRY
 */
#define NV0073_CTRL_CMD_DP_AUXCH_CTRL      (0x731341U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_AUXCH_CTRL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE 16U
#define NV0073_CTRL_DP_AUXCH_CTRL_PARAMS_MESSAGE_ID (0x41U)

typedef struct NV0073_CTRL_DP_AUXCH_CTRL_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bAddrOnly;
    NvU32  cmd;
    NvU32  addr;
    NvU8   data[NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE];
    NvU32  size;
    NvU32  replyType;
    NvU32  retryTimeMs;
} NV0073_CTRL_DP_AUXCH_CTRL_PARAMS;

#define NV0073_CTRL_DP_AUXCH_CMD_TYPE                          3:3
#define NV0073_CTRL_DP_AUXCH_CMD_TYPE_I2C               (0x00000000U)
#define NV0073_CTRL_DP_AUXCH_CMD_TYPE_AUX               (0x00000001U)
#define NV0073_CTRL_DP_AUXCH_CMD_I2C_MOT                       2:2
#define NV0073_CTRL_DP_AUXCH_CMD_I2C_MOT_FALSE          (0x00000000U)
#define NV0073_CTRL_DP_AUXCH_CMD_I2C_MOT_TRUE           (0x00000001U)
#define NV0073_CTRL_DP_AUXCH_CMD_REQ_TYPE                      1:0
#define NV0073_CTRL_DP_AUXCH_CMD_REQ_TYPE_WRITE         (0x00000000U)
#define NV0073_CTRL_DP_AUXCH_CMD_REQ_TYPE_READ          (0x00000001U)
#define NV0073_CTRL_DP_AUXCH_CMD_REQ_TYPE_WRITE_STATUS  (0x00000002U)

#define NV0073_CTRL_DP_AUXCH_ADDR                             20:0

#define NV0073_CTRL_DP_AUXCH_REPLYTYPE                         3:0
#define NV0073_CTRL_DP_AUXCH_REPLYTYPE_ACK              (0x00000000U)
#define NV0073_CTRL_DP_AUXCH_REPLYTYPE_NACK             (0x00000001U)
#define NV0073_CTRL_DP_AUXCH_REPLYTYPE_DEFER            (0x00000002U)
#define NV0073_CTRL_DP_AUXCH_REPLYTYPE_TIMEOUT          (0x00000003U)
#define NV0073_CTRL_DP_AUXCH_REPLYTYPE_I2CNACK          (0x00000004U)
#define NV0073_CTRL_DP_AUXCH_REPLYTYPE_I2CDEFER         (0x00000008U)

//This is not the register field, this is software failure case when we
//have invalid argument
#define NV0073_CTRL_DP_AUXCH_REPLYTYPE_INVALID_ARGUMENT (0xffffffffU)

/*
 * NV0073_CTRL_CMD_DP_AUXCH_SET_SEMA
 *
 * This command can be used to set the semaphore in order to gain control of
 * the aux channel.  This control is only used in HW verification.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the dfp
 *     caps should be returned.  The display ID must a dfp display
 *     as determined with the NV0073_CTRL_CMD_SPECIFIC_GET_TYPE command.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   owner
 *     This parameter is an input to this command.
 *     Here are the current defined fields:
 *       NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_RM
 *         Write the aux channel semaphore for resource manager to own the
 *         the aux channel.
 *       NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_VBIOS
 *         Write the aux channel semaphore for vbios/efi to own the
 *         the aux channel.  This value is used only for HW verification
 *         and should not be used in normal driver operation.
 *       NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_PMU
 *         Write the aux channel semaphore for pmu to own the
 *         the aux channel.  This value is used only by pmu
 *         and should not be used in normal driver operation.
 *       NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_DPU
 *         Write the aux channel semaphore for dpu to own the
 *         the aux channel and should not be used in normal
 *         driver operation.
 *       NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_SEC2
 *         Write the aux channel semaphore for sec2 to own the
 *         the aux channel and should not be used in normal
 *         driver operation.
 *       NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_RELEASE
 *         Write the aux channel semaphore for hardware to own the
 *         the aux channel.  This value is used only for HW verification
 *         and should not be used in normal driver operation.
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_DP_AUXCH_SET_SEMA               (0x731342U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS_MESSAGE_ID (0x42U)

typedef struct NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 owner;
} NV0073_CTRL_DP_AUXCH_SET_SEMA_PARAMS;

#define NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER                 2:0
#define NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_RELEASE (0x00000000U)
#define NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_RM      (0x00000001U)
#define NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_VBIOS   (0x00000002U)
#define NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_PMU     (0x00000003U)
#define NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_DPU     (0x00000004U)
#define NV0073_CTRL_DP_AUXCH_SET_SEMA_OWNER_SEC2    (0x00000005U)

/*
 * NV0073_CTRL_CMD_DP_CTRL
 *
 * This command is used to set various displayPort configurations for
 * the specified displayId such a lane count and link bandwidth.  It
 * is assumed that link training has already occurred.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the dfp
 *     caps should be returned.  The display ID must a dfp display.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   cmd
 *     This parameter is an input to this command.
 *     Here are the current defined fields:
 *       NV0073_CTRL_DP_CMD_SET_LANE_COUNT
 *         Set to specify the number of displayPort lanes to configure.
 *           NV0073_CTRL_DP_CMD_SET_LANE_COUNT_FALSE
 *             No request to set the displayport lane count.
 *           NV0073_CTRL_DP_CMD_SET_LANE_COUNT_TRUE
 *             Set this value to indicate displayport lane count change.
 *       NV0073_CTRL_DP_CMD_SET_LINK_BANDWIDTH
 *         Set to specify a request to change the link bandwidth.
 *           NV0073_CTRL_DP_CMD_SET_LINK_BANDWIDTH_FALSE
 *             No request to set the displayport link bandwidth.
 *           NV0073_CTRL_DP_CMD_SET_LINK_BANDWIDTH_TRUE
 *             Set this value to indicate displayport link bandwidth change.
 *       NV0073_CTRL_DP_CMD_SET_LINK_BANDWIDTH
 *         Set to specify a request to change the link bandwidth.
 *           NV0073_CTRL_DP_CMD_SET_LINK_BANDWIDTH_FALSE
 *             No request to set the displayport link bandwidth.
 *           NV0073_CTRL_DP_CMD_SET_LINK_BANDWIDTH_TRUE
 *             Set this value to indicate displayport link bandwidth change.
 *       NV0073_CTRL_DP_CMD_DISABLE_DOWNSPREAD
 *         Set to disable downspread during link training.
 *           NV0073_CTRL_DP_CMD_DISABLE_DOWNSPREAD_FALSE
 *             Downspread will be enabled.
 *           NV0073_CTRL_DP_CMD_DISABLE_DOWNSPREAD_TRUE
 *             Downspread will be disabled (e.g. for compliance testing).
 *       NV0073_CTRL_DP_CMD_SET_FORMAT_MODE
 *         This field specifies the DP stream mode.
 *           NV0073_CTRL_DP_CMD_SET_FORMAT_MODE_SINGLE_STREAM
 *             This value indicates that single stream mode is specified.
 *           NV0073_CTRL_DP_CMD_SET_FORMAT_MODE_MULTI_STREAM
 *             This value indicates that multi stream mode is specified.
 *       NV0073_CTRL_DP_CMD_FAST_LINK_TRAINING
 *         Set to do Fast link training (avoid AUX transactions for link
 *         training). We need to restore all the previous trained link settings
 *         (e.g. the drive current/preemphasis settings) before doing FLT.
 *         During FLT, we send training pattern 1 followed by training pattern 2
 *         each for a period of 500us.
 *           NV0073_CTRL_DP_CMD_FAST_LINK_TRAINING_NO
 *             Not a fast link training scenario.
 *           NV0073_CTRL_DP_CMD_FAST_LINK_TRAINING_YES
 *             Do fast link training.
 *       NV0073_CTRL_DP_CMD_NO_LINK_TRAINING
 *         Set to do No link training. We need to restore all the previous
 *         trained link settings (e.g. the drive current/preemphasis settings)
 *         before doing NLT, but we don't need to do the Clock Recovery and
 *         Channel Equalization. (Please refer to NVIDIA PANEL SELFREFRESH
 *         CONTROLLER SPECIFICATION 3.1.6 for detail flow)
 *           NV0073_CTRL_DP_CMD_NO_LINK_TRAINING_NO
 *             Not a no link training scenario.
 *           NV0073_CTRL_DP_CMD_NO_LINK_TRAINING_YES
 *             Do no link training.
 *       NV0073_CTRL_DP_CMD_USE_DOWNSPREAD_SETTING
 *         Specifies whether RM should use the DP Downspread setting specified by
 *         NV0073_CTRL_DP_CMD_DISABLE_DOWNSPREAD command regardless of what the Display
 *         is capable of. This is used along with the Fake link training option so that
 *         we can configure the GPU to enable/disable spread when a real display is
 *         not connected.
 *           NV0073_CTRL_DP_CMD_USE_DOWNSPREAD_SETTING_FORCE
 *              RM Always use the DP Downspread setting specified.
 *           NV0073_CTRL_DP_CMD_USE_DOWNSPREAD_SETTING_DEFAULT
 *              RM will enable Downspread only if the display supports it. (default)
 *       NV0073_CTRL_DP_CMD_SKIP_HW_PROGRAMMING
 *         Specifies whether RM should skip HW training of the link.
 *         If this is the case then RM only updates its SW state without actually
 *         touching any HW registers. Clients should use this ONLY if it has determined -
 *         a. link is trained and not lost
 *         b. desired link config is same as current trained link config
 *         c. link is not in D3 (should be in D0)
 *           NV0073_CTRL_DP_CMD_SKIP_HW_PROGRAMMING_NO
 *              RM doesn't skip HW LT as the current Link Config is not the same as the
 *              requested Link Config.
 *           NV0073_CTRL_DP_CMD_SKIP_HW_PROGRAMMING_YES
 *              RM skips HW LT and only updates its SW state as client has determined that
 *              the current state of the link and the requested Link Config is the same.
 *       NV0073_CTRL_DP_CMD_DISABLE_LINK_CONFIG
 *         Set if the client does not want link training to happen.
 *         This should ONLY be used for HW verification.
 *           NV0073_CTRL_DP_CMD_DISABLE_LINK_CONFIG_FALSE
 *             This is normal production behaviour which shall perform
 *             link training or follow the normal procedure for lane count
 *             reduction.
 *           NV0073_CTRL_DP_CMD_DISABLE_LINK_CONFIG_TRUE
 *             Set this value to not perform link config steps, this should
 *             only be turned on for HW verif testing.  If _LINK_BANDWIDTH
 *             or _LANE_COUNT is set, RM will only write to the TX DP registers
 *             and perform no link training.
 *       NV0073_CTRL_DP_CMD_POST_LT_ADJ_REQ_GRANTED
 *         This field specifies if source grants Post Link training Adjustment request or not.
 *           NV0073_CTRL_DP_CMD_POST_LT_ADJ_REQ_GRANTED_NO
 *              Source does not grant Post Link training Adjustment request
 *           NV0073_CTRL_DP_CMD_POST_LT_ADJ_REQ_GRANTED_YES
 *              Source grants Post Link training Adjustment request
 *              Source wants to link train LT Tunable Repeaters
 *       NV0073_CTRL_DP_CMD_FAKE_LINK_TRAINING
 *         This field specifies if fake link training is to be done. This will
 *         program enough of the hardware to avoid any hardware hangs and
 *         depending upon option chosen by the client, OR will be enabled for
 *         transmisssion.
 *           NV0073_CTRL_DP_CMD_FAKE_LINK_TRAINING_NO
 *              No Fake LT will be performed
 *           NV0073_CTRL_DP_CMD_FAKE_LINK_TRAINING_DONOT_TOGGLE_TRANSMISSION
 *              SOR will be not powered up during Fake LT
 *           NV0073_CTRL_DP_CMD_FAKE_LINK_TRAINING_TOGGLE_TRANSMISSION_ON
 *              SOR will be powered up during Fake LT
 *       NV0073_CTRL_DP_CMD_TRAIN_PHY_REPEATER
 *         This field specifies if source wants to link train LT Tunable Repeaters or not.
 *           NV0073_CTRL_DP_CMD_TRAIN_PHY_REPEATER_NO
 *              Source does not want to link train LT Tunable Repeaters
 *           NV0073_CTRL_DP_CMD_TRAIN_PHY_REPEATER_YES
 *       NV0073_CTRL_DP_CMD_BANDWIDTH_TEST
 *         Set if the client wants to reset the link after the link
 *         training is done. As a part of uncommtting a DP display.
 *           NV0073_CTRL_DP_CMD_BANDWIDTH_TEST_NO
 *             This is for normal operation, if DD decided not to reset the link.
 *           NV0073_CTRL_DP_CMD_BANDWIDTH_TEST_YES
 *             This is to reset the link, if DD decided to uncommit the display because
 *             the link is no more required to be enabled, as in a DP compliance test.
 *       NV0073_CTRL_DP_CMD_LINK_CONFIG_CHECK_DISABLE
 *         Set if the client does not want link training to happen.
 *         This should ONLY be used for HW verification if necessary.
 *           NV0073_CTRL_DP_CMD_LINK_CONFIG_CHECK_DISABLE_FALSE
 *             This is normal production behaviour which shall perform
 *             pre link training checks such as if both rx and tx are capable
 *             of the requested config for lane and link bw.
 *           NV0073_CTRL_DP_CMD_LINK_CONFIG_CHECK_DISABLE_TRUE
 *             Set this value to bypass link config check, this should
 *             only be turned on for HW verif testing.  If _LINK_BANDWIDTH
 *             or _LANE_COUNT is set, RM will not check TX and DX caps.
 *       NV0073_CTRL_DP_CMD_FALLBACK_CONFIG
 *         Set if requested config by client fails and if link if being
 *         trained for the fallback config.
 *           NV0073_CTRL_DP_CMD_FALLBACK_CONFIG_FALSE
 *             This is the normal case when the link is being trained for a requested config.
 *           NV0073_CTRL_DP_CMD_LINK_CONFIG_CHECK_DISABLE_TRUE
 *             Set this value in case the link configuration for requested config fails
 *             and the link is being trained for a fallback config.
 *       NV0073_CTRL_DP_CMD_ENABLE_FEC
 *         Specifies whether RM should set NV_DPCD14_FEC_CONFIGURATION_FEC_READY
 *         before link training if client has determined that FEC is required(for DSC).
 *         If required to be enabled RM sets FEC enable bit in panel, start link training.
 *         Enabling/disabling FEC on GPU side is not done during Link training
 *         and RM Ctrl call NV0073_CTRL_CMD_DP_CONFIGURE_FEC has to be called
 *         explicitly to enable/disable FEC after LT(including PostLT LQA).
 *         If enabled, FEC would be disabled while powering down the link.
 *         Client has to make sure to account for 3% overhead of transmitting
 *         FEC symbols while calculating DP bandwidth.
 *           NV0073_CTRL_DP_CMD_ENABLE_FEC_FALSE
 *             This is the normal case when FEC is not required
 *           NV0073_CTRL_DP_CMD_ENABLE_FEC_TRUE
 *             Set this value in case FEC needs to be enabled
 *   data
 *     This parameter is an input and output to this command.
 *     Here are the current defined fields:
 *       NV0073_CTRL_DP_DATA_SET_LANE_COUNT
 *         This field specifies the desired setting for lane count.  A client
 *         may choose any lane count as long as it does not exceed the
 *         capability of DisplayPort receiver as indicated in the
 *         receiver capability field.  The valid values for this field are:
 *           NV0073_CTRL_DP_DATA_SET_LANE_COUNT_0
 *             For zero-lane configurations, link training is shut down.
 *           NV0073_CTRL_DP_DATA_SET_LANE_COUNT_1
 *             For one-lane configurations, lane0 is used.
 *           NV0073_CTRL_DP_DATA_SET_LANE_COUNT_2
 *             For two-lane configurations, lane0 and lane1 is used.
 *           NV0073_CTRL_DP_DATA_SET_LANE_COUNT_4
 *             For four-lane configurations, all lanes are used.
 *           NV0073_CTRL_DP_DATA_SET_LANE_COUNT_8
 *             For devices that supports 8-lane DP.
 *         On return, the lane count setting is returned which may be
 *         different from the requested input setting.
 *       NV0073_CTRL_DP_DATA_SET_LINK_BW
 *         This field specifies the desired setting for link bandwidth.  There
 *         are only four supported main link bandwidth settings.  The
 *         valid values for this field are:
 *           NV0073_CTRL_DP_DATA_SET_LINK_BW_1_62GBPS
 *           NV0073_CTRL_DP_DATA_SET_LINK_BW_2_70GBPS
 *           NV0073_CTRL_DP_DATA_SET_LINK_BW_5_40GBPS
 *           NV0073_CTRL_DP_DATA_SET_LINK_BW_8_10GBPS
 *         On return, the link bandwidth setting is returned which may be
 *         different from the requested input setting.
 *       NV0073_CTRL_DP_DATA_TARGET
 *         This field specifies which physical repeater or sink to be trained.
 *         Client should make sure
 *             1. Physical repeater should be targeted in order, start from the one closest to GPU.
 *             2. All physical repeater is properly trained before targets sink.
 *         The valid values for this field are:
 *           NV0073_CTRL_DP_DATA_TARGET_PHY_REPEATER_X
 *               'X' denotes physical repeater index. It's a 1-based index to
 *                   reserve 0 for _SINK.
 *               'X' can't be more than 8.
 *           NV0073_CTRL_DP_DATA_TARGET_SINK
 *   err
 *     This parameter specifies provides info regarding the outcome
 *     of this calling control call.  If zero, no errors were found.
 *     Otherwise, this parameter will specify the error detected.
 *     The valid parameter is broken down as follows:
 *        NV0073_CTRL_DP_ERR_SET_LANE_COUNT
 *          If set to _ERR, set lane count failed.
 *        NV0073_CTRL_DP_ERR_SET_LINK_BANDWIDTH
 *          If set to _ERR, set link bandwidth failed.
 *        NV0073_CTRL_DP_ERR_DISABLE_DOWNSPREAD
 *          If set to _ERR, disable downspread failed.
 *        NV0073_CTRL_DP_ERR_INVALID_PARAMETER
 *          If set to _ERR, at least one of the calling functions
 *          failed due to an invalid parameter.
 *        NV0073_CTRL_DP_ERR_SET_LINK_TRAINING
 *          If set to _ERR, link training failed.
 *        NV0073_CTRL_DP_ERR_TRAIN_PHY_REPEATER
 *          If set to _ERR, the operation to Link Train repeater is failed.
 *        NV0073_CTRL_DP_ERR_ENABLE_FEC
 *          If set to _ERR, the operation to enable FEC is failed.
 *   retryTimeMs
 *     This parameter is an output to this command.  In case of
 *     NVOS_STATUS_ERROR_RETRY return status, this parameter returns the time
 *     duration in milli-seconds after which client should retry this command.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NVOS_STATUS_ERROR_RETRY
 */

#define NV0073_CTRL_CMD_DP_CTRL                     (0x731343U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_CTRL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_CTRL_PARAMS_MESSAGE_ID (0x43U)

typedef struct NV0073_CTRL_DP_CTRL_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 cmd;
    NvU32 data;
    NvU32 err;
    NvU32 retryTimeMs;
    NvU32 eightLaneDpcdBaseAddr;
} NV0073_CTRL_DP_CTRL_PARAMS;

#define NV0073_CTRL_DP_CMD_SET_LANE_COUNT                           0:0
#define NV0073_CTRL_DP_CMD_SET_LANE_COUNT_FALSE                         (0x00000000U)
#define NV0073_CTRL_DP_CMD_SET_LANE_COUNT_TRUE                          (0x00000001U)
#define NV0073_CTRL_DP_CMD_SET_LINK_BW                              1:1
#define NV0073_CTRL_DP_CMD_SET_LINK_BW_FALSE                            (0x00000000U)
#define NV0073_CTRL_DP_CMD_SET_LINK_BW_TRUE                             (0x00000001U)
#define NV0073_CTRL_DP_CMD_DISABLE_DOWNSPREAD                       2:2
#define NV0073_CTRL_DP_CMD_DISABLE_DOWNSPREAD_FALSE                     (0x00000000U)
#define NV0073_CTRL_DP_CMD_DISABLE_DOWNSPREAD_TRUE                      (0x00000001U)
#define NV0073_CTRL_DP_CMD_UNUSED                                   3:3
#define NV0073_CTRL_DP_CMD_SET_FORMAT_MODE                          4:4
#define NV0073_CTRL_DP_CMD_SET_FORMAT_MODE_SINGLE_STREAM                (0x00000000U)
#define NV0073_CTRL_DP_CMD_SET_FORMAT_MODE_MULTI_STREAM                 (0x00000001U)
#define NV0073_CTRL_DP_CMD_FAST_LINK_TRAINING                       5:5
#define NV0073_CTRL_DP_CMD_FAST_LINK_TRAINING_NO                        (0x00000000U)
#define NV0073_CTRL_DP_CMD_FAST_LINK_TRAINING_YES                       (0x00000001U)
#define NV0073_CTRL_DP_CMD_NO_LINK_TRAINING                         6:6
#define NV0073_CTRL_DP_CMD_NO_LINK_TRAINING_NO                          (0x00000000U)
#define NV0073_CTRL_DP_CMD_NO_LINK_TRAINING_YES                         (0x00000001U)
#define NV0073_CTRL_DP_CMD_SET_ENHANCED_FRAMING                     7:7
#define NV0073_CTRL_DP_CMD_SET_ENHANCED_FRAMING_FALSE                   (0x00000000U)
#define NV0073_CTRL_DP_CMD_SET_ENHANCED_FRAMING_TRUE                    (0x00000001U)
#define NV0073_CTRL_DP_CMD_USE_DOWNSPREAD_SETTING                   8:8
#define NV0073_CTRL_DP_CMD_USE_DOWNSPREAD_SETTING_DEFAULT               (0x00000000U)
#define NV0073_CTRL_DP_CMD_USE_DOWNSPREAD_SETTING_FORCE                 (0x00000001U)
#define NV0073_CTRL_DP_CMD_SKIP_HW_PROGRAMMING                      9:9
#define NV0073_CTRL_DP_CMD_SKIP_HW_PROGRAMMING_NO                       (0x00000000U)
#define NV0073_CTRL_DP_CMD_SKIP_HW_PROGRAMMING_YES                      (0x00000001U)
#define NV0073_CTRL_DP_CMD_POST_LT_ADJ_REQ_GRANTED                10:10
#define NV0073_CTRL_DP_CMD_POST_LT_ADJ_REQ_GRANTED_NO                   (0x00000000U)
#define NV0073_CTRL_DP_CMD_POST_LT_ADJ_REQ_GRANTED_YES                  (0x00000001U)
#define NV0073_CTRL_DP_CMD_FAKE_LINK_TRAINING                     12:11
#define NV0073_CTRL_DP_CMD_FAKE_LINK_TRAINING_NO                        (0x00000000U)
#define NV0073_CTRL_DP_CMD_FAKE_LINK_TRAINING_DONOT_TOGGLE_TRANSMISSION (0x00000001U)
#define NV0073_CTRL_DP_CMD_FAKE_LINK_TRAINING_TOGGLE_TRANSMISSION_ON    (0x00000002U)
#define NV0073_CTRL_DP_CMD_TRAIN_PHY_REPEATER                     13:13
#define NV0073_CTRL_DP_CMD_TRAIN_PHY_REPEATER_NO                        (0x00000000U)
#define NV0073_CTRL_DP_CMD_TRAIN_PHY_REPEATER_YES                       (0x00000001U)
#define NV0073_CTRL_DP_CMD_FALLBACK_CONFIG                        14:14
#define NV0073_CTRL_DP_CMD_FALLBACK_CONFIG_FALSE                        (0x00000000U)
#define NV0073_CTRL_DP_CMD_FALLBACK_CONFIG_TRUE                         (0x00000001U)
#define NV0073_CTRL_DP_CMD_ENABLE_FEC                             15:15
#define NV0073_CTRL_DP_CMD_ENABLE_FEC_FALSE                             (0x00000000U)
#define NV0073_CTRL_DP_CMD_ENABLE_FEC_TRUE                              (0x00000001U)

#define NV0073_CTRL_DP_CMD_BANDWIDTH_TEST                         29:29
#define NV0073_CTRL_DP_CMD_BANDWIDTH_TEST_NO                            (0x00000000U)
#define NV0073_CTRL_DP_CMD_BANDWIDTH_TEST_YES                           (0x00000001U)
#define NV0073_CTRL_DP_CMD_LINK_CONFIG_CHECK_DISABLE              30:30
#define NV0073_CTRL_DP_CMD_LINK_CONFIG_CHECK_DISABLE_FALSE              (0x00000000U)
#define NV0073_CTRL_DP_CMD_LINK_CONFIG_CHECK_DISABLE_TRUE               (0x00000001U)
#define NV0073_CTRL_DP_CMD_DISABLE_LINK_CONFIG                    31:31
#define NV0073_CTRL_DP_CMD_DISABLE_LINK_CONFIG_FALSE                    (0x00000000U)
#define NV0073_CTRL_DP_CMD_DISABLE_LINK_CONFIG_TRUE                     (0x00000001U)

#define NV0073_CTRL_DP_DATA_SET_LANE_COUNT                          4:0
#define NV0073_CTRL_DP_DATA_SET_LANE_COUNT_0                            (0x00000000U)
#define NV0073_CTRL_DP_DATA_SET_LANE_COUNT_1                            (0x00000001U)
#define NV0073_CTRL_DP_DATA_SET_LANE_COUNT_2                            (0x00000002U)
#define NV0073_CTRL_DP_DATA_SET_LANE_COUNT_4                            (0x00000004U)
#define NV0073_CTRL_DP_DATA_SET_LANE_COUNT_8                            (0x00000008U)
#define NV0073_CTRL_DP_DATA_SET_LINK_BW                            15:8
#define NV0073_CTRL_DP_DATA_SET_LINK_BW_1_62GBPS                        (0x00000006U)
#define NV0073_CTRL_DP_DATA_SET_LINK_BW_2_16GBPS                        (0x00000008U)
#define NV0073_CTRL_DP_DATA_SET_LINK_BW_2_43GBPS                        (0x00000009U)
#define NV0073_CTRL_DP_DATA_SET_LINK_BW_2_70GBPS                        (0x0000000AU)
#define NV0073_CTRL_DP_DATA_SET_LINK_BW_3_24GBPS                        (0x0000000CU)
#define NV0073_CTRL_DP_DATA_SET_LINK_BW_4_32GBPS                        (0x00000010U)
#define NV0073_CTRL_DP_DATA_SET_LINK_BW_5_40GBPS                        (0x00000014U)
#define NV0073_CTRL_DP_DATA_SET_LINK_BW_8_10GBPS                        (0x0000001EU)
#define NV0073_CTRL_DP_DATA_SET_ENHANCED_FRAMING                  18:18
#define NV0073_CTRL_DP_DATA_SET_ENHANCED_FRAMING_NO                     (0x00000000U)
#define NV0073_CTRL_DP_DATA_SET_ENHANCED_FRAMING_YES                    (0x00000001U)
#define NV0073_CTRL_DP_DATA_TARGET                                22:19
#define NV0073_CTRL_DP_DATA_TARGET_SINK                                 (0x00000000U)
#define NV0073_CTRL_DP_DATA_TARGET_PHY_REPEATER_0                       (0x00000001U)
#define NV0073_CTRL_DP_DATA_TARGET_PHY_REPEATER_1                       (0x00000002U)
#define NV0073_CTRL_DP_DATA_TARGET_PHY_REPEATER_2                       (0x00000003U)
#define NV0073_CTRL_DP_DATA_TARGET_PHY_REPEATER_3                       (0x00000004U)
#define NV0073_CTRL_DP_DATA_TARGET_PHY_REPEATER_4                       (0x00000005U)
#define NV0073_CTRL_DP_DATA_TARGET_PHY_REPEATER_5                       (0x00000006U)
#define NV0073_CTRL_DP_DATA_TARGET_PHY_REPEATER_6                       (0x00000007U)
#define NV0073_CTRL_DP_DATA_TARGET_PHY_REPEATER_7                       (0x00000008U)

#define NV0073_CTRL_DP_ERR_SET_LANE_COUNT                           0:0
#define NV0073_CTRL_DP_ERR_SET_LANE_COUNT_NOERR                         (0x00000000U)
#define NV0073_CTRL_DP_ERR_SET_LANE_COUNT_ERR                           (0x00000001U)
#define NV0073_CTRL_DP_ERR_SET_LINK_BW                              1:1
#define NV0073_CTRL_DP_ERR_SET_LINK_BW_NOERR                            (0x00000000U)
#define NV0073_CTRL_DP_ERR_SET_LINK_BW_ERR                              (0x00000001U)
#define NV0073_CTRL_DP_ERR_DISABLE_DOWNSPREAD                       2:2
#define NV0073_CTRL_DP_ERR_DISABLE_DOWNSPREAD_NOERR                     (0x00000000U)
#define NV0073_CTRL_DP_ERR_DISABLE_DOWNSPREAD_ERR                       (0x00000001U)
#define NV0073_CTRL_DP_ERR_UNUSED                                   3:3
#define NV0073_CTRL_DP_ERR_CLOCK_RECOVERY                           4:4
#define NV0073_CTRL_DP_ERR_CLOCK_RECOVERY_NOERR                         (0x00000000U)
#define NV0073_CTRL_DP_ERR_CLOCK_RECOVERY_ERR                           (0x00000001U)
#define NV0073_CTRL_DP_ERR_CHANNEL_EQUALIZATION                     5:5
#define NV0073_CTRL_DP_ERR_CHANNEL_EQUALIZATION_NOERR                   (0x00000000U)
#define NV0073_CTRL_DP_ERR_CHANNEL_EQUALIZATION_ERR                     (0x00000001U)
#define NV0073_CTRL_DP_ERR_TRAIN_PHY_REPEATER                       6:6
#define NV0073_CTRL_DP_ERR_TRAIN_PHY_REPEATER_NOERR                     (0x00000000U)
#define NV0073_CTRL_DP_ERR_TRAIN_PHY_REPEATER_ERR                       (0x00000001U)
#define NV0073_CTRL_DP_ERR_ENABLE_FEC                               7:7
#define NV0073_CTRL_DP_ERR_ENABLE_FEC_NOERR                             (0x00000000U)
#define NV0073_CTRL_DP_ERR_ENABLE_FEC_ERR                               (0x00000001U)
#define NV0073_CTRL_DP_ERR_CR_DONE_LANE                            11:8
#define NV0073_CTRL_DP_ERR_CR_DONE_LANE_0_LANE                          (0x00000000U)
#define NV0073_CTRL_DP_ERR_CR_DONE_LANE_1_LANE                          (0x00000001U)
#define NV0073_CTRL_DP_ERR_CR_DONE_LANE_2_LANE                          (0x00000002U)
#define NV0073_CTRL_DP_ERR_CR_DONE_LANE_4_LANE                          (0x00000004U)
#define NV0073_CTRL_DP_ERR_CR_DONE_LANE_8_LANE                          (0x00000008U)
#define NV0073_CTRL_DP_ERR_EQ_DONE_LANE                           15:12
#define NV0073_CTRL_DP_ERR_EQ_DONE_LANE_0_LANE                          (0x00000000U)
#define NV0073_CTRL_DP_ERR_EQ_DONE_LANE_1_LANE                          (0x00000001U)
#define NV0073_CTRL_DP_ERR_EQ_DONE_LANE_2_LANE                          (0x00000002U)
#define NV0073_CTRL_DP_ERR_EQ_DONE_LANE_4_LANE                          (0x00000004U)
#define NV0073_CTRL_DP_ERR_EQ_DONE_LANE_8_LANE                          (0x00000008U)
#define NV0073_CTRL_DP_ERR_INVALID_PARAMETER                      30:30
#define NV0073_CTRL_DP_ERR_INVALID_PARAMETER_NOERR                      (0x00000000U)
#define NV0073_CTRL_DP_ERR_INVALID_PARAMETER_ERR                        (0x00000001U)
#define NV0073_CTRL_DP_ERR_LINK_TRAINING                          31:31
#define NV0073_CTRL_DP_ERR_LINK_TRAINING_NOERR                          (0x00000000U)
#define NV0073_CTRL_DP_ERR_LINK_TRAINING_ERR                            (0x00000001U)

/*
 * NV0073_CTRL_DP_LANE_DATA_PARAMS
 *
 * This structure provides lane characteristics.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the dfp
 *     caps should be returned.  The display ID must a dfp display.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   numLanes
 *      Indicates number of lanes for which the data is valid
 *   data
 *     This parameter is an input to this command.
 *     Here are the current defined fields:
 *       NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS
 *         This field specifies the preemphasis level set in the lane.
 *         The valid values for this field are:
 *           NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS_NONE
 *             No-preemphais for this lane.
 *           NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS_LEVEL1
 *             Preemphasis set to 3.5 dB.
 *           NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS_LEVEL2
 *             Preemphasis set to 6.0 dB.
 *           NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS_LEVEL3
 *             Preemphasis set to 9.5 dB.
 *       NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT
 *         This field specifies the drive current set in the lane.
 *         The valid values for this field are:
 *           NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT_LEVEL0
 *             Drive current level is set to 8 mA
 *           NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT_LEVEL1
 *             Drive current level is set to 12 mA
 *           NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT_LEVEL2
 *             Drive current level is set to 16 mA
 *           NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT_LEVEL3
 *             Drive current level is set to 24 mA
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 */
#define NV0073_CTRL_MAX_LANES                                           8U

typedef struct NV0073_CTRL_DP_LANE_DATA_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 numLanes;
    NvU32 data[NV0073_CTRL_MAX_LANES];
} NV0073_CTRL_DP_LANE_DATA_PARAMS;

#define NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS                   1:0
#define NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS_NONE    (0x00000000U)
#define NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS_LEVEL1  (0x00000001U)
#define NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS_LEVEL2  (0x00000002U)
#define NV0073_CTRL_DP_LANE_DATA_PREEMPHASIS_LEVEL3  (0x00000003U)
#define NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT                  3:2
#define NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT_LEVEL0 (0x00000000U)
#define NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT_LEVEL1 (0x00000001U)
#define NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT_LEVEL2 (0x00000002U)
#define NV0073_CTRL_DP_LANE_DATA_DRIVECURRENT_LEVEL3 (0x00000003U)

/*
 * NV0073_CTRL_CMD_GET_DP_LANE_DATA
 *
 * This command is used to get the current pre-emphasis and drive current
 * level values for the specified number of lanes.
 *
 * The command takes a NV0073_CTRL_DP_LANE_DATA_PARAMS structure as the
 * argument with the appropriate subDeviceInstance and displayId filled.
 * The arguments of this structure and the format of  preemphasis and drive-
 * current levels are described above.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 * NOTE: This control call is only for testing purposes and
 *       should not be used in normal DP operations. Preemphais
 *       and drive current level will be set during Link training
 *       in normal DP operations
 *
 */

#define NV0073_CTRL_CMD_DP_GET_LANE_DATA             (0x731345U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_GET_LANE_DATA_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_GET_LANE_DATA_PARAMS_MESSAGE_ID (0x45U)

typedef NV0073_CTRL_DP_LANE_DATA_PARAMS NV0073_CTRL_DP_GET_LANE_DATA_PARAMS;


/*
 * NV0073_CTRL_CMD_SET_DP_LANE_DATA
 *
 * This command is used to set the pre-emphasis and drive current
 * level values for the specified number of lanes.
 *
 * The command takes a NV0073_CTRL_DP_LANE_DATA_PARAMS structure as the
 * argument with the appropriate subDeviceInstance, displayId, number of
 * lanes, preemphasis and drive current values filled in.
 * The arguments of this structure and the format of  preemphasis and drive-
 * current levels are described above.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 * NOTE: This control call is only for testing purposes and
 *       should not be used in normal DP operations. Preemphais
 *       and drivecurrent will be set during Link training in
 *       normal DP operations
 *
 */

#define NV0073_CTRL_CMD_DP_SET_LANE_DATA (0x731346U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_SET_LANE_DATA_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_SET_LANE_DATA_PARAMS_MESSAGE_ID (0x46U)

typedef NV0073_CTRL_DP_LANE_DATA_PARAMS NV0073_CTRL_DP_SET_LANE_DATA_PARAMS;

/*
 * NV0073_CTRL_DP_CSTM
 *
 * This structure specifies -
 * A) 80 bit DP CSTM Test Pattern data for DP1.x (HBR2 + 8b/10b channel coding)
 *      The fields of this structure are to be specified as follows:
 *        field_31_0   takes bits 31:0
 *        field_63_32  takes bits 63:32
 *        field_95_64  takes bits 79:64
 * B) 264 bit DP CSTM Test Pattern data for DP2.x (128b/132b channel coding)
 *      The fields of this structure are to be specified as follows:
 *        field_31_0     contains bits 31:0
 *        field_63_32    contains bits 63:32
 *        field_95_64    contains bits 95:64
 *        field_127_95   contains bits 127:95
 *        field_159_128  contains bits 159:128
 *        field_191_160  contains bits 191:160
 *        field_223_192  contains bits 223:192
 *        field_255_224  contains bits 255:224
 *        field_263_256  contains bits 263:256
 */
typedef struct NV0073_CTRL_DP_CSTM {
    NvU32 field_31_0;
    NvU32 field_63_32;
    NvU32 field_95_64;
    NvU32 field_127_95;
    NvU32 field_159_128;
    NvU32 field_191_160;
    NvU32 field_223_192;
    NvU32 field_255_224;
    NvU32 field_263_256;
} NV0073_CTRL_DP_CSTM;

#define NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS_CSTM2    15:0
#define NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS_CSTM8     7:0


/*
 * NV0073_CTRL_DP_TESTPATTERN
 *
 * This structure specifies the possible test patterns available in display port.
 */

typedef struct NV0073_CTRL_DP_TESTPATTERN {
    NvU32 testPattern;
} NV0073_CTRL_DP_TESTPATTERN;

#define NV0073_CTRL_DP_TESTPATTERN_DATA                              4:0
#define NV0073_CTRL_DP_TESTPATTERN_DATA_NONE           (0x00000000U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_D10_2          (0x00000001U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_SERMP          (0x00000002U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_PRBS_7         (0x00000003U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_CSTM           (0x00000004U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_HBR2COMPLIANCE (0x00000005U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_CP2520PAT3     (0x00000006U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_TRAINING1      (0x00000007U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_TRAINING2      (0x00000008U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_TRAINING3      (0x00000009U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_TRAINING4      (0x0000000AU)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_CP2520PAT1     (0x0000000BU)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_128B132B_TPS1  (0x0000000CU)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_128B132B_TPS2  (0x0000000DU)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_PRBS_9         (0x0000000EU)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_PRBS_11        (0x0000000FU)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_PRBS_15        (0x00000010U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_PRBS_23        (0x00000011U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_PRBS_31        (0x00000012U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_SQNUM          (0x00000013U)
#define NV0073_CTRL_DP_TESTPATTERN_DATA_CSTM_264       (0x00000014U)

/*
 * NV0073_CTRL_CMD_DP_SET_TESTPATTERN
 *
 * This command forces the main link to output the selected test patterns
 * supported in DP specs.
 *
 * The command takes a NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS structure as the
 * argument with the appropriate subDeviceInstance, displayId and test pattern
 * to be set as inputs.
 * The arguments of this structure and the format of  test patterns are
 * described above.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the dfp
 *     caps should be returned.  The display ID must a dfp display.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   testPattern
 *     This parameter is of type NV0073_CTRL_DP_TESTPATTERN and specifies
 *     the testpattern to set on displayport. The format of this structure
 *     is described above.
 *   laneMask
 *     This parameter specifies the bit mask of DP lanes on which test
 *     pattern is to be applied.
 *   lower
 *     This parameter specifies the lower 64 bits of the CSTM test pattern
 *   upper
 *     This parameter specifies the upper 16 bits of the CSTM test pattern
 *   bIsHBR2
 *     This Boolean parameter is set to TRUE if HBR2 compliance test is
 *     being performed.
  *   bSkipLaneDataOverride
 *      skip override of pre-emp and drive current
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 * NOTE: This control call is only for testing purposes and
 *       should not be used in normal DP operations. Preemphais
 *       and drivecurrent will be set during Link training in
 *       normal DP operations
 *
 */

#define NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS_MESSAGE_ID (0x47U)

typedef struct NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS {
    NvU32                      subDeviceInstance;
    NvU32                      displayId;
    NV0073_CTRL_DP_TESTPATTERN testPattern;
    NvU8                       laneMask;
    NV0073_CTRL_DP_CSTM        cstm;
    NvBool                     bIsHBR2;
    NvBool                     bSkipLaneDataOverride;
} NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS;

#define NV0073_CTRL_CMD_DP_SET_TESTPATTERN (0x731347U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS_MESSAGE_ID" */

/*
 * NV0073_CTRL_CMD_GET_DP_TESTPATTERN
 *
 * This command returns the current test pattern set on the main link of
 * Display Port.
 *
 * The command takes a NV0073_CTRL_DP_GET_TESTPATTERN_PARAMS structure as the
 * argument with the appropriate subDeviceInstance, displayId as inputs and
 * returns the current test pattern in testPattern field of the structure.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the dfp
 *     caps should be returned.  The display ID must a dfp display.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   testPattern
 *     This parameter is of type NV0073_CTRL_DP_TESTPATTERN and specifies the
 *     testpattern set on displayport. The format of this structure is
 *     described above.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 * NOTE: This control call is only for testing purposes and
 *       should not be used in normal DP operations.
 *
 */

#define NV0073_CTRL_DP_GET_TESTPATTERN_PARAMS_MESSAGE_ID (0x48U)

typedef struct NV0073_CTRL_DP_GET_TESTPATTERN_PARAMS {
    NvU32                      subDeviceInstance;
    NvU32                      displayId;
    NV0073_CTRL_DP_TESTPATTERN testPattern;
} NV0073_CTRL_DP_GET_TESTPATTERN_PARAMS;


#define NV0073_CTRL_CMD_DP_GET_TESTPATTERN  (0x731348U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_GET_TESTPATTERN_PARAMS_MESSAGE_ID" */

/*
 * NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA
 *
 * This structure specifies the Pre-emphasis/Drive Current/Postcursor2/TxPu information
 * for a display port device. These are the the current values that RM is
 * using to map the levels for Pre-emphasis and Drive Current for Link
 * Training.
 *   preEmphasis
 *     This field specifies the preemphasis values.
 *   driveCurrent
 *     This field specifies the driveCurrent values.
 *   postcursor2
 *     This field specifies the postcursor2 values.
 *   TxPu
 *     This field specifies the pull-up current source drive values.
 */
#define NV0073_CTRL_MAX_DRIVECURRENT_LEVELS 4U
#define NV0073_CTRL_MAX_PREEMPHASIS_LEVELS  4U
#define NV0073_CTRL_MAX_POSTCURSOR2_LEVELS  4U

typedef struct NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_1 {
    NvU32 preEmphasis;
    NvU32 driveCurrent;
    NvU32 postCursor2;
    NvU32 TxPu;
} NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_1;

typedef NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_1 NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_SLICE1[NV0073_CTRL_MAX_PREEMPHASIS_LEVELS];

typedef NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_SLICE1 NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_SLICE2[NV0073_CTRL_MAX_DRIVECURRENT_LEVELS];

typedef NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_SLICE2 NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA[NV0073_CTRL_MAX_POSTCURSOR2_LEVELS];


/*
 * NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA
 *
 * This command is used to override the Pre-emphasis/Drive Current/PostCursor2/TxPu
 * data in the RM.
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the digital display for which the
 *     data should be returned.  The display ID must a digital display.
 *     If more than one displayId bit is set or the displayId is not a DP,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   dpData
 *     This parameter is of type NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA
 *     and specifies the Pre-emphasis/Drive Current/Postcursor2/TxPu information
 *     for a display port device.
 * The command takes a NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS
 * structure as the argument with the appropriate subDeviceInstance, displayId,
 * and dpData.  The fields of this structure are described above.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS_MESSAGE_ID (0x51U)

typedef struct NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS {
    NvU32                                                    subDeviceInstance;
    NvU32                                                    displayId;
    NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA dpData;
} NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS;

#define NV0073_CTRL_CMD_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA (0x731351U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS_MESSAGE_ID" */

/*
 * NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA
 *
 * This command is used to get the Pre-emphasis/Drive Current/PostCursor2/TxPu data.
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the digital display for which the
 *     data should be returned.  The display ID must a digital display.
 *     If more than one displayId bit is set or the displayId is not a DP,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 * The command takes a NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS
 * structure as the argument with the appropriate subDeviceInstance, displayId,
 * and dpData.  The fields of this structure are described above.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS_MESSAGE_ID (0x52U)

typedef struct NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS {
    NvU32                                                    subDeviceInstance;
    NvU32                                                    displayId;
    NV0073_CTRL_DP_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA dpData;
} NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS;

#define NV0073_CTRL_CMD_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA (0x731352U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS_MESSAGE_ID" */



/*
 * NV0073_CTRL_CMD_DP_MAIN_LINK_CTRL
 *
 * This command is used to set various Main Link configurations for
 * the specified displayId such as powering up/down Main Link.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the DP display which owns
 *     the Main Link to be adjusted.  The display ID must a DP display
 *     as determined with the NV0073_CTRL_CMD_SPECIFIC_GET_TYPE command.
 *     If more than one displayId bit is set or the displayId is not a DP,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   ctrl
 *     Here are the current defined fields:
 *       NV0073_CTRL_DP_MAIN_LINK_CTRL_POWER_STATE_POWERDOWN
 *         This value will power down Main Link.
 *       NV0073_CTRL_DP_MAIN_LINK_CTRL_POWER_STATE_POWERUP
 *         This value will power up Main Link.
 *
*/
#define NV0073_CTRL_CMD_DP_MAIN_LINK_CTRL                       (0x731356U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS_MESSAGE_ID (0x56U)

typedef struct NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 ctrl;
} NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS;

#define  NV0073_CTRL_DP_MAIN_LINK_CTRL_POWER_STATE                          0:0
#define NV0073_CTRL_DP_MAIN_LINK_CTRL_POWER_STATE_POWERDOWN (0x00000000U)
#define NV0073_CTRL_DP_MAIN_LINK_CTRL_POWER_STATE_POWERUP   (0x00000001U)

/*
 * NV0073_CTRL_CMD_DP_SET_AUDIO_MUTESTREAM
 *
 * This command sets the current audio mute state on the main link of Display Port
 *
 * The command takes a NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS structure as the
 * argument with the appropriate subDeviceInstance, displayId as inputs and whether to enable
 * or disable mute in the parameter - mute.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the display for which the audio stream
 *     state should be returned.  The display ID must a DP display.
 *     If the display ID is invalid or if it is not a DP display,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   mute
 *     This parameter is an input to this command.
 *     Here are the current defined values:
 *       NV0073_CTRL_DP_AUDIO_MUTESTREAM_MUTE_DISABLE
 *         Audio mute will be disabled.
 *       NV0073_CTRL_DP_AUDIO_MUTESTREAM_MUTE_ENABLE
 *         Audio mute will be enabled.
 *       NV0073_CTRL_DP_AUDIO_MUTESTREAM_MUTE_AUTO
 *         Audio mute will be automatically controlled by hardware.
 *
 *      Note:  Any other value for mute in NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS is not allowed and
 *              the API will return an error.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 *
 */
#define NV0073_CTRL_CMD_DP_SET_AUDIO_MUTESTREAM             (0x731359U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS_MESSAGE_ID (0x59U)

typedef struct NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 mute;
} NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS;

#define NV0073_CTRL_DP_AUDIO_MUTESTREAM_MUTE_DISABLE (0x00000000U)
#define NV0073_CTRL_DP_AUDIO_MUTESTREAM_MUTE_ENABLE  (0x00000001U)
#define NV0073_CTRL_DP_AUDIO_MUTESTREAM_MUTE_AUTO    (0x00000002U)

/*
 * NV0073_CTRL_CMD_DP_ASSR_CTRL
 *
 * This command is used to control and query DisplayPort ASSR
 * settings for the specified displayId.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the DP display which owns
 *     the Main Link to be adjusted.  The display ID must a DP display
 *     as determined with the NV0073_CTRL_CMD_SPECIFIC_GET_TYPE command.
 *     If more than one displayId bit is set or the displayId is not a DP,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   cmd
 *     This input parameter specifies the command to execute.  Legal
 *     values for this parameter include:
 *       NV0073_CTRL_DP_ASSR_CMD_QUERY_STATE
 *         This field can be used to query ASSR state. When used the ASSR
 *         state value is returned in the data parameter.
 *       NV0073_CTRL_DP_ASSR_CMD_DISABLE
 *         This field can be used to control the ASSR disable state.
 *       NV0073_CTRL_DP_ASSR_CMD_FORCE_STATE
 *         This field can be used to control ASSR State without looking at
 *         whether the display supports it. Used in conjunction with
 *         Fake link training. Note that this updates the state on the
 *         source side only. The sink is assumed to be configured for ASSR
 *         by the client (DD).
 *   data
 *     This parameter specifies the data associated with the cmd
 *     parameter.
 *       NV0073_CTRL_DP_ASSR_DATA_STATE_ENABLED
 *         This field indicates the state of ASSR when queried using cmd
 *         parameter. When used to control the State, it indicates whether
 *         ASSR should be enabled or disabled.
 *           NV0073_CTRL_DP_ASSR_DATA_STATE_ENABLED_NO
 *             When queried this flag indicates that ASSR is not enabled on the sink.
 *             When used as the data for CMD_FORCE_STATE, it requests ASSR to
 *             to be disabled on the source side.
 *           NV0073_CTRL_DP_ASSR_DATA_STATE_ENABLED_YES
 *             When queried this flag indicates that ASSR is not enabled on the sink.
 *             When used as the data for CMD_FORCE_STATE, it requests ASSR to
 *             to be enabled on the source side.
 *   err
 *     This output parameter specifies any errors associated with the cmd
 *     parameter.
 *       NV0073_CTRL_DP_ASSR_ERR_CAP
 *         This field indicates the error pertaining to ASSR capability of
 *         the sink device.
 *           NV0073_CTRL_DP_ASSR_ERR_CAP_NOERR
 *             This flag indicates there is no error.
 *           NV0073_CTRL_DP_ASSR_ERR_CAP_ERR
 *             This flag indicates that the sink is not ASSR capable.
 *
 *  Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV0073_CTRL_CMD_DP_ASSR_CTRL                 (0x73135aU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_ASSR_CTRL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_ASSR_CTRL_PARAMS_MESSAGE_ID (0x5AU)

typedef struct NV0073_CTRL_DP_ASSR_CTRL_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 cmd;
    NvU32 data;
    NvU32 err;
} NV0073_CTRL_DP_ASSR_CTRL_PARAMS;

#define  NV0073_CTRL_DP_ASSR_CMD                                31:0
#define NV0073_CTRL_DP_ASSR_CMD_QUERY_STATE             (0x00000001U)
#define NV0073_CTRL_DP_ASSR_CMD_DISABLE                 (0x00000002U)
#define NV0073_CTRL_DP_ASSR_CMD_FORCE_STATE             (0x00000003U)
#define NV0073_CTRL_DP_ASSR_CMD_ENABLE                  (0x00000004U)
#define  NV0073_CTRL_DP_ASSR_DATA_STATE_ENABLED                  0:0
#define NV0073_CTRL_DP_ASSR_DATA_STATE_ENABLED_NO       (0x00000000U)
#define NV0073_CTRL_DP_ASSR_DATA_STATE_ENABLED_YES      (0x00000001U)
#define  NV0073_CTRL_DP_ASSR_ERR_CAP                             0:0
#define NV0073_CTRL_DP_ASSR_ERR_CAP_NOERR               (0x00000000U)
#define NV0073_CTRL_DP_ASSR_ERR_CAP_ERR                 (0x00000001U)

/*
 * NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID
 *
 * This command is used to assign a displayId from the free pool
 * to a specific AUX Address in a DP 1.2 topology.  The topology
 * is uniquely identified by the DisplayId of the DP connector.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This is the DisplayId of the DP connector to which the topology
 *     is rooted.
 *   preferredDisplayId
 *      Client can sent a preferredDisplayID which RM can use during allocation
 *      if available. If this Id is a part of allDisplayMask in RM then we return
 *      a free available Id to the client. However, if this is set to
 *      NV0073_CTRL_CMD_DP_INVALID_PREFERRED_DISPLAY_ID then we return allDisplayMask value.
 *   useBFM
 *      Set to true if DP-BFM is used during emulation/RTL Sim.
 *
 *   [out] displayIdAssigned
 *     This is the out field that will receive the new displayId.  If the
 *     function fails this is guaranteed to be 0.
 *   [out] allDisplayMask
 *      This is allDisplayMask RM variable which is returned only when
 *      preferredDisplayId is set to NV0073_CTRL_CMD_DP_INVALID_PREFERRED_DISPLAY_ID
 *
 *
 *  Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *
 */
#define NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID  (0x73135bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS_MESSAGE_ID" */

/*
 *  There cannot be more than 128 devices in a topology (also by DP 1.2 specification)
 *  NOTE: Temporarily lowered to pass XAPI RM tests. Should be reevaluated!
 */
#define NV0073_CTRL_CMD_DP_MAX_TOPOLOGY_NODES           120U
#define NV0073_CTRL_CMD_DP_INVALID_PREFERRED_DISPLAY_ID 0xffffffffU

#define NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS_MESSAGE_ID (0x5BU)

typedef struct NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  preferredDisplayId;

    NvBool force;
    NvBool useBFM;

    NvU32  displayIdAssigned;
    NvU32  allDisplayMask;
} NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS;

/*
 * NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID
 *
 * This command is used to return a multistream displayid to the unused pool.
 * You must not call this function while either the ARM or ASSEMBLY state cache
 * refers to this display-id.  The head must not be attached.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This is the displayId to free.
 *
 *  Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_PARAM_STRUCT
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *
 *
 */
#define NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID (0x73135cU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS_MESSAGE_ID (0x5CU)

typedef struct NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
} NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS;

/*
 * NV0073_CTRL_CMD_DP_GET_LINK_CONFIG
 *
 * This command is used to query DisplayPort link config
 * settings on the transmitter side.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the DP display which owns
 *     the Main Link to be queried.
 *     If more than one displayId bit is set or the displayId is not a DP,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   laneCount
 *     Number of lanes the DP transmitter hardware is set up to drive.
 *   linkBW
 *     The BW of each lane that the DP transmitter hardware is set up to drive.
 *     The values returned will be according to the DP specifications.
 *   dp2LinkBW
 *     Current BW of each lane that the DP transmitter hardware is set up to drive is UHBR.
 *     The values returned will be using 10M convention.
 *
 *   Note:
 *   linkBW and dp2LinkBw are mutual exclusive. Only one of the value will be non-zero.
 *
 */
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG (0x731360U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS_MESSAGE_ID (0x60U)

typedef struct NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 laneCount;
    NvU32 linkBW;
    NvU32 dp2LinkBW;
} NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS;

#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT                          3:0
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_0        (0x00000000U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_1        (0x00000001U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_2        (0x00000002U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_4        (0x00000004U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW                             3:0
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW_1_62GBPS    (0x00000006U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW_2_70GBPS    (0x0000000aU)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW_5_40GBPS    (0x00000014U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW_8_10GBPS    (0x0000001EU)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW_2_16GBPS    (0x00000008U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW_2_43GBPS    (0x00000009U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW_3_24GBPS    (0x0000000CU)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW_4_32GBPS    (0x00000010U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LINK_BW_6_75GBPS    (0x00000019U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW                         15:0
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW_1_62GBPS (0x000000A2U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW_2_70GBPS (0x0000010EU)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW_5_40GBPS (0x0000021CU)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW_8_10GBPS (0x0000032AU)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW_2_16GBPS (0x000000D8U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW_2_43GBPS (0x000000F3U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW_3_24GBPS (0x00000114U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW_4_32GBPS (0x000001B0U)
#define NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_DP2LINK_BW_6_75GBPS (0x000002A3U)


/*
 * NV0073_CTRL_CMD_DP_GET_EDP_DATA
 *
 * This command is used to query Embedded DisplayPort information.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     This parameter specifies the ID of the eDP display which owns
 *     the Main Link to be queried.
 *     If more than one displayId bit is set or the displayId is not a eDP,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   data
 *     This output parameter specifies the data associated with the eDP display.
 *     It is only valid if this function returns NV_OK.
 *       NV0073_CTRL_CMD_DP_GET_EDP_DATA_PANEL_POWER
 *         This field indicates the state of the eDP panel power.
 *           NV0073_CTRL_CMD_DP_GET_EDP_DATA_PANEL_POWER_OFF
 *             This eDP panel is powered off.
 *           NV0073_CTRL_CMD_DP_GET_EDP_DATA_PANEL_POWER_ON
 *             This eDP panel is powered on.
 *       NV0073_CTRL_CMD_DP_GET_EDP_DATA_DPCD_POWER_OFF
 *         This field tells the client if DPCD power off command
 *         should be used for the current eDP panel.
 *           NV0073_CTRL_DP_GET_EDP_DATA_DPCD_POWER_OFF_ENABLE
 *             This eDP panel can use DPCD to power off the panel.
 *           NV0073_CTRL_DP_GET_EDP_DATA_DPCD_POWER_OFF_DISABLE
 *             This eDP panel cannot use DPCD to power off the panel.
 *       NV0073_CTRL_DP_GET_EDP_DATA_DPCD_SET_POWER
 *         This field tells the client current eDP panel DPCD SET_POWER (0x600) status
 *            NV0073_CTRL_DP_GET_EDP_DATA_DPCD_SET_POWER_D0
 *              This eDP panel is current up and in full power mode.
 *            NV0073_CTRL_DP_GET_EDP_DATA_DPCD_SET_POWER_D3
 *              This eDP panel is current standby.
 */
#define NV0073_CTRL_CMD_DP_GET_EDP_DATA                        (0x731361U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_GET_EDP_DATA_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_GET_EDP_DATA_PARAMS_MESSAGE_ID (0x61U)

typedef struct NV0073_CTRL_DP_GET_EDP_DATA_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 data;
} NV0073_CTRL_DP_GET_EDP_DATA_PARAMS;

#define NV0073_CTRL_DP_GET_EDP_DATA_PANEL_POWER                                0:0
#define NV0073_CTRL_DP_GET_EDP_DATA_PANEL_POWER_OFF        (0x00000000U)
#define NV0073_CTRL_DP_GET_EDP_DATA_PANEL_POWER_ON         (0x00000001U)
#define NV0073_CTRL_DP_GET_EDP_DATA_DPCD_POWER_OFF                             1:1
#define NV0073_CTRL_DP_GET_EDP_DATA_DPCD_POWER_OFF_ENABLE  (0x00000000U)
#define NV0073_CTRL_DP_GET_EDP_DATA_DPCD_POWER_OFF_DISABLE (0x00000001U)
#define NV0073_CTRL_DP_GET_EDP_DATA_DPCD_POWER_STATE                           2:2
#define NV0073_CTRL_DP_GET_EDP_DATA_DPCD_POWER_STATE_D0    (0x00000000U)
#define NV0073_CTRL_DP_GET_EDP_DATA_DPCD_POWER_STATE_D3    (0x00000001U)
/*
 * NV0073_CTRL_CMD_DP_CONFIG_STREAM
 *
 * This command sets various multi/single stream related params for
 * for a given head.
 *
 *   subDeviceInstance
 *          This parameter specifies the subdevice instance within the
 *          NV04_DISPLAY_COMMON parent device to which the operation should be
 *          directed. This parameter must specify a value between zero and the
 *          total number of subdevices within the parent device.  This parameter
 *          should be set to zero for default behavior.
 *      Head
 *          Specifies the head index for the stream.
 *      sorIndex
 *          Specifies the SOR index for the stream.
 *      dpLink
 *          Specifies the DP link: either 0 or 1 (A , B)
 *      bEnableOverride
 *          Specifies whether we're manually configuring this stream.
 *          If not set, none of the remaining settings have any effect.
 *      bMST
 *          Specifies whether in Multistream or Singlestream mode.
 *      MST/SST
 *          Structures for passing in either Multistream or Singlestream params
 *      slotStart
 *          Specifies the start value of the timeslot
 *      slotEnd
 *          Specifies the end value of the timeslot
 *      PBN
 *          Specifies the PBN for the timeslot.
 *      minHBlank
 *          Specifies the min HBlank
 *      minVBlank
 *          Specifies the min VBlank
 *      sendACT   -- deprecated. A new control call has been added.
 *          Specifies whether ACT has to be sent or not.
 *      tuSize
 *          Specifies TU size value
 *      watermark
 *          Specifies stream watermark.
 *      bEnableTwoHeadOneOr
 *          Whether two head one OR is enabled. If this is set then RM will
 *          replicate SF settings of Master head on Slave head. Head index
 *          passed should be of Master Head.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC: when this command has already been called
 *
 */
#define NV0073_CTRL_CMD_DP_CONFIG_STREAM                   (0x731362U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS_MESSAGE_ID (0x62U)

typedef struct NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  head;
    NvU32  sorIndex;
    NvU32  dpLink;

    NvBool bEnableOverride;
    NvBool bMST;
    NvU32  singleHeadMultistreamMode;
    NvU32  hBlankSym;
    NvU32  vBlankSym;
    NvU32  colorFormat;
    NvBool bEnableTwoHeadOneOr;

    struct {
        NvU32  slotStart;
        NvU32  slotEnd;
        NvU32  PBN;
        NvU32  Timeslice;
        NvBool sendACT;          // deprecated -Use NV0073_CTRL_CMD_DP_SEND_ACT
        NvU32  singleHeadMSTPipeline;
        NvBool bEnableAudioOverRightPanel;
    } MST;

    struct {
        NvBool bEnhancedFraming;
        NvU32  tuSize;
        NvU32  waterMark;
        NvBool bEnableAudioOverRightPanel;
    } SST;
} NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS;

/*
 * NV0073_CTRL_CMD_DP_SET_RATE_GOV
 *
 * This command enables rate governing for a MST.
 *
 *      subDeviceInstance
 *          This parameter specifies the subdevice instance within the
 *          NV04_DISPLAY_COMMON parent device to which the operation should be
 *          directed. This parameter must specify a value between zero and the
 *          total number of subdevices within the parent device.  This parameter
 *          should be set to zero for default behavior.
 *      Head
 *          Specifies the head index for the stream.
 *      sorIndex
 *          Specifies the SOR index for the stream.
 *      flags
 *          Specifies Rate Governing, trigger type and wait on trigger and operation type.
 *
 *     _FLAGS_OPERATION: whether this control call should program or check for status of previous operation.
 *
 *     _FLAGS_STATUS: Out only. Caller should check the status for _FLAGS_OPERATION_CHECK_STATUS through
 *                    this bit.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC: when this command has already been called
 *
 */
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV (0x731363U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS_MESSAGE_ID (0x63U)

typedef struct NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 head;
    NvU32 sorIndex;
    NvU32 flags;
} NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS;

#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_ENABLE_RG                0:0
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_ENABLE_RG_OFF          (0x00000000U)
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_ENABLE_RG_ON           (0x00000001U)
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_TRIGGER_MODE             1:1
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_TRIGGER_MODE_LOADV     (0x00000000U)
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_TRIGGER_MODE_IMMEDIATE (0x00000001U)
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_WAIT_TRIGGER             2:2
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_WAIT_TRIGGER_OFF       (0x00000000U)
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_WAIT_TRIGGER_ON        (0x00000001U)
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_OPERATION                3:3
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_OPERATION_PROGRAM      (0x00000000U)
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_OPERATION_CHECK_STATUS (0x00000001U)
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_STATUS                   31:31
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_STATUS_FAIL            (0x00000000U)
#define NV0073_CTRL_CMD_DP_SET_RATE_GOV_FLAGS_STATUS_PASS            (0x00000001U)

/*
 * NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT
 *
 *  This call is used by the displayport library.  Once
 *  all of the platforms have ported, this call will be
 *  deprecated and made the default behavior.
 *
 *   Disables automatic watermark programming
 *   Disables automatic DP IRQ handling (CP IRQ)
 *   Disables automatic retry on defers
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *
 *
 *  Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 *
 */
#define NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT                    (0x731365U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS_MESSAGE_ID (0x65U)

typedef struct NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS {
    NvU32 subDeviceInstance;
} NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS;

/*
 * NV0073_CTRL_CMD_DP_SET_ECF
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   sorIndex
 *     This parameter specifies the Index of sor for which ecf
 *     should be updated.
 *   ecf
 *      This parameter has the ECF bit mask.
 *
 *  Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *
 */
#define NV0073_CTRL_CMD_DP_SET_ECF (0x731366U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_SET_ECF_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_SET_ECF_PARAMS_MESSAGE_ID (0x66U)

typedef struct NV0073_CTRL_CMD_DP_SET_ECF_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  sorIndex;
    NV_DECLARE_ALIGNED(NvU64 ecf, 8);
    NvBool bForceClearEcf;
    NvBool bAddStreamBack;
} NV0073_CTRL_CMD_DP_SET_ECF_PARAMS;

/*
 * NV0073_CTRL_CMD_DP_SEND_ACT
 *
 * This command sends ACT.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *
 *   displayId
 *      Specifies the root port displayId for which the trigger has to be done.
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_GENERIC: when this command has already been called
 *
 */
#define NV0073_CTRL_CMD_DP_SEND_ACT (0x731367U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS_MESSAGE_ID (0x67U)

typedef struct NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
} NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS;



/*
 * NV0073_CTRL_CMD_DP_GET_CAPS
 *
 * This command returns the following info
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   sorIndex
 *     Specifies the SOR index.
 *   dpVersionsSupported
 *     Specified the DP versions supported by the GPU
 *   UHBRSupportedByGpu
 *     Bitmask to specify the UHBR link rates supported by the GPU.
 *   bIsMultistreamSupported
 *     Returns NV_TRUE if MST is supported by the GPU else NV_FALSE
 *   bIsSCEnabled
 *     Returns NV_TRUE if Stream cloning is supported by the GPU else NV_FALSE
 *   maxLinkRate
 *     Returns Maximum allowed orclk for DP mode of SOR
 *     1 signifies 5.40(HBR2), 2 signifies 2.70(HBR), 3 signifies 1.62(RBR)
 *   bHasIncreasedWatermarkLimits
 *     Returns NV_TRUE if the GPU uses higher watermark limits, else NV_FALSE
 *   bIsPC2Disabled
 *     Returns NV_TRUE if VBIOS flag to disable PostCursor2 is set, else NV_FALSE
 *   bFECSupported
 *     Returns NV_TRUE if GPU supports FEC, else NV_FALSE
 *   bIsTrainPhyRepeater
 *     Returns NV_TRUE if LTTPR Link Training feature is set
 *   bOverrideLinkBw
 *     Returns NV_TRUE if DFP limits defined in DCB have to be honored, else NV_FALSE
 *   bUseRgFlushSequence
 *     Returns NV_TRUE if GPU uses the new RG flush design
 *   bSupportDPDownSpread
 *     Returns NV_TRUE if GPU support downspread.
 *
 *  DSC caps
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_NOT_SUPPORTED
 *
 */

#define NV0073_CTRL_CMD_DP_GET_CAPS   (0x731369U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS_MESSAGE_ID (0x69U)

typedef struct NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS {
    NvU32                          subDeviceInstance;
    NvU32                          sorIndex;
    NvU32                          maxLinkRate;
    NvU32                          dpVersionsSupported;
    NvU32                          UHBRSupportedByGpu;
    NvBool                         bIsMultistreamSupported;
    NvBool                         bIsSCEnabled;
    NvBool                         bHasIncreasedWatermarkLimits;
    NvBool                         bIsPC2Disabled;
    NvBool                         isSingleHeadMSTSupported;
    NvBool                         bFECSupported;
    NvBool                         bIsTrainPhyRepeater;
    NvBool                         bOverrideLinkBw;
    NvBool                         bUseRgFlushSequence;
    NvBool                         bSupportDPDownSpread;
    NV0073_CTRL_CMD_DSC_CAP_PARAMS DSC;
} NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS;

#define NV0073_CTRL_CMD_DP_GET_CAPS_DP_VERSIONS_SUPPORTED_DP1_2                0:0
#define NV0073_CTRL_CMD_DP_GET_CAPS_DP_VERSIONS_SUPPORTED_DP1_2_NO              (0x00000000U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DP_VERSIONS_SUPPORTED_DP1_2_YES             (0x00000001U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DP_VERSIONS_SUPPORTED_DP1_4                1:1
#define NV0073_CTRL_CMD_DP_GET_CAPS_DP_VERSIONS_SUPPORTED_DP1_4_NO              (0x00000000U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DP_VERSIONS_SUPPORTED_DP1_4_YES             (0x00000001U)


#define NV0073_CTRL_CMD_DP_GET_CAPS_MAX_LINK_RATE                           2:0
#define NV0073_CTRL_CMD_DP_GET_CAPS_MAX_LINK_RATE_NONE                          (0x00000000U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_MAX_LINK_RATE_1_62                          (0x00000001U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_MAX_LINK_RATE_2_70                          (0x00000002U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_MAX_LINK_RATE_5_40                          (0x00000003U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_MAX_LINK_RATE_8_10                          (0x00000004U)


#define NV0073_CTRL_CMD_DP_GET_CAPS_DSC_ENCODER_COLOR_FORMAT_RGB                (0x00000001U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_444        (0x00000002U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422 (0x00000004U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420 (0x00000008U)

#define NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_16           (0x00000001U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_8            (0x00000002U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_4            (0x00000003U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_2            (0x00000004U)
#define NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1              (0x00000005U)

#define NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES                                   (0x73136aU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_MSA_PROPERTIES_SYNC_POLARITY_LOW                     (0U)
#define NV0073_CTRL_CMD_DP_MSA_PROPERTIES_SYNC_POLARITY_HIGH                    (1U)

typedef struct NV0073_CTRL_DP_MSA_PROPERTIES_MASK {
    NvU8   miscMask[2];
    NvBool bRasterTotalHorizontal;
    NvBool bRasterTotalVertical;
    NvBool bActiveStartHorizontal;
    NvBool bActiveStartVertical;
    NvBool bSurfaceTotalHorizontal;
    NvBool bSurfaceTotalVertical;
    NvBool bSyncWidthHorizontal;
    NvBool bSyncPolarityHorizontal;
    NvBool bSyncHeightVertical;
    NvBool bSyncPolarityVertical;
    NvBool bReservedEnable[3];
} NV0073_CTRL_DP_MSA_PROPERTIES_MASK;

typedef struct NV0073_CTRL_DP_MSA_PROPERTIES_VALUES {
    NvU8  misc[2];
    NvU16 rasterTotalHorizontal;
    NvU16 rasterTotalVertical;
    NvU16 activeStartHorizontal;
    NvU16 activeStartVertical;
    NvU16 surfaceTotalHorizontal;
    NvU16 surfaceTotalVertical;
    NvU16 syncWidthHorizontal;
    NvU16 syncPolarityHorizontal;
    NvU16 syncHeightVertical;
    NvU16 syncPolarityVertical;
    NvU8  reserved[3];
} NV0073_CTRL_DP_MSA_PROPERTIES_VALUES;

#define NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS_MESSAGE_ID (0x6AU)

typedef struct NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS {
    NvU32                                subDeviceInstance;
    NvU32                                displayId;
    NvBool                               bEnableMSA;
    NvBool                               bStereoPhaseInverse;
    NvBool                               bCacheMsaOverrideForNextModeset;
    NV0073_CTRL_DP_MSA_PROPERTIES_MASK   featureMask;
    NV0073_CTRL_DP_MSA_PROPERTIES_VALUES featureValues;
    NV_DECLARE_ALIGNED(struct NV0073_CTRL_DP_MSA_PROPERTIES_VALUES *pFeatureDebugValues, 8);
} NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS;

/*
 * NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT
 *
 * This command can be used to invoke a fake interrupt for the operation of DP1.2 branch device
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   interruptType
 *     This parameter specifies the type of fake interrupt to be invoked. Possible values are:
 *     0 => IRQ
 *     1 => HPDPlug
 *     2 => HPDUnPlug
 *   displayId
 *     should be for DP only
 *
 */

#define NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT (0x73136bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS_MESSAGE_ID (0x6BU)

typedef struct NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 interruptType;
} NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS;

#define NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_IRQ    (0x00000000U)
#define NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PLUG   (0x00000001U)
#define NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_UNPLUG (0x00000002U)

/*
 * NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG
 *
 * This command sets the MS displayId lit up by driver for further use of VBIOS
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     should be for DP only
 *   activeDevAddr
 *     Active MS panel address
 *   sorIndex
 *     SOR Index
 *   dpLink
 *     DP Sub Link Index
 *   hopCount
 *     Maximum hopcounts in MS address
 *   dpMsDevAddrState
 *     DP Multistream Device Address State. The values can be
 *
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_TIMEOUT
 *
 */
#define NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG         (0x73136cU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS_MESSAGE_ID (0x6CU)

typedef struct NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 activeDevAddr;
    NvU32 sorIndex;
    NvU32 dpLink;
    NvU32 hopCount;
    NvU32 dpMsDevAddrState;
} NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS;



/*
* NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT
*
* This command configures a new bit, NV_PDISP_SF_DP_LINKCTL_TRIGGER_SELECT
* to indicate which pipeline will handle the
* time slots allocation in single head MST mode
*
*      subDeviceInstance
*          This parameter specifies the subdevice instance within the
*          NV04_DISPLAY_COMMON parent device to which the operation should be
*          directed. This parameter must specify a value between zero and the
*          total number of subdevices within the parent device.  This parameter
*          should be set to zero for default behavior
*      Head
*          Specifies the head index for the stream
*      sorIndex
*          Specifies the SOR index for the stream
*      streamIndex
*          Stream Identifier
*
*
* Possible status values returned are:
*      NV_OK
*      NV_ERR_INVALID_ARGUMENT
*      NV_ERR_GENERIC: when this command has already been called
*
*/
#define NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT (0x73136fU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS_MESSAGE_ID (0x6FU)

typedef struct NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 head;
    NvU32 sorIndex;
    NvU32 singleHeadMSTPipeline;
} NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS;

/*
* NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM
*
*  This call is used by the displayport library.& clients of RM
*  Its main function is to configure single Head Multi stream mode
 * this call configures internal RM datastructures to support required mode.
*
*   subDeviceInstance
*     This parameter specifies the subdevice instance within the
*     NV04_DISPLAY_COMMON parent device to which the operation should be
*     directed. This parameter must specify a value between zero and the
*     total number of subdevices within the parent device.  This parameter
*     should be set to zero for default behavior.
*
*   displayIDs
*     This parameter specifies array of DP displayIds to be configured which are driven out from a single head.
*
*   numStreams
*     This parameter specifies number of streams driven from a single head
*     ex: for 2SST & 2MST its value is 2.
*
*   mode
*     This parameter specifies single head multi stream mode to be configured.
*
*   bSetConfigure
*     This parameter configures single head multistream mode
*     if TRUE it sets SST or MST based on 'mode' parameter and updates internal driver data structures with the given information.
*     if FALSE clears the configuration of single head multi stream mode.
*
*   vbiosPrimaryDispIdIndex
*    This parameter specifies vbios master displayID index in displayIDs input array.
*
*  Possible status values returned are:
*   NV_OK
*   NV_ERR_INVALID_ARGUMENT
*   NV_ERR_NOT_SUPPORTED
*
*/
#define NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM (0x73136eU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_SINGLE_HEAD_MAX_STREAMS         (0x00000002U)
#define NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS_MESSAGE_ID (0x6EU)

typedef struct NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayIDs[NV0073_CTRL_CMD_DP_SINGLE_HEAD_MAX_STREAMS];
    NvU32  numStreams;
    NvU32  mode;
    NvBool bSetConfig;
    NvU8   vbiosPrimaryDispIdIndex;
} NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS;

#define NV0073_CTRL_CMD_DP_SINGLE_HEAD_MULTI_STREAM_NONE     (0x00000000U)
#define NV0073_CTRL_CMD_DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST (0x00000001U)
#define NV0073_CTRL_CMD_DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST (0x00000002U)

/*
* NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL
*
* This command configures a new bit, NV_PDISP_SF_DP_LINKCTL_TRIGGER_ALL
* to indicate which if all the pipelines to take affect on ACT (sorFlushUpdates)
* in single head MST mode
*
*      subDeviceInstance
*          This parameter specifies the subdevice instance within the
*          NV04_DISPLAY_COMMON parent device to which the operation should be
*          directed. This parameter must specify a value between zero and the
*          total number of subdevices within the parent device.  This parameter
*          should be set to zero for default behavior
*      Head
*          Specifies the head index for the stream
*      sorIndex
*          Specifies the SOR index for the stream
*      streamIndex
*          Stream Identifier
*
*
* Possible status values returned are:
*      NV_OK
*      NV_ERR_INVALID_ARGUMENT
*      NV_ERR_GENERIC: when this command has already been called
*
*/
#define NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL                   (0x731370U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS_MESSAGE_ID (0x70U)

typedef struct NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  head;
    NvBool enable;
} NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS;



/*
* NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA
*
* This command collects the DP AUX log from the RM aux buffer and
* sends it to the application.
*
*      dpAuxBufferReadSize
*          Specifies the number of logs to be read from the
*          AUX buffer in RM
*      dpNumMessagesRead
*          Specifies the number of logs read from the AUX buffer
*      dpAuxBuffer
*          The local buffer to copy the specified number of logs
*          from RM to user application
*
*
* Possible status values returned are:
*      NV_OK
*      NV_ERR_INVALID_ARGUMENT
*      NV_ERR_GENERIC: when this command has already been called
*
*
*DPAUXPACKET - This structure holds the log information
* auxPacket - carries the hex dump of the message transaction
* auxEvents - Contains the information as in what request and reply type where
* auxRequestTimeStamp - Request timestamp
* auxMessageReqSize - Request Message size
* auxMessageReplySize - Reply message size(how much information was actually send by receiver)
* auxOutPort - DP port number
* auxPortAddress - Address to which data was requested to be read or written
* auxReplyTimeStamp - Reply timestamp
* auxCount - Serial number to keep track of transactions
*/

/*Maximum dp messages size is 16 as per the protocol*/
#define DP_MAX_MSG_SIZE                          16U
#define MAX_LOGS_PER_POLL                        50U

/* Various kinds of DP Aux transactions */
#define NV_DP_AUXLOGGER_REQUEST_TYPE                     3:0
#define NV_DP_AUXLOGGER_REQUEST_TYPE_NULL        0x00000000U
#define NV_DP_AUXLOGGER_REQUEST_TYPE_I2CWR       0x00000001U
#define NV_DP_AUXLOGGER_REQUEST_TYPE_I2CREQWSTAT 0x00000002U
#define NV_DP_AUXLOGGER_REQUEST_TYPE_MOTWR       0x00000003U
#define NV_DP_AUXLOGGER_REQUEST_TYPE_MOTREQWSTAT 0x00000004U
#define NV_DP_AUXLOGGER_REQUEST_TYPE_AUXWR       0x00000005U
#define NV_DP_AUXLOGGER_REQUEST_TYPE_I2CRD       0x00000006U
#define NV_DP_AUXLOGGER_REQUEST_TYPE_MOTRD       0x00000007U
#define NV_DP_AUXLOGGER_REQUEST_TYPE_AUXRD       0x00000008U
#define NV_DP_AUXLOGGER_REQUEST_TYPE_UNKNOWN     0x00000009U

#define NV_DP_AUXLOGGER_REPLY_TYPE                       7:4
#define NV_DP_AUXLOGGER_REPLY_TYPE_NULL          0x00000000U
#define NV_DP_AUXLOGGER_REPLY_TYPE_SB_ACK        0x00000001U
#define NV_DP_AUXLOGGER_REPLY_TYPE_RETRY         0x00000002U
#define NV_DP_AUXLOGGER_REPLY_TYPE_TIMEOUT       0x00000003U
#define NV_DP_AUXLOGGER_REPLY_TYPE_DEFER         0x00000004U
#define NV_DP_AUXLOGGER_REPLY_TYPE_DEFER_TO      0x00000005U
#define NV_DP_AUXLOGGER_REPLY_TYPE_ACK           0x00000006U
#define NV_DP_AUXLOGGER_REPLY_TYPE_ERROR         0x00000007U
#define NV_DP_AUXLOGGER_REPLY_TYPE_UNKNOWN       0x00000008U

#define NV_DP_AUXLOGGER_EVENT_TYPE                       9:8
#define NV_DP_AUXLOGGER_EVENT_TYPE_AUX           0x00000000U
#define NV_DP_AUXLOGGER_EVENT_TYPE_HOT_PLUG      0x00000001U
#define NV_DP_AUXLOGGER_EVENT_TYPE_HOT_UNPLUG    0x00000002U
#define NV_DP_AUXLOGGER_EVENT_TYPE_IRQ           0x00000003U

#define NV_DP_AUXLOGGER_AUXCTL_CMD                       15:12
#define NV_DP_AUXLOGGER_AUXCTL_CMD_INIT          0x00000000U
#define NV_DP_AUXLOGGER_AUXCTL_CMD_I2CWR         0x00000000U
#define NV_DP_AUXLOGGER_AUXCTL_CMD_I2CRD         0x00000001U
#define NV_DP_AUXLOGGER_AUXCTL_CMD_I2CREQWSTAT   0x00000002U
#define NV_DP_AUXLOGGER_AUXCTL_CMD_MOTWR         0x00000004U
#define NV_DP_AUXLOGGER_AUXCTL_CMD_MOTRD         0x00000005U
#define NV_DP_AUXLOGGER_AUXCTL_CMD_MOTREQWSTAT   0x00000006U
#define NV_DP_AUXLOGGER_AUXCTL_CMD_AUXWR         0x00000008U
#define NV_DP_AUXLOGGER_AUXCTL_CMD_AUXRD         0x00000009U


typedef struct DPAUXPACKET {
    NvU32 auxEvents;
    NvU32 auxRequestTimeStamp;
    NvU32 auxMessageReqSize;
    NvU32 auxMessageReplySize;
    NvU32 auxOutPort;
    NvU32 auxPortAddress;
    NvU32 auxReplyTimeStamp;
    NvU32 auxCount;
    NvU8  auxPacket[DP_MAX_MSG_SIZE];
} DPAUXPACKET;
typedef struct DPAUXPACKET *PDPAUXPACKET;

#define NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA (0x731373U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS_MESSAGE_ID (0x73U)

typedef struct NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS {
    //In
    NvU32       subDeviceInstance;
    NvU32       dpAuxBufferReadSize;

    //Out
    NvU32       dpNumMessagesRead;
    DPAUXPACKET dpAuxBuffer[MAX_LOGS_PER_POLL];
} NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS;




/* NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES
 *
 * This setup link rate table for target display to enable indexed link rate
 * and export valid link rates back to client. Client may pass empty table to
 * reset previous setting.
 *
 * subDeviceInstance
 *    client will give a subdevice to get right pGpu/pDisp for it
 * displayId
 *    DisplayId of the display for which the client targets
 * linkRateTbl
 *    Link rates in 200KHz as native granularity from eDP 1.4
 * linkBwTbl
 *    Link rates valid for client to apply to
 * linkBwCount
 *    Total valid link rates
 *
 * Possible status values returned include:
 *   NV_OK
 *   NV_ERR_NOT_SUPPORTED
 *   NV_ERR_INVALID_ARGUMENT
 */

#define NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES (0x731377U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES        8U

#define NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS_MESSAGE_ID (0x77U)

typedef struct NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS {
    // In
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU16 linkRateTbl[NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES];

    // Out
    NvU16 linkBwTbl[NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES];
    NvU8  linkBwCount;
} NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS;


/*
 * NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES
 *
 * This command is used to not depend on supervisor interrupts for setting the
 * stereo msa params. We will not cache the values and can toggle stereo using
 * this ctrl call on demand. Note that this control call will only change stereo
 * settings and will leave other settings as is.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     should be for DP only
 *   bEnableMSA
 *     To enable or disable MSA
 *   bStereoPhaseInverse
 *     To enable or disable Stereo Phase Inverse value
 *   featureMask
 *     Enable/Disable mask of individual MSA property.
 *   featureValues
 *     MSA property value to write
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_NOT_SUPPORTED
 *      NV_ERR_TIMEOUT
 *
 */
#define NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES (0x731378U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS_MESSAGE_ID (0x78U)

typedef struct NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS {
    NvU32                                subDeviceInstance;
    NvU32                                displayId;
    NvBool                               bEnableMSA;
    NvBool                               bStereoPhaseInverse;
    NV0073_CTRL_DP_MSA_PROPERTIES_MASK   featureMask;
    NV0073_CTRL_DP_MSA_PROPERTIES_VALUES featureValues;
} NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS;



/*
 * NV0073_CTRL_CMD_DP_CONFIGURE_FEC
 *
 * This command is used to enable/disable FEC on DP Mainlink.
 * FEC is a prerequisite to DSC. This should be called only
 * after LT completes (including PostLT LQA) while enabling.
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *
 *   displayId
 *     Can only be 1 and must be DP.
 *
 *   bEnableFec
 *     To enable or disable FEC
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_NOT_SUPPORTED
 *
 */
#define NV0073_CTRL_CMD_DP_CONFIGURE_FEC (0x73137aU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS_MESSAGE_ID (0x7AU)

typedef struct NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvBool bEnableFec;
} NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS;

/*
 * NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior
 *   cmd
 *     This parameter is an input to this command.
 *     Here are the current defined fields:
 *       NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_CMD_POWER
 *         Set to specify what operation to run.
 *           NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_CMD_POWER_UP
 *             Request to power up pad.
 *           NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_CMD_POWER_DOWN
 *             Request to power down the pad.
 *   linkBw
 *     This parameter is used to pass in the link bandwidth required to run the
 *     power up sequence. Refer enum DP_LINK_BANDWIDTH for valid values.
 *   laneCount
 *     This parameter is used to pass the lanecount.
 *   sorIndex
 *     This parameter is used to pass the SOR index.
 *   sublinkIndex
 *     This parameter is used to pass the sublink index. Please refer
 *     enum DFPLINKINDEX for valid values
 *   priPadLinkIndex
 *     This parameter is used to pass the padlink index for primary link.
 *     Please refer enum DFPPADLINK for valid index values for Link A~F.
 *   secPadLinkIndex
 *     This parameter is used to pass the padlink index for secondary link.
 *     For Single SST pass in NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PADLINK_INDEX_INVALID
 *   bEnableSpread
 *     This parameter is boolean value used to indicate if spread is to be enabled or disabled.
 */

#define NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD (0x73137bU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS_MESSAGE_ID (0x7BU)

typedef struct NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  cmd;
    NvU32  linkBw;
    NvU32  laneCount;
    NvU32  sorIndex;
    NvU32  sublinkIndex;          // sublink A/B
    NvU32  priPadLinkIndex;       // padlink A/B/C/D/E/F
    NvU32  secPadLinkIndex;       // padlink A/B/C/D/E/F for secondary link in DualSST case.
    NvBool bEnableSpread;
} NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS;

#define NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_CMD_POWER                        0:0
#define NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_CMD_POWER_UP          (0x00000000U)
#define NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_CMD_POWER_DOWN        (0x00000001U)

#define NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PADLINK_INDEX_INVALID (0x000000FFU)

/*
 * NV0073_CTRL_CMD_DP_AUXCH_CTRL
 *
 * This command can be used to perform the I2C Bulk transfer over
 * DP Aux channel. This is the display port specific implementation
 * for sending bulk data over the DpAux channel, by splitting up the
 * data into pieces and retrying for pieces that aren't ACK'd.
 *
 *   subDeviceInstance [IN]
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId [IN]
 *     This parameter specifies the ID of the display for which the dfp
 *     caps should be returned.  The display ID must a dfp display.
 *     If more than one displayId bit is set or the displayId is not a dfp,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   addr [IN]
 *     This parameter is an input to this command.  The addr parameter follows
 *     Section 2.4 in DisplayPort spec and the client should refer to the valid
 *     address in DisplayPort spec.  Only the first 20 bits are valid.
 *   bWrite [IN]
 *     This parameter specifies whether the command is a I2C write (NV_TRUE) or
 *     a I2C read (NV_FALSE).
 *   data [IN/OUT]
 *     In the case of a read transaction, this parameter returns the data from
 *     transaction request.  In the case of a write transaction, the client
 *     should write to this buffer for the data to send.
 *   size [IN/OUT]
 *     Specifies how many data bytes to read/write depending on the
 *     transaction type.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_DP_AUXCH_I2C_TRANSFER_CTRL                (0x73137cU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_MAX_DATA_SIZE           256U

#define NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS_MESSAGE_ID (0x7CU)

typedef struct NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  addr;
    NvBool bWrite;
    NvU8   data[NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_MAX_DATA_SIZE];
    NvU32  size;
} NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS;

/*
 * NV0073_CTRL_CMD_DP_ENABLE_VRR
 *
 * The command is used to enable VRR.
 *
 *   subDeviceInstance [IN]
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior
 *   displayId [IN]
 *     This parameter is an input to this command, specifies the ID of the display
 *     for client targeted to.
 *     The display ID must a DP display.
 *     If more than one displayId bit is set or the displayId is not a DP,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   cmd [IN]
 *     This parameter is an input to this command.
 *
 *      _STAGE: specifies the stage id to execute in the VRR enablement sequence.
 *        _MONITOR_ENABLE_BEGIN:      Send command to the monitor to start monitor
 *                                    enablement procedure.
 *        _MONITOR_ENABLE_CHALLENGE:  Send challenge to the monitor
 *        _MONITOR_ENABLE_CHECK:      Read digest from the monitor, and verify
 *                                    if the result is valid.
 *        _DRIVER_ENABLE_BEGIN:       Send command to the monitor to start driver
 *                                    enablement procedure.
 *        _DRIVER_ENABLE_CHALLENGE:   Read challenge from the monitor and write back
 *                                    corresponding digest.
 *        _DRIVER_ENABLE_CHECK:       Check if monitor enablement worked.
 *        _RESET_MONITOR:             Set the FW state m/c to a known state.
 *        _INIT_PUBLIC_INFO:          Send command to the monitor to prepare public info.
 *        _GET_PUBLIC_INFO:           Read public info from the monitor.
 *        _STATUS_CHECK:              Check if monitor is ready for next command.
 *   result [OUT]
 *     This is an output parameter to reflect the result of the operation.
 */
#define NV0073_CTRL_CMD_DP_ENABLE_VRR (0x73137dU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS_MESSAGE_ID (0x7DU)

typedef struct NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS {
    NvU32 subDeviceInstance;
    NvU32 displayId;
    NvU32 cmd;
    NvU32 result;
} NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS;

#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE                                   3:0
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_MONITOR_ENABLE_BEGIN     (0x00000000U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_MONITOR_ENABLE_CHALLENGE (0x00000001U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_MONITOR_ENABLE_CHECK     (0x00000002U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_DRIVER_ENABLE_BEGIN      (0x00000003U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_DRIVER_ENABLE_CHALLENGE  (0x00000004U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_DRIVER_ENABLE_CHECK      (0x00000005U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_RESET_MONITOR            (0x00000006U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_INIT_PUBLIC_INFO         (0x00000007U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_GET_PUBLIC_INFO          (0x00000008U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_CMD_STAGE_STATUS_CHECK             (0x00000009U)

#define NV0073_CTRL_DP_CMD_ENABLE_VRR_STATUS_OK                          (0x00000000U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_STATUS_PENDING                     (0x80000001U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_STATUS_READ_ERROR                  (0x80000002U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_STATUS_WRITE_ERROR                 (0x80000003U)
#define NV0073_CTRL_DP_CMD_ENABLE_VRR_STATUS_DEVICE_ERROR                (0x80000004U)

/*
 * NV0073_CTRL_CMD_DP_GET_GENERIC_INFOFRAME
 *
 * This command is used to capture the display output packets for DP protocol.
 * Common supported packets are Dynamic Range and mastering infoframe SDP for HDR,
 * VSC SDP for colorimetry and pixel encoding info.
 *
 *   displayID (in)
 *     This parameter specifies the displayID for the display resource to configure.
 *   subDeviceInstance (in)
 *     This parameter specifies the subdevice instance within the NV04_DISPLAY_COMMON
 *     parent device to which the operation should be directed.
 *   infoframeIndex (in)
 *     HW provides support to program 2 generic infoframes per frame for DP.
 *     This parameter indicates which infoframe packet is to be captured.
 *     Possible flags are as follows:
 *       NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_CAPTURE_MODE
 *         This flag indicates the INFOFRAME that needs to be read.
 *         Set to _INFOFRAME0 if RM should read GENERIC_INFOFRAME
 *         Set to _INFOFRAME1 if RM should read GENERIC_INFOFRAME1
 *   packet (out)
 *     pPacket points to the memory for reading the infoframe packet.
 *   bTransmitControl (out)
 *     This gives the transmit mode of infoframes.
 *       If set, means infoframe will be sent as soon as possible and then on
 *       every frame during vblank.
 *       If cleared, means the infoframe will be sent once as soon as possible.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_DP_GET_GENERIC_INFOFRAME                         (0x73137eU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_GENERIC_INFOFRAME_MAX_PACKET_SIZE                 36U

#define NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS_MESSAGE_ID (0x7EU)

typedef struct NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  infoframeIndex;
    NvU8   packet[NV0073_CTRL_DP_GENERIC_INFOFRAME_MAX_PACKET_SIZE];
    NvBool bTransmitControl;
} NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS;


#define NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_CAPTURE_MODE                       0:0
#define NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_CAPTURE_MODE_INFOFRAME0 (0x0000000U)
#define NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_CAPTURE_MODE_INFOFRAME1 (0x0000001U)


/*
 * NV0073_CTRL_CMD_DP_GET_MSA_ATTRIBUTES
 *
 * This command is used to capture the various data attributes sent in the MSA for DP protocol.
 * Refer table 2-94 'MSA Data Fields' in DP1.4a spec document for MSA data field description.
 *
 *   displayID (in)
 *     This parameter specifies the displayID for the display resource to configure.
 *   subDeviceInstance (in)
 *     This parameter specifies the subdevice instance within the NV04_DISPLAY_COMMON
 *     parent device to which the operation should be directed.
 *   mvid, nvid (out)
 *     Video timestamp used by DP sink for regenerating pixel clock.
 *   misc0, misc1 (out)
 *     Miscellaneous MSA attributes.
 *   hTotal, vTotal (out)
 *     Htotal measured in pixel count and vtotal measured in line count.
 *   hActiveStart, vActiveStart (out)
 *     Active start measured from start of leading edge of the sync pulse.
 *   hActiveWidth, vActiveWidth (out)
 *     Active video width and height.
 *   hSyncWidth, vSyncWidth (out)
 *     Width of sync pulse.
 *   hSyncPolarity, vSyncPolarity (out)
 *     Polarity of sync pulse.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_DP_GET_MSA_ATTRIBUTES                        (0x73137fU) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_DP_MSA_MAX_DATA_SIZE                             7U

#define NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS_MESSAGE_ID (0x7FU)

typedef struct NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU32  mvid;
    NvU32  nvid;
    NvU8   misc0;
    NvU8   misc1;
    NvU16  hTotal;
    NvU16  vTotal;
    NvU16  hActiveStart;
    NvU16  vActiveStart;
    NvU16  hActiveWidth;
    NvU16  vActiveWidth;
    NvU16  hSyncWidth;
    NvU16  vSyncWidth;
    NvBool hSyncPolarity;
    NvBool vSyncPolarity;
} NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS;

#define NV0073_CTRL_DP_MSA_ATTRIBUTES_MVID                              23:0
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_NVID                              23:0
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_MISC0                              7:0
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_MISC1                             15:8
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_HTOTAL                            15:0
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_VTOTAL                           31:16
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_HACTIVE_START                     15:0
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_VACTIVE_START                    31:16
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_HACTIVE_WIDTH                     15:0
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_VACTIVE_WIDTH                    31:16
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_HSYNC_WIDTH                       14:0
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_HSYNC_POLARITY                   15:15
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_VSYNC_WIDTH                      30:16
#define NV0073_CTRL_DP_MSA_ATTRIBUTES_VSYNC_POLARITY                   31:31

/*
 * NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL
 *
 * This command is used to query OD capability and status as well as
 * control OD functionality of eDP LCD panels.
 *
 *   subDeviceInstance [in]
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId [in]
 *     This parameter specifies the ID of the DP display which owns
 *     the Main Link to be adjusted.  The display ID must a DP display
 *     as determined with the NV0073_CTRL_CMD_SPECIFIC_GET_TYPE command.
 *     If more than one displayId bit is set or the displayId is not a DP,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   cmd [in]
 *     This parameter is an input to this command.  The cmd parameter tells
 *     whether we have to get the value of a specific field or set the
 *     value in case of a writeable field.
 *   control [in]
 *     This parameter is input by the user. It is used by the user to decide the control
 *     value to be written to change the Sink OD mode. The command to write is
 *     the NV0073_CTRL_CMD_DP_AUXCH_OD_CTL_SET command.
 *   bOdCapable [out]
 *     This parameter reflects the OD capability of the Sink which can be
 *     fetched by using the NV0073_CTRL_CMD_DP_AUXCH_OD_CAPABLE_QUERY command.
 *   bOdControlCapable [out]
 *     This parameter reflects the OD control capability of the Sink which can be
 *     fetched by using the NV0073_CTRL_CMD_DP_AUXCH_OD_CTL_CAPABLE_QUERY command.
 *   bOdStatus [out]
 *     This parameter reflects the Sink OD status which can be
 *     fetched by using the NV0073_CTRL_CMD_DP_AUXCH_OD_STATUS_QUERY command.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */
#define NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL (0x731380U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS_MESSAGE_ID (0x80U)

typedef struct NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU8   control;
    NvU8   cmd;
    NvBool bOdCapable;
    NvBool bOdControlCapable;
    NvBool bOdStatus;
} NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS;

/* valid commands */
#define NV0073_CTRL_CMD_DP_AUXCHQUERY_OD_CAPABLE       0x00000000
#define NV0073_CTRL_CMD_DP_AUXCHQUERY_OD_CTL_CAPABLE   0x00000001
#define NV0073_CTRL_CMD_DP_AUXCHQUERY_OD_STATUS        0x00000002
#define NV0073_CTRL_CMD_DP_AUXCH_OD_CTL_SET            0x00000003

/* valid state values */
#define NV0073_CTRL_CMD_DP_AUXCH_OD_CTL_SET_AUTONOMOUS 0x00000000
#define NV0073_CTRL_CMD_DP_AUXCH_OD_CTL_SET_DISABLE_OD 0x00000002
#define NV0073_CTRL_CMD_DP_AUXCH_OD_CTL_SET_ENABLE_OD  0x00000003

/*
 * NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES
 *
 * This command returns the following info
 *
 *   subDeviceInstance
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId
 *     should be for DP only
 *   bEnableMSA
 *     To enable or disable MSA
 *   bStereoPhaseInverse
 *     To enable or disable Stereo Phase Inverse value
 *   bCacheMsaOverrideForNextModeset
 *     Cache the values and don't apply them until next modeset
 *   featureMask
 *     Enable/Disable mask of individual MSA property
 *   featureValues
 *     MSA property value to write
 *   bDebugValues
 *     To inform whether actual MSA values need to be returned
 *   pFeatureDebugValues
 *     It will actual MSA property value being written on HW.
 *     If its NULL then no error but return nothing
 *
 * Possible status values returned are:
 *      NV_OK
 *      NV_ERR_INVALID_ARGUMENT
 *      NV_ERR_NOT_SUPPORTED
 *      NV_ERR_TIMEOUT
 *
 */
#define NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2 (0x731381U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS_MESSAGE_ID" */

#define NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS_MESSAGE_ID (0x81U)

typedef struct NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS {
    NvU32                                subDeviceInstance;
    NvU32                                displayId;
    NvBool                               bEnableMSA;
    NvBool                               bStereoPhaseInverse;
    NvBool                               bCacheMsaOverrideForNextModeset;
    NV0073_CTRL_DP_MSA_PROPERTIES_MASK   featureMask;
    NV0073_CTRL_DP_MSA_PROPERTIES_VALUES featureValues;
    NvBool                               bDebugValues;
    NV0073_CTRL_DP_MSA_PROPERTIES_VALUES featureDebugValues;
} NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS;



/*
 * NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL
 *
 * This command is used to query VBL capability and status as well as
 * control enable/disable of VBL feature of eDP LCD panels.
 *
 *   subDeviceInstance [in]
 *     This parameter specifies the subdevice instance within the
 *     NV04_DISPLAY_COMMON parent device to which the operation should be
 *     directed. This parameter must specify a value between zero and the
 *     total number of subdevices within the parent device.  This parameter
 *     should be set to zero for default behavior.
 *   displayId [in]
 *     This parameter specifies the ID of the DP display which owns
 *     the Main Link to be adjusted.  The display ID must a DP display
 *     as determined with the NV0073_CTRL_CMD_SPECIFIC_GET_TYPE command.
 *     If more than one displayId bit is set or the displayId is not a DP,
 *     this call will return NV_ERR_INVALID_ARGUMENT.
 *   cmd [in]
 *     This parameter is an input to this command.  The cmd parameter tells
 *     whether we have to get the value of a specific field or set the
 *     value in case of a writeable field.
 *   control [in]
 *     This parameter is input by the user. It is used by the user to decide the control
 *     value to be written to the VBL control field. The command to write is
 *     the NV0073_CTRL_CMD_DP_AUXCH_VBL_CTL_SET command.
 *   bVblControlCapable [out]
 *     This parameter reflects the VBL control capability of the Sink which can be
 *     fetched by using the NV0073_CTRL_CMD_DP_AUXCH_VBL_CTL_CAPABLE_QUERY command.
 *   bVblStatus [out]
 *     This parameter reflects the Sink VBL status which can be
 *     fetched by using the NV0073_CTRL_CMD_DP_AUXCH_VBL_STATUS_QUERY command.
 *
 * Possible status values returned are:
 *   NV_OK
 *   NV_ERR_INVALID_ARGUMENT
 *   NV_ERR_NOT_SUPPORTED
 */

#define NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL (0x731386U) /* finn: Evaluated from "(FINN_NV04_DISPLAY_COMMON_DP_INTERFACE_ID << 8) | NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL_PARAMS_MESSAGE_ID" */

/* valid commands */
#define NV0073_CTRL_CMD_DP_AUXCH_QUERY_VBL_CTL_CAPABLE   0x00000000
#define NV0073_CTRL_CMD_DP_AUXCH_QUERY_VBL_STATUS        0x00000001
#define NV0073_CTRL_CMD_DP_AUXCH_SET_VBL_CTL             0x00000002

/* valid state values */
#define NV0073_CTRL_CMD_DP_AUXCH_SET_VBL_CTL_DISABLE     0x00000000
#define NV0073_CTRL_CMD_DP_AUXCH_SET_VBL_CTL_ENABLE      0x00000001

#define NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL_PARAMS_MESSAGE_ID (0x86U)

typedef struct NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL_PARAMS {
    NvU32  subDeviceInstance;
    NvU32  displayId;
    NvU8   cmd;
    NvU8   control;
    NvBool bVblControlCapable;
    NvBool bVblStatus;
} NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL_PARAMS;
/* _ctrl0073dp_h_ */
