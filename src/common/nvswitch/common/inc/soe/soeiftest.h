
/*
 * SPDX-FileCopyrightText: Copyright (c) 2018 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFTEST_H_
#define _SOEIFTEST_H_

/*!
 * @file   soeiftest.h
 * @brief  SOE Command/Message Interfaces - SOE test
 *         The Test unit ID will be used for dummy test jobs.
 *         Currently we have just the RT timer test job. This will be
 *         run only if the SOE is is debug mode.
 */

/*!
 * Test command type
 */
enum
{
    RM_SOE_TEST_CMD_ID_RTTIMER_TEST          = 0x0,
};


/*!
 * @brief Test command to test RT timer
 */
typedef struct
{
    NvU8                cmdType;     //<! Type of command, MUST be first.
    NvBool              bCheckTime;  //<! Verify that the RT timer is accurate
    NvU8                pad[2];
    NvU32               count;       //<! Count value to program the RT Timer
} RM_SOE_TEST_CMD_RTTIMER_TEST;

enum
{
    FAKEIDLE_UNMASK_INTR  = 0x1,
    FAKEIDLE_MASK_INTR    = 0x2,
    FAKEIDLE_PROGRAM_IDLE = 0x3,
    FAKEIDLE_PROGRAM_BUSY = 0x4,
    FAKEIDLE_CTXSW_DETECT = 0x5,
    FAKEIDLE_MTHD_DETECT  = 0x6,
    FAKEIDLE_COMPLETE     = 0x7,
};

/*!
 * A simple union of all the test commands. Use the 'cmdType' variable to
 * determine the actual type of the command.
 */
typedef union
{
    NvU8                                   cmdType;
    RM_SOE_TEST_CMD_RTTIMER_TEST           rttimer;
} RM_SOE_TEST_CMD;

/*!
 * List of statuses that the Test task/job can send to the RM.
 *
 * RM_SOE_TEST_MSG_STATUS_OK:               Command was successful
 * RM_SOE_TEST_MSG_STATUS_FAIL:             Command failed
 */
#define RM_SOE_TEST_MSG_STATUS_OK              (0x0)
#define RM_SOE_TEST_MSG_STATUS_FAIL            (0x1)

/*!
 * @brief Test msg to test RT timer
 */
typedef struct
{
    NvU8                   msgType; //<! Type of message, MUST be first.
    NvU8                   status;
    NvU8                   pad[2];
    NvU32                  oneShotNs;
    NvU32                  continuousNs;
} RM_SOE_TEST_MSG_RTTIMER_TEST;


/*!
 * A simple union of all the Test messages. Use the 'msgType' variable to
 * determine the actual type of the message. This will be the cmdType of the
 * command for which the message is being sent.
 */
typedef union
{
    NvU8                                  msgType;
    RM_SOE_TEST_MSG_RTTIMER_TEST          rttimer;

} RM_SOE_TEST_MSG;

#endif  // _SOEIFTEST_H_
