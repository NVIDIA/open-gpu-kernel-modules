/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************************************************************\
 *                                                                          *
 *      Confidential Compute HALs                                           *
 *                                                                          *
 \**************************************************************************/

#define NVOC_CONF_COMPUTE_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/conf_compute/conf_compute.h"
#include "published/hopper/gh100/dev_fuse.h"

/*!
 * check if debug mode is enabled.
 *
 * @param[in]  pGpu              GPU object pointer
 * @param[in]  pConfCompute      ConfidentialCompute pointer
 */
NvBool
confComputeIsDebugModeEnabled_GH100
(
    OBJGPU              *pGpu,
    ConfidentialCompute *pConfCompute
)
{
   NvU32 fuseStat = GPU_REG_RD32(pGpu, NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS);

   return !FLD_TEST_DRF(_FUSE_OPT, _SECURE_GSP_DEBUG_DIS, _DATA, _YES, fuseStat);
}

/*!
 * @brief confComputeIsGpuCcCapable
 *        Checks if Gpu can handle ConfCompute workloads.
 *
 * @param[in]     pGpu                     : OBJGPU Pointer
 * @param[in]     pConfCompute             : ConfidentialCompute pointer
 */
NvBool
confComputeIsGpuCcCapable_GH100
(
    OBJGPU               *pGpu,
    ConfidentialCompute  *pConfCompute
)
{
    NvU32 reg;

    if (confComputeIsDebugModeEnabled_HAL(pGpu, pConfCompute))
    {
        NV_PRINTF(LEVEL_ERROR, "Not checking if GPU is capable of accepting conf compute workloads\n");
        return NV_TRUE;
    }

    reg = GPU_REG_RD32(pGpu, NV_FUSE_SPARE_BIT_0);
    if (FLD_TEST_DRF(_FUSE, _SPARE_BIT_0, _DATA, _ENABLE, reg))
    {
        if (FLD_TEST_DRF(_FUSE, _SPARE_BIT_1, _DATA, _ENABLE, GPU_REG_RD32(pGpu, NV_FUSE_SPARE_BIT_1))
            && FLD_TEST_DRF(_FUSE, _SPARE_BIT_2, _DATA, _DISABLE, GPU_REG_RD32(pGpu, NV_FUSE_SPARE_BIT_2)))
        {
            return NV_TRUE;
        }
    }
    return NV_FALSE;
}

NV_STATUS
confComputeDeriveSecrets_GH100(ConfidentialCompute *pConfCompute,
                               NvU32                engine)
{
    switch (engine)
    {
        case MC_ENGINE_IDX_GSP:
            // GSP automatically derives these keys so no need for CPU-RM to trigger it.
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_LOCKED_RPC));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_CPU_GSP_LOCKED_RPC));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_DMA));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_CPU_GSP_DMA));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_REPLAYABLE_FAULT));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_NON_REPLAYABLE_FAULT));
            break;
        case MC_ENGINE_IDX_SEC2:
            // TODO (CONFCOMP-1577)
            // Add calls to GSP to derive keys and retrieve IV masks.
            // IV masks are then placed into the keystore via confComputeKeyStoreDepositIvMask.

            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_KERN));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_KERN));
            break;
        case MC_ENGINE_IDX_CE2:
            // TODO (CONFCOMP-1577)
            // Add calls to GSP to derive keys and retrieve IV masks.
            // IV masks are then placed into the keystore via confComputeKeyStoreDepositIvMask.

            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_H2D_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_D2H_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_H2D_KERN));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_D2H_KERN));
            break;
        case MC_ENGINE_IDX_CE3:
            // TODO (CONFCOMP-1577)
            // Add calls to GSP to derive keys and retrieve IV masks.
            // IV masks are then placed into the keystore via confComputeKeyStoreDepositIvMask.

            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_H2D_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_D2H_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_H2D_KERN));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_D2H_KERN));
            break;
        case MC_ENGINE_IDX_CE4:
            // TODO (CONFCOMP-1577)
            // Add calls to GSP to derive keys and retrieve IV masks.
            // IV masks are then placed into the keystore via confComputeKeyStoreDepositIvMask.

            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_H2D_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_D2H_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_H2D_KERN));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_D2H_KERN));
            break;
        case MC_ENGINE_IDX_CE5:
            // TODO (CONFCOMP-1577)
            // Add calls to GSP to derive keys and retrieve IV masks.
            // IV masks are then placed into the keystore via confComputeKeyStoreDepositIvMask.

            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_H2D_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_D2H_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_H2D_KERN));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_D2H_KERN));
            break;
        case MC_ENGINE_IDX_CE6:
            // TODO (CONFCOMP-1577)
            // Add calls to GSP to derive keys and retrieve IV masks.
            // IV masks are then placed into the keystore via confComputeKeyStoreDepositIvMask.

            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_H2D_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_D2H_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_H2D_KERN));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_D2H_KERN));
            break;
        case MC_ENGINE_IDX_CE7:
            // TODO (CONFCOMP-1577)
            // Add calls to GSP to derive keys and retrieve IV masks.
            // IV masks are then placed into the keystore via confComputeKeyStoreDepositIvMask.

            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_H2D_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_D2H_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_H2D_KERN));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_D2H_KERN));
            break;
        case MC_ENGINE_IDX_CE8:
            // TODO (CONFCOMP-1577)
            // Add calls to GSP to derive keys and retrieve IV masks.
            // IV masks are then placed into the keystore via confComputeKeyStoreDepositIvMask.

            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_H2D_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_D2H_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_H2D_KERN));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_D2H_KERN));
            break;
        case MC_ENGINE_IDX_CE9:
            // TODO (CONFCOMP-1577)
            // Add calls to GSP to derive keys and retrieve IV masks.
            // IV masks are then placed into the keystore via confComputeKeyStoreDepositIvMask.

            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_H2D_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_D2H_USER));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_H2D_KERN));
            confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                                             CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_D2H_KERN));
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}
