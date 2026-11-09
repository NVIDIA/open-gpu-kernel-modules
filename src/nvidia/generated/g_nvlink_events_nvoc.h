
#ifndef _G_NVLINK_EVENTS_NVOC_H_
#define _G_NVLINK_EVENTS_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if defined(SRT_BUILD)
#define RMCFG_FEATURE_x 1
#else
#include "rmconfig.h"
#endif

#include "g_nvlink_events_nvoc.h"

#ifndef _NVLINK_EVENTS_H_
#define _NVLINK_EVENTS_H_

#include "events/gpu/gpu_events.h"
#include "events/gpu/nvlink/nvlink_event_ctx_defs.h"
#include "events/gpu/nvlink/nvlink_event_defs.h"

typedef struct NvlinkAliTrainingFailureCtx
{
    EventContextHeader header;
    NVLINK_ALI_TRAINING_FAILURE_DATA data;
} NvlinkAliTrainingFailureCtx;

typedef struct NvlinkAliTrainingFailureLegacyCtx
{
    EventContextHeader header;
    NVLINK_ALI_TRAINING_FAILURE_LEGACY_DATA data;
} NvlinkAliTrainingFailureLegacyCtx;

typedef struct NvlinkMseErrorCtx
{
    EventContextHeader header;
    NVLINK_MSE_ERROR_DATA data;
} NvlinkMseErrorCtx;

typedef struct NvlinkMseErrorLegacyCtx
{
    EventContextHeader header;
    NVLINK_MSE_ERROR_LEGACY_DATA data;
} NvlinkMseErrorLegacyCtx;

typedef struct NvlinkSwLinkDownCtx
{
    EventContextHeader header;
    NVLINK_SW_LINK_DOWN_ERROR_DATA data;
} NvlinkSwLinkDownCtx;

typedef struct NvlinkSwLinkDownLegacyCtx
{
    EventContextHeader header;
    NVLINK_SW_LINK_DOWN_ERROR_LEGACY_DATA data;
} NvlinkSwLinkDownLegacyCtx;

typedef struct NvlinkFatalErrorNvl3Ctx
{
    EventContextHeader header;
    NVLINK_FATAL_ERROR_DATA_NVL3 data;
} NvlinkFatalErrorNvl3Ctx;

typedef struct NvlinkFatalErrorNvl3LegacyCtx
{
    EventContextHeader header;
    NVLINK_FATAL_ERROR_LEGACY_DATA_NVL3 data;
} NvlinkFatalErrorNvl3LegacyCtx;

typedef struct NvlinkPrivErrorCtx
{
    EventContextHeader header;
    NVLINK_PRIV_ERROR_DATA data;
} NvlinkPrivErrorCtx;

typedef struct NvlinkBlackwellErrorCtx
{
    EventContextHeader header;
    NVLINK_BLACKWELL_ERROR_DATA data;
} NvlinkBlackwellErrorCtx;

typedef struct NvlinkBlackwellLegacyErrorCtx
{
    EventContextHeader header;
    NVLINK_BLACKWELL_LEGACY_ERROR_DATA data;
} NvlinkBlackwellLegacyErrorCtx;

typedef struct NvlinkSawErrorCtx
{
    EventContextHeader header;
    NVLINK_SAW_ERROR_DATA data;
} NvlinkSawErrorCtx;

typedef struct NvlinkTlwErrorCtx
{
    EventContextHeader header;
    NVLINK_TLW_ERROR_DATA data;
} NvlinkTlwErrorCtx;

typedef struct NvlinkRlwErrorCtx
{
    EventContextHeader header;
    NVLINK_RLW_ERROR_DATA data;
} NvlinkRlwErrorCtx;

typedef struct NvlinkTrexErrorCtx
{
    EventContextHeader header;
    NVLINK_TREX_ERROR_DATA data;
} NvlinkTrexErrorCtx;

typedef struct NvlinkNvlpwCtrlErrorCtx
{
    EventContextHeader header;
    NVLINK_NVLPW_CTRL_ERROR_DATA data;
} NvlinkNvlpwCtrlErrorCtx;

typedef struct NvlinkNetirErrorCtx
{
    EventContextHeader header;
    NVLINK_NETIR_ERROR_DATA data;
} NvlinkNetirErrorCtx;



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkAliTrainingFailure;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkAliTrainingFailure {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkAliTrainingFailure *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkAliTrainingFailure *__nvoc_pbase_NvlinkAliTrainingFailure;    // nvlinkalitrainfail

    // Data members
    NvlinkAliTrainingFailureCtx context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkAliTrainingFailure {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkAliTrainingFailure
#define __nvoc_class_id_NvlinkAliTrainingFailure 0x6e0b4bu
typedef struct NvlinkAliTrainingFailure NvlinkAliTrainingFailure;
#endif /* __nvoc_class_id_NvlinkAliTrainingFailure */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkAliTrainingFailure;

#define __staticCast_NvlinkAliTrainingFailure(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkAliTrainingFailure)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkAliTrainingFailure(pThis) ((NvlinkAliTrainingFailure*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkAliTrainingFailure(pThis) \
    ((NvlinkAliTrainingFailure*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkAliTrainingFailure)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkAliTrainingFailure(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkAliTrainingFailure(NvlinkAliTrainingFailure**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData);
#define __objCreate_NvlinkAliTrainingFailure(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData) \
    __nvoc_objCreate_NvlinkAliTrainingFailure((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData)


// Wrapper macros for implementation functions
NV_STATUS nvlinkalitrainfailConstruct_IMPL(struct NvlinkAliTrainingFailure *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData);
#define __nvoc_nvlinkalitrainfailConstruct(pEvent, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData) nvlinkalitrainfailConstruct_IMPL(pEvent, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkAliTrainingFailureLegacy;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkAliTrainingFailureLegacy {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkAliTrainingFailureLegacy *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkAliTrainingFailureLegacy *__nvoc_pbase_NvlinkAliTrainingFailureLegacy;    // nvlinkalitrainfaillegacy

    // Data members
    NvlinkAliTrainingFailureLegacyCtx context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkAliTrainingFailureLegacy {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkAliTrainingFailureLegacy
#define __nvoc_class_id_NvlinkAliTrainingFailureLegacy 0x79d043u
typedef struct NvlinkAliTrainingFailureLegacy NvlinkAliTrainingFailureLegacy;
#endif /* __nvoc_class_id_NvlinkAliTrainingFailureLegacy */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkAliTrainingFailureLegacy;

#define __staticCast_NvlinkAliTrainingFailureLegacy(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkAliTrainingFailureLegacy)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkAliTrainingFailureLegacy(pThis) ((NvlinkAliTrainingFailureLegacy*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkAliTrainingFailureLegacy(pThis) \
    ((NvlinkAliTrainingFailureLegacy*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkAliTrainingFailureLegacy)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkAliTrainingFailureLegacy(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkAliTrainingFailureLegacy(NvlinkAliTrainingFailureLegacy**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData, NvU32 xidCode);
#define __objCreate_NvlinkAliTrainingFailureLegacy(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData, xidCode) \
    __nvoc_objCreate_NvlinkAliTrainingFailureLegacy((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData, xidCode)


// Wrapper macros for implementation functions
NV_STATUS nvlinkalitrainfaillegacyConstruct_IMPL(struct NvlinkAliTrainingFailureLegacy *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU32 entryCount, NvU32 debugDataCount, const NvU8 *pLinkIds, const NvU32 *pDebugData, NvU32 xidCode);
#define __nvoc_nvlinkalitrainfaillegacyConstruct(pEvent, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData, xidCode) nvlinkalitrainfaillegacyConstruct_IMPL(pEvent, pEventBus, severity, entryCount, debugDataCount, pLinkIds, pDebugData, xidCode)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkMseError;
struct NVOC_METADATA__GpuFirmwareFault;


struct NvlinkMseError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkMseError *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuFirmwareFault __nvoc_base_GpuFirmwareFault;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuFirmwareFault *__nvoc_pbase_GpuFirmwareFault;    // gpufwfault super
    struct NvlinkMseError *__nvoc_pbase_NvlinkMseError;    // nvlinkmseerror

    // Data members
    NvlinkMseErrorCtx context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkMseError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuFirmwareFault metadata__GpuFirmwareFault;
};

#ifndef __nvoc_class_id_NvlinkMseError
#define __nvoc_class_id_NvlinkMseError 0x328f75u
typedef struct NvlinkMseError NvlinkMseError;
#endif /* __nvoc_class_id_NvlinkMseError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkMseError;

#define __staticCast_NvlinkMseError(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkMseError)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkMseError(pThis) ((NvlinkMseError*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkMseError(pThis) \
    ((NvlinkMseError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkMseError)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkMseError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkMseError(NvlinkMseError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, NvU32 errorStatus, const NvU32 *pDebugData);
#define __objCreate_NvlinkMseError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, eventCode, severity, errorStatus, pDebugData) \
    __nvoc_objCreate_NvlinkMseError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, eventCode, severity, errorStatus, pDebugData)


// Wrapper macros for implementation functions
NV_STATUS nvlinkmseerrorConstruct_IMPL(struct NvlinkMseError *pEvent, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, NvU32 errorStatus, const NvU32 *pDebugData);
#define __nvoc_nvlinkmseerrorConstruct(pEvent, pEventBus, eventCode, severity, errorStatus, pDebugData) nvlinkmseerrorConstruct_IMPL(pEvent, pEventBus, eventCode, severity, errorStatus, pDebugData)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkMseErrorLegacy;
struct NVOC_METADATA__GpuFirmwareFault;


struct NvlinkMseErrorLegacy {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkMseErrorLegacy *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuFirmwareFault __nvoc_base_GpuFirmwareFault;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuFirmwareFault *__nvoc_pbase_GpuFirmwareFault;    // gpufwfault super
    struct NvlinkMseErrorLegacy *__nvoc_pbase_NvlinkMseErrorLegacy;    // nvlinkmseerrorlegacy

    // Data members
    NvlinkMseErrorLegacyCtx context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkMseErrorLegacy {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuFirmwareFault metadata__GpuFirmwareFault;
};

#ifndef __nvoc_class_id_NvlinkMseErrorLegacy
#define __nvoc_class_id_NvlinkMseErrorLegacy 0x5edd3fu
typedef struct NvlinkMseErrorLegacy NvlinkMseErrorLegacy;
#endif /* __nvoc_class_id_NvlinkMseErrorLegacy */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkMseErrorLegacy;

#define __staticCast_NvlinkMseErrorLegacy(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkMseErrorLegacy)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkMseErrorLegacy(pThis) ((NvlinkMseErrorLegacy*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkMseErrorLegacy(pThis) \
    ((NvlinkMseErrorLegacy*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkMseErrorLegacy)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkMseErrorLegacy(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkMseErrorLegacy(NvlinkMseErrorLegacy**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, const NvU32 *pDebugData, NvU32 xidCode, NVLINK_MSE_LEGACY_XID_ERROR_NAME legacyXidErrorName, NvBool bFatal, NvBool bXContain, NvBool bInjected, NvS32 linkId, NvU32 intrInfo, NvU32 legacyErrorStatus);
#define __objCreate_NvlinkMseErrorLegacy(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, eventCode, severity, pDebugData, xidCode, legacyXidErrorName, bFatal, bXContain, bInjected, linkId, intrInfo, legacyErrorStatus) \
    __nvoc_objCreate_NvlinkMseErrorLegacy((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, eventCode, severity, pDebugData, xidCode, legacyXidErrorName, bFatal, bXContain, bInjected, linkId, intrInfo, legacyErrorStatus)


// Wrapper macros for implementation functions
NV_STATUS nvlinkmseerrorlegacyConstruct_IMPL(struct NvlinkMseErrorLegacy *pEvent, struct EventBus *pEventBus, NvU32 eventCode, NvU32 severity, const NvU32 *pDebugData, NvU32 xidCode, NVLINK_MSE_LEGACY_XID_ERROR_NAME legacyXidErrorName, NvBool bFatal, NvBool bXContain, NvBool bInjected, NvS32 linkId, NvU32 intrInfo, NvU32 legacyErrorStatus);
#define __nvoc_nvlinkmseerrorlegacyConstruct(pEvent, pEventBus, eventCode, severity, pDebugData, xidCode, legacyXidErrorName, bFatal, bXContain, bInjected, linkId, intrInfo, legacyErrorStatus) nvlinkmseerrorlegacyConstruct_IMPL(pEvent, pEventBus, eventCode, severity, pDebugData, xidCode, legacyXidErrorName, bFatal, bXContain, bInjected, linkId, intrInfo, legacyErrorStatus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSwLinkDown;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkSwLinkDown {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkSwLinkDown *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkSwLinkDown *__nvoc_pbase_NvlinkSwLinkDown;    // nvlinkswlinkdown

    // Data members
    NvlinkSwLinkDownCtx context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSwLinkDown {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkSwLinkDown
#define __nvoc_class_id_NvlinkSwLinkDown 0xb7f16fu
typedef struct NvlinkSwLinkDown NvlinkSwLinkDown;
#endif /* __nvoc_class_id_NvlinkSwLinkDown */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSwLinkDown;

#define __staticCast_NvlinkSwLinkDown(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkSwLinkDown)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSwLinkDown(pThis) ((NvlinkSwLinkDown*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSwLinkDown(pThis) \
    ((NvlinkSwLinkDown*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkSwLinkDown)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkSwLinkDown(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkSwLinkDown(NvlinkSwLinkDown**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 linkId, const NvU32 *pDebugData);
#define __objCreate_NvlinkSwLinkDown(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, pDebugData) \
    __nvoc_objCreate_NvlinkSwLinkDown((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, pDebugData)


// Wrapper macros for implementation functions
NV_STATUS nvlinkswlinkdownConstruct_IMPL(struct NvlinkSwLinkDown *pEvent, struct EventBus *pEventBus, NvU32 linkId, const NvU32 *pDebugData);
#define __nvoc_nvlinkswlinkdownConstruct(pEvent, pEventBus, linkId, pDebugData) nvlinkswlinkdownConstruct_IMPL(pEvent, pEventBus, linkId, pDebugData)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSwLinkDownLegacy;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkSwLinkDownLegacy {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkSwLinkDownLegacy *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkSwLinkDownLegacy *__nvoc_pbase_NvlinkSwLinkDownLegacy;    // nvlinkswlinkdownlegacy

    // Data members
    NvlinkSwLinkDownLegacyCtx context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSwLinkDownLegacy {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkSwLinkDownLegacy
#define __nvoc_class_id_NvlinkSwLinkDownLegacy 0x555ef5u
typedef struct NvlinkSwLinkDownLegacy NvlinkSwLinkDownLegacy;
#endif /* __nvoc_class_id_NvlinkSwLinkDownLegacy */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSwLinkDownLegacy;

#define __staticCast_NvlinkSwLinkDownLegacy(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkSwLinkDownLegacy)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSwLinkDownLegacy(pThis) ((NvlinkSwLinkDownLegacy*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSwLinkDownLegacy(pThis) \
    ((NvlinkSwLinkDownLegacy*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkSwLinkDownLegacy)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkSwLinkDownLegacy(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkSwLinkDownLegacy(NvlinkSwLinkDownLegacy**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 linkId, NvU32 xidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected);
#define __objCreate_NvlinkSwLinkDownLegacy(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, xidCode, bFatal, bXContain, bInjected) \
    __nvoc_objCreate_NvlinkSwLinkDownLegacy((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, xidCode, bFatal, bXContain, bInjected)


// Wrapper macros for implementation functions
NV_STATUS nvlinkswlinkdownlegacyConstruct_IMPL(struct NvlinkSwLinkDownLegacy *pEvent, struct EventBus *pEventBus, NvU32 linkId, NvU32 xidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected);
#define __nvoc_nvlinkswlinkdownlegacyConstruct(pEvent, pEventBus, linkId, xidCode, bFatal, bXContain, bInjected) nvlinkswlinkdownlegacyConstruct_IMPL(pEvent, pEventBus, linkId, xidCode, bFatal, bXContain, bInjected)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkFatalErrorNvl3;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkFatalErrorNvl3 {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkFatalErrorNvl3 *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkFatalErrorNvl3 *__nvoc_pbase_NvlinkFatalErrorNvl3;    // nvlinkfatalerrornvl3

    // Data members
    NvlinkFatalErrorNvl3Ctx context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkFatalErrorNvl3 {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkFatalErrorNvl3
#define __nvoc_class_id_NvlinkFatalErrorNvl3 0x2a057au
typedef struct NvlinkFatalErrorNvl3 NvlinkFatalErrorNvl3;
#endif /* __nvoc_class_id_NvlinkFatalErrorNvl3 */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkFatalErrorNvl3;

#define __staticCast_NvlinkFatalErrorNvl3(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkFatalErrorNvl3)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkFatalErrorNvl3(pThis) ((NvlinkFatalErrorNvl3*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkFatalErrorNvl3(pThis) \
    ((NvlinkFatalErrorNvl3*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkFatalErrorNvl3)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkFatalErrorNvl3(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkFatalErrorNvl3(NvlinkFatalErrorNvl3**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 linkId, NvU32 debugDataCount, const NvU32 *pDebugData);
#define __objCreate_NvlinkFatalErrorNvl3(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, debugDataCount, pDebugData) \
    __nvoc_objCreate_NvlinkFatalErrorNvl3((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, debugDataCount, pDebugData)


// Wrapper macros for implementation functions
NV_STATUS nvlinkfatalerrornvl3Construct_IMPL(struct NvlinkFatalErrorNvl3 *pEvent, struct EventBus *pEventBus, NvU32 linkId, NvU32 debugDataCount, const NvU32 *pDebugData);
#define __nvoc_nvlinkfatalerrornvl3Construct(pEvent, pEventBus, linkId, debugDataCount, pDebugData) nvlinkfatalerrornvl3Construct_IMPL(pEvent, pEventBus, linkId, debugDataCount, pDebugData)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkFatalErrorNvl3Legacy;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkFatalErrorNvl3Legacy {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkFatalErrorNvl3Legacy *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkFatalErrorNvl3Legacy *__nvoc_pbase_NvlinkFatalErrorNvl3Legacy;    // nvlinkfatalerrornvl3legacy

    // Data members
    NvlinkFatalErrorNvl3LegacyCtx context;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkFatalErrorNvl3Legacy {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkFatalErrorNvl3Legacy
#define __nvoc_class_id_NvlinkFatalErrorNvl3Legacy 0xded290u
typedef struct NvlinkFatalErrorNvl3Legacy NvlinkFatalErrorNvl3Legacy;
#endif /* __nvoc_class_id_NvlinkFatalErrorNvl3Legacy */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkFatalErrorNvl3Legacy;

#define __staticCast_NvlinkFatalErrorNvl3Legacy(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkFatalErrorNvl3Legacy)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkFatalErrorNvl3Legacy(pThis) ((NvlinkFatalErrorNvl3Legacy*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkFatalErrorNvl3Legacy(pThis) \
    ((NvlinkFatalErrorNvl3Legacy*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkFatalErrorNvl3Legacy)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkFatalErrorNvl3Legacy(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkFatalErrorNvl3Legacy(NvlinkFatalErrorNvl3Legacy**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 linkId, NvU32 dword0, NvU32 dword1, NvU32 dword2, NvU32 dword3, NvU32 dword4, NvU32 dword5, NvU32 dword6);
#define __objCreate_NvlinkFatalErrorNvl3Legacy(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, dword0, dword1, dword2, dword3, dword4, dword5, dword6) \
    __nvoc_objCreate_NvlinkFatalErrorNvl3Legacy((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, dword0, dword1, dword2, dword3, dword4, dword5, dword6)


// Wrapper macros for implementation functions
NV_STATUS nvlinkfatalerrornvl3legacyConstruct_IMPL(struct NvlinkFatalErrorNvl3Legacy *pEvent, struct EventBus *pEventBus, NvU32 linkId, NvU32 dword0, NvU32 dword1, NvU32 dword2, NvU32 dword3, NvU32 dword4, NvU32 dword5, NvU32 dword6);
#define __nvoc_nvlinkfatalerrornvl3legacyConstruct(pEvent, pEventBus, linkId, dword0, dword1, dword2, dword3, dword4, dword5, dword6) nvlinkfatalerrornvl3legacyConstruct_IMPL(pEvent, pEventBus, linkId, dword0, dword1, dword2, dword3, dword4, dword5, dword6)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkPrivError;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkPrivError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkPrivError *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkPrivError *__nvoc_pbase_NvlinkPrivError;    // nvlinkpriverror

    // Data members
    NvlinkPrivErrorCtx context;
    GpuXidEventCtx xid137;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkPrivError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkPrivError
#define __nvoc_class_id_NvlinkPrivError 0xcb3e3fu
typedef struct NvlinkPrivError NvlinkPrivError;
#endif /* __nvoc_class_id_NvlinkPrivError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkPrivError;

#define __staticCast_NvlinkPrivError(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkPrivError)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkPrivError(pThis) ((NvlinkPrivError*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkPrivError(pThis) \
    ((NvlinkPrivError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkPrivError)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkPrivError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkPrivError(NvlinkPrivError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 linkId, NvU32 gpuId);
#define __objCreate_NvlinkPrivError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, gpuId) \
    __nvoc_objCreate_NvlinkPrivError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, gpuId)


// Wrapper macros for implementation functions
NV_STATUS nvlinkpriverrorConstruct_IMPL(struct NvlinkPrivError *pEvent, struct EventBus *pEventBus, NvU32 linkId, NvU32 gpuId);
#define __nvoc_nvlinkpriverrorConstruct(pEvent, pEventBus, linkId, gpuId) nvlinkpriverrorConstruct_IMPL(pEvent, pEventBus, linkId, gpuId)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSawErrorGpuFatal;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkSawErrorGpuFatal {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkSawErrorGpuFatal *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkSawErrorGpuFatal *__nvoc_pbase_NvlinkSawErrorGpuFatal;    // nvlinksawerrorgpufatal

    // Data members
    NvlinkSawErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSawErrorGpuFatal {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkSawErrorGpuFatal
#define __nvoc_class_id_NvlinkSawErrorGpuFatal 0x4cca06u
typedef struct NvlinkSawErrorGpuFatal NvlinkSawErrorGpuFatal;
#endif /* __nvoc_class_id_NvlinkSawErrorGpuFatal */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSawErrorGpuFatal;

#define __staticCast_NvlinkSawErrorGpuFatal(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkSawErrorGpuFatal)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSawErrorGpuFatal(pThis) ((NvlinkSawErrorGpuFatal*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSawErrorGpuFatal(pThis) \
    ((NvlinkSawErrorGpuFatal*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkSawErrorGpuFatal)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkSawErrorGpuFatal(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkSawErrorGpuFatal(NvlinkSawErrorGpuFatal**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkSawErrorGpuFatal(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkSawErrorGpuFatal((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinksawerrorgpufatalConstruct_IMPL(struct NvlinkSawErrorGpuFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinksawerrorgpufatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinksawerrorgpufatalConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSawErrorNonFatalWarning;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkSawErrorNonFatalWarning {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkSawErrorNonFatalWarning *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkSawErrorNonFatalWarning *__nvoc_pbase_NvlinkSawErrorNonFatalWarning;    // nvlinksawerrornonfatalwarning

    // Data members
    NvlinkSawErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSawErrorNonFatalWarning {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkSawErrorNonFatalWarning
#define __nvoc_class_id_NvlinkSawErrorNonFatalWarning 0x499ff1u
typedef struct NvlinkSawErrorNonFatalWarning NvlinkSawErrorNonFatalWarning;
#endif /* __nvoc_class_id_NvlinkSawErrorNonFatalWarning */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSawErrorNonFatalWarning;

#define __staticCast_NvlinkSawErrorNonFatalWarning(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkSawErrorNonFatalWarning)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSawErrorNonFatalWarning(pThis) ((NvlinkSawErrorNonFatalWarning*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSawErrorNonFatalWarning(pThis) \
    ((NvlinkSawErrorNonFatalWarning*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkSawErrorNonFatalWarning)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkSawErrorNonFatalWarning(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkSawErrorNonFatalWarning(NvlinkSawErrorNonFatalWarning**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkSawErrorNonFatalWarning(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkSawErrorNonFatalWarning((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinksawerrornonfatalwarningConstruct_IMPL(struct NvlinkSawErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinksawerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinksawerrornonfatalwarningConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSawErrorNonFatalContained;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkSawErrorNonFatalContained {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkSawErrorNonFatalContained *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkSawErrorNonFatalContained *__nvoc_pbase_NvlinkSawErrorNonFatalContained;    // nvlinksawerrornonfatalcontained

    // Data members
    NvlinkSawErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkSawErrorNonFatalContained {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkSawErrorNonFatalContained
#define __nvoc_class_id_NvlinkSawErrorNonFatalContained 0xed1b66u
typedef struct NvlinkSawErrorNonFatalContained NvlinkSawErrorNonFatalContained;
#endif /* __nvoc_class_id_NvlinkSawErrorNonFatalContained */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkSawErrorNonFatalContained;

#define __staticCast_NvlinkSawErrorNonFatalContained(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkSawErrorNonFatalContained)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSawErrorNonFatalContained(pThis) ((NvlinkSawErrorNonFatalContained*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkSawErrorNonFatalContained(pThis) \
    ((NvlinkSawErrorNonFatalContained*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkSawErrorNonFatalContained)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkSawErrorNonFatalContained(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkSawErrorNonFatalContained(NvlinkSawErrorNonFatalContained**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkSawErrorNonFatalContained(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkSawErrorNonFatalContained((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinksawerrornonfatalcontainedConstruct_IMPL(struct NvlinkSawErrorNonFatalContained *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinksawerrornonfatalcontainedConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinksawerrornonfatalcontainedConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkRlwErrorGpuFatal;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkRlwErrorGpuFatal {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkRlwErrorGpuFatal *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkRlwErrorGpuFatal *__nvoc_pbase_NvlinkRlwErrorGpuFatal;    // nvlinkrlwerrorgpufatal

    // Data members
    NvlinkRlwErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkRlwErrorGpuFatal {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkRlwErrorGpuFatal
#define __nvoc_class_id_NvlinkRlwErrorGpuFatal 0x888655u
typedef struct NvlinkRlwErrorGpuFatal NvlinkRlwErrorGpuFatal;
#endif /* __nvoc_class_id_NvlinkRlwErrorGpuFatal */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkRlwErrorGpuFatal;

#define __staticCast_NvlinkRlwErrorGpuFatal(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkRlwErrorGpuFatal)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkRlwErrorGpuFatal(pThis) ((NvlinkRlwErrorGpuFatal*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkRlwErrorGpuFatal(pThis) \
    ((NvlinkRlwErrorGpuFatal*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkRlwErrorGpuFatal)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkRlwErrorGpuFatal(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkRlwErrorGpuFatal(NvlinkRlwErrorGpuFatal**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkRlwErrorGpuFatal(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkRlwErrorGpuFatal((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinkrlwerrorgpufatalConstruct_IMPL(struct NvlinkRlwErrorGpuFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinkrlwerrorgpufatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinkrlwerrorgpufatalConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkRlwErrorNonFatalWarning;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkRlwErrorNonFatalWarning {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkRlwErrorNonFatalWarning *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkRlwErrorNonFatalWarning *__nvoc_pbase_NvlinkRlwErrorNonFatalWarning;    // nvlinkrlwerrornonfatalwarning

    // Data members
    NvlinkRlwErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkRlwErrorNonFatalWarning {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkRlwErrorNonFatalWarning
#define __nvoc_class_id_NvlinkRlwErrorNonFatalWarning 0x2fbc0du
typedef struct NvlinkRlwErrorNonFatalWarning NvlinkRlwErrorNonFatalWarning;
#endif /* __nvoc_class_id_NvlinkRlwErrorNonFatalWarning */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkRlwErrorNonFatalWarning;

#define __staticCast_NvlinkRlwErrorNonFatalWarning(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkRlwErrorNonFatalWarning)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkRlwErrorNonFatalWarning(pThis) ((NvlinkRlwErrorNonFatalWarning*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkRlwErrorNonFatalWarning(pThis) \
    ((NvlinkRlwErrorNonFatalWarning*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkRlwErrorNonFatalWarning)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkRlwErrorNonFatalWarning(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkRlwErrorNonFatalWarning(NvlinkRlwErrorNonFatalWarning**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkRlwErrorNonFatalWarning(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkRlwErrorNonFatalWarning((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinkrlwerrornonfatalwarningConstruct_IMPL(struct NvlinkRlwErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinkrlwerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinkrlwerrornonfatalwarningConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkRlwErrorApplicationFatal;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkRlwErrorApplicationFatal {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkRlwErrorApplicationFatal *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkRlwErrorApplicationFatal *__nvoc_pbase_NvlinkRlwErrorApplicationFatal;    // nvlinkrlwerrorapplicationfatal

    // Data members
    NvlinkRlwErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkRlwErrorApplicationFatal {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkRlwErrorApplicationFatal
#define __nvoc_class_id_NvlinkRlwErrorApplicationFatal 0x0d9cbau
typedef struct NvlinkRlwErrorApplicationFatal NvlinkRlwErrorApplicationFatal;
#endif /* __nvoc_class_id_NvlinkRlwErrorApplicationFatal */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkRlwErrorApplicationFatal;

#define __staticCast_NvlinkRlwErrorApplicationFatal(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkRlwErrorApplicationFatal)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkRlwErrorApplicationFatal(pThis) ((NvlinkRlwErrorApplicationFatal*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkRlwErrorApplicationFatal(pThis) \
    ((NvlinkRlwErrorApplicationFatal*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkRlwErrorApplicationFatal)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkRlwErrorApplicationFatal(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkRlwErrorApplicationFatal(NvlinkRlwErrorApplicationFatal**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkRlwErrorApplicationFatal(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkRlwErrorApplicationFatal((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinkrlwerrorapplicationfatalConstruct_IMPL(struct NvlinkRlwErrorApplicationFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinkrlwerrorapplicationfatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinkrlwerrorapplicationfatalConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkTlwErrorGpuFatal;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkTlwErrorGpuFatal {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkTlwErrorGpuFatal *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkTlwErrorGpuFatal *__nvoc_pbase_NvlinkTlwErrorGpuFatal;    // nvlinktlwerrorgpufatal

    // Data members
    NvlinkTlwErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkTlwErrorGpuFatal {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkTlwErrorGpuFatal
#define __nvoc_class_id_NvlinkTlwErrorGpuFatal 0x5c1052u
typedef struct NvlinkTlwErrorGpuFatal NvlinkTlwErrorGpuFatal;
#endif /* __nvoc_class_id_NvlinkTlwErrorGpuFatal */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTlwErrorGpuFatal;

#define __staticCast_NvlinkTlwErrorGpuFatal(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkTlwErrorGpuFatal)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkTlwErrorGpuFatal(pThis) ((NvlinkTlwErrorGpuFatal*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkTlwErrorGpuFatal(pThis) \
    ((NvlinkTlwErrorGpuFatal*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkTlwErrorGpuFatal)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkTlwErrorGpuFatal(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkTlwErrorGpuFatal(NvlinkTlwErrorGpuFatal**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkTlwErrorGpuFatal(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkTlwErrorGpuFatal((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinktlwerrorgpufatalConstruct_IMPL(struct NvlinkTlwErrorGpuFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinktlwerrorgpufatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinktlwerrorgpufatalConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkTlwErrorNonFatalContained;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkTlwErrorNonFatalContained {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkTlwErrorNonFatalContained *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkTlwErrorNonFatalContained *__nvoc_pbase_NvlinkTlwErrorNonFatalContained;    // nvlinktlwerrornonfatalcontained

    // Data members
    NvlinkTlwErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkTlwErrorNonFatalContained {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkTlwErrorNonFatalContained
#define __nvoc_class_id_NvlinkTlwErrorNonFatalContained 0xda4760u
typedef struct NvlinkTlwErrorNonFatalContained NvlinkTlwErrorNonFatalContained;
#endif /* __nvoc_class_id_NvlinkTlwErrorNonFatalContained */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTlwErrorNonFatalContained;

#define __staticCast_NvlinkTlwErrorNonFatalContained(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkTlwErrorNonFatalContained)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkTlwErrorNonFatalContained(pThis) ((NvlinkTlwErrorNonFatalContained*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkTlwErrorNonFatalContained(pThis) \
    ((NvlinkTlwErrorNonFatalContained*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkTlwErrorNonFatalContained)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkTlwErrorNonFatalContained(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkTlwErrorNonFatalContained(NvlinkTlwErrorNonFatalContained**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkTlwErrorNonFatalContained(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkTlwErrorNonFatalContained((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinktlwerrornonfatalcontainedConstruct_IMPL(struct NvlinkTlwErrorNonFatalContained *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinktlwerrornonfatalcontainedConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinktlwerrornonfatalcontainedConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkTlwErrorNonFatalWarning;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkTlwErrorNonFatalWarning {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkTlwErrorNonFatalWarning *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkTlwErrorNonFatalWarning *__nvoc_pbase_NvlinkTlwErrorNonFatalWarning;    // nvlinktlwerrornonfatalwarning

    // Data members
    NvlinkTlwErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkTlwErrorNonFatalWarning {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkTlwErrorNonFatalWarning
#define __nvoc_class_id_NvlinkTlwErrorNonFatalWarning 0x80867fu
typedef struct NvlinkTlwErrorNonFatalWarning NvlinkTlwErrorNonFatalWarning;
#endif /* __nvoc_class_id_NvlinkTlwErrorNonFatalWarning */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTlwErrorNonFatalWarning;

#define __staticCast_NvlinkTlwErrorNonFatalWarning(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkTlwErrorNonFatalWarning)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkTlwErrorNonFatalWarning(pThis) ((NvlinkTlwErrorNonFatalWarning*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkTlwErrorNonFatalWarning(pThis) \
    ((NvlinkTlwErrorNonFatalWarning*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkTlwErrorNonFatalWarning)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkTlwErrorNonFatalWarning(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkTlwErrorNonFatalWarning(NvlinkTlwErrorNonFatalWarning**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkTlwErrorNonFatalWarning(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkTlwErrorNonFatalWarning((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinktlwerrornonfatalwarningConstruct_IMPL(struct NvlinkTlwErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinktlwerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinktlwerrornonfatalwarningConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkTrexErrorNonFatalWarning;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkTrexErrorNonFatalWarning {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkTrexErrorNonFatalWarning *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkTrexErrorNonFatalWarning *__nvoc_pbase_NvlinkTrexErrorNonFatalWarning;    // nvlinktrexerrornonfatalwarning

    // Data members
    NvlinkTrexErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkTrexErrorNonFatalWarning {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkTrexErrorNonFatalWarning
#define __nvoc_class_id_NvlinkTrexErrorNonFatalWarning 0xc7edc3u
typedef struct NvlinkTrexErrorNonFatalWarning NvlinkTrexErrorNonFatalWarning;
#endif /* __nvoc_class_id_NvlinkTrexErrorNonFatalWarning */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkTrexErrorNonFatalWarning;

#define __staticCast_NvlinkTrexErrorNonFatalWarning(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkTrexErrorNonFatalWarning)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkTrexErrorNonFatalWarning(pThis) ((NvlinkTrexErrorNonFatalWarning*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkTrexErrorNonFatalWarning(pThis) \
    ((NvlinkTrexErrorNonFatalWarning*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkTrexErrorNonFatalWarning)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkTrexErrorNonFatalWarning(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkTrexErrorNonFatalWarning(NvlinkTrexErrorNonFatalWarning**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkTrexErrorNonFatalWarning(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkTrexErrorNonFatalWarning((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinktrexerrornonfatalwarningConstruct_IMPL(struct NvlinkTrexErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinktrexerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinktrexerrornonfatalwarningConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkNvlpwErrorNonFatalWarning;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkNvlpwErrorNonFatalWarning {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkNvlpwErrorNonFatalWarning *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkNvlpwErrorNonFatalWarning *__nvoc_pbase_NvlinkNvlpwErrorNonFatalWarning;    // nvlinknvlpwerrornonfatalwarning

    // Data members
    NvlinkNvlpwCtrlErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkNvlpwErrorNonFatalWarning {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkNvlpwErrorNonFatalWarning
#define __nvoc_class_id_NvlinkNvlpwErrorNonFatalWarning 0xe8f092u
typedef struct NvlinkNvlpwErrorNonFatalWarning NvlinkNvlpwErrorNonFatalWarning;
#endif /* __nvoc_class_id_NvlinkNvlpwErrorNonFatalWarning */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNvlpwErrorNonFatalWarning;

#define __staticCast_NvlinkNvlpwErrorNonFatalWarning(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkNvlpwErrorNonFatalWarning)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkNvlpwErrorNonFatalWarning(pThis) ((NvlinkNvlpwErrorNonFatalWarning*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkNvlpwErrorNonFatalWarning(pThis) \
    ((NvlinkNvlpwErrorNonFatalWarning*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkNvlpwErrorNonFatalWarning)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkNvlpwErrorNonFatalWarning(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkNvlpwErrorNonFatalWarning(NvlinkNvlpwErrorNonFatalWarning**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkNvlpwErrorNonFatalWarning(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkNvlpwErrorNonFatalWarning((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinknvlpwerrornonfatalwarningConstruct_IMPL(struct NvlinkNvlpwErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinknvlpwerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinknvlpwerrornonfatalwarningConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkNetirErrorNonFatalWarning;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkNetirErrorNonFatalWarning {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkNetirErrorNonFatalWarning *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkNetirErrorNonFatalWarning *__nvoc_pbase_NvlinkNetirErrorNonFatalWarning;    // nvlinknetirerrornonfatalwarning

    // Data members
    NvlinkNetirErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkNetirErrorNonFatalWarning {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkNetirErrorNonFatalWarning
#define __nvoc_class_id_NvlinkNetirErrorNonFatalWarning 0xdfc4a7u
typedef struct NvlinkNetirErrorNonFatalWarning NvlinkNetirErrorNonFatalWarning;
#endif /* __nvoc_class_id_NvlinkNetirErrorNonFatalWarning */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNetirErrorNonFatalWarning;

#define __staticCast_NvlinkNetirErrorNonFatalWarning(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkNetirErrorNonFatalWarning)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkNetirErrorNonFatalWarning(pThis) ((NvlinkNetirErrorNonFatalWarning*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkNetirErrorNonFatalWarning(pThis) \
    ((NvlinkNetirErrorNonFatalWarning*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkNetirErrorNonFatalWarning)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkNetirErrorNonFatalWarning(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkNetirErrorNonFatalWarning(NvlinkNetirErrorNonFatalWarning**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkNetirErrorNonFatalWarning(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkNetirErrorNonFatalWarning((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinknetirerrornonfatalwarningConstruct_IMPL(struct NvlinkNetirErrorNonFatalWarning *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinknetirerrornonfatalwarningConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinknetirerrornonfatalwarningConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkNetirErrorGpuFatal;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkNetirErrorGpuFatal {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkNetirErrorGpuFatal *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkNetirErrorGpuFatal *__nvoc_pbase_NvlinkNetirErrorGpuFatal;    // nvlinknetirerrorgpufatal

    // Data members
    NvlinkNetirErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkNetirErrorGpuFatal {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkNetirErrorGpuFatal
#define __nvoc_class_id_NvlinkNetirErrorGpuFatal 0xaa3699u
typedef struct NvlinkNetirErrorGpuFatal NvlinkNetirErrorGpuFatal;
#endif /* __nvoc_class_id_NvlinkNetirErrorGpuFatal */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNetirErrorGpuFatal;

#define __staticCast_NvlinkNetirErrorGpuFatal(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkNetirErrorGpuFatal)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkNetirErrorGpuFatal(pThis) ((NvlinkNetirErrorGpuFatal*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkNetirErrorGpuFatal(pThis) \
    ((NvlinkNetirErrorGpuFatal*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkNetirErrorGpuFatal)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkNetirErrorGpuFatal(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkNetirErrorGpuFatal(NvlinkNetirErrorGpuFatal**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkNetirErrorGpuFatal(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkNetirErrorGpuFatal((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinknetirerrorgpufatalConstruct_IMPL(struct NvlinkNetirErrorGpuFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinknetirerrorgpufatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinknetirerrorgpufatalConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NVLINK_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkNetirErrorLinkFatal;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkNetirErrorLinkFatal {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkNetirErrorLinkFatal *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuInterconnectError __nvoc_base_GpuInterconnectError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuInterconnectError *__nvoc_pbase_GpuInterconnectError;    // gpuinterconnecterr super
    struct NvlinkNetirErrorLinkFatal *__nvoc_pbase_NvlinkNetirErrorLinkFatal;    // nvlinknetirerrorlinkfatal

    // Data members
    NvlinkNetirErrorCtx context;
    NvlinkBlackwellErrorCtx blackwellContext;
    NvlinkBlackwellLegacyErrorCtx blackwellLegacyContext;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkNetirErrorLinkFatal {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkNetirErrorLinkFatal
#define __nvoc_class_id_NvlinkNetirErrorLinkFatal 0x8b8882u
typedef struct NvlinkNetirErrorLinkFatal NvlinkNetirErrorLinkFatal;
#endif /* __nvoc_class_id_NvlinkNetirErrorLinkFatal */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkNetirErrorLinkFatal;

#define __staticCast_NvlinkNetirErrorLinkFatal(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkNetirErrorLinkFatal)

#ifdef __nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkNetirErrorLinkFatal(pThis) ((NvlinkNetirErrorLinkFatal*) NULL)
#else //__nvoc_nvlink_events_h_disabled
#define __dynamicCast_NvlinkNetirErrorLinkFatal(pThis) \
    ((NvlinkNetirErrorLinkFatal*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkNetirErrorLinkFatal)))
#endif //__nvoc_nvlink_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkNetirErrorLinkFatal(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkNetirErrorLinkFatal(NvlinkNetirErrorLinkFatal**, Dynamic*, NvU32, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __objCreate_NvlinkNetirErrorLinkFatal(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) \
    __nvoc_objCreate_NvlinkNetirErrorLinkFatal((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for implementation functions
NV_STATUS nvlinknetirerrorlinkfatalConstruct_IMPL(struct NvlinkNetirErrorLinkFatal *pEvent, struct EventBus *pEventBus, NvS32 linkId, NvU32 errorStatus, NvU32 intrInfo, const NvU32 *pDebugData, NvU32 debugDataCount, NvU32 legacyXidCode, NvBool bFatal, NvBool bXContain, NvBool bInjected, const char *pSeverityString, const char *pLegacyXidErrorName);
#define __nvoc_nvlinknetirerrorlinkfatalConstruct(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName) nvlinknetirerrorlinkfatalConstruct_IMPL(pEvent, pEventBus, linkId, errorStatus, intrInfo, pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected, pSeverityString, pLegacyXidErrorName)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD




#endif // _NVLINK_EVENTS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_NVLINK_EVENTS_NVOC_H_
