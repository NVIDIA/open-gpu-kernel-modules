/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef _SOEIFTNVL_H_
#define _SOEIFTNVL_H_



/*!
 * @file   soeiftnvl.h
 * @brief  SOE TNVL Command Queue
 *
 *         The TNVL unit ID will be used for sending and recieving
 *         Command Messages between driver and TNVL unit of SOE
 */

/*!
 * Commands offered by the SOE Tnvl Interface.
 */
enum
{
    /*
     * Issue register write command
     */
    RM_SOE_TNVL_CMD_ISSUE_REGISTER_WRITE = 0x0,
    /*
     * Issue pre-lock sequence
     */
    RM_SOE_TNVL_CMD_ISSUE_PRE_LOCK_SEQUENCE = 0x1,
};

/*!
 * TNVL queue command payload
 */

typedef struct
{
    NvU8  cmdType;
    NvU32 offset;
    NvU32 data;
} RM_SOE_TNVL_CMD_REGISTER_WRITE;

typedef struct
{
    NvU8  cmdType;
} RM_SOE_TNVL_CMD_PRE_LOCK_SEQUENCE;

typedef union
{
    NvU8 cmdType;
    RM_SOE_TNVL_CMD_REGISTER_WRITE registerWrite;
    RM_SOE_TNVL_CMD_PRE_LOCK_SEQUENCE preLockSequence;
} RM_SOE_TNVL_CMD;

#endif  // _SOEIFTNVL_H_

