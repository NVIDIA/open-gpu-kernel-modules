/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/bif/kernel_bif.h"
#include "published/ampere/ga100/dev_nv_xve_addendum.h"
#include "published/ampere/ga100/dev_nv_xve.h"
#include "published/ampere/ga100/dev_boot.h"

/* ------------------------ Public Functions -------------------------------- */

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
