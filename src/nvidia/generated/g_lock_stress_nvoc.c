#define NVOC_LOCK_STRESS_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_lock_stress_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xecce10 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_LockStressObject;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_LockStressObject(LockStressObject*);
void __nvoc_init_funcTable_LockStressObject(LockStressObject*);
NV_STATUS __nvoc_ctor_LockStressObject(LockStressObject*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_LockStressObject(LockStressObject*);
void __nvoc_dtor_LockStressObject(LockStressObject*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_LockStressObject;

static const struct NVOC_RTTI __nvoc_rtti_LockStressObject_LockStressObject = {
    /*pClassDef=*/          &__nvoc_class_def_LockStressObject,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_LockStressObject,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_LockStressObject_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_LockStressObject_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_LockStressObject_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_LockStressObject_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_LockStressObject_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(LockStressObject, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_LockStressObject = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_LockStressObject_LockStressObject,
        &__nvoc_rtti_LockStressObject_GpuResource,
        &__nvoc_rtti_LockStressObject_RmResource,
        &__nvoc_rtti_LockStressObject_RmResourceCommon,
        &__nvoc_rtti_LockStressObject_RsResource,
        &__nvoc_rtti_LockStressObject_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_LockStressObject = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(LockStressObject),
        /*classId=*/            classId(LockStressObject),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "LockStressObject",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_LockStressObject,
    /*pCastInfo=*/          &__nvoc_castinfo_LockStressObject,
    /*pExportInfo=*/        &__nvoc_export_info_LockStressObject
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_LockStressObject[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdResetLockStressState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x1000101u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_LockStressObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "lockStressObjCtrlCmdResetLockStressState"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdPerformLockStressAllRmLocks_IMPL,
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
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdPerformLockStressNoGpusLock_IMPL,
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
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdPerformLockStressApiLockReadMode_IMPL,
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
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_IMPL,
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
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_IMPL,
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
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_IMPL,
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
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_IMPL,
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
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_IMPL,
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
        /*pFunc=*/      (void (*)(void)) lockStressObjCtrlCmdGetLockStressCounters_IMPL,
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

};

// 25 up-thunk(s) defined to bridge methods in LockStressObject to superclasses

// lockStressObjControl: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_lockStressObjControl(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_LockStressObject_GpuResource.offset), pCallContext, pParams);
}

// lockStressObjMap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_lockStressObjMap(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_LockStressObject_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

// lockStressObjUnmap: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_lockStressObjUnmap(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_LockStressObject_GpuResource.offset), pCallContext, pCpuMapping);
}

// lockStressObjShareCallback: virtual inherited (gpures) base (gpures)
static NvBool __nvoc_up_thunk_GpuResource_lockStressObjShareCallback(struct LockStressObject *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_LockStressObject_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// lockStressObjGetRegBaseOffsetAndSize: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_lockStressObjGetRegBaseOffsetAndSize(struct LockStressObject *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_LockStressObject_GpuResource.offset), pGpu, pOffset, pSize);
}

// lockStressObjGetMapAddrSpace: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_lockStressObjGetMapAddrSpace(struct LockStressObject *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_LockStressObject_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

// lockStressObjInternalControlForward: virtual inherited (gpures) base (gpures)
static NV_STATUS __nvoc_up_thunk_GpuResource_lockStressObjInternalControlForward(struct LockStressObject *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_LockStressObject_GpuResource.offset), command, pParams, size);
}

// lockStressObjGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
static NvHandle __nvoc_up_thunk_GpuResource_lockStressObjGetInternalObjectHandle(struct LockStressObject *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + __nvoc_rtti_LockStressObject_GpuResource.offset));
}

// lockStressObjAccessCallback: virtual inherited (rmres) base (gpures)
static NvBool __nvoc_up_thunk_RmResource_lockStressObjAccessCallback(struct LockStressObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// lockStressObjGetMemInterMapParams: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_lockStressObjGetMemInterMapParams(struct LockStressObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_LockStressObject_RmResource.offset), pParams);
}

// lockStressObjCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_lockStressObjCheckMemInterUnmap(struct LockStressObject *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_LockStressObject_RmResource.offset), bSubdeviceHandleProvided);
}

// lockStressObjGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_lockStressObjGetMemoryMappingDescriptor(struct LockStressObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_LockStressObject_RmResource.offset), ppMemDesc);
}

// lockStressObjControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_lockStressObjControlSerialization_Prologue(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RmResource.offset), pCallContext, pParams);
}

// lockStressObjControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_lockStressObjControlSerialization_Epilogue(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RmResource.offset), pCallContext, pParams);
}

// lockStressObjControl_Prologue: virtual inherited (rmres) base (gpures)
static NV_STATUS __nvoc_up_thunk_RmResource_lockStressObjControl_Prologue(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RmResource.offset), pCallContext, pParams);
}

// lockStressObjControl_Epilogue: virtual inherited (rmres) base (gpures)
static void __nvoc_up_thunk_RmResource_lockStressObjControl_Epilogue(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RmResource.offset), pCallContext, pParams);
}

// lockStressObjCanCopy: virtual inherited (res) base (gpures)
static NvBool __nvoc_up_thunk_RsResource_lockStressObjCanCopy(struct LockStressObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RsResource.offset));
}

// lockStressObjIsDuplicate: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_lockStressObjIsDuplicate(struct LockStressObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RsResource.offset), hMemory, pDuplicate);
}

// lockStressObjPreDestruct: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_lockStressObjPreDestruct(struct LockStressObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RsResource.offset));
}

// lockStressObjControlFilter: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_lockStressObjControlFilter(struct LockStressObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RsResource.offset), pCallContext, pParams);
}

// lockStressObjIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
static NvBool __nvoc_up_thunk_RsResource_lockStressObjIsPartialUnmapSupported(struct LockStressObject *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RsResource.offset));
}

// lockStressObjMapTo: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_lockStressObjMapTo(struct LockStressObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RsResource.offset), pParams);
}

// lockStressObjUnmapFrom: virtual inherited (res) base (gpures)
static NV_STATUS __nvoc_up_thunk_RsResource_lockStressObjUnmapFrom(struct LockStressObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RsResource.offset), pParams);
}

// lockStressObjGetRefCount: virtual inherited (res) base (gpures)
static NvU32 __nvoc_up_thunk_RsResource_lockStressObjGetRefCount(struct LockStressObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RsResource.offset));
}

// lockStressObjAddAdditionalDependants: virtual inherited (res) base (gpures)
static void __nvoc_up_thunk_RsResource_lockStressObjAddAdditionalDependants(struct RsClient *pClient, struct LockStressObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_LockStressObject_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_LockStressObject = 
{
    /*numEntries=*/     10,
    /*pExportEntries=*/ __nvoc_exported_method_def_LockStressObject
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_LockStressObject(LockStressObject *pThis) {
    __nvoc_lockStressObjDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_LockStressObject(LockStressObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_LockStressObject(LockStressObject *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_LockStressObject_fail_GpuResource;
    __nvoc_init_dataField_LockStressObject(pThis);

    status = __nvoc_lockStressObjConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_LockStressObject_fail__init;
    goto __nvoc_ctor_LockStressObject_exit; // Success

__nvoc_ctor_LockStressObject_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_LockStressObject_fail_GpuResource:
__nvoc_ctor_LockStressObject_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_LockStressObject_1(LockStressObject *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // lockStressObjCtrlCmdResetLockStressState -- exported (id=0x1000101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__lockStressObjCtrlCmdResetLockStressState__ = &lockStressObjCtrlCmdResetLockStressState_IMPL;
#endif

    // lockStressObjCtrlCmdPerformLockStressAllRmLocks -- exported (id=0x1000102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__lockStressObjCtrlCmdPerformLockStressAllRmLocks__ = &lockStressObjCtrlCmdPerformLockStressAllRmLocks_IMPL;
#endif

    // lockStressObjCtrlCmdPerformLockStressNoGpusLock -- exported (id=0x1000103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__lockStressObjCtrlCmdPerformLockStressNoGpusLock__ = &lockStressObjCtrlCmdPerformLockStressNoGpusLock_IMPL;
#endif

    // lockStressObjCtrlCmdPerformLockStressApiLockReadMode -- exported (id=0x1000104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__lockStressObjCtrlCmdPerformLockStressApiLockReadMode__ = &lockStressObjCtrlCmdPerformLockStressApiLockReadMode_IMPL;
#endif

    // lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode -- exported (id=0x1000105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode__ = &lockStressObjCtrlCmdPerformLockStressNoGpusLockApiLockReadMode_IMPL;
#endif

    // lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks -- exported (id=0x1000106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks__ = &lockStressObjCtrlCmdPerformLockStressInternalAllRmLocks_IMPL;
#endif

    // lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock -- exported (id=0x1000107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x9u)
    pThis->__lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock__ = &lockStressObjCtrlCmdPerformLockStressInternalNoGpusLock_IMPL;
#endif

    // lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode -- exported (id=0x1000108)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x108u)
    pThis->__lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode__ = &lockStressObjCtrlCmdPerformLockStressInternalApiLockReadMode_IMPL;
#endif

    // lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode -- exported (id=0x1000109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x109u)
    pThis->__lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode__ = &lockStressObjCtrlCmdPerformLockStressInternalNoGpusLockApiLockReadMode_IMPL;
#endif

    // lockStressObjCtrlCmdGetLockStressCounters -- exported (id=0x100010a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__lockStressObjCtrlCmdGetLockStressCounters__ = &lockStressObjCtrlCmdGetLockStressCounters_IMPL;
#endif

    // lockStressObjControl -- virtual inherited (gpures) base (gpures)
    pThis->__lockStressObjControl__ = &__nvoc_up_thunk_GpuResource_lockStressObjControl;

    // lockStressObjMap -- virtual inherited (gpures) base (gpures)
    pThis->__lockStressObjMap__ = &__nvoc_up_thunk_GpuResource_lockStressObjMap;

    // lockStressObjUnmap -- virtual inherited (gpures) base (gpures)
    pThis->__lockStressObjUnmap__ = &__nvoc_up_thunk_GpuResource_lockStressObjUnmap;

    // lockStressObjShareCallback -- virtual inherited (gpures) base (gpures)
    pThis->__lockStressObjShareCallback__ = &__nvoc_up_thunk_GpuResource_lockStressObjShareCallback;

    // lockStressObjGetRegBaseOffsetAndSize -- virtual inherited (gpures) base (gpures)
    pThis->__lockStressObjGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_lockStressObjGetRegBaseOffsetAndSize;

    // lockStressObjGetMapAddrSpace -- virtual inherited (gpures) base (gpures)
    pThis->__lockStressObjGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_lockStressObjGetMapAddrSpace;

    // lockStressObjInternalControlForward -- virtual inherited (gpures) base (gpures)
    pThis->__lockStressObjInternalControlForward__ = &__nvoc_up_thunk_GpuResource_lockStressObjInternalControlForward;

    // lockStressObjGetInternalObjectHandle -- virtual inherited (gpures) base (gpures)
    pThis->__lockStressObjGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_lockStressObjGetInternalObjectHandle;

    // lockStressObjAccessCallback -- virtual inherited (rmres) base (gpures)
    pThis->__lockStressObjAccessCallback__ = &__nvoc_up_thunk_RmResource_lockStressObjAccessCallback;

    // lockStressObjGetMemInterMapParams -- virtual inherited (rmres) base (gpures)
    pThis->__lockStressObjGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_lockStressObjGetMemInterMapParams;

    // lockStressObjCheckMemInterUnmap -- virtual inherited (rmres) base (gpures)
    pThis->__lockStressObjCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_lockStressObjCheckMemInterUnmap;

    // lockStressObjGetMemoryMappingDescriptor -- virtual inherited (rmres) base (gpures)
    pThis->__lockStressObjGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_lockStressObjGetMemoryMappingDescriptor;

    // lockStressObjControlSerialization_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__lockStressObjControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_lockStressObjControlSerialization_Prologue;

    // lockStressObjControlSerialization_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__lockStressObjControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_lockStressObjControlSerialization_Epilogue;

    // lockStressObjControl_Prologue -- virtual inherited (rmres) base (gpures)
    pThis->__lockStressObjControl_Prologue__ = &__nvoc_up_thunk_RmResource_lockStressObjControl_Prologue;

    // lockStressObjControl_Epilogue -- virtual inherited (rmres) base (gpures)
    pThis->__lockStressObjControl_Epilogue__ = &__nvoc_up_thunk_RmResource_lockStressObjControl_Epilogue;

    // lockStressObjCanCopy -- virtual inherited (res) base (gpures)
    pThis->__lockStressObjCanCopy__ = &__nvoc_up_thunk_RsResource_lockStressObjCanCopy;

    // lockStressObjIsDuplicate -- virtual inherited (res) base (gpures)
    pThis->__lockStressObjIsDuplicate__ = &__nvoc_up_thunk_RsResource_lockStressObjIsDuplicate;

    // lockStressObjPreDestruct -- virtual inherited (res) base (gpures)
    pThis->__lockStressObjPreDestruct__ = &__nvoc_up_thunk_RsResource_lockStressObjPreDestruct;

    // lockStressObjControlFilter -- virtual inherited (res) base (gpures)
    pThis->__lockStressObjControlFilter__ = &__nvoc_up_thunk_RsResource_lockStressObjControlFilter;

    // lockStressObjIsPartialUnmapSupported -- inline virtual inherited (res) base (gpures) body
    pThis->__lockStressObjIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_lockStressObjIsPartialUnmapSupported;

    // lockStressObjMapTo -- virtual inherited (res) base (gpures)
    pThis->__lockStressObjMapTo__ = &__nvoc_up_thunk_RsResource_lockStressObjMapTo;

    // lockStressObjUnmapFrom -- virtual inherited (res) base (gpures)
    pThis->__lockStressObjUnmapFrom__ = &__nvoc_up_thunk_RsResource_lockStressObjUnmapFrom;

    // lockStressObjGetRefCount -- virtual inherited (res) base (gpures)
    pThis->__lockStressObjGetRefCount__ = &__nvoc_up_thunk_RsResource_lockStressObjGetRefCount;

    // lockStressObjAddAdditionalDependants -- virtual inherited (res) base (gpures)
    pThis->__lockStressObjAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_lockStressObjAddAdditionalDependants;
} // End __nvoc_init_funcTable_LockStressObject_1 with approximately 35 basic block(s).


// Initialize vtable(s) for 35 virtual method(s).
void __nvoc_init_funcTable_LockStressObject(LockStressObject *pThis) {

    // Initialize vtable(s) with 35 per-object function pointer(s).
    __nvoc_init_funcTable_LockStressObject_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_LockStressObject(LockStressObject *pThis) {
    pThis->__nvoc_pbase_LockStressObject = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_LockStressObject(pThis);
}

NV_STATUS __nvoc_objCreate_LockStressObject(LockStressObject **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    LockStressObject *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(LockStressObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(LockStressObject));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_LockStressObject);

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_LockStressObject(pThis);
    status = __nvoc_ctor_LockStressObject(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_LockStressObject_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_LockStressObject_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(LockStressObject));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_LockStressObject(LockStressObject **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_LockStressObject(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

