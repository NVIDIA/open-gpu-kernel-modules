/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFCHNMGMT_H_
#define _SOEIFCHNMGMT_H_

/*!
 * @file   soeifchnmgmt.h
 * @brief  SOE Command/Message Interfaces - SOE chnmgmt
 */

/*!
 * @brief Defines for CHNMGMT commands
 */
enum
{
    RM_SOE_CHNMGMT_CMD_ID_ENGINE_RC_RECOVERY,
    RM_SOE_CHNMGMT_CMD_ID_FINISH_RC_RECOVERY,
};

/*!
 * @brief CHNMGMT engine RC command
 */
typedef struct
{
    NvU8 cmdType;  //<! Type of command, MUST be first.
    NvU8 pad[3];
} RM_SOE_CHNMGMT_CMD_ENGINE_RC_RECOVERY;

/*!
 * @brief CHNMGMT finish RC command
 */
typedef struct
{
    NvU8 cmdType;  //<! Type of command, MUST be first.
    NvU8 pad[3];
} RM_SOE_CHNMGMT_CMD_FINISH_RC_RECOVERY;

/*!
 * A simple union of all the chnmgmt commands. Use the 'cmdType' variable to
 * determine the actual type of the command.
 */
typedef union
{
    NvU8                                    cmdType;
    RM_SOE_CHNMGMT_CMD_ENGINE_RC_RECOVERY  engineRcCmd;
    RM_SOE_CHNMGMT_CMD_FINISH_RC_RECOVERY  finishRcCmd;
} RM_SOE_CHNMGMT_CMD;


/*!
 * @brief Defines for CHNMGMT messages
 */
enum
{
    RM_SOE_CHNMGMT_MSG_ID_TRIGGER_RC_RECOVERY,
    RM_SOE_CHNMGMT_MSG_ID_UNBLOCK_RC_RECOVERY,
    RM_SOE_CHNMGMT_MSG_ID_ENGINE_RC_RECOVERY,
    RM_SOE_CHNMGMT_MSG_ID_FINISH_RC_RECOVERY,
};

/*!
 * @brief CHNMGMT trigger RC recovery msg
 */
typedef struct
{
    NvU8 msgType; //<! Tag indicating the message type. MUST be first
    NvU8 pad[3];
} RM_SOE_CHNMGMT_MSG_TRIGGER_RC_RECOVERY;

/*!
 * @brief CHNMGMT unblock RC recovery msg
 */
typedef struct
{
    NvU8 msgType; //<! Tag indicating the message type. MUST be first
    NvU8 pad[3];
} RM_SOE_CHNMGMT_MSG_UNBLOCK_RC_RECOVERY;

/*!
 * @brief CHNMGMT engine RC recovery msg
 */
typedef struct
{
    NvU8 msgType; //<! Tag indicating the message type. MUST be first
    NvU8 pad[3];
} RM_SOE_CHNMGMT_MSG_ENGINE_RC_RECOVERY;

/*!
 * @brief CHNMGMT finish RC recovery msg
 */
typedef struct
{
    NvU8 msgType; //<! Tag indicating the message type. MUST be first
    NvU8 pad[3];
} RM_SOE_CHNMGMT_MSG_FINISH_RC_RECOVERY;

/*!
 * A simple union of all the Test messages. Use the 'msgType' variable to
 * determine the actual type of the message. This will be the cmdType of the
 * command for which the message is being sent.
 */
typedef union
{
    NvU8                                     msgType;
    RM_SOE_CHNMGMT_MSG_TRIGGER_RC_RECOVERY  triggerRC;
    RM_SOE_CHNMGMT_MSG_UNBLOCK_RC_RECOVERY  unblockRC;
    RM_SOE_CHNMGMT_MSG_ENGINE_RC_RECOVERY   engineRC;
    RM_SOE_CHNMGMT_MSG_FINISH_RC_RECOVERY   finishRC;
} RM_SOE_CHNMGMT_MSG;

#endif  // _SOEIFCHNMGMT_H_


