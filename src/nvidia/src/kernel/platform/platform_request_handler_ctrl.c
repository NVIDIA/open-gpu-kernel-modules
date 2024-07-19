/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************************************************************\
*                                                                           *
*    This module contains the Platform Request Handler control interface    *
*                                                                           *
\***************************************************************************/

/* ------------------------ Includes --------------------------------------- */
#include "nvRmReg.h"
#include "platform/platform_request_handler.h"
#include "platform/platform_request_handler_utils.h"
#include "platform/acpi_common.h"
#include "diagnostics/tracer.h"
#include "os/os.h"
#include "gpu/timer/objtmr.h"
#include "core/locks.h"
#include "gps.h"
#include "rmapi/client_resource.h"

/* ------------------------ Datatypes -------------------------------------- */
// private typedef for this source file.
typedef NV_STATUS PfmreqhndlrControlHandler(PlatformRequestHandler *pPlatformRequestHandler, NvU16 locale, NvU32 *pData);

// prototypes, to be used inside _PfmreqhndlrControlHandlerTable[]
static PfmreqhndlrControlHandler   _handleCmdSystemPfmreqhndlrGetSupportedFunctions;
static PfmreqhndlrControlHandler   _handleCmdSystemPfmreqhndlrDataInitUsingSbiosAndAck;

typedef struct
{
    NvU16                       command;
    PfmreqhndlrControlHandler  *handler;
} PFM_REQ_HNDLR_CONTROL_ENTRY;

PFM_REQ_HNDLR_CONTROL_ENTRY _PfmreqhndlrControlTable[] =
{
    { NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_GET_SUPPORTED_FUNCTIONS  , _handleCmdSystemPfmreqhndlrGetSupportedFunctions },
    { NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_DATA_INIT_USING_SBIOS_AND_ACK, _handleCmdSystemPfmreqhndlrDataInitUsingSbiosAndAck },

    // always end with a NULL record
    { NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_INVALID,                       NULL }
};

NvU16  PerfSensorBlockMap[][32] = {
    {
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   0,  1
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   2,  3
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   4,  5
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   6,  7
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   8,  9
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  10, 11
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  12, 13
        PFM_REQ_HNDLR_TC_ENABLE,                              PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  14, 15
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  16, 17
        PFM_REQ_HNDLR_PSR_PUB_ENTRY(PFM_REQ_HNDLR_PM1_STATE_AVAIL),     PFM_REQ_HNDLR_RESERVED_COUNTER,         //  18, 19
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  20, 21
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  22, 23
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  24, 25
        PFM_REQ_HNDLR_CURR_VALUE_PUB_ENTRY(PFM_REQ_HNDLR_TGPU_SENSOR),  PFM_REQ_HNDLR_RESERVED_COUNTER,         //  26, 27
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  28, 29
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER                    //  30, 31
    },
    {
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   0,  1
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   2,  3
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   4,  5
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   6,  7
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   8,  9
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  10, 11
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  12, 13
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  14, 15
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  16, 17
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_VPS_PS20_SUPPORT,                   //  18, 19
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  20, 21
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  22, 23
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  24, 25
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  26, 27
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  28, 29
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER                    //  30, 31
    },
    {
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   0,  1
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   2,  3
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   4,  5
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   6,  7
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   8,  9
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  10, 11
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  12, 13
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  14, 15
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  16, 17
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  18, 19
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  20, 21
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  22, 23
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  24, 25
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  26, 27
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  28, 29
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER                    //  30, 31
    },
    {
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   0,  1
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   2,  3
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   4,  5
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   6,  7
        PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_TGPU_SENSOR),       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //   8,  9
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  10, 11
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  12, 13
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  14, 15
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  16, 17
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  18, 19
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  20, 21
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  22, 23
        PFM_REQ_HNDLR_AVAIL_SENSOR_MSK,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  24, 25
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  26, 27
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER,                   //  28, 29
        PFM_REQ_HNDLR_RESERVED_COUNTER,                       PFM_REQ_HNDLR_RESERVED_COUNTER                    //  30, 31
    }
};

/* ------------------------ Macros ----------------------------------------- */
/*!
 * Macro to detect and request a new update if it is necessary.
 * If system has already applied the update or reset to EDPpeak limit
 * the subsequent requests to do the same are ignored unless its different request.
 */
#define PFM_REQ_HNDLR_IS_EDPPEAK_UPDATE_REQUIRED(bUpdateAcpiRequest, bEnabled)              \
    ((bUpdateAcpiRequest && !bEnabled) || (!bUpdateAcpiRequest && bEnabled))

/*!
 * Macro to detect and request to enable user configurable TGP mode if it is necessary.
 * If system has already applied the update or reset to user configurable TGP mode
 * the subsequent requests to do the same are ignored unless its different request.
 */
#define PFM_REQ_HNDLR_IS_USER_CONFIG_TGP_MODE_UPDATE_REQUIRED(bUpdateAcpiRequest, bEnabled) \
    ((bUpdateAcpiRequest && !bEnabled) || (!bUpdateAcpiRequest && bEnabled))

/*!
 * Macro to detect if a PLATFORM REQUEST HANDLER counter value is valid, the counter is valid if
 * the counter is supported and its value updated with a valid bit set.
 */
#define PFM_REQ_HNDLR_IS_COUNTER_VALID(pPlatformRequestHandler, counterId)                               \
    ((&(pPlatformRequestHandler->sensorData))->PFMREQHNDLRSensorCache[counterId].bSupported &&         \
    (!(&(pPlatformRequestHandler->sensorData))->PFMREQHNDLRSensorCache[counterId].bInvalid))

 /* ------------------------ Defines ---------------------------------------- */
#define PFM_REQ_HNDLR_USE_SBIOS_LIMIT                                (0U)
#define PFM_REQ_HNDLR_DISABLE_LIMIT                                (255U)
#define PFM_REQ_HNDLR_AVAILABLE_SENSOR_BLOCKS                        (4U)
#define NV0000_CTRL_PFM_REQ_HNDLR_THERM_LIMIT_SRC_USER_LIMITED        (1)
#define NV0000_CTRL_PFM_REQ_HNDLR_THERM_LIMIT_SRC_SBIOS_LIMITED       (2)
#define NV0000_CTRL_PFM_REQ_HNDLR_THERM_LIMIT_SRC_SBIOS_UNLIMITED     (3)
#define NV0000_CTRL_PFM_REQ_HNDLR_THERM_LIMIT_SRC_SBIOS_NOT_SUPPORTED (4)
#define NV0000_CTRL_PFM_REQ_HNDLR_THERM_LIMIT_SRC_USER_DISABLED       (5)

/* ---------------------- Static Function Prototypes ----------------------- */
static NV_STATUS _pfmreqhndlrSampleSensorLimit               (PlatformRequestHandler *pPlatformRequestHandler, NvU32 id, NvU32 timeStamp);
static NV_STATUS _pfmreqhndlrSampleSensorLimit_ACPI          (PlatformRequestHandler *pPlatformRequestHandler, NvU32 id, NvU32 timeStamp);
static NV_STATUS _pfmreqhndlrUpdateSystemParamLimit          (PlatformRequestHandler *pPlatformRequestHandler, NvU32 id);
static void      _pfmreqhndlrUpdateSystemParamLimitWorkItem  (NvU32 gpuInstance, void *pParams);
static void      _pfmreqhndlrResetCounter                    (PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData, NvU32 id, NvU32 timeStamp);
static void      _pfmreqhndlrUpdateCounter                   (PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData, NvU32 id, NvBool bValid, NvU32 value, NvU32 timeStamp);
static void      _pfmreqhndlrOverrideCounter                 (PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData, NvU32 id, NvU32 value, NvU32 timeStamp);
static void      _pfmreqhndlrCancelCounterOverride           (PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData, NvU32 id, NvU32 timeStamp);
static void      _pfmreqhndlrInitSupportedCounter            (PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData, NvU32 id, NvBool bVolatile, NvU32 timeStamp);
static NvBool    _pfmreqhndlrIsCacheEntryStale               (PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData, NvU32 counterId, NvU32 timeStamp);
static NV_STATUS _pfmreqhndlrCallPshareStatus                (PlatformRequestHandler *pPlatformRequestHandler, NvBool bInit);
static NvU32     _pfmreqhndlrGetTimeStamp                    (PlatformRequestHandler *pPlatformRequestHandler);
static NV_STATUS _pfmreqhndlrEnablePM1                       (OBJGPU *pGpu, NvBool bEnable);
static NV_STATUS _pfmreqhndlrSetSupportedStatesFromAcpiParams(OBJGPU *pGpu, PlatformRequestHandler *pPlatformRequestHandler, NvU32 supportedSubfuncs);
static NV_STATUS _pfmreqhndlrHandleUserConfigurableTgpModePlatformCustomization(PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu);
static void      _pfmreqhndlrUpdatePlatformPowerModeWorkItem (NvU32  gpuInstance, void  *pParams);
static void      _pfmreqhndlrPlatformPowerModeStateReset     (PlatformRequestHandler *pPlatformRequestHandler);
static NV_STATUS _pfmreqhndlrUpdatePpmdLimit                 (PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvBool bInit);
static NV_STATUS _pfmreqhndlrUpdateTgpuLimit                 (PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu);
static void      _pfmreqhndlrHandlePlatformSetEdppLimitInfoWorkItem(NvU32 gpuInstance, void *pParams);

/* ------------------------ Public Functions  ------------------------------ */
NV_STATUS
pfmreqhndlrControl_IMPL
(
    PlatformRequestHandler   *pPlatformRequestHandler,
    NvU16     command,
    NvU16     locale,
    NvU32    *pData
)
{
    PFM_REQ_HNDLR_CONTROL_ENTRY *pPfmreqhndlrCtrlTableEntry = NULL;

    if (!pPlatformRequestHandler)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    for (pPfmreqhndlrCtrlTableEntry = _PfmreqhndlrControlTable;
         pPfmreqhndlrCtrlTableEntry->handler;
         pPfmreqhndlrCtrlTableEntry++)
    {
        if (command == pPfmreqhndlrCtrlTableEntry->command)
        {
            return pPfmreqhndlrCtrlTableEntry->handler(pPlatformRequestHandler, locale, pData);
        }
    }

    return NV_ERR_NOT_SUPPORTED;
}

/*!
 * Set/Remove the operating limit of a specific system-parameter (denoted by
 * id) using a deferred workitem.
 *
 * @param[in]  pPlatformRequestHandler   *PlatformRequestHandler pointer
 * @param[in]  id     Identifier of parameter (PFM_REQ_HNDLR_*_SENSOR) to update
 * @param[in]  limit
 *     New operating-limit to set for the parameter. All limits are accepted as
 *     unsigned integers, but internally they should be formatted based on the
 *     type of id/sensor they correspond to. Limits for temperature-sensors for
 *     instance should be stored in NvTemp format.
 * @param[in]  bSet   Indicates we are setting or removing the limit
 *
 * @return NV_OK                   Upon successful update
 * @return NV_ERR_INVALID_ARGUMENT Invalid parameter type/id
 * @return NV_ERR_INVALID_REQUEST
 *     Returned if the provided PlatformRequestHandler pointer was NULL or if the corresponding
 *     OBJGPU object could not be found.
 */
NV_STATUS
pfmreqhndlrOperatingLimitUpdate_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler,
    NvU32   id,
    NvU32   limit,
    NvBool  bSet
)
{
    NV_STATUS status = NV_OK;
    OBJGPU   *pGpu   = pfmreqhndlrGetGpu(pPlatformRequestHandler);

    if (id >= PFM_REQ_HNDLR_PSHAREPARAMS_COUNT)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }
    NV_ASSERT_OR_RETURN(NULL != pPlatformRequestHandler, NV_ERR_INVALID_REQUEST);

    if (bSet)
    {
        //
        // Temperature limits arrive in NvTemp format but are stored in NvU32
        // format. Perform the converstion. The limits will be transformed back to
        // NvTemp values before they are used. Once the analogous ACPI code is
        // changed to perform an immediate conversion from NvS32 to NvTemp, the
        // limits can be stored in NvTemp format directly and double-conversion can
        // be removed.
        //
        switch (id)
        {
            case PFM_REQ_HNDLR_TGPU_SENSOR:
                limit = NV_TYPES_NV_TEMP_TO_CELSIUS_ROUNDED((NvTemp)limit);
                break;
        }

        status = pfmreqhndlrUpdatePerfCounter(pPlatformRequestHandler, PFM_REQ_HNDLR_LIMIT(id), limit);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Update perf counter failed.\n");
            DBG_BREAKPOINT();
            return status;
        }
    }
    else
    {
        _pfmreqhndlrResetCounter(&pPlatformRequestHandler->sensorData, PFM_REQ_HNDLR_LIMIT(id),
                         _pfmreqhndlrGetTimeStamp(pPlatformRequestHandler));
    }

    // Record which counter needs to be updated
    pPlatformRequestHandler->sysControlData.queuedCounterMask |= NVBIT(id);

    //
    // Don't submit the workitem again if there is already a pending workitem,
    // all the touched counters will be updated once the workitem is executed.
    //
    if (!pPlatformRequestHandler->sysControlData.bWorkItemPending)
    {
        status = osQueueWorkItem(pGpu,
            _pfmreqhndlrUpdateSystemParamLimitWorkItem, NULL);

        if (status != NV_OK)
        {
            //
            // It is not a real failure if the OS doesn't support workitem,
            // thus setting the status to NV_OK so that we will update the
            // limit directly in this situation. But if the error code is
            // not NV_ERR_NOT_SUPPORTED, then we treat it as real failure
            // and will not do any update. The queued ID will be kept in
            // this situation so that we can still handle it if we get any
            // chance to queue another workitem later.
            //
            if (status == NV_ERR_NOT_SUPPORTED)
            {
                //
                // Reset the queued id if the workitem is not supported (we
                // will update the limit directly)
                //
                    pPlatformRequestHandler->sysControlData.queuedCounterMask &= ~(NVBIT(id));
                    status = NV_OK;
            }
        }
        else
        {
            pPlatformRequestHandler->sysControlData.bWorkItemPending = NV_TRUE;
        }
    }

    if (status == NV_OK)
    {
        if (!pPlatformRequestHandler->sysControlData.bWorkItemPending)
        {
            //
            // Update the limit directly if nothing wrong and no workitem is
            // queued
            //
            return _pfmreqhndlrUpdateSystemParamLimit(pPlatformRequestHandler, id);
        }
        else
        {
            //
            // Let the client knows that more processing is needed since
            // workitem is queued
            //
            return NV_WARN_MORE_PROCESSING_REQUIRED;
        }
    }

    return status;
}

//
// cliresCtrlCmdSystemPfmreqhndlrCtrl
//
// This function process control request for the PLATFORM REQUEST HANDLER object.
//
NV_STATUS
cliresCtrlCmdSystemPfmreqhndlrCtrl_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CTRL_PARAMS *pParams
)
{
    OBJSYS                     *pSys                    = SYS_GET_INSTANCE();
    PlatformRequestHandler     *pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);
    OBJGPU                     *pGpu                    = NULL;
    PPFM_REQ_HNDLR_SENSOR_DATA  pPfmreqhndlrData        = NULL;
    NV_STATUS                   status                  = NV_OK;
    NvU32                       timeStamp;
    NvU32                       newLimit;
    NvU32                       counterVal;
    NvU16                       counterId;
    RM_API                     *pRmApi                  = NULL;

    if (NULL == pPlatformRequestHandler)
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    pGpu             = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    pPfmreqhndlrData = &(pPlatformRequestHandler->sensorData);

    if (NULL == pGpu)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    timeStamp = _pfmreqhndlrGetTimeStamp(pPlatformRequestHandler);

    switch (pParams->cmd)
    {
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_THERM_LIMIT:
        {
            counterId = (NvU16)pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX];

            if (PFM_REQ_HNDLR_VALID_LIMIT_ID(counterId))
            {
                if (NV_OK == pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler,
                                    counterId,
                                    &counterVal))
                {
                    pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_THERMAL_LIMIT] = counterVal;
                    if (pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].bOverridden)
                    {
                        // the Thermal limit is from a user specified limit.
                        pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_LIMIT_SOURCE] =
                            NV0000_CTRL_PFM_REQ_HNDLR_THERM_LIMIT_SRC_USER_LIMITED;
                    }
                    else
                    {
                        // the thermal limit is from the SBIOS.
                        pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_LIMIT_SOURCE] =
                            NV0000_CTRL_PFM_REQ_HNDLR_THERM_LIMIT_SRC_SBIOS_LIMITED;
                    }
                }
                else
                {
                    pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_THERMAL_LIMIT] = 0;
                    if (pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].bSupported)
                    {
                        // the SBIOS is not limiting the thermal sensor.
                        pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_LIMIT_SOURCE] =
                            NV0000_CTRL_PFM_REQ_HNDLR_THERM_LIMIT_SRC_SBIOS_UNLIMITED;
                    }
                    else
                    {
                        // the SBIOS is not supporting this thermal limit.
                        pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_LIMIT_SOURCE] =
                            NV0000_CTRL_PFM_REQ_HNDLR_THERM_LIMIT_SRC_SBIOS_NOT_SUPPORTED;
                    }
                }
                pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_MIN_LIMIT] = 1;
                pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_MAX_LIMIT] = 100;

                status = NV_OK;
            }
            else
            {
                status = NV_ERR_INVALID_ARGUMENT;
            }
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_THERM_LIMIT:
        {
            newLimit  = pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_INPUT_THERMAL_LIMIT];
            counterId = (NvU16)pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX];

            if (PFM_REQ_HNDLR_VALID_LIMIT_ID(counterId))
            {
                if (PFM_REQ_HNDLR_USE_SBIOS_LIMIT == newLimit)
                {
                    // clear any override & invalidate the cached limit.
                    _pfmreqhndlrCancelCounterOverride(pPfmreqhndlrData, counterId, timeStamp);
                }
                else
                {
                    // push the new limit into the sensor cache & mark it as overridden.
                    _pfmreqhndlrOverrideCounter(pPfmreqhndlrData, counterId, pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_INPUT_THERMAL_LIMIT], timeStamp);
                }

                if (PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_TGPU_SENSOR) == counterId)
                {
                    // get & apply the new limit.
                    if (NV_OK == pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler,
                                                counterId,
                                                &counterVal))
                    {
                        NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS  params = { 0 };
                        params.targetTemp = NV_TYPES_CELSIUS_TO_NV_TEMP(newLimit);;

                        status = pRmApi->Control(pRmApi,
                                                 pGpu->hInternalClient,
                                                 pGpu->hInternalSubdevice,
                                                 NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT,
                                                 &params,
                                                 sizeof(params));

                        if (status != NV_OK)
                        {
                            status = NV_ERR_NOT_SUPPORTED;
                            break;
                        }
                    }
                }
                status = NV_OK;
            }
            else
            {
                status = NV_ERR_INVALID_ARGUMENT;
            }
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_STATUS:
        {
            //
            // No longer able to turn on/off the temperature controller, so
            // getting status is no longer needed.
            //
            status = NV_ERR_NOT_SUPPORTED;
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_STATUS:
        {
            // No longer able to turn on/off the temperature controller
            status = NV_ERR_NOT_SUPPORTED;
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_DOWN_N_DELTA:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_HOLD_DELTA:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_UP_DELTA:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_ENGAGE_DELTA:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_CTRL_DISENGAGE_DELTA:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_DOWN_N_DELTA:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_HOLD_DELTA:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_UP_DELTA:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_ENGAGE_DELTA:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_CTRL_DISENGAGE_DELTA:
        {
            status   = NV_ERR_INVALID_ARGUMENT;
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PERF_SENSOR:
        {
            status = pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler,
                            (NvU16)pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_INPUT_SENSOR_INDEX],
                            &counterVal);
            if (NV_OK == status)
            {
                pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_PERF_SENSOR_VALUE]     = counterVal;
                pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_PERF_SENSOR_AVAILABLE] = NV_TRUE;
            }
            else
            {
                pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_PERF_SENSOR_VALUE]     = 0;
                pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_RESULT_PERF_SENSOR_AVAILABLE] = NV_FALSE;
            }
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_CALL_ACPI:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_IGPU_TURBO:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_PERIOD:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_PERIOD:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_NUDGE_FACTOR:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_NUDGE_FACTOR:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_THRESHOLD_SAMPLES:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_THRESHOLD_SAMPLES:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_TEMP_PERF_LIMITS:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_TEMP_PERF_LIMITS:
        {
            status = NV_ERR_NOT_SUPPORTED;
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_PM1_AVAILABLE:
        {
            if (!pPlatformRequestHandler->controlData.bPM1ForcedOff)
            {
                status = _pfmreqhndlrEnablePM1(pGpu,
                             pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_INPUT_PM1_AVAILABLE]);
            }
            else
            {
                status = NV_ERR_NOT_SUPPORTED;
            }
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PM1_AVAILABLE:
        {
            if (!pPlatformRequestHandler->controlData.bPM1ForcedOff)
            {
                status = pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler,
                                            PFM_REQ_HNDLR_PM1_STATE_AVAIL,
                                            &counterVal);
                if (NV_OK == status)
                {
                    pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_PM1_AVAILABLE] = counterVal;
                }
                else
                {
                    pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_PM1_AVAILABLE] = 0;
                }
            }
            else
            {
                pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_OUTPUT_PM1_AVAILABLE] = 0;
                status = NV_ERR_NOT_SUPPORTED;
            }
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_PACKAGE_LIMITS:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_PACKAGE_LIMITS:
        {
            NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX acpiParamsEx = {0};

            // Use EX type struct -- three NvU32s for both input and output for input
            acpiParamsEx.inSize = NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_COUNT * sizeof(NvU32);
            acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_VERSION_IDX] =
                FLD_SET_DRF(0000_CTRL_PFM_REQ_HNDLR, _PPL_ARG0_VERSION, _MAJOR, _V1,
                            acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_VERSION_IDX]);
            acpiParamsEx.outSize = NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_COUNT * sizeof(NvU32);

            if (pParams->cmd == NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_PACKAGE_LIMITS)
            {
                acpiParamsEx.pfmreqhndlrFunc = GPS_FUNC_SETPPL;
                acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_LIMIT1_IDX] =
                    pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_PACKAGE_LIMITS_PL1];
                acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_LIMIT2_IDX] =
                    pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_PACKAGE_LIMITS_PL1] * 2;
            }
            else
            {
                acpiParamsEx.pfmreqhndlrFunc = GPS_FUNC_GETPPL;
            }

            status = pfmreqhndlrCallACPI_EX(pGpu, &acpiParamsEx);
            if (NV_OK == status)
            {
                // Check for major version 1
                if (FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PPL_ARG0_VERSION, _MAJOR, _V1,
                                 acpiParamsEx.output[NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_VERSION_IDX]))
                {
                    pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_PACKAGE_LIMITS_PL1] =
                        acpiParamsEx.output[NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_LIMIT1_IDX];
                    pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_PACKAGE_LIMITS_PL2] =
                        acpiParamsEx.output[NV0000_CTRL_PFM_REQ_HNDLR_PPL_ARGS_LIMIT2_IDX];
                }
                else
                {
                    status = NV_ERR_INVALID_DATA;
                }
            }
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_FREQ_LIMIT:
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_FREQ_LIMIT:
        {
            NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX acpiParamsEx = {0};

            // Use EX type struct -- two NvU32s for both input and output for input
            acpiParamsEx.inSize = NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_COUNT * sizeof(NvU32);
            acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_VERSION_IDX] =
                FLD_SET_DRF(0000_CTRL_PFM_REQ_HNDLR, _TRL_ARG0_VERSION, _MAJOR, _V1,
                            acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_VERSION_IDX]);
            acpiParamsEx.outSize = NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_COUNT * sizeof(NvU32);

            if (pParams->cmd == NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_FREQ_LIMIT)
            {
                acpiParamsEx.pfmreqhndlrFunc = GPS_FUNC_SETTRL;
                acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_FREQ_MHZ_IDX] =
                    pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_CPU_FREQ_LIMIT_MHZ];
            }
            else
            {
                acpiParamsEx.pfmreqhndlrFunc = GPS_FUNC_GETTRL;
                acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_FREQ_MHZ_IDX] = 0;
            }

            status = pfmreqhndlrCallACPI_EX(pGpu, &acpiParamsEx);
            if (NV_OK == status)
            {
                // Check for major version 1
                if (FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _TRL_ARG0_VERSION, _MAJOR, _V1,
                                 acpiParamsEx.output[NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_VERSION_IDX]))
                {
                    pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_CPU_FREQ_LIMIT_MHZ] =
                        acpiParamsEx.output[NV0000_CTRL_PFM_REQ_HNDLR_TRL_ARGS_FREQ_MHZ_IDX];
                }
                else
                {
                    status = NV_ERR_INVALID_DATA;
                }
            }
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PPM:
        {
            //
            // Mask is cached in pfmreqhndlrStateLoad and when we recieve SBIOS notification on battery transitions in AC.
            // Index is cached in pfmreqhndlrStateLoad, SBIOS notification and in the workItem to set the Platform power mode.
            // Return the cached values instead of making ACPI calls.
            //
            pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PPM_INDEX] =
                (NvS32) pPlatformRequestHandler->ppmData.ppmIdxCurrent;
            pParams->result[NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_GET_PPM_AVAILABLE_MASK] =
                (NvS32) pPlatformRequestHandler->ppmData.ppmMaskCurrent;

            status = NV_OK;
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_PPM:
        {

            // Return error if platform power modes are not supported
            if (pPlatformRequestHandler->ppmData.ppmMaskCurrent == NV0000_CTRL_PFM_REQ_HNDLR_PPM_MASK_INVALID)
            {
                status = NV_ERR_INVALID_DATA;
                break;
            }
            NvU8 clientPpmIdx = (NvU8) pParams->input[NV0000_CTRL_PFM_REQ_HNDLR_CMD_TYPE_SET_PPM_INDEX];

            // If requested mode is same as current mode, return success
            if (pPlatformRequestHandler->ppmData.ppmIdxCurrent == clientPpmIdx)
            {
                status = NV_OK;
                break;
            }

            // Return error for invalid mask or if index requested is not present in the mask
            if ((clientPpmIdx == NV0000_CTRL_PFM_REQ_HNDLR_PPM_INDEX_INVALID) ||
                !(pPlatformRequestHandler->ppmData.ppmMaskCurrent & NVBIT(clientPpmIdx)))
            {
                status = NV_ERR_INVALID_DATA;
                break;
            }

            status = NV_ERR_BUSY_RETRY;
            if (!pPlatformRequestHandler->ppmData.bWorkItemPending)
            {
                pPlatformRequestHandler->ppmData.ppmIdxRequested = clientPpmIdx;
                // Create a workItem to change Platform Power Mode
                status = osQueueWorkItem(pGpu, _pfmreqhndlrUpdatePlatformPowerModeWorkItem, NULL);
                if (status == NV_OK)
                {
                    // Queing workitem succeeded, mark it as pending.
                    pPlatformRequestHandler->ppmData.bWorkItemPending = NV_TRUE;
                }
            }
            break;
        }
        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unknown request %x\n", pParams->cmd);
            status = NV_ERR_NOT_SUPPORTED;
            break;
        }
    }
    return status;
}

NV_STATUS
cliresCtrlCmdSystemPfmreqhndlrGetFrmData_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS *pParams
)
{
    OBJSYS    *pSys    = SYS_GET_INSTANCE();
    PlatformRequestHandler       *pPlatformRequestHandler    = SYS_GET_PFM_REQ_HNDLR(pSys);
    NvU8       logicalBufferStart;
    NvU8       block0Size;

    if (NULL==pPlatformRequestHandler)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pParams->nextSampleNumber = pPlatformRequestHandler->frmData.nextSampleNumber;

    // logical buffer starts at the oldest entry
    logicalBufferStart = ((pPlatformRequestHandler->frmData.nextSampleNumber) % NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_FRM_DATA_SAMPLE_SIZE);

    // Block0 goes from the logical start of the buffer to the physical end of the buffer
    block0Size = (NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_FRM_DATA_SAMPLE_SIZE - logicalBufferStart);

    portMemCopy(&(pParams->samples[0]), (sizeof(pPlatformRequestHandler->frmData.samples[0]) * block0Size), &(pPlatformRequestHandler->frmData.samples[logicalBufferStart]), (sizeof(pPlatformRequestHandler->frmData.samples[0]) * block0Size));

    //
    // if the logical buffer is wrapping from the physical end of the buffer back to the beginning,
    // copy the logical end block to the end of the output buffer
    //
    if (0 != logicalBufferStart)
    {
        portMemCopy(&(pParams->samples[block0Size]), (sizeof(pPlatformRequestHandler->frmData.samples[0]) * (logicalBufferStart)), &(pPlatformRequestHandler->frmData.samples[0]), (sizeof(pPlatformRequestHandler->frmData.samples[0]) * (logicalBufferStart)));
    }

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemPfmreqhndlrSetFrmData_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_SET_FRM_DATA_PARAMS *pParams
)
{
    OBJSYS    *pSys         = SYS_GET_INSTANCE();
    PlatformRequestHandler       *pPlatformRequestHandler         = SYS_GET_PFM_REQ_HNDLR(pSys);
    NvU8       sampleNumber = 0;

    if (NULL == pPlatformRequestHandler)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    sampleNumber = ((pPlatformRequestHandler->frmData.nextSampleNumber) % NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_FRM_DATA_SAMPLE_SIZE);
    pPlatformRequestHandler->frmData.samples[sampleNumber]              = pParams->sampleData;
    pPlatformRequestHandler->frmData.samples[sampleNumber].sampleNumber = pPlatformRequestHandler->frmData.nextSampleNumber;
    pPlatformRequestHandler->frmData.nextSampleNumber++;

    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemPfmreqhndlrCallAcpi_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_CALL_ACPI_PARAMS *pParams
)
{
    NV_STATUS                                status;
    OBJSYS                                  *pSys    = SYS_GET_INSTANCE();
    PlatformRequestHandler                                     *pPlatformRequestHandler    = SYS_GET_PFM_REQ_HNDLR(pSys);
    OBJGPU                                  *pGpu;
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS         acpiParams;

    if (NULL == pPlatformRequestHandler)
    {
        return NV_ERR_NOT_SUPPORTED;
    }
    pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    if (NULL == pGpu)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    portMemSet((void *) &acpiParams, 0, sizeof(NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS));
    acpiParams.ctrl.cmd        = pParams->cmd;
    acpiParams.ctrl.input      = pParams->input;
    acpiParams.output.pBuffer  = (NvU8*)&(pParams->result);
    acpiParams.output.bufferSz = sizeof(pParams->result);

    status = pfmreqhndlrCallACPI(pGpu, &acpiParams);

    if (NV_OK != status)
    {
        return status;
    }
    if (0 == acpiParams.output.bufferSz)
    {
        // we used the result field, not the buffer, copy the results to the buffer
        // based on the command type.
        switch(acpiParams.ctrl.cmd)
        {
            case NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SUPPORT:
            {
                pParams->result[0] = acpiParams.output.result[0];
                pParams->result[1] = acpiParams.output.result[1];
                pParams->resultSz  = sizeof(acpiParams.output.result[0]) * 2;
                break;
            }
            case NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PCONTROL:
            case NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHARESTATUS:
            {
                pParams->result[0] = acpiParams.output.result[0];
                pParams->resultSz  = sizeof(acpiParams.output.result[0]);
                break;
            }
            // anything left is either a command that should have used a buffer, or something we don't recognize.
            default:
            {
                pParams->resultSz  = 0;
                break;
            }
        }
    }
    else
    {
        pParams->resultSz =  acpiParams.output.bufferSz;
    }
    return status;
}

NV_STATUS
pfmreqhndlrCallACPI
(
    OBJGPU *pGpu,
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS *pParams
)
{
    OBJSYS   *pSys   = SYS_GET_INSTANCE();
    PlatformRequestHandler      *pPlatformRequestHandler   = SYS_GET_PFM_REQ_HNDLR(pSys);
    NV_STATUS status = NV_OK;
    NvU32     rc;
    NvU16     paramSize;

    switch (pParams->ctrl.cmd)
    {
        case NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SUPPORT:
        {
            // no input parameters
            pParams->output.result[0] = 0;
            pParams->output.result[1] = 0;
            pParams->output.bufferSz  = 0;
            paramSize = sizeof(pParams->output.result);

            if ((rc = osCallACPI_DSM(pGpu,
                            pPlatformRequestHandler->dsmVersion,
                            GPS_FUNC_SUPPORT,
                            pParams->output.result,
                            &paramSize)) != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Unable to retrieve PFM_REQ_HNDLR_SUPPORT. Possibly not supported/enabled by "
                          "CPU/ACPI? rc = %x\n", rc);
                status =  NV_ERR_NOT_SUPPORTED;
                RMTRACE_SBIOS (_ACPI_DSM_ERROR, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_SUPPORT, 0, 0, 0, 0, 0);
                break;
            }
            RMTRACE_SBIOS (_ACPI_DSM_METHOD, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_SUPPORT, pParams->output.result[0], pParams->output.result[1], 0, 0, 0);
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PCONTROL:
        {
            pParams->output.result[0] = pParams->ctrl.input;
            paramSize = sizeof(pParams->output.result[0]);
            pParams->output.bufferSz  = 0;

            if ((rc = osCallACPI_DSM(pGpu,
                            pPlatformRequestHandler->dsmVersion,
                            GPS_FUNC_PCONTROL,
                            (NvU32 *) &pParams->output.result[0],
                            &paramSize)) != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Unable to retrieve PFM_REQ_HNDLR_PCONTROL. Possibly not supported/enabled by "
                          "CPU/ACPI? rc = %x\n", rc);
                status =  NV_ERR_NOT_SUPPORTED;
                RMTRACE_SBIOS (_ACPI_DSM_ERROR, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_PCONTROL, 0, 0, 0, 0, 0);
                break;
            }
            RMTRACE_SBIOS (_ACPI_DSM_METHOD, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_PCONTROL, pParams->ctrl.input, pParams->output.result[0], 0, 0, 0);
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHARESTATUS:
        {
            pParams->output.result[0] = pParams->ctrl.input;
            paramSize = sizeof(pParams->output.result[0]);
            pParams->output.bufferSz  = 0;

            if ((rc = osCallACPI_DSM(pGpu,
                            pPlatformRequestHandler->dsmVersion,
                            GPS_FUNC_PSHARESTATUS,
                            &pParams->output.result[0],
                            &paramSize)) != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Unable to retrieve PFM_REQ_HNDLR_PSHARESTATUS. Possibly not supported/enabled by"
                          " CPU/ACPI? rc = %x\n", rc);
                status =  NV_ERR_NOT_SUPPORTED;
                RMTRACE_SBIOS (_ACPI_DSM_ERROR, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_PSHARESTATUS, 0, 0, 0, 0, 0);
                break;
            }
            RMTRACE_SBIOS (_ACPI_DSM_METHOD, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_PSHARESTATUS, pParams->ctrl.input, pParams->output.result[0], 0, 0, 0);
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHAREPARAMS:
        {
            if (sizeof(PFM_REQ_HNDLR_PSHAREDATA) > pParams->output.bufferSz)
            {
                NV_PRINTF(LEVEL_ERROR, "buffer to small to hold output.\n");
                status =  NV_ERR_NOT_SUPPORTED;
                break;
            }
            *((NvU32*)(pParams->output.pBuffer)) = pParams->ctrl.input;

            if ((rc = osCallACPI_DSM(pGpu,
                            pPlatformRequestHandler->dsmVersion,
                            GPS_FUNC_PSHAREPARAMS,
                            (NvU32 *) pParams->output.pBuffer,
                            &pParams->output.bufferSz)) != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Unable to retrieve PFM_REQ_HNDLR_PSHAREPARAMS. Possibly not supported/enabled by"
                          " CPU/ACPI? rc = %x\n", rc);
                status =  NV_ERR_NOT_SUPPORTED;
                RMTRACE_SBIOS (_ACPI_DSM_ERROR, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_PSHAREPARAMS, 0, 0, 0, 0, 0);
                break;
            }
            RMTRACE_SBIOS (_ACPI_DSM_METHOD, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_PSHAREPARAMS, ((NvU32 *) pParams->output.pBuffer)[0], ((NvU32 *) pParams->output.pBuffer)[3], ((NvU32 *) pParams->output.pBuffer)[6], 0, 0);
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_GETEDPPLIMIT:
        {
            pParams->output.result[0] = pParams->ctrl.input;
            paramSize = sizeof(pParams->output.result[0]);

            if ((rc = osCallACPI_DSM(pGpu,
                        pPlatformRequestHandler->dsmVersion,
                        GPS_FUNC_GETEDPPLIMIT,
                        &pParams->output.result[0],
                        &paramSize)) != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Unable to retrieve PFM_REQ_HNDLR_FUNC_GETEDPPLIMIT. Possibly not supported/enabled by "
                          "ACPI? rc = %x\n", rc);
                status = NV_ERR_NOT_SUPPORTED;
                RMTRACE_SBIOS (_ACPI_DSM_ERROR, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_GETEDPPLIMIT, 0, 0, 0, 0, 0);
                break;
            }
            RMTRACE_SBIOS (_ACPI_DSM_METHOD, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_GETEDPPLIMIT, pParams->output.result[0], pParams->output.result[1], 0, 0, 0);
            break;
        }
        case NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SETEDPPLIMITINFO:
        {
            if ((rc = osCallACPI_DSM(pGpu,
                        pPlatformRequestHandler->dsmVersion,
                        GPS_FUNC_SETEDPPLIMITINFO,
                        (NvU32 *) pParams->output.pBuffer,
                        &pParams->output.bufferSz)) != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Unable to retrieve PFM_REQ_HNDLR_FUNC_SETEDPPLIMITINFO. Possibly not supported/enabled by "
                          "ACPI? rc = %x\n", rc);
                status = NV_ERR_NOT_SUPPORTED;
                RMTRACE_SBIOS (_ACPI_DSM_ERROR, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_SETEDPPLIMITINFO, 0, 0, 0, 0, 0);
                break;
            }
            RMTRACE_SBIOS (_ACPI_DSM_METHOD, pGpu->gpuId, pPlatformRequestHandler->dsmVersion, GPS_FUNC_SETEDPPLIMITINFO, pParams->output.result[0], pParams->output.result[1], 0, 0, 0);
            break;
        }
        default:
        {
            NV_PRINTF(LEVEL_ERROR, "Unknown request %x\n",
                      DRF_VAL(0000_CTRL_PFM_REQ_HNDLR, _CALL_ACPI, _CMD_TYPE, pParams->ctrl.cmd));
            status =  NV_ERR_NOT_SUPPORTED;
            break;
        }
    }
    return status;
}

/*!
 * EX version of pfmreqhndlrCallACPI used only for PPL and TRL calls, which require
 * a greater number of input args.
 *
 * @param[in]      pGpu       GPU object pointer
 * @param[in, out] pParams    Call parameters
 *
 * Returns NV_OK if handled successfully; otherwise propogates errors.
 */
NV_STATUS
pfmreqhndlrCallACPI_EX
(
    OBJGPU                              *pGpu,
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX *pParams
)
{
    NvU32     idx;
    NvU16     paramSize;
    NV_STATUS status;
    OBJSYS   *pSys   = SYS_GET_INSTANCE();
    PlatformRequestHandler      *pPlatformRequestHandler   = SYS_GET_PFM_REQ_HNDLR(pSys);

    for (idx = 0; idx < NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX_MAX_SZ; ++idx)
    {
        pParams->output[idx] = pParams->input[idx];
    }
    paramSize = pParams->outSize;

    if ((status = osCallACPI_DSM(pGpu,
                                 pPlatformRequestHandler->dsmVersion,
                                 pParams->pfmreqhndlrFunc,
                                 pParams->output,
                                 &paramSize)) != NV_OK)
    {
        status = NV_ERR_NOT_SUPPORTED;
    }
    return status;
}

//
// pfmreqhndlrInitSensors
//
// This function initialises the PLATFORM REQUEST HANDLER sensor data.
// it initializes all the data, and determines which sensors are available for sampling.
//
NV_STATUS
pfmreqhndlrInitSensors
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    NV_STATUS                           status          = NV_OK;
    OBJGPU                             *pGpu            = NULL;
    NvU32                               timeStamp       = 0;
    PFM_REQ_HNDLR_PSHAREDATA           *pPShareParams   = NULL;
    PFM_REQ_HNDLR_SENSOR_DATA          *pPfmreqhndlrData;
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS
                                        acpiParams;

    NV_ASSERT_OR_RETURN(NULL != pPlatformRequestHandler, NV_ERR_INVALID_REQUEST);

    pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    NV_ASSERT_OR_RETURN(NULL != pGpu, NV_ERR_INVALID_REQUEST);

    pPfmreqhndlrData =  &(pPlatformRequestHandler->sensorData);

    // init the PLATFORM REQUEST HANDLER data cache.
    portMemSet((void *)pPfmreqhndlrData, 0, sizeof(PFM_REQ_HNDLR_SENSOR_DATA));

    //
    // Initialize a logical sw version based of ACPI dsm version
    //
    if ((pPlatformRequestHandler->getProperty(pPlatformRequestHandler, PDB_PROP_PFMREQHNDLR_IS_PLATFORM_LEGACY)) ||
           (pPlatformRequestHandler->dsmVersion == ACPI_DSM_FUNCTION_GPS_2X))
    {
        pPfmreqhndlrData->PFMREQHNDLRACPIData.acpiVersionSw =
            NV0000_CTRL_PFM_REQ_HNDLR_ACPI_REVISION_SW_2X;
    }

    // Cache the current timestamp
    timeStamp = _pfmreqhndlrGetTimeStamp(pPlatformRequestHandler);

    pPfmreqhndlrData->counterHoldPeriod   =
        PFM_REQ_HNDLR_DEFAULT_COUNTER_HOLD_PERIOD_MS;

    //
    // init the PSHARESTATUS ACPI cache.
    // set up the defaults
    //
    pPfmreqhndlrData->PFMREQHNDLRACPIData.bPfmReqHndlrSupported    = NV_FALSE;

    //
    // Check if PLATFORM REQUEST HANDLER is supported on this platform
    // determine if we can call the PLATFORM REQUEST HANDLER ACPI call.
    //
    portMemSet((void *) &acpiParams, 0, sizeof(NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS));
    acpiParams.ctrl.cmd        = NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SUPPORT;
    acpiParams.ctrl.input      = 0;
    acpiParams.output.pBuffer  = NULL;
    acpiParams.output.bufferSz = 0;
    if (NV_OK == pfmreqhndlrCallACPI(pGpu, &acpiParams))
    {
        // Set up all the control flags based on all the supported subfunctions
        status = _pfmreqhndlrSetSupportedStatesFromAcpiParams(pGpu,
            pPlatformRequestHandler, acpiParams.output.result[1]);
        if (NV_OK != status)
        {
            NV_ASSERT_FAILED("Get all the supported PlatformRequestHandler subfunctions");
            return status;
        }
    }
    if (pPfmreqhndlrData->PFMREQHNDLRACPIData.bPfmReqHndlrSupported)
    {
        // call PSHARESTATUS to get the initial values.
        _pfmreqhndlrCallPshareStatus(pPlatformRequestHandler, NV_TRUE);

        // PShare param is available, determine which sensors it is reporting.
        pPShareParams = portMemAllocNonPaged(sizeof(PFM_REQ_HNDLR_PSHAREDATA));
        if (NULL != pPShareParams)
        {
            portMemSet((void *)pPShareParams, 0, sizeof(PFM_REQ_HNDLR_PSHAREDATA));
            portMemSet((void *)&acpiParams, 0, sizeof(NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS));

            acpiParams.ctrl.cmd = NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHAREPARAMS;
            acpiParams.ctrl.input =
                DRF_DEF(0000_CTRL_PFM_REQ_HNDLR,  _PSHARE_PARAMS, _STATUS_QUERY_TYPE, _SUPPORTED_FIELDS) |
                DRF_DEF(0000_CTRL_PFM_REQ_HNDLR,  _PSHARE_PARAMS, _STATUS_TGPU,       _TRUE)             |
                DRF_DEF(0000_CTRL_PFM_REQ_HNDLR,  _PSHARE_PARAMS, _STATUS_PPMD,       _TRUE);
            acpiParams.output.pBuffer  = (void *) pPShareParams;
            acpiParams.output.bufferSz = sizeof(PFM_REQ_HNDLR_PSHAREDATA);

            if (NV_OK == pfmreqhndlrCallACPI(pGpu, &acpiParams))
            {
                // Determine the SBP(PFMREQHNDLR) version on this platform
                // save the version number
                pPfmreqhndlrData->version = pPShareParams->ulVersion;

                // save a mask of available counters
                pPfmreqhndlrData->PFMREQHNDLRShareParamsAvailMask = pPShareParams->status &
                    (DRF_DEF(0000_CTRL_PFM_REQ_HNDLR,  _PSHARE_PARAMS, _STATUS_TGPU, _TRUE)             |
                     DRF_DEF(0000_CTRL_PFM_REQ_HNDLR,  _PSHARE_PARAMS, _STATUS_PPMD, _TRUE));

                _pfmreqhndlrInitSupportedCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_AVAIL_SENSOR_MSK, NV_FALSE, timeStamp);
                _pfmreqhndlrUpdateCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_AVAIL_SENSOR_MSK, NV_TRUE, pPfmreqhndlrData->PFMREQHNDLRShareParamsAvailMask, timeStamp);

                // check each counter & if available, flag current value, limit, & period as available.
                if (0 != (DRF_VAL(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS_STATUS, _TGPU, pPShareParams->status)))
                {
                    _pfmreqhndlrInitSupportedCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_TGPU_SENSOR), NV_TRUE, timeStamp);
                    _pfmreqhndlrInitSupportedCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_PERIOD(PFM_REQ_HNDLR_TGPU_SENSOR), NV_FALSE, timeStamp);
                    _pfmreqhndlrUpdateCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_PERIOD(PFM_REQ_HNDLR_TGPU_SENSOR), NV_TRUE, pPShareParams->tGpu, timeStamp);
                }
                if (PFM_REQ_HNDLR_IS_ACPI_VERSION_SW_2X(pPlatformRequestHandler) &&
                    (0 != (DRF_VAL(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS_STATUS, _PPMD, pPShareParams->status))))
                {
                    // Invalidate Platform Power Mode cache on boot
                    _pfmreqhndlrPlatformPowerModeStateReset(pPlatformRequestHandler);

                    // Init PPMD sensor if this is enabled
                    _pfmreqhndlrInitSupportedCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_PPMD_SENSOR), NV_TRUE, timeStamp);

                    // Sample the PPMD counter from init time
                    _pfmreqhndlrUpdatePpmdLimit(pPlatformRequestHandler, pGpu, NV_TRUE);
                }
            }
            portMemFree(pPShareParams);
        }

    }

    // as long as there is an PlatformRequestHandler object, these counters should always be available.
    // The following counters are non-Volatile, they are being updated as the corresponding functions change,
    // so they do not need to be sampled on request for a current value.
    _pfmreqhndlrInitSupportedCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_VPS_PS20_SUPPORT,        NV_TRUE,  timeStamp);

    // The following counters are volatile, and need to be sampled on request for a current value.
    _pfmreqhndlrInitSupportedCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_PM1_STATE_AVAIL,         NV_TRUE, timeStamp);

    // The following counters are dynamically controlled by the Thermal controller.
    // they are currently handled as volatile counters, but could be changed to non-volatile counters
    // if TC is modified to push the new values to PFMREQHNDLR when they change.
    // (note with the current policy of seperating TC & PFMREQHNDLR this is unlikely)
    _pfmreqhndlrInitSupportedCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_TC_ENABLE,               NV_TRUE, timeStamp);

    return status;
}

/*!
 * Set/update the value of a specific perf-counter (denoted by id).
 *
 * @param[in]  pPlatformRequestHandler   *PlatformRequestHandler pointer
 * @param[in]  id     Identifier of counter to update
 * @param[in]  value  New value to set for the counter
 *
 * @return NV_OK                   Upon successful update
 * @return NV_ERR_INVALID_ARGUMENT Invalid counter id
 * @return NV_ERR_INVALID_REQUEST
 *     Returned if the provided PlatformRequestHandler pointer was NULL or if the corresponding
 *     OBJGPU object could not be found.
 */
NV_STATUS
pfmreqhndlrUpdatePerfCounter
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    NvU32   id,
    NvU32   value
)
{
    OBJGPU          *pGpu      = NULL;
    PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData;
    NvU32            timeStamp;

    // is the Id within range?
    if (id >= PFM_REQ_HNDLR_NUM_COUNTERS)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (NULL == pPlatformRequestHandler)
    {
        NV_ASSERT(pPlatformRequestHandler);
        return NV_ERR_INVALID_REQUEST;
    }

    pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    if (NULL == pGpu)
    {
        return NV_ERR_INVALID_REQUEST;
    }
    // determine the current ms timestamp.
    timeStamp = _pfmreqhndlrGetTimeStamp(pPlatformRequestHandler);

    pPfmreqhndlrData =  &(pPlatformRequestHandler->sensorData);

    _pfmreqhndlrUpdateCounter(pPfmreqhndlrData, id, NV_TRUE, value, timeStamp);

    return NV_OK;
}

//
// PfmreqhndlrSampleCounter
//
// This function determines if it is appropriate to update the specified counter & does so if it is.
//  Note that this function contains all the code specific to the sampling of each counter.
//
NV_STATUS
pfmreqhndlrSampleCounter
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    NvU32   id,
    NvU32   timeStamp
)
{
    NV_STATUS                     status           = NV_ERR_NOT_SUPPORTED;
    PFM_REQ_HNDLR_SENSOR_DATA    *pPfmreqhndlrData = &(pPlatformRequestHandler->sensorData);
    PFM_REQ_HNDLR_SENSOR_COUNTER *pCounter         = &(pPfmreqhndlrData->PFMREQHNDLRSensorCache[id]);
    OBJGPU                       *pGpu             = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    NvBool                        bPM1Available    = NV_TRUE;

    NV_ASSERT_OR_RETURN(NULL != pPlatformRequestHandler, NV_ERR_INVALID_REQUEST);

    if (0 == timeStamp)
    {
        timeStamp = _pfmreqhndlrGetTimeStamp(pPlatformRequestHandler);
    }

    // check if we can sample the counter.
    // (marked available & can be sampled now.)
    if (pCounter->bSupported
        && (pCounter->bInvalid || ((timeStamp - pCounter->lastSampled) >= pCounter->minInterval)))
    {
        // Sample the requested counter.
        switch (id)
        {
            case PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_TGPU_SENSOR):
            case PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_PPMD_SENSOR):
            {
                status = _pfmreqhndlrSampleSensorLimit(pPlatformRequestHandler, id, timeStamp);
                break;
            }
            case PFM_REQ_HNDLR_PM1_STATE_AVAIL:
            {
                if (!pPlatformRequestHandler->controlData.bPM1ForcedOff)
                {
                     if (pfmreqhndlrHandleCheckPM1Available(pPlatformRequestHandler, pGpu, &bPM1Available) == NV_OK)
                     {
                        _pfmreqhndlrUpdateCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_PM1_STATE_AVAIL, NV_TRUE, (NvU32)bPM1Available, timeStamp);
                        status = NV_OK;
                     }
                }
                //If PM1 is forced off or Boost Clocks not available
                else
                {
                    status = NV_ERR_NOT_SUPPORTED;
                }
                break;
            }
            case PFM_REQ_HNDLR_VPS_PS20_SUPPORT:
            {
                _pfmreqhndlrUpdateCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_VPS_PS20_SUPPORT, NV_TRUE, pPlatformRequestHandler->vPstateCache.bVpsPs20Supported, timeStamp);
                break;
            }
            case PFM_REQ_HNDLR_TC_ENABLE:
            {
                _pfmreqhndlrUpdateCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_TC_ENABLE, NV_TRUE, NV_TRUE, timeStamp);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    return status;
}

/*!
 * @brief   Retrieves the requested counter and returns it if available.
 *
 * @param[in]   pPlatformRequestHandler            *PlatformRequestHandler pointer
 * @param[in]   counterId       ID of the requested counter
 * @param[out]  pCounterVal     buffer to hold value of requested counter
 *
 * @return  NV_ERR_INVALID_POINTER  NULL pointer passed
 * @return  NV_ERR_NOT_SUPPORTED    various failures (TODO)
 * @return  NV_ERR_INVALID_DATA     counter is not available
 * @return  NV_OK                   counter successfully retrieved
 */
NV_STATUS
pfmreqhndlrGetPerfSensorCounterById_IMPL
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    NvU16   counterId,
    NvU32  *pCounterVal
)
{
    OBJSYS          *pSys     = SYS_GET_INSTANCE();
    OBJGPU          *pGpu     = NULL;
    OBJOS           *pOs      = NULL;
    PPFM_REQ_HNDLR_SENSOR_DATA pPfmreqhndlrData = &(pPlatformRequestHandler->sensorData);
    NV_STATUS        status   = NV_ERR_INVALID_DATA;
    NvU32            timeStamp;

    NV_ASSERT_OR_RETURN(NULL != pCounterVal, NV_ERR_INVALID_POINTER);
    NV_ASSERT_OR_RETURN(NULL != pPlatformRequestHandler, NV_ERR_NOT_SUPPORTED);

    *pCounterVal = 0;

    pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    if(pGpu == NULL){
        return NV_ERR_NOT_SUPPORTED;
    }

    pOs  = SYS_GET_OS(pSys);
    if(pOs == NULL){
        return NV_ERR_NOT_SUPPORTED;
    }

    timeStamp = _pfmreqhndlrGetTimeStamp(pPlatformRequestHandler);

    // Is the Id within range?
    if (counterId >= PFM_REQ_HNDLR_NUM_COUNTERS)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Invalid counter Id specified = %d.\n",
            counterId);
            DBG_BREAKPOINT();
            status = NV_ERR_NOT_SUPPORTED;
            goto pfmreqhndlrGetPerfSensorCounterById_IMPL_exit;
    }
    // is the counter available?
    if ((!pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].bSupported) &&
        (!pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].bVolatile))
    {
        goto pfmreqhndlrGetPerfSensorCounterById_IMPL_exit;
    }

    // Has the counter gone stale since the last sampling?
    if (_pfmreqhndlrIsCacheEntryStale(pPfmreqhndlrData, counterId, timeStamp))
    {
        // sample the counter
        if (PFM_REQ_HNDLR_IS_ACPI_VERSION_SW_2X(pPlatformRequestHandler))
        {
            // 2X path
            status = pfmreqhndlrSampleCounter(pPlatformRequestHandler, counterId, timeStamp);
            if (status != NV_OK)
            {
                if (status == NV_ERR_NOT_SUPPORTED)
                {
                    // OK to sample unsupported counters (and fall through).
                    status = NV_OK;
                }
                else
                {
                    // Fail for any other error code (and bail)
                    NV_PRINTF(LEVEL_ERROR,
                        "Sampling counter failed for counter id = %d.\n",
                        counterId);
                    DBG_BREAKPOINT();
                    goto pfmreqhndlrGetPerfSensorCounterById_IMPL_exit;
                }
            }
        }
        else
        {
            // 1X path
            pfmreqhndlrSampleCounter(pPlatformRequestHandler, counterId, timeStamp);
        }
    }
    //
    // NOTE per spec, some counters may be reported as available,
    // but may not be available under some conditions.
    // I.E. pshare params may report a counter as available when supported fields are queried,
    // but the limits are only available if the SBIOS wants that particular sensor limited.
    // so these sensors may be intermittently available.
    // to check for this case, check to see if the counter is still stale.
    //
    if (!_pfmreqhndlrIsCacheEntryStale(pPfmreqhndlrData, counterId, timeStamp) &&
        !pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].bInvalid)
    {
        // the counter is not stale,
        // copy it to the output
        *pCounterVal = pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].value;
        status = NV_OK;
        goto pfmreqhndlrGetPerfSensorCounterById_IMPL_exit;
    }

pfmreqhndlrGetPerfSensorCounterById_IMPL_exit:
    return status;
}

NV_STATUS
pfmreqhndlrHandleStatusChangeEvent
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    OBJGPU *pGpu
)
{
    NV_STATUS status              = NV_OK;
    NvU32     data                = 0;
    NvBool    bIsPctrlSupported   = NV_TRUE;

    NV_ASSERT_OR_RETURN(NULL != pPlatformRequestHandler, NV_ERR_INVALID_REQUEST);

    // get the latest pshare status
    status = _pfmreqhndlrCallPshareStatus(pPlatformRequestHandler, NV_FALSE);
    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("PshareStatus", status);
    }

    if (PFM_REQ_HNDLR_IS_ACPI_VERSION_SW_2X(pPlatformRequestHandler))
    {
        // Additionally if PCONTROL subfunction is supported call it
        NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS    acpiParams;

        portMemSet((void *) &acpiParams, 0, sizeof(NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS));
        acpiParams.ctrl.cmd        = NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SUPPORT;
        acpiParams.ctrl.input      = 0;
        acpiParams.output.pBuffer  = NULL;
        acpiParams.output.bufferSz = 0;
        status = pfmreqhndlrCallACPI(pGpu, &acpiParams);
        if (status != NV_OK)
        {
            NV_ASSERT_OK_FAILED("supported subfunction", status);
            goto pfmreqhndlrHandleStatusChangeEvent_exit;
        }
        if (0 == (DRF_VAL(0000_CTRL_PFM_REQ_HNDLR, _SUPPORTED, _PCONTROL_AVAIL, acpiParams.output.result[1])))
        {
            // PCONTTOL is not supported on 2X, can be hard coded
            bIsPctrlSupported = NV_FALSE;
        }
    }

    if (bIsPctrlSupported)
    {
        status = pfmreqhndlrPcontrol(pPlatformRequestHandler, data, NV_TRUE);
        if (status != NV_OK)
        {
            NV_ASSERT_FAILED("Pcontrol command");
            goto pfmreqhndlrHandleStatusChangeEvent_exit;
        }
    }

pfmreqhndlrHandleStatusChangeEvent_exit:
    return status;
}

/*!
 * Helper routine to check PM1 Available Status
 *
 * @param[in]      pPlatformRequestHandler    *PlatformRequestHandler object pointer
 * @param[in]      pGpu                        GPU object pointer
 * @param[out]     bIsPm1Available             Status of PM1 Availability
 *
 * @return NV_OK if RMAPI call was succesful.
 * @return bubble up errors from called RMAPI
 */

NV_STATUS
pfmreqhndlrHandleCheckPM1Available_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler,
    OBJGPU                 *pGpu,
    NvBool                 *pbPM1Available
)
{
    NV_STATUS                                                         status = NV_ERR_GENERIC;
    NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS  params = { 0 };
    RM_API                                                           *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    params.flag = NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_GET_PM1_STATUS;

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE,
                             &params,
                             sizeof(params));

    if (status == NV_OK)
    {
        *pbPM1Available = params.bStatus;
    }

    return status;
}

/*!
 * Handler routine to Update or Reset EDPpeak limit of GPU
 *
 * @param[in]      pPlatformRequestHandler         *PlatformRequestHandler object pointer
 * @param[in]      pGpu         GPU object pointer
 * @param[in]      bEnable      Enable or Reset the settings.
 *
 * @return NV_OK if the update was successful.
 * @return NV_ERR_NOT_SUPPORTED if the POR support is not present
 * @return NV_ERR_INVALID_STATE for invalid PMU state
 * @return  bubble up errors triggered by called code
 */
NV_STATUS
pfmreqhndlrHandleEdppeakLimitUpdate_IMPL
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    OBJGPU *pGpu,
    NvBool  bEnable
)
{
    RM_API     *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS   status = NV_OK;

    if (gpuIsGpuFullPower(pGpu))
    {
        NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS params = { 0 };

        params.bEnable = bEnable;
        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT,
                                 &params,
                                 sizeof(params));

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Error updating EDPpeak Limit State: 0x%08x\n", status);
            DBG_BREAKPOINT();
        }
        else
        {
            // Update the cached enable bit, if everything went well
            pPlatformRequestHandler->controlData.bEDPpeakUpdateEnabled = bEnable;
        }
    }

    return status;
}

/*!
 * Handler routine to Update or Reset EDPpeak limit from platform
 *
 * @param[in]      pPlatformRequestHandler          PlatformRequestHandler object pointer
 * @param[in]      pGpu                             GPU object pointer
 * @param[in]      platformEdppLimit                Platform EDPpeak limit value
 *
 * @return NV_OK if the update was successful.
 * @return NV_ERR_NOT_SUPPORTED if the POR support is not present
 * @return NV_ERR_INVALID_STATE for invalid PMU state
 * @return  bubble up errors triggered by called code
 */
NV_STATUS
pfmreqhndlrHandlePlatformEdppLimitUpdate_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler,
    OBJGPU *pGpu,
    NvU32   platformEdppLimit
)
{
    RM_API     *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS   status = NV_OK;

    if (gpuIsGpuFullPower(pGpu))
    {
        NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT_PARAMS params = { 0 };

        params.clientLimit = platformEdppLimit;
        params.bEnable     = NV_TRUE;

        // Platform can remove its EDPp and fall back to GPU default by setting a limit of value 0
        if (platformEdppLimit == 0)
        {
            params.bEnable = NV_FALSE;
        }
        status = pRmApi->Control(pRmApi,
                                 pGpu->hInternalClient,
                                 pGpu->hInternalSubdevice,
                                 NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_UPDATE_EDPP_LIMIT,
                                 &params,
                                 sizeof(params));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Error updating EDPpeak Limit State: 0x%08x\n", status);
            DBG_BREAKPOINT();
        }
    }

    return status;
}

/*!
 * Handle EDPpeak new limit query from platform
 *
 * @param[in]      pPlatformRequestHandler          PlatformRequestHandler object pointer
 * @param[in]      pGpu                             GPU object pointer
 * @param[in]      pPlatformEdppLimit               Pointer to Platform EDPpeak limit value
 *
 * @return NV_OK if the update was successful.
 * @return NV_ERR_NOT_SUPPORTED if the POR support is not present
 * @return  bubble up errors triggered by called code
 */
NV_STATUS
pfmreqhndlrHandlePlatformGetEdppLimit_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler,
    OBJGPU  *pGpu,
    NvU32   *pPlatformEdppLimit
)
{
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS acpiParams;
    NV_STATUS status = NV_OK;

    portMemSet((void*)&acpiParams, 0, sizeof(NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS));

    acpiParams.ctrl.cmd = NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_GETEDPPLIMIT;

    // Input is V1
    acpiParams.ctrl.input = NV0000_CTRL_PFM_REQ_HNDLR_EDPP_VERSION_V10;
    status = pfmreqhndlrCallACPI(pGpu, &acpiParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error querying EDPpeak Limit from platform: 0x%08x\n", status);
        DBG_BREAKPOINT();
        status = NV_ERR_NOT_SUPPORTED;
        goto _pfmreqhndlrHandlePlatformGetEdppLimit_IMPL_IMPL_exit;
    }

    *pPlatformEdppLimit = acpiParams.output.result[0];

_pfmreqhndlrHandlePlatformGetEdppLimit_IMPL_IMPL_exit:
    return status;
}

/*!
 * Handle send GPU EDPpeak limit info to platform
 *
 * @param[in]      pPlatformRequestHandler          PlatformRequestHandler object pointer
 * @param[in]      pGpu                             GPU object pointer
 *
 * @return NV_OK if the update was successful.
 * @return  bubble up errors triggered by called code
 */
NV_STATUS
pfmreqhndlrHandlePlatformSetEdppLimitInfo_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler,
    OBJGPU *pGpu
)
{

    RM_API                  *pRmApi      = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_STATUS                status      = NV_OK;
    PFM_REQ_HNDLR_EDPP_DATA *pEdppLimit  =
            &pPlatformRequestHandler->controlData.edppLimit;

    if (gpuIsGpuFullPower(pGpu))
    {
        NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO_PARAMS params = { 0 };

        status = pRmApi->Control(pRmApi,
            pGpu->hInternalClient,
            pGpu->hInternalSubdevice,
            NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_GET_EDPP_LIMIT_INFO,
            &params,
            sizeof(params));
        if (status == NV_OK)
        {
            pEdppLimit->edppLimitInfo.limitMin       = params.limitMin;
            pEdppLimit->edppLimitInfo.limitRated     = params.limitRated;
            pEdppLimit->edppLimitInfo.limitMax       = params.limitMax;
            pEdppLimit->edppLimitInfo.limitCurr      = params.limitCurr;
            pEdppLimit->edppLimitInfo.limitBattRated = params.limitBattRated;
            pEdppLimit->edppLimitInfo.limitBattMax   = params.limitBattMax;
            pEdppLimit->edppLimitInfo.rsvd           = 0;
            pEdppLimit->status                       = NV_OK;
        }
        else
        {
            pEdppLimit->status = NV_ERR_INVALID_ARGUMENT;
        }
    }
    else
    {
        pEdppLimit->status = NV_ERR_GPU_NOT_FULL_POWER;
    }

    if (!pEdppLimit->bWorkItemPending)
    {
        // Queue workitem to send the EDP limit info to platform
        status = osQueueWorkItem(pGpu,
                    _pfmreqhndlrHandlePlatformSetEdppLimitInfoWorkItem, NULL);
        if(status == NV_OK)
        {
            pEdppLimit->bWorkItemPending = NV_TRUE;
        }
    }

    return status;
}

/*!
 * Handler routine to enable or Reset GPU User Configurable TGP mode
 *
 * @param[in]      pPlatformRequestHandler         *PlatformRequestHandler object pointer
 * @param[in]      pGpu         GPU object pointer
 * @param[in]      bEnable      Enable or Reset the settings.
 *
 * @return NV_OK if the update was successful.
 * @return NV_ERR_NOT_SUPPORTED if the POR support is not present
 * @return NV_ERR_INVALID_STATE for invalid PMU state
 * @return  bubble up errors triggered by called code
 */
NV_STATUS
pfmreqhndlrHandleUserConfigurableTgpMode_IMPL
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    OBJGPU *pGpu,
    NvBool  bEnable
)
{
    NV_STATUS                                                              status = NV_ERR_GENERIC;
    NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE_PARAMS  params = { 0 };
    RM_API                                                                *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    params.bEnable = bEnable;

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TGP_MODE,
                             &params,
                             sizeof(params));
    if (status == NV_OK)
    {
        // Update the cached enable bit, if everything went well
        pPlatformRequestHandler->controlData.bUserConfigTGPmodeEnabled = bEnable;
    }

    return status;
}

/* ---------------------- Static Functions --------------------------------- */

static NV_STATUS
_handleCmdSystemPfmreqhndlrGetSupportedFunctions
(
    PlatformRequestHandler
            *pPlatformRequestHandler,
    NvU16    locale,
    NvU32   *pData
)
{
    OBJGPU* pGpu = NULL;

    *pData = 0;


    if (locale != NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_LOCALE_SYSTEM)
    {
        return NV_ERR_INVALID_COMMAND;
    }

    pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    *pData |= (NV_OK == testIfDsmSubFunctionEnabled(pGpu, pPlatformRequestHandler->dsmVersion,
                                                    NV_ACPI_ALL_FUNC_SUPPORT)) ?
               NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_SYS_DEF_FUNC_SUPPORT : 0;

    return NV_OK;
}

static NV_STATUS
_handleCmdSystemPfmreqhndlrDataInitUsingSbiosAndAck
(
    PlatformRequestHandler
            *pPlatformRequestHandler,
    NvU16    locale,
    NvU32   *pData
)
{
    OBJGPU    *pGpu;
    NV_STATUS  status  = NV_OK;

    if (locale != NV0000_CTRL_CMD_SYSTEM_PFM_REQ_HNDLR_LOCALE_SYSTEM)
    {
        return NV_ERR_INVALID_COMMAND;
    }

    pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);

    /* Full GPU check to ensure device is on */
    NV_ASSERT_OR_RETURN(FULL_GPU_SANITY_CHECK(pGpu), NV_ERR_INVALID_STATE);

    if (pPlatformRequestHandler->getProperty(pPlatformRequestHandler, PDB_PROP_PFMREQHNDLR_SUPPORTED))
    {
        status = pfmreqhndlrHandleStatusChangeEvent(pPlatformRequestHandler, pGpu);
    }
    else
    {
        status = NV_ERR_NOT_SUPPORTED;
    }

    return status;
}

/*!
 * Get a current 32 bit timestamp in MS
 *
 * @param[in]  pPlatformRequestHandler   *PlatformRequestHandler pointer
 *
 * @return 32 bit MS timestamp.
 */
static NvU32
_pfmreqhndlrGetTimeStamp
(
    PlatformRequestHandler    *pPlatformRequestHandler
)
{
    OBJGPU *pGpu      = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    OBJTMR *pTmr      = GPU_GET_TIMER(pGpu);
    NvU32   timeStamp = 0;

    timeStamp = (NvU32)(tmrGetTime_HAL(pGpu, pTmr) / 1000000L);

    return timeStamp;
}

/*!
 * Configure a counter with an override value.
 *
 * @param[in]  pPfmreqhndlrData     pointer to the PlatformRequestHandler data structure
 * @param[in]  id           identifier of counter to whose Override should be canceled.
 * @param[in]  value        new value for the specified counter. All values are accepted as
 *                          unsigned integers, but internally they should be formatted based on the
 *                          type of id/sensor they correspond to.
 * @param[in]  timeStamp    current time stamp in MS used to indicate when the counter was last updated
 *
 */
static void
_pfmreqhndlrOverrideCounter
(
    PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData,
    NvU32            id,
    NvU32            value,
    NvU32            timeStamp
)
{
    // update the counter with the new value.
    _pfmreqhndlrUpdateCounter(pPfmreqhndlrData, id, NV_TRUE, value, timeStamp);

    // lock it in as an overriden value.
    pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bOverridden = NV_TRUE;
}

/*!
 * Cancel an override of a specified counter.
 *
 * @param[in]  pPfmreqhndlrData     pointer to the PlatformRequestHandler data structure
 * @param[in]  id           Identifier of counter to whose Override should be canceled.
 * @param[in]  timeStamp    current time stamp in MSused to indicate when the caounter was last updated
 *
 * @note if a non-volatile counter has its override canceled, it may not be refreshed as part of a query,
 * it is up to the calling function to ensure that the counter is placed in a valid (I.E. updated) state.
 */
static void
_pfmreqhndlrCancelCounterOverride
(
    PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData,
    NvU32            id,
    NvU32            timeStamp
)
{
    if (pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bOverridden)
    {
        pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bOverridden = NV_FALSE;
        _pfmreqhndlrResetCounter(pPfmreqhndlrData, id, timeStamp);
    }
}

/*!
 * place the counter into a reset state where it does not contain a valid value, and the sample is expired.
 *  it updates the counter so that the value is marked as invalid & the timestamp
 *  is set so that the counter can be updated
 *
 * @param[in]  pPfmreqhndlrData Pointer to the PlatformRequestHandler data structure
 * @param[in]  id       Identifier of counter to which should be marked invalid.
 *
 * @note if a non-volatile counter is marked as invalid, it may not be refreshed as part of a query,
 * it is up to the calling function to ensure that the counter is placed in a valid (I.E. updated) state.
 */
static void
_pfmreqhndlrResetCounter
(
    PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData,
    NvU32            id,
    NvU32            timeStamp
)
{
    if (!pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bOverridden)
    {
        pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bInvalid    = NV_TRUE;

        // use a timestamp that marks the value as stale in order to ensure that
        // volatile counters are refreshed prior to being reported.
        pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].lastSampled = timeStamp - NV_MAX(pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].minInterval, pPfmreqhndlrData->counterHoldPeriod) - 1;
    }
}

/*!
 * Update a counter to a specified state (valid/invalid) & value.
 *  it updates the specified counter based on the result of the sampling
 *  (as reported in bValid) and the value provided.
 *
 * @param[in]  pPfmreqhndlrData     pointer to the PlatformRequestHandler data structure
 * @param[in]  id           identifier of counter to be updated.
 * @param[in]  bValid       a bool indicating a valid value is being provided for the counter.
 * @param[in]  value        new value for the specified counter. All values are accepted as
 *                          unsigned integers, but internally they should be formatted based on the
 *                          type of id/sensor they correspond to.
 * @param[in]  timeStamp    current time stamp in MS used to indicate when the counter was last updated
 *
 */
static void
_pfmreqhndlrUpdateCounter
(
    PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData,
    NvU32            id,
    NvBool           bValid,
    NvU32            value,
    NvU32            timeStamp
)
{
    if (bValid || pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bOverridden)
    {
        // only update the value if the current value is not an override.
        if (!pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bOverridden)
        {
            pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].value   = value;
        }
        pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].lastSampled = timeStamp;
        pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bInvalid    = NV_FALSE;
    }
    else
    {
        // The counter is not valid or overridden, mark it as invalid.
        pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bInvalid    = NV_TRUE;
    }
}

/*!
 * Enables/disables PM1 (boost clocks).
 *
 * @param[in]   pGpu        OBJGPU pointer
 * @param[in]   bEnable     Boolean value to turn on/off PM1 (boost clocks)
 *
 * @return NV_OK                 PM1 (boost clocks) successfully enabled/disabled
 * @return NV_ERR_NOT_SUPPORTED  PM1 (boost clocks) not supported
 * @return NV_ERR_INVALID_STATE  Could not enable/disable PM1
 */
static NV_STATUS
_pfmreqhndlrEnablePM1
(
    OBJGPU *pGpu,
    NvBool  bEnable
)
{
    NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE_PARAMS  params = { 0 };
    RM_API                                                           *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    params.bEnable = bEnable;

    return pRmApi->Control(pRmApi,
                           pGpu->hInternalClient,
                           pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_PM1_STATE,
                           &params,
                           sizeof(params));
}

/*!
 * Sample and update the Platform Power Mode data.
 *
 * @param[in] pPlatformRequestHandler  *PlatformRequestHandler object poiner
 * @param[in] pGpu  OBJGPU object pointer
 * @param[in] If called from PlatformRequestHandler init sequence
 *
 * @return NV_OK on success
 * @return Bubble up error otherwise
 */

static NV_STATUS
_pfmreqhndlrUpdatePpmdLimit
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    OBJGPU *pGpu,
    NvBool  bInit
)
{
    NV_STATUS     status     = NV_OK;
    PFM_REQ_HNDLR_PPM_DATA *pPpmData   = NULL;
    NvU32         counterVal = 0;
    NV2080_PLATFORM_POWER_MODE_CHANGE_STATUS params = { 0 };
    NvU32 info32 = 0;

    status = pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler,
        PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_PPMD_SENSOR),
        &counterVal);
    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("PlatformRequestHandler failed to get platform power mode from SBIOS", status);
        goto _pfmreqhndlrUpdatePpmdLimit_exit;
    }

    if (PFM_REQ_HNDLR_IS_COUNTER_VALID(pPlatformRequestHandler, PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_PPMD_SENSOR)))
    {
        pPpmData =  &(pPlatformRequestHandler->ppmData);

        // Update the power mode mask as received from the platform
        pPpmData->ppmMaskCurrent =
            DRF_VAL(0000_CTRL_PFM_REQ_HNDLR, _PPM, _MASK, counterVal);

        //
        // Cache the platform requested power mode during init time only.
        // Runtime, the system request should result in a client notification only.
        //
        if (bInit)
        {
            pPpmData->ppmIdxCurrent =
                DRF_VAL(0000_CTRL_PFM_REQ_HNDLR, _PPM, _INDEX, counterVal);
        }

        // Notify clients with the new limits
        params.platformPowerModeIndex = DRF_VAL(0000_CTRL_PFM_REQ_HNDLR, _PPM, _INDEX, counterVal);
        params.platformPowerModeMask  = pPpmData->ppmMaskCurrent;
        params.eventReason            = NV2080_PLATFORM_POWER_MODE_CHANGE_ACPI_NOTIFICATION;

        info32 = (params.platformPowerModeIndex << DRF_SHIFT(NV2080_PLATFORM_POWER_MODE_CHANGE_INFO_INDEX)) |
                 (params.platformPowerModeMask << DRF_SHIFT(NV2080_PLATFORM_POWER_MODE_CHANGE_INFO_MASK)) |
                 (params.eventReason << DRF_SHIFT(NV2080_PLATFORM_POWER_MODE_CHANGE_INFO_REASON));

        gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_PLATFORM_POWER_MODE_CHANGE, &params, sizeof(params), info32, 0);
    }

_pfmreqhndlrUpdatePpmdLimit_exit:
    return status;
}

static NV_STATUS
_pfmreqhndlrUpdateTgpuLimit
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    OBJGPU *pGpu
)
{
    NvU32      counterVal;
    NvTemp     targetTemp = PFM_REQ_HNDLR_TEMP_0_C;
    NV_STATUS  status     = NV_ERR_NOT_SUPPORTED;
    RM_API    *pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);

    // note -- we need to go through the call to ensure any override is honored.
    status = pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler,
                PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_TGPU_SENSOR),
                &counterVal);

    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("PlatformRequestHandler failed to get target temp from SBIOS", status);
        return status;
    }

    targetTemp = NV_TYPES_CELSIUS_TO_NV_TEMP(counterVal);

    // 2x operation
    if (PFM_REQ_HNDLR_IS_ACPI_VERSION_SW_2X(pPlatformRequestHandler))
    {
        // 2X operation Will not allow a 0C default value from SBIOS to SWTC interface
        if (targetTemp == PFM_REQ_HNDLR_TEMP_0_C)
        {
            // The VBIOS default limit takes effect
            status = NV_OK;
            return status;
        }
    }


    NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS params = { 0 };
    params.targetTemp = targetTemp;

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT,
                             &params,
                             sizeof(params));

    return status;
}

/*!
 * @brief helper function to update each system control parameter.
 *
 * @param[in]  pPlatformRequestHandler   *PlatformRequestHandler pointer
 * @param[in]  id     Identifier of counter to update
 */
static NV_STATUS
_pfmreqhndlrUpdateSystemParamLimit(PlatformRequestHandler *pPlatformRequestHandler, NvU32 id)
{
    NV_STATUS status;
    OBJGPU   *pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);

    switch (id)
    {
        case PFM_REQ_HNDLR_TGPU_SENSOR:
        {
            status = _pfmreqhndlrUpdateTgpuLimit(pPlatformRequestHandler, pGpu);
            break;
        }
        default:
        {
            status = NV_ERR_NOT_SUPPORTED;
            break;
        }
    }

    return status;
}

/*!
 * @brief passive worker thread to platform power mode.
 *
 * @param[in]   gpuInstance     GPU instance ID
 * @param[in]   newIndex         requested Platform Power Mode Index
 */
static void
_pfmreqhndlrUpdatePlatformPowerModeWorkItem
(
    NvU32  gpuInstance,
    void  *pParams
)
{
    OBJSYS                             *pSys           = SYS_GET_INSTANCE();
    PlatformRequestHandler                                *pPlatformRequestHandler           = SYS_GET_PFM_REQ_HNDLR(pSys);
    OBJGPU                             *pGpu           = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    NvU32                               status         = NV_OK;
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX acpiParamsEx   = {0};
    NvU32                               gpuLockFlag    = GPUS_LOCK_FLAGS_NONE;

    if (FULL_GPU_SANITY_CHECK(pGpu))
    {
        // Attempt to acquire locks/semaphore
        if (pfmreqhndlrPassiveModeTransition(NV_TRUE, API_LOCK_FLAGS_NONE,
                gpuLockFlag) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to acquire the locks/semaphore!\n");
            goto pfmreqhndlrUpdatePlatformPowerModeWorkItemExit;
        }

        // Call ACPI to set the new platform power mode
        acpiParamsEx.inSize = NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_COUNT * sizeof(NvU32);
        acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_VERSION_IDX] =
            FLD_SET_DRF(0000_CTRL_PFM_REQ_HNDLR, _PPM_ARG0_VERSION, _MAJOR, _V1,
                acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_VERSION_IDX]);
        acpiParamsEx.outSize = NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_COUNT * sizeof(NvU32);
        acpiParamsEx.pfmreqhndlrFunc = GPS_FUNC_SETPPM;
        acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_IDX] =
            FLD_SET_DRF_NUM(0000_CTRL_PFM_REQ_HNDLR, _PPM_ARGS, _INDEX,
                pPlatformRequestHandler->ppmData.ppmIdxRequested, acpiParamsEx.input[NV0000_CTRL_PFM_REQ_HNDLR_PPM_ARGS_IDX]);

        status = pfmreqhndlrCallACPI_EX(pGpu, &acpiParamsEx);

        //
        // If ACPI call is successful, cache the new Index for future Get PPM calls
        // and notify NvCPL
        //
        if (NV_OK == status)
        {
            NV2080_PLATFORM_POWER_MODE_CHANGE_STATUS params = {0};
            NvU32 info32                                    = 0;
            pPlatformRequestHandler->ppmData.ppmIdxCurrent                     = pPlatformRequestHandler->ppmData.ppmIdxRequested;
            params.platformPowerModeIndex                   = pPlatformRequestHandler->ppmData.ppmIdxCurrent;
            params.platformPowerModeMask                    = pPlatformRequestHandler->ppmData.ppmMaskCurrent;
            params.eventReason                              = NV2080_PLATFORM_POWER_MODE_CHANGE_COMPLETION;

            info32 = (params.platformPowerModeIndex << DRF_SHIFT(NV2080_PLATFORM_POWER_MODE_CHANGE_INFO_INDEX)) |
                     (params.platformPowerModeMask << DRF_SHIFT(NV2080_PLATFORM_POWER_MODE_CHANGE_INFO_MASK)) |
                     (params.eventReason << DRF_SHIFT(NV2080_PLATFORM_POWER_MODE_CHANGE_INFO_REASON));

            gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_PLATFORM_POWER_MODE_CHANGE, &params, sizeof(params), info32, 0);
        }

        // Release locks/semaphore
        pfmreqhndlrPassiveModeTransition(NV_FALSE, API_LOCK_FLAGS_NONE, gpuLockFlag);
    }

pfmreqhndlrUpdatePlatformPowerModeWorkItemExit:
    // Reset on exit
    pPlatformRequestHandler->ppmData.ppmIdxRequested  = NV0000_CTRL_PFM_REQ_HNDLR_PPM_INDEX_INVALID;
    pPlatformRequestHandler->ppmData.bWorkItemPending = NV_FALSE;
}

/*!
 * @brief passive worker thread to update the system control parameters.
 *
 * @param[in]   gpuInstance     GPU instance ID
 * @param[in]   pParam          unused
 */
static void
_pfmreqhndlrUpdateSystemParamLimitWorkItem
(
    NvU32  gpuInstance,
    void  *pParams
)
{
    OBJSYS    *pSys    = SYS_GET_INSTANCE();
    PlatformRequestHandler       *pPlatformRequestHandler    = SYS_GET_PFM_REQ_HNDLR(pSys);
    NvU32      id;

    // Attempt to acquire locks/semaphore
    if (pfmreqhndlrPassiveModeTransition(NV_TRUE, API_LOCK_FLAGS_NONE, GPUS_LOCK_FLAGS_NONE) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to acquire the locks/semaphore!\n");

        pPlatformRequestHandler->sysControlData.bWorkItemPending = NV_FALSE;
        return;
    }

    FOR_EACH_INDEX_IN_MASK(32, id,  pPlatformRequestHandler->sysControlData.queuedCounterMask)
    {
        _pfmreqhndlrUpdateSystemParamLimit(pPlatformRequestHandler, id);
    }
    FOR_EACH_INDEX_IN_MASK_END;

    pPlatformRequestHandler->sysControlData.bWorkItemPending = NV_FALSE;
    pPlatformRequestHandler->sysControlData.queuedCounterMask = 0;

    // Release locks/semaphore
    pfmreqhndlrPassiveModeTransition(NV_FALSE, API_LOCK_FLAGS_NONE, GPUS_LOCK_FLAGS_NONE);
}

/*!
 *
 * _pfmreqhndlrCallPshareStatus
 *
 * This function makes the PSHARESTATUS ACPI call. It sets up the call based on
 * the ACPI cache & updates the cache with the results.
 *
 * @param[in]      pPlatformRequestHandler         *PlatformRequestHandler object pointer
 * @param[in]      bInit        If called from PlatformRequestHandler init sequence
 *
 * @return NV_OK if the update was successful.
 * @return NV_ERR_INVALID_STATE for invalid PMU state
 * @return  bubble up errors triggered by called code
 */
static NV_STATUS
_pfmreqhndlrCallPshareStatus
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    NvBool  bInit
)
{
    OBJGPU                    *pGpu             = NULL;
    PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData = NULL;
    NV_STATUS                  status           = NV_OK;
    NV_STATUS                  lclStatus        = NV_OK;
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS
            acpiParams;
    NvU32   timeStamp;

    NV_ASSERT_OR_RETURN(NULL != pPlatformRequestHandler, NV_ERR_INVALID_REQUEST);

    pGpu             = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    pPfmreqhndlrData = &pPlatformRequestHandler->sensorData;

    acpiParams.ctrl.cmd        = NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHARESTATUS;
    acpiParams.ctrl.input      = 0;
    acpiParams.output.pBuffer  = NULL;
    acpiParams.output.bufferSz = 0;

    status = pfmreqhndlrCallACPI(pGpu, &acpiParams);

    if (NV_OK == status)
    {
        NvBool bQueryEdppRequired = NV_FALSE;
        NvBool bPlatformEdpUpdate = NV_FALSE;

        // Platform EDPp limit handling check
        bQueryEdppRequired = FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARESTATUS, _PLATFORM_GETEDPPEAKLIMIT_SET, _TRUE, acpiParams.output.result[0]);
        bPlatformEdpUpdate = FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARESTATUS, _PLATFORM_SETEDPPEAKLIMITINFO_SET, _TRUE, acpiParams.output.result[0]);

        // update the cache values
        pPfmreqhndlrData->PFMREQHNDLRACPIData.bSystemParamLimitUpdate  = FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARESTATUS, _UPDATE_LIMIT, _PENDING, acpiParams.output.result[0]);

        pPfmreqhndlrData->PFMREQHNDLRACPIData.bEDPpeakLimitUpdateRequest    =
            FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARESTATUS, _EDPPEAK_LIMIT_UPDATE, _TRUE, acpiParams.output.result[0]);
        pPfmreqhndlrData->PFMREQHNDLRACPIData.bUserConfigTGPmodeRequest     =
            FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARESTATUS, _USER_CONFIG_TGP_MODE, _ENABLE, acpiParams.output.result[0]);
        pPfmreqhndlrData->PFMREQHNDLRACPIData.bPlatformUserConfigTGPSupport =
            FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARESTATUS, _PLAT_USER_CONFIG_TGP_MODE_SUPPORT, _ENABLE, acpiParams.output.result[0]);

        //
        // 1.A. EDPpeak limit update: Handle the legacy way where a VBIOS static limit is applied
        // Update only if in reset state and Reset only if is in enabled state
        //
        if (PFM_REQ_HNDLR_IS_EDPPEAK_UPDATE_REQUIRED(pPfmreqhndlrData->PFMREQHNDLRACPIData.bEDPpeakLimitUpdateRequest,
            pPlatformRequestHandler->controlData.bEDPpeakUpdateEnabled) && !bInit)
        {
           // Update or Reset the EDPp limit if needed
            lclStatus = pfmreqhndlrHandleEdppeakLimitUpdate(pPlatformRequestHandler, pGpu,
                pPfmreqhndlrData->PFMREQHNDLRACPIData.bEDPpeakLimitUpdateRequest);
                if ((NV_OK != lclStatus) &&
                    (NV_ERR_NOT_SUPPORTED != lclStatus))
                {
                    status = lclStatus;
                }
        }

        //
        // 1.B. EDPpeak limit update : Handle platform supplied limit
        //
        if (bQueryEdppRequired)
        {
            NvU32 platformEdppLimit = 0;

            // Query platform for the new EDPp value
            lclStatus = pfmreqhndlrHandlePlatformGetEdppLimit(pPlatformRequestHandler, pGpu, &platformEdppLimit);
            if ((NV_OK != lclStatus) &&
                (NV_ERR_NOT_SUPPORTED != lclStatus))
            {
                status = lclStatus;
            }
            else
            {
                // Cache the EDPp limit from platform request
                pPfmreqhndlrData->PFMREQHNDLRACPIData.platformEdppLimit = platformEdppLimit;
            }

            if (bInit)
            {
                // Defer init time EDPp update after prereq is satisfied
                pPlatformRequestHandler->controlData.edppLimit.bDifferPlatformEdppLimit = NV_TRUE;
            }
            else
            {
                // Followed by apply the platform limit on the GPU
                lclStatus = pfmreqhndlrHandlePlatformEdppLimitUpdate(pPlatformRequestHandler, pGpu,
                                pPfmreqhndlrData->PFMREQHNDLRACPIData.platformEdppLimit);
                if ((NV_OK != lclStatus) &&
                    (NV_ERR_NOT_SUPPORTED != lclStatus))
                {
                    status = lclStatus;
                }
            }
        }

        //
        // 2. User configurable TGP mode (Turbo)  Update
        // Enable only if in reset state and Reset only if in enabled state
        //
        if (PFM_REQ_HNDLR_IS_USER_CONFIG_TGP_MODE_UPDATE_REQUIRED(pPfmreqhndlrData->PFMREQHNDLRACPIData.bUserConfigTGPmodeRequest,
            pPlatformRequestHandler->controlData.bUserConfigTGPmodeEnabled) && !bInit)
        {
            // Enable or Reset the configurable TGP mode  if needed
            lclStatus = pfmreqhndlrHandleUserConfigurableTgpMode(pPlatformRequestHandler, pGpu,
                pPfmreqhndlrData->PFMREQHNDLRACPIData.bUserConfigTGPmodeRequest);
            if ((NV_OK != lclStatus) &&
                (NV_ERR_NOT_SUPPORTED != lclStatus))
            {
                status = lclStatus;
            }
        }

        // 3. Platform static customization for user config TGP can be handled here one time
        if ((pPfmreqhndlrData->PFMREQHNDLRACPIData.bPlatformUserConfigTGPSupport) &&
            (!pPlatformRequestHandler->controlData.bPlatformUserConfigTGPmodeEnabled) &&
            (bInit))
        {
            lclStatus = _pfmreqhndlrHandleUserConfigurableTgpModePlatformCustomization(pPlatformRequestHandler, pGpu);
            if ((NV_OK != lclStatus) &&
                (NV_ERR_NOT_SUPPORTED != lclStatus))
            {
                status = lclStatus;
            }
        }

        if (pPfmreqhndlrData->PFMREQHNDLRACPIData.bSystemParamLimitUpdate)
        {
            timeStamp = _pfmreqhndlrGetTimeStamp(pPlatformRequestHandler);
            _pfmreqhndlrResetCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_TGPU_SENSOR), timeStamp);
            _pfmreqhndlrResetCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_PPMD_SENSOR), timeStamp);

            lclStatus = _pfmreqhndlrUpdateTgpuLimit(pPlatformRequestHandler, pGpu);
            if ((NV_OK == status) && (NV_ERR_NOT_SUPPORTED != lclStatus))
            {
                status = lclStatus;
            }

            if (PFM_REQ_HNDLR_IS_ACPI_VERSION_SW_2X(pPlatformRequestHandler))
            {
                // Run time PPMD update from the system
                lclStatus = _pfmreqhndlrUpdatePpmdLimit(pPlatformRequestHandler, pGpu, NV_FALSE);
                if ((NV_OK == status) && (NV_ERR_NOT_SUPPORTED != lclStatus))
                {
                    status = lclStatus;
                }
            }

            // Reset the system parameter update flag
            pPfmreqhndlrData->PFMREQHNDLRACPIData.bSystemParamLimitUpdate = NV_FALSE;
        }

        if (bPlatformEdpUpdate)
        {
            lclStatus = pfmreqhndlrHandlePlatformSetEdppLimitInfo(pPlatformRequestHandler, pGpu);
            if ((NV_OK != lclStatus) && (NV_ERR_NOT_SUPPORTED != lclStatus))
            {
                status = lclStatus;
            }
        }
    }

    return status;
}

/*!
 * Setup the initial state for the given counter as a supported counter.
 *
 * @param[in]  pPfmreqhndlrData     Pointer to the Sensor Data cache.
 * @param[in]  id           Identifier of counter to be updated.
 * @param[in]  bVolatile    bool indicating if the counter is volatile or not.
 *                          Volatile counters can go stale & will be sampled
 *                          on request for a sampling, whereas it is the
 *                          responsability of the code associated with controlling
 *                          the counter value to update a non-volatile counter.
 * @param[in]  timeStamp    current time stamp in MS used to create a stale
 *                          timestamp if needed to ensure that the first time
 *                          the counter is querried we try to sample it.
 *
 */
static void
_pfmreqhndlrInitSupportedCounter
(
    PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData,
    NvU32            id,
    NvBool           bVolatile,
    NvU32            timeStamp
)
{
    pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bSupported  = NV_TRUE;
    pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].bVolatile   = bVolatile;
    pPfmreqhndlrData->PFMREQHNDLRSensorCache[id].minInterval = PFM_REQ_HNDLR_DEFAULT_COUNTER_HOLD_PERIOD_MS;
    _pfmreqhndlrResetCounter(pPfmreqhndlrData, id, timeStamp);
}

//
// _pfmreqhndlrGetPerfSensorCounters
//
// This function collects the requested counters and returns those that are actually available.
//
NV_STATUS
pfmreqhndlrGetPerfSensorCounters
(
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams,
    NvU16                                                             mapFlags
)
{
    OBJSYS                  *pSys                    = SYS_GET_INSTANCE();
    PlatformRequestHandler  *pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);
    NvU32            counterMask;
    NvU32            counterVal;
    NvU16            idx;

    NV_ASSERT_OR_RETURN(NULL != pPlatformRequestHandler, NV_ERR_NOT_SUPPORTED);

    // is the block Id within range?
    if (pParams->blockId >= PFM_REQ_HNDLR_AVAILABLE_SENSOR_BLOCKS)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // run thru & get each counter specified in the request mask.
    for (idx = 0; idx < PFM_REQ_HNDLR_MAX_SENSORS_IN_BLOCK; ++idx)
    {
        pParams->counterBlock[idx] = 0;
        counterMask                = (1 << idx);

        // are we interested in this counter?
        if (((pParams->countersReq & counterMask) != 0) &&
            ((PerfSensorBlockMap[pParams->blockId][idx] & mapFlags) == mapFlags))
        {
            if (NV_OK == pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler,
                                PFM_REQ_HNDLR_PSR_ID(PerfSensorBlockMap[pParams->blockId][idx]),
                                &counterVal))
            {
                pParams->counterBlock[idx] = counterVal;
                pParams->countersReturned |= counterMask;
            }
        }
    }

    return NV_OK;
}

//
// _pfmreqhndlrIsCacheEntryStale
//
// This function returns a bool indicating if the specified cache entry is stale.
//
static NvBool
_pfmreqhndlrIsCacheEntryStale
(
    PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData,
    NvU32            counterId,
    NvU32            timeStamp
)
{
    NvU32   age;
    NvBool  bStale = NV_FALSE;

    // if the counter is Volatile, check to see if it has aged out.
    if ((pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].bVolatile) &&
        (!pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].bOverridden))
    {
        // calculate the age allowing for wrap.
        age = timeStamp - pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].lastSampled;

        // if the age went negitive, make the age 0.
        if ((NvS32)age < 0)
        {
            age = 0;
        }
        bStale = ((pPfmreqhndlrData->counterHoldPeriod < age) && (pPfmreqhndlrData->PFMREQHNDLRSensorCache[counterId].minInterval < age));
    }
    return  bStale;
}

static NV_STATUS
_pfmreqhndlrSampleSensorLimit
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    NvU32   id,
    NvU32   timeStamp
)
{
    NV_STATUS        status   = NV_ERR_NOT_SUPPORTED;

    if (!PFM_REQ_HNDLR_VALID_LIMIT_ID(id))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // Use ACPI only
    status = _pfmreqhndlrSampleSensorLimit_ACPI(pPlatformRequestHandler, id, timeStamp);

    return status;
}

static NV_STATUS
_pfmreqhndlrSampleSensorLimit_ACPI
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    NvU32   id,
    NvU32   timeStamp
)
{
    PFM_REQ_HNDLR_SENSOR_DATA                   *pPfmreqhndlrData      = &pPlatformRequestHandler->sensorData;
    OBJGPU                            *pGpu          = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS   acpiParams    = {{ 0 }};
    PFM_REQ_HNDLR_PSHAREDATA                     pshareParams  = { 0 };
    NV_STATUS                          status;
    NvU8                               i;

    // all the counters for this case are read in a single block
    acpiParams.ctrl.cmd   = NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHAREPARAMS;
    acpiParams.ctrl.input = pPfmreqhndlrData->PFMREQHNDLRShareParamsAvailMask |
        DRF_DEF(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS, _STATUS_QUERY_TYPE, _CURRENT_LIMITS);

    // exclude counters that are up-to-date or have forced overrides
    for (i = 0; i < PFM_REQ_HNDLR_PSHAREPARAMS_COUNT; i++)
    {
        if (((timeStamp - pPfmreqhndlrData->PFMREQHNDLRSensorCache[PFM_REQ_HNDLR_LIMIT(i)].lastSampled) <
                  pPfmreqhndlrData->PFMREQHNDLRSensorCache[PFM_REQ_HNDLR_LIMIT(i)].minInterval) ||
              pPfmreqhndlrData->PFMREQHNDLRSensorCache[PFM_REQ_HNDLR_LIMIT(i)].bOverridden)
        {
            acpiParams.ctrl.input &= ~NVBIT(i + DRF_BASE(NV0000_CTRL_PFM_REQ_HNDLR_PSHARE_PARAMS_STATUS_TGPU));
        }
    }

    // bail-out if everything is already up-to-date
    if (acpiParams.ctrl.input == 0)
    {
        return NV_OK;
    }

    acpiParams.output.pBuffer  = (NvU8 *)&pshareParams;
    acpiParams.output.bufferSz = sizeof(pshareParams);
    status = pfmreqhndlrCallACPI(pGpu, &acpiParams);
    if (status == NV_OK)
    {
        if (FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS_STATUS, _TGPU, _TRUE, acpiParams.ctrl.input))
        {
            _pfmreqhndlrUpdateCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_TGPU_SENSOR), FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS_STATUS, _TGPU, _TRUE, pshareParams.status), pshareParams.tGpu, timeStamp);
        }
        if (FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS_STATUS, _PPMD, _TRUE, acpiParams.ctrl.input))
        {
            _pfmreqhndlrUpdateCounter(pPfmreqhndlrData, PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_PPMD_SENSOR), FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS_STATUS, _PPMD, _TRUE, pshareParams.status), pshareParams.ppmd, timeStamp);
        }
    }
    return status;
}

/*!
 * Set up PlatformRequestHandler control flags based on the ACPI supported subfunction lists.
 *
 * @param[in] pGpu                  OBJGPU pointer
 * @param[in] pPlatformRequestHandler                  *PlatformRequestHandler pointer
 * @param[in] supportedSubfuncs     NvU32 supported subfunctions list from SBIOS
 *
 * @return  NV_OK    always succeeds
 *
 */
static NV_STATUS
_pfmreqhndlrSetSupportedStatesFromAcpiParams
(
    OBJGPU     *pGpu,
    PlatformRequestHandler
               *pPlatformRequestHandler,
    NvU32       supportedSubfuncs

)
{
    PFM_REQ_HNDLR_SENSOR_DATA    *pPfmreqhndlrData = &(pPlatformRequestHandler->sensorData);

    pPfmreqhndlrData->PFMREQHNDLRACPIData.bPfmReqHndlrSupported =
        ((0 != (DRF_VAL(0000_CTRL_PFM_REQ_HNDLR, _SUPPORTED, _PSHARESTATUS_AVAIL, supportedSubfuncs))) &&
         (0 != (DRF_VAL(0000_CTRL_PFM_REQ_HNDLR, _SUPPORTED, _PSHAREPARAMS_AVAIL, supportedSubfuncs))));

    return NV_OK;
}

/*!
 * Handler routine to platform costumization to user mode TGP
 *
 * @param[in]      pPlatformRequestHandler         *PlatformRequestHandler object pointer
 *
 * @return NV_OK if the update was successful.
 * @return NV_ERR_NOT_SUPPORTED if the POR support is not present
 * @return  bubble up errors triggered by called code
 */
static NV_STATUS
_pfmreqhndlrHandleUserConfigurableTgpModePlatformCustomization
(
    PlatformRequestHandler    *pPlatformRequestHandler,
    OBJGPU                    *pGpu
)
{
    NV_STATUS                                  status      = NV_OK;
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS acpiParams;
    PFM_REQ_HNDLR_PSHAREDATA                   pshareParams;

    portMemSet((void *) &acpiParams, 0, sizeof(NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS));
    portMemSet((void *) &pshareParams, 0, sizeof(PFM_REQ_HNDLR_PSHAREDATA));

    acpiParams.ctrl.cmd   = NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PSHAREPARAMS;
    acpiParams.ctrl.input = DRF_DEF(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS, _STATUS_CTGP, _TRUE) |
                            DRF_DEF(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS, _STATUS_QUERY_TYPE, _CURRENT_LIMITS);
    acpiParams.output.pBuffer = (NvU8*)&pshareParams;
    acpiParams.output.bufferSz = sizeof(pshareParams);
    status = pfmreqhndlrCallACPI(pGpu, &acpiParams);
    if (status != NV_OK)
    {
        DBG_BREAKPOINT();
        status = NV_ERR_NOT_SUPPORTED;
        goto _pfmreqhndlrHandleUserConfigurableTgpModePlatfromCustomization_exit;
    }

    //Set the customization config tgp delta
    if (FLD_TEST_DRF(0000_CTRL_PFM_REQ_HNDLR, _PSHARE_PARAMS_STATUS, _CTGP, _TRUE, pshareParams.status))
    {
        // Cached the platform limit delta in the PlatformRequestHandler ACPI cache
        pPlatformRequestHandler->sensorData.PFMREQHNDLRACPIData.platformLimitDeltamW = pshareParams.ctgp;

        // Config TGP from platform is enabled
        pPlatformRequestHandler->controlData.bPlatformUserConfigTGPmodeEnabled = NV_TRUE;

        NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2_PARAMS   params = { 0 };
        RM_API                                                                 *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        params.ctgpOffsetmW = pshareParams.ctgp;

        pRmApi->Control(pRmApi,
                        pGpu->hInternalClient,
                        pGpu->hInternalSubdevice,
                        NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_CONFIGURE_TURBO_V2,
                        &params,
                        sizeof(params));
    }

_pfmreqhndlrHandleUserConfigurableTgpModePlatfromCustomization_exit:
    return status;
}

/*!
 * Reset Platform Power Mode cache on PlatformRequestHandler init and PlatformRequestHandler state unload
 *
 * @param[in]  pPlatformRequestHandler   *PlatformRequestHandler pointer
 *
 */
static void
_pfmreqhndlrPlatformPowerModeStateReset
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    PFM_REQ_HNDLR_PPM_DATA    *pPpmData = NULL;

    pPpmData =  &(pPlatformRequestHandler->ppmData);

    // Init the PPM Cache Data
    pPpmData->ppmMaskCurrent   = NV0000_CTRL_PFM_REQ_HNDLR_PPM_MASK_INVALID;
    pPpmData->ppmIdxCurrent    = NV0000_CTRL_PFM_REQ_HNDLR_PPM_INDEX_INVALID;
    pPpmData->bWorkItemPending = NV_FALSE;
    pPpmData->ppmIdxRequested  = NV0000_CTRL_PFM_REQ_HNDLR_PPM_INDEX_INVALID;
}

/*!
 * @brief passive worker thread to communcate EDPpeak info to platform.
 *
 * @param[in]   gpuInstance     GPU instance ID
 * @param[in]   pParams         unused
*/
static void
_pfmreqhndlrHandlePlatformSetEdppLimitInfoWorkItem
(
    NvU32  gpuInstance,
    void  *pParams
)
{
    OBJSYS                  *pSys                    = SYS_GET_INSTANCE();
    PlatformRequestHandler  *pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);
    OBJGPU                  *pGpu                    =
        pfmreqhndlrGetGpu(pPlatformRequestHandler);
    PFM_REQ_HNDLR_EDPP_DATA *pEdppLimit              =
        &pPlatformRequestHandler->controlData.edppLimit;
    NvU32   status       = NV_OK;
    NvU32   gpuLockFlag  = GPUS_LOCK_FLAGS_NONE;
    NvBool lockAcquired  = NV_FALSE;
    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS
            acpiParams;
    NV0000_CTRL_PFM_REQ_HNDLR_EDPP_LIMIT_INFO_V1
            edppLimitInfo;

    if (FULL_GPU_SANITY_CHECK(pGpu))
    {
        // Attempt to acquire locks/semaphore
        if (pfmreqhndlrPassiveModeTransition(NV_TRUE, API_LOCK_FLAGS_NONE,
            gpuLockFlag) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to acquire the locks/semaphore!\n");
            goto _pfmreqhndlrHandlePlatformSetEdppLimitInfoWorkItem_exit;
        }
        lockAcquired = NV_TRUE;

        portMemSet((void*)&acpiParams, 0, sizeof(NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS));
        portMemSet((void*)&edppLimitInfo, 0, sizeof(NV0000_CTRL_PFM_REQ_HNDLR_EDPP_LIMIT_INFO_V1));

        edppLimitInfo.ulVersion = NV0000_CTRL_PFM_REQ_HNDLR_EDPP_VERSION_V10;
        edppLimitInfo.limitLast =
            pPlatformRequestHandler->sensorData.PFMREQHNDLRACPIData.platformEdppLimit;

        // Fill in the other EDPpeak data if we received them from GPU successfully
        if (pEdppLimit->status == NV_OK)
        {
            edppLimitInfo.limitMin       = pEdppLimit->edppLimitInfo.limitMin;
            edppLimitInfo.limitRated     = pEdppLimit->edppLimitInfo.limitRated;
            edppLimitInfo.limitMax       = pEdppLimit->edppLimitInfo.limitMax;
            edppLimitInfo.limitCurr      = pEdppLimit->edppLimitInfo.limitCurr;
            edppLimitInfo.limitBattRated = pEdppLimit->edppLimitInfo.limitBattRated;
            edppLimitInfo.limitBattMax   = pEdppLimit->edppLimitInfo.limitBattMax;
        }

        // Call the platform and communicate the EDPp info
        acpiParams.ctrl.cmd = NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_SETEDPPLIMITINFO;

        acpiParams.output.pBuffer = (NvU8*)&edppLimitInfo;
        acpiParams.output.bufferSz = sizeof(edppLimitInfo);

        status = pfmreqhndlrCallACPI(pGpu, &acpiParams);
        if (status != NV_OK)
        {
            DBG_BREAKPOINT();
            goto _pfmreqhndlrHandlePlatformSetEdppLimitInfoWorkItem_exit;
        }

    }

_pfmreqhndlrHandlePlatformSetEdppLimitInfoWorkItem_exit:

    // Make workitem available
    pEdppLimit->bWorkItemPending = NV_FALSE;
    if (lockAcquired)
    {
        // Release locks/semaphore
        pfmreqhndlrPassiveModeTransition(NV_FALSE, API_LOCK_FLAGS_NONE, gpuLockFlag);
    }
    return;
}

/*
 * NOTE. HOSUNGK. These functions are for backwards compatibility with KMD and NvAPI
 * These will be removed after promotion from bugfix_main
 */
NV_STATUS
cliresCtrlCmdSystemGpsCtrl_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_CTRL_PARAMS *pParams
)
{
    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemGpsGetFrmData_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_GET_FRM_DATA_PARAMS *pParams
)
{
    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemGpsSetFrmData_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_SET_FRM_DATA_PARAMS *pParams
)
{
    return NV_OK;
}

NV_STATUS
cliresCtrlCmdSystemGpsCallAcpi_IMPL
(
    RmClientResource *pRmCliRes,
    NV0000_CTRL_SYSTEM_GPS_CALL_ACPI_PARAMS *pParams
)
{
    return NV_OK;
}
