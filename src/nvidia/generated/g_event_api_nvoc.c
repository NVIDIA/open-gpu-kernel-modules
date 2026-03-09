#define NVOC_EVENT_API_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_event_api_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__d5f150 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NotifShare;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

// Forward declarations for NotifShare
void __nvoc_init__RsShared(RsShared*);
void __nvoc_init__NotifShare(NotifShare*);
void __nvoc_init_funcTable_NotifShare(NotifShare*);
NV_STATUS __nvoc_ctor_NotifShare(NotifShare*);
void __nvoc_init_dataField_NotifShare(NotifShare*);
void __nvoc_dtor_NotifShare(NotifShare*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NotifShare;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NotifShare;

// Down-thunk(s) to bridge NotifShare methods from ancestors (if any)

// Up-thunk(s) to bridge NotifShare methods to ancestors (if any)

// Class-specific details for NotifShare
const struct NVOC_CLASS_DEF __nvoc_class_def_NotifShare = 
{
    .classInfo.size =               sizeof(NotifShare),
    .classInfo.classId =            classId(NotifShare),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "NotifShare",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NotifShare,
    .pCastInfo =          &__nvoc_castinfo__NotifShare,
    .pExportInfo =        &__nvoc_export_info__NotifShare
};


// Metadata with per-class RTTI with ancestor(s)
static const struct NVOC_METADATA__NotifShare __nvoc_metadata__NotifShare = {
    .rtti.pClassDef = &__nvoc_class_def_NotifShare,    // (shrnotif) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NotifShare,
    .rtti.offset    = 0,
    .metadata__RsShared.rtti.pClassDef = &__nvoc_class_def_RsShared,    // (shr) super
    .metadata__RsShared.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.rtti.offset    = NV_OFFSETOF(NotifShare, __nvoc_base_RsShared),
    .metadata__RsShared.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^2
    .metadata__RsShared.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RsShared.metadata__Object.rtti.offset    = NV_OFFSETOF(NotifShare, __nvoc_base_RsShared.__nvoc_base_Object),
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NotifShare = {
    .numRelatives = 3,
    .relatives = {
        &__nvoc_metadata__NotifShare.rtti,    // [0]: (shrnotif) this
        &__nvoc_metadata__NotifShare.metadata__RsShared.rtti,    // [1]: (shr) super
        &__nvoc_metadata__NotifShare.metadata__RsShared.metadata__Object.rtti,    // [2]: (obj) super^2
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__NotifShare = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct NotifShare object.
void __nvoc_shrnotifDestruct(NotifShare*);
void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_NotifShare(NotifShare* pThis) {

// Call destructor.
    __nvoc_shrnotifDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_NotifShare(NotifShare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct NotifShare object.
NV_STATUS __nvoc_ctor_RsShared(RsShared *pShared);
NV_STATUS __nvoc_ctor_NotifShare(NotifShare *pNotifShare) {
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RsShared(&pNotifShare->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_NotifShare_fail_RsShared;

    // Initialize data fields.
    __nvoc_init_dataField_NotifShare(pNotifShare);

    // Call the constructor for this class.
    status = __nvoc_shrnotifConstruct(pNotifShare);
    if (status != NV_OK) goto __nvoc_ctor_NotifShare_fail__init;
    goto __nvoc_ctor_NotifShare_exit; // Success

    // Unwind on error.
__nvoc_ctor_NotifShare_fail__init:
    __nvoc_dtor_RsShared(&pNotifShare->__nvoc_base_RsShared);
__nvoc_ctor_NotifShare_fail_RsShared:
__nvoc_ctor_NotifShare_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NotifShare_1(NotifShare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_NotifShare_1


// Initialize vtable(s): Nothing to do for empty vtables
void __nvoc_init_funcTable_NotifShare(NotifShare *pThis) {
    __nvoc_init_funcTable_NotifShare_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NotifShare(NotifShare *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;    // (obj) super^2
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;    // (shr) super
    pThis->__nvoc_pbase_NotifShare = pThis;    // (shrnotif) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RsShared(&pThis->__nvoc_base_RsShared);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RsShared.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NotifShare.metadata__RsShared.metadata__Object;    // (obj) super^2
    pThis->__nvoc_base_RsShared.__nvoc_metadata_ptr = &__nvoc_metadata__NotifShare.metadata__RsShared;    // (shr) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NotifShare;    // (shrnotif) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NotifShare(pThis);
}

NV_STATUS __nvoc_objCreate_NotifShare(NotifShare **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    NotifShare *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(NotifShare));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(NotifShare));

    __nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__NotifShare(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_NotifShare(__nvoc_pThis);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_NotifShare_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_NotifShare_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(NotifShare));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_NotifShare(NotifShare **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;

    __nvoc_status = __nvoc_objCreate_NotifShare(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags);

    return __nvoc_status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__854293 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_EventApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

// Forward declarations for EventApi
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__EventApi(EventApi*);
void __nvoc_init_funcTable_EventApi(EventApi*);
NV_STATUS __nvoc_ctor_EventApi(EventApi*, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);
void __nvoc_init_dataField_EventApi(EventApi*);
void __nvoc_dtor_EventApi(EventApi*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__EventApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__EventApi;

// Down-thunk(s) to bridge EventApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super

// Up-thunk(s) to bridge EventApi methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super
NvBool __nvoc_up_thunk_RmResource_eventapiAccessCallback(struct EventApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_eventapiShareCallback(struct EventApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventapiGetMemInterMapParams(struct EventApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventapiCheckMemInterUnmap(struct EventApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventapiGetMemoryMappingDescriptor(struct EventApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventapiControlSerialization_Prologue(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_eventapiControlSerialization_Epilogue(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventapiControl_Prologue(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_eventapiControl_Epilogue(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_eventapiCanCopy(struct EventApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventapiIsDuplicate(struct EventApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_eventapiPreDestruct(struct EventApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventapiControl(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventapiControlFilter(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventapiMap(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventapiUnmap(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_eventapiIsPartialUnmapSupported(struct EventApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventapiMapTo(struct EventApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventapiUnmapFrom(struct EventApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_eventapiGetRefCount(struct EventApi *pResource);    // this
void __nvoc_up_thunk_RsResource_eventapiAddAdditionalDependants(struct RsClient *pClient, struct EventApi *pResource, RsResourceRef *pReference);    // this

// Class-specific details for EventApi
const struct NVOC_CLASS_DEF __nvoc_class_def_EventApi = 
{
    .classInfo.size =               sizeof(EventApi),
    .classInfo.classId =            classId(EventApi),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "EventApi",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_EventApi,
    .pCastInfo =          &__nvoc_castinfo__EventApi,
    .pExportInfo =        &__nvoc_export_info__EventApi
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__EventApi __nvoc_metadata__EventApi = {
    .rtti.pClassDef = &__nvoc_class_def_EventApi,    // (eventapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_EventApi,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(EventApi, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),

    .vtable.__eventapiAccessCallback__ = &__nvoc_up_thunk_RmResource_eventapiAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__eventapiShareCallback__ = &__nvoc_up_thunk_RmResource_eventapiShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__eventapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_eventapiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__eventapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_eventapiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__eventapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_eventapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__eventapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_eventapiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__eventapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_eventapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__eventapiControl_Prologue__ = &__nvoc_up_thunk_RmResource_eventapiControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__eventapiControl_Epilogue__ = &__nvoc_up_thunk_RmResource_eventapiControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__eventapiCanCopy__ = &__nvoc_up_thunk_RsResource_eventapiCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__eventapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_eventapiIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__eventapiPreDestruct__ = &__nvoc_up_thunk_RsResource_eventapiPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__eventapiControl__ = &__nvoc_up_thunk_RsResource_eventapiControl,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &resControl_IMPL,    // virtual
    .vtable.__eventapiControlFilter__ = &__nvoc_up_thunk_RsResource_eventapiControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__eventapiMap__ = &__nvoc_up_thunk_RsResource_eventapiMap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__eventapiUnmap__ = &__nvoc_up_thunk_RsResource_eventapiUnmap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__eventapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_eventapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__eventapiMapTo__ = &__nvoc_up_thunk_RsResource_eventapiMapTo,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__eventapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_eventapiUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__eventapiGetRefCount__ = &__nvoc_up_thunk_RsResource_eventapiGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__eventapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_eventapiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__EventApi = {
    .numRelatives = 5,
    .relatives = {
        &__nvoc_metadata__EventApi.rtti,    // [0]: (eventapi) this
        &__nvoc_metadata__EventApi.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__EventApi.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__EventApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__EventApi.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
    }
};

// 21 up-thunk(s) defined to bridge methods in EventApi to superclasses

// eventapiAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_eventapiAccessCallback(struct EventApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// eventapiShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_eventapiShareCallback(struct EventApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// eventapiGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventapiGetMemInterMapParams(struct EventApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource)), pParams);
}

// eventapiCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventapiCheckMemInterUnmap(struct EventApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// eventapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventapiGetMemoryMappingDescriptor(struct EventApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource)), ppMemDesc);
}

// eventapiControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventapiControlSerialization_Prologue(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventapiControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_eventapiControlSerialization_Epilogue(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventapiControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventapiControl_Prologue(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventapiControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_eventapiControl_Epilogue(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventapiCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_eventapiCanCopy(struct EventApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventapiIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventapiIsDuplicate(struct EventApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// eventapiPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_eventapiPreDestruct(struct EventApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventapiControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventapiControl(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// eventapiControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventapiControlFilter(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// eventapiMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventapiMap(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// eventapiUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventapiUnmap(struct EventApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// eventapiIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_eventapiIsPartialUnmapSupported(struct EventApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventapiMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventapiMapTo(struct EventApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// eventapiUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventapiUnmapFrom(struct EventApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// eventapiGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_eventapiGetRefCount(struct EventApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventapiAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_eventapiAddAdditionalDependants(struct RsClient *pClient, struct EventApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(EventApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__EventApi = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct EventApi object.
void __nvoc_eventapiDestruct(EventApi*);
void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_EventApi(EventApi* pThis) {

// Call destructor.
    __nvoc_eventapiDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_EventApi(EventApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct EventApi object.
NV_STATUS __nvoc_ctor_RmResource(RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams);    // inline
NV_STATUS __nvoc_ctor_EventApi(EventApi *pEvent, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_RmResource(&pEvent->__nvoc_base_RmResource, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_EventApi_fail_RmResource;

    // Initialize data fields.
    __nvoc_init_dataField_EventApi(pEvent);

    // Call the constructor for this class.
    status = __nvoc_eventapiConstruct(pEvent, pCallContext, pParams);
    if (status != NV_OK) goto __nvoc_ctor_EventApi_fail__init;
    goto __nvoc_ctor_EventApi_exit; // Success

    // Unwind on error.
__nvoc_ctor_EventApi_fail__init:
    __nvoc_dtor_RmResource(&pEvent->__nvoc_base_RmResource);
__nvoc_ctor_EventApi_fail_RmResource:
__nvoc_ctor_EventApi_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_EventApi_1(EventApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_EventApi_1


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_EventApi(EventApi *pThis) {
    __nvoc_init_funcTable_EventApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__EventApi(EventApi *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_EventApi = pThis;    // (eventapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__EventApi.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__EventApi.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__EventApi.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__EventApi.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__EventApi;    // (eventapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_EventApi(pThis);
}

NV_STATUS __nvoc_objCreate_EventApi(EventApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, struct CALL_CONTEXT *pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams)
{
    NV_STATUS __nvoc_status;
    Object *__nvoc_pParentObj = NULL;
    EventApi *__nvoc_pThis;

    // Don't allocate memory if the caller has already done so.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
    {
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_ppThis != NULL && *__nvoc_ppThis != NULL, NV_ERR_INVALID_PARAMETER);
        __nvoc_pThis = *__nvoc_ppThis;
    }

    // Allocate memory
    else
    {
        __nvoc_pThis = portMemAllocNonPaged(sizeof(EventApi));
        NV_CHECK_OR_RETURN(LEVEL_ERROR, __nvoc_pThis != NULL, NV_ERR_NO_MEMORY);
    }

    // Zero is the initial value for everything.
    portMemSet(__nvoc_pThis, 0, sizeof(EventApi));

    __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = __nvoc_createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (__nvoc_pParent != NULL && !(__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        __nvoc_pParentObj = dynamicCast(__nvoc_pParent, Object);
        objAddChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        __nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    // Initialize vtable, RTTI, etc., then call constructor.
    __nvoc_init__EventApi(__nvoc_pThis);
    __nvoc_status = __nvoc_ctor_EventApi(__nvoc_pThis, pCallContext, pParams);
    if (__nvoc_status != NV_OK) goto __nvoc_objCreate_EventApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *__nvoc_ppThis = __nvoc_pThis;

    // Success
    return NV_OK;

    // Do not call destructors here since the constructor already called them.
__nvoc_objCreate_EventApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (__nvoc_pParentObj != NULL)
        objRemoveChild(__nvoc_pParentObj, &__nvoc_pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Zero out memory that was allocated by caller.
    if (__nvoc_createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(__nvoc_pThis, 0, sizeof(EventApi));

    // Free memory allocated by `__nvoc_handleObjCreateMemAlloc`.
    else
    {
        portMemFree(__nvoc_pThis);
        *__nvoc_ppThis = NULL;
    }

    // Failure
    return __nvoc_status;
}

NV_STATUS __nvoc_objCreateDynamic_EventApi(EventApi **__nvoc_ppThis, Dynamic *__nvoc_pParent, NvU32 __nvoc_createFlags, va_list __nvoc_args) {
    NV_STATUS __nvoc_status;
    struct CALL_CONTEXT *pCallContext = va_arg(__nvoc_args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL *pParams = va_arg(__nvoc_args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    __nvoc_status = __nvoc_objCreate_EventApi(__nvoc_ppThis, __nvoc_pParent, __nvoc_createFlags, pCallContext, pParams);

    return __nvoc_status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__f8f965 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

// Forward declarations for INotifier
void __nvoc_init__INotifier(INotifier*);
void __nvoc_init_funcTable_INotifier(INotifier*);
NV_STATUS __nvoc_ctor_INotifier(INotifier*, struct CALL_CONTEXT *pCallContext);
void __nvoc_init_dataField_INotifier(INotifier*);
void __nvoc_dtor_INotifier(INotifier*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__INotifier;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__INotifier;

// Down-thunk(s) to bridge INotifier methods from ancestors (if any)

// Up-thunk(s) to bridge INotifier methods to ancestors (if any)

// Class-specific details for INotifier
// Not instantiable because it's not derived from class "Object"
// Not instantiable because it's an abstract class with following pure virtual functions:
//  inotifyGetNotificationListPtr
//  inotifySetNotificationShare
//  inotifyGetNotificationShare
//  inotifyUnregisterEvent
//  inotifyGetOrAllocNotifShare
const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier = 
{
    .classInfo.size =               sizeof(INotifier),
    .classInfo.classId =            classId(INotifier),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "INotifier",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    .pCastInfo =          &__nvoc_castinfo__INotifier,
    .pExportInfo =        &__nvoc_export_info__INotifier
};


// Metadata with per-class RTTI and vtable
static const struct NVOC_METADATA__INotifier __nvoc_metadata__INotifier = {
    .rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_INotifier,
    .rtti.offset    = 0,

    .vtable.__inotifyGetNotificationListPtr__ = NULL,    // pure virtual
    .vtable.__inotifySetNotificationShare__ = NULL,    // pure virtual
    .vtable.__inotifyGetNotificationShare__ = NULL,    // pure virtual
    .vtable.__inotifyUnregisterEvent__ = NULL,    // pure virtual
    .vtable.__inotifyGetOrAllocNotifShare__ = NULL,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__INotifier = {
    .numRelatives = 1,
    .relatives = {
        &__nvoc_metadata__INotifier.rtti,    // [0]: (inotify) this
    }
};

const struct NVOC_EXPORT_INFO __nvoc_export_info__INotifier = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct INotifier object.
void __nvoc_inotifyDestruct(INotifier*);
void __nvoc_dtor_INotifier(INotifier* pThis) {

// Call destructor.
    __nvoc_inotifyDestruct(pThis);
    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_INotifier(INotifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct INotifier object.
NV_STATUS __nvoc_ctor_INotifier(INotifier *pNotifier, struct CALL_CONTEXT *pCallContext) {
    NV_STATUS status = NV_OK;

    // Initialize data fields.
    __nvoc_init_dataField_INotifier(pNotifier);

    // Call the constructor for this class.
    status = __nvoc_inotifyConstruct(pNotifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_INotifier_fail__init;
    goto __nvoc_ctor_INotifier_exit; // Success

    // Unwind on error.
__nvoc_ctor_INotifier_fail__init:
__nvoc_ctor_INotifier_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_INotifier_1(INotifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_INotifier_1


// Initialize vtable(s) for 5 virtual method(s).
void __nvoc_init_funcTable_INotifier(INotifier *pThis) {
    __nvoc_init_funcTable_INotifier_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__INotifier(INotifier *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_INotifier = pThis;    // (inotify) this

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__INotifier;    // (inotify) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_INotifier(pThis);
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__a8683b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

// Forward declarations for Notifier
void __nvoc_init__INotifier(INotifier*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init_funcTable_Notifier(Notifier*);
NV_STATUS __nvoc_ctor_Notifier(Notifier*, struct CALL_CONTEXT *pCallContext);
void __nvoc_init_dataField_Notifier(Notifier*);
void __nvoc_dtor_Notifier(Notifier*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__Notifier;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__Notifier;

// Down-thunk(s) to bridge Notifier methods from ancestors (if any)
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // this
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // this
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

// Up-thunk(s) to bridge Notifier methods to ancestors (if any)

// Class-specific details for Notifier
// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier = 
{
    .classInfo.size =               sizeof(Notifier),
    .classInfo.classId =            classId(Notifier),
    .classInfo.providerId =         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
    .classInfo.name =               "Notifier",
#endif
    .objCreatefn =        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    .pCastInfo =          &__nvoc_castinfo__Notifier,
    .pExportInfo =        &__nvoc_export_info__Notifier
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__Notifier __nvoc_metadata__Notifier = {
    .rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Notifier,
    .rtti.offset    = 0,
    .metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super
    .metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__INotifier.rtti.offset    = NV_OFFSETOF(Notifier, __nvoc_base_INotifier),

    .vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__Notifier = {
    .numRelatives = 2,
    .relatives = {
        &__nvoc_metadata__Notifier.rtti,    // [0]: (notify) this
        &__nvoc_metadata__Notifier.metadata__INotifier.rtti,    // [1]: (inotify) super
    }
};

// 5 down-thunk(s) defined to bridge methods in Notifier from superclasses

// notifyGetNotificationListPtr: virtual override (inotify) base (inotify)
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) - NV_OFFSETOF(Notifier, __nvoc_base_INotifier)));
}

// notifyGetNotificationShare: virtual override (inotify) base (inotify)
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) - NV_OFFSETOF(Notifier, __nvoc_base_INotifier)));
}

// notifySetNotificationShare: virtual override (inotify) base (inotify)
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) - NV_OFFSETOF(Notifier, __nvoc_base_INotifier)), pNotifShare);
}

// notifyUnregisterEvent: virtual override (inotify) base (inotify)
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) - NV_OFFSETOF(Notifier, __nvoc_base_INotifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// notifyGetOrAllocNotifShare: virtual override (inotify) base (inotify)
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) - NV_OFFSETOF(Notifier, __nvoc_base_INotifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__Notifier = 
{
    .numEntries=     0,
    .pExportEntries= 0
};


// Destruct Notifier object.
void __nvoc_notifyDestruct(Notifier*);
void __nvoc_dtor_INotifier(INotifier*);
void __nvoc_dtor_Notifier(Notifier* pThis) {

// Call destructor.
    __nvoc_notifyDestruct(pThis);

// Recurse to superclass destructors.
    __nvoc_dtor_INotifier(&pThis->__nvoc_base_INotifier);

    PORT_UNREFERENCED_VARIABLE(pThis);
}
void __nvoc_init_dataField_Notifier(Notifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}


// Construct Notifier object.
NV_STATUS __nvoc_ctor_INotifier(INotifier *pNotifier, struct CALL_CONTEXT *pCallContext);
NV_STATUS __nvoc_ctor_Notifier(Notifier *pNotifier, struct CALL_CONTEXT *pCallContext) {    // inline
    NV_STATUS status = NV_OK;

    // Recurse to ancestor constructor(s).
    status = __nvoc_ctor_INotifier(&pNotifier->__nvoc_base_INotifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_Notifier_fail_INotifier;

    // Initialize data fields.
    __nvoc_init_dataField_Notifier(pNotifier);

    // Call the constructor for this class.
    status = __nvoc_notifyConstruct(pNotifier, pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_Notifier_fail__init;
    goto __nvoc_ctor_Notifier_exit; // Success

    // Unwind on error.
__nvoc_ctor_Notifier_fail__init:
    __nvoc_dtor_INotifier(&pNotifier->__nvoc_base_INotifier);
__nvoc_ctor_Notifier_fail_INotifier:
__nvoc_ctor_Notifier_exit:
    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Notifier_1(Notifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_Notifier_1


// Initialize vtable(s) for 5 virtual method(s).
void __nvoc_init_funcTable_Notifier(Notifier *pThis) {
    __nvoc_init_funcTable_Notifier_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__Notifier(Notifier *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_INotifier;    // (inotify) super
    pThis->__nvoc_pbase_Notifier = pThis;    // (notify) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__INotifier(&pThis->__nvoc_base_INotifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__Notifier.metadata__INotifier;    // (inotify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__Notifier;    // (notify) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_Notifier(pThis);
}

