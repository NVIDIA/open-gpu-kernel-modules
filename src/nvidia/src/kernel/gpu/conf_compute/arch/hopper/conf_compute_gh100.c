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
#include "rmapi/rmapi.h"
#include "conf_compute/cc_keystore.h"

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
    OBJGPU *pGpu = ENG_GET_GPU(pConfCompute);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    switch (engine)
    {
        case MC_ENGINE_IDX_GSP:
            // GSP automatically derives these keys so no need for CPU-RM to trigger it.
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_LOCKED_RPC)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_CPU_GSP_LOCKED_RPC)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_DMA)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_CPU_GSP_DMA)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_REPLAYABLE_FAULT)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_GSP, CC_LKEYID_GSP_CPU_NON_REPLAYABLE_FAULT)));
            break;
        case MC_ENGINE_IDX_SEC2:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_SEC2;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_SWL_KEYS,
                                                   &params,
                                                   sizeof(params)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_USER),
                                                 (void*)&params.ivMaskSet[NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_USER].ivMask[0]);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_KERN),
                                                 (void*)&params.ivMaskSet[NV2080_CTRL_INTERNAL_CONF_COMPUTE_IVMASK_SWL_KERNEL].ivMask[0]);

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_DATA_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_SEC2, CC_LKEYID_CPU_SEC2_HMAC_KERN)));
        }
        break;
        case MC_ENGINE_IDX_CE2:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_COPY0;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS,
                                                   &params,
                                                   sizeof(params)));

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_H2D_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_D2H_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_H2D_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_D2H_KERN)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_H2D_USER),
                                                 (void*)&params.ivMaskSet[0].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_D2H_USER),
                                                 (void*)&params.ivMaskSet[1].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_H2D_KERN),
                                                 (void*)&params.ivMaskSet[2].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE0, CC_LKEYID_LCE_D2H_KERN),
                                                 (void*)&params.ivMaskSet[3].ivMask);
            break;
        }
        case MC_ENGINE_IDX_CE3:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_COPY1;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS,
                                                   &params,
                                                   sizeof(params)));

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_H2D_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_D2H_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_H2D_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_D2H_KERN)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_H2D_USER),
                                                 (void*)&params.ivMaskSet[0].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_D2H_USER),
                                                 (void*)&params.ivMaskSet[1].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_H2D_KERN),
                                                 (void*)&params.ivMaskSet[2].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE1, CC_LKEYID_LCE_D2H_KERN),
                                                 (void*)&params.ivMaskSet[3].ivMask);
            break;
        }
        case MC_ENGINE_IDX_CE4:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_COPY2;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS,
                                                   &params,
                                                   sizeof(params)));

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_H2D_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_D2H_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_H2D_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_D2H_KERN)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_H2D_USER),
                                                 (void*)&params.ivMaskSet[0].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_D2H_USER),
                                                 (void*)&params.ivMaskSet[1].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_H2D_KERN),
                                                 (void*)&params.ivMaskSet[2].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE2, CC_LKEYID_LCE_D2H_KERN),
                                                 (void*)&params.ivMaskSet[3].ivMask);
            break;
        }
        case MC_ENGINE_IDX_CE5:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_COPY3;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS,
                                                   &params,
                                                   sizeof(params)));

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_H2D_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_D2H_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_H2D_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_D2H_KERN)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_H2D_USER),
                                                 (void*)&params.ivMaskSet[0].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_D2H_USER),
                                                 (void*)&params.ivMaskSet[1].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_H2D_KERN),
                                                 (void*)&params.ivMaskSet[2].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE3, CC_LKEYID_LCE_D2H_KERN),
                                                 (void*)&params.ivMaskSet[3].ivMask);
            break;
        }
        case MC_ENGINE_IDX_CE6:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_COPY4;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS,
                                                   &params,
                                                   sizeof(params)));

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_H2D_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_D2H_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_H2D_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_D2H_KERN)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_H2D_USER),
                                                 (void*)&params.ivMaskSet[0].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_D2H_USER),
                                                 (void*)&params.ivMaskSet[1].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_H2D_KERN),
                                                 (void*)&params.ivMaskSet[2].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE4, CC_LKEYID_LCE_D2H_KERN),
                                                 (void*)&params.ivMaskSet[3].ivMask);
            break;
        }
        case MC_ENGINE_IDX_CE7:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_COPY5;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS,
                                                   &params,
                                                   sizeof(params)));

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_H2D_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_D2H_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_H2D_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_D2H_KERN)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_H2D_USER),
                                                 (void*)&params.ivMaskSet[0].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_D2H_USER),
                                                 (void*)&params.ivMaskSet[1].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_H2D_KERN),
                                                 (void*)&params.ivMaskSet[2].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE5, CC_LKEYID_LCE_D2H_KERN),
                                                 (void*)&params.ivMaskSet[3].ivMask);
            break;
        }
        case MC_ENGINE_IDX_CE8:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_COPY6;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS,
                                                   &params,
                                                   sizeof(params)));

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_H2D_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_D2H_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_H2D_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_D2H_KERN)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_H2D_USER),
                                                 (void*)&params.ivMaskSet[0].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_D2H_USER),
                                                 (void*)&params.ivMaskSet[1].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_H2D_KERN),
                                                 (void*)&params.ivMaskSet[2].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE6, CC_LKEYID_LCE_D2H_KERN),
                                                 (void*)&params.ivMaskSet[3].ivMask);
            break;
        }
        case MC_ENGINE_IDX_CE9:
        {
            NV2080_CTRL_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS_PARAMS params = {0};

            params.engineId = NV2080_ENGINE_TYPE_COPY7;
            NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                                                   pGpu->hInternalClient,
                                                   pGpu->hInternalSubdevice,
                                                   NV2080_CTRL_CMD_INTERNAL_CONF_COMPUTE_DERIVE_LCE_KEYS,
                                                   &params,
                                                   sizeof(params)));

            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_H2D_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_D2H_USER)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_H2D_KERN)));
            NV_ASSERT_OK_OR_RETURN(confComputeKeyStoreDeriveKey_HAL(pConfCompute,
                CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_D2H_KERN)));

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_H2D_USER),
                                                 (void*)&params.ivMaskSet[0].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_D2H_USER),
                                                 (void*)&params.ivMaskSet[1].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_H2D_KERN),
                                                 (void*)&params.ivMaskSet[2].ivMask);

            confComputeKeyStoreDepositIvMask_HAL(pConfCompute,
                                                 CC_GKEYID_GEN(CC_KEYSPACE_LCE7, CC_LKEYID_LCE_D2H_KERN),
                                                 (void*)&params.ivMaskSet[3].ivMask);
            break;
        }
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}
