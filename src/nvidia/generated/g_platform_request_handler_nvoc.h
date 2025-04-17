
#ifndef _G_PLATFORM_REQUEST_HANDLER_NVOC_H_
#define _G_PLATFORM_REQUEST_HANDLER_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2015-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#pragma once
#include "g_platform_request_handler_nvoc.h"

#ifndef PLATFORM_REQUEST_HANDLER_H
#define PLATFORM_REQUEST_HANDLER_H

/*!
 * @file
 * @brief Provides definitions for PlatformRequestHandler data structures and interfaces.
 *
 * Defines and structures used for the PlatformRequestHandler Object.
 * PlatformRequestHandler handles ACPI events from SBIOS within the Resource Manager.
 * PlatformRequestHandler is a child of OBJSYS object.
 */

/* ------------------------ Includes --------------------------------------- */
#include "core/core.h"
#include "nvoc/object.h"
#include "platform/platform_request_handler_utils.h"
#include "ctrl/ctrl0000/ctrl0000system.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"
#include "nvfixedtypes.h"

/* ------------------------ Macros ----------------------------------------- */
//
// Macro to check if SW ACPI version 2X or not
//
#define PFM_REQ_HNDLR_IS_ACPI_VERSION_SW_2X(pPlatformRequestHandler)                                     \
    (pPlatformRequestHandler->sensorData.PFMREQHNDLRACPIData.acpiVersionSw ==                            \
     NV0000_CTRL_PFM_REQ_HNDLR_ACPI_REVISION_SW_2X)

#define PFM_REQ_HNDLR_MAX_SENSORS_IN_BLOCK    (32U)

#define PFM_REQ_HNDLR_MAX_GPU_SUPPORTED        (4U)

// The following macros are used to attach flags to the sensor IDs in the
// Sensor block map and to isolate the IDs from the Sensor block map.
#define PFM_REQ_HNDLR_PSR_PUB_TAG             (0x00001000UL)
#define PFM_REQ_HNDLR_PSR_ID_MASK             (0x00000FFFUL)
#define PFM_REQ_HNDLR_PSR_PUB_ENTRY(x)        ((x) | PFM_REQ_HNDLR_PSR_PUB_TAG)
#define PFM_REQ_HNDLR_PSR_ID(x)               ((x) & PFM_REQ_HNDLR_PSR_ID_MASK)
#define PFM_REQ_HNDLR_CURR_VALUE_PUB_ENTRY(s) PFM_REQ_HNDLR_PSR_PUB_ENTRY(PFM_REQ_HNDLR_CURR_VALUE(s))

// PlatformRequestHandler Counter IDs (block independent)
#define PFM_REQ_HNDLR_TGPU_SENSOR             NV0000_CTRL_SYSTEM_PARAM_TGPU
#define PFM_REQ_HNDLR_CTGP_SENSOR             NV0000_CTRL_SYSTEM_PARAM_CTGP
#define PFM_REQ_HNDLR_PPMD_SENSOR             NV0000_CTRL_SYSTEM_PARAM_PPMD
#define PFM_REQ_HNDLR_PSHAREPARAMS_COUNT      (9U)
#define PFM_REQ_HNDLR_CURR_BASE               (2U)
#define PFM_REQ_HNDLR_CURR_VALUE(s)           (PFM_REQ_HNDLR_CURR_BASE + ((s)%PFM_REQ_HNDLR_PSHAREPARAMS_COUNT))
#define PFM_REQ_HNDLR_LIMIT_BASE              (PFM_REQ_HNDLR_CURR_BASE + PFM_REQ_HNDLR_PSHAREPARAMS_COUNT)
#define PFM_REQ_HNDLR_LIMIT(s)                (PFM_REQ_HNDLR_LIMIT_BASE + ((s)%PFM_REQ_HNDLR_PSHAREPARAMS_COUNT))
#define PFM_REQ_HNDLR_PERIOD_BASE             (PFM_REQ_HNDLR_LIMIT_BASE + PFM_REQ_HNDLR_PSHAREPARAMS_COUNT)
#define PFM_REQ_HNDLR_PERIOD(s)               (PFM_REQ_HNDLR_PERIOD_BASE + ((s)%PFM_REQ_HNDLR_PSHAREPARAMS_COUNT))
#define PFM_REQ_HNDLR_VALID_SENSOR_ID(b,v)    (((b) <= (v)) && ((v) < (b+PFM_REQ_HNDLR_PSHAREPARAMS_COUNT)))
#define PFM_REQ_HNDLR_VALID_VALUE_ID(v)       PFM_REQ_HNDLR_VALID_SENSOR_ID(PFM_REQ_HNDLR_CURR_BASE,v)
#define PFM_REQ_HNDLR_VALID_LIMIT_ID(v)       PFM_REQ_HNDLR_VALID_SENSOR_ID(PFM_REQ_HNDLR_LIMIT_BASE,v)
#define PFM_REQ_HNDLR_AVAIL_SENSOR_MSK        (23U)
#define PFM_REQ_HNDLR_TC_ENABLE               (71U)
#define PFM_REQ_HNDLR_PM1_STATE_AVAIL         (74U)
#define PFM_REQ_HNDLR_TDP_IDX                 (96U)
#define PFM_REQ_HNDLR_VPS_PS20_SUPPORT        (97U)
#define PFM_REQ_HNDLR_RESERVED_COUNTER        (100U) // This should be the last counter, update as needed.

#define PFM_REQ_HNDLR_NUM_COUNTERS            (PFM_REQ_HNDLR_RESERVED_COUNTER + 1)

#define PFM_REQ_HNDLR_DEFAULT_COUNTER_HOLD_PERIOD_MS   (20U)

//
// PRH internal handling of a temp 0C from a platform request.
//
#define PFM_REQ_HNDLR_TEMP_0_C                 NV_TYPES_CELSIUS_TO_NV_TEMP(0)

//
// PRH handling for an invalid VP state index.
//
#define PFM_REQ_HNDLR_VPSTATE_INDEX_INVALID    NV_U8_MAX

// Header to sensor structure
typedef struct
{
    NvU32   status;
    NvU32   ulVersion;

    NvU32   tGpu;

    // Reserved legacy fields, do not use!.
    NvU32   rsvd[6];
    NvU32   ctgp;
    NvU32   ppmd;
} PFM_REQ_HNDLR_PSHAREDATA, *PPFM_REQ_HNDLR_PSHAREDATA;

// Single sensor block
typedef struct
{
    NvBool  bSupported;                 // indicates if counter is supported on the current system.
    NvBool  bVolatile;                  // indicates if counter is volatile or non-volatile.
    NvBool  bInvalid;                   // indicates if the counter is valid.
    NvBool  bOverridden;                // indicates if counter has been overridden by the user.
    NvU32   value;                      // value of the counter.
    NvU32   lastSampled;                // the last time each sample was collected.
    NvU32   minInterval;                // minimum time between samples
} PFM_REQ_HNDLR_SENSOR_COUNTER;

typedef struct
{
    // driver set values
    NvBool      bPfmReqHndlrSupported;          // reflects if all the required ACPI commands for PlatformRequestHandler are supported.

    // ACPI set values.
    NvBool      bSystemParamLimitUpdate;        // reflects SBIOS current value for new update limits being available.
    NvBool      bEDPpeakLimitUpdateRequest;     // reflects SBIOS current request for EDPpeak limit update event.
    NvBool      bUserConfigTGPmodeRequest;      // reflects SBIOS current request for User Configurable TGP mode aka Turbo mode update event.
    NvBool      bPlatformUserConfigTGPSupport;  // reflects SBIOS static requests to override power delta for User Configurable TGP mode
    NvU32       platformLimitDeltamW;           // Cached limit from platform custimization
    NvU32       prevSbiosVPStateLimit;          // reflects previous VPState requested to be set by SBIOS.
    NvU32       platformEdppLimit;              // Cached last EDPp limit request from platform

    NvU32       acpiVersionSw;                  // mapping between spec and supported sw state
} PFM_REQ_HNDLR_ACPI_CACHE;

/*!
 * Data cache for VPstate Info
 */
typedef struct
{
    NvBool bVpsPs20Supported;   // reflects VPStates PS 20 Support
    NvU32  vPstateIdxHighest;   // reflects the highest VPstate Idx from VBIOS
} PFM_REQ_HNDLR_VPSTATE_CACHE;

/*!
 * Data cache for PlatformRequestHandler sensor provider interface
 */
typedef struct
{
    NvU32                         version;                    // SBIOS reported version of PlatformRequestHandler support
    NvU32                         counterHoldPeriod;          // The period of time a volatile counter will be considered valid
    PFM_REQ_HNDLR_SENSOR_COUNTER  PFMREQHNDLRSensorCache[PFM_REQ_HNDLR_NUM_COUNTERS];
    PFM_REQ_HNDLR_ACPI_CACHE      PFMREQHNDLRACPIData;
    NvU32                         PFMREQHNDLRShareParamsAvailMask;    // mask of the SBIOS reported PShareParams counters.
} PFM_REQ_HNDLR_SENSOR_DATA, *PPFM_REQ_HNDLR_SENSOR_DATA;

/*!
 * Platform Power Mode related data
 */
typedef struct
{

    /*!
     * Set while OS work item is pending execution.
     */
    NvBool bWorkItemPending;

    /*!
     * Mask of all the platform power modes supported on the system.
     */
    NvU8   ppmMaskCurrent;

    /*!
     * Current Platform Power Mode.
     */
    NvU8   ppmIdxCurrent;

    /*!
     * Requested new Platform Power mode index to be set.
     */
    NvU8   ppmIdxRequested;
} PFM_REQ_HNDLR_PPM_DATA;

/*!
 * EDPPeak control data
 */
typedef struct
{
    /*!
     * Set while OS workitem is pending execution.
     */
    NvBool  bWorkItemPending;

    /*!
     * EDPpeak limit info data status.
     */
    NvBool bDifferPlatformEdppLimit;

    /*!
     * EDPpeak limit info data status.
     */
    NV_STATUS  status;

    /*!
     * EDPpeak info data.
     */
    NV0000_CTRL_PFM_REQ_HNDLR_EDPP_LIMIT_INFO_V1 edppLimitInfo;
} PFM_REQ_HNDLR_EDPP_DATA;

/*!
 * Control tracking and cache limits from SBIOS
 */
typedef struct
{
    NvBool  bStateInitialized;
    NvBool  bPM1ForcedOff;
    NvBool  bTGPUOverrideRequired;

    // Current status of system applied EDPpeak update event
    NvBool  bEDPpeakUpdateEnabled;

    // Current status of system applied user configurable TGP enable event
    NvBool  bUserConfigTGPmodeEnabled;

    // Current status of platform customized and applied user configurable TGP delta
    NvBool  bPlatformUserConfigTGPmodeEnabled;

    // Current EDPPeak limit control
    PFM_REQ_HNDLR_EDPP_DATA  edppLimit;
} PFM_REQ_HNDLR_CONTROL_DATA;

/*!
 * When updating GPU System control parameters, we may evaluate the _DSM method
 * to query the latest limit inside SBIOS. But we are not allowed to evaluate
 * ACPI method in level > PASSIVE, thus if we want to update the system limit at
 * DPC level, we need to queue a workitem to defer the ACPI evaluation to
 * passive level. This struct is used to record whether a workitem is queued for
 * this purpose and what counter has been touched before the workitem is
 * executed. (Refer to _pfmreqhndlrUpdateSystemParamLimitWorkItem for more details)
 */
typedef struct
{
    /*!
     * Set while OS work item is pending execution.
     */
    NvBool    bWorkItemPending;

    /*!
     * Bit mask for the counter IDs of the sensor data which has been updated
     * before the pending work item was executed.
     */
    NvU32     queuedCounterMask;
} PFM_REQ_HNDLR_SYS_CONTROL_DATA;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PLATFORM_REQUEST_HANDLER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PlatformRequestHandler;
struct NVOC_METADATA__Object;


struct PlatformRequestHandler {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PlatformRequestHandler *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct PlatformRequestHandler *__nvoc_pbase_PlatformRequestHandler;    // pfmreqhndlr

    // 3 PDB properties
    NvBool PDB_PROP_PFMREQHNDLR_SUPPORTED;
    NvBool PDB_PROP_PFMREQHNDLR_SYS_CONTROL_SUPPORTED;
    NvBool PDB_PROP_PFMREQHNDLR_IS_PLATFORM_LEGACY;

    // Data members
    NvU32 pfmreqhndlrSupportedGpuIdx;
    PFM_REQ_HNDLR_SENSOR_DATA sensorData;
    PFM_REQ_HNDLR_PPM_DATA ppmData;
    PFM_REQ_HNDLR_CONTROL_DATA controlData;
    PFM_REQ_HNDLR_SYS_CONTROL_DATA sysControlData;
    NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_FRM_DATA_PARAMS frmData;
    PFM_REQ_HNDLR_VPSTATE_CACHE vPstateCache;
    NvU32 dsmVersion;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PlatformRequestHandler {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_PlatformRequestHandler_TYPEDEF__
#define __NVOC_CLASS_PlatformRequestHandler_TYPEDEF__
typedef struct PlatformRequestHandler PlatformRequestHandler;
#endif /* __NVOC_CLASS_PlatformRequestHandler_TYPEDEF__ */

#ifndef __nvoc_class_id_PlatformRequestHandler
#define __nvoc_class_id_PlatformRequestHandler 0x641a7f
#endif /* __nvoc_class_id_PlatformRequestHandler */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PlatformRequestHandler;

#define __staticCast_PlatformRequestHandler(pThis) \
    ((pThis)->__nvoc_pbase_PlatformRequestHandler)

#ifdef __nvoc_platform_request_handler_h_disabled
#define __dynamicCast_PlatformRequestHandler(pThis) ((PlatformRequestHandler*) NULL)
#else //__nvoc_platform_request_handler_h_disabled
#define __dynamicCast_PlatformRequestHandler(pThis) \
    ((PlatformRequestHandler*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PlatformRequestHandler)))
#endif //__nvoc_platform_request_handler_h_disabled

// Property macros
#define PDB_PROP_PFMREQHNDLR_SYS_CONTROL_SUPPORTED_BASE_CAST
#define PDB_PROP_PFMREQHNDLR_SYS_CONTROL_SUPPORTED_BASE_NAME PDB_PROP_PFMREQHNDLR_SYS_CONTROL_SUPPORTED
#define PDB_PROP_PFMREQHNDLR_IS_PLATFORM_LEGACY_BASE_CAST
#define PDB_PROP_PFMREQHNDLR_IS_PLATFORM_LEGACY_BASE_NAME PDB_PROP_PFMREQHNDLR_IS_PLATFORM_LEGACY
#define PDB_PROP_PFMREQHNDLR_SUPPORTED_BASE_CAST
#define PDB_PROP_PFMREQHNDLR_SUPPORTED_BASE_NAME PDB_PROP_PFMREQHNDLR_SUPPORTED

NV_STATUS __nvoc_objCreateDynamic_PlatformRequestHandler(PlatformRequestHandler**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PlatformRequestHandler(PlatformRequestHandler**, Dynamic*, NvU32);
#define __objCreate_PlatformRequestHandler(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_PlatformRequestHandler((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS pfmreqhndlrConstruct_IMPL(struct PlatformRequestHandler *arg_pPlatformRequestHandler);

#define __nvoc_pfmreqhndlrConstruct(arg_pPlatformRequestHandler) pfmreqhndlrConstruct_IMPL(arg_pPlatformRequestHandler)
NV_STATUS pfmreqhndlrStateInit_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrStateInit(struct PlatformRequestHandler *pPlatformRequestHandler) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrStateInit(pPlatformRequestHandler) pfmreqhndlrStateInit_IMPL(pPlatformRequestHandler)
#endif //__nvoc_platform_request_handler_h_disabled

void pfmreqhndlrStateDestroy_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline void pfmreqhndlrStateDestroy(struct PlatformRequestHandler *pPlatformRequestHandler) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrStateDestroy(pPlatformRequestHandler) pfmreqhndlrStateDestroy_IMPL(pPlatformRequestHandler)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrStateLoad_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrStateLoad(struct PlatformRequestHandler *pPlatformRequestHandler) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrStateLoad(pPlatformRequestHandler) pfmreqhndlrStateLoad_IMPL(pPlatformRequestHandler)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrStateUnload_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrStateUnload(struct PlatformRequestHandler *pPlatformRequestHandler) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrStateUnload(pPlatformRequestHandler) pfmreqhndlrStateUnload_IMPL(pPlatformRequestHandler)
#endif //__nvoc_platform_request_handler_h_disabled

NvBool pfmreqhndlrIsInitialized_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NvBool pfmreqhndlrIsInitialized(struct PlatformRequestHandler *pPlatformRequestHandler) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_FALSE;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrIsInitialized(pPlatformRequestHandler) pfmreqhndlrIsInitialized_IMPL(pPlatformRequestHandler)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrControl_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, NvU16 arg2, NvU16 arg3, NvU32 *arg4);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrControl(struct PlatformRequestHandler *pPlatformRequestHandler, NvU16 arg2, NvU16 arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrControl(pPlatformRequestHandler, arg2, arg3, arg4) pfmreqhndlrControl_IMPL(pPlatformRequestHandler, arg2, arg3, arg4)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrPcontrol_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, NvU32 input, NvBool bSbiosCall);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrPcontrol(struct PlatformRequestHandler *pPlatformRequestHandler, NvU32 input, NvBool bSbiosCall) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrPcontrol(pPlatformRequestHandler, input, bSbiosCall) pfmreqhndlrPcontrol_IMPL(pPlatformRequestHandler, input, bSbiosCall)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrOperatingLimitUpdate_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, NvU32 id, NvU32 limit, NvBool bSet);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrOperatingLimitUpdate(struct PlatformRequestHandler *pPlatformRequestHandler, NvU32 id, NvU32 limit, NvBool bSet) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrOperatingLimitUpdate(pPlatformRequestHandler, id, limit, bSet) pfmreqhndlrOperatingLimitUpdate_IMPL(pPlatformRequestHandler, id, limit, bSet)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrHandleCheckPM1Available_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvBool *pbPM1Available);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrHandleCheckPM1Available(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvBool *pbPM1Available) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrHandleCheckPM1Available(pPlatformRequestHandler, pGpu, pbPM1Available) pfmreqhndlrHandleCheckPM1Available_IMPL(pPlatformRequestHandler, pGpu, pbPM1Available)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrGetPerfSensorCounterById_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, NvU16 counterId, NvU32 *pCounterVal);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrGetPerfSensorCounterById(struct PlatformRequestHandler *pPlatformRequestHandler, NvU16 counterId, NvU32 *pCounterVal) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrGetPerfSensorCounterById(pPlatformRequestHandler, counterId, pCounterVal) pfmreqhndlrGetPerfSensorCounterById_IMPL(pPlatformRequestHandler, counterId, pCounterVal)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrHandleEdppeakLimitUpdate_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvBool bEnable);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrHandleEdppeakLimitUpdate(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvBool bEnable) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrHandleEdppeakLimitUpdate(pPlatformRequestHandler, pGpu, bEnable) pfmreqhndlrHandleEdppeakLimitUpdate_IMPL(pPlatformRequestHandler, pGpu, bEnable)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrHandlePlatformEdppLimitUpdate_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvU32 platformEdppLimit);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrHandlePlatformEdppLimitUpdate(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvU32 platformEdppLimit) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrHandlePlatformEdppLimitUpdate(pPlatformRequestHandler, pGpu, platformEdppLimit) pfmreqhndlrHandlePlatformEdppLimitUpdate_IMPL(pPlatformRequestHandler, pGpu, platformEdppLimit)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrHandlePlatformGetEdppLimit_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvU32 *pPlatformEdppLimit);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrHandlePlatformGetEdppLimit(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvU32 *pPlatformEdppLimit) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrHandlePlatformGetEdppLimit(pPlatformRequestHandler, pGpu, pPlatformEdppLimit) pfmreqhndlrHandlePlatformGetEdppLimit_IMPL(pPlatformRequestHandler, pGpu, pPlatformEdppLimit)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrHandlePlatformSetEdppLimitInfo_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrHandlePlatformSetEdppLimitInfo(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrHandlePlatformSetEdppLimitInfo(pPlatformRequestHandler, pGpu) pfmreqhndlrHandlePlatformSetEdppLimitInfo_IMPL(pPlatformRequestHandler, pGpu)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrHandleUserConfigurableTgpMode_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvBool bEnable);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrHandleUserConfigurableTgpMode(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NvBool bEnable) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrHandleUserConfigurableTgpMode(pPlatformRequestHandler, pGpu, bEnable) pfmreqhndlrHandleUserConfigurableTgpMode_IMPL(pPlatformRequestHandler, pGpu, bEnable)
#endif //__nvoc_platform_request_handler_h_disabled

NV_STATUS pfmreqhndlrStateSync_IMPL(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams);

#ifdef __nvoc_platform_request_handler_h_disabled
static inline NV_STATUS pfmreqhndlrStateSync(struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu, NV2080_CTRL_INTERNAL_PFM_REQ_HNDLR_STATE_SYNC_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("PlatformRequestHandler was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_platform_request_handler_h_disabled
#define pfmreqhndlrStateSync(pPlatformRequestHandler, pGpu, pParams) pfmreqhndlrStateSync_IMPL(pPlatformRequestHandler, pGpu, pParams)
#endif //__nvoc_platform_request_handler_h_disabled

#undef PRIVATE_FIELD


/* ------------------------ Function Prototypes ---------------------------- */

OBJGPU   *pfmreqhndlrGetGpu                             (struct PlatformRequestHandler *pPlatformRequestHandler);
NV_STATUS pfmreqhndlrInitGpu                            (struct PlatformRequestHandler *pPlatformRequestHandler);
NV_STATUS pfmreqhndlrInitSensors                        (struct PlatformRequestHandler *pPlatformRequestHandler);
NV_STATUS pfmreqhndlrUpdatePerfCounter                  (struct PlatformRequestHandler *pPlatformRequestHandler, NvU32 id, NvU32 value);
NV_STATUS pfmreqhndlrHandleStatusChangeEvent            (struct PlatformRequestHandler *pPlatformRequestHandler, OBJGPU *pGpu);
NV_STATUS pfmreqhndlrCallACPI                           (OBJGPU *pGpu, NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS *pParams);
NV_STATUS pfmreqhndlrCallACPI_EX                        (OBJGPU *pGpu, NV0000_CTRL_PFM_REQ_HNDLR_CALL_ACPI_PARAMS_EX *pParams);
NV_STATUS pfmreqhndlrPassiveModeTransition              (NvBool bEnter, NvU32 apiLockFlag, NvU32 gpuLockFlag);
NV_STATUS pfmreqhndlrQueryPlatformPowerModeDataFromBIOS (struct PlatformRequestHandler *pPlatformRequestHandler);
NV_STATUS pfmreqhndlrGetPerfSensorCounters              (NV0000_CTRL_SYSTEM_PFM_REQ_HNDLR_GET_PERF_SENSOR_COUNTERS_PARAMS *pParams, NvU16 mapFlags);

#endif // PLATFORM_REQUEST_HANDLER_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PLATFORM_REQUEST_HANDLER_NVOC_H_
