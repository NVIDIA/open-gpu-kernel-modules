/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** HW State Routines ***************************\
*                                                                           *
*         Blackwell specific Descriptor List management functions           *
*                                                                           *
\***************************************************************************/

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/eng_desc.h"
#include "gpu/gpu_child_class_defs.h"
#include "os/os.h"
#include "nverror.h"
#include "nvrm_registry.h"

#include "published/blackwell/gb100/dev_vm.h"
#include "published/blackwell/gb100/dev_boot.h"
#include "published/blackwell/gb100/dev_boot_addendum.h"
#include "published/blackwell/gb100/dev_mnoc_pri_zb.h"
#include "published/blackwell/gb100/dev_pcfg_pf0.h"
#include "published/blackwell/gb100/dev_nv_pcie_config_reg_addendum.h"

static NV_STATUS _gpuFindPcieRegAddr_GB100(OBJGPU *pGpu, NvU32 regId, NvU32 *pRegAddr);
static NvU32     _gpuGetPciePartitionId_GB100(OBJGPU *pGpu, NvU32 hwDefAddr);
static NvU32     _gpuGetPcieCfgCapId_GB100(OBJGPU *pGpu, NvU32 hwDefAddr);
static NvU32     _gpuGetPcieExtCfgCapId_GB100(OBJGPU *pGpu, NvU32 hwDefAddr);
static NvU32     _gpuGetPcieCfgMsgboxId_GB100(OBJGPU *pGpu, NvU32 hwDefAddr);
static NV_STATUS _gpuGetPcieCfgCapBaseAddr_GB100(OBJGPU *pGpu, NvU32 hwDefAddr, NvU32 *pCapBaseAddr);
static NV_STATUS _gpuGetPcieExtCfgCapBaseAddr_GB100(OBJGPU *pGpu, NvU32 hwDefAddr, NvU32 *pCapBaseAddr);
static NvU32     _gpuGetPcieCfgRegOffset_GB100(OBJGPU *pGpu, NvU32 hwDefAddr);
static NvU32     _gpuGetPcieExtCfgRegOffset_GB100(OBJGPU *pGpu, NvU32 hwDefAddr);
static NvBool    _gpuMnocMboxSendReceiverCond(OBJGPU *pGpu, void *pCondData);
static NV_STATUS _gpuMnocMboxSendReceiverReady_GB100(OBJGPU *pGpu, IoAperture *pMboxAperture, NvU32 port, RMTIMEOUT *pTimeout);
static NvBool    _gpuMnocMboxSendCreditCond(OBJGPU *pGpu, void *pCondData);
static NV_STATUS _gpuMnocMboxSendCreditCheck_GB100(OBJGPU *pGpu, IoAperture *pMboxAperture, NvU32 port, RMTIMEOUT *pTimeout);
static NV_STATUS _gpuMnocMboxSendErrorCheck_GB100(OBJGPU *pGpu, IoAperture *pMboxAperture, NvU32 port);
static NvBool    _gpuMnocMboxPollCond(OBJGPU *pGpu, void *pCondData);
static NV_STATUS _gpuMnocMboxPollForMsg_GB100(OBJGPU *pGpu, IoAperture *pMboxAperture, NvU32 port, RMTIMEOUT *pTimeout);

//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//

static const GPUCHILDPRESENT gpuChildrenPresent_GB100[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 20),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 8),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(Spdm, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
    GPU_CHILD_PRESENT(KernelFsp, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(KernelCcu, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GB100(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GB100);
    return gpuChildrenPresent_GB100;
}


//
// List of GPU children that present for the chip. List entries contain$
// {CLASS-ID, # of instances} pairs, e.g.: {CE, 2} is 2 instance of OBJCE. This$
// list controls only engine presence. Order is defined by$
// gpuGetChildrenOrder_HAL.$
//
// IMPORTANT: This function is to be deleted. Engine removal should instead be$
// handled by <eng>ConstructEngine returning NV_ERR_NOT_SUPPORTED. PLEASE DO NOT$
// FORK THIS LIST!$
//
// List entries contain {CLASS-ID, # of instances} pairs.
//

static const GPUCHILDPRESENT gpuChildrenPresent_GB102[] =
{
    GPU_CHILD_PRESENT(OBJTMR, 1),
    GPU_CHILD_PRESENT(KernelMIGManager, 1),
    GPU_CHILD_PRESENT(KernelGraphicsManager, 1),
    GPU_CHILD_PRESENT(KernelRc, 1),
    GPU_CHILD_PRESENT(Intr, 1),
    GPU_CHILD_PRESENT(NvDebugDump, 1),
    GPU_CHILD_PRESENT(OBJSWENG, 1),
    GPU_CHILD_PRESENT(OBJUVM, 1),
    GPU_CHILD_PRESENT(KernelBif, 1),
    GPU_CHILD_PRESENT(KernelBus, 1),
    GPU_CHILD_PRESENT(KernelCE, 10),
    GPU_CHILD_PRESENT(KernelDisplay, 1),
    GPU_CHILD_PRESENT(VirtMemAllocator, 1),
    GPU_CHILD_PRESENT(KernelMemorySystem, 1),
    GPU_CHILD_PRESENT(MemoryManager, 1),
    GPU_CHILD_PRESENT(KernelFifo, 1),
    GPU_CHILD_PRESENT(KernelGmmu, 1),
    GPU_CHILD_PRESENT(KernelGraphics, 4),
    GPU_CHILD_PRESENT(KernelHwpm, 1),
    GPU_CHILD_PRESENT(KernelMc, 1),
    GPU_CHILD_PRESENT(SwIntr, 1),
    GPU_CHILD_PRESENT(KernelNvlink, 1),
    GPU_CHILD_PRESENT(KernelPerf, 1),
    GPU_CHILD_PRESENT(KernelPmu, 1),
    GPU_CHILD_PRESENT(KernelFsp, 1),
    GPU_CHILD_PRESENT(KernelGsp, 1),
    GPU_CHILD_PRESENT(KernelSec2, 1),
    GPU_CHILD_PRESENT(Spdm, 1),
    GPU_CHILD_PRESENT(ConfidentialCompute, 1),
    GPU_CHILD_PRESENT(KernelCcu, 1),
};

const GPUCHILDPRESENT *
gpuGetChildrenPresent_GB102(OBJGPU *pGpu, NvU32 *pNumEntries)
{
    *pNumEntries = NV_ARRAY_ELEMENTS(gpuChildrenPresent_GB102);
    return gpuChildrenPresent_GB102;
}

/*!
 * @brief If this is a GSP-RM platform, config accesses are not allowed
 *
 * @param[in] pGpu  GPU object pointer
 *
 * @returns NV_OK                Config space access supported
 *          NV_ERR_NOT_SUPPORTED Config space access not supported
 */
NV_STATUS
gpuConfigAccessSanityCheck_GB100
(
    OBJGPU *pGpu
)
{
    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        NV_PRINTF(LEVEL_ERROR, "Config register access restricted on GSP-RM. Please update call stack on Bug 4610008!\n");
        DBG_BREAKPOINT();
        return NV_ERR_NOT_SUPPORTED;
    }
    else
    {
        return NV_OK;
    }
}

/*!
 * @brief Read the non-private registers on vGPU through mirror space
 *
 * @param[in]  pGpu   GPU object pointer
 * @param[in]  index  Register offset in PCIe config space
 * @param[out] pData  Value of the register
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuReadPassThruConfigReg_GB100
(
    OBJGPU *pGpu,
    NvU32   index,
    NvU32  *pData
)
{
    NvU32 domain   = gpuGetDomain(pGpu);
    NvU8  bus      = gpuGetBus(pGpu);
    NvU8  device   = gpuGetDevice(pGpu);
    NvU8  function = 0;

    if (pGpu->hPci == NULL)
    {
        pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
    }

    *pData = osPciReadDword(pGpu->hPci, index);

    return NV_OK;
}

/*!
 * @brief Write to pcie spec registers using config cycles
 *
 * @param[in] pGpu       GPU object pointer
 * @param[in] hwDefAddr  HW defined register address
 * @param[in] value      Write this value to the register
 *
 * @returns    NV_OK on success
 */
static NV_STATUS
_gpuFindPcieRegAddr_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr,
    NvU32  *pRegAddr
)
{
    NV_STATUS status      = NV_OK;
    NvU32     partitionId = 0;
    NvU32     offset      = 0;
    NvU32     capBaseAddr = 0;

    partitionId = _gpuGetPciePartitionId_GB100(pGpu, hwDefAddr);

    if (partitionId == NV_PCIE_PARTITION_ID_TYPE0_HEADER)
    {
        //
        // In Type0 Header partition, there are no capability groups, so the
        // partition's base address is the base address we need
        //
        capBaseAddr = NV_PCIE_PARTITION_ID_TYPE0_HEADER_BASE_ADDR;
        offset      = hwDefAddr;
    }
    else if (partitionId == NV_PCIE_PARTITION_ID_CFG_SPACE)
    {
        status = _gpuGetPcieCfgCapBaseAddr_GB100(pGpu, hwDefAddr, &capBaseAddr);
        if (status == NV_OK)
        {
            offset = _gpuGetPcieCfgRegOffset_GB100(pGpu, hwDefAddr);
        }
    }
    else if (partitionId == NV_PCIE_PARTITION_ID_EXT_CFG_SPACE)
    {
        status = _gpuGetPcieExtCfgCapBaseAddr_GB100(pGpu, hwDefAddr, &capBaseAddr);
        if (status == NV_OK)
        {
            offset = _gpuGetPcieExtCfgRegOffset_GB100(pGpu, hwDefAddr);
        }
    }
    else
    {
        status = NV_ERR_NOT_SUPPORTED;
        DBG_BREAKPOINT();
    }

    if (status == NV_OK)
    {
        // Final address of register
        *pRegAddr = capBaseAddr + offset;
    }
    else
    {
        *pRegAddr = 0;
    }

    return status;
}

/*!
 * @brief Write to pcie spec registers using config cycles
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] hwDefAddr HW defined register address
 * @param[in] value     Write this value to the register
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuWriteBusConfigCycle_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr,
    NvU32   value
)
{
    extern NV_STATUS gpuWriteBusConfigCycle_GM107(OBJGPU *pGpu, NvU32 hwDefAddr, NvU32 value);
    NV_STATUS status   = NV_OK;
    NvU32     domain   = gpuGetDomain(pGpu);
    NvU8      bus      = gpuGetBus(pGpu);
    NvU8      device   = gpuGetDevice(pGpu);
    NvU8      function = 0;
    NvU32     regAddr  = 0;

    if (!IS_SILICON(pGpu))
    {
        return gpuWriteBusConfigCycle_GM107(pGpu, hwDefAddr, value);
    }

    status = gpuConfigAccessSanityCheck_HAL(pGpu);
    if (status != NV_OK)
    {
        return status;
    }

    // Find config register address via linked list traversal
    status = _gpuFindPcieRegAddr_GB100(pGpu, hwDefAddr, &regAddr);
    if (status == NV_OK)
    {
        // Write to the address
        if (pGpu->hPci == NULL)
        {
            pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
        }

        osPciWriteDword(pGpu->hPci, regAddr, value);
    }

    return status;
}

/*!
 * @brief Read the pcie spec registers using config cycles
 *
 * @param[in]  pGpu      GPU object pointer
 * @param[in]  hwDefAddr HW defined register address
 * @param[out] pData     Value of the register
 *
 * @returns    NV_OK on success
 */
NV_STATUS
gpuReadBusConfigCycle_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr,
    NvU32  *pData
)
{
    extern NV_STATUS gpuReadBusConfigCycle_GM107(OBJGPU *pGpu, NvU32 hwDefAddr, NvU32 *pData);
    NV_STATUS status   = NV_OK;
    NvU32     domain   = gpuGetDomain(pGpu);
    NvU8      bus      = gpuGetBus(pGpu);
    NvU8      device   = gpuGetDevice(pGpu);
    NvU8      function = 0;
    NvU32     regAddr  = 0;

    if (!IS_SILICON(pGpu))
    {
        return gpuReadBusConfigCycle_GM107(pGpu, hwDefAddr, pData);
    }

    status = gpuConfigAccessSanityCheck_HAL(pGpu);
    if (status != NV_OK)
    {
        return status;
    }

    // Find config register address via linked list traversal
    status = _gpuFindPcieRegAddr_GB100(pGpu, hwDefAddr, &regAddr);
    if (status == NV_OK)
    {
        // Read the address
        if (pGpu->hPci == NULL)
        {
            pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
        }

        *pData = osPciReadDword(pGpu->hPci, regAddr);
    }

    return status;
}

/*!
 * @brief Check if register being accessed is within guest BAR0 space.
 *
 * @param[in] pGpu   OBJGPU pointer
 * @param[in] addr   Address being validated
 */
NV_STATUS
gpuSanityCheckVirtRegAccess_GB100
(
    OBJGPU *pGpu,
    NvU32   addr
)
{
    // Not applicable in PV mode
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
    {
        return NV_OK;
    }

    // Check if address in NV_VIRTUAL_FUNCTION range, if not error out.
    if ((addr < DRF_EXTENT(NV_VIRTUAL_FUNCTION_PRIV)) ||
        ((addr >= DRF_BASE(NV_VIRTUAL_FUNCTION)) && (addr < DRF_EXTENT(NV_VIRTUAL_FUNCTION))))
    {
        return NV_OK;
    }

    return NV_ERR_INVALID_ADDRESS;
}

/*!
 * @brief Get the partition ID from PCIE Config Space
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] hwDefAddr HW defined register address
 *
 * @return Partition ID for the hwDefAddr
 */
static NvU32
_gpuGetPciePartitionId_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr
)
{
    if (hwDefAddr <= NV_PCIE_PARTITION_ID_TYPE0_HEADER_END)
    {
        return NV_PCIE_PARTITION_ID_TYPE0_HEADER;
    }
    else if ((hwDefAddr >= NV_PCIE_PARTITION_ID_CFG_SPACE_START) &&
             (hwDefAddr <= NV_PCIE_PARTITION_ID_CFG_SPACE_END))
    {
        return NV_PCIE_PARTITION_ID_CFG_SPACE;
    }
    else if (hwDefAddr >= NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_START)
    {
        return NV_PCIE_PARTITION_ID_EXT_CFG_SPACE;
    }
    else
    {
        return NV_PCIE_PARTITION_ID_INVALID;
    }
}

/*!
 * @brief Get the base address of the capability group corresponding to the hwDefAddr
 *        in PCI Config Space partition
 *
 * @param[in]  pGpu     GPU object pointer
 * @param[in]  hwDefAddr  Register address
 * @param[out] pRegAddr Final register address to be read
 *
 * Overview:
 * 1. Get the details that uniquely identify group of the hwDefAddr i.e, msgboxId & capId
 * 2. Read base address of this partition to get to first node of linklist
 * 3. Traverse the linked list to get the base address of the capability group
 *     a. Start from 0x34 base address, and go to next address until you find
 *        the cap_id you are looking for.
 *     b. First differentiating factor is cap_id, so extract that and if it is
 *        anything other than 0x9 then we have found the group address we are
 *        looking for, so exit.
 *     c. If cap_id is 0x9 - then we are in the VSC/VSEC register space.
 *     d. The differentiating factor for these registers is the MSGBOX_ID.
 *        Loop over until we find the right MSGBOX_ID, once found, we have
 *        the group address we are looking for, so exit.
 *
 * @return NV_OK                   Success
 *         NV_ERR_INVALID_ADDRESS  Unrecognised hwDefAddr
 */
static NV_STATUS
_gpuGetPcieCfgCapBaseAddr_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr,
    NvU32  *pCapBaseAddr
)
{
    NV_STATUS status         = NV_OK;
    NvU32     capBaseAddr    = NV_PCIE_PARTITION_ID_CFG_SPACE_BASE_ADDR;
    NvU32     regVal         = 0;
    NvU32     curCapId       = 0;
    NvU32     curMsgBoxId    = 0;
    NvU32     targetCapId    = 0;
    NvU32     targetMsgBoxId = 0;
    NvU32     domain         = gpuGetDomain(pGpu);
    NvU8      bus            = gpuGetBus(pGpu);
    NvU8      device         = gpuGetDevice(pGpu);
    NvU8      function       = 0;
    NvU8      regCount       = 0;

    if (pGpu->hPci == NULL)
    {
        pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
    }

    // 1. Get the details that uniquely identify capability group of the hwDefAddr i.e, capId & msgboxId
    targetCapId = _gpuGetPcieCfgCapId_GB100(pGpu, hwDefAddr);
    if (targetCapId == 0)
    {
        status = NV_ERR_INVALID_ADDRESS;
        *pCapBaseAddr = capBaseAddr;
        NV_PRINTF(LEVEL_INFO, "capId for register 0x%x not found\n", hwDefAddr);
        return status;
    }

    targetMsgBoxId = _gpuGetPcieCfgMsgboxId_GB100(pGpu, hwDefAddr);

    // 2. Read base address of this partition to get to first node of linklist
    regVal = osPciReadDword(pGpu->hPci, capBaseAddr);
    if (regVal == 0xFFFFFFFF)
    {
        NV_PRINTF(LEVEL_INFO, "Register read failed : 0x%x\n", capBaseAddr);
        status = NV_ERR_INVALID_STATE;
        NV_ASSERT(0);
        return status;
    }

    // 3. Traverse the linked list to get the base address of the capability group
    capBaseAddr = (regVal & 0xFF);
    while ((capBaseAddr != 0) && ((regCount++) <= NV_ARRAY_ELEMENTS(pcieCfgRegInfo)))
    {
        regVal = osPciReadDword(pGpu->hPci, capBaseAddr);
        if (regVal == 0xFFFFFFFF)
        {
            NV_PRINTF(LEVEL_INFO, "Register read failed : 0x%x\n", capBaseAddr);
            status = NV_ERR_INVALID_STATE;
            NV_ASSERT(0);
            return status;
        }

        curCapId = (regVal & NV_PCIE_PARTITION_ID_CFG_SPACE_CAP_ID_MASK);
        if (curCapId == targetCapId)
        {
            if (curCapId == NV_PCIE_REG_CAP_ID_CFG_VENDOR_SPECIFIC_CAP)
            {
                curMsgBoxId = (regVal & NV_PCIE_PARTITION_ID_CFG_SPACE_MSGBOX_ID_MASK);
                curMsgBoxId = (curMsgBoxId >> portUtilCountTrailingZeros32(NV_PCIE_PARTITION_ID_CFG_SPACE_MSGBOX_ID_MASK));
                if (curMsgBoxId == targetMsgBoxId)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        capBaseAddr = (regVal & NV_PCIE_PARTITION_ID_CFG_SPACE_NEXT_PTR_MASK);
        capBaseAddr = (capBaseAddr >> portUtilCountTrailingZeros32(NV_PCIE_PARTITION_ID_CFG_SPACE_NEXT_PTR_MASK));
    }

    *pCapBaseAddr = capBaseAddr;

    if ((capBaseAddr == 0) ||
        (regCount > NV_ARRAY_ELEMENTS(pcieCfgRegInfo)))
    {
        //
        // This particular register is NOT part of the linklist; this is a valid scenario,
        // however for the caller to differentiate return invalid_addr status
        //
        status = NV_ERR_INVALID_ADDRESS;
        NV_PRINTF(LEVEL_INFO, "Register 0x%x not part of PCIe linked list\n", hwDefAddr);
    }
    else
    {
        status = NV_OK;
    }

    return status;
}

/*!
 * @brief Helper function to get the group address of
 *        PCIE Extended Config Space
 *
 * @param[in]  pGpu          GPU object pointer
 * @param[in]  hwDefAddr     HW defined register address
 * @param[out] pCapBaseAddr  Group address
 *
 * Overview of linklist traversal:
 * 1. Get the details that uniquely identify group of the hwDefAddr i.e, capId, targetVendorId, targetDvsecLen
 * 2. Traverse the linked list to get the base address of the capability group
 *      a. Start from 0x100 base address, and go to next address until you find
 *         the cap_id you are looking for.
 *      b. First differentiating factor is cap_id, so extract that and if it is
 *         anything other than 0x23 then we have found the group address we are
 *         looking for, so exit.
 *      c. If cap_id is 0x23 - then we are in the DVSEC register space.
 *         ALL DVSEC register groups have the same 0x23 as the cap_id.
 *      d. To differentiate amongst the DVSEC groups, the differentiator is vendor_id.
 *      e. If there are multiple registers belonging to the same vendor, then they
 *         are differentiated based on DVSEC_LENGTH field.
 *      f. Once the right vendor & dvsec length is identified, we have found the
 *         group address we are looking for, so exit.
 *
 * @return NV_OK                   Success
 *         NV_ERR_INVALID_ADDRESS  Unrecognised hwDefAddr
 */
static NV_STATUS
_gpuGetPcieExtCfgCapBaseAddr_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr,
    NvU32  *pCapBaseAddr
)
{
    NV_STATUS status         = NV_OK;
    NvU32     capBaseAddr    = NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_BASE_ADDR;
    NvU32     regVal         = 0;
    NvU32     regVal2        = 0;
    NvU32     curCapId       = 0;
    NvU32     venIdAddr      = 0;
    NvU32     curVendorId    = 0;
    NvU32     curDvsecLen    = 0;
    NvU32     targetVendorId = 0;
    NvU32     targetDvsecLen = 0;
    NvU32     targetCapId    = 0;
    NvU32     domain         = gpuGetDomain(pGpu);
    NvU8      bus            = gpuGetBus(pGpu);
    NvU8      device         = gpuGetDevice(pGpu);
    NvU8      function       = 0;
    NvU8      regCount       = 0;

    *pCapBaseAddr = 0;

    if (pGpu->hPci == NULL)
    {
        pGpu->hPci = osPciInitHandle(domain, bus, device, function, NULL, NULL);
    }

    // 1. Get the details that uniquely identify group of the hwDefAddr i.e, capId, targetVendorId, targetDvsecLen
    targetCapId = _gpuGetPcieExtCfgCapId_GB100(pGpu, hwDefAddr);
    if (targetCapId == 0)
    {
        status = NV_ERR_INVALID_ADDRESS;
        *pCapBaseAddr = capBaseAddr;
        NV_PRINTF(LEVEL_INFO, "capId for register 0x%x not found\n", hwDefAddr);
        return status;
    }

    if (targetCapId == NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP)
    {
        gpuGetPcieExtCfgDvsecInfo_HAL(pGpu, hwDefAddr, &targetVendorId, &targetDvsecLen);
    }

    // 2. Traverse the linked list to get the base address of the capability group
    while ((capBaseAddr != 0) && ((regCount++) <= NV_ARRAY_ELEMENTS(pcieExtCfgRegInfo)))
    {
        regVal = osPciReadDword(pGpu->hPci, capBaseAddr);
        if (regVal == 0xFFFFFFFF)
        {
            NV_PRINTF(LEVEL_INFO, "Register read failed : 0x%x\n", capBaseAddr);
            status = NV_ERR_INVALID_STATE;
            NV_ASSERT(0);
            return status;
        }

        // If target cap_id is found, we found the correct group address
        curCapId = (regVal & NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_CAP_ID_MASK);
        if (curCapId == targetCapId)
        {
            // Check if cap_id is for DVSEC registers. Every DVSEC register has same cap_id of 0x23
            if (curCapId == NV_PCIE_REG_CAP_ID_EXT_CFG_DVSEC_CAP)
            {
                venIdAddr = (capBaseAddr + 0x4);
                regVal2   = osPciReadDword(pGpu->hPci, venIdAddr);
                if (regVal == 0xFFFFFFFF)
                {
                    NV_PRINTF(LEVEL_INFO, "Register read failed : 0x%x\n", venIdAddr);
                    status = NV_ERR_INVALID_STATE;
                    NV_ASSERT(0);
                    return status;
                }

                // To differentiate amongst different DVSEC group of registers, check the vendorId
                curVendorId = (regVal2 & NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_VENDOR_ID_MASK);
                if (curVendorId == targetVendorId)
                {
                    // If correct vendorId is found, find the right group by checking dvsecLength field
                    curDvsecLen = (regVal2 & NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_DVSEC_LEN_MASK);
                    curDvsecLen = (curDvsecLen >> portUtilCountTrailingZeros32(NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_DVSEC_LEN_MASK));
                    if (curDvsecLen == targetDvsecLen)
                    {
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }

        // Going for the next loop, hence update capBaseAddr
        capBaseAddr = (regVal & NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_NEXT_PTR_MASK);
        capBaseAddr = (capBaseAddr >> portUtilCountTrailingZeros32(NV_PCIE_PARTITION_ID_EXT_CFG_SPACE_NEXT_PTR_MASK));
    }

    *pCapBaseAddr = capBaseAddr;

    if ((capBaseAddr == 0) ||
        (regCount > NV_ARRAY_ELEMENTS(pcieExtCfgRegInfo)))
    {
        //
        // This particular register is NOT part of the linklist; this is a valid scenario,
        // however for the caller to differentiate return invalid_addr status
        //
        status = NV_ERR_INVALID_ADDRESS;
        NV_PRINTF(LEVEL_INFO, "Register 0x%x not part of PCIe linked list\n", hwDefAddr);
    }
    else
    {
        status = NV_OK;
    }

    return status;
}

/*!
 * @brief Get the CAPABILITY_ID corresponding to hwDefAddr
 *        in PCI Config Space partition
 *
 * @param[in]  pGpu      GPU object pointer
 * @param[in]  hwDefAddr HW defined register address
 *
 * @return CAPABILITY_ID corresponding to the hwDefAddr
 */
static NvU32
_gpuGetPcieCfgCapId_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr
)
{
    NvU32 i = 0;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pcieCfgRegInfo); i++)
    {
        if ((hwDefAddr >= pcieCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpFirstAddr) &&
            (hwDefAddr <= pcieCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpLastAddr))
        {
            return pcieCfgRegInfo[i].hwDefRegInfo.capId;
        }
    }

    return 0;
}

/*!
 * @brief Get the CAPABILITY_ID corresponding to hwDefAddr
 *        in PCI Config Space partition
 *
 * @param[in]  pGpu      GPU object pointer
 * @param[in]  hwDefAddr HW defined register address
 *
 * @return CAPABILITY_ID corresponding to the hwDefAddr
 */
static NvU32
_gpuGetPcieExtCfgCapId_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr
)
{
    NvU32 i = 0;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pcieExtCfgRegInfo); i++)
    {
        if ((hwDefAddr >= pcieExtCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpFirstAddr) &&
            (hwDefAddr <= pcieExtCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpLastAddr))
        {
            return pcieExtCfgRegInfo[i].hwDefRegInfo.capId;
        }
    }

    return 0;
}

/*!
 * @brief Get the msgbox ID corresponding to hwDefAddr
 *        in PCI Config Space partition
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] hwDefAddr HW defined register address
 *
 * @return Msgbox ID for the hwDefAddr
 */
static NvU32
_gpuGetPcieCfgMsgboxId_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr
)
{
    NvU32 msgboxId = 0;
    NvU32 i        = 0;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pcieCfgRegInfo); i++)
    {
        if ((hwDefAddr >= pcieCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpFirstAddr) &&
            (hwDefAddr <= pcieCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpLastAddr))
        {
            msgboxId = pcieCfgRegInfo[i].msgboxId;
            break;
        }
    }

    return msgboxId;
}

/*!
 * @brief Get the DVSEC Info corresponding to a DVSEC register
 *        in Extended Config Space partition
 *
 * @param[in]  pGpu      GPU object pointer
 * @param[in]  hwDefAddr HW defined register address
 * @param[out] pVenId    Vendor ID of the DVSEC register
 * @param[out] pDvsecLen DVSEC Length of the DVSEC register
 */
void
gpuGetPcieExtCfgDvsecInfo_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr,
    NvU32  *pVenId,
    NvU32  *pDvsecLen
)
{
    NvU32 i = 0;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pcieExtCfgRegInfo); i++)
    {
        if ((hwDefAddr >= pcieExtCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpFirstAddr) &&
            (hwDefAddr <= pcieExtCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpLastAddr))
        {
            *pVenId    = pcieExtCfgRegInfo[i].vendorId;
            *pDvsecLen = pcieExtCfgRegInfo[i].dvsecLen;
            return;
        }
    }

    NV_ASSERT_FAILED("VendorId and DvsecLength fields not found\n");
}

/*!
 * @brief Get the offset corresponding to hwDefAddr
 *        in Extended Config Space partition
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] hwDefAddr HW defined register address
 *
 * @return Offset of the register away from CAP_GRP base address
 */
static NvU32
_gpuGetPcieCfgRegOffset_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr
)
{
    NvU32 offset = 0;
    NvU32 i      = 0;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pcieCfgRegInfo); i++)
    {
        if ((hwDefAddr >= pcieCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpFirstAddr) &&
            (hwDefAddr <= pcieCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpLastAddr))
        {
            offset = (hwDefAddr - pcieCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpFirstAddr);
            break;
        }
    }

    return offset;
}

/*!
 * @brief Get the offset corresponding to hwDefAddr
 *        in Extended Config Space partition
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] hwDefAddr HW defined register address
 *
 * @return Offset of the register away from CAP_GRP base address
 */
static NvU32
_gpuGetPcieExtCfgRegOffset_GB100
(
    OBJGPU *pGpu,
    NvU32   hwDefAddr
)
{
    NvU32 offset = 0;
    NvU32 i      = 0;

    for (i = 0; i < NV_ARRAY_ELEMENTS(pcieExtCfgRegInfo); i++)
    {
        if ((hwDefAddr >= pcieExtCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpFirstAddr) &&
            (hwDefAddr <= pcieExtCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpLastAddr))
        {
            offset = (hwDefAddr - pcieExtCfgRegInfo[i].hwDefRegInfo.hwDefCapGrpFirstAddr);
            break;
        }
    }

    return offset;
}

/*!
 * @brief Get GPU ID based on PCIE config reads.
 * Also determine other properties of the PCIE capabilities.
 *
 * @param[in]   pGpu  OBJGPU pointer
 * @returns void.
 */
void
gpuGetIdInfo_GB100(OBJGPU *pGpu)
{
    NvU32 data;
    NvU32 deviceId;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_REVISION_ID_AND_CLASS_CODE, &data) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                "unable to read NV_PF0_REVISION_ID_AND_CLASS_CODE\n");
        return;
    }

    // we only need the FIB and MASK values
    pGpu->idInfo.PCIRevisionID = (data & ~GPU_DRF_SHIFTMASK(NV_PF0_REVISION_ID_AND_CLASS_CODE_PROGRAMMING_INTERFACE)
                                       & ~GPU_DRF_SHIFTMASK(NV_PF0_REVISION_ID_AND_CLASS_CODE_SUB_CLASS_CODE)
                                       & ~GPU_DRF_SHIFTMASK(NV_PF0_REVISION_ID_AND_CLASS_CODE_BASE_CLASS_CODE));

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_SUBSYSTEM_ID_AND_VENDOR_ID, &data) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_PF0_SUBSYSTEM_ID_AND_VENDOR_ID\n");
        return;
    }
    pGpu->idInfo.PCISubDeviceID = data;

    if (GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DEVICE_VENDOR_ID, &deviceId) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "unable to read NV_PF0_DEVICE_VENDOR_ID\n");
        return;
    }
    // For universal GPU use unlatched value
    pGpu->idInfo.PCIDeviceID = deviceId;

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        //
        // In vgpu production environment, RM replaces direct GPU register
        // reads in NV_XTL space w/ OS pci reads when running inside the VF. It
        // therefore gets the correct device id. The same mechanism is not used
        // when running on MODS. Hence, RM gets bogus values of device id on
        // MODS runs. In this case, RM will have to explicitly ask MODS to
        // issue a read of the NV_PF0_DEVICE_VENDOR_ID register from the PF's pci
        // config space.
        //
        if (RMCFG_FEATURE_PLATFORM_MODS && IS_VIRTUAL_WITH_SRIOV(pGpu))
        {
            NV_ASSERT_OR_RETURN_VOID(osReadPFPciConfigInVF(NV_PF0_DEVICE_VENDOR_ID,
                                                &deviceId) == NV_OK);
            pGpu->idInfo.PCIDeviceID = deviceId;
            NV_PRINTF(LEVEL_INFO, "pci_dev_id = 0x%x\n", pGpu->idInfo.PCIDeviceID);
        }
    }
}

/*!
 * @brief        Handle SEC_FAULT
 *
 * @param[in]    pGpu   GPU object pointer
 */
void
gpuHandleSecFault_GB100
(
    OBJGPU *pGpu
)
{
    NvU32   secDebug = 0;

    //
    // Read SEC_FAULT config space to determine what went wrong.
    // Do not return early on error, we must take the GPU down.
    //
    NV_ASSERT_OK(GPU_BUS_CFG_CYCLE_RD32(pGpu, NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1, &secDebug));

    MODS_ARCH_ERROR_PRINTF("NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1:0x%x\n", secDebug);
    NV_PRINTF(LEVEL_FATAL, "SEC_FAULT lockdown detected. This is fatal. "
                           "RM will now shut down. NV_PF0_DVSEC0_SEC_FAULT_REGISTER_1: 0x%x\n", secDebug);

#define LOG_SEC_FAULT(field) \
    if (DRF_VAL(_PF0, _DVSEC0_SEC_FAULT_REGISTER_1, field, secDebug) != 0) \
    { \
        MODS_ARCH_ERROR_PRINTF("DVSEC0_SEC_FAULT_REGISTER_1" #field "\n"); \
        NV_PRINTF(LEVEL_FATAL, "SEC_FAULT type: " #field "\n"); \
        nvErrorLog_va((void *)(pGpu), SEC_FAULT_ERROR, \
                      "SEC_FAULT: " #field ); \
    }

    LOG_SEC_FAULT(_FUSE_POD);
    LOG_SEC_FAULT(_FUSE_SCPM);
    LOG_SEC_FAULT(_FSP_SCPM);
    LOG_SEC_FAULT(_SEC2_SCPM);
    LOG_SEC_FAULT(_FSP_DCLS);
    LOG_SEC_FAULT(_SEC2_DCLS);
    LOG_SEC_FAULT(_GSP_DCLS);
    LOG_SEC_FAULT(_PMU_DCLS);
    LOG_SEC_FAULT(_IFF_SEQUENCE_TOO_BIG);
    LOG_SEC_FAULT(_PRE_IFF_CRC_CHECK_FAILED);
    LOG_SEC_FAULT(_POST_IFF_CRC_CHECK_FAILED);
    LOG_SEC_FAULT(_IFF_ECC_UNCORRECTABLE_ERROR);
    LOG_SEC_FAULT(_IFF_CMD_FORMAT_ERROR);
    LOG_SEC_FAULT(_IFF_PRI_ERROR);
    LOG_SEC_FAULT(_C2C_MISC_LINK_ERROR);
    LOG_SEC_FAULT(_FSP_WDT);
    LOG_SEC_FAULT(_GSP_WDT);
    LOG_SEC_FAULT(_PMU_WDT);
    LOG_SEC_FAULT(_SEC2_WDT);
    LOG_SEC_FAULT(_C2C_HBI_LINK_ERROR);
    LOG_SEC_FAULT(_FSP_EMP);
    LOG_SEC_FAULT(_FSP_UNCORRECTABLE_ERRORS);
    LOG_SEC_FAULT(_FUSE_POD_2ND);
    LOG_SEC_FAULT(_FUSE_SCPM_2ND);
    LOG_SEC_FAULT(_IFF_SEQUENCE_TOO_BIG_2ND);
    LOG_SEC_FAULT(_PRE_IFF_CRC_CHECK_FAILED_2ND);
    LOG_SEC_FAULT(_POST_IFF_CRC_CHECK_FAILED_2ND);
    LOG_SEC_FAULT(_IFF_ECC_UNCORRECTABLE_ERROR_2ND);
    LOG_SEC_FAULT(_IFF_CMD_FORMAT_ERROR_2ND);
    LOG_SEC_FAULT(_IFF_PRI_ERROR_2ND);
    LOG_SEC_FAULT(_DEVICE_LOCKDOWN);
    LOG_SEC_FAULT(_FUNCTION_LOCKDOWN);

#undef LOG_SEC_FAULT

    //
    // After SEC_FAULT occurs, the GPU will only return SCPM dummy values until properly reset.
    // Only cold reset will clear SEC_FAULT, not hot reset. This GPU is as good as lost.
    // handleGpuLost first to setGpuDisconnectedProperties so that another reg read does not
    // happen when the notifier is sent below.
    //
    osHandleGpuLost(pGpu);

    //
    // Send SEC_FAULT notification. This should tells any MODS test testing for this
    // error to pass and exit
    //
    gpuNotifySubDeviceEvent(pGpu,
                            NV2080_NOTIFIERS_SEC_FAULT_ERROR,
                            NULL,
                            0,
                            0,
                            SEC_FAULT_ERROR);
}

/*!
 * @brief Check if CC bit has been set in the scratch register
 *
 * @param[in]  pGpu  GPU object pointer
 */
NvBool
gpuIsCCEnabledInHw_GB100
(
    OBJGPU *pGpu
)
{
    NvU32 val = GPU_REG_RD32(pGpu, NV_PMC_SCRATCH_RESET_2_CC);
    return FLD_TEST_DRF(_PMC, _SCRATCH_RESET_2_CC, _MODE_ENABLED, _TRUE, val);
}

/*!
 * @brief Check if dev mode bit has been set in the scratch register
 *
 * @param[in]  pGpu  GPU object pointer
 */
NvBool
gpuIsDevModeEnabledInHw_GB100
(
    OBJGPU *pGpu
)
{
    NvU32 val = GPU_REG_RD32(pGpu, NV_PMC_SCRATCH_RESET_2_CC);
    return FLD_TEST_DRF(_PMC, _SCRATCH_RESET_2_CC, _DEV_ENABLED, _TRUE, val);
}

/*!
 * Check if Nvlink multiGPU mode has been set
 *
 * @param[in]  pGpu  GPU object pointer
 */
NvBool
gpuIsMultiGpuNvleEnabledInHw_GB100
(
    OBJGPU *pGpu
)
{
    NvU32 data;

    //
    // NVLink Multi-GPU mode is set when it is enabled via a regkey or when NVLE mode bit is set in HW.
    // TODO : Nvlink multiGPU regkey to be removed once we have stable vbios, BUG 5178914
    //
    if (((osReadRegistryDword(pGpu, NV_REG_STR_RM_CC_MULTI_GPU_NVLE_MODE_ENABLED, &data) == NV_OK) &&
        (data == NV_REG_STR_RM_CC_MULTI_GPU_NVLE_MODE_ENABLED_YES)) || gpuIsNvleModeEnabledInHw_HAL(pGpu))
    {
        return NV_TRUE;
    }
    return NV_FALSE;
}

/*
 * @brief Check if NVLE mode bit has been set in the scratch register
 *
 * @param[in]  pGpu  GPU object pointer
 */
NvBool
gpuIsNvleModeEnabledInHw_GB100
(
    OBJGPU *pGpu
)
{
    NvU32 val = GPU_REG_RD32(pGpu, NV_PMC_SCRATCH_RESET_2_CC);
    return FLD_TEST_DRF(_PMC, _SCRATCH_RESET_2_CC, _NVLE_MODE_ENABLED, _TRUE, val);
}

/*!
 * @brief Wait before receiving the next message from mailbox
 *
 * @param[in]     pGpu     GPU object pointer
 * @param[in]     pMboxAperture Aperture for accessing mailbox registers
 * @param[in]     port     MNOC port number [0..3]
 * @param[in]     pTimeout Timeout for operation
 * @param[out]    pMsgBuf  Pointer to the message buffer
 * @param[in/out] pMsgSize Message size in bytes
 *
 * @returns NV_OK upon successfully message received
 */
NV_STATUS
gpuMnocMboxSyncRecv_GB100
(
    OBJGPU    *pGpu,
    IoAperture *pMboxAperture,
    NvU32      port,
    RMTIMEOUT *pTimeout,
    void      *pMsgBuf,
    NvU32     *pMsgSize
)
{
    NV_STATUS status;

    // Wait for new message to be ready
    status = _gpuMnocMboxPollForMsg_GB100(pGpu, pMboxAperture, port, pTimeout);
    if (status != NV_OK)
    {
        return status;
    }

    // Read received message
    return gpuMnocMboxRecv_HAL(pGpu, pMboxAperture, port, pMsgBuf, pMsgSize);
}

/*!
 * @brief Send MNOC mailbox message
 *        This function is used to send MNOC mailbox message
 *
 * Send API follows below steps to transmit message-
 * 1. Polling for the mailbox send port to become ready
 * 2. Configuring mailbox message size into to mailbox send port INFO register
 * 3. Transmitting the message, (up to) 4 bytes at a time
 *    a. Check for mailbox send port buffer overflow
 * 4. Check for transmit error
 *
 * @param[in] pGpu     GPU object pointer
 * @param[in] pMboxAperture Aperture for accessing mailbox registers
 * @param[in] port     The mailbox port number [0..3]
 * @param[in] pMsgBuf  Pointer to the message buffer
 * @param[in] msgSize  Message size in bytes
 *
 * @returns NV_OK upon sending MNOC mailbox message successfully
 */
NV_STATUS
gpuMnocMboxSend_GB100
(
    OBJGPU    *pGpu,
    IoAperture *pMboxAperture,
    NvU32      port,
    RMTIMEOUT *pTimeout,
    void      *pMsgBuf,
    NvU32      msgSize
)
{
    NvU32 copyDataSize;
    NvU32 data;
    NvU32 regVal;
    NvU32 offset;

    // Sanity check for message and size
    if ((pMsgBuf == NULL)                         ||
        (msgSize < gpuMnocMboxMinMessageSize_HAL(pGpu)) ||
        (msgSize > gpuMnocMboxMaxMessageSize_HAL(pGpu)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Timeout waiting (polling) for mailbox send port to become ready
    if (_gpuMnocMboxSendReceiverReady_GB100(pGpu, pMboxAperture, port, pTimeout) != NV_OK)
    {
        return NV_ERR_NOT_READY;
    }

    // Configuring mailbox message size into the mailbox send port INFO register
    regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX(port));
    regVal = FLD_SET_DRF_NUM(_MNOC_ZB_PRI, _MESSAGE_INFO_0_RECEIVEMBOX, _MESSAGE_SIZE,
                             msgSize, regVal);
    regVal = FLD_SET_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_RECEIVEMBOX, _MESSAGE_TRIGGER,
                         _SET, regVal);
    REG_WR32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX(port), regVal);

    // Transmitting the message, (up to) 4 bytes at a time
    offset = 0;
    while (msgSize > 0)
    {
        // Check available credit after every 64 bytes (i.e. 16 dword) transfer
        if ((offset == NV_ALIGN_UP(offset, 64)) &&
            _gpuMnocMboxSendCreditCheck_GB100(pGpu, pMboxAperture, port, pTimeout) != NV_OK)
        {
            return NV_ERR_TIMEOUT;
        }

        // Transmitting message
        data = 0x0;
        copyDataSize = NV_MIN(sizeof(NvU32), msgSize);
        portMemCopy(&data, copyDataSize, (NvU8*)((NvU8*)pMsgBuf + offset), copyDataSize);
        REG_WR32(pMboxAperture, NV_MNOC_ZB_PRI_WDATA_0_RECEIVEMBOX(port), data);

        offset  += copyDataSize;
        msgSize -= copyDataSize;
    }

    // Check for transmit error
    return _gpuMnocMboxSendErrorCheck_GB100(pGpu, pMboxAperture, port);
}

/*!
 * @brief Receive MNOC mbox message
 *        This function is used to receive MNOC mailbox message
 *
 * Receive MNOC mailbox API follows below steps
 * 1. Reading received mailbox message size and sanity check for message size
 * 2. Receive the whole message, (up to) 4 bytes at a time and copy into the receive buffer
 *
 * @param[in]     pGpu     GPU object pointer
 * @param[in]     pMboxAperture Aperture for accessing mailbox registers
 * @param[in]     port     The mailbox port number [0..3]
 * @param[out]    pMsgBuf  Pointer to the message buffer
 * @param[in/out] pMsgSize Message size in bytes
 *
 * @returns NV_OK upon successfully MNOC mbox message received
 */
NV_STATUS
gpuMnocMboxRecv_GB100
(
    OBJGPU *pGpu,
    IoAperture *pMboxAperture,
    NvU32   port,
    void   *pMsgBuf,
    NvU32  *pMsgSize
)
{
    NvU32 regVal;
    NvU32 offset;
    NvU32 recvMsgSize;
    NvU32 copyDataSize;

    // Sanity check for message buffer
    if (pMsgBuf == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Sanity check for whether a message is actually available to be received
    if (!gpuMnocMboxIsMsgAvailable_HAL(pGpu, pMboxAperture, port))
    {
        return NV_ERR_NOT_READY;
    }

    // Reading mailbox message size
    regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port));
    recvMsgSize = DRF_VAL(_MNOC_ZB_PRI, _MESSAGE_INFO_0_SENDMBOX, _MESSAGE_SIZE, regVal);

    // Sanity check for received size
    NV_ASSERT_OR_RETURN(recvMsgSize >= gpuMnocMboxMinMessageSize_HAL(pGpu), NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(recvMsgSize <= gpuMnocMboxMaxMessageSize_HAL(pGpu), NV_ERR_INVALID_STATE);

    // Sanity check for input size
    if (*pMsgSize < recvMsgSize)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    // Receive the whole message, (up to) 4 bytes at a time
    offset = 0;
    while (recvMsgSize > 0)
    {
        // Reading received message from the mailbox receive port
        regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_RDATA_0_SENDMBOX(port));
        copyDataSize = NV_MIN(sizeof(NvU32), recvMsgSize);

        // Copying mailbox message into receive buffer
        portMemCopy(((NvU8*)pMsgBuf + offset), copyDataSize,
                    (NvU8*)&regVal, copyDataSize);
        offset += copyDataSize;
        recvMsgSize -= copyDataSize;
    }

    // Updating how many bytes received
    *pMsgSize = offset;

    regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port));
    // Check if there was any error while receiving mailbox message
    if (FLD_TEST_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_SENDMBOX, _ERR, _TRUE, regVal))
    {
        return NV_ERR_GENERIC;
    }
    else if (offset < DRF_VAL(_MNOC_ZB_PRI, _MESSAGE_INFO_0_SENDMBOX, _MESSAGE_SIZE, regVal))
    {
        return NV_ERR_OUT_OF_RANGE;
    }

    return NV_OK;
}

/*!
 * @brief Check if next MNOC mailbox message is available
 *
 * @param[in] pGpu     GPU object pointer
 * @param[in] pMboxAperture Aperture for accessing mailbox registers
 * @param[in] port     MNOC mailbox port number [0..3]
 *
 * @returns   NV_TRUE  New message available
 *            NV_FALSE New message not available
 */
NvBool
gpuMnocMboxIsMsgAvailable_GB100
(
    OBJGPU *pGpu,
    IoAperture *pMboxAperture,
    NvU32   port
)
{
    NvU32 regVal;

    regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port));

    return FLD_TEST_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_SENDMBOX, _MESSAGE_READY, _TRUE, regVal);
}

/*!
 * @brief Enable MNOC interrupt
 */
void
gpuMnocMboxInterruptEnable_GB100
(
    OBJGPU     *pGpu,
    IoAperture *pMboxAperture,
    NvU32       port
)
{
    NvU32 regVal;

    // Enable the MNOC mailbox interrupt at source level.
    regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port));
    regVal = FLD_SET_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_SENDMBOX, _INTR_ENABLE, _ENABLED, regVal);
    REG_WR32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port), regVal);
}

/*!
 * @brief Disable MNOC interrupt
 */
void
gpuMnocMboxInterruptDisable_GB100
(
    OBJGPU     *pGpu,
    IoAperture *pMboxAperture,
    NvU32       port
)
{
    NvU32 regVal;

    // Disable the MNOC mailbox interrupt at source level.
    regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port));
    regVal = FLD_SET_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_SENDMBOX, _INTR_ENABLE, _DISABLED, regVal);
    REG_WR32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port), regVal);
}

/*!
 * @brief Is mailbox interrupt raised ?
 */
NvBool
gpuMnocMboxInterruptRaised_GB100
(
    OBJGPU     *pGpu,
    IoAperture *pMboxAperture,
    NvU32       port
)
{
    NvU32 regVal;

    regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port));
    return FLD_TEST_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_SENDMBOX, _INTR_STATUS, _SET, regVal);
}

/*!
 * @brief clear mailbox interrupt
 */
void
gpuMnocMboxInterruptClear_GB100
(
    OBJGPU     *pGpu,
    IoAperture *pMboxAperture,
    NvU32       port
)
{
    NvU32 regVal;

    regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port));
    regVal = FLD_SET_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_SENDMBOX, _INTR_STATUS, _W1CLR, regVal);
    REG_WR32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX(port), regVal);
}

/*!
 * @brief return the minimum transfer size thru MNOC MBOX
 */
NvU32
gpuMnocMboxMinMessageSize_GB100
(
    OBJGPU *pGpu
)
{
    return sizeof(NvU32);
}

/*!
 * @brief return the maximum transfer size thru MNOC MBOX
 */
NvU32
gpuMnocMboxMaxMessageSize_GB100
(
    OBJGPU *pGpu
)
{
    return DRF_MASK(NV_MNOC_ZB_PRI_MESSAGE_INFO_0_SENDMBOX_MESSAGE_SIZE) + 1;
}

// Used with gpuTimeoutCondWait to wrap the arguments to the timeout condition checks
typedef struct 
{
    IoAperture *pMboxAperture;
    NvU32       port;
} MnocWaitArg;

/*!
 * @brief GpuWaitConditionFunc for MBOX receiver ready
 *
 * @param[in] pGpu          GPU object pointer
 * @param[in] pCondData     Wrapper around the aperture and port
 *
 * @returns   NV_TRUE       Receiver is ready
 */
NvBool
_gpuMnocMboxSendReceiverCond
(
    OBJGPU *pGpu,
    void   *pCondData
)
{
    MnocWaitArg *pArg = (MnocWaitArg*) pCondData;
    NvU32 regVal = REG_RD32(pArg->pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX(pArg->port));
    // Check if receiver is ready for next message
    return FLD_TEST_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_RECEIVEMBOX, _RECEIVE_READY, _TRUE, regVal);
}

/*!
 * @brief Helper function is used to wait until the receiver is ready
 *
 * @param[in] pGpu     GPU object pointer
 * @param[in] pMboxAperture Aperture for accessing mailbox registers
 * @param[in] port     The mailbox port number [0..3]
 *
 * @returns NV_OK when receiver is ready
 */
static NV_STATUS
_gpuMnocMboxSendReceiverReady_GB100
(
    OBJGPU     *pGpu,
    IoAperture *pMboxAperture,
    NvU32       port,
    RMTIMEOUT  *pTimeout
)
{
    MnocWaitArg waitArg = {pMboxAperture, port};
    return gpuTimeoutCondWait(pGpu, _gpuMnocMboxSendReceiverCond, &waitArg, pTimeout);
}

/*!
 * @brief GpuWaitConditionFunc for credit available
 *
 * @param[in] pGpu          GPU object pointer
 * @param[in] pCondData     Wrapper around the aperture and port
 *
 * @returns   NV_TRUE       Credit is available
 */
NvBool
_gpuMnocMboxSendCreditCond
(
    OBJGPU *pGpu,
    void   *pCondData
)
{
    MnocWaitArg *pArg = (MnocWaitArg*) pCondData;
    NvU32 regVal = REG_RD32(pArg->pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX(pArg->port));
    return FLD_TEST_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_RECEIVEMBOX, _CREDIT_AVAILABLE, _TRUE, regVal);
}

/*!
 * @brief Helper function is used to check the receiver port credit availability
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] pMboxAperture Aperture for accessing mailbox registers
 * @param[in] port      The mailbox port number [0..3]
 *
 * @returns NV_OK when mailbox send port has send credit
 */
static NV_STATUS
_gpuMnocMboxSendCreditCheck_GB100
(
    OBJGPU     *pGpu,
    IoAperture *pMboxAperture,
    NvU32       port,
    RMTIMEOUT  *pTimeout
)
{
    MnocWaitArg waitArg = {pMboxAperture, port};
    return gpuTimeoutCondWait(pGpu, _gpuMnocMboxSendCreditCond, &waitArg, pTimeout);
}

/*!
 * @brief Helper function is used to check message transmit error
 *
 * @param[in] pGpu      GPU object pointer
 * @param[in] pMboxAperture Aperture for accessing mailbox registers
 * @param[in] port      The mailbox port number [0..3]
 *
 * @returns NV_OK upon message successfully sent
 */
static NV_STATUS
_gpuMnocMboxSendErrorCheck_GB100
(
    OBJGPU     *pGpu,
    IoAperture *pMboxAperture,
    NvU32       port
)
{
    NvU32 regVal;

    regVal = REG_RD32(pMboxAperture, NV_MNOC_ZB_PRI_MESSAGE_INFO_0_RECEIVEMBOX(port));

    // Check if there was any error while sending mailbox message
    if (FLD_TEST_DRF(_MNOC_ZB_PRI, _MESSAGE_INFO_0_RECEIVEMBOX, _ERR,_TRUE, regVal))
        return NV_ERR_GENERIC;

    return NV_OK;
}

/*!
 * @brief GpuWaitConditionFunc for new MNOC message
 *
 * @param[in] pGpu          GPU object pointer
 * @param[in] pCondData     Wrapper around the aperture and port
 *
 * @returns   NV_TRUE       New message ready at mailbox port
 */
NvBool
_gpuMnocMboxPollCond
(
    OBJGPU *pGpu,
    void   *pCondData
)
{
    MnocWaitArg *pArg = (MnocWaitArg*) pCondData;
    return gpuMnocMboxIsMsgAvailable_HAL(pGpu, pArg->pMboxAperture, pArg->port);
}

/*!
 * @brief Poll for new MNOC message
 *
 * @param[in] pGpu          GPU object pointer
 * @param[in] pMboxAperture Aperture for accessing mailbox registers
 * @param[in] port          MNOC port number [0..3]
 * @param[in] pTimeout      Timeout for operation
 *
 * @returns   NV_OK    New message ready at mailbox port
 */
static NV_STATUS
_gpuMnocMboxPollForMsg_GB100
(
    OBJGPU     *pGpu,
    IoAperture *pMboxAperture,
    NvU32       port,
    RMTIMEOUT  *pTimeout
)
{
    MnocWaitArg waitArg = {pMboxAperture, port};
    return gpuTimeoutCondWait(pGpu, _gpuMnocMboxPollCond, &waitArg, pTimeout);
}

