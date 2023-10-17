#ifndef _G_KERNEL_SM_DEBUGGER_SESSION_NVOC_H_
#define _G_KERNEL_SM_DEBUGGER_SESSION_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifdef NVOC_KERNEL_SM_DEBUGGER_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct RmDebuggerSession {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct RsSession __nvoc_base_RsSession;
    struct Object *__nvoc_pbase_Object;
    struct RsShared *__nvoc_pbase_RsShared;
    struct RsSession *__nvoc_pbase_RsSession;
    struct RmDebuggerSession *__nvoc_pbase_RmDebuggerSession;
    void (*__dbgSessionRemoveDependant__)(struct RmDebuggerSession *, struct RsResourceRef *);
    void (*__dbgSessionRemoveDependency__)(struct RmDebuggerSession *, struct RsResourceRef *);
};

#ifndef __NVOC_CLASS_RmDebuggerSession_TYPEDEF__
#define __NVOC_CLASS_RmDebuggerSession_TYPEDEF__
typedef struct RmDebuggerSession RmDebuggerSession;
#endif /* __NVOC_CLASS_RmDebuggerSession_TYPEDEF__ */

#ifndef __nvoc_class_id_RmDebuggerSession
#define __nvoc_class_id_RmDebuggerSession 0x48fa7d
#endif /* __nvoc_class_id_RmDebuggerSession */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmDebuggerSession;

#define __staticCast_RmDebuggerSession(pThis) \
    ((pThis)->__nvoc_pbase_RmDebuggerSession)

#ifdef __nvoc_kernel_sm_debugger_session_h_disabled
#define __dynamicCast_RmDebuggerSession(pThis) ((RmDebuggerSession*)NULL)
#else //__nvoc_kernel_sm_debugger_session_h_disabled
#define __dynamicCast_RmDebuggerSession(pThis) \
    ((RmDebuggerSession*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(RmDebuggerSession)))
#endif //__nvoc_kernel_sm_debugger_session_h_disabled


NV_STATUS __nvoc_objCreateDynamic_RmDebuggerSession(RmDebuggerSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_RmDebuggerSession(RmDebuggerSession**, Dynamic*, NvU32);
#define __objCreate_RmDebuggerSession(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_RmDebuggerSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define dbgSessionRemoveDependant(pDbgSession, pResourceRef) dbgSessionRemoveDependant_DISPATCH(pDbgSession, pResourceRef)
#define dbgSessionRemoveDependency(pDbgSession, pResourceRef) dbgSessionRemoveDependency_DISPATCH(pDbgSession, pResourceRef)
void dbgSessionRemoveDependant_IMPL(struct RmDebuggerSession *pDbgSession, struct RsResourceRef *pResourceRef);

static inline void dbgSessionRemoveDependant_DISPATCH(struct RmDebuggerSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    pDbgSession->__dbgSessionRemoveDependant__(pDbgSession, pResourceRef);
}

void dbgSessionRemoveDependency_IMPL(struct RmDebuggerSession *pDbgSession, struct RsResourceRef *pResourceRef);

static inline void dbgSessionRemoveDependency_DISPATCH(struct RmDebuggerSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    pDbgSession->__dbgSessionRemoveDependency__(pDbgSession, pResourceRef);
}

#undef PRIVATE_FIELD


#ifdef NVOC_KERNEL_SM_DEBUGGER_SESSION_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelSMDebuggerSession {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct GpuResource __nvoc_base_GpuResource;
    struct Notifier __nvoc_base_Notifier;
    struct Object *__nvoc_pbase_Object;
    struct RsResource *__nvoc_pbase_RsResource;
    struct RmResourceCommon *__nvoc_pbase_RmResourceCommon;
    struct RmResource *__nvoc_pbase_RmResource;
    struct GpuResource *__nvoc_pbase_GpuResource;
    struct INotifier *__nvoc_pbase_INotifier;
    struct Notifier *__nvoc_pbase_Notifier;
    struct KernelSMDebuggerSession *__nvoc_pbase_KernelSMDebuggerSession;
    NV_STATUS (*__ksmdbgssnInternalControlForward__)(struct KernelSMDebuggerSession *, NvU32, void *, NvU32);
    NvHandle (*__ksmdbgssnGetInternalObjectHandle__)(struct KernelSMDebuggerSession *);
    NV_STATUS (*__ksmdbgssnCtrlCmdSMDebugModeEnable__)(struct KernelSMDebuggerSession *);
    NV_STATUS (*__ksmdbgssnCtrlCmdSMDebugModeDisable__)(struct KernelSMDebuggerSession *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetModeMMUDebug__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugGetModeMMUDebug__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetModeErrbarDebug__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugGetModeErrbarDebug__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_GET_MODE_ERRBAR_DEBUG_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetExceptionMask__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadSingleSmErrorState__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadAllSmErrorStates__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugClearSingleSmErrorState__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugClearAllSmErrorStates__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSuspendContext__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugResumeContext__)(struct KernelSMDebuggerSession *);
    NV_STATUS (*__ksmdbgssnCtrlCmdReadSurface__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdWriteSurface__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdGetMappings__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_GET_MAPPINGS_PARAMETERS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetNextStopTriggerType__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_SET_SINGLE_STEP_INTERRUPT_HANDLING_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadMemory__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugWriteMemory__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugExecRegOps__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_STOP_TRIGGER_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_RUN_TRIGGER_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SKIP_IDLE_WARP_DETECT_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_GET_SINGLE_SM_DEBUGGER_STATUS_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadBatchMemory__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugWriteBatchMemory__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *);
    NV_STATUS (*__ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__)(struct KernelSMDebuggerSession *, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *);
    NvBool (*__ksmdbgssnShareCallback__)(struct KernelSMDebuggerSession *, struct RsClient *, struct RsResourceRef *, RS_SHARE_POLICY *);
    NV_STATUS (*__ksmdbgssnCheckMemInterUnmap__)(struct KernelSMDebuggerSession *, NvBool);
    NV_STATUS (*__ksmdbgssnGetOrAllocNotifShare__)(struct KernelSMDebuggerSession *, NvHandle, NvHandle, struct NotifShare **);
    NV_STATUS (*__ksmdbgssnMapTo__)(struct KernelSMDebuggerSession *, RS_RES_MAP_TO_PARAMS *);
    NV_STATUS (*__ksmdbgssnGetMapAddrSpace__)(struct KernelSMDebuggerSession *, struct CALL_CONTEXT *, NvU32, NV_ADDRESS_SPACE *);
    void (*__ksmdbgssnSetNotificationShare__)(struct KernelSMDebuggerSession *, struct NotifShare *);
    NvU32 (*__ksmdbgssnGetRefCount__)(struct KernelSMDebuggerSession *);
    void (*__ksmdbgssnAddAdditionalDependants__)(struct RsClient *, struct KernelSMDebuggerSession *, RsResourceRef *);
    NV_STATUS (*__ksmdbgssnControl_Prologue__)(struct KernelSMDebuggerSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__ksmdbgssnGetRegBaseOffsetAndSize__)(struct KernelSMDebuggerSession *, struct OBJGPU *, NvU32 *, NvU32 *);
    NV_STATUS (*__ksmdbgssnUnmapFrom__)(struct KernelSMDebuggerSession *, RS_RES_UNMAP_FROM_PARAMS *);
    void (*__ksmdbgssnControl_Epilogue__)(struct KernelSMDebuggerSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__ksmdbgssnControlLookup__)(struct KernelSMDebuggerSession *, struct RS_RES_CONTROL_PARAMS_INTERNAL *, const struct NVOC_EXPORTED_METHOD_DEF **);
    NV_STATUS (*__ksmdbgssnControl__)(struct KernelSMDebuggerSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__ksmdbgssnUnmap__)(struct KernelSMDebuggerSession *, struct CALL_CONTEXT *, struct RsCpuMapping *);
    NV_STATUS (*__ksmdbgssnGetMemInterMapParams__)(struct KernelSMDebuggerSession *, RMRES_MEM_INTER_MAP_PARAMS *);
    NV_STATUS (*__ksmdbgssnGetMemoryMappingDescriptor__)(struct KernelSMDebuggerSession *, struct MEMORY_DESCRIPTOR **);
    NV_STATUS (*__ksmdbgssnControlFilter__)(struct KernelSMDebuggerSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NV_STATUS (*__ksmdbgssnUnregisterEvent__)(struct KernelSMDebuggerSession *, NvHandle, NvHandle, NvHandle, NvHandle);
    NV_STATUS (*__ksmdbgssnControlSerialization_Prologue__)(struct KernelSMDebuggerSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    NvBool (*__ksmdbgssnCanCopy__)(struct KernelSMDebuggerSession *);
    void (*__ksmdbgssnPreDestruct__)(struct KernelSMDebuggerSession *);
    NV_STATUS (*__ksmdbgssnIsDuplicate__)(struct KernelSMDebuggerSession *, NvHandle, NvBool *);
    void (*__ksmdbgssnControlSerialization_Epilogue__)(struct KernelSMDebuggerSession *, struct CALL_CONTEXT *, struct RS_RES_CONTROL_PARAMS_INTERNAL *);
    PEVENTNOTIFICATION *(*__ksmdbgssnGetNotificationListPtr__)(struct KernelSMDebuggerSession *);
    struct NotifShare *(*__ksmdbgssnGetNotificationShare__)(struct KernelSMDebuggerSession *);
    NV_STATUS (*__ksmdbgssnMap__)(struct KernelSMDebuggerSession *, struct CALL_CONTEXT *, struct RS_CPU_MAP_PARAMS *, struct RsCpuMapping *);
    NvBool (*__ksmdbgssnAccessCallback__)(struct KernelSMDebuggerSession *, struct RsClient *, void *, RsAccessRight);
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

#ifndef __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
#define __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__
typedef struct KernelSMDebuggerSession KernelSMDebuggerSession;
#endif /* __NVOC_CLASS_KernelSMDebuggerSession_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelSMDebuggerSession
#define __nvoc_class_id_KernelSMDebuggerSession 0x4adc81
#endif /* __nvoc_class_id_KernelSMDebuggerSession */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSMDebuggerSession;

#define __staticCast_KernelSMDebuggerSession(pThis) \
    ((pThis)->__nvoc_pbase_KernelSMDebuggerSession)

#ifdef __nvoc_kernel_sm_debugger_session_h_disabled
#define __dynamicCast_KernelSMDebuggerSession(pThis) ((KernelSMDebuggerSession*)NULL)
#else //__nvoc_kernel_sm_debugger_session_h_disabled
#define __dynamicCast_KernelSMDebuggerSession(pThis) \
    ((KernelSMDebuggerSession*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelSMDebuggerSession)))
#endif //__nvoc_kernel_sm_debugger_session_h_disabled


NV_STATUS __nvoc_objCreateDynamic_KernelSMDebuggerSession(KernelSMDebuggerSession**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelSMDebuggerSession(KernelSMDebuggerSession**, Dynamic*, NvU32, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
#define __objCreate_KernelSMDebuggerSession(ppNewObj, pParent, createFlags, arg_pCallContext, arg_pParams) \
    __nvoc_objCreate_KernelSMDebuggerSession((ppNewObj), staticCast((pParent), Dynamic), (createFlags), arg_pCallContext, arg_pParams)

#define ksmdbgssnInternalControlForward(arg0, command, pParams, size) ksmdbgssnInternalControlForward_DISPATCH(arg0, command, pParams, size)
#define ksmdbgssnGetInternalObjectHandle(arg0) ksmdbgssnGetInternalObjectHandle_DISPATCH(arg0)
#define ksmdbgssnCtrlCmdSMDebugModeEnable(pKernelSMDebuggerSession) ksmdbgssnCtrlCmdSMDebugModeEnable_DISPATCH(pKernelSMDebuggerSession)
#define ksmdbgssnCtrlCmdSMDebugModeDisable(pKernelSMDebuggerSession) ksmdbgssnCtrlCmdSMDebugModeDisable_DISPATCH(pKernelSMDebuggerSession)
#define ksmdbgssnCtrlCmdDebugSetModeMMUDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetModeMMUDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugGetModeMMUDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugGetModeMMUDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetModeErrbarDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugGetModeErrbarDebug(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetExceptionMask(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetExceptionMask_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugReadSingleSmErrorState(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugReadAllSmErrorStates(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugClearSingleSmErrorState(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugClearAllSmErrorStates(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSuspendContext(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSuspendContext_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugResumeContext(pKernelSMDebuggerSession) ksmdbgssnCtrlCmdDebugResumeContext_DISPATCH(pKernelSMDebuggerSession)
#define ksmdbgssnCtrlCmdReadSurface(arg0, arg1) ksmdbgssnCtrlCmdReadSurface_DISPATCH(arg0, arg1)
#define ksmdbgssnCtrlCmdWriteSurface(arg0, arg1) ksmdbgssnCtrlCmdWriteSurface_DISPATCH(arg0, arg1)
#define ksmdbgssnCtrlCmdGetMappings(arg0, arg1) ksmdbgssnCtrlCmdGetMappings_DISPATCH(arg0, arg1)
#define ksmdbgssnCtrlCmdDebugSetNextStopTriggerType(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugReadMemory(arg0, arg1) ksmdbgssnCtrlCmdDebugReadMemory_DISPATCH(arg0, arg1)
#define ksmdbgssnCtrlCmdDebugWriteMemory(arg0, arg1) ksmdbgssnCtrlCmdDebugWriteMemory_DISPATCH(arg0, arg1)
#define ksmdbgssnCtrlCmdDebugExecRegOps(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugExecRegOps_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus(pKernelSMDebuggerSession, pParams) ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_DISPATCH(pKernelSMDebuggerSession, pParams)
#define ksmdbgssnCtrlCmdDebugReadBatchMemory(arg0, arg1) ksmdbgssnCtrlCmdDebugReadBatchMemory_DISPATCH(arg0, arg1)
#define ksmdbgssnCtrlCmdDebugWriteBatchMemory(arg0, arg1) ksmdbgssnCtrlCmdDebugWriteBatchMemory_DISPATCH(arg0, arg1)
#define ksmdbgssnCtrlCmdDebugReadMMUFaultInfo(arg0, arg1) ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_DISPATCH(arg0, arg1)
#define ksmdbgssnShareCallback(pGpuResource, pInvokingClient, pParentRef, pSharePolicy) ksmdbgssnShareCallback_DISPATCH(pGpuResource, pInvokingClient, pParentRef, pSharePolicy)
#define ksmdbgssnCheckMemInterUnmap(pRmResource, bSubdeviceHandleProvided) ksmdbgssnCheckMemInterUnmap_DISPATCH(pRmResource, bSubdeviceHandleProvided)
#define ksmdbgssnGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare) ksmdbgssnGetOrAllocNotifShare_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare)
#define ksmdbgssnMapTo(pResource, pParams) ksmdbgssnMapTo_DISPATCH(pResource, pParams)
#define ksmdbgssnGetMapAddrSpace(pGpuResource, pCallContext, mapFlags, pAddrSpace) ksmdbgssnGetMapAddrSpace_DISPATCH(pGpuResource, pCallContext, mapFlags, pAddrSpace)
#define ksmdbgssnSetNotificationShare(pNotifier, pNotifShare) ksmdbgssnSetNotificationShare_DISPATCH(pNotifier, pNotifShare)
#define ksmdbgssnGetRefCount(pResource) ksmdbgssnGetRefCount_DISPATCH(pResource)
#define ksmdbgssnAddAdditionalDependants(pClient, pResource, pReference) ksmdbgssnAddAdditionalDependants_DISPATCH(pClient, pResource, pReference)
#define ksmdbgssnControl_Prologue(pResource, pCallContext, pParams) ksmdbgssnControl_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnGetRegBaseOffsetAndSize(pGpuResource, pGpu, pOffset, pSize) ksmdbgssnGetRegBaseOffsetAndSize_DISPATCH(pGpuResource, pGpu, pOffset, pSize)
#define ksmdbgssnUnmapFrom(pResource, pParams) ksmdbgssnUnmapFrom_DISPATCH(pResource, pParams)
#define ksmdbgssnControl_Epilogue(pResource, pCallContext, pParams) ksmdbgssnControl_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnControlLookup(pResource, pParams, ppEntry) ksmdbgssnControlLookup_DISPATCH(pResource, pParams, ppEntry)
#define ksmdbgssnControl(pGpuResource, pCallContext, pParams) ksmdbgssnControl_DISPATCH(pGpuResource, pCallContext, pParams)
#define ksmdbgssnUnmap(pGpuResource, pCallContext, pCpuMapping) ksmdbgssnUnmap_DISPATCH(pGpuResource, pCallContext, pCpuMapping)
#define ksmdbgssnGetMemInterMapParams(pRmResource, pParams) ksmdbgssnGetMemInterMapParams_DISPATCH(pRmResource, pParams)
#define ksmdbgssnGetMemoryMappingDescriptor(pRmResource, ppMemDesc) ksmdbgssnGetMemoryMappingDescriptor_DISPATCH(pRmResource, ppMemDesc)
#define ksmdbgssnControlFilter(pResource, pCallContext, pParams) ksmdbgssnControlFilter_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnUnregisterEvent(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent) ksmdbgssnUnregisterEvent_DISPATCH(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent)
#define ksmdbgssnControlSerialization_Prologue(pResource, pCallContext, pParams) ksmdbgssnControlSerialization_Prologue_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnCanCopy(pResource) ksmdbgssnCanCopy_DISPATCH(pResource)
#define ksmdbgssnPreDestruct(pResource) ksmdbgssnPreDestruct_DISPATCH(pResource)
#define ksmdbgssnIsDuplicate(pResource, hMemory, pDuplicate) ksmdbgssnIsDuplicate_DISPATCH(pResource, hMemory, pDuplicate)
#define ksmdbgssnControlSerialization_Epilogue(pResource, pCallContext, pParams) ksmdbgssnControlSerialization_Epilogue_DISPATCH(pResource, pCallContext, pParams)
#define ksmdbgssnGetNotificationListPtr(pNotifier) ksmdbgssnGetNotificationListPtr_DISPATCH(pNotifier)
#define ksmdbgssnGetNotificationShare(pNotifier) ksmdbgssnGetNotificationShare_DISPATCH(pNotifier)
#define ksmdbgssnMap(pGpuResource, pCallContext, pParams, pCpuMapping) ksmdbgssnMap_DISPATCH(pGpuResource, pCallContext, pParams, pCpuMapping)
#define ksmdbgssnAccessCallback(pResource, pInvokingClient, pAllocParams, accessRight) ksmdbgssnAccessCallback_DISPATCH(pResource, pInvokingClient, pAllocParams, accessRight)
NV_STATUS ksmdbgssnInternalControlForward_IMPL(struct KernelSMDebuggerSession *arg0, NvU32 command, void *pParams, NvU32 size);

static inline NV_STATUS ksmdbgssnInternalControlForward_DISPATCH(struct KernelSMDebuggerSession *arg0, NvU32 command, void *pParams, NvU32 size) {
    return arg0->__ksmdbgssnInternalControlForward__(arg0, command, pParams, size);
}

NvHandle ksmdbgssnGetInternalObjectHandle_IMPL(struct KernelSMDebuggerSession *arg0);

static inline NvHandle ksmdbgssnGetInternalObjectHandle_DISPATCH(struct KernelSMDebuggerSession *arg0) {
    return arg0->__ksmdbgssnGetInternalObjectHandle__(arg0);
}

static inline NV_STATUS ksmdbgssnCtrlCmdSMDebugModeEnable_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365313U), ((void *)0), 0);
}

static inline NV_STATUS ksmdbgssnCtrlCmdSMDebugModeEnable_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdSMDebugModeEnable__(pKernelSMDebuggerSession);
}

static inline NV_STATUS ksmdbgssnCtrlCmdSMDebugModeDisable_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365314U), ((void *)0), 0);
}

static inline NV_STATUS ksmdbgssnCtrlCmdSMDebugModeDisable_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdSMDebugModeDisable__(pKernelSMDebuggerSession);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeMMUDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365315U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeMMUDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetModeMMUDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeMMUDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365316U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeMMUDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetModeMMUDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365328U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetModeErrbarDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_ERRBAR_DEBUG_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365329U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_MODE_ERRBAR_DEBUG_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetModeErrbarDebug__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetExceptionMask_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365317U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetExceptionMask_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetExceptionMask__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365319U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugReadSingleSmErrorState__(pKernelSMDebuggerSession, pParams);
}

NV_STATUS ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_IMPL(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *pParams);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugReadAllSmErrorStates__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365321U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugClearSingleSmErrorState__(pKernelSMDebuggerSession, pParams);
}

NV_STATUS ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_IMPL(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *pParams);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugClearAllSmErrorStates__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSuspendContext_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365325U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSuspendContext_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSuspendContext__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugResumeContext_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365326U), ((void *)0), 0);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugResumeContext_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugResumeContext__(pKernelSMDebuggerSession);
}

NV_STATUS ksmdbgssnCtrlCmdReadSurface_IMPL(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *arg1);

static inline NV_STATUS ksmdbgssnCtrlCmdReadSurface_DISPATCH(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *arg1) {
    return arg0->__ksmdbgssnCtrlCmdReadSurface__(arg0, arg1);
}

NV_STATUS ksmdbgssnCtrlCmdWriteSurface_IMPL(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *arg1);

static inline NV_STATUS ksmdbgssnCtrlCmdWriteSurface_DISPATCH(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS *arg1) {
    return arg0->__ksmdbgssnCtrlCmdWriteSurface__(arg0, arg1);
}

NV_STATUS ksmdbgssnCtrlCmdGetMappings_IMPL(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_GET_MAPPINGS_PARAMETERS *arg1);

static inline NV_STATUS ksmdbgssnCtrlCmdGetMappings_DISPATCH(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_GET_MAPPINGS_PARAMETERS *arg1) {
    return arg0->__ksmdbgssnCtrlCmdGetMappings__(arg0, arg1);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365323U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetNextStopTriggerType__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_STEP_INTERRUPT_HANDLING_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365324U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_STEP_INTERRUPT_HANDLING_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling__(pKernelSMDebuggerSession, pParams);
}

NV_STATUS ksmdbgssnCtrlCmdDebugReadMemory_IMPL(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *arg1);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadMemory_DISPATCH(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS *arg1) {
    return arg0->__ksmdbgssnCtrlCmdDebugReadMemory__(arg0, arg1);
}

NV_STATUS ksmdbgssnCtrlCmdDebugWriteMemory_IMPL(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *arg1);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugWriteMemory_DISPATCH(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS *arg1) {
    return arg0->__ksmdbgssnCtrlCmdDebugWriteMemory__(arg0, arg1);
}

NV_STATUS ksmdbgssnCtrlCmdDebugExecRegOps_IMPL(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *pParams);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugExecRegOps_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugExecRegOps__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365330U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_STOP_TRIGGER_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365331U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_STOP_TRIGGER_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_RUN_TRIGGER_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365332U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_RUN_TRIGGER_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SKIP_IDLE_WARP_DETECT_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365333U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SKIP_IDLE_WARP_DETECT_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect__(pKernelSMDebuggerSession, pParams);
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_fcf1ac(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_SINGLE_SM_DEBUGGER_STATUS_PARAMS *pParams) {
    return ksmdbgssnInternalControlForward(pKernelSMDebuggerSession, (2212365334U), pParams, sizeof (*pParams));
}

static inline NV_STATUS ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_DISPATCH(struct KernelSMDebuggerSession *pKernelSMDebuggerSession, NV83DE_CTRL_DEBUG_GET_SINGLE_SM_DEBUGGER_STATUS_PARAMS *pParams) {
    return pKernelSMDebuggerSession->__ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus__(pKernelSMDebuggerSession, pParams);
}

NV_STATUS ksmdbgssnCtrlCmdDebugReadBatchMemory_IMPL(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *arg1);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadBatchMemory_DISPATCH(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *arg1) {
    return arg0->__ksmdbgssnCtrlCmdDebugReadBatchMemory__(arg0, arg1);
}

NV_STATUS ksmdbgssnCtrlCmdDebugWriteBatchMemory_IMPL(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *arg1);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugWriteBatchMemory_DISPATCH(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS *arg1) {
    return arg0->__ksmdbgssnCtrlCmdDebugWriteBatchMemory__(arg0, arg1);
}

NV_STATUS ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_IMPL(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *arg1);

static inline NV_STATUS ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_DISPATCH(struct KernelSMDebuggerSession *arg0, NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS *arg1) {
    return arg0->__ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__(arg0, arg1);
}

static inline NvBool ksmdbgssnShareCallback_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return pGpuResource->__ksmdbgssnShareCallback__(pGpuResource, pInvokingClient, pParentRef, pSharePolicy);
}

static inline NV_STATUS ksmdbgssnCheckMemInterUnmap_DISPATCH(struct KernelSMDebuggerSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return pRmResource->__ksmdbgssnCheckMemInterUnmap__(pRmResource, bSubdeviceHandleProvided);
}

static inline NV_STATUS ksmdbgssnGetOrAllocNotifShare_DISPATCH(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return pNotifier->__ksmdbgssnGetOrAllocNotifShare__(pNotifier, hNotifierClient, hNotifierResource, ppNotifShare);
}

static inline NV_STATUS ksmdbgssnMapTo_DISPATCH(struct KernelSMDebuggerSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return pResource->__ksmdbgssnMapTo__(pResource, pParams);
}

static inline NV_STATUS ksmdbgssnGetMapAddrSpace_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return pGpuResource->__ksmdbgssnGetMapAddrSpace__(pGpuResource, pCallContext, mapFlags, pAddrSpace);
}

static inline void ksmdbgssnSetNotificationShare_DISPATCH(struct KernelSMDebuggerSession *pNotifier, struct NotifShare *pNotifShare) {
    pNotifier->__ksmdbgssnSetNotificationShare__(pNotifier, pNotifShare);
}

static inline NvU32 ksmdbgssnGetRefCount_DISPATCH(struct KernelSMDebuggerSession *pResource) {
    return pResource->__ksmdbgssnGetRefCount__(pResource);
}

static inline void ksmdbgssnAddAdditionalDependants_DISPATCH(struct RsClient *pClient, struct KernelSMDebuggerSession *pResource, RsResourceRef *pReference) {
    pResource->__ksmdbgssnAddAdditionalDependants__(pClient, pResource, pReference);
}

static inline NV_STATUS ksmdbgssnControl_Prologue_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ksmdbgssnControl_Prologue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ksmdbgssnGetRegBaseOffsetAndSize_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return pGpuResource->__ksmdbgssnGetRegBaseOffsetAndSize__(pGpuResource, pGpu, pOffset, pSize);
}

static inline NV_STATUS ksmdbgssnUnmapFrom_DISPATCH(struct KernelSMDebuggerSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return pResource->__ksmdbgssnUnmapFrom__(pResource, pParams);
}

static inline void ksmdbgssnControl_Epilogue_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__ksmdbgssnControl_Epilogue__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ksmdbgssnControlLookup_DISPATCH(struct KernelSMDebuggerSession *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return pResource->__ksmdbgssnControlLookup__(pResource, pParams, ppEntry);
}

static inline NV_STATUS ksmdbgssnControl_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pGpuResource->__ksmdbgssnControl__(pGpuResource, pCallContext, pParams);
}

static inline NV_STATUS ksmdbgssnUnmap_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__ksmdbgssnUnmap__(pGpuResource, pCallContext, pCpuMapping);
}

static inline NV_STATUS ksmdbgssnGetMemInterMapParams_DISPATCH(struct KernelSMDebuggerSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return pRmResource->__ksmdbgssnGetMemInterMapParams__(pRmResource, pParams);
}

static inline NV_STATUS ksmdbgssnGetMemoryMappingDescriptor_DISPATCH(struct KernelSMDebuggerSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return pRmResource->__ksmdbgssnGetMemoryMappingDescriptor__(pRmResource, ppMemDesc);
}

static inline NV_STATUS ksmdbgssnControlFilter_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ksmdbgssnControlFilter__(pResource, pCallContext, pParams);
}

static inline NV_STATUS ksmdbgssnUnregisterEvent_DISPATCH(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return pNotifier->__ksmdbgssnUnregisterEvent__(pNotifier, hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static inline NV_STATUS ksmdbgssnControlSerialization_Prologue_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return pResource->__ksmdbgssnControlSerialization_Prologue__(pResource, pCallContext, pParams);
}

static inline NvBool ksmdbgssnCanCopy_DISPATCH(struct KernelSMDebuggerSession *pResource) {
    return pResource->__ksmdbgssnCanCopy__(pResource);
}

static inline void ksmdbgssnPreDestruct_DISPATCH(struct KernelSMDebuggerSession *pResource) {
    pResource->__ksmdbgssnPreDestruct__(pResource);
}

static inline NV_STATUS ksmdbgssnIsDuplicate_DISPATCH(struct KernelSMDebuggerSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return pResource->__ksmdbgssnIsDuplicate__(pResource, hMemory, pDuplicate);
}

static inline void ksmdbgssnControlSerialization_Epilogue_DISPATCH(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    pResource->__ksmdbgssnControlSerialization_Epilogue__(pResource, pCallContext, pParams);
}

static inline PEVENTNOTIFICATION *ksmdbgssnGetNotificationListPtr_DISPATCH(struct KernelSMDebuggerSession *pNotifier) {
    return pNotifier->__ksmdbgssnGetNotificationListPtr__(pNotifier);
}

static inline struct NotifShare *ksmdbgssnGetNotificationShare_DISPATCH(struct KernelSMDebuggerSession *pNotifier) {
    return pNotifier->__ksmdbgssnGetNotificationShare__(pNotifier);
}

static inline NV_STATUS ksmdbgssnMap_DISPATCH(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return pGpuResource->__ksmdbgssnMap__(pGpuResource, pCallContext, pParams, pCpuMapping);
}

static inline NvBool ksmdbgssnAccessCallback_DISPATCH(struct KernelSMDebuggerSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return pResource->__ksmdbgssnAccessCallback__(pResource, pInvokingClient, pAllocParams, accessRight);
}

NV_STATUS ksmdbgssnConstruct_IMPL(struct KernelSMDebuggerSession *arg_pKernelSMDebuggerSession, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);

#define __nvoc_ksmdbgssnConstruct(arg_pKernelSMDebuggerSession, arg_pCallContext, arg_pParams) ksmdbgssnConstruct_IMPL(arg_pKernelSMDebuggerSession, arg_pCallContext, arg_pParams)
void ksmdbgssnDestruct_IMPL(struct KernelSMDebuggerSession *arg0);

#define __nvoc_ksmdbgssnDestruct(arg0) ksmdbgssnDestruct_IMPL(arg0)
void ksmdbgssnFreeCallback_IMPL(struct KernelSMDebuggerSession *arg0);

#ifdef __nvoc_kernel_sm_debugger_session_h_disabled
static inline void ksmdbgssnFreeCallback(struct KernelSMDebuggerSession *arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelSMDebuggerSession was disabled!");
}
#else //__nvoc_kernel_sm_debugger_session_h_disabled
#define ksmdbgssnFreeCallback(arg0) ksmdbgssnFreeCallback_IMPL(arg0)
#endif //__nvoc_kernel_sm_debugger_session_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_SM_DEBUGGER_SESSION_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_SM_DEBUGGER_SESSION_NVOC_H_
