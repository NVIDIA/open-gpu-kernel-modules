/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//
// NOTE: This file must contain only preprocessor directives and GMCAPI entries
//       Do not define any types here (or in any files included from here).
//       Any macro definitions that can have different values depending on the
//       context must be #undef'd at the end of the file.
//

//
// There are two types of GMCAPIs:
//   - GMCAPI_PHYSICAL is invoked by the Kernel and handled by Physical RM
//   - GMCAPI_KERNEL   is invoked by Physical RM and handled by Kernel
// If you want both definitions to be the same, you can just define GMCAPI.
// If you only care about one group, leave the other undefined.
//
#if !defined(GMCAPI) && !defined(GMCAPI_KERNEL) && !defined(GMCAPI_PHYSICAL)
#error "Define GMCAPI or GMCAPI_{KERNEL,PHYSICAL} macros before including this file"
#elif defined(GMCAPI) && (defined(GMCAPI_KERNEL) || defined(GMCAPI_PHYSICAL))
#error "Cannot define both GMCAPI and GMCAPI_{KERNEL,PHYSICAL} at the same time"
#endif

#if defined(GMCAPI)
#define GMCAPI_KERNEL    GMCAPI
#define GMCAPI_PHYSICAL  GMCAPI
#endif
#if !defined(GMCAPI_KERNEL)
#define GMCAPI_KERNEL(...)
#endif
#if !defined(GMCAPI_PHYSICAL)
#define GMCAPI_PHYSICAL(...)
#endif

//
// The format for the GMCAPI table is:
//   GMCAPI(numeric_id, symbolic_id, handler_func, input_type, output_type, flags, ...)
// where:
//   - numeric_id    - unique, permanent 32bit ID for this API
//   - symbolic_id   - symbolic constant name that is assigned this value
//   - handler_func  - external handler func that is invoked for this API
//   - input_type    - type of the input params for this API
//   - output_type   - type of the output params for this API
//   - flags         - Combination of GMCAPI_FLAGS_*
//
// The types can also be one of:
//   - GMCAPI_NONE    - If the API has no IN or OUT params
//   - GMCAPI_DYNAMIC - If the API deals with dynamic payloads instead of fixed types
//
// The signature of the handler function is:
//     NV_STATUS GmcApiHandler(GMCAPI_CONTEXT *pContext);
//

#define GMCAPI_ID(cat, id) (((GMCAPI_CATEGORY_##cat & 0xFFu) << 16u) | ((id) & 0xFFFFu))

// Reserved API range that can't be used
#define GMCAPI_CATEGORY_RESERVED   0x00u

//
// GSP management APIs
//
#define GMCAPI_CATEGORY_GSP_MGMT   0x01u

GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(GSP_MGMT, 0x0001),
/* symbolic_id  = */ GSP_INIT,
/* handler_func = */ gmcapiGspInit_DOES_NOT_EXIST, // see gspRpcProcessInitMessage()
/* input_type   = */ GMCAPI_DYNAMIC,
/* output_type  = */ GMCAPI_DYNAMIC,
/* flags        = */ 0,
)
// GSP -> CPU events
GMCAPI_KERNEL(
/* numeric_id   = */ GMCAPI_ID(GSP_MGMT, 0x0002),
/* symbolic_id  = */ EXEC_GENERIC_BOOTLOADER,
/* handler_func = */ gmcapiLoadAndExecGenericBootloader,
/* input_type   = */ GspLoadExecGenericBootloaderParams,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ GMCAPI_FLAGS_FIRE_AND_FORGET,
)
GMCAPI_KERNEL(
/* numeric_id   = */ GMCAPI_ID(GSP_MGMT, 0x0003),
/* symbolic_id  = */ EXEC_HS_BINARY,
/* handler_func = */ gmcapiLoadAndExecHsBinary,
/* input_type   = */ GspLoadExecHsBinaryParams,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ GMCAPI_FLAGS_FIRE_AND_FORGET,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(GSP_MGMT, 0x0004),
/* symbolic_id  = */ GSP_SUSPEND,
/* handler_func = */ gmcapiGspSuspend,
/* input_type   = */ GmcApiGspSuspendParams,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ GMCAPI_FLAGS_FIRE_AND_FORGET,
)
GMCAPI_KERNEL(
/* numeric_id   = */ GMCAPI_ID(GSP_MGMT, 0x0005),
/* symbolic_id  = */ GSP_RESUME_DONE,
/* handler_func = */ gmcapiGspResumeDone,
/* input_type   = */ GmcApiGspResumeDoneParams,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ GMCAPI_FLAGS_FIRE_AND_FORGET,
)

#define GMCAPI_CATEGORY_VGPU_MGMT  0x02u

// VGPU Management APIs
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0001),
/* symbolic_id  = */ ADD_VGPU_TYPE,
/* handler_func = */ gmcapiAddVgpuType,
/* input_type   = */ GMCAPI_DYNAMIC,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ 0,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0002),
/* symbolic_id  = */ QUERY_SUPPORTED_VGPU_TYPES,
/* handler_func = */ gmcapiQuerySupportedVgpuTypes,
/* input_type   = */ GMCAPI_NONE,
/* output_type  = */ GMCAPI_DYNAMIC, /* NvU32 vGPU type IDs. Valid count N = outParamSize / sizeof(NvU32) */
/* flags        = */ 0,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0003),
/* symbolic_id  = */ QUERY_CREATABLE_VGPU_TYPES,
/* handler_func = */ gmcapiQueryCreatableVgpuTypes,
/* input_type   = */ GMCAPI_NONE,
/* output_type  = */ GMCAPI_DYNAMIC, /* NvU32 vGPU type IDs. Valid count N = outParamSize / sizeof(NvU32) */
/* flags        = */ 0,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0004),
/* symbolic_id  = */ ASSIGN_VGPU_TYPE,
/* handler_func = */ gmcapiAssignVgpuType,
/* input_type   = */ GmcapiAssignVgpuTypeInParams,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ 0,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0005),
/* symbolic_id  = */ DEASSIGN_VGPU_TYPE,
/* handler_func = */ gmcapiDeassignVgpuType,
/* input_type   = */ GmcapiDeassignVgpuTypeInParams,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ 0,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0006),
/* symbolic_id  = */ QUERY_VGPU_PROPERTIES,
/* handler_func = */ gmcapiQueryVgpuProperties,
/* input_type   = */ GmcapiQueryVgpuPropertiesInParams,
/* output_type  = */ GMCAPI_DYNAMIC, /* NVKV encoded vGPU properties params. */
/* flags        = */ 0,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0007),
/* symbolic_id  = */ QUERY_ASSIGNED_VF_VGPU_TYPE,
/* handler_func = */ gmcapiQueryAssignedVfVgpuType,
/* input_type   = */ GmcapiQueryAssignedVfVgpuTypeInParams,
/* output_type  = */ GmcapiQueryAssignedVfVgpuTypeOutParams,
/* flags        = */ 0,
)

// GSP VGPU Plugin Task APIs
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0020),
/* symbolic_id  = */ BOOTLOAD_GSP_VGPU_PLUGIN_TASK,
/* handler_func = */ gmcapiBootloadGspVgpuPluginTask,
/* input_type   = */ GMCAPI_DYNAMIC, /* NVKV encoded bootload params, see gmcapi_vgpu.h */
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ 0,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0021),
/* symbolic_id  = */ SHUTDOWN_GSP_VGPU_PLUGIN_TASK,
/* handler_func = */ gmcapiShutdownGspVgpuPluginTask,
/* input_type   = */ GmcApiShutdownGspVgpuPluginTaskRequest,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ GMCAPI_FLAGS_FIRE_AND_FORGET,
)
GMCAPI_KERNEL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0022),
/* symbolic_id  = */ SHUTDOWN_GSP_VGPU_PLUGIN_TASK_COMPLETE,
/* handler_func = */ gmcapiShutdownGspVgpuPluginTaskComplete,
/* input_type   = */ GmcApiShutdownGspVgpuPluginTaskCompleteEvent,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ GMCAPI_FLAGS_FIRE_AND_FORGET,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0023),
/* symbolic_id  = */ CLEANUP_GSP_VGPU_PLUGIN_RESOURCES,
/* handler_func = */ gmcapiCleanupGspVgpuPluginResources,
/* input_type   = */ GmcApiCleanupGspVgpuPluginResourcesRequest,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ 0,
)
GMCAPI_KERNEL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0024),
/* symbolic_id  = */ VGPU_PLUGIN_TRIGGERED_EVENT,
/* handler_func = */ gmcapiVgpuPluginTriggeredEvent,
/* input_type   = */ GmcApiVgpuPluginTriggeredEvent,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ GMCAPI_FLAGS_FIRE_AND_FORGET,
)
//
// Guest FB scrubbing / GSP CeUtils
//
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0025),
/* symbolic_id  = */ VGPU_MGR_SCRUB_GUEST_FB,
/* handler_func = */ gmcapiVgpuMgrScrubGuestFb,
/* input_type   = */ GmcapiVgpuMgrScrubGuestFbInParams,
/* output_type  = */ GmcapiVgpuMgrScrubGuestFbOutParams,
/* flags        = */ 0,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0026),
/* symbolic_id  = */ VGPU_MGR_ALLOC_GSP_CEUTILS,
/* handler_func = */ gmcapiVgpuMgrAllocGspCeUtils,
/* input_type   = */ GmcapiVgpuMgrAllocGspCeUtilsInParams,
/* output_type  = */ GmcapiVgpuMgrAllocGspCeUtilsOutParams,
/* flags        = */ 0,
)
GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(VGPU_MGMT, 0x0027),
/* symbolic_id  = */ VGPU_MGR_FREE_GSP_CEUTILS,
/* handler_func = */ gmcapiVgpuMgrFreeGspCeUtils,
/* input_type   = */ GmcapiVgpuMgrFreeGspCeUtilsInParams,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ 0,
)

#define GMCAPI_CATEGORY_SCHED  0x03u

GMCAPI_PHYSICAL(
/* numeric_id   = */ GMCAPI_ID(SCHED, 0x0001),
/* symbolic_id  = */ SCHED_CONTROL,
/* handler_func = */ gmcapiSchedControl,
/* input_type   = */ GMCAPI_DYNAMIC,  /* NVKV encoded, see gmcapi_sched.h */
/* output_type  = */ GMCAPI_DYNAMIC,
/* flags        = */ 0,
)

#define GMCAPI_CATEGORY_MEMORY_MGMT  0x04u

GMCAPI_KERNEL(
/* numeric_id   = */ GMCAPI_ID(MEMORY_MGMT, 0x0001),
/* symbolic_id  = */ MMU_FAULT_QUEUED,
/* handler_func = */ gmcapiMmuFaultQueued,
/* input_type   = */ GMCAPI_NONE,
/* output_type  = */ GMCAPI_NONE,
/* flags        = */ GMCAPI_FLAGS_FIRE_AND_FORGET,
)

#undef GMCAPI
#undef GMCAPI_KERNEL
#undef GMCAPI_PHYSICAL
