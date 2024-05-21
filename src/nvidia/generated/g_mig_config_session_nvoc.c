#define NVOC_MIG_CONFIG_SESSION_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_mig_config_session_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0x36a941 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_MIGConfigSession;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_MIGConfigSession(MIGConfigSession*);
void __nvoc_init_funcTable_MIGConfigSession(MIGConfigSession*);
NV_STATUS __nvoc_ctor_MIGConfigSession(MIGConfigSession*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_MIGConfigSession(MIGConfigSession*);
void __nvoc_dtor_MIGConfigSession(MIGConfigSession*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_MIGConfigSession;

static const struct NVOC_RTTI __nvoc_rtti_MIGConfigSession_MIGConfigSession = {
    /*pClassDef=*/          &__nvoc_class_def_MIGConfigSession,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_MIGConfigSession,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_MIGConfigSession_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGConfigSession, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_MIGConfigSession_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGConfigSession, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_MIGConfigSession_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGConfigSession, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_MIGConfigSession_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(MIGConfigSession, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_MIGConfigSession = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_MIGConfigSession_MIGConfigSession,
        &__nvoc_rtti_MIGConfigSession_RmResource,
        &__nvoc_rtti_MIGConfigSession_RmResourceCommon,
        &__nvoc_rtti_MIGConfigSession_RsResource,
        &__nvoc_rtti_MIGConfigSession_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_MIGConfigSession = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(MIGConfigSession),
        /*classId=*/            classId(MIGConfigSession),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "MIGConfigSession",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_MIGConfigSession,
    /*pCastInfo=*/          &__nvoc_castinfo_MIGConfigSession,
    /*pExportInfo=*/        &__nvoc_export_info_MIGConfigSession
};

// 21 up-thunk(s) defined to bridge methods in MIGConfigSession to superclasses

// migconfigsessionAccessCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_migconfigsessionAccessCallback(struct MIGConfigSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// migconfigsessionShareCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_migconfigsessionShareCallback(struct MIGConfigSession *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// migconfigsessionGetMemInterMapParams: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migconfigsessionGetMemInterMapParams(struct MIGConfigSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MIGConfigSession_RmResource.offset), pParams);
}

// migconfigsessionCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migconfigsessionCheckMemInterUnmap(struct MIGConfigSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MIGConfigSession_RmResource.offset), bSubdeviceHandleProvided);
}

// migconfigsessionGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migconfigsessionGetMemoryMappingDescriptor(struct MIGConfigSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MIGConfigSession_RmResource.offset), ppMemDesc);
}

// migconfigsessionControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migconfigsessionControlSerialization_Prologue(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RmResource.offset), pCallContext, pParams);
}

// migconfigsessionControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_migconfigsessionControlSerialization_Epilogue(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RmResource.offset), pCallContext, pParams);
}

// migconfigsessionControl_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migconfigsessionControl_Prologue(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RmResource.offset), pCallContext, pParams);
}

// migconfigsessionControl_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_migconfigsessionControl_Epilogue(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RmResource.offset), pCallContext, pParams);
}

// migconfigsessionCanCopy: virtual inherited (res) base (rmres)
static NvBool __nvoc_up_thunk_RsResource_migconfigsessionCanCopy(struct MIGConfigSession *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset));
}

// migconfigsessionIsDuplicate: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migconfigsessionIsDuplicate(struct MIGConfigSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset), hMemory, pDuplicate);
}

// migconfigsessionPreDestruct: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_migconfigsessionPreDestruct(struct MIGConfigSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset));
}

// migconfigsessionControl: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migconfigsessionControl(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset), pCallContext, pParams);
}

// migconfigsessionControlFilter: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migconfigsessionControlFilter(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset), pCallContext, pParams);
}

// migconfigsessionMap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migconfigsessionMap(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

// migconfigsessionUnmap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migconfigsessionUnmap(struct MIGConfigSession *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset), pCallContext, pCpuMapping);
}

// migconfigsessionIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
static NvBool __nvoc_up_thunk_RsResource_migconfigsessionIsPartialUnmapSupported(struct MIGConfigSession *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset));
}

// migconfigsessionMapTo: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migconfigsessionMapTo(struct MIGConfigSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset), pParams);
}

// migconfigsessionUnmapFrom: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migconfigsessionUnmapFrom(struct MIGConfigSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset), pParams);
}

// migconfigsessionGetRefCount: virtual inherited (res) base (rmres)
static NvU32 __nvoc_up_thunk_RsResource_migconfigsessionGetRefCount(struct MIGConfigSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset));
}

// migconfigsessionAddAdditionalDependants: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_migconfigsessionAddAdditionalDependants(struct RsClient *pClient, struct MIGConfigSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGConfigSession_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_MIGConfigSession = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_MIGConfigSession(MIGConfigSession *pThis) {
    __nvoc_migconfigsessionDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_MIGConfigSession(MIGConfigSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_MIGConfigSession(MIGConfigSession *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MIGConfigSession_fail_RmResource;
    __nvoc_init_dataField_MIGConfigSession(pThis);

    status = __nvoc_migconfigsessionConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_MIGConfigSession_fail__init;
    goto __nvoc_ctor_MIGConfigSession_exit; // Success

__nvoc_ctor_MIGConfigSession_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_MIGConfigSession_fail_RmResource:
__nvoc_ctor_MIGConfigSession_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_MIGConfigSession_1(MIGConfigSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // migconfigsessionAccessCallback -- virtual inherited (rmres) base (rmres)
    pThis->__migconfigsessionAccessCallback__ = &__nvoc_up_thunk_RmResource_migconfigsessionAccessCallback;

    // migconfigsessionShareCallback -- virtual inherited (rmres) base (rmres)
    pThis->__migconfigsessionShareCallback__ = &__nvoc_up_thunk_RmResource_migconfigsessionShareCallback;

    // migconfigsessionGetMemInterMapParams -- virtual inherited (rmres) base (rmres)
    pThis->__migconfigsessionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_migconfigsessionGetMemInterMapParams;

    // migconfigsessionCheckMemInterUnmap -- virtual inherited (rmres) base (rmres)
    pThis->__migconfigsessionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_migconfigsessionCheckMemInterUnmap;

    // migconfigsessionGetMemoryMappingDescriptor -- virtual inherited (rmres) base (rmres)
    pThis->__migconfigsessionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_migconfigsessionGetMemoryMappingDescriptor;

    // migconfigsessionControlSerialization_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__migconfigsessionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_migconfigsessionControlSerialization_Prologue;

    // migconfigsessionControlSerialization_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__migconfigsessionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_migconfigsessionControlSerialization_Epilogue;

    // migconfigsessionControl_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__migconfigsessionControl_Prologue__ = &__nvoc_up_thunk_RmResource_migconfigsessionControl_Prologue;

    // migconfigsessionControl_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__migconfigsessionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_migconfigsessionControl_Epilogue;

    // migconfigsessionCanCopy -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionCanCopy__ = &__nvoc_up_thunk_RsResource_migconfigsessionCanCopy;

    // migconfigsessionIsDuplicate -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionIsDuplicate__ = &__nvoc_up_thunk_RsResource_migconfigsessionIsDuplicate;

    // migconfigsessionPreDestruct -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionPreDestruct__ = &__nvoc_up_thunk_RsResource_migconfigsessionPreDestruct;

    // migconfigsessionControl -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionControl__ = &__nvoc_up_thunk_RsResource_migconfigsessionControl;

    // migconfigsessionControlFilter -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionControlFilter__ = &__nvoc_up_thunk_RsResource_migconfigsessionControlFilter;

    // migconfigsessionMap -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionMap__ = &__nvoc_up_thunk_RsResource_migconfigsessionMap;

    // migconfigsessionUnmap -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionUnmap__ = &__nvoc_up_thunk_RsResource_migconfigsessionUnmap;

    // migconfigsessionIsPartialUnmapSupported -- inline virtual inherited (res) base (rmres) body
    pThis->__migconfigsessionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_migconfigsessionIsPartialUnmapSupported;

    // migconfigsessionMapTo -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionMapTo__ = &__nvoc_up_thunk_RsResource_migconfigsessionMapTo;

    // migconfigsessionUnmapFrom -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionUnmapFrom__ = &__nvoc_up_thunk_RsResource_migconfigsessionUnmapFrom;

    // migconfigsessionGetRefCount -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionGetRefCount__ = &__nvoc_up_thunk_RsResource_migconfigsessionGetRefCount;

    // migconfigsessionAddAdditionalDependants -- virtual inherited (res) base (rmres)
    pThis->__migconfigsessionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_migconfigsessionAddAdditionalDependants;
} // End __nvoc_init_funcTable_MIGConfigSession_1 with approximately 21 basic block(s).


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_MIGConfigSession(MIGConfigSession *pThis) {

    // Initialize vtable(s) with 21 per-object function pointer(s).
    __nvoc_init_funcTable_MIGConfigSession_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_MIGConfigSession(MIGConfigSession *pThis) {
    pThis->__nvoc_pbase_MIGConfigSession = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_MIGConfigSession(pThis);
}

NV_STATUS __nvoc_objCreate_MIGConfigSession(MIGConfigSession **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MIGConfigSession *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MIGConfigSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MIGConfigSession));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MIGConfigSession);

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_MIGConfigSession(pThis);
    status = __nvoc_ctor_MIGConfigSession(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MIGConfigSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MIGConfigSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MIGConfigSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MIGConfigSession(MIGConfigSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MIGConfigSession(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

