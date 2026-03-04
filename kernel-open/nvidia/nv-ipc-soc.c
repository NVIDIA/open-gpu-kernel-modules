/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define  __NO_VERSION__

#include "os-interface.h"
#include "nv-linux.h"

#include "dce_rm_client_ipc.h"

#if defined(NV_LINUX_PLATFORM_TEGRA_DCE_DCE_CLIENT_IPC_H_PRESENT)
#include <linux/platform/tegra/dce/dce-client-ipc.h>

static const NvU32 dceClientRmIpcTypeMap[DCE_CLIENT_RM_IPC_TYPE_MAX] = {
    [DCE_CLIENT_RM_IPC_TYPE_SYNC] = DCE_CLIENT_IPC_TYPE_CPU_RM,
    [DCE_CLIENT_RM_IPC_TYPE_EVENT] = DCE_CLIENT_IPC_TYPE_RM_EVENT,
};

static NV_STATUS validate_dce_client_ipc_interface_type(NvU32 interfaceType)
{
    if (interfaceType >= DCE_CLIENT_RM_IPC_TYPE_MAX)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (dceClientRmIpcTypeMap[interfaceType] >= DCE_CLIENT_IPC_TYPE_MAX)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

NvU32 nv_tegra_get_rm_interface_type(NvU32 clientIpcType)
{
    NvU32 interfaceType = DCE_CLIENT_RM_IPC_TYPE_SYNC;

    for (interfaceType = DCE_CLIENT_RM_IPC_TYPE_SYNC;
         interfaceType < DCE_CLIENT_RM_IPC_TYPE_MAX;
         interfaceType++)
    {
        if (dceClientRmIpcTypeMap[interfaceType] == clientIpcType)
            return interfaceType;
    }

    return NV_ERR_INVALID_DATA; 
}

NV_STATUS nv_tegra_dce_register_ipc_client
(
    NvU32 interfaceType,
    void *usrCtx,
    nvTegraDceClientIpcCallback callbackFn,
    NvU32 *handle
)
{
    NvU32 dceClientInterfaceType = DCE_CLIENT_IPC_TYPE_MAX;

    if (validate_dce_client_ipc_interface_type(interfaceType) != NV_OK)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    dceClientInterfaceType = dceClientRmIpcTypeMap[interfaceType];

    return tegra_dce_register_ipc_client(dceClientInterfaceType, callbackFn, usrCtx, handle);
}

NV_STATUS nv_tegra_dce_client_ipc_send_recv
(
    NvU32 clientId,
    void *msg,
    NvU32 msgLength
)
{
    struct dce_ipc_message dce_ipc_msg;

    memset(&dce_ipc_msg, 0, sizeof(struct dce_ipc_message));
    dce_ipc_msg.tx.data = msg;
    dce_ipc_msg.rx.data = msg;
    dce_ipc_msg.tx.size = msgLength;
    dce_ipc_msg.rx.size = msgLength;

    return tegra_dce_client_ipc_send_recv(clientId, &dce_ipc_msg);
}

NV_STATUS nv_tegra_dce_unregister_ipc_client(NvU32 clientId)
{
    return tegra_dce_unregister_ipc_client(clientId);
}
#else
NvU32 nv_tegra_get_rm_interface_type(NvU32 clientIpcType)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS nv_tegra_dce_register_ipc_client
(
    NvU32 interfaceType,
    void *usrCtx,
    nvTegraDceClientIpcCallback callbackFn,
    NvU32 *handle
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS nv_tegra_dce_client_ipc_send_recv
(
    NvU32 clientId,
    void *msg,
    NvU32 msgLength
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS nv_tegra_dce_unregister_ipc_client(NvU32 clientId)
{
    return NV_ERR_NOT_SUPPORTED;
}
#endif

