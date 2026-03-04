/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if IS_ENABLED(CONFIG_TEGRA_BPMP)
#include <soc/tegra/bpmp-abi.h>
#include <soc/tegra/bpmp.h>
#endif  // IS_ENABLED(CONFIG_TEGRA_BPMP)

/*!
 * @brief Sends an MRQ (message-request) to BPMP
 *
 * The request, response, and ret parameters of this function correspond to the
 * components of the tegra_bpmp_message struct, which BPMP uses to receive
 * MRQs.
 *
 * @param[in]  nv                 Per GPU Linux state
 * @param[in]  mrq                MRQ_xxx ID specifying what is requested
 * @param[in]  request_data       Pointer to request input data
 * @param[in]  request_data_size  Size of structure pointed to by pRequestData
 * @param[out] response_data      Pointer to response output data
 * @param[in]  response_data_size Size of structure pointed to by pResponseData
 * @param[out] ret                MRQ return code (from "ret" element of
 *                                  tegra_bpmp_message struct)
 * @param[out] api_ret            Return code from tegra_bpmp_transfer call
 *
 * @returns NV_OK if successful,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available,
 *          NV_ERR_INVALID_POINTER if the tegra_bpmp struct pointer could not
 *            be obtained from nv, or
 *          NV_ERR_GENERIC if the tegra_bpmp_transfer call failed (see apiRet
 *            for Linux error code).
 */
NV_STATUS NV_API_CALL
nv_bpmp_send_mrq
(
    nv_state_t *nv,
    NvU32       mrq,
    const void *request_data,
    NvU32       request_data_size,
    void       *response_data,
    NvU32       response_data_size,
    NvS32      *ret,
    NvS32      *api_ret
)
{
#if IS_ENABLED(CONFIG_TEGRA_BPMP) && NV_SUPPORTS_PLATFORM_DEVICE
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct tegra_bpmp *bpmp;
    struct tegra_bpmp_message msg;

    bpmp = tegra_bpmp_get(nvl->dev);
    if (IS_ERR(bpmp))
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM:  Error getting bpmp struct: %s\n",
                  PTR_ERR(bpmp));
        return NV_ERR_INVALID_POINTER;
    }

    // Send the MRQ request to BPMP.
    memset(&msg, 0, sizeof(msg));
    msg.mrq = mrq;
    msg.tx.data = request_data;
    msg.tx.size = (size_t) request_data_size;
    msg.rx.data = response_data;
    msg.rx.size = (size_t) response_data_size;

    *api_ret = (NvS32) tegra_bpmp_transfer(bpmp, &msg);

    if (*api_ret == 0)
    {
        *ret = (NvS32) msg.rx.ret;
        return NV_OK;
    }
    else
    {
        return NV_ERR_GENERIC;
    }
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

