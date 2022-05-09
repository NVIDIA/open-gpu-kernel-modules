/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_auxdefs.h                                                      *
*    Definitions for DPCD AUX offsets                                       *
*    Should be used sparingly (DPCD HAL preferred)                          *
*                                                                           *
\***************************************************************************/

#ifndef __DP_AUXDEFS_H__
#define __DP_AUXDEFS_H__

#define DPCD_MESSAGEBOX_SIZE                    48

//
// This definitions are being used for orin Hdcp opensourcing. Ideally this
// should be replaced with build flags. Bug ID: 200733434
//
#define DP_OPTION_HDCP_SUPPORT_ENABLE           1   /* HDCP Enable */

#define DP_OPTION_HDCP_12_ENABLED               1   /* DP1.2 HDCP ENABLE */

#define DP_OPTION_QSE_ENABLED                   1   /* Remove here when QSE p4r check-in */

//
//  If a message is outstanding for at least 4 seconds
//  assume no reply is coming through
//
#define DPCD_MESSAGE_REPLY_TIMEOUT              4000

#define DPCD_LINK_ADDRESS_MESSAGE_RETRIES       20  // 20 retries
#define DPCD_LINK_ADDRESS_MESSAGE_COOLDOWN      10  // 10ms between attempts

// pointing to the defaults for LAM settings to start with
#define DPCD_REMOTE_DPCD_WRITE_MESSAGE_RETRIES   DPCD_LINK_ADDRESS_MESSAGE_RETRIES
#define DPCD_REMOTE_DPCD_WRITE_MESSAGE_COOLDOWN  DPCD_LINK_ADDRESS_MESSAGE_COOLDOWN

#define DPCD_REMOTE_DPCD_READ_MESSAGE_RETRIES   7   // 7 retries
#define DPCD_REMOTE_DPCD_READ_MESSAGE_COOLDOWN  DPCD_LINK_ADDRESS_MESSAGE_COOLDOWN
#define DPCD_REMOTE_DPCD_READ_MESSAGE_COOLDOWN_BKSV 20  // 20ms between attempts

#define DPCD_QUERY_STREAM_MESSAGE_RETRIES       7   // 7 retries
#define DPCD_QUERY_STREAM_MESSAGE_COOLDOWN      20  // 20ms between attempts

#define MST_EDID_RETRIES                        20
#define MST_EDID_COOLDOWN                       10

#define MST_ALLOCATE_RETRIES                    10
#define MST_ALLOCATE_COOLDOWN                   10

#define HDCP_AUTHENTICATION_RETRIES             6   // 6 retries
#define HDCP_CPIRQ_RXSTAUS_RETRIES              3  
#define HDCP_AUTHENTICATION_COOLDOWN            1000// 1 sec between attempts
#define HDCP22_AUTHENTICATION_COOLDOWN          2000// 2 sec between attempts
#define HDCP_AUTHENTICATION_COOLDOWN_HPD        3000// 3 sec for first stream Add
#define HDCP_CPIRQ_RXSTATUS_COOLDOWN            20  // 20ms between attempts

// Need to re-submit Stream Validation request to falcon microcontroller after 1 sec if current request fails
#define HDCP_STREAM_VALIDATION_RESUBMIT_COOLDOWN    1000

//
// Wait till 8secs for completion of the KSV and Stream Validation, if that doesn't complete
// then timeout.
//
#define HDCP_STREAM_VALIDATION_REQUEST_COOLDOWN 8000

#define DPCD_OUI_NVIDIA                         0x00044B

//
// Define maximum retry count that checking Payload ID table updated before
// trigger ACT sequence.
//
#define PAYLOADIDTABLE_UPDATED_CHECK_RETRIES    300

#endif // __DP_AUXDEFS_H__
