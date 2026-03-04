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

//
// Custom byte definition obtained from the following doc:
// "LINKX BOOT FAIL AND RECOVERY INDICATIONS"
// 
#define MODULE_FLAGS_CUSTOM_BYTE_BOOT_FAILURE  0:0
#define MODULE_FLAGS_CUSTOM_BYTE_RECOVERY      1:1

static const char*
_cci_onboard_phase_to_text
(
    CCI_MODULE_ONBOARD_PHASE phase,
    CCI_MODULE_ONBOARD_SUBPHASE_OPTICAL subphase
)
{
    switch (phase)
    {
        case CCI_ONBOARD_PHASE_CHECK_CONDITION:
        {
            return "CCI Onboard Phase Check Condition";
        }
        case CCI_ONBOARD_PHASE_IDENTIFY:
        {
            return "CCI Onboard Phase Identify";
        }
        case CCI_ONBOARD_PHASE_INIT_COPPER:
        {
            return "CCI Onboard Phase Init Copper";
        }
        case CCI_ONBOARD_PHASE_INIT_DIRECT:
        {
            return "CCI Onboard Phase Init Direct";
        }
        case CCI_ONBOARD_PHASE_INIT_OPTICAL:
        {
            switch (subphase)
            {
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_START:
                {
                    return "CCI Onboard Subphase Init Optical Start";
                }
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_CMIS_SELECT_APPLICATION:
                {
                    return "CCI Onboard Subphase Init Optical CMIS Select Application";
                }
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_CONFIGURE_LINKS:
                {
                    return "CCI Onboard Subphase Init Optical Configure Links";
                }
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_DISABLE_ALI:
                {
                    return "CCI Onboard Subphase Init Optical Disable ALI";
                }
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_SETUP:
                {
                    return "CCI Onboard Subphase Init Optical Pretrain Setup";
                }
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_SEND_CDB:
                {
                    return "CCI Onboard Subphase Init Optical Pretrain Send CDB";
                }
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_PRETRAIN_POLL:
                {
                    return "CCI Onboard Subphase Init Optical Pretrain Poll";
                }
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_GO_TRANSPARENT:
                {
                    return "CCI Onboard Subphase Init Optical Go Transparent";
                }
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_RESET_LINKS:
                {
                    return "CCI Onboard Subphase Init Optical Reset Links";
                }
                case CCI_ONBOARD_SUBPHASE_INIT_OPTICAL_ENABLE_ALI:
                {
                    return "CCI Onboard Subphase Init Optical Enable ALI";
                }
                default:
                {
                    return "Unknown";
                }
            }
        }
        case CCI_ONBOARD_PHASE_LAUNCH_ALI:
        {
            return "CCI Onboard Phase Launch ALI";
        }
        case CCI_ONBOARD_PHASE_SLEEP:
        {
            return "CCI Onboard Phase Sleep";
        }
        case CCI_ONBOARD_PHASE_MONITOR:
        {
            return "CCI Onboard Phase Monitor";
        }
        default:
        {
            return "Unknown";
        }
    }
}

static void
_cci_check_module_boot_failure
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    NvlStatus retval;
    NvU8 fwInfo[CMIS_CDB_LPL_MAX_SIZE];
    NvU8 fwStatusFlags;
    NvU8 moduleFlagsCustomByte;

    retval = cciCmisRead(device, moduleId, 0, 0, 
                         CMIS_MODULE_FLAGS_CUSTOM_BYTE, 
                         1, &moduleFlagsCustomByte);
    if (retval != NVL_SUCCESS)
    {
        NVSWITCH_PRINT(device, ERROR,
                       "%s: Failed to get module custom flags\n",
                       __FUNCTION__);
        return;
    }

    if (REF_NUM(MODULE_FLAGS_CUSTOM_BYTE_BOOT_FAILURE, moduleFlagsCustomByte) ||
        REF_NUM(MODULE_FLAGS_CUSTOM_BYTE_RECOVERY, moduleFlagsCustomByte))
    {
        retval = cciGetXcvrFWInfo(device, moduleId, fwInfo);
        if (retval != NVL_SUCCESS)
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_MODULE_BOOT,
                                "Module %d boot failure\n", moduleId);
        }
        else
        {
            fwStatusFlags = fwInfo[CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS];

            if (REF_VAL(CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS_IMAGE_A_RUNNING, 
                        fwStatusFlags))
            {
                // NULL terminate string
                fwInfo[CMIS_CDB_GET_FW_INFO_LPL_IMAGE_A_EXTRA_STRING + 
                    CMIS_CDB_GET_FW_INFO_LPL_IMAGE_FACTORY_BOOT_EXTRA_STRING_SIZE - 1] = 0;

                if (REF_NUM(MODULE_FLAGS_CUSTOM_BYTE_BOOT_FAILURE, moduleFlagsCustomByte))
                {
                    NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_MODULE_BOOT,
                                        "Module %d Image A boot failure\n", 
                                        moduleId);
                }
                else
                {
                    NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_MODULE_RECOVERY,
                                        "Module %d Image A recovery failure\n", 
                                        moduleId);
                }

                NVSWITCH_PRINT(device, ERROR, "%s: Module %d Image A boot/recovery failure. ExtraString: %s\n",
                                __FUNCTION__, moduleId, 
                                &fwInfo[CMIS_CDB_GET_FW_INFO_LPL_IMAGE_A_EXTRA_STRING]);
            }

            if (REF_VAL(CMIS_CDB_GET_FW_INFO_LPL_FW_STATUS_FLAGS_IMAGE_B_RUNNING, 
                        fwStatusFlags))
            {
                // NULL terminate string
                fwInfo[CMIS_CDB_GET_FW_INFO_LPL_IMAGE_B_EXTRA_STRING + 
                    CMIS_CDB_GET_FW_INFO_LPL_IMAGE_FACTORY_BOOT_EXTRA_STRING_SIZE - 1] = 0;

                if (REF_NUM(MODULE_FLAGS_CUSTOM_BYTE_BOOT_FAILURE, moduleFlagsCustomByte))
                {
                    NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_MODULE_BOOT,
                                    "Module %d Image B boot failure\n", 
                                    moduleId);
                }
                else
                {
                    NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_MODULE_RECOVERY,
                                    "Module %d Image B recovery failure\n", 
                                    moduleId);
                }

                NVSWITCH_PRINT(device, ERROR, "%s: Module %d Image B boot/recovery failure. ExtraString: %s\n",
                                __FUNCTION__, moduleId, 
                                &fwInfo[CMIS_CDB_GET_FW_INFO_LPL_IMAGE_B_EXTRA_STRING]);
            }
        }   
    }
}

void
cciModuleOnboardCheckErrors
(
    nvswitch_device *device,
    NvU8 moduleId
)
{
    CCI_MODULE_STATE *pOnboardState;

    pOnboardState = &device->pCci->moduleState[moduleId];

    if (pOnboardState->onboardError.bOnboardFailure)
    {   
        if (device->regkeys.cci_error_log_enable)
        {
            NVSWITCH_PRINT_SXID(device, NVSWITCH_ERR_HW_CCI_MODULE, "Module %d failed %s\n",
                                moduleId,
                                _cci_onboard_phase_to_text(
                                pOnboardState->onboardError.failedOnboardState.onboardPhase,
                                pOnboardState->onboardError.failedOnboardState.onboardSubPhase.optical));
        }
    }

    _cci_check_module_boot_failure(device, moduleId);
}

NvBool
cciReportLinkErrors
(
    nvswitch_device *device,
    NvU32 linkNumber
)
{
    NvlStatus retval;
    NvU64 mode;

    if ((device->pCci == NULL) || (!device->pCci->bInitialized))
    {
        NVSWITCH_PRINT(device, ERROR,
            "%s: CCI not supported\n",
            __FUNCTION__);
        return -NVL_ERR_NOT_SUPPORTED;
    }

    if (!cciIsLinkManaged(device, linkNumber))
    {
        return NV_TRUE;
    }

    retval = cciGetLinkMode(device, linkNumber, &mode);
    if (retval != NVL_SUCCESS)
    {
        return NV_TRUE;
    }

    if (mode == NVLINK_LINKSTATE_TRAINING_CCI)
    {
        return NV_FALSE;
    }
    else
    {
        return NV_TRUE;
    }
}
