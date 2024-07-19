/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * For the given link, check whether the link state is at the requested state.
 *
 * @param[in]  link       NVLink link pointer
 * @param[in]  linkState  Target Link State
 *
 * return NV_TRUE if the link is in the given state
 */
NvBool
nvlink_core_check_link_state
(
    nvlink_link *link,
    NvU64        linkState
)
{
    NvU64     crntDlLinkMode = NVLINK_LINKSTATE_OFF;
    NvU64     crntTlLinkMode = NVLINK_LINKSTATE_OFF;
    NvlStatus status         = NVL_SUCCESS;

    if (link == NULL)
    {
        return NV_FALSE;
    }

    switch (linkState)
    {
        case NVLINK_LINKSTATE_RESET:
        case NVLINK_LINKSTATE_SAFE:
        if (link->version >= NVLINK_DEVICE_VERSION_50)
            return NVL_SUCCESS;

            // fall-through
        case NVLINK_LINKSTATE_OFF:
        case NVLINK_LINKSTATE_HS:
        {
            status = link->link_handlers->get_dl_link_mode(link, &crntDlLinkMode);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to get DL link mode for %s:%s\n",
                    __FUNCTION__, link->dev->deviceName, link->linkName));
                return NV_FALSE;
            }

            if (crntDlLinkMode == linkState)
            {
                return NV_TRUE;
            }
            break;
        }
        case NVLINK_LINKSTATE_ALI:
        {
            status = link->link_handlers->get_tl_link_mode(link, &crntTlLinkMode);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to get TL link mode for %s:%s\n",
                    __FUNCTION__, link->dev->deviceName, link->linkName));
                return NV_FALSE;
            }

            status = link->link_handlers->get_dl_link_mode(link, &crntDlLinkMode);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to get DL link mode for %s:%s\n",
                    __FUNCTION__, link->dev->deviceName, link->linkName));
                return NV_FALSE;
            }

            if (crntTlLinkMode == NVLINK_LINKSTATE_HS &&
                (crntDlLinkMode == NVLINK_LINKSTATE_HS ||
                 crntDlLinkMode == NVLINK_LINKSTATE_SLEEP))
            {
                return NV_TRUE;
            }
            break;
        }
        case NVLINK_LINKSTATE_SLEEP:
        case NVLINK_LINKSTATE_ACTIVE_PENDING:
        {
            status = link->link_handlers->get_tl_link_mode(link, &crntTlLinkMode);
            if (status != NVL_SUCCESS)
            {
                NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                    "%s: Unable to get TL link mode for %s:%s\n",
                    __FUNCTION__, link->dev->deviceName, link->linkName));
                return NV_FALSE;
            }

            if (crntTlLinkMode == linkState)
            {
                return NV_TRUE;
            }
            break;
        }
    }

    // return false for default case or the states are not matching
    return NV_FALSE;
}

/**
 * For the given link, check whether the tx sublink state is at the
 * requested state.
 *
 * @param[in]  link            NVLink link pointer
 * @param[in]  txSublinkState  Target Tx Sublink State
 *
 * return NV_TRUE if the tx sublink is in the given state
 */
NvBool
nvlink_core_check_tx_sublink_state
(
    nvlink_link *link,
    NvU64        txSublinkState
)
{
    NvlStatus status = NVL_SUCCESS;

    NvU64 crntTxSublinkMode    = NVLINK_SUBLINK_STATE_TX_OFF;
    NvU32 crntTxSublinkSubMode = NVLINK_SUBLINK_SUBSTATE_TX_STABLE;

    if (link == NULL)
    {
        return NV_FALSE;
    }

    status = link->link_handlers->get_tx_mode(link,
                                              &crntTxSublinkMode,
                                              &crntTxSublinkSubMode);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to get TX sublink mode for %s:%s\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));
        return NV_FALSE;
    }

    switch (txSublinkState)
    {
        case NVLINK_SUBLINK_STATE_TX_OFF:
        {
            if (crntTxSublinkMode == NVLINK_SUBLINK_STATE_TX_OFF)
            {
                return NV_TRUE;
            }
            break;
        }
        case NVLINK_SUBLINK_STATE_TX_SAFE:
        {
            if (crntTxSublinkMode == NVLINK_SUBLINK_STATE_TX_SAFE)
            {
                return NV_TRUE;
            }
            break;
        }
        case NVLINK_SUBLINK_STATE_TX_HS:
        {
            if ((crntTxSublinkMode == NVLINK_SUBLINK_STATE_TX_HS) ||
                (crntTxSublinkMode == NVLINK_SUBLINK_STATE_TX_SINGLE_LANE))
            {
                return NV_TRUE;
            }
            break;
        }
    }

    // return false for default case or the states are not matching
    return NV_FALSE;
}

/**
 * For the given link, check whether the rx sublink state is at the
 * requested state.
 *
 * @param[in]  link            NVLink link pointer
 * @param[in]  rxSublinkState  Target Rx Sublink State
 *
 * return NV_TRUE if the rx sublink is in the given state
 */
NvBool
nvlink_core_check_rx_sublink_state
(
    nvlink_link *link,
    NvU64        rxSublinkState
)
{
    NvlStatus status = NVL_SUCCESS;

    NvU64 crntRxSublinkMode    = NVLINK_SUBLINK_STATE_RX_OFF;
    NvU32 crntRxSublinkSubMode = NVLINK_SUBLINK_SUBSTATE_RX_STABLE;

    if (link == NULL)
    {
        return NV_FALSE;
    }

    status = link->link_handlers->get_rx_mode(link,
                                              &crntRxSublinkMode,
                                              &crntRxSublinkSubMode);
    if (status != NVL_SUCCESS)
    {
        NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
            "%s: Unable to get TX sublink mode for %s:%s\n",
            __FUNCTION__, link->dev->deviceName, link->linkName));
        return NV_FALSE;
    }

    switch (rxSublinkState)
    {
        case NVLINK_SUBLINK_STATE_RX_OFF:
        {
            if (crntRxSublinkMode == NVLINK_SUBLINK_STATE_RX_OFF)
            {
                return NV_TRUE;
            }
            break;
        }
        case NVLINK_SUBLINK_STATE_RX_SAFE:
        {
            if (crntRxSublinkMode == NVLINK_SUBLINK_STATE_RX_SAFE)
            {
                return NV_TRUE;
            }
            break;
        }
        case NVLINK_SUBLINK_STATE_RX_HS:
        {
            if ((crntRxSublinkMode == NVLINK_SUBLINK_STATE_RX_HS) ||
                (crntRxSublinkMode == NVLINK_SUBLINK_STATE_RX_SINGLE_LANE))
            {
                return NV_TRUE;
            }
            break;
        }
    }

    // return false for default case or the states are not matching
    return NV_FALSE;
}

/**
 * Poll the link to reach the specified state upto the given timeout.
 * Link state transition is considered failed once timeout occurs.
 *
 * @param[in]  link       NVLink link pointer
 * @param[in]  linkState  Target Link state
 * @param[in]  timeout    Timeout
 *
 * return NVL_SUCCESS if the link transitioned to the target state
 */
NvlStatus
nvlink_core_poll_link_state
(
    nvlink_link *link,
    NvU64        linkState,
    NvU32        timeout
)
{
    NvU64 currentLinkState = ~0;

    if (link == NULL)
    {
        return NVL_BAD_ARGS;
    }

    if (link->version >= NVLINK_DEVICE_VERSION_50)
    {
        switch (linkState)
        {
        case NVLINK_LINKSTATE_RESET:
        case NVLINK_LINKSTATE_SAFE:
            return NVL_SUCCESS;
        case NVLINK_LINKSTATE_OFF:
        case NVLINK_LINKSTATE_HS:
        case NVLINK_LINKSTATE_ALI:
        case NVLINK_LINKSTATE_SLEEP:
        case NVLINK_LINKSTATE_ACTIVE_PENDING:
        default:
            break;
        }
    }

    link->link_handlers->get_dl_link_mode(link, &currentLinkState);

    while (currentLinkState != linkState)
    {
        nvlink_sleep(1);

        timeout--;

        if (!timeout)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Timeout occured while polling on link.\n",
                __FUNCTION__));

            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Link info: device: %s link: %s link state "
                "expected: 0x%08llx actual: 0x%08llx.\n",
                __FUNCTION__, link->dev->deviceName, link->linkName,
                linkState, currentLinkState));

            return NVL_ERR_INVALID_STATE;
        }

        link->link_handlers->get_dl_link_mode(link, &currentLinkState);
    }

    return NVL_SUCCESS;
}

/**
 * Poll for a given timeout period for the sublink to reach a given
 * state. Sublink state transition is considered as failed once the
 * timeout occurs
 *
 * @param[in]  localTxSubLink           Local  NVLink pointer
 * @param[in]  localTxSubLinkState      Local  Tx Sublink State
 * @param[in]  localTxSubLinkSubState   Local  Tx Sublink Substate
 * @param[in]  remoteRxSubLink          Remote NVLink pointer
 * @param[in]  remoteRxSubLinkState     Remote Rx Sublink State
 * @param[in]  remoteRxSubLinkSubState  Remote Rx Sublink Substate
 * @param[in]  timeout                  Timeout
 *
 * return NVL_SUCCESS is the sublink transitioned to the given state
 */
NvlStatus
nvlink_core_poll_sublink_state
(
    nvlink_link *localTxSubLink,
    NvU64        localTxSubLinkState,
    NvU32        localTxSubLinkSubState,
    nvlink_link *remoteRxSubLink,
    NvU64        remoteRxSubLinkState,
    NvU32        remoteRxSubLinkSubState,
    NvU32        timeout
)
{
    NvlStatus status = NVL_SUCCESS;

    if ((localTxSubLink == NULL) || (remoteRxSubLink == NULL))
    {
        return NVL_BAD_ARGS;
    }

    if (localTxSubLink->version >= NVLINK_DEVICE_VERSION_50)
        return NVL_SUCCESS;

    // check for tx sublink if a valid link is specified
    if (localTxSubLink)
    {
        status = nvlink_core_poll_tx_sublink_state(localTxSubLink,
                                                   localTxSubLinkState,
                                                   localTxSubLinkSubState,
                                                   timeout);
        if (status != NVL_SUCCESS)
        {
            // polling on tx sublink failed. skip any rx polling
            return status;
        }
    }

    //
    // check for rx sublink if a valid link is specified and no
    // timeout on tx sublink (if it was specified)
    //
    if (remoteRxSubLink)
    {
        status = nvlink_core_poll_rx_sublink_state(remoteRxSubLink,
                                                   remoteRxSubLinkState,
                                                   remoteRxSubLinkSubState,
                                                   timeout);
    }

    return status;
}

/**
 * Poll for the tx sublink to reach the specified state upto the given
 * timeout. Sublink state transition is considered failed once timeout
 * occurs.
 *
 * @param[in]  link               NVLink pointer
 * @param[in]  txSublinkState     Tx Sublink State
 * @param[in]  txSublinkSubState  Tx Sublink Substate
 * @param[in]  timeout            Timeout
 *
 * return NVL_SUCCESS if the tx sublink transitioned to the target state
 */
NvlStatus
nvlink_core_poll_tx_sublink_state
(
    nvlink_link *link,
    NvU64        txSublinkState,
    NvU32        txSublinkSubState,
    NvU32        timeout
)
{
    NvU64 currentTxSublinkState    = ~0;
    NvU32 currentTxSublinkSubState = ~0;

    if (link == NULL)
    {
        return NVL_BAD_ARGS;
    }

    if (link->version >= NVLINK_DEVICE_VERSION_50)
        return NVL_SUCCESS;

    link->link_handlers->get_tx_mode(link,
                                     &currentTxSublinkState,
                                     &currentTxSublinkSubState);

    while (!((currentTxSublinkState == txSublinkState) &&
             (currentTxSublinkSubState == txSublinkSubState)))
    {
        nvlink_sleep(1);

        timeout--;

        if (!timeout)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Timeout occured while polling on link.\n",
                __FUNCTION__));

            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Link info: device: %s link: %s txsublink state"
                " expected: 0x%08llx actual: 0x%08llx.\n",
                __FUNCTION__, link->dev->deviceName, link->linkName,
                txSublinkState, currentTxSublinkState));

            return NVL_ERR_INVALID_STATE;
        }

        link->link_handlers->get_tx_mode(link,
                                         &currentTxSublinkState,
                                         &currentTxSublinkSubState);
    }

    return NVL_SUCCESS;
}

/**
 * Poll for the rx sublink to reach the specified state upto the given
 * timeout. Sublink state transition is considered failed once timeout
 * occurs.
 *
 * @param[in]  link               NVLink pointer
 * @param[in]  rxSublinkState     Rx Sublink State
 * @param[in]  rxSublinkSubState  Rx Sublink Substate
 * @param[in]  timeout            Timeout
 *
 * return NVL_SUCCESS if the rx sublink transitioned to the target state
 */
NvlStatus
nvlink_core_poll_rx_sublink_state
(
    nvlink_link *link,
    NvU64        rxSublinkState,
    NvU32        rxSublinkSubState,
    NvU32        timeout
)
{
    NvU64 currentRxSublinkState    = ~0;
    NvU32 currentRxSublinkSubState = ~0;

    if (link == NULL)
    {
        return NVL_BAD_ARGS;
    }

    if (link->version >= NVLINK_DEVICE_VERSION_50)
        return NVL_SUCCESS;

    link->link_handlers->get_rx_mode(link,
                                     &currentRxSublinkState,
                                     &currentRxSublinkSubState);

    while (!((currentRxSublinkState == rxSublinkState) &&
             (currentRxSublinkSubState == rxSublinkSubState)))
    {
        nvlink_sleep(1);

        timeout--;

        if (!timeout)
        {
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Timeout occured while polling on link.\n",
                __FUNCTION__));

            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_ERRORS,
                "%s: Link info: device: %s link: %s rxsublink state "
                "expected: 0x%08llx actual: 0x%08llx.\n",
                __FUNCTION__, link->dev->deviceName, link->linkName,
                rxSublinkState, currentRxSublinkState));

            return NVL_ERR_INVALID_STATE;
        }

        link->link_handlers->get_rx_mode(link,
                                         &currentRxSublinkState,
                                         &currentRxSublinkSubState);
    }

    return NVL_SUCCESS;
}
