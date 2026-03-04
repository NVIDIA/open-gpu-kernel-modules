/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "common_nvswitch.h"
#include "cci/cci_priv_nvswitch.h"

/* -------------------- Object construction/initialization ------------------- */

NvBool
nvswitch_is_cci_supported
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_is_cci_supported(device);
}

NvlStatus
nvswitch_cci_discovery
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_cci_discovery(device);
}

void
nvswitch_cci_setup_gpio_pins
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_cci_setup_gpio_pins(device);
}

NvlStatus
nvswitch_cci_ports_cpld_read
(
    nvswitch_device *device,
    NvU8 reg,
    NvU8 *pData
)
{
    return device->hal.nvswitch_cci_ports_cpld_read(device, reg, pData);
}

NvlStatus
nvswitch_cci_ports_cpld_write
(
    nvswitch_device *device,
    NvU8 reg,
    NvU8 data
)
{
    return device->hal.nvswitch_cci_ports_cpld_write(device, reg, data);
}

NvlStatus
nvswitch_cci_reset
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_cci_reset(device);
}

NvlStatus
nvswitch_cci_reset_links
(
    nvswitch_device *device,
    NvU64 linkMask
)
{
    return device->hal.nvswitch_cci_reset_links(device, linkMask);
}

void 
nvswitch_cci_get_xcvrs_present
(
    nvswitch_device *device,
    NvU32 *pMaskPresent
)
{
    device->hal.nvswitch_cci_get_xcvrs_present(device, pMaskPresent);
}

void 
nvswitch_cci_get_xcvrs_present_change
(
    nvswitch_device *device,
    NvU32 *pMaskPresentChange
)
{
    device->hal.nvswitch_cci_get_xcvrs_present_change(device, pMaskPresentChange);
}

void 
nvswitch_cci_update_link_state_led
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_cci_update_link_state_led(device);
}

NvlStatus
nvswitch_cci_set_xcvr_led_state
(
    nvswitch_device *device,
    NvU32 client, 
    NvU32 osfp, 
    NvBool bSetLocate
)
{
    return device->hal.nvswitch_cci_set_xcvr_led_state(device, client, osfp, bSetLocate);
}

NvlStatus
nvswitch_cci_get_xcvr_led_state
(
    nvswitch_device *device,
    NvU32 client, 
    NvU32 osfp, 
    NvU8 *pLedState
)
{
    return device->hal.nvswitch_cci_get_xcvr_led_state(device, client, osfp, pLedState);
}

NvlStatus
nvswitch_cci_setup_onboard
(
    nvswitch_device *device
)
{
    return device->hal.nvswitch_cci_setup_onboard(device);
}

NvlStatus
nvswitch_cci_setup_module_path
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp
)
{
    return device->hal.nvswitch_cci_setup_module_path(device, client, osfp);
}

NvlStatus
nvswitch_cci_module_access_cmd
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            osfp,
    NvU32            addr,
    NvU32            length,
    NvU8            *pValArray,
    NvBool           bRead,
    NvBool           bBlk
)
{
    return device->hal.nvswitch_cci_module_access_cmd(device, client, osfp, addr, length,
                                                              pValArray, bRead, bBlk);
}

NvlStatus
nvswitch_cci_apply_control_set_values
(
    nvswitch_device *device,
    NvU32           client,
    NvU32           moduleMask
)
{
    return device->hal.nvswitch_cci_apply_control_set_values(device, client, moduleMask);
}

NvlStatus 
nvswitch_cci_cmis_cage_bezel_marking
(
    nvswitch_device *device,
    NvU8 cageIndex, 
    char *pBezelMarking
)
{
    return device->hal.nvswitch_cci_cmis_cage_bezel_marking(device, cageIndex, pBezelMarking);
}

NvlStatus
nvswitch_cci_get_grading_values
(
    nvswitch_device *device,
    NvU32           client,
    NvU32           linkId,
    NvU8            *laneMask,
    NVSWITCH_CCI_GRADING_VALUES *pGrading
)
{
    return device->hal.nvswitch_cci_get_grading_values(device, client, linkId, laneMask, pGrading);
}

NvlStatus
nvswitch_cci_get_xcvr_mask
(
    nvswitch_device *device,
    NvU32           *pMaskAll,
    NvU32           *pMaskPresent
)
{
    return device->hal.nvswitch_cci_get_xcvr_mask(device, pMaskAll, pMaskPresent);
}

void
nvswitch_cci_set_xcvr_present
(
    nvswitch_device *device,
    NvU32 maskPresent
)
{
    device->hal.nvswitch_cci_set_xcvr_present(device, maskPresent);
}

void 
nvswitch_cci_destroy
(
    nvswitch_device *device
)
{
    device->hal.nvswitch_cci_destroy(device);
}

NvBool
cciSupported
(
    nvswitch_device *device
)
{
    PCCI pCci = device->pCci;
    NvlStatus retval;

    if (pCci == NULL)
    {
        return NV_FALSE;
    }

    if (pCci->bDiscovered)
    {
        return pCci->bSupported;
    }

    // Discover if CCI supported board
    retval = nvswitch_cci_discovery(device);
    if (retval == NVL_SUCCESS)
    {
        pCci->bSupported = NV_TRUE;
    }
    else
    {
        pCci->bSupported = NV_FALSE;
    }

    pCci->bDiscovered = NV_TRUE;

    return pCci->bSupported;
}

CCI *
cciAllocNew(void)
{
    CCI *pCci = nvswitch_os_malloc(sizeof(*pCci));
    if (pCci != NULL)
    {
        nvswitch_os_memset(pCci, 0, sizeof(*pCci));
    }

    return pCci;
}

static void
_nvswitch_cci_poll_callback
(
    nvswitch_device *device
)
{
    PCCI pCci = device->pCci;
    NvU32 i;

    // call all functions at specified frequencies
    for (i = 0; i < NVSWITCH_CCI_CALLBACK_NUM_MAX; i++)
    {
        if ((pCci->callbackList[i].functionPtr != NULL) &&
            ((pCci->callbackCounter % pCci->callbackList[i].interval) == 0))
        {
            pCci->callbackList[i].functionPtr(device);
        }
    }
    pCci->callbackCounter++;
}

NvlStatus
cciInit
(
    nvswitch_device    *device,
    CCI                *pCci,
    NvU32               pci_device_id
)
{
    if (!nvswitch_is_tnvl_mode_enabled(device))
    {
        nvswitch_task_create(device, _nvswitch_cci_poll_callback,
                             NVSWITCH_INTERVAL_1SEC_IN_NS / NVSWITCH_CCI_POLLING_RATE_HZ,
                             0);
    }
    else
    {
        NVSWITCH_PRINT(device, INFO, "Skipping CCI background task when TNVL is enabled\n");
    }

    return NVL_SUCCESS;
}

// reverse of cciInit()
void
cciDestroy
(
    nvswitch_device    *device,
    CCI                *pCci
)
{
    nvswitch_cci_destroy(device);
}

static NvBool
_nvswitch_cci_module_present
(
    nvswitch_device *device,
    NvU32           osfp
)
{
    return !!(device->pCci->osfpMaskPresent & NVBIT(osfp));
}

static NvlStatus
_nvswitch_cci_get_module_id
(
    nvswitch_device *device,
    NvU32           linkId,
    NvU32           *osfp
)
{
    PCCI pCci = device->pCci;
    NvU32 i;

    for (i = 0; i < pCci->osfp_map_size; i++)
    {
        if (pCci->osfp_map[i].linkId == linkId)
        {
            *osfp = pCci->osfp_map[i].moduleId;

            if (!(device->pCci->cagesMask & NVBIT(*osfp)))
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: osfp %d associated with link %d is not supported\n",
                    __FUNCTION__, linkId, *osfp);
                return -NVL_NOT_FOUND;
            }

            return NVL_SUCCESS;
        }
    }

    return -NVL_NOT_FOUND;
}

NvlStatus
cciWrite
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp,
    NvU32 addr,
    NvU32 length,
    NvU8 *pVal
)
{
    NvlStatus status = NVL_SUCCESS;
    NvBool bRead = NV_FALSE;
    NvBool bBlk = NV_FALSE;

    if (!pVal)
    {
        NVSWITCH_PRINT(device, ERROR,
             "%s: Bad Args!\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if (!device->pCci->bInitialized)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI is not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (!cciModulePresent(device, osfp))
    {
        return -NVL_NOT_FOUND;
    }

    status = nvswitch_cci_setup_module_path(device, client, osfp);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    status = nvswitch_cci_module_access_cmd(device, client, osfp, addr, 
                                            length, pVal, bRead, bBlk);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    return NVL_SUCCESS;
}

NvlStatus
cciRead
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp,
    NvU32 addr,
    NvU32 length,
    NvU8 *pVal
)
{
    NvlStatus status = NVL_SUCCESS;
    NvBool bRead = NV_TRUE;
    NvBool bBlk = NV_FALSE;

    if (!pVal)
    {
        NVSWITCH_PRINT(device, ERROR,
             "%s: Bad Args!\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if (!device->pCci->bInitialized)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI is not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (!cciModulePresent(device, osfp))
    {
        return -NVL_NOT_FOUND;
    }

    status = nvswitch_cci_setup_module_path(device, client, osfp);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    status = nvswitch_cci_module_access_cmd(device, client, osfp, addr, 
                                            length, pVal, bRead, bBlk);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    return NVL_SUCCESS;
}

/*
 * @brief Set bank an page in the CMIS memory table.
 *
 * CMIS4 states, "For a bank change, the host shall write the Bank Select
 * and Page Select registers in the same TWI transaction".
 *
 * Write to Page 0h, byte 126 sets the bank and page.
 */
NvlStatus
cciSetBankAndPage
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp,
    NvU8 bank,
    NvU8 page
)
{
    NvU8 temp[2];

    // Modules with flat memory will fail setting of page and bank
    if (device->pCci->isFlatMemory[osfp])
    {
        if ((page > 0x0) || (bank > 0x0))
        {
            return -NVL_BAD_ARGS;
        }
        return NVL_SUCCESS;
    }

    temp[0] = bank;
    temp[1] = page;

    return cciWrite(device, client, osfp, 126, 2, temp);
}

/*
 * @brief Gets the current bank and page in the CMIS memory table.
 *
 * Read from Page 0h, byte 126 to get the bank and page.
 */
NvlStatus
cciGetBankAndPage
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp,
    NvU8 *pBank,
    NvU8 *pPage
)
{
    NvlStatus status;
    NvU8 temp[2] = {0};

    status = cciRead(device, client, osfp, 126, 2, temp);

    if (pBank != NULL)
    {
        *pBank = temp[0];
    }

    if (pPage != NULL)
    {
        *pPage = temp[1];
    }

    return status;
}

#define NVSWITCH_CCI_MAX_CDB_LENGTH 128

/*
 * @brief Send commands for Command Data Block(CDB) communication.
 *
 * CDB reads and writes are performed on memory map pages 9Fh-AFh.
 *
 * Page 9Fh is used to specify the CDB command and use
 * local payload (LPL) of 120 bytes.
 *
 * Pages A0h-AFh contain up to 2048 bytes of extended payload (EPL).
 *
 * Payload may be a zero-length array if no payload to send.
 *
 * (ref CMIS rev4.0, sections 7.2, 8.13, 8.2.7, 8.4.11).
 */
NvlStatus
cciSendCDBCommand
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp,
    NvU32 command,
    NvU32 length,
    NvU8 *payload,
    NvBool padding
)
{
    NvU8 cmd_msb = (command >> 8) & 0xff;
    NvU8 cmd_lsb = command & 0xff;
    NvU32 chkcode;
    NvU32 i;
    NvU8 temp[8];

    if (length > NVSWITCH_CCI_MAX_CDB_LENGTH)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Command length %d exceeded max length %d. "
            "CDB yet to support extended payloads\n",
            __FUNCTION__, length, NVSWITCH_CCI_MAX_CDB_LENGTH);
        return -NVL_ERR_GENERIC;
    }

    //
    // Compute checksum over payload, including header bytes(command, length,...)
    //
    // CdbChkCode is the ones complement of the summation of page 9Fh,
    // bytes 128 to (135+LPL_Length) with bytes 133,134 and 135 equal to 0.
    // (ref CMIS 4.0, sec 8.13)
    //
    chkcode = cmd_msb + cmd_lsb + length;

    // now add payload bytes
    if (length > 0)
    {
        for (i = 0; i < length; i++)
        {
            chkcode += payload[i];
        }
    }
    chkcode = (~(chkcode & 0xff)) & 0xff;

    // # nw: page 198 of cmis4 spec.
    // # nw:

    // Set page to 0x9F to setup CDB command
    cciSetBankAndPage(device, client, osfp, 0, 0x9f);

    //
    // Send CDB message
    //
    // Fill page 9Fh bytes 128-135 in the order -
    // [cmd_msb, cmd_lsb, epl_length msb, epl_length lsb, lpl_length,
    //  chkcode, resp_length = 0, resp_chkcode = 0]
    //
    // LPL starts from Bytes 136 upto 120 bytes.
    // The command is triggered when byte 129 is written. So fill bytes 128, 129 at the end.
    //

    // #1. Write bytes 130-135. The "header portion", minus the first two bytes
    // which is the command code.
    temp[0] = 0;       // epl_length msb
    temp[1] = 0;       // epl_length lsb
    temp[2] = length;  // lpl_length
    temp[3] = chkcode; // cdb chkcode
    temp[4] = 0;       // response length
    temp[5] = 0;       // response chkcode
    cciWrite(device, client, osfp, 130, 6, temp);

    // #2. If payload's not empty, write the payload (bytes 136 to 255).
    // If payload is empty, infer the command is payload-less and skip.
    if ((length > 0) && padding)
    {
        for (i = length; i < CMIS_CDB_LPL_MAX_SIZE; i++)
        {
            payload[i] = 0;
        }
        cciWrite(device, client, osfp, 136, CMIS_CDB_LPL_MAX_SIZE, payload);
    }
    else if ((length > 0) && !padding)
    {
        cciWrite(device, client, osfp, 136, length, payload);
    }

    // # 3. Write the command code (bytes 128,129), which additionally
    // kicks off processing of the command by the module.
    temp[0] = cmd_msb;
    temp[1] = cmd_lsb;
    cciWrite(device, client, osfp, 128, 2, temp);

    return NVL_SUCCESS;
}

/*!
 * @brief Waits for CDB command completion and returns status.
 *
 * Page 00h byte 37 contains status bits.
 * (see CMIS rev4.0, Table 9-3, CDB Command 0000h: QUERY-Status)
 */
NvlStatus
cciGetCDBStatus
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp,
    NvU8 *pStatus
)
{
    NvBool status_busy;
    NvBool status_fail;
    NvU8 cdb_result;
    NvU8 status;
    NVSWITCH_TIMEOUT timeout;

    status = 0;

    nvswitch_timeout_create(10 * NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);

    do
    {
        cciRead(device, client, osfp, CMIS_CDB_BLOCK_STATUS_BYTE(0), 1, &status);
        *pStatus = status;

        // Quit when the STS_BUSY bit goes to 0
        if (FLD_TEST_REF(CMIS_CDB_BLOCK_STATUS_BYTE_BUSY, _FALSE, status))
        {
            break;
        }

        if (nvswitch_timeout_check(&timeout))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for CDB command to complete! "
                "STATUS = 0x%x\n",
                __FUNCTION__, status);
            break;
        }

        nvswitch_os_sleep(10);
    } while (NV_TRUE);

    status_busy = (status >> 7) & 0x1;
    status_fail = (status >> 6) & 0x1;
    cdb_result =  status & 0x3f;

    if (status_busy) // status is busy
    {
        if (cdb_result == 0x01)
        {
            NVSWITCH_PRINT(device, INFO,
                "%s: CDB status = BUSY. Last Command Result: "
                "'Command is captured but not processed'\n",
                __FUNCTION__);
        }
        else if (cdb_result == 0x02)
        {
            NVSWITCH_PRINT(device, INFO,
                "%s: CDB status = BUSY. Last Command Result: "
                "'Command checking is in progress'\n",
                __FUNCTION__);
        }
        else if (cdb_result == 0x03)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: CDB status = BUSY. Last Command Result: "
                "'Command execution is in progress'\n",
                __FUNCTION__);
        }
        else
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: CDB status = BUSY. Last Command Result: "
                "Unknown (0x%x)\n",
                __FUNCTION__, cdb_result);
        }

        return -NVL_ERR_GENERIC;
    }

    if (status_fail) // status failed
    {
        if (cdb_result == 0x01)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: CDB status = FAIL. Last Command Result: "
                "'CMD Code unknown'\n",
                __FUNCTION__);
        }
        else if (cdb_result == 0x02)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: CDB status = FAIL. Last Command Result: "
                "'Parameter range error or not supported'\n",
                __FUNCTION__);
        }
        else if (cdb_result == 0x03)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: CDB status = FAIL. Last Command Result: "
                "'Previous CMD was not ABORTED by CMD Abort'\n",
                __FUNCTION__);
        }
        else if (cdb_result == 0x04)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: CDB status = FAIL. Last Command Result: "
                "'Command checking time out'\n",
                __FUNCTION__);
        }
        else if (cdb_result == 0x05)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: CDB status = FAIL. Last Command Result: "
                "'CdbCheckCode Error'\n",
                __FUNCTION__);
        }
        else if (cdb_result == 0x06)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: CDB status = FAIL. Last Command Result: "
                "'Password Error'\n",
                __FUNCTION__);
        }
        else
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: CDB status = FAIL. Last Command Result: "
                "Unknown (0x%x)\n",
                __FUNCTION__, cdb_result);
        }

        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

/*!
 * @brief Waits for CDB command completion.
 *
 * Page 00h byte 37 contains status bits. BIT 7 is the busy bit.
 * (see CMIS rev4.0, Table 9-3, CDB Command 0000h: QUERY-Status)
 */
NvlStatus
cciWaitForCDBComplete
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp
)
{
    NVSWITCH_TIMEOUT timeout;
    NvU8 status;

    status = 0;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);

    do
    {
        cciRead(device, client, osfp, CMIS_CDB_BLOCK_STATUS_BYTE(0), 1, &status);

        // Return when the STS_BUSY bit goes to 0
        if (FLD_TEST_REF(CMIS_CDB_BLOCK_STATUS_BYTE_BUSY, _FALSE, status))
        {
            return NVL_SUCCESS;
        }

        if (nvswitch_timeout_check(&timeout))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout waiting for CDB command to complete! STATUS = 0x%x\n",
                __FUNCTION__, status);
            return -NVL_ERR_GENERIC;
        }

        nvswitch_os_sleep(10);
    } while (NV_TRUE);
}

/*!
 * @brief Get the CDB response data.
 *
 * This function must be sent after CDB status is success.
 *
 * Page 9Fh, bytes 134-255 contains response data
 *   Byte 134 : Response LPL Length of the data returned by CDB command code.
 *   Byte 135 : Response LPL ChkCode
 *   Bytes 136-255 : Local payload of the module response.
 */
NvlStatus
cciGetCDBResponse
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp,
    NvU8 *response,
    NvU32 *resLength
)
{
    NvU8 header[8] = {0};
    NvU32 rlpllen;    // Response local payload length
    NvU8 rlplchkcode; // Response local payload check code
    NvU8 chksum = 0;
    NvU32 i;
    NvBool bSkipChecksum = NV_FALSE;

    cciSetBankAndPage(device, client, osfp, 0, 0x9f);

    // get header
    cciRead(device, client, osfp, 128, 8, header);

    // get reported response length
    rlpllen = header[6];
    rlplchkcode = header[7];

    // TODO : Remove this once FW support improves
    if (rlpllen == 0)
    {
        // bug with earlier Stallion FW, presume hit it an read maximum-sized lpl.
        // Assume the maximum length of 120 and skip checksum because it will also
        // be zero
        rlpllen = CMIS_CDB_LPL_MAX_SIZE;
        bSkipChecksum = NV_TRUE;
    }

    if (rlpllen > CMIS_CDB_LPL_MAX_SIZE)
    {
        NVSWITCH_PRINT(device, ERROR,
                    "%s: Error: Invalid CDB response length: %d\n",
                    __FUNCTION__, rlpllen);
        return -NVL_ERR_GENERIC;
    }

    if (rlpllen != 0)
    {
        // get response
        cciRead(device, client, osfp, 136, rlpllen, response);

        if (!bSkipChecksum)
        {
            // compute checksum of response
            for (i = 0; i < rlpllen; i++)
            {
                chksum += response[i];
            }

            // and compare against rlplchkcode (byte 7 of page 9Fh)
            if ((~chksum & 0xff) != rlplchkcode)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Error: RLPLChkCode incorrect for returned data\n",
                    __FUNCTION__);
                return -NVL_ERR_GENERIC;
            }
        }
    }

    if (resLength != NULL)
    {
        *resLength = rlpllen;
    }

    return NVL_SUCCESS;
}

/*!
 * @brief Get the CDB command and get response.
 */
NvlStatus
cciSendCDBCommandAndGetResponse
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp,
    NvU32 command,
    NvU32 payLength,
    NvU8 *payload,
    NvU32 *resLength,
    NvU8 *response,
    NvBool padding
)
{
    NvlStatus retval;
    NvU8 status = 0;

    if (!cciModulePresent(device, osfp))
    {
        NVSWITCH_PRINT(device, INFO,
                "%s: osfp %d is missing\n",
                __FUNCTION__, osfp);
        return -NVL_NOT_FOUND;
    }

    // Wait for CDB status to be free
    retval = cciWaitForCDBComplete(device, client, osfp);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: CDB is busy!!\n",
            __FUNCTION__);
        return -NVL_ERR_GENERIC;
    }

    retval = cciSendCDBCommand(device, client, osfp, command, payLength, payload, padding);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Failed to send CDB Command: 0x%x\n",
            __FUNCTION__, command);
        return -NVL_ERR_GENERIC;
    }

    retval = cciGetCDBStatus(device, client, osfp, &status);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CDB command failed! result = 0x%x\n",
            __FUNCTION__, status);
        return -NVL_ERR_GENERIC;
    }

    retval = cciGetCDBResponse(device, client, osfp, response, resLength);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get CDB command response\n",
            __FUNCTION__);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

NvlStatus 
cciRegisterCallback
(
    nvswitch_device *device,
    NvU32 callbackId,
    void (*functionPtr)(nvswitch_device*),
    NvU32 rateHz
)
{
    PCCI pCci = device->pCci;

    if ((callbackId >= NVSWITCH_CCI_CALLBACK_NUM_MAX) ||
        (functionPtr == NULL))
    {
        return -NVL_BAD_ARGS;
    }

    if ((rateHz == 0) || ((NVSWITCH_CCI_POLLING_RATE_HZ % rateHz) != 0))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Input rate must divide main polling rate: %d\n",
            __FUNCTION__, NVSWITCH_CCI_POLLING_RATE_HZ);
        return -NVL_BAD_ARGS;
    }

    if (pCci->callbackList[callbackId].functionPtr != NULL)
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: CCI callback previously set.\n",
            __FUNCTION__);
    }

    pCci->callbackList[callbackId].interval = NVSWITCH_CCI_POLLING_RATE_HZ/rateHz;
    pCci->callbackList[callbackId].functionPtr = functionPtr;

    return NVL_SUCCESS;
}

// CCI CONTROL CALLS
NvlStatus
nvswitch_ctrl_get_cci_fw_revisions
(
    nvswitch_device *device,
    NVSWITCH_CCI_GET_FW_REVISION_PARAMS *pParams
)
{
    return cciGetFWRevisions(device, 0, pParams->linkId,
                             pParams->revisions);
}

NvlStatus
nvswitch_ctrl_get_grading_values
(
    nvswitch_device *device,
    NVSWITCH_CCI_GET_GRADING_VALUES_PARAMS *pParams
)
{
    return cciGetGradingValues(device, 0, pParams->linkId,
                               &pParams->laneMask, &pParams->grading);
}

NvlStatus
nvswitch_ctrl_get_ports_cpld_info
(
    nvswitch_device *device,
    NVSWITCH_CCI_GET_PORTS_CPLD_INFO_PARAMS *pParams
)
{
    NvlStatus retval = NVL_SUCCESS;
    NvU8 val;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    retval = nvswitch_cci_ports_cpld_read(device, CPLD_MACHXO3_VERSION_MAJOR, &val);
    if (retval != NVL_SUCCESS)
    {
        return -NVL_IO_ERROR;
    }
    pParams->versionMajor = val;

    retval = nvswitch_cci_ports_cpld_read(device, CPLD_MACHXO3_VERSION_MINOR, &val);
    if (retval != NVL_SUCCESS)
    {
        return -NVL_IO_ERROR;
    }
    pParams->versionMinor = val;

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_set_locate_led
(
    nvswitch_device *device,
    NVSWITCH_CCI_SET_LOCATE_LED_PARAMS *pParams
)
{
    NvlStatus retval = NVL_SUCCESS;
    NvU32 cagesMask;
    NvU8 ledState;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    retval = cciGetXcvrMask(device, &cagesMask, NULL);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    if (!(cagesMask & NVBIT(pParams->cageIndex)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Module cage does not exist\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if (pParams->portNum > 1)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid port number\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    ledState = device->pCci->xcvrCurrentLedState[pParams->cageIndex];

    if (pParams->portNum == 0)
    {
        ledState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_A,
                                   pParams->bSetLocateOn ? CCI_LED_STATE_LOCATE : 
                                                           CCI_LED_STATE_OFF,
                                   ledState);
    }
    else
    {
        ledState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_B,
                                   pParams->bSetLocateOn ? CCI_LED_STATE_LOCATE : 
                                                           CCI_LED_STATE_OFF,
                                   ledState);
    }
    
    cciSetNextXcvrLedState(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                           pParams->cageIndex, ledState);
    cciSetXcvrLedState(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                       pParams->cageIndex, NV_FALSE);

    return NVL_SUCCESS;
}

NvlStatus
nvswitch_ctrl_cci_request_ali
(
    nvswitch_device *device,
    NVSWITCH_REQUEST_ALI_PARAMS *pParams
)
{
    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return cciRequestALI(device, pParams->linkMaskTrain);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 * @Brief : Reset CCI on this device.
 *
 * This is temporary and eventually moved to bios.
 * Tracking this in bug 200668898.
 *
 */
static NvlStatus
_nvswitch_reset_cci
(
    nvswitch_device *device
)
{
    return nvswitch_cci_reset(device);
}

/*
 * @Brief : Execute CCI pre-reset sequence for secure reset.
 */
static NvlStatus
_nvswitch_cci_prepare_for_reset
(
    nvswitch_device *device
)
{
    nvswitch_cci_setup_gpio_pins(device);

    return NVL_SUCCESS;
}

/*
 * Check for CMIS boards by pinging on OSFP devices
 */
static NvlStatus
_nvswitch_identify_cci_devices
(
    nvswitch_device *device
)
{
    PCCI pCci = device->pCci;
    NvU32 maskPresent;

    maskPresent = 0;

    nvswitch_cci_get_xcvrs_present(device, &maskPresent);
    pCci->osfpMaskPresent = maskPresent;
    pCci->osfpMaskAll = pCci->cagesMask;
        
    NVSWITCH_PRINT(device, SETUP,
                "%s: Identified modules mask: 0x%x\n",
                __FUNCTION__, pCci->osfpMaskPresent);

    pCci->bInitialized = NV_TRUE;
    
    return NVL_SUCCESS;
}

NvBool
cciIsLinkManaged
(
    nvswitch_device *device,
    NvU32 linkNumber
)
{
    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        return NV_FALSE;
    }

    return  !!(device->pCci->linkMask & NVBIT64(linkNumber));
}

NvlStatus
cciGetLinkMode
(
    nvswitch_device *device,
    NvU32 linkNumber,
    NvU64 *mode
)
{
    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (mode == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    if (!cciIsLinkManaged(device, linkNumber))
    {
        return -NVL_BAD_ARGS;
    }

    if (cciLinkTrainIdle(device, linkNumber))
    {
        *mode = NVLINK_LINKSTATE_OFF;
    }
    else
    {
        *mode = NVLINK_LINKSTATE_TRAINING_CCI;
    }

    return NVL_SUCCESS;
}

/*
 * @Brief : Bootstrap CCI on the specified device
 *
 * @param[in] device Bootstrap CCI on this device
 */
NvlStatus
cciLoad
(
    nvswitch_device *device
)
{
    NvlStatus status;

    if (IS_FMODEL(device) || IS_RTLSIM(device))
    {
        NVSWITCH_PRINT(device, SETUP,
            "%s: Skipping CCI init on preSilicon.\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (device->pCci == NULL)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_INIT,
            "Failed to init CCI(0)\n");
        return -NVL_BAD_ARGS;
    }

    // Prepare CCI for reset.
    status = _nvswitch_cci_prepare_for_reset(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_RESET,
            "Failed to reset CCI(0)\n");
        return status;
    }

    // Reset CCI
    status = _nvswitch_reset_cci(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_RESET,
            "Failed to reset CCI(1)\n");
        return status;
    }

    // Identify CCI devices
    status = _nvswitch_identify_cci_devices(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_INIT,
            "Failed to init CCI(1)\n");
        return status;
    }

    // Complete CCI setup
    status = nvswitch_cci_setup_onboard(device);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_INIT,
            "Failed to init CCI(2)\n");
        return status;
    }

    NVSWITCH_PRINT(device, SETUP,
                   "%s: CCI load successful.\n",
                   __FUNCTION__);

    return status;
}

/*
 * @brief Get FW revisions of the osfp device by link Id
 *
 *  Module FW revision is obtained from CDB command 0x100.
 *  (ref CMIS rev4.0, Table 9-16 CDB Command 0100h: Get firmware Info)
 */
NvlStatus
cciGetFWRevisions
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            linkId,
    NVSWITCH_CCI_GET_FW_REVISIONS *pRevisions
)
{
    NvU32 osfp;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (cciGetModuleId(device, linkId, &osfp) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get moduleid associated with link %d\n",
            __FUNCTION__, linkId);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return cciGetXcvrFWRevisions(device, client, osfp, pRevisions);
}

static NvlStatus
_cciGetXcvrFWRevisionsFlatMem
(
    nvswitch_device *device,
    NvU32           client,
    NvU32            osfp,
    NVSWITCH_CCI_GET_FW_REVISIONS *pRevisions
)
{
    NvlStatus retVal;
    NvU8 versionByte;

    retVal = cciCmisRead(device, osfp, 0, 0, 
                         CMIS_ACTIVE_FW_MAJOR_REVISION, 1, &versionByte);
    if (retVal != NVL_SUCCESS)
    {
        return retVal;
    }

    pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].major = versionByte;

    retVal = cciCmisRead(device, osfp, 0, 0, 
                         CMIS_ACTIVE_FW_MINOR_REVISION, 1, &versionByte);
    if (retVal != NVL_SUCCESS)
    {
        return retVal;
    }

    pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].minor = versionByte;

    if (pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].major ||
        pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].minor)
    {
        pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].flags =
                FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _ACTIVE, _YES,
                    pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].flags);
        pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].flags =
                FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _PRESENT, _YES,
                    pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].flags);
    }
    
    // Entries for images A and B will be left as 0
    
    return NVL_SUCCESS;
}

/*
 * @brief Get FW revisions of the osfp device by OSFP xceiver index
 *
 *  Module FW revision is obtained from CDB command 0x100.
 *  (ref CMIS rev4.0, Table 9-16 CDB Command 0100h: Get firmware Info)
 */
NvlStatus
cciGetXcvrFWRevisions
(
    nvswitch_device *device,
    NvU32           client,
    NvU32            osfp,
    NVSWITCH_CCI_GET_FW_REVISIONS *pRevisions
)
{
    NvU8 response[CMIS_CDB_LPL_MAX_SIZE];
    NvU32 resLength;
    NvlStatus retVal;
    NvU8 status;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (osfp >= (sizeof(device->pCci->osfpMaskAll) * 8))
    {
        return -NVL_BAD_ARGS;
    }

    if (!(device->pCci->osfpMaskPresent & NVBIT(osfp)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: osfp %d is missing\n",
            __FUNCTION__, osfp);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    nvswitch_os_memset(pRevisions, 0, sizeof(NVSWITCH_CCI_GET_FW_REVISIONS));

    if (device->pCci->isFlatMemory[osfp])
    {
        retVal = _cciGetXcvrFWRevisionsFlatMem(device, client, osfp, pRevisions);
        if (retVal != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to get FW revisions\n",
                __FUNCTION__);
            return -NVL_ERR_GENERIC;
        }

        return NVL_SUCCESS;
    }

    retVal = cciSendCDBCommandAndGetResponse(device, client, osfp,
        0x100, 0, NULL, &resLength, response, NV_FALSE);
    if (retVal != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get FW revisions\n",
            __FUNCTION__);
        return -NVL_ERR_GENERIC;
    }

    // Byte 0(or 136) contains FW status
    status = response[0];

    pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].flags = 0;
    if (status == 0)
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Factory Boot Image is Running\n",
            __FUNCTION__);

       pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].flags =
           FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _ACTIVE, _YES,
                       pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].flags);
    }

    if (response[1] & NVBIT(2))
    {
        //
        // For Factory Image,
        //   Byte 74(or 210) contains major revision
        //   Byte 75(or 211) contains minor revision
        //   Byte 76, 77(or 212, 213) contains build number
        //
        pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].flags =
            FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _PRESENT, _YES,
                        pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].flags);
        pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].major = response[74];
        pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].minor = response[75];
        pRevisions[NVSWITCH_CCI_FW_IMAGE_FACTORY].build = (response[76] << 4 | response[77]);
    }

    pRevisions[NVSWITCH_CCI_FW_IMAGE_A].flags = 0;
    if (status & NVBIT(0))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Image A is Running\n",
            __FUNCTION__);

        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].flags =
            FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _ACTIVE, _YES,
                        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].flags);
    }
    if (status & NVBIT(1))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Image A is committed, module boots from Image A\n",
            __FUNCTION__);
        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].flags =
            FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _COMMITED, _YES,
                        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].flags);
    }
    if (status & NVBIT(2))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Image A is erased/empty\n",
            __FUNCTION__);
        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].flags =
            FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _EMPTY, _YES,
                        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].flags);
    }

    if (response[1] & NVBIT(0))
    {
        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].flags =
            FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _PRESENT, _YES,
                        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].flags);
        //
        // For Image A,
        //   Byte 2(or 138) contains major revision
        //   Byte 3(or 139) contains minor revision
        //   Byte 4, 5(or 140, 141) contains build number
        //
        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].major = response[2];
        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].minor = response[3];
        pRevisions[NVSWITCH_CCI_FW_IMAGE_A].build = (response[4] << 4 | response[5]);
    }

    pRevisions[NVSWITCH_CCI_FW_IMAGE_B].flags = 0;
    if (status & NVBIT(4))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Image B is Running\n",
            __FUNCTION__);

        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].flags =
            FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _ACTIVE, _YES,
                        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].flags);
    }
    if (status & NVBIT(5))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Image B is committed, module boots from Image B\n",
            __FUNCTION__);
        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].flags =
            FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _COMMITED, _YES,
                        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].flags);
    }
    if (status & NVBIT(6))
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Image B is erased/empty\n",
            __FUNCTION__);
        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].flags =
            FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _EMPTY, _YES,
                        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].flags);
    }

    if (response[1] & NVBIT(1))
    {
        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].flags =
            FLD_SET_DRF(SWITCH, _CCI_FW_FLAGS, _PRESENT, _YES,
                        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].flags);
        //
        // For Image B,
        //   Byte 38(or 174) contains major revision
        //   Byte 39(or 175) contains minor revision
        //   Byte 40, 41(or 176, 177) contains build number
        //
        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].major = response[38];
        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].minor = response[39];
        pRevisions[NVSWITCH_CCI_FW_IMAGE_B].build = (response[40] << 8 | response[41]);
    }

    return NVL_SUCCESS;
}

/*
 * @brief Get xceiver LED state by OSFP xceiver index.
 *
 */
NvlStatus
cciGetXcvrLedState
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            osfp,
    NvU8            *pLedState
)
{
    NvlStatus status;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = nvswitch_cci_get_xcvr_led_state(device, client, osfp, pLedState);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    return status;
}

/*
 * @brief Set the next LED state for the given OSFP.
 *        The HW will reflect this state on the next iteration of link 
 *        state update callback.   
 */
NvlStatus
cciSetNextXcvrLedState
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            osfp,
    NvU8             nextLedState
)
{
    NvlStatus status;
    NvU32 cagesMask;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = cciGetXcvrMask(device, &cagesMask, NULL);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    if ((cagesMask & NVBIT32(osfp)) == 0)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Module cage not found\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    device->pCci->xcvrNextLedState[osfp] = nextLedState;

    return status;
}

/*
 * @brief Set HW xceiver LED state (Locate On/Off) by OSFP xceiver index.
 *        If Locate is off then set LED state based on link state.
 *
 */
NvlStatus
cciSetXcvrLedState
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            osfp,
    NvBool           bSetLocate
)
{
    NvlStatus status;
    NvU32 cagesMask;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    status = cciGetXcvrMask(device, &cagesMask, NULL);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    if ((cagesMask & NVBIT32(osfp)) == 0)
    {
        return -NVL_BAD_ARGS;
    }

    status = nvswitch_cci_set_xcvr_led_state(device, client, osfp, bSetLocate);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    return status;
}

/*
 * @brief Determine which OSFP transceivers are connected
 *
 */
void
cciDetectXcvrsPresent
(
    nvswitch_device *device
)
{
    NvU32 maskPresent;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return;
    }

    maskPresent = 0;

    nvswitch_cci_get_xcvrs_present(device, &maskPresent);
    nvswitch_cci_set_xcvr_present(device, maskPresent);
}

/*
 * @brief Get the bitset mask of connected OSFP transceivers
 *
 */
NvlStatus
cciGetXcvrMask
(
    nvswitch_device *device,
    NvU32           *pMaskAll,
    NvU32           *pMaskPresent
)
{
    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return nvswitch_cci_get_xcvr_mask(device, pMaskAll, pMaskPresent);
}

NvlStatus
cciConfigureNvlinkModeModule
(
    nvswitch_device *device,
    NvU32            client,
    NvU8             moduleId,
    NvU64            linkMask,
    NvBool           freeze_maintenance,
    NvBool           restart_training,
    NvBool           nvlink_mode
)
{
    NvlStatus status;
    NvU8 payload[CMIS_CDB_LPL_MAX_SIZE];
    NvU8 response[CMIS_CDB_LPL_MAX_SIZE];
    NvU32 resLength;
    NvU8  linkId;
    NvU8  laneMask;
    NvU8  laneMaskTemp;

    laneMask = 0;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    FOR_EACH_INDEX_IN_MASK(64, linkId, linkMask)
    {
        status = cciGetLaneMask(device, linkId, &laneMaskTemp);
        if (status != NVL_SUCCESS)
        {
            return status;
        }
        laneMask |= laneMaskTemp;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    payload[0] = 0;
    payload[1] = (freeze_maintenance << 4) + (restart_training << 1) + nvlink_mode;
    payload[2] = 0;

    // Tx
    payload[3] = laneMask; 
    payload[4] = 0;

    // Rx
    payload[5] = laneMask;

    status = cciSendCDBCommandAndGetResponse(device, client, moduleId, NVSWITCH_CCI_CDB_CMD_ID, 6,
                                            payload, &resLength, response, NV_FALSE);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed to configure nvlink mode\n",
                       __FUNCTION__);
        return status;
    }

    return NVL_SUCCESS;
}

/*
 * @brief Configures individual RX and TX osfp lanes to NVLink mode
 *
 * Freeze Maintenance(FM) : When pre-training starts this should be set, and later
 *   cleared once link becomes active.
 *
 * Restart Training(RT) : must be set only when linkup flow is reset and pre-training
 *   should be performed.
 *
 * Nvlink Mode(NV) : Must be set to 0x1 for NVLink Mode.
 *
 * CDB address 0xCD19. Write sequence -
 *  [0, (0,0,0,0,FM,0,0,RT,NV), TX CH Mask 15..8(00h), TX CH Mask 7..0(FFh), RX CH Mask 15..8 (00h), RX CH Mask 7..0(FFh)]
 *
 * (ref cdb_prospector.pdf)
 */
NvlStatus
cciConfigureNvlinkMode
(
    nvswitch_device *device,
    NvU32            client,
    NvU32            linkId,
    NvBool           bTx,
    NvBool           freeze_maintenance,
    NvBool           restart_training,
    NvBool           nvlink_mode
)
{
    NvU8 payload[CMIS_CDB_LPL_MAX_SIZE];
    NvU8 response[CMIS_CDB_LPL_MAX_SIZE];
    NvU32 resLength;
    NvlStatus status;
    NvU32 osfp;
    NvU8 laneMask;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (cciGetModuleId(device, linkId, &osfp) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get moduleId associated with link %d\n",
            __FUNCTION__, linkId);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (!cciModulePresent(device, osfp))
    {
        return -NVL_NOT_FOUND;
    }

    if (cciGetLaneMask(device, linkId, &laneMask) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get osfp lanemask associated with link %d\n",
            __FUNCTION__, linkId);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    payload[0] = 0;
    payload[1] = (freeze_maintenance<<4)+(restart_training<<1)+nvlink_mode;
    payload[2] = 0;
    payload[3] = bTx ? laneMask : 0;
    payload[4] = 0;
    payload[5] = bTx ? 0 : laneMask;

    status = cciSendCDBCommandAndGetResponse(device, client, osfp,
        NVSWITCH_CCI_CDB_CMD_ID, 6, payload, &resLength, response, NV_FALSE);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to configure nvlink mode\n",
            __FUNCTION__);
        return status;
    }

    return NVL_SUCCESS;
}

/*
 * @brief Check for optical pre training completion
 *
 * CDB address CD20h - Retrieves individual RX and TX channels training status.
 * Read sequence -
 *  [TX CH Mask 15..8(00h), TX CH Mask 7..0(FFh), RX CH Mask 15..8 (00h), RX CH Mask 7..0(FFh)]
 *
 * (ref cdb_prospector.pdf)
 */
NvBool
cciCheckForPreTraining
(
    nvswitch_device *device,
    NvU32           client,
    NvU32           linkId,
    NvBool          bTx
)
{
    NvlStatus status;
    NvU8 response[CMIS_CDB_LPL_MAX_SIZE];
    NvU32 resLength;
    NvU32 osfp;
    NvU8 train_mask;
    NvU8 lane_mask;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return NV_FALSE;
    }

    if (cciGetModuleId(device, linkId, &osfp) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get moduleId associated with link %d\n",
            __FUNCTION__, linkId);
        return NV_FALSE;
    }

    if (!cciModulePresent(device, osfp))
    {
        return NV_FALSE;
    }

    if (cciGetLaneMask(device, linkId, &lane_mask) != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get osfp lanemask associated with link %d\n",
            __FUNCTION__, linkId);
        return NV_FALSE;
    }

    status = cciSendCDBCommandAndGetResponse(device, client, osfp,
        0xcd20, 0, NULL, &resLength, response, NV_FALSE);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get nvlink status\n",
            __FUNCTION__);
        return NV_FALSE;
    }

    train_mask = bTx ? response[1] : response[3];
    if ((lane_mask & train_mask) == lane_mask)
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: pre-training completed on link %d!\n",
            __FUNCTION__, linkId);        
        return NV_TRUE;
    }

    return NV_FALSE;
}

NvlStatus
cciApplyControlSetValues
(
    nvswitch_device *device,
    NvU32           client,
    NvU32           moduleMask
)
{
    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    return nvswitch_cci_apply_control_set_values(device, client, moduleMask);
}

NvlStatus
cciGetGradingValues
(
    nvswitch_device *device,
    NvU32           client,
    NvU32           linkId,
    NvU8            *laneMask,
    NVSWITCH_CCI_GRADING_VALUES *pGrading
)
{
    return nvswitch_cci_get_grading_values(device, client, linkId, laneMask, pGrading);
}

/*
 * @brief Gets the mapping between cageIndex and link Ids
 * Returns a bitmask containing all links mapped to the given
 * cage. Also returns a value that encodes other information
 * including the mapping between OSFP link lane and Nvlink link
 */
NvlStatus
cciGetCageMapping
(
    nvswitch_device *device,
    NvU8            cageIndex,
    NvU64           *pLinkMask,
    NvU64           *pEncodedValue
)
{
    NVSWITCH_CCI_MODULE_LINK_LANE_MAP *p_nvswitch_cci_osfp_map;
    NvU64 linkMask;
    NvU64 encodedValue;
    NvU8 *pEncodedByte;
    NvU32 nvswitch_cci_osfp_map_size;
    NvU32 i;
    NvU8 linkId;
    NvU8 osfpLane;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not initialized\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    linkMask = 0;
    encodedValue = 0;
    pEncodedByte = (NvU8 *)&encodedValue;
    p_nvswitch_cci_osfp_map = device->pCci->osfp_map;
    nvswitch_cci_osfp_map_size = device->pCci->osfp_map_size;

    for (i = 0; i < nvswitch_cci_osfp_map_size; i++)
    {
        if (p_nvswitch_cci_osfp_map[i].moduleId == cageIndex)
        {
            linkId = p_nvswitch_cci_osfp_map[i].linkId;
            NVSWITCH_ASSERT(linkId <= 63);

            linkMask |= NVBIT64(linkId);
            FOR_EACH_INDEX_IN_MASK(8, osfpLane, p_nvswitch_cci_osfp_map[i].laneMask)
            {
                pEncodedByte[osfpLane] =
                    REF_NUM(NVSWITCH_CCI_CMIS_NVLINK_MAPPING_ENCODED_VALUE_LINK_ID, linkId);
            }
            FOR_EACH_INDEX_IN_MASK_END;
        }
    }

    if (pLinkMask != NULL)
    {
        *pLinkMask = linkMask;
    }

    if (pEncodedValue != NULL)
    {
        *pEncodedValue = encodedValue;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cciCmisAccessSetup
(
    nvswitch_device *device,
    NvU8            cageIndex,
    NvU8            bank,
    NvU8            page,
    NvU8            address,
    NvU8            count,
    NvU8            *pSavedBank,
    NvU8            *pSavedPage
)
{
    NvlStatus status;
    NvU32 cagesMask;

    status = cciGetXcvrMask(device, &cagesMask, NULL);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    if (!(cagesMask & NVBIT(cageIndex)))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Provided cage index does not exist.\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if (count == 0 ||
        count > NVSWITCH_CCI_CMIS_MEMORY_ACCESS_BUF_SIZE)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Invalid byte count.\n",
            __FUNCTION__);
        return -NVL_BAD_ARGS;
    }

    if (device->pCci->isFlatMemory[cageIndex])
    {
        if (page != 0 || bank !=0)
        {
            return -NVL_ERR_NOT_SUPPORTED;
        }
        else
        {
            return NVL_SUCCESS;
        }
    }

    if (address >= 0x80)
    {
        // Save previous bank and page
        status = cciGetBankAndPage(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                                   cageIndex, pSavedBank, pSavedPage);
        if (status != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to save bank and page.\n",
                __FUNCTION__);
            return status;
        }

        // Don't change bank and page unnecessarily
        if (*pSavedBank != bank || *pSavedPage != page)
        {
            status = cciSetBankAndPage(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                                    cageIndex, bank, page);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Failed to set bank and page.\n",
                    __FUNCTION__);
                return status;
            }          
        }
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cciCmisAccessRestore
(
    nvswitch_device *device,
    NvU8            cageIndex,
    NvU8            bank,
    NvU8            page,
    NvU8            address,
    NvU8            savedBank,
    NvU8            savedPage
)
{
    NvlStatus status;

    if (device->pCci->isFlatMemory[cageIndex])
    {
        if (page != 0 || bank !=0)
        {
            return -NVL_ERR_NOT_SUPPORTED;
        }
        else
        {
            return NVL_SUCCESS;
        }
    }

    // Restore previous bank and page
    if (address >= 0x80)
    {
        if (savedBank != bank || savedPage != page)
        {
            status = cciSetBankAndPage(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                                    cageIndex, savedBank, savedPage);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Failed to restore bank and page.\n",
                    __FUNCTION__);
                return status;
            }   
        }
    }

    return NVL_SUCCESS;
}

static NvBool
_cciCmisAccessAllowed
(
    nvswitch_device *device,
    NvU8            cageIndex
)
{
    PCCI pCci = device->pCci;
    NvU32 pid;
    NvlStatus status;

    status = nvswitch_os_get_pid(&pid);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: CCI CMIS lock not supported\n",
            __FUNCTION__);
        return NV_TRUE;
    }

    // Reject clients with different PIDs that attempt access
    if (pCci->cmisAccessLock[cageIndex].bLocked && 
        (pid != pCci->cmisAccessLock[cageIndex].pid))
    {
        return NV_FALSE; 
    }

    return NV_TRUE;
}

/*!
 * @brief Try to obtain lock for CMIS accesses
 */
NvBool
cciCmisAccessTryLock
(
    nvswitch_device *device,
    NvU8            cageIndex
)
{
    PCCI pCci = device->pCci;
    NvU64 timestampCurr;
    NvU64 timestampSaved;
    NvU32 pid;
    NvlStatus status;

    status = nvswitch_os_get_pid(&pid);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: CCI CMIS lock not supported\n",
            __FUNCTION__);
        return NV_TRUE;
    }

    timestampCurr = nvswitch_os_get_platform_time();
    timestampSaved = pCci->cmisAccessLock[cageIndex].timestamp;

    // Release lock if modules accesses have been idle
    if ((timestampCurr - timestampSaved) >= NVSWITCH_CCI_CMIS_LOCK_TIMEOUT)
    {
        cciCmisAccessReleaseLock(device, cageIndex);
    }
    
    if (!_cciCmisAccessAllowed(device, cageIndex))
    {
        return NV_FALSE;
    }
   
    // Lock CMIS access to module. External clients may attempt to lock multiple times 
    pCci->cmisAccessLock[cageIndex].bLocked = NV_TRUE;
    pCci->cmisAccessLock[cageIndex].timestamp = timestampCurr;
    pCci->cmisAccessLock[cageIndex].pid = pid;

    return NV_TRUE;
}

/*!
 * @brief Release lock for CMIS accesses
 */
void
cciCmisAccessReleaseLock
(
    nvswitch_device *device,
    NvU8            cageIndex
)
{
    PCCI pCci = device->pCci;
    NvU32 pid;
    NvlStatus status;

    status = nvswitch_os_get_pid(&pid);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, WARN,
            "%s: CCI CMIS lock not supported\n",
            __FUNCTION__);
        return;
    }

    pCci->cmisAccessLock[cageIndex].bLocked = NV_FALSE;
}

/*!
 * @brief Read from specified module cage.
 *        Sets the bank and page if necessary.
 */
NvlStatus
cciCmisRead
(
    nvswitch_device *device,
    NvU8            cageIndex,
    NvU8            bank,
    NvU8            page,
    NvU8            address,
    NvU8            count,
    NvU8            *pData
)
{
    NvU8 savedBank;
    NvU8 savedPage;
    NvlStatus status;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    // Prevent internal driver CMIS reads from occuring while access is locked
    if (!_cciCmisAccessAllowed(device, cageIndex))
    {
        return -NVL_ERR_STATE_IN_USE;
    }

    // Perform checks and save bank/page if needed
    status = _cciCmisAccessSetup(device, cageIndex, bank, page, 
                                 address, count, &savedBank, &savedPage);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    status = cciRead(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                     cageIndex, address, count, pData);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to read from module cage %d\n",
            __FUNCTION__, cageIndex);
        return status;
    }

    status = _cciCmisAccessRestore(device, cageIndex, bank, page, 
                                   address, savedBank, savedPage);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    return NVL_SUCCESS;
}

/*!
 * @brief Write to specified module cage.
 *        Sets the bank and page if necessary.
 */
NvlStatus
cciCmisWrite
(
    nvswitch_device *device,
    NvU8            cageIndex,
    NvU8            bank,
    NvU8            page,
    NvU8            address,
    NvU8            count,
    NvU8            *pData
)
{
    NvU8 savedBank;
    NvU8 savedPage;
    NvlStatus status;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    // Prevent internal driver CMIS reads from occuring while access is locked
    if (!_cciCmisAccessAllowed(device, cageIndex))
    {
        return -NVL_ERR_STATE_IN_USE;
    }

    // Perform checks and save bank/page if needed
    status = _cciCmisAccessSetup(device, cageIndex, bank, page, 
                                 address, count, &savedBank, &savedPage);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    status = cciWrite(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                     cageIndex, address, count, pData);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to write to module cage %d\n",
            __FUNCTION__, cageIndex);
        return status;
    }

    status = _cciCmisAccessRestore(device, cageIndex, bank, page, 
                                   address, savedBank, savedPage);
    if (status != NVL_SUCCESS)
    {
        return status;
    }

    return NVL_SUCCESS;
}

NvlStatus
cciCmisCageBezelMarking
(
    nvswitch_device *device,
    NvU8 cageIndex,
    char *pBezelMarking
)
{
    return nvswitch_cci_cmis_cage_bezel_marking(device, cageIndex, pBezelMarking);
}

// Helper functions for CCI subcomponents
NvlStatus
cciGetModuleId
(
    nvswitch_device *device,
    NvU32           linkId,
    NvU32           *osfp
)
{
    return _nvswitch_cci_get_module_id(device, linkId, osfp);
}

NvBool
cciModulePresent
(
    nvswitch_device *device,
    NvU32           osfp
)
{
    return _nvswitch_cci_module_present(device, osfp);
}

void
cciGetModulePresenceChange
(
    nvswitch_device *device,
    NvU32           *pModuleMask
)
{
    nvswitch_cci_get_xcvrs_present_change(device, pModuleMask);
}

NvlStatus
cciResetModule
(
    nvswitch_device *device,
    NvU32           moduleId
)
{
    NvlStatus retval;
    NvU8 regByte;

    // Assert OSFP reset
    regByte = NVBIT32(moduleId);
    retval = nvswitch_cci_ports_cpld_write(device, CPLD_MACHXO3_PORTS_RESET_REG1, regByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    regByte = (NVBIT32(moduleId) >> 8);
    retval = nvswitch_cci_ports_cpld_write(device, CPLD_MACHXO3_PORTS_RESET_REG2, regByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    nvswitch_os_sleep(1);

    // De-assert OSFP reset
    retval = nvswitch_cci_ports_cpld_write(device, CPLD_MACHXO3_PORTS_RESET_REG1, 0);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    retval = nvswitch_cci_ports_cpld_write(device, CPLD_MACHXO3_PORTS_RESET_REG2, 0);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Assert lpmode
    retval = cciSetLPMode(device, moduleId, NV_TRUE);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    return NVL_SUCCESS;
}

/*
 * @brief Get FW info 
 *        Module FW info is obtained from CDB command 0x100.
 *        (ref CMIS rev4.0, Table 9-16 CDB Command 0100h: Get firmware Info)
 *
 * @pre pInfo points to a buffer of size CMIS_CDB_LPL_MAX_SIZE bytes
 */
NvlStatus
cciGetXcvrFWInfo
(
    nvswitch_device *device,
    NvU32 moduleId,
    NvU8 *pInfo
)
{
    NvlStatus retVal;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (pInfo == NULL)
    {
        return -NVL_BAD_ARGS;
    }

    if (!cciModulePresent(device, moduleId))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Module %d is missing\n",
            __FUNCTION__, moduleId);
        return -NVL_NOT_FOUND;
    }

    retVal = cciSendCDBCommandAndGetResponse(device, 0, moduleId,
        0x100, 0, NULL, NULL, pInfo, NV_FALSE);
    if (retVal != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get FW revisions\n",
            __FUNCTION__);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

NvlStatus
cciSetLPMode
(
    nvswitch_device *device,
    NvU8 moduleId,
    NvBool bAssert
)
{
    NvlStatus retval;
    NvU8 valReg1;
    NvU8 valReg2;

    retval = nvswitch_cci_ports_cpld_read(device, CPLD_MACHXO3_PORTS_LPMODE_REG1, &valReg1);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    retval = nvswitch_cci_ports_cpld_read(device, CPLD_MACHXO3_PORTS_LPMODE_REG2, &valReg2);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    if (bAssert)
    {
        valReg1 = valReg1 | NVBIT32(moduleId);
        valReg2 = valReg2 | (NVBIT32(moduleId) >> 8);
    }
    else
    {
        valReg1 = valReg1 & ~NVBIT32(moduleId);
        valReg2 = valReg2 & ~(NVBIT32(moduleId) >> 8);
    }

    retval = nvswitch_cci_ports_cpld_write(device, CPLD_MACHXO3_PORTS_LPMODE_REG1, valReg1);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    retval = nvswitch_cci_ports_cpld_write(device, CPLD_MACHXO3_PORTS_LPMODE_REG2, valReg2);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    return NVL_SUCCESS;
}

NvBool
cciCheckLPMode
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;
    NvU8 valReg1;
    NvU8 valReg2;

    retval = nvswitch_cci_ports_cpld_read(device, CPLD_MACHXO3_PORTS_LPMODE_REG1, &valReg1);
    if (retval != NVL_SUCCESS)
    {
        return NV_FALSE;
    }

    retval = nvswitch_cci_ports_cpld_read(device, CPLD_MACHXO3_PORTS_LPMODE_REG2, &valReg2);
    if (retval != NVL_SUCCESS)
    {
        return NV_FALSE;
    }

    if ((valReg1 & NVBIT32(moduleId)) ||
        (valReg2 & (NVBIT32(moduleId) >> 8)))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

void
cciPingModules
(
    nvswitch_device *device,
    NvU32 *pMaskPresent
)
{
    NVSWITCH_CTRL_I2C_INDEXED_PARAMS i2c_params = { 0 };
    NvU32 idx_i2cdevice;
    PCCI pCci;
    NvU32 presentMask;
    NvU32 client;
    NvlStatus retval;

    pCci = device->pCci;
    presentMask = 0;
    client = NVSWITCH_I2C_ACQUIRER_CCI_INITIALIZE;

    for (idx_i2cdevice = 0; idx_i2cdevice < pCci->osfp_num; idx_i2cdevice++)
    {
        retval = nvswitch_cci_setup_module_path(device, client, idx_i2cdevice);
        if (retval != NVL_SUCCESS)
        {
            continue;
        }

        i2c_params.port = pCci->osfp_i2c_info[idx_i2cdevice].i2cPortLogical;
        i2c_params.address = (NvU16) pCci->osfp_i2c_info[idx_i2cdevice].i2cAddress;
        i2c_params.bIsRead = NV_FALSE;
        i2c_params.flags =
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _START, _SEND) |
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _RESTART, _NONE) |
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _STOP, _SEND) |
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _ADDRESS_MODE, _7BIT) |
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _INDEX_LENGTH, _ZERO) |
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _FLAVOR, _HW) |
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _SPEED_MODE, _400KHZ) |
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _BLOCK_PROTOCOL, _DISABLED) |
            DRF_DEF(SWITCH_CTRL, _I2C_FLAGS, _TRANSACTION_MODE, _PING);
        i2c_params.messageLength = 0;
        i2c_params.acquirer = client;

        if (nvswitch_ctrl_i2c_indexed(device, &i2c_params) == NVL_SUCCESS)
        {
            // Only print if newly present OSFP
            if (!cciModulePresent(device, idx_i2cdevice))
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s: Identified osfp = %d, port = %d, addr = 0x%x\n",
                    __FUNCTION__, idx_i2cdevice, i2c_params.port, i2c_params.address);
            }

            presentMask |= NVBIT(idx_i2cdevice);
        }
    }

    if (pMaskPresent != NULL)
    {
        *pMaskPresent = presentMask;
    }
}

void
cciGetAllLinks
(
    nvswitch_device *device,
    NvU64 *pLinkMaskAll
)
{
    PCCI pCci = device->pCci;
    NvU64 linkMaskAll;
    NvU32 i;

    linkMaskAll = 0;
    for (i = 0; i < pCci->osfp_map_size; i++)
    {
        linkMaskAll |= NVBIT64(pCci->osfp_map[i].linkId);
    }

    if (pLinkMaskAll != NULL)
    {
        *pLinkMaskAll = linkMaskAll;
    }
}

NvlStatus
cciGetModuleMask
(
    nvswitch_device *device,
    NvU64 linkMask,
    NvU32 *pModuleMask
)
{
    NvU32 cciModuleMask;
    NvU32 moduleId;
    NvU8 linkId;
    NvlStatus retval;

    cciModuleMask = 0;

    FOR_EACH_INDEX_IN_MASK(64, linkId, linkMask)
    {
        retval = cciGetModuleId(device, linkId, &moduleId);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to get moduleId associated with link %d\n",
                __FUNCTION__, linkId);
            return retval;
        }

        cciModuleMask |= NVBIT32(moduleId);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    *pModuleMask = cciModuleMask;

    return NVL_SUCCESS;
}

NvBool
cciCheckXcvrForLinkTraffic
(
    nvswitch_device *device,
    NvU32 osfp,
    NvU64 linkMask
)
{
    NVSWITCH_GET_THROUGHPUT_COUNTERS_PARAMS *pCounterParams = NULL;
    NvU64 *pCounterValues;
    NvU64 tpCounterPreviousSum;
    NvU64 tpCounterCurrentSum;
    NvBool bTraffic = NV_FALSE;
    NvU8 linkNum;

    pCounterParams = nvswitch_os_malloc(sizeof(*pCounterParams));
    if (pCounterParams == NULL)
        goto out;

    pCounterParams->counterMask = NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_TX |
                                  NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_RX;
    pCounterParams->linkMask = linkMask;
    if (nvswitch_ctrl_get_throughput_counters(device,
        pCounterParams) != NVL_SUCCESS)
    {
        goto out;
    }

    // Sum TX/RX traffic for each link
    FOR_EACH_INDEX_IN_MASK(64, linkNum, linkMask)
    {
        pCounterValues = pCounterParams->counters[linkNum].values;

        tpCounterPreviousSum = device->pCci->tpCounterPreviousSum[linkNum];

        // Sum taken to save space as it is unlikely to overflow before system is reset
        tpCounterCurrentSum = pCounterValues[NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_TX] +
                              pCounterValues[NVSWITCH_THROUGHPUT_COUNTERS_TYPE_DATA_RX];

        device->pCci->tpCounterPreviousSum[linkNum] = tpCounterCurrentSum;

        // Skip traffic check in first call on system start up
        if (device->pCci->callbackCounter == 0)
        {
            continue;
        }

        if (tpCounterCurrentSum > tpCounterPreviousSum)
        {
            bTraffic = NV_TRUE;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

out:
    nvswitch_os_free(pCounterParams);
    return bTraffic;
}

NvlStatus
cciGetLaneMask
(
    nvswitch_device *device,
    NvU32           linkId,
    NvU8           *laneMask
)
{
    NVSWITCH_CCI_MODULE_LINK_LANE_MAP * module_map = device->pCci->osfp_map;
    NvU32 module_map_size = device->pCci->osfp_map_size;
    NvU32 i;

    for (i = 0; i < module_map_size; i++)
    {
        if (module_map[i].linkId == linkId)
        {
            *laneMask = module_map[i].laneMask;

            return NVL_SUCCESS;
        }
    }

    return -NVL_NOT_FOUND;
}

void
cciSetModulePower
(
    nvswitch_device *device,
    NvU32 moduleId,
    NvBool bPowerOn
)
{
    NvlStatus status;
    NvU8 regVal;

    if (moduleId < 8)
    {
        status = nvswitch_cci_ports_cpld_read(device, 
                                  CPLD_MACHXO3_LD_SW_EN_REG1,
                                  &regVal);
        if (status == NVL_SUCCESS)
        {
            regVal = bPowerOn ? regVal | NVBIT(moduleId) :
                                regVal & ~NVBIT(moduleId);
            nvswitch_cci_ports_cpld_write(device, 
                                  CPLD_MACHXO3_LD_SW_EN_REG1,
                                  regVal);
        }
    }
    else
    {
        status = nvswitch_cci_ports_cpld_read(device, 
                                  CPLD_MACHXO3_LD_SW_EN_REG2,
                                  &regVal);
        if (status == NVL_SUCCESS)
        {
            regVal = bPowerOn ? regVal | NVBIT(moduleId - 8) :
                                regVal & ~NVBIT(moduleId - 8);
            nvswitch_cci_ports_cpld_write(device, 
                                  CPLD_MACHXO3_LD_SW_EN_REG2,
                                  regVal);
        }
    }
}

static void
_cciClearModuleFault
(
    nvswitch_device *device,
    NvU32 moduleId
)
{
    if (moduleId < 8)
    {
        nvswitch_cci_ports_cpld_write(device, 
                                  CPLD_MACHXO3_LD_SW_FAULT_REG1,
                                  ~NVBIT(moduleId));
    }
    else
    {
        nvswitch_cci_ports_cpld_write(device, 
                                  CPLD_MACHXO3_LD_SW_FAULT_REG2,
                                  ~NVBIT(moduleId - 8));
    }
}

static NvBool
_cciCheckModuleFault
(
    nvswitch_device *device,
    NvU32 moduleId
)
{
    NvlStatus status;
    NvU8 regVal;

    if (moduleId < 8)
    {
        status = nvswitch_cci_ports_cpld_read(device, CPLD_MACHXO3_LD_SW_FAULT_REG1, &regVal);
        if (status != NVL_SUCCESS)
        {
            return NV_TRUE;
        }
        
        if (regVal & NVBIT(moduleId))
        {
            return NV_TRUE;
        }
    }
    else
    {
        status = nvswitch_cci_ports_cpld_read(device, CPLD_MACHXO3_LD_SW_FAULT_REG2, &regVal);
        if (status != NVL_SUCCESS)
        {
            return NV_TRUE;
        }

        if (regVal & NVBIT(moduleId - 8))
        {
            return NV_TRUE;
        }
    }
   
    return NV_FALSE;
}

NvBool
cciModuleHWGood
(
    nvswitch_device *device,
    NvU32 moduleId
)
{
    if (_cciCheckModuleFault(device, moduleId))
    {
        // Attempt to recover module by power cycling 
        cciSetModulePower(device, moduleId, NV_FALSE);
        nvswitch_os_sleep(1);
        cciSetModulePower(device, moduleId, NV_TRUE);

        // Clear fault flag and check again
        _cciClearModuleFault(device, moduleId);
        nvswitch_os_sleep(1);

        if (_cciCheckModuleFault(device, moduleId))
        {
            return NV_FALSE;
        }
    }

    return NV_TRUE;
}

