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

// Down-thunk(s) to bridge RmDebuggerSession methods from ancestors (if any)
void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependant(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef);    // this
void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependency(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef);    // this

// 2 down-thunk(s) defined to bridge methods in RmDebuggerSession from superclasses

// dbgSessionRemoveDependant: virtual override (session) base (session)
void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependant(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    dbgSessionRemoveDependant((struct RmDebuggerSession *)(((unsigned char *) pDbgSession) - NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession)), pResourceRef);
}

// dbgSessionRemoveDependency: virtual override (session) base (session)
void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependency(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    dbgSessionRemoveDependency((struct RmDebuggerSession *)(((unsigned char *) pDbgSession) - NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession)), pResourceRef);
}


// Up-thunk(s) to bridge RmDebuggerSession methods to ancestors (if any)

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
} // End __nvoc_init_funcTable_RmDebuggerSession_1


// Initialize vtable(s) for 2 virtual method(s).
void __nvoc_init_funcTable_RmDebuggerSession(RmDebuggerSession *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__RmDebuggerSession vtable = {
        .__dbgSessionRemoveDependant__ = &dbgSessionRemoveDependant_IMPL,    // virtual override (session) base (session)
        .RsSession.__sessionRemoveDependant__ = &__nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependant,    // virtual
        .__dbgSessionRemoveDependency__ = &dbgSessionRemoveDependency_IMPL,    // virtual override (session) base (session)
        .RsSession.__sessionRemoveDependency__ = &__nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependency,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_RsSession.__nvoc_vtable = &vtable.RsSession;    // (session) super
    pThis->__nvoc_vtable = &vtable;    // (dbgSession) this
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
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de032au,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_MODE_MMU_GCC_DEBUG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug_fcf1ac,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*flags=*/      0x248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de032bu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_GET_MODE_MMU_GCC_DEBUG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug"
#endif
    },

};

// Down-thunk(s) to bridge KernelSMDebuggerSession methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_down_thunk_KernelSMDebuggerSession_gpuresInternalControlForward(struct GpuResource *arg_this, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_down_thunk_KernelSMDebuggerSession_gpuresGetInternalObjectHandle(struct GpuResource *arg_this);    // this

// 2 down-thunk(s) defined to bridge methods in KernelSMDebuggerSession from superclasses

// ksmdbgssnInternalControlForward: virtual override (gpures) base (gpures)
NV_STATUS __nvoc_down_thunk_KernelSMDebuggerSession_gpuresInternalControlForward(struct GpuResource *arg_this, NvU32 command, void *pParams, NvU32 size) {
    return ksmdbgssnInternalControlForward((struct KernelSMDebuggerSession *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)), command, pParams, size);
}

// ksmdbgssnGetInternalObjectHandle: virtual override (gpures) base (gpures)
NvHandle __nvoc_down_thunk_KernelSMDebuggerSession_gpuresGetInternalObjectHandle(struct GpuResource *arg_this) {
    return ksmdbgssnGetInternalObjectHandle((struct KernelSMDebuggerSession *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)));
}


// Up-thunk(s) to bridge KernelSMDebuggerSession methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnControl(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnMap(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnUnmap(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_ksmdbgssnShareCallback(struct KernelSMDebuggerSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnGetRegBaseOffsetAndSize(struct KernelSMDebuggerSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnGetMapAddrSpace(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NvBool __nvoc_up_thunk_RmResource_ksmdbgssnAccessCallback(struct KernelSMDebuggerSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnGetMemInterMapParams(struct KernelSMDebuggerSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnCheckMemInterUnmap(struct KernelSMDebuggerSession *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnGetMemoryMappingDescriptor(struct KernelSMDebuggerSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Prologue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Epilogue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnControl_Prologue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_ksmdbgssnControl_Epilogue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_ksmdbgssnCanCopy(struct KernelSMDebuggerSession *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnIsDuplicate(struct KernelSMDebuggerSession *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_ksmdbgssnPreDestruct(struct KernelSMDebuggerSession *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnControlFilter(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_ksmdbgssnIsPartialUnmapSupported(struct KernelSMDebuggerSession *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnMapTo(struct KernelSMDebuggerSession *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnUnmapFrom(struct KernelSMDebuggerSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_ksmdbgssnGetRefCount(struct KernelSMDebuggerSession *pResource);    // this
void __nvoc_up_thunk_RsResource_ksmdbgssnAddAdditionalDependants(struct RsClient *pClient, struct KernelSMDebuggerSession *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationListPtr(struct KernelSMDebuggerSession *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationShare(struct KernelSMDebuggerSession *pNotifier);    // this
void __nvoc_up_thunk_Notifier_ksmdbgssnSetNotificationShare(struct KernelSMDebuggerSession *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_ksmdbgssnUnregisterEvent(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_ksmdbgssnGetOrAllocNotifShare(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

// 28 up-thunk(s) defined to bridge methods in KernelSMDebuggerSession to superclasses

// ksmdbgssnControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnControl(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// ksmdbgssnMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnMap(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// ksmdbgssnUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnUnmap(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// ksmdbgssnShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_ksmdbgssnShareCallback(struct KernelSMDebuggerSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// ksmdbgssnGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnGetRegBaseOffsetAndSize(struct KernelSMDebuggerSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// ksmdbgssnGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_ksmdbgssnGetMapAddrSpace(struct KernelSMDebuggerSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// ksmdbgssnAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_ksmdbgssnAccessCallback(struct KernelSMDebuggerSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// ksmdbgssnGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnGetMemInterMapParams(struct KernelSMDebuggerSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// ksmdbgssnCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnCheckMemInterUnmap(struct KernelSMDebuggerSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// ksmdbgssnGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnGetMemoryMappingDescriptor(struct KernelSMDebuggerSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// ksmdbgssnControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Prologue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// ksmdbgssnControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Epilogue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// ksmdbgssnControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_ksmdbgssnControl_Prologue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// ksmdbgssnControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_ksmdbgssnControl_Epilogue(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// ksmdbgssnCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_ksmdbgssnCanCopy(struct KernelSMDebuggerSession *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// ksmdbgssnIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnIsDuplicate(struct KernelSMDebuggerSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// ksmdbgssnPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_ksmdbgssnPreDestruct(struct KernelSMDebuggerSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// ksmdbgssnControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnControlFilter(struct KernelSMDebuggerSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// ksmdbgssnIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_ksmdbgssnIsPartialUnmapSupported(struct KernelSMDebuggerSession *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// ksmdbgssnMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnMapTo(struct KernelSMDebuggerSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// ksmdbgssnUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_ksmdbgssnUnmapFrom(struct KernelSMDebuggerSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// ksmdbgssnGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_ksmdbgssnGetRefCount(struct KernelSMDebuggerSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// ksmdbgssnAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_ksmdbgssnAddAdditionalDependants(struct RsClient *pClient, struct KernelSMDebuggerSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// ksmdbgssnGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationListPtr(struct KernelSMDebuggerSession *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_Notifier)));
}

// ksmdbgssnGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationShare(struct KernelSMDebuggerSession *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_Notifier)));
}

// ksmdbgssnSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_ksmdbgssnSetNotificationShare(struct KernelSMDebuggerSession *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_Notifier)), pNotifShare);
}

// ksmdbgssnUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_ksmdbgssnUnregisterEvent(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// ksmdbgssnGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_ksmdbgssnGetOrAllocNotifShare(struct KernelSMDebuggerSession *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_KernelSMDebuggerSession = 
{
    /*numEntries=*/     31,
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

    // ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug -- inline exported (id=0x83de032a) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug__ = &ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug_fcf1ac;
#endif

    // ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug -- inline exported (id=0x83de032b) body
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
    pThis->__ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug__ = &ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug_fcf1ac;
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
} // End __nvoc_init_funcTable_KernelSMDebuggerSession_1 with approximately 32 basic block(s).


// Initialize vtable(s) for 61 virtual method(s).
void __nvoc_init_funcTable_KernelSMDebuggerSession(KernelSMDebuggerSession *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__KernelSMDebuggerSession vtable = {
        .__ksmdbgssnInternalControlForward__ = &ksmdbgssnInternalControlForward_IMPL,    // virtual override (gpures) base (gpures)
        .GpuResource.__gpuresInternalControlForward__ = &__nvoc_down_thunk_KernelSMDebuggerSession_gpuresInternalControlForward,    // virtual
        .__ksmdbgssnGetInternalObjectHandle__ = &ksmdbgssnGetInternalObjectHandle_IMPL,    // virtual override (gpures) base (gpures)
        .GpuResource.__gpuresGetInternalObjectHandle__ = &__nvoc_down_thunk_KernelSMDebuggerSession_gpuresGetInternalObjectHandle,    // virtual
        .__ksmdbgssnControl__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnControl,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
        .__ksmdbgssnMap__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnMap,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
        .__ksmdbgssnUnmap__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnUnmap,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
        .__ksmdbgssnShareCallback__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnShareCallback,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
        .GpuResource.RmResource.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__ksmdbgssnGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
        .__ksmdbgssnGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_ksmdbgssnGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
        .GpuResource.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
        .__ksmdbgssnAccessCallback__ = &__nvoc_up_thunk_RmResource_ksmdbgssnAccessCallback,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__ksmdbgssnGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_ksmdbgssnGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__ksmdbgssnCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_ksmdbgssnCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__ksmdbgssnGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_ksmdbgssnGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__ksmdbgssnControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__ksmdbgssnControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_ksmdbgssnControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__ksmdbgssnControl_Prologue__ = &__nvoc_up_thunk_RmResource_ksmdbgssnControl_Prologue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__ksmdbgssnControl_Epilogue__ = &__nvoc_up_thunk_RmResource_ksmdbgssnControl_Epilogue,    // virtual inherited (rmres) base (gpures)
        .GpuResource.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .GpuResource.RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .GpuResource.RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__ksmdbgssnCanCopy__ = &__nvoc_up_thunk_RsResource_ksmdbgssnCanCopy,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__ksmdbgssnIsDuplicate__ = &__nvoc_up_thunk_RsResource_ksmdbgssnIsDuplicate,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__ksmdbgssnPreDestruct__ = &__nvoc_up_thunk_RsResource_ksmdbgssnPreDestruct,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__ksmdbgssnControlFilter__ = &__nvoc_up_thunk_RsResource_ksmdbgssnControlFilter,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__ksmdbgssnIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_ksmdbgssnIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
        .GpuResource.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .GpuResource.RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .GpuResource.RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__ksmdbgssnMapTo__ = &__nvoc_up_thunk_RsResource_ksmdbgssnMapTo,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__ksmdbgssnUnmapFrom__ = &__nvoc_up_thunk_RsResource_ksmdbgssnUnmapFrom,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__ksmdbgssnGetRefCount__ = &__nvoc_up_thunk_RsResource_ksmdbgssnGetRefCount,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__ksmdbgssnAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_ksmdbgssnAddAdditionalDependants,    // virtual inherited (res) base (gpures)
        .GpuResource.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .GpuResource.RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .GpuResource.RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
        .__ksmdbgssnGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationListPtr,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
        .__ksmdbgssnGetNotificationShare__ = &__nvoc_up_thunk_Notifier_ksmdbgssnGetNotificationShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
        .__ksmdbgssnSetNotificationShare__ = &__nvoc_up_thunk_Notifier_ksmdbgssnSetNotificationShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
        .__ksmdbgssnUnregisterEvent__ = &__nvoc_up_thunk_Notifier_ksmdbgssnUnregisterEvent,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
        .__ksmdbgssnGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_ksmdbgssnGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
        .Notifier.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
        .Notifier.INotifier.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.GpuResource.RmResource.RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_vtable = &vtable.GpuResource.RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_vtable = &vtable.GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_vtable = &vtable.Notifier.INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_vtable = &vtable.Notifier;    // (notify) super
    pThis->__nvoc_vtable = &vtable;    // (ksmdbgssn) this

    // Initialize vtable(s) with 31 per-object function pointer(s).
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

