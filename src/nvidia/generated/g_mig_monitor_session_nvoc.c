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

// 21 up-thunk(s) defined to bridge methods in MIGMonitorSession to superclasses

// migmonitorsessionAccessCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_migmonitorsessionAccessCallback(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// migmonitorsessionShareCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_migmonitorsessionShareCallback(struct MIGMonitorSession *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// migmonitorsessionGetMemInterMapParams: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionGetMemInterMapParams(struct MIGMonitorSession *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pParams);
}

// migmonitorsessionCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionCheckMemInterUnmap(struct MIGMonitorSession *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), bSubdeviceHandleProvided);
}

// migmonitorsessionGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionGetMemoryMappingDescriptor(struct MIGMonitorSession *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), ppMemDesc);
}

// migmonitorsessionControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Prologue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pCallContext, pParams);
}

// migmonitorsessionControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Epilogue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pCallContext, pParams);
}

// migmonitorsessionControl_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_migmonitorsessionControl_Prologue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pCallContext, pParams);
}

// migmonitorsessionControl_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_migmonitorsessionControl_Epilogue(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RmResource.offset), pCallContext, pParams);
}

// migmonitorsessionCanCopy: virtual inherited (res) base (rmres)
static NvBool __nvoc_up_thunk_RsResource_migmonitorsessionCanCopy(struct MIGMonitorSession *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset));
}

// migmonitorsessionIsDuplicate: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionIsDuplicate(struct MIGMonitorSession *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), hMemory, pDuplicate);
}

// migmonitorsessionPreDestruct: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_migmonitorsessionPreDestruct(struct MIGMonitorSession *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset));
}

// migmonitorsessionControl: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionControl(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pCallContext, pParams);
}

// migmonitorsessionControlFilter: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionControlFilter(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pCallContext, pParams);
}

// migmonitorsessionMap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionMap(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

// migmonitorsessionUnmap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionUnmap(struct MIGMonitorSession *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pCallContext, pCpuMapping);
}

// migmonitorsessionIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
static NvBool __nvoc_up_thunk_RsResource_migmonitorsessionIsPartialUnmapSupported(struct MIGMonitorSession *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset));
}

// migmonitorsessionMapTo: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionMapTo(struct MIGMonitorSession *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pParams);
}

// migmonitorsessionUnmapFrom: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_migmonitorsessionUnmapFrom(struct MIGMonitorSession *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pParams);
}

// migmonitorsessionGetRefCount: virtual inherited (res) base (rmres)
static NvU32 __nvoc_up_thunk_RsResource_migmonitorsessionGetRefCount(struct MIGMonitorSession *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset));
}

// migmonitorsessionAddAdditionalDependants: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_migmonitorsessionAddAdditionalDependants(struct RsClient *pClient, struct MIGMonitorSession *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_MIGMonitorSession_RsResource.offset), pReference);
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

// Vtable initialization
static void __nvoc_init_funcTable_MIGMonitorSession_1(MIGMonitorSession *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // migmonitorsessionAccessCallback -- virtual inherited (rmres) base (rmres)
    pThis->__migmonitorsessionAccessCallback__ = &__nvoc_up_thunk_RmResource_migmonitorsessionAccessCallback;

    // migmonitorsessionShareCallback -- virtual inherited (rmres) base (rmres)
    pThis->__migmonitorsessionShareCallback__ = &__nvoc_up_thunk_RmResource_migmonitorsessionShareCallback;

    // migmonitorsessionGetMemInterMapParams -- virtual inherited (rmres) base (rmres)
    pThis->__migmonitorsessionGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_migmonitorsessionGetMemInterMapParams;

    // migmonitorsessionCheckMemInterUnmap -- virtual inherited (rmres) base (rmres)
    pThis->__migmonitorsessionCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_migmonitorsessionCheckMemInterUnmap;

    // migmonitorsessionGetMemoryMappingDescriptor -- virtual inherited (rmres) base (rmres)
    pThis->__migmonitorsessionGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_migmonitorsessionGetMemoryMappingDescriptor;

    // migmonitorsessionControlSerialization_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__migmonitorsessionControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Prologue;

    // migmonitorsessionControlSerialization_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__migmonitorsessionControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_migmonitorsessionControlSerialization_Epilogue;

    // migmonitorsessionControl_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__migmonitorsessionControl_Prologue__ = &__nvoc_up_thunk_RmResource_migmonitorsessionControl_Prologue;

    // migmonitorsessionControl_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__migmonitorsessionControl_Epilogue__ = &__nvoc_up_thunk_RmResource_migmonitorsessionControl_Epilogue;

    // migmonitorsessionCanCopy -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionCanCopy__ = &__nvoc_up_thunk_RsResource_migmonitorsessionCanCopy;

    // migmonitorsessionIsDuplicate -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionIsDuplicate__ = &__nvoc_up_thunk_RsResource_migmonitorsessionIsDuplicate;

    // migmonitorsessionPreDestruct -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionPreDestruct__ = &__nvoc_up_thunk_RsResource_migmonitorsessionPreDestruct;

    // migmonitorsessionControl -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionControl__ = &__nvoc_up_thunk_RsResource_migmonitorsessionControl;

    // migmonitorsessionControlFilter -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionControlFilter__ = &__nvoc_up_thunk_RsResource_migmonitorsessionControlFilter;

    // migmonitorsessionMap -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionMap__ = &__nvoc_up_thunk_RsResource_migmonitorsessionMap;

    // migmonitorsessionUnmap -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionUnmap__ = &__nvoc_up_thunk_RsResource_migmonitorsessionUnmap;

    // migmonitorsessionIsPartialUnmapSupported -- inline virtual inherited (res) base (rmres) body
    pThis->__migmonitorsessionIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_migmonitorsessionIsPartialUnmapSupported;

    // migmonitorsessionMapTo -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionMapTo__ = &__nvoc_up_thunk_RsResource_migmonitorsessionMapTo;

    // migmonitorsessionUnmapFrom -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionUnmapFrom__ = &__nvoc_up_thunk_RsResource_migmonitorsessionUnmapFrom;

    // migmonitorsessionGetRefCount -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionGetRefCount__ = &__nvoc_up_thunk_RsResource_migmonitorsessionGetRefCount;

    // migmonitorsessionAddAdditionalDependants -- virtual inherited (res) base (rmres)
    pThis->__migmonitorsessionAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_migmonitorsessionAddAdditionalDependants;
} // End __nvoc_init_funcTable_MIGMonitorSession_1 with approximately 21 basic block(s).


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_MIGMonitorSession(MIGMonitorSession *pThis) {

    // Initialize vtable(s) with 21 per-object function pointer(s).
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

NV_STATUS __nvoc_objCreate_MIGMonitorSession(MIGMonitorSession **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    MIGMonitorSession *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(MIGMonitorSession), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(MIGMonitorSession));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_MIGMonitorSession);

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

    __nvoc_init_MIGMonitorSession(pThis);
    status = __nvoc_ctor_MIGMonitorSession(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_MIGMonitorSession_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_MIGMonitorSession_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(MIGMonitorSession));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_MIGMonitorSession(MIGMonitorSession **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_MIGMonitorSession(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

