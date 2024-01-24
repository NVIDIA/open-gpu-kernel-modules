#define NVOC_NVENCSESSION_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_nvencsession_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x3434af = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvencSession;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;

void __nvoc_init_NvencSession(NvencSession*);
void __nvoc_init_funcTable_NvencSession(NvencSession*);
NV_STATUS __nvoc_ctor_NvencSession(NvencSession*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_NvencSession(NvencSession*);
void __nvoc_dtor_NvencSession(NvencSession*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NvencSession;

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_NvencSession = {
    /*pClassDef=*/          &__nvoc_class_def_NvencSession,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NvencSession,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource.__nvoc_base_RmResource),
};

static const struct NVOC_RTTI __nvoc_rtti_NvencSession_GpuResource = {
    /*pClassDef=*/          &__nvoc_class_def_GpuResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NvencSession, __nvoc_base_GpuResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NvencSession = {
    /*numRelatives=*/       6,
    /*relatives=*/ {
        &__nvoc_rtti_NvencSession_NvencSession,
        &__nvoc_rtti_NvencSession_GpuResource,
        &__nvoc_rtti_NvencSession_RmResource,
        &__nvoc_rtti_NvencSession_RmResourceCommon,
        &__nvoc_rtti_NvencSession_RsResource,
        &__nvoc_rtti_NvencSession_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NvencSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NvencSession),
        /*classId=*/            classId(NvencSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NvencSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NvencSession,
    /*pCastInfo=*/          &__nvoc_castinfo_NvencSession,
    /*pExportInfo=*/        &__nvoc_export_info_NvencSession
};

static NvBool __nvoc_thunk_GpuResource_nvencsessionShareCallback(struct NvencSession *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_nvencsessionCheckMemInterUnmap(struct NvencSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_NvencSession_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_nvencsessionMapTo(struct NvencSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_nvencsessionGetMapAddrSpace(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pCallContext, mapFlags, pAddrSpace);
}

static NvU32 __nvoc_thunk_RsResource_nvencsessionGetRefCount(struct NvencSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RsResource.offset));
}

static void __nvoc_thunk_RsResource_nvencsessionAddAdditionalDependants(struct RsClient *pClient, struct NvencSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RmResource_nvencsessionControl_Prologue(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_nvencsessionGetRegBaseOffsetAndSize(struct NvencSession *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pGpu, pOffset, pSize);
}

static NV_STATUS __nvoc_thunk_GpuResource_nvencsessionInternalControlForward(struct NvencSession *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), command, pParams, size);
}

static NV_STATUS __nvoc_thunk_RsResource_nvencsessionUnmapFrom(struct NvencSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_nvencsessionControl_Epilogue(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pCallContext, pParams);
}

static NvHandle __nvoc_thunk_GpuResource_nvencsessionGetInternalObjectHandle(struct NvencSession *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset));
}

static NV_STATUS __nvoc_thunk_GpuResource_nvencsessionControl(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_nvencsessionUnmap(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_nvencsessionGetMemInterMapParams(struct NvencSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_NvencSession_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_nvencsessionGetMemoryMappingDescriptor(struct NvencSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_NvencSession_RmResource.offset), ppMemDesc);
}

static NV_STATUS __nvoc_thunk_RsResource_nvencsessionControlFilter(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_nvencsessionControlSerialization_Prologue(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_nvencsessionCanCopy(struct NvencSession *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RsResource.offset));
}

static NvBool __nvoc_thunk_RsResource_nvencsessionIsPartialUnmapSupported(struct NvencSession *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RsResource.offset));
}

static void __nvoc_thunk_RsResource_nvencsessionPreDestruct(struct NvencSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_nvencsessionIsDuplicate(struct NvencSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_nvencsessionControlSerialization_Epilogue(struct NvencSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_GpuResource_nvencsessionMap(struct NvencSession *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *)pGpuResource) + __nvoc_rtti_NvencSession_GpuResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_nvencsessionAccessCallback(struct NvencSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_NvencSession_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_NvencSession[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvencsessionCtrlCmdNvencSwSessionUpdateInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0bc0101u,
        /*paramSize=*/  sizeof(NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvencSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvencsessionCtrlCmdNvencSwSessionUpdateInfo"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xa0bc0102u,
        /*paramSize=*/  sizeof(NVA0BC_CTRL_NVENC_SW_SESSION_UPDATE_INFO_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvencSession.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_NvencSession = 
{
    /*numEntries=*/     2,
    /*pExportEntries=*/ __nvoc_exported_method_def_NvencSession
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_NvencSession(NvencSession *pThis) {
    __nvoc_nvencsessionDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NvencSession(NvencSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_NvencSession(NvencSession *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvencSession_fail_GpuResource;
    __nvoc_init_dataField_NvencSession(pThis);

    status = __nvoc_nvencsessionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvencSession_fail__init;
    goto __nvoc_ctor_NvencSession_exit; // Success

__nvoc_ctor_NvencSession_fail__init:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_NvencSession_fail_GpuResource:
__nvoc_ctor_NvencSession_exit:

    return status;
}

static void __nvoc_init_funcTable_NvencSession_1(NvencSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__nvencsessionCtrlCmdNvencSwSessionUpdateInfo__ = &nvencsessionCtrlCmdNvencSwSessionUpdateInfo_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2__ = &nvencsessionCtrlCmdNvencSwSessionUpdateInfoV2_IMPL;
#endif

    pThis->__nvencsessionShareCallback__ = &__nvoc_thunk_GpuResource_nvencsessionShareCallback;

    pThis->__nvencsessionCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_nvencsessionCheckMemInterUnmap;

    pThis->__nvencsessionMapTo__ = &__nvoc_thunk_RsResource_nvencsessionMapTo;

    pThis->__nvencsessionGetMapAddrSpace__ = &__nvoc_thunk_GpuResource_nvencsessionGetMapAddrSpace;

    pThis->__nvencsessionGetRefCount__ = &__nvoc_thunk_RsResource_nvencsessionGetRefCount;

    pThis->__nvencsessionAddAdditionalDependants__ = &__nvoc_thunk_RsResource_nvencsessionAddAdditionalDependants;

    pThis->__nvencsessionControl_Prologue__ = &__nvoc_thunk_RmResource_nvencsessionControl_Prologue;

    pThis->__nvencsessionGetRegBaseOffsetAndSize__ = &__nvoc_thunk_GpuResource_nvencsessionGetRegBaseOffsetAndSize;

    pThis->__nvencsessionInternalControlForward__ = &__nvoc_thunk_GpuResource_nvencsessionInternalControlForward;

    pThis->__nvencsessionUnmapFrom__ = &__nvoc_thunk_RsResource_nvencsessionUnmapFrom;

    pThis->__nvencsessionControl_Epilogue__ = &__nvoc_thunk_RmResource_nvencsessionControl_Epilogue;

    pThis->__nvencsessionGetInternalObjectHandle__ = &__nvoc_thunk_GpuResource_nvencsessionGetInternalObjectHandle;

    pThis->__nvencsessionControl__ = &__nvoc_thunk_GpuResource_nvencsessionControl;

    pThis->__nvencsessionUnmap__ = &__nvoc_thunk_GpuResource_nvencsessionUnmap;

    pThis->__nvencsessionGetMemInterMapParams__ = &__nvoc_thunk_RmResource_nvencsessionGetMemInterMapParams;

    pThis->__nvencsessionGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_nvencsessionGetMemoryMappingDescriptor;

    pThis->__nvencsessionControlFilter__ = &__nvoc_thunk_RsResource_nvencsessionControlFilter;

    pThis->__nvencsessionControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_nvencsessionControlSerialization_Prologue;

    pThis->__nvencsessionCanCopy__ = &__nvoc_thunk_RsResource_nvencsessionCanCopy;

    pThis->__nvencsessionIsPartialUnmapSupported__ = &__nvoc_thunk_RsResource_nvencsessionIsPartialUnmapSupported;

    pThis->__nvencsessionPreDestruct__ = &__nvoc_thunk_RsResource_nvencsessionPreDestruct;

    pThis->__nvencsessionIsDuplicate__ = &__nvoc_thunk_RsResource_nvencsessionIsDuplicate;

    pThis->__nvencsessionControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_nvencsessionControlSerialization_Epilogue;

    pThis->__nvencsessionMap__ = &__nvoc_thunk_GpuResource_nvencsessionMap;

    pThis->__nvencsessionAccessCallback__ = &__nvoc_thunk_RmResource_nvencsessionAccessCallback;
}

void __nvoc_init_funcTable_NvencSession(NvencSession *pThis) {
    __nvoc_init_funcTable_NvencSession_1(pThis);
}

void __nvoc_init_GpuResource(GpuResource*);
void __nvoc_init_NvencSession(NvencSession *pThis) {
    pThis->__nvoc_pbase_NvencSession = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;
    __nvoc_init_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init_funcTable_NvencSession(pThis);
}

NV_STATUS __nvoc_objCreate_NvencSession(NvencSession **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    NvencSession *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NvencSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(NvencSession));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NvencSession);

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

    __nvoc_init_NvencSession(pThis);
    status = __nvoc_ctor_NvencSession(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_NvencSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NvencSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NvencSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NvencSession(NvencSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_NvencSession(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

