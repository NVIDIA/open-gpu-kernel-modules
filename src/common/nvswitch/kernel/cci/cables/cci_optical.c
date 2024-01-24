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
#include "cci/cci_cables_nvswitch.h"

#include "nvswitch/ls10/dev_nvlipt_lnk_ip.h"
#include "nvswitch/ls10/dev_nvldl_ip.h"
#include "ls10/ls10.h"

static NvlStatus
_cci_cmis_deactivate_lanes
(
    nvswitch_device *device,
    NvU8 moduleId,
    NvU8 deactivateLaneMask
)
{
    NvlStatus retval;
    NVSWITCH_TIMEOUT timeout;
    NvU64 timeoutMs;
    NvU8 dataPathState[4];
    NvU8 byte;
    NvU8 laneMaskTemp;
    NvU8 laneNum;
    NvU8 laneState;
    
    laneMaskTemp = 0;

    // Deactivate lanes 
    byte = deactivateLaneMask;
    retval = cciCmisWrite(device, moduleId, 0, 0x10,
                          CMIS_DATA_PATH_CONTROL_BYTE, 1, &byte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    // Read max timeout value
    retval = cciCmisRead(device, moduleId, 0, 0x1, 
                         CMIS_DATA_PATH_MAX_DURATION_BYTE, 1, &byte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    } 

    timeoutMs = CMIS_MAX_DURATION_EN_TO_MS_MAP(
                REF_VAL(CMIS_DATA_PATH_MAX_DURATION_BYTE_DEINIT, byte));
    NVSWITCH_ASSERT(timeoutMs != 0);

    nvswitch_timeout_create(timeoutMs * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);

    do
    {
        // Check if lanes are deactivated
        retval = cciCmisRead(device, moduleId, 0, 0x11,
                             CMIS_DATA_PATH_STATE, CMIS_DATA_PATH_STATE_SIZE,
                             dataPathState);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }

        laneMaskTemp = 0; 
        FOR_EACH_INDEX_IN_MASK(8, laneNum, deactivateLaneMask)
        {
            byte = dataPathState[CMIS_DATA_PATH_STATE_LANE_BYTE_MAP(laneNum)];

            laneState = (laneNum % 2) ? REF_VAL(CMIS_DATA_PATH_STATE_BYTE_LANE_1, byte) :
                                        REF_VAL(CMIS_DATA_PATH_STATE_BYTE_LANE_0, byte);

            if (laneState == CMIS_DATA_PATH_STATE_LANE_DEACTIVATED) 
            {
                laneMaskTemp |= NVBIT(laneNum); 
            } 
        }
        FOR_EACH_INDEX_IN_MASK_END;

        // Operation successful when all lanes are in expected state
        if (laneMaskTemp == deactivateLaneMask)
        {
            break;
        }

        if (nvswitch_timeout_check(&timeout))
        {
            return -NVL_ERR_GENERIC;
        }

        nvswitch_os_sleep(10);
    } while (NV_TRUE);

    return NVL_SUCCESS;
}

static NvlStatus
_cci_cmis_check_config_errors
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;
    NVSWITCH_TIMEOUT timeout;
    NvU64 timeoutMs;
    NvU32 i;
    NvU8 errorCodes[4];
    NvU8 byte;
    NvBool bLanesAccepted;

    bLanesAccepted = NV_FALSE;

    // Read max timeout value
    retval = cciCmisRead(device, moduleId, 0, 0x1, 
                         CMIS_DATA_PATH_MAX_DURATION_BYTE, 1, &byte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    } 

    timeoutMs = CMIS_MAX_DURATION_EN_TO_MS_MAP(
                REF_VAL(CMIS_DATA_PATH_MAX_DURATION_BYTE_DEINIT, byte));
    NVSWITCH_ASSERT(timeoutMs != 0);

    nvswitch_timeout_create(timeoutMs * NVSWITCH_INTERVAL_1MSEC_IN_NS, &timeout);

    do
    {
        // Check for errors in config(applied in deactivated state)
        retval = cciCmisRead(device, moduleId, 0, 0x11,
                             CMIS_DATA_PATH_CONFIG_ERROR_CODES, 4, errorCodes);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }

        if ((*(NvU32*)errorCodes) == CMIS_DATA_PATH_CONFIG_ALL_LANES_ACCEPTED)
        {
            bLanesAccepted = NV_TRUE;
            break;
        }

        if (nvswitch_timeout_check(&timeout))
        {
            break;
        }

        nvswitch_os_sleep(10);
    } while (NV_TRUE);

    if (!bLanesAccepted)
    {
        for (i = 0; i < 4; i++)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Lane %d, error code: 0x%x. Lane %d, error code: 0x%x",
                __FUNCTION__, 2*i, (errorCodes[i] & 0xF), 2*i + 1, (errorCodes[i] >> 4));
        }
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

/*
 * Operation is performed on all lanes in the given module
 */
static NvlStatus
_cci_cmis_select_application
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    NvlStatus retval;
    NvU32 i;
    NvU8 nvl4AppSel[] = {0x20, 0x20, 0x24, 0x24, 0x28, 0x28, 0x2c, 0x2c};
    NvU8 nvl4AppSelTemp[8];
    NvU8 byte;

    pOnboardState = &device->pCci->moduleState[moduleId];

    // Nothing to do if module already setup
    if (pOnboardState->bModuleOnboarded)
    {
        return NVL_SUCCESS;
    }

    // 1. Deactivate all lanes
    retval = _cci_cmis_deactivate_lanes(device, moduleId, 0xFF);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, 
            "%s: Module %d: Lanes deactivate failed.\n",
            __FUNCTION__, moduleId);
        return retval;
    }

    // 2. Select application
    retval = cciCmisWrite(device, moduleId, 0, 0x10,
                          CMIS_STAGE_CONTROL_SET_0_APP_SEL, 8, nvl4AppSel);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, 
            "%s: Module %d: Select application failed.\n",
            __FUNCTION__, moduleId);
        return retval;
    }

    // 3. Apply application
    byte = 0xFF;
    retval = cciCmisWrite(device, moduleId, 0, 0x10,
                          CMIS_STAGE_CONTROL_SET_0, 1, &byte);
    if (retval != NVL_SUCCESS)
    {
         NVSWITCH_PRINT(device, ERROR, 
            "%s: Module %d: Apply application failed.\n",
            __FUNCTION__, moduleId);
        return retval;
    }

    // 4. Check for errors
    retval = _cci_cmis_check_config_errors(device, moduleId);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR, 
            "%s: Module %d: Datapath config failed.\n",
            __FUNCTION__, moduleId);
        return retval;
    }

    // 5. Verify that the application selection went through
    retval = cciCmisRead(device, moduleId, 0, 0x11,
                         CMIS_ACTIVE_CONTROL_SET, 8, nvl4AppSelTemp);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    for (i = 0; i < 8; i++)
    {
        if (nvl4AppSelTemp[i] != nvl4AppSel[i])
        {
            NVSWITCH_PRINT(device, ERROR, 
                "%s: Module %d: Application selection failed.\n",
            __FUNCTION__, moduleId);
            return -NVL_ERR_GENERIC;
        }
    }

    // 6. De-assert lpmode
    retval = cciSetLPMode(device, moduleId, NV_FALSE);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }
    
    return NVL_SUCCESS;
}

static NvlStatus
_cci_disable_ALI
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;

    // Have SOE perform this phase
    retval = cciModulesOnboardSOE(device, moduleId);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cci_enable_ALI
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;

    // Have SOE perform this phase
    retval = cciModulesOnboardSOE(device, moduleId);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    return NVL_SUCCESS;
}


static NvlStatus
_cci_configure_links
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;

    // Have SOE perform this phase
    retval = cciModulesOnboardSOE(device, moduleId);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cci_pretrain_setup
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    NvlStatus retval;
    NvU64 linkTrainMask;
    NvU32 linkId;
    NvU8 laneMask;
    NvU8 byte;

    retval = cciSetLedsInitialize(device, moduleId);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    pOnboardState = &device->pCci->moduleState[moduleId];

    linkTrainMask = pOnboardState->linkTrainMask;

    // Train links to enable IOBIST
    FOR_EACH_INDEX_IN_MASK(64, linkId, linkTrainMask)
    {
        retval = cciGetLaneMask(device, linkId, &laneMask);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }

        retval = _cci_cmis_deactivate_lanes(device, moduleId, laneMask);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }

        retval = nvswitch_cci_initialization_sequence_ls10(device, linkId);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }

        retval = nvswitch_cci_enable_iobist_ls10(device, linkId, NV_TRUE);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Activate lanes
    byte = 0;
    retval = cciCmisWrite(device, moduleId, 0, 0x10,
                        CMIS_DATA_PATH_CONTROL_BYTE, 1, &byte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cci_pretrain_send_cdb
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    NvlStatus retval;
    NvU64 linkTrainMask;
    NvBool freeze_maintenance;
    NvBool restart_training;
    NvBool nvlink_mode;

    pOnboardState = &device->pCci->moduleState[moduleId];

    linkTrainMask = pOnboardState->linkTrainMask;
    freeze_maintenance = NV_TRUE;
    restart_training = NV_TRUE;
    nvlink_mode = NV_TRUE;

    retval = cciConfigureNvlinkModeModule(device, NVSWITCH_I2C_ACQUIRER_CCI_TRAIN,
                moduleId, linkTrainMask,
                freeze_maintenance, restart_training, nvlink_mode);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Pre-train CMD to module failed.\n",
            __FUNCTION__);
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cci_go_transparant
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    NvlStatus retval;
    NvU64 linkTrainMask;
    NvBool freeze_maintenance;
    NvBool restart_training;
    NvBool nvlink_mode;

    pOnboardState = &device->pCci->moduleState[moduleId];

    linkTrainMask = pOnboardState->linkTrainMask;
    freeze_maintenance = NV_TRUE;
    restart_training = NV_FALSE;
    nvlink_mode = NV_TRUE;

    retval = cciConfigureNvlinkModeModule(device, NVSWITCH_I2C_ACQUIRER_CCI_TRAIN,
                    moduleId, linkTrainMask,
                    freeze_maintenance, restart_training, nvlink_mode);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to go transparent on links on module: %d\n",
            __FUNCTION__, moduleId);
        return retval;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cci_reset_links
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    NvlStatus retval;
    nvlink_link *link;
    NvU64 linkTrainMask;
    NvU32 linkId;

    pOnboardState = &device->pCci->moduleState[moduleId];

    linkTrainMask = pOnboardState->linkTrainMask;

    FOR_EACH_INDEX_IN_MASK(64, linkId, linkTrainMask)
    {
        link = nvswitch_get_link(device, linkId);

        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID(device, link->linkNumber, NVLIPT_LNK) ||
            (linkId >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: Failed to get link: %d\n",
                __FUNCTION__, linkId);
            return -NVL_ERR_GENERIC;
        }

        if (nvswitch_is_link_in_reset(device, link))
        {
            continue;
        }

        retval = nvswitch_cci_deinitialization_sequence_ls10(device, linkId);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: nvswitch_cci_deinitialization_sequence_ls10 failed for link: %d\n",
                __FUNCTION__, linkId);
            return -NVL_ERR_GENERIC;
        }

        // Disable IOBIST
        retval = nvswitch_cci_enable_iobist_ls10(device, linkId, NV_FALSE);
        if (retval != NVL_SUCCESS)
        {
            return -NVL_ERR_GENERIC;
        }

        // Shutdown link 
        retval = nvswitch_request_tl_link_state_ls10(link,
            NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_SHUTDOWN,
            NV_TRUE);

        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: TL link shutdown request failed for link: %d\n",
                __FUNCTION__, linkId);
            return -NVL_ERR_GENERIC;
        }
        
        // Reset link
        retval = nvswitch_request_tl_link_state_ls10(link,
            NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_RESET,
            NV_TRUE);

        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: TL link reset request failed for link: %d\n",
                __FUNCTION__, linkId);
            return -NVL_ERR_GENERIC;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;
    
    return NVL_SUCCESS;
}

static void
_cci_init_optical_start_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_ONBOARD_STATE nextStateSuccess;
    CCI_MODULE_ONBOARD_STATE nextStateFail;

    nvswitch_os_memset(&nextStateSuccess, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    nvswitch_os_memset(&nextStateFail, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    
    nextStateSuccess.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
    nextStateSuccess.onboardSubPhase.optical = CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_CMIS_SELECT_APPLICATION;
    nextStateFail.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    cciModuleOnboardPerformPhaseAsync(device, moduleId,
                                      _cci_reset_links,
                                      nextStateSuccess,
                                      nextStateFail);
}

static void
_cci_cmis_select_application_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_ONBOARD_STATE nextStateSuccess;
    CCI_MODULE_ONBOARD_STATE nextStateFail;

    nvswitch_os_memset(&nextStateSuccess, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    nvswitch_os_memset(&nextStateFail, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    nextStateSuccess.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
    nextStateSuccess.onboardSubPhase.optical = CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_CONFIGURE_LINKS;
    nextStateFail.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    cciModuleOnboardPerformPhaseAsync(device, moduleId,
                                      _cci_cmis_select_application,
                                      nextStateSuccess,
                                      nextStateFail);
}

static void
_cci_configure_links_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_ONBOARD_STATE nextStateSuccess;
    CCI_MODULE_ONBOARD_STATE nextStateFail;

    nvswitch_os_memset(&nextStateSuccess, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    nvswitch_os_memset(&nextStateFail, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    nextStateSuccess.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
    nextStateSuccess.onboardSubPhase.optical = CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_DISABLE_ALI;
    nextStateFail.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    cciModuleOnboardPerformPhaseAsync(device, moduleId,
                                      _cci_configure_links,
                                      nextStateSuccess,
                                      nextStateFail);
}

static void
_cci_disable_ALI_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_ONBOARD_STATE nextStateSuccess;
    CCI_MODULE_ONBOARD_STATE nextStateFail;

    nvswitch_os_memset(&nextStateSuccess, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    nvswitch_os_memset(&nextStateFail, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    nextStateSuccess.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
    nextStateSuccess.onboardSubPhase.optical = CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_SETUP;
    nextStateFail.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    cciModuleOnboardPerformPhaseAsync(device, moduleId,
                                      _cci_disable_ALI,
                                      nextStateSuccess,
                                      nextStateFail);
}

static void
_cci_pretrain_setup_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_ONBOARD_STATE nextStateSuccess;
    CCI_MODULE_ONBOARD_STATE nextStateFail;

    nvswitch_os_memset(&nextStateSuccess, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    nvswitch_os_memset(&nextStateFail, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    nextStateSuccess.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
    nextStateSuccess.onboardSubPhase.optical = CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_SEND_CDB;
    nextStateFail.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    cciModuleOnboardPerformPhaseAsync(device, moduleId,
                                      _cci_pretrain_setup,
                                      nextStateSuccess,
                                      nextStateFail);
}

static void
_cci_pretrain_send_cdb_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_ONBOARD_STATE nextStateSuccess;
    CCI_MODULE_ONBOARD_STATE nextStateFail;

    nvswitch_os_memset(&nextStateSuccess, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    nvswitch_os_memset(&nextStateFail, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    nextStateSuccess.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
    nextStateSuccess.onboardSubPhase.optical = CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_POLL;
    nextStateFail.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    cciModuleOnboardPerformPhaseAsync(device, moduleId,
                                      _cci_pretrain_send_cdb,
                                      nextStateSuccess,
                                      nextStateFail);
}

static void
_cci_pretrain_poll_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{   
    CCI_MODULE_STATE *pOnboardState;
    CCI_MODULE_ONBOARD_STATE nextState;
    NvU64 linkTrainMaskDone;
    NvU32 linkId;
    NvBool bTx;
    NvBool bPreTrainDone;

    pOnboardState = &device->pCci->moduleState[moduleId];
    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    linkTrainMaskDone = 0;
    FOR_EACH_INDEX_IN_MASK(64, linkId, pOnboardState->linkTrainMask)
    {
        bTx = NV_TRUE;
        bPreTrainDone = cciCheckForPreTraining(device,
                        NVSWITCH_I2C_ACQUIRER_CCI_TRAIN, linkId, bTx);
        if (!bPreTrainDone)
        {
            continue;
        }

        bTx = NV_FALSE;
        bPreTrainDone = cciCheckForPreTraining(device,
                        NVSWITCH_I2C_ACQUIRER_CCI_TRAIN, linkId, bTx);
        if (!bPreTrainDone)
        {
            continue;
        }

        linkTrainMaskDone |= NVBIT64(linkId);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    //
    // Allow link training to proceed 
    //  to prevent possible deadlock
    //
    if (linkTrainMaskDone)
    {
        pOnboardState->bPartialLinkTrainComplete = NV_TRUE;
        pOnboardState->preTrainCounter++;

        // Train remaining links on next attempts
        if (pOnboardState->preTrainCounter == CCI_ONBOARD_PHASE_POLL_MAX)
        {
            if (pOnboardState->linkTrainMask != linkTrainMaskDone)
            {
                pOnboardState->bLinkTrainDeferred = NV_TRUE;
            }
            pOnboardState->linkTrainMask = linkTrainMaskDone;
            pOnboardState->preTrainCounter = 0;
        }
    }

    nextState.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
    if (linkTrainMaskDone == pOnboardState->linkTrainMask)
    {
        nextState.onboardSubPhase.optical = 
            CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_GO_TRANSPARENT;
    }
    else
    {
        // Stay in poll phase until pre-training complete
        nextState.onboardSubPhase.optical =
            CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_POLL;
    }

    cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
}

static void
_cci_go_transparant_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;
    CCI_MODULE_ONBOARD_STATE nextState;
    CCI_MODULE_STATE *pOnboardState;

    pOnboardState = &device->pCci->moduleState[moduleId];

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    retval = _cci_go_transparant(device, moduleId);
    if (retval == NVL_SUCCESS)
    {
        // Move to next phase after sleep
        nextState.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
        nextState.onboardSubPhase.optical = CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_RESET_LINKS;
        cciModuleOnboardSleepAsync(device, moduleId, 10000, nextState);
    }
    else
    {
        pOnboardState->onboardError.bOnboardFailure = NV_TRUE;
        pOnboardState->onboardError.failedOnboardState = pOnboardState->currOnboardState;
        nextState.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
        cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
    }
}

static void
_cci_reset_links_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;
    CCI_MODULE_ONBOARD_STATE nextState;
    CCI_MODULE_STATE *pOnboardState;

    pOnboardState = &device->pCci->moduleState[moduleId];

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    retval = _cci_reset_links(device, moduleId);
    if (retval == NVL_SUCCESS)
    {
        // Move to next phase after sleep
        nextState.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
        nextState.onboardSubPhase.optical = CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_ENABLE_ALI;
        cciModuleOnboardSleepAsync(device, moduleId, 10000, nextState);
    }
    else
    {
        pOnboardState->onboardError.bOnboardFailure = NV_TRUE;
        pOnboardState->onboardError.failedOnboardState = pOnboardState->currOnboardState;
        nextState.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
        cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
    }
}

static void
_cci_enable_ALI_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_ONBOARD_STATE nextStateSuccess;
    CCI_MODULE_ONBOARD_STATE nextStateFail;

    nvswitch_os_memset(&nextStateSuccess, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    nvswitch_os_memset(&nextStateFail, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    nextStateSuccess.onboardPhase = CCI_ONBOARD_PHASE_LAUNCH_ALI;
    nextStateFail.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    cciModuleOnboardPerformPhaseAsync(device, moduleId,
                                      _cci_enable_ALI,
                                      nextStateSuccess,
                                      nextStateFail);
}

/*
 * @brief Initialize optical cables asynchronously
 *
 * @param[in]  device            nvswitch_device  pointer
 * @param[in]  moduleId          Module will be initialized. 
 *                             
 */
void 
cciCablesInitializeOpticalAsync
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;

    pOnboardState = &device->pCci->moduleState[moduleId];

    NVSWITCH_ASSERT(pOnboardState->currOnboardState.onboardPhase == CCI_ONBOARD_PHASE_INIT_OPTICAL);
    
    switch (pOnboardState->currOnboardState.onboardSubPhase.optical)
    {
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_START: 
        {
            _cci_init_optical_start_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_CMIS_SELECT_APPLICATION: 
        {
            _cci_cmis_select_application_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_CONFIGURE_LINKS: 
        {
            _cci_configure_links_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_DISABLE_ALI: 
        {
            _cci_disable_ALI_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_SETUP: 
        {
            _cci_pretrain_setup_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_SEND_CDB: 
        {
            _cci_pretrain_send_cdb_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_POLL: 
        {
            _cci_pretrain_poll_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_GO_TRANSPARENT: 
        {
            _cci_go_transparant_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_RESET_LINKS: 
        {
            _cci_reset_links_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_ENABLE_ALI: 
        {
            _cci_enable_ALI_async(device, moduleId);
            break;
        }
        default:
        {
            break;
        }
    }
}
