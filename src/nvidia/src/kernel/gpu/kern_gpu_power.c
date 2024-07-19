/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"
#include "gpu/kern_gpu_power.h"
#include "os/os.h"
#include "nvRmReg.h"
#include "diagnostics/journal.h"
#include "diagnostics/tracer.h"
#include "jt.h"
#include "ctrl/ctrl2080/ctrl2080power.h"
#include "gpu/subdevice/subdevice.h"

#include "g_journal_pb.h"


// Polling loop delays
#define GC6_PWR_CHECK_DELAY_MS         5
#define GC6_BUS_CHECK_DELAY_MS         1

//
// Static functions
//
static NV_STATUS _gpuGc6EntrySanityCheck(OBJGPU *, NV2080_CTRL_GC6_ENTRY_PARAMS *);
static void      _gpuGc6EntrySwStateUpdate(OBJGPU *, NV2080_CTRL_GC6_ENTRY_PARAMS *);
static void      _gpuGc6EntryFailed(OBJGPU *);
static NV_STATUS _gpuGc6EntryStateUnload(OBJGPU *);
static NV_STATUS _gpuGc6ExitSanityCheck(OBJGPU *);
static NV_STATUS _gpuGc6ExitGpuPowerOn(OBJGPU *, NV2080_CTRL_GC6_EXIT_PARAMS *);
static NV_STATUS _gpuGc6ExitStateLoad(OBJGPU *, NV2080_CTRL_GC6_EXIT_PARAMS *);
static void      _gpuGc6ExitSwStateRestore(OBJGPU *);
static void      _gpuForceGc6inD3Hot(OBJGPU *, NV2080_CTRL_GC6_EXIT_PARAMS *);

/*!
 * @brief: This HAL function executes the steps of GC6 exit sequence
 *
 * @param[in]     pGpu          GPU object pointer
 * @param[in,out] pParams       Parameter struct
 *
 * @return
 *    NV_OK                     Success
 *    NV_ERR_GENERIC            Generic error (bad state or stuck)
 */
NV_STATUS
gpuGc6Exit_IMPL(
    OBJGPU *pGpu,
    NV2080_CTRL_GC6_EXIT_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;

    // Check if we need to execute GPU_ON step
    if (GPU_IS_GC6_STEP_ID_SET(GPU_OFF, pGpu->gc6State.executedStepMask))
    {
         // Sanity error checking
        status = _gpuGc6ExitSanityCheck(pGpu);
        if (status != NV_OK)
        {
            goto gpuGc6Exit_IMPL_exit;
        }

        // GPU power on
        status = _gpuGc6ExitGpuPowerOn(pGpu, pParams);
        if (status != NV_OK)
        {
            goto gpuGc6Exit_IMPL_exit;
        }

        // Set the GPU state as GC6_STATE_EXITING
        SET_GPU_GC6_STATE_AND_LOG(pGpu, GPU_GC6_STATE_EXITING);

        // GPU power on
        status = _gpuGc6ExitStateLoad(pGpu, pParams);
        if (status != NV_OK)
        {
            goto gpuGc6Exit_IMPL_exit;
        }

        // Set the GPU state as GC6_STATE_EXITED
        SET_GPU_GC6_STATE_AND_LOG(pGpu, GPU_GC6_STATE_EXITED);
        NV_PRINTF(LEVEL_INFO, "GPU is now in D0 state.\n");

        // Restore SW state
        _gpuGc6ExitSwStateRestore(pGpu);

    }

    // Check if we need to execute SR_EXIT step
    if (GPU_IS_GC6_STEP_ID_SET(SR_ENTRY, pGpu->gc6State.executedStepMask))
    {

    }

gpuGc6Exit_IMPL_exit:
    // Reset the executed step mask
    pGpu->gc6State.executedStepMask = 0;

    return status;
}

/*!
 * @brief: This HAL function executes the steps of GC6 entry sequence
 *
 * @param[in]     pGpu          GPU object pointer
 * @param[in,out] pParams       Parameter struct
 *
 * @return
 *    NV_OK                     Success
 *    NV_ERR_INVALID_ARGUMENT   Invalid action
 *    NV_ERR_NOT_SUPPORTED      Chip doesn't support the feature
 *    NV_ERR_NOT_READY          Chip not ready to execute operation
 *    NV_ERR_INVALID_STATE      Chip unable to execute command at current state
 *    NV_ERR_TIMEOUT            RM timed out when trying to complete call
 *    NV_ERR_GENERIC            Generic error (bad state or stuck)
 */
NV_STATUS
gpuGc6Entry_IMPL(
    OBJGPU *pGpu,
    NV2080_CTRL_GC6_ENTRY_PARAMS *pParams
)
{
    NV_STATUS status    = NV_OK;

    // Check if we need to execute GPU_OFF step
    if (GPU_IS_GC6_STEP_ID_SET(GPU_OFF, pParams->stepMask))
    {
         // Sanity error checking
        status = _gpuGc6EntrySanityCheck(pGpu, pParams);
        if (status != NV_OK)
        {
            goto gpuGc6Entry_IMPL_exit;
        }

        // SW state update
        _gpuGc6EntrySwStateUpdate(pGpu, pParams);

        // Set the GPU state as GC6_STATE_ENTERING
        SET_GPU_GC6_STATE_AND_LOG(pGpu, GPU_GC6_STATE_ENTERING);

        // RM state unload
        status = _gpuGc6EntryStateUnload(pGpu);
        if (status != NV_OK)
        {
            goto gpuGc6Entry_IMPL_exit;
        }

        // Sanity check p-state is in P8
        status = gpuGc6EntryPstateCheck(pGpu);
        if (status != NV_OK)
        {
            _gpuGc6EntryFailed(pGpu);
            goto gpuGc6Entry_IMPL_exit;
        }

        // GPU power off
        status = gpuGc6EntryGpuPowerOff(pGpu);
        if (status != NV_OK)
        {
            goto gpuGc6Entry_IMPL_exit;
        }

        // Set the GPU state as GC6_STATE_ENTERED
        SET_GPU_GC6_STATE_AND_LOG(pGpu, GPU_GC6_STATE_ENTERED);
        NV_PRINTF(LEVEL_INFO, "GPU is now in GC6 state.\n");

        // Add this step in executed step mask
        pGpu->gc6State.executedStepMask |= NVBIT(NV2080_CTRL_GC6_STEP_ID_GPU_OFF);
    }

gpuGc6Entry_IMPL_exit:
    return status;
}

/*!
 * @brief: Helper function for doing powering down GPU during GC6 entry
 *
 * @param[in] pGpu OBJGPU pointer
 *
 * @return
 *    NV_OK   Success
 *    NV_ERR  Otherwise
 */
NV_STATUS
gpuGc6EntryGpuPowerOff_IMPL(OBJGPU *pGpu)
{
    NV_STATUS status = NV_OK;

    // SW pre-settings before calling power off.
    NV_ASSERT_OK_OR_RETURN(gpuPrePowerOff_HAL(pGpu));

    if ((pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_ACTIVE) &&
         !pGpu->acpiMethodData.jtMethodData.bSBIOSCaps))
    {
        status = NV_OK;
        NV_PRINTF(LEVEL_INFO,
                  "Skip call to power off GPU in OSPM RTD3\n");
        goto _gpuGc6EntryGpuPowerOff_EXIT;
    }

    // call on all GC6 systems that contain SBIOS JT caps
    status = gpuPowerOff(pGpu);

_gpuGc6EntryGpuPowerOff_EXIT:
    if (status != NV_OK)
    {
        _gpuGc6EntryFailed(pGpu);
        NV_PRINTF(LEVEL_ERROR, "Call to power off GPU failed.\n");
        DBG_BREAKPOINT();
    }

    return status;
}

/*!
 * @brief Power off GPU and enter GC6 state.
 *
 * This function implements last phase of GC6 entry sequence as defined in
 * JT Architecture Specification.
 *
 * @param[in] pGpu      GPU object pointer
 *
 * @return  NV_OK
 *     If GPU has entered (or is entering) GC6 state.
 *
 * @return other   Bubbles up errors from
 *                 @ref osGC6PowerControl on failure.
 */
NV_STATUS
gpuPowerOff_KERNEL(OBJGPU *pGpu)
{

    NvU32 deferCmd = 0x134c02;

    if (!pGpu->acpiMethodData.jtMethodData.bSBIOSCaps &&
         pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_ACTIVE))
    {
        return NV_OK;
    }

    NV_ASSERT_OK_OR_RETURN(
        osGC6PowerControl(pGpu, deferCmd, NULL));

    return NV_OK;
}

/*!
 * @brief Power on GPU
 *
 * This function implements first phase of GC6 exit sequence as defined in
 * JT Architecture Specification.
 *
 * @param[in] pGpu      GPU object pointer
 *
 * @return  NV_OK
 *     If GPU showed up on the bus.
 *
 * @return  NV_ERR_TIMEOUT
 *     If timeout occurred while waiting for GPU to show up on the bus.
 *
 * @return other   Bubbles up errors from @ref osGC6PowerControl on failure.
 */
NV_STATUS
gpuPowerOn_KERNEL(OBJGPU *pGpu)
{
    NV_STATUS status;
    NvU32     powerStatus;
    RMTIMEOUT timeout;

    // We do not make any _DSM calls for OSPM + RTD3
    if (!pGpu->acpiMethodData.jtMethodData.bSBIOSCaps &&
         pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_ACTIVE))
    {
        return NV_OK;
    }

    //
    // Pick the correct command to send to the _DSM
    // If we are on MsHybrid, the OS has already made the call to power on GPU
    //
    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_MSHYBRID_GC6_ACTIVE))
    {
        // clear the deferred entry/exit state in the SBIOS
        NV_ASSERT_OK_OR_RETURN(
            osGC6PowerControl(pGpu, 0x8000, &powerStatus));
    }

    //
    // wait for SBIOS to report that GPU is powered on. When the GPU is reported
    // as "powered on" both the below conditions are true
    // 1. The Pex Link is Powered Up
    // 2. EC reports PWR_OK to be asserted.
    //
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    while (NV_TRUE)
    {
        NV_ASSERT_OK_OR_RETURN(
            osGC6PowerControl(pGpu, 0x0, &powerStatus));

        if (FLD_TEST_DRF(_JT_FUNC, _POWERCONTROL, _GPU_POWER_STATE, _PWOK, powerStatus))
        {
            break;
        }

        status = gpuCheckTimeout(pGpu, &timeout);
        if (status == NV_ERR_TIMEOUT)
        {
            RMTRACE_GPU(_GC6_POWERON_ERR_TO, pGpu->gpuId, 0, 0, 0, 0, 0, 0, 0);
            NV_PRINTF(LEVEL_ERROR,
                      "Timeout waiting for GPU to enter PWOK/ON state.Current State %x\n",
                      powerStatus);
            DBG_BREAKPOINT();
            return status;
        }

        // For same reasons as above, loosen the polling loop.
        osDelay(GC6_PWR_CHECK_DELAY_MS);
    }

    //
    // There is a possibility that GPU hasn't yet appeared on the bus even
    // though the link is up. Wait here for the GPU to appear on the bus before
    // proceeding. Note, it's not recommended to poll on the device config
    // space till the pex link in the root port is restored, but since link
    // should be up by the time OS_GC6_GET_GPU_POWER_STATE returns STATUS_ON
    // it is safe to do so here. We want to be absolutely sure that the GPU
    // is on the bus before we proceed and restore the GPU config space.
    //
    if (!gpuIsOnTheBus(pGpu))
    {
        NV_PRINTF(LEVEL_WARNING,
                  "GPU is not yet on the bus after GC6 power-up.\n");

        gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
        while (!gpuIsOnTheBus(pGpu))
        {
            status = gpuCheckTimeout(pGpu, &timeout);
            if (status == NV_ERR_TIMEOUT)
            {
                RMTRACE_GPU(_GC6_POWERON_ERR_BUS_TO, pGpu->gpuId, 0, 0, 0, 0, 0, 0, 0);
                NV_PRINTF(LEVEL_ERROR,
                          "Timeout waiting for GPU to appear on the bus.\n");
                DBG_BREAKPOINT();
                return status;
            }

            // For same reasons as above, loosen the polling loop.
            osDelay(GC6_BUS_CHECK_DELAY_MS);
        }
    }

    return NV_OK;
}

/*!
 * @brief Check if GPU is on the bus.
 *
 * @param[in]      pGpu    GPU object pointer
 *
 * @return 'NV_TRUE' if GPU is on the bus; 'NV_FALSE' otherwise.
 */
NvBool
gpuIsOnTheBus_IMPL(OBJGPU *pGpu)
{
    NvU32   domain = gpuGetDomain(pGpu);
    NvU8    bus    = gpuGetBus(pGpu);
    NvU8    device = gpuGetDevice(pGpu);
    NvU16   vendorId, deviceId;
    void   *handle;

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        //
        // TODO: This is a temporary WAR. GSP-RM should not be doing any config access,
        //       especially for Blackwell where config mirror is no longer available.
        //       This functionality is just to verify if GPU is on the bus,
        //       so we can either assume that in case of physical RM calling this function,
        //       GPU will always be on the bus or we need to figure out a different way
        //       to verify the same. To reach to any conclusion we need further discussion and
        //       analysis, hence adding this WAR temporarily. Bug 4315004.
        //
        return NV_TRUE;
    }

    handle = osPciInitHandle(domain, bus, device, 0, &vendorId, &deviceId);
    return (handle != NULL);
}

/*!
 * @brief: Helper function for sanity error checking during GC6 entry
 *
 * @param[in]   pGpu       OBJGPU pointer
 * @param[in]   pParams    Pointer to RmCtrl parameters
 *
 * @return
 *    NV_OK                   Success
 *    NV_ERR                  Otherwise
 */
static NV_STATUS
_gpuGc6EntrySanityCheck(OBJGPU *pGpu, NV2080_CTRL_GC6_ENTRY_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;

    //
    // Sanity check RTD3 enablement
    // TODO: replace with PDB that indicates system support of RTD3, not just the chip
    //
    if (pParams->params.bIsRTD3Transition &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Cannot perform RTD3 as chip does not support.\n");
        DBG_BREAKPOINT();
        return NV_ERR_NOT_SUPPORTED;
    }

    if (IS_GPU_GC6_STATE_ENTERED(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU is already in GC6 state or stuck in transition.\n");
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    // Wait for GPU to be ready for GC6
    status = gpuWaitGC6Ready_HAL(pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPU is not ready to transition from D0 to GC6 state.\n");
        DBG_BREAKPOINT();
    }

    return status;
}

/*!
 * @brief: Helper function for updating SW state during GC6 entry
 *
 * @param[in]   pGpu       OBJGPU pointer
 * @param[in]   pParams    Pointer to RmCtrl parameters
 */
static void
_gpuGc6EntrySwStateUpdate(OBJGPU *pGpu, NV2080_CTRL_GC6_ENTRY_PARAMS *pParams)
{

    // set state if we are doing an MsHybrid GC6 cycle OR Fast GC6 cycle
    if (pParams->flavorId == NV2080_CTRL_GC6_FLAVOR_ID_MSHYBRID)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_MSHYBRID_GC6_ACTIVE, NV_TRUE);
    }

    // set state if we are doing a RTD3 GC6 cycle
    if (pParams->params.bIsRTD3Transition)
    {
        pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_ACTIVE, NV_TRUE);

    }
}

/*!
 * @brief: Helper function for restoring SW state during failed GC6 entry
 *
 * @param[in]   pGpu       OBJGPU pointer
 */
static void
_gpuGc6EntryFailed(OBJGPU *pGpu)
{
    SET_GPU_GC6_STATE_AND_LOG(pGpu, GPU_GC6_STATE_ENTERING_FAILED);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_MSHYBRID_GC6_ACTIVE, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_FAST_GC6_ACTIVE, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_ACTIVE, NV_FALSE);
}

/*!
 * @brief: Helper function for doing RM state unload during GC6 entry
 *
 * @param[in]   pGpu       OBJGPU pointer
 *
 * @return
 *    NV_OK                   Success
 *    NV_ERR                  Otherwise
 */
static NV_STATUS
_gpuGc6EntryStateUnload(OBJGPU *pGpu)
{
    NV_STATUS status;

    pGpu->setProperty(pGpu, PDB_PROP_GPU_FORCE_PERF_BIOS_LEVEL, NV_TRUE);
    status = gpuEnterStandby(pGpu);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_FORCE_PERF_BIOS_LEVEL, NV_FALSE);

    if (status != NV_OK)
    {
        _gpuGc6EntryFailed(pGpu);
        NV_PRINTF(LEVEL_ERROR,
                  "GPU is unable to transition from D0 to GC6 state.\n");
        DBG_BREAKPOINT();
    }

    return status;
}

/*!
 * @brief: Helper function for sanity error checking during GC6 exit
 *
 * @param[in]   pGpu       OBJGPU pointer
 *
 * @return
 *    NV_OK                   Success
 *    NV_ERR                  Otherwise
 */
static NV_STATUS
_gpuGc6ExitSanityCheck(OBJGPU *pGpu)
{
    NV_STATUS status = NV_OK;


    // Make sure that the current GC6 state is GPU_STATE_ENTERED_GC6
    if (!IS_GPU_GC6_STATE_ENTERED(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "GPU is not in GC6 state.\n");
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    return status;
}

/*!
 * @brief: Helper function for doing powering up GPU during GC6 exit
 *
 * @param[in]   pGpu       OBJGPU pointer
 * @param[in]   pParams    Pointer to RmCtrl parameters
 *
 * @return
 *    NV_OK                   Success
 *    NV_ERR                  Otherwise
 */
static NV_STATUS
_gpuGc6ExitGpuPowerOn(OBJGPU *pGpu, NV2080_CTRL_GC6_EXIT_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;

    return status;
}

/*!
 * @brief: Helper function for restoring SW state during GC6 exit
 *
 * @param[in]   pGpu       OBJGPU pointer
 */
static void
_gpuGc6ExitSwStateRestore(OBJGPU *pGpu)
{
    pGpu->setProperty(pGpu, PDB_PROP_GPU_MSHYBRID_GC6_ACTIVE, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_FAST_GC6_ACTIVE, NV_FALSE);
    pGpu->setProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_ACTIVE, NV_FALSE);
}

/*!
 * @brief: Helper function to force GC6 cycle in this D3 hot exit
 *
 * @param[in]   pGpu       OBJGPU pointer
 */
static void
_gpuForceGc6inD3Hot(OBJGPU *pGpu, NV2080_CTRL_GC6_EXIT_PARAMS *pParams)
{

    pParams->params.bIsRTD3HotTransition = NV_FALSE;
    //
    // If this was a D3Hot cycle (no power down), force a GC6 cycle
    // This is due to instability of some memory types with D3Hot cycles
    // Skip this on RTD3 cycles since we won't have the _DSMs to force cycles
    // Summarize the scenario that needs to do _DSM reset
    // 1. It's OSPM but not RTD3.
    // 2. GPU power was not cut.
    // 3. The D3Hot WAR is enabled
    // Note _FORCE_D3HOT works only on NVPM
    //

    //
    // Enable BAR0 accesses here for RTLSIM, EMU, and silicon MFG MODS.
    // FMODEL will not be reset, so no need to enable.
    // Windows will be pci bus driver to restore, but below function will check before enabling.
    // Only enable BAR0 accesses here,
    // and leave whole restore steps in gpuPowerManagementResumePreLoadPhysical().
    //
    if (!IS_FMODEL(pGpu) && IS_GPU_GC6_STATE_EXITING(pGpu))
    {
        NV_STATUS tempStatus = kbifPollDeviceOnBus(pGpu, GPU_GET_KERNEL_BIF(pGpu));

        if (tempStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Enable BAR0 accesses failed.\n");
        }
    }

    // The power remains is necessary for d3hot
    if (!gpuCompletedGC6PowerOff_HAL(pGpu)
        )
    {
        pParams->params.bIsRTD3HotTransition = NV_TRUE;

        NV_PRINTF(LEVEL_ERROR,
                  "D3Hot detected. Going to recover from D3Hot\n");

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_CHIP_SUPPORTS_RTD3_DEF)
            )
        {
            if (!pGpu->getProperty(pGpu, PDB_PROP_GPU_RTD3_GC6_ACTIVE))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "D3Hot case for Turing and later but legacy GC6/FGC6 flavor.\n");
            }
        }
        // none RTD3 D3Hot and none legacy D3Hot WAR
        else
        {
            NV_PRINTF(LEVEL_ERROR,
                      "D3Hot case for none RTD3 and no WAR enabled.\n");
        }

    } // End of if (!gpuCompletedGC6PowerOff_HAL(pGpu))
}

/*!
 * @brief: Helper function for doing RM state load during GC6 exit
 *
 * @param[in]   pGpu       OBJGPU pointer
 * @param[in]   pParams    Pointer to RmCtrl parameters
 *
 * @return
 *    NV_OK                   Success
 *    NV_ERR                  Otherwise
 */
static NV_STATUS
_gpuGc6ExitStateLoad(OBJGPU *pGpu, NV2080_CTRL_GC6_EXIT_PARAMS *pParams)
{
    NV_STATUS status = NV_OK;

    pGpu->setProperty(pGpu, PDB_PROP_GPU_FORCE_PERF_BIOS_LEVEL, NV_TRUE);

    // Force GC6 cycle if this D3 hot
    _gpuForceGc6inD3Hot(pGpu, pParams);

    status = gpuResumeFromStandby(pGpu);

    pGpu->setProperty(pGpu, PDB_PROP_GPU_FORCE_PERF_BIOS_LEVEL, NV_FALSE);

    if (status != NV_OK)
    {
        SET_GPU_GC6_STATE_AND_LOG(pGpu, GPU_GC6_STATE_EXITING_FAILED);
        NV_PRINTF(LEVEL_ERROR,
                  "GPU is unable to transition from GC6 to D0 state.\n");
        DBG_BREAKPOINT();
    }

    return status;
}

/*!
 * @brief: This command executes the steps of GC6 entry sequence
 *
 * @param[in]  pSubDevice
 * @param[in]  pParams
 *
 * @return
 *    NV_OK                     Success
 *    NV_ERR_INVALID_ARGUMENT   Invalid action
 *    NV_ERR_NOT_SUPPORTED      Chip doesn't support the feature
 *    NV_ERR_NOT_READY          Chip not ready to execute operation
 *    NV_ERR_INVALID_STATE      Chip unable to execute command at current state
 *    NV_ERR_TIMEOUT            RM timed out when trying to complete call
 *    NV_ERR_GENERIC            Generic error (bad state or stuck)
 */
NV_STATUS
subdeviceCtrlCmdGc6Entry_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GC6_ENTRY_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    return gpuGc6Entry(pGpu, pParams);
}

/*!
 * @brief: This command executes the steps of GC6 exit sequence
 *
 * @param[in]  pSubDevice
 * @param[in]  pParams
 *
 * @return
 *    NV_OK                     Success
 *    NV_ERR_GENERIC            Generic error (bad state or stuck)
 */
NV_STATUS
subdeviceCtrlCmdGc6Exit_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_GC6_EXIT_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    return gpuGc6Exit(pGpu, pParams);
}
