
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
#include "gpu/error_cont.h"
#define ECC_ROW_REMAPPING_FAILURE_STRING_MAX_LENGTH 128U
#define ECC_DRAM_BANK_REMAP_XID_VALUE_INVALID 0xFFFFFFFFU

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

typedef struct DramEccIntrStormCtx
{
    EventContextHeader header;
    NvU32 fbpaIndexVirt;
} DramEccIntrStormCtx;

typedef struct SmEccIntrStormCtx
{
    EventContextHeader header;
} SmEccIntrStormCtx;

typedef struct EccResidualUncorrectableErrorCtx
{
    EventContextHeader header;
    NvU32 dramCount;
    NvU32 ltcCount;
    NvU32 mmuCount;
    NvU32 pcieCount;
} EccResidualUncorrectableErrorCtx;

typedef enum L2_ERROR_TYPE
{
    L2_ERROR_TYPE_DSTG = 0,
    L2_ERROR_TYPE_TSTG = 1,
    L2_ERROR_TYPE_RSTG = 2,
    L2_ERROR_TYPE_UNKNOWN = 3,
} L2_ERROR_TYPE;

typedef struct LtcErrorCtx
{
    EventContextHeader header;
    NvU32 dieletType;
    NvU32 dieletId;
    NvU32 ltc;
    NvU32 slice;
    NvBool bvidL2;
    L2_ERROR_TYPE errorType;
} LtcErrorCtx;

typedef struct LtcUnsupportedClientPoisonErrorCtx
{
    EventContextHeader header;
    NvU32 partition;
    NvU32 slice;
} LtcUnsupportedClientPoisonErrorCtx;

typedef struct LrcFatalErrorCtx
{
    EventContextHeader header;
    NvU32 partition;
    NvU32 slice;
} LrcFatalErrorCtx;

typedef struct PcieIngressPoisonContainedErrorCtx
{
    EventContextHeader header;
    NvU32 gfid;
} PcieIngressPoisonContainedErrorCtx;

typedef struct PciePoisonErrorCtx
{
    EventContextHeader header;
} PciePoisonErrorCtx;

typedef struct FbHubPoisonErrorCtx
{
    EventContextHeader header;
} FbHubPoisonErrorCtx;

typedef struct MmuPoisonErrorCtx
{
    EventContextHeader header;
} MmuPoisonErrorCtx;

typedef struct SmPoisonErrorCtx
{
    EventContextHeader header;
    NvU32 nv2080EngineType;
} SmPoisonErrorCtx;

typedef struct GccPoisonErrorCtx
{
    EventContextHeader header;
    NvU32 nv2080EngineType;
} GccPoisonErrorCtx;

typedef struct ContextSwitchPoisonErrorCtx
{
    EventContextHeader header;
    NvU32 nv2080EngineType;
} ContextSwitchPoisonErrorCtx;

typedef struct GspPoisonErrorCtx
{
    EventContextHeader header;
} GspPoisonErrorCtx;

typedef struct PmuPoisonErrorCtx
{
    EventContextHeader header;
} PmuPoisonErrorCtx;

typedef struct Sec2PoisonErrorCtx
{
    EventContextHeader header;
} Sec2PoisonErrorCtx;

typedef struct FbFalconPoisonErrorCtx
{
    EventContextHeader header;
} FbFalconPoisonErrorCtx;

typedef struct VideoEnginePoisonErrorCtx
{
    EventContextHeader header;
    NvU32 nv2080EngineType;
} VideoEnginePoisonErrorCtx;

typedef struct CopyEnginePoisonErrorCtx
{
    EventContextHeader header;
    NvU32 nv2080EngineType;
} CopyEnginePoisonErrorCtx;

typedef struct DramErrorCtx
{
    EventContextHeader header;
    NvU32 fbpa;
    NvU32 sublocation;
    NvBool bHbm;
    NvU32 encodedBankRemapRegVal;
    NvU64 physicalAddress;
} DramErrorCtx;

typedef struct DramCbcErrorCtx
{
    EventContextHeader header;
    NvU32 ltcPart;
    NvU32 slice;
    NvU32 fbpaPart;
    NvU32 fbpaSubloc;
    NvBool bHbm;
    NvU64 physicalAddress;
} DramCbcErrorCtx;

typedef struct EccUnitUncorrectableErrorCtx
{
    EventContextHeader header;
    NvU32 unit;
} EccUnitUncorrectableErrorCtx;

typedef struct CopyEngineUncorrectableErrorCtx
{
    EventContextHeader header;
    NvU32 dieletType;
    NvU32 dieletId;
    NvU32 pce;
} CopyEngineUncorrectableErrorCtx;

typedef struct SmUncorrectableErrorCtx
{
    EventContextHeader header;
    NvU32 unit;
    NvU32 gpc;
    NvU32 tpc;
} SmUncorrectableErrorCtx;

typedef struct GrUncorrectableErrorCtx
{
    EventContextHeader header;
    NvU32 gpc;
} GrUncorrectableErrorCtx;

typedef struct RiscvUncorrectableErrorCtx
{
    EventContextHeader header;
    NvU32 location;
    NvBool bHasLocation;
} RiscvUncorrectableErrorCtx;

typedef struct PmuUncorrectableErrorCtx
{
    EventContextHeader header;
    NvU32 location;
    NvBool bHasLocation;
    NvBool bIsFalcon;
} PmuUncorrectableErrorCtx;

typedef struct LpsecUncorrectableErrorCtx
{
    EventContextHeader header;
    NvU32 sequencer;
} LpsecUncorrectableErrorCtx;

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


NV_STATUS rrpendingDeserialize(OBJGPU *pGpu,
                               const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
                               const NvU8 *pContextData,
                               NvU32 contextDataSize);
NV_STATUS rrtablefullDeserialize(OBJGPU *pGpu,
                                 const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
                                 const NvU8 *pContextData,
                                 NvU32 contextDataSize);


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


/* ------------------------ Sparing Repair  ------------------------------------ */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SparingRepairPending;
struct NVOC_METADATA__GpuResourceRetirement;


struct SparingRepairPending {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SparingRepairPending *__nvoc_metadata_ptr;
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
    struct SparingRepairPending *__nvoc_pbase_SparingRepairPending;    // sparingrepairpending

    // Data members
    GpuXidEventCtx xid156;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SparingRepairPending {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceRetirement metadata__GpuResourceRetirement;
};

#ifndef __nvoc_class_id_SparingRepairPending
#define __nvoc_class_id_SparingRepairPending 0xbe89e4u
typedef struct SparingRepairPending SparingRepairPending;
#endif /* __nvoc_class_id_SparingRepairPending */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SparingRepairPending;

#define __staticCast_SparingRepairPending(pThis) \
    ((pThis)->__nvoc_pbase_SparingRepairPending)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_SparingRepairPending(pThis) ((SparingRepairPending*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_SparingRepairPending(pThis) \
    ((SparingRepairPending*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SparingRepairPending)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SparingRepairPending(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SparingRepairPending(SparingRepairPending**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_SparingRepairPending(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_SparingRepairPending((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS sparingrepairpendingConstruct_IMPL(struct SparingRepairPending *pEvent, struct EventBus *pEventBus);
#define __nvoc_sparingrepairpendingConstruct(pEvent, pEventBus) sparingrepairpendingConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__SparingRepairNotPossible;
struct NVOC_METADATA__GpuResourceExhausted;


struct SparingRepairNotPossible {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SparingRepairNotPossible *__nvoc_metadata_ptr;
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
    struct SparingRepairNotPossible *__nvoc_pbase_SparingRepairNotPossible;    // sparingrepairnotpossible

    // Data members
    GpuXidEventCtx xid157;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SparingRepairNotPossible {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResourceExhausted metadata__GpuResourceExhausted;
};

#ifndef __nvoc_class_id_SparingRepairNotPossible
#define __nvoc_class_id_SparingRepairNotPossible 0xa29d84u
typedef struct SparingRepairNotPossible SparingRepairNotPossible;
#endif /* __nvoc_class_id_SparingRepairNotPossible */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SparingRepairNotPossible;

#define __staticCast_SparingRepairNotPossible(pThis) \
    ((pThis)->__nvoc_pbase_SparingRepairNotPossible)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_SparingRepairNotPossible(pThis) ((SparingRepairNotPossible*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_SparingRepairNotPossible(pThis) \
    ((SparingRepairNotPossible*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SparingRepairNotPossible)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SparingRepairNotPossible(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SparingRepairNotPossible(SparingRepairNotPossible**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_SparingRepairNotPossible(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_SparingRepairNotPossible((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS sparingrepairnotpossibleConstruct_IMPL(struct SparingRepairNotPossible *pEvent, struct EventBus *pEventBus);
#define __nvoc_sparingrepairnotpossibleConstruct(pEvent, pEventBus) sparingrepairnotpossibleConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__SparingEvent;
struct NVOC_METADATA__GpuStateChanged;


struct SparingEvent {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SparingEvent *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuStateChanged __nvoc_base_GpuStateChanged;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^5
    struct Event *__nvoc_pbase_Event;    // event super^4
    struct OperationalEvent *__nvoc_pbase_OperationalEvent;    // opevt super^3
    struct GpuOperationalEvent *__nvoc_pbase_GpuOperationalEvent;    // gpuopevt super^2
    struct GpuStateChanged *__nvoc_pbase_GpuStateChanged;    // gpustatechanged super
    struct SparingEvent *__nvoc_pbase_SparingEvent;    // sparingevent

    // Data members
    GpuXidEventCtx xid184;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SparingEvent {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuStateChanged metadata__GpuStateChanged;
};

#ifndef __nvoc_class_id_SparingEvent
#define __nvoc_class_id_SparingEvent 0xe78eddu
typedef struct SparingEvent SparingEvent;
#endif /* __nvoc_class_id_SparingEvent */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SparingEvent;

#define __staticCast_SparingEvent(pThis) \
    ((pThis)->__nvoc_pbase_SparingEvent)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_SparingEvent(pThis) ((SparingEvent*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_SparingEvent(pThis) \
    ((SparingEvent*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SparingEvent)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SparingEvent(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SparingEvent(SparingEvent**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_SparingEvent(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_SparingEvent((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS sparingeventConstruct_IMPL(struct SparingEvent *pEvent, struct EventBus *pEventBus);
#define __nvoc_sparingeventConstruct(pEvent, pEventBus) sparingeventConstruct_IMPL(pEvent, pEventBus)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ------------------------ MemSys ECC errors ---------------------------- */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtcError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct LtcError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LtcError *__nvoc_metadata_ptr;
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
    struct LtcError *__nvoc_pbase_LtcError;    // ltcerr

    // Data members
    LtcErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtcError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_LtcError
#define __nvoc_class_id_LtcError 0x62b176u
typedef struct LtcError LtcError;
#endif /* __nvoc_class_id_LtcError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtcError;

#define __staticCast_LtcError(pThis) \
    ((pThis)->__nvoc_pbase_LtcError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_LtcError(pThis) ((LtcError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_LtcError(pThis) \
    ((LtcError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LtcError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LtcError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LtcError(LtcError**, Dynamic*, NvU32, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc);
#define __objCreate_LtcError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc) \
    __nvoc_objCreate_LtcError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, loc)


// Wrapper macros for implementation functions
NV_STATUS ltcerrConstruct_IMPL(struct LtcError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc);
#define __nvoc_ltcerrConstruct(pEvent, pEventBus, loc) ltcerrConstruct_IMPL(pEvent, pEventBus, loc)


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
struct NVOC_METADATA__DramError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct DramError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DramError *__nvoc_metadata_ptr;
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
    struct DramError *__nvoc_pbase_DramError;    // dramerr

    // Data members
    DramErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid171;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_DramError
#define __nvoc_class_id_DramError 0x3080d3u
typedef struct DramError DramError;
#endif /* __nvoc_class_id_DramError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramError;

#define __staticCast_DramError(pThis) \
    ((pThis)->__nvoc_pbase_DramError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_DramError(pThis) ((DramError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_DramError(pThis) \
    ((DramError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DramError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DramError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DramError(DramError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvBool bHbm, NvU32 encodedBankRemapRegVal);
#define __objCreate_DramError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, fbpa, sublocation, physicalAddress, bHbm, encodedBankRemapRegVal) \
    __nvoc_objCreate_DramError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, fbpa, sublocation, physicalAddress, bHbm, encodedBankRemapRegVal)


// Wrapper macros for implementation functions
NV_STATUS dramerrConstruct_IMPL(struct DramError *pEvent, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvBool bHbm, NvU32 encodedBankRemapRegVal);
#define __nvoc_dramerrConstruct(pEvent, pEventBus, fbpa, sublocation, physicalAddress, bHbm, encodedBankRemapRegVal) dramerrConstruct_IMPL(pEvent, pEventBus, fbpa, sublocation, physicalAddress, bHbm, encodedBankRemapRegVal)


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
struct NVOC_METADATA__DramSidebandError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct DramSidebandError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DramSidebandError *__nvoc_metadata_ptr;
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
    struct DramSidebandError *__nvoc_pbase_DramSidebandError;    // dramsidebanderr

    // Data members
    DramErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid171;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramSidebandError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_DramSidebandError
#define __nvoc_class_id_DramSidebandError 0xa8a87cu
typedef struct DramSidebandError DramSidebandError;
#endif /* __nvoc_class_id_DramSidebandError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramSidebandError;

#define __staticCast_DramSidebandError(pThis) \
    ((pThis)->__nvoc_pbase_DramSidebandError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_DramSidebandError(pThis) ((DramSidebandError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_DramSidebandError(pThis) \
    ((DramSidebandError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DramSidebandError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DramSidebandError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DramSidebandError(DramSidebandError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvU32 encodedBankRemapRegVal);
#define __objCreate_DramSidebandError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, fbpa, sublocation, physicalAddress, encodedBankRemapRegVal) \
    __nvoc_objCreate_DramSidebandError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, fbpa, sublocation, physicalAddress, encodedBankRemapRegVal)


// Wrapper macros for implementation functions
NV_STATUS dramsidebanderrConstruct_IMPL(struct DramSidebandError *pEvent, struct EventBus *pEventBus, NvU32 fbpa, NvU32 sublocation, NvU64 physicalAddress, NvU32 encodedBankRemapRegVal);
#define __nvoc_dramsidebanderrConstruct(pEvent, pEventBus, fbpa, sublocation, physicalAddress, encodedBankRemapRegVal) dramsidebanderrConstruct_IMPL(pEvent, pEventBus, fbpa, sublocation, physicalAddress, encodedBankRemapRegVal)


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
struct NVOC_METADATA__DramCbcFatalError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct DramCbcFatalError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DramCbcFatalError *__nvoc_metadata_ptr;
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
    struct DramCbcFatalError *__nvoc_pbase_DramCbcFatalError;    // dramcbcfatalerr

    // Data members
    DramCbcErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid171;
    GpuXidEventCtx xid95;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramCbcFatalError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_DramCbcFatalError
#define __nvoc_class_id_DramCbcFatalError 0x1689b9u
typedef struct DramCbcFatalError DramCbcFatalError;
#endif /* __nvoc_class_id_DramCbcFatalError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramCbcFatalError;

#define __staticCast_DramCbcFatalError(pThis) \
    ((pThis)->__nvoc_pbase_DramCbcFatalError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_DramCbcFatalError(pThis) ((DramCbcFatalError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_DramCbcFatalError(pThis) \
    ((DramCbcFatalError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DramCbcFatalError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DramCbcFatalError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DramCbcFatalError(DramCbcFatalError**, Dynamic*, NvU32, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm);
#define __objCreate_DramCbcFatalError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc, bHbm) \
    __nvoc_objCreate_DramCbcFatalError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, loc, bHbm)


// Wrapper macros for implementation functions
NV_STATUS dramcbcfatalerrConstruct_IMPL(struct DramCbcFatalError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm);
#define __nvoc_dramcbcfatalerrConstruct(pEvent, pEventBus, loc, bHbm) dramcbcfatalerrConstruct_IMPL(pEvent, pEventBus, loc, bHbm)


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
struct NVOC_METADATA__DramCbcNonFatalError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct DramCbcNonFatalError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__DramCbcNonFatalError *__nvoc_metadata_ptr;
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
    struct DramCbcNonFatalError *__nvoc_pbase_DramCbcNonFatalError;    // dramcbcnonfatalerr

    // Data members
    DramCbcErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid171;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__DramCbcNonFatalError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_DramCbcNonFatalError
#define __nvoc_class_id_DramCbcNonFatalError 0xb1540au
typedef struct DramCbcNonFatalError DramCbcNonFatalError;
#endif /* __nvoc_class_id_DramCbcNonFatalError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DramCbcNonFatalError;

#define __staticCast_DramCbcNonFatalError(pThis) \
    ((pThis)->__nvoc_pbase_DramCbcNonFatalError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_DramCbcNonFatalError(pThis) ((DramCbcNonFatalError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_DramCbcNonFatalError(pThis) \
    ((DramCbcNonFatalError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(DramCbcNonFatalError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_DramCbcNonFatalError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_DramCbcNonFatalError(DramCbcNonFatalError**, Dynamic*, NvU32, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm);
#define __objCreate_DramCbcNonFatalError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc, bHbm) \
    __nvoc_objCreate_DramCbcNonFatalError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, loc, bHbm)


// Wrapper macros for implementation functions
NV_STATUS dramcbcnonfatalerrConstruct_IMPL(struct DramCbcNonFatalError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, NvBool bHbm);
#define __nvoc_dramcbcnonfatalerrConstruct(pEvent, pEventBus, loc, bHbm) dramcbcnonfatalerrConstruct_IMPL(pEvent, pEventBus, loc, bHbm)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


// Intentionally not uncontained since the L2 will go into containment

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtcFatalError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct LtcFatalError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LtcFatalError *__nvoc_metadata_ptr;
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
    struct LtcFatalError *__nvoc_pbase_LtcFatalError;    // ltcfatalerr

    // Data members
    LtcErrorCtx context;
    GpuXidEventCtx xid95;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtcFatalError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_LtcFatalError
#define __nvoc_class_id_LtcFatalError 0x9cb3f5u
typedef struct LtcFatalError LtcFatalError;
#endif /* __nvoc_class_id_LtcFatalError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtcFatalError;

#define __staticCast_LtcFatalError(pThis) \
    ((pThis)->__nvoc_pbase_LtcFatalError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_LtcFatalError(pThis) ((LtcFatalError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_LtcFatalError(pThis) \
    ((LtcFatalError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LtcFatalError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LtcFatalError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LtcFatalError(LtcFatalError**, Dynamic*, NvU32, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, L2_ERROR_TYPE errorType);
#define __objCreate_LtcFatalError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc, errorType) \
    __nvoc_objCreate_LtcFatalError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, loc, errorType)


// Wrapper macros for implementation functions
NV_STATUS ltcfatalerrConstruct_IMPL(struct LtcFatalError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc, L2_ERROR_TYPE errorType);
#define __nvoc_ltcfatalerrConstruct(pEvent, pEventBus, loc, errorType) ltcfatalerrConstruct_IMPL(pEvent, pEventBus, loc, errorType)


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
struct NVOC_METADATA__LtcUnsupportedClientPoisonUncontainedError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct LtcUnsupportedClientPoisonUncontainedError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LtcUnsupportedClientPoisonUncontainedError *__nvoc_metadata_ptr;
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
    struct LtcUnsupportedClientPoisonUncontainedError *__nvoc_pbase_LtcUnsupportedClientPoisonUncontainedError;    // ltcclientpoisonuncontainederr

    // Data members
    LtcUnsupportedClientPoisonErrorCtx context;
    GpuXidEventCtx xid95;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtcUnsupportedClientPoisonUncontainedError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_LtcUnsupportedClientPoisonUncontainedError
#define __nvoc_class_id_LtcUnsupportedClientPoisonUncontainedError 0xd5e463u
typedef struct LtcUnsupportedClientPoisonUncontainedError LtcUnsupportedClientPoisonUncontainedError;
#endif /* __nvoc_class_id_LtcUnsupportedClientPoisonUncontainedError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtcUnsupportedClientPoisonUncontainedError;

#define __staticCast_LtcUnsupportedClientPoisonUncontainedError(pThis) \
    ((pThis)->__nvoc_pbase_LtcUnsupportedClientPoisonUncontainedError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_LtcUnsupportedClientPoisonUncontainedError(pThis) ((LtcUnsupportedClientPoisonUncontainedError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_LtcUnsupportedClientPoisonUncontainedError(pThis) \
    ((LtcUnsupportedClientPoisonUncontainedError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LtcUnsupportedClientPoisonUncontainedError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LtcUnsupportedClientPoisonUncontainedError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LtcUnsupportedClientPoisonUncontainedError(LtcUnsupportedClientPoisonUncontainedError**, Dynamic*, NvU32, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc);
#define __objCreate_LtcUnsupportedClientPoisonUncontainedError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc) \
    __nvoc_objCreate_LtcUnsupportedClientPoisonUncontainedError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, loc)


// Wrapper macros for implementation functions
NV_STATUS ltcclientpoisonuncontainederrConstruct_IMPL(struct LtcUnsupportedClientPoisonUncontainedError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc);
#define __nvoc_ltcclientpoisonuncontainederrConstruct(pEvent, pEventBus, loc) ltcclientpoisonuncontainederrConstruct_IMPL(pEvent, pEventBus, loc)


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
struct NVOC_METADATA__LtcUnsupportedClientPoisonContainedError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct LtcUnsupportedClientPoisonContainedError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LtcUnsupportedClientPoisonContainedError *__nvoc_metadata_ptr;
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
    struct LtcUnsupportedClientPoisonContainedError *__nvoc_pbase_LtcUnsupportedClientPoisonContainedError;    // ltcclientpoisoncontainederr

    // Data members
    LtcUnsupportedClientPoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LtcUnsupportedClientPoisonContainedError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_LtcUnsupportedClientPoisonContainedError
#define __nvoc_class_id_LtcUnsupportedClientPoisonContainedError 0x355a88u
typedef struct LtcUnsupportedClientPoisonContainedError LtcUnsupportedClientPoisonContainedError;
#endif /* __nvoc_class_id_LtcUnsupportedClientPoisonContainedError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LtcUnsupportedClientPoisonContainedError;

#define __staticCast_LtcUnsupportedClientPoisonContainedError(pThis) \
    ((pThis)->__nvoc_pbase_LtcUnsupportedClientPoisonContainedError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_LtcUnsupportedClientPoisonContainedError(pThis) ((LtcUnsupportedClientPoisonContainedError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_LtcUnsupportedClientPoisonContainedError(pThis) \
    ((LtcUnsupportedClientPoisonContainedError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LtcUnsupportedClientPoisonContainedError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LtcUnsupportedClientPoisonContainedError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LtcUnsupportedClientPoisonContainedError(LtcUnsupportedClientPoisonContainedError**, Dynamic*, NvU32, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc);
#define __objCreate_LtcUnsupportedClientPoisonContainedError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, loc) \
    __nvoc_objCreate_LtcUnsupportedClientPoisonContainedError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, loc)


// Wrapper macros for implementation functions
NV_STATUS ltcclientpoisoncontainederrConstruct_IMPL(struct LtcUnsupportedClientPoisonContainedError *pEvent, struct EventBus *pEventBus, NV_ERROR_CONT_LOCATION loc);
#define __nvoc_ltcclientpoisoncontainederrConstruct(pEvent, pEventBus, loc) ltcclientpoisoncontainederrConstruct_IMPL(pEvent, pEventBus, loc)


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
struct NVOC_METADATA__LrcFatalError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct LrcFatalError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LrcFatalError *__nvoc_metadata_ptr;
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
    struct LrcFatalError *__nvoc_pbase_LrcFatalError;    // lrcfatalerr

    // Data members
    LrcFatalErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
    GpuXidEventCtx xid95;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LrcFatalError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_LrcFatalError
#define __nvoc_class_id_LrcFatalError 0x075c90u
typedef struct LrcFatalError LrcFatalError;
#endif /* __nvoc_class_id_LrcFatalError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LrcFatalError;

#define __staticCast_LrcFatalError(pThis) \
    ((pThis)->__nvoc_pbase_LrcFatalError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_LrcFatalError(pThis) ((LrcFatalError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_LrcFatalError(pThis) \
    ((LrcFatalError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LrcFatalError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LrcFatalError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LrcFatalError(LrcFatalError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 partition, NvU32 slice);
#define __objCreate_LrcFatalError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, partition, slice) \
    __nvoc_objCreate_LrcFatalError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, partition, slice)


// Wrapper macros for implementation functions
NV_STATUS lrcfatalerrConstruct_IMPL(struct LrcFatalError *pEvent, struct EventBus *pEventBus, NvU32 partition, NvU32 slice);
#define __nvoc_lrcfatalerrConstruct(pEvent, pEventBus, partition, slice) lrcfatalerrConstruct_IMPL(pEvent, pEventBus, partition, slice)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* ------------------------ Poison errors ---------------------------- */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieIngressPoisonContainedError;
struct NVOC_METADATA__GpuInterconnectError;


struct PcieIngressPoisonContainedError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PcieIngressPoisonContainedError *__nvoc_metadata_ptr;
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
    struct PcieIngressPoisonContainedError *__nvoc_pbase_PcieIngressPoisonContainedError;    // pcieingresspoisoncontainederr

    // Data members
    PcieIngressPoisonContainedErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieIngressPoisonContainedError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_PcieIngressPoisonContainedError
#define __nvoc_class_id_PcieIngressPoisonContainedError 0xfce4f3u
typedef struct PcieIngressPoisonContainedError PcieIngressPoisonContainedError;
#endif /* __nvoc_class_id_PcieIngressPoisonContainedError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieIngressPoisonContainedError;

#define __staticCast_PcieIngressPoisonContainedError(pThis) \
    ((pThis)->__nvoc_pbase_PcieIngressPoisonContainedError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_PcieIngressPoisonContainedError(pThis) ((PcieIngressPoisonContainedError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_PcieIngressPoisonContainedError(pThis) \
    ((PcieIngressPoisonContainedError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PcieIngressPoisonContainedError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PcieIngressPoisonContainedError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PcieIngressPoisonContainedError(PcieIngressPoisonContainedError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 gfid);
#define __objCreate_PcieIngressPoisonContainedError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, gfid) \
    __nvoc_objCreate_PcieIngressPoisonContainedError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, gfid)


// Wrapper macros for implementation functions
NV_STATUS pcieingresspoisoncontainederrConstruct_IMPL(struct PcieIngressPoisonContainedError *pEvent, struct EventBus *pEventBus, NvU32 gfid);
#define __nvoc_pcieingresspoisoncontainederrConstruct(pEvent, pEventBus, gfid) pcieingresspoisoncontainederrConstruct_IMPL(pEvent, pEventBus, gfid)


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
struct NVOC_METADATA__PcieIngressPoisonUncontainedError;
struct NVOC_METADATA__GpuInterconnectError;


struct PcieIngressPoisonUncontainedError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PcieIngressPoisonUncontainedError *__nvoc_metadata_ptr;
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
    struct PcieIngressPoisonUncontainedError *__nvoc_pbase_PcieIngressPoisonUncontainedError;    // pcieingresspoisonuncontainederr

    // Data members
    PciePoisonErrorCtx context;
    GpuXidEventCtx xid95;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieIngressPoisonUncontainedError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_PcieIngressPoisonUncontainedError
#define __nvoc_class_id_PcieIngressPoisonUncontainedError 0xf20fadu
typedef struct PcieIngressPoisonUncontainedError PcieIngressPoisonUncontainedError;
#endif /* __nvoc_class_id_PcieIngressPoisonUncontainedError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieIngressPoisonUncontainedError;

#define __staticCast_PcieIngressPoisonUncontainedError(pThis) \
    ((pThis)->__nvoc_pbase_PcieIngressPoisonUncontainedError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_PcieIngressPoisonUncontainedError(pThis) ((PcieIngressPoisonUncontainedError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_PcieIngressPoisonUncontainedError(pThis) \
    ((PcieIngressPoisonUncontainedError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PcieIngressPoisonUncontainedError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PcieIngressPoisonUncontainedError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PcieIngressPoisonUncontainedError(PcieIngressPoisonUncontainedError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_PcieIngressPoisonUncontainedError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_PcieIngressPoisonUncontainedError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS pcieingresspoisonuncontainederrConstruct_IMPL(struct PcieIngressPoisonUncontainedError *pEvent, struct EventBus *pEventBus);
#define __nvoc_pcieingresspoisonuncontainederrConstruct(pEvent, pEventBus) pcieingresspoisonuncontainederrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__PcieEgressPoisonError;
struct NVOC_METADATA__GpuInterconnectError;


struct PcieEgressPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PcieEgressPoisonError *__nvoc_metadata_ptr;
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
    struct PcieEgressPoisonError *__nvoc_pbase_PcieEgressPoisonError;    // pcieegresspoisonerr

    // Data members
    PciePoisonErrorCtx context;
    GpuXidEventCtx xid95;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieEgressPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_PcieEgressPoisonError
#define __nvoc_class_id_PcieEgressPoisonError 0x5f1cc9u
typedef struct PcieEgressPoisonError PcieEgressPoisonError;
#endif /* __nvoc_class_id_PcieEgressPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieEgressPoisonError;

#define __staticCast_PcieEgressPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_PcieEgressPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_PcieEgressPoisonError(pThis) ((PcieEgressPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_PcieEgressPoisonError(pThis) \
    ((PcieEgressPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PcieEgressPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PcieEgressPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PcieEgressPoisonError(PcieEgressPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_PcieEgressPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_PcieEgressPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS pcieegresspoisonerrConstruct_IMPL(struct PcieEgressPoisonError *pEvent, struct EventBus *pEventBus);
#define __nvoc_pcieegresspoisonerrConstruct(pEvent, pEventBus) pcieegresspoisonerrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__CopyEnginePoisonContainedError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct CopyEnginePoisonContainedError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__CopyEnginePoisonContainedError *__nvoc_metadata_ptr;
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
    struct CopyEnginePoisonContainedError *__nvoc_pbase_CopyEnginePoisonContainedError;    // copyenginepoisoncontainederr

    // Data members
    CopyEnginePoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CopyEnginePoisonContainedError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_CopyEnginePoisonContainedError
#define __nvoc_class_id_CopyEnginePoisonContainedError 0x4823f5u
typedef struct CopyEnginePoisonContainedError CopyEnginePoisonContainedError;
#endif /* __nvoc_class_id_CopyEnginePoisonContainedError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CopyEnginePoisonContainedError;

#define __staticCast_CopyEnginePoisonContainedError(pThis) \
    ((pThis)->__nvoc_pbase_CopyEnginePoisonContainedError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_CopyEnginePoisonContainedError(pThis) ((CopyEnginePoisonContainedError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_CopyEnginePoisonContainedError(pThis) \
    ((CopyEnginePoisonContainedError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CopyEnginePoisonContainedError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CopyEnginePoisonContainedError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CopyEnginePoisonContainedError(CopyEnginePoisonContainedError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __objCreate_CopyEnginePoisonContainedError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType) \
    __nvoc_objCreate_CopyEnginePoisonContainedError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, nv2080EngineType)


// Wrapper macros for implementation functions
NV_STATUS copyenginepoisoncontainederrConstruct_IMPL(struct CopyEnginePoisonContainedError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __nvoc_copyenginepoisoncontainederrConstruct(pEvent, pEventBus, nv2080EngineType) copyenginepoisoncontainederrConstruct_IMPL(pEvent, pEventBus, nv2080EngineType)


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
struct NVOC_METADATA__CopyEnginePoisonUncontainedError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct CopyEnginePoisonUncontainedError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__CopyEnginePoisonUncontainedError *__nvoc_metadata_ptr;
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
    struct CopyEnginePoisonUncontainedError *__nvoc_pbase_CopyEnginePoisonUncontainedError;    // copyenginepoisonuncontainederr

    // Data members
    CopyEnginePoisonErrorCtx context;
    GpuXidEventCtx xid95;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CopyEnginePoisonUncontainedError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_CopyEnginePoisonUncontainedError
#define __nvoc_class_id_CopyEnginePoisonUncontainedError 0xcd5e25u
typedef struct CopyEnginePoisonUncontainedError CopyEnginePoisonUncontainedError;
#endif /* __nvoc_class_id_CopyEnginePoisonUncontainedError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CopyEnginePoisonUncontainedError;

#define __staticCast_CopyEnginePoisonUncontainedError(pThis) \
    ((pThis)->__nvoc_pbase_CopyEnginePoisonUncontainedError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_CopyEnginePoisonUncontainedError(pThis) ((CopyEnginePoisonUncontainedError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_CopyEnginePoisonUncontainedError(pThis) \
    ((CopyEnginePoisonUncontainedError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CopyEnginePoisonUncontainedError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CopyEnginePoisonUncontainedError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CopyEnginePoisonUncontainedError(CopyEnginePoisonUncontainedError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __objCreate_CopyEnginePoisonUncontainedError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType) \
    __nvoc_objCreate_CopyEnginePoisonUncontainedError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, nv2080EngineType)


// Wrapper macros for implementation functions
NV_STATUS copyenginepoisonuncontainederrConstruct_IMPL(struct CopyEnginePoisonUncontainedError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __nvoc_copyenginepoisonuncontainederrConstruct(pEvent, pEventBus, nv2080EngineType) copyenginepoisonuncontainederrConstruct_IMPL(pEvent, pEventBus, nv2080EngineType)


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
struct NVOC_METADATA__SmPoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct SmPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SmPoisonError *__nvoc_metadata_ptr;
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
    struct SmPoisonError *__nvoc_pbase_SmPoisonError;    // smpoisonerr

    // Data members
    SmPoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SmPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_SmPoisonError
#define __nvoc_class_id_SmPoisonError 0xec940du
typedef struct SmPoisonError SmPoisonError;
#endif /* __nvoc_class_id_SmPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SmPoisonError;

#define __staticCast_SmPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_SmPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_SmPoisonError(pThis) ((SmPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_SmPoisonError(pThis) \
    ((SmPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SmPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SmPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SmPoisonError(SmPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __objCreate_SmPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType) \
    __nvoc_objCreate_SmPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, nv2080EngineType)


// Wrapper macros for implementation functions
NV_STATUS smpoisonerrConstruct_IMPL(struct SmPoisonError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __nvoc_smpoisonerrConstruct(pEvent, pEventBus, nv2080EngineType) smpoisonerrConstruct_IMPL(pEvent, pEventBus, nv2080EngineType)


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
struct NVOC_METADATA__GccPoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct GccPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GccPoisonError *__nvoc_metadata_ptr;
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
    struct GccPoisonError *__nvoc_pbase_GccPoisonError;    // gccpoisonerr

    // Data members
    GccPoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GccPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_GccPoisonError
#define __nvoc_class_id_GccPoisonError 0x055ca3u
typedef struct GccPoisonError GccPoisonError;
#endif /* __nvoc_class_id_GccPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GccPoisonError;

#define __staticCast_GccPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_GccPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_GccPoisonError(pThis) ((GccPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_GccPoisonError(pThis) \
    ((GccPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GccPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GccPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GccPoisonError(GccPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __objCreate_GccPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType) \
    __nvoc_objCreate_GccPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, nv2080EngineType)


// Wrapper macros for implementation functions
NV_STATUS gccpoisonerrConstruct_IMPL(struct GccPoisonError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __nvoc_gccpoisonerrConstruct(pEvent, pEventBus, nv2080EngineType) gccpoisonerrConstruct_IMPL(pEvent, pEventBus, nv2080EngineType)


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
struct NVOC_METADATA__ContextSwitchPoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct ContextSwitchPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ContextSwitchPoisonError *__nvoc_metadata_ptr;
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
    struct ContextSwitchPoisonError *__nvoc_pbase_ContextSwitchPoisonError;    // ctxswpoisonerr

    // Data members
    ContextSwitchPoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ContextSwitchPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_ContextSwitchPoisonError
#define __nvoc_class_id_ContextSwitchPoisonError 0x8f50d0u
typedef struct ContextSwitchPoisonError ContextSwitchPoisonError;
#endif /* __nvoc_class_id_ContextSwitchPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ContextSwitchPoisonError;

#define __staticCast_ContextSwitchPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_ContextSwitchPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_ContextSwitchPoisonError(pThis) ((ContextSwitchPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_ContextSwitchPoisonError(pThis) \
    ((ContextSwitchPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ContextSwitchPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ContextSwitchPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ContextSwitchPoisonError(ContextSwitchPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __objCreate_ContextSwitchPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType) \
    __nvoc_objCreate_ContextSwitchPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, nv2080EngineType)


// Wrapper macros for implementation functions
NV_STATUS ctxswpoisonerrConstruct_IMPL(struct ContextSwitchPoisonError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __nvoc_ctxswpoisonerrConstruct(pEvent, pEventBus, nv2080EngineType) ctxswpoisonerrConstruct_IMPL(pEvent, pEventBus, nv2080EngineType)


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
struct NVOC_METADATA__MmuPoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct MmuPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MmuPoisonError *__nvoc_metadata_ptr;
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
    struct MmuPoisonError *__nvoc_pbase_MmuPoisonError;    // mmupoisonerr

    // Data members
    MmuPoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MmuPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_MmuPoisonError
#define __nvoc_class_id_MmuPoisonError 0x9ef007u
typedef struct MmuPoisonError MmuPoisonError;
#endif /* __nvoc_class_id_MmuPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MmuPoisonError;

#define __staticCast_MmuPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_MmuPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_MmuPoisonError(pThis) ((MmuPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_MmuPoisonError(pThis) \
    ((MmuPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MmuPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MmuPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MmuPoisonError(MmuPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_MmuPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_MmuPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS mmupoisonerrConstruct_IMPL(struct MmuPoisonError *pEvent, struct EventBus *pEventBus);
#define __nvoc_mmupoisonerrConstruct(pEvent, pEventBus) mmupoisonerrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__FbHubFatalPoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct FbHubFatalPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__FbHubFatalPoisonError *__nvoc_metadata_ptr;
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
    struct FbHubFatalPoisonError *__nvoc_pbase_FbHubFatalPoisonError;    // fbhubfatalpoisonerr

    // Data members
    FbHubPoisonErrorCtx context;
    GpuXidEventCtx xid95;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FbHubFatalPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_FbHubFatalPoisonError
#define __nvoc_class_id_FbHubFatalPoisonError 0x2e9cc4u
typedef struct FbHubFatalPoisonError FbHubFatalPoisonError;
#endif /* __nvoc_class_id_FbHubFatalPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_FbHubFatalPoisonError;

#define __staticCast_FbHubFatalPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_FbHubFatalPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_FbHubFatalPoisonError(pThis) ((FbHubFatalPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_FbHubFatalPoisonError(pThis) \
    ((FbHubFatalPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FbHubFatalPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_FbHubFatalPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FbHubFatalPoisonError(FbHubFatalPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_FbHubFatalPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_FbHubFatalPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS fbhubfatalpoisonerrConstruct_IMPL(struct FbHubFatalPoisonError *pEvent, struct EventBus *pEventBus);
#define __nvoc_fbhubfatalpoisonerrConstruct(pEvent, pEventBus) fbhubfatalpoisonerrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__FbHubNonFatalPoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct FbHubNonFatalPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__FbHubNonFatalPoisonError *__nvoc_metadata_ptr;
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
    struct FbHubNonFatalPoisonError *__nvoc_pbase_FbHubNonFatalPoisonError;    // fbhubnonfatalpoisonerr

    // Data members
    FbHubPoisonErrorCtx context;
    GpuXidEventCtx xid95;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FbHubNonFatalPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_FbHubNonFatalPoisonError
#define __nvoc_class_id_FbHubNonFatalPoisonError 0x148a19u
typedef struct FbHubNonFatalPoisonError FbHubNonFatalPoisonError;
#endif /* __nvoc_class_id_FbHubNonFatalPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_FbHubNonFatalPoisonError;

#define __staticCast_FbHubNonFatalPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_FbHubNonFatalPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_FbHubNonFatalPoisonError(pThis) ((FbHubNonFatalPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_FbHubNonFatalPoisonError(pThis) \
    ((FbHubNonFatalPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FbHubNonFatalPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_FbHubNonFatalPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FbHubNonFatalPoisonError(FbHubNonFatalPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_FbHubNonFatalPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_FbHubNonFatalPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS fbhubnonfatalpoisonerrConstruct_IMPL(struct FbHubNonFatalPoisonError *pEvent, struct EventBus *pEventBus);
#define __nvoc_fbhubnonfatalpoisonerrConstruct(pEvent, pEventBus) fbhubnonfatalpoisonerrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__GspPoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct GspPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GspPoisonError *__nvoc_metadata_ptr;
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
    struct GspPoisonError *__nvoc_pbase_GspPoisonError;    // gsppoisonerr

    // Data members
    GspPoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_GspPoisonError
#define __nvoc_class_id_GspPoisonError 0x690d4bu
typedef struct GspPoisonError GspPoisonError;
#endif /* __nvoc_class_id_GspPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspPoisonError;

#define __staticCast_GspPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_GspPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_GspPoisonError(pThis) ((GspPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_GspPoisonError(pThis) \
    ((GspPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GspPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GspPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GspPoisonError(GspPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_GspPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_GspPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS gsppoisonerrConstruct_IMPL(struct GspPoisonError *pEvent, struct EventBus *pEventBus);
#define __nvoc_gsppoisonerrConstruct(pEvent, pEventBus) gsppoisonerrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__PmuPoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct PmuPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PmuPoisonError *__nvoc_metadata_ptr;
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
    struct PmuPoisonError *__nvoc_pbase_PmuPoisonError;    // pmupoisonerr

    // Data members
    PmuPoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PmuPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_PmuPoisonError
#define __nvoc_class_id_PmuPoisonError 0xf3d5dfu
typedef struct PmuPoisonError PmuPoisonError;
#endif /* __nvoc_class_id_PmuPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PmuPoisonError;

#define __staticCast_PmuPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_PmuPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_PmuPoisonError(pThis) ((PmuPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_PmuPoisonError(pThis) \
    ((PmuPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PmuPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PmuPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PmuPoisonError(PmuPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_PmuPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_PmuPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS pmupoisonerrConstruct_IMPL(struct PmuPoisonError *pEvent, struct EventBus *pEventBus);
#define __nvoc_pmupoisonerrConstruct(pEvent, pEventBus) pmupoisonerrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__Sec2PoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct Sec2PoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Sec2PoisonError *__nvoc_metadata_ptr;
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
    struct Sec2PoisonError *__nvoc_pbase_Sec2PoisonError;    // sec2poisonerr

    // Data members
    Sec2PoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Sec2PoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_Sec2PoisonError
#define __nvoc_class_id_Sec2PoisonError 0xb507fbu
typedef struct Sec2PoisonError Sec2PoisonError;
#endif /* __nvoc_class_id_Sec2PoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2PoisonError;

#define __staticCast_Sec2PoisonError(pThis) \
    ((pThis)->__nvoc_pbase_Sec2PoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_Sec2PoisonError(pThis) ((Sec2PoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_Sec2PoisonError(pThis) \
    ((Sec2PoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Sec2PoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Sec2PoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Sec2PoisonError(Sec2PoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_Sec2PoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_Sec2PoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS sec2poisonerrConstruct_IMPL(struct Sec2PoisonError *pEvent, struct EventBus *pEventBus);
#define __nvoc_sec2poisonerrConstruct(pEvent, pEventBus) sec2poisonerrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__FbFalconPoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct FbFalconPoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__FbFalconPoisonError *__nvoc_metadata_ptr;
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
    struct FbFalconPoisonError *__nvoc_pbase_FbFalconPoisonError;    // fbfalconpoisonerr

    // Data members
    FbFalconPoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FbFalconPoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_FbFalconPoisonError
#define __nvoc_class_id_FbFalconPoisonError 0x04f443u
typedef struct FbFalconPoisonError FbFalconPoisonError;
#endif /* __nvoc_class_id_FbFalconPoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_FbFalconPoisonError;

#define __staticCast_FbFalconPoisonError(pThis) \
    ((pThis)->__nvoc_pbase_FbFalconPoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_FbFalconPoisonError(pThis) ((FbFalconPoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_FbFalconPoisonError(pThis) \
    ((FbFalconPoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FbFalconPoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_FbFalconPoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FbFalconPoisonError(FbFalconPoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_FbFalconPoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_FbFalconPoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS fbfalconpoisonerrConstruct_IMPL(struct FbFalconPoisonError *pEvent, struct EventBus *pEventBus);
#define __nvoc_fbfalconpoisonerrConstruct(pEvent, pEventBus) fbfalconpoisonerrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__VideoEnginePoisonError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct VideoEnginePoisonError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__VideoEnginePoisonError *__nvoc_metadata_ptr;
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
    struct VideoEnginePoisonError *__nvoc_pbase_VideoEnginePoisonError;    // videoenginepoisonerr

    // Data members
    VideoEnginePoisonErrorCtx context;
    GpuXidEventCtx xid94;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__VideoEnginePoisonError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_VideoEnginePoisonError
#define __nvoc_class_id_VideoEnginePoisonError 0x0e1cb3u
typedef struct VideoEnginePoisonError VideoEnginePoisonError;
#endif /* __nvoc_class_id_VideoEnginePoisonError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VideoEnginePoisonError;

#define __staticCast_VideoEnginePoisonError(pThis) \
    ((pThis)->__nvoc_pbase_VideoEnginePoisonError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_VideoEnginePoisonError(pThis) ((VideoEnginePoisonError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_VideoEnginePoisonError(pThis) \
    ((VideoEnginePoisonError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VideoEnginePoisonError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_VideoEnginePoisonError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VideoEnginePoisonError(VideoEnginePoisonError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __objCreate_VideoEnginePoisonError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080EngineType) \
    __nvoc_objCreate_VideoEnginePoisonError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, nv2080EngineType)


// Wrapper macros for implementation functions
NV_STATUS videoenginepoisonerrConstruct_IMPL(struct VideoEnginePoisonError *pEvent, struct EventBus *pEventBus, NvU32 nv2080EngineType);
#define __nvoc_videoenginepoisonerrConstruct(pEvent, pEventBus, nv2080EngineType) videoenginepoisonerrConstruct_IMPL(pEvent, pEventBus, nv2080EngineType)


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


/* ------------------------ SRAM ECC errors ------------------------------------- */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_RAS_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieUncorrectableError;
struct NVOC_METADATA__GpuInterconnectError;


struct PcieUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PcieUncorrectableError *__nvoc_metadata_ptr;
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
    struct PcieUncorrectableError *__nvoc_pbase_PcieUncorrectableError;    // pcieuceerr

    // Data members
    EccUnitUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_PcieUncorrectableError
#define __nvoc_class_id_PcieUncorrectableError 0x52be0au
typedef struct PcieUncorrectableError PcieUncorrectableError;
#endif /* __nvoc_class_id_PcieUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieUncorrectableError;

#define __staticCast_PcieUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_PcieUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_PcieUncorrectableError(pThis) ((PcieUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_PcieUncorrectableError(pThis) \
    ((PcieUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PcieUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PcieUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PcieUncorrectableError(PcieUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 unit);
#define __objCreate_PcieUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, unit) \
    __nvoc_objCreate_PcieUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, unit)


// Wrapper macros for implementation functions
NV_STATUS pcieuceerrConstruct_IMPL(struct PcieUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 unit);
#define __nvoc_pcieuceerrConstruct(pEvent, pEventBus, unit) pcieuceerrConstruct_IMPL(pEvent, pEventBus, unit)


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
struct NVOC_METADATA__CopyEngineUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct CopyEngineUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__CopyEngineUncorrectableError *__nvoc_metadata_ptr;
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
    struct CopyEngineUncorrectableError *__nvoc_pbase_CopyEngineUncorrectableError;    // copyengineuce

    // Data members
    CopyEngineUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CopyEngineUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_CopyEngineUncorrectableError
#define __nvoc_class_id_CopyEngineUncorrectableError 0x973621u
typedef struct CopyEngineUncorrectableError CopyEngineUncorrectableError;
#endif /* __nvoc_class_id_CopyEngineUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CopyEngineUncorrectableError;

#define __staticCast_CopyEngineUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_CopyEngineUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_CopyEngineUncorrectableError(pThis) ((CopyEngineUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_CopyEngineUncorrectableError(pThis) \
    ((CopyEngineUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CopyEngineUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CopyEngineUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CopyEngineUncorrectableError(CopyEngineUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 dieletType, NvU32 dieletId, NvU32 pce);
#define __objCreate_CopyEngineUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, dieletType, dieletId, pce) \
    __nvoc_objCreate_CopyEngineUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, dieletType, dieletId, pce)


// Wrapper macros for implementation functions
NV_STATUS copyengineuceConstruct_IMPL(struct CopyEngineUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 dieletType, NvU32 dieletId, NvU32 pce);
#define __nvoc_copyengineuceConstruct(pEvent, pEventBus, dieletType, dieletId, pce) copyengineuceConstruct_IMPL(pEvent, pEventBus, dieletType, dieletId, pce)


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
struct NVOC_METADATA__MmuUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct MmuUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__MmuUncorrectableError *__nvoc_metadata_ptr;
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
    struct MmuUncorrectableError *__nvoc_pbase_MmuUncorrectableError;    // mmuuceerr

    // Data members
    EccUnitUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__MmuUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_MmuUncorrectableError
#define __nvoc_class_id_MmuUncorrectableError 0x7346b4u
typedef struct MmuUncorrectableError MmuUncorrectableError;
#endif /* __nvoc_class_id_MmuUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_MmuUncorrectableError;

#define __staticCast_MmuUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_MmuUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_MmuUncorrectableError(pThis) ((MmuUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_MmuUncorrectableError(pThis) \
    ((MmuUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(MmuUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_MmuUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_MmuUncorrectableError(MmuUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 unit);
#define __objCreate_MmuUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, unit) \
    __nvoc_objCreate_MmuUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, unit)


// Wrapper macros for implementation functions
NV_STATUS mmuuceerrConstruct_IMPL(struct MmuUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 unit);
#define __nvoc_mmuuceerrConstruct(pEvent, pEventBus, unit) mmuuceerrConstruct_IMPL(pEvent, pEventBus, unit)


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
struct NVOC_METADATA__FecsUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct FecsUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__FecsUncorrectableError *__nvoc_metadata_ptr;
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
    struct FecsUncorrectableError *__nvoc_pbase_FecsUncorrectableError;    // fecsuceerr

    // Data members
    struct EventContextHeader context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FecsUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_FecsUncorrectableError
#define __nvoc_class_id_FecsUncorrectableError 0xde7383u
typedef struct FecsUncorrectableError FecsUncorrectableError;
#endif /* __nvoc_class_id_FecsUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_FecsUncorrectableError;

#define __staticCast_FecsUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_FecsUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_FecsUncorrectableError(pThis) ((FecsUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_FecsUncorrectableError(pThis) \
    ((FecsUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FecsUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_FecsUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FecsUncorrectableError(FecsUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus);
#define __objCreate_FecsUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus) \
    __nvoc_objCreate_FecsUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus)


// Wrapper macros for implementation functions
NV_STATUS fecsuceerrConstruct_IMPL(struct FecsUncorrectableError *pEvent, struct EventBus *pEventBus);
#define __nvoc_fecsuceerrConstruct(pEvent, pEventBus) fecsuceerrConstruct_IMPL(pEvent, pEventBus)


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
struct NVOC_METADATA__SmUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct SmUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SmUncorrectableError *__nvoc_metadata_ptr;
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
    struct SmUncorrectableError *__nvoc_pbase_SmUncorrectableError;    // smuceerr

    // Data members
    SmUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__SmUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_SmUncorrectableError
#define __nvoc_class_id_SmUncorrectableError 0x66dff4u
typedef struct SmUncorrectableError SmUncorrectableError;
#endif /* __nvoc_class_id_SmUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SmUncorrectableError;

#define __staticCast_SmUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_SmUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_SmUncorrectableError(pThis) ((SmUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_SmUncorrectableError(pThis) \
    ((SmUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SmUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_SmUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SmUncorrectableError(SmUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 unit, NvU32 gpc, NvU32 tpc);
#define __objCreate_SmUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, unit, gpc, tpc) \
    __nvoc_objCreate_SmUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, unit, gpc, tpc)


// Wrapper macros for implementation functions
NV_STATUS smuceerrConstruct_IMPL(struct SmUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 unit, NvU32 gpc, NvU32 tpc);
#define __nvoc_smuceerrConstruct(pEvent, pEventBus, unit, gpc, tpc) smuceerrConstruct_IMPL(pEvent, pEventBus, unit, gpc, tpc)


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
struct NVOC_METADATA__GccUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct GccUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GccUncorrectableError *__nvoc_metadata_ptr;
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
    struct GccUncorrectableError *__nvoc_pbase_GccUncorrectableError;    // gccuceerr

    // Data members
    GrUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GccUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_GccUncorrectableError
#define __nvoc_class_id_GccUncorrectableError 0x46e41fu
typedef struct GccUncorrectableError GccUncorrectableError;
#endif /* __nvoc_class_id_GccUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GccUncorrectableError;

#define __staticCast_GccUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_GccUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_GccUncorrectableError(pThis) ((GccUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_GccUncorrectableError(pThis) \
    ((GccUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GccUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GccUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GccUncorrectableError(GccUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 gpc);
#define __objCreate_GccUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, gpc) \
    __nvoc_objCreate_GccUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, gpc)


// Wrapper macros for implementation functions
NV_STATUS gccuceerrConstruct_IMPL(struct GccUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 gpc);
#define __nvoc_gccuceerrConstruct(pEvent, pEventBus, gpc) gccuceerrConstruct_IMPL(pEvent, pEventBus, gpc)


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
struct NVOC_METADATA__ContextSwitchUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct ContextSwitchUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__ContextSwitchUncorrectableError *__nvoc_metadata_ptr;
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
    struct ContextSwitchUncorrectableError *__nvoc_pbase_ContextSwitchUncorrectableError;    // ctxswuceerr

    // Data members
    GrUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__ContextSwitchUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_ContextSwitchUncorrectableError
#define __nvoc_class_id_ContextSwitchUncorrectableError 0xe36111u
typedef struct ContextSwitchUncorrectableError ContextSwitchUncorrectableError;
#endif /* __nvoc_class_id_ContextSwitchUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ContextSwitchUncorrectableError;

#define __staticCast_ContextSwitchUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_ContextSwitchUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_ContextSwitchUncorrectableError(pThis) ((ContextSwitchUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_ContextSwitchUncorrectableError(pThis) \
    ((ContextSwitchUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(ContextSwitchUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_ContextSwitchUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_ContextSwitchUncorrectableError(ContextSwitchUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 gpc);
#define __objCreate_ContextSwitchUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, gpc) \
    __nvoc_objCreate_ContextSwitchUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, gpc)


// Wrapper macros for implementation functions
NV_STATUS ctxswuceerrConstruct_IMPL(struct ContextSwitchUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 gpc);
#define __nvoc_ctxswuceerrConstruct(pEvent, pEventBus, gpc) ctxswuceerrConstruct_IMPL(pEvent, pEventBus, gpc)


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
struct NVOC_METADATA__GpcMmuUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct GpcMmuUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GpcMmuUncorrectableError *__nvoc_metadata_ptr;
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
    struct GpcMmuUncorrectableError *__nvoc_pbase_GpcMmuUncorrectableError;    // gpcmmuuceerr

    // Data members
    GrUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GpcMmuUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_GpcMmuUncorrectableError
#define __nvoc_class_id_GpcMmuUncorrectableError 0xf31a41u
typedef struct GpcMmuUncorrectableError GpcMmuUncorrectableError;
#endif /* __nvoc_class_id_GpcMmuUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpcMmuUncorrectableError;

#define __staticCast_GpcMmuUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_GpcMmuUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_GpcMmuUncorrectableError(pThis) ((GpcMmuUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_GpcMmuUncorrectableError(pThis) \
    ((GpcMmuUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GpcMmuUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GpcMmuUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GpcMmuUncorrectableError(GpcMmuUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 gpc);
#define __objCreate_GpcMmuUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, gpc) \
    __nvoc_objCreate_GpcMmuUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, gpc)


// Wrapper macros for implementation functions
NV_STATUS gpcmmuuceerrConstruct_IMPL(struct GpcMmuUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 gpc);
#define __nvoc_gpcmmuuceerrConstruct(pEvent, pEventBus, gpc) gpcmmuuceerrConstruct_IMPL(pEvent, pEventBus, gpc)


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
struct NVOC_METADATA__GspUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct GspUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__GspUncorrectableError *__nvoc_metadata_ptr;
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
    struct GspUncorrectableError *__nvoc_pbase_GspUncorrectableError;    // gspuceerr

    // Data members
    RiscvUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__GspUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_GspUncorrectableError
#define __nvoc_class_id_GspUncorrectableError 0xb8abcbu
typedef struct GspUncorrectableError GspUncorrectableError;
#endif /* __nvoc_class_id_GspUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GspUncorrectableError;

#define __staticCast_GspUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_GspUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_GspUncorrectableError(pThis) ((GspUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_GspUncorrectableError(pThis) \
    ((GspUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(GspUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_GspUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_GspUncorrectableError(GspUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location);
#define __objCreate_GspUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, bHasLocation, location) \
    __nvoc_objCreate_GspUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, bHasLocation, location)


// Wrapper macros for implementation functions
NV_STATUS gspuceerrConstruct_IMPL(struct GspUncorrectableError *pEvent, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location);
#define __nvoc_gspuceerrConstruct(pEvent, pEventBus, bHasLocation, location) gspuceerrConstruct_IMPL(pEvent, pEventBus, bHasLocation, location)


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
struct NVOC_METADATA__LpsecUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct LpsecUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__LpsecUncorrectableError *__nvoc_metadata_ptr;
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
    struct LpsecUncorrectableError *__nvoc_pbase_LpsecUncorrectableError;    // lpsecuceerr

    // Data members
    LpsecUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__LpsecUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_LpsecUncorrectableError
#define __nvoc_class_id_LpsecUncorrectableError 0x6a47f6u
typedef struct LpsecUncorrectableError LpsecUncorrectableError;
#endif /* __nvoc_class_id_LpsecUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_LpsecUncorrectableError;

#define __staticCast_LpsecUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_LpsecUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_LpsecUncorrectableError(pThis) ((LpsecUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_LpsecUncorrectableError(pThis) \
    ((LpsecUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(LpsecUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_LpsecUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_LpsecUncorrectableError(LpsecUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 sequencer);
#define __objCreate_LpsecUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, sequencer) \
    __nvoc_objCreate_LpsecUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, sequencer)


// Wrapper macros for implementation functions
NV_STATUS lpsecuceerrConstruct_IMPL(struct LpsecUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 sequencer);
#define __nvoc_lpsecuceerrConstruct(pEvent, pEventBus, sequencer) lpsecuceerrConstruct_IMPL(pEvent, pEventBus, sequencer)


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
struct NVOC_METADATA__NvlinkUncorrectableError;
struct NVOC_METADATA__GpuInterconnectError;


struct NvlinkUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvlinkUncorrectableError *__nvoc_metadata_ptr;
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
    struct NvlinkUncorrectableError *__nvoc_pbase_NvlinkUncorrectableError;    // nvlinkuceerr

    // Data members
    EccUnitUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__NvlinkUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_NvlinkUncorrectableError
#define __nvoc_class_id_NvlinkUncorrectableError 0xb35a90u
typedef struct NvlinkUncorrectableError NvlinkUncorrectableError;
#endif /* __nvoc_class_id_NvlinkUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvlinkUncorrectableError;

#define __staticCast_NvlinkUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_NvlinkUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_NvlinkUncorrectableError(pThis) ((NvlinkUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_NvlinkUncorrectableError(pThis) \
    ((NvlinkUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvlinkUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_NvlinkUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvlinkUncorrectableError(NvlinkUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 nv2080GpuEccUnit);
#define __objCreate_NvlinkUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, nv2080GpuEccUnit) \
    __nvoc_objCreate_NvlinkUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, nv2080GpuEccUnit)


// Wrapper macros for implementation functions
NV_STATUS nvlinkuceerrConstruct_IMPL(struct NvlinkUncorrectableError *pEvent, struct EventBus *pEventBus, NvU32 nv2080GpuEccUnit);
#define __nvoc_nvlinkuceerrConstruct(pEvent, pEventBus, nv2080GpuEccUnit) nvlinkuceerrConstruct_IMPL(pEvent, pEventBus, nv2080GpuEccUnit)


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
struct NVOC_METADATA__PmuUncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct PmuUncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PmuUncorrectableError *__nvoc_metadata_ptr;
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
    struct PmuUncorrectableError *__nvoc_pbase_PmuUncorrectableError;    // pmuuceerr

    // Data members
    PmuUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PmuUncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_PmuUncorrectableError
#define __nvoc_class_id_PmuUncorrectableError 0xc6a5e0u
typedef struct PmuUncorrectableError PmuUncorrectableError;
#endif /* __nvoc_class_id_PmuUncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PmuUncorrectableError;

#define __staticCast_PmuUncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_PmuUncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_PmuUncorrectableError(pThis) ((PmuUncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_PmuUncorrectableError(pThis) \
    ((PmuUncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PmuUncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PmuUncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PmuUncorrectableError(PmuUncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvBool bIsFalcon, NvBool bHasLocation, NvU32 location);
#define __objCreate_PmuUncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, bIsFalcon, bHasLocation, location) \
    __nvoc_objCreate_PmuUncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, bIsFalcon, bHasLocation, location)


// Wrapper macros for implementation functions
NV_STATUS pmuuceerrConstruct_IMPL(struct PmuUncorrectableError *pEvent, struct EventBus *pEventBus, NvBool bIsFalcon, NvBool bHasLocation, NvU32 location);
#define __nvoc_pmuuceerrConstruct(pEvent, pEventBus, bIsFalcon, bHasLocation, location) pmuuceerrConstruct_IMPL(pEvent, pEventBus, bIsFalcon, bHasLocation, location)


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
struct NVOC_METADATA__Sec2UncorrectableError;
struct NVOC_METADATA__GpuMemoryIntegrityError;


struct Sec2UncorrectableError {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__Sec2UncorrectableError *__nvoc_metadata_ptr;
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
    struct Sec2UncorrectableError *__nvoc_pbase_Sec2UncorrectableError;    // sec2uceerr

    // Data members
    RiscvUncorrectableErrorCtx context;
    GpuXidEventCtx xid48;
    GpuXidEventCtx xid172;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__Sec2UncorrectableError {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuMemoryIntegrityError metadata__GpuMemoryIntegrityError;
};

#ifndef __nvoc_class_id_Sec2UncorrectableError
#define __nvoc_class_id_Sec2UncorrectableError 0x260c4bu
typedef struct Sec2UncorrectableError Sec2UncorrectableError;
#endif /* __nvoc_class_id_Sec2UncorrectableError */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Sec2UncorrectableError;

#define __staticCast_Sec2UncorrectableError(pThis) \
    ((pThis)->__nvoc_pbase_Sec2UncorrectableError)

#ifdef __nvoc_ras_events_h_disabled
#define __dynamicCast_Sec2UncorrectableError(pThis) ((Sec2UncorrectableError*) NULL)
#else //__nvoc_ras_events_h_disabled
#define __dynamicCast_Sec2UncorrectableError(pThis) \
    ((Sec2UncorrectableError*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(Sec2UncorrectableError)))
#endif //__nvoc_ras_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_Sec2UncorrectableError(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_Sec2UncorrectableError(Sec2UncorrectableError**, Dynamic*, NvU32, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location);
#define __objCreate_Sec2UncorrectableError(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, bHasLocation, location) \
    __nvoc_objCreate_Sec2UncorrectableError((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, bHasLocation, location)


// Wrapper macros for implementation functions
NV_STATUS sec2uceerrConstruct_IMPL(struct Sec2UncorrectableError *pEvent, struct EventBus *pEventBus, NvBool bHasLocation, NvU32 location);
#define __nvoc_sec2uceerrConstruct(pEvent, pEventBus, bHasLocation, location) sec2uceerrConstruct_IMPL(pEvent, pEventBus, bHasLocation, location)


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
