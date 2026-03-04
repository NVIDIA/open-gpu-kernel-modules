/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFCCI_H_
#define _SOEIFCCI_H_

#include "nvfixedtypes.h"

/*!
 * @file   soeifcci.h
 * @brief  SOE CCI Command Queue
 *
 *         The CCI unit ID will be used for sending and recieving
 *         Command Messages between driver and CCI unt of SOE
 */

/*!
 * @brief CCI Message IDs
 */
enum
{
    RM_SOE_CCI_MSG_ID_ALI_OPTIMIZE_LINK_INFO,
};

/*!
 * @brief Message for ALI link info
 */
typedef struct
{
    NvU8 msgType;
    
    // Global link id of the link to be optimized
    NvU8 linkId;
} RM_SOE_CCI_MSG_ALI_OPTIMIZE_LINK_INFO;

/*!
 * @brief A simple union of all CCI messages.
 *        Use msgType to determine the actual type of the message.
 */
typedef union
{
    NvU8 msgType;

    // The following struct is expected to include msgType as its first member
    RM_SOE_CCI_MSG_ALI_OPTIMIZE_LINK_INFO linkInfo;
} RM_SOE_CCI_MSG;

/*!
 * @brief CCI Command types
 */
enum
{
    RM_SOE_CCI_CMD_ID_SEND_MSG_TO_DRIVER,
};

/*!
 * @brief Send aysncronous message about CCI events.
 */
typedef struct
{
    NvU8 cmdType;
    union
    {
        NvU8 msgType;
        RM_SOE_CCI_MSG_ALI_OPTIMIZE_LINK_INFO linkInfo;
    } data;
} RM_SOE_CCI_CMD_SEND_MSG_TO_DRIVER;

/*!
 * @brief A simple union of all CCI commands.
 *        Use cmdType to determine the actual type of the command.
 */
typedef union
{
    NvU8 cmdType;

    // The following struct is expected to include cmdType as its first member
    RM_SOE_CCI_CMD_SEND_MSG_TO_DRIVER msg;
} RM_SOE_CCI_CMD;

#endif  // _SOEIFCCI_H_
