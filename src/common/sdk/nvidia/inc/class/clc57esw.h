/*
 * Copyright (c) 2009-2018, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef _clc57e_sw_h_
#define _clc57e_sw_h_

/* This file is *not* auto-generated. */

#define NVC57E_WINDOWS_NOTIFY_RM                                                        (0x0000058C)
#define NVC57E_WINDOWS_NOTIFY_RM_VSYNC_STATE_CHANGE                                             0:0
#define NVC57E_WINDOWS_NOTIFY_RM_VSYNC_STATE_CHANGE_FALSE                               (0x00000000)
#define NVC57E_WINDOWS_NOTIFY_RM_VSYNC_STATE_CHANGE_TRUE                                (0x00000001)
#define NVC57E_WINDOWS_NOTIFY_RM_VSYNC_STATE                                                    1:1
#define NVC57E_WINDOWS_NOTIFY_RM_VSYNC_STATE_OFF                                        (0x00000000)
#define NVC57E_WINDOWS_NOTIFY_RM_VSYNC_STATE_ON                                         (0x00000001)
#define NVC57E_WINDOWS_NOTIFY_RM_ASSOCIATED_HEAD                                                7:4

#define SwSetMClkSwitch                                                                 Reserved05[1]

#define NVC57E_SW_SET_MCLK_SWITCH                                                       (0x000002B4)
#define NVC57E_SW_SET_MCLK_SWITCH_ENABLE                                                        0:0
#define NVC57E_SW_SET_MCLK_SWITCH_ENABLE_FALSE                                          (0x00000000)
#define NVC57E_SW_SET_MCLK_SWITCH_ENABLE_TRUE                                           (0x00000001)

#endif // _clc57e_sw_h_

