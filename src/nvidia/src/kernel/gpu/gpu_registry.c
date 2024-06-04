/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "gpu/gpu.h"
#include "os/os.h"
#include "gpu/gpu_timeout.h"
#include "gpu/gpu_access.h"
#include "core/thread_state.h"
#include "nvdevid.h"
#include "nvrm_registry.h"

#include "virtualization/hypervisor/hypervisor.h"

static void _gpuInitGlobalSurfaceOverride(OBJGPU *pGpu);

/*!
 * @brief Read out any overrides and settings from the registry
 */
NV_STATUS
gpuInitRegistryOverrides_KERNEL
(
    OBJGPU *pGpu
)
{
    NvU32 data32 = 0;

    // Override timeout settings
    timeoutRegistryOverride(&pGpu->timeoutData, pGpu);

    // Check the registry for an override of the "broken FB" property.
    if (osReadRegistryDword(pGpu, NV_REG_STR_GPU_BROKEN_FB,
            &data32) != NV_OK)
    {
        // Apply defaults based on the chip and mask.
        data32 = NV_REG_STR_GPU_BROKEN_FB_DEFAULT;
    }

    // for 0FB set as though FB memory is broken to cover all the tests
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
    {
        data32 = FLD_SET_DRF(_REG_STR_GPU, _BROKEN_FB, _MEMORY, _BROKEN, data32);
    }

    // Set the property for the broken memory access.
    if (FLD_TEST_DRF(_REG_STR_GPU, _BROKEN_FB, _MEMORY, _BROKEN, data32))
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_BROKEN_FB, NV_TRUE);
    }

    if (osReadRegistryDword(pGpu,
                        NV_REG_STR_RM_INST_VPR, &pGpu->instVprOverrides) != NV_OK)
    {
        pGpu->instVprOverrides = 0;
    }

    //
    // Persistent compute mode rules (fix for bug 544798): If a
    // client had enabled compute mode earlier, we would have
    // recorded this in the registry.
    //
    pGpu->computeModeRules = NV2080_CTRL_GPU_COMPUTE_MODE_RULES_NONE;
    if (NV_OK == osReadRegistryDword(pGpu,
                NV_REG_STR_RM_COMPUTE_MODE_RULES, &data32))
    {
        pGpu->computeModeRules = data32;
    }

    // Check to see if we have any ThreadState registry overrides
    threadStateInitRegistryOverrides(pGpu);

    // Check to see if we enable surprise removal support
    // Enable SR support by default, disable if the regkey is set to 0
    pGpu->bSurpriseRemovalSupported = NV_TRUE;
    if (osReadRegistryDword(pGpu,
                            NV_REG_STR_RM_GPU_SURPRISE_REMOVAL, &data32) == NV_OK)
    {
        if (data32 == 0)
        {
            pGpu->bSurpriseRemovalSupported = NV_FALSE;
        }
    }

    if (pGpu->bSriovCapable)
    {
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_SET_SRIOV_MODE, &data32) == NV_OK)
        {
            NV_PRINTF(LEVEL_INFO, "Overriding SRIOV Mode to %u\n",
                      (data32 == NV_REG_STR_RM_SET_SRIOV_MODE_ENABLED));

            pGpu->bSriovEnabled = (data32 == NV_REG_STR_RM_SET_SRIOV_MODE_ENABLED);
        }
        else
        {
            if (hypervisorIsVgxHyper() && !RMCFG_FEATURE_PLATFORM_GSP)
            {
                if (!IsTURING(pGpu))
                {
                    pGpu->bSriovEnabled = NV_TRUE;

                    //
                    // Set the registry key for GSP-RM to consume without having
                    // to evaluate hypervisor support
                    //
                    osWriteRegistryDword(pGpu, NV_REG_STR_RM_SET_SRIOV_MODE,
                                               NV_REG_STR_RM_SET_SRIOV_MODE_ENABLED);
                }
            }
        }

        NV_PRINTF(LEVEL_INFO, "SRIOV status[%d].\n", pGpu->bSriovEnabled);
    }

    if (pGpu->bSriovEnabled && (IS_GSP_CLIENT(pGpu) || RMCFG_FEATURE_PLATFORM_GSP))
    {
        pGpu->bVgpuGspPluginOffloadEnabled = NV_TRUE;
    }
    // Do not enable VGPU-GSP plugin offload on guest for MODS, MODS doesn't support GSP
    if (IS_VIRTUAL_WITH_SRIOV(pGpu) && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu))
    {
        {
            pGpu->bVgpuGspPluginOffloadEnabled = pGpu->getProperty(pGpu, PDB_PROP_GPU_VGPU_OFFLOAD_CAPABLE);
        }
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_CLIENT_RM_ALLOCATED_CTX_BUFFER, &data32) == NV_OK)
    {
        pGpu->bClientRmAllocatedCtxBuffer = (data32 == NV_REG_STR_RM_CLIENT_RM_ALLOCATED_CTX_BUFFER_ENABLED);

        NV_PRINTF(LEVEL_INFO, "Setting Client RM managed context buffer to %u\n",
                  pGpu->bClientRmAllocatedCtxBuffer);
    }
    else if (IS_GSP_CLIENT(pGpu) || RMCFG_FEATURE_PLATFORM_GSP)
    {
        pGpu->bClientRmAllocatedCtxBuffer = NV_TRUE;
    }
    else if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB) &&
        (pGpu->bSriovEnabled || IS_VIRTUAL_WITH_SRIOV(pGpu)))
    {
        // For zero-FB config + SRIOV
        pGpu->bClientRmAllocatedCtxBuffer = NV_TRUE;
        NV_PRINTF(LEVEL_INFO,
            "Enabled Client RM managed context buffer for zero-FB + SRIOV.\n");
    }
    else if ( RMCFG_FEATURE_MODS_FEATURES || !(pGpu->bSriovEnabled || IS_VIRTUAL(pGpu)) )
    {
        // TODO : enable this feature on mods
        pGpu->bClientRmAllocatedCtxBuffer = NV_FALSE;
    }

    if ((pGpu->bSriovEnabled && !gpuIsWarBug200577889SriovHeavyEnabled(pGpu)) ||
        RMCFG_FEATURE_PLATFORM_GSP || IS_GSP_CLIENT(pGpu))
    {
        if (osReadRegistryDword(pGpu, NV_REG_STR_RM_SPLIT_VAS_MGMT_SERVER_CLIENT_RM, &data32) == NV_OK)
        {
            pGpu->bSplitVasManagementServerClientRm =
                (data32 == NV_REG_STR_RM_SPLIT_VAS_MGMT_SERVER_CLIENT_RM_ENABLED);
        }
        else
        {
            pGpu->bSplitVasManagementServerClientRm = NV_TRUE;
        }

        NV_PRINTF(LEVEL_INFO, "Split VAS mgmt between Server/Client RM %u\n",
                  pGpu->bSplitVasManagementServerClientRm);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_GPU_FABRIC_PROBE,
                            &pGpu->fabricProbeRegKeyOverride) == NV_OK)
    {
        pGpu->fabricProbeRegKeyOverride |= \
                        DRF_NUM(_REG_STR, _RM_GPU_FABRIC_PROBE, _OVERRIDE, 1);
    }

    pGpu->bBf3WarBug4040336Enabled = NV_FALSE;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_DMA_ADJUST_PEER_MMIO_BF3,
                            &data32) == NV_OK)
    {
        pGpu->bBf3WarBug4040336Enabled = (data32 == NV_REG_STR_RM_DMA_ADJUST_PEER_MMIO_BF3_ENABLE);
    }

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_ITERATIVE_MMU_WALKER, &data32) == NV_OK)
    {
        pGpu->bIterativeMmuWalker = (data32 == NV_REG_STR_RM_ITERATIVE_MMU_WALKER_ENABLED);
    }

#if defined(GPU_LOAD_FAILURE_TEST_SUPPORTED)
    if (osReadRegistryDword(pGpu, NV_REG_STR_GPU_LOAD_FAILURE_TEST, &data32) == NV_OK)
    {
        pGpu->loadFailurePathTestControl = data32;
    }
#endif

    return NV_OK;
}

/*!
 * @brief Initialize gpu instLocOverrides, called after registry overrides to avoid ordering issues
 */
NV_STATUS
gpuInitInstLocOverrides_IMPL
(
    OBJGPU *pGpu
)
{
    NvU32 data32 = 0;
    //
    // If Hopper CC mode or protected pcie is enabled, move all except few buffers to FB
    //
    if (((osReadRegistryDword(pGpu, NV_REG_STR_RM_CONFIDENTIAL_COMPUTE, &data32) == NV_OK) &&
         FLD_TEST_DRF(_REG_STR, _RM_CONFIDENTIAL_COMPUTE, _ENABLED, _YES, data32) &&
         pGpu->getProperty(pGpu, PDB_PROP_GPU_CC_FEATURE_CAPABLE)) || gpuIsCCEnabledInHw_HAL(pGpu) ||
        ((osReadRegistryDword(pGpu, NV_REG_STR_RM_CC_MULTI_GPU_MODE, &data32) == NV_OK) &&
         (data32 == NV_REG_STR_RM_CC_MULTI_GPU_MODE_PROTECTED_PCIE)) || gpuIsProtectedPcieEnabledInHw_HAL(pGpu))
    {

        pGpu->instLocOverrides  = NV_REG_STR_RM_INST_LOC_ALL_VID;
        pGpu->instLocOverrides2 = NV_REG_STR_RM_INST_LOC_ALL_VID;
        pGpu->instLocOverrides3 = NV_REG_STR_RM_INST_LOC_ALL_VID;
        pGpu->instLocOverrides4 = NV_REG_STR_RM_INST_LOC_ALL_VID;

        // Only FW_SEC_LIC & FLCN UCODE buffers are required to be in NCOH now. These will be moved to VIDMEM eventually.
        pGpu->instLocOverrides4 = FLD_SET_DRF(_REG_STR, _RM_INST_LOC_4, _FW_SEC_LIC_COMMAND, _NCOH, pGpu->instLocOverrides4);
        pGpu->instLocOverrides4 = FLD_SET_DRF(_REG_STR, _RM_INST_LOC_4, _FLCN_UCODE_BUFFERS, _NCOH, pGpu->instLocOverrides4);
    }
    else
    {
        //
        // The pGpu fields are initialized to zero. Try to fill them from the
        // registry; if the reads fail, the values will remain zero.
        //
        osReadRegistryDword(pGpu, NV_REG_STR_RM_INST_LOC,   &pGpu->instLocOverrides);
        osReadRegistryDword(pGpu, NV_REG_STR_RM_INST_LOC_2, &pGpu->instLocOverrides2);
        osReadRegistryDword(pGpu, NV_REG_STR_RM_INST_LOC_3, &pGpu->instLocOverrides3);
        osReadRegistryDword(pGpu, NV_REG_STR_RM_INST_LOC_4, &pGpu->instLocOverrides4);

        //
        // Currently only InstLoc uses the global registry override
        // If no global override, leave the regkey hierarchy
        //
        _gpuInitGlobalSurfaceOverride(pGpu);
    }

    //
    // If instloc overrides were not provided, then default everything to
    // sysmem for ZeroFB (except for a few things if L2 cache is available).
    // Ampere onwards, default value of RMInstLoc will change to 0x10000000
    // since MODS will append pmu_instloc_coh coh to commandlines when SMC
    // args are present. Until MODS is fixed to not modify RMInstLoc when there
    // is no commandline arg doing so, change RM to recognize 0x10000000 as the
    // new default value; else we fail on 0 FB chips that do not use -fb_broken
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) &&
        ((pGpu->instLocOverrides == 0) || (pGpu->instLocOverrides == 0x10000000)) &&
        pGpu->instLocOverrides2 == 0 &&
        pGpu->instLocOverrides3 == 0 &&
        pGpu->instLocOverrides4 == 0)
    {
        pGpu->instLocOverrides  = NV_REG_STR_RM_INST_LOC_ALL_COH;
        pGpu->instLocOverrides2 = NV_REG_STR_RM_INST_LOC_ALL_COH;
        pGpu->instLocOverrides3 = NV_REG_STR_RM_INST_LOC_ALL_COH;
        // Leave instLocOverrides4 as _DEFAULT until all flavors are tested.
    }

    //
    // Move all checks of instLocOverrides to the end of the function to avoid
    // ordering issues.
    //

    // If All inst is in sysmem, set the property as well as caching attribute
    if (((pGpu->instLocOverrides == NV_REG_STR_RM_INST_LOC_ALL_NCOH) ||
         (pGpu->instLocOverrides == 0x9aaaaaaa)) &&
        (pGpu->instLocOverrides2 == NV_REG_STR_RM_INST_LOC_ALL_NCOH) &&
        (pGpu->instLocOverrides3 == NV_REG_STR_RM_INST_LOC_ALL_NCOH))
        // TODO: Check instLocOverrides4 after MODS is updated.
    {
        // Force to _DEFAULT until all flavors are tested and MODS is updated.
        pGpu->instLocOverrides4 = NV_REG_STR_RM_INST_LOC_ALL_DEFAULT;
        pGpu->instLocOverrides4 = FLD_SET_DRF(_REG_STR, _RM_INST_LOC_4, _BAR, _NCOH, pGpu->instLocOverrides4);
        pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM, NV_TRUE);
        pGpu->instCacheOverride = NV_MEMORY_UNCACHED;
    }
    else if (((pGpu->instLocOverrides == NV_REG_STR_RM_INST_LOC_ALL_COH) ||
              (pGpu->instLocOverrides == 0x95555555)) &&
             (pGpu->instLocOverrides2 == NV_REG_STR_RM_INST_LOC_ALL_COH) &&
             (pGpu->instLocOverrides3 == NV_REG_STR_RM_INST_LOC_ALL_COH))
        // TODO: Check instLocOverrides4 after MODS is updated.
    {
        pGpu->instLocOverrides4 = NV_REG_STR_RM_INST_LOC_ALL_DEFAULT; // Force to _DEFAULT until all flavors are tested and MODS is updated.
        pGpu->instLocOverrides4 = FLD_SET_DRF(_REG_STR, _RM_INST_LOC_4, _BAR, _COH, pGpu->instLocOverrides4);
        pGpu->setProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM, NV_TRUE);
        pGpu->instCacheOverride = NV_MEMORY_CACHED;
    }

    //
    // If all-inst-in-sysmem is specified, we must leave all bits in instLocOverrides,
    // instLocOverrides2, and instLocOverrides3 alone, because they are checked bitwise
    // all over RM. all-inst-in-sysmem is specified with bit patterns 0x55555555
    // (coh) or 0xaaaaaaaa (non-coh) for instLocOverrides/2/3. If it is not
    // all-inst-in-sysmem (i.e. a la carte selection of bits about what goes in sysmem
    // and what goes in FB), we are free to clear BAR PTEs/PDEs in sysmem bits and
    // we must on Volta+, because on Volta and Turing, reflected BAR mappings (e.g.
    // accessing a BAR PTE on the CPU as CPU->XVE/XTL->HOST/XAL->FBHUB->HOST/XAL->XVE/XTL->SYSMEM)
    // may cause a deadlock in the GPU. On Ampere+, it will surely cause a deadlock.
    // Note that the BAR PTE accesses to SYSMEM originating inside the GPU (from CEs and
    // SMs) are fine.
    //
    if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_ALL_INST_IN_SYSMEM) &&
        !gpuIsCacheOnlyModeEnabled(pGpu) &&
        !(FLD_TEST_DRF(_REG_STR_RM, _INST_LOC, _BAR_PTE, _DEFAULT, pGpu->instLocOverrides) &&
          FLD_TEST_DRF(_REG_STR_RM, _INST_LOC, _BAR_PDE, _DEFAULT, pGpu->instLocOverrides)))
    {
        pGpu->instLocOverrides = FLD_SET_DRF(_REG, _STR_RM_INST_LOC, _BAR_PTE, _DEFAULT, pGpu->instLocOverrides);
        pGpu->instLocOverrides = FLD_SET_DRF(_REG, _STR_RM_INST_LOC, _BAR_PDE, _DEFAULT, pGpu->instLocOverrides);
        NV_PRINTF(LEVEL_WARNING, "Ignoring regkeys to place BAR PTE/PDE in SYSMEM\n");
    }

    return NV_OK;
}

#define GP100_BYPASS_47BIT_PA_WAR 4

/*!
 * @brief   This function sets the global surface location override value based
 *          on a regkey or a PDB property. The PDB property can be auto-set or
 *          can be set (elsewhere) based on the platform and chip.
 */
static void
_gpuInitGlobalSurfaceOverride
(
    OBJGPU *pGpu
)
{
    NvU32 globalOverride;

    //
    // Precedence of global overrides.
    // 1. HAL layer forces an override
    // 2. Regkey override.
    //
    if (pGpu->bInstLoc47bitPaWar)
    {
        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_BROKEN_FB) ||
            (pGpu->instLocOverrides  != 0) ||
            (pGpu->instLocOverrides2 != 0) ||
            (pGpu->instLocOverrides3 != 0) ||
            (pGpu->instLocOverrides4 != 0))
        {
            NV_PRINTF(LEVEL_ERROR,
                "INSTLOC overrides may not work with large mem systems on GP100+\n");
        }
        else
        {
            globalOverride = GP100_BYPASS_47BIT_PA_WAR;
            pGpu->bRegUsesGlobalSurfaceOverrides = NV_TRUE;
        }
    }

    if (!pGpu->bRegUsesGlobalSurfaceOverrides)
    {
        NvU32 data32;
        if (osReadRegistryDword(pGpu, NV_REG_STR_GLOBAL_SURFACE_OVERRIDE, &data32) == NV_OK)
        {
            if (DRF_VAL(_REG_STR, _GLOBAL_SURFACE_OVERRIDE_RM, _ENABLE, data32) == NV_REG_STR_GLOBAL_SURFACE_OVERRIDE_ENABLE)
            {
                globalOverride = DRF_VAL(_REG_STR, _GLOBAL_SURFACE_OVERRIDE_RM, _VALUE, data32);
                pGpu->bRegUsesGlobalSurfaceOverrides = NV_TRUE;
            }
        }
    }

    // Apply global overrides, if any
    if (pGpu->bRegUsesGlobalSurfaceOverrides)
    {
        if (globalOverride == GP100_BYPASS_47BIT_PA_WAR)
        {
            // force units not supporting 47 bit PAs to vidmem for GP100
            pGpu->instLocOverrides  = FLD_SET_DRF(_REG_STR_RM, _INST_LOC, _INSTBLK, _VID, pGpu->instLocOverrides);
            pGpu->instLocOverrides  = FLD_SET_DRF(_REG_STR_RM, _INST_LOC, _RUNLIST, _VID, pGpu->instLocOverrides);
            pGpu->instLocOverrides  = FLD_SET_DRF(_REG_STR_RM, _INST_LOC, _USERD,   _VID, pGpu->instLocOverrides);
            pGpu->instLocOverrides  = FLD_SET_DRF(_REG_STR_RM, _INST_LOC, _PMUINST, _VID, pGpu->instLocOverrides);
            pGpu->instLocOverrides2 = 0;
            pGpu->instLocOverrides3 = FLD_SET_DRF(_REG_STR_RM, _INST_LOC_3, _FLCNINST, _VID, pGpu->instLocOverrides3);
            pGpu->instLocOverrides4 = 0;
        }
        else
        {
            NvU32 i;
            NvU32 ovBits;

            ovBits = DRF_VAL(_REG_STR, _GLOBAL_SURFACE_OVERRIDE_RM, _VALUE, globalOverride);
            pGpu->instLocOverrides = 0;

            // Propagate two-bit global override value to 32-bit RM override value
            for (i = 0; i < 32; i += 2)
            {
                pGpu->instLocOverrides |= (ovBits << i);
            }
            pGpu->instLocOverrides2 = pGpu->instLocOverrides;
            pGpu->instLocOverrides3 = pGpu->instLocOverrides;
            pGpu->instLocOverrides4 = pGpu->instLocOverrides;
        }
    }
}
