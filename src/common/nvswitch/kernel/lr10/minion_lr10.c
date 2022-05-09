/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "nvlink_export.h"
#include "common_nvswitch.h"
#include "lr10/lr10.h"
#include "lr10/minion_lr10.h"
#include "lr10/minion_production_ucode_lr10_dbg.h"
#include "lr10/minion_production_ucode_lr10_prod.h"
#include "regkey_nvswitch.h"

#include "nvswitch/lr10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/lr10/dev_minion_ip.h"
#include "nvswitch/lr10/dev_minion_ip_addendum.h"
#include "nvswitch/lr10/dev_ingress_ip.h"
#include "nvswitch/lr10/dev_egress_ip.h"

/*
 * @Brief : Check if MINION is already running.
 *
 * The function assumes that if one of MINIONs is running, all of them are
 * running. This approach needs to be fixed.
 *
 * TODO: Refactor minion code to check for each minion's status individually.
 *
 * @param[in] device Bootstrap MINIONs on this device
 */
static NvBool
_nvswitch_check_running_minions
(
    nvswitch_device *device
)
{
    NvU32  data, i;
    NvBool bMinionRunning = NV_FALSE;

    for (i = 0; i < NVSWITCH_ENG_COUNT(device, MINION, ); i++)
    {
        if (!NVSWITCH_ENG_IS_VALID(device, MINION, i))
        {
            NVSWITCH_PRINT(device, SETUP,
                "%s: MINION instance %d is not valid.\n",
                 __FUNCTION__, i);
            continue;
        }

        data = NVSWITCH_MINION_RD32_LR10(device, i, _CMINION, _FALCON_IRQSTAT);
        if (FLD_TEST_DRF(_CMINION, _FALCON_IRQSTAT, _HALT, _FALSE, data))
        {
            data = NVSWITCH_MINION_RD32_LR10(device, i, _MINION, _MINION_STATUS);
            if (FLD_TEST_DRF(_MINION,  _MINION_STATUS, _STATUS, _BOOT, data))
            {
                //
                // Set initialized flag if MINION is running.
                // We don't want to bootstrap a falcon that is already running.
                //
                nvswitch_set_minion_initialized(device, i, NV_TRUE);

                NVSWITCH_PRINT(device, SETUP,
                    "%s: MINION instance %d is already bootstrapped.\n",
                    __FUNCTION__, i);
                bMinionRunning = NV_TRUE;
            }
        }
    }

    return bMinionRunning;
}

/*
 * @Brief : MINION pre init routine
 *          Waits for scrubbing to finish
 *
 * @param[in] device  MINIONs on this device
 */
static NvlStatus
_nvswitch_minion_pre_init
(
    nvswitch_device *device
)
{
    NvU32            data;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;
    NvU32            idx_minion;
    NvlStatus        status = NVL_SUCCESS;
    NvU32            falconIntrMask, falconIntrDest;
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    // Find first valid MINION instance
    for (idx_minion = 0; idx_minion < NVSWITCH_ENG_COUNT(device, MINION, ); idx_minion++)
    {
        if (NVSWITCH_ENG_IS_VALID(device, MINION, idx_minion))
        {
            break;
        }
    }
    if (idx_minion >= NVSWITCH_ENG_COUNT(device, MINION, ))
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: No MINIONs instantiated.  Skipping MINION pre-init\n",
            __FUNCTION__);
        goto _nvswitch_minion_pre_init_exit;
    }

    // Since we are not using Falcon DMA to load ucode, set REQUIRE_CTX to FALSE
    NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_DMACTL, 0x0);

    //
    // Initialize falcon specific interrupts before MINION is loaded.
    // Once MINION is loaded, these registers get locked down.
    //

    // falcon interrupt mask is set through IRQMSET
    falconIntrMask = (DRF_DEF(_CMINION, _FALCON_IRQMSET, _WDTMR, _SET) | 
                      DRF_DEF(_CMINION, _FALCON_IRQMSET, _HALT, _SET)  |
                      DRF_DEF(_CMINION, _FALCON_IRQMSET, _EXTERR, _SET)|
                      DRF_DEF(_CMINION, _FALCON_IRQMSET, _SWGEN0, _SET)|
                      DRF_DEF(_CMINION, _FALCON_IRQMSET, _SWGEN1, _SET));

    // falcon interrupt routing to the HOST
    falconIntrDest = (DRF_DEF(_CMINION, _FALCON_IRQDEST, _HOST_WDTMR,  _HOST) |
                      DRF_DEF(_CMINION, _FALCON_IRQDEST, _HOST_HALT,   _HOST) |
                      DRF_DEF(_CMINION, _FALCON_IRQDEST, _HOST_EXTERR, _HOST) |
                      DRF_DEF(_CMINION, _FALCON_IRQDEST, _HOST_SWGEN0, _HOST) |
                      DRF_DEF(_CMINION, _FALCON_IRQDEST, _HOST_SWGEN1,   _HOST)        |
                      DRF_DEF(_CMINION, _FALCON_IRQDEST, _TARGET_WDTMR,  _HOST_NORMAL) |
                      DRF_DEF(_CMINION, _FALCON_IRQDEST, _TARGET_HALT,   _HOST_NORMAL) |
                      DRF_DEF(_CMINION, _FALCON_IRQDEST, _TARGET_EXTERR, _HOST_NORMAL) |
                      DRF_DEF(_CMINION, _FALCON_IRQDEST, _TARGET_SWGEN0, _HOST_NORMAL) |
                      DRF_DEF(_CMINION, _FALCON_IRQDEST, _TARGET_SWGEN1, _HOST_NORMAL));

    NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IRQMSET, falconIntrMask);
    NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IRQDEST, falconIntrDest);
    chip_device->intr_minion_dest = falconIntrDest;

    //
    // As soon as we access any falcon reg (above), the scrubber will start scrubbing
    // IMEM and DMEM. Wait for the scrubber to finish scrubbing.
    //
    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        nvswitch_timeout_create(10*NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    }
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        // Check if scrubbing was done for first enabled MINION
        data = NVSWITCH_MINION_RD32_LR10(device, idx_minion, _CMINION, _FALCON_DMACTL);
        if (FLD_TEST_DRF(_CMINION, _FALCON_DMACTL, _DMEM_SCRUBBING, _DONE, data) &&
            FLD_TEST_DRF(_CMINION, _FALCON_DMACTL, _IMEM_SCRUBBING, _DONE, data))
        {
            break;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    if (!FLD_TEST_DRF(_CMINION, _FALCON_DMACTL, _DMEM_SCRUBBING, _DONE, data) ||
        !FLD_TEST_DRF(_CMINION, _FALCON_DMACTL, _IMEM_SCRUBBING, _DONE, data))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Timeout waiting for scrubbing to finish on MINION %d.\n",
            __FUNCTION__, idx_minion);
        status = -NVL_ERR_INVALID_STATE;
        return status;
    }

_nvswitch_minion_pre_init_exit:
    return status;
}

/*
 * @Brief : Copy the minion ucode to IMEM and DMEM in broadcast mode
 *
 * @param[in] device  Copy ucode to all MINIONS associated with the device
 */
static NvlStatus
_nvswitch_minion_copy_ucode_bc
(
    nvswitch_device                *device,
    const NvU32                    *minion_ucode_data,
    const NvU32                    *minion_ucode_header
)
{
    const PFALCON_UCODE_HDR_INFO_LR10 pUcodeHeader =
        (PFALCON_UCODE_HDR_INFO_LR10) &minion_ucode_header[0];
    const NvU32 *pHeader = &minion_ucode_header[0];

    NvU32 data, i, app, dataSize;
    NvU32 appCodeOffset, appCodeSize, appDataOffset, appDataSize;
    NvU16 tag;
    NvU32 idx_minion;

    // Find first valid MINION instance
    for (idx_minion = 0; idx_minion < NVSWITCH_ENG_COUNT(device, MINION, ); idx_minion++)
    {
        if (NVSWITCH_ENG_IS_VALID(device, MINION, idx_minion))
        {
            break;
        }
    }
    if (idx_minion >= NVSWITCH_ENG_COUNT(device, MINION, ))
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: No MINIONs instantiated.  Skipping MINION ucode load\n",
            __FUNCTION__);
        goto _nvswitch_minion_copy_ucode_bc_exit;
    }

    dataSize = sizeof(minion_ucode_data[0]);

    // Initialize address of IMEM to 0x0 and set auto-increment on write
    data = 0;
    data = FLD_SET_DRF_NUM(_CMINION, _FALCON_IMEMC, _OFFS, 0x0, data);
    data = FLD_SET_DRF_NUM(_CMINION, _FALCON_IMEMC, _BLK, 0x0, data);
    data = FLD_SET_DRF(_CMINION, _FALCON_IMEMC, _AINCW, _TRUE, data);
    NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IMEMC(0), data);

    //
    // Initialize IMEM tag to 0 explicitly even though power-on value is 0.
    // Writes to IMEM don't work if we don't do this
    //
    tag = 0;
    NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IMEMT(0), tag);

    // Copy over IMEM part of the ucode and tag along the way
    for (i = 0; i < (pUcodeHeader->osCodeSize / dataSize) ; i++)
    {
        // Increment tag for after every block (256 bytes)
        if (i && ((i % (FALCON_IMEM_BLK_SIZE_BYTES_LR10 / dataSize)) == 0))
        {
            tag++;
            NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IMEMT(0), (NvU32) tag);
        }

        // Copy IMEM DWORD by DWORD
        data = minion_ucode_data[(pUcodeHeader->osCodeOffset / dataSize) + i];
        NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IMEMD(0), data);
    }

    // Initialize address of DMEM to 0x0 and set auto-increment on write
    data = 0;
    data = FLD_SET_DRF_NUM(_CMINION, _FALCON_DMEMC, _OFFS, 0x0, data);
    data = FLD_SET_DRF_NUM(_CMINION, _FALCON_DMEMC, _BLK, 0x0, data);
    data = FLD_SET_DRF(_CMINION, _FALCON_DMEMC, _AINCW, _TRUE, data);
    NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_DMEMC(0), data);

    // Copy over DMEM part of the ucode
    for (i = 0; i < (pUcodeHeader->osDataSize / dataSize) ; i++)
    {
        // Copy DMEM DWORD by DWORD
        data = minion_ucode_data[(pUcodeHeader->osDataOffset / dataSize) + i];
        NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_DMEMD(0), data);
    }

    // Copy over any apps in the ucode with the appropriate tags
    if (pUcodeHeader->numApps)
    {
        for (app = 0; app < pUcodeHeader->numApps ; app++)
        {
            // Index into the app code info
            appCodeOffset = pHeader[FALCON_CODE_HDR_APP_CODE_START_LR10 + 2*app];
            appCodeSize   = pHeader[FALCON_CODE_HDR_APP_CODE_START_LR10 + 2*app + 1];

            // Index into the app data info using appCodeStart offset as a base
            appDataOffset = pHeader[FALCON_CODE_HDR_APP_CODE_START_LR10 +
                2*pUcodeHeader->numApps + 2*app];
            appDataSize   = pHeader[FALCON_CODE_HDR_APP_CODE_START_LR10 +
                2*pUcodeHeader->numApps + 2*app + 1];

            // Mark the following IMEM blocks as secure
            data = NVSWITCH_MINION_RD32_LR10(device, idx_minion, _CMINION, _FALCON_IMEMC(0));
            data = FLD_SET_DRF_NUM(_CMINION, _FALCON_IMEMC, _SECURE, 0x1, data);
            NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IMEMC(0), data);

            // Copy to IMEM and tag along the way
            tag = (NvU16)(appCodeOffset >> 8);
            NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IMEMT(0), tag);

            // Copy app code to IMEM picking up where last load left off
            for (i = 0; i < (appCodeSize / dataSize); i++)
            {
                if (i && ((i % (FALCON_IMEM_BLK_SIZE_BYTES_LR10 / dataSize)) == 0))
                {
                    tag++;
                    NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IMEMT(0), tag);
                }

                data = minion_ucode_data[(appCodeOffset / dataSize) + i];
                NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_IMEMD(0), data);
            }

            // Copy app data to DMEM picking up where last load left off
            for (i = 0; i < (appDataSize / dataSize); i++)
            {
                data = minion_ucode_data[appDataOffset + i];
                NVSWITCH_MINION_WR32_BCAST_LR10(device, _CMINION, _FALCON_DMEMD(0), data);
            }
        }
    }

_nvswitch_minion_copy_ucode_bc_exit:
    return NVL_SUCCESS;
}

/*
 * @brief : Print MINION ucode (first 8 DWORDS).
 *          This is used for diagnostic purposes only.
 *
 * @param[in] device Print ucode for a MINION on this device
 * @param[in] link   Print ucode for MINION associated with the link
 */
static void
_nvswitch_minion_print_ucode
(
    nvswitch_device *device,
    NvU32            instance
)
{
#if defined(DEBUG) || defined(DEVELOP) || defined(NV_MODS)
    NvU32 data, i;
    NvU32 buf[8];

    data = 0;
    data = FLD_SET_DRF_NUM(_CMINION, _FALCON_IMEMC, _OFFS, 0x0, data);
    data = FLD_SET_DRF_NUM(_CMINION, _FALCON_IMEMC, _BLK, 0x0, data);
    data = FLD_SET_DRF(_CMINION, _FALCON_IMEMC, _AINCR, _TRUE, data);
    NVSWITCH_MINION_WR32_LR10(device, instance, _CMINION, _FALCON_IMEMC(0), data);

    NVSWITCH_PRINT(device, SETUP, "MINION IMEMD = \n");
    for (i = 0; i < 8 ; i++)
    {
        buf[i] = NVSWITCH_MINION_RD32_LR10(device, instance, _CMINION, _FALCON_IMEMD(0));
    }
    NVSWITCH_PRINT(device, SETUP, " 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
                   buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

    NVSWITCH_PRINT(device, SETUP, "MINION IMEMC = \n");
    for (i = 0; i < 8 ; i++)
    {
        buf[i] = NVSWITCH_MINION_RD32_LR10(device, instance, _CMINION, _FALCON_IMEMC(0));
    }
    NVSWITCH_PRINT(device, SETUP, " 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
                   buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

    data = 0;
    data = FLD_SET_DRF_NUM(_CMINION, _FALCON_DMEMC, _OFFS, 0x0, data);
    data = FLD_SET_DRF_NUM(_CMINION, _FALCON_DMEMC, _BLK, 0x0, data);
    data = FLD_SET_DRF(_CMINION, _FALCON_DMEMC, _AINCR, _TRUE, data);
    NVSWITCH_MINION_WR32_LR10(device, instance, _CMINION, _FALCON_DMEMC(0), data);

    NVSWITCH_PRINT(device, SETUP, "MINION DMEMD = \n");
    for (i = 0; i < 8 ; i++)
    {
        buf[i] = NVSWITCH_MINION_RD32_LR10(device, instance, _CMINION, _FALCON_DMEMD(0));
    }
    NVSWITCH_PRINT(device, SETUP, " 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
                   buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);

    NVSWITCH_PRINT(device, SETUP, "MINION DMEMC = \n");
    for (i = 0; i < 8 ; i++)
    {
        buf[i] = NVSWITCH_MINION_RD32_LR10(device, instance, _CMINION, _FALCON_DMEMC(0));
    }
    NVSWITCH_PRINT(device, SETUP, " 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
                   buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6], buf[7]);
#endif  //defined(DEBUG) || defined(DEVELOP) || defined(NV_MODS)
}

/*
 * @Brief : Test MINION by sending SWINTR DLCMD
 *
 * @param[in] device Send command to MINION on this device
 * @param[in] link   DLCMD will be sent on this link
 *
 * @return           Returns true if the DLCMD passed
 */
static NvBool
_nvswitch_minion_test_dlcmd
(
    nvswitch_device *device,
    NvU32            linkNumber
)
{
    NvU32 interrupts, localLinkNumber;
    localLinkNumber = linkNumber % NVSWITCH_LINKS_PER_MINION;

    if (nvswitch_minion_send_command(device, linkNumber,
        NV_MINION_NVLINK_DL_CMD_COMMAND_SWINTR, 0) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: SWINTR DL CMD failed for link %d.\n",
            __FUNCTION__, linkNumber);
        return NV_FALSE;
    }

    interrupts = NVSWITCH_MINION_LINK_RD32_LR10(device, linkNumber,
                    _MINION, _NVLINK_LINK_INTR(localLinkNumber));

    if (DRF_VAL(_MINION, _NVLINK_LINK_INTR, _CODE, interrupts) ==
        NV_MINION_NVLINK_LINK_INTR_CODE_SWREQ)
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: Received NON-FATAL INTR_CODE = SWREQ, SUBCODE = 0x%x."
            " SWINTR DLCMD was executed successfully.\n",
            __FUNCTION__,
            DRF_VAL(_MINION, _NVLINK_LINK_INTR, _SUBCODE, interrupts));

        // clear the interrupt
        interrupts = DRF_NUM(_MINION, _NVLINK_LINK_INTR, _STATE, 1);
        NVSWITCH_MINION_LINK_WR32_LR10(device, linkNumber, _MINION,
                                       _NVLINK_LINK_INTR(localLinkNumber), interrupts);

        return NV_TRUE;
    }
    else
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: No SWINTR interrupt received. DL CMD failed for link %d.\n",
            __FUNCTION__, linkNumber);
        return NV_FALSE;
    }

    return NV_TRUE;
}

static void
_nvswitch_print_minion_info
(
    nvswitch_device *device,
    NvU32 id
)
{
#if defined(DEVELOP) || defined(DEBUG)
    NvU32   falcon_os;
    NvU32   falcon_mailbox;
    NvU32   falcon_sctl;

    falcon_os = NVSWITCH_MINION_RD32_LR10(device, id, _CMINION, _FALCON_OS);
    falcon_mailbox = NVSWITCH_MINION_RD32_LR10(device, id, _CMINION, _FALCON_MAILBOX1);
    falcon_sctl = NVSWITCH_MINION_RD32_LR10(device, id, _CMINION, _FALCON_SCTL);

    // Dump the ucode ID string epilog
    NVSWITCH_PRINT(device, SETUP,
        "MINION Falcon ucode version info: Ucode v%d.%d  Phy v%d\n",
        (falcon_os >> 16) & 0xFFFF,
        falcon_os & 0xFFFF,
        falcon_mailbox);

    // Display security level info at info level, very useful for logs.
    NVSWITCH_PRINT(device, SETUP,
       "%s: NV_CMINION_FALCON_SCTL : 0x%08X\n",
       __FUNCTION__, falcon_sctl);
#endif
}

/*
 * @Brief : Bootstrap MINION associated with the link
 *
 * @param[in] device Bootstrap MINION on this device
 * @param[in] link   Bootstrap MINION associated with the link
 */
static NvlStatus
_nvswitch_minion_bootstrap
(
    nvswitch_device *device
)
{
    NvU32            data, i, link_num;
    NvU64            link_mask;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;
    NvlStatus        status = NVL_SUCCESS;

    for (i = 0; i < NVSWITCH_ENG_COUNT(device, MINION, ) ; i++)
    {
        if (!NVSWITCH_ENG_IS_VALID(device, MINION, i))
        {
            NVSWITCH_PRINT(device, WARN,
                "%s: MINION[%d] is not valid.  Skipping\n",
                __FUNCTION__, i);
            continue;
        }

        if (nvswitch_is_minion_initialized(device, i))
        {
            NVSWITCH_PRINT(device, WARN,
                "%s: MINION[%d] is already bootstrapped.\n",
                __FUNCTION__, i);
            continue;
        }

        // Verify if the ucode was written properly
        _nvswitch_minion_print_ucode(device, i);

        // Write boot vector to 0x0
        data = NVSWITCH_MINION_RD32_LR10(device, i, _CMINION, _FALCON_BOOTVEC);
        data = FLD_SET_DRF_NUM(_CMINION, _FALCON_BOOTVEC, _VEC, 0x0, data);
        NVSWITCH_MINION_WR32_LR10(device, i, _CMINION, _FALCON_BOOTVEC, data);

        //
        // Start the Falcon
        // If a falcon is managed (and thus supports secure mode), we need to
        // write NV_PFALCON_FALCON_CPUCTL_ALIAS_STARTCPU = _TRUE.
        // Below write is a nop in secure mode.
        //
        data = NVSWITCH_MINION_RD32_LR10(device, i, _CMINION, _FALCON_CPUCTL);
        data = FLD_SET_DRF(_CMINION, _FALCON_CPUCTL, _STARTCPU, _TRUE, data);
        NVSWITCH_MINION_WR32_LR10(device, i, _CMINION, _FALCON_CPUCTL, data);

        if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
        {
            nvswitch_timeout_create(10*NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
        }
        else
        {
            nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
        }

        //
        // We will exit this if we recieve bootstrap signal OR
        // if we timeout waiting for bootstrap signal       OR
        // if bootstrap fails
        //
        do
        {
            keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

            data = NVSWITCH_MINION_RD32_LR10(device, i, _MINION, _MINION_STATUS);

            // The INIT sequence has completed, success?
            if (FLD_TEST_DRF(_MINION, _MINION_STATUS, _STATUS, _BOOT, data))
            {
                // MINION Init succeeded.
                NVSWITCH_PRINT(device, SETUP,
                    "%s: NVLink MINION %d bootstrap complete signal received.\n",
                    __FUNCTION__, i);

                _nvswitch_print_minion_info(device, i);
                break;
            }

            //
            // Check if any falcon interrupts are hit & pending.
            // TODO: Check return status of the call below
            //
            nvswitch_minion_service_falcon_interrupts_lr10(device, i);

            nvswitch_os_sleep(1);
        }
        while (keepPolling);

        if (!FLD_TEST_DRF(_MINION, _MINION_STATUS, _STATUS, _BOOT, data))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for NVLink MINION %d to complete bootstrap!"
                "NV_CMINION_MINION_STATUS = 0x%08x\n",
                __FUNCTION__, i, data);
            // Bug 2974064: Review this timeout handling (fall through)
        }
        nvswitch_set_minion_initialized(device, i, NV_TRUE);

        // Run a test DLCMD to see if MINION is accepting commands.
        link_mask = NVSWITCH_NVLIPT_GET_LOCAL_LINK_MASK64(i);
        FOR_EACH_INDEX_IN_MASK(64, link_num, link_mask)
        {
            // Pick a valid lick in this NVLipt
            if (nvswitch_is_link_valid(device, link_num))
            {
                break;
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;

        if (!_nvswitch_minion_test_dlcmd(device, link_num))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Unable to bootstrap MINION %d.\n",
                __FUNCTION__, i);
            nvswitch_set_minion_initialized(device, i, NV_FALSE);
            return -NVL_ERR_INVALID_STATE;
        }
        else
        {
            NVSWITCH_PRINT(device, SETUP,
                "%s: MINION %d successfully bootstrapped and accepting DLCMDs.\n",
                __FUNCTION__, i);
            nvswitch_set_minion_initialized(device, i, NV_TRUE);
        }
    }

    return status;
}

/*
 * @Brief : Send MINION DL CMD for a particular link
 *
 * @param[in] device     Send command to MINION on this device
 * @param[in] linkNumber DLCMD will be sent on this link number
 *
 * @return           Returns true if the DLCMD passed
 */
NvlStatus
nvswitch_minion_send_command_lr10
(
    nvswitch_device *device,
    NvU32            linkNumber,
    NvU32            command,
    NvU32            scratch0
)
{
    NvU32            data = 0, localLinkNumber, statData = 0;
    NvU32            ingressEccRegVal = 0, egressEccRegVal = 0;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    localLinkNumber = linkNumber % NVSWITCH_LINKS_PER_MINION;

    if (!nvswitch_is_minion_initialized(device, NVSWITCH_GET_LINK_ENG_INST(device, linkNumber, MINION)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: MINION %d is not initialized for link %08x.\n",
            __FUNCTION__, NVSWITCH_GET_LINK_ENG_INST(device, linkNumber, MINION),
            linkNumber);
        return NVL_SUCCESS;
    }

    data = NVSWITCH_MINION_LINK_RD32_LR10(device, linkNumber, _MINION, _NVLINK_DL_CMD(localLinkNumber));
    if (FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_CMD, _FAULT, 1, data))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: MINION %d is in fault state. NV_MINION_NVLINK_DL_CMD(%d) = %08x\n",
            __FUNCTION__, NVSWITCH_GET_LINK_ENG_INST(device, linkNumber, MINION),
            linkNumber, data);
        return -NVL_ERR_INVALID_STATE;
    }

    // Write to minion scratch if needed by command
    switch (command)
    {
        case NV_MINION_NVLINK_DL_CMD_COMMAND_CONFIGEOM:
            data = 0;
            data = FLD_SET_DRF_NUM(_MINION, _MISC_0, _SCRATCH_SWRW_0, scratch0, data);
            NVSWITCH_MINION_WR32_LR10(device,
                NVSWITCH_GET_LINK_ENG_INST(device, linkNumber, MINION), _MINION, _MISC_0, data);
            break;
        case NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE1:
            //
            // WAR bug 2708497
            // Before INITPHASE1, we must clear these values, then set back to
            // _PROD after the call
            // NV_INGRESS_ERR_ECC_CTRL_NCISOC_PARITY_ENABLE
            // NV_EGRESS_ERR_ECC_CTRL_NCISOC_PARITY_ENABLE
            //

            ingressEccRegVal = NVSWITCH_NPORT_RD32_LR10(device, linkNumber, _INGRESS, _ERR_ECC_CTRL);
            NVSWITCH_NPORT_WR32_LR10(device, linkNumber, _INGRESS, _ERR_ECC_CTRL,
                FLD_SET_DRF(_INGRESS, _ERR_ECC_CTRL, _NCISOC_PARITY_ENABLE, _DISABLE, ingressEccRegVal));

            egressEccRegVal = NVSWITCH_NPORT_RD32_LR10(device, linkNumber, _EGRESS, _ERR_ECC_CTRL);
            NVSWITCH_NPORT_WR32_LR10(device, linkNumber, _EGRESS, _ERR_ECC_CTRL,
                FLD_SET_DRF(_EGRESS, _ERR_ECC_CTRL, _NCISOC_PARITY_ENABLE, _DISABLE, egressEccRegVal));
            break;
        default:
            break;
    }

    data = FLD_SET_DRF_NUM(_MINION, _NVLINK_DL_CMD, _COMMAND, command, data);
    data = FLD_SET_DRF_NUM(_MINION, _NVLINK_DL_CMD, _FAULT,   1,   data);
    NVSWITCH_MINION_LINK_WR32_LR10(device, linkNumber, _MINION, _NVLINK_DL_CMD(localLinkNumber), data);

    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(NVSWITCH_INTERVAL_5MSEC_IN_NS, &timeout);
    }

    //
    // We will exit this if the command is successful OR
    // if timeout waiting for the READY bit to be set OR
    // if it generates a MINION FAULT
    //
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        data = NVSWITCH_MINION_LINK_RD32_LR10(device, linkNumber, _MINION, _NVLINK_DL_CMD(localLinkNumber));
        if (FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_CMD, _READY, 1, data))
        {
            // The command has completed, success?
            if (FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_CMD, _FAULT, 1, data))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: NVLink MINION command faulted!"
                    " NV_MINION_NVLINK_DL_CMD(%d) = 0x%08x\n",
                    __FUNCTION__, linkNumber, data);

                // Pull fault code and subcode
                if (nvswitch_minion_get_dl_status(device, linkNumber,
                            NV_NVLSTAT_MN00, 0, &statData) == NVL_SUCCESS)
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s: Minion DLCMD Fault code = 0x%x, Sub-code = 0x%x\n",
                        __FUNCTION__,
                        DRF_VAL(_NVLSTAT, _MN00, _LINK_INTR_CODE, statData),
                        DRF_VAL(_NVLSTAT, _MN00, _LINK_INTR_SUBCODE, statData));
                }
                else
                {
                    NVSWITCH_PRINT(device, ERROR,
                        "%s: Failed to get code and subcode from DLSTAT, link %d\n",
                        __FUNCTION__, linkNumber);
                }

                // Clear the fault and return
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Clearing NVLink MINION fault for link %d\n",
                    __FUNCTION__, linkNumber);

                data = FLD_SET_DRF_NUM(_MINION, _NVLINK_DL_CMD, _FAULT, 1, 0x0);
                NVSWITCH_MINION_LINK_WR32_LR10(device, linkNumber, _MINION, _NVLINK_DL_CMD(localLinkNumber), data);
                return -NVL_ERR_INVALID_STATE;
            }
            else
            {
                NVSWITCH_PRINT(device, SETUP,
                    "%s: NVLink MINION command %x was sent successfully for link %d\n",
                    __FUNCTION__, command, linkNumber);
                break;
            }
        }

        if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
        {
            nvswitch_os_sleep(1);
        }
    }
    while (keepPolling);

    if (!FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_CMD, _READY, 1, data))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Timeout waiting for NVLink MINION command to complete!"
            " NV_MINION_NVLINK_DL_CMD(%d) = 0x%08x\n",
            __FUNCTION__, linkNumber, data);
        return -NVL_ERR_INVALID_STATE;
    }

    if (command == NV_MINION_NVLINK_DL_CMD_COMMAND_INITPHASE1)
    {
        //
        // WAR bug 2708497
        // Before INITPHASE1, we must clear these values, then set back to
        // _PROD after the call
        // NV_INGRESS_ERR_ECC_CTRL_NCISOC_PARITY_ENABLE
        // NV_EGRESS_ERR_ECC_CTRL_NCISOC_PARITY_ENABLE
        //
        NVSWITCH_NPORT_WR32_LR10(device, linkNumber, _INGRESS, _ERR_ECC_CTRL, ingressEccRegVal);
        NVSWITCH_NPORT_WR32_LR10(device, linkNumber, _EGRESS,  _ERR_ECC_CTRL, egressEccRegVal);
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Load minion ucode from regkeys
 *          Overrides minion image from the regkeys
 *
 * @param   device  The nvswitch device
 */
static NvlStatus
_nvswitch_load_minion_ucode_image_from_regkeys
(
    nvswitch_device *device
)
{
    NvlStatus status = NVL_SUCCESS;

    NvU32 *data = NULL;
    NvU32 *header = NULL;
    NvU32 data_size;
    NvU32 header_size;

    if (!NV_SWITCH_REGKEY_PRIVATE_ALLOWED)
    {
        // Regkey override of ucode image only allowed on internal use debug drivers.
        return -NVL_ERR_GENERIC;
    }

    status = nvswitch_os_read_registry_dword(device->os_handle,
                    NV_SWITCH_REGKEY_MINION_SET_UCODE_HDR_SIZE, &header_size);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    status = nvswitch_os_read_registry_dword(device->os_handle,
                    NV_SWITCH_REGKEY_MINION_SET_UCODE_DATA_SIZE, &data_size);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    if (header_size == 0 || data_size == 0)
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: Failed to query ucode size via regkey.\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    header = nvswitch_os_malloc(header_size);
    if (header == NULL)
    {
        status = -NVL_NO_MEM;
        goto done;
    }

    data = nvswitch_os_malloc(data_size);
    if (data == NULL)
    {
        status = -NVL_NO_MEM;
        goto done;
    }

    status = nvswitch_os_read_registery_binary(device->os_handle,
                    NV_SWITCH_REGKEY_MINION_SET_UCODE_HDR, (NvU8*)header, header_size);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: Failed to query ucode header.\n",
            __FUNCTION__);
        goto done;
    }

    status = nvswitch_os_read_registery_binary(device->os_handle,
                    NV_SWITCH_REGKEY_MINION_SET_UCODE_DATA, (NvU8*)data, data_size);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: Failed to query ucode data.\n",
            __FUNCTION__);
        goto done;
    }

    // Copy the ucode to IMEM and DMEM by using backdoor PMB access
    status = _nvswitch_minion_copy_ucode_bc(device, data, header);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unable to copy MINION ucode in broadcast mode!\n",
            __FUNCTION__);
        goto done;
    }
    else
    {
        NVSWITCH_PRINT(device, SETUP,
            "Successfully loaded MINION microcode override.\n");
    }

done:
    if (header != NULL)
    {
        nvswitch_os_free(header);
    }

    if (data != NULL)
    {
        nvswitch_os_free(data);
    }

    return status;
}

/*
 * @Brief : Load minion ucode image
 *
 * @param   device            The nvswitch device
 */
static NvlStatus
_nvswitch_load_minion_ucode_image
(
    nvswitch_device *device
)
{
    NvlStatus status;
    NvU32 data;
    NvBool bDebugMode = NV_FALSE;

    // load ucode image via regkey
    status = _nvswitch_load_minion_ucode_image_from_regkeys(device);
    if (status == NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Successfully loaded ucode via regkey\n",
            __FUNCTION__);
        return status;
    }

    //
    // Determine if _dbg or _prod ucode needs to be loaded
    // Read from MINION 0 - we don't support MINIONs being in different debug modes
    //
    data = NVSWITCH_MINION_RD32_LR10(device, 0, _CMINION, _SCP_CTL_STAT);
    bDebugMode = FLD_TEST_DRF(_CMINION, _SCP_CTL_STAT, _DEBUG_MODE, _DISABLED, data) ?
                 (NV_FALSE) : (NV_TRUE);

    //
    // If ucode load fails via regkey fallback to the default ucode.
    // Copy the ucode to IMEM and DMEM by using backdoor PMB access
    //
    if (bDebugMode)
    {
        status = _nvswitch_minion_copy_ucode_bc(device, minion_ucode_data_lr10_dbg, minion_ucode_header_lr10_dbg);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Unable to copy dbg MINION ucode in broadcast mode!\n",
                __FUNCTION__);
            return status;
        }
    }
    else
    {
        status = _nvswitch_minion_copy_ucode_bc(device, minion_ucode_data_lr10_prod, minion_ucode_header_lr10_prod);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Unable to copy prod MINION ucode in broadcast mode!\n",
                __FUNCTION__);
            return status;
        }
    }

    return status;
}

/*
 * @Brief : Bootstrap all MINIONs on the specified device
 *
 * @param[in] device Bootstrap MINIONs on this device
 */
NvlStatus
nvswitch_init_minion_lr10
(
    nvswitch_device *device
)
{
    NvlStatus status = NVL_SUCCESS;

    if (_nvswitch_check_running_minions(device))
    {
        return NVL_SUCCESS;
    }

    status = _nvswitch_minion_pre_init(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: MINION pre init failed\n",
            __FUNCTION__);
        return status;
    }

    // Load MINION
    status = _nvswitch_load_minion_ucode_image(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to load MINION ucode image!\n",
            __FUNCTION__);
        return status;
    }

   status = _nvswitch_minion_bootstrap(device);
   if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Unable to bootstrap MINION!\n",
            __FUNCTION__);
        return status;
    }

    return status;
}

NvlStatus
nvswitch_minion_get_dl_status_lr10
(
    nvswitch_device *device,
    NvU32            linkId,
    NvU32            statusIdx,
    NvU32            statusArgs,
    NvU32           *statusData
)
{
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;
    NvU32            regData, localLinkNumber;
    localLinkNumber = linkId % NVSWITCH_LINKS_PER_MINION;

    if (!nvswitch_is_minion_initialized(device, NVSWITCH_GET_LINK_ENG_INST(device, linkId, MINION)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: MINION %d is not initialized for link %08x.\n",
            __FUNCTION__, NVSWITCH_GET_LINK_ENG_INST(device, linkId, MINION),
            linkId);
        return -NVL_ERR_INVALID_STATE;
    }

    // Query the DL status interface to get the data
    NVSWITCH_MINION_LINK_WR32_LR10(device, linkId, _MINION, _NVLINK_DL_STAT(localLinkNumber),
            DRF_NUM(_MINION, _NVLINK_DL_STAT, _ARGS, statusArgs) |
            DRF_NUM(_MINION, _NVLINK_DL_STAT, _STATUSIDX, statusIdx));

    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(20 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
    }

    // Poll for READY bit to be set
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        regData = NVSWITCH_MINION_LINK_RD32_LR10(device, linkId, _MINION, _NVLINK_DL_STAT(localLinkNumber));
        if (FLD_TEST_DRF_NUM(_MINION, _NVLINK_DL_STAT, _READY, 1, regData))
        {
            *statusData = NVSWITCH_MINION_LINK_RD32_LR10(device, linkId, _MINION, _NVLINK_DL_STATDATA(localLinkNumber));
            return NVL_SUCCESS;
        }
        if (IS_FMODEL(device) || IS_RTLSIM(device))
        {
            nvswitch_os_sleep(1);
        }
    }
    while (keepPolling);

    NVSWITCH_PRINT(device, ERROR,
        "%s: Timeout waiting for DL_STAT request to complete"
        " NV_MINION_NVLINK_DL_STAT(%d) = 0x%08x\n",
        __FUNCTION__, linkId, regData);
    return -NVL_ERR_INVALID_STATE;
}

NvlStatus
nvswitch_minion_get_initoptimize_status_lr10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;
    NvU32            statData;
    NvlStatus        status;

    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        nvswitch_timeout_create(100 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(20 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }

    // Poll for READY bit to be set
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        // Poll for INITOPTIMIZE status on MINION DL STAT interface
        status = nvswitch_minion_get_dl_status(device, linkId, NV_NVLSTAT_UC01, 0, &statData);
        if (status != NVL_SUCCESS)
        {
            return status;
        }

        if (FLD_TEST_DRF_NUM(_NVLSTAT, _UC01, _TRAINING_GOOD, 0x1, statData))
        {
            NVSWITCH_PRINT(device, INFO,
                "%s: INITOPTIMIZE TRAINING_GOOD on link: %d\n",
                __FUNCTION__, linkId);
            return NVL_SUCCESS;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    NVSWITCH_PRINT(device, ERROR,
        "%s: Timeout waiting for INITOPTIMIZE TRAINING_GOOD on link: %d\n",
        __FUNCTION__, linkId);
    return -NVL_ERR_INVALID_STATE;
}

NvlStatus
nvswitch_minion_get_initnegotiate_status_lr10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvU32            statData;
    NvlStatus        status;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(2 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }

    // Poll for READY bit to be set
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        // Check INITNEGOTIATE status on MINION DL STAT interface
        status = nvswitch_minion_get_dl_status(device, linkId, NV_NVLSTAT_UC01, 0, &statData);
        if (status != NVL_SUCCESS)
        {
            return status;
        }

        if (FLD_TEST_DRF(_NVLSTAT, _UC01, _CONFIG_GOOD, _SUCCESS, statData))
        {
            NVSWITCH_PRINT(device, INFO,
                    "%s: INITNEGOTIATE CONFIG_GOOD on link: %d\n",
                    __FUNCTION__, linkId);

            return NVL_SUCCESS;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    NVSWITCH_PRINT(device, ERROR,
        "%s: Timeout waiting for INITNEGOTIATE CONFIG_GOOD on link: %d\n",
        __FUNCTION__, linkId);

    return -NVL_ERR_INVALID_STATE;
}

NvlStatus
nvswitch_minion_get_rxdet_status_lr10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvU32            statData;
    NvlStatus        status;
    NVSWITCH_TIMEOUT timeout;
    NvBool           keepPolling;

    if (IS_FMODEL(device) || IS_EMULATION(device) || IS_RTLSIM(device))
    {
        nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);
    }
    else
    {
        nvswitch_timeout_create(20 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);
    }

    // Poll for READY bit to be set
    do
    {
        keepPolling = (nvswitch_timeout_check(&timeout)) ? NV_FALSE : NV_TRUE;

        // Check RXDET status on MINION DL STAT interface
        status = nvswitch_minion_get_dl_status(device, linkId, NV_NVLSTAT_LNK2, 0, &statData);
        if (status != NVL_SUCCESS)
        {
            return status;
        }

        if (FLD_TEST_DRF(_NVLSTAT, _LNK2, _RXDET_LINK_STATUS, _FOUND, statData))
        {
            NVSWITCH_PRINT(device, INFO,
                    "%s: RXDET LINK_STATUS = FOUND on link: %d\n",
                    __FUNCTION__, linkId);

            // Retrieve which lanes were found (should be all)
            device->link[linkId].lane_rxdet_status_mask =
                    DRF_VAL(_NVLSTAT, _LNK2, _RXDET_LANE_STATUS, statData);

            //
            // MINION doesn't have knowledge of lane reversal and therefore
            // reports logical lanes. We must reverse the bitmask here if applicable
            // since RM reports physical lanes.
            //
            if (nvswitch_link_lane_reversed_lr10(device, linkId))
            {
                NVSWITCH_REVERSE_BITMASK_32(NVSWITCH_NUM_LANES_LR10,
                        device->link[linkId].lane_rxdet_status_mask);
            }

            return NVL_SUCCESS;
        }

        if (FLD_TEST_DRF(_NVLSTAT, _LNK2, _RXDET_LINK_STATUS, _TIMEOUT, statData))
        {
            NVSWITCH_PRINT(device, ERROR,
                    "%s: RXDET LINK_STATUS = TIMEOUT on link: %d\n",
                    __FUNCTION__, linkId);

            // Retrieve which lanes were found
            device->link[linkId].lane_rxdet_status_mask =
                    DRF_VAL(_NVLSTAT, _LNK2, _RXDET_LANE_STATUS, statData);

            //
            // MINION doesn't have knowledge of lane reversal and therefore
            // reports logical lanes. We must reverse the bitmask here if applicable
            // since RM reports physical lanes.
            //
            if (nvswitch_link_lane_reversed_lr10(device, linkId))
            {
                NVSWITCH_REVERSE_BITMASK_32(NVSWITCH_NUM_LANES_LR10,
                        device->link[linkId].lane_rxdet_status_mask);
            }

            return -NVL_ERR_INVALID_STATE;
        }

        nvswitch_os_sleep(1);
    }
    while (keepPolling);

    NVSWITCH_PRINT(device, ERROR,
        "%s: Timeout waiting for RXDET STATUS on link: %d\n",
        __FUNCTION__, linkId);

    return -NVL_ERR_INVALID_STATE;
}

NvlStatus
nvswitch_minion_set_rx_term_lr10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    if (nvswitch_minion_send_command(device, linkId,
        NV_MINION_NVLINK_DL_CMD_COMMAND_INITRXTERM, 0) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: INITRXTERM DL CMD failed for link %d.\n",
            __FUNCTION__, linkId);
        return -NVL_ERR_INVALID_STATE;
    }

    return NVL_SUCCESS;
}

NvU32
nvswitch_minion_get_line_rate_Mbps_lr10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvU32     statData;
    NvlStatus status;

    status = nvswitch_minion_get_dl_status(device, linkId, NV_NVLSTAT_LNK3, 0, &statData);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to retrieve LINERATE from MINION DLSTAT for link %d.\n",
            __FUNCTION__, linkId);
    }

    return DRF_NUM(_NVLSTAT, _LNK3, _LINERATE, statData);
}

NvU32
nvswitch_minion_get_data_rate_KiBps_lr10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvU32     statData;
    NvlStatus status;

    status = nvswitch_minion_get_dl_status(device, linkId, NV_NVLSTAT_LNK5, 0, &statData);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to retrieve DATARATE from MINION DLSTAT for link %d.\n",
            __FUNCTION__, linkId);
    }

    return DRF_NUM(_NVLSTAT, _LNK5, _DATARATE, statData);
}

NvlStatus
nvswitch_set_minion_initialized_lr10
(
    nvswitch_device *device,
    NvU32 idx_minion,
    NvBool initialized
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    if (!NVSWITCH_ENG_VALID_LR10(device, MINION, idx_minion))
    {
        return -NVL_BAD_ARGS;
    }

    chip_device->engMINION[idx_minion].initialized = initialized;
    return NVL_SUCCESS;
}

NvBool
nvswitch_is_minion_initialized_lr10
(
    nvswitch_device *device,
    NvU32 idx_minion
)
{
    lr10_device *chip_device = NVSWITCH_GET_CHIP_DEVICE_LR10(device);

    if (!NVSWITCH_ENG_VALID_LR10(device, MINION, idx_minion))
    {
        return NV_FALSE;
    }
    return (chip_device->engMINION[idx_minion].initialized != 0);
}

NvlStatus
nvswitch_minion_clear_dl_error_counters_lr10
(
    nvswitch_device *device,
    NvU32            linkId
)
{
    NvlStatus status;

    status = nvswitch_minion_send_command(device, linkId,
                                               NV_MINION_NVLINK_DL_CMD_COMMAND_DLSTAT_CLR_DLERRCNT, 0);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s : Failed to clear error count to MINION for (%s):(%d).\n",
            __FUNCTION__, device->name, linkId);
    }
    return status;
}

