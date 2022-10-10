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

static void __nvoc_init_funcTable_NotifShare_1(NotifShare *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

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

NV_STATUS __nvoc_objCreate_NotifShare(NotifShare **ppThis, Dynamic *pParent, NvU32 createFlags) {
    NV_STATUS status;
    Object *pParentObj;
    NotifShare *pThis;

    pThis = portMemAllocNonPaged(sizeof(NotifShare));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(NotifShare));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_NotifShare);

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

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_NotifShare_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
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

static NvBool __nvoc_thunk_RmResource_eventShareCallback(struct Event *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RmResource.offset), pInvokingClient, pParentRef, pSharePolicy);
}

static NV_STATUS __nvoc_thunk_RmResource_eventCheckMemInterUnmap(struct Event *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Event_RmResource.offset), bSubdeviceHandleProvided);
}

static NV_STATUS __nvoc_thunk_RsResource_eventControl(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControl((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_eventGetMemInterMapParams(struct Event *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Event_RmResource.offset), pParams);
}

static NV_STATUS __nvoc_thunk_RmResource_eventGetMemoryMappingDescriptor(struct Event *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *)pRmResource) + __nvoc_rtti_Event_RmResource.offset), ppMemDesc);
}

static NvU32 __nvoc_thunk_RsResource_eventGetRefCount(struct Event *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_eventControlFilter(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset), pCallContext, pParams);
}

static void __nvoc_thunk_RsResource_eventAddAdditionalDependants(struct RsClient *pClient, struct Event *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset), pReference);
}

static NV_STATUS __nvoc_thunk_RsResource_eventUnmap(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset), pCallContext, pCpuMapping);
}

static NV_STATUS __nvoc_thunk_RmResource_eventControl_Prologue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RmResource.offset), pCallContext, pParams);
}

static NvBool __nvoc_thunk_RsResource_eventCanCopy(struct Event *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_eventMapTo(struct Event *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset), pParams);
}

static void __nvoc_thunk_RsResource_eventPreDestruct(struct Event *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset));
}

static NV_STATUS __nvoc_thunk_RsResource_eventUnmapFrom(struct Event *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset), pParams);
}

static void __nvoc_thunk_RmResource_eventControl_Epilogue(struct Event *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RmResource.offset), pCallContext, pParams);
}

static NV_STATUS __nvoc_thunk_RsResource_eventControlLookup(struct Event *pResource, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams, const struct NVOC_EXPORTED_METHOD_DEF **ppEntry) {
    return resControlLookup((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset), pParams, ppEntry);
}

static NV_STATUS __nvoc_thunk_RsResource_eventMap(struct Event *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RsResource.offset), pCallContext, pParams, pCpuMapping);
}

static NvBool __nvoc_thunk_RmResource_eventAccessCallback(struct Event *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *)pResource) + __nvoc_rtti_Event_RmResource.offset), pInvokingClient, pAllocParams, accessRight);
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

static void __nvoc_init_funcTable_Event_1(Event *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__eventShareCallback__ = &__nvoc_thunk_RmResource_eventShareCallback;

    pThis->__eventCheckMemInterUnmap__ = &__nvoc_thunk_RmResource_eventCheckMemInterUnmap;

    pThis->__eventControl__ = &__nvoc_thunk_RsResource_eventControl;

    pThis->__eventGetMemInterMapParams__ = &__nvoc_thunk_RmResource_eventGetMemInterMapParams;

    pThis->__eventGetMemoryMappingDescriptor__ = &__nvoc_thunk_RmResource_eventGetMemoryMappingDescriptor;

    pThis->__eventGetRefCount__ = &__nvoc_thunk_RsResource_eventGetRefCount;

    pThis->__eventControlFilter__ = &__nvoc_thunk_RsResource_eventControlFilter;

    pThis->__eventAddAdditionalDependants__ = &__nvoc_thunk_RsResource_eventAddAdditionalDependants;

    pThis->__eventUnmap__ = &__nvoc_thunk_RsResource_eventUnmap;

    pThis->__eventControl_Prologue__ = &__nvoc_thunk_RmResource_eventControl_Prologue;

    pThis->__eventCanCopy__ = &__nvoc_thunk_RsResource_eventCanCopy;

    pThis->__eventMapTo__ = &__nvoc_thunk_RsResource_eventMapTo;

    pThis->__eventPreDestruct__ = &__nvoc_thunk_RsResource_eventPreDestruct;

    pThis->__eventUnmapFrom__ = &__nvoc_thunk_RsResource_eventUnmapFrom;

    pThis->__eventControl_Epilogue__ = &__nvoc_thunk_RmResource_eventControl_Epilogue;

    pThis->__eventControlLookup__ = &__nvoc_thunk_RsResource_eventControlLookup;

    pThis->__eventMap__ = &__nvoc_thunk_RsResource_eventMap;

    pThis->__eventAccessCallback__ = &__nvoc_thunk_RmResource_eventAccessCallback;
}

void __nvoc_init_funcTable_Event(Event *pThis) {
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

NV_STATUS __nvoc_objCreate_Event(Event **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status;
    Object *pParentObj;
    Event *pThis;

    pThis = portMemAllocNonPaged(sizeof(Event));
    if (pThis == NULL) return NV_ERR_NO_MEMORY;

    portMemSet(pThis, 0, sizeof(Event));

    __nvoc_initRtti(staticCast(pThis, Dynamic), &__nvoc_class_def_Event);

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

    *ppThis = pThis;
    return NV_OK;

__nvoc_objCreate_Event_cleanup:
    // do not call destructors here since the constructor already called them
    portMemFree(pThis);
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

static void __nvoc_init_funcTable_INotifier_1(INotifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__inotifyGetNotificationListPtr__ = NULL;

    pThis->__inotifySetNotificationShare__ = NULL;

    pThis->__inotifyGetNotificationShare__ = NULL;

    pThis->__inotifyUnregisterEvent__ = NULL;

    pThis->__inotifyGetOrAllocNotifShare__ = NULL;
}

void __nvoc_init_funcTable_INotifier(INotifier *pThis) {
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

static PEVENTNOTIFICATION *__nvoc_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *)pNotifier) - __nvoc_rtti_Notifier_INotifier.offset));
}

static struct NotifShare *__nvoc_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) - __nvoc_rtti_Notifier_INotifier.offset));
}

static void __nvoc_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *)pNotifier) - __nvoc_rtti_Notifier_INotifier.offset), pNotifShare);
}

static NV_STATUS __nvoc_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *)pNotifier) - __nvoc_rtti_Notifier_INotifier.offset), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

static NV_STATUS __nvoc_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *)pNotifier) - __nvoc_rtti_Notifier_INotifier.offset), hNotifierClient, hNotifierResource, ppNotifShare);
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

static void __nvoc_init_funcTable_Notifier_1(Notifier *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    pThis->__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL;

    pThis->__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL;

    pThis->__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL;

    pThis->__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL;

    pThis->__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL;

    pThis->__nvoc_base_INotifier.__inotifyGetNotificationListPtr__ = &__nvoc_thunk_Notifier_inotifyGetNotificationListPtr;

    pThis->__nvoc_base_INotifier.__inotifyGetNotificationShare__ = &__nvoc_thunk_Notifier_inotifyGetNotificationShare;

    pThis->__nvoc_base_INotifier.__inotifySetNotificationShare__ = &__nvoc_thunk_Notifier_inotifySetNotificationShare;

    pThis->__nvoc_base_INotifier.__inotifyUnregisterEvent__ = &__nvoc_thunk_Notifier_inotifyUnregisterEvent;

    pThis->__nvoc_base_INotifier.__inotifyGetOrAllocNotifShare__ = &__nvoc_thunk_Notifier_inotifyGetOrAllocNotifShare;
}

void __nvoc_init_funcTable_Notifier(Notifier *pThis) {
    __nvoc_init_funcTable_Notifier_1(pThis);
}

void __nvoc_init_INotifier(INotifier*);
void __nvoc_init_Notifier(Notifier *pThis) {
    pThis->__nvoc_pbase_Notifier = pThis;
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_INotifier;
    __nvoc_init_INotifier(&pThis->__nvoc_base_INotifier);
    __nvoc_init_funcTable_Notifier(pThis);
}

