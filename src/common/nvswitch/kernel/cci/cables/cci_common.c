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

#include "nvlink_export.h"

#include "nvswitch/ls10/dev_nvlipt_lnk_ip.h"
#include "ls10/ls10.h"

#include "soe/soe_nvswitch.h"
#include "flcn/flcnable_nvswitch.h"
#include "flcn/flcn_nvswitch.h"
#include "rmflcncmdif_nvswitch.h"
#include "soe/soeifcmn.h"

static void
_cci_get_active_fault_link_masks
(
    nvswitch_device *device,
    NvU64 linkMask,
    NvU64 *pLinkMaskActive,
    NvU64 *pLinkMaskActivePending,
    NvU64 *pLinkMaskFault
)
{
    nvlink_link *link;
    NvU64 linkStateDl;
    NvU64 linkStateTl;
    NvU8 linkNum;
    NvU64 linkMaskActive;
    NvU64 linkMaskActivePending;
    NvU64 linkMaskFault;

    linkMaskActive = 0;
    linkMaskActivePending = 0;
    linkMaskFault = 0;

    FOR_EACH_INDEX_IN_MASK(64, linkNum, linkMask)
    {
        link = nvswitch_get_link(device, linkNum);

        if ((link == NULL) ||
            (device->hal.nvswitch_corelib_get_dl_link_mode(link, &linkStateDl) != NVL_SUCCESS) ||
            (device->hal.nvswitch_corelib_get_tl_link_mode(link, &linkStateTl) != NVL_SUCCESS))
        {
            continue;
        }

        switch (linkStateDl)
        {
            case NVLINK_LINKSTATE_HS:
            {
                linkMaskActive |= NVBIT64(linkNum);
                break;
            }
            case NVLINK_LINKSTATE_FAULT:
            {
                linkMaskFault |= NVBIT64(linkNum);
                break;
            }
            default:
            {
                break;
            }
        }

        switch (linkStateTl)
        {
            case NVLINK_LINKSTATE_ACTIVE_PENDING:
            {
                linkMaskActivePending |= NVBIT64(linkNum);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    if (pLinkMaskActive != NULL)
    {
        *pLinkMaskActive = linkMaskActive;
    }

    if (pLinkMaskActivePending != NULL)
    {
        *pLinkMaskActivePending = linkMaskActivePending;
    }

    if (pLinkMaskFault != NULL)
    {
        *pLinkMaskFault = linkMaskFault;
    }
}

static NvU8
_cci_cmis_checksum
( 
    NvU8 *data,
    NvU32 size
)
{
    NvU32 i;
    NvU8 checksum = 0;

    for (i = 0; i < size; ++i)
    {
        checksum += data[i];
    }
    return checksum;
}

static NvlStatus
_cci_module_detect
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    PCCI pCci = device->pCci;
    NvlStatus retval;
    NvU8 charateristicsByte;

    retval = cciCmisRead(device, moduleId, 0, 0, 
                         CMIS_CHARACTERISTICS_BYTE, 1, &charateristicsByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    } 

    //
    // Must be set before further CMIS accesses
    // This is done to avoid attempts at setting the page and bank for
    //  modules with flat memory
    //
    if (REF_VAL(CMIS_CHARACTERISTICS_BYTE_FLAT_MEM, charateristicsByte) == 0x1)
    {
        pCci->isFlatMemory[moduleId] = NV_TRUE;
    }
    else
    {
        pCci->isFlatMemory[moduleId] = NV_FALSE;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cci_module_cable_detect
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    PCCI pCci = device->pCci;
    NvlStatus retval;
    NvU8 mitByte;
    NvU8 siControls[2];

    pCci->cableType[moduleId] = CABLE_TYPE_INVALID;

    retval = cciCmisRead(device, moduleId, 0, 0, 
                         CMIS_MIT_BYTE, 1, &mitByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    NVSWITCH_PRINT(device, INFO,
         "%s: Module %d CMIS MIT byte 0x%x\n", __FUNCTION__, moduleId, mitByte);

    if (mitByte >= CMIS_MIT_BYTE_RESERVED)
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (mitByte >= CMIS_MIT_BYTE_COPPER)
    {
        switch (mitByte)
        {
            case CMIS_MIT_BYTE_COPPER_UNEQUALIZED:
            {
                // OPT loopback modules incorrectly report non-flat memory
                pCci->isFlatMemory[moduleId] = NV_TRUE;
                pCci->rxDetEnable[moduleId] = NV_TRUE; 
                break;
            }
            case CMIS_MIT_BYTE_COPPER_PASSIVE_EQUALIZED:
            case CMIS_MIT_BYTE_COPPER_NEAR_FAR_END_LIMITING_ACTIVE_EQUALIZED:
            case CMIS_MIT_BYTE_COPPER_FAR_END_LIMITING_ACTIVE_EQUALIZED:
            case CMIS_MIT_BYTE_COPPER_NEAR_END_LIMITING_ACTIVE_EQUALIZED:
            case CMIS_MIT_BYTE_COPPER_LINEAR_ACTIVE_EQUALIZED:
            {
                pCci->rxDetEnable[moduleId] = NV_FALSE;
                break; 
            }
            default:
            {
                return -NVL_ERR_NOT_SUPPORTED;
            }
        }
       
        pCci->cableType[moduleId] = CABLE_TYPE_COPPER;
    }
    else
    {
        NVSWITCH_ASSERT(!pCci->isFlatMemory[moduleId]);

        retval = cciCmisRead(device, moduleId, 0, 1, 
                         CMIS_SI_CONTROLS, 2, siControls);
        if (retval != NVL_SUCCESS)
        {
            return retval;
        }

        // rx and tx CDR must both be either implemented or not implemented
        if (mitByte >= 8 || 
            (REF_VAL(CMIS_SI_CONTROLS_CDR_IMPLEMENTED, siControls[0]) !=
             REF_VAL(CMIS_SI_CONTROLS_CDR_IMPLEMENTED, siControls[1])))
        {
            pCci->cableType[moduleId] = CABLE_TYPE_INVALID;
        }

        if ((REF_VAL(CMIS_SI_CONTROLS_CDR_IMPLEMENTED, siControls[0]) == 0x1) &&
            (REF_VAL(CMIS_SI_CONTROLS_CDR_IMPLEMENTED, siControls[1]) == 0x1))
        {
            pCci->cableType[moduleId] = CABLE_TYPE_OPTICAL;
        }
        else
        {
            pCci->cableType[moduleId] = CABLE_TYPE_DIRECT;
        }

        pCci->rxDetEnable[moduleId] = NV_FALSE; 
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cci_module_validate
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    PCCI pCci = device->pCci;
    NvlStatus retval;
    NvU8 revByte;
    NvU8 checksumByte;
    NvU8 checksumBuf[CMIS_CHECKSUM_LENGTH];
    NvU8 checksum;

    // Passive copper so do nothing
    if (pCci->cableType[moduleId] == CABLE_TYPE_COPPER &&
        pCci->rxDetEnable[moduleId])
    {
        return NVL_SUCCESS;
    }

    retval = cciCmisRead(device, moduleId, 0, 0, 
                         CMIS_REV_BYTE, 1, &revByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    if (!(REF_VAL(CMIS_REV_BYTE_WHOLE, revByte) == 0x4 ||
          REF_VAL(CMIS_REV_BYTE_WHOLE, revByte) == 0x5))
    {
        NVSWITCH_PRINT(device, ERROR,
         "%s: CMIS version %d.%d not supported\n", __FUNCTION__,
          REF_VAL(CMIS_REV_BYTE_WHOLE, revByte), 
          REF_VAL(CMIS_REV_BYTE_DECIMAL, revByte));
        return -NVL_ERR_NOT_SUPPORTED;
    }

    retval = cciCmisRead(device, moduleId, 0, 0, 
                         128, CMIS_CHECKSUM_LENGTH, checksumBuf);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    checksum = _cci_cmis_checksum(checksumBuf, CMIS_CHECKSUM_LENGTH);
 
    retval = cciCmisRead(device, moduleId, 0, 0, 
                         CMIS_CHECKSUM_BYTE, 1, &checksumByte);
    if (retval != NVL_SUCCESS)
    {
        return retval;
    }

    if (checksum != checksumByte)
    {
        NVSWITCH_PRINT(device, ERROR,
         "%s: CMIS checksum error\n", __FUNCTION__);
        return -NVL_ERR_GENERIC;
    }

    return NVL_SUCCESS;
}

static NvlStatus
_cci_module_identify
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;

    // Already identified module
    if (device->pCci->moduleState[moduleId].bModuleIdentified)
    {
        return NVL_SUCCESS;
    }

    if (!cciModuleHWGood(device, moduleId))
    {
        // Mark as faulty
        device->pCci->isFaulty[moduleId] = NV_TRUE;

        NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_MODULE,
                           "Module %d faulty\n", moduleId);

        return -NVL_ERR_GENERIC;
    } 

    NVSWITCH_ASSERT(device->pCci->isFaulty[moduleId] == NV_FALSE);

    // Detect basic module characteristics
    retval = _cci_module_detect(device, moduleId);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Module detection failed. Module %d\n", __FUNCTION__, moduleId);
        return retval;
    } 

    // Determine cable type
    retval = _cci_module_cable_detect(device, moduleId);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Module cable detection failed. Module %d\n", __FUNCTION__, moduleId);
        return retval;
    }

    // Validate module
    retval = _cci_module_validate(device, moduleId);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Module validate failed. Module %d\n", __FUNCTION__, moduleId);
        return retval;
    } 

    device->pCci->moduleState[moduleId].bModuleIdentified = NV_TRUE;

    return NVL_SUCCESS;
}

static void 
_cci_reset_module_state_hw
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    // HW reset via CPLD
    cciResetModule(device, moduleId);

    NVSWITCH_PRINT(device, INFO,
            "%s: Module %d HW reset\n", __FUNCTION__, moduleId);
}

static void 
_cci_reset_module_state_sw
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;

    pOnboardState = &device->pCci->moduleState[moduleId];

    // Clear SW state
    nvswitch_os_memset(pOnboardState, 0, sizeof(CCI_MODULE_STATE));

    if (device->pCci->bModeContinuousALI[moduleId])
    {
        pOnboardState->currOnboardState.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    }
    else
    {
        pOnboardState->currOnboardState.onboardPhase = CCI_ONBOARD_PHASE_NON_CONTINUOUS_ALI;
    }
    device->pCci->isFaulty[moduleId] = NV_FALSE;

    NVSWITCH_PRINT(device, INFO,
            "%s: Module %d SW reset\n", __FUNCTION__, moduleId);
}

/**
 * @brief Return the mask of enabled CCI managed links mapped to the given module  
 *
 * @param[in]  device      nvswitch_device  pointer
 * @param[in]  moduleId
 * @param[out] pLinkMask
 */
static void 
_cci_get_enabled_link_mask
(
    nvswitch_device *device,
    NvU8 moduleId,
    NvU64 *pLinkMask
)
{
    NvlStatus status;
    NvU64 linkMask;

    linkMask = 0;

    status = cciGetCageMapping(device, moduleId, &linkMask, NULL);
    if (status != NVL_SUCCESS)
    {
        linkMask = 0;
    }

    // Skip disabled links
    linkMask &= device->pCci->linkMask;

    if (pLinkMask != NULL)
    {
        *pLinkMask = linkMask;
    }
}

/**
 * @brief Setup link training mask and module HW for onboarding
 *
 * @param[in]  device      nvswitch_device  pointer
 * @param[in]  moduleId
 *
 */
static void 
_cci_setup_onboard
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    NvU64 linkMask;
    NvU64 linkMaskActive;
    NvU64 linkMaskActivePending;
    NvU64 linkMaskFault;
    NvU64 linkMaskReset;
    NvU64 linkMaskResetForced;

    pOnboardState = &device->pCci->moduleState[moduleId];
    linkMask = 0;
    linkMaskActive = 0;

    _cci_get_enabled_link_mask(device, moduleId, &linkMask);

    _cci_get_active_fault_link_masks(device, linkMask, &linkMaskActive, &linkMaskActivePending, &linkMaskFault);

    NVSWITCH_PRINT(device, INFO,
                       "%s: Module %d link masks: ACTIVE 0x%llx ACTIVE_PENDING 0x%llx FAULT 0x%llx\n",
                       __FUNCTION__, moduleId, linkMaskActive, linkMaskActivePending, linkMaskFault);

    if (device->pCci->bModeContinuousALI[moduleId])
    {
        // Only train non-active links
        pOnboardState->linkTrainMask = linkMask & ~linkMaskActive;
    }
    else
    {
        // Clear ALI requests for links that are active pending 
        device->pCci->linkMaskAliRequested &= ~linkMaskActivePending;
        pOnboardState->linkTrainMask = linkMask & device->pCci->linkMaskAliRequested;   
    }
    
    // Skip attempts to retrain links that are still being trained in MINION
    pOnboardState->linkTrainMask &= ~linkMaskActivePending;

    if (linkMaskActive || linkMaskActivePending)
    {
        // Module is considered onboarded if any link is active
        pOnboardState->bModuleOnboarded = NV_TRUE;
    }

    // Nothing to do
    if (pOnboardState->linkTrainMask == 0)
    {
        return;
    }

    pOnboardState->bPartialLinkTrainComplete = NV_FALSE;
    pOnboardState->bLinkTrainComplete = NV_FALSE;

    // Reset links that are going to be trained
    linkMaskReset = pOnboardState->linkTrainMask & linkMaskFault;
    linkMaskResetForced = pOnboardState->linkTrainMask & ~linkMaskFault;

    // Faulted links
    if (linkMaskReset)
    {
        device->hal.nvswitch_cci_reset_and_drain_links(device, linkMaskReset, NV_FALSE);
    }

    // Link re-training might occur on links that have not faulted
    if (linkMaskResetForced)
    {
        device->hal.nvswitch_cci_reset_and_drain_links(device, linkMaskResetForced, NV_TRUE);
    }
    
    //
    // Reset module if all enabled links mapped to a module are going to be trained
    //  or if module has not been successfully onboarded
    //
    if (pOnboardState->linkTrainMask == linkMask || !pOnboardState->bModuleOnboarded)
    {
        _cci_reset_module_state_hw(device, moduleId);
        pOnboardState->bModuleOnboarded = NV_FALSE;
    }
}

static void 
_cci_module_check_onboard_condition_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    CCI_MODULE_ONBOARD_STATE nextState;
    NvBool bDoOnboard;

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    pOnboardState = &device->pCci->moduleState[moduleId];

    // Do nothing if module HW is faulty
    if (device->pCci->isFaulty[moduleId])
    {
        bDoOnboard = NV_FALSE;
    }
    else
    {
        _cci_setup_onboard(device, moduleId);

        // Check if onboarding should actually be performed after setup
        bDoOnboard = cciModulePresent(device, moduleId) &&
                     pOnboardState->linkTrainMask &&
                     (pOnboardState->onboardAttempts < CCI_ONBOARD_MAX_ATTEMPTS);
    }
    
    if (bDoOnboard)
    {
        pOnboardState->onboardAttempts++;
        nextState.onboardPhase = CCI_ONBOARD_PHASE_IDENTIFY;

        NVSWITCH_PRINT(device, INFO,
                       "%s: Module %d on-board attempt %d\n",
                       __FUNCTION__, moduleId, pOnboardState->onboardAttempts);
    }
    else
    {
        nextState.onboardPhase = CCI_ONBOARD_PHASE_MONITOR;
    }

    cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
}

static void
_cci_module_identify_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;
    PCCI pCci = device->pCci;
    CCI_MODULE_ONBOARD_STATE nextState;
    CCI_MODULE_STATE *pOnboardState;

    pOnboardState = &device->pCci->moduleState[moduleId];

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    retval = _cci_module_identify(device, moduleId);
    if (retval == NVL_SUCCESS)
    {
        switch (pCci->cableType[moduleId])
        {
            case CABLE_TYPE_COPPER:
            {
                nextState.onboardPhase = CCI_ONBOARD_PHASE_INIT_COPPER;
                break;
            }
            case CABLE_TYPE_DIRECT:
            {
                nextState.onboardPhase = CCI_ONBOARD_PHASE_INIT_DIRECT;
                break;
            }
            case CABLE_TYPE_OPTICAL:
            {
                nextState.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
                break;
            }
            default:
            {
                // Invalid cable type
                pOnboardState->onboardError.bOnboardFailure = NV_TRUE;
                pOnboardState->onboardError.failedOnboardState = pOnboardState->currOnboardState;
                nextState.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
                break;
            }
        }
    }
    else
    {
        pOnboardState->onboardError.bOnboardFailure = NV_TRUE;
        pOnboardState->onboardError.failedOnboardState = pOnboardState->currOnboardState;
        nextState.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    }

    cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
}

static void
_cci_launch_ALI_async
(
    nvswitch_device *device,
    NvU32 moduleId
)
{
    NvlStatus retval  = NVL_SUCCESS;
    CCI_MODULE_STATE *pOnboardState;
    CCI_MODULE_ONBOARD_STATE nextState;
    nvlink_link *link;
    NvU64 linkTrainMask;
    NvU32 linkId;
    
    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    pOnboardState = &device->pCci->moduleState[moduleId];

    linkTrainMask = pOnboardState->linkTrainMask;

    FOR_EACH_INDEX_IN_MASK(64, linkId, linkTrainMask)
    {
        NVSWITCH_ASSERT(linkId < NVSWITCH_LINK_COUNT(device));

        link = nvswitch_get_link(device, linkId);

        if ((link == NULL) ||
            !NVSWITCH_IS_LINK_ENG_VALID(device, linkId, NVLIPT_LNK) ||
            (linkId >= NVSWITCH_NVLINK_MAX_LINKS))
        {
            continue;
        }

        if (!nvswitch_is_link_in_reset(device, link))
        {
            NVSWITCH_PRINT(device, INFO,
                "%s: Link: %d not in reset\n",
                __FUNCTION__, linkId);
            continue;
        }

        NVSWITCH_PRINT(device, INFO,
                "%s: ALI launching on link: %d\n",
                __FUNCTION__, linkId);

        retval = nvswitch_request_tl_link_state_ls10(link,
                NV_NVLIPT_LNK_CTRL_LINK_STATE_REQUEST_REQUEST_ACTIVE,
                NV_FALSE);

        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT(device, ERROR,
                "%s: TL link state request to active for ALI failed for link: %d\n",
                __FUNCTION__, linkId);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;
    
    nextState.onboardPhase = CCI_ONBOARD_PHASE_MONITOR;

    cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
}

static void
_cci_module_onboard_sleep_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    CCI_MODULE_ONBOARD_STATE nextState;

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    pOnboardState = &device->pCci->moduleState[moduleId];
    
    // Move to saved state if enough time has elapsed
    if (nvswitch_os_get_platform_time() >= pOnboardState->sleepState.wakeUpTimestamp)
    {
        nextState = pOnboardState->sleepState.returnState;
    }
    else
    {
        nextState = pOnboardState->currOnboardState;
    }

    cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
}

static void
_cci_module_onboard_monitor_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvU64 linkMask;
    NvU64 linkMaskActive;
    NvU64 linkMaskFault;
    CCI_MODULE_STATE *pOnboardState;
    CCI_MODULE_ONBOARD_STATE nextState;
    NvBool bRetryOnboard;

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    pOnboardState = &device->pCci->moduleState[moduleId];
    nextState.onboardPhase = CCI_ONBOARD_PHASE_MONITOR;
    linkMask = 0;
    linkMaskActive = 0;
    linkMaskFault = 0;
    bRetryOnboard = NV_FALSE;

    _cci_get_enabled_link_mask(device, moduleId, &linkMask);

    // 1. Get active/fault link masks and update onboarding state
    _cci_get_active_fault_link_masks(device, linkMask, &linkMaskActive, NULL, &linkMaskFault);

    if (linkMaskActive)
    {
        // Module is considered onboarded if any link is active
        pOnboardState->bModuleOnboarded = NV_TRUE;

        if (!device->pCci->bModeContinuousALI[moduleId])
        {
            // Only clear active links that ALI was requested for
            device->pCci->linkMaskAliRequested &= ~(pOnboardState->linkTrainMask & linkMaskActive);
        }

        // Clear trained links from train mask
        pOnboardState->linkTrainMask &= ~linkMaskActive;
    }

    //
    // In non-continuous ALI mode, only handle faulted links 
    //  that are being trained
    //
    if (!device->pCci->bModeContinuousALI[moduleId])
    {
        linkMaskFault &= pOnboardState->linkTrainMask;
    }
    
    // 2. Handle module/link errors and trigger re-onboard if necessary
    if (linkMaskFault || pOnboardState->onboardError.bOnboardFailure)
    {
        //
        // ACTIVE -> FAULT transition detected
        // This could be a hot plug event(remote end)
        // Reset SW state to re-attempt training for non-ACTIVE module links
        //
        // Hot plug path only entered if continuous ALI requested
        //
        if ((pOnboardState->linkMaskActiveSaved & linkMaskFault) &&
            device->bModeContinuousALI)
        {
            // In case faults occur in the middle of onboarding, keep track of onboarding attempts 
            if (pOnboardState->onboardAttempts > 0)
            {
                NVSWITCH_PRINT(device, INFO,
                    "%s: ACTIVE -> FAULT detected on module %d links 0x%llx. On-board attempts: %d\n",
                    __FUNCTION__, moduleId, pOnboardState->linkMaskActiveSaved & linkMaskFault,
                                    pOnboardState->onboardAttempts);
            }

            //
            // HW reset will be performed during onboard setup if the
            //  event is actually a hot plug
            //
            _cci_reset_module_state_sw(device, moduleId);
            bRetryOnboard = NV_TRUE;
        }
        
        if (pOnboardState->onboardAttempts == CCI_ONBOARD_MAX_ATTEMPTS)
        {
            // Check will only be performed once per module until a hot plug event
            if (!pOnboardState->onboardError.bErrorsChecked)
            {
                cciSetLPMode(device, moduleId, NV_FALSE);
                cciModuleOnboardCheckErrors(device, moduleId);
                pOnboardState->onboardError.bErrorsChecked = NV_TRUE;
            }
            
            //
            // Remaining non-active links failed to train so 
            //  remove from ALI requested mask
            //
            if (!device->pCci->bModeContinuousALI[moduleId])
            {
                device->pCci->linkMaskAliRequested &= ~pOnboardState->linkTrainMask;
            }

            // Clear links from train mask after all attempts exhausted
            pOnboardState->linkTrainMask = 0;
        }
        else
        {
            // Attempt to re-onboard links/module
            bRetryOnboard = NV_TRUE;
            nvswitch_os_memset(&pOnboardState->onboardError, 0, sizeof(CCI_MODULE_ONBOARD_ERROR));
            nextState.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
        }
    }

    // 3. Re-attempt onboard if training for some links were deferred
    if ((pOnboardState->linkTrainMask == 0) &&
        pOnboardState->bLinkTrainDeferred)
    {
        NVSWITCH_PRINT(device, INFO,
            "%s: Module %d link training deferred\n",
            __FUNCTION__, moduleId);

        pOnboardState->bLinkTrainDeferred = NV_FALSE;
        bRetryOnboard = NV_TRUE;
        nextState.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    }

    //
    // 4. Mark the link training portion of onboarding as complete if the
    //     link train mask is empty and if there are no retry requests.
    //
    if ((pOnboardState->linkTrainMask == 0) && !bRetryOnboard)
    { 
        if (!pOnboardState->bLinkTrainComplete && linkMaskActive)
        {
            NVSWITCH_PRINT(device, INFO,
            "%s: Module %d links 0x%llx ACTIVE. On-board attempts: %d\n",
            __FUNCTION__, moduleId, linkMaskActive, pOnboardState->onboardAttempts);
        }

        pOnboardState->bLinkTrainComplete = NV_TRUE;
            
        if (device->pCci->bModeContinuousALI[moduleId])
        {
            pOnboardState->linkMaskActiveSaved = linkMaskActive;
        }
        
        //
        // Move to non-continuous ALI mode or stay in it.
        //  The only way to transition back to continuous ALI would be 
        //  from the non-continuous ALI phase
        //
        if (!device->bModeContinuousALI ||
            !device->pCci->bModeContinuousALI[moduleId])
        {
            device->pCci->bModeContinuousALI[moduleId] = NV_FALSE;
            nextState.onboardPhase = CCI_ONBOARD_PHASE_NON_CONTINUOUS_ALI;
        }
    }

    cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
}

static void
_cci_module_non_continuous_ALI_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus status;
    CCI_MODULE_ONBOARD_STATE nextState;
    NvU64 linkMask;

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));
    nextState.onboardPhase = CCI_ONBOARD_PHASE_NON_CONTINUOUS_ALI;
    linkMask = 0;

    // Go back to continuous ALI mode
    if (device->bModeContinuousALI)
    {
        device->pCci->bModeContinuousALI[moduleId] = NV_TRUE;
        nextState.onboardPhase = CCI_ONBOARD_PHASE_MONITOR;
        _cci_reset_module_state_sw(device, moduleId);
        goto _cci_module_non_continuous_ALI_async_exit;
    }

    status = _cci_module_identify(device, moduleId);
    if (status != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: Failed to identify module %d\n",
            __FUNCTION__, moduleId);
    }

    // Attempt to train requested links
    _cci_get_enabled_link_mask(device, moduleId, &linkMask);
    if (device->pCci->linkMaskAliRequested & linkMask)
    {
        _cci_reset_module_state_sw(device, moduleId);
        nextState.onboardPhase = CCI_ONBOARD_PHASE_CHECK_CONDITION;
    }

_cci_module_non_continuous_ALI_async_exit:

    cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
}

static void 
_cci_module_onboard_async
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;
    CCI_MODULE_ONBOARD_STATE nextState;

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    pOnboardState = &device->pCci->moduleState[moduleId];

    switch (pOnboardState->currOnboardState.onboardPhase)
    {
        case CCI_ONBOARD_PHASE_CHECK_CONDITION: 
        {
            _cci_module_check_onboard_condition_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_PHASE_IDENTIFY: 
        {
            _cci_module_identify_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_PHASE_INIT_COPPER: 
        {
            cciCablesInitializeCopperAsync(device, moduleId);
            break;
        }
        case CCI_ONBOARD_PHASE_INIT_DIRECT: 
        {
            cciCablesInitializeDirectAsync(device, moduleId);
            break;
        }
        case CCI_ONBOARD_PHASE_INIT_OPTICAL: 
        {
            cciCablesInitializeOpticalAsync(device, moduleId);
            break;
        }
        case CCI_ONBOARD_PHASE_LAUNCH_ALI: 
        {
            _cci_launch_ALI_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_PHASE_SLEEP: 
        {
            _cci_module_onboard_sleep_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_PHASE_MONITOR: 
        {
            _cci_module_onboard_monitor_async(device, moduleId);
            break;
        }
        case CCI_ONBOARD_PHASE_NON_CONTINUOUS_ALI: 
        {
            _cci_module_non_continuous_ALI_async(device, moduleId);
            break;
        }
        default:
        {
            // Not expected
            NVSWITCH_ASSERT(0);
            nextState.onboardPhase = CCI_ONBOARD_PHASE_MONITOR;
            cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
            break;
        }
    }
}

void
cciModulesOnboardInit
(
    nvswitch_device *device
)
{
    NvlStatus status;
    CCI_MODULE_STATE *pOnboardState;
    NvU32 cageMask;
    NvU32 moduleMaskPresent;
    NvU8 moduleId;
    NvU64 linkMask;
    NvU64 linkMaskActive;

    status = cciGetXcvrMask(device, &cageMask, &moduleMaskPresent);
    if (status != NVL_SUCCESS)
    {
        return;
    }

    FOR_EACH_INDEX_IN_MASK(32, moduleId, cageMask)
    {
        device->pCci->bModeContinuousALI[moduleId] = device->bModeContinuousALI;
    }
    FOR_EACH_INDEX_IN_MASK_END;

    // Check if on-boarding needed on init
    FOR_EACH_INDEX_IN_MASK(32, moduleId, moduleMaskPresent)
    {
        pOnboardState = &device->pCci->moduleState[moduleId];
        linkMask = 0;
        linkMaskActive = 0;

        _cci_reset_module_state_sw(device, moduleId);
        _cci_get_enabled_link_mask(device, moduleId, &linkMask);
        _cci_get_active_fault_link_masks(device, linkMask, &linkMaskActive, NULL, NULL);

        // Check if links on module are already active
        if (linkMaskActive)
        {
            NVSWITCH_PRINT(device, INFO,
            "%s: Module %d links 0x%llx ACTIVE.\n",
            __FUNCTION__, moduleId, linkMaskActive);

            // Module is considered onboarded if any link is active
            pOnboardState->bModuleOnboarded = NV_TRUE;
            pOnboardState->linkMaskActiveSaved = linkMaskActive;

            if (linkMaskActive == linkMask)
            {
                pOnboardState->bLinkTrainComplete = NV_TRUE;
            }

            status = _cci_module_identify(device, moduleId);
            if (status != NVL_SUCCESS)
            {
                NVSWITCH_PRINT(device, ERROR,
                    "%s: Module identification failed. Module %d\n", __FUNCTION__, moduleId);
            }
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;
}

static NvBool 
_cciCmisAccessSafe
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    return !cciCheckLPMode(device, moduleId);
}

void 
cciModulesOnboardCallback
(
    nvswitch_device *device
)
{
    NvlStatus retval;
    NvU32 cagesMask;
    NvU32 moduleMaskPresentChange;
    NvU32 moduleId;

    cciDetectXcvrsPresent(device);
    
    retval = cciGetXcvrMask(device, &cagesMask, NULL);
    if (retval != NVL_SUCCESS)
    {
        return;
    }

    cciGetModulePresenceChange(device, &moduleMaskPresentChange);

    FOR_EACH_INDEX_IN_MASK(32, moduleId, moduleMaskPresentChange)
    {
        //
        // Hot plug/unplug detected(local end), so reset SW state for future on-board
        // HW reset will be performed during onboard setup
        //
        _cci_reset_module_state_sw(device, moduleId);
        cciCmisAccessReleaseLock(device, moduleId);
    }
    FOR_EACH_INDEX_IN_MASK_END;
    
    FOR_EACH_INDEX_IN_MASK(32, moduleId, cagesMask)
    {
        // Try to acquire CMIS access lock for module
        if (!cciCmisAccessTryLock(device, moduleId))
        {
            continue;
        }

        _cci_module_onboard_async(device, moduleId);

        // Release CMIS access lock if safe to do so
        if (_cciCmisAccessSafe(device, moduleId))
        {
            cciCmisAccessReleaseLock(device, moduleId);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;
}

NvBool
cciLinkTrainIdle
(
    nvswitch_device *device,
    NvU8 linkId
)
{
    NvlStatus retval;
    CCI_MODULE_STATE *pOnboardState;
    NvU32 moduleId;
    NvBool bLinkTrainIdle;

    bLinkTrainIdle = NV_FALSE;

    retval = cciGetModuleId(device, linkId, &moduleId);
    if (retval != NVL_SUCCESS)
    {
        return NV_TRUE;
    }

    if (cciModulePresent(device, moduleId))
    {
        pOnboardState = &device->pCci->moduleState[moduleId];
        bLinkTrainIdle = !(pOnboardState->linkTrainMask & NVBIT64(linkId));
        
        return bLinkTrainIdle; 
    }
    else
    {
        return NV_TRUE;
    }
}

NvBool
cciModuleOnboardFailed
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;

    pOnboardState = &device->pCci->moduleState[moduleId];

    if (pOnboardState->onboardAttempts < CCI_ONBOARD_MAX_ATTEMPTS)
    {
        return NV_FALSE;
    }
    else
    {
        return pOnboardState->onboardError.bOnboardFailure;
    }
}

void
cciModuleOnboardShutdown
(
    nvswitch_device *device
)
{
    CCI_MODULE_STATE *pOnboardState;
    NvU32 cagesMask;
    NvU8 moduleId;
    NvU8 ledState;
    NvlStatus retval;

    retval = cciGetXcvrMask(device, &cagesMask, NULL);
    if (retval != NVL_SUCCESS)
    {
        return;
    }

    ledState = 0;
    ledState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_A,
                               CCI_LED_STATE_OFF, ledState);
    ledState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_B,
                               CCI_LED_STATE_OFF, ledState);
    
    // CCI temporarily disables ALI during onboarding so re-enable during driver unload
    FOR_EACH_INDEX_IN_MASK(32, moduleId, cagesMask)
    {
        // Force Enable ALI onboard phase
        pOnboardState = &device->pCci->moduleState[moduleId];
        pOnboardState->currOnboardState.onboardPhase = CCI_ONBOARD_PHASE_INIT_OPTICAL;
        pOnboardState->currOnboardState.onboardSubPhase.optical = CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_ENABLE_ALI;

        // Have SOE re-enable ALI for links assoicated with module
        retval = cciModulesOnboardSOE(device, moduleId);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_SHUTDOWN,
                "Failed to re-enable ALI for module %d links.\n",
                moduleId);
        }

        //
        // Turn off LEDs since driver is being unloaded and 
        //  will no longer be monitoring links/modules
        //
        cciSetNextXcvrLedState(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                           moduleId, ledState);
        cciSetXcvrLedState(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                       moduleId, NV_FALSE);

        // Prevent Onboard state machine from performing other phases
        pOnboardState->currOnboardState.onboardPhase = CCI_ONBOARD_PHASE_MONITOR;
    }
    FOR_EACH_INDEX_IN_MASK_END;
}

static void
_cciSetupCmdModulesOnboardSOE
(
    nvswitch_device *device,
    NvU8 moduleId,
    RM_SOE_CORE_CMD_PERFORM_ONBOARD_PHASE *pCmd
)
{
    PCCI pCci = device->pCci;
    CCI_MODULE_STATE *pOnboardState;
    NvU64 linkMask;

    pOnboardState = &device->pCci->moduleState[moduleId];

    pCmd->cmdType = RM_SOE_CORE_CMD_PERFORM_MODULE_ONBOARD_PHASE;
    pCmd->moduleId = moduleId;
    pCmd->onboardPhase = pOnboardState->currOnboardState.onboardPhase;

    switch (pCmd->onboardPhase)
    {
        case CCI_ONBOARD_PHASE_INIT_OPTICAL:
        {
            pCmd->onboardSubPhase = pOnboardState->currOnboardState.onboardSubPhase.optical;
            break;
        }
        default:
        {
            break;
        }
    }
    
    pCmd->attributes.rxDetEnable = pCci->rxDetEnable[moduleId];

    linkMask = pOnboardState->linkTrainMask;
  
    RM_FLCN_U64_PACK(&pCmd->linkMask, &linkMask);
}

NvlStatus
cciModulesOnboardSOE
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    FLCN                *pFlcn;
    NvU32               cmdSeqDesc;
    NV_STATUS           status;
    NvU8                flcnStatus;
    RM_FLCN_CMD_SOE     cmd;
    RM_FLCN_MSG_SOE     msg;
    RM_SOE_CORE_CMD_PERFORM_ONBOARD_PHASE *pPerformOnboardPhaseCmd;
    NVSWITCH_TIMEOUT    timeout;

    if (!nvswitch_is_soe_supported(device))
    {
        return -NVL_ERR_NOT_SUPPORTED;
    }

    pFlcn = device->pSoe->pFlcn;

    nvswitch_os_memset(&cmd, 0, sizeof(RM_FLCN_CMD_SOE));
    nvswitch_os_memset(&msg, 0, sizeof(RM_FLCN_MSG_SOE));

    cmd.hdr.unitId = RM_SOE_UNIT_CORE;
    cmd.hdr.size   = RM_SOE_CMD_SIZE(CORE, PERFORM_ONBOARD_PHASE);

    msg.hdr.unitId = RM_SOE_UNIT_CORE;
    msg.hdr.size   = RM_SOE_MSG_SIZE(CORE, PERFORM_ONBOARD_PHASE);

    pPerformOnboardPhaseCmd = &cmd.cmd.core.performOnboardPhase;
    _cciSetupCmdModulesOnboardSOE(device, moduleId, pPerformOnboardPhaseCmd);

    cmdSeqDesc = 0;

    nvswitch_timeout_create(NVSWITCH_INTERVAL_1SEC_IN_NS * 5, &timeout);
    status = flcnQueueCmdPostBlocking(device, pFlcn,
                                      (PRM_FLCN_CMD)&cmd,
                                      (PRM_FLCN_MSG)&msg,   // pMsg
                                      NULL, // pPayload
                                      SOE_RM_CMDQ_LOG_ID,
                                      &cmdSeqDesc,
                                      &timeout);
    if (status != NV_OK)
    {
        NVSWITCH_PRINT(device, ERROR, "%s: Failed onboard SOE phase for module 0x%x\n", 
                       __FUNCTION__, moduleId);
        return -NVL_ERR_INVALID_STATE;
    }

    flcnStatus = msg.msg.core.performOnboardPhase.flcnStatus;

    if (flcnStatus != FLCN_OK)
    {
        if (flcnStatus == FLCN_ERR_INVALID_ARGUMENT)
        {
            return -NVL_BAD_ARGS;
        }
        else
        {
            return -NVL_ERR_GENERIC;
        }
    }

    return NVL_SUCCESS;
}

NvlStatus
cciModuleEject
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    return -NVL_ERR_NOT_IMPLEMENTED;
}

NvlStatus 
cciRequestALI
(
    nvswitch_device *device,
    NvU64 linkMaskTrain
)
{
    NvlStatus status;
    CCI_MODULE_STATE *pOnboardState;
    NvU32 linkId;
    NvU32 moduleId;

    if (device->bModeContinuousALI)
    {
        return -NVL_ERR_INVALID_STATE;
    }

    FOR_EACH_INDEX_IN_MASK(64, linkId, linkMaskTrain)
    {   
        status = cciGetModuleId(device, linkId, &moduleId);
        if (status != NVL_SUCCESS)
        {
            continue;
        }

        pOnboardState = &device->pCci->moduleState[moduleId];
        
        // Reset state to prevent deadlock in synchronization stages of link training
        if (!device->pCci->bModeContinuousALI[moduleId] &&
            !pOnboardState->bPartialLinkTrainComplete)
        {
            _cci_reset_module_state_sw(device, moduleId);
        }

        device->pCci->linkMaskAliRequested |= NVBIT64(linkId);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    return NVL_SUCCESS;
}


// Helper functions

NvlStatus
cciSetLedsInitialize
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvU8 ledState;

    ledState = 0;
    ledState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_A,
                               CCI_LED_STATE_INITIALIZE, ledState);
    ledState = FLD_SET_REF_NUM(CCI_LED_STATE_LED_B,
                               CCI_LED_STATE_INITIALIZE, ledState);
    cciSetNextXcvrLedState(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                           moduleId, ledState);
    cciSetXcvrLedState(device, NVSWITCH_I2C_ACQUIRER_CCI_UX,
                       moduleId, NV_FALSE);
                       
    return NVL_SUCCESS;
}

/**
 * @brief Moves a module into the provided next state.
 *        This should be called as the last operation of a particular phase.
 *        The purpose of this function is to provide a trace
 *        point for phase transitions to aid with clarity and logging.   
 *
 * @param[in]  device            nvswitch_device  pointer
 * @param[in]  moduleId          Modules to perfrom phase transition on 
 * @param[in]  nextState         The next state a module will be put into
 *
 */
void
cciModuleOnboardPerformPhaseTransitionAsync
(
    nvswitch_device *device,
    NvU8 moduleId,
    CCI_MODULE_ONBOARD_STATE nextState
)
{
    CCI_MODULE_STATE *pOnboardState;

    pOnboardState = &device->pCci->moduleState[moduleId];

    pOnboardState->prevOnboardState = pOnboardState->currOnboardState;
    pOnboardState->currOnboardState = nextState;
}

/**
 * @brief Inserts a non-blocking delay between the current module state and the
 *        provided return state.
 *        Will setup the sleep state to move the input module into the provided 
 *          return state after the given sleep period.  
 *        This should be called as the last operation of a particular phase.
 *
 * @param[in]  device            nvswitch_device pointer
 * @param[in]  moduleId          Modules to perfrom phase transition on 
 * @param[in]  sleepMs           Time delay in ms. Note that the sleep resolution will
 *                                a multiple of the polling period of the onboard state machine   
 * @param[in]  returnState       The state a module will be put into after the delay
 *
 */
void
cciModuleOnboardSleepAsync
(
    nvswitch_device *device,
    NvU8 moduleId,
    NvU32 sleepMs,
    CCI_MODULE_ONBOARD_STATE returnState
)
{
    CCI_MODULE_STATE *pOnboardState;
    CCI_MODULE_ONBOARD_STATE nextState;

    nvswitch_os_memset(&nextState, 0, sizeof(CCI_MODULE_ONBOARD_STATE));

    pOnboardState = &device->pCci->moduleState[moduleId];

    // Save information to return from the sleep phase
    pOnboardState->sleepState.wakeUpTimestamp = nvswitch_os_get_platform_time() +
                                                (sleepMs * NVSWITCH_INTERVAL_1MSEC_IN_NS);
    pOnboardState->sleepState.returnState = returnState;

    // Move to sleep phase
    nextState.onboardPhase = CCI_ONBOARD_PHASE_SLEEP;
    cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextState);
}

void
cciModuleOnboardPerformPhaseAsync
(
    nvswitch_device *device,
    NvU8 moduleId, 
    NvlStatus (*func)(nvswitch_device *, NvU8),
    CCI_MODULE_ONBOARD_STATE nextStateSuccess,
    CCI_MODULE_ONBOARD_STATE nextStateFail
)
{
    NvlStatus retval;
    CCI_MODULE_STATE *pOnboardState;

    pOnboardState = &device->pCci->moduleState[moduleId];

    retval = func(device, moduleId);
    if (retval == NVL_SUCCESS)
    {
        cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextStateSuccess);
    }
    else
    {
        pOnboardState->onboardError.bOnboardFailure = NV_TRUE;
        pOnboardState->onboardError.failedOnboardState = pOnboardState->currOnboardState;
        cciModuleOnboardPerformPhaseTransitionAsync(device, moduleId, nextStateFail);
    }
}
