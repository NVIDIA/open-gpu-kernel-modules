#define NVOC_EVENT_H_PRIVATE_ACCESS_ALLOWED
#include "nvoc/runtime.h"
#include "nvoc/rtti.h"
#include "nvtypes.h"
#include "nvport/nvport.h"
#include "nvport/inline/util_valist.h"
#include "utils/nvassert.h"
#include "g_event_nvoc.h"

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xd5f150 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NotifShare;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsShared;

void __nvoc_init_NotifShare(NotifShare*);
void __nvoc_init_funcTable_NotifShare(NotifShare*);
NV_STATUS __nvoc_ctor_NotifShare(NotifShare*);
void __nvoc_init_dataField_NotifShare(NotifShare*);
void __nvoc_dtor_NotifShare(NotifShare*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_NotifShare;

static const struct NVOC_RTTI __nvoc_rtti_NotifShare_NotifShare = {
    /*pClassDef=*/          &__nvoc_class_def_NotifShare,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NotifShare,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_NotifShare_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NotifShare, __nvoc_base_RsShared.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_NotifShare_RsShared = {
    /*pClassDef=*/          &__nvoc_class_def_RsShared,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(NotifShare, __nvoc_base_RsShared),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_NotifShare = {
    /*numRelatives=*/       3,
    /*relatives=*/ {
        &__nvoc_rtti_NotifShare_NotifShare,
        &__nvoc_rtti_NotifShare_RsShared,
        &__nvoc_rtti_NotifShare_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_NotifShare = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NotifShare),
        /*classId=*/            classId(NotifShare),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NotifShare",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NotifShare,
    /*pCastInfo=*/          &__nvoc_castinfo_NotifShare,
    /*pExportInfo=*/        &__nvoc_export_info_NotifShare
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_NotifShare = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RsShared(RsShared*);
void __nvoc_dtor_NotifShare(NotifShare *pThis) {
    __nvoc_shrnotifDestruct(pThis);
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NotifShare(NotifShare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RsShared(RsShared* );
NV_STATUS __nvoc_ctor_NotifShare(NotifShare *pThis) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RsShared(&pThis->__nvoc_base_RsShared);
    if (status != NV_OK) goto __nvoc_ctor_NotifShare_fail_RsShared;
    __nvoc_init_dataField_NotifShare(pThis);

    status = __nvoc_shrnotifConstruct(pThis);
    if (status != NV_OK) goto __nvoc_ctor_NotifShare_fail__init;
    goto __nvoc_ctor_NotifShare_exit; // Success

__nvoc_ctor_NotifShare_fail__init:
    __nvoc_dtor_RsShared(&pThis->__nvoc_base_RsShared);
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

void __nvoc_init_RsShared(RsShared*);
void __nvoc_init_NotifShare(NotifShare *pThis) {
    pThis->__nvoc_pbase_NotifShare = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RsShared.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsShared = &pThis->__nvoc_base_RsShared;
    __nvoc_init_RsShared(&pThis->__nvoc_base_RsShared);
    __nvoc_init_funcTable_NotifShare(pThis);
}

NV_STATUS __nvoc_objCreate_NotifShare(NotifShare **ppThis, Dynamic *pParent, NvU32 createFlags)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    NotifShare *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NotifShare), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(NotifShare));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NotifShare);

    pThis->__nvoc_base_RsShared.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RsShared.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init_NotifShare(pThis);
    status = __nvoc_ctor_NotifShare(pThis);
    if (status != NV_OK) goto __nvoc_objCreate_NotifShare_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NotifShare_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RsShared.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NotifShare));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NotifShare(NotifShare **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;

    status = __nvoc_objCreate_NotifShare(ppThis, pParent, createFlags);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xa4ecfc = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Event;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;

void __nvoc_init_Event(Event*);
void __nvoc_init_funcTable_Event(Event*);
NV_STATUS __nvoc_ctor_Event(Event*, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams);
void __nvoc_init_dataField_Event(Event*);
void __nvoc_dtor_Event(Event*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Event;

static const struct NVOC_RTTI __nvoc_rtti_Event_Event = {
    /*pClassDef=*/          &__nvoc_class_def_Event,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Event,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Event_Object = {
    /*pClassDef=*/          &__nvoc_class_def_Object,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
};

static const struct NVOC_RTTI __nvoc_rtti_Event_RsResource = {
    /*pClassDef=*/          &__nvoc_class_def_RsResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource),
};

static const struct NVOC_RTTI __nvoc_rtti_Event_RmResourceCommon = {
    /*pClassDef=*/          &__nvoc_class_def_RmResourceCommon,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
};

static const struct NVOC_RTTI __nvoc_rtti_Event_RmResource = {
    /*pClassDef=*/          &__nvoc_class_def_RmResource,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Event, __nvoc_base_RmResource),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Event = {
    /*numRelatives=*/       5,
    /*relatives=*/ {
        &__nvoc_rtti_Event_Event,
        &__nvoc_rtti_Event_RmResource,
        &__nvoc_rtti_Event_RmResourceCommon,
        &__nvoc_rtti_Event_RsResource,
        &__nvoc_rtti_Event_Object,
    },
};

const struct NVOC_CLASS_DEF __nvoc_class_def_Event = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Event),
        /*classId=*/            classId(Event),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Event",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_Event,
    /*pCastInfo=*/          &__nvoc_castinfo_Event,
    /*pExportInfo=*/        &__nvoc_export_info_Event
};

// 21 up-thunk(s) defined to bridge methods in Event to superclasses

// eventAccessCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_eventAccessCallback(struct Event *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
}

// eventShareCallback: virtual inherited (rmres) base (rmres)
static NvBool __nvoc_up_thunk_RmResource_eventShareCallback(struct Event *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

// eventGetMemInterMapParams: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_eventGetMemInterMapParams(struct Event *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_Event_RmResource.offset), pParams);
}

// eventCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_eventCheckMemInterUnmap(struct Event *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_Event_RmResource.offset), bSubdeviceHandleProvided);
}

// eventGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_eventGetMemoryMappingDescriptor(struct Event *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + __nvoc_rtti_Event_RmResource.offset), ppMemDesc);
}

// eventControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_eventControlSerialization_Prologue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RmResource.offset), pCallContext, pParams);
}

// eventControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_eventControlSerialization_Epilogue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RmResource.offset), pCallContext, pParams);
}

// eventControl_Prologue: virtual inherited (rmres) base (rmres)
static NV_STATUS __nvoc_up_thunk_RmResource_eventControl_Prologue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RmResource.offset), pCallContext, pParams);
}

// eventControl_Epilogue: virtual inherited (rmres) base (rmres)
static void __nvoc_up_thunk_RmResource_eventControl_Epilogue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RmResource.offset), pCallContext, pParams);
}

// eventCanCopy: virtual inherited (res) base (rmres)
static NvBool __nvoc_up_thunk_RsResource_eventCanCopy(struct Event *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset));
}

// eventIsDuplicate: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_eventIsDuplicate(struct Event *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset), hMemory, pDuplicate);
}

// eventPreDestruct: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_eventPreDestruct(struct Event *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset));
}

// eventControl: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_eventControl(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset), pCallContext, pParams);
}

// eventControlFilter: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_eventControlFilter(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset), pCallContext, pParams);
}

// eventMap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_eventMap(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

// eventUnmap: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_eventUnmap(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset), pCallContext, pCpuMapping);
}

// eventIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
static NvBool __nvoc_up_thunk_RsResource_eventIsPartialUnmapSupported(struct Event *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset));
}

// eventMapTo: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_eventMapTo(struct Event *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset), pParams);
}

// eventUnmapFrom: virtual inherited (res) base (rmres)
static NV_STATUS __nvoc_up_thunk_RsResource_eventUnmapFrom(struct Event *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset), pParams);
}

// eventGetRefCount: virtual inherited (res) base (rmres)
static NvU32 __nvoc_up_thunk_RsResource_eventGetRefCount(struct Event *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset));
}

// eventAddAdditionalDependants: virtual inherited (res) base (rmres)
static void __nvoc_up_thunk_RsResource_eventAddAdditionalDependants(struct RsClient *pClient, struct Event *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + __nvoc_rtti_Event_RsResource.offset), pReference);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_Event = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_Event(Event *pThis) {
    __nvoc_eventDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Event(Event *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Event(Event *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Event_fail_RmResource;
    __nvoc_init_dataField_Event(pThis);

    status = __nvoc_eventConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_Event_fail__init;
    goto __nvoc_ctor_Event_exit; // Success

__nvoc_ctor_Event_fail__init:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_Event_fail_RmResource:
__nvoc_ctor_Event_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Event_1(Event *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // eventAccessCallback -- virtual inherited (rmres) base (rmres)
    pThis->__eventAccessCallback__ = &__nvoc_up_thunk_RmResource_eventAccessCallback;

    // eventShareCallback -- virtual inherited (rmres) base (rmres)
    pThis->__eventShareCallback__ = &__nvoc_up_thunk_RmResource_eventShareCallback;

    // eventGetMemInterMapParams -- virtual inherited (rmres) base (rmres)
    pThis->__eventGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_eventGetMemInterMapParams;

    // eventCheckMemInterUnmap -- virtual inherited (rmres) base (rmres)
    pThis->__eventCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_eventCheckMemInterUnmap;

    // eventGetMemoryMappingDescriptor -- virtual inherited (rmres) base (rmres)
    pThis->__eventGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_eventGetMemoryMappingDescriptor;

    // eventControlSerialization_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__eventControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_eventControlSerialization_Prologue;

    // eventControlSerialization_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__eventControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_eventControlSerialization_Epilogue;

    // eventControl_Prologue -- virtual inherited (rmres) base (rmres)
    pThis->__eventControl_Prologue__ = &__nvoc_up_thunk_RmResource_eventControl_Prologue;

    // eventControl_Epilogue -- virtual inherited (rmres) base (rmres)
    pThis->__eventControl_Epilogue__ = &__nvoc_up_thunk_RmResource_eventControl_Epilogue;

    // eventCanCopy -- virtual inherited (res) base (rmres)
    pThis->__eventCanCopy__ = &__nvoc_up_thunk_RsResource_eventCanCopy;

    // eventIsDuplicate -- virtual inherited (res) base (rmres)
    pThis->__eventIsDuplicate__ = &__nvoc_up_thunk_RsResource_eventIsDuplicate;

    // eventPreDestruct -- virtual inherited (res) base (rmres)
    pThis->__eventPreDestruct__ = &__nvoc_up_thunk_RsResource_eventPreDestruct;

    // eventControl -- virtual inherited (res) base (rmres)
    pThis->__eventControl__ = &__nvoc_up_thunk_RsResource_eventControl;

    // eventControlFilter -- virtual inherited (res) base (rmres)
    pThis->__eventControlFilter__ = &__nvoc_up_thunk_RsResource_eventControlFilter;

    // eventMap -- virtual inherited (res) base (rmres)
    pThis->__eventMap__ = &__nvoc_up_thunk_RsResource_eventMap;

    // eventUnmap -- virtual inherited (res) base (rmres)
    pThis->__eventUnmap__ = &__nvoc_up_thunk_RsResource_eventUnmap;

    // eventIsPartialUnmapSupported -- inline virtual inherited (res) base (rmres) body
    pThis->__eventIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_eventIsPartialUnmapSupported;

    // eventMapTo -- virtual inherited (res) base (rmres)
    pThis->__eventMapTo__ = &__nvoc_up_thunk_RsResource_eventMapTo;

    // eventUnmapFrom -- virtual inherited (res) base (rmres)
    pThis->__eventUnmapFrom__ = &__nvoc_up_thunk_RsResource_eventUnmapFrom;

    // eventGetRefCount -- virtual inherited (res) base (rmres)
    pThis->__eventGetRefCount__ = &__nvoc_up_thunk_RsResource_eventGetRefCount;

    // eventAddAdditionalDependants -- virtual inherited (res) base (rmres)
    pThis->__eventAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_eventAddAdditionalDependants;
} // End __nvoc_init_funcTable_Event_1 with approximately 21 basic block(s).


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_Event(Event *pThis) {

    // Initialize vtable(s) with 21 per-object function pointer(s).
    __nvoc_init_funcTable_Event_1(pThis);
}

void __nvoc_init_RmResource(RmResource*);
void __nvoc_init_Event(Event *pThis) {
    pThis->__nvoc_pbase_Event = pThis;
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;
    __nvoc_init_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init_funcTable_Event(pThis);
}

NV_STATUS __nvoc_objCreate_Event(Event **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    Event *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(Event), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(Event));

    // Initialize runtime type information.
    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Event);

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

    __nvoc_init_Event(pThis);
    status = __nvoc_ctor_Event(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_Event_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_Event_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(Event));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_Event(Event **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_Event(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xf8f965 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

void __nvoc_init_INotifier(INotifier*);
void __nvoc_init_funcTable_INotifier(INotifier*);
NV_STATUS __nvoc_ctor_INotifier(INotifier*, struct CALL_CONTEXT * arg_pCallContext);
void __nvoc_init_dataField_INotifier(INotifier*);
void __nvoc_dtor_INotifier(INotifier*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_INotifier;

static const struct NVOC_RTTI __nvoc_rtti_INotifier_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_INotifier,
    /*offset=*/             0,
};

static const struct NVOC_CASTINFO __nvoc_castinfo_INotifier = {
    /*numRelatives=*/       1,
    /*relatives=*/ {
        &__nvoc_rtti_INotifier_INotifier,
    },
};

// Not instantiable because it's not derived from class "Object"
// Not instantiable because it's an abstract class with following pure virtual functions:
//  inotifyGetNotificationListPtr
//  inotifySetNotificationShare
//  inotifyGetNotificationShare
//  inotifyUnregisterEvent
//  inotifyGetOrAllocNotifShare
const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(INotifier),
        /*classId=*/            classId(INotifier),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "INotifier",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo_INotifier,
    /*pExportInfo=*/        &__nvoc_export_info_INotifier
};

const struct NVOC_EXPORT_INFO __nvoc_export_info_INotifier = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_INotifier(INotifier *pThis) {
    __nvoc_inotifyDestruct(pThis);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_INotifier(INotifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_INotifier(INotifier *pThis, struct CALL_CONTEXT * arg_pCallContext) {
    NV_STATUS status = NV_OK;
    __nvoc_init_dataField_INotifier(pThis);

    status = __nvoc_inotifyConstruct(pThis, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_INotifier_fail__init;
    goto __nvoc_ctor_INotifier_exit; // Success

__nvoc_ctor_INotifier_fail__init:
__nvoc_ctor_INotifier_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_INotifier_1(INotifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // inotifyGetNotificationListPtr -- pure virtual
    pThis->__inotifyGetNotificationListPtr__ = NULL;

    // inotifySetNotificationShare -- pure virtual
    pThis->__inotifySetNotificationShare__ = NULL;

    // inotifyGetNotificationShare -- pure virtual
    pThis->__inotifyGetNotificationShare__ = NULL;

    // inotifyUnregisterEvent -- pure virtual
    pThis->__inotifyUnregisterEvent__ = NULL;

    // inotifyGetOrAllocNotifShare -- pure virtual
    pThis->__inotifyGetOrAllocNotifShare__ = NULL;
} // End __nvoc_init_funcTable_INotifier_1 with approximately 5 basic block(s).


// Initialize vtable(s) for 5 virtual method(s).
void __nvoc_init_funcTable_INotifier(INotifier *pThis) {

    // Initialize vtable(s) with 5 per-object function pointer(s).
    __nvoc_init_funcTable_INotifier_1(pThis);
}

void __nvoc_init_INotifier(INotifier *pThis) {
    pThis->__nvoc_pbase_INotifier = pThis;
    __nvoc_init_funcTable_INotifier(pThis);
}

#ifdef DEBUG
char __nvoc_class_id_uniqueness_check_0xa8683b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;

void __nvoc_init_Notifier(Notifier*);
void __nvoc_init_funcTable_Notifier(Notifier*);
NV_STATUS __nvoc_ctor_Notifier(Notifier*, struct CALL_CONTEXT * arg_pCallContext);
void __nvoc_init_dataField_Notifier(Notifier*);
void __nvoc_dtor_Notifier(Notifier*);
extern const struct NVOC_EXPORT_INFO __nvoc_export_info_Notifier;

static const struct NVOC_RTTI __nvoc_rtti_Notifier_Notifier = {
    /*pClassDef=*/          &__nvoc_class_def_Notifier,
    /*dtor=*/               (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_Notifier,
    /*offset=*/             0,
};

static const struct NVOC_RTTI __nvoc_rtti_Notifier_INotifier = {
    /*pClassDef=*/          &__nvoc_class_def_INotifier,
    /*dtor=*/               &__nvoc_destructFromBase,
    /*offset=*/             NV_OFFSETOF(Notifier, __nvoc_base_INotifier),
};

static const struct NVOC_CASTINFO __nvoc_castinfo_Notifier = {
    /*numRelatives=*/       2,
    /*relatives=*/ {
        &__nvoc_rtti_Notifier_Notifier,
        &__nvoc_rtti_Notifier_INotifier,
    },
};

// Not instantiable because it's not derived from class "Object"
const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(Notifier),
        /*classId=*/            classId(Notifier),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "Notifier",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) NULL,
    /*pCastInfo=*/          &__nvoc_castinfo_Notifier,
    /*pExportInfo=*/        &__nvoc_export_info_Notifier
};

// 5 down-thunk(s) defined to bridge methods in Notifier from superclasses

// notifyGetNotificationListPtr: virtual override (inotify) base (inotify)
static PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) - __nvoc_rtti_Notifier_INotifier.offset));
}

// notifyGetNotificationShare: virtual override (inotify) base (inotify)
static struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) - __nvoc_rtti_Notifier_INotifier.offset));
}

// notifySetNotificationShare: virtual override (inotify) base (inotify)
static void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) - __nvoc_rtti_Notifier_INotifier.offset), pNotifShare);
}

// notifyUnregisterEvent: virtual override (inotify) base (inotify)
static NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) - __nvoc_rtti_Notifier_INotifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// notifyGetOrAllocNotifShare: virtual override (inotify) base (inotify)
static NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) - __nvoc_rtti_Notifier_INotifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info_Notifier = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_INotifier(INotifier*);
void __nvoc_dtor_Notifier(Notifier *pThis) {
    __nvoc_notifyDestruct(pThis);
    __nvoc_dtor_INotifier(&pThis->__nvoc_base_INotifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_Notifier(Notifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_INotifier(INotifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_Notifier(Notifier *pThis, struct CALL_CONTEXT * arg_pCallContext) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_INotifier(&pThis->__nvoc_base_INotifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_Notifier_fail_INotifier;
    __nvoc_init_dataField_Notifier(pThis);

    status = __nvoc_notifyConstruct(pThis, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_Notifier_fail__init;
    goto __nvoc_ctor_Notifier_exit; // Success

__nvoc_ctor_Notifier_fail__init:
    __nvoc_dtor_INotifier(&pThis->__nvoc_base_INotifier);
__nvoc_ctor_Notifier_fail_INotifier:
__nvoc_ctor_Notifier_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_Notifier_1(Notifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // notifyGetNotificationListPtr -- virtual override (inotify) base (inotify)
    pThis->__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL;
    pThis->__nvoc_base_INotifier.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr;

    // notifyGetNotificationShare -- virtual override (inotify) base (inotify)
    pThis->__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL;
    pThis->__nvoc_base_INotifier.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare;

    // notifySetNotificationShare -- virtual override (inotify) base (inotify)
    pThis->__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL;
    pThis->__nvoc_base_INotifier.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare;

    // notifyUnregisterEvent -- virtual override (inotify) base (inotify)
    pThis->__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL;
    pThis->__nvoc_base_INotifier.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent;

    // notifyGetOrAllocNotifShare -- virtual override (inotify) base (inotify)
    pThis->__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL;
    pThis->__nvoc_base_INotifier.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare;
} // End __nvoc_init_funcTable_Notifier_1 with approximately 10 basic block(s).


// Initialize vtable(s) for 5 virtual method(s).
void __nvoc_init_funcTable_Notifier(Notifier *pThis) {

    // Initialize vtable(s) with 5 per-object function pointer(s).
    __nvoc_init_funcTable_Notifier_1(pThis);
}

void __nvoc_init_INotifier(INotifier*);
void __nvoc_init_Notifier(Notifier *pThis) {
    pThis->__nvoc_pbase_Notifier = pThis;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_INotifier;
    __nvoc_init_INotifier(&pThis->__nvoc_base_INotifier);
    __nvoc_init_funcTable_Notifier(pThis);
}

