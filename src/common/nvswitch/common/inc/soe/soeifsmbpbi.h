/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFSMBPBI_H_
#define _SOEIFSMBPBI_H_

/*!
 * @file   soeifsmbpbi.h
 * @brief  SOE SMBPBI Command Queue
 *
 *         The SMBPBI unit ID will be used for sending and recieving
 *         Command Messages between bomb and SMBPBI unit of SOE
 */

/*!
 * Test command/event type
 */
enum
{
    RM_SOE_SMBPBI_EVT_INTR = 0x0,
    RM_SOE_SMBPBI_CMD_ID_INIT,
    RM_SOE_SMBPBI_CMD_ID_UNLOAD,
    RM_SOE_SMBPBI_CMD_ID_SET_LINK_ERROR_INFO,
};

/*!
 * SMBPBI queue init command payload
 */
typedef struct
{
    NvU8        cmdType;
    NvU32       bombPollingPeriodUs;
    RM_FLCN_U64 dmaHandle;
} RM_SOE_SMBPBI_CMD_INIT, *PRM_SOE_SMBPBI_CMD_INIT;

/*!
 * SMBPBI queue Msgbox command payload
 */
typedef struct
{
    NvU8    cmdType;
    NvU32   msgboxCmd;
} RM_PMU_SMBPBI_CMD_MSGBOX, *PRM_PMU_SMBPBI_CMD_MSGBOX;

/*!
 * SMBPBI queue unload command payload
 */
typedef struct
{
    NvU8    cmdType;
} RM_SOE_SMBPBI_CMD_UNLOAD, *PRM_SOE_SMBPBI_CMD_UNLOAD;

/*!
 * Training error info bitmasks
 */
typedef struct
{
    NvBool      isValid;
    RM_FLCN_U64 attemptedTrainingMask0;
    RM_FLCN_U64 trainingErrorMask0;
} RM_SOE_SMBPBI_TRAINING_ERROR_INFO,
*PRM_SOE_SMBPBI_TRAINING_ERROR_INFO;

/*!
 * Runtime error link bitmask
 */
typedef struct
{
    NvBool      isValid;
    RM_FLCN_U64 mask0;
} RM_SOE_SMBPBI_RUNTIME_ERROR_INFO,
*PRM_SOE_SMBPBI_RUNTIME_ERROR_INFO;

/*!
 * SMBPBI queue set training error command payload
 */
typedef struct
{
    NvU8                              cmdType;
    RM_SOE_SMBPBI_TRAINING_ERROR_INFO trainingErrorInfo;
    RM_SOE_SMBPBI_RUNTIME_ERROR_INFO  runtimeErrorInfo;
} RM_SOE_SMBPBI_CMD_SET_LINK_ERROR_INFO,
*PRM_SOE_SMBPBI_CMD_SET_LINK_ERROR_INFO;

/*!
 * SMBPBI queue command payload
 */
typedef union
{
    NvU8                                   cmdType;
    RM_PMU_SMBPBI_CMD_MSGBOX               msgbox;
    RM_SOE_SMBPBI_CMD_INIT                 init;
    RM_SOE_SMBPBI_CMD_UNLOAD               unload;
    RM_SOE_SMBPBI_CMD_SET_LINK_ERROR_INFO  linkErrorInfo;
} RM_SOE_SMBPBI_CMD;

#endif  // _SOEIFSMBPBI_H_
