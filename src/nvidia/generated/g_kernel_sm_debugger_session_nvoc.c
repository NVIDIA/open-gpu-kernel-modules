#define NVOC_KERNEL_SM_DEBUGGER_SESSION_H_PRIVATE_ACCESS_ALLOWED

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_kernel_sm_debugger_session_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__48fa7d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmDebuggerSession;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsSession;

// Forward declarations for RmDebuggerSession
void __nvoc_init__RsSession(RsSession*);
void __nvoc_init__RmDebuggerSession(RmDebuggerSession*);
void __nvoc_init_funcTable_RmDebuggerSession(RmDebuggerSession*);
NV_STATUS __nvoc_ctor_RmDebuggerSession(RmDebuggerSession*);
void __nvoc_init_dataField_RmDebuggerSession(RmDebuggerSession*);
void __nvoc_dtor_RmDebuggerSession(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__RmDebuggerSession;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__RmDebuggerSession;

// Down-thunk(s) to bridge RmDebuggerSession methods from ancestors (if any)
void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependant(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef);    // this
void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependency(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef);    // this

// Up-thunk(s) to bridge RmDebuggerSession methods to ancestors (if any)

// Class-specific details for RmDebuggerSession
const struct NVOC_CLASS_DEF __nvoc_class_def_RmDebuggerSession = 
{
    .classInfo.size =               sizeof(RmDebuggerSession),
    .classInfo.classId =            classId(RmDebuggerSession),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "RmDebuggerSession",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_RmDebuggerSession,
    .pCastInfo =          &__nvoc_castinfo__RmDebuggerSession,
    .pExportInfo =        &__nvoc_export_info__RmDebuggerSession
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__RmDebuggerSession __nvoc_metadata__RmDebuggerSession = {
    .rtti.pClassDef = &__nvoc_class_def_RmDebuggerSession,    // (dbgSession) this
    .rtti.dtor      = &__nvoc_dtor_RmDebuggerSession,
    .rtti.offset    = 0,
    .metadata__RsSession.rtti.pClassDef = &__nvoc_class_def_RsSession,    // (session) super
    .metadata__RsSession.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsSession.rtti.offset    = NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession),
    .metadata__RsSession.metadata__RsShared.rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) super^2
    .metadata__RsSession.metadata__RsShared.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsSession.metadata__RsShared.rtti.offset    = NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession.__nvoc_base_RsShared),
    .metadata__RsSession.metadata__RsShared.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RsSession.metadata__RsShared.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsSession.metadata__RsShared.metadata__Object.rtti.offset    = NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object),

    .metadata__RsSession.vtable.__sessionRemoveDependant__ = &__nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependant,    // virtual
    .metadata__RsSession.vtable.__sessionRemoveDependency__ = &__nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependency,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__RmDebuggerSession = {
    .numRelatives = 4,
    .relatives = {
        &__nvoc_metadata__RmDebuggerSession.rtti,    // [0]: (dbgSession) this
        &__nvoc_metadata__RmDebuggerSession.metadata__RsSession.rtti,    // [1]: (session) super
        &__nvoc_metadata__RmDebuggerSession.metadata__RsSession.metadata__RsShared.rtti,    // [2]: (shr) super^2
        &__nvoc_metadata__RmDebuggerSession.metadata__RsSession.metadata__RsShared.metadata__Object.rtti,    // [3]: (obj) super^3
    }
};

// 2 down-thunk(s) defined to bridge methods in RmDebuggerSession from superclasses

// dbgSessionRemoveDependant: virtual override (session) base (session)
void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependant(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    dbgSessionRemoveDependant_IMPL((struct RmDebuggerSession *)(((unsigned char *) pDbgSession) - NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession)), pResourceRef);
}

// dbgSessionRemoveDependency: virtual override (session) base (session)
void __nvoc_down_thunk_RmDebuggerSession_sessionRemoveDependency(struct RsSession *pDbgSession, struct RsResourceRef *pResourceRef) {
    dbgSessionRemoveDependency_IMPL((struct RmDebuggerSession *)(((unsigned char *) pDbgSession) - NV_OFFSETOF(RmDebuggerSession, __nvoc_base_RsSession)), pResourceRef);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__RmDebuggerSession = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct RmDebuggerSession object.
void __nvoc_dtor_RsSession(Dynamic*);
void __nvoc_dtor_RmDebuggerSession(Dynamic* pThis) {

    RmDebuggerSession *__nvoc_this = (RmDebuggerSession *) pThis;

// Recurse to superclass destructors.
    __nvoc_dtor_RsSession((Dynamic *) &__nvoc_this->__nvoc_base_RsSession);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_RmDebuggerSession(RmDebuggerSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct RmDebuggerSession object.
NV_STATUS __nvoc_ctor_RsSession(RsSession *pSession);
NV_STATUS __nvoc_ctor_RmDebuggerSession(RmDebuggerSession *pThis) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RsSession(&pThis->__nvoc_base_RsSession);
    if (status != NV_OK) goto __nvoc_ctor_RmDebuggerSession_fail_RsSession;

    // Initialize data fields.
    __nvoc_init_dataField_RmDebuggerSession(pThis);
    goto __nvoc_ctor_RmDebuggerSession_exit; // Success

    // Unwind on error.
__nvoc_ctor_RmDebuggerSession_fail_RsSession:
__nvoc_ctor_RmDebuggerSession_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_RmDebuggerSession_1(RmDebuggerSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_RmDebuggerSession_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_RmDebuggerSession(RmDebuggerSession *pThis) {
    __nvoc_init_funcTable_RmDebuggerSession_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__RmDebuggerSession(RmDebuggerSession *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsSession.__nvoc_base_RsShared;    // (shr) super^2
    pThis->__nvoc_pbase_RsSession = &pThis->__nvoc_base_RsSession;    // (session) super
    pThis->__nvoc_pbase_RmDebuggerSession = pThis;    // (dbgSession) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsSession(&pThis->__nvoc_base_RsSession);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__RmDebuggerSession.metadata__RsSession.metadata__RsShared.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_metadata_ptr = &__nvoc_metadata__RmDebuggerSession.metadata__RsSession.metadata__RsShared;    // (shr) super^2
    pThis->__nvoc_base_RsSession.__nvoc_metadata_ptr = &__nvoc_metadata__RmDebuggerSession.metadata__RsSession;    // (session) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__RmDebuggerSession;    // (dbgSession) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_RmDebuggerSession(pThis);
}

NV_STATUS __nvoc_objCreate_RmDebuggerSession(RmDebuggerSession **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    RmDebuggerSession *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(RmDebuggerSession));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(RmDebuggerSession));

    __nvoc_pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__RmDebuggerSession(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_RmDebuggerSession(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_RmDebuggerSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_RmDebuggerSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsSession.__nvoc_base_RsShared.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(RmDebuggerSession));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_RmDebuggerSession(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {

    return __nvoc_objCreate_RmDebuggerSession((RmDebuggerSession **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__4adc81 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSMDebuggerSession;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for KernelSMDebuggerSession
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__KernelSMDebuggerSession(KernelSMDebuggerSession*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelSMDebuggerSession(KernelSMDebuggerSession*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_KernelSMDebuggerSession(KernelSMDebuggerSession*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_KernelSMDebuggerSession(KernelSMDebuggerSession*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_KernelSMDebuggerSession(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelSMDebuggerSession;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelSMDebuggerSession;

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
NvBool __nvoc_down_thunk_GpuResource_resShareCallback(struct RsResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_down_thunk_KernelSMDebuggerSession_gpuresInternalControlForward(struct GpuResource *arg_this, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_down_thunk_KernelSMDebuggerSession_gpuresGetInternalObjectHandle(struct GpuResource *arg_this);    // this

// Up-thunk(s) to bridge KernelSMDebuggerSession methods to ancestors (if any)

// Class-specific details for KernelSMDebuggerSession
const struct NVOC_CLASS_DEF __nvoc_class_def_KernelSMDebuggerSession = 
{
    .classInfo.size =               sizeof(KernelSMDebuggerSession),
    .classInfo.classId =            classId(KernelSMDebuggerSession),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "KernelSMDebuggerSession",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_KernelSMDebuggerSession,
    .pCastInfo =          &__nvoc_castinfo__KernelSMDebuggerSession,
    .pExportInfo =        &__nvoc_export_info__KernelSMDebuggerSession
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdSMDebugModeEnable__EXPORT(void *pKernelSMDebuggerSession) {
    return ksmdbgssnCtrlCmdSMDebugModeEnable_fcf1ac(pKernelSMDebuggerSession);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdSMDebugModeDisable__EXPORT(void *pKernelSMDebuggerSession) {
    return ksmdbgssnCtrlCmdSMDebugModeDisable_fcf1ac(pKernelSMDebuggerSession);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetModeMMUDebug__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetModeMMUDebug_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugGetModeMMUDebug__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugGetModeMMUDebug_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetModeErrbarDebug__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetModeErrbarDebug_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugGetModeErrbarDebug__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugGetModeErrbarDebug_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetExceptionMask__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetExceptionMask_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetNonPreemptableDebuggerContext__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetNonPreemptableDebuggerContext_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetRecoverySuppressionTimeout__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetRecoverySuppressionTimeout_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugReadSingleSmErrorState__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugReadSingleSmErrorState_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugReadAllSmErrorStates__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugReadAllSmErrorStates_IMPL(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugClearSingleSmErrorState__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugClearSingleSmErrorState_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugClearAllSmErrorStates__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugClearAllSmErrorStates_IMPL(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00050048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSuspendContext__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSuspendContext_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugResumeContext__EXPORT(void *pKernelSMDebuggerSession) {
    return ksmdbgssnCtrlCmdDebugResumeContext_fcf1ac(pKernelSMDebuggerSession);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS ksmdbgssnCtrlCmdReadSurface__EXPORT(void *arg_this, void *arg2) {
    return ksmdbgssnCtrlCmdReadSurface_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS ksmdbgssnCtrlCmdWriteSurface__EXPORT(void *arg_this, void *arg2) {
    return ksmdbgssnCtrlCmdWriteSurface_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS ksmdbgssnCtrlCmdGetMappings__EXPORT(void *arg_this, void *arg2) {
    return ksmdbgssnCtrlCmdGetMappings_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetNextStopTriggerType__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetNextStopTriggerType_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS ksmdbgssnCtrlCmdDebugReadMemory__EXPORT(void *arg_this, void *arg2) {
    return ksmdbgssnCtrlCmdDebugReadMemory_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS ksmdbgssnCtrlCmdDebugWriteMemory__EXPORT(void *arg_this, void *arg2) {
    return ksmdbgssnCtrlCmdDebugWriteMemory_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010208u)
static NV_STATUS ksmdbgssnCtrlCmdDebugExecRegOps__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugExecRegOps_IMPL(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010208u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00010248u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS ksmdbgssnCtrlCmdDebugReadBatchMemory__EXPORT(void *arg_this, void *arg2) {
    return ksmdbgssnCtrlCmdDebugReadBatchMemory_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS ksmdbgssnCtrlCmdDebugWriteBatchMemory__EXPORT(void *arg_this, void *arg2) {
    return ksmdbgssnCtrlCmdDebugWriteBatchMemory_IMPL(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__EXPORT(void *arg_this, void *arg2) {
    return ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_DISPATCH(arg_this, arg2);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)
static NV_STATUS ksmdbgssnCtrlCmdDebugSetDropDeferredRC__EXPORT(void *pKernelSMDebuggerSession, void *pParams) {
    return ksmdbgssnCtrlCmdDebugSetDropDeferredRC_fcf1ac(pKernelSMDebuggerSession, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000048u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_KernelSMDebuggerSession[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetNonPreemptableDebuggerContext__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de022du,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_NON_PREEMPTABLE_DEBUGGER_CONTEXT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetNonPreemptableDebuggerContext"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetRecoverySuppressionTimeout__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de022eu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_RECOVERY_SUPPRESSION_TIMEOUT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetRecoverySuppressionTimeout"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdSMDebugModeEnable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0301u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdSMDebugModeEnable"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdSMDebugModeDisable__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0302u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdSMDebugModeDisable"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetModeMMUDebug__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0307u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_MODE_MMU_DEBUG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetModeMMUDebug"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugGetModeMMUDebug__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0308u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_GET_MODE_MMU_DEBUG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugGetModeMMUDebug"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetExceptionMask__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0309u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_EXCEPTION_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetExceptionMask"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugReadSingleSmErrorState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de030bu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_READ_SINGLE_SM_ERROR_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugReadSingleSmErrorState"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugReadAllSmErrorStates__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de030cu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_READ_ALL_SM_ERROR_STATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugReadAllSmErrorStates"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugClearSingleSmErrorState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de030fu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_CLEAR_SINGLE_SM_ERROR_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugClearSingleSmErrorState"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugClearAllSmErrorStates__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x50048u)
        /*flags=*/      0x50048u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0310u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_CLEAR_ALL_SM_ERROR_STATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugClearAllSmErrorStates"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetNextStopTriggerType__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0313u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_NEXT_STOP_TRIGGER_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetNextStopTriggerType"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetSingleStepInterruptHandling__EXPORT,
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
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugReadMemory__EXPORT,
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
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugWriteMemory__EXPORT,
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
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSuspendContext__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0317u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_CMD_DEBUG_SUSPEND_CONTEXT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSuspendContext"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugResumeContext__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0318u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugResumeContext"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdReadSurface__EXPORT,
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
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdWriteSurface__EXPORT,
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
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdGetMappings__EXPORT,
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
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10208u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugExecRegOps__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10208u)
        /*flags=*/      0x10208u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de031du,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_EXEC_REG_OPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugExecRegOps"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetModeErrbarDebug__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de031fu,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_MODE_ERRBAR_DEBUG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetModeErrbarDebug"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugGetModeErrbarDebug__EXPORT,
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
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10248u)
        /*flags=*/      0x10248u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0321u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_SINGLE_SM_SINGLE_STEP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetSingleSmSingleStep"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetSingleSmStopTrigger__EXPORT,
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
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetSingleSmRunTrigger__EXPORT,
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
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetSingleSmSkipIdleWarpDetect__EXPORT,
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
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugGetSingleSmDebuggerStatus__EXPORT,
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
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugReadBatchMemory__EXPORT,
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
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugWriteBatchMemory__EXPORT,
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
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__EXPORT,
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
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetDropDeferredRC__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x83de0329u,
        /*paramSize=*/  sizeof(NV83DE_CTRL_DEBUG_SET_DROP_DEFERRED_RC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_KernelSMDebuggerSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "ksmdbgssnCtrlCmdDebugSetDropDeferredRC"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugSetModeMMUGccDebug__EXPORT,
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
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x248u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &ksmdbgssnCtrlCmdDebugGetModeMMUGccDebug__EXPORT,
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


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__KernelSMDebuggerSession __nvoc_metadata__KernelSMDebuggerSession = {
    .rtti.pClassDef = &__nvoc_class_def_KernelSMDebuggerSession,    // (ksmdbgssn) this
    .rtti.dtor      = &__nvoc_dtor_KernelSMDebuggerSession,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &__nvoc_down_thunk_KernelSMDebuggerSession_gpuresInternalControlForward,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &__nvoc_down_thunk_KernelSMDebuggerSession_gpuresGetInternalObjectHandle,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelSMDebuggerSession = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__KernelSMDebuggerSession.rtti,    // [0]: (ksmdbgssn) this
        &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__KernelSMDebuggerSession.metadata__Notifier.rtti,    // [6]: (notify) super
        &__nvoc_metadata__KernelSMDebuggerSession.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^2
    }
};

// 2 down-thunk(s) defined to bridge methods in KernelSMDebuggerSession from superclasses

// ksmdbgssnInternalControlForward: virtual override (gpures) base (gpures)
NV_STATUS __nvoc_down_thunk_KernelSMDebuggerSession_gpuresInternalControlForward(struct GpuResource *arg_this, NvU32 command, void *pParams, NvU32 size) {
    return ksmdbgssnInternalControlForward_IMPL((struct KernelSMDebuggerSession *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)), command, pParams, size);
}

// ksmdbgssnGetInternalObjectHandle: virtual override (gpures) base (gpures)
NvHandle __nvoc_down_thunk_KernelSMDebuggerSession_gpuresGetInternalObjectHandle(struct GpuResource *arg_this) {
    return ksmdbgssnGetInternalObjectHandle_IMPL((struct KernelSMDebuggerSession *)(((unsigned char *) arg_this) - NV_OFFSETOF(KernelSMDebuggerSession, __nvoc_base_GpuResource)));
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelSMDebuggerSession = 
{
    .numEntries=     34,
    .pExportEntries= __nvoc_exported_method_def_KernelSMDebuggerSession
};


// Destruct KernelSMDebuggerSession object.
void __nvoc_ksmdbgssnDestruct(KernelSMDebuggerSession*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_Notifier(Dynamic*);
void __nvoc_dtor_KernelSMDebuggerSession(Dynamic* pThis) {

    KernelSMDebuggerSession *__nvoc_this = (KernelSMDebuggerSession *) pThis;

// Call destructor.
    __nvoc_ksmdbgssnDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier((Dynamic *) &__nvoc_this->__nvoc_base_Notifier);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_KernelSMDebuggerSession(KernelSMDebuggerSession *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}


// Construct KernelSMDebuggerSession object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_Notifier(Notifier *pNotifier, struct CALL_CONTEXT *pCallContext);    // inline
NV_STATUS __nvoc_ctor_KernelSMDebuggerSession(KernelSMDebuggerSession *pKernelSMDebuggerSession, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pKernelSMDebuggerSession->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelSMDebuggerSession_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pKernelSMDebuggerSession->__nvoc_base_Notifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_KernelSMDebuggerSession_fail_Notifier;

    // Initialize data fields.
    __nvoc_init_dataField_KernelSMDebuggerSession(pKernelSMDebuggerSession, pRmhalspecowner);

    // Call the constructor for this class.
    status = __nvoc_ksmdbgssnConstruct(pKernelSMDebuggerSession, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelSMDebuggerSession_fail__init;
    goto __nvoc_ctor_KernelSMDebuggerSession_exit; // Success

    // Unwind on error.
__nvoc_ctor_KernelSMDebuggerSession_fail__init:
    __nvoc_dtor_Notifier((Dynamic *)&pKernelSMDebuggerSession->__nvoc_base_Notifier);
__nvoc_ctor_KernelSMDebuggerSession_fail_Notifier:
    __nvoc_dtor_GpuResource((Dynamic *)&pKernelSMDebuggerSession->__nvoc_base_GpuResource);
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

    // ksmdbgssnCtrlCmdDebugReadMMUFaultInfo -- halified (2 hals) exported (id=0x83de0328) body
    if (( ((rmVariantHal_HalVarIdx >> 5) == 0UL) && ((1UL << (rmVariantHal_HalVarIdx & 0x1f)) & 0x00000001UL) )) /* RmVariantHal: VF */ 
    {
        pThis->__ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__ = &ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_395e98;
    }
    else
    {
        pThis->__ksmdbgssnCtrlCmdDebugReadMMUFaultInfo__ = &ksmdbgssnCtrlCmdDebugReadMMUFaultInfo_IMPL;
    }
} // End __nvoc_init_funcTable_KernelSMDebuggerSession_1 with approximately 2 basic block(s).


// Initialize vtable(s) for 1 virtual method(s).
void __nvoc_init_funcTable_KernelSMDebuggerSession(KernelSMDebuggerSession *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 1 per-object function pointer(s).
    __nvoc_init_funcTable_KernelSMDebuggerSession_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__KernelSMDebuggerSession(KernelSMDebuggerSession *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_KernelSMDebuggerSession = pThis;    // (ksmdbgssn) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelSMDebuggerSession.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__KernelSMDebuggerSession.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__KernelSMDebuggerSession.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelSMDebuggerSession;    // (ksmdbgssn) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelSMDebuggerSession(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_KernelSMDebuggerSession(KernelSMDebuggerSession **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    KernelSMDebuggerSession *__nvoc_pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(KernelSMDebuggerSession));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(KernelSMDebuggerSession));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, __nvoc_pParent != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelSMDebuggerSession_cleanup);

    // Link the child into the parent unless flagged not to do so.
    if (!(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // HALs are defined by the parent or the first super class.
    if ((pRmhalspecowner = dynamicCast(__nvoc_pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, __nvoc_pParent);
    NV_CHECK_TRUE_OR_GOTO(__nvoc_status, LEVEL_ERROR, pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT, __nvoc_objCreate_KernelSMDebuggerSession_cleanup);

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__KernelSMDebuggerSession(__nvoc_pThis, pRmhalspecowner);
    __nvoc_status = __nvoc_ctor_KernelSMDebuggerSession(__nvoc_pThis, pRmhalspecowner, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_KernelSMDebuggerSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_KernelSMDebuggerSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(KernelSMDebuggerSession));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelSMDebuggerSession(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_KernelSMDebuggerSession((KernelSMDebuggerSession **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

