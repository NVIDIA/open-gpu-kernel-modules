/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/* ------------------------ Includes --------------------------------------- */
#include "nvRmReg.h"
#include "gpu/timer/objtmr.h"
#include "platform/platform_request_handler.h"
#include "gpu_mgr/gpu_mgr.h"
#include "core/locks.h"
#include "diagnostics/tracer.h"
#include "gps.h"
#include "platform/platform.h"
#include "rmapi/client_resource.h"

/* ------------------------ Datatypes -------------------------------------- */
/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Defines ---------------------------------------- */
/* ---------------------- Static Function Prototypes ----------------------- */
static NV_STATUS         _pfmreqhndlrSupportExists               (PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu);
static NvBool            _pfmreqhndlrCheckAndGetPM1ForcedOffState(PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu);
static GpuPrereqCallback _pfmreqhndlrPmgrPmuPostLoadPrereqCallback;
static GpuPrereqCallback _pfmreqhndlrThermPmuPostInitPrereqCallback;

/* ---------------------- Extern Function Prototypes ----------------------- */
extern NV_STATUS pfmreqhndlrCallback(OBJGPU *pGpu, OBJTMR *pTmr, void *pVoid);

/* ------------------------ Public Functions  ------------------------------ */

// Constructor
NV_STATUS
pfmreqhndlrConstruct_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    NV_PRINTF(LEVEL_INFO, "\n");
    return NV_OK;
}

/*!
 * @brief   Stores the index of first GPU which supports PLATFORM_REQUEST_HANDLER
 *
 * @param[in]   pPlatformRequestHandler        *PlatformRequestHandler pointer
 *
 * @return  NV_ERR_NOT_SUPPORTED    If GPU does not support PRH
 * @return  NV_OK                   If found a GPU that supports PRH
 */
NV_STATUS
pfmreqhndlrInitGpu
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    pPlatformRequestHandler->dsmVersion = ACPI_DSM_FUNCTION_INVALID;

    OBJGPU   *pGpu     = NULL;
    NvU32     gpuIndex = 0;
    NV_STATUS status   = NV_ERR_NOT_SUPPORTED;
    NvU32     gpuCount;
    NvU32     gpuMask;

    // Initialize index to default (invalid) value.
    pPlatformRequestHandler->pfmreqhndlrSupportedGpuIdx = PFM_REQ_HNDLR_MAX_GPU_SUPPORTED;
    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);

    // Invalid GPU configuration
    if (gpuCount < 1 || gpuCount > PFM_REQ_HNDLR_MAX_GPU_SUPPORTED)
    {
        NV_PRINTF(LEVEL_INFO, "PLATFORM_REQUEST_HANDLER: Cannot support %u GPUs yet\n", gpuCount);
    }
    else
    {
        while (((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL) &&
               (pGpu->pPrereqTracker != NULL))
        {
            // get pfmreqhndlr support option.
            if(testIfDsmSubFunctionEnabled(pGpu, ACPI_DSM_FUNCTION_GPS_2X, GPS_FUNC_SUPPORT) == NV_OK)
            {
                pPlatformRequestHandler->dsmVersion = ACPI_DSM_FUNCTION_GPS_2X;
                status = NV_OK;
            }
            else if(testIfDsmSubFunctionEnabled(pGpu, ACPI_DSM_FUNCTION_GPS, GPS_FUNC_SUPPORT) == NV_OK)
            {
                pPlatformRequestHandler->dsmVersion = ACPI_DSM_FUNCTION_GPS;
                status = NV_OK;
            }

            if (status == NV_OK)
            {
                RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

                // Check if PRH dependent modules have been initialized and in a valid state
                if (pRmApi->Control(pRmApi,
                                    pGpu->hInternalClient,
                                    pGpu->hInternalSubdevice,
                                    NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_DEPENDENCY_CHECK,
                                    NULL,
                                    0) == NV_OK)
                {
                    //
                    // Store the GPU index of the current GPU.
                    // gpumgrGetNextGpu() increments gpuIndex to point to the next
                    // GPU, so need to subtract 1 to get the current GPU index.
                    //
                    pPlatformRequestHandler->pfmreqhndlrSupportedGpuIdx = gpuIndex - 1;
                    break;
                }
            }
        }
    }

    return NV_OK;
}

/*!
 * @brief   Returns the GPU which supports PLATFORM_REQUEST_HANDLER
 *
 * @param[in]  pPlatformRequestHandler   *PlatformRequestHandler pointer
 *
 * @return     pGpu   if at least one GPU supports ACPI_DSM_FUNCTION_PFM_REQ_HNDLR
 * @return     NULL   if no GPU supports ACPI_DSM_FUNCTION_PFM_REQ_HNDLR
 */
POBJGPU
pfmreqhndlrGetGpu
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    OBJGPU *pGpu = NULL;
    NvU32   gpuIndex;
    NvU32   gpuCount;
    NvU32   gpuMask;

    gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);

    // gpumgrGetNextGpu() increments the index, so pass a copy to it.
    gpuIndex = pPlatformRequestHandler->pfmreqhndlrSupportedGpuIdx;

    if (gpuIndex < PFM_REQ_HNDLR_MAX_GPU_SUPPORTED)
    {
        pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex);
    }

    return pGpu;
}

/*!
 * Acquire/release all locks/semaphores before executing passive mode code.
 *
 * @param[in]   bEnter  specify if this call was invoked prior (NV_TRUE) or
 *                      after (NV_FALSE) executing passive mode code
 * @param[in]   APILockFlag  flag for the RM API lock
 * @param[in]   GPULockFlag  flag for RM GPU lock
 *
 * @return  NV_ERR_INVALID_OBJECT   failed to look-up system object
 * @return  propagates an error code of other interfaces called within
 */
NV_STATUS
pfmreqhndlrPassiveModeTransition
(
    NvBool  bEnter,
    NvU32   apiLockFlag,
    NvU32   gpuLockFlag
)
{
    OBJSYS     *pSys            = SYS_GET_INSTANCE();
    NvBool      bReleaseRmSema  = NV_FALSE;
    NvBool      bReleaseApiLock = NV_FALSE;
    NvBool      bReleaseGpuLock = NV_FALSE;
    NV_STATUS   status          = NV_OK;

    NV_ASSERT_OR_RETURN(pSys != NULL, NV_ERR_INVALID_OBJECT);

    if (bEnter)
    {
        if (status == NV_OK)
        {
            status = osCondAcquireRmSema(pSys->pSema);

            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "Error acquiring semaphore!\n");
                DBG_BREAKPOINT();
            }
        }

        if (status == NV_OK)
        {
            status = rmapiLockAcquire(apiLockFlag, RM_LOCK_MODULES_PFM_REQ_HNDLR);

            if (status != NV_OK)
            {
                bReleaseRmSema  = NV_TRUE;

                NV_PRINTF(LEVEL_ERROR, "Error acquiring API lock!\n");
                DBG_BREAKPOINT();
            }
        }

        if (status == NV_OK)
        {
            status = rmGpuLocksAcquire(gpuLockFlag, RM_LOCK_MODULES_PFM_REQ_HNDLR);

            if (status != NV_OK)
            {
                bReleaseApiLock = NV_TRUE;
                bReleaseRmSema  = NV_TRUE;

                NV_PRINTF(LEVEL_ERROR, "Error acquiring GPUs lock!\n");
                DBG_BREAKPOINT();
            }
        }
    }
    else
    {
        bReleaseRmSema  = NV_TRUE;
        bReleaseApiLock = NV_TRUE;
        bReleaseGpuLock = NV_TRUE;
    }

    if (bReleaseGpuLock)
    {
        rmGpuLocksRelease(gpuLockFlag, NULL);
    }

    if (bReleaseApiLock)
    {
        rmapiLockRelease();
    }

    if (bReleaseRmSema)
    {
        osReleaseRmSema(pSys->pSema, NULL);
    }

    return status;
}

/*!
 * @brief Handle PFM_REQ_HNDLR_EVENT_STATUS_CHANGE event.
 *
 * This function is called for PlatformRequestHandler when SBIOS triggers
 * PFM_REQ_HNDLR_EVENT_STATUS_CHANGE event and also when v-Pstate change heppen in RM
 * to update it to SBIOS. This control call will send ACK back to SBIOS if
 * get a call from SBIOS and do further processing required.
 *
 * @param[in]  pPlatformRequestHandler       *PlatformRequestHandler object pointer
 * @param[in]  input      Data used to send in Pcontrol ACPI call
 * @param[in]  bSbiosCall True if we get a call from SBIOS, false if get
 *                        call to update SBIOS about new v-Pstate
 *
 * @return 'NV_OK'
 *     If the command successfully completes.
 * @return 'NV_ERR_NOT_SUPPORTED'
 *     If invalid type of command is requested.
 */
NV_STATUS
pfmreqhndlrPcontrol_IMPL(PlatformRequestHandler *pPlatformRequestHandler, NvU32 input, NvBool bSbiosCall)
{
    OBJGPU                    *pGpu             = NULL;
    PFM_REQ_HNDLR_SENSOR_DATA *pPfmreqhndlrData = NULL;
    NvU32                      indexPstate      = 0;        // Pstate index received by SBIOS
    NvU32                      maxVPstate       = 0;        // Max v-Pstate corresponding to Pstate index
    NvU32                      inputData        = 0;        // Input data sent to SBIOS
    NvU32                      vpStateMapping   = 0;        // v-Pstate mapping corresponding Pstate index
    NvU32                      responseData     = 0;        // Responce data received from SBIOS
    NvU32                      requestData      = 0;        // Request data send to SBIOS
    NV_STATUS                  status           = NV_OK;
    RM_API                    *pRmApi           = NULL;
    NvBool                     bDone            = NV_FALSE;

    if (NULL == pPlatformRequestHandler)
    {
        NV_PRINTF(LEVEL_ERROR, "PlatformRequestHandler pointer is NULL\n");
        DBG_BREAKPOINT();
        return NV_ERR_NOT_SUPPORTED;
    }

    pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    if (NULL == pGpu)
    {
        NV_PRINTF(LEVEL_ERROR, "Gpu pointer is NULL\n");
        DBG_BREAKPOINT();
        return NV_ERR_NOT_SUPPORTED;
    }
    pPfmreqhndlrData        = &(pPlatformRequestHandler->sensorData);
    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    //
    // Setup the initial PFM_REQ_HNDLR_FUNC_PCONTROL call
    //

    //Updating input data before sending request for Pcontrol call
    inputData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _RES, NV_PB_PFM_REQ_HNDLR_PCTRL_RES_CHANGE_EVENT, inputData);
    // Setting reserved bit equal to zero
    inputData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _RESERVED, NV_PB_PFM_REQ_HNDLR_PCTRL_RESERVED_BITS, inputData);
    // Last received v-Pstate limit in input data designated by bits [15:8].
    inputData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _LAST_VPSTATE_LIMIT, pPfmreqhndlrData->PFMREQHNDLRACPIData.prevSbiosVPStateLimit, inputData);
    // Send slowest externally available v-Pstate number in [23:16] of input data.
    // NOTE: Notion of a "Slow" vpstate is not applicable in PSTATES 3.0+, this usage is broken for non pstates 2.x environments.
    inputData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _SLOW_EXT_VPSTATE, pPlatformRequestHandler->vPstateCache.vPstateIdxHighest, inputData);
    // Setting fastest available v-Pstate to 0
    inputData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _FAST_VPSTATE, 0, inputData);

    NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS acpiParams = {0};
    acpiParams.ctrl.cmd        = NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_CMD_PCONTROL;
    acpiParams.ctrl.input      = inputData;
    acpiParams.output.pBuffer  = (NvU8*)&(requestData);
    acpiParams.output.bufferSz = sizeof(requestData);

    bDone = NV_FALSE;

    while (!bDone)
    {
        // make the previously setup PFM_REQ_HNDLR_FUNC_PCONTROL call
        status = pfmreqhndlrCallACPI(pGpu, &acpiParams);

        if (NV_OK != status)
        {
            NV_PRINTF(LEVEL_ERROR, "ACPI call failed. Error : 0x%x\n", status);
            DBG_BREAKPOINT();

            break;
        }
        requestData = acpiParams.output.result[0];

        switch(DRF_VAL(_PB, _PFM_REQ_HNDLR_PCTRL, _REQ, requestData))
        {
            case NV_PB_PFM_REQ_HNDLR_PCTRL_REQ_NO_ACTION:
            {
                // No action is needed, exit the function
                NV_PRINTF(LEVEL_INFO,
                          "No action is required in response to SBIOS event PCONTROL\n");
                bDone = NV_TRUE;

                break;
            }
            case NV_PB_PFM_REQ_HNDLR_PCTRL_REQ_VPSTATE_INFO:
            {
                //
                // Setup a response PFM_REQ_HNDLR_FUNC_PCONTROL call to return the VP-State range
                //
                NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING_PARAMS params = {0};
                params.pStateIdx = DRF_VAL(_PB, _PFM_REQ_HNDLR_PCTRL, _INDEX_PSTATE, requestData);

                status = pRmApi->Control(pRmApi,
                                         pGpu->hInternalClient,
                                         pGpu->hInternalSubdevice,
                                         NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_MAPPING,
                                         &params,
                                         sizeof(params));
                if (status == NV_OK)
                {
                    vpStateMapping = params.vPstateIdxMapping;
                }

                // Send response for v-Pstate information in response type.
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _RES, NV_PB_PFM_REQ_HNDLR_PCTRL_RES_VPSTATE_INFO, responseData);
                // Setting reserved bit equal to zero
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _RESERVED, NV_PB_PFM_REQ_HNDLR_PCTRL_RESERVED_BITS, responseData);
                // Send Pstate index received from previous return value of Pstate index in [15:8] of input data.
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _INDEX_PSTATE, indexPstate, responseData);
                // Send slowest externally available v-Pstate number in [23:16] of input data.
                // NOTE: Notion of a "Slow" vpstate is not applicable in PSTATES 3.0+, this usage is broken for non pstates 2.x environments.
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _SLOW_EXT_VPSTATE, pPlatformRequestHandler->vPstateCache.vPstateIdxHighest, responseData);
                // Send v-Pstate mapping for indexPstate in [31:24] of input data.
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _MAPPING_VPSTATE, vpStateMapping, responseData);

                acpiParams.ctrl.input      = responseData;
                requestData                = 0;
                acpiParams.output.pBuffer  = (NvU8*)&(requestData);
                acpiParams.output.bufferSz = sizeof(requestData);

                // call will actually be made at the top of the while loop in the next pass.
                break;
            }
            case NV_PB_PFM_REQ_HNDLR_PCTRL_REQ_VPSTATE_SET:
            {
                maxVPstate = DRF_VAL(_PB, _PFM_REQ_HNDLR_PCTRL, _MAX_VPSTATE_LEVEL, requestData);

                NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE_PARAMS params = {0};
                params.vPstateIdx = maxVPstate;

                pRmApi->Control(pRmApi,
                                pGpu->hInternalClient,
                                pGpu->hInternalSubdevice,
                                NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_SET_VPSTATE,
                                &params,
                                sizeof(params));

                //
                // Setup a response PFM_REQ_HNDLR_FUNC_PCONTROL call to indicate we have changed the current VP-State
                //

                // Send response for v-Pstate information in response type
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _RES, NV_PB_PFM_REQ_HNDLR_PCTRL_RES_VPSTATE_SET, responseData);
                // Setting reserved bit equal to zero
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _RESERVED, NV_PB_PFM_REQ_HNDLR_PCTRL_RESERVED_BITS, responseData);
                // Last received v-Pstate limit in input data designated by bits [15:8].
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _LAST_VPSTATE_LIMIT, pPfmreqhndlrData->PFMREQHNDLRACPIData.prevSbiosVPStateLimit, responseData);
                // Return slowest of input data designated by bits [23:16].
                // NOTE: Notion of a "Slow" vpstate is not applicable in PSTATES 3.0+, this usage is broken for non pstates 2.x environments.
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _SLOW_EXT_VPSTATE, pPlatformRequestHandler->vPstateCache.vPstateIdxHighest, responseData);
                // Fastest available v-Pstate of input data designated by bits [31:24] which is 0.
                responseData = FLD_SET_DRF_NUM (_PB, _PFM_REQ_HNDLR_PCTRL, _FAST_VPSTATE, 0, responseData);

                pPfmreqhndlrData->PFMREQHNDLRACPIData.prevSbiosVPStateLimit
                    = maxVPstate;
                acpiParams.ctrl.input       = responseData;
                requestData                 = 0;
                acpiParams.output.pBuffer   = (NvU8*)&(requestData);
                acpiParams.output.bufferSz  = sizeof(requestData);

                // call will actually be made at the top of the while loop in the next pass.
                break;
            }
            default:
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Received a wrong request type from SBIOS, which is not supported\n");
                status =  NV_ERR_NOT_SUPPORTED;

                bDone = NV_TRUE;

                break;
            }
        }
    }

    return status;
}
/* ---------------------- Static Functions --------------------------------- */

NvBool
pfmreqhndlrIsInitialized_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    NV_ASSERT(pPlatformRequestHandler);

    return pPlatformRequestHandler->controlData.bStateInitialized;
}

/*!
 * @brief   Initializes the PlatformRequestHandler control SW state.
 *
 * @param[in]   pPlatformRequestHandler    *PlatformRequestHandler pointer
 *
 * @return  NV_ERR_INVALID_STATE    failed to look-up required object pointers
 * @return  NV_ERR_NOT_SUPPORTED    system doesn't support requested PlatformRequestHandler variant
 * @return  NV_OK                   PlatformRequestHandler was successfully initialized
 */
NV_STATUS
pfmreqhndlrStateInit_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    NV_STATUS  status = NV_ERR_INVALID_STATE;
    OBJGPU    *pGpu   = NULL;
    RM_API    *pRmApi = NULL;

    if (pPlatformRequestHandler == NULL)
    {
          NV_PRINTF(LEVEL_ERROR, "PlatformRequestHandler pointer is NULL\n");
          DBG_BREAKPOINT();
          return NV_ERR_NOT_SUPPORTED;
    }

    if (pfmreqhndlrIsInitialized(pPlatformRequestHandler))
    {
        // Skip if it has been intialized already
        status = NV_OK;
        goto pfmreqhndlrStateInit_IMPL_exit;
    }

    status = pfmreqhndlrInitGpu(pPlatformRequestHandler);
    if (status != NV_OK)
    {
        goto pfmreqhndlrStateInit_IMPL_exit;
    }

    // Validate we have everything.
    pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);

    if (pGpu == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        goto pfmreqhndlrStateInit_IMPL_exit;
    }
    pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    // Init state varibles.
    pPlatformRequestHandler->controlData.bTGPUOverrideRequired             = NV_TRUE;
    pPlatformRequestHandler->controlData.bEDPpeakUpdateEnabled             = NV_FALSE;
    pPlatformRequestHandler->controlData.bUserConfigTGPmodeEnabled         = NV_FALSE;
    pPlatformRequestHandler->controlData.bPlatformUserConfigTGPmodeEnabled = NV_FALSE;

    // Initialize FRL reporting data
    pPlatformRequestHandler->frmData.nextSampleNumber = 0;

    // Check if PlatformRequestHandler is supported through ACPI
    if (_pfmreqhndlrSupportExists(pPlatformRequestHandler, pGpu) == NV_OK)
    {
        NV2080_CTRL_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE_PARAMS ctrlParams = { 0 };

        pPlatformRequestHandler->setProperty(pPlatformRequestHandler,
            PDB_PROP_PFMREQHNDLR_SUPPORTED, NV_TRUE);

        ctrlParams.bIsSysCtrlSupported = NV_TRUE;

        status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_SMBPBI_PFM_REQ_HNDLR_CAP_UPDATE,
                             &ctrlParams,
                             sizeof(ctrlParams));
        if (status == NV_OK)
        {
            pPlatformRequestHandler->setProperty(pPlatformRequestHandler,
                PDB_PROP_PFMREQHNDLR_SYS_CONTROL_SUPPORTED, NV_TRUE);

            pPlatformRequestHandler->setProperty(pPlatformRequestHandler,
                PDB_PROP_PFMREQHNDLR_IS_PLATFORM_LEGACY, ctrlParams.bIsPlatformLegacy);
        }
    }
    else
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto pfmreqhndlrStateInit_IMPL_exit;
    }

    // Initialize the sensors based on ACPI
    pfmreqhndlrInitSensors(pPlatformRequestHandler);

    pPlatformRequestHandler->controlData.bStateInitialized = NV_TRUE;

    // Check and get PM1ForcedOff state
    pPlatformRequestHandler->controlData.bPM1ForcedOff =
        _pfmreqhndlrCheckAndGetPM1ForcedOffState(pPlatformRequestHandler, pGpu);

    // Cache VPstate PS 20 Support and Highest VPstate Idx
    // Set the load sync with PMGR
    NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO_PARAMS params = { 0 };

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_VPSTATE_INFO,
                             &params,
                             sizeof(params));
    if (status == NV_OK)
    {
        pPlatformRequestHandler->vPstateCache.bVpsPs20Supported = params.bVpsPs20Supported;
        pPlatformRequestHandler->vPstateCache.vPstateIdxHighest = params.vPstateIdxHighest;
    }
    else
    {
        // Set the index to invalid incase of error
        pPlatformRequestHandler->vPstateCache.bVpsPs20Supported = NV_FALSE;
        pPlatformRequestHandler->vPstateCache.vPstateIdxHighest = PFM_REQ_HNDLR_VPSTATE_INDEX_INVALID;
    }

    // Set the load sync with PMGR
    NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS ctrlParams = { 0 };

    ctrlParams.flags = NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_PMGR_LOAD;

    status = pRmApi->Control(pRmApi,
                 pGpu->hInternalClient,
                 pGpu->hInternalSubdevice,
                 NV2080_CTRL_CMD_INTERNAL_PMGR_PFM_REQ_HNDLR_STATE_LOAD_SYNC,
                 &ctrlParams,
                 sizeof(ctrlParams));
    if (status != NV_OK)
    {
        goto pfmreqhndlrStateInit_IMPL_exit;
    }

    // 2X only set the init sync with THERM
    if (PFM_REQ_HNDLR_IS_ACPI_VERSION_SW_2X(pPlatformRequestHandler))
    {
        ctrlParams.flags = NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_THERM_INIT;

        status = pRmApi->Control(pRmApi,
                    pGpu->hInternalClient,
                    pGpu->hInternalSubdevice,
                    NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_STATE_INIT_SYNC,
                    &ctrlParams,
                    sizeof(ctrlParams));
        if (status != NV_OK)
        {
            goto pfmreqhndlrStateInit_IMPL_exit;
        }
    }

pfmreqhndlrStateInit_IMPL_exit:
    return status;
}

/*!
 * @brief deallocates PlatformRequestHandler data.
 *
 * frees up the memory from PlatformRequestHandler global data
 * dynamically allocated during init.
 *
 * @param[in]   pPlatformRequestHandler    *PlatformRequestHandler object pointer
 */
void
pfmreqhndlrStateDestroy_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    NV_ASSERT(pPlatformRequestHandler);

    pPlatformRequestHandler->controlData.bStateInitialized = NV_FALSE;
}

NV_STATUS
pfmreqhndlrStateLoad_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    OBJGPU    *pGpu          = NULL;
    NvBool     bPM1Available = NV_FALSE;
    NvTemp     targetTemp    = PFM_REQ_HNDLR_TEMP_0_C;
    NvU32      data32        = 0;
    RM_API    *pRmApi        = NULL;
    NV_STATUS  status        = NV_OK;

    NV_ASSERT(pPlatformRequestHandler);

    // Validate we have everything.
    pGpu = pfmreqhndlrGetGpu(pPlatformRequestHandler);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }
    if (!pPlatformRequestHandler->getProperty(pPlatformRequestHandler, PDB_PROP_PFMREQHNDLR_SUPPORTED))
    {
        return NV_OK;
    }

    if (!PFM_REQ_HNDLR_IS_ACPI_VERSION_SW_2X(pPlatformRequestHandler))
    {
        // Find default Temp for Thermal Controller from SBIOS.
        if (NV_OK == pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler,
                                                PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_TGPU_SENSOR),
                                                &data32))
        {
            targetTemp = NV_TYPES_CELSIUS_TO_NV_TEMP(data32);
        }

        if (targetTemp != PFM_REQ_HNDLR_TEMP_0_C)
        {
            if (pPlatformRequestHandler->controlData.bTGPUOverrideRequired)
            {
                NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS  params = { 0 };
                params.targetTemp = targetTemp;
                pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

                status = pRmApi->Control(pRmApi,
                                         pGpu->hInternalClient,
                                         pGpu->hInternalSubdevice,
                                         NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT,
                                         &params,
                                         sizeof(params));

                if (status == NV_OK)
                {
                    pPlatformRequestHandler->controlData.bTGPUOverrideRequired = NV_FALSE;
                }
                else
                {
                    NV_ASSERT_FAILED("PRH failed to update thermal limit!");

                }
            }

            if (!pPlatformRequestHandler->controlData.bPM1ForcedOff)
            {
                if (pfmreqhndlrHandleCheckPM1Available(pPlatformRequestHandler, pGpu, &bPM1Available) == NV_OK)
                {
                    pfmreqhndlrUpdatePerfCounter(pPlatformRequestHandler, PFM_REQ_HNDLR_PM1_STATE_AVAIL, bPM1Available);
                }
            }
            else
            {
                pfmreqhndlrUpdatePerfCounter(pPlatformRequestHandler, PFM_REQ_HNDLR_PM1_STATE_AVAIL, NV_FALSE);
            }
        }
    }

    return NV_OK;
}

NV_STATUS
pfmreqhndlrStateUnload_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler
)
{
    return NV_OK;
}

NV_STATUS
pfmreqhndlrStateSync_IMPL
(
    PlatformRequestHandler *pPlatformRequestHandler,
    OBJGPU                 *pGpu,
    NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS
                           *pParams
)
{
    NV_STATUS  status = NV_OK;

    switch(pParams->flags)
    {
        case NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_PMGR_LOAD:
        {
            status = _pfmreqhndlrPmgrPmuPostLoadPrereqCallback(pGpu, NV_TRUE);
            break;
        }
        case NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_THERM_INIT:
        {
            status = _pfmreqhndlrThermPmuPostInitPrereqCallback(pGpu, NV_TRUE);
            break;
        }
        case NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_SMBPBI_OP_CLEAR:
        {
            status = pfmreqhndlrOperatingLimitUpdate(pPlatformRequestHandler, pParams->syncData.data.smbpbi.sensorId,
                0, NV_FALSE);
            break;
        }
        case NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_FLAGS_SMBPBI_OP_SET:
        {
            status = pfmreqhndlrOperatingLimitUpdate(pPlatformRequestHandler, pParams->syncData.data.smbpbi.sensorId,
                pParams->syncData.data.smbpbi.limit, NV_TRUE);
            break;
        }
        default:
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    return status;
}

/*!
 * @brief   Checks if ACPI supports calls necessary for the PLATFORM_REQUEST_HANDLER
 *
 * @param[in]   pPlatformRequestHandler    *PlatformRequestHandler pointer
 * @param[in]   pGpu    OBJGPU pointer
 *
 * @return  NV_ERR_NOT_SUPPORTED    ACPI does not provide support for PLATFORM_REQUEST_HANDLER
 * @return  NV_OK                   otherwise
 */
static NV_STATUS
_pfmreqhndlrSupportExists
(
    PlatformRequestHandler *pPlatformRequestHandler,
    OBJGPU *pGpu
)
{
    NvU16      outDataSize  = 0U;
    NV_STATUS  status       = NV_OK;
    NvU8  supportFuncs[MAX_DSM_SUPPORTED_FUNCS_RTN_LEN];
    NvU8  i;

    if (pPlatformRequestHandler->dsmVersion == ACPI_DSM_FUNCTION_INVALID)
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto _pfmreqhndlrSupportExists_exit;
    }

    portMemSet(supportFuncs, 0, sizeof(supportFuncs));
    outDataSize = sizeof(supportFuncs);


    if (osCallACPI_DSM(pGpu,
                       pPlatformRequestHandler->dsmVersion,
                       GPS_FUNC_SUPPORT,
                       (NvU32 *)&supportFuncs,
                       &outDataSize) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "GPS FUNC_SUPPORT is not supported on this Platform, Failing ACPI-GPS subfunction 0x%x.\n",
                  GPS_FUNC_SUPPORT);
        status = NV_ERR_NOT_SUPPORTED;
        goto _pfmreqhndlrSupportExists_exit;
    }
    else
    {
        for (i = 0; i < MAX_DSM_SUPPORTED_FUNCS_RTN_LEN; i++)
        {
            if (supportFuncs[i] != 0)
            {
                // Atleast one subfunction is enabled for PRH, platform support exists for PRH
                status = NV_OK;
                goto _pfmreqhndlrSupportExists_exit;
            }
        }

        // None of the subfunctions are enabled for PRH, no platform support exists for PRH
        NV_PRINTF(LEVEL_INFO,
                  "PRH : This system doesn't support any ACPI-GPS-subfunctions.\n");
        status = NV_ERR_NOT_SUPPORTED;
        goto _pfmreqhndlrSupportExists_exit;
    }

_pfmreqhndlrSupportExists_exit:
    return status;
}

/*!
 * @brief   Check and get PM1ForcedOff state
 *
 * @param[in]    pPlatformRequestHandler    *PlatformRequestHandler pointer
 * @param[in]    pGpu                        OBJGPU pointer
 *
 * @return       NV_TRUE   if PM1ForcedOff state is set
 * @return       NV_FALSE  if PM1ForcedOff state is not set
 */
NvBool
_pfmreqhndlrCheckAndGetPM1ForcedOffState
(
    PlatformRequestHandler  *pPlatformRequestHandler,
    OBJGPU                  *pGpu
)
{
    RM_API    *pRmApi     = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NvBool     bForcedOff = NV_FALSE; // Return NV_FALSE to match Legacy return values
    NV_STATUS  status     = NV_ERR_GENERIC;

    NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE_PARAMS params = { 0 };
    params.flag = NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_GET_PM1_FORCED_OFF_STATUS;

    status = pRmApi->Control(pRmApi,
                             pGpu->hInternalClient,
                             pGpu->hInternalSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_PERF_PFM_REQ_HNDLR_GET_PM1_STATE,
                             &params,
                             sizeof(params));

    if (status == NV_OK)
    {
        bForcedOff = params.bStatus;
    }

    return bForcedOff;

}

/*!
 * @copydoc GpuPrereqCallback
 */
static NV_STATUS
_pfmreqhndlrThermPmuPostInitPrereqCallback
(
    OBJGPU *pGpu,
    NvBool  bSatisfied
)
{
    OBJSYS                 *pSys                    = SYS_GET_INSTANCE();
    PlatformRequestHandler *pPlatformRequestHandler = SYS_GET_PFM_REQ_HNDLR(pSys);
    NV_STATUS               status                  = NV_OK;
    NvU32                   data32                  = 0;
    NvTemp                  targetTemp;
    NvBool                  bPM1Available           = NV_FALSE;
    RM_API                 *pRmApi                  = GPU_GET_PHYSICAL_RMAPI(pGpu);
    //
    // bSatisfied handling:
    // Only needs to be applied after THERM post pmu init bSatisfied = NV_TRUE
    // No need to revert the settings since it is a client applied limit
    // so the bSatisfied = NV_FALSE is not required to be handled.
    //
    // satisfy()/retract () handling: is not supported for this callback since it
    // depends on prereq _THERM_PMU_LOADED.
    //
    if (bSatisfied)
    {
        // Find default Temp for Thermal Controller from SBIOS.
        status = pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler, PFM_REQ_HNDLR_LIMIT(PFM_REQ_HNDLR_TGPU_SENSOR),
                                             &data32);
        if (status == NV_OK)
        {
            // Overrride with system Temp
            targetTemp = NV_TYPES_CELSIUS_TO_NV_TEMP(data32);
        }
        else
        {
            // Override value is set to 0C
            targetTemp = PFM_REQ_HNDLR_TEMP_0_C;

            // Fall back to legacy implementation
            status = NV_OK;
        }
        if (targetTemp != PFM_REQ_HNDLR_TEMP_0_C)
        {
            if (pPlatformRequestHandler->controlData.bTGPUOverrideRequired)
            {
                NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT_PARAMS  params = { 0 };
                params.targetTemp = targetTemp;

                status = pRmApi->Control(pRmApi,
                                         pGpu->hInternalClient,
                                         pGpu->hInternalSubdevice,
                                         NV2080_CTRL_CMD_INTERNAL_THERM_PFM_REQ_HNDLR_UPDATE_TGPU_LIMIT,
                                         &params,
                                         sizeof(params));

                if (status == NV_OK)
                {
                    pPlatformRequestHandler->controlData.bTGPUOverrideRequired = NV_FALSE;
                }
                else
                {
                    NV_ASSERT_FAILED("PRH failed to update thermal limit!");

                // Fall back to legacy implementation
                    status = NV_OK;
                }
            }

            if (!pPlatformRequestHandler->controlData.bPM1ForcedOff)
            {
                if (pfmreqhndlrHandleCheckPM1Available(pPlatformRequestHandler, pGpu, &bPM1Available) == NV_OK)
                {
                    pfmreqhndlrUpdatePerfCounter(pPlatformRequestHandler, PFM_REQ_HNDLR_PM1_STATE_AVAIL, bPM1Available);
                }
                else
                {
                    pfmreqhndlrUpdatePerfCounter(pPlatformRequestHandler, PFM_REQ_HNDLR_PM1_STATE_AVAIL, NV_FALSE);
                }
            }
            else
            {
                pfmreqhndlrUpdatePerfCounter(pPlatformRequestHandler, PFM_REQ_HNDLR_PM1_STATE_AVAIL, NV_FALSE);
            }
        }
    }

    return status;
}

/*!
 * @copydoc GpuPrereqCallback
 */
NV_STATUS
_pfmreqhndlrPmgrPmuPostLoadPrereqCallback
(
    OBJGPU *pGpu,
    NvBool  bSatisfied
)
{
    OBJSYS    *pSys   = SYS_GET_INSTANCE();
    PlatformRequestHandler
              *pPlatformRequestHandler
                      = SYS_GET_PFM_REQ_HNDLR(pSys);
    NV_STATUS  status = NV_OK;
    NV_STATUS  lcstatus;

    //
    // bSatisfied handling:
    // Only needs to be applied after PMGR post load bSatisfied = NV_TRUE
    // No need to revert the settings since it is a client applied limit
    // so the bSatisfied = NV_FALSE is not required to be handled.
    //
    // satisfy()/retract () handling: is not supported for this callback since it
    // depends on prereq _PMGR_PMU_LOADED .
    //
    if (bSatisfied)
    {
        //
        // EDPpeak event update, at this point SBIOS requested state should match
        // with current control state if not trigger an update.
        //
        if (pPlatformRequestHandler->sensorData.PFMREQHNDLRACPIData.bEDPpeakLimitUpdateRequest !=
            pPlatformRequestHandler->controlData.bEDPpeakUpdateEnabled)
        {
            // Update or Reset the EDPp limit if needed
            lcstatus = pfmreqhndlrHandleEdppeakLimitUpdate(pPlatformRequestHandler, pGpu,
                        pPlatformRequestHandler->sensorData.PFMREQHNDLRACPIData.bEDPpeakLimitUpdateRequest);
            if ((lcstatus != NV_OK) && (lcstatus != NV_ERR_NOT_SUPPORTED))
            {
                NV_ASSERT_OK_FAILED("Failed to apply the EDPpeak limit from system", status);
                status = lcstatus;
                goto _pfmreqhndlrPmgrPmuPostLoadPrereqCallback_exit;
            }
        }

        //
        // User configurable TGP mode (Turbo) event update, at this point SBIOS requested
        // state should match with current control state if not trigger an update.
        //
        if (pPlatformRequestHandler->sensorData.PFMREQHNDLRACPIData.bUserConfigTGPmodeRequest !=
            pPlatformRequestHandler->controlData.bUserConfigTGPmodeEnabled)
        {
            // Enable or Reset the use configurable TGP mode (TGP Turbo mode) if needed.
            lcstatus = pfmreqhndlrHandleUserConfigurableTgpMode(pPlatformRequestHandler, pGpu,
                        pPlatformRequestHandler->sensorData.PFMREQHNDLRACPIData.bUserConfigTGPmodeRequest);
            if ((lcstatus != NV_OK) && (lcstatus != NV_ERR_NOT_SUPPORTED))
            {
                NV_ASSERT_OK_FAILED("Failed to update user configurable TGP (Turbo) mode from system",
                    status);
                status = lcstatus;
                goto _pfmreqhndlrPmgrPmuPostLoadPrereqCallback_exit;
            }
        }

        //
        // Platform EDPp limit if differed apply it after prereq is satisfied
        //
        if (pPlatformRequestHandler->controlData.edppLimit.bDifferPlatformEdppLimit)
        {
            lcstatus = pfmreqhndlrHandlePlatformEdppLimitUpdate(pPlatformRequestHandler, pGpu,
                pPlatformRequestHandler->sensorData.PFMREQHNDLRACPIData.platformEdppLimit);
            if ((NV_OK != lcstatus) &&
                (NV_ERR_NOT_SUPPORTED != lcstatus))
            {
                NV_ASSERT_OK_FAILED("Failed to update platform EDPpeak limit",
                    status);
                status = lcstatus;
                goto _pfmreqhndlrPmgrPmuPostLoadPrereqCallback_exit;
            }

            // Reset the differ update flag
            pPlatformRequestHandler->controlData.edppLimit.bDifferPlatformEdppLimit = NV_FALSE;
        }
    }

_pfmreqhndlrPmgrPmuPostLoadPrereqCallback_exit:
    return status;
}

