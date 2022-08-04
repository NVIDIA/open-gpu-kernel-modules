/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NV_DPCD20_DSC_SUPPORT                                                   (0x00000060) /* R-XUR */
#define NV_DPCD20_DSC_SUPPORT_PASS_THROUGH_SUPPORT                                       1:1 /* R-XUF */
#define NV_DPCD20_DSC_SUPPORT_PASS_THROUGH_SUPPORT_NO                           (0x00000000) /* R-XUV */
#define NV_DPCD20_DSC_SUPPORT_PASS_THROUGH_SUPPORT_YES                          (0x00000001) /* R-XUV */

#define NV_DPCD20_DSC_PASS_THROUGH                                              (0x00000160) /* R-XUR */
#define NV_DPCD20_DSC_PASS_THROUGH_ENABLE                                                1:1 /* R-XUF */
#define NV_DPCD20_DSC_PASS_THROUGH_ENABLE_NO                                    (0x00000000) /* R-XUV */
#define NV_DPCD20_DSC_PASS_THROUGH_ENABLE_YES                                   (0x00000001) /* R-XUV */

#define NV_DPCD20_GUID_2                                                        (0x00000040) /* R-XUR */

// PANEL REPLAY RELATED DPCD
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY                                         (0x000000B0)
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SUPPORTED                                       0:0
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SUPPORTED_NO                            (0x00000000)
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SUPPORTED_YES                           (0x00000001)
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SEL_UPDATE                                      1:1
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SEL_UPDATE_NO                           (0x00000000)
#define NV_DPCD20_PANEL_REPLAY_CAPABILITY_SEL_UPDATE_YES                          (0x00000001)

#define NV_DPCD20_PANEL_REPLAY_CONFIGURATION                                      (0x000001B0)
#define NV_DPCD20_PANEL_REPLAY_CONFIGURATION_ENABLE_PR_MODE                               0:0
#define NV_DPCD20_PANEL_REPLAY_CONFIGURATION_ENABLE_PR_MODE_NO                    (0x00000000)
#define NV_DPCD20_PANEL_REPLAY_CONFIGURATION_ENABLE_PR_MODE_YES                   (0x00000001)

/// BRANCH SPECIFIC DSC CAPS
#define NV_DPCD20_BRANCH_DSC_OVERALL_THROUGHPUT_MODE_0                            (0x000000A0)
#define NV_DPCD20_BRANCH_DSC_OVERALL_THROUGHPUT_MODE_0_VALUE                              7:0

#define NV_DPCD20_BRANCH_DSC_OVERALL_THROUGHPUT_MODE_1                            (0x000000A1)
#define NV_DPCD20_BRANCH_DSC_OVERALL_THROUGHPUT_MODE_1_VALUE                              7:0

#define NV_DPCD20_BRANCH_DSC_MAXIMUM_LINE_BUFFER_WIDTH                            (0x000000A2)
#define NV_DPCD20_BRANCH_DSC_MAXIMUM_LINE_BUFFER_WIDTH_VALUE                              7:0

