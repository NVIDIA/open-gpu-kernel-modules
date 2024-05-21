/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlink.h"
#include "nvlink_export.h"
#include "nvlink_os.h"
#include "../nvlink_ctx.h"
#include "../nvlink_helper.h"

/**
 * Initialize all the endpoints from OFF to SWCFG state
 *
 * @param[in]  links    Array of link endpoints to initialize
 * @param[in]  numLinks Number of links in the array
 * @param[in]  flags    Flags to determine whether init is sync/async
 */
void
nvlink_core_init_links_from_off_to_swcfg
(
    nvlink_link **pLinks,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvlStatus  status = NVL_SUCCESS;
    NvU64      linkMode;
    NvU32      i;

    // Sanity check the links array
    if (pLinks == NULL)
    {
        nvlink_assert(0);
        return;
    }

    // Return early if there are no links to initialize
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No links to initialize\n",
            __FUNCTION__));
        return;
    }

    // Step 1: Perform INITPHASE1 on all endpoints
    nvlink_core_initphase1(pLinks, numLinks, flags);

    // Get state on all links. This ensures NVLINK_LINKSTATE_INITPHASE1 completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < numLinks; i++)
        {
            if (pLinks[i] == NULL)
                continue;

            status = pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is in bad state\n",
                    __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
            }
        }
    }

    // Step 2.0: RECEIVER DETECT: Enable RX termination on all the endpoints
    nvlink_core_rx_init_term(pLinks, numLinks, flags);

    // Get state on all links. This ensures NVLINK_SUBLINK_STATE_RX_INIT_TERM completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < numLinks; i++)
        {
            if (pLinks[i] == NULL)
                continue;

            // If receiver detect has passed for the link, move to next link
            if (pLinks[i]->bRxDetected)
                continue;

            status = pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is in bad state\n",
                    __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
            }
        }
    }

    // Step 2.1 RECEIVER DETECT :Perform receiver detect on all the endpoints
    nvlink_core_set_rx_detect(pLinks, numLinks, flags);

    // Get state on all links. This ensures receiver detect command completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < numLinks; i++)
        {
            if (pLinks[i] == NULL)
                continue;

            // In NVLink3.0 and 3.1, RXDET must be called serially - done above (Bug 2546220)
            if (!((pLinks[i]->version == NVLINK_DEVICE_VERSION_30) ||
                  (pLinks[i]->version == NVLINK_DEVICE_VERSION_31)))
            {
                // If receiver detect has passed for the link, move to next link
                if (pLinks[i]->bRxDetected)
                    continue;

                status = pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode);
                if ((status != NVL_SUCCESS) ||
                    (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: Link %s:%s is in bad state\n",
                        __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
                }
            }
        }
    }

    // Step 2.2 RECEIVER DETECT :Poll for output of receiver detect on all the endpoints
    nvlink_core_get_rx_detect(pLinks, numLinks, flags);


    /***************** Receiver Detect is completed at this point ****************/
    /***************** Proceed with the link initialization steps ****************/


    // Enable Common mode on all Tx's
    nvlink_core_enable_common_mode(pLinks, numLinks, flags);

    // Get state on all links. This ensures NVLINK_SUBLINK_STATE_TX_COMMON_MODE completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < numLinks; i++)
        {
            if (pLinks[i] == NULL)
                continue;

            // If receiver detect failed for the link, move to next link
            if (!pLinks[i]->bRxDetected || pLinks[i]->bTxCommonModeFail)
                continue;

            status = pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is in bad state\n",
                    __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
            }
        }
    }

    // Put all Rx's in RXCAL
    nvlink_core_calibrate_links(pLinks, numLinks, flags);

    // Disable Tx common mode
    nvlink_core_disable_common_mode(pLinks, numLinks, flags);

    // Set Data Ready and Enable
    nvlink_core_enable_data(pLinks, numLinks, flags);

    // Get state on all links. This ensures NVLINK_SUBLINK_STATE_TX_DATA_READY completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < numLinks; i++)
        {
            if (pLinks[i] == NULL)
                continue;

            // If receiver detect failed for the link, move to next link
            if (!pLinks[i]->bRxDetected || pLinks[i]->bTxCommonModeFail)
                continue;

            status = pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is in bad state\n",
                    __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
            }
        }
    }

    // Put the links in SAFE mode
    for (i = 0; i < numLinks; i++)
    {
        if (pLinks[i] == NULL)
            continue;

        // If receiver detect failed for the link, move to next link
        if (!pLinks[i]->bRxDetected || pLinks[i]->bTxCommonModeFail)
            continue;

        linkMode = 0;
        if (pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s",
                __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
        }

        if ((linkMode != NVLINK_LINKSTATE_SAFE) && (linkMode != NVLINK_LINKSTATE_HS))
        {
            // Check if the link has reached failed state
            if (pLinks[i]->state == NVLINK_LINKSTATE_FAIL)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s:%s marked as failed.\n",
                    pLinks[i]->dev->deviceName, pLinks[i]->linkName));
                continue;
            }

            //
            // Check if number of attempts to put the link into
            // safe state has already exceeded the maximum number
            // of retries. If yes, mark the link as failed
            //
            // On NVLink3.0, we don't support retraining in the driver.
            // However MODS test 252 (on NVL3+ specifically)  will train 
            // HS->OFF->HS many times. This check causes RM to stop
            // training after NVLINK_MAX_NUM_SAFE_RETRIES times
            //
            if ((pLinks[i]->safe_retries > NVLINK_MAX_NUM_SAFE_RETRIES) &&
                (pLinks[i]->version < NVLINK_DEVICE_VERSION_30))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "Max safe mode retries reached for %s:%s. Marking it as failed.\n",

                pLinks[i]->dev->deviceName, pLinks[i]->linkName));
                pLinks[i]->state = NVLINK_LINKSTATE_FAIL;
                continue;
            }

            // Put the link in safe state and increment the retry count
            pLinks[i]->link_handlers->set_dl_link_mode(pLinks[i], NVLINK_LINKSTATE_SAFE, flags);
            pLinks[i]->safe_retries++;
        }
    }

    // Poll for links to enter SAFE mode
    for (i = 0; i < numLinks; i++)
    {
        if (pLinks[i] == NULL)
            continue;

        status = nvlink_core_wait_for_link_init(pLinks[i]);
        if (status == NVL_SUCCESS)
        {
            pLinks[i]->powerStateTransitionStatus = nvlink_power_state_in_L0;
        }
    }

    // Send INITNEGOTIATE to all the links
    nvlink_core_initnegotiate(pLinks, numLinks, flags);
}

/**
 * Initialize all the endpoints from OFF to SWCFG state
 * Used for NvLink 4.0+
 *
 * @param[in]  links    Array of link endpoints to initialize
 * @param[in]  numLinks Number of links in the array
 * @param[in]  flags    Flags to determine whether init is sync/async
 */
void
nvlink_core_init_links_from_off_to_swcfg_non_ALI
(
    nvlink_link **pLinks,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvlStatus  status = NVL_SUCCESS;
    NvU64      linkMode;
    NvU32      i;

    // Sanity check the links array
    if (pLinks == NULL)
    {
        nvlink_assert(0);
        return;
    }

    // Return early if there are no links to initialize
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: No links to initialize\n",
            __FUNCTION__));
        return;
    }

    // Step 1: Perform INITPHASE1 on all endpoints
    nvlink_core_initphase1(pLinks, numLinks, flags);

    // Get state on all links. This ensures NVLINK_LINKSTATE_INITPHASE1 completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < numLinks; i++)
        {
            if (pLinks[i] == NULL)
                continue;

            status = pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is in bad state\n",
                    __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
            }
        }
    }
    
    // Step 2 RECEIVER DETECT :Perform receiver detect on all the endpoints
    nvlink_core_set_rx_detect(pLinks, numLinks, flags);

    // Get state on all links. This ensures receiver detect command completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < numLinks; i++)
        {
            if (pLinks[i] == NULL)
                continue;

            // In NVLink3.0 and 3.1, RXDET must be called serially - done above (Bug 2546220)
            if (!((pLinks[i]->version == NVLINK_DEVICE_VERSION_30) ||
                  (pLinks[i]->version == NVLINK_DEVICE_VERSION_31)))
            {
                // If receiver detect has passed for the link, move to next link
                if (pLinks[i]->bRxDetected)
                    continue;

                status = pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode);
                if ((status != NVL_SUCCESS) ||
                    (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: Link %s:%s is in bad state\n",
                        __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
                }
            }
        }
    }

    // Step 2.1 RECEIVER DETECT :Poll for output of receiver detect on all the endpoints
    nvlink_core_get_rx_detect(pLinks, numLinks, flags);


    /***************** Receiver Detect is completed at this point ****************/
    /***************** Proceed with the link initialization steps ****************/

    // Step 3: Enable Common mode on all Tx's
    nvlink_core_enable_common_mode(pLinks, numLinks, flags);

    // Get state on all links. This ensures NVLINK_SUBLINK_STATE_TX_COMMON_MODE completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < numLinks; i++)
        {
            if (pLinks[i] == NULL)
                continue;

            // If receiver detect failed for the link, move to next link
            if (!pLinks[i]->bRxDetected || pLinks[i]->bTxCommonModeFail)
                continue;

            status = pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is in bad state\n",
                    __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
            }
        }
    }

    // Step 4: call INITPHASE5
    nvlink_core_initphase5(pLinks, numLinks, flags);

    // Get state on all links. This ensures NVLINK_SUBLINK_STATE_TX_DATA_READY completes
    if (flags == NVLINK_STATE_CHANGE_ASYNC)
    {
        for (i = 0; i < numLinks; i++)
        {
            if (pLinks[i] == NULL)
                continue;

            // If receiver detect failed for the link, move to next link
            if (!pLinks[i]->bRxDetected || pLinks[i]->bTxCommonModeFail || pLinks[i]->bInitphase5Fails)
                continue;

            status = pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode);
            if ((status != NVL_SUCCESS) ||
                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link %s:%s is in bad state\n",
                    __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
            }
        }
    }

    // Step 5: Put the links in SAFE mode
    for (i = 0; i < numLinks; i++)
    {
        if (pLinks[i] == NULL)
            continue;

        // If receiver detect failed for the link, move to next link
        if (!pLinks[i]->bRxDetected || pLinks[i]->bTxCommonModeFail || pLinks[i]->bInitphase5Fails)
            continue;

        linkMode = 0;
        if (pLinks[i]->link_handlers->get_dl_link_mode(pLinks[i], &linkMode))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s",
                __FUNCTION__, pLinks[i]->dev->deviceName, pLinks[i]->linkName));
        }

        if ((linkMode != NVLINK_LINKSTATE_SAFE) && (linkMode != NVLINK_LINKSTATE_HS))
        {
            // Check if the link has reached failed state
            if (pLinks[i]->state == NVLINK_LINKSTATE_FAIL)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s:%s marked as failed.\n",
                    pLinks[i]->dev->deviceName, pLinks[i]->linkName));
                continue;
            }

            //
            // Check if number of attempts to put the link into
            // safe state has already exceeded the maximum number
            // of retries. If yes, mark the link as failed
            //
            // On NVLink3.0, we don't support retraining in the driver.
            // However MODS test 252 (on NVL3+ specifically)  will train 
            // HS->OFF->HS many times. This check causes RM to stop
            // training after NVLINK_MAX_NUM_SAFE_RETRIES times
            //
            if ((pLinks[i]->safe_retries > NVLINK_MAX_NUM_SAFE_RETRIES) &&
                (pLinks[i]->version < NVLINK_DEVICE_VERSION_30))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "Max safe mode retries reached for %s:%s. Marking it as failed.\n",

                pLinks[i]->dev->deviceName, pLinks[i]->linkName));
                pLinks[i]->state = NVLINK_LINKSTATE_FAIL;
                continue;
            }

            // Put the link in safe state and increment the retry count
            pLinks[i]->link_handlers->set_dl_link_mode(pLinks[i], NVLINK_LINKSTATE_SAFE, flags);
            pLinks[i]->safe_retries++;
        }
    }

    // Poll for links to enter SAFE mode
    for (i = 0; i < numLinks; i++)
    {
        if (pLinks[i] == NULL)
            continue;

        status = nvlink_core_wait_for_link_init(pLinks[i]);
        if (status == NVL_SUCCESS)
        {
            pLinks[i]->powerStateTransitionStatus = nvlink_power_state_in_L0;
        }
    }

    // Send INITNEGOTIATE to all the links
    nvlink_core_initnegotiate(pLinks, numLinks, flags);
}

/**
 * Kick-off INITPHASE5 on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_initphase5
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvlStatus status     = NVL_SUCCESS;
        NvU64     dlLinkMode = 0;

        if (links[i] == NULL)
            continue;

        // INITPHASE5 is supported only for NVLINK version >= 4.0
        if (links[i]->version < NVLINK_DEVICE_VERSION_40)
            continue;

        // If receiver detect failed for the link, move to next link
        if (!links[i]->bRxDetected || links[i]->bTxCommonModeFail)
            continue;

        if (links[i]->link_handlers->get_dl_link_mode(links[i], &dlLinkMode))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
        }

        // Check if the link has reached failed state
        if (links[i]->state == NVLINK_LINKSTATE_FAIL)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                "%s: %s:%s marked as failed.\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            continue;
        }

        // Skip this step if link is in HS/SAFE
        if (dlLinkMode == NVLINK_LINKSTATE_HS ||
            dlLinkMode == NVLINK_LINKSTATE_SAFE)
        {
            continue;
        }

        status = links[i]->link_handlers->set_dl_link_mode(links[i],
                                                  NVLINK_LINKSTATE_INITPHASE5,
                                                  flags);

        // Although it fails we need to continue with the next link
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Initphase5 failed on Device:Link %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            links[i]->bInitphase5Fails = NV_TRUE;
        }
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Kick-off INITPHASE1 on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_initphase1
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvlStatus status     = NVL_SUCCESS;
        NvU64     tlLinkMode = 0;
        NvU64     dlLinkMode = 0;
        NvU64     txMode     = 0;
        NvU32     txSubMode  = 0;
        NvU64     rxMode     = 0;
        NvU32     rxSubMode  = 0;
        NvBool    bPhyUnlocked  = NV_FALSE;

        if (links[i] == NULL)
            continue;

        // INITPHASE1 is supported only for NVLINK version >= 3.0
        if (links[i]->version < NVLINK_DEVICE_VERSION_30)
            continue;

        if (links[i]->version >= NVLINK_DEVICE_VERSION_40)
            links[i]->link_handlers->get_uphy_load(links[i], &bPhyUnlocked);

        if (!bPhyUnlocked)
        {
            if (links[i]->link_handlers->get_tl_link_mode(links[i], &tlLinkMode))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to get TL link mode for %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            }

            if (links[i]->link_handlers->get_dl_link_mode(links[i], &dlLinkMode))
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to get link mode for %s:%s",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            }

            if (dlLinkMode != NVLINK_LINKSTATE_RESET)
            {
                if (links[i]->link_handlers->get_tx_mode(links[i], &txMode, &txSubMode))
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                       "%s: Unable to get tx sublink mode for %s:%s",
                       __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                }

                if (links[i]->link_handlers->get_rx_mode(links[i], &rxMode, &rxSubMode))
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: Unable to get rx sublink mode for %s:%s",
                        __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                }
            }
        }

        //
        // After pseudo-clean shutdown, sublink states are in OFF while
        // link states stay in SWCFG. So, use sublink state here to determine
        // if we should perform INITPHASE1 to cover both cold boot and
        // pseudo-clean shutdown cases
        // We also check the tl link state to see if the link is L2. Exiting
        // from L2 also requires INITPHASE1 to be re-run
        //
        if ((tlLinkMode == NVLINK_LINKSTATE_SLEEP) ||
            (dlLinkMode == NVLINK_LINKSTATE_RESET) ||
            (bPhyUnlocked)                         ||
            ((txMode    == NVLINK_SUBLINK_STATE_TX_OFF) &&
             (rxMode    == NVLINK_SUBLINK_STATE_RX_OFF)))
        {
            // Check if the link has reached failed state
            if (links[i]->state == NVLINK_LINKSTATE_FAIL)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: %s:%s marked as failed.\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }

            status = links[i]->link_handlers->set_dl_link_mode(links[i],
                                                      NVLINK_LINKSTATE_INITPHASE1,
                                                      flags);

            // Although it fails we need to continue with the next link
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Initphase failed on Device:Link %s:%s",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            }
        }
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Kick-off INITRXTERM on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_rx_init_term
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvU64 linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        if (links[i]->version < NVLINK_DEVICE_VERSION_22)
            continue;

        // If receiver detect has passed for the link, move to next link
        if (links[i]->bRxDetected)
            continue;

        if (links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
        }

        switch (linkMode)
        {
            case NVLINK_LINKSTATE_SAFE:
            case NVLINK_LINKSTATE_HS:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: %s:%s is already trained to SAFE or HIGH SPEED "
                    " RX Termination should have been enabled on the link\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            case NVLINK_LINKSTATE_FAULT:
            case NVLINK_LINKSTATE_RECOVERY:
            case NVLINK_LINKSTATE_FAIL:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link in bad state. Cannot enable RX termination "
                    "from current state for %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            default:
            {
                switch (links[i]->rx_sublink_state)
                {
                    case NVLINK_SUBLINK_STATE_RX_RXCAL:
                        continue;

                    default:
                    {
                        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                            "%s: Enabling RX Termination on %s:%s\n",
                            __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

                        if ((links[i]->link_handlers->set_rx_mode(links[i],
                                                        NVLINK_SUBLINK_STATE_RX_INIT_TERM,
                                                        flags)) != 0)
                        {
                            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                                "%s: Unable to enable RX termination for %s:%s\n",
                                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Kick-off receiver detect on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_set_rx_detect
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvlStatus status   = NVL_SUCCESS;
        NvU64     linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        if (links[i]->version < NVLINK_DEVICE_VERSION_22)
            continue;

        // If receiver detect has passed for the link, move to next link
        if (links[i]->bRxDetected)
            continue;

        if (links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
        }

        switch (linkMode)
        {
            case NVLINK_LINKSTATE_SAFE:
            case NVLINK_LINKSTATE_HS:
            {
                links[i]->bRxDetected = NV_TRUE;

                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: %s:%s is already trained to SAFE or HIGH SPEED\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            case NVLINK_LINKSTATE_FAULT:
            case NVLINK_LINKSTATE_RECOVERY:
            case NVLINK_LINKSTATE_FAIL:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link in bad state. Cannot perform RXDET from current state for %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            default:
            {
                switch (links[i]->rx_sublink_state)
                {
                    case NVLINK_SUBLINK_STATE_RX_RXCAL:
                        continue;

                    default:
                    {
                        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                            "%s: Performing RXDET on %s:%s\n",
                            __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

                        if ((links[i]->link_handlers->set_rx_detect(links[i], flags)) != 0)
                        {
                            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                                "%s: Unable to perform receiver detect for %s:%s\n",
                                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                        }

                        // In NVLink3.0 and 3.1, RXDET must be called serially (Bug 2546220)
                        if ((links[i]->version == NVLINK_DEVICE_VERSION_30) ||
                            (links[i]->version == NVLINK_DEVICE_VERSION_31))
                        {
                            // Get state on all links. This ensures receiver detect command completes
                            status = links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode);
                            if ((status != NVL_SUCCESS) ||
                                (linkMode == NVLINK_LINKSTATE_FAIL) || (linkMode == NVLINK_LINKSTATE_FAULT))
                            {
                                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                                    "%s: Link %s:%s is in bad state\n",
                                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                            }

                            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                                "%s: Checking RXDET status on %s:%s\n",
                                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

                            if ((links[i]->link_handlers->get_rx_detect(links[i])) != 0)
                            {
                                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                                    "%s: Receiver detect failed for %s:%s\n",
                                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                            }
                            else
                            {
                                links[i]->bRxDetected = NV_TRUE;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Get receiver detect status on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_get_rx_detect
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvU64 linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        // If receiver detect has passed for the link, move to next link
        if (links[i]->bRxDetected)
            continue;

        //
        // In NVLink3.0 and 3.1, RXDET must be called serially (Bug 2546220).
        // So this would have been already addressed
        //
        if ((links[i]->version == NVLINK_DEVICE_VERSION_30) ||
            (links[i]->version == NVLINK_DEVICE_VERSION_31))
            continue;

        if (links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
        }

        switch (linkMode)
        {
            case NVLINK_LINKSTATE_SAFE:
            case NVLINK_LINKSTATE_HS:
            {
                links[i]->bRxDetected = NV_TRUE;

                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: %s:%s is already trained to SAFE or HIGH SPEED. "
                    "RXDET should have passed on the link. Set bRxDetected to TRUE\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            case NVLINK_LINKSTATE_FAULT:
            case NVLINK_LINKSTATE_RECOVERY:
            case NVLINK_LINKSTATE_FAIL:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Link in bad state. Cannot poll RXDET from current state for %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            default:
            {
                switch (links[i]->rx_sublink_state)
                {
                    case NVLINK_SUBLINK_STATE_RX_RXCAL:
                        continue;

                    default:
                    {
                        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                            "%s: Checking RXDET status on %s:%s\n",
                            __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

                        if ((links[i]->link_handlers->get_rx_detect(links[i])) != 0)
                        {
                            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                                "%s: Receiver detect failed for %s:%s\n",
                                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                        }
                        else
                        {
                            links[i]->bRxDetected = NV_TRUE;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Get Enable TX common mode on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_enable_common_mode
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvU64 linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        if (!links[i]->bRxDetected)
        {
            // link did not pass RXDET, don't do anything
            continue;
        }

        if (links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode))
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            continue;
        }

        switch (linkMode)
        {
            case NVLINK_LINKSTATE_SAFE:
            case NVLINK_LINKSTATE_HS:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: %s:%s is already trained to SAFE or HIGH SPEED\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            case NVLINK_LINKSTATE_FAULT:
            case NVLINK_LINKSTATE_RECOVERY:
            case NVLINK_LINKSTATE_FAIL:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Cannot put Tx in common mode from current state for %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            default:
            {
                switch (links[i]->tx_sublink_state)
                {
                    case NVLINK_SUBLINK_STATE_TX_COMMON_MODE:
                    case NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE:
                    case NVLINK_SUBLINK_STATE_TX_DATA_READY:
                        continue;

                    default:
                    {
                        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                            "%s: Setting common mode on %s:%s\n",
                            __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

                        if ((links[i]->link_handlers->set_tx_mode(links[i],
                                                      NVLINK_SUBLINK_STATE_TX_COMMON_MODE,
                                                      flags)) != 0)
                        {
                            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                                "%s: Unable to put Tx in common mode for %s:%s\n",
                                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

                            links[i]->bTxCommonModeFail = NV_TRUE;
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Perform RX calibration on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_calibrate_links
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvlStatus status   = NVL_SUCCESS;
        NvU64     linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        // If receiver detect failed for the link, move to next link
        if (!links[i]->bRxDetected || links[i]->bTxCommonModeFail)
            continue;

        status = links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            continue;
        }

        switch (linkMode)
        {
            case NVLINK_LINKSTATE_SAFE:
            case NVLINK_LINKSTATE_HS:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: %s:%s is already trained to SAFE or HIGH SPEED\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            case NVLINK_LINKSTATE_FAULT:
            case NVLINK_LINKSTATE_RECOVERY:
            case NVLINK_LINKSTATE_FAIL:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Cannot put Rx in RXCAL mode from current state for %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            default:
            {
                if (links[i]->rx_sublink_state == NVLINK_SUBLINK_STATE_RX_RXCAL)
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                        "%s: Sublink already in RXCAL on %s:%s\n",
                        __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                    continue;
                }

                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: Setting RXCAL on %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

                if ((links[i]->link_handlers->set_rx_mode(links[i],
                                                    NVLINK_SUBLINK_STATE_RX_RXCAL,
                                                    flags)) != 0)
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: Unable to put Rx in RXCAL mode for %s:%s\n",
                        __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                }
                break;
            }
        }
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Get Disable TX common mode on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_disable_common_mode
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvlStatus status   = NVL_SUCCESS;
        NvU64     linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        if (!links[i]->bRxDetected || links[i]->bTxCommonModeFail)
        {
            // link did not pass RXDET or failed in common mode, don't do anything
            continue;
        }

        status = links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            continue;
        }

        switch (linkMode)
        {
            case NVLINK_LINKSTATE_SAFE:
            case NVLINK_LINKSTATE_HS:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: %s:%s is already trained to SAFE or HIGH SPEED\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            case NVLINK_LINKSTATE_FAULT:
            case NVLINK_LINKSTATE_RECOVERY:
            case NVLINK_LINKSTATE_FAIL:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Cannot disable Tx common mode from current state for %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            default:
            {
                switch (links[i]->tx_sublink_state)
                {
                    case NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE:
                    case NVLINK_SUBLINK_STATE_TX_DATA_READY:
                        continue;

                    default:
                    {
                        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                            "%s: Disabling common mode on %s:%s\n",
                            __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

                        if ((links[i]->link_handlers->set_tx_mode(links[i],
                                              NVLINK_SUBLINK_STATE_TX_COMMON_MODE_DISABLE,
                                              flags)) != 0)
                        {
                            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                                "%s: Unable to disable Tx common mode for %s:%s\n",
                                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Enable data on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_enable_data
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvlStatus status   = NVL_SUCCESS;
        NvU64     linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        // If receiver detect failed for the link, move to next link
        if (!links[i]->bRxDetected || links[i]->bTxCommonModeFail)
            continue;

        status = links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            continue;
        }

        switch (linkMode)
        {
            case NVLINK_LINKSTATE_SAFE:
            case NVLINK_LINKSTATE_HS:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s: %s:%s is already trained to SAFE or HIGH SPEED\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            case NVLINK_LINKSTATE_FAULT:
            case NVLINK_LINKSTATE_RECOVERY:
            case NVLINK_LINKSTATE_FAIL:
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Cannot put Data Ready and Enable from current state for %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }
            default:
            {
                switch (links[i]->tx_sublink_state)
                {
                    case NVLINK_SUBLINK_STATE_TX_DATA_READY:
                        continue;

                    default:
                    {
                        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                            "%s: Setting Data Ready on %s:%s\n",
                            __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));

                        if ((links[i]->link_handlers->set_tx_mode(links[i],
                                                       NVLINK_SUBLINK_STATE_TX_DATA_READY,
                                                       flags)) != 0)
                        {
                            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                                "%s: Unable to set Data Ready and Data Enable %s:%s\n",
                                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    //
    // We could have links which are faulty and cannot be initialized. But proceeding
    // the initialization sequence allows us to use other non-faulty links. Therefore
    // return success always.
    //
    return NVL_SUCCESS;
}

/**
 * Send INITNEGOTIATE command on the given array of links
 *
 * @param[in]  links     Array of nvlink_link pointers
 * @param[in]  numLinks  Number of links in the array
 * @param[in]  flags     Flags - Async/Sync
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_initnegotiate
(
    nvlink_link **links,
    NvU32         numLinks,
    NvU32         flags
)
{
    NvU32 i;

    // Sanity check the links array
    if (links == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    // Return early if link array is empty
    if (numLinks == 0)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
            "%s: Link array is empty\n",
            __FUNCTION__));
        return NVL_SUCCESS;
    }

    for (i = 0; i < numLinks; i++)
    {
        NvlStatus status   = NVL_SUCCESS;
        NvU64     linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        // If receiver detect failed for the link, move to next link
        if (!links[i]->bRxDetected || links[i]->bTxCommonModeFail ||
             links[i]->bSafeTransitionFail || links[i]->bInitphase5Fails)
        {
            continue;
        }

        if (links[i]->version < NVLINK_DEVICE_VERSION_30)
            continue;

        // Packet injection can only happen on links that are in SAFE or ACTIVE
        status = links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            continue;
        }

        if (linkMode == NVLINK_LINKSTATE_HS)
        {
            continue;
        }
        else if (linkMode == NVLINK_LINKSTATE_SAFE)
        {
            // Check if the link has reached failed state
            if (links[i]->state == NVLINK_LINKSTATE_FAIL)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s:%s marked as failed.\n",
                    links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }

            if ((links[i]->link_handlers->set_dl_link_mode(links[i],
                                                       NVLINK_LINKSTATE_INITNEGOTIATE,
                                                       flags)) != 0)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: INITNEGOTIATE Failed on device:link %s:%s\n",
                    __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            }
        }
    }

    // Bug 2398907 mentioned that a link pair can take upto 125us for DL stat to have CONFIG_GOOD.
    nvlink_sleep(1);

    for (i = 0; i < numLinks; i++)
    {
        NvlStatus status   = NVL_SUCCESS;
        NvU64     linkMode = NVLINK_LINKSTATE_OFF;

        if (links[i] == NULL)
            continue;

        // If receiver detect failed for the link, move to next link
        if (!links[i]->bRxDetected || links[i]->bTxCommonModeFail ||
             links[i]->bSafeTransitionFail)
        {
            continue;
        }

        if (links[i]->version < NVLINK_DEVICE_VERSION_30)
            continue;

        status = links[i]->link_handlers->get_dl_link_mode(links[i], &linkMode);
        if (status != NVL_SUCCESS)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Unable to get link mode for %s:%s\n",
                __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
            continue;
        }

        if ((linkMode == NVLINK_LINKSTATE_SAFE) || (linkMode == NVLINK_LINKSTATE_HS))
        {
            // Check if the link has reached failed state
            if (links[i]->state == NVLINK_LINKSTATE_FAIL)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
                    "%s:%s marked as failed.\n",
                    links[i]->dev->deviceName, links[i]->linkName));
                continue;
            }

            if (!(links[i]->bInitnegotiateConfigGood))
            {
                if (!((links[i]->link_handlers->set_dl_link_mode(links[i],
                                                 NVLINK_LINKSTATE_POST_INITNEGOTIATE,
                                                 flags)) == 0))
                {
                    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                        "%s: DL stat CONFIG GOOD failed on  %s:%s\n",
                        __FUNCTION__, links[i]->dev->deviceName, links[i]->linkName));
                }
            }
        }
    }

    return NVL_SUCCESS;
}

/**
 * Poll on SAFE/SWCFG on the given link
 *
 * @param[in]  link  nvlink_link pointer
 *
 * return NvlStatus
 */
NvlStatus
nvlink_core_wait_for_link_init
(
    nvlink_link *link
)
{
    NvlStatus status   = NVL_SUCCESS;
    NvU64     linkMode = NVLINK_LINKSTATE_OFF;

    if (link == NULL)
    {
        nvlink_assert(0);
        return NVL_BAD_ARGS;
    }

    //
    // Check for SW fail flags to exit early
    //
    // Note: We don't check for !bRxDetected here since driver unload/reload
    // clears our SW state leading incorrectly skipping links (bug 3164375).
    // For RXDET status, the linkstate checks below are sufficient
    //
    if (link->bTxCommonModeFail || link->bInitphase5Fails)
    {
        return NVL_ERR_INVALID_STATE;
    }

    status = link->link_handlers->get_dl_link_mode(link, &linkMode);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to get link mode for %s:%s\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));

        return status;
    }

    // skip polling if link become faulty
    if ((linkMode == NVLINK_LINKSTATE_FAULT)    ||
        (linkMode == NVLINK_LINKSTATE_RECOVERY) ||
        (linkMode == NVLINK_LINKSTATE_FAIL))
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to put link in SAFE %s:%s\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));

        // link is in bad state. don't do anything
        return NVL_ERR_INVALID_STATE;
    }

    // skip polling if link already transitioned or has not begun training
    if ((linkMode == NVLINK_LINKSTATE_SAFE) ||
        (linkMode == NVLINK_LINKSTATE_HS))
    {
        return NVL_SUCCESS;
    }
    else if ((linkMode == NVLINK_LINKSTATE_OFF)   ||
             (linkMode == NVLINK_LINKSTATE_RESET))
    {
        // Failing status is expected for non-initialized links
        return NVL_ERR_GENERIC;
    }

    // reset state since we're trying again
    link->bSafeTransitionFail = NV_FALSE;

    // poll for link state
    status = nvlink_core_poll_link_state(link,
                                         NVLINK_LINKSTATE_SAFE,
                                         NVLINK_TRANSITION_SAFE_TIMEOUT);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to put link in SAFE %s:%s\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));

        link->bSafeTransitionFail = NV_TRUE;
        return status;
    }

    // poll sublink state as well.
    status = nvlink_core_poll_sublink_state(link,
                                            NVLINK_SUBLINK_STATE_TX_SAFE,
                                            NVLINK_SUBLINK_SUBSTATE_TX_STABLE,
                                            link,
                                            NVLINK_SUBLINK_STATE_RX_SAFE,
                                            NVLINK_SUBLINK_SUBSTATE_RX_STABLE,
                                            NVLINK_TRANSITION_SAFE_TIMEOUT);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to put sublink in SAFE %s:%s\n",
             __FUNCTION__, link->dev->deviceName, link->linkName));

        link->bSafeTransitionFail = NV_TRUE;
        return status;
    }

    // link is in SAFE state, initialization is success.
    return NVL_SUCCESS;
}

