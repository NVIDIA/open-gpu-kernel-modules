/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

static void _nvlink_core_print_link(nvlink_link *);

/**
 * Print link state of a single nvlink_link
 *
 * @param[in]  link  NVLink Link pointer
 */
void
nvlink_core_print_link_state
(
    nvlink_link *link
)
{
    NvU64 linkMode         = 0;
    NvU64 txSublinkMode    = 0;
    NvU64 rxSublinkMode    = 0;
    NvU32 txSublinkSubMode = 0;
    NvU32 rxSublinkSubMode = 0;

    if (link == NULL)
        return;

    link->link_handlers->get_dl_link_mode(link, &linkMode);
    link->link_handlers->get_tx_mode(link, &txSublinkMode, &txSublinkSubMode);
    link->link_handlers->get_rx_mode(link, &rxSublinkMode, &rxSublinkSubMode);

    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
        "%s: %s:%s linkMode: %lld txSublinkMode: %lld rxSublinkMode: %lld\n",
        __FUNCTION__,
        link->dev->deviceName, link->linkName,
        linkMode, txSublinkMode, rxSublinkMode));
}

/**
 * Print information of a single nvlink intranode connection
 *
 * @param[in]  conn  NVLink connection pointer
 */
void
nvlink_core_print_intranode_conn
(
    nvlink_intranode_conn *conn
)
{
    if (NULL == conn) return;

    _nvlink_core_print_link(conn->end0);
    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO, " <---> "));
    _nvlink_core_print_link(conn->end1);
    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO, "\n"));
}

/**
 * Print link type and device information of a single nvlink_link
 *
 * @param[in]  link  NVLink Link pointer
 */
static void
_nvlink_core_print_link
(
    nvlink_link *link
)
{
    if (link == NULL)
        return;

    switch (link->dev->type)
    {
        case NVLINK_DEVICE_TYPE_GPU:
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO, "NVGPU"));
            break;
        case NVLINK_DEVICE_TYPE_IBMNPU:
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO, "IBMNPU"));
            break;
        case NVLINK_DEVICE_TYPE_NVSWITCH:
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO, "NVSWITCH"));
            break;
        case NVLINK_DEVICE_TYPE_EBRIDGE:
            NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO, "EBRIDGE"));
            break;
    }

    NVLINK_PRINT((DBG_MODULE_NVLINK_CORE, NVLINK_DBG_LEVEL_INFO,
        "(%x): %04x:%02x:%02x.%x %s",
        link->dev->pciInfo.device,
        link->dev->pciInfo.domain,
        link->dev->pciInfo.bus,
        link->dev->pciInfo.device,
        link->dev->pciInfo.function,
        link->linkName));
}
