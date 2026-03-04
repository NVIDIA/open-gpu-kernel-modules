/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
// Source file:      ctrl/ctrlc56f.finn
//




/* AMPERE_CHANNEL_GPFIFO_A control commands and parameters */

#include "ctrl/ctrlxxxx.h"
#include "ctrl/ctrlc36f.h" // This control call interface is an ALIAS of C36F

#include "cc_drv.h"



#define NVC56F_CTRL_CMD(cat,idx)  \
    NVXXXX_CTRL_CMD(0xC36F, NVC56F_CTRL_##cat, idx)

/* AMPERE_CHANNEL_GPFIFO_A command categories (6bits) */
#define NVC56F_CTRL_RESERVED (0x00)
#define NVC56F_CTRL_GPFIFO   (0x01)

/*
 * NVC56F_CTRL_CMD_NULL
 *
 * This command does nothing.
 * This command does not take any parameters.
 *
 * Possible status values returned is: NV_OK
*/
#define NVC56F_CTRL_CMD_NULL (NVC36F_CTRL_CMD_NULL)

/*
 * NVC56F_CTRL_CMD_GET_KMB_STAT_ADDR
 *
 *    This struct defines the addresses to log encryption statistics
 *    amountEncryptedAddr
 *         Amount of bytes encrypted
 *    numberEncryptedAddr
 *         Number of times data was encrypted.
 */
typedef struct NVC56F_CTRL_CMD_GET_KMB_STAT_ADDR {
    NV_DECLARE_ALIGNED(NvP64 amountEncryptedAddr, 8);
    NV_DECLARE_ALIGNED(NvP64 numberEncryptedAddr, 8);
} NVC56F_CTRL_CMD_GET_KMB_STAT_ADDR;

/*
 * NVC56F_CTRL_CMD_GET_KMB
 *
 *    This command returns the Key Material Bundle (KMB) for the current channel.
 *
 *    kmb     [OUT]            The KMB for the channel.
 *    hMemory [IN]             Memory handle to the encryption statistics buffer for the channel.
 *    keyRotationLowerThreshold [OUT] Key rotation lower threshold.
 *    keyRotationUpperThreshold [OUT] Key rotation upper threshold.
 *
 *    The key rotation thresholds are in units of
 *    (amount of data encrypted in units of 16 bytes + number of encryption invocations).
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */
#define NVC56F_CTRL_CMD_GET_KMB (0xc56f010b) /* finn: Evaluated from "(FINN_AMPERE_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC56F_CTRL_CMD_GET_KMB_PARAMS_MESSAGE_ID" */

#define NVC56F_CTRL_CMD_GET_KMB_PARAMS_MESSAGE_ID (0xBU)

typedef struct NVC56F_CTRL_CMD_GET_KMB_PARAMS {
    CC_KMB   kmb;
    NvHandle hMemory;
    NV_DECLARE_ALIGNED(NvU64 keyRotationLowerThreshold, 8);
    NV_DECLARE_ALIGNED(NvU64 keyRotationUpperThreshold, 8);
} NVC56F_CTRL_CMD_GET_KMB_PARAMS;

/*
 * NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV
 *
 *    This command rotates the IVs for secure channels.
 *
 *    rotateIvType [IN]    Which IVs to rotate.
 *    updatedKmb [OUT]     Updated KMB after the IV rotation.
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */
#define NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV (0xc56f010c) /* finn: Evaluated from "(FINN_AMPERE_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS_MESSAGE_ID" */

#define NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS_MESSAGE_ID (0xCU)

typedef struct NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS {
    ROTATE_IV_TYPE rotateIvType;
    CC_KMB         updatedKmb;
} NVC56F_CTRL_ROTATE_SECURE_CHANNEL_IV_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_SET_SECURITY_POLICY
 *
 *    This command sets the CC security policy.
 *    It can only be set before GpuReadyState is set.
 *    Is ignored after GpuReadyState is set.
 *
 *    attackerAdvantage [IN]
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_STATE
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */

/*
 *    The minimum and maximum values for attackerAdvantage.
 *    The probability of an attacker successfully guessing the contents of an encrypted packet go up ("attacker advantage").
 */
#define SECURITY_POLICY_ATTACKER_ADVANTAGE_DEFAULT (60)
#define SET_SECURITY_POLICY_ATTACKER_ADVANTAGE_MIN (50)
#define SET_SECURITY_POLICY_ATTACKER_ADVANTAGE_MAX (65)

#define NV_CONF_COMPUTE_CTRL_SET_SECURITY_POLICY   (0xc56f010d) /* finn: Evaluated from "(FINN_AMPERE_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NV_CONF_COMPUTE_CTRL_SET_SECURITY_POLICY_PARAMS_MESSAGE_ID" */

#define NV_CONF_COMPUTE_CTRL_SET_SECURITY_POLICY_PARAMS_MESSAGE_ID (0xDU)

typedef struct NV_CONF_COMPUTE_CTRL_SET_SECURITY_POLICY_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 attackerAdvantage, 8);
} NV_CONF_COMPUTE_CTRL_SET_SECURITY_POLICY_PARAMS;

/*
 * NV_CONF_COMPUTE_CTRL_GET_SECURITY_POLICY
 *
 *    This command get the CC security policy.
 *
 *    attackerAdvantage [OUT]
 *
 *    Possible status values returned are:
 *     NV_OK
 *     NV_ERR_INVALID_OBJECT_HANDLE
 *     NV_ERR_INVALID_ARGUMENT
 *     NV_ERR_NOT_SUPPORTED
 */

#define NV_CONF_COMPUTE_CTRL_GET_SECURITY_POLICY (0xc56f010e) /* finn: Evaluated from "(FINN_AMPERE_CHANNEL_GPFIFO_A_GPFIFO_INTERFACE_ID << 8) | NV_CONF_COMPUTE_CTRL_GET_SECURITY_POLICY_PARAMS_MESSAGE_ID" */

#define NV_CONF_COMPUTE_CTRL_GET_SECURITY_POLICY_PARAMS_MESSAGE_ID (0xEU)

typedef struct NV_CONF_COMPUTE_CTRL_GET_SECURITY_POLICY_PARAMS {
    NV_DECLARE_ALIGNED(NvU64 attackerAdvantage, 8);
} NV_CONF_COMPUTE_CTRL_GET_SECURITY_POLICY_PARAMS;



/* _ctrlc56f.h_ */

