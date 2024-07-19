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

#define NVOC_KERNEL_IOCTRL_H_PRIVATE_ACCESS_ALLOWED

#include "os/os.h"
#include "nvrm_registry.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "kernel/gpu/nvlink/kernel_ioctrl.h"

/*!
 * @brief MINION construct
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelIoctrl  IOCTRL object pointer
 *
 * @return NV_OK if successful
 */
NV_STATUS
kioctrlMinionConstruct_GV100
(
    OBJGPU       *pGpu,
    KernelIoctrl *pKernelIoctrl
)
{
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);

    // By default, MINION is available based on chip type. Condition further...
    if (pKernelIoctrl->getProperty(pKernelIoctrl, PDB_PROP_KIOCTRL_MINION_AVAILABLE))
    {
        // Trim based on model/platform/testing/schedule
        NvBool bEnableMinion = kioctrlGetMinionEnableDefault_HAL(pGpu, pKernelIoctrl);

        if (!bEnableMinion)
        {
            NV_PRINTF(LEVEL_INFO,
                      "NVLink MINION is not supported on this platform, disabling.\n");
        }

        NvU32 minionControl = knvlinkGetMinionControl(pGpu, pKernelNvlink);

        // Read in any MINION registry overrides.
        if (NV_OK == osReadRegistryDword(pGpu,
                    NV_REG_STR_RM_NVLINK_MINION_CONTROL, &minionControl))
        {
            NV_PRINTF(LEVEL_INFO, "%s: 0x%x\n",
                      NV_REG_STR_RM_NVLINK_MINION_CONTROL, minionControl);

            // Select requested enable state
            switch (DRF_VAL(_REG_STR_RM, _NVLINK_MINION_CONTROL, _ENABLE, minionControl))
            {
                case NV_REG_STR_RM_NVLINK_MINION_CONTROL_ENABLE_FORCE_ON:
                    NV_PRINTF(LEVEL_INFO,
                              "NVLink MINION force enable requested by command line override.\n");
                    pKernelIoctrl->setProperty(pKernelIoctrl, PDB_PROP_KIOCTRL_MINION_FORCE_BOOT, NV_TRUE);
                    bEnableMinion = NV_TRUE;
                    break;
                case NV_REG_STR_RM_NVLINK_MINION_CONTROL_ENABLE_FORCE_OFF:
                    NV_PRINTF(LEVEL_INFO,
                              "NVLink MINION force disable requested by command line override.\n");
                    bEnableMinion = NV_FALSE;
                    break;
                default:
                    break;
            }

            switch (DRF_VAL(_REG_STR_RM, _NVLINK_MINION_CONTROL, _CACHE_SEEDS, minionControl))
            {
                case NV_REG_STR_RM_NVLINK_MINION_CONTROL_CACHE_SEEDS_ENABLE:
                {
                    NV_PRINTF(LEVEL_INFO,
                              "Regkey: Minion seed caching is force enabled\n");
                    pKernelIoctrl->setProperty(pKernelIoctrl, PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS, NV_TRUE);
                    break;
                }
                case NV_REG_STR_RM_NVLINK_MINION_CONTROL_CACHE_SEEDS_DISABLE:
                {
                    NV_PRINTF(LEVEL_INFO,
                              "Regkey: Minion seed caching is force disabled\n");
                    pKernelIoctrl->setProperty(pKernelIoctrl, PDB_PROP_KIOCTRL_MINION_CACHE_SEEDS, NV_FALSE);
                    break;
                }
            }

            switch (DRF_VAL(_REG_STR_RM, _NVLINK_MINION_CONTROL, _ALI_TRAINING, minionControl))
            {
                case NV_REG_STR_RM_NVLINK_MINION_CONTROL_ALI_TRAINING_ENABLE:
                {
                    NV_PRINTF(LEVEL_INFO,
                              "Regkey: ALI training is force enabled\n");
                    pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_MINION_FORCE_ALI_TRAINING, NV_TRUE);
                    pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_MINION_FORCE_NON_ALI_TRAINING, NV_FALSE);
                    break;
                }
                case NV_REG_STR_RM_NVLINK_MINION_CONTROL_ALI_TRAINING_DISABLE:
                {
                    NV_PRINTF(LEVEL_INFO,
                              "Regkey: non-ALI training is force enabled\n");
                    pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_MINION_FORCE_NON_ALI_TRAINING, NV_TRUE);
                    pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_MINION_FORCE_ALI_TRAINING, NV_FALSE);
                    break;
                }
                default:
                {
                    pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_MINION_FORCE_NON_ALI_TRAINING, NV_FALSE);
                    pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_MINION_FORCE_ALI_TRAINING, NV_FALSE);
                    break;
                }
            }

            switch (DRF_VAL(_REG_STR_RM, _NVLINK_MINION_CONTROL, _GFW_BOOT_DISABLE, minionControl))
            {
                case NV_REG_STR_RM_NVLINK_MINION_CONTROL_GFW_BOOT_DISABLE_DISABLE:
                {
                    NV_PRINTF(LEVEL_INFO,
                              "Regkey: Minion boot from GFW disabled\n");
                    pKernelNvlink->setProperty(pKernelNvlink, PDB_PROP_KNVLINK_MINION_GFW_BOOT, NV_FALSE);
                    break;
                }
                default:
                {
                    if (pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_MINION_GFW_BOOT))
                    {
                       NV_PRINTF(LEVEL_INFO,
                             "Regkey: Minion boot from GFW enabled by default\n");
                    }
                    else
                    {
                        NV_PRINTF(LEVEL_INFO,
                          "Regkey: Minion boot from GFW disabled by default\n");
                    }
                    break;
                }
            }
        }

        // Flush the final minion enable setting
        pKernelIoctrl->setProperty(pKernelIoctrl, PDB_PROP_KIOCTRL_MINION_AVAILABLE, bEnableMinion);
    }

    return NV_OK;
}

/*!
 * @brief Determine the default MINION enable state.
 *
 * @param[in]  pGpu           OBJGPU pointer
 * @param[in]  pKernelIoctrl  IOCTRL object pointer
 *
 * @return NV_TRUE is enable by default
 */
NvBool
kioctrlGetMinionEnableDefault_GV100
(
    OBJGPU       *pGpu,
    KernelIoctrl *pKernelIoctrl
)
{
    //
    // Arch requests that NVLink MINION always be DISABLED by default on RTL
    // for performance reasons.  They will force enable when needed.
    //
    if (IS_RTLSIM(pGpu))
    {
        return NV_FALSE;
    }

    // MINION support is fully enabled by default on everything else.
    return NV_TRUE;
}
