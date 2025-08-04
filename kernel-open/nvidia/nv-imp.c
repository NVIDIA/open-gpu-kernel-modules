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

#if defined(NV_SOC_TEGRA_TEGRA_BPMP_H_PRESENT) || IS_ENABLED(CONFIG_TEGRA_BPMP)
#include <soc/tegra/bpmp-abi.h>
#endif

#if IS_ENABLED(CONFIG_TEGRA_BPMP)
#include <soc/tegra/bpmp.h>
#elif defined(NV_SOC_TEGRA_TEGRA_BPMP_H_PRESENT)
#include <soc/tegra/tegra_bpmp.h>
#endif  // IS_ENABLED(CONFIG_TEGRA_BPMP)

#if defined NV_DT_BINDINGS_INTERCONNECT_TEGRA_ICC_ID_H_PRESENT
#include <dt-bindings/interconnect/tegra_icc_id.h>
#endif

#ifdef NV_LINUX_PLATFORM_TEGRA_MC_UTILS_H_PRESENT
#include <linux/platform/tegra/mc_utils.h>
#endif

//
// IMP requires information from various BPMP and MC driver functions.  The
// macro below checks that all of the required functions are present.
//
#define IMP_SUPPORT_FUNCTIONS_PRESENT \
    (defined(NV_SOC_TEGRA_TEGRA_BPMP_H_PRESENT) || \
     IS_ENABLED(CONFIG_TEGRA_BPMP)) && \
    defined(NV_LINUX_PLATFORM_TEGRA_MC_UTILS_H_PRESENT)

//
// Also create a macro to check if all the required ICC symbols are present.
// DT endpoints are defined in dt-bindings/interconnect/tegra_icc_id.h.
//
#define ICC_SUPPORT_FUNCTIONS_PRESENT \
    defined(NV_DT_BINDINGS_INTERCONNECT_TEGRA_ICC_ID_H_PRESENT)

/*!
 * @brief Returns IMP-relevant data collected from other modules
 *
 * @param[out]  tegra_imp_import_data   Structure to receive the data
 *
 * @returns NV_OK if successful,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available.
 */
NV_STATUS NV_API_CALL
nv_imp_get_import_data
(
    TEGRA_IMP_IMPORT_DATA *tegra_imp_import_data
)
{
#if IMP_SUPPORT_FUNCTIONS_PRESENT
    tegra_imp_import_data->num_dram_channels = get_dram_num_channels();
    nv_printf(NV_DBG_INFO, "NVRM: num_dram_channels = %u\n",
              tegra_imp_import_data->num_dram_channels);

    return NV_OK;
#else   // IMP_SUPPORT_FUNCTIONS_PRESENT
    return NV_ERR_NOT_SUPPORTED;
#endif
}

/*!
 * @brief Tells BPMP whether or not RFL is valid
 * 
 * Display HW generates an ok_to_switch signal which asserts when mempool
 * occupancy is high enough to be able to turn off memory long enough to
 * execute a dramclk frequency switch without underflowing display output.
 * ok_to_switch drives the RFL ("request for latency") signal in the memory
 * unit, and the switch sequencer waits for this signal to go active before
 * starting a dramclk switch.  However, if the signal is not valid (e.g., if
 * display HW or SW has not been initialized yet), the switch sequencer ignores
 * the signal.  This API tells BPMP whether or not the signal is valid.
 *
 * @param[in] nv        Per GPU Linux state
 * @param[in] bEnable   True if RFL will be valid; false if invalid
 *
 * @returns NV_OK if successful,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available, or
 *          NV_ERR_GENERIC if some other kind of error occurred.
 */
NV_STATUS NV_API_CALL
nv_imp_enable_disable_rfl
(
    nv_state_t *nv,
    NvBool bEnable
)
{
    NV_STATUS status = NV_ERR_NOT_SUPPORTED;
#if IMP_SUPPORT_FUNCTIONS_PRESENT
#if IS_ENABLED(CONFIG_TEGRA_BPMP) && NV_SUPPORTS_PLATFORM_DEVICE
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    struct tegra_bpmp *bpmp = tegra_bpmp_get(nvl->dev);
    struct tegra_bpmp_message msg;
    struct mrq_emc_disp_rfl_request emc_disp_rfl_request;
    int rc;

    memset(&emc_disp_rfl_request, 0, sizeof(emc_disp_rfl_request));
    emc_disp_rfl_request.mode = bEnable ? EMC_DISP_RFL_MODE_ENABLED :
                                          EMC_DISP_RFL_MODE_DISABLED;
    msg.mrq = MRQ_EMC_DISP_RFL;
    msg.tx.data = &emc_disp_rfl_request;
    msg.tx.size = sizeof(emc_disp_rfl_request);
    msg.rx.data = NULL;
    msg.rx.size = 0;

    rc = tegra_bpmp_transfer(bpmp, &msg);
    if (rc == 0)
    {
        nv_printf(NV_DBG_INFO,
                  "\"Wait for RFL\" is %s via MRQ_EMC_DISP_RFL\n",
                  bEnable ? "enabled" : "disabled");
        status = NV_OK;
    }
    else
    {
        nv_printf(NV_DBG_ERRORS,
                  "MRQ_EMC_DISP_RFL failed to %s \"Wait for RFL\" (error code = %d)\n",
                  bEnable ? "enable" : "disable",
                  rc);
        status = NV_ERR_GENERIC;
    }
#else
    nv_printf(NV_DBG_ERRORS, "nv_imp_enable_disable_rfl stub called!\n");
#endif
#endif
    return status;
}

/*! 
 * @brief Obtains a handle for the display data path
 * 
 * If a handle is obtained successfully, it is not returned to the caller; it
 * is saved for later use by subsequent nv_imp_icc_set_bw calls.
 * nv_imp_icc_get must be called prior to calling nv_imp_icc_set_bw.
 *
 * @param[out] nv   Per GPU Linux state
 *
 * @returns NV_OK if successful,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available, or
 *          NV_ERR_GENERIC if some other error occurred.
 */
NV_STATUS NV_API_CALL
nv_imp_icc_get
(
    nv_state_t *nv
)
{
#if ICC_SUPPORT_FUNCTIONS_PRESENT && NV_SUPPORTS_PLATFORM_DEVICE
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    NV_STATUS status = NV_OK;

#if defined(NV_DEVM_ICC_GET_PRESENT)
    // Needs to use devm_of_icc_get function as per the latest ICC driver
    nvl->nv_imp_icc_path =
        devm_of_icc_get(nvl->dev, "read-1");

    if (nvl->nv_imp_icc_path == NULL)
    {
        nv_printf(NV_DBG_INFO, "NVRM: devm_of_icc_get failed\n");
        return NV_ERR_NOT_SUPPORTED;
    }
    else if (!IS_ERR(nvl->nv_imp_icc_path))
    {
        nvl->is_upstream_icc_path = NV_TRUE;
        return NV_OK;
    }
    //
    // Till we modify all DTs to have interconnect node specified as per
    // the latest ICC driver, fallback to older ICC mechanism.
    //
#endif

    nvl->nv_imp_icc_path = NULL;

#if defined(NV_ICC_GET_PRESENT)
    struct device_node *np;
    // Check if ICC is present in the device tree, and enabled.
    np = of_find_node_by_path("/icc");
    if (np != NULL)
    {
        if (of_device_is_available(np))
        {
            // Get the ICC data path.
            nvl->nv_imp_icc_path =
                icc_get(nvl->dev, TEGRA_ICC_DISPLAY, TEGRA_ICC_PRIMARY);
        }
        of_node_put(np);
    }
#else
    nv_printf(NV_DBG_ERRORS, "NVRM: icc_get() not present\n");
    return NV_ERR_NOT_SUPPORTED;
#endif

    if (nvl->nv_imp_icc_path == NULL)
    {
        nv_printf(NV_DBG_INFO, "NVRM: icc_get disabled\n");
        status = NV_ERR_NOT_SUPPORTED;
    }
    else if (IS_ERR(nvl->nv_imp_icc_path))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: invalid path = %ld\n",
                  PTR_ERR(nvl->nv_imp_icc_path));
        nvl->nv_imp_icc_path = NULL;
        status = NV_ERR_GENERIC;
    }
    return status;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

/*!
 * @brief Releases the handle obtained by nv_imp_icc_get
 * 
 * @param[in] nv    Per GPU Linux state
 */
void
nv_imp_icc_put
(
    nv_state_t *nv
)
{
#if ICC_SUPPORT_FUNCTIONS_PRESENT
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);

#if defined(NV_DEVM_ICC_GET_PRESENT)
    //
    // If devm_of_icc_get API is used for requesting the bandwidth,
    // it does not require to call put explicitly.
    //
    if (nvl->is_upstream_icc_path)
    {
        goto done;
    }
#endif

#if defined(NV_ICC_PUT_PRESENT) && NV_SUPPORTS_PLATFORM_DISPLAY_DEVICE
    if (nvl->nv_imp_icc_path != NULL)
    {
        icc_put(nvl->nv_imp_icc_path);
    }
#else
    nv_printf(NV_DBG_ERRORS, "icc_put() not present\n");
#endif

done:
    nvl->nv_imp_icc_path = NULL;
#endif
}

/*!
 * @brief Allocates a specified amount of ISO memory bandwidth for display
 * 
 * floor_bw_kbps is the minimum required (i.e., floor) dramclk frequency
 * multiplied by the width of the pipe over which the display data will travel.
 * (It is understood that the bandwidth calculated by multiplying the clock
 * frequency by the pipe width will not be realistically achievable, due to
 * overhead in the memory subsystem.  ICC will not actually use the bandwidth
 * value, except to reverse the calculation to get the required dramclk
 * frequency.)
 *
 * nv_imp_icc_get must be called prior to calling this function.
 *
 * @param[in]   nv              Per GPU Linux state
 * @param[in]   avg_bw_kbps     Amount of ISO memory bandwidth requested
 * @param[in]   floor_bw_kbps   Min required dramclk freq * pipe width
 *
 * @returns NV_OK if successful,
 *          NV_ERR_INSUFFICIENT_RESOURCES if one of the bandwidth values is too
 *            high, and bandwidth cannot be allocated,
 *          NV_ERR_NOT_SUPPORTED if the functionality is not available, or
 *          NV_ERR_GENERIC if some other kind of error occurred.
 */
NV_STATUS NV_API_CALL
nv_imp_icc_set_bw
(
    nv_state_t *nv,
    NvU32       avg_bw_kbps,
    NvU32       floor_bw_kbps
)
{
#if ICC_SUPPORT_FUNCTIONS_PRESENT && NV_SUPPORTS_PLATFORM_DEVICE
    nv_linux_state_t *nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    int rc;
    NV_STATUS status = NV_OK;
 
    //
    // avg_bw_kbps can be either ISO bw request or NISO bw request.
    // Use floor_bw_kbps to make floor requests.
    //
#if defined(NV_ICC_SET_BW_PRESENT)
    //
    // nv_imp_icc_path will be NULL on AV + L systems because ICC is disabled.
    // In this case, skip the allocation call, and just return a success
    // status.
    //
    if (nvl->nv_imp_icc_path == NULL)
    {
        return NV_OK;
    }
    rc = icc_set_bw(nvl->nv_imp_icc_path, avg_bw_kbps, floor_bw_kbps);
#else
    nv_printf(NV_DBG_ERRORS, "icc_set_bw() not present\n");
    return NV_ERR_NOT_SUPPORTED;
#endif

    if (rc < 0)
    {
        // A negative return value indicates an error.
        if (rc == -ENOMEM)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
        }
        else
        {
            status = NV_ERR_GENERIC;
        }
    }
    return status;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

