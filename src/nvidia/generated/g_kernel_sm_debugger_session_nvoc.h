
#ifndef _G_KERNEL_SM_DEBUGGER_SESSION_NVOC_H_
#define _G_KERNEL_SM_DEBUGGER_SESSION_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_sm_debugger_session_nvoc.h"

#ifndef KERNEL_SM_DEBUGGER_SESSION_H
#define KERNEL_SM_DEBUGGER_SESSION_H

#include "gpu/gpu_halspec.h"
#include "gpu/gpu_resource.h"
#include "rmapi/event.h"
#include "rmapi/control.h"
#include "kernel/gpu/gr/kernel_sm_debugger_exception.h"

#include "ctrl/ctrl83de.h"


struct KernelGraphicsObject;

#ifndef __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__
typedef struct KernelGraphicsObject KernelGraphicsObject;
#endif /* __NVOC_CLASS_KernelGraphicsObject_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsObject
#define __nvoc_class_id_KernelGraphicsObject 0x097648
#endif /* __nvoc_class_id_KernelGraphicsObject */



/*
 *  Valid Values for smdebugMode
 */
#define  SM_DEBUG_MODE_ENABLE           (0x00000001)
#define  SM_DEBUG_MODE_DISABLE          (0x00000002)

/*
 *  Valid Values for stopOnAnyWARPMode
 */
#define  STOP_ON_ANYWARP_MODE_ENABLED   (0x00000001)
#define  STOP_ON_ANYWARP_MODE_DISABLED  (0x00000002)

/*
 *  Valid Values for stopOnAnySMMode
 */
#define  STOP_ON_ANYSM_MODE_ENABLED     (0x00000001)
#define  STOP_ON_ANYSM_MODE_DISABLED    (0x00000002)

//
// Debugger Session object for automatically freeing and
// invalidating the debugger object when underlying objects that it
// relies on are freed before it.
//

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_SM_DEBUGGER_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__RmDebuggerSession;
struct NVOC_METADATA__RsSession;
struct NVOC_VTABLE__RmDebuggerSession;


struct RmDebuggerSession {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__RmDebuggerSession *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct RsSession __nvoc_base_RsSession;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^3
    struct RsShared *__nvoc_pbase_RsShared;    // shr super^2
    struct RsSession *__nvoc_pbase_RsSession;    // session super
    struct RmDebuggerSession *__nvoc_pbase_RmDebuggerSession;    // dbgSession
};


// Vtable with 2 per-class function pointers
struct NVOC_VTABLE__RmDebuggerSession {
    void (*__dbgSessionRemoveDependant__)(struct RmDebuggerSession * /*this*/, struct RsResourceRef *);  // virtual override (session) base (session)
    void (*__dbgSessionRemoveDependency__)(struct RmDebuggerSession * /*this*/, struct RsResourceRef *);  // virtual override (session) base (session)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__RmDebuggerSession {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__RsSession metadata__RsSession;
    const struct NVOC_VTABLE__RmDebuggerSession vtable;
};

#ifndef __NVOC_CLASS_RmDebuggerSession_TYPEDEF__
#define __NVOC_CLASS_RmDebuggerSession_TYPEDEF__
typedef struct RmDebuggerSession RmDebuggerSession;
#endif /* __NVOC_CLASS_RmDebuggerSession_TYPEDEF__ */

#ifndef __nvoc_class_id_RmDebuggerSession
#define __nvoc_class_id_RmDebuggerSession 0x48fa7d
#endif /* __nvoc_class_id_RmDebuggerSession */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmDebuggerSession;

#define __staticCast_RmDebuggerSession(pThis) \
    ((pThis)->__nvoc_pbase_RmDebuggerSession)

#ifdef __nvoc_kernel_sm_debugger_session_h_disabled
#define __dynamicCast_RmDebuggerSession(pThis) ((RmDebuggerSession*) NULL)
#else //__nvoc_kernel_sm_debugger_session_h_disabled
#define __dynamicCast_RmDebuggerSession(pThis) \
    ((RmDebuggerSession*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RmDebuggerSession)))
#endif //__nvoc_kernel_sm_debugger_session_h_disabled

NV_STATUS __nvoc_objCreateDynamic_RmDebuggerSession(RmDebuggerSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RmDebuggerSession(RmDebuggerSession**, Dynamic*, NvU32);
#define __objCreate_RmDebuggerSession(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_RmDebuggerSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define dbgSessionRemoveDependant_FNPTR(pDbgSession) pDbgSession->__nvoc_metadata_ptr->vtable.__dbgSessionRemoveDependant__
#define dbgSessionRemoveDependant(pDbgSession, pResourceRef) dbgSessionRemoveDependant_DISPATCH(pDbgSession, pResourceRef)
#define dbgSessionRemoveDependency_FNPTR(pDbgSession) pDbgSession->__nvoc_metadata_ptr->vtable.__dbgSessionRemoveDependency__
#define dbgSessionRemoveDependency(pDbgSession, pResourceRef) dbgSessionRemoveDependency_DISPATCH(pDbgSession, pResourceRef)

// Dispatch functions
static inline void dbgSessionRemoveDependant_DISPATCH(struct RmDebuggerSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    pDbgSession->__nvoc_metadata_ptr->vtable.__dbgSessionRemoveDependant__(pDbgSession, pResourceRef);
}

static inline void dbgSessionRemoveDependency_DISPATCH(struct RmDebuggerSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    pDbgSession->__nvoc_metadata_ptr->vtable.__dbgSessionRemoveDependency__(pDbgSession, pResourceRef);
}

void dbgSessionRemoveDependant_IMPL(struct RmDebuggerSession *pDbgSession, struct RsResourceRef *pResourceRef);

void dbgSessionRemoveDependency_IMPL(struct RmDebuggerSession *pDbgSession, struct RsResourceRef *pResourceRef);

#undef PRIVATE_FIELD



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_SM_DEBUGGER_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelSMDebuggerSession;
struct NVOC_METADATA__GpuResource;
struct NVOC_METADATA__Notifier;
struct NVOC_VTABLE__KernelSMDebuggerSession;


struct KernelSMDebuggerSession {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelSMDebuggerSession *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^4
    struct RsResource *__nvoc_pbase_RsResource;    // res super^3
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;    // rmrescmn super^3
    struct RmResource *__nvoc_pbase_RmResource;    // rmres super^2
    struct GpuResource *__nvoc_pbase_GpuResource;    // gpures super
    struct INotifier *__nvoc_pbase_INotifier;    // inotify super^2
    struct Notifier *__nvoc_pbase_Notifier;    // notify super
    struct KernelSMDebuggerSession *__nvoc_pbase_KernelSMDebuggerSession;    // ksmdbgssn

    // Vtable with 31 per-object function pointers
    NV_STATUS (*__ksmdbgssnCtrlCmdSMDebugModeEnable__)(struct KernelSMDebuggerSession * /*this*/);  // inline exported (id=0x83de0301) body
    NV_STATUS (*__ksmdbgssnCtrlCmdSMDebugModeDisable__)(struct KernelSMDebuggerSession * /*this*/);  // inline exported (id=0x83de0302) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetModeMMUDebug__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *);  // inline exported (id=0x83de0307) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugGetModeMMUDebug__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *);  // inline exported (id=0x83de0308) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_MODE_MMU_GCC_DEBUG_PARAMS *);  // inline exported (id=0x83de032a) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_GET_MODE_MMU_GCC_DEBUG_PARAMS *);  // inline exported (id=0x83de032b) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetModeErrbarDebug__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *);  // inline exported (id=0x83de031f) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugGetModeErrbarDebug__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_GET_MODE_ERRBAR_DEBUG_PARAMS *);  // inline exported (id=0x83de0320) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetExceptionMask__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *);  // inline exported (id=0x83de0309) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadSingleSmErrorState__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *);  // inline exported (id=0x83de030b) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadAllSmErrorStates__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *);  // exported (id=0x83de030c)
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugClearSingleSmErrorState__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *);  // inline exported (id=0x83de030f) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugClearAllSmErrorStates__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *);  // exported (id=0x83de0310)
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSuspendContext__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *);  // inline exported (id=0x83de0317) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugResumeContext__)(struct KernelSMDebuggerSession * /*this*/);  // inline exported (id=0x83de0318) body
    NV_STATUS (*__ksmdbgssnCtrlCmdReadSurface__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *);  // exported (id=0x83de031a)
    NV_STATUS (*__ksmdbgssnCtrlCmdWriteSurface__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *);  // exported (id=0x83de031b)
    NV_STATUS (*__ksmdbgssnCtrlCmdGetMappings__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_GET_MAPPINGS_PARAMETERS *);  // exported (id=0x83de031c)
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetNextStopTriggerType__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *);  // inline exported (id=0x83de0313) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_SINGLE_STEP_INTERRUPT_HANDLING_PARAMS *);  // inline exported (id=0x83de0314) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadMemory__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *);  // exported (id=0x83de0315)
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugWriteMemory__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *);  // exported (id=0x83de0316)
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugExecRegOps__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *);  // exported (id=0x83de031d)
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *);  // inline exported (id=0x83de0321) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_STOP_TRIGGER_PARAMS *);  // inline exported (id=0x83de0322) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_RUN_TRIGGER_PARAMS *);  // inline exported (id=0x83de0323) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SKIP_IDLE_WARP_DETECT_PARAMS *);  // inline exported (id=0x83de0324) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_GET_SINGLE_SM_DEBUGGER_STATUS_PARAMS *);  // inline exported (id=0x83de0325) body
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadBatchMemory__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *);  // exported (id=0x83de0326)
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugWriteBatchMemory__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *);  // exported (id=0x83de0327)
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__)(struct KernelSMDebuggerSession * /*this*/, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *);  // halified (2 hals) exported (id=0x83de0328)

    // Data members
    struct RmDebuggerSession *PRIVATE_FIELD(pDebugSession);
    struct KernelGraphicsObject *PRIVATE_FIELD(pObject);
    NvHandle PRIVATE_FIELD(hDebugger);
    NvHandle PRIVATE_FIELD(hDebuggerClient);
    NvHandle PRIVATE_FIELD(hChannel);
    NvHandle PRIVATE_FIELD(hChannelClient);
    NvHandle PRIVATE_FIELD(hSubdevice);
    NvHandle PRIVATE_FIELD(hInternalClient);
    NvHandle PRIVATE_FIELD(hInternalDevice);
    NvHandle PRIVATE_FIELD(hInternalSubdevice);
    NvHandle PRIVATE_FIELD(hInternalSubscription);
    NvHandle PRIVATE_FIELD(hInternalMemMapping);
};


// Vtable with 30 per-class function pointers
struct NVOC_VTABLE__KernelSMDebuggerSession {
    NV_STATUS (*__ksmdbgssnInternalControlForward__)(struct KernelSMDebuggerSession * /*this*/, NvU32, void *, NvU32);  // virtual override (gpures) base (gpures)
    NvHandle (*__ksmdbgssnGetInternalObjectHandle__)(struct KernelSMDebuggerSession * /*this*/);  // virtual override (gpures) base (gpures)
    NV_STATUS (*__ksmdbgssnControl__)(struct KernelSMDebuggerSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__ksmdbgssnMap__)(struct KernelSMDebuggerSession * /*this*/, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__ksmdbgssnUnmap__)(struct KernelSMDebuggerSession * /*this*/, struct CALL_CONTEXT *, struct RsCpuMapping *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__ksmdbgssnShareCallback__)(struct KernelSMDebuggerSession * /*this*/, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__ksmdbgssnGetRegBaseOffsetAndSize__)(struct KernelSMDebuggerSession * /*this*/, struct OBJGPU *, NvU32 *, NvU32 *);  // virtual inherited (gpures) base (gpures)
    NV_STATUS (*__ksmdbgssnGetMapAddrSpace__)(struct KernelSMDebuggerSession * /*this*/, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);  // virtual inherited (gpures) base (gpures)
    NvBool (*__ksmdbgssnAccessCallback__)(struct KernelSMDebuggerSession * /*this*/, struct RsClient *, void *, RsAccessRight);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ksmdbgssnGetMemInterMapParams__)(struct KernelSMDebuggerSession * /*this*/, RMRES_MEM_INTER_MAP_PARAMS *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ksmdbgssnCheckMemInterUnmap__)(struct KernelSMDebuggerSession * /*this*/, NvBool);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ksmdbgssnGetMemoryMappingDescriptor__)(struct KernelSMDebuggerSession * /*this*/, struct MEMORY_DESCRIPTOR **);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ksmdbgssnControlSerialization_Prologue__)(struct KernelSMDebuggerSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__ksmdbgssnControlSerialization_Epilogue__)(struct KernelSMDebuggerSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NV_STATUS (*__ksmdbgssnControl_Prologue__)(struct KernelSMDebuggerSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    void (*__ksmdbgssnControl_Epilogue__)(struct KernelSMDebuggerSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (rmres) base (gpures)
    NvBool (*__ksmdbgssnCanCopy__)(struct KernelSMDebuggerSession * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__ksmdbgssnIsDuplicate__)(struct KernelSMDebuggerSession * /*this*/, NvHandle, NvBool *);  // virtual inherited (res) base (gpures)
    void (*__ksmdbgssnPreDestruct__)(struct KernelSMDebuggerSession * /*this*/);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__ksmdbgssnControlFilter__)(struct KernelSMDebuggerSession * /*this*/, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);  // virtual inherited (res) base (gpures)
    NvBool (*__ksmdbgssnIsPartialUnmapSupported__)(struct KernelSMDebuggerSession * /*this*/);  // inline virtual inherited (res) base (gpures) body
    NV_STATUS (*__ksmdbgssnMapTo__)(struct KernelSMDebuggerSession * /*this*/, RS_RES_MAP_TO_PARAMS *);  // virtual inherited (res) base (gpures)
    NV_STATUS (*__ksmdbgssnUnmapFrom__)(struct KernelSMDebuggerSession * /*this*/, RS_RES_UNMAP_FROM_PARAMS *);  // virtual inherited (res) base (gpures)
    NvU32 (*__ksmdbgssnGetRefCount__)(struct KernelSMDebuggerSession * /*this*/);  // virtual inherited (res) base (gpures)
    void (*__ksmdbgssnAddAdditionalDependants__)(struct RsClient *, struct KernelSMDebuggerSession * /*this*/, RsResourceRef *);  // virtual inherited (res) base (gpures)
    PEVENTNOTIFICATION * (*__ksmdbgssnGetNotificationListPtr__)(struct KernelSMDebuggerSession * /*this*/);  // virtual inherited (notify) base (notify)
    struct NotifShare * (*__ksmdbgssnGetNotificationShare__)(struct KernelSMDebuggerSession * /*this*/);  // virtual inherited (notify) base (notify)
    void (*__ksmdbgssnSetNotificationShare__)(struct KernelSMDebuggerSession * /*this*/, struct NotifShare *);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__ksmdbgssnUnregisterEvent__)(struct KernelSMDebuggerSession * /*this*/, NvHandle, NvHandle, NvHandle, NvHandle);  // virtual inherited (notify) base (notify)
    NV_STATUS (*__ksmdbgssnGetOrAllocNotifShare__)(struct KernelSMDebuggerSession * /*this*/, NvHandle, NvHandle, struct NotifShare **);  // virtual inherited (notify) base (notify)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelSMDebuggerSession {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__GpuResource metadata__GpuResource;
    const struct NVOC_METADATA__Notifier metadata__Notifier;
    const struct NVOC_VTABLE__KernelSMDebuggerSession vtable;
};

#ifndef __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
#define __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
typedef struct KernelSMDebuggerSession KernelSMDebuggerSession;
#endif /* __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelSMDebuggerSession
#define __nvoc_class_id_KernelSMDebuggerSession 0x4adc81
#endif /* __nvoc_class_id_KernelSMDebuggerSession */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSMDebuggerSession;

#define __staticCast_KernelSMDebuggerSession(pThis) \
    ((pThis)->__nvoc_pbase_KernelSMDebuggerSession)

#ifdef __nvoc_kernel_sm_debugger_session_h_disabled
#define __dynamicCast_KernelSMDebuggerSession(pThis) ((KernelSMDebuggerSession*) NULL)
#else //__nvoc_kernel_sm_debugger_session_h_disabled
#define __dynamicCast_KernelSMDebuggerSession(pThis) \
    ((KernelSMDebuggerSession*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelSMDebuggerSession)))
#endif //__nvoc_kernel_sm_debugger_session_h_disabled

NV_STATUS __nvoc_objCreateDynamic_KernelSMDebuggerSession(KernelSMDebuggerSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelSMDebuggerSession(KernelSMDebuggerSession**, Dynamic*, NvU32, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
#define __objCreate_KernelSMDebuggerSession(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelSMDebuggerSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)


// Wrapper macros
#define ksmdbgssnInternalControlForward_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__ksmdbgssnInternalControlForward__
#define ksmdbgssnInternalControlForward(arg_this, command, pParams, size) ksmdbgssnInternalControlForward_DISPATCH(arg_this, command, pParams, size)
#define ksmdbgssnGetInternalObjectHandle_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetInternalObjectHandle__
#define ksmdbgssnGetInternalObjectHandle(arg_this) ksmdbgssnGetInternalObjectHandle_DISPATCH(arg_this)
#define ksmdbgssnCtrlCmdSMDebugModeEnable_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdSMDebugModeEnable__
#define ksmdbgssnCtrlCmdSMDebugModeEnable(pKernelSMDebuggerSession) ksmdbgssnCtrlCmdSMDebugModeEnable_DISPATCH(pKernelSMDebuggerSession)
#define ksmdbgssnCtrlCmdSMDebugModeDisable_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdSMDebugModeDisable__
#define ksmdbgssnCtrlCmdSMDebugModeDisable(pKernelSMDebuggerSession) ksmdbgssnCtrlCmdSMDebugModeDisable_DISPATCH(pKernelSMDebuggerSession)
#define ksmdbgssnCtrlCmdDebugSetModeMMUDebug_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetModeMMUDebug__
#define ksmdbgssnCtrlCmdDebugSetModeMMUDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetModeMMUDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugGetModeMMUDebug_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetModeMMUDebug__
#define ksmdbgssnCtrlCmdDebugGetModeMMUDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugGetModeMMUDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug__
#define ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug__
#define ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetModeErrbarDebug__
#define ksmdbgssnCtrlCmdDebugSetModeErrbarDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetModeErrbarDebug__
#define ksmdbgssnCtrlCmdDebugGetModeErrbarDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetExceptionMask_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetExceptionMask__
#define ksmdbgssnCtrlCmdDebugSetExceptionMask(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetExceptionMask_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugReadSingleSmErrorState__
#define ksmdbgssnCtrlCmdDebugReadSingleSmErrorState(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugReadAllSmErrorStates__
#define ksmdbgssnCtrlCmdDebugReadAllSmErrorStates(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugClearSingleSmErrorState__
#define ksmdbgssnCtrlCmdDebugClearSingleSmErrorState(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugClearAllSmErrorStates__
#define ksmdbgssnCtrlCmdDebugClearAllSmErrorStates(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSuspendContext_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSuspendContext__
#define ksmdbgssnCtrlCmdDebugSuspendContext(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSuspendContext_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugResumeContext_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugResumeContext__
#define ksmdbgssnCtrlCmdDebugResumeContext(pKernelSMDebuggerSession) ksmdbgssnCtrlCmdDebugResumeContext_DISPATCH(pKernelSMDebuggerSession)
#define ksmdbgssnCtrlCmdReadSurface_FNPTR(arg_this) arg_this->__ksmdbgssnCtrlCmdReadSurface__
#define ksmdbgssnCtrlCmdReadSurface(arg_this, arg2) ksmdbgssnCtrlCmdReadSurface_DISPATCH(arg_this, arg2)
#define ksmdbgssnCtrlCmdWriteSurface_FNPTR(arg_this) arg_this->__ksmdbgssnCtrlCmdWriteSurface__
#define ksmdbgssnCtrlCmdWriteSurface(arg_this, arg2) ksmdbgssnCtrlCmdWriteSurface_DISPATCH(arg_this, arg2)
#define ksmdbgssnCtrlCmdGetMappings_FNPTR(arg_this) arg_this->__ksmdbgssnCtrlCmdGetMappings__
#define ksmdbgssnCtrlCmdGetMappings(arg_this, arg2) ksmdbgssnCtrlCmdGetMappings_DISPATCH(arg_this, arg2)
#define ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetNextStopTriggerType__
#define ksmdbgssnCtrlCmdDebugSetNextStopTriggerType(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling__
#define ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugReadMemory_FNPTR(arg_this) arg_this->__ksmdbgssnCtrlCmdDebugReadMemory__
#define ksmdbgssnCtrlCmdDebugReadMemory(arg_this, arg2) ksmdbgssnCtrlCmdDebugReadMemory_DISPATCH(arg_this, arg2)
#define ksmdbgssnCtrlCmdDebugWriteMemory_FNPTR(arg_this) arg_this->__ksmdbgssnCtrlCmdDebugWriteMemory__
#define ksmdbgssnCtrlCmdDebugWriteMemory(arg_this, arg2) ksmdbgssnCtrlCmdDebugWriteMemory_DISPATCH(arg_this, arg2)
#define ksmdbgssnCtrlCmdDebugExecRegOps_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugExecRegOps__
#define ksmdbgssnCtrlCmdDebugExecRegOps(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugExecRegOps_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep__
#define ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger__
#define ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger__
#define ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect__
#define ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_FNPTR(pKernelSMDebuggerSession) pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus__
#define ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugReadBatchMemory_FNPTR(arg_this) arg_this->__ksmdbgssnCtrlCmdDebugReadBatchMemory__
#define ksmdbgssnCtrlCmdDebugReadBatchMemory(arg_this, arg2) ksmdbgssnCtrlCmdDebugReadBatchMemory_DISPATCH(arg_this, arg2)
#define ksmdbgssnCtrlCmdDebugWriteBatchMemory_FNPTR(arg_this) arg_this->__ksmdbgssnCtrlCmdDebugWriteBatchMemory__
#define ksmdbgssnCtrlCmdDebugWriteBatchMemory(arg_this, arg2) ksmdbgssnCtrlCmdDebugWriteBatchMemory_DISPATCH(arg_this, arg2)
#define ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_FNPTR(arg_this) arg_this->__ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__
#define ksmdbgssnCtrlCmdDebugReadMMUFaultInfo(arg_this, arg2) ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_DISPATCH(arg_this, arg2)
#define ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_HAL(arg_this, arg2) ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_DISPATCH(arg_this, arg2)
#define ksmdbgssnControl_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresControl__
#define ksmdbgssnControl(pGpuResource, pCallContext, pParams) ksmdbgssnControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define ksmdbgssnMap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresMap__
#define ksmdbgssnMap(pGpuResource, pCallContext, pParams, pCpuMapping) ksmdbgssnMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define ksmdbgssnUnmap_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresUnmap__
#define ksmdbgssnUnmap(pGpuResource, pCallContext, pCpuMapping) ksmdbgssnUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define ksmdbgssnShareCallback_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresShareCallback__
#define ksmdbgssnShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) ksmdbgssnShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define ksmdbgssnGetRegBaseOffsetAndSize_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetRegBaseOffsetAndSize__
#define ksmdbgssnGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) ksmdbgssnGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define ksmdbgssnGetMapAddrSpace_FNPTR(pGpuResource) pGpuResource->__nvoc_base_GpuResource.__nvoc_metadata_ptr->vtable.__gpuresGetMapAddrSpace__
#define ksmdbgssnGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) ksmdbgssnGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define ksmdbgssnAccessCallback_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresAccessCallback__
#define ksmdbgssnAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) ksmdbgssnAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
#define ksmdbgssnGetMemInterMapParams_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemInterMapParams__
#define ksmdbgssnGetMemInterMapParams(pRmResource, pParams) ksmdbgssnGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define ksmdbgssnCheckMemInterUnmap_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresCheckMemInterUnmap__
#define ksmdbgssnCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) ksmdbgssnCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define ksmdbgssnGetMemoryMappingDescriptor_FNPTR(pRmResource) pRmResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresGetMemoryMappingDescriptor__
#define ksmdbgssnGetMemoryMappingDescriptor(pRmResource, ppMemDesc) ksmdbgssnGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define ksmdbgssnControlSerialization_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Prologue__
#define ksmdbgssnControlSerialization_Prologue(pResource, pCallContext, pParams) ksmdbgssnControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnControlSerialization_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControlSerialization_Epilogue__
#define ksmdbgssnControlSerialization_Epilogue(pResource, pCallContext, pParams) ksmdbgssnControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnControl_Prologue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Prologue__
#define ksmdbgssnControl_Prologue(pResource, pCallContext, pParams) ksmdbgssnControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnControl_Epilogue_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr->vtable.__rmresControl_Epilogue__
#define ksmdbgssnControl_Epilogue(pResource, pCallContext, pParams) ksmdbgssnControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnCanCopy_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resCanCopy__
#define ksmdbgssnCanCopy(pResource) ksmdbgssnCanCopy_DISPATCH(pResource)
#define ksmdbgssnIsDuplicate_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsDuplicate__
#define ksmdbgssnIsDuplicate(pResource, hMemory, pDuplicate) ksmdbgssnIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define ksmdbgssnPreDestruct_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resPreDestruct__
#define ksmdbgssnPreDestruct(pResource) ksmdbgssnPreDestruct_DISPATCH(pResource)
#define ksmdbgssnControlFilter_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resControlFilter__
#define ksmdbgssnControlFilter(pResource, pCallContext, pParams) ksmdbgssnControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnIsPartialUnmapSupported_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resIsPartialUnmapSupported__
#define ksmdbgssnIsPartialUnmapSupported(pResource) ksmdbgssnIsPartialUnmapSupported_DISPATCH(pResource)
#define ksmdbgssnMapTo_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resMapTo__
#define ksmdbgssnMapTo(pResource, pParams) ksmdbgssnMapTo_DISPATCH(pResource, pParams)
#define ksmdbgssnUnmapFrom_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resUnmapFrom__
#define ksmdbgssnUnmapFrom(pResource, pParams) ksmdbgssnUnmapFrom_DISPATCH(pResource, pParams)
#define ksmdbgssnGetRefCount_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resGetRefCount__
#define ksmdbgssnGetRefCount(pResource) ksmdbgssnGetRefCount_DISPATCH(pResource)
#define ksmdbgssnAddAdditionalDependants_FNPTR(pResource) pResource->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr->vtable.__resAddAdditionalDependants__
#define ksmdbgssnAddAdditionalDependants(pClient, pResource, pReference) ksmdbgssnAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define ksmdbgssnGetNotificationListPtr_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationListPtr__
#define ksmdbgssnGetNotificationListPtr(pNotifier) ksmdbgssnGetNotificationListPtr_DISPATCH(pNotifier)
#define ksmdbgssnGetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetNotificationShare__
#define ksmdbgssnGetNotificationShare(pNotifier) ksmdbgssnGetNotificationShare_DISPATCH(pNotifier)
#define ksmdbgssnSetNotificationShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifySetNotificationShare__
#define ksmdbgssnSetNotificationShare(pNotifier, pNotifShare) ksmdbgssnSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define ksmdbgssnUnregisterEvent_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyUnregisterEvent__
#define ksmdbgssnUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) ksmdbgssnUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define ksmdbgssnGetOrAllocNotifShare_FNPTR(pNotifier) pNotifier->__nvoc_base_Notifier.__nvoc_metadata_ptr->vtable.__notifyGetOrAllocNotifShare__
#define ksmdbgssnGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) ksmdbgssnGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)

// Dispatch functions
static inline NV_STATUS ksmdbgssnInternalControlForward_DISPATCH(struct KernelSMDebuggerSession *arg_this, NvU32 command, void *pParams, NvU32 size) {
    return arg_this->__nvoc_metadata_ptr->vtable.__ksmdbgssnInternalControlForward__(arg_this, command, pParams, size);
}

static inline NvHandle ksmdbgssnGetInternalObjectHandle_DISPATCH(struct KernelSMDebuggerSession *arg_this) {
    return arg_this->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetInternalObjectHandle__(arg_this);
}

static inline NV_STATUS ksmdbgssnCtrlCmdSMDebugModeEnable_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdSMDebugModeEnable__(pKernelSMDebuggerSession);
}

static inline NV_STATUS ksmdbgssnCtrlCmdSMDebugModeDisable_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdSMDebugModeDisable__(pKernelSMDebuggerSession);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeMMUDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetModeMMUDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeMMUDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetModeMMUDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_MMU_GCC_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_MMU_GCC_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetModeErrbarDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_ERRBAR_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetModeErrbarDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetExceptionMask_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetExceptionMask__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugReadSingleSmErrorState__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugReadAllSmErrorStates__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugClearSingleSmErrorState__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugClearAllSmErrorStates__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSuspendContext_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSuspendContext__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugResumeContext_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugResumeContext__(pKernelSMDebuggerSession);
}

static inline NV_STATUS ksmdbgssnCtrlCmdReadSurface_DISPATCH(struct KernelSMDebuggerSession *arg_this, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *arg2) {
    return arg_this->__ksmdbgssnCtrlCmdReadSurface__(arg_this, arg2);
}

static inline NV_STATUS ksmdbgssnCtrlCmdWriteSurface_DISPATCH(struct KernelSMDebuggerSession *arg_this, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *arg2) {
    return arg_this->__ksmdbgssnCtrlCmdWriteSurface__(arg_this, arg2);
}

static inline NV_STATUS ksmdbgssnCtrlCmdGetMappings_DISPATCH(struct KernelSMDebuggerSession *arg_this, NV83DE_CTRL_DEBUG_GET_MAPPINGS_PARAMETERS *arg2) {
    return arg_this->__ksmdbgssnCtrlCmdGetMappings__(arg_this, arg2);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetNextStopTriggerType__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_STEP_INTERRUPT_HANDLING_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadMemory_DISPATCH(struct KernelSMDebuggerSession *arg_this, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *arg2) {
    return arg_this->__ksmdbgssnCtrlCmdDebugReadMemory__(arg_this, arg2);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugWriteMemory_DISPATCH(struct KernelSMDebuggerSession *arg_this, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *arg2) {
    return arg_this->__ksmdbgssnCtrlCmdDebugWriteMemory__(arg_this, arg2);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugExecRegOps_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugExecRegOps__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_STOP_TRIGGER_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_RUN_TRIGGER_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SKIP_IDLE_WARP_DETECT_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_SINGLE_SM_DEBUGGER_STATUS_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadBatchMemory_DISPATCH(struct KernelSMDebuggerSession *arg_this, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *arg2) {
    return arg_this->__ksmdbgssnCtrlCmdDebugReadBatchMemory__(arg_this, arg2);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugWriteBatchMemory_DISPATCH(struct KernelSMDebuggerSession *arg_this, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *arg2) {
    return arg_this->__ksmdbgssnCtrlCmdDebugWriteBatchMemory__(arg_this, arg2);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_DISPATCH(struct KernelSMDebuggerSession *arg_this, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *arg2) {
    return arg_this->__ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__(arg_this, arg2);
}

static inline NV_STATUS ksmdbgssnControl_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS ksmdbgssnMap_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NV_STATUS ksmdbgssnUnmap_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NvBool ksmdbgssnShareCallback_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS ksmdbgssnGetRegBaseOffsetAndSize_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS ksmdbgssnGetMapAddrSpace_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline NvBool ksmdbgssnAccessCallback_DISPATCH(struct KernelSMDebuggerSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

static inline NV_STATUS ksmdbgssnGetMemInterMapParams_DISPATCH(struct KernelSMDebuggerSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS ksmdbgssnCheckMemInterUnmap_DISPATCH(struct KernelSMDebuggerSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS ksmdbgssnGetMemoryMappingDescriptor_DISPATCH(struct KernelSMDebuggerSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS ksmdbgssnControlSerialization_Prologue_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline void ksmdbgssnControlSerialization_Epilogue_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ksmdbgssnControl_Prologue_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnControl_Prologue__(pResource, pCallContext, pParams);
}

static inline void ksmdbgssnControl_Epilogue_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NvBool ksmdbgssnCanCopy_DISPATCH(struct KernelSMDebuggerSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnCanCopy__(pResource);
}

static inline NV_STATUS ksmdbgssnIsDuplicate_DISPATCH(struct KernelSMDebuggerSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void ksmdbgssnPreDestruct_DISPATCH(struct KernelSMDebuggerSession *pResource) {
    pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnPreDestruct__(pResource);
}

static inline NV_STATUS ksmdbgssnControlFilter_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnControlFilter__(pResource, pCallContext, pParams);
}

static inline NvBool ksmdbgssnIsPartialUnmapSupported_DISPATCH(struct KernelSMDebuggerSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnIsPartialUnmapSupported__(pResource);
}

static inline NV_STATUS ksmdbgssnMapTo_DISPATCH(struct KernelSMDebuggerSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnMapTo__(pResource, pParams);
}

static inline NV_STATUS ksmdbgssnUnmapFrom_DISPATCH(struct KernelSMDebuggerSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnUnmapFrom__(pResource, pParams);
}

static inline NvU32 ksmdbgssnGetRefCount_DISPATCH(struct KernelSMDebuggerSession *pResource) {
    return pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetRefCount__(pResource);
}

static inline void ksmdbgssnAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelSMDebuggerSession *pResource, RsResourceRef *pReference) {
    pResource->__nvoc_metadata_ptr->vtable.__ksmdbgssnAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline PEVENTNOTIFICATION * ksmdbgssnGetNotificationListPtr_DISPATCH(struct KernelSMDebuggerSession *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare * ksmdbgssnGetNotificationShare_DISPATCH(struct KernelSMDebuggerSession *pNotifier) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetNotificationShare__(pNotifier);
}

static inline void ksmdbgssnSetNotificationShare_DISPATCH(struct KernelSMDebuggerSession *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__nvoc_metadata_ptr->vtable.__ksmdbgssnSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NV_STATUS ksmdbgssnUnregisterEvent_DISPATCH(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__ksmdbgssnUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS ksmdbgssnGetOrAllocNotifShare_DISPATCH(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__nvoc_metadata_ptr->vtable.__ksmdbgssnGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

NV_STATUS ksmdbgssnInternalControlForward_IMPL(struct KernelSMDebuggerSession *arg1, NvU32 command, void *pParams, NvU32 size);

NvHandle ksmdbgssnGetInternalObjectHandle_IMPL(struct KernelSMDebuggerSession *arg1);

static inline NV_STATUS ksmdbgssnCtrlCmdSMDebugModeEnable_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365313U), ((void *)0), 0);
}

static inline NV_STATUS ksmdbgssnCtrlCmdSMDebugModeDisable_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365314U), ((void *)0), 0);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeMMUDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365315U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeMMUDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365316U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_MMU_GCC_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365336U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_MMU_GCC_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365337U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365328U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_ERRBAR_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365329U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetExceptionMask_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365317U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365319U), pParams, sizeof (*pParams));
}

NV_STATUS ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_IMPL(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *pParams);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365321U), pParams, sizeof (*pParams));
}

NV_STATUS ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_IMPL(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *pParams);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSuspendContext_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365325U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugResumeContext_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365326U), ((void *)0), 0);
}

NV_STATUS ksmdbgssnCtrlCmdReadSurface_IMPL(struct KernelSMDebuggerSession *arg1, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *arg2);

NV_STATUS ksmdbgssnCtrlCmdWriteSurface_IMPL(struct KernelSMDebuggerSession *arg1, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *arg2);

NV_STATUS ksmdbgssnCtrlCmdGetMappings_IMPL(struct KernelSMDebuggerSession *arg1, NV83DE_CTRL_DEBUG_GET_MAPPINGS_PARAMETERS *arg2);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365323U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_STEP_INTERRUPT_HANDLING_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365324U), pParams, sizeof (*pParams));
}

NV_STATUS ksmdbgssnCtrlCmdDebugReadMemory_IMPL(struct KernelSMDebuggerSession *arg1, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *arg2);

NV_STATUS ksmdbgssnCtrlCmdDebugWriteMemory_IMPL(struct KernelSMDebuggerSession *arg1, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *arg2);

NV_STATUS ksmdbgssnCtrlCmdDebugExecRegOps_IMPL(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *pParams);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365330U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_STOP_TRIGGER_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365331U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_RUN_TRIGGER_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365332U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SKIP_IDLE_WARP_DETECT_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365333U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_SINGLE_SM_DEBUGGER_STATUS_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365334U), pParams, sizeof (*pParams));
}

NV_STATUS ksmdbgssnCtrlCmdDebugReadBatchMemory_IMPL(struct KernelSMDebuggerSession *arg1, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *arg2);

NV_STATUS ksmdbgssnCtrlCmdDebugWriteBatchMemory_IMPL(struct KernelSMDebuggerSession *arg1, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *arg2);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_46f6a7(struct KernelSMDebuggerSession *arg1, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *arg2) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_IMPL(struct KernelSMDebuggerSession *arg1, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *arg2);

NV_STATUS ksmdbgssnConstruct_IMPL(struct KernelSMDebuggerSession *arg_pKernelSMDebuggerSession, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_ksmdbgssnConstruct(arg_pKernelSMDebuggerSession, arg_pCallContext, arg_pParams) ksmdbgssnConstruct_IMPL(arg_pKernelSMDebuggerSession, arg_pCallContext, arg_pParams)
void ksmdbgssnDestruct_IMPL(struct KernelSMDebuggerSession *arg1);

#define __nvoc_ksmdbgssnDestruct(arg1) ksmdbgssnDestruct_IMPL(arg1)
void ksmdbgssnFreeCallback_IMPL(struct KernelSMDebuggerSession *arg1);

#ifdef __nvoc_kernel_sm_debugger_session_h_disabled
static inline void ksmdbgssnFreeCallback(struct KernelSMDebuggerSession *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelSMDebuggerSession was disabled!");
}
#else //__nvoc_kernel_sm_debugger_session_h_disabled
#define ksmdbgssnFreeCallback(arg1) ksmdbgssnFreeCallback_IMPL(arg1)
#endif //__nvoc_kernel_sm_debugger_session_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_SM_DEBUGGER_SESSION_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_SM_DEBUGGER_SESSION_NVOC_H_
