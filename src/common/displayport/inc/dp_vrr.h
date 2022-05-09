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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_vrr.h                                                          *
*    Prototypes and definitions related to VRR enablement               *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_VRR_H
#define INCLUDED_DP_VRR_H

#include "dp_object.h"

// Worstcase VRR enablement handshake timeout of 600ms (40x15ms)
#define VRR_ENABLE_STATUS_TIMEOUT_THRESHOLD      40
#define VRR_ENABLE_STATUS_TIMEOUT_INTERVAL_MS    15

// Retry enablement threshold in notifyShortPulse()
#define VRR_MAX_RETRIES                        3

namespace DisplayPort
{
    enum VrrEnableStage
    {
        VRR_ENABLE_STAGE_MONITOR_ENABLE_BEGIN,
        VRR_ENABLE_STAGE_MONITOR_ENABLE_CHALLENGE,
        VRR_ENABLE_STAGE_MONITOR_ENABLE_CHECK,
        VRR_ENABLE_STAGE_DRIVER_ENABLE_BEGIN,
        VRR_ENABLE_STAGE_DRIVER_ENABLE_CHALLENGE,
        VRR_ENABLE_STAGE_DRIVER_ENABLE_CHECK,
        VRR_ENABLE_STAGE_RESET_MONITOR,
        VRR_ENABLE_STAGE_INIT_PUBLIC_INFO,
        VRR_ENABLE_STAGE_GET_PUBLIC_INFO,
        VRR_ENABLE_STAGE_STATUS_CHECK,
    };

    struct DeviceImpl;

    class VrrEnablement : virtual public Object
    {
        private:
            DeviceImpl *parent;
            bool        bMonitorEnabled;

            bool    vrrGetPublicInfo(void);
            bool    vrrWaitOnEnableStatus(void);
            bool    vrrEnableMonitor(void);
            bool    vrrEnableDriver(void);

        public:

            VrrEnablement(DeviceImpl *parent)
             : parent(parent)
            {
                reset();
            }

            ~VrrEnablement()
            {
                parent = NULL;
                reset();
            }

            bool start(void);
            void reset(void)
            {
                bMonitorEnabled = false;
            }
            bool isMonitorEnabled(void);
            bool isDriverEnabled(void);
    };
}

#endif
