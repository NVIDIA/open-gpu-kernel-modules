/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


/*!
 * @file
 * @brief  Provides KERNEL only functions for OBJBIF
 */

/* ------------------------ Includes ---------------------------------------- */
#include "gpu/bif/kernel_bif.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/gpu.h"
#include "gpu/intr/intr.h"
#include "os/os.h"
#include "platform/chipset/chipset.h"
#include "core/locks.h"
#include "nvrm_registry.h"
#include "diagnostics/tracer.h"
#include "nvpcie.h"
#include "vgpu/vgpu_events.h"

/* ------------------------ Macros ------------------------------------------ */
/* ------------------------ Compile Time Checks ----------------------------- */
/* ------------------------ Static Function Prototypes ---------------------- */
static void _kbifInitRegistryOverrides(OBJGPU *, KernelBif *);
static void _kbifCheckIfGpuExists(OBJGPU *, void*);
static NV_STATUS _kbifSetPcieRelaxedOrdering(OBJGPU *, KernelBif *, NvBool);

/* ------------------------ Public Functions -------------------------------- */

/*!
 * @brief KernelBif Constructor
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  BIF object pointer
 * @param[in] engDesc     Engine descriptor
 */
NV_STATUS
kbifConstructEngine_IMPL
(
    OBJGPU        *pGpu,
    KernelBif     *pKernelBif,
    ENGDESCRIPTOR  engDesc
)
{
    // Initialize registry overrides
    _kbifInitRegistryOverrides(pGpu, pKernelBif);

    // WAR for Bug 3208922 - disables P2P on Ampere NB
    kbifApplyWARBug3208922_HAL(pGpu, pKernelBif);

    // Disables P2P on VF
    kbifDisableP2PTransactions_HAL(pGpu, pKernelBif);

    // Cache MNOC interface support
    kbifIsMnocSupported_HAL(pGpu, pKernelBif);

    // Cache VF info
    kbifCacheVFInfo_HAL(pGpu, pKernelBif);

    // Used to track when the link has gone into Recovery, which can cause CEs.
    pKernelBif->EnteredRecoverySinceErrorsLastChecked = NV_FALSE;

    return NV_OK;
}

/*!
 * @brief KernelBif Constructor
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  BIF object pointer
 */
NV_STATUS
kbifStateInitLocked_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    OBJSYS    *pSys   = SYS_GET_INSTANCE();
    OBJOS     *pOS    = SYS_GET_OS(pSys);
    OBJCL     *pCl    = SYS_GET_CL(pSys);
    NV_STATUS  status = NV_OK;

    // Return early if GPU is connected to an unsupported chipset
    if (pCl->getProperty(pCl, PDB_PROP_CL_UNSUPPORTED_CHIPSET))
    {
        return NV_ERR_NOT_COMPATIBLE;
    }

    // Initialize OS mapping and core logic
    status = osInitMapping(pGpu);
    if (status != NV_OK)
    {
        return status;
    }

    // Initialize BIF static info
    kbifStaticInfoInit(pGpu, pKernelBif);

    // Initialize DMA caps
    kbifInitDmaCaps(pGpu, pKernelBif);

    // Check for OS w/o usable PAT support
    if ((kbifGetBusIntfType_HAL(pKernelBif) ==
         NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS) &&
        pOS->getProperty(pOS, PDB_PROP_OS_PAT_UNSUPPORTED))
    {
        NV_PRINTF(LEVEL_INFO,
                  "BIF disabling noncoherent on OS w/o usable PAT support\n");

        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_SUPPORT_NONCOHERENT, NV_FALSE);
    }

    return status;
}

/*!
 * @brief KernelBif state load
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  BIF object pointer
 * @param[in] flags       GPU state flag
 */
NV_STATUS
kbifStateLoad_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      flags
)
{
    NV_PRINTF(LEVEL_INFO, "BIF DMA Caps: %08x\n", kbifGetDmaCaps(pGpu, pKernelBif));

    // Apply C73 chipset WAR
    kbifExecC73War_HAL(pGpu, pKernelBif);

    // Check for stale PCI-E dev ctrl/status errors and AER errors
    kbifClearConfigErrors(pGpu, pKernelBif, NV_TRUE, KBIF_CLEAR_XVE_AER_ALL_MASK);

    //
    // A vGPU cannot disappear and these accesses are
    // particularly expensive on vGPUs
    //
    if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_CHECK_IF_GPU_EXISTS_DEF) &&
        !IS_VIRTUAL(pGpu))
    {
        osSchedule1SecondCallback(pGpu, _kbifCheckIfGpuExists, NULL, NV_OS_1HZ_REPEAT);
    }

    return NV_OK;
}

/*!
 * @brief Configure PCIe Relaxed Ordering in BIF
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  KBIF object pointer
 * @param[in] enableRo    Enable/disable RO
 */
static NV_STATUS
_kbifSetPcieRelaxedOrdering
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvBool    enableRo
)
{
    NV2080_CTRL_INTERNAL_BIF_SET_PCIE_RO_PARAMS pcieRo;
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS  status;

    pcieRo.enableRo = enableRo;

    status = pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_BIF_SET_PCIE_RO,
                             &pcieRo, sizeof(pcieRo));
    if (status != NV_OK) {
        NV_PRINTF(LEVEL_ERROR, "NV2080_CTRL_CMD_INTERNAL_BIF_SET_PCIE_RO failed %s (0x%x)\n",
                  nvstatusToString(status), status);
        return status;
    }

    return NV_OK;
}

/*!
 * @brief KernelBif state post-load
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  KBIF object pointer
 * @param[in] flags       GPU state flag
 */
NV_STATUS
kbifStatePostLoad_IMPL
(
    OBJGPU      *pGpu,
    KernelBif   *pKernelBif,
    NvU32       flags
)
{
    NV_STATUS status;

    kbifInitRelaxedOrderingFromEmulatedConfigSpace(pGpu, pKernelBif);
    if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_PCIE_RELAXED_ORDERING_SET_IN_EMULATED_CONFIG_SPACE)) {
        //
        // This is done from StatePostLoad() to guarantee that BIF's StateLoad()
        // is already completed for both monolithic RM and GSP RM.
        //
        status = _kbifSetPcieRelaxedOrdering(pGpu, pKernelBif, NV_TRUE);
        if (status != NV_OK)
            return NV_OK;
    }

    return NV_OK;
}

/*!
 * @brief KernelBif state unload
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  BIF object pointer
 * @param[in] flags       GPU state flag
 */
NV_STATUS
kbifStateUnload_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvU32      flags
)
{

    return NV_OK;
}

/*!
 * @brief Initialize DMA caps
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  BIF object pointer
 */
void
kbifInitDmaCaps_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl  = SYS_GET_CL(pSys);

    pKernelBif->dmaCaps = REF_DEF(BIF_DMA_CAPS_NOSNOOP, _CTXDMA);

    // Set the coherency cap on host RM based on the chipset
    if (IsAMODEL(pGpu) ||
        pCl->getProperty(pCl, PDB_PROP_CL_IS_CHIPSET_IO_COHERENT))
    {
        pKernelBif->dmaCaps |= REF_DEF(BIF_DMA_CAPS_SNOOP, _CTXDMA);
    }
}

NvU32
kbifGetDmaCaps_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 retval;

    // Start out with system specific DMA caps
    retval = pKernelBif->dmaCaps;

    // If noncoherent support is disabled, mask out SNOOP caps
    if (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_SUPPORT_NONCOHERENT))
    {
        retval &= ~DRF_SHIFTMASK(BIF_DMA_CAPS_NOSNOOP);
    }

    return retval;
}

/*!
 * @brief Initialize BIF static info in Kernel object through RPC
 *
 * @param[in] pGpu        GPU object pointer
 * @param[in] pKernelBif  BIF object pointer
 */
NV_STATUS
kbifStaticInfoInit_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NV2080_CTRL_INTERNAL_BIF_GET_STATIC_INFO_PARAMS *pStaticInfo;
    RM_API    *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS  status = NV_OK;

    // Allocate memory for the command parameter
    pStaticInfo = portMemAllocNonPaged(sizeof(*pStaticInfo));
    if (pStaticInfo == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Could not allocate pStaticInfo for KernelBif");
        status = NV_ERR_NO_MEMORY;
        goto kBifStaticInfoInit_IMPL_exit;
    }
    portMemSet(pStaticInfo, 0, sizeof(*pStaticInfo));

    // Send the command
    NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
                        pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                                        NV2080_CTRL_CMD_INTERNAL_BIF_GET_STATIC_INFO,
                                        pStaticInfo, sizeof(*pStaticInfo)),
                        kBifStaticInfoInit_IMPL_exit);

    // Initialize Kernel object fields with RPC response
    pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_PCIE_GEN4_CAPABLE,
                            pStaticInfo->bPcieGen4Capable);
    pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_IS_C2C_LINK_UP,
                            pStaticInfo->bIsC2CLinkUp);
    pKernelBif->dmaWindowStartAddress = pStaticInfo->dmaWindowStartAddress;

kBifStaticInfoInit_IMPL_exit:
    portMemFree(pStaticInfo);

    return status;
}

/*!
 * @brief Initialize PCI-E config space bits based on chipset and GPU support.
 */
void
kbifInitPcieDeviceControlStatus
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    OBJCL  *pCl  = SYS_GET_CL(pSys);

    kbifEnableExtendedTagSupport_HAL(pGpu, pKernelBif);

    //
    // Bug 382675 and 482867: Many SBIOSes default to disabling relaxed
    // ordering on GPUs, we want to always force it back on unless
    // the upstream root port is known to be broken with respect to this
    // feature.
    //
    if (!pCl->getProperty(pCl, PDB_PROP_CL_RELAXED_ORDERING_NOT_CAPABLE))
    {
        kbifPcieConfigEnableRelaxedOrdering_HAL(pGpu, pKernelBif);
    }
    else
    {
        kbifPcieConfigDisableRelaxedOrdering_HAL(pGpu, pKernelBif);
    }

    //
    // WAR for bug 3661529. All GH100 SKUs will need the NoSnoop WAR.
    // But currently GSP-RM does not detect this correctly,
    //
    if (IsGH100(pGpu))
    {
        pCl->setProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR, NV_TRUE);
    }

    if (!pCl->getProperty(pCl, PDB_PROP_CL_NOSNOOP_NOT_CAPABLE) &&
        !pCl->getProperty(pCl, PDB_PROP_CL_ROOTPORT_NEEDS_NOSNOOP_WAR))
    {
        // Bug 393398 - Re-enable DEVICE_CONTROL_STATUS_ENABLE_NO_SNOOP
        kbifEnableNoSnoop_HAL(pGpu, pKernelBif, NV_TRUE);
    }
    else
    {
        //
        // Check for NO_SNOOP P2P bug on specific chipset.  More info in bug 332764.
        // Check for NO_SNOOP enabled by default on specific CPU. Refer bug 1511622.
        //
        kbifEnableNoSnoop_HAL(pGpu, pKernelBif, NV_FALSE);
    }
}

/*!
 * @brief Check and rearm MSI
 *
 * @param[in]   pGpu          GPU object pointer
 * @param[in]   pKernelBif    BIF object pointer
 *
 * @return NV_TRUE   if MSI is enabled
 *         NV_FALSE  if MSI is disabled
 */
void
kbifCheckAndRearmMSI_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);

    if (kbifIsMSIEnabled(pGpu, pKernelBif))
    {
        if (!IS_VIRTUAL(pGpu))
        {
            // Send EOI to rearm
            if (pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_USE_CONFIG_SPACE_TO_REARM_MSI))
            {
                kbifRearmMSI_HAL(pGpu, pKernelBif);
            }
            else
            {
                intrRetriggerTopLevel_HAL(pGpu, pIntr);
            }
        }
    }
    else if (kbifIsMSIXEnabled(pGpu, pKernelBif))
    {
        intrRetriggerTopLevel_HAL(pGpu, pIntr);
    }
}

/*!
 * @brief Checks if MSI is enabled. Prefers to check the SW cache, but if
 * uncached, checks HW state and updates the SW cache for future use
 *
 * @param[in]   pGpu          GPU object pointer
 * @param[in]   pKernelBif    BIF object pointer
 *
 * @return NV_TRUE   if MSI is enabled
 *         NV_FALSE  if MSI is disabled
 */
NvBool
kbifIsMSIEnabled_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    //
    // Bug 418883: We shall rely upon value cached at boot, for the value
    // should not change during execution. If however, we must ever change
    // this back to be read at every ISR, we *must* read the value through
    // PCI CFG cycles.
    //
    if (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_MSI_CACHED))
    {
        if (kbifIsMSIEnabledInHW_HAL(pGpu, pKernelBif))
        {
            pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_IS_MSI_ENABLED, NV_TRUE);

            if (IS_VIRTUAL(pGpu))
            {
                // vGPU guests want an FYI print that re-arming is not required
                NV_PRINTF(LEVEL_WARNING,
                          "MSI is enabled for vGPU, but no need to re-ARM\n");
            }
        }
        else
        {
            pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_IS_MSI_ENABLED, NV_FALSE);
        }
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_IS_MSI_CACHED, NV_TRUE);
    }

    return pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_MSI_ENABLED);
}

/*!
 * @brief Checks if MSI-X is enabled. Prefers to check the SW cache, but if
 * uncached, checks HW state and updates the SW cache for future use
 *
 * @param[in]   pGpu          GPU object pointer
 * @param[in]   pKernelBif    BIF object pointer
 *
 * @return NV_TRUE   if MSI is enabled
 *         NV_FALSE  if MSI is disabled
 */
NvBool
kbifIsMSIXEnabled_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    if (!pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_MSIX_CACHED))
    {
        if (kbifIsMSIXEnabledInHW_HAL(pGpu, pKernelBif))
        {
            pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_IS_MSIX_ENABLED, NV_TRUE);
        }
        else
        {
            pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_IS_MSIX_ENABLED, NV_FALSE);
        }
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_IS_MSIX_CACHED, NV_TRUE);
    }
    return pKernelBif->getProperty(pKernelBif, PDB_PROP_KBIF_IS_MSIX_ENABLED);
}

/*!
 * @brief Clear PCIe HW PCIe config space error counters.
 * All of these should be cleared using config cycles.
 *
 * @param[in]   pGpu          GPU object pointer
 * @param[in]   pKernelBif    BIF object pointer
 */
void
kbifClearConfigErrors_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif,
    NvBool     bClearStatus,
    NvU32      xveAerFlagsMask
)
{
    NvU32 xveStatusFlags = 0;
    NvU32 xveStatus      = 0;
    NvU32 xveAerFlags    = 0;

    if ((bClearStatus) &&
        (kbifGetXveStatusBits_HAL(pGpu, pKernelBif, &xveStatusFlags, &xveStatus) == NV_OK) &&
        (xveStatusFlags != 0))
    {
        NV_PRINTF(LEVEL_WARNING, "PCI-E device status errors pending (%08X):\n",
                  xveStatusFlags);
#ifdef DEBUG
        if ( xveStatusFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_CORR_ERROR )
        {
            NV_PRINTF(LEVEL_WARNING, "     _CORR_ERROR_DETECTED\n");
        }
        if ( xveStatusFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_NON_FATAL_ERROR )
        {
            NV_PRINTF(LEVEL_WARNING, "     _NON_FATAL_ERROR_DETECTED\n");
        }
        if ( xveStatusFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_FATAL_ERROR )
        {
            NV_PRINTF(LEVEL_WARNING, "     _FATAL_ERROR_DETECTED\n");
        }
        if ( xveStatusFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_ERRORS_UNSUPP_REQUEST )
        {
            NV_PRINTF(LEVEL_WARNING, "     _UNSUPP_REQUEST_DETECTED\n");
        }
#endif
        NV_PRINTF(LEVEL_WARNING, "Clearing these errors..\n");
        kbifClearXveStatus_HAL(pGpu, pKernelBif, &xveStatus);
    }

    if ((xveAerFlagsMask) &&
        (kbifGetXveAerBits_HAL(pGpu, pKernelBif, &xveAerFlags) == NV_OK))
    {
        xveAerFlags &= xveAerFlagsMask;

        if (xveAerFlags != 0)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "PCI-E device AER errors pending (%08X):\n",
                      xveAerFlags);
#ifdef DEBUG
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_TRAINING_ERR)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_TRAINING_ERR\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_DLINK_PROTO_ERR)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_DLINK_PROTO_ERR\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_POISONED_TLP)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_POISONED_TLP\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_FC_PROTO_ERR)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_FC_PROTO_ERR\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_TIMEOUT)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_CPL_TIMEOUT\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_CPL_ABORT)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_CPL_ABORT\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNEXP_CPL)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_UNEXP_CPL\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_RCVR_OVERFLOW)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_RCVR_OVERFLOW\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_MALFORMED_TLP)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_MALFORMED_TLP\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_ECRC_ERROR)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_ECRC_ERROR\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_UNCORR_UNSUPPORTED_REQ)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_UNCORR_UNSUPPORTED_REQ\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RCV_ERR)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_CORR_RCV_ERR\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_TLP)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_CORR_BAD_TLP\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_BAD_DLLP)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_CORR_BAD_DLLP\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_ROLLOVER)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_CORR_RPLY_ROLLOVER\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_RPLY_TIMEOUT)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_CORR_RPLY_TIMEOUT\n");
            }
            if (xveAerFlags & NV2080_CTRL_BUS_INFO_PCIE_LINK_AER_CORR_ADVISORY_NONFATAL)
            {
                NV_PRINTF(LEVEL_WARNING, "     _AER_CORR_ADVISORY_NONFATAL\n");
            }
#endif
            NV_PRINTF(LEVEL_WARNING, "Clearing these errors..\n");
            kbifClearXveAer_HAL(pGpu, pKernelBif, xveAerFlags);
        }
    }
}

/*!
 * @brief The PCI bus family means it has the concept of bus/dev/func
 *        and compatible PCI config space.
 */
NvBool
kbifIsPciBusFamily_IMPL
(
    KernelBif *pKernelBif
)
{
    NvU32 busType = kbifGetBusIntfType_HAL(pKernelBif);

    return ((busType == NV2080_CTRL_BUS_INFO_TYPE_PCI) ||
            (busType == NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS) ||
            (busType == NV2080_CTRL_BUS_INFO_TYPE_FPCI));
}

/*!
 * @brief Regkey Overrides for Bif
 *
 * @param[in]   pGpu          GPU object pointer
 * @param[in]   pKernelBif    BIF object pointer
 */
static void
_kbifInitRegistryOverrides
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 data32;

    // P2P Override
    pKernelBif->p2pOverride = BIF_P2P_NOT_OVERRIDEN;
    if (osReadRegistryDword(pGpu, NV_REG_STR_CL_FORCE_P2P, &data32) == NV_OK)
    {
        pKernelBif->p2pOverride = data32;
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_READS_DISABLED, FLD_TEST_DRF(_REG_STR, _CL_FORCE_P2P, _READ, _DISABLE, data32));
        pKernelBif->setProperty(pKernelBif, PDB_PROP_KBIF_P2P_WRITES_DISABLED, FLD_TEST_DRF(_REG_STR, _CL_FORCE_P2P, _WRITE, _DISABLE, data32));
    }

    // P2P force type override
    pKernelBif->forceP2PType = NV_REG_STR_RM_FORCE_P2P_TYPE_DEFAULT;
    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_FORCE_P2P_TYPE, &data32) == NV_OK &&
        (data32 <= NV_REG_STR_RM_FORCE_P2P_TYPE_MAX))
    {
        pKernelBif->forceP2PType = data32;
    }

    // Peer Mapping override
    pKernelBif->peerMappingOverride = NV_REG_STR_PEERMAPPING_OVERRIDE_DEFAULT;
    if (osReadRegistryDword(pGpu, NV_REG_STR_PEERMAPPING_OVERRIDE, &data32) == NV_OK)
    {
        NV_PRINTF(LEVEL_INFO, "allow peermapping reg key = %d\n", data32);
        pKernelBif->peerMappingOverride = !!data32;
    }

}

/*!
 * Callback function to check if GPU exists
 *
 * @param[in]  pGpu    GPU object pointer
 * @param[in]  rsvd    Reserved  field
 */
static void
_kbifCheckIfGpuExists
(
    OBJGPU *pGpu,
    void   *rsvd
)
{
    if (FULL_GPU_SANITY_CHECK(pGpu))
    {
        if (gpuVerifyExistence_HAL(pGpu) != NV_OK)
        {
            osRemove1SecondRepeatingCallback(pGpu, _kbifCheckIfGpuExists, NULL);
        }
    }
}

NvU32
kbifGetGpuLinkCapabilities_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 addrLinkCap = 0;
    NvU32 data        = 0;

    if (NV_OK != kbifGetBusOptionsAddr_HAL(pGpu, pKernelBif, BUS_OPTIONS_LINK_CAPABILITIES, &addrLinkCap))
    {
        return 0;
    }

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, addrLinkCap, &data))
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read %x\n", addrLinkCap);
        return 0;
    }

    return data;
}

NvU32
kbifGetGpuLinkControlStatus_IMPL
(
    OBJGPU    *pGpu,
    KernelBif *pKernelBif
)
{
    NvU32 addrLinkControlStatus = 0;
    NvU32 data                  = 0;

    if (NV_OK != kbifGetBusOptionsAddr_HAL(pGpu, pKernelBif, BUS_OPTIONS_LINK_CONTROL_STATUS, &addrLinkControlStatus))
    {
        return 0;
    }

    if (NV_OK != GPU_BUS_CFG_RD32(pGpu, addrLinkControlStatus, &data ))
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to read %x\n", addrLinkControlStatus);
        return 0;
    }

    return data;
}

static NvBool
_doesBoardHaveMultipleGpusAndSwitch(OBJGPU *pGpu)
{
    if (((gpuIsMultiGpuBoard(pGpu)) ||
        (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_GEMINI)))&&
        ((pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_PLX_PRESENT))  ||
         (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR03_PRESENT)) ||
         (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR04_PRESENT))))
    {
        return NV_TRUE;
    }
    else
    {
        return NV_FALSE;
    }
}

NV_STATUS
kbifControlGetPCIEInfo_IMPL
(
    OBJGPU               *pGpu,
    KernelBif            *pKernelBif,
    NV2080_CTRL_BUS_INFO *pBusInfo
)
{
    OBJSYS *pSys  = SYS_GET_INSTANCE();
    OBJCL  *pCl   = SYS_GET_CL(pSys);
    NvU32   index = pBusInfo->index;
    NvU32   data  = 0;

    if (kbifGetBusIntfType_HAL(pKernelBif) != NV2080_CTRL_BUS_INFO_TYPE_PCI_EXPRESS)
    {
        // KMD cannot handle error codes for this ctrl call, hence returning
        // NV_OK, once KMD fixes the bug:3545197, RM can return NV_ERR_NOT_SUPPORTED
        return NV_OK;
    }

    switch (index)
    {
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CAPS:
        {
            data = kbifGetGpuLinkCapabilities(pGpu, pKernelBif);
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_LINK_CAPS:
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_LINK_CAPS:
        {
            if (_doesBoardHaveMultipleGpusAndSwitch(pGpu))
            {
                if (clPcieReadPortConfigReg(pGpu, pCl,
                                            &pGpu->gpuClData.boardUpstreamPort,
                                            CL_PCIE_LINK_CAP, &data) != NV_OK)
                {
                    data = 0;
                }
            }
            else
            {
                data = kbifGetGpuLinkCapabilities(pGpu, pKernelBif);
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_GEN_INFO:
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_GEN_INFO:
        {
            NvU32 temp;

            if (_doesBoardHaveMultipleGpusAndSwitch(pGpu))
            {
                if (clPcieReadPortConfigReg(pGpu, pCl,
                                            &pGpu->gpuClData.boardUpstreamPort,
                                            CL_PCIE_LINK_CTRL_STATUS, &temp) != NV_OK)
                {
                    data = 0;
                    break;
                }
                else
                {
                    temp = REF_VAL(NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED, temp);
                    if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_64000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _CURR_LEVEL, _GEN6, data);
                    }
                    else if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_32000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _CURR_LEVEL, _GEN5, data);
                    }
                    else if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_16000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _CURR_LEVEL, _GEN4, data);
                    }
                    else if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_8000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _CURR_LEVEL, _GEN3, data);
                    }
                    else if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CTRL_STATUS_LINK_SPEED_5000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _CURR_LEVEL, _GEN2, data);
                    }
                    else
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _CURR_LEVEL, _GEN1, data);
                    }
                }

                if (clPcieReadPortConfigReg(pGpu, pCl,
                                            &pGpu->gpuClData.boardUpstreamPort,
                                            CL_PCIE_LINK_CAP, &temp) != NV_OK)
                {
                    data = 0;
                    break;
                }
                else
                {
                    temp = REF_VAL(NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED, temp);
                    if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_64000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _GEN, _GEN6, data);
                    }
                    else if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_32000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _GEN, _GEN5, data);
                    }
                    else if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_16000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _GEN, _GEN4, data);
                    }
                    else if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_8000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _GEN, _GEN3, data);
                    }
                    else if (temp == NV2080_CTRL_BUS_INFO_PCIE_LINK_CAP_MAX_SPEED_5000MBPS)
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _GEN, _GEN2, data);
                    }
                    else
                    {
                        data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_PCIE_LINK_CAP,
                                           _GEN, _GEN1, data);
                    }
                }
            }
            else
            {
                if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
                {
                    NV2080_CTRL_BUS_INFO busInfo = {0};
                    NV_STATUS rmStatus = NV_OK;

                    busInfo.index = NV2080_CTRL_BUS_INFO_INDEX_PCIE_GEN_INFO;

                    if ((rmStatus = kbusSendBusInfo(pGpu, GPU_GET_KERNEL_BUS(pGpu), &busInfo)) != NV_OK)
                    {
                        NV_PRINTF(LEVEL_INFO, "Squashing rmStatus: %x \n", rmStatus);
                        rmStatus = NV_OK;
                        busInfo.data = 0;
                    }
                    data = busInfo.data;
                }
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_CAPS:
        {
            if (clPcieReadPortConfigReg(pGpu, pCl,
                                        &pGpu->gpuClData.rootPort,
                                        CL_PCIE_LINK_CAP, &data) != NV_OK)
            {
                data = 0;
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_DOWNSTREAM_LINK_CAPS:
        {
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR03_PRESENT) ||
                pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR04_PRESENT))
            {
                if (clPcieReadPortConfigReg(pGpu, pCl,
                                            &pGpu->gpuClData.boardDownstreamPort,
                                            CL_PCIE_LINK_CAP, &data) != NV_OK)
                {
                    data = 0;
                }
            }
            else
            {
                // no br03/br04, same as link from RC
                if (clPcieReadPortConfigReg(pGpu, pCl,
                                            &pGpu->gpuClData.rootPort,
                                            CL_PCIE_LINK_CAP, &data) != NV_OK)
                {
                    data = 0;
                }
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_CTRL_STATUS:
        {
            data = kbifGetGpuLinkControlStatus(pGpu, pKernelBif);
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_UPSTREAM_LINK_CTRL_STATUS:
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_BOARD_LINK_CTRL_STATUS:
        {
            if (_doesBoardHaveMultipleGpusAndSwitch(pGpu))
            {
                if (clPcieReadPortConfigReg(pGpu, pCl,
                                            &pGpu->gpuClData.boardUpstreamPort,
                                            CL_PCIE_LINK_CTRL_STATUS, &data) != NV_OK)
                {
                    data = 0;
                }
            }
            else
            {
                data = kbifGetGpuLinkControlStatus(pGpu, pKernelBif);
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_CTRL_STATUS:
        {
            if (clPcieReadPortConfigReg(pGpu, pCl,
                                        &pGpu->gpuClData.rootPort,
                                        CL_PCIE_LINK_CTRL_STATUS,
                                        &data) != NV_OK)
            {
                data = 0;
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_DOWNSTREAM_LINK_CTRL_STATUS:
        {
            if (pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR03_PRESENT) ||
                pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_BR04_PRESENT))
            {
                if (clPcieReadPortConfigReg(pGpu, pCl,
                                            &pGpu->gpuClData.boardDownstreamPort,
                                            CL_PCIE_LINK_CTRL_STATUS, &data) != NV_OK)
                {
                    data = 0;
                }
            }
            else
            {
                // no br03/br04, same as link from RC
                if (clPcieReadPortConfigReg(pGpu, pCl,
                                            &pGpu->gpuClData.rootPort,
                                            CL_PCIE_LINK_CTRL_STATUS,
                                            &data) != NV_OK)
                {
                    data = 0;
                }
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_ERRORS:
        {
            NvU32 xveStatus = 0;

            if (pKernelBif != NULL)
            {
                if (kbifGetXveStatusBits_HAL(pGpu, pKernelBif, &data, &xveStatus) != NV_OK)
                {
                    data = 0;
                    break;
                }
                if (kbifClearXveStatus_HAL(pGpu, pKernelBif, &xveStatus) != NV_OK)
                {
                    data = 0;
                }
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_ROOT_LINK_ERRORS:
        {
            NvU32 clStatus = 0;

            if (clPcieReadDevCtrlStatus(pGpu, pCl, &data, &clStatus) != NV_OK)
            {
                data = 0;
                break;
            }
            if (clPcieClearDevCtrlStatus(pGpu, pCl, &clStatus) != NV_OK)
            {
                data = 0;
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_PCIE_GPU_LINK_AER:
        {
            if (pKernelBif != NULL)
            {
                if (kbifGetXveAerBits_HAL(pGpu, pKernelBif, &data) != NV_OK)
                {
                    data = 0;
                    break;
                }
                if (kbifClearXveAer_HAL(pGpu, pKernelBif, data) != NV_OK)
                {
                    data = 0;
                }
            }
            break;
        }
        case NV2080_CTRL_BUS_INFO_INDEX_MSI_INFO:
        {
            if (kbifIsMSIEnabledInHW_HAL(pGpu, pKernelBif))
            {
                data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_MSI,
                                   _STATUS, _ENABLED, data);
            }
            else
            {
                data = FLD_SET_DRF(2080, _CTRL_BUS_INFO_MSI,
                                   _STATUS, _DISABLED, data);
            }
            break;
        }

        default:
            break;
    }

    pBusInfo->data = data;
    return NV_OK;
}

/*!
 * @brief To ensure GPU is back on bus and accessible by polling device ID
 *
 * @param[in]  pGpu        GPU object pointer
 * @param[in]  pKernelBif  Kernel BIF object pointer
 *
 * @returns NV_OK
 * @returns NV_ERR_TIMEOUT
 */
NV_STATUS
kbifPollDeviceOnBus_IMPL
(
    OBJGPU     *pGpu,
    KernelBif  *pKernelBif
)
{
    RMTIMEOUT timeout;

    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);

    while (osPciInitHandle(gpuGetDomain(pGpu),
                           gpuGetBus(pGpu),
                           gpuGetDevice(pGpu), 0, NULL, NULL) == NULL)
    {
        if (gpuCheckTimeout(pGpu, &timeout) == NV_ERR_TIMEOUT)
        {
            NV_PRINTF(LEVEL_ERROR, "Timeout polling GPU back on bus\n");
            DBG_BREAKPOINT();
            return NV_ERR_TIMEOUT;
        }
        osDelayUs(100);
    }

    return NV_OK;
}

