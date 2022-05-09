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
* Module: dp_vrr.cpp                                                        *
*    Implementation of VRR enablement                                       *
*                                                                           *
\***************************************************************************/
#include "dp_connectorimpl.h"
#include "dp_vrr.h"

using namespace DisplayPort;

bool VrrEnablement::start()
{
    bool rc;

    DP_LOG(("DPHAL_VRR_ENABLE> **** VRR Enablement Started ****"));
    rc = vrrGetPublicInfo();
    if(rc)
    {
        rc = vrrEnableMonitor();
        if(rc != true)
        {
            return false;
        }
        rc = vrrEnableDriver();
        if(rc != true)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    DP_LOG(("DPHAL_VRR_ENABLE> **** VRR Enablement Ends ****"));

    return true;
}

bool VrrEnablement::vrrGetPublicInfo()
{
    MainLink *main = this->parent->connector->main;
    if (main->vrrRunEnablementStage(VRR_ENABLE_STAGE_INIT_PUBLIC_INFO, NULL) != true)
    {
        return false;
    }
    if (main->vrrRunEnablementStage(VRR_ENABLE_STAGE_RESET_MONITOR, NULL) != true)
    {
        return false;
    }
    else
    {
        if (vrrWaitOnEnableStatus() != true)
        {
           return false;
        }
    }

    if (main->vrrRunEnablementStage(VRR_ENABLE_STAGE_GET_PUBLIC_INFO, NULL) != true)
    {
        return false;
    }

    return vrrWaitOnEnableStatus();
}

bool VrrEnablement::vrrEnableMonitor()
{
    MainLink *main = this->parent->connector->main;

    DP_LOG(("DPHAL_VRR_ENABLE> ** VRR_MON_ENABLE starts **"));

    // Always set the enable F/W state m/c to a known state.
    if(main->vrrRunEnablementStage(VRR_ENABLE_STAGE_RESET_MONITOR, NULL) != true)
    {
        return false;
    }

    // Wait for VRR to be 'ready'.
    if (vrrWaitOnEnableStatus() != true)
    {
       return false;
    }

    if(main->vrrRunEnablementStage(VRR_ENABLE_STAGE_MONITOR_ENABLE_BEGIN, NULL) != true)
    {
        return false;
    }

    // Wait for VRR to be 'ready'.
    if (vrrWaitOnEnableStatus() != true)
    {
       return false;
    }

    main->vrrRunEnablementStage(VRR_ENABLE_STAGE_MONITOR_ENABLE_CHALLENGE, NULL);

    // Wait for VRR to be ready.
    if (vrrWaitOnEnableStatus() != true)
    {
       return false;
    }
    // Compare and enable on successful comparison.
    if(main->vrrRunEnablementStage(VRR_ENABLE_STAGE_MONITOR_ENABLE_CHECK, NULL) == true)
    {
        this->bMonitorEnabled = true;
    }

    DP_LOG(("DPHAL_VRR_ENABLE> ** VRR_MON_ENABLE ends **"));

    return this->bMonitorEnabled;
}

bool VrrEnablement::vrrEnableDriver()
{
    NvU32 enableResult;

    MainLink *main = this->parent->connector->main;

    DP_LOG(("DPHAL_VRR_ENABLE> ** VRR_DRV_ENABLE starts **"));

    // Always set the enable F/W state m/c to a known state.
    if(main->vrrRunEnablementStage(VRR_ENABLE_STAGE_RESET_MONITOR, NULL) != true)
    {
        return false;
    }

    // Wait for VRR to be 'ready'.
    if (vrrWaitOnEnableStatus() != true)
    {
       return false;
    }

    if (main->vrrRunEnablementStage(VRR_ENABLE_STAGE_DRIVER_ENABLE_BEGIN, &enableResult) != true)
    {
        return false;
    }

    if (enableResult == NV0073_CTRL_DP_CMD_ENABLE_VRR_STATUS_PENDING)
    {
        // Wait for VRR to be ready.
        if (vrrWaitOnEnableStatus() != true)
        {
           return false;
        }
    }
    else if (enableResult == NV0073_CTRL_DP_CMD_ENABLE_VRR_STATUS_OK)
    {
        return true;
    }

    if (main->vrrRunEnablementStage(VRR_ENABLE_STAGE_DRIVER_ENABLE_CHALLENGE, NULL) != true)
    {
        return false;
    }

    // Wait for VRR to be 'ready'.
    if (vrrWaitOnEnableStatus() != true)
    {
       return false;
    }

    if (main->vrrRunEnablementStage(VRR_ENABLE_STAGE_DRIVER_ENABLE_CHECK, NULL) != true)
    {
        return false;
    }

    DP_LOG(("DPHAL_VRR_ENABLE> ** VRR_DRV_ENABLE ends **"));

    return true;
}

bool VrrEnablement::vrrWaitOnEnableStatus(void)
{
    NvU32 timeout = VRR_ENABLE_STATUS_TIMEOUT_THRESHOLD;
    NvU32 enableResult;

    MainLink *main = this->parent->connector->main;
    ConnectorImpl *connector = this->parent->connector;
    do
    {
        if (main->vrrRunEnablementStage(VRR_ENABLE_STAGE_STATUS_CHECK, &enableResult) == true)
        {
            return true;
        }
        else
        {
            if (enableResult == NV0073_CTRL_DP_CMD_ENABLE_VRR_STATUS_READ_ERROR)
            {
                return false;
            }
            else if (enableResult == NV0073_CTRL_DP_CMD_ENABLE_VRR_STATUS_PENDING)
            {
                Timeout timeout(connector->timer, VRR_ENABLE_STATUS_TIMEOUT_INTERVAL_MS);
                while(timeout.valid());
                continue;
            }
            else
            {
                return false;
            }
        }
    }while(--timeout);

    return false;
}

bool VrrEnablement::isMonitorEnabled(void)
{
    return (this->bMonitorEnabled);
}

bool VrrEnablement::isDriverEnabled(void)
{
    NvU32 enableResult;
    MainLink *main = this->parent->connector->main;
    if (main->vrrRunEnablementStage(VRR_ENABLE_STAGE_DRIVER_ENABLE_CHECK,
                                    &enableResult) == true)
    {
        return true;
    }
    return false;
}
