/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/* ------------------------ Includes ---------------------------------------- */
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/fifo/kernel_fifo.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "published/ampere/ga100/dev_boot.h"
#include "kernel/gpu/mc/kernel_mc.h"
#include "platform/chipset/chipset.h"
#include "published/ampere/ga100/dev_nv_xve.h"
#include "published/ampere/ga100/dev_nv_xve_addendum.h"
#include "published/ampere/ga100/dev_nv_pcfg_xve_regmap.h"
#include "nvmisc.h"

/* ------------------------ Public Functions -------------------------------- */

// XVE register map for PCIe config space
static const NvU32 xveRegMapValid[] = NV_PCFG_XVE_REGISTER_VALID_MAP;
static const NvU32 xveRegMapWrite[] = NV_PCFG_XVE_REGISTER_WR_MAP;

/* ------------------------ Public Functions -------------------------------- */

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
kbifInitXveRegMap_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU8       func
)
{
    extern NvU32 kbifInitXveRegMap_GM107(OBJGPU *pGpu, KernelBif *pKernelBif, NvU8 func);
    NV_STATUS  status      = NV_OK;
    NvU32      controlSize = 0;

    if (func == 0)
    {
        pKernelBif->xveRegmapRef[0].nFunc              = 0;
        pKernelBif->xveRegmapRef[0].xveRegMapValid     = xveRegMapValid;
        pKernelBif->xveRegmapRef[0].xveRegMapWrite     = xveRegMapWrite;
        pKernelBif->xveRegmapRef[0].numXveRegMapValid  = NV_ARRAY_ELEMENTS(xveRegMapValid);
        pKernelBif->xveRegmapRef[0].numXveRegMapWrite  = NV_ARRAY_ELEMENTS(xveRegMapWrite);
        pKernelBif->xveRegmapRef[0].bufBootConfigSpace = pKernelBif->cacheData.gpuBootConfigSpace;
        // Each MSIX table entry is 4 NvU32s
        controlSize = kbifGetMSIXTableVectorControlSize_HAL(pGpu, pKernelBif);
        if (pKernelBif->xveRegmapRef[0].bufMsixTable == NULL)
            pKernelBif->xveRegmapRef[0].bufMsixTable = portMemAllocNonPaged(controlSize * 4 * sizeof(NvU32));
        NV_ASSERT_OR_RETURN(pKernelBif->xveRegmapRef[0].bufMsixTable != NULL, NV_ERR_NO_MEMORY);
    }
    else if (func == 1)
    {
        // Init regmap for Fn1 using older HAL
        status = kbifInitXveRegMap_GM107(pGpu, pKernelBif, 1);
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid argument, func: %d.\n", func);
        NV_ASSERT(0);
        status = NV_ERR_INVALID_ARGUMENT;
    }

    return status;
}

/*!
 * @brief Apply WAR for bug 3208922 - disable P2P on Ampere NB
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 */
void
kbifApplyWARBug3208922_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    if (IsMobile(pGpu))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED,  NV_TRUE);
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED, NV_TRUE);
    }
}

/*!
 * @brief Check for RO enablement request in emulated config space.
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  BIF object pointer
 */
void
kbifInitRelaxedOrderingFromEmulatedConfigSpace_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 passthroughEmulatedConfig = osPciReadDword(osPciInitHandle(gpuGetDomain(pGpu),
                                                                     gpuGetBus(pGpu),
                                                                     gpuGetDevice(pGpu),
                                                                     0, NULL, NULL),
                                                     NV_XVE_PASSTHROUGH_EMULATED_CONFIG);
    NvBool roEnabled = DRF_VAL(_XVE, _PASSTHROUGH_EMULATED_CONFIG, _RELAXED_ORDERING_ENABLE, passthroughEmulatedConfig);

    pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE, roEnabled);
}

/*!
 * @brief Check and cache 64b BAR0 support
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 */
void
kbifCache64bBar0Support_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 regVal = 0;

    // Read config register
    if (GPU_BUS_CFG_RD32(pGpu, NV_XVE_DBG_CYA_0,
                         &regVal) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read NV_XVE_DBG_CYA_0\n");
        return;
    }

    // Check if 64b BAR0 is supported
    if (FLD_TEST_DRF(_XVE, _DBG_CYA_0, _BAR0_ADDR_WIDTH,
                     _64BIT, regVal))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED, NV_TRUE);
    }
}

/*!
 * @brief Restore the BAR0 register from the given config space buffer
 * BAR0 register restore has to use the config cycle write.
 *
 * @param[in] pGpu              GPU object pointer
 * @param[in] pKernelBif        Pointer to KernelBif object
 * @param[in] handle            PCI handle for GPU
 * @param[in] bufConfigSpace    Stored config space
 */
void
kbifRestoreBar0_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    void      *handle,
    NvU32     *bufConfigSpace
)
{
    NvU32      bar0LoRegOffset;
    NvU32      bar0HiRegOffset;
    NvU32     *pBarRegOffsets = pKernelBif->barRegOffsets;

    if (pBarRegOffsets == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "pBarRegOffsets is NULL!\n");
        NV_ASSERT(pBarRegOffsets);
        return;
    }

    bar0LoRegOffset = pBarRegOffsets[NV_XVE_BAR0_LO_INDEX];
    bar0HiRegOffset = pBarRegOffsets[NV_XVE_BAR0_HI_INDEX];

    osPciWriteDword(handle, bar0LoRegOffset,
                    bufConfigSpace[bar0LoRegOffset/sizeof(NvU32)]);
    if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED))
    {
        osPciWriteDword(handle, bar0HiRegOffset,
                        bufConfigSpace[bar0HiRegOffset/sizeof(NvU32)]);
    }
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
kbifAnyBarsAreValid_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU8       bus    = gpuGetBus(pGpu);
    NvU8       device = gpuGetDevice(pGpu);
    NvU32      domain = gpuGetDomain(pGpu);
    NvU16      vendorId;
    NvU16      deviceId;
    void      *handle;
    NvU32      bar0LoRegOffset;
    NvU32      bar0HiRegOffset;
    NvU32      bar1LoRegOffset;
    NvU32      bar1HiRegOffset;
    NvU32     *pBarRegOffsets = pKernelBif->barRegOffsets;

    if (pBarRegOffsets == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "pBarRegOffsets is NULL!\n");
        NV_ASSERT(pBarRegOffsets);
        return NV_FALSE;
    }

    bar0LoRegOffset = pBarRegOffsets[NV_XVE_BAR0_LO_INDEX];
    bar0HiRegOffset = pBarRegOffsets[NV_XVE_BAR0_HI_INDEX];
    bar1LoRegOffset = pBarRegOffsets[NV_XVE_BAR1_LO_INDEX];
    bar1HiRegOffset = pBarRegOffsets[NV_XVE_BAR1_HI_INDEX];

    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);

    // BAR0_HI register is valid only if 64b BAR0 is enabled
    if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED))
    {
        if ((osPciReadDword(handle, bar0LoRegOffset) ==
             pKernelBif->cacheData.gpuBootConfigSpace[bar0LoRegOffset/sizeof(NvU32)]) &&
            (osPciReadDword(handle, bar0HiRegOffset) ==
             pKernelBif->cacheData.gpuBootConfigSpace[bar0HiRegOffset/sizeof(NvU32)]))
        {
            // BAR0 is valid
            return NV_TRUE;
        }
    }
    else
    {
        if (osPciReadDword(handle, bar0LoRegOffset) ==
            pKernelBif->cacheData.gpuBootConfigSpace[bar0LoRegOffset/sizeof(NvU32)])
        {
            // BAR0 is valid
            return NV_TRUE;
        }
    }

    // BAR1_LO and BAR1_HI registers are valid even if 64b BAR0 is disabled
    if ((osPciReadDword(handle, bar1LoRegOffset) ==
         pKernelBif->cacheData.gpuBootConfigSpace[bar1LoRegOffset/sizeof(NvU32)]) &&
        (osPciReadDword(handle, bar1HiRegOffset) ==
         pKernelBif->cacheData.gpuBootConfigSpace[bar1HiRegOffset/sizeof(NvU32)]))
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
kbifRestoreBarsAndCommand_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    void   *handle;
    NvU8   bus      = gpuGetBus(pGpu);
    NvU8   device   = gpuGetDevice(pGpu);
    NvU32  domain   = gpuGetDomain(pGpu);
    NvU16  vendorId;
    NvU16  deviceId;
    NvU32 *pBarRegOffsets = pKernelBif->barRegOffsets;
    NvU32  i;

    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);

    // Restore all BAR registers
    for (i = 0; i < KBIF_NUM_BAR_OFFSET_ENTRIES; i++)
    {
        if (pBarRegOffsets[i] != KBIF_INVALID_BAR_REG_OFFSET)
        {
            osPciWriteDword(handle, pBarRegOffsets[i],
                            pKernelBif->cacheData.gpuBootConfigSpace[pBarRegOffsets[i]/sizeof(NvU32)]);
        }
    }

    // Restore Device Control register
    osPciWriteDword(handle, NV_XVE_DEV_CTRL,
                    pKernelBif->cacheData.gpuBootConfigSpace[NV_XVE_DEV_CTRL/sizeof(NvU32)]);

    if (GPU_REG_RD32(pGpu, NV_PMC_BOOT_0) != pGpu->chipId0)
    {
        return NV_ERR_INVALID_READ;
    }

    return NV_OK;
}


/*!
 * @brief Store config space offsets of BAR registers into the KernelBif object.
 * Keeping this sub-routine chip specific since NV_XVE_BAR*_LO/HI_INDEX could
 * be changed for future chips
 *
 * @param[in] pGpu              GPU object pointer
 * @param[in] pKernelBif        KernelBif object pointer
 * @param[in] firstBarRegOffset Offset of first BAR register into the config space
 */
void
kbifStoreBarRegOffsets_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      firstBarRegOffset
)
{
    NvU32      i;
    NvU32      currOffset;
    NvU32     *pBarRegOffsets = pKernelBif->barRegOffsets;


    //
    // This array will have entries corresponding to
    // BAR0_LO, BAR0_HI, BAR1_LO, BAR1_HI, BAR2_LO, BAR2_HI and BAR3
    // If 64b BAR0 is NOT enabled, BAR0_HI register is non-existant
    // So for 64b BAR0 disabled case and if firstBarRegoffset is 0x10, this
    // regoffset array will be {0x10, 0xFFFF, 0x14, 0x18, 0x1C, 0x20, 0x24}
    // If 64b BAR0 is enabled, BAR3 register is non-existant. So in this case
    // assuming firstBarRegoffset of 0x10, regoffsetArray will be
    // {0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0xFFFF}
    //
    currOffset = firstBarRegOffset;
    for (i = 0; i < KBIF_NUM_BAR_OFFSET_ENTRIES; i++)
    {
        // If 64b BAR0 is NOT enabled, BAR0_HI register is non-existant
        if ((i == NV_XVE_BAR0_HI_INDEX) &&
            pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED))
        {
            pBarRegOffsets[i] = KBIF_INVALID_BAR_REG_OFFSET;
            continue;
        }

        // If 64b BAR0 is enabled, BAR3 register is non-existant
        if ((i == NV_XVE_BAR3_INDEX) && 
            pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_64BIT_BAR0_SUPPORTED))
        {
            pBarRegOffsets[i] = KBIF_INVALID_BAR_REG_OFFSET;
            continue;
        }

        pBarRegOffsets[i] = currOffset;
        currOffset        = currOffset + 4;
    }
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
 * @param[in]  pGpu       GPU object pointer
 * @param[in]  pKernelBif KernelBif object pointer
 *
 * @return  None
 */
void
kbifPrepareForFullChipReset_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    KernelBus    *pKernelBus    = GPU_GET_KERNEL_BUS(pGpu);
    KernelMc     *pKernelMc     = GPU_GET_KERNEL_MC(pGpu);
    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    OBJSYS       *pSys          = SYS_GET_INSTANCE();
    OBJCL        *pCl           = SYS_GET_CL(pSys);

    NvBool bIsFLRSupportedAndEnabled;
    NvU32  oldPmc, oldPmcDevice;

    bIsFLRSupportedAndEnabled = (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FLR_SUPPORTED)) &&
                                !(pKernelBif->bForceDisableFLR);

    // If FLR is supported, we must always shut down any NVLinks connected to this GPU
    if (bIsFLRSupportedAndEnabled && pKernelNvlink &&
        pKernelNvlink->getProperty(pKernelNvlink, PDB_PROP_KNVLINK_ENABLED))
    {
        if (knvlinkPrepareForXVEReset(pGpu, pKernelNvlink, NV_FALSE) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "NVLINK prepare for fullchip reset failed.\n");

            DBG_BREAKPOINT();
        }
    }

    //
    // FLR/SW_RESET pre-conditioning is required for Turing and earlier chips.
    // For Ampere+ chips, it is only required if p2p mailbox support is enabled
    // Note that for SW_RESET, we use pre-conditioning in all cases
    //
    if ((gpumgrGetGpuLinkCount(pGpu->gpuInstance) > 0) && (kbusIsP2pInitialized(pKernelBus)))
    {
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED, NV_TRUE);
    }

    //
    // For GA100+ chips we don't need any pre-conditioning WARs for PF-FLR besides NVLink if
    // p2p mailbox support is not enabled. We still use this pre-conditioning for
    // SW RESET
    //
    if (bIsFLRSupportedAndEnabled &&
        (!(pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FLR_PRE_CONDITIONING_REQUIRED))))
    {
        goto _bifPrepareForFullChipReset_GA100_exit;
    }

    pGpu->setProperty(pGpu, PDB_PROP_GPU_PREPARING_FULLCHIP_RESET, NV_TRUE);

    // Disable P2P on all GPUs before RESET,
    if (kbusPrepareForXVEReset_HAL(pGpu, pKernelBus) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "BUS prepare for devinit failed.\n");

        DBG_BREAKPOINT();
    }

    if (kbifPrepareForXveReset_HAL(pGpu, pKernelBif) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "BIF prepare for devinit failed.\n");

        DBG_BREAKPOINT();
    }

    // First Reset PMC
    oldPmc       = GPU_REG_RD32(pGpu, NV_PMC_ENABLE);
    oldPmcDevice = GPU_REG_RD32(pGpu, NV_PMC_DEVICE_ENABLE(0));
    kbifResetHostEngines_HAL(pGpu, pKernelBif, pKernelMc);

    if (!pCl->getProperty(pCl, PDB_PROP_CL_PCIE_CONFIG_ACCESSIBLE)
    )
    {
        //
        // We can not issue SW_RESET or Function Level Reset(FLR) as we are not able to access PCI
        // config space. Now we have disabled engines, please re-enable them before
        // tearing down RM. To be safe, do it right here.
        //
        GPU_REG_WR32(pGpu, NV_PMC_ENABLE, oldPmc);
        GPU_REG_WR32(pGpu, NV_PMC_DEVICE_ENABLE(0), oldPmcDevice);
    }

_bifPrepareForFullChipReset_GA100_exit:
    pGpu->setProperty(pGpu, PDB_PROP_GPU_PREPARING_FULLCHIP_RESET, NV_FALSE);
}

/*!
 * @brief  Reset host engines in PMC_ENABLE and PMC_DEVICE_ENABLE.
 *
 * @param[in]  pGpu       The GPU object
 * @param[in]  pKernelBif KernelBif object pointer
 */
void
kbifResetHostEngines_GA100
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif,
    KernelMc  *pKernelMc
)
{
    NvU32 engineMask;

    // First Reset engines in NV_PMC_ENABLE
    engineMask = kbifGetValidEnginesToReset_HAL(pGpu, pKernelBif);
    if (engineMask)
    {
        NV_ASSERT_OK(kmcWritePmcEnableReg_HAL(pGpu, pKernelMc, engineMask, NV_FALSE, NV_FALSE));
    }

    //
    // Reset engines in NV_PMC_DEVICE_ENABLE. For Ampere and later chips,
    // host engines have moved from NV_PMC_ENABLE to NV_PMC_DEVICE_ENABLE,
    // hence we need to ensure that engines in the NV_PMC_DEVICE_ENABLE are
    // reset too.
    //
    engineMask = kbifGetValidDeviceEnginesToReset_HAL(pGpu, pKernelBif);
    if (engineMask)
    {
        NV_ASSERT_OK(kmcWritePmcEnableReg_HAL(pGpu, pKernelMc, engineMask, NV_FALSE,
            gpuIsUsePmcDeviceEnableForHostEngineEnabled(pGpu)));
    }
}

/*!
 * @brief  Get the NV_PMC_ENABLE bit of the valid Engines to reset.
 *
 * @param[in]  pGpu       The GPU object
 * @param[in]  pKernelBif KernelBif object pointer
 * 
 * @return All valid engines in NV_PMC_ENABLE.
 */
NvU32
kbifGetValidEnginesToReset_GA100
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    return (DRF_DEF(_PMC, _ENABLE, _PDISP,   _ENABLED) |
            DRF_DEF(_PMC, _ENABLE, _PERFMON, _ENABLED));
}


/*!
 * @brief  Get the NV_PMC_DEVICE_ENABLE bit of the valid Engines to reset. Gets this
 * data by reading the device info table and returns engines with valid reset_id.
 *
 * @param[in]  pGpu       The GPU object
 * @param[in]  pKernelBif KernelBif object pointer
 *
 * @return All valid engines in NV_PMC_DEVICE_ENABLE.
 */
NvU32
kbifGetValidDeviceEnginesToReset_GA100
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    const NvU32 numEngines = kfifoGetNumEngines_HAL(pGpu, pKernelFifo);
    NV_STATUS status;
    NvU32 engineID;
    NvU32 regVal = 0;
    NvU32 resetIdx;

    //
    // If hardware increases the size of this register in future chips, we would
    // need to catch this and fork another HAL.
    // Already obsoleted by GH100, but keeping the check
    //
    ct_assert(NV_PMC_DEVICE_ENABLE__SIZE_1 <= 1);

    for (engineID = 0; engineID < numEngines; engineID++)
    {
        status = kfifoEngineInfoXlate_HAL(pGpu, pKernelFifo, ENGINE_INFO_TYPE_INVALID,
                    engineID, ENGINE_INFO_TYPE_RESET, &resetIdx);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Unable to get Reset index for engine ID (%u)\n",
                      engineID);
            continue;
        }

        // We got the resetIdx. Lets set the bit in NV_PMC_DEVICE_ENABLE.
        regVal = FLD_IDX_SET_DRF(_PMC, _DEVICE_ENABLE, _STATUS_BIT, resetIdx, _ENABLE, regVal);
    }

    return regVal;
}

/*!
 *  @brief Get the migration bandwidth
 *
 *  @param[out]     pBandwidth  Migration bandwidth
 *
 *  @returns        NV_STATUS
 */
NV_STATUS
kbifGetMigrationBandwidth_GA100
(
    OBJGPU        *pGpu,
    KernelBif     *pKernelBif,
    NvU32         *pBandwidth
)
{
    // Migration bandwidth in MegaBytes/second
    *pBandwidth = 500;

    return NV_OK;
}

/*!
 * @brief  Reset the chip.
 *
 * Use Function Level Reset(FLR) to reset as much of the chip as possible.
 * If FLR is not supported use the XVE sw reset logic .
 *
 * @param[in]  pGpu       GPU object pointer
 * @param[in]  pKernelBif KernelBif  object pointer
 *
 * @return  NV_STATUS
 */
NV_STATUS
kbifDoFullChipReset_GA100
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    NV_STATUS  status     = NV_OK;
    NvBool     bIsFLRSupportedAndEnabled;

    //
    // We support FLR for SKUs which are FLR capable.
    // Also check if we want to enforce legacy reset behavior by disabling FLR
    //
    bIsFLRSupportedAndEnabled = (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_FLR_SUPPORTED)) &&
                                !(pKernelBif->bForceDisableFLR);

    //
    // If FLR is supported, issue FLR otherwise issue SW_RESET
    // There is no point in issuing SW_RESET if FLR fails because both of them reset Fn0
    // except that FLR resets XVE unit as well
    //
    if (bIsFLRSupportedAndEnabled)
    {
        NV_ASSERT_OK(status = kbifDoFunctionLevelReset_HAL(pGpu, pKernelBif));
    }
    else
    {
         NV_PRINTF(LEVEL_ERROR, "FLR is either not supported or is disabled.\n");
    }
    pGpu->setProperty(pGpu, PDB_PROP_GPU_IN_FULLCHIP_RESET, NV_TRUE);

    // execute rest of the sequence for SW_RESET only if we have not issued FLR above

    return status;
}


/*!
 * @brief For function 0, clear 'outstanding downstream host reads' counter
 *        using config cycles
 *
 * @param[in] pGpu          GPU object pointer
 * @param[in] pKernelBif    KernelBif object pointer
 */
void
kbifClearDownstreamReadCounter_GA100
(
    OBJGPU *pGpu,
    KernelBif *pKernelBif
)
{
    void   *handle;
    NvU32  tempRegVal;
    NvU16  vendorId;
    NvU16  deviceId;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL *pCl    = SYS_GET_CL(pSys);
    NvU32  domain = gpuGetDomain(pGpu);
    NvU8   bus    = gpuGetBus(pGpu);
    NvU8   device = gpuGetDevice(pGpu);

    //
    // Use config cycles(and not BAR0 transactions) to reset the counter.
    // Currently, this function is called after FLR and before rmInit sequence.
    // After FLR or in fact after most resets we don't use the standard macro
    // to read/write a register(until rmInit sequence is complete).The macro
    // will not allow access when PDB_PROP_GPU_IS_LOST is true. This PDB is
    // required to keep other accesses from touching the GPU.
    //
    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);

    tempRegVal = osPciReadDword(handle, NV_XVE_DBG0);
    tempRegVal = FLD_SET_DRF(_XVE, _DBG0,
                             _OUTSTANDING_DOWNSTREAM_READ_CNTR_RESET, _TRIGGER,
                             tempRegVal);

    //
    // After triggering the RESET bit, this does not wait for RESET to be
    // complete. It is upto the caller to decide if it really wants to wait
    // for the completion. For example, for FLR case, we do not want to waste
    // cycles on this - it's okay to assume that this write will go through.
    //
    clPcieWriteDword(pCl, gpuGetDomain(pGpu), gpuGetBus(pGpu),
                     gpuGetDevice(pGpu), 0, NV_XVE_DBG0, tempRegVal);
}

