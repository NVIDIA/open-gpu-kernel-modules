/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "soe/soe_nvswitch.h"
#include "soe/soe_priv_nvswitch.h"
#include "soe/soebif.h"
#include "rmlsfm.h"

#include "nvlink_export.h"
#include "common_nvswitch.h"
#include "lr10/lr10.h"
#include "lr10/soe_lr10.h"
#include "soe/bin/g_soeuc_lr10_dbg.h"
#include "soe/bin/g_soeuc_lr10_prd.h"
#include "soe/soeifcmn.h"
#include "nvswitch/lr10/dev_soe_ip.h"
#include "nvswitch/lr10/dev_soe_ip_addendum.h"
#include "nvswitch/lr10/dev_falcon_v4.h"
#include "nvswitch/lr10/dev_nvlsaw_ip.h"
#include "nvswitch/lr10/dev_therm.h"
#include "regkey_nvswitch.h"

#include "flcn/flcnable_nvswitch.h"
#include "flcn/flcn_nvswitch.h"

#include "rmflcncmdif_nvswitch.h"

/*
 * @Brief : Reset SOE at the engine level.
 *
 * @param[in] device Reset SOE on this device
 */
static NvlStatus
_nvswitch_reset_soe
(
    nvswitch_device *device
)
{
    NvU32 value;

    // Assert reset
    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _ENGINE);
    value = FLD_SET_DRF(_SOE, _FALCON, _ENGINE_RESET, _TRUE, value);
    NVSWITCH_SOE_WR32_LR10(device, 0, _SOE_FALCON, _ENGINE, value);

    //
    // TODO: Track down correct delay, if any.
    // Currently GPU does not enforce a delay, use 1ms for now.
    //
    nvswitch_os_sleep(1);

    // Unassert reset
    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _ENGINE);
    value = FLD_SET_DRF(_SOE, _FALCON, _ENGINE_RESET, _FALSE, value);
    NVSWITCH_SOE_WR32_LR10(device, 0, _SOE_FALCON, _ENGINE, value);

    // Wait for reset to complete
    if (flcnWaitForResetToFinish_HAL(device, device->pSoe->pFlcn) != NV_OK)
    {
        // TODO: Fix up NV_STATUS translation, anything but NV_OK is a failure.
        return NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Copy the soe ucode to IMEM and DMEM and write soe ucode entrypoint
 *          to boot vector register.
 *
 * @param[in] device  Copy ucode to this device's SOE
 */
static NvlStatus
_nvswitch_soe_copy_ucode_cpubitbang
(
    nvswitch_device                *device,
    const NvU32                    *soe_ucode_data,
    const NvU32                    *soe_ucode_header
)
{
    const PSOE_UCODE_HDR_INFO_LR10 pUcodeHeader =
        (PSOE_UCODE_HDR_INFO_LR10) &(soe_ucode_header[0]);

    NvU32 dataSize, data, i, appCount;
    NvU32 appCodeStartOffset, appCodeSize, appCodeImemOffset;
    NvU32 appDataStartOffset, appDataSize, appDataDmemOffset;
    NvU32 appCodeIsSecure;
    NvU16 tag;
    FLCN *pFlcn = device->pSoe->pFlcn;

    dataSize = sizeof(soe_ucode_data[0]);

    // Initialize address of IMEM to 0x0 and set auto-increment on write
    data = 0;
    data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_IMEMC, _OFFS, 0x0, data);
    data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_IMEMC, _BLK, 0x0, data);
    data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_IMEMC, _AINCW, 0x1, data);
    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_IMEMC(0), data);

    for (appCount = 0; appCount < pUcodeHeader -> numApps; appCount++)
    {
        appCodeStartOffset = pUcodeHeader -> apps[appCount].appCodeStartOffset;
        appCodeSize = pUcodeHeader -> apps[appCount].appCodeSize;
        appCodeImemOffset = pUcodeHeader -> apps[appCount].appCodeImemOffset;
        appCodeIsSecure = pUcodeHeader -> apps[appCount].appCodeIsSecure;
        appDataStartOffset = pUcodeHeader -> apps[appCount].appDataStartOffset;
        appDataSize = pUcodeHeader -> apps[appCount].appDataSize;
        appDataDmemOffset = pUcodeHeader -> apps[appCount].appDataDmemOffset;

        if(appCodeSize > 0)
        {
            // Mark the following code as secure or unsecure
            data = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IMEMC(0));
            data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_IMEMC, _SECURE, appCodeIsSecure, data);
            flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_IMEMC(0), data);
            // Initialize IMEM tag.
            // Writes to IMEM don't work if we don't do this
            tag = (NvU16)(appCodeImemOffset >> 8);
            flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_IMEMT(0), (NvU32) tag);

            // Copy over IMEM part of the ucode and tag along the way
            for (i = 0; i < (appCodeSize / dataSize); i++)
            {
                // Increment tag for after every block (256 bytes)
                if (i && ((i % ((256/dataSize))) == 0))
                {
                    tag++;
                    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_IMEMT(0), (NvU32) tag);
                }

                // Copy IMEM DWORD by DWORD
                data = soe_ucode_data[(appCodeStartOffset / dataSize) + i];
                flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_IMEMD(0), data);
            }
        }

        if(appDataSize > 0)
        {
            // Initialize address of DMEM to appDataDmemOffset and set auto-increment on write
            data = 0;
            data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMEMC, _OFFS, (appDataDmemOffset&0xFF), data);
            data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMEMC, _BLK, appDataDmemOffset>>8, data);
            data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMEMC, _AINCW, 0x1, data);
            flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_DMEMC(0), data);

            // Copy over DMEM part of the ucode
            for (i = 0; i < (appDataSize / dataSize); i++)
            {
                // Copy DMEM DWORD by DWORD
                data = soe_ucode_data[appDataStartOffset/dataSize + i];
                flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_DMEMD(0), data);
            }
        }
    }

    //
    // In this ucode load path, we bit bang, we do not use DMA,
    // so set REQUIRE_CTX to FALSE. This must be set before we start SOE.
    //
    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_DMACTL,
                     DRF_NUM(_PFALCON_FALCON, _DMACTL, _REQUIRE_CTX, NV_FALSE));

    // Write soe ucode entrypoint to boot vector register
    data = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_BOOTVEC);
    data = FLD_SET_DRF_NUM(_PFALCON, _FALCON_BOOTVEC, _VEC, pUcodeHeader->codeEntryPoint, data);
    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_BOOTVEC, data);

    return NVL_SUCCESS;
}

/*
 * @Brief : Send a command to pFlcn for testing (this function is temporary)
 *
 * @param   device  The nvswitch device
 * @param   pFlcn   The flcn
 */
static NV_STATUS
_nvswitch_soe_send_test_cmd
(
    nvswitch_device *device
)
{
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout;
    NvU32               cmdSeqDesc;
    NV_STATUS           status;

    FLCN *pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_NULL;
    // sending nothing but a header for UNIT_NULL
    cmd.hdr.size   = RM_FLCN_QUEUE_HDR_SIZE;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 5, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&cmd,
                                NULL,   // pMsg             - not used for now
                                NULL,   // pPayload         - not used for now
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_ASSERT(status == NV_OK);
        return status;
    }

    return status;
}

static NvlStatus
_nvswitch_get_soe_ucode_binaries
(
    nvswitch_device *device,
    const NvU32 **soe_ucode_data,
    const NvU32 **soe_ucode_header
)
{
    NvU32 debug_mode;

    if (!soe_ucode_data || !soe_ucode_header)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: SOE get ucode binaries BadArgs!\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    debug_mode = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_SCP, _CTL_STAT);
    debug_mode = DRF_VAL(_SOE, _SCP_CTL_STAT, _DEBUG_MODE, debug_mode);

    if (debug_mode)
    {
        *soe_ucode_data = soe_ucode_data_lr10_dbg;
        *soe_ucode_header = soe_ucode_header_lr10_dbg;
    }
    else
    {
        *soe_ucode_data = soe_ucode_data_lr10_prd;
        *soe_ucode_header = soe_ucode_header_lr10_prd;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Load soe ucode image into SOE Falcon
 *
 * @param   device            The nvswitch device
 */
static NvlStatus
_nvswitch_load_soe_ucode_image
(
    nvswitch_device *device
)
{
    NvlStatus status;
    const NvU32 *soe_ucode_data;
    const NvU32 *soe_ucode_header;

    status = _nvswitch_get_soe_ucode_binaries(device, &soe_ucode_data, &soe_ucode_header);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get SOE ucode binaries!\n",
            __FUNCTION__);
        return status;
    }

    status = _nvswitch_soe_copy_ucode_cpubitbang(device, soe_ucode_data,
                                                     soe_ucode_header);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unable to copy SOE ucode!\n",
            __FUNCTION__);
        return status;
    }

    return status;
}

/*
 * @Brief : Bootstrap SOE
 *
 * @param[in] device Bootstrap SOE on this device
 */
static NvlStatus
_nvswitch_soe_bootstrap
(
    nvswitch_device *device
)
{
    NVSWITCH_TIMEOUT timeout;
    NvU32 data;
    FLCN *pFlcn;

    // POR requires SOE, no SOE, total failure.
    if (!NVSWITCH_ENG_IS_VALID(device, SOE, 0))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: SOE is not present, failing driver load.\n",
            __FUNCTION__);
        return -NVL_INITIALIZATION_TOTAL_FAILURE;
    }

    pFlcn = device->pSoe->pFlcn;

    //
    // Start the SOE Falcon
    //
    data = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_CPUCTL);
    data = FLD_SET_DRF(_PFALCON, _FALCON_CPUCTL, _STARTCPU, _TRUE, data);
    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_CPUCTL, data);

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 20, &timeout);

    //
    // We will exit this if we recieve bootstrap signal OR
    // if we timeout waiting for bootstrap signal       OR
    // if bootstrap fails
    //
    while (1)
    {
        data = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_MAILBOX1);
        if (data == SOE_BOOTSTRAP_SUCCESS)
        {
            pFlcn->engDescUc.initialized = NV_TRUE;
            return NVL_SUCCESS;
        }

        //
        // Check if SOE has halted unexpectedly.
        //
        // The explicit check is required because the interrupts
        // are not yet enabled as the device is still initializing.
        //
        if (soeIsCpuHalted_HAL(device, ((PSOE)pFlcn->pFlcnable)))
        {
            break;
        }

        if (nvswitch_timeout_check(&timeout))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for SOE to complete bootstrap!"
                "NV_PFALCON_FALCON_MAILBOX1 = 0x%08x\n",
                __FUNCTION__, data);
            NVSWITCH_ASSERT(0);
            break;
        }
    }

    NVSWITCH_PRINT(device, SETUP,
            "%s: Failed to bootstrap SOE.\n",
            __FUNCTION__);

    // Log any failures SOE may have had during bootstrap
    (void)soeService_HAL(device, ((PSOE)pFlcn->pFlcnable));
    return -NVL_ERR_INVALID_STATE;
}

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
/*!
 * Helper function to dump some registers for debug.
 *
 * @param[in]  device  nvswitch_device pointer
 */
static void
dumpDebugRegisters
(
    nvswitch_device *device
)
{
    NvU32    value;

    // Mail boxes and CPU control
    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _MAILBOX0);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_MAILBOX0: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _MAILBOX1);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_MAILBOX1: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _CPUCTL);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_CPUCTL: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _DEBUGINFO);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_DEBUGINFO: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _EXCI);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_EXCI: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _SCTL);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_SCTL: 0x%x\n",
                   __FUNCTION__,
                   value);

    // Legacy steering and interrupts
    value = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _STEER_INTR_LEGACY);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_NVLSAW_NVSPMC_STEER_INTR_LEGACY: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _INTR_SOE_EN_LEGACY);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_NVLSAW_NVSPMC_INTR_SOE_EN_LEGACY: 0x%x\n",
                   __FUNCTION__,
                   value);

    // Correctable steering and interrupts
    value = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _STEER_INTR_CORRECTABLE);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_NVLSAW_NVSPMC_STEER_INTR_CORRECTABLE: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _INTR_SOE_EN_CORRECTABLE);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_NVLSAW_NVSPMC_INTR_SOE_EN_CORRECTABLE: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SAW_RD32_LR10(device, _NVLSAW_NVSPMC, _INTR_SOE_LEGACY);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_NVLSAW_NVSPMC_INTR_SOE_LEGACY: 0x%x\n",
                   __FUNCTION__,
                   value);

    // EXTIO interrupts
    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_MISC, _EXTIO_IRQSTAT);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_MISC_EXTIO_IRQSTAT: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_MISC, _EXTIO_IRQMASK);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_MISC_EXTIO_IRQMASK: 0x%x\n",
                   __FUNCTION__,
                   value);


    // Falcon top level interrupts
    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _IRQSTAT);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_IRQSTAT: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _IRQSTAT_ALIAS);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_IRQSTAT_ALIAS: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _IRQMODE);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_IRQMODE: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _IRQMASK);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_IRQMASK: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _IRQDEST);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_IRQDEST: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _IRQDEST2);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_IRQDEST2: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _IRQSCMASK);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: NV_SOE_FALCON_IRQSCMASK: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_REG_RD32(device, _THERM, _MSGBOX_COMMAND);
    NVSWITCH_PRINT(device, ERROR,
                   "%s: MSGBOX_COMMAND: 0x%x\n",
                   __FUNCTION__,
                   value);

    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _RESET_PRIV_LEVEL_MASK);
    NVSWITCH_PRINT(device, ERROR,
                "%s: NV_SOE_FALCON_RESET_PRIV_LEVEL_MASK: 0x%x\n",
                __FUNCTION__,
                value);
    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _IRQTMR_PRIV_LEVEL_MASK);
    NVSWITCH_PRINT(device, ERROR,
                "%s: NV_SOE_FALCON_IRQTMR_PRIV_LEVEL_MASK: 0x%x\n",
                __FUNCTION__,
                value);
    value = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _EXE_PRIV_LEVEL_MASK);
    NVSWITCH_PRINT(device, ERROR,
                "%s: NV_SOE_FALCON_EXE_PRIV_LEVEL_MASK: 0x%x\n",
                __FUNCTION__,
                value);
}
#endif // defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)

/*
 * @Brief : Request SOE GFW image to exit and halt
 *
 *     i)   Query for SOE firmware validation state.
 *     ii)  Request for SOE to exit and halt.
 *     iii) Wait fot IFR to complete and exit by halting SOE.
 */
static NvlStatus
_nvswitch_soe_request_gfw_image_halt
(
    nvswitch_device *device
)
{
    NvU32 val;
    NVSWITCH_TIMEOUT timeout;
    FLCN* pFlcn = device->pSoe->pFlcn;

    //
    // Poll for firmware boot state.
    // GFW takes around 150ms to finish it's sequence.
    //
    nvswitch_timeout_create(1000 * NV_GFW_SOE_EXIT_AND_HALT_TIMEOUT, &timeout);
    do
    {
        val = NVSWITCH_REG_RD32(device, _GFW, _SOE_BOOT);
        if (FLD_TEST_DRF(_GFW, _SOE_BOOT, _PROGRESS, _COMPLETED, val) &&
            !FLD_TEST_DRF(_GFW, _SOE_BOOT, _VALIDATION_STATUS, _IN_PROGRESS, val))
        {
            break;
        }

        if (nvswitch_timeout_check(&timeout))
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_TIMEOUT,
                "SOE reset timeout error(0)\n");
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for SOE GFW boot to complete. rc = 0x%x.\n",
                __FUNCTION__, val);
            return -NVL_ERR_INVALID_STATE;
        }

        nvswitch_os_sleep(5);
    } while (NV_TRUE);

    // Check for firmware validation status.
    if (!FLD_TEST_DRF(_GFW, _SOE_BOOT, _VALIDATION_STATUS,
        _PASS_TRUSTED, val))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: SOE Firmware validation failed. rc = 0x%x\n",
            __FUNCTION__, val);
        return -NVL_ERR_INVALID_STATE;
    }

    // Request SOE GFW image to exit and halt.
    val = NVSWITCH_REG_RD32(device, _GFW, _SOE_EXIT_AND_HALT);
    val = FLD_SET_DRF(_GFW, _SOE_EXIT_AND_HALT, _REQUESTED, _YES, val);
    NVSWITCH_REG_WR32(device, _GFW, _SOE_EXIT_AND_HALT, val);

    //
    // Wait for SOE to halt.
    //
    do
    {
        val = flcnRegRead_HAL(device, pFlcn, NV_SOE_FALCON_CPUCTL);
        if (FLD_TEST_DRF(_SOE, _FALCON_CPUCTL, _HALTED, _TRUE, val))
        {
            NVSWITCH_PRINT(device, INFO,
                "%s: Handshake with SOE GFW successful.\n",
                __FUNCTION__);
            break;
        }

        if (nvswitch_timeout_check(&timeout))
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_TIMEOUT,
                "SOE reset timeout error(1)\n");
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for SOE GFW image to exit and halt.\n",
                __FUNCTION__);
            return -NVL_ERR_INVALID_STATE;
        }

        nvswitch_os_sleep(5);
    } while (NV_TRUE);

    return NVL_SUCCESS;
}

void
nvswitch_soe_unregister_events_lr10
(
    nvswitch_device *device
)
{
    PFLCN pFlcn = device->pSoe->pFlcn;
    PSOE   pSoe  = (PSOE)device->pSoe;
    NV_STATUS status;

    // un-register thermal callback funcion
    status = flcnQueueEventUnregister(device, pFlcn,
                                      pSoe->thermEvtDesc);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to un-register thermal event handler.\n",
            __FUNCTION__);
    }
}

/*
 * @Brief : Register callback functions for events
 *          and messages from SOE.
 */
NvlStatus
nvswitch_soe_register_event_callbacks_lr10
(
    nvswitch_device *device
)
{
    PFLCN pFlcn = device->pSoe->pFlcn;
    PSOE   pSoe  = (PSOE)device->pSoe;
    NV_STATUS status;

    // Register Thermal callback funcion
    status = flcnQueueEventRegister(
                 device, pFlcn,
                 RM_SOE_UNIT_THERM,
                 NULL,
                 nvswitch_therm_soe_callback_lr10,
                 NULL,
                 &pSoe->thermEvtDesc);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to register thermal event handler.\n",
            __FUNCTION__);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Request SOE old driver image to provide L0 write permissions
 *          for reset registers to perform reset and boot up the new image.
 */
static NvlStatus
_nvswitch_soe_request_reset_permissions
(
    nvswitch_device *device
)
{
    NVSWITCH_TIMEOUT timeout;
    NvU32 reset_plm, engctl_plm;

    // Request reset access.
    NVSWITCH_REG_WR32(device, _SOE, _RESET_SEQUENCE,
        DRF_DEF(_SOE, _RESET_SEQUENCE, _REQUESTED, _YES));

    // Poll on reset PLMs.
    nvswitch_timeout_create(20 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
    do
    {
        // Verify if SOE has given L0 write access for reset registers.
        reset_plm = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _RESET_PRIV_LEVEL_MASK);
        engctl_plm = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE_FALCON, _ENGCTL_PRIV_LEVEL_MASK);

        if (FLD_TEST_DRF(_SOE_FALCON, _RESET_PRIV_LEVEL_MASK,  _WRITE_PROTECTION_LEVEL0, _ENABLE, reset_plm) &&
            FLD_TEST_DRF(_SOE_FALCON, _ENGCTL_PRIV_LEVEL_MASK, _WRITE_PROTECTION_LEVEL0, _ENABLE, engctl_plm))
        {
            NVSWITCH_PRINT(device, INFO,
                "%s: Got write access for reset registers from SOE.\n",
                __FUNCTION__);
            break;
        }

        if (nvswitch_timeout_check(&timeout))
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_TIMEOUT,
                "SOE reset timeout error(2)\n");
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for SOE to provide write access for reset registers.\n",
                __FUNCTION__);

            NVSWITCH_PRINT(device, ERROR,
                "%s: NV_SOE_FALCON_RESET_PRIV_LEVEL_MASK = 0x%x, NV_SOE_FALCON_ENGCTL_PRIV_LEVEL_MASK = 0x%x.\n",
                __FUNCTION__, reset_plm, engctl_plm);

            return -NVL_ERR_INVALID_STATE;
        }

        nvswitch_os_sleep(1);
    } while (NV_TRUE);

    return NVL_SUCCESS;
}

/*
 * @Brief : Execute SOE pre-reset sequence for secure reset.
 */
static NvlStatus
_nvswitch_soe_prepare_for_reset
(
    nvswitch_device *device
)
{
    NvlStatus status;
    NvU32 val;

    if (IS_FMODEL(device) || IS_RTLSIM(device) || IS_EMULATION(device))
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: Skipping SOE pre-reset sequence on pre-silicon.\n",
            __FUNCTION__);
        return NVL_SUCCESS;
    }

    val = NVSWITCH_REG_RD32(device, _GFW, _SOE_PROGRESS_CODE);
    if (!FLD_TEST_DRF(_GFW, _SOE_PROGRESS_CODE, _VALUE, _COMPLETED, val))
    {
        // Request SOE GFW to exit and halt.
        status = _nvswitch_soe_request_gfw_image_halt(device);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: GFW shutdown request failed!\n",
                __FUNCTION__);
        }
    }
    else
    {
        //
        // The SOE image from previous driver load needs to be reset.
        // Request reset permissions from that SOE image to perform the reset.
        //
        status = _nvswitch_soe_request_reset_permissions(device);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: SOE reset request failed!\n",
                __FUNCTION__);
        }
    }

    return status;
}

/*
 * @Brief : Bootstrap SOE on the specified device
 *
 * @param[in] device Bootstrap SOE on this device
 */
NvlStatus
nvswitch_init_soe_lr10
(
    nvswitch_device *device
)
{
    NvlStatus status;

    // Prepare SOE for reset.
    status = _nvswitch_soe_prepare_for_reset(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_RESET,
            "Failed to reset SOE(0)\n");
        return status;
    }

    // Reset SOE
    status = _nvswitch_reset_soe(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_RESET,
            "Failed to reset SOE(1)\n");
        return status;
    }

    // Load SOE
    status = _nvswitch_load_soe_ucode_image(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_BOOTSTRAP,
            "Failed to boot SOE(0)\n");
        return status;
    }

    // Start SOE
   status = _nvswitch_soe_bootstrap(device);
   if (status != NVL_SUCCESS)
   {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_BOOTSTRAP,
            "Failed to boot SOE(1)\n");
        return status;
    }

    // Sanity the command and message queues as a final check
    if (_nvswitch_soe_send_test_cmd(device) != NV_OK)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_BOOTSTRAP,
            "Failed to boot SOE(2)\n");

#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
        dumpDebugRegisters(device);
#endif // defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)

        return -NVL_ERR_INVALID_STATE;
    }

    // Register SOE callbacks
    status = nvswitch_soe_register_event_callbacks(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_COMMAND_QUEUE,
            "Failed to register SOE events\n");
        return status;
    }

    NVSWITCH_PRINT(device, SETUP,
                   "%s: SOE successfully bootstrapped.\n",
                   __FUNCTION__);

    return status;
}

void
nvswitch_soe_init_l2_state_lr10
(
    nvswitch_device *device
)
{
    NVSWITCH_PRINT(device, WARN,
        "%s: Function not implemented on lr10\n",
        __FUNCTION__);
}

/**
 * @brief  SOE construct
 *
 * @param[in] device            nvswitch_device  pointer
 * @param[in] pFlcnable         SOE pointer
 *
 * @return NV_OK
 */
static NV_STATUS
_soeConstruct_LR10
(
    nvswitch_device *device,
    FLCNABLE        *pFlcnable
)
{
    SOE                *pSoe  = (PSOE)pFlcnable;
    FLCN               *pFlcn = ENG_GET_FLCN(pFlcnable);
    PFALCON_QUEUE_INFO  pQueueInfo;
    NV_STATUS           status;

    NVSWITCH_ASSERT(pFlcn != NULL);

    //
    // Set SOE specific Falcon state
    // This is where any default Falcon state should be overridden if necessary.
    //
    pFlcn->name                    = "SOE";
    pFlcn->pFlcnable               = pFlcnable;
    pFlcn->bQueuesEnabled          = NV_TRUE;
    pFlcn->numQueues               = SOE_QUEUE_NUM;
    pFlcn->numSequences            = RM_SOE_MAX_NUM_SEQUENCES;
    pFlcn->bEmemEnabled            = NV_TRUE;
    pFlcn->engineTag               = ENG_TAG_SOE;

    nvswitch_os_memset(pSoe->seqInfo, 0, sizeof(pSoe->seqInfo));

    // Do the HAL dependent init for Falcon
    status = flcnConstruct_HAL(device, pFlcn);

    pQueueInfo = pFlcn->pQueueInfo;
    NVSWITCH_ASSERT(pQueueInfo != NULL);
    NVSWITCH_ASSERT(pQueueInfo->pQueues != NULL);

    //
    // Fill in the Message Queue handling details
    //
    pQueueInfo->maxUnitId          = RM_SOE_UNIT_END;
    pQueueInfo->maxMsgSize         = sizeof(RM_FLCN_MSG_SOE);
    pQueueInfo->initEventUnitId    = RM_SOE_UNIT_INIT;

    return status;
}

/**
 * @brief  SOE destruct
 *
 * @param[in] device            nvswitch_device  pointer
 * @param[in] pFlcnable         SOE pointer
 */
static void
_soeDestruct_LR10
(
    nvswitch_device *device,
    FLCNABLE        *pFlcnable
)
{
    flcnDestruct_HAL(device, ENG_GET_FLCN(pFlcnable));
}

 /*!
 * @brief Sets up the external configuration for accessing registers,etc.
 *
 * @param[in] device         nvswitch_device pointer
 * @param[in] pSoe           FLCNABLE pointer
 * @param[in] pConfig        FALCON_EXTERNAL_CONFIG pointer
 *
 * @returns void.
 */
static void
_soeGetExternalConfig_LR10
(
    nvswitch_device        *device,
    FLCNABLE               *pSoe,
    PFALCON_EXTERNAL_CONFIG pConfig
)
{
    PFLCN               pFlcn = ENG_GET_FLCN(pSoe);
    PFALCON_QUEUE_INFO  pQueueInfo;

    NVSWITCH_ASSERT(pFlcn != NULL);

    pConfig->bResetInPmc    = NV_TRUE;
    pConfig->fbifBase       = NV_SOE_FBIF_TRANSCFG(0);

    pQueueInfo = pFlcn->pQueueInfo;
    NVSWITCH_ASSERT(pQueueInfo != NULL);

    // Populate the falcon queue details
    pQueueInfo->cmdQHeadSize        = NV_SOE_QUEUE_HEAD__SIZE_1;
    pQueueInfo->cmdQTailSize        = NV_SOE_QUEUE_TAIL__SIZE_1;
    pQueueInfo->msgQHeadSize        = NV_SOE_MSGQ_HEAD__SIZE_1;
    pQueueInfo->msgQTailSize        = NV_SOE_MSGQ_TAIL__SIZE_1;

    pQueueInfo->cmdQHeadBaseAddress = NV_SOE_QUEUE_HEAD(0);
    pQueueInfo->cmdQHeadStride      = NV_SOE_QUEUE_HEAD(1) - NV_SOE_QUEUE_HEAD(0);
    pQueueInfo->cmdQTailBaseAddress = NV_SOE_QUEUE_TAIL(0);
    pQueueInfo->cmdQTailStride      = NV_SOE_QUEUE_TAIL(1) - NV_SOE_QUEUE_TAIL(0);
    pQueueInfo->msgQHeadBaseAddress = NV_SOE_MSGQ_HEAD(0);
    pQueueInfo->msgQTailBaseAddress = NV_SOE_MSGQ_TAIL(0);

    pQueueInfo->maxCmdQueueIndex    = SOE_RM_CMDQ_LOG_ID__LAST;
}

/*!
 * @brief Top level service routine
 *
 * @param[in] device         nvswitch_device  pointer
 * @param[in] pSoe           SOE  pointer
 *
 * @return 32-bit interrupt status AFTER all known interrupt-sources were
 *         serviced.
 */
static NvU32
_soeService_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    NvBool  bRecheckMsgQ    = NV_FALSE;
    NvU32   clearBits       = 0;
    NvU32   intrStatus;
    PFLCN   pFlcn  = ENG_GET_FLCN(pSoe);

    if (pFlcn == NULL)
    {
        NVSWITCH_ASSERT(0);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Get the IRQ status and mask the sources not directed to host.
    intrStatus = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQSTAT) &
                 flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQMASK) &
                 flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQDEST);

    // Exit if there is nothing to do
    if (intrStatus == 0)
    {
       return 0;
    }

    // Service pending interrupts
    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _WDTMR, _TRUE))
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_WATCHDOG,
            "SOE Watchdog error\n");
        NVSWITCH_PRINT(device, INFO,
                    "%s: Watchdog timer fired. We do not support this "
                    "yet.\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);

        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _WDTMR, _SET);
    }

    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _EXTERR, _TRUE))
    {
        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _EXTERR, _SET);

        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_EXTERR, "SOE EXTERR\n");
        soeServiceExterr_HAL(device, pSoe);
    }

    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _HALT, _TRUE))
    {
        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _HALT, _SET);

        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_SOE_HALT, "SOE HALTED\n");
        soeServiceHalt_HAL(device, pSoe);
    }

    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _SWGEN0, _TRUE))
    {
        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _SWGEN0, _SET);

        NVSWITCH_PRINT(device, INFO,
                    "%s: Received a message from SOE via SWGEN0\n",
                    __FUNCTION__);
        soeProcessMessages_HAL(device, pSoe);
        bRecheckMsgQ = NV_TRUE;
    }

    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _SWGEN1, _TRUE))
    {
        clearBits |= DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _SWGEN1, _SET);

        NVSWITCH_PRINT(device, INFO,
                    "%s: Received a SWGEN1 interrupt\n",
                    __FUNCTION__);
    }

    // Clear any sources that were serviced and get the new status.
    flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQSCLR, clearBits);

    // Re-read interrupt status before retriggering to return correct value
    intrStatus = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQSTAT) &
                 flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQMASK) &
                 flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQDEST);

    //
    // If we just processed a SWGEN0 message queue interrupt, peek
    // into the message queue and see if any messages were missed the last time
    // the queue was purged (above). If it is not empty, re-generate SWGEN0
    // (since it is now cleared) and exit. As long as an interrupt is pending,
    // this function will be re-entered and the message(s) will be processed.
    //
    if (bRecheckMsgQ)
    {
        PFALCON_QUEUE_INFO      pQueueInfo;
        FLCNQUEUE              *pMsgQ;

        pQueueInfo = pFlcn->pQueueInfo;

        NVSWITCH_ASSERT(pQueueInfo != NULL);
        NVSWITCH_ASSERT(pQueueInfo->pQueues != NULL);

        pMsgQ = &pQueueInfo->pQueues[SOE_RM_MSGQ_LOG_ID];

        if (!pMsgQ->isEmpty(device, pFlcn, pMsgQ))
        {
           // It is not necessary to RMW IRQSSET (zeros are ignored)
           flcnRegWrite_HAL(device, pFlcn, NV_PFALCON_FALCON_IRQSSET,
                            DRF_DEF(_PFALCON, _FALCON_IRQSSET, _SWGEN0, _SET));
        }
    }

    flcnIntrRetrigger_HAL(device, pFlcn);

    return intrStatus;
}

/*!
 * Called by soeService to handle a SOE halt. This function will dump the
 * current status of SOE and then trap the CPU for further inspection for a
 * debug build.
 *
 * @param[in]  device  nvswitch_device pointer
 * @param[in]  pSoe    SOE object pointer
 */
static void
_soeServiceHalt_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    PFLCN    pFlcn = ENG_GET_FLCN(pSoe);
    NvU32    value;

    NVSWITCH_PRINT(device, ERROR,
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                "!!                   ** SOE HALTED **                !!\n"
                "!! Please file a bug with the following information. !!\n"
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");

    // TODO: Break out the register dumps to specific hals
#if defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)
        dumpDebugRegisters(device);
        flcnDbgInfoCapturePcTrace_HAL(device, pFlcn);
#endif // defined(DEVELOP) || defined(DEBUG) || defined(NV_MODS)

    //
    // If the halt was related to security, we store the information in
    // MAILBOX0. Print out an error that clearly indicates the reason for the
    // halt.
    //
    value = flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_MAILBOX0);

    if (value == LSF_FALCON_MODE_TOKEN_FLCN_INSECURE)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "SOE HAS HALTED BECAUSE IT IS NOT RUNNING IN "
                    "SECURE MODE\n");
    }

    NVSWITCH_ASSERT(0);
}

/*!
 * Depending on the direction of the copy, copies 'sizeBytes' to/from 'pBuf'
 * from/to DMEM offset dmemAddr. Note the below statement about dmemAddr.
 * The address must be located in the EMEM region located directly above the
 * maximum virtual address of DMEM.
 *
 * @param[in]   device      nvswitch_device pointer
 * @param[in]   pSoe        SOE pointer
 * @param[in]   dmemAddr    The DMEM address for the copy
 * @param[out]  pBuf        pPointer to the buffer containing the data to copy
 * @param[in]   sizeBytes   The number of bytes to copy from EMEM
 * @param[in]   port        EMEM port
 * @param[in]   bCopyFrom   Boolean representing the copy direction (to/from EMEM)
 */
static void
_soeEmemTransfer_LR10
(
    nvswitch_device *device,
    PSOE             pSoe,
    NvU32            dmemAddr,
    NvU8            *pBuf,
    NvU32            sizeBytes,
    NvU8             port,
    NvBool           bCopyFrom
)
{
    NvU32       numWords;
    NvU32       numBytes;
    NvU32      *pData = (NvU32 *)pBuf;
    NvU32       startEmem;
    NvU32       endEmem;
    NvU32       reg32;
    NvU32       i;
    NvU32       ememCOffset;
    NvU32       ememDOffset;
    NvU32       maxEmemPorts = soeEmemPortSizeGet_HAL(device, pSoe);
    PFLCN       pFlcn        = ENG_GET_FLCN(pSoe);
    NV_STATUS status;

    if (pFlcn == NULL)
    {
        NVSWITCH_ASSERT(pFlcn != NULL);
        return;
    }

    status = soeEmemPortToRegAddr_HAL(device, pSoe, port, &ememCOffset, &ememDOffset);
    if (status != NV_OK)
    {
        NVSWITCH_ASSERT(status == NV_OK);
        return;
    }

    // Simply return if the copy-size is zero
    if (sizeBytes == 0)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: zero-byte copy requested.\n", __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return;
    }

    // The source must be 4-byte aligned
    if (!NV_IS_ALIGNED(dmemAddr, FLCN_DMEM_ACCESS_ALIGNMENT))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Address is not 4-byte aligned. dmemAddr=0x%08x\n",
             __FUNCTION__, dmemAddr);
        NVSWITCH_ASSERT(0);
        return;
    }

    // Check the port. Only one port for SOE LR10.
    if (port >= maxEmemPorts)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: only %d ports supported. Accessed port=%d\n",
            __FUNCTION__, maxEmemPorts, port);
        NVSWITCH_ASSERT(0);
        return;
    }

    //
    // Verify that the dmemAddr address is located in EMEM, above addressable DMEM,
    // and that the copy does not overshoot the end of EMEM.
    //
    startEmem = soeGetEmemStartOffset_HAL(device, pSoe);

    // END_EMEM = START_EMEM + SIZE_EMEM (the size of EMEM is given in blocks)
    endEmem = startEmem + soeGetEmemSize_HAL(device, pSoe);

    if (dmemAddr < startEmem || (dmemAddr + sizeBytes) > endEmem)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: copy must be in EMEM aperature [0x%x, 0x%x)\n",
            __FUNCTION__, startEmem, endEmem);
        NVSWITCH_ASSERT(0);
        return;
    }

    // Convert to EMEM offset for use by EMEMC/EMEMD
    dmemAddr -= startEmem;

    // Calculate the number of words and bytes
    numWords = sizeBytes >> 2;
    numBytes = sizeBytes & 0x3;

    // Mask off all but the OFFSET and BLOCK in EMEM offset
    reg32 = dmemAddr & (DRF_SHIFTMASK(NV_SOE_EMEMC_OFFS) |
                   DRF_SHIFTMASK(NV_SOE_EMEMC_BLK));

    if (bCopyFrom)
    {
        // mark auto-increment on read
        reg32 = FLD_SET_DRF(_SOE, _EMEMC, _AINCR, _TRUE, reg32);
    }
    else
    {
        // mark auto-increment on write
        reg32 = FLD_SET_DRF(_SOE, _EMEMC, _AINCW, _TRUE, reg32);
    }
    flcnRegWrite_HAL(device, pFlcn, ememCOffset, reg32);

    // Directly copy as many words as possible
    for (i = 0; i < numWords; i++)
    {
        if (bCopyFrom)
        {
            pData[i] = flcnRegRead_HAL(device, pFlcn, ememDOffset);
        }
        else
        {
            flcnRegWrite_HAL(device, pFlcn, ememDOffset, pData[i]);
        }
    }

    // Check if there are leftover bytes to copy
    if (numBytes > 0)
    {
        NvU32 bytesCopied = numWords << 2;

        //
        // Read the contents first. If we're copying to the EMEM, we've set
        // autoincrement on write, so reading does not modify the pointer. We
        // can, thus, do a read/modify/write without needing to worry about the
        // pointer having moved forward. There is no special explanation needed
        // if we're copying from the EMEM since this is the last access to HW
        // in that case.
        //
        reg32 = flcnRegRead_HAL(device, pFlcn, ememDOffset);
        if (bCopyFrom)
        {
            for (i = 0; i < numBytes; i++)
            {
                pBuf[bytesCopied + i] = ((NvU8 *)&reg32)[i];
            }
        }
        else
        {
            for (i = 0; i < numBytes; i++)
            {
                ((NvU8 *)&reg32)[i] = pBuf[bytesCopied + i];
            }
            flcnRegWrite_HAL(device, pFlcn, ememDOffset, reg32);
        }
    }
}

/*!
 * Get the EMEM size in bytes
 *
 * @param[in]  device      nvswitch_device pointer
 * @param[in]  pSoe        SOE pointer
 */
static NvU32
_soeGetEmemSize_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    NvU32 data = flcnRegRead_HAL(device, ENG_GET_FLCN(pSoe), NV_SOE_HWCFG);
    return DRF_VAL(_SOE, _HWCFG, _EMEM_SIZE, data) * FLCN_BLK_ALIGNMENT;
}

/*!
 * Get the EMEM start offset in DMEM VA space
 *
 * @param[in]  device      nvswitch_device pointer
 * @param[in]  pSoe        SOE pointer
 */
static NvU32
_soeGetEmemStartOffset_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    //
    // EMEM is mapped at the top of DMEM VA space
    // START_EMEM = DMEM_VA_MAX = 2^(DMEM_TAG_WIDTH + 8)
    //
    NvU32 data = flcnRegRead_HAL(device, ENG_GET_FLCN(pSoe), NV_SOE_FALCON_HWCFG1);
    return (1 << (DRF_VAL(_SOE, _FALCON_HWCFG1, _DMEM_TAG_WIDTH, data) + 8));
}

/*!
 * Get the EMEMC/D register addresses for the specified port
 *
 * @param[in]  device      nvswitch_device pointer
 * @param[in]  pSoe        SOE pointer
 * @param[in]  port        EMEM port number
 * @param[out] pEmemCAddr  BAR0 address of the specified EMEMC port
 * @param[out] pEmemDAddr  BAR0 address of the specified EMEMD port
 */
static NV_STATUS
_soeEmemPortToRegAddr_LR10
(
    nvswitch_device *device,
    PSOE             pSoe,
    NvU32            port,
    NvU32           *pEmemCAddr,
    NvU32           *pEmemDAddr
)
{
    if (!pEmemCAddr || !pEmemDAddr)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    if (pEmemCAddr)
    {
        *pEmemCAddr = NV_SOE_EMEMC(port);
    }
    if (pEmemDAddr)
    {
        *pEmemDAddr = NV_SOE_EMEMD(port);
    }
    return NV_OK;
}

/*!
 * Called by soeService to handle a SOE exterr. This function will dump the
 * current status of SOE and then trap the CPU for further inspection for a
 * debug build.
 *
 * @param[in]  device  nvswitch_device object pointer
 * @param[in]  pSoe    SOE object pointer
 */
static void
_soeServiceExterr_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    PFLCN pFlcn = ENG_GET_FLCN(pSoe);
    NvU32 extErrAddrOffset = 0, extErrStatOffset = 0;
    NvU32 exterrStatVal;

    NVSWITCH_PRINT(device, ERROR,
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                "!!                   ** SOE EXTERR **                !!\n"
                "!! Please file a bug with the following information. !!\n"
                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");

    if (pFlcn == NULL)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    NVSWITCH_PRINT(device, ERROR,
        "<<<<<<<<<<< SOE DEBUG INFORMATION >>>>>>>>>>>\n");
    NVSWITCH_PRINT(device, ERROR,
        "OS VERSION (FALCON_OS): %u\n",
        flcnRegRead_HAL(device, pFlcn, NV_PFALCON_FALCON_OS));

    if (soeGetExtErrRegAddrs_HAL(device, pSoe, &extErrAddrOffset, &extErrStatOffset) != NV_OK)
    {
        NVSWITCH_ASSERT(0);
        return;
    }

    NVSWITCH_PRINT(device, ERROR,
        "EXTERRADDR            : %u\n",
         flcnRegRead_HAL(device, pFlcn, extErrAddrOffset));

    exterrStatVal = flcnRegRead_HAL(device, pFlcn, extErrStatOffset);
    NVSWITCH_PRINT(device, ERROR,
        "EXTERRSTAT            : %u\n", exterrStatVal);
    NVSWITCH_PRINT(device, ERROR,
        "(AT PC)               : 0x%08X\n",
        DRF_VAL(_SOE, _FALCON_EXTERRSTAT, _PC, exterrStatVal));

    //
    // HW will continue to assert this interrupt as long as the _VALID bit is
    // set. Clear it to allow reporting of further failures since we have
    // already alerted the user that a transaction has failed.
    //
     flcnRegWrite_HAL(device, pFlcn, extErrStatOffset, FLD_SET_DRF(_SOE, _FALCON_EXTERRSTAT, _VALID, _FALSE, exterrStatVal));

    // Break to allow the user to inspect this on a debug build.
    NVSWITCH_ASSERT(0);
}

/*!
 * Get the bar0 offsets of NV_SOE_FALCON_EXTERRADDR and/or NV_SOE_FALCON_EXTERRSTAT
 *
 * @param[in]  device      nvswitch_device pointer
 * @param[in]  pSoe        SOE pointer
 * @param[out] pExtErrAddr BAR0 offset of NV_SOE_FALCON_EXTERRADDR
 * @param[out] pExtErrAddr BAR0 offset of NV_SOE_FALCON_EXTERRSTAT
 */
static NV_STATUS
_soeGetExtErrRegAddrs_LR10
(
    nvswitch_device *device,
    PSOE             pSoe,
    NvU32           *pExtErrAddr,
    NvU32           *pExtErrStat
)
{
     if (!pExtErrAddr || !pExtErrStat)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    if (pExtErrAddr)
    {
        *pExtErrAddr = NV_SOE_FALCON_EXTERRADDR;
    }
    if (pExtErrStat)
    {
        *pExtErrStat = NV_SOE_FALCON_EXTERRSTAT;
    }
    return NV_OK;
}

/*
 * Get number of EMEM ports
 *
 * @param[in]  device  nvswitch_device pointer
 * @param[in]  pSoe    SOE pointer
 */
static NvU32
_soeEmemPortSizeGet_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    return NV_SOE_EMEMC__SIZE_1;
}

/**
 * @brief   sets pEngDescUc and pEngDescBc to the discovered
 * engine that matches this flcnable instance
 *
 * @param[in]   device       nvswitch_device pointer
 * @param[in]   pSoe         SOE pointer
 * @param[out]  pEngDescUc  pointer to the UniCast Engine
 *       Descriptor
 * @param[out]  pEngDescBc  pointer to the BroadCast Engine
 *       Descriptor
 */
static void
_soeFetchEngines_LR10
(
    nvswitch_device         *device,
    FLCNABLE                *pSoe,
    ENGINE_DESCRIPTOR_TYPE  *pEngDescUc,
    ENGINE_DESCRIPTOR_TYPE  *pEngDescBc
)
{
    pEngDescUc->initialized = NV_FALSE;
    if (NVSWITCH_ENG_IS_VALID(device, SOE, 0))
    {
        pEngDescUc->base = NVSWITCH_GET_ENG(device, SOE, , 0);
    }
    else
    {
        pEngDescUc->base = 0;
    }

    pEngDescBc->initialized = NV_FALSE;
    pEngDescBc->base = 0;
}

/*!
 * @brief Determine if the SOE Falcon CPU is halted
 *
 * @param[in] device         nvswitch_device  pointer
 * @param[in] pSoe           SOE  pointer
 *
 * @return NvBool reflecting the SOE Falcon CPU halted state
 */
static NvBool
_soeIsCpuHalted_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    NvU32 data = flcnRegRead_HAL(device, ENG_GET_FLCN(pSoe), NV_PFALCON_FALCON_CPUCTL);
    return (FLD_TEST_DRF(_PFALCON, _FALCON_CPUCTL, _HALTED, _TRUE, data));
}

static NvlStatus
_soeDmaStartTest
(
    nvswitch_device *device,
    void            *cpuAddr,
    NvU64           dmaHandle,
    NvU16           xferSize,
    NvU8            subCmd
)
{
    FLCN *pFlcn       = device->pSoe->pFlcn;
    NvU32               cmdSeqDesc;
    NV_STATUS           status;
    RM_FLCN_CMD_SOE     cmd;
    RM_SOE_CORE_CMD_DMA_TEST *pDmaCmd;
    NVSWITCH_TIMEOUT    timeout;

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, DMA_TEST);

    pDmaCmd = &cmd.cmd.core.dma_test;
    RM_FLCN_U64_PACK(&pDmaCmd->dmaHandle, &dmaHandle);
    pDmaCmd->xferSize    = xferSize;
    pDmaCmd->dataPattern = SOE_DMA_TEST_XFER_PATTERN;
    pDmaCmd->cmdType = RM_SOE_CORE_CMD_DMA_SELFTEST;
    pDmaCmd->subCmdType  = subCmd;

    cmdSeqDesc = 0;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 5, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                (PRM_FLCN_CMD)&cmd,
                                NULL,   // pMsg
                                NULL,   // pPayload
                                SOE_RM_CMDQ_LOG_ID,
                                &cmdSeqDesc,
                                &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to send DMA test command to SOE\n");
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_soeValidateDmaTestResult
(
    nvswitch_device *device,
    void            *cpuAddr,
    NvU16           xferSize
)
{
    NvU16 iter;

    // Verify data written by SOE DMA matches what we expect.
    for (iter = 0; iter < SOE_DMA_TEST_BUF_SIZE; iter++)
    {
        NvU8 data = ((NvU8*) cpuAddr)[iter];

        // SOE would only touch data as much as the xfer size.
        if (iter < xferSize)
        {
            if (data != SOE_DMA_TEST_XFER_PATTERN)
            {
                NVSWITCH_PRINT(device, ERROR, "Incorrect data byte at offset %d = 0x%04x"
                                " for xfersize = %d\n", iter, data, xferSize);
                return -NVL_ERR_INVALID_STATE;
            }
        }
        // We expect the rest of the data to be at init value.
        else
        {
            if (data != SOE_DMA_TEST_INIT_PATTERN)
            {
                NVSWITCH_PRINT(device, ERROR, "Incorrect data byte at offset %d = 0x%04x"
                                " for xferSize = 0x%04x\n", iter, data, xferSize);
                return -NVL_ERR_INVALID_STATE;
            }
        }
    }

    return NVL_SUCCESS;
}

static NvlStatus
_soeDmaSelfTest
(
    nvswitch_device *device
)
{
    NvlStatus   ret;
    void        *cpuAddr;
    NvU64       dmaHandle;
    NvU16       xferSize;

    ret = nvswitch_os_alloc_contig_memory(device->os_handle, &cpuAddr, SOE_DMA_TEST_BUF_SIZE,
                                            (device->dma_addr_width == 32));

    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "nvswitch_os_alloc_contig_memory returned %d\n", ret);
        return ret;
    }

    // SOE DMA Write test

    nvswitch_os_memset(cpuAddr, SOE_DMA_TEST_INIT_PATTERN, SOE_DMA_TEST_BUF_SIZE);

    ret = nvswitch_os_map_dma_region(device->os_handle, cpuAddr, &dmaHandle,
                                        SOE_DMA_TEST_BUF_SIZE, NVSWITCH_DMA_DIR_TO_SYSMEM);

    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "nvswitch_os_map_dma_region returned %d\n", ret);
        goto _soeDmaSelfTest_exit;
    }

    // SOE DMA transfer sizes are in powers of 2.
    for (xferSize = SOE_DMA_MIN_SIZE; xferSize <= SOE_DMA_MAX_SIZE; xferSize <<= 1)
    {
        ret = nvswitch_os_sync_dma_region_for_device(device->os_handle, dmaHandle, SOE_DMA_TEST_BUF_SIZE,
                                                        NVSWITCH_DMA_DIR_TO_SYSMEM);

        if (ret != NVL_SUCCESS)
            break;

        ret = _soeDmaStartTest(device, cpuAddr, dmaHandle, xferSize,
                                RM_SOE_DMA_WRITE_TEST_SUBCMD);

        if (ret != NVL_SUCCESS)
            break;

        ret = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle, SOE_DMA_TEST_BUF_SIZE,
                                                    NVSWITCH_DMA_DIR_TO_SYSMEM);

        if (ret != NVL_SUCCESS)
            break;

        ret = _soeValidateDmaTestResult(device, cpuAddr, xferSize);

        if (ret != NVL_SUCCESS)
            break;

        nvswitch_os_memset(cpuAddr, SOE_DMA_TEST_INIT_PATTERN, SOE_DMA_TEST_BUF_SIZE);
    }

    nvswitch_os_unmap_dma_region(device->os_handle, cpuAddr, dmaHandle,
                                    SOE_DMA_TEST_BUF_SIZE, NVSWITCH_DMA_DIR_TO_SYSMEM);

    if (ret != NVL_SUCCESS)
        goto _soeDmaSelfTest_exit;

    // SOE DMA read test

    nvswitch_os_memset(cpuAddr, SOE_DMA_TEST_INIT_PATTERN, SOE_DMA_TEST_BUF_SIZE);

    //
    // 4B/8B reads will overfetch 16B from PCIe. The Falcon logic ignores the extra
    // data. In the case of this test the reads only occur from the start of
    // a DMA mapped buffer which is larger than 16B, hence the selftest does
    // not need special handling for this behavior. However this will need to
    // be handled for other cases where SW cannot guarentee that the overfetch
    // will not exceed mapped regions.
    //
    //

    ret = nvswitch_os_map_dma_region(device->os_handle, cpuAddr, &dmaHandle,
                                        SOE_DMA_TEST_BUF_SIZE, NVSWITCH_DMA_DIR_FROM_SYSMEM);

    if (ret != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "nvswitch_os_map_dma_region returned %d\n", ret);
        goto _soeDmaSelfTest_exit;
    }

    for (xferSize = SOE_DMA_MIN_SIZE; xferSize <= SOE_DMA_MAX_SIZE; xferSize <<= 1)
    {
        ret = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle, SOE_DMA_TEST_BUF_SIZE,
                                                    NVSWITCH_DMA_DIR_FROM_SYSMEM);

        if (ret != NVL_SUCCESS)
            break;

        // Fill in relevant data for the read test.
        nvswitch_os_memset(cpuAddr, SOE_DMA_TEST_XFER_PATTERN, xferSize);

        ret = nvswitch_os_sync_dma_region_for_device(device->os_handle, dmaHandle, SOE_DMA_TEST_BUF_SIZE,
                                                        NVSWITCH_DMA_DIR_FROM_SYSMEM);

        if (ret != NVL_SUCCESS)
            break;

        ret = _soeDmaStartTest(device, cpuAddr, dmaHandle, xferSize,
                                RM_SOE_DMA_READ_TEST_SUBCMD);

        if (ret != NVL_SUCCESS)
            break;
    }

    nvswitch_os_unmap_dma_region(device->os_handle, cpuAddr, dmaHandle,
                                    SOE_DMA_TEST_BUF_SIZE, NVSWITCH_DMA_DIR_FROM_SYSMEM);

_soeDmaSelfTest_exit:

    nvswitch_os_free_contig_memory(device->os_handle, cpuAddr, SOE_DMA_TEST_BUF_SIZE);

    return ret;
}

static NvlStatus
_soeTestDma_LR10
(
    nvswitch_device *device
)
{
    NvlStatus retval;

    retval = _soeDmaSelfTest(device);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "SOE DMA selftest failed\n");
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "SOE DMA selftest succeeded\n");
    }

    return retval;
}

/*!
 * @brief Send the EOM parameters to SOE
 *
 * @param[in]   device    nvswitch device pointer
 * @param[in]   mode      Node of EOM
 * @param[in]   nblks     Number of blocks
 * @param[in]   nerrs     Number of Errors.
 *
 * @return      NVL_SUCCESS on success
 */
static NvlStatus
_soeSetPexEOM_LR10
(
    nvswitch_device *device,
    NvU8 mode,
    NvU8 nblks,
    NvU8 nerrs,
    NvU8 berEyeSel
)
{
    FLCN               *pFlcn = device->pSoe->pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status = NV_OK;
    RM_FLCN_CMD_SOE     cmd;
    RM_SOE_BIF_CMD_EOM *pBifCmd = NULL;
    NVSWITCH_TIMEOUT    timeout = {0};

   nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_BIF;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(BIF, EOM);
    cmd.cmd.bif.cmdType = RM_SOE_BIF_CMD_UPDATE_EOM;
    //
    // We use SOE to set the EOM UPHY register since its Decode trapped and
    // hence CPU accessible.
    //
    pBifCmd = &cmd.cmd.bif.eomctl;
    pBifCmd->mode  = mode;
    pBifCmd->nblks = nblks;
    pBifCmd->nerrs = nerrs;
    pBifCmd->berEyeSel = berEyeSel;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 5, &timeout);

    status = flcnQueueCmdPostBlocking(device,
                 pFlcn,
                 (PRM_FLCN_CMD)&cmd,
                 NULL,   // pMsg
                 NULL,   // pPayload
                 SOE_RM_CMDQ_LOG_ID,
                 &cmdSeqDesc,
                 &timeout);

    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to Set EOM via SOE, Error 0x%x\n",
            __FUNCTION__, status);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

/*!
 * @brief Send the EOM parameters to SOE
 *
 * @param[in]   device    nvswitch device pointer
 * @param[in]   mode      Node of EOM
 * @param[in]   nblks     Number of blocks
 * @param[in]   nerrs     Number of Errors.
 *
 * @return      NVL_SUCCESS on success
 */
static NvlStatus
_soeGetPexEomStatus_LR10
(
    nvswitch_device *device,
    NvU8 mode,
    NvU8 nblks,
    NvU8 nerrs,
    NvU8 berEyeSel,
    NvU32 laneMask,
    NvU16 *pEomStatus
)
{
    FLCN *pFlcn = device->pSoe->pFlcn;
    NvU32 cmdSeqDesc = 0;
    NV_STATUS status = NV_OK;
    RM_FLCN_CMD_SOE cmd;
    RM_SOE_BIF_CMD_EOM_STATUS *pBifCmd = NULL;
    NVSWITCH_TIMEOUT timeout = { 0 };
    NvU64 dmaHandle = 0;
    NvU8 *pReadBuffer = NULL;
    NvU32 bufferSize = BIF_MAX_PCIE_LANES * sizeof(NvU16);

    if (bufferSize > SOE_DMA_MAX_SIZE)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Buffer size too large\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    // Create DMA mapping
    status = nvswitch_os_alloc_contig_memory(device->os_handle,
                 (void**)&pReadBuffer, bufferSize, (device->dma_addr_width == 32));
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to allocate contig memory\n",
            __FUNCTION__);
        return status;
    }

    status = nvswitch_os_map_dma_region(device->os_handle,
                                        pReadBuffer,
                                        &dmaHandle,
                                        bufferSize,
                                        NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to map dma region to sysmem\n");
        nvswitch_os_free_contig_memory(device->os_handle, pReadBuffer, bufferSize);
        return status;
    }

   nvswitch_os_memset(&cmd, 0, sizeof(cmd));
   nvswitch_os_memset(pReadBuffer, 0, bufferSize);

    cmd.hdr.unitId = RM_SOE_UNIT_BIF;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(BIF, EOM_STATUS);
    cmd.cmd.bif.cmdType = RM_SOE_BIF_CMD_GET_EOM_STATUS;

    pBifCmd = &cmd.cmd.bif.eomStatus;
    pBifCmd->mode  = mode;
    pBifCmd->nblks = nblks;
    pBifCmd->nerrs = nerrs;
    pBifCmd->berEyeSel = berEyeSel;
    pBifCmd->laneMask = laneMask;
    RM_FLCN_U64_PACK(&pBifCmd->dmaHandle, &dmaHandle);

    status = nvswitch_os_sync_dma_region_for_device(device->os_handle, dmaHandle,
        bufferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "Failed to yield to DMA controller\n");
        goto _soeGetPexEomStatus_LR10_exit;
    }

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 5, &timeout);

    status = flcnQueueCmdPostBlocking(device,
                 pFlcn,
                 (PRM_FLCN_CMD)&cmd,
                 NULL,   // pMsg
                 NULL,   // pPayload
                 SOE_RM_CMDQ_LOG_ID,
                 &cmdSeqDesc,
                 &timeout);

    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to Get EOM status via SOE, Error 0x%x\n",
            __FUNCTION__, status);
        status = -NVL_ERR_INVALID_STATE;
        goto _soeGetPexEomStatus_LR10_exit;
    }

    status = nvswitch_os_sync_dma_region_for_cpu(device->os_handle, dmaHandle,
        bufferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "DMA controller failed to yield back\n");
        goto _soeGetPexEomStatus_LR10_exit;
    }

    nvswitch_os_memcpy(((NvU8*)pEomStatus), pReadBuffer, bufferSize);

_soeGetPexEomStatus_LR10_exit :
    nvswitch_os_unmap_dma_region(device->os_handle, pReadBuffer, dmaHandle,
        bufferSize, NVSWITCH_DMA_DIR_TO_SYSMEM);
    nvswitch_os_free_contig_memory(device->os_handle, pReadBuffer, bufferSize);

    return status;
}

/*!
 * @brief Get the register values of UPHY registers
 *
 * Read the register value from a scratch register updated by SOE.
 *
 * @param[in]   device            nvswitch device pointer
 * @param[in]   regAddress        Register address whose value is to be retrieved
 * @param[in]   laneSelectMask    Mask of lanes to read from
 * @param[out]  *pRegValue        Value of register address
 *
 * Read the register value from a scratch register updated by SOE.
 *
 * @return      NVL_SUCCESS on success
 */
static NvlStatus
_soeGetUphyDlnCfgSpace_LR10
(
    nvswitch_device *device,
    NvU32 regAddress,
    NvU32 laneSelectMask,
    NvU16 *pRegValue
)
{
    FLCN               *pFlcn = device->pSoe->pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status = NV_OK;
    RM_FLCN_CMD_SOE     cmd;
    RM_SOE_BIF_CMD_UPHY_DLN_CFG_SPACE *pBifCmd = NULL;
    NVSWITCH_TIMEOUT    timeout = { 0 };

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_BIF;
    cmd.hdr.size = RM_SOE_CMD_SIZE(BIF, UPHY_DLN_CFG_SPACE);
    cmd.cmd.bif.cmdType = RM_SOE_BIF_CMD_GET_UPHY_DLN_CFG_SPACE;

    pBifCmd = &cmd.cmd.bif.cfgctl;
    pBifCmd->regAddress = regAddress;
    pBifCmd->laneSelectMask = laneSelectMask;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);

    status = flcnQueueCmdPostBlocking(device,
                 pFlcn,
                 (PRM_FLCN_CMD)&cmd,
                 NULL,   // pMsg
                 NULL,   // pPayload
                 SOE_RM_CMDQ_LOG_ID,
                 &cmdSeqDesc,
                 &timeout);

    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to execute BIF GET_UPHY_DLN_CFG_SPACE via SOE, Error 0x%x\n",
            __FUNCTION__, status);
        return -NVL_ERR_INVALID_STATE;
    }

    *pRegValue = NVSWITCH_SOE_RD32_LR10(device, 0, _SOE, _MAILBOX(0));

    return NVL_SUCCESS;
}

static NvlStatus
_soeForceThermalSlowdown_LR10
(
    nvswitch_device *device,
    NvBool slowdown,
    NvU32  periodUs
)
{
    FLCN               *pFlcn = device->pSoe->pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status = NV_OK;
    RM_FLCN_CMD_SOE     cmd;
    NVSWITCH_TIMEOUT    timeout = {0};

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));
    cmd.hdr.unitId = RM_SOE_UNIT_THERM;
    cmd.hdr.size = RM_SOE_CMD_SIZE(THERM, FORCE_SLOWDOWN);
    cmd.cmd.therm.cmdType = RM_SOE_THERM_FORCE_SLOWDOWN;
    cmd.cmd.therm.slowdown.slowdown = slowdown;
    cmd.cmd.therm.slowdown.periodUs = periodUs;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                  (PRM_FLCN_CMD)&cmd,
                                  NULL,   // pMsg             - not used for now
                                  NULL,   // pPayload         - not used for now
                                  SOE_RM_CMDQ_LOG_ID,
                                  &cmdSeqDesc,
                                  &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Thermal slowdown failed. rc:%d\n",
            __FUNCTION__, status);

        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_soeSetPcieLinkSpeed_LR10
(
    nvswitch_device *device,
    NvU32 linkSpeed
)
{
    FLCN               *pFlcn = device->pSoe->pFlcn;
    NvU32               cmdSeqDesc = 0;
    NV_STATUS           status = NV_OK;
    RM_FLCN_CMD_SOE     cmd;
    RM_SOE_BIF_CMD_PCIE_LINK_SPEED *pBifCmd = NULL;
    NVSWITCH_TIMEOUT    timeout = { 0 };

    nvswitch_os_memset(&cmd, 0, sizeof(cmd));

    cmd.hdr.unitId = RM_SOE_UNIT_BIF;
    cmd.hdr.size = RM_SOE_CMD_SIZE(BIF, PCIE_LINK_SPEED);
    cmd.cmd.bif.cmdType = RM_SOE_BIF_CMD_SET_PCIE_LINK_SPEED;

    pBifCmd = &cmd.cmd.bif.speedctl;
    pBifCmd->linkSpeed = linkSpeed;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);

    status = flcnQueueCmdPostBlocking(device,
                 pFlcn,
                 (PRM_FLCN_CMD)&cmd,
                 NULL,   // pMsg
                 NULL,   // pPayload
                 SOE_RM_CMDQ_LOG_ID,
                 &cmdSeqDesc,
                 &timeout);

    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to execute BIF SET_PCIE_LINK_SPEED via SOE, Error 0x%x\n",
            __FUNCTION__, status);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

/*!
 * Use the SOE INIT Message to construct and initialize all SOE Queues.
 *
 * @param[in]      device nvswitch_device pointer
 * @param[in]      pSoe   SOE object pointer
 * @param[in]      pMsg   Pointer to the INIT Message
 *
 * @return 'NV_OK' upon successful creation of all SOE Queues
 */
static NV_STATUS
_soeQMgrCreateQueuesFromInitMsg
(
    nvswitch_device  *device,
    PFLCNABLE         pSoe,
    RM_FLCN_MSG_SOE  *pMsg
)
{
    RM_SOE_INIT_MSG_SOE_INIT *pInit;
    NvU32                     i;
    NvU32                     queueLogId;
    NV_STATUS                 status;
    FLCNQUEUE                *pQueue;
    PFLCN                     pFlcn = ENG_GET_FLCN(pSoe);
    PFALCON_QUEUE_INFO        pQueueInfo;

    NVSWITCH_ASSERT(pFlcn != NULL);

    pQueueInfo = pFlcn->pQueueInfo;
    NVSWITCH_ASSERT(pQueueInfo != NULL);

    pInit = &pMsg->msg.init.soeInit;
    NVSWITCH_ASSERT(pInit->numQueues <= pFlcn->numQueues);

    for (i = 0; i < pFlcn->numQueues; i++)
    {
        queueLogId = pInit->qInfo[i].queueLogId;
        NVSWITCH_ASSERT(queueLogId < pFlcn->numQueues);
        pQueue = &pQueueInfo->pQueues[queueLogId];
        status = flcnQueueConstruct_dmem_nvswitch(
                     device,
                     pFlcn,
                     &pQueue,                                  // ppQueue
                     queueLogId,                               // Logical ID of the queue
                     pInit->qInfo[i].queuePhyId,               // Physical ID of the queue
                     pInit->qInfo[i].queueOffset,              // offset
                     pInit->qInfo[i].queueSize,                // size
                     RM_FLCN_QUEUE_HDR_SIZE);                  // cmdHdrSize
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                        "%s: Error constructing SOE Queue (status="
                        "0x%08x).\n", __FUNCTION__, status);
            NVSWITCH_ASSERT(0);
            return status;
        }
    }
    return NV_OK;
}

/*!
 * Purges all the messages from the SOE's message queue.  Each message will
 * be analyzed, clients will be notified of status, and events will be routed
 * to all registered event listeners.
 *
 * @param[in]  device nvswitch_device pointer
 * @param[in]  pSoe   SOE object pointer
 *
 * @return 'NV_OK' if the message queue was successfully purged.
 */
static NV_STATUS
_soeProcessMessages_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    RM_FLCN_MSG_SOE  soeMessage;
    NV_STATUS        status;
    PFLCN            pFlcn  = ENG_GET_FLCN(pSoe);

    // keep processing messages until no more exist in the message queue
    while (NV_OK == (status = flcnQueueReadData(
                                     device,
                                     pFlcn,
                                     SOE_RM_MSGQ_LOG_ID,
                                     (RM_FLCN_MSG *)&soeMessage, NV_TRUE)))
    {
        NVSWITCH_PRINT(device, INFO,
                    "%s: unitId=0x%02x, size=0x%02x, ctrlFlags=0x%02x, " \
                    "seqNumId=0x%02x\n",
                    __FUNCTION__,
                    soeMessage.hdr.unitId,
                    soeMessage.hdr.size,
                    soeMessage.hdr.ctrlFlags,
                    soeMessage.hdr.seqNumId);

        // check to see if the message is a reply or an event.
        if ((soeMessage.hdr.ctrlFlags &= RM_FLCN_QUEUE_HDR_FLAGS_EVENT) != 0)
        {
            flcnQueueEventHandle(device, pFlcn, (RM_FLCN_MSG *)&soeMessage, NV_OK);
        }
        // the message is a response from a previously queued command
        else
        {
            flcnQueueResponseHandle(device, pFlcn, (RM_FLCN_MSG *)&soeMessage);
        }
    }

    //
    // Status NV_ERR_NOT_READY implies, Queue is empty.
    // Log the message in other error cases.
    //
    if (status != NV_ERR_NOT_READY)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: unexpected error while purging message queue (status=0x%x).\n",
            __FUNCTION__, (status));
    }

    return status;
}

/*!
 * @brief Read the INIT message directly out of the Message Queue.
 *
 * This function accesses the Message Queue directly using the HAL.  It does
 * NOT and may NOT use the queue manager as it has not yet been constructed and
 * initialized.  The Message Queue may not be empty when this function is called
 * and the first message in the queue MUST be the INIT message.
 *
 * @param[in]   device  nvswitch_device pointer
 * @param[in]   pSoe    SOE object pointer
 * @param[out]  pMsg    Message structure to fill with the INIT message data
 *
 * @return 'NV_OK' upon successful extraction of the INIT message.
 * @return
 *     'NV_ERR_INVALID_STATE' if the first message found was not an INIT
 *     message or if the message was improperly formatted.
 */
static NV_STATUS
_soeGetInitMessage
(
    nvswitch_device  *device,
    PSOE              pSoe,
    RM_FLCN_MSG_SOE  *pMsg
)
{
    PFLCN               pFlcn   = ENG_GET_FLCN(pSoe);
    NV_STATUS           status  = NV_OK;
    NvU32               tail    = 0;
    PFALCON_QUEUE_INFO  pQueueInfo;
    // on the GPU, rmEmemPortId = sec2RmEmemPortIdGet_HAL(...);
    NvU8                rmEmemPortId = 0;

    if (pFlcn == NULL)
    {
        NVSWITCH_ASSERT(pFlcn != NULL);
        return NV_ERR_INVALID_POINTER;
    }

    pQueueInfo = pFlcn->pQueueInfo;
    if (pQueueInfo == NULL)
    {
        NVSWITCH_ASSERT(pQueueInfo != NULL);
        return NV_ERR_INVALID_POINTER;
    }

    //
    // Message queue 0 is used by SOE to communicate with RM
    // Check SOE_CMDMGMT_MSG_QUEUE_RM in //uproc/soe/inc/soe_cmdmgmt.h
    //
    pQueueInfo->pQueues[SOE_RM_MSGQ_LOG_ID].queuePhyId = 0;

    // read the header starting at the current tail position
    (void)flcnMsgQueueTailGet(device, pFlcn,
        &pQueueInfo->pQueues[SOE_RM_MSGQ_LOG_ID], &tail);
    if (pFlcn->bEmemEnabled)
    {
        //
        // We use the offset in DMEM for the src address, since
        // EmemCopyFrom automatically converts it to the offset in EMEM
        //
        flcnableEmemCopyFrom(
            device, pFlcn->pFlcnable,
            tail,                   // src
            (NvU8 *)&pMsg->hdr,     // pDst
            RM_FLCN_QUEUE_HDR_SIZE, // numBytes
            rmEmemPortId);          // port
    }
    else
    {
        status = flcnDmemCopyFrom(device,
                                  pFlcn,
                                  tail,                     // src
                                  (NvU8 *)&pMsg->hdr,       // pDst
                                  RM_FLCN_QUEUE_HDR_SIZE,   // numBytes
                                  0);                       // port
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to copy from SOE DMEM\n", __FUNCTION__);
            NVSWITCH_ASSERT(0);
            goto _soeGetInitMessage_exit;
        }
    }

    if (pMsg->hdr.unitId != RM_SOE_UNIT_INIT)
    {
        status = NV_ERR_INVALID_STATE;
        NVSWITCH_ASSERT(0);
        goto _soeGetInitMessage_exit;
    }

    // read the message body and update the tail position
    if (pFlcn->bEmemEnabled)
    {
        //
        // We use the offset in DMEM for the src address, since
        // EmemCopyFrom automatically converts it to the offset in EMEM
        //
        flcnableEmemCopyFrom(
            device, pFlcn->pFlcnable,
            tail + RM_FLCN_QUEUE_HDR_SIZE,              // src
            (NvU8 *)&pMsg->msg,                         // pDst
            pMsg->hdr.size - RM_FLCN_QUEUE_HDR_SIZE,    // numBytes
            rmEmemPortId);                              // port
    }
    else
    {
        status = flcnDmemCopyFrom(device,
            pFlcn,
            tail + RM_FLCN_QUEUE_HDR_SIZE,              // src
            (NvU8 *)&pMsg->msg,                         // pDst
            pMsg->hdr.size - RM_FLCN_QUEUE_HDR_SIZE,    // numBytes
            0);                                         // port
        if (status != NV_OK)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to copy from SOE DMEM\n", __FUNCTION__);
            NVSWITCH_ASSERT(0);
            goto _soeGetInitMessage_exit;
        }
    }

    tail += NV_ALIGN_UP(pMsg->hdr.size, SOE_DMEM_ALIGNMENT);
    flcnMsgQueueTailSet(device, pFlcn,
        &pQueueInfo->pQueues[SOE_RM_MSGQ_LOG_ID], tail);

_soeGetInitMessage_exit:
    return status;
}

/*!
 * This function exists to solve a natural chicken-and-egg problem that arises
 * due to the fact that queue information (location, size, id, etc...) is
 * relayed to the RM as a message in a queue.  Queue construction is done when
 * the message arrives and the normal queue read/write functions are not
 * available until construction is complete.  Construction cannot be done until
 * the message is read from the queue.  Therefore, the very first message read
 * from the Message Queue must be considered as a special-case and must NOT use
 * any functionality provided by the SOE's queue manager.
 *
 * @param[in]  device  nvswitch_device pointer
 * @param[in]  pSoe    SOE object pointer
 *
 * @return 'NV_OK'
 *     Upon successful extraction and processing of the first SOE message.
 */
static NV_STATUS
_soeProcessMessagesPreInit_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    RM_FLCN_MSG_SOE   msg;
    NV_STATUS        status;
    PFLCN            pFlcn  = ENG_GET_FLCN(pSoe);

    // extract the "INIT" message (this is never expected to fail)
    status = _soeGetInitMessage(device, pSoe, &msg);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "%s: Failed to extract the INIT message "
                    "from the SOE Message Queue (status=0x%08x).",
                    __FUNCTION__, status);
        NVSWITCH_ASSERT(0);
        return status;
    }

    //
    // Now hookup the "real" message-processing function and handle the "INIT"
    // message.
    //
    pSoe->base.pHal->processMessages = _soeProcessMessages_LR10;
    return flcnQueueEventHandle(device, pFlcn, (RM_FLCN_MSG *)&msg, NV_OK);
}

/*!
 * @brief Process the "INIT" message sent from the SOE ucode application.
 *
 * When the SOE ucode is done initializing, it will post an INIT message in
 * the Message Queue that contains all the necessary attributes that are
 * needed to enqueuing commands and extracting messages from the queues.
 * The packet will also contain the offset and size of portion of DMEM that
 * the RM must manage.  Upon receiving this message it will be assume that
 * the SOE is ready to start accepting commands.
 *
 * @param[in]  device  nvswitch_device pointer
 * @param[in]  pSoe    SOE object pointer
 * @param[in]  pMsg    Pointer to the event's message data
 *
 * @return 'NV_OK' if the event was successfully handled.
 */
static NV_STATUS
_soeHandleInitEvent_LR10
(
    nvswitch_device  *device,
    PFLCNABLE         pSoe,
    RM_FLCN_MSG      *pGenMsg
)
{
    NV_STATUS         status;
    PFLCN             pFlcn = ENG_GET_FLCN(pSoe);
    RM_FLCN_MSG_SOE *pMsg  = (RM_FLCN_MSG_SOE *)pGenMsg;

    if (pFlcn == NULL)
    {
        NVSWITCH_ASSERT(pFlcn != NULL);
        return NV_ERR_INVALID_POINTER;
    }

    NVSWITCH_PRINT(device, INFO,
                "%s: Received INIT message from SOE\n",
                __FUNCTION__);

    //
    // Pass the INIT message to the queue manager to allow it to create the
    // queues.
    //
    status = _soeQMgrCreateQueuesFromInitMsg(device, pSoe, pMsg);
    if (status != NV_OK)
    {
        NVSWITCH_ASSERT(0);
        return status;
    }

    flcnDbgInfoDmemOffsetSet(device, pFlcn,
        pMsg->msg.init.soeInit.osDebugEntryPoint);

    // the SOE ucode is now initialized and ready to accept commands
    pFlcn->bOSReady = NV_TRUE;

    return NV_OK;
}

/*!
 * Loop until SOE RTOS is loaded and gives us an INIT message
 *
 * @param[in]  device  nvswitch_device object pointer
 * @param[in]  pSoe    SOE object pointer
 */
static NV_STATUS
_soeWaitForInitAck_LR10
(
    nvswitch_device *device,
    PSOE             pSoe
)
{
    PFLCN            pFlcn  = ENG_GET_FLCN(pSoe);
    NVSWITCH_TIMEOUT timeout;
    NvBool bKeepPolling;

    // If INIT message is already loaded, return.
    if (pFlcn->bOSReady)
    {
        return NV_OK;
    }

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 5, &timeout);
    do
    {
        bKeepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        soeService_HAL(device, pSoe);
        if (pFlcn->bOSReady)
        {
            return NV_OK;
        }

        nvswitch_os_sleep(1);
    }
    while (bKeepPolling);

    if (!pFlcn->bOSReady)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s Timeout while waiting for SOE bootup\n",
            __FUNCTION__);
        NVSWITCH_ASSERT(0);
        return NV_ERR_TIMEOUT;
    }

    return NV_OK;
}

NvlStatus
nvswitch_soe_issue_ingress_stop_lr10
(
    nvswitch_device *device,
    NvU32 nport,
    NvBool bStop
)
{
    // Not supported on LR10
    return NVL_SUCCESS;
}

/*!
 * @brief   set hal function pointers for functions defined in LR10 (i.e. this file)
 *
 * this function has to be at the end of the file so that all the
 * other functions are already defined.
 *
 * @param[in] pFlcnable   The flcnable for which to set hals
 */
void
soeSetupHal_LR10
(
    SOE *pSoe
)
{
    soe_hal *pHal = pSoe->base.pHal;
    flcnable_hal *pParentHal = (flcnable_hal *)pHal;
    //set any functions we want to override
    pParentHal->construct            = _soeConstruct_LR10;
    pParentHal->destruct             = _soeDestruct_LR10;
    pParentHal->getExternalConfig    = _soeGetExternalConfig_LR10;
    pParentHal->fetchEngines         = _soeFetchEngines_LR10;
    pParentHal->handleInitEvent      = _soeHandleInitEvent_LR10;

    // set any functions specific to SOE
    pHal->service               = _soeService_LR10;
    pHal->serviceHalt           = _soeServiceHalt_LR10;
    pHal->getEmemSize           = _soeGetEmemSize_LR10;
    pHal->ememTransfer          = _soeEmemTransfer_LR10;
    pHal->getEmemSize           = _soeGetEmemSize_LR10;
    pHal->getEmemStartOffset    = _soeGetEmemStartOffset_LR10;
    pHal->ememPortToRegAddr     = _soeEmemPortToRegAddr_LR10;
    pHal->serviceExterr         = _soeServiceExterr_LR10;
    pHal->getExtErrRegAddrs     = _soeGetExtErrRegAddrs_LR10;
    pHal->ememPortSizeGet       = _soeEmemPortSizeGet_LR10;
    pHal->isCpuHalted           = _soeIsCpuHalted_LR10;
    pHal->testDma               = _soeTestDma_LR10;
    pHal->setPexEOM             = _soeSetPexEOM_LR10;
    pHal->getUphyDlnCfgSpace    = _soeGetUphyDlnCfgSpace_LR10;
    pHal->forceThermalSlowdown  = _soeForceThermalSlowdown_LR10;
    pHal->setPcieLinkSpeed      = _soeSetPcieLinkSpeed_LR10;
    pHal->getPexEomStatus       = _soeGetPexEomStatus_LR10;
    pHal->processMessages       = _soeProcessMessagesPreInit_LR10;
    pHal->waitForInitAck        = _soeWaitForInitAck_LR10;
}
