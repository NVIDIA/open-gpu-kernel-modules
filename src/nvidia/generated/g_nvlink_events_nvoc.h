
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


#endif // _NVLINK_EVENTS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_NVLINK_EVENTS_NVOC_H_
