
#ifndef _G_PCIE_EVENTS_NVOC_H_
#define _G_PCIE_EVENTS_NVOC_H_

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

#include "g_pcie_events_nvoc.h"

#ifndef _PCIE_EVENTS_H_
#define _PCIE_EVENTS_H_

#include "events/gpu/gpu_events.h"
#include "events/gpu/pcie/pcie_event_ctx_defs.h"
#include "events/gpu/pcie/pcie_event_defs.h"

/* -------------------------------------------------------------------------
 * XID 167 — Fatal PCIe XAL Timeout
 *
 * Fired when a fatal timeout is detected in the PCIe XAL endpoint layer.
 * Captures which XAL register triggered the timeout and its raw value.
 * Emits XID 167 to the OS log, serializes to EventBuffer and CPER.
 * -------------------------------------------------------------------------
 */
typedef struct PcieFatalTimeoutCtx
{
    EventContextHeader   header;
    PCIE_FATAL_TIMEOUT_DATA data;
} PcieFatalTimeoutCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PCIE_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieFatalTimeout;
struct NVOC_METADATA__GpuInterconnectError;


struct PcieFatalTimeout {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PcieFatalTimeout *__nvoc_metadata_ptr;
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
    struct PcieFatalTimeout *__nvoc_pbase_PcieFatalTimeout;    // pciefataltimeout

    // Data members
    PcieFatalTimeoutCtx context;
    GpuXidEventCtx xid167;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieFatalTimeout {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_PcieFatalTimeout
#define __nvoc_class_id_PcieFatalTimeout 0x2fb654u
typedef struct PcieFatalTimeout PcieFatalTimeout;
#endif /* __nvoc_class_id_PcieFatalTimeout */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieFatalTimeout;

#define __staticCast_PcieFatalTimeout(pThis) \
    ((pThis)->__nvoc_pbase_PcieFatalTimeout)

#ifdef __nvoc_pcie_events_h_disabled
#define __dynamicCast_PcieFatalTimeout(pThis) ((PcieFatalTimeout*) NULL)
#else //__nvoc_pcie_events_h_disabled
#define __dynamicCast_PcieFatalTimeout(pThis) \
    ((PcieFatalTimeout*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PcieFatalTimeout)))
#endif //__nvoc_pcie_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PcieFatalTimeout(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PcieFatalTimeout(PcieFatalTimeout**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 errorType, NvU32 regVal);
#define __objCreate_PcieFatalTimeout(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, errorType, regVal) \
    __nvoc_objCreate_PcieFatalTimeout((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, errorType, regVal)


// Wrapper macros for implementation functions
NV_STATUS pciefataltimeoutConstruct_IMPL(struct PcieFatalTimeout *pEvent, struct EventBus *pEventBus, NvU32 errorType, NvU32 regVal);
#define __nvoc_pciefataltimeoutConstruct(pEvent, pEventBus, errorType, regVal) pciefataltimeoutConstruct_IMPL(pEvent, pEventBus, errorType, regVal)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* -------------------------------------------------------------------------
 * XID 176 — Correctable PCIe Error Fault
 *
 * Fired when a correctable error rate threshold is exceeded in the PCIe layer.
 * The hardware has already corrected the error; this event records that the
 * threshold was crossed for trend analysis. No CPER output — the error is
 * self-healing and does not require firmware-level persistence.
 * Emits XID 176 to the OS log and serializes to EventBuffer only.
 * -------------------------------------------------------------------------
 */
typedef struct PcieCorrectableErrorFaultCtx
{
    EventContextHeader             header;
    PCIE_CORRECTABLE_ERROR_FAULT_DATA data;
} PcieCorrectableErrorFaultCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PCIE_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieCorrectableErrorFault;
struct NVOC_METADATA__GpuInterconnectError;


struct PcieCorrectableErrorFault {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PcieCorrectableErrorFault *__nvoc_metadata_ptr;
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
    struct PcieCorrectableErrorFault *__nvoc_pbase_PcieCorrectableErrorFault;    // pciecorrecterr

    // Data members
    PcieCorrectableErrorFaultCtx context;
    GpuXidEventCtx xid176;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieCorrectableErrorFault {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_PcieCorrectableErrorFault
#define __nvoc_class_id_PcieCorrectableErrorFault 0x55f461u
typedef struct PcieCorrectableErrorFault PcieCorrectableErrorFault;
#endif /* __nvoc_class_id_PcieCorrectableErrorFault */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieCorrectableErrorFault;

#define __staticCast_PcieCorrectableErrorFault(pThis) \
    ((pThis)->__nvoc_pbase_PcieCorrectableErrorFault)

#ifdef __nvoc_pcie_events_h_disabled
#define __dynamicCast_PcieCorrectableErrorFault(pThis) ((PcieCorrectableErrorFault*) NULL)
#else //__nvoc_pcie_events_h_disabled
#define __dynamicCast_PcieCorrectableErrorFault(pThis) \
    ((PcieCorrectableErrorFault*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PcieCorrectableErrorFault)))
#endif //__nvoc_pcie_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PcieCorrectableErrorFault(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PcieCorrectableErrorFault(PcieCorrectableErrorFault**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 ceRateTupleNum, NvU32 ceRateNerdNumber, NvU32 ceRateTupleVerNum);
#define __objCreate_PcieCorrectableErrorFault(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, ceRateTupleNum, ceRateNerdNumber, ceRateTupleVerNum) \
    __nvoc_objCreate_PcieCorrectableErrorFault((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, ceRateTupleNum, ceRateNerdNumber, ceRateTupleVerNum)


// Wrapper macros for implementation functions
NV_STATUS pciecorrecterrConstruct_IMPL(struct PcieCorrectableErrorFault *pEvent, struct EventBus *pEventBus, NvU32 ceRateTupleNum, NvU32 ceRateNerdNumber, NvU32 ceRateTupleVerNum);
#define __nvoc_pciecorrecterrConstruct(pEvent, pEventBus, ceRateTupleNum, ceRateNerdNumber, ceRateTupleVerNum) pciecorrecterrConstruct_IMPL(pEvent, pEventBus, ceRateTupleNum, ceRateNerdNumber, ceRateTupleVerNum)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


/* -------------------------------------------------------------------------
 * XID 182 — PCIe Reset Failure
 *
 * Fired when a PCIe reset attempt fails. Because a reset failure leaves the
 * GPU in an indeterminate state and may precede a system hang, this event
 * serializes to both EventBuffer and CPER in addition to the OS log XID.
 * -------------------------------------------------------------------------
 */
typedef struct PcieResetFailureCtx
{
    EventContextHeader    header;
    PCIE_RESET_FAILURE_DATA data;
} PcieResetFailureCtx;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_PCIE_EVENTS_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieResetFailure;
struct NVOC_METADATA__GpuInterconnectError;


struct PcieResetFailure {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__PcieResetFailure *__nvoc_metadata_ptr;
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
    struct PcieResetFailure *__nvoc_pbase_PcieResetFailure;    // pcieresetfailure

    // Data members
    PcieResetFailureCtx context;
    GpuXidEventCtx xid182;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__PcieResetFailure {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuInterconnectError metadata__GpuInterconnectError;
};

#ifndef __nvoc_class_id_PcieResetFailure
#define __nvoc_class_id_PcieResetFailure 0x6c0c10u
typedef struct PcieResetFailure PcieResetFailure;
#endif /* __nvoc_class_id_PcieResetFailure */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_PcieResetFailure;

#define __staticCast_PcieResetFailure(pThis) \
    ((pThis)->__nvoc_pbase_PcieResetFailure)

#ifdef __nvoc_pcie_events_h_disabled
#define __dynamicCast_PcieResetFailure(pThis) ((PcieResetFailure*) NULL)
#else //__nvoc_pcie_events_h_disabled
#define __dynamicCast_PcieResetFailure(pThis) \
    ((PcieResetFailure*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(PcieResetFailure)))
#endif //__nvoc_pcie_events_h_disabled

NV_STATUS __nvoc_objCreateDynamic_PcieResetFailure(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_PcieResetFailure(PcieResetFailure**, Dynamic*, NvU32, struct EventBus *pEventBus, NvU32 resetStatus, NvU32 resetRecAction);
#define __objCreate_PcieResetFailure(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags, pEventBus, resetStatus, resetRecAction) \
    __nvoc_objCreate_PcieResetFailure((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags), pEventBus, resetStatus, resetRecAction)


// Wrapper macros for implementation functions
NV_STATUS pcieresetfailureConstruct_IMPL(struct PcieResetFailure *pEvent, struct EventBus *pEventBus, NvU32 resetStatus, NvU32 resetRecAction);
#define __nvoc_pcieresetfailureConstruct(pEvent, pEventBus, resetStatus, resetRecAction) pcieresetfailureConstruct_IMPL(pEvent, pEventBus, resetStatus, resetRecAction)


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // _PCIE_EVENTS_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_PCIE_EVENTS_NVOC_H_
