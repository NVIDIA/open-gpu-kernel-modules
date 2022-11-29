#define NVOC_MIG_MONITOR_SESSION_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mig_monitor_session_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x29e15c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MIGMonitorSession;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_MIGMonitorSession(MIGMonitorSession*);
void __nvoc_init_funcTable_MIGMonitorSession(MIGMonitorSession*);
NV_STATUS __nvoc_ctor_MIGMonitorSession(MIGMonitorSession*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MIGMonitorSession(MIGMonitorSession*);
void __nvoc_dtor_MIGMonitorSession(MIGMonitorSession*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MIGMonitorSession;

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_MIGMonitorSession = {
    /*pClassDef=*/          &__nvoc_class_def_MIGMonitorSession,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MIGMonitorSession,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MIGMonitorSession_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGMonitorSession, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MIGMonitorSession = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_MIGMonitorSession_MIGMonitorSession,
        &__nvoc_rtti_MIGMonitorSession_RmResource,
        &__nvoc_rtti_MIGMonitorSession_RmResourceCommon,
        &__nvoc_rtti_MIGMonitorSession_RsResource,
        &__nvoc_rtti_MIGMonitorSession_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MIGMonitorSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MIGMonitorSession),
        /*classId=*/            classId(MIGMonitorSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MIGMonitorSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MIGMonitorSession,
    /*pCastInfo=*/          &__nvoc_castinfo_MIGMonitorSession,
    /*pExportInfo=*/        &__nvoc_export_info_MIGMonitorSession
};

static NvBool __nvoc_thunk_RmResource_migmonitorsessionShareCallback(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_migmonitorsessionCheckMemInterUnmap(struct MIGMonitorSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_migmonitorsessionControl(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_migmonitorsessionGetMemInterMapParams(struct MIGMonitorSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_migmonitorsessionGetMemoryMappingDescriptor(struct MIGMonitorSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), ppMemDesc);
}

static NvU32 __nvoc_thunk_RsResource_migmonitorsessionGetRefCount(struct MIGMonitorSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_migmonitorsessionControlFilter(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_migmonitorsessionAddAdditionalDependants(struct RsClient *pClient, struct MIGMonitorSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_migmonitorsessionUnmap(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_migmonitorsessionControl_Prologue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_migmonitorsessionCanCopy(struct MIGMonitorSession *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_migmonitorsessionMapTo(struct MIGMonitorSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pParams);
}

static void __nvoc_thunk_RsResource_migmonitorsessionPreDestruct(struct MIGMonitorSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_migmonitorsessionUnmapFrom(struct MIGMonitorSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_migmonitorsessionIsDuplicate(struct MIGMonitorSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), hMemory, pDuplicate);
}

static void __nvoc_thunk_RmResource_migmonitorsessionControl_Epilogue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_migmonitorsessionControlLookup(struct MIGMonitorSession *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_migmonitorsessionMap(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_migmonitorsessionAccessCallback(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

const struct NVOC_EXPORT_INFO __nvoc_export_info_MIGMonitorSession = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_MIGMonitorSession(MIGMonitorSession *pThis) {
    __nvoc_migmonitorsessionDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MIGMonitorSession(MIGMonitorSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MIGMonitorSession(MIGMonitorSession *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MIGMonitorSession_fail_RmResource;
    __nvoc_init_dataField_MIGMonitorSession(pThis);

    status = __nvoc_migmonitorsessionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MIGMonitorSession_fail__init;
    goto __nvoc_ctor_MIGMonitorSession_exit; // Success

__nvoc_ctor_MIGMonitorSession_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_MIGMonitorSession_fail_RmResource:
__nvoc_ctor_MIGMonitorSession_exit:

    return status;
}

static void __nvoc_init_funcTable_MIGMonitorSession_1(MIGMonitorSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__migmonitorsessionShareCallback__ = &__nvoc_thunk_RmResource_migmonitorsessionShareCallback;

    pThis->__migmonitorsessionCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_migmonitorsessionCheckMemInterUnmap;

    pThis->__migmonitorsessionControl__ = &__nvoc_thunk_RsResource_migmonitorsessionControl;

    pThis->__migmonitorsessionGetMemInterMapParams__ = &__nvoc_thunk_RmResource_migmonitorsessionGetMemInterMapParams;

    pThis->__migmonitorsessionGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_migmonitorsessionGetMemoryMappingDescriptor;

    pThis->__migmonitorsessionGetRefCount__ = &__nvoc_thunk_RsResource_migmonitorsessionGetRefCount;

    pThis->__migmonitorsessionControlFilter__ = &__nvoc_thunk_RsResource_migmonitorsessionControlFilter;

    pThis->__migmonitorsessionAddAdditionalDependants__ = &__nvoc_thunk_RsResource_migmonitorsessionAddAdditionalDependants;

    pThis->__migmonitorsessionUnmap__ = &__nvoc_thunk_RsResource_migmonitorsessionUnmap;

    pThis->__migmonitorsessionControl_Prologue__ = &__nvoc_thunk_RmResource_migmonitorsessionControl_Prologue;

    pThis->__migmonitorsessionCanCopy__ = &__nvoc_thunk_RsResource_migmonitorsessionCanCopy;

    pThis->__migmonitorsessionMapTo__ = &__nvoc_thunk_RsResource_migmonitorsessionMapTo;

    pThis->__migmonitorsessionPreDestruct__ = &__nvoc_thunk_RsResource_migmonitorsessionPreDestruct;

    pThis->__migmonitorsessionUnmapFrom__ = &__nvoc_thunk_RsResource_migmonitorsessionUnmapFrom;

    pThis->__migmonitorsessionIsDuplicate__ = &__nvoc_thunk_RsResource_migmonitorsessionIsDuplicate;

    pThis->__migmonitorsessionControl_Epilogue__ = &__nvoc_thunk_RmResource_migmonitorsessionControl_Epilogue;

    pThis->__migmonitorsessionControlLookup__ = &__nvoc_thunk_RsResource_migmonitorsessionControlLookup;

    pThis->__migmonitorsessionMap__ = &__nvoc_thunk_RsResource_migmonitorsessionMap;

    pThis->__migmonitorsessionAccessCallback__ = &__nvoc_thunk_RmResource_migmonitorsessionAccessCallback;
}

void __nvoc_init_funcTable_MIGMonitorSession(MIGMonitorSession *pThis) {
    __nvoc_init_funcTable_MIGMonitorSession_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_MIGMonitorSession(MIGMonitorSession *pThis) {
    pThis->__nvoc_pbase_MIGMonitorSession = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_MIGMonitorSession(pThis);
}

NV_STATUS __nvoc_objCreate_MIGMonitorSession(MIGMonitorSession **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    MIGMonitorSession *pThis;

    pThis = portMemAllocNonPaged(sizeof(MIGMonitorSession));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(MIGMonitorSession));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MIGMonitorSession);

    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_MIGMonitorSession(pThis);
    status = __nvoc_ctor_MIGMonitorSession(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MIGMonitorSession_cleanup;

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_MIGMonitorSession_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MIGMonitorSession(MIGMonitorSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MIGMonitorSession(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

