#define NVOC_FM_SESSION_API_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_fm_session_api_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xdfbd08 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FmSessionApi;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_FmSessionApi(FmSessionApi*);
void __nvoc_init_funcTable_FmSessionApi(FmSessionApi*);
NV_STATUS __nvoc_ctor_FmSessionApi(FmSessionApi*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_FmSessionApi(FmSessionApi*);
void __nvoc_dtor_FmSessionApi(FmSessionApi*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_FmSessionApi;

static const struct NVOC_RTTI __nvoc_rtti_FmSessionApi_FmSessionApi = {
    /*pClassDef=*/          &__nvoc_class_def_FmSessionApi,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_FmSessionApi,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_FmSessionApi_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FmSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_FmSessionApi_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FmSessionApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_FmSessionApi_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FmSessionApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_FmSessionApi_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(FmSessionApi, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_FmSessionApi = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_FmSessionApi_FmSessionApi,
        &__nvoc_rtti_FmSessionApi_RmResource,
        &__nvoc_rtti_FmSessionApi_RmResourceCommon,
        &__nvoc_rtti_FmSessionApi_RsResource,
        &__nvoc_rtti_FmSessionApi_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_FmSessionApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(FmSessionApi),
        /*classId=*/            classId(FmSessionApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "FmSessionApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_FmSessionApi,
    /*pCastInfo=*/          &__nvoc_castinfo_FmSessionApi,
    /*pExportInfo=*/        &__nvoc_export_info_FmSessionApi
};

static NvBool __nvoc_thunk_RmResource_fmsessionapiShareCallback(struct FmSessionApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_fmsessionapiCheckMemInterUnmap(struct FmSessionApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_FmSessionApi_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_fmsessionapiControl(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_fmsessionapiGetMemInterMapParams(struct FmSessionApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_FmSessionApi_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_fmsessionapiGetMemoryMappingDescriptor(struct FmSessionApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_FmSessionApi_RmResource.offset), ppMemDesc);
}

static NvU32 __nvoc_thunk_RsResource_fmsessionapiGetRefCount(struct FmSessionApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_fmsessionapiControlFilter(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_fmsessionapiAddAdditionalDependants(struct RsClient *pClient, struct FmSessionApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_fmsessionapiUnmapFrom(struct FmSessionApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_fmsessionapiControlSerialization_Prologue(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_fmsessionapiControl_Prologue(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_fmsessionapiCanCopy(struct FmSessionApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_fmsessionapiUnmap(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset), pCallContext, pCpuMapping);
}

static void __nvoc_thunk_RsResource_fmsessionapiPreDestruct(struct FmSessionApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_fmsessionapiMapTo(struct FmSessionApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_fmsessionapiIsDuplicate(struct FmSessionApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_fmsessionapiControlSerialization_Epilogue(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RmResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RmResource_fmsessionapiControl_Epilogue(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_fmsessionapiControlLookup(struct FmSessionApi *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_fmsessionapiMap(struct FmSessionApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_fmsessionapiAccessCallback(struct FmSessionApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_FmSessionApi_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_FmSessionApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) fmsessionapiCtrlCmdSetFmState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf0101u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_FmSessionApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "fmsessionapiCtrlCmdSetFmState"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) fmsessionapiCtrlCmdClearFmState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
        /*flags=*/      0x10u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xf0102u,
        /*paramSize=*/  0,
        /*pClassInfo=*/ &(__nvoc_class_def_FmSessionApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "fmsessionapiCtrlCmdClearFmState"
#endif
    },

};

const struct NVOC_EXPORT_INFO __nvoc_export_info_FmSessionApi = 
{
    /*numEntries=*/     2,
    /*pExportEntries=*/ __nvoc_exported_method_def_FmSessionApi
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_FmSessionApi(FmSessionApi *pThis) {
    __nvoc_fmsessionapiDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_FmSessionApi(FmSessionApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_FmSessionApi(FmSessionApi *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_FmSessionApi_fail_RmResource;
    __nvoc_init_dataField_FmSessionApi(pThis);

    status = __nvoc_fmsessionapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_FmSessionApi_fail__init;
    goto __nvoc_ctor_FmSessionApi_exit; // Success

__nvoc_ctor_FmSessionApi_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_FmSessionApi_fail_RmResource:
__nvoc_ctor_FmSessionApi_exit:

    return status;
}

static void __nvoc_init_funcTable_FmSessionApi_1(FmSessionApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__fmsessionapiCtrlCmdSetFmState__ = &fmsessionapiCtrlCmdSetFmState_IMPL;
#endif

#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x10u)
    pThis->__fmsessionapiCtrlCmdClearFmState__ = &fmsessionapiCtrlCmdClearFmState_IMPL;
#endif

    pThis->__fmsessionapiShareCallback__ = &__nvoc_thunk_RmResource_fmsessionapiShareCallback;

    pThis->__fmsessionapiCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_fmsessionapiCheckMemInterUnmap;

    pThis->__fmsessionapiControl__ = &__nvoc_thunk_RsResource_fmsessionapiControl;

    pThis->__fmsessionapiGetMemInterMapParams__ = &__nvoc_thunk_RmResource_fmsessionapiGetMemInterMapParams;

    pThis->__fmsessionapiGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_fmsessionapiGetMemoryMappingDescriptor;

    pThis->__fmsessionapiGetRefCount__ = &__nvoc_thunk_RsResource_fmsessionapiGetRefCount;

    pThis->__fmsessionapiControlFilter__ = &__nvoc_thunk_RsResource_fmsessionapiControlFilter;

    pThis->__fmsessionapiAddAdditionalDependants__ = &__nvoc_thunk_RsResource_fmsessionapiAddAdditionalDependants;

    pThis->__fmsessionapiUnmapFrom__ = &__nvoc_thunk_RsResource_fmsessionapiUnmapFrom;

    pThis->__fmsessionapiControlSerialization_Prologue__ = &__nvoc_thunk_RmResource_fmsessionapiControlSerialization_Prologue;

    pThis->__fmsessionapiControl_Prologue__ = &__nvoc_thunk_RmResource_fmsessionapiControl_Prologue;

    pThis->__fmsessionapiCanCopy__ = &__nvoc_thunk_RsResource_fmsessionapiCanCopy;

    pThis->__fmsessionapiUnmap__ = &__nvoc_thunk_RsResource_fmsessionapiUnmap;

    pThis->__fmsessionapiPreDestruct__ = &__nvoc_thunk_RsResource_fmsessionapiPreDestruct;

    pThis->__fmsessionapiMapTo__ = &__nvoc_thunk_RsResource_fmsessionapiMapTo;

    pThis->__fmsessionapiIsDuplicate__ = &__nvoc_thunk_RsResource_fmsessionapiIsDuplicate;

    pThis->__fmsessionapiControlSerialization_Epilogue__ = &__nvoc_thunk_RmResource_fmsessionapiControlSerialization_Epilogue;

    pThis->__fmsessionapiControl_Epilogue__ = &__nvoc_thunk_RmResource_fmsessionapiControl_Epilogue;

    pThis->__fmsessionapiControlLookup__ = &__nvoc_thunk_RsResource_fmsessionapiControlLookup;

    pThis->__fmsessionapiMap__ = &__nvoc_thunk_RsResource_fmsessionapiMap;

    pThis->__fmsessionapiAccessCallback__ = &__nvoc_thunk_RmResource_fmsessionapiAccessCallback;
}

void __nvoc_init_funcTable_FmSessionApi(FmSessionApi *pThis) {
    __nvoc_init_funcTable_FmSessionApi_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_FmSessionApi(FmSessionApi *pThis) {
    pThis->__nvoc_pbase_FmSessionApi = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_FmSessionApi(pThis);
}

NV_STATUS __nvoc_objCreate_FmSessionApi(FmSessionApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    FmSessionApi *pThis;

    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(FmSessionApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    portMemSet(pThis, 0, sizeof(FmSessionApi));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_FmSessionApi);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_FmSessionApi(pThis);
    status = __nvoc_ctor_FmSessionApi(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_FmSessionApi_cleanup;

    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_FmSessionApi_cleanup:
    // do not call destructors here since the constructor already called them
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(FmSessionApi));
    else
        portMemFree(pThis);

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_FmSessionApi(FmSessionApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_FmSessionApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

