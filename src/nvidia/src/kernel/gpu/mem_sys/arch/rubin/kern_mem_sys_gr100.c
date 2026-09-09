/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "core/locks.h"
#include "g_kern_mem_sys_nvoc.h"
#include "gpu/gpu.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/bus/kern_bus.h"

#include "nvtypes.h"
#include "published/rubin/gr100/hwproject.h"
#include "published/rubin/gr100/dev_ltc_zb.h"
#include "published/rubin/gr100/dev_fbpa.h"
#include "published/rubin/gr100/dev_hshub_zb.h"
#include "published/rubin/gr100/dev_vm.h"
#include "published/rubin/gr100/dev_top_zb.h"
#include "published/rubin/gr100/dev_pri_sysl2_ltc.h"

// MC FLA Ranges are 2MB aligned
#define MC_FLA_OFFSET_TABLE_RANGE_SHIFT  RM_PAGE_SHIFT_2M
// MC FLA offsets are 256B aligned
#define MC_FLA_OFFSET_TABLE_OFFSET_SHIFT 8
#define MC_FLA_OFFSET_TABLE_OFFSET_ALIGNMENT 256

NvU32
kmemsysGetL2EccDedCountRegAddr_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               fbpa,
    NvU32               subp
)
{
    return (NV_LTC_PRI_BASE + NV_PLTC_LTS0_L2_CACHE_ECC_UNCORRECTED_ERR_COUNT +
            (fbpa * NV_LTC_PRI_STRIDE) + (subp * NV_LTS_PRI_STRIDE));
}

NvU32
kmemsysGetEccDedCountRegAddr_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU32               fbpa,
    NvU32               subp
)
{
    return NV_PFB_FBPA_0_ECC_DED_COUNT(subp) + (fbpa * NV_FBPA_PRI_STRIDE);
}

/*!
 * @brief Validate the sysmemFlushBuffer val and assert
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns NV_STATUS - NV_OK if sysmemFlushBuffer is valid otherwise NV_ERR_INVALID_STATE
 */
NV_STATUS
kmemsysAssertSysmemFlushBufferValid_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32       regHshubEgPcieFlushSysmemAddrValHi = 0;
    NvU32       regHshubEgPcieFlushSysmemAddrValLo = 0;
    IoAperture *pHshub0IoAperture = kmemsysInitHshub0Aperture_HAL(pGpu, pKernelMemorySystem);
    NV_STATUS   status = NV_OK;
    NV_ASSERT_OR_RETURN(pHshub0IoAperture != NULL, NV_ERR_INVALID_POINTER);

    regHshubEgPcieFlushSysmemAddrValLo = REG_RD32(pHshub0IoAperture,
                                               NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO);
    regHshubEgPcieFlushSysmemAddrValHi = REG_RD32(pHshub0IoAperture,
                                               NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI);

    if (regHshubEgPcieFlushSysmemAddrValLo == 0 && regHshubEgPcieFlushSysmemAddrValHi == 0)
    {
        status = NV_ERR_INVALID_STATE;
    }

    kmemsysDestroyHshub0Aperture_HAL(pGpu, pKernelMemorySystem, pHshub0IoAperture);

    return status;
}

/*!
 * @brief Write the sysmemFlushBuffer val into the NV_PFB_FBHUB_BASE_PCIE_FLUSH_SYSMEM_ADDR register
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns void
 */
void
kmemsysProgramSysmemFlushBuffer_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32       alignedSysmemFlushBufferAddr   = 0x0;
    NvU32       alignedSysmemFlushBufferAddrHi = 0x0;
    NvU32       regValHi;
    NvU32       regValLo;
    IoAperture *pHshub0IoAperture = kmemsysInitHshub0Aperture_HAL(pGpu, pKernelMemorySystem);

    NV_ASSERT_OR_RETURN_VOID(pHshub0IoAperture != NULL);
    NV_ASSERT(pKernelMemorySystem->sysmemFlushBuffer != 0);

    // alignedSysmemFlushBufferAddr will have the lower 32 bits of the buffer address
    alignedSysmemFlushBufferAddr = NvU64_LO32(pKernelMemorySystem->sysmemFlushBuffer);

    // alignedSysmemFlushBufferAddrHi will have the upper 32 bits of the buffer address
    alignedSysmemFlushBufferAddrHi = NvU64_HI32(pKernelMemorySystem->sysmemFlushBuffer);

    // Assert when Sysmem Flush buffer has more than 52-bit address
    NV_ASSERT((alignedSysmemFlushBufferAddrHi & (~NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK)) == 0);

    alignedSysmemFlushBufferAddrHi &= NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI_ADR_MASK;

    regValHi = DRF_NUM(_PFB_HSHUB_ZB, _EG_PCIE_FLUSH_SYSMEM_ADDR_HI, _ADR, alignedSysmemFlushBufferAddrHi);
    regValLo = DRF_NUM(_PFB_HSHUB_ZB, _EG_PCIE_FLUSH_SYSMEM_ADDR_LO, _ADR, alignedSysmemFlushBufferAddr);

    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_HI, ((NvU32)regValHi));
    REG_WR32(pHshub0IoAperture, NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO, ((NvU32)regValLo));

    kmemsysDestroyHshub0Aperture_HAL(pGpu, pKernelMemorySystem, pHshub0IoAperture);
}

/*!
 * @brief Gets the address shift for the sysmem flush buffer address.
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns NvU32 - The number of bits the address needs to be shifted by
 */
NvU32
kmemsysGetFlushSysmemBufferAddrShift_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    return portUtilCountTrailingZeros32(NV_PFB_HSHUB_ZB_EG_PCIE_FLUSH_SYSMEM_ADDR_LO_ADR_MASK);
}

/*!
 * @brief Checks Non-PASID ATS support. On GR10x Non-PASID ATS is
 * supported only in C2C path.
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in[ pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns NvBool - Returns NV_TRUE if Non-PASID ATS is supported.
 */
NvBool
kmemsysIsNonPasidAtsSupported_GR100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);

    if (gpuIsSelfHosted(pGpu) &&
        (pKernelBif != NULL) &&
        pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP))
    {
        if (pKernelMemorySystem->nonPasIdAtsOverride.bEnabled)
        {
            return pKernelMemorySystem->nonPasIdAtsOverride.bValue;
        }
        else
        {
            return NV_TRUE;
        }
    }

    return NV_FALSE;
}

/*
 * @brief Allocate an entry in the MC FLA offset table
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem KernelMemorySystem pointer
 * @param[in] pParams             Parameters for the FLA attachment
 * @param[in] pFabricMemDesc      FLA memory descriptor for checking the 2MB guard page
 * @param[out] pOffsetTableIndex  Index in the table for the new entry
 *
 * @returns NV_STATUS - Status of the entry allocation or NV_OK if no allocation
 *                      was necessary
 */
NV_STATUS
kmemsysMcFlaOffsetTableAlloc_GR100
(
    OBJGPU                        *pGpu,
    KernelMemorySystem            *pKernelMemorySystem,
    NV00FD_CTRL_ATTACH_MEM_PARAMS *pParams,
    MEMORY_DESCRIPTOR             *pFabricMemDesc,
    MEMORY_DESCRIPTOR             *pPhysMemDesc,
    NvU8                          *pOffsetTableIndex
)
{
    NvU32 gpuMask;
    NvU32 tableInstance;
    NvU32 tableIndex;
    NvU32 offsetVal;
    NvU64 fabricLast2MB = memdescGetSize(pFabricMemDesc) - RM_PAGE_SIZE_2M;
    NvU64 baseAddr;
    NvU64 endAddr;
    NvU32 shiftedBase;
    NvU32 shiftedEnd;
    NvU32 subPageOffset = pParams->subPageOffset;

    NV_ASSERT_OR_RETURN(pPhysMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    if (subPageOffset == 0)
    {
        return NV_OK;
    }

    if (!NV_IS_ALIGNED64(pParams->offset, memdescGetPageSize(pPhysMemDesc, AT_GPU)))
    {
        NV_PRINTF(LEVEL_ERROR, "MC FLA mapping fabric offset must be aligned to physical page size\n");
        return NV_ERR_INVALID_OFFSET;
    }

    if (!NV_IS_ALIGNED(subPageOffset, MC_FLA_OFFSET_TABLE_OFFSET_ALIGNMENT))
    {
        NV_PRINTF(LEVEL_ERROR, "MC FLA mapping subPageOffset must be 256B aligned\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (subPageOffset >= RM_PAGE_SIZE_2M)
    {
        NV_PRINTF(LEVEL_ERROR, "MC FLA mapping subPageOffset must be less than 2MB\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!memdescGetContiguity(pFabricMemDesc, AT_GPU))
    {
        NV_PRINTF(LEVEL_ERROR, "MC FLA mapping must use a contiguous FLA allocation\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pParams->mapLength < RM_PAGE_SIZE_2M)
    {
        NV_PRINTF(LEVEL_ERROR, "MC FLA mapping mapLength must be at least 2MB\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Offset table entries are restricted from the last 2MB page of the fabric space to
    // avoid accessing off the end of the space when adding the subPageOffset.
    //
    if ((pParams->mapLength > fabricLast2MB) ||
        (pParams->offset > (fabricLast2MB - pParams->mapLength)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "MC FLA mappings using subPageOffset must not extend into the last 2MB of the FLA space\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    gpuMask = NVBIT(gpuGetInstance(pGpu));
    NV_ASSERT_OR_RETURN(rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask),
                        NV_ERR_INVALID_LOCK_STATE);

    // Find the first available tableIndex
    tableIndex = bitVectorCountTrailingZeros(&pKernelMemorySystem->mcFlaOffsetTableFreeEntries);
    NV_CHECK_OR_RETURN(LEVEL_INFO,
                       tableIndex <= NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_INDEX_INDEX_MAX,
                       NV_ERR_MC_FLA_OFFSET_TABLE_FULL);

    bitVectorClr(&pKernelMemorySystem->mcFlaOffsetTableFreeEntries, tableIndex);

    *pOffsetTableIndex = (NvU8) tableIndex;

    offsetVal = subPageOffset >> MC_FLA_OFFSET_TABLE_OFFSET_SHIFT;
    offsetVal |= DRF_NUM(_VIRTUAL_FUNCTION_PRIV, _IG_MC_FLA_ADDR_ALIGN_TABLE_OFFSET_256B_ALIGNED_VALID,
                         _VALID, 1);

    baseAddr = memdescGetPhysAddr(pFabricMemDesc, AT_GPU, pParams->offset);
    shiftedBase = (NvU32)(baseAddr >> MC_FLA_OFFSET_TABLE_RANGE_SHIFT);

    endAddr = baseAddr + pParams->mapLength;
    shiftedEnd = (NvU32)(endAddr >> MC_FLA_OFFSET_TABLE_RANGE_SHIFT);

    for (tableInstance = 0;
         tableInstance < NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_INDEX__SIZE_1;
         tableInstance++)
    {
        // Must be written in the order of index, start, end, offset
        GPU_VREG_WR32(pGpu,
                      NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_INDEX(tableInstance),
                      tableIndex);
        GPU_VREG_WR32(pGpu,
                      NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_START_ADDR_2MB_ALIGNED(tableInstance),
                      shiftedBase);
        GPU_VREG_WR32(pGpu,
                      NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_END_ADDR_2MB_ALIGNED(tableInstance),
                      shiftedEnd);
        GPU_VREG_WR32(pGpu,
                      NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_OFFSET_256B_ALIGNED_VALID(tableInstance),
                      offsetVal);
    }

    return NV_OK;
}

/*!
 * @brief Free an entry in the MC FLA offset table
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem KernelMemorySystem pointer
 * @param[in] offsetTableIndex    Index of the table entry to be removed
 *
 * @returns NV_STATUS - Status of the entry removal
 */
void
kmemsysMcFlaOffsetTableFree_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvU8                offsetTableIndex
)
{
    NvU32 tableInstance;
    NvU32 gpuMask = NVBIT(gpuGetInstance(pGpu));

    NV_ASSERT_OR_RETURN_VOID(rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask));

    if (offsetTableIndex > NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_INDEX_INDEX_MAX)
        return;

    NV_ASSERT_OR_RETURN_VOID(!bitVectorTest(&pKernelMemorySystem->mcFlaOffsetTableFreeEntries, offsetTableIndex));

    // Disable the offset table index on every table instance
    for (tableInstance = 0;
         tableInstance < NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_INDEX__SIZE_1;
         tableInstance++)
    {
        GPU_VREG_WR32(pGpu,
                      NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_INDEX(tableInstance),
                      offsetTableIndex);
        GPU_VREG_WR32(pGpu,
                      NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_START_ADDR_2MB_ALIGNED(tableInstance),
                      0);
        GPU_VREG_WR32(pGpu,
                      NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_END_ADDR_2MB_ALIGNED(tableInstance),
                      0);
        GPU_VREG_WR32(pGpu,
                      NV_VIRTUAL_FUNCTION_PRIV_IG_MC_FLA_ADDR_ALIGN_TABLE_OFFSET_256B_ALIGNED_VALID(tableInstance),
                      0);
    }
    bitVectorSet(&pKernelMemorySystem->mcFlaOffsetTableFreeEntries, offsetTableIndex);
}

/*!
 * @brief Writeback and invalidate device cache for system memory
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem KernelMemorySystem pointer
 *
 * @returns NV_STATUS - Status
 */
NV_STATUS
kmemsysWriteBackAndInvalidateSysL2_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    KernelBus *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NV_STATUS rmStatus;
    NvU32 reg;
    RMTIMEOUT timeout = {0, };

    reg = NV_VIRTUAL_FUNCTION_PRIV_FUNC_L2_SYSMEM_INVALIDATE;
    
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    rmStatus = kmemsysDoCacheOp_HAL(pGpu, pKernelMemorySystem, reg, 0, 0, &timeout);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to writeback & invalidate gpu L2 cache for sysmem. "
            "Status:0x%x\n", rmStatus);
        return rmStatus;
    }

    rmStatus = kbusSendSysmembarSingleWithXalUflush_HAL(pGpu, pKernelBus);
    if (rmStatus != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to send sysmembar after WB&I for sysmem. Status:0x%x\n",
            rmStatus);
    }

    return rmStatus;
}

/*!
 * Allocates and initializes SYS LTC Apertures.
 *
 * @param[in] pGpu                pointer for OBJGPU instance.
 * @param[in] pKernelMemorySystem pointer for KernelMemorySystem instance.
 * @return NV_ERR_NO_MEMORY       if memory allocation fails,
 *         NV_OK upon success.
 */
NV_STATUS
kmemsysInitializeSysLtcApertures_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
#define MAX_INSTANCES 6
    NvU32  devicePriBases[MAX_INSTANCES] = {0};
    NvU32 unitCount, unitIndex;
    NvU32 i;
    NV_STATUS status = NV_OK;

    NV_ASSERT_OK_OR_RETURN(gpuConstructDeviceInfoTable_HAL(pGpu));

    unitCount = 0;
    for (i = 0; i < pGpu->numDeviceInfoEntries; ++i)
    {
        const DEVICE_INFO_ENTRY *pEntry = &pGpu->pDeviceInfoTable[i];
        if (pEntry->typeEnum == NV_PTOP_ZB_DEVICE_INFO_DEV_TYPE_ENUM_SYSLTC)
        {
            NV_ASSERT_OR_ELSE(pEntry->instanceId < MAX_INSTANCES,
                NV_PRINTF(LEVEL_ERROR,
                    "Not enough instance slots available, please expand MAX_INSTANCES\n");
                status = NV_ERR_INVALID_STATE;
                return NV_ERR_INVALID_STATE);

            devicePriBases[unitCount] = pEntry->devicePriBase;
            NV_PRINTF(LEVEL_INFO, "SYS LTC IO aperture %u pribase 0x%x\n", unitCount, pEntry->devicePriBase);
            unitCount++;
        }
    }

    if (unitCount == 0)
    {
        pKernelMemorySystem->sysLtcApertureCount = 0;
        pKernelMemorySystem->pSysLtcApertures = NULL;
        pKernelMemorySystem->pSysLtcBcAperture =  NULL;
        return NV_OK;
    }

    pKernelMemorySystem->pSysLtcApertures = portMemAllocNonPaged(unitCount * (sizeof(IoAperture)));
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelMemorySystem->pSysLtcApertures != NULL, NV_ERR_NO_MEMORY);
    portMemSet(pKernelMemorySystem->pSysLtcApertures, 0, unitCount * (sizeof(IoAperture)));
    pKernelMemorySystem->sysLtcApertureCount = unitCount;

    for (unitIndex = 0; unitIndex < unitCount; unitIndex++)
    {
        NvU32 devicePriBase = devicePriBases[unitIndex];
        NV_ASSERT_OK_OR_GOTO(status,
                ioaprtInit(&(pKernelMemorySystem->pSysLtcApertures[unitIndex]),
                    pGpu->pIOApertures[DEVICE_INDEX_GPU],
                    devicePriBase, NV_LTC_PRI_STRIDE),
                failed);
    }

    if (unitCount > 0)
    {
        NV_ASSERT_OK_OR_GOTO(status,
            objCreate(&pKernelMemorySystem->pSysLtcBcAperture, staticCast(pKernelMemorySystem, Dynamic),
                      SwBcAperture, pKernelMemorySystem->pSysLtcApertures, pKernelMemorySystem->sysLtcApertureCount),
            failed);
    }

    NV_PRINTF(LEVEL_INFO, "SYS LTC IO aperture Count %u\n", pKernelMemorySystem->sysLtcApertureCount);

    return NV_OK;

failed:
    if (pKernelMemorySystem->pSysLtcBcAperture != NULL)
    {
        objDelete(pKernelMemorySystem->pSysLtcBcAperture);
        pKernelMemorySystem->pSysLtcBcAperture = NULL;
    }

    for (unitIndex = 0; unitIndex < unitCount; unitIndex++)
    {
        objDelete(&((pKernelMemorySystem->pSysLtcApertures[unitIndex])));
    }

    portMemFree(pKernelMemorySystem->pSysLtcApertures);
    pKernelMemorySystem->pSysLtcApertures = NULL;
    pKernelMemorySystem->sysLtcApertureCount = 0;

    return status;
}

/*!
 * @brief For Non-vCXL mode: Set or unset NV_PSYSL2LTC_LTSS_CBC_NUM_ACTIVE_LTCS_SYS_ALL_FORCE_VOL
 *           Set -> disable caching of sysmem in sysl2
 *           Unset -> enable caching of sysmem in sysl2
 *        For vCXL mode: routes to OS layer API to program sysmem caching via PCI config
 *
 * @param[in] pGpu                OBJGPU pointer
 * @param[in] pKernelMemorySystem KernelMemorySystem pointer
 * @param[in] bEnable             Enable/Disable flag
 */
void
kmemsysForceSetSysmemCaching_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvBool              bEnable
)
{
    NvU32 regVal;

    // vCXL case is handled by PCI config space register
    if (osGpuIsCxlDevice(pGpu))
    {
        osCxlSetCaching(pGpu, bEnable);
        return;
    }

    // skip for Non SelfHosted or Non Cache-Coherent systems
    if (!gpuIsSelfHosted(pGpu) || !pKernelMemorySystem->bIsSysl2CacheCoherent)
    {
        return;
    }

    regVal = REG_RD32(pKernelMemorySystem->pSysLtcBcAperture, NV_PSYSL2LTC_LTSS_CBC_NUM_ACTIVE_LTCS);

    if (bEnable)
    {
        regVal =  FLD_SET_DRF(_PSYSL2LTC, _LTSS_CBC_NUM_ACTIVE_LTCS, _SYS_ALL_FORCE_VOL, _DISABLE, regVal);
    }
    else
    {
        regVal =  FLD_SET_DRF(_PSYSL2LTC, _LTSS_CBC_NUM_ACTIVE_LTCS, _SYS_ALL_FORCE_VOL, _ENABLE, regVal);
    }

    REG_WR32(pKernelMemorySystem->pSysLtcBcAperture, NV_PSYSL2LTC_LTSS_CBC_NUM_ACTIVE_LTCS, regVal);

    // WB & I also if disabling cache
    if (!bEnable)
    {
        kmemsysWriteBackAndInvalidateSysL2_HAL(pGpu, pKernelMemorySystem);
    }
}

/*!
 * Releases the memory for SYS LTC apertures.
 *
 * @param[in] pGpu                pointer for OBJGPU instance.
 * @param[in] pKernelMemorySystem pointer for KernelMemorySystem instance.
 */
void kmemsysDestroySysLtcApertures_GR100
(
    OBJGPU *pGpu,
    KernelMemorySystem *pKernelMemorySystem
)
{
    NvU32 unitIndex;

    if (pKernelMemorySystem->pSysLtcBcAperture != NULL)
    {
        objDelete(pKernelMemorySystem->pSysLtcBcAperture);
        pKernelMemorySystem->pSysLtcBcAperture = NULL;
    }

    for (unitIndex = 0; unitIndex < pKernelMemorySystem->sysLtcApertureCount; unitIndex++)
    {
        objDelete(&((pKernelMemorySystem->pSysLtcApertures[unitIndex])));
    }

    portMemFree(pKernelMemorySystem->pSysLtcApertures);
    pKernelMemorySystem->pSysLtcApertures = NULL;
    pKernelMemorySystem->sysLtcApertureCount = 0;
}

/*!
 * @brief Flush CPU cache coherently on Vera+ platforms
 *
 * On Vera+ platforms, the standard OS cache flush is
 * ineffective. Use the GPU L2 probe filter to actively pull dirty cache lines.
 *
 * @param[in] pGpu                  GPU object pointer
 * @param[in] pKernelMemorySystem   KernelMemorySystem object pointer
 * @param[in] address               CPU address to flush
 * @param[in] size                  Size of region to flush
 */
void
kmemsysFlushCoherentCpuCache_GR100
(
    OBJGPU             *pGpu,
    KernelMemorySystem *pKernelMemorySystem,
    NvUPtr              address,
    NvU64               size
)
{
    kmemsysCleanLTCProbeFilter(pGpu, pKernelMemorySystem);
}

