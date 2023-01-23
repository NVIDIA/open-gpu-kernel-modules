/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef SDK_NVSTATUS_H
#define SDK_NVSTATUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

typedef NvU32 NV_STATUS;

#define NV_STATUS_CODE( name, code, string ) name = (code),

enum 
{
    #include "nvstatuscodes.h"
};

#undef NV_STATUS_CODE

/*!
 * @def         NV_STATUS_LEVEL_OK
 * @see         NV_STATUS_LEVEL
 * @brief       Success: No error or special condition
 */
#define NV_STATUS_LEVEL_OK              0

/*!
 * @def         NV_STATUS_LEVEL_WARN
 * @see         NV_STATUS_LEVEL
 * @brief       Success, but there is an special condition
 *
 * @details     In general, NV_STATUS_LEVEL_WARN status codes are handled the
 *              same as NV_STATUS_LEVEL_OK, but are usefil to indicate that
 *              there is a condition that may be specially handled.
 *
 *              Therefore, in most cases, client function should test for
 *              status <= NV_STATUS_LEVEL_WARN or status > NV_STATUS_LEVEL_WARN
 *              to determine success v. failure of a call.
 */
#define NV_STATUS_LEVEL_WARN            1

/*!
 * @def         NV_STATUS_LEVEL_ERR
 * @see         NV_STATUS_LEVEL
 * @brief       Unrecoverable error condition
 */
#define NV_STATUS_LEVEL_ERR             3

/*!
 * @def         NV_STATUS_LEVEL
 * @see         NV_STATUS_LEVEL_OK
 * @see         NV_STATUS_LEVEL_WARN
 * @see         NV_STATUS_LEVEL_ERR
 * @brief       Level of the status code
 *
 * @warning     IMPORTANT: When comparing NV_STATUS_LEVEL(_S) against one of
 *              these constants, it is important to use '<=' or '>' (rather
 *              than '<' or '>=').
 *
 *              For example. do:
 *                  if (NV_STATUS_LEVEL(status) <= NV_STATUS_LEVEL_WARN)
 *              rather than:
 *                  if (NV_STATUS_LEVEL(status) < NV_STATUS_LEVEL_ERR)
 *
 *              By being consistent in this manner, it is easier to systematically
 *              add additional level constants.  New levels are likely to lower
 *              (rather than raise) the severity of _ERR codes.  For example,
 *              if we were to add NV_STATUS_LEVEL_RETRY to indicate hardware
 *              failures that may be recoverable (e.g. RM_ERR_TIMEOUT_RETRY
 *              or RM_ERR_BUSY_RETRY), it would be less severe than
 *              NV_STATUS_LEVEL_ERR the level to which these status codes now
 *              belong.  Using '<=' and '>' ensures your code is not broken in
 *              cases like this.
 */
#define NV_STATUS_LEVEL(_S)                                               \
    ((_S) == NV_OK?                               NV_STATUS_LEVEL_OK:     \
    ((_S) != NV_ERR_GENERIC && (_S) & 0x00010000? NV_STATUS_LEVEL_WARN:   \
                                                  NV_STATUS_LEVEL_ERR))

/*!
 * @def         NV_STATUS_LEVEL
 * @see         NV_STATUS_LEVEL_OK
 * @see         NV_STATUS_LEVEL_WARN
 * @see         NV_STATUS_LEVEL_ERR
 * @brief       Character representing status code level
 */
#define NV_STATUS_LEVEL_CHAR(_S)                           \
    ((_S) == NV_OK?                                '0':    \
    ((_S) != NV_ERR_GENERIC && (_S) & 0x00010000?  'W':    \
                                                   'E'))

// Function definitions
const char *nvstatusToString(NV_STATUS nvStatusIn);

#ifdef __cplusplus
}
#endif

#endif /* SDK_NVSTATUS_H */
