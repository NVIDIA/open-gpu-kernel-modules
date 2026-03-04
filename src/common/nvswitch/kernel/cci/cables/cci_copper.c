/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"

#include "cci/cci_nvswitch.h"
#include "cci/cci_priv_nvswitch.h"
#include "cci/cci_cables_nvswitch.h"

#include "nvswitch/ls10/dev_nvlipt_lnk_ip.h"
#include "ls10/ls10.h"

static NvlStatus
_cci_cable_initialize_links
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    NvlStatus retval;
    NvlStatus loopStatus;
    NvU64 linkState;
    NvU64 linkMask;
    NvU32 linkId;
    nvlink_link *link;

    loopStatus = NVL_SUCCESS;
    pOnboardState = &device->pCci->moduleState[moduleId];

    // De-assert lpmode
    retval = cciSetLPMode(device, moduleId, NV_FALSE);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    linkMask = pOnboardState->linkTrainMask;

    FOR_EACH_INDEX_IN_MASK(64, linkId, linkMask)
    {
        NVSWITCH_ASSERT(linkId < NVSWITCH_LINK_COUNT(device));

        link = nvswitch_get_link(device, linkId);

        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID(device, link->linkNumber, NVLIPT_LNK) ||
            (linkId >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            continue;
        }

        retval = link->link_handlers->get_dl_link_mode(link, &linkState);

        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to get link state for link: %d\n",
                __FUNCTION__, linkId);
            continue;
        }

        // This only makes sense to do in the driver
        if (linkState == NVLINK_LINKSTATE_HS ||
            linkState == NVLINK_LINKSTATE_SAFE ||
            linkState == NVLINK_LINKSTATE_RECOVERY ||
            linkState == NVLINK_LINKSTATE_SLEEP)
        {
            continue;
        }

        // Shutdown and Reset link
        if (linkState != NVLINK_LINKSTATE_RESET)
        {
            retval = nvswitch_request_tl_link_state_ls10(link,
                NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_SHUTDOWN,
                NV_TRUE);

            if (retval != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: TL link shutdown request failed for link: %d\n",
                    __FUNCTION__, linkId);
            }

            retval = nvswitch_request_tl_link_state_ls10(link,
                NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_RESET,
                NV_TRUE);

            if (retval != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: TL link reset request failed for link: %d\n",
                    __FUNCTION__, linkId);
                loopStatus = retval;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if (loopStatus != NVL_SUCCESS)
    {
        return loopStatus;
    }

    // Have SOE perform system register setup
    retval = cciModulesOnboardSOE(device, moduleId);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "%s: System register setup failed for module: %d\n",
                    __FUNCTION__, moduleId);
        return retval;
    }

    return NVL_SUCCESS;
}

/*
 * @brief Initialize copper cable
 *
 * @param[in]  device            nvswitch_device  pointer
 * @param[in]  moduleId          Module will be initialized. 
 *                             
 */
void 
cciCablesInitializeCopperAsync
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_ONBOARD_STATE nextStateSuccess;
    CCI_MODULE_ONBOARD_STATE nextStateFail;

    nvswitch_os_memset(&nextStateSuccess, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    nvswitch_os_memset(&nextStateFail, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    nextStateSuccess.onboardPhase = CCI_ONBOARD_PHASE_LAUNCH_ALI;
    nextStateFail.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    cciModuleOnboardPerformPhaseAsync(device, moduleId,
                                      _cci_cable_initialize_links,
                                      nextStateSuccess,
                                      nextStateFail);
    cciSetLedsInitialize(device, moduleId);
}
