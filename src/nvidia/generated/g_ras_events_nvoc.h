
#ifndef _G_RAS_EVENTS_NVOC_H_
#define _G_RAS_EVENTS_NVOC_H_

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

#include "g_ras_events_nvoc.h"

#ifndef _RAS_EVENTS_H_
#define _RAS_EVENTS_H_

#include "events/gpu/gpu_events.h"
#include "events/gpu/ras/ras_event_defs.h"
#include "ctrl/ctrl2080/ctrl2080gpu.h"
#define ECC_ROW_REMAPPING_FAILURE_STRING_MAX_LENGTH 128U

typedef NvU32 ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON;
enum
{
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_INTERNAL_ERROR                 = 0x00U,
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_DISABLED_RETIREMENT            = 0x01U,
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_ROW_REMAP              = 0x02U,
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_BANK_REMAP             = 0x03U,
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_LTS_SWAP               = 0x04U,
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_ADDRESS_RETRIEVAL_ERROR        = 0x05U,
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_ECC_ADDRESS_TRACKING_ERROR     = 0x06U,
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_UNKNOWN_ERROR                  = 0x07U,
};

typedef struct RowRemappingCtx
{
    EventContextHeader header;
    NvU64 physicalAddress;
    NvU32 source;
} RowRemappingCtx;

typedef struct RowRemappingFailureCtx
{
    EventContextHeader header;
    NvU64 physicalAddress;
    char errorString[ECC_ROW_REMAPPING_FAILURE_STRING_MAX_LENGTH];
} RowRemappingFailureCtx;

typedef struct DramRetirementFailureInternalErrorCtx
{
    EventContextHeader header;
    NvU64 physicalAddress;
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON errorReason;
} DramRetirementFailureInternalErrorCtx;

typedef struct BankRemappingCtx
{
    EventContextHeader header;
    NvU32 source;
} BankRemappingCtx;

typedef struct LtsRepairCtx
{
    EventContextHeader header;
    NvU32 location;
    NvU32 sublocation;
    NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction;
    char errorString[ECC_ROW_REMAPPING_FAILURE_STRING_MAX_LENGTH];
} LtsRepairCtx;

typedef struct TpcRepairCtx
{
    EventContextHeader header;
    NvU32 location;
    NvU32 sublocation;
    NvU32 destinationLocation;
    NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction;
    char errorString[ECC_ROW_REMAPPING_FAILURE_STRING_MAX_LENGTH];
} TpcRepairCtx;

/* ------------------------ Row Remapping ---------------------------------------- */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemapPending;
struct NVOC_METADATA__GpuResourceRetirement;


struct RowRemapPending {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RowRemapPending *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceRetirement __nvoc_base_GpuResourceRetirement;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceRetirement *__nvoc_pbase_GpuResourceRetirement;    // gpuresret super
    struct RowRemapPending *__nvoc_pbase_RowRemapPending;    // rrpending

    // Data members
    RowRemappingCtx context;
    GpuXidEventCtx xid63;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemapPending {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceRetirement metadata__GpuResourceRetirement;
};

#ifndef __nvoc_class_id_RowRemapPending
#define __nvoc_class_id_RowRemapPending 0x670eb2u
typedef struct RowRemapPending RowRemapPending;
#endif /* __nvoc_class_id_RowRemapPending */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemapPending;

#define __staticCast_RowRemapPending(pThis) \
    ((pThis)->__nvoc_pbase_RowRemapPending)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemapPending(pThis) ((RowRemapPending*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemapPending(pThis) \
    ((RowRemapPending*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RowRemapPending)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RowRemapPending(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RowRemapPending(RowRemapPending**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 source, NvU64 physicalAddress);
#define __objCreate_RowRemapPending(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, source, physicalAddress) \
    __nvoc_objCreate_RowRemapPending((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, source, physicalAddress)


// Wrapper macros for implementation functions
NV_STATUS rrpendingConstruct_IMPL(struct RowRemapPending *pEvent, struct EventBus *pEventBus, NvU32 source, NvU64 physicalAddress);
#define __nvoc_rrpendingConstruct(pEvent, pEventBus, source, physicalAddress) rrpendingConstruct_IMPL(pEvent, pEventBus, source, physicalAddress)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingFailureTableFull;
struct NVOC_METADATA__GpuResourceExhausted;


struct RowRemappingFailureTableFull {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RowRemappingFailureTableFull *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct RowRemappingFailureTableFull *__nvoc_pbase_RowRemappingFailureTableFull;    // rrtablefull

    // Data members
    RowRemappingFailureCtx context;
    GpuXidEventCtx xid64;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingFailureTableFull {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_RowRemappingFailureTableFull
#define __nvoc_class_id_RowRemappingFailureTableFull 0x0f7b49u
typedef struct RowRemappingFailureTableFull RowRemappingFailureTableFull;
#endif /* __nvoc_class_id_RowRemappingFailureTableFull */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureTableFull;

#define __staticCast_RowRemappingFailureTableFull(pThis) \
    ((pThis)->__nvoc_pbase_RowRemappingFailureTableFull)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingFailureTableFull(pThis) ((RowRemappingFailureTableFull*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingFailureTableFull(pThis) \
    ((RowRemappingFailureTableFull*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RowRemappingFailureTableFull)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RowRemappingFailureTableFull(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RowRemappingFailureTableFull(RowRemappingFailureTableFull**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __objCreate_RowRemappingFailureTableFull(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress) \
    __nvoc_objCreate_RowRemappingFailureTableFull((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, physicalAddress)


// Wrapper macros for implementation functions
NV_STATUS rrtablefullConstruct_IMPL(struct RowRemappingFailureTableFull *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __nvoc_rrtablefullConstruct(pEvent, pEventBus, physicalAddress) rrtablefullConstruct_IMPL(pEvent, pEventBus, physicalAddress)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingFailureBankFull;
struct NVOC_METADATA__GpuResourceExhausted;


struct RowRemappingFailureBankFull {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RowRemappingFailureBankFull *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct RowRemappingFailureBankFull *__nvoc_pbase_RowRemappingFailureBankFull;    // rrbankfull

    // Data members
    RowRemappingFailureCtx context;
    GpuXidEventCtx xid64;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingFailureBankFull {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_RowRemappingFailureBankFull
#define __nvoc_class_id_RowRemappingFailureBankFull 0x1e6334u
typedef struct RowRemappingFailureBankFull RowRemappingFailureBankFull;
#endif /* __nvoc_class_id_RowRemappingFailureBankFull */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureBankFull;

#define __staticCast_RowRemappingFailureBankFull(pThis) \
    ((pThis)->__nvoc_pbase_RowRemappingFailureBankFull)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingFailureBankFull(pThis) ((RowRemappingFailureBankFull*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingFailureBankFull(pThis) \
    ((RowRemappingFailureBankFull*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RowRemappingFailureBankFull)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RowRemappingFailureBankFull(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RowRemappingFailureBankFull(RowRemappingFailureBankFull**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __objCreate_RowRemappingFailureBankFull(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress) \
    __nvoc_objCreate_RowRemappingFailureBankFull((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, physicalAddress)


// Wrapper macros for implementation functions
NV_STATUS rrbankfullConstruct_IMPL(struct RowRemappingFailureBankFull *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __nvoc_rrbankfullConstruct(pEvent, pEventBus, physicalAddress) rrbankfullConstruct_IMPL(pEvent, pEventBus, physicalAddress)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingFailureReservedRow;
struct NVOC_METADATA__GpuResourceExhausted;


struct RowRemappingFailureReservedRow {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RowRemappingFailureReservedRow *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct RowRemappingFailureReservedRow *__nvoc_pbase_RowRemappingFailureReservedRow;    // rrreservedrow

    // Data members
    RowRemappingFailureCtx context;
    GpuXidEventCtx xid64;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingFailureReservedRow {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_RowRemappingFailureReservedRow
#define __nvoc_class_id_RowRemappingFailureReservedRow 0xfa1d6au
typedef struct RowRemappingFailureReservedRow RowRemappingFailureReservedRow;
#endif /* __nvoc_class_id_RowRemappingFailureReservedRow */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureReservedRow;

#define __staticCast_RowRemappingFailureReservedRow(pThis) \
    ((pThis)->__nvoc_pbase_RowRemappingFailureReservedRow)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingFailureReservedRow(pThis) ((RowRemappingFailureReservedRow*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingFailureReservedRow(pThis) \
    ((RowRemappingFailureReservedRow*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RowRemappingFailureReservedRow)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RowRemappingFailureReservedRow(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RowRemappingFailureReservedRow(RowRemappingFailureReservedRow**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __objCreate_RowRemappingFailureReservedRow(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress) \
    __nvoc_objCreate_RowRemappingFailureReservedRow((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, physicalAddress)


// Wrapper macros for implementation functions
NV_STATUS rrreservedrowConstruct_IMPL(struct RowRemappingFailureReservedRow *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __nvoc_rrreservedrowConstruct(pEvent, pEventBus, physicalAddress) rrreservedrowConstruct_IMPL(pEvent, pEventBus, physicalAddress)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingFailureInternalError;
struct NVOC_METADATA__GpuUnclassifiedError;


struct RowRemappingFailureInternalError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RowRemappingFailureInternalError *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuUnclassifiedError __nvoc_base_GpuUnclassifiedError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuUnclassifiedError *__nvoc_pbase_GpuUnclassifiedError;    // gpuunclassifiederr super
    struct RowRemappingFailureInternalError *__nvoc_pbase_RowRemappingFailureInternalError;    // rrinternalerr

    // Data members
    RowRemappingFailureCtx context;
    GpuXidEventCtx xid64;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingFailureInternalError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuUnclassifiedError metadata__GpuUnclassifiedError;
};

#ifndef __nvoc_class_id_RowRemappingFailureInternalError
#define __nvoc_class_id_RowRemappingFailureInternalError 0xfd614fu
typedef struct RowRemappingFailureInternalError RowRemappingFailureInternalError;
#endif /* __nvoc_class_id_RowRemappingFailureInternalError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingFailureInternalError;

#define __staticCast_RowRemappingFailureInternalError(pThis) \
    ((pThis)->__nvoc_pbase_RowRemappingFailureInternalError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingFailureInternalError(pThis) ((RowRemappingFailureInternalError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingFailureInternalError(pThis) \
    ((RowRemappingFailureInternalError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RowRemappingFailureInternalError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RowRemappingFailureInternalError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RowRemappingFailureInternalError(RowRemappingFailureInternalError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 physicalAddress, const char *pErrorString);
#define __objCreate_RowRemappingFailureInternalError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress, pErrorString) \
    __nvoc_objCreate_RowRemappingFailureInternalError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, physicalAddress, pErrorString)


// Wrapper macros for implementation functions
NV_STATUS rrinternalerrConstruct_IMPL(struct RowRemappingFailureInternalError *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress, const char *pErrorString);
#define __nvoc_rrinternalerrConstruct(pEvent, pEventBus, physicalAddress, pErrorString) rrinternalerrConstruct_IMPL(pEvent, pEventBus, physicalAddress, pErrorString)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingPageOfflineFailure;
struct NVOC_METADATA__GpuUnclassifiedError;


struct RowRemappingPageOfflineFailure {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RowRemappingPageOfflineFailure *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuUnclassifiedError __nvoc_base_GpuUnclassifiedError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuUnclassifiedError *__nvoc_pbase_GpuUnclassifiedError;    // gpuunclassifiederr super
    struct RowRemappingPageOfflineFailure *__nvoc_pbase_RowRemappingPageOfflineFailure;    // rrpageofflinefailure

    // Data members
    RowRemappingFailureCtx context;
    GpuXidEventCtx xid64;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__RowRemappingPageOfflineFailure {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuUnclassifiedError metadata__GpuUnclassifiedError;
};

#ifndef __nvoc_class_id_RowRemappingPageOfflineFailure
#define __nvoc_class_id_RowRemappingPageOfflineFailure 0xa09afcu
typedef struct RowRemappingPageOfflineFailure RowRemappingPageOfflineFailure;
#endif /* __nvoc_class_id_RowRemappingPageOfflineFailure */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RowRemappingPageOfflineFailure;

#define __staticCast_RowRemappingPageOfflineFailure(pThis) \
    ((pThis)->__nvoc_pbase_RowRemappingPageOfflineFailure)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingPageOfflineFailure(pThis) ((RowRemappingPageOfflineFailure*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_RowRemappingPageOfflineFailure(pThis) \
    ((RowRemappingPageOfflineFailure*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RowRemappingPageOfflineFailure)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RowRemappingPageOfflineFailure(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RowRemappingPageOfflineFailure(RowRemappingPageOfflineFailure**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __objCreate_RowRemappingPageOfflineFailure(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress) \
    __nvoc_objCreate_RowRemappingPageOfflineFailure((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, physicalAddress)


// Wrapper macros for implementation functions
NV_STATUS rrpageofflinefailureConstruct_IMPL(struct RowRemappingPageOfflineFailure *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __nvoc_rrpageofflinefailureConstruct(pEvent, pEventBus, physicalAddress) rrpageofflinefailureConstruct_IMPL(pEvent, pEventBus, physicalAddress)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ------------------------ DRAM Retirement ---------------------------------------- */


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramRetirementFailureInternalError;
struct NVOC_METADATA__GpuUnclassifiedError;


struct DramRetirementFailureInternalError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DramRetirementFailureInternalError *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuUnclassifiedError __nvoc_base_GpuUnclassifiedError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuUnclassifiedError *__nvoc_pbase_GpuUnclassifiedError;    // gpuunclassifiederr super
    struct DramRetirementFailureInternalError *__nvoc_pbase_DramRetirementFailureInternalError;    // drinternalerr

    // Data members
    DramRetirementFailureInternalErrorCtx context;
    GpuXidEventCtx xid64;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramRetirementFailureInternalError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuUnclassifiedError metadata__GpuUnclassifiedError;
};

#ifndef __nvoc_class_id_DramRetirementFailureInternalError
#define __nvoc_class_id_DramRetirementFailureInternalError 0xffd4deu
typedef struct DramRetirementFailureInternalError DramRetirementFailureInternalError;
#endif /* __nvoc_class_id_DramRetirementFailureInternalError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureInternalError;

#define __staticCast_DramRetirementFailureInternalError(pThis) \
    ((pThis)->__nvoc_pbase_DramRetirementFailureInternalError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_DramRetirementFailureInternalError(pThis) ((DramRetirementFailureInternalError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_DramRetirementFailureInternalError(pThis) \
    ((DramRetirementFailureInternalError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DramRetirementFailureInternalError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DramRetirementFailureInternalError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DramRetirementFailureInternalError(DramRetirementFailureInternalError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 physicalAddress, ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON errorReason);
#define __objCreate_DramRetirementFailureInternalError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress, errorReason) \
    __nvoc_objCreate_DramRetirementFailureInternalError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, physicalAddress, errorReason)


// Wrapper macros for implementation functions
NV_STATUS drinternalerrConstruct_IMPL(struct DramRetirementFailureInternalError *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress, ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON errorReason);
#define __nvoc_drinternalerrConstruct(pEvent, pEventBus, physicalAddress, errorReason) drinternalerrConstruct_IMPL(pEvent, pEventBus, physicalAddress, errorReason)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramRetirementFailureInforomFull;
struct NVOC_METADATA__GpuResourceExhausted;


struct DramRetirementFailureInforomFull {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DramRetirementFailureInforomFull *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct DramRetirementFailureInforomFull *__nvoc_pbase_DramRetirementFailureInforomFull;    // drinforomfull

    // Data members
    RowRemappingFailureCtx context;
    GpuXidEventCtx xid64;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramRetirementFailureInforomFull {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_DramRetirementFailureInforomFull
#define __nvoc_class_id_DramRetirementFailureInforomFull 0xaeaec2u
typedef struct DramRetirementFailureInforomFull DramRetirementFailureInforomFull;
#endif /* __nvoc_class_id_DramRetirementFailureInforomFull */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureInforomFull;

#define __staticCast_DramRetirementFailureInforomFull(pThis) \
    ((pThis)->__nvoc_pbase_DramRetirementFailureInforomFull)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_DramRetirementFailureInforomFull(pThis) ((DramRetirementFailureInforomFull*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_DramRetirementFailureInforomFull(pThis) \
    ((DramRetirementFailureInforomFull*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DramRetirementFailureInforomFull)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DramRetirementFailureInforomFull(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DramRetirementFailureInforomFull(DramRetirementFailureInforomFull**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __objCreate_DramRetirementFailureInforomFull(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress) \
    __nvoc_objCreate_DramRetirementFailureInforomFull((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, physicalAddress)


// Wrapper macros for implementation functions
NV_STATUS drinforomfullConstruct_IMPL(struct DramRetirementFailureInforomFull *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __nvoc_drinforomfullConstruct(pEvent, pEventBus, physicalAddress) drinforomfullConstruct_IMPL(pEvent, pEventBus, physicalAddress)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramRetirementFailureHwLimit;
struct NVOC_METADATA__GpuResourceExhausted;


struct DramRetirementFailureHwLimit {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DramRetirementFailureHwLimit *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct DramRetirementFailureHwLimit *__nvoc_pbase_DramRetirementFailureHwLimit;    // drhwlimit

    // Data members
    RowRemappingFailureCtx context;
    GpuXidEventCtx xid64;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramRetirementFailureHwLimit {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_DramRetirementFailureHwLimit
#define __nvoc_class_id_DramRetirementFailureHwLimit 0x84944cu
typedef struct DramRetirementFailureHwLimit DramRetirementFailureHwLimit;
#endif /* __nvoc_class_id_DramRetirementFailureHwLimit */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureHwLimit;

#define __staticCast_DramRetirementFailureHwLimit(pThis) \
    ((pThis)->__nvoc_pbase_DramRetirementFailureHwLimit)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_DramRetirementFailureHwLimit(pThis) ((DramRetirementFailureHwLimit*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_DramRetirementFailureHwLimit(pThis) \
    ((DramRetirementFailureHwLimit*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DramRetirementFailureHwLimit)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DramRetirementFailureHwLimit(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DramRetirementFailureHwLimit(DramRetirementFailureHwLimit**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __objCreate_DramRetirementFailureHwLimit(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress) \
    __nvoc_objCreate_DramRetirementFailureHwLimit((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, physicalAddress)


// Wrapper macros for implementation functions
NV_STATUS drhwlimitConstruct_IMPL(struct DramRetirementFailureHwLimit *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __nvoc_drhwlimitConstruct(pEvent, pEventBus, physicalAddress) drhwlimitConstruct_IMPL(pEvent, pEventBus, physicalAddress)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramRetirementFailureNoSpare;
struct NVOC_METADATA__GpuResourceExhausted;


struct DramRetirementFailureNoSpare {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DramRetirementFailureNoSpare *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct DramRetirementFailureNoSpare *__nvoc_pbase_DramRetirementFailureNoSpare;    // drnospare

    // Data members
    RowRemappingFailureCtx context;
    GpuXidEventCtx xid64;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramRetirementFailureNoSpare {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_DramRetirementFailureNoSpare
#define __nvoc_class_id_DramRetirementFailureNoSpare 0x4da508u
typedef struct DramRetirementFailureNoSpare DramRetirementFailureNoSpare;
#endif /* __nvoc_class_id_DramRetirementFailureNoSpare */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramRetirementFailureNoSpare;

#define __staticCast_DramRetirementFailureNoSpare(pThis) \
    ((pThis)->__nvoc_pbase_DramRetirementFailureNoSpare)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_DramRetirementFailureNoSpare(pThis) ((DramRetirementFailureNoSpare*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_DramRetirementFailureNoSpare(pThis) \
    ((DramRetirementFailureNoSpare*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DramRetirementFailureNoSpare)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DramRetirementFailureNoSpare(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DramRetirementFailureNoSpare(DramRetirementFailureNoSpare**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __objCreate_DramRetirementFailureNoSpare(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, physicalAddress) \
    __nvoc_objCreate_DramRetirementFailureNoSpare((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, physicalAddress)


// Wrapper macros for implementation functions
NV_STATUS drnospareConstruct_IMPL(struct DramRetirementFailureNoSpare *pEvent, struct EventBus *pEventBus, NvU64 physicalAddress);
#define __nvoc_drnospareConstruct(pEvent, pEventBus, physicalAddress) drnospareConstruct_IMPL(pEvent, pEventBus, physicalAddress)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__BankRemappingPending;
struct NVOC_METADATA__GpuResourceRetirement;


struct BankRemappingPending {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__BankRemappingPending *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceRetirement __nvoc_base_GpuResourceRetirement;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceRetirement *__nvoc_pbase_GpuResourceRetirement;    // gpuresret super
    struct BankRemappingPending *__nvoc_pbase_BankRemappingPending;    // brpending

    // Data members
    BankRemappingCtx context;
    GpuXidEventCtx xid177;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__BankRemappingPending {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceRetirement metadata__GpuResourceRetirement;
};

#ifndef __nvoc_class_id_BankRemappingPending
#define __nvoc_class_id_BankRemappingPending 0x25375bu
typedef struct BankRemappingPending BankRemappingPending;
#endif /* __nvoc_class_id_BankRemappingPending */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_BankRemappingPending;

#define __staticCast_BankRemappingPending(pThis) \
    ((pThis)->__nvoc_pbase_BankRemappingPending)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_BankRemappingPending(pThis) ((BankRemappingPending*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_BankRemappingPending(pThis) \
    ((BankRemappingPending*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(BankRemappingPending)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_BankRemappingPending(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_BankRemappingPending(BankRemappingPending**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 source);
#define __objCreate_BankRemappingPending(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, source) \
    __nvoc_objCreate_BankRemappingPending((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, source)


// Wrapper macros for implementation functions
NV_STATUS brpendingConstruct_IMPL(struct BankRemappingPending *pEvent, struct EventBus *pEventBus, NvU32 source);
#define __nvoc_brpendingConstruct(pEvent, pEventBus, source) brpendingConstruct_IMPL(pEvent, pEventBus, source)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ------------------------ LTS/Channel Repair  ---------------------------------------- */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtsRepairPending;
struct NVOC_METADATA__GpuResourceRetirement;


struct LtsRepairPending {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LtsRepairPending *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceRetirement __nvoc_base_GpuResourceRetirement;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceRetirement *__nvoc_pbase_GpuResourceRetirement;    // gpuresret super
    struct LtsRepairPending *__nvoc_pbase_LtsRepairPending;    // ltsrepairpending

    // Data members
    LtsRepairCtx context;
    GpuXidEventCtx xid160;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtsRepairPending {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceRetirement metadata__GpuResourceRetirement;
};

#ifndef __nvoc_class_id_LtsRepairPending
#define __nvoc_class_id_LtsRepairPending 0xff6030u
typedef struct LtsRepairPending LtsRepairPending;
#endif /* __nvoc_class_id_LtsRepairPending */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtsRepairPending;

#define __staticCast_LtsRepairPending(pThis) \
    ((pThis)->__nvoc_pbase_LtsRepairPending)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_LtsRepairPending(pThis) ((LtsRepairPending*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_LtsRepairPending(pThis) \
    ((LtsRepairPending*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LtsRepairPending)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LtsRepairPending(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LtsRepairPending(LtsRepairPending**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString);
#define __objCreate_LtsRepairPending(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, sublocation, location, recoveryAction, pErrorString) \
    __nvoc_objCreate_LtsRepairPending((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, sublocation, location, recoveryAction, pErrorString)


// Wrapper macros for implementation functions
NV_STATUS ltsrepairpendingConstruct_IMPL(struct LtsRepairPending *pEvent, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString);
#define __nvoc_ltsrepairpendingConstruct(pEvent, pEventBus, sublocation, location, recoveryAction, pErrorString) ltsrepairpendingConstruct_IMPL(pEvent, pEventBus, sublocation, location, recoveryAction, pErrorString)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemoryChannelRepairPending;
struct NVOC_METADATA__GpuResourceRetirement;


struct MemoryChannelRepairPending {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MemoryChannelRepairPending *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceRetirement __nvoc_base_GpuResourceRetirement;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceRetirement *__nvoc_pbase_GpuResourceRetirement;    // gpuresret super
    struct MemoryChannelRepairPending *__nvoc_pbase_MemoryChannelRepairPending;    // memorychannelrepairpending

    // Data members
    LtsRepairCtx context;
    GpuXidEventCtx xid160;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemoryChannelRepairPending {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceRetirement metadata__GpuResourceRetirement;
};

#ifndef __nvoc_class_id_MemoryChannelRepairPending
#define __nvoc_class_id_MemoryChannelRepairPending 0xe88f92u
typedef struct MemoryChannelRepairPending MemoryChannelRepairPending;
#endif /* __nvoc_class_id_MemoryChannelRepairPending */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryChannelRepairPending;

#define __staticCast_MemoryChannelRepairPending(pThis) \
    ((pThis)->__nvoc_pbase_MemoryChannelRepairPending)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_MemoryChannelRepairPending(pThis) ((MemoryChannelRepairPending*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_MemoryChannelRepairPending(pThis) \
    ((MemoryChannelRepairPending*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryChannelRepairPending)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MemoryChannelRepairPending(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryChannelRepairPending(MemoryChannelRepairPending**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString);
#define __objCreate_MemoryChannelRepairPending(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, sublocation, location, recoveryAction, pErrorString) \
    __nvoc_objCreate_MemoryChannelRepairPending((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, sublocation, location, recoveryAction, pErrorString)


// Wrapper macros for implementation functions
NV_STATUS memorychannelrepairpendingConstruct_IMPL(struct MemoryChannelRepairPending *pEvent, struct EventBus *pEventBus, NvU32 sublocation, NvU32 location, NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction, const char *pErrorString);
#define __nvoc_memorychannelrepairpendingConstruct(pEvent, pEventBus, sublocation, location, recoveryAction, pErrorString) memorychannelrepairpendingConstruct_IMPL(pEvent, pEventBus, sublocation, location, recoveryAction, pErrorString)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtsRepairFailure;
struct NVOC_METADATA__GpuResourceExhausted;


struct LtsRepairFailure {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LtsRepairFailure *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct LtsRepairFailure *__nvoc_pbase_LtsRepairFailure;    // ltsrepairfailure

    // Data members
    LtsRepairCtx context;
    GpuXidEventCtx xid161;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtsRepairFailure {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_LtsRepairFailure
#define __nvoc_class_id_LtsRepairFailure 0x7df945u
typedef struct LtsRepairFailure LtsRepairFailure;
#endif /* __nvoc_class_id_LtsRepairFailure */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtsRepairFailure;

#define __staticCast_LtsRepairFailure(pThis) \
    ((pThis)->__nvoc_pbase_LtsRepairFailure)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_LtsRepairFailure(pThis) ((LtsRepairFailure*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_LtsRepairFailure(pThis) \
    ((LtsRepairFailure*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LtsRepairFailure)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LtsRepairFailure(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LtsRepairFailure(LtsRepairFailure**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_LtsRepairFailure(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_LtsRepairFailure((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS ltsrepairfailureConstruct_IMPL(struct LtsRepairFailure *pEvent, struct EventBus *pEventBus);
#define __nvoc_ltsrepairfailureConstruct(pEvent, pEventBus) ltsrepairfailureConstruct_IMPL(pEvent, pEventBus)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemoryChannelRepairFailure;
struct NVOC_METADATA__GpuResourceExhausted;


struct MemoryChannelRepairFailure {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MemoryChannelRepairFailure *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct MemoryChannelRepairFailure *__nvoc_pbase_MemoryChannelRepairFailure;    // memorychannelrepairfailure

    // Data members
    LtsRepairCtx context;
    GpuXidEventCtx xid161;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MemoryChannelRepairFailure {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_MemoryChannelRepairFailure
#define __nvoc_class_id_MemoryChannelRepairFailure 0xa9a3e9u
typedef struct MemoryChannelRepairFailure MemoryChannelRepairFailure;
#endif /* __nvoc_class_id_MemoryChannelRepairFailure */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MemoryChannelRepairFailure;

#define __staticCast_MemoryChannelRepairFailure(pThis) \
    ((pThis)->__nvoc_pbase_MemoryChannelRepairFailure)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_MemoryChannelRepairFailure(pThis) ((MemoryChannelRepairFailure*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_MemoryChannelRepairFailure(pThis) \
    ((MemoryChannelRepairFailure*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MemoryChannelRepairFailure)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MemoryChannelRepairFailure(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MemoryChannelRepairFailure(MemoryChannelRepairFailure**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_MemoryChannelRepairFailure(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_MemoryChannelRepairFailure((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS memorychannelrepairfailureConstruct_IMPL(struct MemoryChannelRepairFailure *pEvent, struct EventBus *pEventBus);
#define __nvoc_memorychannelrepairfailureConstruct(pEvent, pEventBus) memorychannelrepairfailureConstruct_IMPL(pEvent, pEventBus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ------------------------ TPC Repair  ---------------------------------------- */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TpcRepairPendingSameGpc;
struct NVOC_METADATA__GpuResourceRetirement;


struct TpcRepairPendingSameGpc {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__TpcRepairPendingSameGpc *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceRetirement __nvoc_base_GpuResourceRetirement;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceRetirement *__nvoc_pbase_GpuResourceRetirement;    // gpuresret super
    struct TpcRepairPendingSameGpc *__nvoc_pbase_TpcRepairPendingSameGpc;    // tpcrepairpendingsamegpc

    // Data members
    TpcRepairCtx context;
    GpuXidEventCtx xid156;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TpcRepairPendingSameGpc {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceRetirement metadata__GpuResourceRetirement;
};

#ifndef __nvoc_class_id_TpcRepairPendingSameGpc
#define __nvoc_class_id_TpcRepairPendingSameGpc 0x758a69u
typedef struct TpcRepairPendingSameGpc TpcRepairPendingSameGpc;
#endif /* __nvoc_class_id_TpcRepairPendingSameGpc */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairPendingSameGpc;

#define __staticCast_TpcRepairPendingSameGpc(pThis) \
    ((pThis)->__nvoc_pbase_TpcRepairPendingSameGpc)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_TpcRepairPendingSameGpc(pThis) ((TpcRepairPendingSameGpc*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_TpcRepairPendingSameGpc(pThis) \
    ((TpcRepairPendingSameGpc*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(TpcRepairPendingSameGpc)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_TpcRepairPendingSameGpc(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_TpcRepairPendingSameGpc(TpcRepairPendingSameGpc**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
#define __objCreate_TpcRepairPendingSameGpc(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, tpcId, gpcId) \
    __nvoc_objCreate_TpcRepairPendingSameGpc((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, tpcId, gpcId)


// Wrapper macros for implementation functions
NV_STATUS tpcrepairpendingsamegpcConstruct_IMPL(struct TpcRepairPendingSameGpc *pEvent, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
#define __nvoc_tpcrepairpendingsamegpcConstruct(pEvent, pEventBus, tpcId, gpcId) tpcrepairpendingsamegpcConstruct_IMPL(pEvent, pEventBus, tpcId, gpcId)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TpcRepairPendingDifferentGpc;
struct NVOC_METADATA__GpuResourceRetirement;


struct TpcRepairPendingDifferentGpc {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__TpcRepairPendingDifferentGpc *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceRetirement __nvoc_base_GpuResourceRetirement;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceRetirement *__nvoc_pbase_GpuResourceRetirement;    // gpuresret super
    struct TpcRepairPendingDifferentGpc *__nvoc_pbase_TpcRepairPendingDifferentGpc;    // tpcrepairpendingdifferentgpc

    // Data members
    TpcRepairCtx context;
    GpuXidEventCtx xid156;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TpcRepairPendingDifferentGpc {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceRetirement metadata__GpuResourceRetirement;
};

#ifndef __nvoc_class_id_TpcRepairPendingDifferentGpc
#define __nvoc_class_id_TpcRepairPendingDifferentGpc 0x98d3e3u
typedef struct TpcRepairPendingDifferentGpc TpcRepairPendingDifferentGpc;
#endif /* __nvoc_class_id_TpcRepairPendingDifferentGpc */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairPendingDifferentGpc;

#define __staticCast_TpcRepairPendingDifferentGpc(pThis) \
    ((pThis)->__nvoc_pbase_TpcRepairPendingDifferentGpc)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_TpcRepairPendingDifferentGpc(pThis) ((TpcRepairPendingDifferentGpc*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_TpcRepairPendingDifferentGpc(pThis) \
    ((TpcRepairPendingDifferentGpc*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(TpcRepairPendingDifferentGpc)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_TpcRepairPendingDifferentGpc(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_TpcRepairPendingDifferentGpc(TpcRepairPendingDifferentGpc**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
#define __objCreate_TpcRepairPendingDifferentGpc(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, tpcId, gpcId) \
    __nvoc_objCreate_TpcRepairPendingDifferentGpc((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, tpcId, gpcId)


// Wrapper macros for implementation functions
NV_STATUS tpcrepairpendingdifferentgpcConstruct_IMPL(struct TpcRepairPendingDifferentGpc *pEvent, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
#define __nvoc_tpcrepairpendingdifferentgpcConstruct(pEvent, pEventBus, tpcId, gpcId) tpcrepairpendingdifferentgpcConstruct_IMPL(pEvent, pEventBus, tpcId, gpcId)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TpcRepairFailureNoSpare;
struct NVOC_METADATA__GpuResourceExhausted;


struct TpcRepairFailureNoSpare {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__TpcRepairFailureNoSpare *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct TpcRepairFailureNoSpare *__nvoc_pbase_TpcRepairFailureNoSpare;    // tpcrepairfailurenospare

    // Data members
    TpcRepairCtx context;
    GpuXidEventCtx xid157;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TpcRepairFailureNoSpare {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_TpcRepairFailureNoSpare
#define __nvoc_class_id_TpcRepairFailureNoSpare 0x78f4feu
typedef struct TpcRepairFailureNoSpare TpcRepairFailureNoSpare;
#endif /* __nvoc_class_id_TpcRepairFailureNoSpare */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairFailureNoSpare;

#define __staticCast_TpcRepairFailureNoSpare(pThis) \
    ((pThis)->__nvoc_pbase_TpcRepairFailureNoSpare)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_TpcRepairFailureNoSpare(pThis) ((TpcRepairFailureNoSpare*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_TpcRepairFailureNoSpare(pThis) \
    ((TpcRepairFailureNoSpare*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(TpcRepairFailureNoSpare)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_TpcRepairFailureNoSpare(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_TpcRepairFailureNoSpare(TpcRepairFailureNoSpare**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
#define __objCreate_TpcRepairFailureNoSpare(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, tpcId, gpcId) \
    __nvoc_objCreate_TpcRepairFailureNoSpare((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, tpcId, gpcId)


// Wrapper macros for implementation functions
NV_STATUS tpcrepairfailurenospareConstruct_IMPL(struct TpcRepairFailureNoSpare *pEvent, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
#define __nvoc_tpcrepairfailurenospareConstruct(pEvent, pEventBus, tpcId, gpcId) tpcrepairfailurenospareConstruct_IMPL(pEvent, pEventBus, tpcId, gpcId)


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
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TpcRepairFailureNoSpareMig;
struct NVOC_METADATA__GpuResourceExhausted;


struct TpcRepairFailureNoSpareMig {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__TpcRepairFailureNoSpareMig *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResourceExhausted __nvoc_base_GpuResourceExhausted;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuResourceExhausted *__nvoc_pbase_GpuResourceExhausted;    // gpuresexh super
    struct TpcRepairFailureNoSpareMig *__nvoc_pbase_TpcRepairFailureNoSpareMig;    // tpcrepairfailurenosparemig

    // Data members
    TpcRepairCtx context;
    GpuXidEventCtx xid157;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__TpcRepairFailureNoSpareMig {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_TpcRepairFailureNoSpareMig
#define __nvoc_class_id_TpcRepairFailureNoSpareMig 0xb6fda9u
typedef struct TpcRepairFailureNoSpareMig TpcRepairFailureNoSpareMig;
#endif /* __nvoc_class_id_TpcRepairFailureNoSpareMig */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_TpcRepairFailureNoSpareMig;

#define __staticCast_TpcRepairFailureNoSpareMig(pThis) \
    ((pThis)->__nvoc_pbase_TpcRepairFailureNoSpareMig)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_TpcRepairFailureNoSpareMig(pThis) ((TpcRepairFailureNoSpareMig*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_TpcRepairFailureNoSpareMig(pThis) \
    ((TpcRepairFailureNoSpareMig*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(TpcRepairFailureNoSpareMig)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_TpcRepairFailureNoSpareMig(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_TpcRepairFailureNoSpareMig(TpcRepairFailureNoSpareMig**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
#define __objCreate_TpcRepairFailureNoSpareMig(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, tpcId, gpcId) \
    __nvoc_objCreate_TpcRepairFailureNoSpareMig((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, tpcId, gpcId)


// Wrapper macros for implementation functions
NV_STATUS tpcrepairfailurenosparemigConstruct_IMPL(struct TpcRepairFailureNoSpareMig *pEvent, struct EventBus *pEventBus, NvU32 tpcId, NvU32 gpcId);
#define __nvoc_tpcrepairfailurenosparemigConstruct(pEvent, pEventBus, tpcId, gpcId) tpcrepairfailurenosparemigConstruct_IMPL(pEvent, pEventBus, tpcId, gpcId)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ------------------------ Misc ---------------------------------------- */
typedef struct DramEccIntrStormCtx
{
    EventContextHeader header;
    NvU32 fbpaIndexVirt;
} DramEccIntrStormCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramEccIntrStorm;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct DramEccIntrStorm {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DramEccIntrStorm *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuMemoryIntegrityError __nvoc_base_GpuMemoryIntegrityError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuMemoryIntegrityError *__nvoc_pbase_GpuMemoryIntegrityError;    // gpumemintegrityerr super
    struct DramEccIntrStorm *__nvoc_pbase_DramEccIntrStorm;    // cedramstorm

    // Data members
    DramEccIntrStormCtx context;
    GpuXidEventCtx xid92;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramEccIntrStorm {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_DramEccIntrStorm
#define __nvoc_class_id_DramEccIntrStorm 0x51ad4eu
typedef struct DramEccIntrStorm DramEccIntrStorm;
#endif /* __nvoc_class_id_DramEccIntrStorm */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramEccIntrStorm;

#define __staticCast_DramEccIntrStorm(pThis) \
    ((pThis)->__nvoc_pbase_DramEccIntrStorm)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_DramEccIntrStorm(pThis) ((DramEccIntrStorm*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_DramEccIntrStorm(pThis) \
    ((DramEccIntrStorm*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DramEccIntrStorm)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DramEccIntrStorm(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DramEccIntrStorm(DramEccIntrStorm**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 fbpaIndexVirt);
#define __objCreate_DramEccIntrStorm(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, fbpaIndexVirt) \
    __nvoc_objCreate_DramEccIntrStorm((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, fbpaIndexVirt)


// Wrapper macros for implementation functions
NV_STATUS cedramstormConstruct_IMPL(struct DramEccIntrStorm *pEvent, struct EventBus *pEventBus, NvU32 fbpaIndexVirt);
#define __nvoc_cedramstormConstruct(pEvent, pEventBus, fbpaIndexVirt) cedramstormConstruct_IMPL(pEvent, pEventBus, fbpaIndexVirt)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


typedef struct SmEccIntrStormCtx
{
    EventContextHeader header;
} SmEccIntrStormCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SmEccIntrStorm;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct SmEccIntrStorm {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SmEccIntrStorm *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuMemoryIntegrityError __nvoc_base_GpuMemoryIntegrityError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuMemoryIntegrityError *__nvoc_pbase_GpuMemoryIntegrityError;    // gpumemintegrityerr super
    struct SmEccIntrStorm *__nvoc_pbase_SmEccIntrStorm;    // cesmstorm

    // Data members
    SmEccIntrStormCtx context;
    GpuXidEventCtx xid92;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SmEccIntrStorm {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_SmEccIntrStorm
#define __nvoc_class_id_SmEccIntrStorm 0xb4edfdu
typedef struct SmEccIntrStorm SmEccIntrStorm;
#endif /* __nvoc_class_id_SmEccIntrStorm */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SmEccIntrStorm;

#define __staticCast_SmEccIntrStorm(pThis) \
    ((pThis)->__nvoc_pbase_SmEccIntrStorm)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_SmEccIntrStorm(pThis) ((SmEccIntrStorm*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_SmEccIntrStorm(pThis) \
    ((SmEccIntrStorm*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SmEccIntrStorm)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SmEccIntrStorm(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SmEccIntrStorm(SmEccIntrStorm**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_SmEccIntrStorm(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_SmEccIntrStorm((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS cesmstormConstruct_IMPL(struct SmEccIntrStorm *pEvent, struct EventBus *pEventBus);
#define __nvoc_cesmstormConstruct(pEvent, pEventBus) cesmstormConstruct_IMPL(pEvent, pEventBus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


typedef struct EccResidualUncorrectableErrorCtx
{
    EventContextHeader header;
    NvU32 dramCount;
    NvU32 ltcCount;
    NvU32 mmuCount;
    NvU32 pcieCount;
} EccResidualUncorrectableErrorCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__EccResidualUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct EccResidualUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__EccResidualUncorrectableError *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuMemoryIntegrityError __nvoc_base_GpuMemoryIntegrityError;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuMemoryIntegrityError *__nvoc_pbase_GpuMemoryIntegrityError;    // gpumemintegrityerr super
    struct EccResidualUncorrectableError *__nvoc_pbase_EccResidualUncorrectableError;    // eccresidual

    // Data members
    EccResidualUncorrectableErrorCtx context;
    GpuXidEventCtx xid140;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__EccResidualUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_EccResidualUncorrectableError
#define __nvoc_class_id_EccResidualUncorrectableError 0x162294u
typedef struct EccResidualUncorrectableError EccResidualUncorrectableError;
#endif /* __nvoc_class_id_EccResidualUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_EccResidualUncorrectableError;

#define __staticCast_EccResidualUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_EccResidualUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_EccResidualUncorrectableError(pThis) ((EccResidualUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_EccResidualUncorrectableError(pThis) \
    ((EccResidualUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(EccResidualUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_EccResidualUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_EccResidualUncorrectableError(EccResidualUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 severity, NvU32 dramCount, NvU32 ltcCount, NvU32 mmuCount, NvU32 pcieCount);
#define __objCreate_EccResidualUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, severity, dramCount, ltcCount, mmuCount, pcieCount) \
    __nvoc_objCreate_EccResidualUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, severity, dramCount, ltcCount, mmuCount, pcieCount)


// Wrapper macros for implementation functions
NV_STATUS eccresidualConstruct_IMPL(struct EccResidualUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 severity, NvU32 dramCount, NvU32 ltcCount, NvU32 mmuCount, NvU32 pcieCount);
#define __nvoc_eccresidualConstruct(pEvent, pEventBus, severity, dramCount, ltcCount, mmuCount, pcieCount) eccresidualConstruct_IMPL(pEvent, pEventBus, severity, dramCount, ltcCount, mmuCount, pcieCount)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _RAS_EVENTS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_RAS_EVENTS_NVOC_H_
