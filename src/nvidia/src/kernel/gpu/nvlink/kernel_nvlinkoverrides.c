/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERNEL_NVLINK_H_PRIVATE_ACCESS_ALLOWED

#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"
#include "nvrm_registry.h"
#include "os/os.h"

/*!
 * @brief Apply NVLink overrides from Registry
 *
 * @param[in] pGpu           OBJGPU pointer
 * @param[in] pKernelNvlink  KernelNvlink pointer
 *
 * @return  NV_OK on success
 */
NV_STATUS
knvlinkApplyRegkeyOverrides_IMPL
(
	OBJGPU       *pGpu,
	KernelNvlink *pKernelNvlink
)
{
    NvU32 regdata;

    // Initialize the settings

    // Link training settings
    pKernelNvlink->bEnableTrainingAtLoad = NV_FALSE;
    pKernelNvlink->bSkipLinkTraining     = NV_FALSE;

    // Link enable/disable filtering
    pKernelNvlink->bRegistryLinkOverride   = NV_FALSE;
    pKernelNvlink->registryLinkMask        = 0;
    pKernelNvlink->vbiosDisabledLinkMask   = 0;
    pKernelNvlink->regkeyDisabledLinksMask = 0;

    // Clock and speed settings
    pKernelNvlink->nvlinkLinkSpeed = NV_REG_STR_RM_NVLINK_SPEED_CONTROL_SPEED_DEFAULT;

    // Power management settings
    pKernelNvlink->bDisableL2Mode         = NV_FALSE;

    // Debug Settings
    pKernelNvlink->bLinkTrainingDebugSpew = NV_FALSE;

    // Registry overrides for forcing NVLINK on/off
    if (NV_OK == osReadRegistryDword(pGpu,
                NV_REG_STR_RM_NVLINK_CONTROL, &pKernelNvlink->registryControl))
    {
        NV_PRINTF(LEVEL_INFO, "registryControl: 0x%x\n",
                  pKernelNvlink->registryControl);

        // If NVLink is force disabled through regkey override
        if (FLD_TEST_DRF(_REG_STR_RM, _NVLINK_CONTROL, _FORCE_DISABLE, _YES,
                         pKernelNvlink->registryControl) &&
            FLD_TEST_DRF(_REG_STR_RM, _NVLINK_CONTROL, _FORCE_ENABLE, _NO,
                         pKernelNvlink->registryControl))
        {
            NV_PRINTF(LEVEL_INFO,
                      "Disabling NVLINK (forced disable via regkey)\n");

            return NV_ERR_NOT_SUPPORTED;
        }
        else if (!knvlinkIsNvlinkDefaultEnabled(pGpu, pKernelNvlink) &&
                 FLD_TEST_DRF(_REG_STR_RM, _NVLINK_CONTROL, _FORCE_ENABLE, _NO,
                              pKernelNvlink->registryControl))
        {
            // NVLink is not default enabled and is not force enabled as well
            NV_PRINTF(LEVEL_INFO,
                      "Disabling NVLINK (disabled by platform default)\n");

            return NV_ERR_NOT_SUPPORTED;
        }
        else if (FLD_TEST_DRF(_REG_STR_RM, _NVLINK_CONTROL, _FORCE_DISABLE, _YES,
                              pKernelNvlink->registryControl) &&
                 FLD_TEST_DRF(_REG_STR_RM, _NVLINK_CONTROL, _FORCE_ENABLE, _YES,
                              pKernelNvlink->registryControl))
        {
            // NVLink is both force enabled and disabled. Fallback to default
            NV_PRINTF(LEVEL_INFO,
                      "Conflict in Nvlink Force Enable/Disable. Reverting to platform default.\n");

            if (!knvlinkIsNvlinkDefaultEnabled(pGpu, pKernelNvlink))
            {
                NV_PRINTF(LEVEL_INFO,
                          "Disabling NVLINK (disabled by platform default)\n");

                return NV_ERR_NOT_SUPPORTED;
            }
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "NVLink is enabled\n");
            pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENABLED, NV_TRUE);
        }

        //
        // Regkey overrides to trigger link init and training during driver load
        //     a. Initialize to swcfg mode during driver load
        //     b. Train to active mode during driver load
        //
        if (FLD_TEST_DRF(_REG_STR_RM, _NVLINK_CONTROL, _TRAIN_AT_LOAD, _YES,
                         pKernelNvlink->registryControl))
        {
            NV_PRINTF(LEVEL_INFO,
                      "Overriding NvLink training during driver load via regkey.\n");
            pKernelNvlink->bEnableTrainingAtLoad = NV_TRUE;
        }
        else
        {
            pKernelNvlink->bEnableTrainingAtLoad = NV_FALSE;
            pKernelNvlink->bEnableSafeModeAtLoad = NV_FALSE;
        }

        // Regkey override to skip link initialization and training
        if (FLD_TEST_DRF(_REG_STR_RM, _NVLINK_CONTROL, _SKIP_TRAIN, _YES,
                         pKernelNvlink->registryControl))
        {
            pKernelNvlink->bSkipLinkTraining = NV_TRUE;
        }
        else
        {
            pKernelNvlink->bSkipLinkTraining = NV_FALSE;
        }

        // Regkey override to skip forced config if enabled
        if (FLD_TEST_DRF(_REG_STR_RM, _NVLINK_CONTROL, _FORCE_AUTOCONFIG, _YES,
                         pKernelNvlink->registryControl))
        {
            pKernelNvlink->bForceAutoconfig = NV_TRUE;
        }
        else
        {
            pKernelNvlink->bForceAutoconfig = NV_FALSE;
        }

        if (FLD_TEST_DRF(_REG_STR_RM, _NVLINK_CONTROL, _LINK_TRAINING_DEBUG_SPEW, _ON,
                         pKernelNvlink->registryControl))
        {
            pKernelNvlink->bLinkTrainingDebugSpew = NV_TRUE;
            NV_PRINTF(LEVEL_INFO,
                "Link training debug spew turned on!\n");
        }

    }
    else if (!knvlinkIsNvlinkDefaultEnabled(pGpu, pKernelNvlink))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Disabling NVLINK (disabled by platform default)\n");

        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // Registry overrides for nvlink register initialization
    //
    // NOTE: Reginit has been deprecated on Ampere and beyond
    //
    if (NV_OK == osReadRegistryDword(pGpu,
        NV_REG_STR_RM_NVLINK_VERBOSE_MASK_CONTROL, &pKernelNvlink->verboseMask))
    {
        if (DRF_VAL(_REG_STR_RM, _NVLINK_VERBOSE_MASK_CONTROL, _REGINIT,
                    pKernelNvlink->verboseMask)
            == NV_REG_STR_RM_NVLINK_VERBOSE_MASK_CONTROL_REGINIT_ON)
        {
            NV_PRINTF(LEVEL_INFO,
                      "Forcing NVLINK Verbose Reg Init Prints enabled via regkey\n");
        }
    }

    // Registry overrides to disable a set of links
    if (NV_OK == osReadRegistryDword(pGpu,
        NV_REG_STR_RM_NVLINK_DISABLE_LINKS, &pKernelNvlink->regkeyDisabledLinksMask))
    {
        NV_PRINTF(LEVEL_INFO, "Disable NvLinks 0x%x via regkey\n",
                  pKernelNvlink->regkeyDisabledLinksMask);
    }

    //
    // Registry overrides to enable NvLinks selectively
    //
    // NOTE: This is used only on Pascal. Volta and beyond, this should not be used
    //
    if (NV_OK == osReadRegistryDword(pGpu,
                NV_REG_STR_RM_NVLINK_ENABLE, &pKernelNvlink->registryLinkMask))
    {
        pKernelNvlink->bRegistryLinkOverride = NV_TRUE;
        NV_PRINTF(LEVEL_INFO, "Enable NvLinks 0x%x via regkey\n",
                  pKernelNvlink->registryLinkMask);
    }

    // Registry overrides for disabling nvlink P2P loopback
    if (NV_OK == osReadRegistryDword(pGpu,
                 NV_REG_STR_RM_NVLINK_DISABLE_P2P_LOOPBACK, &regdata) &&
        regdata == NV_REG_STR_RM_NVLINK_DISABLE_P2P_LOOPBACK_TRUE)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_NVLINK_P2P_LOOPBACK_DISABLED, NV_TRUE);
    }

    // Regkey overrides for NVLink power management controls
    if (NV_OK == osReadRegistryDword(pGpu,
                         NV_REG_STR_RM_NVLINK_LINK_PM_CONTROL, &regdata))
    {
        NV_PRINTF(LEVEL_INFO, "RM NVLink Link PM controlled via regkey\n");

        // Whether L2 power state has been disabled by regkey
        if (FLD_TEST_DRF(_REG_STR_RM, _NVLINK_LINK_PM_CONTROL, _L2_MODE,
                         _DISABLE, regdata))
        {
            NV_PRINTF(LEVEL_INFO,
                      "NVLink L2 power state disabled via regkey\n");
            pKernelNvlink->bDisableL2Mode = NV_TRUE;
        }
    }

    // If lane disable and lane shutdown is force enabled through regkey
    if (NV_OK == osReadRegistryDword(pGpu,
                     NV_REG_STR_RM_NVLINK_FORCE_LANESHUTDOWN, &regdata) &&
        regdata == NV_REG_STR_RM_NVLINK_FORCE_LANESHUTDOWN_TRUE)
    {
        NV_PRINTF(LEVEL_INFO,
                  "NVLink lanedisable and laneshutdown is forced enabled via regkey\n");

        pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_LANE_SHUTDOWN_ON_UNLOAD, NV_TRUE);
    }

    //
    // Registry override to control SYSMEM device type for reginit flow when
    // using force config.
    //
    if (NV_OK == osReadRegistryDword(pGpu,
        NV_REG_STR_RM_NVLINK_SYSMEM_DEVICE_TYPE, &pKernelNvlink->forcedSysmemDeviceType))
    {
        NV_PRINTF(LEVEL_INFO,
                  "Forcing NVLINK SYSMEM device type with 0x%x via regkey\n",
                  pKernelNvlink->forcedSysmemDeviceType);
    }
    else
    {
        pKernelNvlink->forcedSysmemDeviceType = NV2080_CTRL_NVLINK_DEVICE_INFO_DEVICE_TYPE_EBRIDGE;
    }

    if (NV_OK == osReadRegistryDword(pGpu,
                 NV_REG_STR_RM_NVLINK_FORCED_LOOPBACK_ON_SWITCH, &regdata))
    {
        if (FLD_TEST_DRF(_REG_STR_RM, _NVLINK_FORCED_LOOPBACK_ON_SWITCH, _MODE, _ENABLED, regdata))
        {
            pKernelNvlink->setProperty(pGpu, PDB_PROP_KNVLINK_FORCED_LOOPBACK_ON_SWITCH_MODE_ENABLED, NV_TRUE);
            NV_PRINTF(LEVEL_INFO,
                      "Forced Loopback on switch is enabled\n");
        }        
    }

    // Registry override to enable nvlink encryption
    if (NV_OK == osReadRegistryDword(pGpu,
                 NV_REG_STR_RM_NVLINK_ENCRYPTION, &regdata))
    {
        if (FLD_TEST_DRF(_REG_STR_RM, _NVLINK_ENCRYPTION, _MODE, _ENABLE, regdata))
        {
            pKernelNvlink->setProperty(pGpu, PDB_PROP_KNVLINK_ENCRYPTION_ENABLED, NV_TRUE);
            NV_PRINTF(LEVEL_INFO,
                      "Nvlink Encryption is enabled\n");
        }        
    }

    return NV_OK;
}
