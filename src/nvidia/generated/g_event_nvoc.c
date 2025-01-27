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

// Down-thunk(s) to bridge NotifShare methods from ancestors (if any)

// Up-thunk(s) to bridge NotifShare methods to ancestors (if any)

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

// Down-thunk(s) to bridge Event methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super

// Up-thunk(s) to bridge Event methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_eventAccessCallback(struct Event *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_eventShareCallback(struct Event *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventGetMemInterMapParams(struct Event *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventCheckMemInterUnmap(struct Event *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventGetMemoryMappingDescriptor(struct Event *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventControlSerialization_Prologue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_eventControlSerialization_Epilogue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_eventControl_Prologue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_eventControl_Epilogue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_eventCanCopy(struct Event *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventIsDuplicate(struct Event *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_eventPreDestruct(struct Event *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventControl(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventControlFilter(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventMap(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventUnmap(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_eventIsPartialUnmapSupported(struct Event *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventMapTo(struct Event *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_eventUnmapFrom(struct Event *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_eventGetRefCount(struct Event *pResource);    // this
void __nvoc_up_thunk_RsResource_eventAddAdditionalDependants(struct RsClient *pClient, struct Event *pResource, RsResourceRef *pReference);    // this

// 21 up-thunk(s) defined to bridge methods in Event to superclasses

// eventAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_eventAccessCallback(struct Event *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// eventShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_eventShareCallback(struct Event *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// eventGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventGetMemInterMapParams(struct Event *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource)), pParams);
}

// eventCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventCheckMemInterUnmap(struct Event *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// eventGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventGetMemoryMappingDescriptor(struct Event *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource)), ppMemDesc);
}

// eventControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventControlSerialization_Prologue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_eventControlSerialization_Epilogue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventControl_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_eventControl_Prologue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventControl_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_eventControl_Epilogue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource)), pCallContext, pParams);
}

// eventCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_eventCanCopy(struct Event *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventIsDuplicate(struct Event *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// eventPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_eventPreDestruct(struct Event *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventControl: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventControl(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// eventControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventControlFilter(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// eventMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventMap(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// eventUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventUnmap(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// eventIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_eventIsPartialUnmapSupported(struct Event *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventMapTo(struct Event *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// eventUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_eventUnmapFrom(struct Event *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// eventGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_eventGetRefCount(struct Event *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// eventAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_eventAddAdditionalDependants(struct RsClient *pClient, struct Event *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(Event, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
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
} // End __nvoc_init_funcTable_Event_1


// Initialize vtable(s) for 21 virtual method(s).
void __nvoc_init_funcTable_Event(Event *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__Event vtable = {
        .__eventAccessCallback__ = &__nvoc_up_thunk_RmResource_eventAccessCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
        .__eventShareCallback__ = &__nvoc_up_thunk_RmResource_eventShareCallback,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
        .__eventGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_eventGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
        .__eventCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_eventCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
        .__eventGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_eventGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
        .__eventControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_eventControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
        .__eventControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_eventControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
        .__eventControl_Prologue__ = &__nvoc_up_thunk_RmResource_eventControl_Prologue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
        .__eventControl_Epilogue__ = &__nvoc_up_thunk_RmResource_eventControl_Epilogue,    // virtual inherited (rmres) base (rmres)
        .RmResource.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
        .RmResource.RsResource.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
        .__eventCanCopy__ = &__nvoc_up_thunk_RsResource_eventCanCopy,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
        .__eventIsDuplicate__ = &__nvoc_up_thunk_RsResource_eventIsDuplicate,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
        .__eventPreDestruct__ = &__nvoc_up_thunk_RsResource_eventPreDestruct,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
        .__eventControl__ = &__nvoc_up_thunk_RsResource_eventControl,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControl__ = &resControl_IMPL,    // virtual
        .__eventControlFilter__ = &__nvoc_up_thunk_RsResource_eventControlFilter,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
        .__eventMap__ = &__nvoc_up_thunk_RsResource_eventMap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMap__ = &resMap_IMPL,    // virtual
        .__eventUnmap__ = &__nvoc_up_thunk_RsResource_eventUnmap,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmap__ = &resUnmap_IMPL,    // virtual
        .__eventIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_eventIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
        .RmResource.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
        .RmResource.RsResource.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
        .__eventMapTo__ = &__nvoc_up_thunk_RsResource_eventMapTo,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resMapTo__ = &resMapTo_IMPL,    // virtual
        .__eventUnmapFrom__ = &__nvoc_up_thunk_RsResource_eventUnmapFrom,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
        .__eventGetRefCount__ = &__nvoc_up_thunk_RsResource_eventGetRefCount,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
        .__eventAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_eventAddAdditionalDependants,    // virtual inherited (res) base (rmres)
        .RmResource.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
        .RmResource.RsResource.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_vtable = &vtable.RmResource.RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_vtable = &vtable.RmResource;    // (rmres) super
    pThis->__nvoc_vtable = &vtable;    // (event) this
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

// Down-thunk(s) to bridge INotifier methods from ancestors (if any)

// Up-thunk(s) to bridge INotifier methods to ancestors (if any)

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
} // End __nvoc_init_funcTable_INotifier_1


// Initialize vtable(s) for 5 virtual method(s).
void __nvoc_init_funcTable_INotifier(INotifier *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__INotifier vtable = {
        .__inotifyGetNotificationListPtr__ = NULL,    // pure virtual
        .__inotifySetNotificationShare__ = NULL,    // pure virtual
        .__inotifyGetNotificationShare__ = NULL,    // pure virtual
        .__inotifyUnregisterEvent__ = NULL,    // pure virtual
        .__inotifyGetOrAllocNotifShare__ = NULL,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_vtable = &vtable;    // (inotify) this
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

// Down-thunk(s) to bridge Notifier methods from ancestors (if any)
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // this
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // this
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

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


// Up-thunk(s) to bridge Notifier methods to ancestors (if any)

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
} // End __nvoc_init_funcTable_Notifier_1


// Initialize vtable(s) for 5 virtual method(s).
void __nvoc_init_funcTable_Notifier(Notifier *pThis) {

    // Per-class vtable definition
    static const struct NVOC_VTABLE__Notifier vtable = {
        .__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
        .INotifier.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
        .__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .INotifier.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
        .__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
        .INotifier.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
        .__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
        .INotifier.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
        .__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
        .INotifier.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
    };

    // Pointer(s) to per-class vtable(s)
    pThis->__nvoc_base_INotifier.__nvoc_vtable = &vtable.INotifier;    // (inotify) super
    pThis->__nvoc_vtable = &vtable;    // (notify) this
    __nvoc_init_funcTable_Notifier_1(pThis);
}

void __nvoc_init_INotifier(INotifier*);
void __nvoc_init_Notifier(Notifier *pThis) {
    pThis->__nvoc_pbase_Notifier = pThis;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_INotifier;
    __nvoc_init_INotifier(&pThis->__nvoc_base_INotifier);
    __nvoc_init_funcTable_Notifier(pThis);
}

