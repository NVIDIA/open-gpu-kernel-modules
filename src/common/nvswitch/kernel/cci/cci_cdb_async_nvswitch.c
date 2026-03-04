/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "cci/cci_nvswitch.h"
#include "cci/cci_priv_nvswitch.h"

/*!
 * @brief Checks for CDB command completion.
 *
 * Page 00h byte 37 contains status bits. BIT 7 is the busy bit.
 * (see CMIS rev4.0, Table 9-3, CDB Command 0000h: QUERY-Status)
 */
static NvlStatus
_cci_check_for_cdb_complete
(
    nvswitch_device *device,
    NvU32 client,
    NvU32 osfp
)
{
    NvU8 status;

    cciRead(device, client, osfp, CMIS_CDB_BLOCK_STATUS_BYTE(0), 1, &status);

    // Return when the STS_BUSY bit goes to 0
    if (FLD_TEST_REF(CMIS_CDB_BLOCK_STATUS_BYTE_BUSY, _FALSE, status))
    {
        return NVL_SUCCESS;
    }

    return NVL_ERR_STATE_IN_USE;
}

/*!
 * @brief Check if module can recieve CDB command
 */
static NvlStatus
_cci_check_cdb_ready
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_CDB_STATE *pCdbState;
    NvlStatus retval;

    pCdbState = &device->pCci->cdbState[moduleId];
    
    if (pCdbState->cdbPhase != CCI_CDB_PHASE_CHECK_READY)
    {
        return -NVL_ERR_GENERIC;
    }

    if (!cciModulePresent(device, moduleId))
    {
        NVSWITCH_PRINT(device, INFO,
                "%s: osfp %d is missing\n",
                __FUNCTION__, moduleId);
        return -NVL_NOT_FOUND;
    }

    retval = _cci_check_for_cdb_complete(device, pCdbState->client, moduleId);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    pCdbState->cdbPhase = CCI_CDB_PHASE_SEND_COMMAND;

    return NVL_SUCCESS;
}

/*!
 * @brief Send CDB command
 */
static NvlStatus
_cci_send_cdb_command
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;
    CCI_CDB_STATE *pCdbState;
    NvU8 payload[CMIS_CDB_LPL_MAX_SIZE];
    NvU32 payLength;
    NvU32 command;
    NvBool padding;
    NvU8 laneMask;

    pCdbState = &device->pCci->cdbState[moduleId];

    if (pCdbState->cdbPhase != CCI_CDB_PHASE_SEND_COMMAND)
    {
        return -NVL_ERR_GENERIC;
    }

    if (!cciModulePresent(device, moduleId))
    {
        NVSWITCH_PRINT(device, INFO,
                "%s: osfp %d is missing\n",
                __FUNCTION__, moduleId);
        return -NVL_NOT_FOUND;
    }

    // Roll up lanes that will be operated on
    laneMask = pCdbState->laneMasksPending[0] | 
               pCdbState->laneMasksPending[1] |
               pCdbState->laneMasksPending[2] |
               pCdbState->laneMasksPending[3];

    // Clear lanes whose commands will be triggered
    pCdbState->laneMasksPending[0] &= ~laneMask;
    pCdbState->laneMasksPending[1] &= ~laneMask;
    pCdbState->laneMasksPending[2] &= ~laneMask;
    pCdbState->laneMasksPending[3] &= ~laneMask;

    payload[0] = 0;
    payload[1] = (pCdbState->freeze_maintenance << 4) + 
                 (pCdbState->restart_training << 1) + 
                 pCdbState->nvlink_mode;
    payload[2] = 0;

    // Tx
    payload[3] = laneMask; 
    payload[4] = 0;

    // Rx
    payload[5] = laneMask;

    payLength = 6;
    command = NVSWITCH_CCI_CDB_CMD_ID;
    padding = NV_FALSE;

    retval = cciSendCDBCommand(device, pCdbState->client, moduleId, command, payLength, payload, padding);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Failed to send CDB Command: 0x%x\n",
            __FUNCTION__, command);
        return -NVL_ERR_GENERIC;
    }

    pCdbState->cdbPhase = CCI_CDB_PHASE_GET_RESPONSE;

    return NVL_SUCCESS;
}

/*!
 * @brief Get get CDB response.
 */
static NvlStatus
_cci_get_cdb_response
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;
    CCI_CDB_STATE *pCdbState;
    NvU8 cdbStatus = 0;
    NvU8 response[CMIS_CDB_LPL_MAX_SIZE];
    NvU32 resLength;

    pCdbState = &device->pCci->cdbState[moduleId];

    if (pCdbState->cdbPhase != CCI_CDB_PHASE_GET_RESPONSE)
    {
        return -NVL_ERR_GENERIC;
    }

    if (!cciModulePresent(device, moduleId))
    {
        NVSWITCH_PRINT(device, INFO,
                "%s: osfp %d is missing\n",
                __FUNCTION__, moduleId);
        return -NVL_NOT_FOUND;
    }

    retval = _cci_check_for_cdb_complete(device, pCdbState->client, moduleId);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    retval = cciGetCDBStatus(device, pCdbState->client, moduleId, &cdbStatus);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CDB command failed! result = 0x%x\n",
            __FUNCTION__, cdbStatus);
        return -NVL_ERR_GENERIC;
    }

    retval = cciGetCDBResponse(device, pCdbState->client, moduleId, response, &resLength);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to get CDB command response\n",
            __FUNCTION__);
        return -NVL_ERR_GENERIC;
    }

    pCdbState->cdbPhase = CCI_CDB_PHASE_CHECK_DONE;

    return NVL_SUCCESS;
}

/*!
 * @brief 
 */
static NvlStatus
_cci_check_cdb_done
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_CDB_STATE *pCdbState;

    pCdbState = &device->pCci->cdbState[moduleId];

    if (pCdbState->cdbPhase != CCI_CDB_PHASE_CHECK_DONE)
    {
        return -NVL_ERR_GENERIC;
    }

    // Finish pending links
    if (pCdbState->laneMasksPending[0] ||
        pCdbState->laneMasksPending[1] ||
        pCdbState->laneMasksPending[2] ||
        pCdbState->laneMasksPending[3])
    {
        pCdbState->cdbPhase = CCI_CDB_PHASE_CHECK_READY;
    }
    else
    {
        pCdbState->cdbPhase = CCI_CDB_PHASE_IDLE;
    }

    return NVL_SUCCESS;
}

/*
 * @brief Performs CDB stages on a module without blocking
 *
 */
static void
_cci_cdb_perform_phases
(
    nvswitch_device *device,
    NvU8  moduleId
)
{
    NVSWITCH_TIMEOUT timeout;
    NvlStatus retval = NVL_SUCCESS;
    NvBool bContinue = NV_TRUE;
    CCI_CDB_STATE *pCdbState;

    pCdbState = &device->pCci->cdbState[moduleId];

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS, &timeout);

    do
    {
        switch (pCdbState->cdbPhase)
        {
            case CCI_CDB_PHASE_CHECK_READY:
            {
                retval = _cci_check_cdb_ready(device, moduleId);
                break;
            }
            case CCI_CDB_PHASE_SEND_COMMAND:
            {
                retval = _cci_send_cdb_command(device, moduleId);
                break;
            }
            case CCI_CDB_PHASE_GET_RESPONSE:
            {
                retval = _cci_get_cdb_response(device, moduleId);
                break;
            }
            case CCI_CDB_PHASE_CHECK_DONE:
            {
                retval = _cci_check_cdb_done(device, moduleId);
                break;
            }
            default:
            {
                retval = NVL_SUCCESS;
                bContinue = NV_FALSE;
                NVSWITCH_ASSERT(pCdbState->cdbPhase == CCI_CDB_PHASE_IDLE);
                break;
            }
        }

        // Module is busy
        if (retval == NVL_ERR_STATE_IN_USE)
        {
            // Nothing more to do for now
            bContinue = NV_FALSE;
        }

        if (retval < 0)
        {
            NVSWITCH_PRINT(device, ERROR,
                    "%s: CDB error module %d, phase %d!\n",
                    __FUNCTION__, moduleId, pCdbState->cdbPhase);
            bContinue = NV_FALSE;
            pCdbState->cdbPhase = CCI_CDB_PHASE_CHECK_DONE;
        }

        // Just in case
        if (nvswitch_timeout_check(&timeout))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Timeout!\n",
                    __FUNCTION__);
            break;
        }
    } while(bContinue);
}

void
cciProcessCDBCallback
(
    nvswitch_device         *device
)
{
    NVSWITCH_TIMEOUT timeout;
    NvU32 moduleMaskPriority;
    NvU32 moduleMask;
    NvU8  moduleId;

    moduleMaskPriority = 0;

    nvswitch_timeout_create(20 * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);

    // Attempt to complete CDB commands for present modules
    if (cciGetXcvrMask(device, &moduleMask, NULL) == NVL_SUCCESS)
    {
        FOR_EACH_INDEX_IN_MASK(32, moduleId, moduleMask)
        {   
            //
            // Prioritize sending CDB commands. 
            // This also prioritizes getting responses for CDBs in the event that 
            // fewer than all lanes of a module were operated on. 
            // 
            if (device->pCci->cdbState[moduleId].laneMasksPending[0] ||
                device->pCci->cdbState[moduleId].laneMasksPending[1] ||
                device->pCci->cdbState[moduleId].laneMasksPending[2] ||
                device->pCci->cdbState[moduleId].laneMasksPending[3])
            {
                moduleMaskPriority |= NVBIT32(moduleId);
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;

        FOR_EACH_INDEX_IN_MASK(32, moduleId, moduleMaskPriority)
        {   
            _cci_cdb_perform_phases(device, moduleId);
        }
        FOR_EACH_INDEX_IN_MASK_END;

        FOR_EACH_INDEX_IN_MASK(32, moduleId, moduleMask)
        {   
            _cci_cdb_perform_phases(device, moduleId);

            // Short circuit getting non time sensistive responses
            if (nvswitch_timeout_check(&timeout))
            {
                break;
            }
        }
        FOR_EACH_INDEX_IN_MASK_END;
    }
}

NvlStatus
cciConfigureNvlinkModeAsync
(
    nvswitch_device *device,
    NvU32            client,
    NvU8             linkId,
    NvBool           freeze_maintenance,
    NvBool           restart_training,
    NvBool           nvlink_mode
)
{
    NvlStatus retval;
    CCI_CDB_STATE *pCdbState;
    NvU32 moduleId;
    NvU8 laneMask;
    NvU8 laneMasksIndex;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    retval = cciGetModuleId(device, linkId, &moduleId);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    if (!cciModulePresent(device, moduleId))
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

    pCdbState = &device->pCci->cdbState[moduleId];

    // Add to first available slot
    for (laneMasksIndex = 0; laneMasksIndex < NVSWITCH_CCI_NUM_LINKS_PER_OSFP_LS10; laneMasksIndex++)
    {
        if (pCdbState->laneMasksPending[laneMasksIndex] == 0)
        {
            pCdbState->laneMasksPending[laneMasksIndex] = laneMask;
            break;
        }
    }

    if (pCdbState->cdbPhase != CCI_CDB_PHASE_IDLE)
    {
        // Don't support queuing multiple CDB command types
        NVSWITCH_ASSERT(pCdbState->client == client);
        NVSWITCH_ASSERT(pCdbState->freeze_maintenance == freeze_maintenance);
        NVSWITCH_ASSERT(pCdbState->restart_training == restart_training);   
        NVSWITCH_ASSERT(pCdbState->nvlink_mode == nvlink_mode);

        return NVL_SUCCESS;
    }

    // Setup cdbstate for callback
    pCdbState->client = client;
    pCdbState->freeze_maintenance = freeze_maintenance;
    pCdbState->restart_training = restart_training;
    pCdbState->nvlink_mode = nvlink_mode;
    pCdbState->cdbPhase = CCI_CDB_PHASE_CHECK_READY;
    
    return NVL_SUCCESS;
}
