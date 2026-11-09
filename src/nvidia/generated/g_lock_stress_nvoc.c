#define NVOC_LOCK_STRESS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_lock_stress_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__ecce10 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LockStressObject;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

// Forward declarations for LockStressObject
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__LockStressObject(LockStressObject*);
void __nvoc_init_funcTable_LockStressObject(LockStressObject*);
NV_STATUS __nvoc_ctor_LockStressObject(LockStressObject*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_LockStressObject(LockStressObject*);
void __nvoc_dtor_LockStressObject(Dynamic*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__LockStressObject;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__LockStressObject;

// Down-thunk(s) to bridge LockStressObject methods from ancestors (if any)
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

// Up-thunk(s) to bridge LockStressObject methods to ancestors (if any)

// Class-specific details for LockStressObject
const struct NVOC_CLASS_DEF __nvoc_class_def_LockStressObject = 
{
    .classInfo.size =               sizeof(LockStressObject),
    .classInfo.classId =            classId(LockStressObject),
    .classInfo.visibility =         NVOC_EVENT_VISIBILITY_NONEVENT,
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "LockStressObject",
#endif
    .objCreatefn =        &__nvoc_objCreateDynamic_LockStressObject,
    .pCastInfo =          &__nvoc_castinfo__LockStressObject,
    .pExportInfo =        &__nvoc_export_info__LockStressObject
};

// By default, all exported methods are enabled.
#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

// Exported trampoline function definitions
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS lockStressObjCtrlCmdResetLockStressState__EXPORT(void *pResource) {
    return lockStressObjCtrlCmdResetLockStressState_IMPL(pResource);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS lockStressObjCtrlCmdPerformLockStressAllRmLocks__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdPerformLockStressAllRmLocks_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLock__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdPerformLockStressNoGpusLock_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)
static NV_STATUS lockStressObjCtrlCmdPerformLockStressApiLockReadMode__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdPerformLockStressApiLockReadMode_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)
static NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)
static NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000108u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)
static NV_STATUS lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000109u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)
static NV_STATUS lockStressObjCtrlCmdGetLockStressCounters__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdGetLockStressCounters_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00000008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100009u)
static NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestNoLock__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdRecursiveGpuLockTestNoLock_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100009u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100008u)
static NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestAllLock__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdRecursiveGpuLockTestAllLock_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100008u)

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100018u)
static NV_STATUS lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock__EXPORT(void *pResource, void *pParams) {
    return lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock_IMPL(pResource, pParams);
}
#endif // !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x00100018u)

// Exported method array
static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_LockStressObject[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdResetLockStressState__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000101u,
        /*paramSize=*/  0 /* Singleton parameter list */,
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdResetLockStressState"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdPerformLockStressAllRmLocks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000102u,
        /*paramSize=*/  sizeof(NV0100_CTRL_PERFORM_LOCK_STRESS_ALL_RM_LOCKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdPerformLockStressAllRmLocks"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdPerformLockStressNoGpusLock__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000103u,
        /*paramSize=*/  sizeof(NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdPerformLockStressNoGpusLock"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdPerformLockStressApiLockReadMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000104u,
        /*paramSize=*/  sizeof(NV0100_CTRL_PERFORM_LOCK_STRESS_API_LOCK_READ_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdPerformLockStressApiLockReadMode"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000105u,
        /*paramSize=*/  sizeof(NV0100_CTRL_PERFORM_LOCK_STRESS_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000106u,
        /*paramSize=*/  sizeof(NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_ALL_RM_LOCKS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
        /*flags=*/      0x9u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000107u,
        /*paramSize=*/  sizeof(NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
        /*flags=*/      0x108u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000108u,
        /*paramSize=*/  sizeof(NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_API_LOCK_READ_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
        /*flags=*/      0x109u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000109u,
        /*paramSize=*/  sizeof(NV0100_CTRL_PERFORM_LOCK_STRESS_INTERNAL_NO_GPUS_LOCK_API_LOCK_READ_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdGetLockStressCounters__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x100010au,
        /*paramSize=*/  sizeof(NV0100_CTRL_GET_LOCK_STRESS_COUNTERS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdGetLockStressCounters"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100009u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdRecursiveGpuLockTestNoLock__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100009u)
        /*flags=*/      0x100009u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x100010bu,
        /*paramSize=*/  sizeof(NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdRecursiveGpuLockTestNoLock"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100008u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdRecursiveGpuLockTestAllLock__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100008u)
        /*flags=*/      0x100008u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x100010cu,
        /*paramSize=*/  sizeof(NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdRecursiveGpuLockTestAllLock"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100018u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) &lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock__EXPORT,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100018u)
        /*flags=*/      0x100018u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x100010du,
        /*paramSize=*/  sizeof(NV0100_CTRL_CMD_RECURSIVE_GPU_LOCK_TEST_PARAM),
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdRecursiveGpuLockTestDeviceLock"
#endif
    },
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__LockStressObject __nvoc_metadata__LockStressObject = {
    .rtti.pClassDef = &__nvoc_class_def_LockStressObject,    // (lockStressObj) this
    .rtti.dtor      = &__nvoc_dtor_LockStressObject,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_GpuResource_resShareCallback,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
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
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__LockStressObject = {
    .numRelatives = 6,
    .relatives = {
        &__nvoc_metadata__LockStressObject.rtti,    // [0]: (lockStressObj) this
        &__nvoc_metadata__LockStressObject.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__LockStressObject.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__LockStressObject.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__LockStressObject.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__LockStressObject.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__LockStressObject = 
{
    .numEntries=     13,
    .pExportEntries= __nvoc_exported_method_def_LockStressObject
};


// Destruct LockStressObject object.
void __nvoc_lockStressObjDestruct(LockStressObject*);
void __nvoc_dtor_GpuResource(Dynamic*);
void __nvoc_dtor_LockStressObject(Dynamic* pThis) {

    LockStressObject *__nvoc_this = (LockStressObject *) pThis;

// Call destructor.
    __nvoc_lockStressObjDestruct(__nvoc_this);

// Recurse to superclass destructors.
    __nvoc_dtor_GpuResource((Dynamic *) &__nvoc_this->__nvoc_base_GpuResource);

    PORT_UNREFERENCED_VARIABLE(__nvoc_this);
}
void __nvoc_init_dataField_LockStressObject(LockStressObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct LockStressObject object.
NV_STATUS __nvoc_ctor_GpuResource(GpuResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_LockStressObject(LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_GpuResource(&pResource->__nvoc_base_GpuResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_LockStressObject_fail_GpuResource;

    // Initialize data fields.
    __nvoc_init_dataField_LockStressObject(pResource);

    // Call the constructor for this class.
    status = __nvoc_lockStressObjConstruct(pResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_LockStressObject_fail__init;
    goto __nvoc_ctor_LockStressObject_exit; // Success

    // Unwind on error.
__nvoc_ctor_LockStressObject_fail__init:
    __nvoc_dtor_GpuResource((Dynamic *)&pResource->__nvoc_base_GpuResource);
__nvoc_ctor_LockStressObject_fail_GpuResource:
__nvoc_ctor_LockStressObject_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LockStressObject_1(LockStressObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_LockStressObject_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_LockStressObject(LockStressObject *pThis) {
    __nvoc_init_funcTable_LockStressObject_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__LockStressObject(LockStressObject *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_LockStressObject = pThis;    // (lockStressObj) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__LockStressObject.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__LockStressObject.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__LockStressObject.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__LockStressObject.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__LockStressObject.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__LockStressObject;    // (lockStressObj) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_LockStressObject(pThis);
}

NV_STATUS __nvoc_objCreate_LockStressObject(LockStressObject **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    LockStressObject *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(LockStressObject));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(LockStressObject));

    __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__LockStressObject(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_LockStressObject(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_LockStressObject_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_LockStressObject_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(LockStressObject));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_LockStressObject(Dynamic **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    return __nvoc_objCreate_LockStressObject((LockStressObject **) __nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);
}

