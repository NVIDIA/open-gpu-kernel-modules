#define NVOC_KERNEL_SM_DEBUGGER_SESSION_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_sm_debugger_session_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x48fa7d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmDebuggerSession;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsSession;

void __nvoc_init_RmDebuggerSession(RmDebuggerSession*);
void __nvoc_init_funcTable_RmDebuggerSession(RmDebuggerSession*);
NV_STATUS __nvoc_ctor_RmDebuggerSession(RmDebuggerSession*);
void __nvoc_init_dataField_RmDebuggerSession(RmDebuggerSession*);
void __nvoc_dtor_RmDebuggerSession(RmDebuggerSession*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RmDebuggerSession;

static const struct NVOC_RTTI __nvoc_rtti_RmDebuggerSession_RmDebuggerSession = {
    /*pClassDef=*/          &__nvoc_class_def_RmDebuggerSession,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RmDebuggerSession,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RmDebuggerSession_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_RmDebuggerSession_RsShared = {
    /*pClassDef=*/          &__nvoc_class_def_RsShared,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession.__nvoc_base_RsShared),
};

static const struct NVOC_RTTI __nvoc_rtti_RmDebuggerSession_RsSession = {
    /*pClassDef=*/          &__nvoc_class_def_RsSession,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RmDebuggerSession = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_RmDebuggerSession_RmDebuggerSession,
        &__nvoc_rtti_RmDebuggerSession_RsSession,
        &__nvoc_rtti_RmDebuggerSession_RsShared,
        &__nvoc_rtti_RmDebuggerSession_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_RmDebuggerSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RmDebuggerSession),
        /*classId=*/            classId(RmDebuggerSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RmDebuggerSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RmDebuggerSession,
    /*pCastInfo=*/          &__nvoc_castinfo_RmDebuggerSession,
    /*pExportInfo=*/        &__nvoc_export_info_RmDebuggerSession
};

// 2 down-thunk(s) defined to bridge methods in RmDebuggerSession from superclasses

// dbgSessionRemoveDependant: virtual override (session) base (session)
static void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependant(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    dbgSessionRemoveDependant((struct RmDebuggerSession *)(((unsigned char *) pDbgSession) - __nvoc_rtti_RmDebuggerSession_RsSession.offset), pResourceRef);
}

// dbgSessionRemoveDependency: virtual override (session) base (session)
static void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependency(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    dbgSessionRemoveDependency((struct RmDebuggerSession *)(((unsigned char *) pDbgSession) - __nvoc_rtti_RmDebuggerSession_RsSession.offset), pResourceRef);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_RmDebuggerSession = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsSession(RsSession*);
void __nvoc_dtor_RmDebuggerSession(RmDebuggerSession *pThis) {
    __nvoc_dtor_RsSession(&pThis->__nvoc_base_RsSession);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RmDebuggerSession(RmDebuggerSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsSession(RsSession* );
NV_STATUS __nvoc_ctor_RmDebuggerSession(RmDebuggerSession *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsSession(&pThis->__nvoc_base_RsSession);
    if (status != NV_OK) goto __nvoc_ctor_RmDebuggerSession_fail_RsSession;
    __nvoc_init_dataField_RmDebuggerSession(pThis);
    goto __nvoc_ctor_RmDebuggerSession_exit; // Success

__nvoc_ctor_RmDebuggerSession_fail_RsSession:
__nvoc_ctor_RmDebuggerSession_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RmDebuggerSession_1(RmDebuggerSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // dbgSessionRemoveDependant -- virtual override (session) base (session)
    pThis->__dbgSessionRemoveDependant__ = &dbgSessionRemoveDependant_IMPL;
    pThis->__nvoc_base_RsSession.__sessionRemoveDependant__ = &__nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependant;

    // dbgSessionRemoveDependency -- virtual override (session) base (session)
    pThis->__dbgSessionRemoveDependency__ = &dbgSessionRemoveDependency_IMPL;
    pThis->__nvoc_base_RsSession.__sessionRemoveDependency__ = &__nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependency;
} // End __nvoc_init_funcTable_RmDebuggerSession_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 2 virtual method(s).
void __nvoc_init_funcTable_RmDebuggerSession(RmDebuggerSession *pThis) {

    // Initialize vtable(s) with 2 per-object function pointer(s).
    __nvoc_init_funcTable_RmDebuggerSession_1(pThis);
}

void __nvoc_init_RsSession(RsSession*);
void __nvoc_init_RmDebuggerSession(RmDebuggerSession *pThis) {
    pThis->__nvoc_pbase_RmDebuggerSession = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsSession.__nvoc_base_RsShared;
    pThis->__nvoc_pbase_RsSession = &pThis->__nvoc_base_RsSession;
    __nvoc_init_RsSession(&pThis->__nvoc_base_RsSession);
    __nvoc_init_funcTable_RmDebuggerSession(pThis);
}

NV_STATUS __nvoc_objCreate_RmDebuggerSession(RmDebuggerSession **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    RmDebuggerSession *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(RmDebuggerSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(RmDebuggerSession));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RmDebuggerSession);

    pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_RmDebuggerSession(pThis);
    status = __nvoc_ctor_RmDebuggerSession(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_RmDebuggerSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_RmDebuggerSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(RmDebuggerSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RmDebuggerSession(RmDebuggerSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_RmDebuggerSession(ppThis, pParent, createFlags);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x4adc81 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSMDebuggerSession;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

void __nvoc_init_KernelSMDebuggerSession(KernelSMDebuggerSession*, RmHalspecOwner* );
void __nvoc_init_funcTable_KernelSMDebuggerSession(KernelSMDebuggerSession*, RmHalspecOwner* );
NV_STATUS __nvoc_ctor_KernelSMDebuggerSession(KernelSMDebuggerSession*, RmHalspecOwner* , struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_KernelSMDebuggerSession(KernelSMDebuggerSession*, RmHalspecOwner* );
void __nvoc_dtor_KernelSMDebuggerSession(KernelSMDebuggerSession*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSMDebuggerSession;

static const struct NVOC_RTTI __nvoc_rtti_KernelSMDebuggerSession_KernelSMDebuggerSession = {
    /*pClassDef=*/          &__nvoc_class_def_KernelSMDebuggerSession,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelSMDebuggerSession,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSMDebuggerSession_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSMDebuggerSession_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSMDebuggerSession_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSMDebuggerSession_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSMDebuggerSession_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSMDebuggerSession_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_Notifier.__nvoc_base_INotifier),
};

static const struct NVOC_RTTI __nvoc_rtti_KernelSMDebuggerSession_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_Notifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_KernelSMDebuggerSession = {
    /*numRelatives=*/       8,
    /*relatives=*/ {
        &__nvoc_rtti_KernelSMDebuggerSession_KernelSMDebuggerSession,
        &__nvoc_rtti_KernelSMDebuggerSession_Notifier,
        &__nvoc_rtti_KernelSMDebuggerSession_INotifier,
        &__nvoc_rtti_KernelSMDebuggerSession_GpuResource,
        &__nvoc_rtti_KernelSMDebuggerSession_RmResource,
        &__nvoc_rtti_KernelSMDebuggerSession_RmResourceCommon,
        &__nvoc_rtti_KernelSMDebuggerSession_RsResource,
        &__nvoc_rtti_KernelSMDebuggerSession_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSMDebuggerSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelSMDebuggerSession),
        /*classId=*/            classId(KernelSMDebuggerSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelSMDebuggerSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelSMDebuggerSession,
    /*pCastInfo=*/          &__nvoc_castinfo_KernelSMDebuggerSession,
    /*pExportInfo=*/        &__nvoc_export_info_KernelSMDebuggerSession
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelSMDebuggerSession[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdSMDebugModeEnable_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0301u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdSMDebugModeEnable"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdSMDebugModeDisable_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0302u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdSMDebugModeDisable"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetModeMMUDebug_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0307u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetModeMMUDebug"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugGetModeMMUDebug_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0308u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugGetModeMMUDebug"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetExceptionMask_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0309u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetExceptionMask"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de030bu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugReadSingleSmErrorState"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de030cu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugReadAllSmErrorStates"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de030fu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugClearSingleSmErrorState"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
        /*flags=*/      0x40048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0310u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugClearAllSmErrorStates"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0313u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetNextStopTriggerType"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0314u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_SINGLE_STEP_INTERRUPT_HANDLING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugReadMemory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0315u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_READ_MEMORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugReadMemory"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugWriteMemory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0316u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_WRITE_MEMORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugWriteMemory"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSuspendContext_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0317u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSuspendContext"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugResumeContext_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0318u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugResumeContext"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdReadSurface_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de031au,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdReadSurface"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdWriteSurface_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de031bu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_ACCESS_SURFACE_PARAMETERS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdWriteSurface"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdGetMappings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de031cu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_GET_MAPPINGS_PARAMETERS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdGetMappings"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugExecRegOps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
        /*flags=*/      0x208u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de031du,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugExecRegOps"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de031fu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetModeErrbarDebug"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0320u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_GET_MODE_ERRBAR_DEBUG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugGetModeErrbarDebug"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0321u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0322u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_SINGLE_SM_STOP_TRIGGER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0323u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_SINGLE_SM_RUN_TRIGGER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0324u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SKIP_IDLE_WARP_DETECT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0325u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_GET_SINGLE_SM_DEBUGGER_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugReadBatchMemory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0326u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugReadBatchMemory"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugWriteBatchMemory_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0327u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_ACCESS_MEMORY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugWriteBatchMemory"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_DISPATCH,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0328u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_READ_MMU_FAULT_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugReadMMUFaultInfo"
#endif
    },

};

// 2 down-thunk(s) defined to bridge methods in KernelSMDebuggerSession from superclasses

// ksmdbgssnInternalControlForward: virtual override (gpures) base (gpures)
static NV_STATUS __nvoc_down_thunk_KernelSMDebuggerSession_gpuresInternalControlForward(struct GpuResource *arg_this, NvU32 command, void *pParams, NvU32 size) {
    return ksmdbgssnInternalControlForward((struct KernelSMDebuggerSession *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelSMDebuggerSession_GpuResource.offset), command, pParams, size);
}

// ksmdbgssnGetInternalObjectHandle: virtual override (gpures) base (gpures)
static NvHandle __nvoc_down_thunk_KernelSMDebuggerSession_gpuresGetInternalObjectHandle(struct GpuResource *arg_this) {
    return ksmdbgssnGetInternalObjectHandle((struct KernelSMDebuggerSession *)(((unsigned char *) arg_this) - __nvoc_rtti_KernelSMDebuggerSession_GpuResource.offset));
}


// 28 up-thunk(s) defined to bridge methods in KernelSMDebuggerSession to superclasses

// ksmdbgssnControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnControl(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelSMDebuggerSession_GpuResource.offset), pCallContext, pParams);
}

// ksmdbgssnMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnMap(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelSMDebuggerSession_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// ksmdbgssnUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnUnmap(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelSMDebuggerSession_GpuResource.offset), pCallContext, pCpuMapping);
}

// ksmdbgssnShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_ksmdbgssnShareCallback(struct KernelSMDebuggerSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelSMDebuggerSession_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// ksmdbgssnGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnGetRegBaseOffsetAndSize(struct KernelSMDebuggerSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelSMDebuggerSession_GpuResource.offset), pGpu, pOffset, pSize);
}

// ksmdbgssnGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnGetMapAddrSpace(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_KernelSMDebuggerSession_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// ksmdbgssnAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_ksmdbgssnAccessCallback(struct KernelSMDebuggerSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// ksmdbgssnGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnGetMemInterMapParams(struct KernelSMDebuggerSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelSMDebuggerSession_RmResource.offset), pParams);
}

// ksmdbgssnCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnCheckMemInterUnmap(struct KernelSMDebuggerSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelSMDebuggerSession_RmResource.offset), bSubdeviceHandleProvided);
}

// ksmdbgssnGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnGetMemoryMappingDescriptor(struct KernelSMDebuggerSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_KernelSMDebuggerSession_RmResource.offset), ppMemDesc);
}

// ksmdbgssnControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Prologue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RmResource.offset), pCallContext, pParams);
}

// ksmdbgssnControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Epilogue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RmResource.offset), pCallContext, pParams);
}

// ksmdbgssnControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnControl_Prologue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RmResource.offset), pCallContext, pParams);
}

// ksmdbgssnControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_ksmdbgssnControl_Epilogue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RmResource.offset), pCallContext, pParams);
}

// ksmdbgssnCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_ksmdbgssnCanCopy(struct KernelSMDebuggerSession *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RsResource.offset));
}

// ksmdbgssnIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnIsDuplicate(struct KernelSMDebuggerSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RsResource.offset), hMemory, pDuplicate);
}

// ksmdbgssnPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_ksmdbgssnPreDestruct(struct KernelSMDebuggerSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RsResource.offset));
}

// ksmdbgssnControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnControlFilter(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RsResource.offset), pCallContext, pParams);
}

// ksmdbgssnIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_ksmdbgssnIsPartialUnmapSupported(struct KernelSMDebuggerSession *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RsResource.offset));
}

// ksmdbgssnMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnMapTo(struct KernelSMDebuggerSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RsResource.offset), pParams);
}

// ksmdbgssnUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnUnmapFrom(struct KernelSMDebuggerSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RsResource.offset), pParams);
}

// ksmdbgssnGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_ksmdbgssnGetRefCount(struct KernelSMDebuggerSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RsResource.offset));
}

// ksmdbgssnAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_ksmdbgssnAddAdditionalDependants(struct RsClient *pClient, struct KernelSMDebuggerSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_KernelSMDebuggerSession_RsResource.offset), pReference);
}

// ksmdbgssnGetNotificationListPtr: virtual inherited (notify) base (notify)
static PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationListPtr(struct KernelSMDebuggerSession *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelSMDebuggerSession_Notifier.offset));
}

// ksmdbgssnGetNotificationShare: virtual inherited (notify) base (notify)
static struct NotifShare * __nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationShare(struct KernelSMDebuggerSession *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelSMDebuggerSession_Notifier.offset));
}

// ksmdbgssnSetNotificationShare: virtual inherited (notify) base (notify)
static void __nvoc_up_thunk_Notifier_ksmdbgssnSetNotificationShare(struct KernelSMDebuggerSession *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelSMDebuggerSession_Notifier.offset), pNotifShare);
}

// ksmdbgssnUnregisterEvent: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_ksmdbgssnUnregisterEvent(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelSMDebuggerSession_Notifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// ksmdbgssnGetOrAllocNotifShare: virtual inherited (notify) base (notify)
static NV_STATUS __nvoc_up_thunk_Notifier_ksmdbgssnGetOrAllocNotifShare(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + __nvoc_rtti_KernelSMDebuggerSession_Notifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSMDebuggerSession = 
{
    /*numEntries=*/     29,
    /*pExportEntries=*/ __nvoc_exported_method_def_KernelSMDebuggerSession
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_KernelSMDebuggerSession(KernelSMDebuggerSession *pThis) {
    __nvoc_ksmdbgssnDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelSMDebuggerSession(KernelSMDebuggerSession *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_KernelSMDebuggerSession(KernelSMDebuggerSession *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelSMDebuggerSession_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_KernelSMDebuggerSession_fail_Notifier;
    __nvoc_init_dataField_KernelSMDebuggerSession(pThis, pRmhalspecowner);

    status = __nvoc_ksmdbgssnConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelSMDebuggerSession_fail__init;
    goto __nvoc_ctor_KernelSMDebuggerSession_exit; // Success

__nvoc_ctor_KernelSMDebuggerSession_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_KernelSMDebuggerSession_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_KernelSMDebuggerSession_fail_GpuResource:
__nvoc_ctor_KernelSMDebuggerSession_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelSMDebuggerSession_1(KernelSMDebuggerSession *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // ksmdbgssnInternalControlForward -- virtual override (gpures) base (gpures)
    pThis->__ksmdbgssnInternalControlForward__ = &ksmdbgssnInternalControlForward_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresInternalControlForward__ = &__nvoc_down_thunk_KernelSMDebuggerSession_gpuresInternalControlForward;

    // ksmdbgssnGetInternalObjectHandle -- virtual override (gpures) base (gpures)
    pThis->__ksmdbgssnGetInternalObjectHandle__ = &ksmdbgssnGetInternalObjectHandle_IMPL;
    pThis->__nvoc_base_GpuResource.__gpuresGetInternalObjectHandle__ = &__nvoc_down_thunk_KernelSMDebuggerSession_gpuresGetInternalObjectHandle;

    // ksmdbgssnCtrlCmdSMDebugModeEnable -- inline exported (id=0x83de0301) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__ksmdbgssnCtrlCmdSMDebugModeEnable__ = &ksmdbgssnCtrlCmdSMDebugModeEnable_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdSMDebugModeDisable -- inline exported (id=0x83de0302) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__ksmdbgssnCtrlCmdSMDebugModeDisable__ = &ksmdbgssnCtrlCmdSMDebugModeDisable_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugSetModeMMUDebug -- inline exported (id=0x83de0307) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugSetModeMMUDebug__ = &ksmdbgssnCtrlCmdDebugSetModeMMUDebug_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugGetModeMMUDebug -- inline exported (id=0x83de0308) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugGetModeMMUDebug__ = &ksmdbgssnCtrlCmdDebugGetModeMMUDebug_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugSetModeErrbarDebug -- inline exported (id=0x83de031f) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugSetModeErrbarDebug__ = &ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugGetModeErrbarDebug -- inline exported (id=0x83de0320) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__ksmdbgssnCtrlCmdDebugGetModeErrbarDebug__ = &ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugSetExceptionMask -- inline exported (id=0x83de0309) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugSetExceptionMask__ = &ksmdbgssnCtrlCmdDebugSetExceptionMask_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugReadSingleSmErrorState -- inline exported (id=0x83de030b) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugReadSingleSmErrorState__ = &ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugReadAllSmErrorStates -- exported (id=0x83de030c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
    pThis->__ksmdbgssnCtrlCmdDebugReadAllSmErrorStates__ = &ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_IMPL;
#endif

    // ksmdbgssnCtrlCmdDebugClearSingleSmErrorState -- inline exported (id=0x83de030f) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugClearSingleSmErrorState__ = &ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugClearAllSmErrorStates -- exported (id=0x83de0310)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40048u)
    pThis->__ksmdbgssnCtrlCmdDebugClearAllSmErrorStates__ = &ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_IMPL;
#endif

    // ksmdbgssnCtrlCmdDebugSuspendContext -- inline exported (id=0x83de0317) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugSuspendContext__ = &ksmdbgssnCtrlCmdDebugSuspendContext_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugResumeContext -- inline exported (id=0x83de0318) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugResumeContext__ = &ksmdbgssnCtrlCmdDebugResumeContext_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdReadSurface -- exported (id=0x83de031a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__ksmdbgssnCtrlCmdReadSurface__ = &ksmdbgssnCtrlCmdReadSurface_IMPL;
#endif

    // ksmdbgssnCtrlCmdWriteSurface -- exported (id=0x83de031b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__ksmdbgssnCtrlCmdWriteSurface__ = &ksmdbgssnCtrlCmdWriteSurface_IMPL;
#endif

    // ksmdbgssnCtrlCmdGetMappings -- exported (id=0x83de031c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__ksmdbgssnCtrlCmdGetMappings__ = &ksmdbgssnCtrlCmdGetMappings_IMPL;
#endif

    // ksmdbgssnCtrlCmdDebugSetNextStopTriggerType -- inline exported (id=0x83de0313) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugSetNextStopTriggerType__ = &ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling -- inline exported (id=0x83de0314) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling__ = &ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugReadMemory -- exported (id=0x83de0315)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__ksmdbgssnCtrlCmdDebugReadMemory__ = &ksmdbgssnCtrlCmdDebugReadMemory_IMPL;
#endif

    // ksmdbgssnCtrlCmdDebugWriteMemory -- exported (id=0x83de0316)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__ksmdbgssnCtrlCmdDebugWriteMemory__ = &ksmdbgssnCtrlCmdDebugWriteMemory_IMPL;
#endif

    // ksmdbgssnCtrlCmdDebugExecRegOps -- exported (id=0x83de031d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x208u)
    pThis->__ksmdbgssnCtrlCmdDebugExecRegOps__ = &ksmdbgssnCtrlCmdDebugExecRegOps_IMPL;
#endif

    // ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep -- inline exported (id=0x83de0321) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep__ = &ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger -- inline exported (id=0x83de0322) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger__ = &ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger -- inline exported (id=0x83de0323) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger__ = &ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect -- inline exported (id=0x83de0324) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect__ = &ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus -- inline exported (id=0x83de0325) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus__ = &ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugReadBatchMemory -- exported (id=0x83de0326)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__ksmdbgssnCtrlCmdDebugReadBatchMemory__ = &ksmdbgssnCtrlCmdDebugReadBatchMemory_IMPL;
#endif

    // ksmdbgssnCtrlCmdDebugWriteBatchMemory -- exported (id=0x83de0327)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__ksmdbgssnCtrlCmdDebugWriteBatchMemory__ = &ksmdbgssnCtrlCmdDebugWriteBatchMemory_IMPL;
#endif

    // ksmdbgssnCtrlCmdDebugReadMMUFaultInfo -- halified (2 hals) exported (id=0x83de0328)
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__ = &ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_46f6a7;
    }
    else
    {
        pThis->__ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__ = &ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_IMPL;
    }

    // ksmdbgssnControl -- virtual inherited (gpures) base (gpures)
    pThis->__ksmdbgssnControl__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnControl;

    // ksmdbgssnMap -- virtual inherited (gpures) base (gpures)
    pThis->__ksmdbgssnMap__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnMap;

    // ksmdbgssnUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__ksmdbgssnUnmap__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnUnmap;

    // ksmdbgssnShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__ksmdbgssnShareCallback__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnShareCallback;

    // ksmdbgssnGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__ksmdbgssnGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnGetRegBaseOffsetAndSize;

    // ksmdbgssnGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__ksmdbgssnGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnGetMapAddrSpace;

    // ksmdbgssnAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__ksmdbgssnAccessCallback__ = &__nvoc_up_thunk_RmResource_ksmdbgssnAccessCallback;

    // ksmdbgssnGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__ksmdbgssnGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_ksmdbgssnGetMemInterMapParams;

    // ksmdbgssnCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__ksmdbgssnCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_ksmdbgssnCheckMemInterUnmap;

    // ksmdbgssnGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__ksmdbgssnGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_ksmdbgssnGetMemoryMappingDescriptor;

    // ksmdbgssnControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__ksmdbgssnControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Prologue;

    // ksmdbgssnControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__ksmdbgssnControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Epilogue;

    // ksmdbgssnControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__ksmdbgssnControl_Prologue__ = &__nvoc_up_thunk_RmResource_ksmdbgssnControl_Prologue;

    // ksmdbgssnControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__ksmdbgssnControl_Epilogue__ = &__nvoc_up_thunk_RmResource_ksmdbgssnControl_Epilogue;

    // ksmdbgssnCanCopy -- virtual inherited (res) base (gpures)
    pThis->__ksmdbgssnCanCopy__ = &__nvoc_up_thunk_RsResource_ksmdbgssnCanCopy;

    // ksmdbgssnIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__ksmdbgssnIsDuplicate__ = &__nvoc_up_thunk_RsResource_ksmdbgssnIsDuplicate;

    // ksmdbgssnPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__ksmdbgssnPreDestruct__ = &__nvoc_up_thunk_RsResource_ksmdbgssnPreDestruct;

    // ksmdbgssnControlFilter -- virtual inherited (res) base (gpures)
    pThis->__ksmdbgssnControlFilter__ = &__nvoc_up_thunk_RsResource_ksmdbgssnControlFilter;

    // ksmdbgssnIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__ksmdbgssnIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_ksmdbgssnIsPartialUnmapSupported;

    // ksmdbgssnMapTo -- virtual inherited (res) base (gpures)
    pThis->__ksmdbgssnMapTo__ = &__nvoc_up_thunk_RsResource_ksmdbgssnMapTo;

    // ksmdbgssnUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__ksmdbgssnUnmapFrom__ = &__nvoc_up_thunk_RsResource_ksmdbgssnUnmapFrom;

    // ksmdbgssnGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__ksmdbgssnGetRefCount__ = &__nvoc_up_thunk_RsResource_ksmdbgssnGetRefCount;

    // ksmdbgssnAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__ksmdbgssnAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_ksmdbgssnAddAdditionalDependants;

    // ksmdbgssnGetNotificationListPtr -- virtual inherited (notify) base (notify)
    pThis->__ksmdbgssnGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationListPtr;

    // ksmdbgssnGetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__ksmdbgssnGetNotificationShare__ = &__nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationShare;

    // ksmdbgssnSetNotificationShare -- virtual inherited (notify) base (notify)
    pThis->__ksmdbgssnSetNotificationShare__ = &__nvoc_up_thunk_Notifier_ksmdbgssnSetNotificationShare;

    // ksmdbgssnUnregisterEvent -- virtual inherited (notify) base (notify)
    pThis->__ksmdbgssnUnregisterEvent__ = &__nvoc_up_thunk_Notifier_ksmdbgssnUnregisterEvent;

    // ksmdbgssnGetOrAllocNotifShare -- virtual inherited (notify) base (notify)
    pThis->__ksmdbgssnGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_ksmdbgssnGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_KernelSMDebuggerSession_1 with approximately 62 basic block(s).


// Initialize vtable(s) for 59 virtual method(s).
void __nvoc_init_funcTable_KernelSMDebuggerSession(KernelSMDebuggerSession *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 59 per-object function pointer(s).
    __nvoc_init_funcTable_KernelSMDebuggerSession_1(pThis, pRmhalspecowner);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_KernelSMDebuggerSession(KernelSMDebuggerSession *pThis, RmHalspecOwner *pRmhalspecowner) {
    pThis->__nvoc_pbase_KernelSMDebuggerSession = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_Notifier(&pThis->__nvoc_base_Notifier);
    __nvoc_init_funcTable_KernelSMDebuggerSession(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelSMDebuggerSession(KernelSMDebuggerSession **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelSMDebuggerSession *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelSMDebuggerSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelSMDebuggerSession));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_KernelSMDebuggerSession);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init_KernelSMDebuggerSession(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelSMDebuggerSession(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelSMDebuggerSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelSMDebuggerSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelSMDebuggerSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelSMDebuggerSession(KernelSMDebuggerSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelSMDebuggerSession(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

