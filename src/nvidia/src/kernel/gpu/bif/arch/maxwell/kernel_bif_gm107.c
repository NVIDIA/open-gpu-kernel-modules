/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/* ------------------------- System Includes -------------------------------- */
#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/bus/kern_bus.h"
#include "platform/chipset/chipset.h"
#include "nvdevid.h"
#include <rmapi/nv_gpu_ops.h>
#include "nvmisc.h"
#include "kernel/gpu/mc/kernel_mc.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "gpu/mem_sys/kern_mem_sys.h"

#include "published/maxwell/gm107/dev_boot.h"
#include "published/maxwell/gm107/dev_nv_xp.h"
#include "published/maxwell/gm107/dev_nv_xve.h"
#include "published/maxwell/gm107/dev_nv_xve_addendum.h"
#include "published/maxwell/gm107/dev_nv_xve1.h"
#include "published/maxwell/gm107/dev_nv_pcfg_xve_addendum.h"
#include "published/maxwell/gm107/dev_nv_pcfg_xve1_addendum.h"

#include "nvpcie.h"

// Defines for C73 chipset registers
#ifndef NV_XVR_VEND_XP1
#define NV_XVR_VEND_XP1                                  0x00000F04 /* RW-4R */

#define NV_XVR_VEND_XP1_IGNORE_L0S                            23:23 /* RWIVF */
#define NV_XVR_VEND_XP1_IGNORE_L0S_INIT                  0x00000000 /* RWI-V */
#define NV_XVR_VEND_XP1_IGNORE_L0S__PROD                 0x00000000 /* RW--V */
#define NV_XVR_VEND_XP1_IGNORE_L0S_EN                    0x00000001 /* RW--V */
#endif

// Factor by which vGPU migration API bandwidth should be derated
#define VGPU_MIGRATION_API_DERATE_FACTOR   5

// XVE register map for PCIe config space
static const NvU32 xveRegMapValid[] = NV_PCFG_XVE_REGISTER_VALID_MAP;
static const NvU32 xveRegMapWrite[] = NV_PCFG_XVE_REGISTER_WR_MAP;
static const NvU32 xve1RegMapValid[] = NV_PCFG_XVE1_REGISTER_VALID_MAP;
static const NvU32 xve1RegMapWrite[] = NV_PCFG_XVE1_REGISTER_WR_MAP;

static NV_STATUS _kbifSavePcieConfigRegisters_GM107(OBJGPU *pGpu, KernelBif *pKernelBif, const PKBIF_XVE_REGMAP_REF pRegmapRef);
static NV_STATUS _kbifRestorePcieConfigRegisters_GM107(OBJGPU *pGpu, KernelBif *pKernelBif, const PKBIF_XVE_REGMAP_REF pRegmapRef);

/* ------------------------ Public Functions -------------------------------- */

/*!
 * @brief Get PCIe config test registers
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 */
void
kbifGetPcieConfigAccessTestRegisters_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pciStart,
    NvU32     *pcieStart
)
{
   *pciStart  = NV_XVE_ID;
   *pcieStart = NV_XVE_VCCAP_HDR;
}

/*!
 * @brief Verify PCIe config test registers
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 *
 * @return  NV_OK
 */
NV_STATUS
kbifVerifyPcieConfigAccessTestRegisters_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      nvXveId,
    NvU32      nvXveVccapHdr
)
{
    NvU32 data;

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_RD32(pGpu, NV_XVE_ID, &data));

    if (FLD_TEST_DRF(_XVE, _ID, _VENDOR, _NVIDIA, data))
    {
        if (data != nvXveId)
            return NV_ERR_NOT_SUPPORTED;

        NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_RD32(pGpu, NV_XVE_VCCAP_HDR, &data));

        if (FLD_TEST_DRF(_XVE, _VCCAP_HDR, _ID, _VC, data) &&
            FLD_TEST_DRF(_XVE, _VCCAP_HDR, _VER, _1, data))
        {
            if (data != nvXveVccapHdr)
                return NV_ERR_NOT_SUPPORTED;
            return NV_OK;
        }
    }
    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * @brief Re-arm MSI
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifRearmMSI_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NV_STATUS status = gpuSanityCheckRegisterAccess(pGpu, 0, NULL);

    if (status != NV_OK)
    {
        return;
    }

    // The 32 byte value doesn't matter, HW only looks at the offset.
    osGpuWriteReg032(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_CYA_2, 0);
}

/*!
 * @brief Check if MSI is enabled in HW
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 *
 * @return  True if MSI enabled else False
 */
NvBool
kbifIsMSIEnabledInHW_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 data32;

    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        return NV_FALSE;
    }

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_MSI_CTRL, &data32))
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_XVE_MSI_CTRL\n");
    }

    return FLD_TEST_DRF(_XVE, _MSI_CTRL, _MSI, _ENABLE, data32);
}

/*!
 * @brief Check if access to PCI config space is enabled
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 * @return  True if access to PCI config space is enabled
 */
NvBool
kbifIsPciIoAccessEnabled_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   data = 0;

    if (NV_OK == GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEV_CTRL, &data))
    {
        if (FLD_TEST_DRF(_XVE, _DEV_CTRL, _CMD_IO_SPACE, _ENABLED, data))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
 * @brief Check if device is a 3D controller
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 * @return  True if device is a 3D controller
 */
NvBool
kbifIs3dController_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32   data = 0;

    if (NV_OK == GPU_BUS_CFG_RD32(pGpu, NV_XVE_REV_ID, &data))
    {
        if (FLD_TEST_DRF(_XVE, _REV_ID, _CLASS_CODE, _3D, data))
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*!
 * @brief Enable/disable no snoop for GPU
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 * @param[in]  bEnable     True if No snoop needs to be enabled
 *
 * @return NV_OK If no snoop modified as requested
 */
NV_STATUS
kbifEnableNoSnoop_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvBool     bEnable
)
{
    NvU8  fieldVal;
    NvU32 regVal;

    regVal = GPU_REG_RD32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_DEVICE_CONTROL_STATUS);

    fieldVal = bEnable ? 1 : 0;
    regVal   = FLD_SET_DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS,
                               _ENABLE_NO_SNOOP, fieldVal, regVal);

    GPU_REG_WR32(pGpu, DEVICE_BASE(NV_PCFG) + NV_XVE_DEVICE_CONTROL_STATUS, regVal);

    return NV_OK;
}

/*!
 * @brief Enables Relaxed Ordering PCI-E Capability in the PCI Config Space
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifPcieConfigEnableRelaxedOrdering_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 xveDevCtrlStatus;

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, &xveDevCtrlStatus))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_XVE_DEVICE_CONTROL_STATUS!\n");
        DBG_BREAKPOINT();
    }
    else
    {
        GPU_BUS_CFG_FLD_WR_DRF_DEF(pGpu, xveDevCtrlStatus, _XVE, _DEVICE_CONTROL_STATUS,
                                   _ENABLE_RELAXED_ORDERING, _INIT);
    }
}

/*!
 * @brief Disables Relaxed Ordering PCI-E Capability in the PCI Config Space
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifPcieConfigDisableRelaxedOrdering_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 xveDevCtrlStatus;

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, &xveDevCtrlStatus))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_XVE_DEVICE_CONTROL_STATUS!\n");
        DBG_BREAKPOINT();
    }
    else
    {
        xveDevCtrlStatus = FLD_SET_DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS,
                                           _ENABLE_RELAXED_ORDERING, 0, xveDevCtrlStatus);
        GPU_BUS_CFG_WR32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, xveDevCtrlStatus);
    }
}

/*!
 * Helper function for bifSavePcieConfigRegisters_GM107()
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[in]  pKernelBif     Kernel Bif object pointer
 * @param[in]  pRegmapRef     XVE Register map structure pointer
 *
 * @return  'NV_OK' if successful, an RM error code otherwise.
 */
static NV_STATUS
_kbifSavePcieConfigRegisters_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    const PKBIF_XVE_REGMAP_REF pRegmapRef
)
{
    NV_STATUS status;
    NvU16     index;

    // Read and save config space offset based on the bit map
    for (index = 0; index < pRegmapRef->numXveRegMapValid; index++)
    {
        NvU16 i, regOffset, bufOffset;
        NvU32 mask = 1;

        for (i = 0; i < sizeof(pRegmapRef->xveRegMapValid[0]) * 8; i++)
        {
            mask = 1 << i;
            NV_ASSERT((pRegmapRef->xveRegMapWrite[index] & mask) == 0 ||
                      (pRegmapRef->xveRegMapValid[index] & mask) != 0);

            if ((pRegmapRef->xveRegMapValid[index] & mask) == 0)
            {
                continue;
            }

            bufOffset = (index * sizeof(pRegmapRef->xveRegMapValid[0]) * 8) + i;
            regOffset = bufOffset * sizeof(pRegmapRef->bufBootConfigSpace[0]);

            status = PCI_FUNCTION_BUS_CFG_RD32(pGpu, pRegmapRef->nFunc,
                                               regOffset, &pRegmapRef->bufBootConfigSpace[bufOffset]);
            if (status != NV_OK)
            {
                return status;
            }
        }
    }

    pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_SECONDARY_BUS_RESET_SUPPORTED, NV_TRUE);

    return NV_OK;
}

/*!
 * Save boot time PCIe Config space
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[in]  pKernelBif     Kernel Bif object pointer
 *
 * @return  'NV_OK' if successful, an RM error code otherwise.
 */
NV_STATUS
kbifSavePcieConfigRegisters_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NV_STATUS status;

    //
    // Save config space if GPU is about to enter Function Level Reset
    // OR if GPU is about to enter GC6 state
    // OR if on non-windows platform, FORCE_PCIE_CONFIG_SAVE is set and SBR is snabled
    // OR if on windows platform, SBR is enabled
    //
    if (!pKernelBif->bPreparingFunctionLevelReset &&
        !IS_GPU_GC6_STATE_ENTERING(pGpu) &&
        !((RMCFG_FEATURE_PLATFORM_WINDOWS ||
           pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FORCE_PCIE_CONFIG_SAVE)) &&
          pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_SECONDARY_BUS_RESET_ENABLED)))
    {
        return NV_OK;
    }

    // save pcie config space for function 0
    status = _kbifSavePcieConfigRegisters_GM107(pGpu, pKernelBif,
                                                &pKernelBif->xveRegmapRef[0]);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Saving PCIe config space failed for gpu.\n");
        NV_ASSERT(0);
        return status;
    }

    // No need to save/restore azalia config space if gpu is in GC6 cycle or if it is in FLR
    if (IS_GPU_GC6_STATE_ENTERING(pGpu) ||
        pKernelBif->bPreparingFunctionLevelReset)
    {
        return NV_OK;
    }

    // Return early if device is not multifunction (azalia is disabled or not present)
    if (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_DEVICE_IS_MULTIFUNCTION))
    {
        return NV_OK;
    }

    // Save pcie config space for function 1
    status = _kbifSavePcieConfigRegisters_GM107(pGpu, pKernelBif,
                                                &pKernelBif->xveRegmapRef[1]);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Saving PCIe config space failed for azalia.\n");
        NV_ASSERT(0);
    }

    return status;
}

/*!
 * Helper function for bifRestorePcieConfigRegisters_GM107()
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[in]  pKernelBif     Kernel Bif object pointer
 * @param[in]  pRegmapRef     XVE Register map structure pointer
 *
 * @return  'NV_OK' if successful, an RM error code otherwise.
 */
static NV_STATUS
_kbifRestorePcieConfigRegisters_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    const PKBIF_XVE_REGMAP_REF pRegmapRef
)
{
    NvU32      domain = gpuGetDomain(pGpu);
    NvU8       bus    = gpuGetBus(pGpu);
    NvU8       device = gpuGetDevice(pGpu);
    NvU16      vendorId;
    NvU16      deviceId;
    NvU32      val;
    NV_STATUS  status;
    void      *handle;
    NvU16      index;
    RMTIMEOUT  timeout;
    NvBool     bGcxPmuCfgRestore;

    {
        bGcxPmuCfgRestore = pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_GCX_PMU_CFG_SPACE_RESTORE);
    }

    handle = osPciInitHandle(domain, bus, device, pRegmapRef->nFunc,
                             &vendorId, &deviceId);
    NV_ASSERT_OR_RETURN(handle, NV_ERR_INVALID_POINTER);

    if (IS_GPU_GC6_STATE_EXITING(pGpu) &&
        bGcxPmuCfgRestore)
    {
        //
        // PMU Will Restore the config Space
        // As a last step PMU should set CMD_MEMORY_SPACE ENABLED after it restores the config space
        // Poll This register to see if PMU is finished or not otherwise timeout.
        //
        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
        do
        {
            val = osPciReadDword(handle, NV_XVE_DEV_CTRL);
            status = gpuCheckTimeout(pGpu, &timeout);
            if (status == NV_ERR_TIMEOUT)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Timeout waiting for PCIE Config Space Restore from PMU, RM takes over\n");
                DBG_BREAKPOINT();

                NvU32 *pReg = NULL;
                pReg  = &pRegmapRef->bufBootConfigSpace[NV_XVE_DEV_CTRL /
                                                sizeof(pRegmapRef->bufBootConfigSpace[0])];
                *pReg = FLD_SET_DRF(_XVE, _DEV_CTRL, _CMD_MEMORY_SPACE, _ENABLED, *pReg);
                osPciWriteDword(handle, NV_XVE_DEV_CTRL, pRegmapRef->bufBootConfigSpace[1]);
                osPciWriteDword(handle, NV_XVE_BAR0, pRegmapRef->bufBootConfigSpace[4]);

                break;
            }
        } while (FLD_TEST_DRF(_XVE, _DEV_CTRL, _CMD_MEMORY_SPACE, _DISABLED, val));

        return NV_OK;
    }

    // Enable BAR0 accesses so we can restore config space more quickly.
    kbifRestoreBar0_HAL(pGpu, pKernelBif, handle, pRegmapRef->bufBootConfigSpace);

    // Enable required fields of NV_XVE_DEV_CTRL
    val = osPciReadDword(handle, NV_XVE_DEV_CTRL);
    val = FLD_SET_DRF(_XVE, _DEV_CTRL, _CMD_MEMORY_SPACE, _ENABLED, val) |
          FLD_SET_DRF(_XVE, _DEV_CTRL, _CMD_BUS_MASTER, _ENABLED, val);
    osPciWriteDword(handle, NV_XVE_DEV_CTRL, val);

    // Restore only the valid config space offsets based on bit map
    for (index = 0; index < pRegmapRef->numXveRegMapValid; index++)
    {
        NvU16 i, regOffset, bufOffset;

        for (i = 0; i < sizeof(pRegmapRef->xveRegMapValid[0]) * 8; i++)
        {
            if ((pRegmapRef->xveRegMapWrite[index] & (1 << i)) == 0)
            {
                continue;
            }

            bufOffset = (index * sizeof(pRegmapRef->xveRegMapValid[0]) * 8) + i;
            regOffset = bufOffset * sizeof(pRegmapRef->bufBootConfigSpace[0]);
            if (regOffset == NV_XVE_DEV_CTRL)
            {
                continue;
            }

            //
            // This is a special case where we don't use the standard macro to write a register.
            // The macro will not allow access when PDB_PROP_GPU_IS_LOST is true.
            // This check is required to keep other accesses from touching the GPU for now.
            //
            osGpuWriteReg032(pGpu,
                             ((pRegmapRef->nFunc == 0) ? DEVICE_BASE(NV_PCFG) : DEVICE_BASE(NV_PCFG1)) + regOffset,
                             pRegmapRef->bufBootConfigSpace[bufOffset]);

            if (pRegmapRef->nFunc != 0)
            {
                status = PCI_FUNCTION_BUS_CFG_WR32(pGpu, pRegmapRef->nFunc, regOffset,
                                                   pRegmapRef->bufBootConfigSpace[bufOffset]);
                if (status != NV_OK)
                {
                    return status;
                }
            }
        }
    }

    //
    // Restore saved value of NV_XVE_DEV_CTRL, the second register saved in the buffer.
    // If we reach this point, it's RM-CPU restoration path.
    // Check if PMU_CFG_SPACE_RESTORE property was enabled
    // to confirm it's a debugging parallel restoration and
    // set back to _ENABLE before restoration
    //
    if (bGcxPmuCfgRestore)
    {
        NvU32 *pReg = NULL;
        pReg  = &pRegmapRef->bufBootConfigSpace[NV_XVE_DEV_CTRL /
                                                sizeof(pRegmapRef->bufBootConfigSpace[0])];
        *pReg = FLD_SET_DRF(_XVE, _DEV_CTRL, _CMD_MEMORY_SPACE, _ENABLED, *pReg);

    }

    osPciWriteDword(handle, NV_XVE_DEV_CTRL, pRegmapRef->bufBootConfigSpace[1]);

    return NV_OK;
}

/*!
 * Restore boot time PCIe Config space
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel Bif object pointer
 *
 * @return  'NV_OK' if successful, an RM error code otherwise.
 */
NV_STATUS
kbifRestorePcieConfigRegisters_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NV_STATUS status;
    RMTIMEOUT timeout;
    NvU64     timeStampStart;
    NvU64     timeStampEnd;

    if (pKernelBif->xveRegmapRef[0].bufBootConfigSpace == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Config space buffer is NULL!\n");
        NV_ASSERT(0);
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    // Restore pcie config space for function 0
    status = _kbifRestorePcieConfigRegisters_GM107(pGpu, pKernelBif,
                                                   &pKernelBif->xveRegmapRef[0]);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Restoring PCIe config space failed for gpu.\n");
        NV_ASSERT(0);
        return status;
    }

    // No need to save/restore azalia config space if gpu is in GC6 cycle or if it is in FLR
    if (IS_GPU_GC6_STATE_EXITING(pGpu) ||
        pKernelBif->bInFunctionLevelReset)
    {
        //
        // Check that GPU is really accessible.
        // Skip on pre-silicon because there can be timing issues in the test between device ready and this code.
        // Todo: find a safe timeout for pre-silicon runs
        //
        if (IS_SILICON(pGpu))
        {
            // Check if GPU is actually accessible before continue
            osGetPerformanceCounter(&timeStampStart);
            gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
            NvU32 pmcBoot0 = GPU_REG_RD32(pGpu, NV_PMC_BOOT_0);

            while (pmcBoot0 != pGpu->chipId0)
            {
                NV_PRINTF(LEVEL_INFO,
                          "GPU not back on the bus after %s, 0x%x != 0x%x!\n",
                          pKernelBif->bInFunctionLevelReset?"FLR":"GC6 exit", pmcBoot0, pGpu->chipId0);
                pmcBoot0 = GPU_REG_RD32(pGpu, NV_PMC_BOOT_0);
                NV_ASSERT(0);
                status = gpuCheckTimeout(pGpu, &timeout);
                if (status == NV_ERR_TIMEOUT)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Timeout GPU not back on the bus after %s,\n", pKernelBif->bInFunctionLevelReset?"FLR":"GC6 exit");
                    DBG_BREAKPOINT();
                    return status;
                }
            }

            osGetPerformanceCounter(&timeStampEnd);
            NV_PRINTF(LEVEL_ERROR,
                      "Time spend on GPU back on bus is 0x%x ns,\n",
                      (NvU32)NV_MIN(NV_U32_MAX, timeStampEnd - timeStampStart));
        }

        return NV_OK;
    }

    // Return early if device is not multifunction (azalia is disabled or not present)
    if (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_DEVICE_IS_MULTIFUNCTION))
    {
        return NV_OK;
    }

    // Restore pcie config space for function 1
    status = _kbifRestorePcieConfigRegisters_GM107(pGpu, pKernelBif,
                                                   &pKernelBif->xveRegmapRef[1]);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Restoring PCIe config space failed for azalia.\n");
        NV_ASSERT(0);
    }

    return status;
}

/*!
 * @brief Get XVE status bits
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[in]   pKernelBif  BIF object pointer
 * @param[out]  pBits       PCIe error status values
 * @param[out]  pStatus     Full XVE status
 *
 * @return  NV_OK
 */
NV_STATUS
kbifGetXveStatusBits_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBits,
    NvU32     *pStatus
)
{
    // control/status reg
    NvU32 xveDevCtrlStatus;

    if (NV_OK  != GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, &xveDevCtrlStatus))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unable to read NV_XVE_DEVICE_CONTROL_STATUS!\n");
    }
    if ( pBits == NULL )
        return NV_ERR_GENERIC;

    *pBits = 0;

    // The register read above returns garbage on fmodel, so just return.
    if (IS_FMODEL(pGpu))
    {
        if (pStatus)
        {
            *pStatus = 0;
        }
        return NV_OK;
    }

    if (pStatus)
        *pStatus = xveDevCtrlStatus;

    if (xveDevCtrlStatus & DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS, _CORR_ERROR_DETECTED, 1))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR;
    if (xveDevCtrlStatus & DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS, _NON_FATAL_ERROR_DETECTED, 1))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_NON_FATAL_ERROR;
    if (xveDevCtrlStatus & DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS, _FATAL_ERROR_DETECTED, 1))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_FATAL_ERROR;
    if (xveDevCtrlStatus & DRF_NUM(_XVE, _DEVICE_CONTROL_STATUS, _UNSUPP_REQUEST_DETECTED, 1))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_UNSUPP_REQUEST;

    if (pKernelBif->EnteredRecoverySinceErrorsLastChecked)
    {
        pKernelBif->EnteredRecoverySinceErrorsLastChecked = NV_FALSE;
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_ENTERED_RECOVERY;
    }

    return NV_OK;
}

/*!
 * @brief Clear the XVE status bits
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[in]   pKernelBif  BIF object pointer
 * @param[out]  pStatus     Full XVE status
 *
 * @return  NV_OK
 */
NV_STATUS
kbifClearXveStatus_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pStatus
)
{
    NvU32 xveDevCtrlStatus;

    if (pStatus)
    {
        xveDevCtrlStatus = *pStatus;
        if (xveDevCtrlStatus == 0)
        {
            return NV_OK;
        }
    }
    else
    {
        if (NV_OK  != GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, &xveDevCtrlStatus))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to read NV_XVE_DEVICE_CONTROL_STATUS!\n");
        }
    }

    GPU_BUS_CFG_WR32(pGpu, NV_XVE_DEVICE_CONTROL_STATUS, xveDevCtrlStatus);

    return NV_OK;
}

/*!
 * @brief Get XVE AER bits
 *
 * @param[in]   pGpu        GPU object pointer
 * @param[in]   pKernelBif  BIF object pointer
 * @param[out]  pBits       PCIe AER error status values
 *
 * @return  NV_OK
 */
NV_STATUS
kbifGetXveAerBits_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBits
)
{
    NvU32 xveAerUncorr;
    NvU32 xveAerCorr;

    if (pBits == NULL)
    {
        return NV_ERR_GENERIC;
    }

    *pBits = 0;

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_AER_UNCORR_ERR, &xveAerUncorr))
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_XVE_AER_UNCORR_ERR\n");
        return NV_ERR_GENERIC;
    }
    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, NV_XVE_AER_CORR_ERR, &xveAerCorr))
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_XVE_AER_CORR_ERR\n");
        return NV_ERR_GENERIC;
    }

    // The register read above returns garbage on fmodel, so just return.
    if (IS_FMODEL(pGpu))
    {
        return NV_OK;
    }

    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _DLINK_PROTO_ERR, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _POISONED_TLP, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _CPL_TIMEOUT, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _UNEXP_CPL, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _MALFORMED_TLP, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP;
    if (FLD_TEST_DRF(_XVE, _AER_UNCORR_ERR, _UNSUPPORTED_REQ, _ACTIVE, xveAerUncorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ;

    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _RCV_ERR, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _BAD_TLP, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _BAD_DLLP , _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _RPLY_ROLLOVER, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _RPLY_TIMEOUT, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT;
    if (FLD_TEST_DRF(_XVE, _AER_CORR_ERR, _ADVISORY_NONFATAL, _ACTIVE, xveAerCorr))
        *pBits |= NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_ADVISORY_NONFATAL;

    return NV_OK;
}

/*!
 * @brief Clear the XVE AER bits
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 * @param[in]  bits        PCIe AER error status values
 *
 * @return  NV_OK
 */
NV_STATUS
kbifClearXveAer_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      bits
)
{
    NvU32 xveAerUncorr = 0;
    NvU32 xveAerCorr   = 0;

    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _DLINK_PROTO_ERR, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _POISONED_TLP, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _CPL_TIMEOUT, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _UNEXP_CPL, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _MALFORMED_TLP, _CLEAR, xveAerUncorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ)
        xveAerUncorr = FLD_SET_DRF(_XVE, _AER_UNCORR_ERR, _UNSUPPORTED_REQ, _CLEAR, xveAerUncorr);

    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _RCV_ERR, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _BAD_TLP, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _BAD_DLLP, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _RPLY_ROLLOVER, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _RPLY_TIMEOUT, _CLEAR, xveAerCorr);
    if (bits & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_ADVISORY_NONFATAL)
        xveAerCorr = FLD_SET_DRF(_XVE, _AER_CORR_ERR, _ADVISORY_NONFATAL, _CLEAR, xveAerCorr);

    if (xveAerUncorr != 0)
    {
        GPU_BUS_CFG_WR32(pGpu, NV_XVE_AER_UNCORR_ERR, xveAerUncorr);
    }
    if (xveAerCorr != 0)
    {
        GPU_BUS_CFG_WR32(pGpu, NV_XVE_AER_CORR_ERR, xveAerCorr);
    }

    return NV_OK;
}

/*!
 * @brief Returns the BAR0 offset and size of the PCI config space mirror
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 * @param[out]  pBase      BAR0 offset of the PCI config space mirror
 * @param[out]  pSize      Size in bytes of the PCI config space mirror
 *
 * @returns NV_OK
 */
NV_STATUS
kbifGetPciConfigSpacePriMirror_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32     *pBase,
    NvU32     *pSize
)
{
    *pBase = DEVICE_BASE(NV_PCFG);
    *pSize = DEVICE_EXTENT(NV_PCFG) - DEVICE_BASE(NV_PCFG) + 1;
    return NV_OK;
}

/*!
 * @brief C73 chipset WAR
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 */
void
kbifExecC73War_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    OBJSYS  *pSys = SYS_GET_INSTANCE();
    OBJOS   *pOS  = SYS_GET_OS(pSys);
    OBJCL   *pCl  = SYS_GET_CL(pSys);
    NvU32    val;

    if (CS_NVIDIA_C73 == pCl->Chipset)
    {
        //
        // Turn off L0s on the chipset which are required by the suspend/resume
        // cycles in Vista. See bug 400044 for more details.
        //

        // vAddr is a mapped cpu virtual addr into the root ports config space.
        if (!pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS) &&
            (pGpu->gpuClData.rootPort.vAddr != 0))
        {
            val = MEM_RD32((NvU8*)pGpu->gpuClData.rootPort.vAddr+NV_XVR_VEND_XP1);
            val = FLD_SET_DRF(_XVR, _VEND_XP1, _IGNORE_L0S, _EN, val);
            MEM_WR32((NvU8*)pGpu->gpuClData.rootPort.vAddr+NV_XVR_VEND_XP1, val);
        }
        else if (pOS->getProperty(pOS, PDB_PROP_OS_DOES_NOT_ALLOW_DIRECT_PCIE_MAPPINGS) &&
                 pGpu->gpuClData.rootPort.addr.valid)
        {
            val = osPciReadDword(pGpu->gpuClData.rootPort.addr.handle, NV_XVR_VEND_XP1);
            val = FLD_SET_DRF(_XVR, _VEND_XP1, _IGNORE_L0S, _EN, val);
            osPciWriteDword(pGpu->gpuClData.rootPort.addr.handle, NV_XVR_VEND_XP1, val);
        }
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Cannot turn off L0s on C73 chipset, suspend/resume may fail (Bug 400044).\n");
            DBG_BREAKPOINT();
        }
    }
}

NV_STATUS
kbifGetBusOptionsAddr_GM107
(
    OBJGPU     *pGpu,
    KernelBif  *pKernelBif,
    BUS_OPTIONS options,
    NvU32      *addrReg
)
{
    NV_STATUS status = NV_OK;

    switch (options)
    {
        case BUS_OPTIONS_DEV_CONTROL_STATUS:
            *addrReg = NV_XVE_DEVICE_CONTROL_STATUS;
            break;
        case BUS_OPTIONS_DEV_CONTROL_STATUS_2:
            *addrReg = NV_XVE_DEVICE_CONTROL_STATUS_2;
            break;
        case BUS_OPTIONS_LINK_CONTROL_STATUS:
            *addrReg = NV_XVE_LINK_CONTROL_STATUS;
            break;
        case BUS_OPTIONS_LINK_CAPABILITIES:
            *addrReg = NV_XVE_LINK_CAPABILITIES;
            break;
        case BUS_OPTIONS_L1_PM_SUBSTATES_CTRL_1:
            *addrReg = NV_XVE_L1_PM_SUBSTATES_CTRL1;
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Invalid register type passed 0x%x\n",
                      options);
            status = NV_ERR_GENERIC;
            break;
    }
    return status;
}

NV_STATUS
kbifDisableSysmemAccess_GM107
(
    OBJGPU     *pGpu,
    KernelBif  *pKernelBif,
    NvBool      bDisable
)
{
    NV_STATUS status = NV_OK;
    RM_API   *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS params = {0};

    // Only support on Windows
    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_PLATFORM_WINDOWS, NV_ERR_NOT_SUPPORTED);

    params.bDisable = bDisable;
    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS,
                             &params,
                             sizeof(NV2080_CTRL_INTERNAL_BIF_DISABLE_SYSTEM_MEMORY_ACCESS_PARAMS));

    // Only set the PDB in kernel if it was set in physical successfully
    if (status == NV_OK)
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_SYSTEM_ACCESS_DISABLED, bDisable);
    }

    return status;
}

/*!
 * This function setups the xve register map pointers
 *
 * @param[in]  pGpu           GPU object pointer
 * @param[in]  pKernelBif     Pointer to KernelBif object
 * @param[in]  func           PCIe function number
 *
 * @return  'NV_OK' if successful, an RM error code otherwise.
 */
NV_STATUS
kbifInitXveRegMap_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU8       func
)
{
    if (func == 0)
    {
        pKernelBif->xveRegmapRef[0].nFunc              = 0;
        pKernelBif->xveRegmapRef[0].xveRegMapValid     = xveRegMapValid;
        pKernelBif->xveRegmapRef[0].xveRegMapWrite     = xveRegMapWrite;
        pKernelBif->xveRegmapRef[0].numXveRegMapValid  = NV_ARRAY_ELEMENTS(xveRegMapValid);
        pKernelBif->xveRegmapRef[0].numXveRegMapWrite  = NV_ARRAY_ELEMENTS(xveRegMapWrite);
        pKernelBif->xveRegmapRef[0].bufBootConfigSpace = pKernelBif->cacheData.gpuBootConfigSpace;
        // No MSIX for this GPU
        pKernelBif->xveRegmapRef[0].bufMsixTable       = NULL;
    }
    else if (func == 1)
    {
        pKernelBif->xveRegmapRef[1].nFunc              = 1;
        pKernelBif->xveRegmapRef[1].xveRegMapValid     = xve1RegMapValid;
        pKernelBif->xveRegmapRef[1].xveRegMapWrite     = xve1RegMapWrite;
        pKernelBif->xveRegmapRef[1].numXveRegMapValid  = NV_ARRAY_ELEMENTS(xve1RegMapValid);
        pKernelBif->xveRegmapRef[1].numXveRegMapWrite  = NV_ARRAY_ELEMENTS(xve1RegMapWrite);
        pKernelBif->xveRegmapRef[1].bufBootConfigSpace = pKernelBif->cacheData.azaliaBootConfigSpace;
        // No MSIX for this func
        pKernelBif->xveRegmapRef[1].bufMsixTable       = NULL;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid argument, func: %d.\n", func);
        NV_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * @brief Clears Bus Master Enable bit in command register, disabling
 *  Function 0 - from issuing any new requests to sysmem.
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  KernelBif object pointer
 *
 * @return NV_OK
 */
NV_STATUS
kbifStopSysMemRequests_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvBool     bStop
)
{
    NvU32 regVal;

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_RD32(pGpu, NV_XVE_DEV_CTRL, &regVal));

    if (bStop)
    {
        regVal = FLD_SET_DRF(_XVE, _DEV_CTRL, _CMD_BUS_MASTER, _DISABLED, regVal);
    }
    else
    {
        regVal = FLD_SET_DRF(_XVE, _DEV_CTRL, _CMD_BUS_MASTER, _ENABLED, regVal);
    }

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_WR32(pGpu, NV_XVE_DEV_CTRL, regVal));

    return NV_OK;
}


/*
 * @brief Restore the BAR0 register from the given config space buffer
 * BAR0 register restore has to use the config cycle write.
 *
 * @param[in] pGpu              GPU object pointer
 * @param[in] pKernelBif        Pointer to KernelBif object
 * @param[in] handle            PCI handle for GPU
 * @param[in] bufConfigSpace    Stored config space
 */
void
kbifRestoreBar0_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    void      *handle,
    NvU32     *bufConfigSpace
)
{
    //
    // Not much ROI in storing BAR offsets for legacy chips since
    // BAR offsets are not going to change ever for legacy chips
    //
    osPciWriteDword(handle, NV_XVE_BAR0,
                    bufConfigSpace[NV_XVE_BAR0/sizeof(NvU32)]);
}


/*!
 * @brief Check if any of the BAR register reads returns a valid value.
 *
 * @param[in] pGpu          GPU object pointer
 * @param[in] pKernelBif    KernelBif object pointer
 *
 * @returns   NV_TRUE if any BAR register read returns a valid value
 *            NV_FALSE if all the BAR registers return an invalid values
 */
NvBool
kbifAnyBarsAreValid_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 domain = gpuGetDomain(pGpu);
    NvU8 bus = gpuGetBus(pGpu);
    NvU8 device = gpuGetDevice(pGpu);
    NvU16 vendorId, deviceId;
    void *handle;

    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);

    if (osPciReadDword(handle, NV_XVE_BAR0) == pKernelBif->cacheData.gpuBootConfigSpace[4])
    {
        // BAR0 is valid
        return NV_TRUE;
    }

    if ((osPciReadDword(handle, NV_XVE_BAR1_LO) == pKernelBif->cacheData.gpuBootConfigSpace[5]) &&
        (osPciReadDword(handle, NV_XVE_BAR1_HI) == pKernelBif->cacheData.gpuBootConfigSpace[6]))
    {
        // BAR1 is valid
        return NV_TRUE;
    }

    return NV_FALSE;
}

/*!
 * @brief Try restoring BAR registers and command register using config cycles
 *
 * @param[in] pGpu          GPU object pointer
 * @param[in] pKernelBif    KernelBif object pointer
 *
 * @returns    NV_OK on success
 *             NV_ERR_INVALID_READ if the register read returns unexpected value
 */
NV_STATUS
kbifRestoreBarsAndCommand_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 domain = gpuGetDomain(pGpu);
    NvU8 bus = gpuGetBus(pGpu);
    NvU8 device = gpuGetDevice(pGpu);
    NvU16 vendorId, deviceId;
    void *handle;

    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);

    osPciWriteDword(handle, NV_XVE_BAR0, pKernelBif->cacheData.gpuBootConfigSpace[4]);
    osPciWriteDword(handle, NV_XVE_BAR1_LO, pKernelBif->cacheData.gpuBootConfigSpace[5]);
    osPciWriteDword(handle, NV_XVE_BAR1_HI, pKernelBif->cacheData.gpuBootConfigSpace[6]);
    osPciWriteDword(handle, NV_XVE_BAR2_LO, pKernelBif->cacheData.gpuBootConfigSpace[7]);
    osPciWriteDword(handle, NV_XVE_BAR2_HI, pKernelBif->cacheData.gpuBootConfigSpace[8]);
    osPciWriteDword(handle, NV_XVE_BAR3, pKernelBif->cacheData.gpuBootConfigSpace[9]);
    osPciWriteDword(handle, NV_XVE_DEV_CTRL, pKernelBif->cacheData.gpuBootConfigSpace[1]);

    if (GPU_REG_RD32(pGpu, NV_PMC_BOOT_0) != pGpu->chipId0)
    {
        return NV_ERR_INVALID_READ;
    }

    return NV_OK;
}

/*
*!
 * @brief  Do any operations to get ready for a XVE sw reset.
 *
 * @param[in]  pGpu     GPU object pointer
 * @param[in]  pBif     BIF object pointer
 *
 * @return  NV_STATUS
 */
NV_STATUS
kbifPrepareForXveReset_GM107
(
    OBJGPU *    pGpu,
    KernelBif *pKernelBif
)
{
    NV_STATUS status;

    return status = NV_OK;
}


/*!
 * @brief  Do any work necessary to be able to do a full chip reset.
 *
 * This code needs to accomplish these objectives:
 *
 *  - Perform any operations needed to make sure that the HW will not glitch or
 *  otherwise fail after the reset.  This includes all potential WARs.
 *  - Perform any operations needed to make sure the devinit scripts can be
 *  executed properly after the reset.
 *  - Modify any SW state that could potentially cause errors during/after the reset.
 *
 * @param[in] pGpu       GPU object pointer
 * @param[in] pKernelBif KernelBif object pointer
 *
 * @return  None
 */
void
kbifPrepareForFullChipReset_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    KernelBus    *pKernelBus                = GPU_GET_KERNEL_BUS(pGpu);
    KernelMc     *pKernelMc                 = GPU_GET_KERNEL_MC(pGpu);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    KernelNvlink *pKernelNvlink             = GPU_GET_KERNEL_NVLINK(pGpu);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_PREPARING_FULLCHIP_RESET, NV_TRUE);

    //
    // This function must be called before the pmu detach since engines need to
    // be powered-up before we detach the PMU, and power-up sequence requires PMU
    //
    if (kmcPrepareForXVEReset_HAL(pGpu, pKernelMc) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "MC prepare for XVE reset failed.\n");

        DBG_BREAKPOINT();
    }

    // Set FB_IFACE to disable
    if (kmemsysPrepareForXVEReset_HAL(pGpu, pKernelMemorySystem) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "FB_IFACE disable for fullchip reset failed.\n");

        DBG_BREAKPOINT();
    }

    // Disable P2P on all GPUs before RESET,
    if (kbusPrepareForXVEReset_HAL(pGpu, pKernelBus) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "BUS prepare for devinit failed.\n");

        DBG_BREAKPOINT();
    }

    // Disable any NVLinks connected to this GPU
    if ((pKernelNvlink!= NULL) && pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENABLED))
    {
        if (knvlinkPrepareForXVEReset(pGpu, pKernelNvlink, NV_FALSE) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "NVLINK prepare for fullchip reset failed.\n");

            DBG_BREAKPOINT();
        }
    }

    if (kbifPrepareForXveReset_HAL(pGpu, pKernelBif) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "BIF prepare for devinit failed.\n");

        DBG_BREAKPOINT();
    }

    pGpu->setProperty(pGpu, PDB_PROP_GPU_PREPARING_FULLCHIP_RESET, NV_FALSE);
}


/*!
 * @brief HAL specific BIF software state initialization
 *
 * @param[in] pGpu       GPU object pointer
 * @param[in] pKernelBif KernelBif object pointer
 *
 * @return    NV_OK on success
 */
NV_STATUS
kbifInit_GM107
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    // Cache the offsets of BAR registers into an array for subsequent use
    kbifStoreBarRegOffsets_HAL(pGpu, pKernelBif, NV_XVE_BAR0);

    return NV_OK;
}

/*!
 *  @brief Get the migration bandwidth
 *
 *  @param[out]     pBandwidth  Migration bandwidth
 *
 *  @returns        NV_STATUS
 */
NV_STATUS
kbifGetMigrationBandwidth_GM107
(
    OBJGPU        *pGpu,
    KernelBif     *pKernelBif,
    NvU32         *pBandwidth
)
{
    NV_STATUS rmStatus = NV_OK;
    NV2080_CTRL_BUS_INFO busInfo = {0};

    NvU32 pcieLinkRate    = 0;
    NvU32 lanes           = 0;
    NvU32 pciLinkMaxSpeed = 0;
    NvU32 pciLinkGenInfo  = 0;

    busInfo.index = NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN_INFO;
    busInfo.data = 0;

    if (IS_GSP_CLIENT(pGpu))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kbusSendBusInfo(pGpu, GPU_GET_KERNEL_BUS(pGpu), &busInfo));
    }
    else
    {
        if (kbifIsPciBusFamily(pKernelBif))
        {
            NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kbifControlGetPCIEInfo(pGpu, pKernelBif, &busInfo));
        }
        else
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    pciLinkGenInfo = DRF_VAL(2080, _CTRL_BUS, _INFO_PCIE_LINK_CAP_GEN, busInfo.data);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kbifGetPciLinkMaxSpeedByPciGenInfo(pGpu, pKernelBif, pciLinkGenInfo, &pciLinkMaxSpeed));

    busInfo.index = NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CTRL_STATUS;
    busInfo.data = 0;

    if (kbifIsPciBusFamily(pKernelBif))
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kbifControlGetPCIEInfo(pGpu, pKernelBif, &busInfo));
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    lanes = DRF_VAL(2080, _CTRL_BUS, _INFO_PCIE_LINK_CTRL_STATUS_LINK_WIDTH, busInfo.data);
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, calculatePCIELinkRateMBps(lanes, pciLinkMaxSpeed, &pcieLinkRate));
    *pBandwidth = (pcieLinkRate / VGPU_MIGRATION_API_DERATE_FACTOR);

    return rmStatus;
}

/*!
 * @brief Destructor
 *
 * @param[in] pKernelBif
 *
 * @returns void
 */
void
kbifDestruct_GM107
(
    KernelBif *pKernelBif
)
{
    portMemFree(pKernelBif->xveRegmapRef[0].bufMsixTable);
    pKernelBif->xveRegmapRef[0].bufMsixTable = NULL;
}




/*!
 * @brief  Reset the chip.
 *
 * Use the XVE sw reset logic to reset as much of the chip as possible.
 *
 * @param[in]  pGpu       GPU object pointer
 * @param[in]  pKernelBif KernelBif object pointer
 *
 * @return  NV_STATUS
 */
NV_STATUS
kbifDoFullChipReset_GM107
(
    OBJGPU *pGpu,
    KernelBif  *pKernelBif
)
{
    OBJSYS *pSys  = SYS_GET_INSTANCE();
    OBJCL *pCl = SYS_GET_CL(pSys);
    NvU32  tempRegVal;
    NvU32  oldPmc, newPmc;
    NV_STATUS status;

    // First Reset PMC
    oldPmc = GPU_REG_RD32(pGpu, NV_PMC_ENABLE);
    newPmc = oldPmc;

    newPmc &= ~(DRF_DEF(_PMC, _ENABLE, _MSPPP, _ENABLE) |
                DRF_DEF(_PMC, _ENABLE, _PMEDIA, _ENABLED) |
                DRF_DEF(_PMC, _ENABLE, _CE0, _ENABLED) |
                DRF_DEF(_PMC, _ENABLE, _CE1, _ENABLED) |
                DRF_DEF(_PMC, _ENABLE, _PFIFO, _ENABLED) |
                DRF_DEF(_PMC, _ENABLE, _PGRAPH, _ENABLED) |
                DRF_DEF(_PMC, _ENABLE, _PWR, _ENABLED) |
                DRF_DEF(_PMC, _ENABLE, _MSVLD, _ENABLED) |
                DRF_DEF(_PMC, _ENABLE, _MSPDEC, _ENABLED) |
                DRF_DEF(_PMC, _ENABLE, _PDISP, _ENABLED));

    GPU_REG_WR32(pGpu, NV_PMC_ENABLE, newPmc);
    GPU_REG_RD32(pGpu, NV_PMC_ENABLE);
    GPU_REG_RD32(pGpu, NV_PMC_ENABLE);

    //
    // Before doing SW_RESET, init NV_XP_PL_CYA_1_BLOCK_HOST2XP_HOLD_LTSSM to 1.
    // else when NV_XVE_SW_RESET to 0, host will try to do rom init and will assert
    // HOST2XP_HOLD_LTSSM to 0 which will cause ltssm to goto detect.
    //
    tempRegVal = GPU_REG_RD32(pGpu, NV_XP_PL_CYA_1(0));
    do
    {
        tempRegVal |= DRF_NUM(_XP, _PL_CYA_1, _BLOCK_HOST2XP_HOLD_LTSSM, 1);
        GPU_REG_WR32(pGpu, NV_XP_PL_CYA_1(0), tempRegVal);
        tempRegVal = GPU_REG_RD32(pGpu, NV_XP_PL_CYA_1(0));
    } while ((tempRegVal & DRF_NUM(_XP, _PL_CYA_1, _BLOCK_HOST2XP_HOLD_LTSSM, 1)) == 0);

    NV_ASSERT_OK_OR_RETURN(GPU_BUS_CFG_RD32(pGpu, NV_XVE_SW_RESET, &tempRegVal));
    tempRegVal = FLD_SET_DRF(_XVE, _SW_RESET, _RESET, _ENABLE, tempRegVal);
    clPcieWriteDword(pCl, gpuGetDomain(pGpu), gpuGetBus(pGpu),
                     gpuGetDevice(pGpu), 0, NV_XVE_SW_RESET, tempRegVal);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET, NV_TRUE);

    // wait a bit to make sure GPU is reset
    osDelay(1);

    //
    // Come out of reset. Note that when SW/full-chip reset is triggered by the
    // above write to NV_XVE_SW_RESET, BAR0 priv writes do not work and thus
    // this write must be a PCI config bus write.
    //
    tempRegVal = FLD_SET_DRF(_XVE, _SW_RESET, _RESET, _DISABLE, tempRegVal);
    clPcieWriteDword(pCl, gpuGetDomain(pGpu), gpuGetBus(pGpu),
                     gpuGetDevice(pGpu), 0, NV_XVE_SW_RESET, tempRegVal);


    //
    // When bug 1511451 is present, SW_RESET will clear BAR3, and IO accesses
    // will fail when legacy VBIOS is called. Apply the related SW WAR now.
    //
    status = kbifApplyWarForBug1511451_HAL(pGpu, pKernelBif);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed while applying WAR for Bug 1511451\n");
        NV_ASSERT(0);
    }

    return status;
}

NV_STATUS
kbifApplyWarForBug1511451_GM107
(
    OBJGPU    *pGpu,
    KernelBif  *pKernelBif
)
{
    NvU32      domain = gpuGetDomain(pGpu);
    NvU8       bus    = gpuGetBus(pGpu);
    NvU8       device = gpuGetDevice(pGpu);
    NvU16      vendorId;
    NvU16      deviceId;
    void      *handle;

    // If SBR is not supported, BAR3 will have not been saved and we are at risk.
    if (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_SECONDARY_BUS_RESET_SUPPORTED))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "SBR not supported so saved BAR3 is not valid, skipping restore!!!\n");
        return NV_ERR_INVALID_STATE;
    }

    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);

    osPciWriteDword(handle, NV_XVE_BAR3, pKernelBif->cacheData.gpuBootConfigSpace[NV_XVE_BAR3/sizeof(NvU32)]);

    return NV_OK;
}

/**
 *
 * @brief : This HAL always issues SBR
 *
 */
NV_STATUS
kbifDoSecondaryBusResetOrFunctionLevelReset_GM107
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    // Since FLR is only supported for Turing and later, we simply issue SBR here
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_SECONDARY_BUS_RESET, NV_TRUE);
    return kbifDoSecondaryBusHotReset_HAL(pGpu, pKernelBif);
}


NV_STATUS
kbifDoSecondaryBusHotReset_GM107
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    {
        OBJCL  *pCl                 = SYS_GET_CL(SYS_GET_INSTANCE());
        NBADDR *pUpstreamPort       = &pGpu->gpuClData.upstreamPort.addr;
        void   *pUpstreamPortHandle = NULL;
        NvU16   portVendorId;
        NvU16   portDeviceId;
        NvU16   data;

        pUpstreamPortHandle = osPciInitHandle(pUpstreamPort->domain,
                                              pUpstreamPort->bus,
                                              pUpstreamPort->device,
                                              0,
                                              &portVendorId,
                                              &portDeviceId);

        //
        // Set secondary bus reset bit, triggering a hot reset on the port the
        // GPU is plugged into.
        //
        data = osPciReadWord(pUpstreamPortHandle, PCI_HEADER_TYPE1_BRIDGE_CONTROL);
        clPcieWriteWord(pCl,
                        pGpu->gpuClData.upstreamPort.addr.domain,
                        pGpu->gpuClData.upstreamPort.addr.bus,
                        pGpu->gpuClData.upstreamPort.addr.device,
                        pGpu->gpuClData.upstreamPort.addr.func,
                        PCI_HEADER_TYPE1_BRIDGE_CONTROL,
                        (data | 0x40));

        //
        // We must must ensure a minimum reset duration (Trst) of 1ms, as defined
        // in the PCI Local Bus Specification, Revision 3.0
        //
        osDelayUs(1000);

        // Clear secondary bus reset bit, bringing the port out of hot reset.
        data &= 0xFFBF;
        clPcieWriteWord(pCl,
                        pGpu->gpuClData.upstreamPort.addr.domain,
                        pGpu->gpuClData.upstreamPort.addr.bus,
                        pGpu->gpuClData.upstreamPort.addr.device,
                        pGpu->gpuClData.upstreamPort.addr.func,
                        PCI_HEADER_TYPE1_BRIDGE_CONTROL,
                        data);
        //
        // We must wait at least 100 ms from the end of the reset before it is
        // permitted to issue Configuration Requests to the GPU.
        //
        osDelayUs(100000);
    }

    return kbifWaitForConfigAccessAfterReset(pGpu, pKernelBif);
}



