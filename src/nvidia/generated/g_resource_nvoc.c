#define NVOC_RESOURCE_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_resource_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x8ef259 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

void __nvoc_init_RmResourceCommon(RmResourceCommon*);
void __nvoc_init_funcTable_RmResourceCommon(RmResourceCommon*);
NV_STATUS __nvoc_ctor_RmResourceCommon(RmResourceCommon*);
void __nvoc_init_dataField_RmResourceCommon(RmResourceCommon*);
void __nvoc_dtor_RmResourceCommon(RmResourceCommon*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RmResourceCommon;

static const struct NVOC_RTTI __nvoc_rtti_RmResourceCommon_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RmResourceCommon,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RmResourceCommon = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_RmResourceCommon_RmResourceCommon,
    },
};

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RmResourceCommon),
        /*classId=*/            classId(RmResourceCommon),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RmResourceCommon",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo_RmResourceCommon,
    /*pExportInfo=*/        &__nvoc_export_info_RmResourceCommon
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_RmResourceCommon = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResourceCommon(RmResourceCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RmResourceCommon(RmResourceCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResourceCommon(RmResourceCommon *pThis) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_RmResourceCommon(pThis);

    status = __nvoc_rmrescmnConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_RmResourceCommon_fail__init;
    goto __nvoc_ctor_RmResourceCommon_exit; // Success

__nvoc_ctor_RmResourceCommon_fail__init:
__nvoc_ctor_RmResourceCommon_exit:

    return status;
}

static void __nvoc_init_funcTable_RmResourceCommon_1(RmResourceCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_funcTable_RmResourceCommon(RmResourceCommon *pThis) {
    __nvoc_init_funcTable_RmResourceCommon_1(pThis);
}

void __nvoc_init_RmResourceCommon(RmResourceCommon *pThis) {
    pThis->__nvoc_pbase_RmResourceCommon = pThis;
    __nvoc_init_funcTable_RmResourceCommon(pThis);
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x03610d = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_funcTable_RmResource(RmResource*);
NV_STATUS __nvoc_ctor_RmResource(RmResource*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_RmResource(RmResource*);
void __nvoc_dtor_RmResource(RmResource*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_RmResource;

static const struct NVOC_RTTI __nvoc_rtti_RmResource_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_RmResource,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_RmResource_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RmResource, __nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_RmResource_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RmResource, __nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_RmResource_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(RmResource, __nvoc_base_RmResourceCommon),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_RmResource = {
    /*numRelatives=*/       4,
    /*relatives=*/ {
        &__nvoc_rtti_RmResource_RmResource,
        &__nvoc_rtti_RmResource_RmResourceCommon,
        &__nvoc_rtti_RmResource_RsResource,
        &__nvoc_rtti_RmResource_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(RmResource),
        /*classId=*/            classId(RmResource),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "RmResource",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_RmResource,
    /*pCastInfo=*/          &__nvoc_castinfo_RmResource,
    /*pExportInfo=*/        &__nvoc_export_info_RmResource
};

static NvBool __nvoc_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) - __nvoc_rtti_RmResource_RsResource.offset), pInvokingClient, pAllocParams, accessRight);
}

static NvBool __nvoc_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) - __nvoc_rtti_RmResource_RsResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) - __nvoc_rtti_RmResource_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) - __nvoc_rtti_RmResource_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset), pParams);
}

static NvU32 __nvoc_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset), pReference);
}

static NvBool __nvoc_thunk_RsResource_rmresCanCopy(struct RmResource *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset));
}

static void __nvoc_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_rmresControlLookup(struct RmResource *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_RmResource_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_RmResource = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsResource(RsResource*);
void __nvoc_dtor_RmResourceCommon(RmResourceCommon*);
void __nvoc_dtor_RmResource(RmResource *pThis) {
    __nvoc_dtor_RsResource(&pThis->__nvoc_base_RsResource);
    __nvoc_dtor_RmResourceCommon(&pThis->__nvoc_base_RmResourceCommon);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_RmResource(RmResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsResource(RsResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_RmResourceCommon(RmResourceCommon* );
NV_STATUS __nvoc_ctor_RmResource(RmResource *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsResource(&pThis->__nvoc_base_RsResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RmResource_fail_RsResource;
    status = __nvoc_ctor_RmResourceCommon(&pThis->__nvoc_base_RmResourceCommon);
    if (status != NV_OK) goto __nvoc_ctor_RmResource_fail_RmResourceCommon;
    __nvoc_init_dataField_RmResource(pThis);

    status = __nvoc_rmresConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_RmResource_fail__init;
    goto __nvoc_ctor_RmResource_exit; // Success

__nvoc_ctor_RmResource_fail__init:
    __nvoc_dtor_RmResourceCommon(&pThis->__nvoc_base_RmResourceCommon);
__nvoc_ctor_RmResource_fail_RmResourceCommon:
    __nvoc_dtor_RsResource(&pThis->__nvoc_base_RsResource);
__nvoc_ctor_RmResource_fail_RsResource:
__nvoc_ctor_RmResource_exit:

    return status;
}

static void __nvoc_init_funcTable_RmResource_1(RmResource *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__rmresAccessCallback__ = &rmresAccessCallback_IMPL;

    pThis->__rmresShareCallback__ = &rmresShareCallback_IMPL;

    pThis->__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL;

    pThis->__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL;

    pThis->__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL;

    pThis->__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL;

    pThis->__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL;

    pThis->__nvoc_base_RsResource.__resAccessCallback__ = &__nvoc_thunk_RmResource_resAccessCallback;

    pThis->__nvoc_base_RsResource.__resShareCallback__ = &__nvoc_thunk_RmResource_resShareCallback;

    pThis->__nvoc_base_RsResource.__resControl_Prologue__ = &__nvoc_thunk_RmResource_resControl_Prologue;

    pThis->__nvoc_base_RsResource.__resControl_Epilogue__ = &__nvoc_thunk_RmResource_resControl_Epilogue;

    pThis->__rmresControl__ = &__nvoc_thunk_RsResource_rmresControl;

    pThis->__rmresUnmap__ = &__nvoc_thunk_RsResource_rmresUnmap;

    pThis->__rmresMapTo__ = &__nvoc_thunk_RsResource_rmresMapTo;

    pThis->__rmresGetRefCount__ = &__nvoc_thunk_RsResource_rmresGetRefCount;

    pThis->__rmresControlFilter__ = &__nvoc_thunk_RsResource_rmresControlFilter;

    pThis->__rmresAddAdditionalDependants__ = &__nvoc_thunk_RsResource_rmresAddAdditionalDependants;

    pThis->__rmresCanCopy__ = &__nvoc_thunk_RsResource_rmresCanCopy;

    pThis->__rmresPreDestruct__ = &__nvoc_thunk_RsResource_rmresPreDestruct;

    pThis->__rmresUnmapFrom__ = &__nvoc_thunk_RsResource_rmresUnmapFrom;

    pThis->__rmresControlLookup__ = &__nvoc_thunk_RsResource_rmresControlLookup;

    pThis->__rmresMap__ = &__nvoc_thunk_RsResource_rmresMap;
}

void __nvoc_init_funcTable_RmResource(RmResource *pThis) {
    __nvoc_init_funcTable_RmResource_1(pThis);
}

void __nvoc_init_RsResource(RsResource*);
void __nvoc_init_RmResourceCommon(RmResourceCommon*);
void __nvoc_init_RmResource(RmResource *pThis) {
    pThis->__nvoc_pbase_RmResource = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResourceCommon;
    __nvoc_init_RsResource(&pThis->__nvoc_base_RsResource);
    __nvoc_init_RmResourceCommon(&pThis->__nvoc_base_RmResourceCommon);
    __nvoc_init_funcTable_RmResource(pThis);
}

NV_STATUS __nvoc_objCreate_RmResource(RmResource **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    RmResource *pThis;

    pThis = portMemAllocNonPaged(sizeof(RmResource));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(RmResource));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_RmResource);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_RmResource(pThis);
    status = __nvoc_ctor_RmResource(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_RmResource_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_RmResource_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_RmResource(RmResource **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_RmResource(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

