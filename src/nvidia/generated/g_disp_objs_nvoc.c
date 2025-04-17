#define NVOC_DISP_OBJS_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_disp_objs_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xe9980c = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for DisplayApi
void __nvoc_init__RmResource(RmResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__DisplayApi(DisplayApi*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_DisplayApi(DisplayApi*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_DisplayApi(DisplayApi*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_DisplayApi(DisplayApi*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DisplayApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DisplayApi;

// Down-thunk(s) to bridge DisplayApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_down_thunk_DisplayApi_resControl(struct RsResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_down_thunk_DisplayApi_rmresControl_Prologue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this
void __nvoc_down_thunk_DisplayApi_rmresControl_Epilogue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this

// Up-thunk(s) to bridge DisplayApi methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_dispapiAccessCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_dispapiShareCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams(struct DisplayApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap(struct DisplayApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor(struct DisplayApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispapiCanCopy(struct DisplayApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispapiIsDuplicate(struct DisplayApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispapiPreDestruct(struct DisplayApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispapiControlFilter(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported(struct DisplayApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMapTo(struct DisplayApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmapFrom(struct DisplayApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispapiGetRefCount(struct DisplayApi *pResource);    // this
void __nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants(struct RsClient *pClient, struct DisplayApi *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr(struct DisplayApi *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispapiGetNotificationShare(struct DisplayApi *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispapiSetNotificationShare(struct DisplayApi *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispapiUnregisterEvent(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DisplayApi),
        /*classId=*/            classId(DisplayApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DisplayApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DisplayApi,
    /*pCastInfo=*/          &__nvoc_castinfo__DisplayApi,
    /*pExportInfo=*/        &__nvoc_export_info__DisplayApi
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__DisplayApi __nvoc_metadata__DisplayApi = {
    .rtti.pClassDef = &__nvoc_class_def_DisplayApi,    // (dispapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DisplayApi,
    .rtti.offset    = 0,
    .metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super
    .metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.rtti.offset    = NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource),
    .metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^2
    .metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^3
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^2
    .metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(DisplayApi, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(DisplayApi, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__dispapiControl__ = &dispapiControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_DisplayApi_resControl,    // virtual
    .vtable.__dispapiControl_Prologue__ = &dispapiControl_Prologue_IMPL,    // virtual override (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Prologue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Prologue,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__dispapiControl_Epilogue__ = &dispapiControl_Epilogue_IMPL,    // virtual override (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControl_Epilogue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Epilogue,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__dispapiAccessCallback__ = &__nvoc_up_thunk_RmResource_dispapiAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__dispapiShareCallback__ = &__nvoc_up_thunk_RmResource_dispapiShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__dispapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__dispapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__dispapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__dispapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__dispapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__dispapiCanCopy__ = &__nvoc_up_thunk_RsResource_dispapiCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__dispapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispapiIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__dispapiPreDestruct__ = &__nvoc_up_thunk_RsResource_dispapiPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__dispapiControlFilter__ = &__nvoc_up_thunk_RsResource_dispapiControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__dispapiMap__ = &__nvoc_up_thunk_RsResource_dispapiMap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__dispapiUnmap__ = &__nvoc_up_thunk_RsResource_dispapiUnmap,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__dispapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__dispapiMapTo__ = &__nvoc_up_thunk_RsResource_dispapiMapTo,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__dispapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispapiUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__dispapiGetRefCount__ = &__nvoc_up_thunk_RsResource_dispapiGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__dispapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__dispapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__dispapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__dispapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__dispapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispapiUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__dispapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DisplayApi = {
    .numRelatives = 7,
    .relatives = {
        &__nvoc_metadata__DisplayApi.rtti,    // [0]: (dispapi) this
        &__nvoc_metadata__DisplayApi.metadata__RmResource.rtti,    // [1]: (rmres) super
        &__nvoc_metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti,    // [2]: (res) super^2
        &__nvoc_metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [3]: (obj) super^3
        &__nvoc_metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [4]: (rmrescmn) super^2
        &__nvoc_metadata__DisplayApi.metadata__Notifier.rtti,    // [5]: (notify) super
        &__nvoc_metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti,    // [6]: (inotify) super^2
    }
};

// 3 down-thunk(s) defined to bridge methods in DisplayApi from superclasses

// dispapiControl: virtual override (res) base (rmres)
NV_STATUS __nvoc_down_thunk_DisplayApi_resControl(struct RsResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *) pDisplayApi) - NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispapiControl_Prologue: virtual override (res) base (rmres)
NV_STATUS __nvoc_down_thunk_DisplayApi_rmresControl_Prologue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *) pDisplayApi) - NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource)), pCallContext, pRsParams);
}

// dispapiControl_Epilogue: virtual override (res) base (rmres)
void __nvoc_down_thunk_DisplayApi_rmresControl_Epilogue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *) pDisplayApi) - NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource)), pCallContext, pRsParams);
}


// 23 up-thunk(s) defined to bridge methods in DisplayApi to superclasses

// dispapiAccessCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_dispapiAccessCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispapiShareCallback: virtual inherited (rmres) base (rmres)
NvBool __nvoc_up_thunk_RmResource_dispapiShareCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispapiGetMemInterMapParams: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams(struct DisplayApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource)), pParams);
}

// dispapiCheckMemInterUnmap: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap(struct DisplayApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor(struct DisplayApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource)), ppMemDesc);
}

// dispapiControlSerialization_Prologue: virtual inherited (rmres) base (rmres)
NV_STATUS __nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// dispapiControlSerialization_Epilogue: virtual inherited (rmres) base (rmres)
void __nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource)), pCallContext, pParams);
}

// dispapiCanCopy: virtual inherited (res) base (rmres)
NvBool __nvoc_up_thunk_RsResource_dispapiCanCopy(struct DisplayApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispapiIsDuplicate: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_dispapiIsDuplicate(struct DisplayApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispapiPreDestruct: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_dispapiPreDestruct(struct DisplayApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispapiControlFilter: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_dispapiControlFilter(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispapiMap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// dispapiUnmap: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// dispapiIsPartialUnmapSupported: inline virtual inherited (res) base (rmres) body
NvBool __nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported(struct DisplayApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispapiMapTo: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMapTo(struct DisplayApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispapiUnmapFrom: virtual inherited (res) base (rmres)
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmapFrom(struct DisplayApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispapiGetRefCount: virtual inherited (res) base (rmres)
NvU32 __nvoc_up_thunk_RsResource_dispapiGetRefCount(struct DisplayApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispapiAddAdditionalDependants: virtual inherited (res) base (rmres)
void __nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants(struct RsClient *pClient, struct DisplayApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DisplayApi, __nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispapiGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr(struct DisplayApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DisplayApi, __nvoc_base_Notifier)));
}

// dispapiGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_dispapiGetNotificationShare(struct DisplayApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DisplayApi, __nvoc_base_Notifier)));
}

// dispapiSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_dispapiSetNotificationShare(struct DisplayApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DisplayApi, __nvoc_base_Notifier)), pNotifShare);
}

// dispapiUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_dispapiUnregisterEvent(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DisplayApi, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispapiGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DisplayApi, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DisplayApi = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_RmResource(RmResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_DisplayApi(DisplayApi *pThis) {
    __nvoc_dispapiDestruct(pThis);
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DisplayApi(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_RmResource(RmResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_RmResource(&pThis->__nvoc_base_RmResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DisplayApi_fail_RmResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_DisplayApi_fail_Notifier;
    __nvoc_init_dataField_DisplayApi(pThis, pRmhalspecowner);

    status = __nvoc_dispapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DisplayApi_fail__init;
    goto __nvoc_ctor_DisplayApi_exit; // Success

__nvoc_ctor_DisplayApi_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_DisplayApi_fail_Notifier:
    __nvoc_dtor_RmResource(&pThis->__nvoc_base_RmResource);
__nvoc_ctor_DisplayApi_fail_RmResource:
__nvoc_ctor_DisplayApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DisplayApi_1(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
} // End __nvoc_init_funcTable_DisplayApi_1


// Initialize vtable(s) for 26 virtual method(s).
void __nvoc_init_funcTable_DisplayApi(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner) {
    __nvoc_init_funcTable_DisplayApi_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__DisplayApi(DisplayApi *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^3
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^2
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_RmResource;    // (rmres) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_DisplayApi = pThis;    // (dispapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__RmResource(&pThis->__nvoc_base_RmResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^3
    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__DisplayApi.metadata__RmResource.metadata__RsResource;    // (res) super^2
    pThis->__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^2
    pThis->__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__DisplayApi.metadata__RmResource;    // (rmres) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__DisplayApi.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__DisplayApi.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DisplayApi;    // (dispapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DisplayApi(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_DisplayApi(DisplayApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DisplayApi *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DisplayApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DisplayApi));

    pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__DisplayApi(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DisplayApi(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DisplayApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DisplayApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DisplayApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DisplayApi(DisplayApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DisplayApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x999839 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispObject;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;

// Forward declarations for DispObject
void __nvoc_init__DisplayApi(DisplayApi*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__DispObject(DispObject*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_DispObject(DispObject*, RmHalspecOwner *pRmhalspecowner);
NV_STATUS __nvoc_ctor_DispObject(DispObject*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_DispObject(DispObject*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_dtor_DispObject(DispObject*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DispObject;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DispObject;

// Down-thunk(s) to bridge DispObject methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super^2
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super^2
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_DisplayApi_resControl(struct RsResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_DisplayApi_rmresControl_Prologue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super
void __nvoc_down_thunk_DisplayApi_rmresControl_Epilogue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super

// Up-thunk(s) to bridge DispObject methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_dispapiAccessCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_up_thunk_RmResource_dispapiShareCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams(struct DisplayApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap(struct DisplayApi *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor(struct DisplayApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispapiCanCopy(struct DisplayApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiIsDuplicate(struct DisplayApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_dispapiPreDestruct(struct DisplayApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiControlFilter(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported(struct DisplayApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMapTo(struct DisplayApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmapFrom(struct DisplayApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_dispapiGetRefCount(struct DisplayApi *pResource);    // super
void __nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants(struct RsClient *pClient, struct DisplayApi *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr(struct DisplayApi *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_dispapiGetNotificationShare(struct DisplayApi *pNotifier);    // super
void __nvoc_up_thunk_Notifier_dispapiSetNotificationShare(struct DisplayApi *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispapiUnregisterEvent(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_DisplayApi_dispobjControl(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_DisplayApi_dispobjControl_Prologue(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this
void __nvoc_up_thunk_DisplayApi_dispobjControl_Epilogue(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this
NvBool __nvoc_up_thunk_RmResource_dispobjAccessCallback(struct DispObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_dispobjShareCallback(struct DispObject *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispobjGetMemInterMapParams(struct DispObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispobjCheckMemInterUnmap(struct DispObject *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispobjGetMemoryMappingDescriptor(struct DispObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispobjControlSerialization_Prologue(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispobjControlSerialization_Epilogue(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispobjCanCopy(struct DispObject *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispobjIsDuplicate(struct DispObject *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispobjPreDestruct(struct DispObject *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispobjControlFilter(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispobjMap(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispobjUnmap(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_dispobjIsPartialUnmapSupported(struct DispObject *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispobjMapTo(struct DispObject *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispobjUnmapFrom(struct DispObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispobjGetRefCount(struct DispObject *pResource);    // this
void __nvoc_up_thunk_RsResource_dispobjAddAdditionalDependants(struct RsClient *pClient, struct DispObject *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispobjGetNotificationListPtr(struct DispObject *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispobjGetNotificationShare(struct DispObject *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispobjSetNotificationShare(struct DispObject *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispobjUnregisterEvent(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispobjGetOrAllocNotifShare(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_DispObject = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispObject),
        /*classId=*/            classId(DispObject),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispObject",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispObject,
    /*pCastInfo=*/          &__nvoc_castinfo__DispObject,
    /*pExportInfo=*/        &__nvoc_export_info__DispObject
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DispObject[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetPinsetCount_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700115u,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_PINSET_COUNT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetPinsetCount"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetPinsetPeer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700116u,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_PINSET_PEER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetPinsetPeer"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetRmFreeFlags_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700117u,
        /*paramSize=*/  sizeof(NV5070_CTRL_SET_RMFREE_FLAGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetRmFreeFlags"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdIMPSetGetParameter_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700118u,
        /*paramSize=*/  sizeof(NV5070_CTRL_IMP_SET_GET_PARAMETER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdIMPSetGetParameter"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetMempoolWARForBlitTearing_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700119u,
        /*paramSize=*/  sizeof(NV5070_CTRL_SET_MEMPOOL_WAR_FOR_BLIT_TEARING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetMempoolWARForBlitTearing"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700202u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_RG_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgStatus"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgUnderflowProp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700203u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_RG_UNDERFLOW_PROP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgUnderflowProp"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetRgUnderflowProp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700204u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_RG_UNDERFLOW_PROP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetRgUnderflowProp"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgFliplockProp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700205u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_RG_FLIPLOCK_PROP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgFliplockProp"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetRgFliplockProp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700206u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_RG_FLIPLOCK_PROP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetRgFliplockProp"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgConnectedLockpinStateless_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x5070020au,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_RG_CONNECTED_LOCKPIN_STATELESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgConnectedLockpinStateless"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetPinsetLockpins_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x5070020bu,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_PINSET_LOCKPINS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetPinsetLockpins"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetRgScanLine_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x5070020cu,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_RG_SCAN_LINE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetRgScanLine"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetFrameLockHeaderLockPins_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x5070020du,
        /*paramSize=*/  sizeof(NV5070_CTRL_GET_FRAMELOCK_HEADER_LOCKPINS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetFrameLockHeaderLockPins"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdGetSorOpMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700422u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_GET_SOR_OP_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdGetSorOpMode"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetSorOpMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700423u,
        /*paramSize=*/  sizeof(NV5070_CTRL_CMD_SET_SOR_OP_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetSorOpMode"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSetSorFlushMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700457u,
        /*paramSize=*/  sizeof(NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSetSorFlushMode"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdSystemGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x50700709u,
        /*paramSize=*/  sizeof(NV5070_CTRL_SYSTEM_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdSystemGetCapsV2"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispobjCtrlCmdC370SetSorFlushMode_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700401u,
        /*paramSize=*/  sizeof(NVC370_CTRL_SET_SOR_FLUSH_MODE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispObject.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispobjCtrlCmdC370SetSorFlushMode"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__DispObject __nvoc_metadata__DispObject = {
    .rtti.pClassDef = &__nvoc_class_def_DispObject,    // (dispobj) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispObject,
    .rtti.offset    = 0,
    .metadata__DisplayApi.rtti.pClassDef = &__nvoc_class_def_DisplayApi,    // (dispapi) super
    .metadata__DisplayApi.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.rtti.offset    = NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi),
    .metadata__DisplayApi.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__DisplayApi.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.rtti.offset    = NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource),
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__DisplayApi.metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super^2
    .metadata__DisplayApi.metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__Notifier.rtti.offset    = NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_Notifier),
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^3
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__dispobjControl__ = &__nvoc_up_thunk_DisplayApi_dispobjControl,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControl__ = &dispapiControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_DisplayApi_resControl,    // virtual
    .vtable.__dispobjControl_Prologue__ = &__nvoc_up_thunk_DisplayApi_dispobjControl_Prologue,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControl_Prologue__ = &dispapiControl_Prologue_IMPL,    // virtual override (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl_Prologue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Prologue,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__dispobjControl_Epilogue__ = &__nvoc_up_thunk_DisplayApi_dispobjControl_Epilogue,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControl_Epilogue__ = &dispapiControl_Epilogue_IMPL,    // virtual override (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Epilogue,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__dispobjAccessCallback__ = &__nvoc_up_thunk_RmResource_dispobjAccessCallback,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiAccessCallback__ = &__nvoc_up_thunk_RmResource_dispapiAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__dispobjShareCallback__ = &__nvoc_up_thunk_RmResource_dispobjShareCallback,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiShareCallback__ = &__nvoc_up_thunk_RmResource_dispapiShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__dispobjGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispobjGetMemInterMapParams,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__dispobjCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispobjCheckMemInterUnmap,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__dispobjGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispobjGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__dispobjControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispobjControlSerialization_Prologue,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__dispobjControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispobjControlSerialization_Epilogue,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__dispobjCanCopy__ = &__nvoc_up_thunk_RsResource_dispobjCanCopy,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiCanCopy__ = &__nvoc_up_thunk_RsResource_dispapiCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__dispobjIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispobjIsDuplicate,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispapiIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__dispobjPreDestruct__ = &__nvoc_up_thunk_RsResource_dispobjPreDestruct,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiPreDestruct__ = &__nvoc_up_thunk_RsResource_dispapiPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__dispobjControlFilter__ = &__nvoc_up_thunk_RsResource_dispobjControlFilter,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControlFilter__ = &__nvoc_up_thunk_RsResource_dispapiControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__dispobjMap__ = &__nvoc_up_thunk_RsResource_dispobjMap,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiMap__ = &__nvoc_up_thunk_RsResource_dispapiMap,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__dispobjUnmap__ = &__nvoc_up_thunk_RsResource_dispobjUnmap,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiUnmap__ = &__nvoc_up_thunk_RsResource_dispapiUnmap,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__dispobjIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispobjIsPartialUnmapSupported,    // inline virtual inherited (res) base (dispapi) body
    .metadata__DisplayApi.vtable.__dispapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__dispobjMapTo__ = &__nvoc_up_thunk_RsResource_dispobjMapTo,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiMapTo__ = &__nvoc_up_thunk_RsResource_dispapiMapTo,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__dispobjUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispobjUnmapFrom,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispapiUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__dispobjGetRefCount__ = &__nvoc_up_thunk_RsResource_dispobjGetRefCount,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetRefCount__ = &__nvoc_up_thunk_RsResource_dispapiGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__dispobjAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispobjAddAdditionalDependants,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__dispobjGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispobjGetNotificationListPtr,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__dispobjGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispobjGetNotificationShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__dispobjSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispobjSetNotificationShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__dispobjUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispobjUnregisterEvent,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispapiUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__dispobjGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispobjGetOrAllocNotifShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DispObject = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__DispObject.rtti,    // [0]: (dispobj) this
        &__nvoc_metadata__DispObject.metadata__DisplayApi.rtti,    // [1]: (dispapi) super
        &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__Notifier.rtti,    // [6]: (notify) super^2
        &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^3
    }
};

// 26 up-thunk(s) defined to bridge methods in DispObject to superclasses

// dispobjControl: virtual inherited (dispapi) base (dispapi)
NV_STATUS __nvoc_up_thunk_DisplayApi_dispobjControl(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi)), pCallContext, pParams);
}

// dispobjControl_Prologue: virtual inherited (dispapi) base (dispapi)
NV_STATUS __nvoc_up_thunk_DisplayApi_dispobjControl_Prologue(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi)), pCallContext, pRsParams);
}

// dispobjControl_Epilogue: virtual inherited (dispapi) base (dispapi)
void __nvoc_up_thunk_DisplayApi_dispobjControl_Epilogue(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi)), pCallContext, pRsParams);
}

// dispobjAccessCallback: virtual inherited (rmres) base (dispapi)
NvBool __nvoc_up_thunk_RmResource_dispobjAccessCallback(struct DispObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispobjShareCallback: virtual inherited (rmres) base (dispapi)
NvBool __nvoc_up_thunk_RmResource_dispobjShareCallback(struct DispObject *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispobjGetMemInterMapParams: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispobjGetMemInterMapParams(struct DispObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pParams);
}

// dispobjCheckMemInterUnmap: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispobjCheckMemInterUnmap(struct DispObject *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispobjGetMemoryMappingDescriptor: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispobjGetMemoryMappingDescriptor(struct DispObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), ppMemDesc);
}

// dispobjControlSerialization_Prologue: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispobjControlSerialization_Prologue(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispobjControlSerialization_Epilogue: virtual inherited (rmres) base (dispapi)
void __nvoc_up_thunk_RmResource_dispobjControlSerialization_Epilogue(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispobjCanCopy: virtual inherited (res) base (dispapi)
NvBool __nvoc_up_thunk_RsResource_dispobjCanCopy(struct DispObject *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispobjIsDuplicate: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispobjIsDuplicate(struct DispObject *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispobjPreDestruct: virtual inherited (res) base (dispapi)
void __nvoc_up_thunk_RsResource_dispobjPreDestruct(struct DispObject *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispobjControlFilter: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispobjControlFilter(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispobjMap: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispobjMap(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// dispobjUnmap: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispobjUnmap(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// dispobjIsPartialUnmapSupported: inline virtual inherited (res) base (dispapi) body
NvBool __nvoc_up_thunk_RsResource_dispobjIsPartialUnmapSupported(struct DispObject *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispobjMapTo: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispobjMapTo(struct DispObject *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispobjUnmapFrom: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispobjUnmapFrom(struct DispObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispobjGetRefCount: virtual inherited (res) base (dispapi)
NvU32 __nvoc_up_thunk_RsResource_dispobjGetRefCount(struct DispObject *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispobjAddAdditionalDependants: virtual inherited (res) base (dispapi)
void __nvoc_up_thunk_RsResource_dispobjAddAdditionalDependants(struct RsClient *pClient, struct DispObject *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispobjGetNotificationListPtr: virtual inherited (notify) base (dispapi)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispobjGetNotificationListPtr(struct DispObject *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_Notifier)));
}

// dispobjGetNotificationShare: virtual inherited (notify) base (dispapi)
struct NotifShare * __nvoc_up_thunk_Notifier_dispobjGetNotificationShare(struct DispObject *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_Notifier)));
}

// dispobjSetNotificationShare: virtual inherited (notify) base (dispapi)
void __nvoc_up_thunk_Notifier_dispobjSetNotificationShare(struct DispObject *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_Notifier)), pNotifShare);
}

// dispobjUnregisterEvent: virtual inherited (notify) base (dispapi)
NV_STATUS __nvoc_up_thunk_Notifier_dispobjUnregisterEvent(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispobjGetOrAllocNotifShare: virtual inherited (notify) base (dispapi)
NV_STATUS __nvoc_up_thunk_Notifier_dispobjGetOrAllocNotifShare(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispObject, __nvoc_base_DisplayApi.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DispObject = 
{
    /*numEntries=*/     19,
    /*pExportEntries=*/ __nvoc_exported_method_def_DispObject
};

void __nvoc_dtor_DisplayApi(DisplayApi*);
void __nvoc_dtor_DispObject(DispObject *pThis) {
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispObject(DispObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);
}

NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_DispObject(DispObject *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispObject_fail_DisplayApi;
    __nvoc_init_dataField_DispObject(pThis, pRmhalspecowner);

    status = __nvoc_dispobjConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispObject_fail__init;
    goto __nvoc_ctor_DispObject_exit; // Success

__nvoc_ctor_DispObject_fail__init:
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
__nvoc_ctor_DispObject_fail_DisplayApi:
__nvoc_ctor_DispObject_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DispObject_1(DispObject *pThis, RmHalspecOwner *pRmhalspecowner) {
    RmVariantHal *rmVariantHal = &pRmhalspecowner->rmVariantHal;
    const unsigned long rmVariantHal_HalVarIdx = (unsigned long)rmVariantHal->__nvoc_HalVarIdx;
    PORT_UNREFERENCED_VARIABLE(pThis);
    PORT_UNREFERENCED_VARIABLE(pRmhalspecowner);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal);
    PORT_UNREFERENCED_VARIABLE(rmVariantHal_HalVarIdx);

    // dispobjCtrlCmdGetPinsetCount -- exported (id=0x50700115)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdGetPinsetCount__ = &dispobjCtrlCmdGetPinsetCount_IMPL;
#endif

    // dispobjCtrlCmdGetPinsetPeer -- exported (id=0x50700116)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdGetPinsetPeer__ = &dispobjCtrlCmdGetPinsetPeer_IMPL;
#endif

    // dispobjCtrlCmdSetMempoolWARForBlitTearing -- exported (id=0x50700119)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdSetMempoolWARForBlitTearing__ = &dispobjCtrlCmdSetMempoolWARForBlitTearing_IMPL;
#endif

    // dispobjCtrlCmdGetPinsetLockpins -- exported (id=0x5070020b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdGetPinsetLockpins__ = &dispobjCtrlCmdGetPinsetLockpins_IMPL;
#endif

    // dispobjCtrlCmdGetFrameLockHeaderLockPins -- exported (id=0x5070020d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispobjCtrlCmdGetFrameLockHeaderLockPins__ = &dispobjCtrlCmdGetFrameLockHeaderLockPins_IMPL;
#endif

    // dispobjCtrlCmdSetRmFreeFlags -- exported (id=0x50700117)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispobjCtrlCmdSetRmFreeFlags__ = &dispobjCtrlCmdSetRmFreeFlags_IMPL;
#endif

    // dispobjCtrlCmdIMPSetGetParameter -- exported (id=0x50700118)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdIMPSetGetParameter__ = &dispobjCtrlCmdIMPSetGetParameter_IMPL;
#endif

    // dispobjCtrlCmdGetRgStatus -- exported (id=0x50700202)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispobjCtrlCmdGetRgStatus__ = &dispobjCtrlCmdGetRgStatus_IMPL;
#endif

    // dispobjCtrlCmdGetRgUnderflowProp -- exported (id=0x50700203)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdGetRgUnderflowProp__ = &dispobjCtrlCmdGetRgUnderflowProp_IMPL;
#endif

    // dispobjCtrlCmdSetRgUnderflowProp -- exported (id=0x50700204)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdSetRgUnderflowProp__ = &dispobjCtrlCmdSetRgUnderflowProp_IMPL;
#endif

    // dispobjCtrlCmdGetRgFliplockProp -- exported (id=0x50700205)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdGetRgFliplockProp__ = &dispobjCtrlCmdGetRgFliplockProp_IMPL;
#endif

    // dispobjCtrlCmdSetRgFliplockProp -- exported (id=0x50700206)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispobjCtrlCmdSetRgFliplockProp__ = &dispobjCtrlCmdSetRgFliplockProp_IMPL;
#endif

    // dispobjCtrlCmdGetRgConnectedLockpinStateless -- exported (id=0x5070020a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispobjCtrlCmdGetRgConnectedLockpinStateless__ = &dispobjCtrlCmdGetRgConnectedLockpinStateless_IMPL;
#endif

    // dispobjCtrlCmdGetRgScanLine -- exported (id=0x5070020c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdGetRgScanLine__ = &dispobjCtrlCmdGetRgScanLine_IMPL;
#endif

    // dispobjCtrlCmdGetSorOpMode -- exported (id=0x50700422)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispobjCtrlCmdGetSorOpMode__ = &dispobjCtrlCmdGetSorOpMode_IMPL;
#endif

    // dispobjCtrlCmdSetSorOpMode -- exported (id=0x50700423)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispobjCtrlCmdSetSorOpMode__ = &dispobjCtrlCmdSetSorOpMode_IMPL;
#endif

    // dispobjCtrlCmdSetSorFlushMode -- exported (id=0x50700457)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispobjCtrlCmdSetSorFlushMode__ = &dispobjCtrlCmdSetSorFlushMode_IMPL;
#endif

    // dispobjCtrlCmdC370SetSorFlushMode -- exported (id=0xc3700401)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispobjCtrlCmdC370SetSorFlushMode__ = &dispobjCtrlCmdC370SetSorFlushMode_IMPL;
#endif

    // dispobjCtrlCmdSystemGetCapsV2 -- exported (id=0x50700709)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispobjCtrlCmdSystemGetCapsV2__ = &dispobjCtrlCmdSystemGetCapsV2_IMPL;
#endif
} // End __nvoc_init_funcTable_DispObject_1 with approximately 19 basic block(s).


// Initialize vtable(s) for 45 virtual method(s).
void __nvoc_init_funcTable_DispObject(DispObject *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize vtable(s) with 19 per-object function pointer(s).
    __nvoc_init_funcTable_DispObject_1(pThis, pRmhalspecowner);
}

// Initialize newly constructed object.
void __nvoc_init__DispObject(DispObject *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^3
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier;    // (notify) super^2
    pThis->__nvoc_pbase_DisplayApi = &pThis->__nvoc_base_DisplayApi;    // (dispapi) super
    pThis->__nvoc_pbase_DispObject = pThis;    // (dispobj) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier;    // (inotify) super^3
    pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispObject.metadata__DisplayApi.metadata__Notifier;    // (notify) super^2
    pThis->__nvoc_base_DisplayApi.__nvoc_metadata_ptr = &__nvoc_metadata__DispObject.metadata__DisplayApi;    // (dispapi) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DispObject;    // (dispobj) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DispObject(pThis, pRmhalspecowner);
}

NV_STATUS __nvoc_objCreate_DispObject(DispObject **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispObject *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispObject), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispObject));

    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__DispObject(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DispObject(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispObject_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispObject_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispObject));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispObject(DispObject **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispObject(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x36aa0b = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvDispApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispObject;

// Forward declarations for NvDispApi
void __nvoc_init__DispObject(DispObject*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__NvDispApi(NvDispApi*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_NvDispApi(NvDispApi*);
NV_STATUS __nvoc_ctor_NvDispApi(NvDispApi*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_NvDispApi(NvDispApi*);
void __nvoc_dtor_NvDispApi(NvDispApi*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__NvDispApi;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__NvDispApi;

// Down-thunk(s) to bridge NvDispApi methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super^3
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super^3
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super^3
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^3
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^3
NV_STATUS __nvoc_down_thunk_DisplayApi_resControl(struct RsResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_DisplayApi_rmresControl_Prologue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super^2
void __nvoc_down_thunk_DisplayApi_rmresControl_Epilogue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super^2

// Up-thunk(s) to bridge NvDispApi methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^3
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^3
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^3
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^3
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^3
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^3
NvBool __nvoc_up_thunk_RmResource_dispapiAccessCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_up_thunk_RmResource_dispapiShareCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams(struct DisplayApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap(struct DisplayApi *pRmResource, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor(struct DisplayApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_dispapiCanCopy(struct DisplayApi *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_dispapiIsDuplicate(struct DisplayApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_dispapiPreDestruct(struct DisplayApi *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_dispapiControlFilter(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported(struct DisplayApi *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMapTo(struct DisplayApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmapFrom(struct DisplayApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_dispapiGetRefCount(struct DisplayApi *pResource);    // super^2
void __nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants(struct RsClient *pClient, struct DisplayApi *pResource, RsResourceRef *pReference);    // super^2
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr(struct DisplayApi *pNotifier);    // super^2
struct NotifShare * __nvoc_up_thunk_Notifier_dispapiGetNotificationShare(struct DisplayApi *pNotifier);    // super^2
void __nvoc_up_thunk_Notifier_dispapiSetNotificationShare(struct DisplayApi *pNotifier, struct NotifShare *pNotifShare);    // super^2
NV_STATUS __nvoc_up_thunk_Notifier_dispapiUnregisterEvent(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^2
NV_STATUS __nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^2
NV_STATUS __nvoc_up_thunk_DisplayApi_dispobjControl(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_DisplayApi_dispobjControl_Prologue(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super
void __nvoc_up_thunk_DisplayApi_dispobjControl_Epilogue(struct DispObject *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super
NvBool __nvoc_up_thunk_RmResource_dispobjAccessCallback(struct DispObject *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_up_thunk_RmResource_dispobjShareCallback(struct DispObject *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispobjGetMemInterMapParams(struct DispObject *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispobjCheckMemInterUnmap(struct DispObject *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispobjGetMemoryMappingDescriptor(struct DispObject *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispobjControlSerialization_Prologue(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispobjControlSerialization_Epilogue(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispobjCanCopy(struct DispObject *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispobjIsDuplicate(struct DispObject *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_dispobjPreDestruct(struct DispObject *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispobjControlFilter(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispobjMap(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispobjUnmap(struct DispObject *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_dispobjIsPartialUnmapSupported(struct DispObject *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispobjMapTo(struct DispObject *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispobjUnmapFrom(struct DispObject *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_dispobjGetRefCount(struct DispObject *pResource);    // super
void __nvoc_up_thunk_RsResource_dispobjAddAdditionalDependants(struct RsClient *pClient, struct DispObject *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispobjGetNotificationListPtr(struct DispObject *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_dispobjGetNotificationShare(struct DispObject *pNotifier);    // super
void __nvoc_up_thunk_Notifier_dispobjSetNotificationShare(struct DispObject *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispobjUnregisterEvent(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispobjGetOrAllocNotifShare(struct DispObject *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_DisplayApi_nvdispapiControl(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_DisplayApi_nvdispapiControl_Prologue(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this
void __nvoc_up_thunk_DisplayApi_nvdispapiControl_Epilogue(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this
NvBool __nvoc_up_thunk_RmResource_nvdispapiAccessCallback(struct NvDispApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_nvdispapiShareCallback(struct NvDispApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_nvdispapiGetMemInterMapParams(struct NvDispApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_nvdispapiCheckMemInterUnmap(struct NvDispApi *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_nvdispapiGetMemoryMappingDescriptor(struct NvDispApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_nvdispapiControlSerialization_Prologue(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_nvdispapiControlSerialization_Epilogue(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_nvdispapiCanCopy(struct NvDispApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiIsDuplicate(struct NvDispApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_nvdispapiPreDestruct(struct NvDispApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiControlFilter(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiMap(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiUnmap(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_nvdispapiIsPartialUnmapSupported(struct NvDispApi *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiMapTo(struct NvDispApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiUnmapFrom(struct NvDispApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_nvdispapiGetRefCount(struct NvDispApi *pResource);    // this
void __nvoc_up_thunk_RsResource_nvdispapiAddAdditionalDependants(struct RsClient *pClient, struct NvDispApi *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_nvdispapiGetNotificationListPtr(struct NvDispApi *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_nvdispapiGetNotificationShare(struct NvDispApi *pNotifier);    // this
void __nvoc_up_thunk_Notifier_nvdispapiSetNotificationShare(struct NvDispApi *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_nvdispapiUnregisterEvent(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_nvdispapiGetOrAllocNotifShare(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_NvDispApi = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(NvDispApi),
        /*classId=*/            classId(NvDispApi),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "NvDispApi",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_NvDispApi,
    /*pCastInfo=*/          &__nvoc_castinfo__NvDispApi,
    /*pExportInfo=*/        &__nvoc_export_info__NvDispApi
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_NvDispApi[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdIdleChannel_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700101u,
        /*paramSize=*/  sizeof(NVC370_CTRL_IDLE_CHANNEL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdIdleChannel"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdSetAccl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700102u,
        /*paramSize=*/  sizeof(NVC370_CTRL_SET_ACCL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdSetAccl"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdGetAccl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700103u,
        /*paramSize=*/  sizeof(NVC370_CTRL_GET_ACCL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdGetAccl"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdGetChannelInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700104u,
        /*paramSize=*/  sizeof(NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdGetChannelInfo"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdGetLockpinsCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700201u,
        /*paramSize=*/  sizeof(NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdGetLockpinsCaps"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdSetSwaprdyGpioWar_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700202u,
        /*paramSize=*/  sizeof(NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdSetSwaprdyGpioWar"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3700602u,
        /*paramSize=*/  sizeof(NVC370_CTRL_CMD_SET_FORCE_MODESWITCH_FLAGS_OVERRIDES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_NvDispApi.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__NvDispApi __nvoc_metadata__NvDispApi = {
    .rtti.pClassDef = &__nvoc_class_def_NvDispApi,    // (nvdispapi) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_NvDispApi,
    .rtti.offset    = 0,
    .metadata__DispObject.rtti.pClassDef = &__nvoc_class_def_DispObject,    // (dispobj) super
    .metadata__DispObject.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispObject.rtti.offset    = NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject),
    .metadata__DispObject.metadata__DisplayApi.rtti.pClassDef = &__nvoc_class_def_DisplayApi,    // (dispapi) super^2
    .metadata__DispObject.metadata__DisplayApi.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispObject.metadata__DisplayApi.rtti.offset    = NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi),
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^3
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.rtti.offset    = NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource),
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^4
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^4
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super^3
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.rtti.offset    = NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier),
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^4
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__nvdispapiControl__ = &__nvoc_up_thunk_DisplayApi_nvdispapiControl,    // virtual inherited (dispapi) base (dispobj)
    .metadata__DispObject.vtable.__dispobjControl__ = &__nvoc_up_thunk_DisplayApi_dispobjControl,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiControl__ = &dispapiControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_DisplayApi_resControl,    // virtual
    .vtable.__nvdispapiControl_Prologue__ = &__nvoc_up_thunk_DisplayApi_nvdispapiControl_Prologue,    // virtual inherited (dispapi) base (dispobj)
    .metadata__DispObject.vtable.__dispobjControl_Prologue__ = &__nvoc_up_thunk_DisplayApi_dispobjControl_Prologue,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiControl_Prologue__ = &dispapiControl_Prologue_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl_Prologue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Prologue,    // virtual override (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__nvdispapiControl_Epilogue__ = &__nvoc_up_thunk_DisplayApi_nvdispapiControl_Epilogue,    // virtual inherited (dispapi) base (dispobj)
    .metadata__DispObject.vtable.__dispobjControl_Epilogue__ = &__nvoc_up_thunk_DisplayApi_dispobjControl_Epilogue,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiControl_Epilogue__ = &dispapiControl_Epilogue_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Epilogue,    // virtual override (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__nvdispapiAccessCallback__ = &__nvoc_up_thunk_RmResource_nvdispapiAccessCallback,    // virtual inherited (rmres) base (dispobj)
    .metadata__DispObject.vtable.__dispobjAccessCallback__ = &__nvoc_up_thunk_RmResource_dispobjAccessCallback,    // virtual inherited (rmres) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiAccessCallback__ = &__nvoc_up_thunk_RmResource_dispapiAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__nvdispapiShareCallback__ = &__nvoc_up_thunk_RmResource_nvdispapiShareCallback,    // virtual inherited (rmres) base (dispobj)
    .metadata__DispObject.vtable.__dispobjShareCallback__ = &__nvoc_up_thunk_RmResource_dispobjShareCallback,    // virtual inherited (rmres) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiShareCallback__ = &__nvoc_up_thunk_RmResource_dispapiShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__nvdispapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_nvdispapiGetMemInterMapParams,    // virtual inherited (rmres) base (dispobj)
    .metadata__DispObject.vtable.__dispobjGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispobjGetMemInterMapParams,    // virtual inherited (rmres) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__nvdispapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_nvdispapiCheckMemInterUnmap,    // virtual inherited (rmres) base (dispobj)
    .metadata__DispObject.vtable.__dispobjCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispobjCheckMemInterUnmap,    // virtual inherited (rmres) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__nvdispapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_nvdispapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (dispobj)
    .metadata__DispObject.vtable.__dispobjGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispobjGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__nvdispapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_nvdispapiControlSerialization_Prologue,    // virtual inherited (rmres) base (dispobj)
    .metadata__DispObject.vtable.__dispobjControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispobjControlSerialization_Prologue,    // virtual inherited (rmres) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__nvdispapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_nvdispapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (dispobj)
    .metadata__DispObject.vtable.__dispobjControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispobjControlSerialization_Epilogue,    // virtual inherited (rmres) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__nvdispapiCanCopy__ = &__nvoc_up_thunk_RsResource_nvdispapiCanCopy,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjCanCopy__ = &__nvoc_up_thunk_RsResource_dispobjCanCopy,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiCanCopy__ = &__nvoc_up_thunk_RsResource_dispapiCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__nvdispapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_nvdispapiIsDuplicate,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispobjIsDuplicate,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispapiIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__nvdispapiPreDestruct__ = &__nvoc_up_thunk_RsResource_nvdispapiPreDestruct,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjPreDestruct__ = &__nvoc_up_thunk_RsResource_dispobjPreDestruct,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiPreDestruct__ = &__nvoc_up_thunk_RsResource_dispapiPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__nvdispapiControlFilter__ = &__nvoc_up_thunk_RsResource_nvdispapiControlFilter,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjControlFilter__ = &__nvoc_up_thunk_RsResource_dispobjControlFilter,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiControlFilter__ = &__nvoc_up_thunk_RsResource_dispapiControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__nvdispapiMap__ = &__nvoc_up_thunk_RsResource_nvdispapiMap,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjMap__ = &__nvoc_up_thunk_RsResource_dispobjMap,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiMap__ = &__nvoc_up_thunk_RsResource_dispapiMap,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__nvdispapiUnmap__ = &__nvoc_up_thunk_RsResource_nvdispapiUnmap,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjUnmap__ = &__nvoc_up_thunk_RsResource_dispobjUnmap,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiUnmap__ = &__nvoc_up_thunk_RsResource_dispapiUnmap,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__nvdispapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_nvdispapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (dispobj) body
    .metadata__DispObject.vtable.__dispobjIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispobjIsPartialUnmapSupported,    // inline virtual inherited (res) base (dispapi) body
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__nvdispapiMapTo__ = &__nvoc_up_thunk_RsResource_nvdispapiMapTo,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjMapTo__ = &__nvoc_up_thunk_RsResource_dispobjMapTo,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiMapTo__ = &__nvoc_up_thunk_RsResource_dispapiMapTo,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__nvdispapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_nvdispapiUnmapFrom,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispobjUnmapFrom,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispapiUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__nvdispapiGetRefCount__ = &__nvoc_up_thunk_RsResource_nvdispapiGetRefCount,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjGetRefCount__ = &__nvoc_up_thunk_RsResource_dispobjGetRefCount,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiGetRefCount__ = &__nvoc_up_thunk_RsResource_dispapiGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__nvdispapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_nvdispapiAddAdditionalDependants,    // virtual inherited (res) base (dispobj)
    .metadata__DispObject.vtable.__dispobjAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispobjAddAdditionalDependants,    // virtual inherited (res) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__nvdispapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_nvdispapiGetNotificationListPtr,    // virtual inherited (notify) base (dispobj)
    .metadata__DispObject.vtable.__dispobjGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispobjGetNotificationListPtr,    // virtual inherited (notify) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__nvdispapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_nvdispapiGetNotificationShare,    // virtual inherited (notify) base (dispobj)
    .metadata__DispObject.vtable.__dispobjGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispobjGetNotificationShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__nvdispapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_nvdispapiSetNotificationShare,    // virtual inherited (notify) base (dispobj)
    .metadata__DispObject.vtable.__dispobjSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispobjSetNotificationShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__nvdispapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_nvdispapiUnregisterEvent,    // virtual inherited (notify) base (dispobj)
    .metadata__DispObject.vtable.__dispobjUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispobjUnregisterEvent,    // virtual inherited (notify) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispapiUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__nvdispapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_nvdispapiGetOrAllocNotifShare,    // virtual inherited (notify) base (dispobj)
    .metadata__DispObject.vtable.__dispobjGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispobjGetOrAllocNotifShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DispObject.metadata__DisplayApi.vtable.__dispapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__NvDispApi = {
    .numRelatives = 9,
    .relatives = {
        &__nvoc_metadata__NvDispApi.rtti,    // [0]: (nvdispapi) this
        &__nvoc_metadata__NvDispApi.metadata__DispObject.rtti,    // [1]: (dispobj) super
        &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.rtti,    // [2]: (dispapi) super^2
        &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__RmResource.rtti,    // [3]: (rmres) super^3
        &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti,    // [4]: (res) super^4
        &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [5]: (obj) super^5
        &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [6]: (rmrescmn) super^4
        &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__Notifier.rtti,    // [7]: (notify) super^3
        &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti,    // [8]: (inotify) super^4
    }
};

// 26 up-thunk(s) defined to bridge methods in NvDispApi to superclasses

// nvdispapiControl: virtual inherited (dispapi) base (dispobj)
NV_STATUS __nvoc_up_thunk_DisplayApi_nvdispapiControl(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi)), pCallContext, pParams);
}

// nvdispapiControl_Prologue: virtual inherited (dispapi) base (dispobj)
NV_STATUS __nvoc_up_thunk_DisplayApi_nvdispapiControl_Prologue(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi)), pCallContext, pRsParams);
}

// nvdispapiControl_Epilogue: virtual inherited (dispapi) base (dispobj)
void __nvoc_up_thunk_DisplayApi_nvdispapiControl_Epilogue(struct NvDispApi *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi)), pCallContext, pRsParams);
}

// nvdispapiAccessCallback: virtual inherited (rmres) base (dispobj)
NvBool __nvoc_up_thunk_RmResource_nvdispapiAccessCallback(struct NvDispApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// nvdispapiShareCallback: virtual inherited (rmres) base (dispobj)
NvBool __nvoc_up_thunk_RmResource_nvdispapiShareCallback(struct NvDispApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// nvdispapiGetMemInterMapParams: virtual inherited (rmres) base (dispobj)
NV_STATUS __nvoc_up_thunk_RmResource_nvdispapiGetMemInterMapParams(struct NvDispApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource)), pParams);
}

// nvdispapiCheckMemInterUnmap: virtual inherited (rmres) base (dispobj)
NV_STATUS __nvoc_up_thunk_RmResource_nvdispapiCheckMemInterUnmap(struct NvDispApi *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// nvdispapiGetMemoryMappingDescriptor: virtual inherited (rmres) base (dispobj)
NV_STATUS __nvoc_up_thunk_RmResource_nvdispapiGetMemoryMappingDescriptor(struct NvDispApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource)), ppMemDesc);
}

// nvdispapiControlSerialization_Prologue: virtual inherited (rmres) base (dispobj)
NV_STATUS __nvoc_up_thunk_RmResource_nvdispapiControlSerialization_Prologue(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource)), pCallContext, pParams);
}

// nvdispapiControlSerialization_Epilogue: virtual inherited (rmres) base (dispobj)
void __nvoc_up_thunk_RmResource_nvdispapiControlSerialization_Epilogue(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource)), pCallContext, pParams);
}

// nvdispapiCanCopy: virtual inherited (res) base (dispobj)
NvBool __nvoc_up_thunk_RsResource_nvdispapiCanCopy(struct NvDispApi *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// nvdispapiIsDuplicate: virtual inherited (res) base (dispobj)
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiIsDuplicate(struct NvDispApi *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// nvdispapiPreDestruct: virtual inherited (res) base (dispobj)
void __nvoc_up_thunk_RsResource_nvdispapiPreDestruct(struct NvDispApi *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// nvdispapiControlFilter: virtual inherited (res) base (dispobj)
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiControlFilter(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// nvdispapiMap: virtual inherited (res) base (dispobj)
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiMap(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// nvdispapiUnmap: virtual inherited (res) base (dispobj)
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiUnmap(struct NvDispApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// nvdispapiIsPartialUnmapSupported: inline virtual inherited (res) base (dispobj) body
NvBool __nvoc_up_thunk_RsResource_nvdispapiIsPartialUnmapSupported(struct NvDispApi *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// nvdispapiMapTo: virtual inherited (res) base (dispobj)
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiMapTo(struct NvDispApi *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// nvdispapiUnmapFrom: virtual inherited (res) base (dispobj)
NV_STATUS __nvoc_up_thunk_RsResource_nvdispapiUnmapFrom(struct NvDispApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// nvdispapiGetRefCount: virtual inherited (res) base (dispobj)
NvU32 __nvoc_up_thunk_RsResource_nvdispapiGetRefCount(struct NvDispApi *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// nvdispapiAddAdditionalDependants: virtual inherited (res) base (dispobj)
void __nvoc_up_thunk_RsResource_nvdispapiAddAdditionalDependants(struct RsClient *pClient, struct NvDispApi *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// nvdispapiGetNotificationListPtr: virtual inherited (notify) base (dispobj)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_nvdispapiGetNotificationListPtr(struct NvDispApi *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier)));
}

// nvdispapiGetNotificationShare: virtual inherited (notify) base (dispobj)
struct NotifShare * __nvoc_up_thunk_Notifier_nvdispapiGetNotificationShare(struct NvDispApi *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier)));
}

// nvdispapiSetNotificationShare: virtual inherited (notify) base (dispobj)
void __nvoc_up_thunk_Notifier_nvdispapiSetNotificationShare(struct NvDispApi *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier)), pNotifShare);
}

// nvdispapiUnregisterEvent: virtual inherited (notify) base (dispobj)
NV_STATUS __nvoc_up_thunk_Notifier_nvdispapiUnregisterEvent(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// nvdispapiGetOrAllocNotifShare: virtual inherited (notify) base (dispobj)
NV_STATUS __nvoc_up_thunk_Notifier_nvdispapiGetOrAllocNotifShare(struct NvDispApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(NvDispApi, __nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__NvDispApi = 
{
    /*numEntries=*/     7,
    /*pExportEntries=*/ __nvoc_exported_method_def_NvDispApi
};

void __nvoc_dtor_DispObject(DispObject*);
void __nvoc_dtor_NvDispApi(NvDispApi *pThis) {
    __nvoc_dtor_DispObject(&pThis->__nvoc_base_DispObject);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_NvDispApi(NvDispApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DispObject(DispObject* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_NvDispApi(NvDispApi *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DispObject(&pThis->__nvoc_base_DispObject, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvDispApi_fail_DispObject;
    __nvoc_init_dataField_NvDispApi(pThis);

    status = __nvoc_nvdispapiConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_NvDispApi_fail__init;
    goto __nvoc_ctor_NvDispApi_exit; // Success

__nvoc_ctor_NvDispApi_fail__init:
    __nvoc_dtor_DispObject(&pThis->__nvoc_base_DispObject);
__nvoc_ctor_NvDispApi_fail_DispObject:
__nvoc_ctor_NvDispApi_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_NvDispApi_1(NvDispApi *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // nvdispapiCtrlCmdIdleChannel -- exported (id=0xc3700101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__nvdispapiCtrlCmdIdleChannel__ = &nvdispapiCtrlCmdIdleChannel_IMPL;
#endif

    // nvdispapiCtrlCmdSetAccl -- exported (id=0xc3700102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__nvdispapiCtrlCmdSetAccl__ = &nvdispapiCtrlCmdSetAccl_IMPL;
#endif

    // nvdispapiCtrlCmdGetAccl -- exported (id=0xc3700103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__nvdispapiCtrlCmdGetAccl__ = &nvdispapiCtrlCmdGetAccl_IMPL;
#endif

    // nvdispapiCtrlCmdGetChannelInfo -- exported (id=0xc3700104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__nvdispapiCtrlCmdGetChannelInfo__ = &nvdispapiCtrlCmdGetChannelInfo_IMPL;
#endif

    // nvdispapiCtrlCmdSetSwaprdyGpioWar -- exported (id=0xc3700202)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__nvdispapiCtrlCmdSetSwaprdyGpioWar__ = &nvdispapiCtrlCmdSetSwaprdyGpioWar_IMPL;
#endif

    // nvdispapiCtrlCmdGetLockpinsCaps -- exported (id=0xc3700201)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__nvdispapiCtrlCmdGetLockpinsCaps__ = &nvdispapiCtrlCmdGetLockpinsCaps_IMPL;
#endif

    // nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides -- exported (id=0xc3700602)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides__ = &nvdispapiCtrlCmdSetForceModeswitchFlagsOverrides_IMPL;
#endif
} // End __nvoc_init_funcTable_NvDispApi_1 with approximately 7 basic block(s).


// Initialize vtable(s) for 33 virtual method(s).
void __nvoc_init_funcTable_NvDispApi(NvDispApi *pThis) {

    // Initialize vtable(s) with 7 per-object function pointer(s).
    __nvoc_init_funcTable_NvDispApi_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__NvDispApi(NvDispApi *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^4
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource;    // (rmres) super^3
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^4
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier;    // (notify) super^3
    pThis->__nvoc_pbase_DisplayApi = &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi;    // (dispapi) super^2
    pThis->__nvoc_pbase_DispObject = &pThis->__nvoc_base_DispObject;    // (dispobj) super
    pThis->__nvoc_pbase_NvDispApi = pThis;    // (nvdispapi) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__DispObject(&pThis->__nvoc_base_DispObject, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RsResource;    // (res) super^4
    pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__RmResource;    // (rmres) super^3
    pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__Notifier.metadata__INotifier;    // (inotify) super^4
    pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi.metadata__Notifier;    // (notify) super^3
    pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_metadata_ptr = &__nvoc_metadata__NvDispApi.metadata__DispObject.metadata__DisplayApi;    // (dispapi) super^2
    pThis->__nvoc_base_DispObject.__nvoc_metadata_ptr = &__nvoc_metadata__NvDispApi.metadata__DispObject;    // (dispobj) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__NvDispApi;    // (nvdispapi) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_NvDispApi(pThis);
}

NV_STATUS __nvoc_objCreate_NvDispApi(NvDispApi **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    NvDispApi *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(NvDispApi), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(NvDispApi));

    pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__NvDispApi(pThis, pRmhalspecowner);
    status = __nvoc_ctor_NvDispApi(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_NvDispApi_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_NvDispApi_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_DispObject.__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(NvDispApi));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_NvDispApi(NvDispApi **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_NvDispApi(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x6aa5e2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispSwObj;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;

// Forward declarations for DispSwObj
void __nvoc_init__DisplayApi(DisplayApi*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__DispSwObj(DispSwObj*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_DispSwObj(DispSwObj*);
NV_STATUS __nvoc_ctor_DispSwObj(DispSwObj*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_DispSwObj(DispSwObj*);
void __nvoc_dtor_DispSwObj(DispSwObj*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DispSwObj;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DispSwObj;

// Down-thunk(s) to bridge DispSwObj methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super^2
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super^2
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_DisplayApi_resControl(struct RsResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_DisplayApi_rmresControl_Prologue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super
void __nvoc_down_thunk_DisplayApi_rmresControl_Epilogue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super

// Up-thunk(s) to bridge DispSwObj methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_dispapiAccessCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_up_thunk_RmResource_dispapiShareCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams(struct DisplayApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap(struct DisplayApi *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor(struct DisplayApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispapiCanCopy(struct DisplayApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiIsDuplicate(struct DisplayApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_dispapiPreDestruct(struct DisplayApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiControlFilter(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported(struct DisplayApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMapTo(struct DisplayApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmapFrom(struct DisplayApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_dispapiGetRefCount(struct DisplayApi *pResource);    // super
void __nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants(struct RsClient *pClient, struct DisplayApi *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr(struct DisplayApi *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_dispapiGetNotificationShare(struct DisplayApi *pNotifier);    // super
void __nvoc_up_thunk_Notifier_dispapiSetNotificationShare(struct DisplayApi *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispapiUnregisterEvent(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_DisplayApi_dispswobjControl(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_DisplayApi_dispswobjControl_Prologue(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this
void __nvoc_up_thunk_DisplayApi_dispswobjControl_Epilogue(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this
NvBool __nvoc_up_thunk_RmResource_dispswobjAccessCallback(struct DispSwObj *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_dispswobjShareCallback(struct DispSwObj *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispswobjGetMemInterMapParams(struct DispSwObj *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispswobjCheckMemInterUnmap(struct DispSwObj *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispswobjGetMemoryMappingDescriptor(struct DispSwObj *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispswobjControlSerialization_Prologue(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispswobjControlSerialization_Epilogue(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispswobjCanCopy(struct DispSwObj *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjIsDuplicate(struct DispSwObj *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispswobjPreDestruct(struct DispSwObj *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjControlFilter(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjMap(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjUnmap(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_dispswobjIsPartialUnmapSupported(struct DispSwObj *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjMapTo(struct DispSwObj *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjUnmapFrom(struct DispSwObj *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispswobjGetRefCount(struct DispSwObj *pResource);    // this
void __nvoc_up_thunk_RsResource_dispswobjAddAdditionalDependants(struct RsClient *pClient, struct DispSwObj *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispswobjGetNotificationListPtr(struct DispSwObj *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispswobjGetNotificationShare(struct DispSwObj *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispswobjSetNotificationShare(struct DispSwObj *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispswobjUnregisterEvent(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispswobjGetOrAllocNotifShare(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_DispSwObj = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispSwObj),
        /*classId=*/            classId(DispSwObj),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispSwObj",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispSwObj,
    /*pCastInfo=*/          &__nvoc_castinfo__DispSwObj,
    /*pExportInfo=*/        &__nvoc_export_info__DispSwObj
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DispSwObj[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispswobjCtrlCmdIsModePossible_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3720101u,
        /*paramSize=*/  sizeof(NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispSwObj.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispswobjCtrlCmdIsModePossible"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispswobjCtrlCmdIsModePossibleOrSettings_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3720102u,
        /*paramSize=*/  sizeof(NVC372_CTRL_IS_MODE_POSSIBLE_OR_SETTINGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispSwObj.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispswobjCtrlCmdIsModePossibleOrSettings"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispswobjCtrlCmdVideoAdaptiveRefreshRate_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3720103u,
        /*paramSize=*/  sizeof(NVC372_CTRL_CMD_VIDEO_ADAPTIVE_REFRESH_RATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispSwObj.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispswobjCtrlCmdVideoAdaptiveRefreshRate"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x49u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispswobjCtrlCmdGetActiveViewportPointIn_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x49u)
        /*flags=*/      0x49u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0xc3720104u,
        /*paramSize=*/  sizeof(NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispSwObj.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispswobjCtrlCmdGetActiveViewportPointIn"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__DispSwObj __nvoc_metadata__DispSwObj = {
    .rtti.pClassDef = &__nvoc_class_def_DispSwObj,    // (dispswobj) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispSwObj,
    .rtti.offset    = 0,
    .metadata__DisplayApi.rtti.pClassDef = &__nvoc_class_def_DisplayApi,    // (dispapi) super
    .metadata__DisplayApi.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.rtti.offset    = NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi),
    .metadata__DisplayApi.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__DisplayApi.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.rtti.offset    = NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource),
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__DisplayApi.metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super^2
    .metadata__DisplayApi.metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__Notifier.rtti.offset    = NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_Notifier),
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^3
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__dispswobjControl__ = &__nvoc_up_thunk_DisplayApi_dispswobjControl,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControl__ = &dispapiControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_DisplayApi_resControl,    // virtual
    .vtable.__dispswobjControl_Prologue__ = &__nvoc_up_thunk_DisplayApi_dispswobjControl_Prologue,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControl_Prologue__ = &dispapiControl_Prologue_IMPL,    // virtual override (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl_Prologue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Prologue,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__dispswobjControl_Epilogue__ = &__nvoc_up_thunk_DisplayApi_dispswobjControl_Epilogue,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControl_Epilogue__ = &dispapiControl_Epilogue_IMPL,    // virtual override (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Epilogue,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__dispswobjAccessCallback__ = &__nvoc_up_thunk_RmResource_dispswobjAccessCallback,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiAccessCallback__ = &__nvoc_up_thunk_RmResource_dispapiAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__dispswobjShareCallback__ = &__nvoc_up_thunk_RmResource_dispswobjShareCallback,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiShareCallback__ = &__nvoc_up_thunk_RmResource_dispapiShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__dispswobjGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispswobjGetMemInterMapParams,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__dispswobjCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispswobjCheckMemInterUnmap,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__dispswobjGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispswobjGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__dispswobjControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispswobjControlSerialization_Prologue,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__dispswobjControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispswobjControlSerialization_Epilogue,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__dispswobjCanCopy__ = &__nvoc_up_thunk_RsResource_dispswobjCanCopy,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiCanCopy__ = &__nvoc_up_thunk_RsResource_dispapiCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__dispswobjIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispswobjIsDuplicate,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispapiIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__dispswobjPreDestruct__ = &__nvoc_up_thunk_RsResource_dispswobjPreDestruct,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiPreDestruct__ = &__nvoc_up_thunk_RsResource_dispapiPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__dispswobjControlFilter__ = &__nvoc_up_thunk_RsResource_dispswobjControlFilter,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControlFilter__ = &__nvoc_up_thunk_RsResource_dispapiControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__dispswobjMap__ = &__nvoc_up_thunk_RsResource_dispswobjMap,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiMap__ = &__nvoc_up_thunk_RsResource_dispapiMap,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__dispswobjUnmap__ = &__nvoc_up_thunk_RsResource_dispswobjUnmap,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiUnmap__ = &__nvoc_up_thunk_RsResource_dispapiUnmap,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__dispswobjIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispswobjIsPartialUnmapSupported,    // inline virtual inherited (res) base (dispapi) body
    .metadata__DisplayApi.vtable.__dispapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__dispswobjMapTo__ = &__nvoc_up_thunk_RsResource_dispswobjMapTo,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiMapTo__ = &__nvoc_up_thunk_RsResource_dispapiMapTo,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__dispswobjUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispswobjUnmapFrom,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispapiUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__dispswobjGetRefCount__ = &__nvoc_up_thunk_RsResource_dispswobjGetRefCount,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetRefCount__ = &__nvoc_up_thunk_RsResource_dispapiGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__dispswobjAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispswobjAddAdditionalDependants,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__dispswobjGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispswobjGetNotificationListPtr,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__dispswobjGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispswobjGetNotificationShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__dispswobjSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispswobjSetNotificationShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__dispswobjUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispswobjUnregisterEvent,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispapiUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__dispswobjGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispswobjGetOrAllocNotifShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DispSwObj = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__DispSwObj.rtti,    // [0]: (dispswobj) this
        &__nvoc_metadata__DispSwObj.metadata__DisplayApi.rtti,    // [1]: (dispapi) super
        &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__Notifier.rtti,    // [6]: (notify) super^2
        &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^3
    }
};

// 26 up-thunk(s) defined to bridge methods in DispSwObj to superclasses

// dispswobjControl: virtual inherited (dispapi) base (dispapi)
NV_STATUS __nvoc_up_thunk_DisplayApi_dispswobjControl(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi)), pCallContext, pParams);
}

// dispswobjControl_Prologue: virtual inherited (dispapi) base (dispapi)
NV_STATUS __nvoc_up_thunk_DisplayApi_dispswobjControl_Prologue(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi)), pCallContext, pRsParams);
}

// dispswobjControl_Epilogue: virtual inherited (dispapi) base (dispapi)
void __nvoc_up_thunk_DisplayApi_dispswobjControl_Epilogue(struct DispSwObj *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi)), pCallContext, pRsParams);
}

// dispswobjAccessCallback: virtual inherited (rmres) base (dispapi)
NvBool __nvoc_up_thunk_RmResource_dispswobjAccessCallback(struct DispSwObj *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispswobjShareCallback: virtual inherited (rmres) base (dispapi)
NvBool __nvoc_up_thunk_RmResource_dispswobjShareCallback(struct DispSwObj *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispswobjGetMemInterMapParams: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispswobjGetMemInterMapParams(struct DispSwObj *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pParams);
}

// dispswobjCheckMemInterUnmap: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispswobjCheckMemInterUnmap(struct DispSwObj *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispswobjGetMemoryMappingDescriptor: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispswobjGetMemoryMappingDescriptor(struct DispSwObj *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), ppMemDesc);
}

// dispswobjControlSerialization_Prologue: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispswobjControlSerialization_Prologue(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispswobjControlSerialization_Epilogue: virtual inherited (rmres) base (dispapi)
void __nvoc_up_thunk_RmResource_dispswobjControlSerialization_Epilogue(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispswobjCanCopy: virtual inherited (res) base (dispapi)
NvBool __nvoc_up_thunk_RsResource_dispswobjCanCopy(struct DispSwObj *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispswobjIsDuplicate: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjIsDuplicate(struct DispSwObj *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispswobjPreDestruct: virtual inherited (res) base (dispapi)
void __nvoc_up_thunk_RsResource_dispswobjPreDestruct(struct DispSwObj *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispswobjControlFilter: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjControlFilter(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispswobjMap: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjMap(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// dispswobjUnmap: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjUnmap(struct DispSwObj *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// dispswobjIsPartialUnmapSupported: inline virtual inherited (res) base (dispapi) body
NvBool __nvoc_up_thunk_RsResource_dispswobjIsPartialUnmapSupported(struct DispSwObj *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispswobjMapTo: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjMapTo(struct DispSwObj *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispswobjUnmapFrom: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispswobjUnmapFrom(struct DispSwObj *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispswobjGetRefCount: virtual inherited (res) base (dispapi)
NvU32 __nvoc_up_thunk_RsResource_dispswobjGetRefCount(struct DispSwObj *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispswobjAddAdditionalDependants: virtual inherited (res) base (dispapi)
void __nvoc_up_thunk_RsResource_dispswobjAddAdditionalDependants(struct RsClient *pClient, struct DispSwObj *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispswobjGetNotificationListPtr: virtual inherited (notify) base (dispapi)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispswobjGetNotificationListPtr(struct DispSwObj *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_Notifier)));
}

// dispswobjGetNotificationShare: virtual inherited (notify) base (dispapi)
struct NotifShare * __nvoc_up_thunk_Notifier_dispswobjGetNotificationShare(struct DispSwObj *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_Notifier)));
}

// dispswobjSetNotificationShare: virtual inherited (notify) base (dispapi)
void __nvoc_up_thunk_Notifier_dispswobjSetNotificationShare(struct DispSwObj *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_Notifier)), pNotifShare);
}

// dispswobjUnregisterEvent: virtual inherited (notify) base (dispapi)
NV_STATUS __nvoc_up_thunk_Notifier_dispswobjUnregisterEvent(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispswobjGetOrAllocNotifShare: virtual inherited (notify) base (dispapi)
NV_STATUS __nvoc_up_thunk_Notifier_dispswobjGetOrAllocNotifShare(struct DispSwObj *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispSwObj, __nvoc_base_DisplayApi.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DispSwObj = 
{
    /*numEntries=*/     4,
    /*pExportEntries=*/ __nvoc_exported_method_def_DispSwObj
};

void __nvoc_dtor_DisplayApi(DisplayApi*);
void __nvoc_dtor_DispSwObj(DispSwObj *pThis) {
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispSwObj(DispSwObj *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_DispSwObj(DispSwObj *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispSwObj_fail_DisplayApi;
    __nvoc_init_dataField_DispSwObj(pThis);

    status = __nvoc_dispswobjConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispSwObj_fail__init;
    goto __nvoc_ctor_DispSwObj_exit; // Success

__nvoc_ctor_DispSwObj_fail__init:
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
__nvoc_ctor_DispSwObj_fail_DisplayApi:
__nvoc_ctor_DispSwObj_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DispSwObj_1(DispSwObj *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // dispswobjCtrlCmdIsModePossible -- exported (id=0xc3720101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispswobjCtrlCmdIsModePossible__ = &dispswobjCtrlCmdIsModePossible_IMPL;
#endif

    // dispswobjCtrlCmdIsModePossibleOrSettings -- exported (id=0xc3720102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispswobjCtrlCmdIsModePossibleOrSettings__ = &dispswobjCtrlCmdIsModePossibleOrSettings_IMPL;
#endif

    // dispswobjCtrlCmdVideoAdaptiveRefreshRate -- exported (id=0xc3720103)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispswobjCtrlCmdVideoAdaptiveRefreshRate__ = &dispswobjCtrlCmdVideoAdaptiveRefreshRate_IMPL;
#endif

    // dispswobjCtrlCmdGetActiveViewportPointIn -- exported (id=0xc3720104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x49u)
    pThis->__dispswobjCtrlCmdGetActiveViewportPointIn__ = &dispswobjCtrlCmdGetActiveViewportPointIn_IMPL;
#endif
} // End __nvoc_init_funcTable_DispSwObj_1 with approximately 4 basic block(s).


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_DispSwObj(DispSwObj *pThis) {

    // Initialize vtable(s) with 4 per-object function pointer(s).
    __nvoc_init_funcTable_DispSwObj_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DispSwObj(DispSwObj *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^3
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier;    // (notify) super^2
    pThis->__nvoc_pbase_DisplayApi = &pThis->__nvoc_base_DisplayApi;    // (dispapi) super
    pThis->__nvoc_pbase_DispSwObj = pThis;    // (dispswobj) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__Notifier.metadata__INotifier;    // (inotify) super^3
    pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispSwObj.metadata__DisplayApi.metadata__Notifier;    // (notify) super^2
    pThis->__nvoc_base_DisplayApi.__nvoc_metadata_ptr = &__nvoc_metadata__DispSwObj.metadata__DisplayApi;    // (dispapi) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DispSwObj;    // (dispswobj) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DispSwObj(pThis);
}

NV_STATUS __nvoc_objCreate_DispSwObj(DispSwObj **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispSwObj *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispSwObj), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispSwObj));

    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__DispSwObj(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DispSwObj(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispSwObj_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispSwObj_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispSwObj));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispSwObj(DispSwObj **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispSwObj(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x41f4f2 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DisplayApi;

// Forward declarations for DispCommon
void __nvoc_init__DisplayApi(DisplayApi*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__DispCommon(DispCommon*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_DispCommon(DispCommon*);
NV_STATUS __nvoc_ctor_DispCommon(DispCommon*, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_DispCommon(DispCommon*);
void __nvoc_dtor_DispCommon(DispCommon*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DispCommon;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DispCommon;

// Down-thunk(s) to bridge DispCommon methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super^2
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super^2
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_DisplayApi_resControl(struct RsResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_DisplayApi_rmresControl_Prologue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super
void __nvoc_down_thunk_DisplayApi_rmresControl_Epilogue(struct RmResource *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // super

// Up-thunk(s) to bridge DispCommon methods to ancestors (if any)
NvBool __nvoc_up_thunk_RsResource_rmresCanCopy(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresIsDuplicate(struct RmResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_rmresPreDestruct(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControl(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresControlFilter(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmap(struct RmResource *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported(struct RmResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresMapTo(struct RmResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_rmresUnmapFrom(struct RmResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_rmresGetRefCount(struct RmResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_rmresAddAdditionalDependants(struct RsClient *pClient, struct RmResource *pResource, RsResourceRef *pReference);    // super^2
NvBool __nvoc_up_thunk_RmResource_dispapiAccessCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NvBool __nvoc_up_thunk_RmResource_dispapiShareCallback(struct DisplayApi *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams(struct DisplayApi *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap(struct DisplayApi *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor(struct DisplayApi *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispapiCanCopy(struct DisplayApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiIsDuplicate(struct DisplayApi *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_dispapiPreDestruct(struct DisplayApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiControlFilter(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmap(struct DisplayApi *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported(struct DisplayApi *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiMapTo(struct DisplayApi *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispapiUnmapFrom(struct DisplayApi *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_dispapiGetRefCount(struct DisplayApi *pResource);    // super
void __nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants(struct RsClient *pClient, struct DisplayApi *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr(struct DisplayApi *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_dispapiGetNotificationShare(struct DisplayApi *pNotifier);    // super
void __nvoc_up_thunk_Notifier_dispapiSetNotificationShare(struct DisplayApi *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispapiUnregisterEvent(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare(struct DisplayApi *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_DisplayApi_dispcmnControl(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_DisplayApi_dispcmnControl_Prologue(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this
void __nvoc_up_thunk_DisplayApi_dispcmnControl_Epilogue(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams);    // this
NvBool __nvoc_up_thunk_RmResource_dispcmnAccessCallback(struct DispCommon *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NvBool __nvoc_up_thunk_RmResource_dispcmnShareCallback(struct DispCommon *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispcmnGetMemInterMapParams(struct DispCommon *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispcmnCheckMemInterUnmap(struct DispCommon *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispcmnGetMemoryMappingDescriptor(struct DispCommon *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispcmnControlSerialization_Prologue(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispcmnControlSerialization_Epilogue(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispcmnCanCopy(struct DispCommon *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnIsDuplicate(struct DispCommon *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispcmnPreDestruct(struct DispCommon *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnControlFilter(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnMap(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnUnmap(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_RsResource_dispcmnIsPartialUnmapSupported(struct DispCommon *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnMapTo(struct DispCommon *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnUnmapFrom(struct DispCommon *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispcmnGetRefCount(struct DispCommon *pResource);    // this
void __nvoc_up_thunk_RsResource_dispcmnAddAdditionalDependants(struct RsClient *pClient, struct DispCommon *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispcmnGetNotificationListPtr(struct DispCommon *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispcmnGetNotificationShare(struct DispCommon *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispcmnSetNotificationShare(struct DispCommon *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispcmnUnregisterEvent(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispcmnGetOrAllocNotifShare(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_DispCommon = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispCommon),
        /*classId=*/            classId(DispCommon),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispCommon",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispCommon,
    /*pCastInfo=*/          &__nvoc_castinfo__DispCommon,
    /*pExportInfo=*/        &__nvoc_export_info__DispCommon
};

#if !defined(NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG)
#define NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(x)      (0)
#endif

static const struct NVOC_EXPORTED_METHOD_DEF __nvoc_exported_method_def_DispCommon[] = 
{
    {               /*  [0] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetCapsV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730101u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_CAPS_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetCapsV2"
#endif
    },
    {               /*  [1] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetNumHeads_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*flags=*/      0x4au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730102u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_NUM_HEADS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetNumHeads"
#endif
    },
    {               /*  [2] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetScanline_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730104u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_SCANLINE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetScanline"
#endif
    },
    {               /*  [3] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetVblankCounter_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730105u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_VBLANK_COUNTER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetVblankCounter"
#endif
    },
    {               /*  [4] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetVblankEnable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730106u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_VBLANK_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetVblankEnable"
#endif
    },
    {               /*  [5] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x82004au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetSuppported_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x82004au)
        /*flags=*/      0x82004au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730107u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_SUPPORTED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetSuppported"
#endif
    },
    {               /*  [6] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x848u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetConnectState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x848u)
        /*flags=*/      0x848u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730108u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_CONNECT_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetConnectState"
#endif
    },
    {               /*  [7] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHotplugConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730109u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_SET_HOTPLUG_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHotplugConfig"
#endif
    },
    {               /*  [8] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHotplugState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73010au,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_HOTPLUG_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHotplugState"
#endif
    },
    {               /*  [9] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHeadRoutingMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73010bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_HEAD_ROUTING_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHeadRoutingMap"
#endif
    },
    {               /*  [10] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetActive_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73010cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetActive"
#endif
    },
    {               /*  [11] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetAcpiIdMap_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730115u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_ACPI_ID_MAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetAcpiIdMap"
#endif
    },
    {               /*  [12] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x82004au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetInternalDisplays_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x82004au)
        /*flags=*/      0x82004au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730116u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_INTERNAL_DISPLAYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetInternalDisplays"
#endif
    },
    {               /*  [13] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemAcpiSubsystemActivated_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730117u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_ACPI_SUBSYSTEM_ACTIVATED_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemAcpiSubsystemActivated"
#endif
    },
    {               /*  [14] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetConnectorTable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73011du,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_CONNECTOR_TABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetConnectorTable"
#endif
    },
    {               /*  [15] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetBootDisplays_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73011eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_BOOT_DISPLAYS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetBootDisplays"
#endif
    },
    {               /*  [16] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemExecuteAcpiMethod_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
        /*flags=*/      0xau,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730120u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_EXECUTE_ACPI_METHOD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemExecuteAcpiMethod"
#endif
    },
    {               /*  [17] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemVrrDisplayInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73012cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_VRR_DISPLAY_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemVrrDisplayInfo"
#endif
    },
    {               /*  [18] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHotplugUnplugState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73012du,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHotplugUnplugState"
#endif
    },
    {               /*  [19] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdClearELVBlock_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73012eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_CLEAR_ELV_BLOCK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdClearELVBlock"
#endif
    },
    {               /*  [20] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemArmLightweightSupervisor_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73012fu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_ARM_LIGHTWEIGHT_SUPERVISOR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemArmLightweightSupervisor"
#endif
    },
    {               /*  [21] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemConfigVrrPstateSwitch_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730134u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_CONFIG_VRR_PSTATE_SWITCH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemConfigVrrPstateSwitch"
#endif
    },
    {               /*  [22] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73013du,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_QUERY_DISPLAY_IDS_WITH_MUX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemQueryDisplayIdsWithMux"
#endif
    },
    {               /*  [23] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemAllocateDisplayBandwidth_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730143u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemAllocateDisplayBandwidth"
#endif
    },
    {               /*  [24] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetHotplugEventConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730144u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetHotplugEventConfig"
#endif
    },
    {               /*  [25] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemSetHotplugEventConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730145u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_HOTPLUG_EVENT_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemSetHotplugEventConfig"
#endif
    },
    {               /*  [26] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpRecordChannelRegisters_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73014au,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_RECORD_CHANNEL_REGS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpRecordChannelRegisters"
#endif
    },
    {               /*  [27] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemCheckSidebandI2cSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73014bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_I2C_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemCheckSidebandI2cSupport"
#endif
    },
    {               /*  [28] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemCheckSidebandSrSupport_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73014cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_CHECK_SIDEBAND_SR_SUPPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemCheckSidebandSrSupport"
#endif
    },
    {               /*  [29] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdVRRSetRgLineActive_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73014du,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdVRRSetRgLineActive"
#endif
    },
    {               /*  [30] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetLoadVCounterInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730154u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_GET_LOADV_COUNTER_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetLoadVCounterInfo"
#endif
    },
    {               /*  [31] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemInternalAllocateDisplayBandwidth_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730157u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_INTERNAL_ALLOCATE_DISPLAY_BANDWIDTH_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemInternalAllocateDisplayBandwidth"
#endif
    },
    {               /*  [32] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemNotifyDrrMscgWar_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730159u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_NOTIFY_DRR_MSCG_WAR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemNotifyDrrMscgWar"
#endif
    },
    {               /*  [33] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSystemGetCrashLockCounterInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
        /*flags=*/      0x8u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730160u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_GET_CRASH_LOCK_COUNTER_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSystemGetCrashLockCounterInfo"
#endif
    },
    {               /*  [34] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetI2cPortid_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730211u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_I2C_PORTID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetI2cPortid"
#endif
    },
    {               /*  [35] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x820046u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetType_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x820046u)
        /*flags=*/      0x820046u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730240u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_TYPE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetType"
#endif
    },
    {               /*  [36] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificFakeDevice_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730243u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SPECIFIC_FAKE_DEVICE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificFakeDevice"
#endif
    },
    {               /*  [37] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetEdidV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730245u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetEdidV2"
#endif
    },
    {               /*  [38] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetEdidV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730246u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_EDID_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetEdidV2"
#endif
    },
    {               /*  [39] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetConnectorData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730250u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_CONNECTOR_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetConnectorData"
#endif
    },
    {               /*  [40] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiEnable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730273u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_HDMI_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiEnable"
#endif
    },
    {               /*  [41] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificCtrlHdmi_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730274u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_CTRL_HDMI_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificCtrlHdmi"
#endif
    },
    {               /*  [42] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730275u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SPECIFIC_SET_HDMI_AUDIO_MUTESTREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiAudioMutestream"
#endif
    },
    {               /*  [43] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetAcpiIdMapping_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730284u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_ACPI_ID_MAPPING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetAcpiIdMapping"
#endif
    },
    {               /*  [44] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730285u,
        /*paramSize=*/  sizeof(NV0073_CTRL_GET_ACPI_DOD_DISPLAY_PORT_ATTACHMENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment"
#endif
    },
    {               /*  [45] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetAllHeadMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730287u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetAllHeadMask"
#endif
    },
    {               /*  [46] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetOdPacket_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730288u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_OD_PACKET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetOdPacket"
#endif
    },
    {               /*  [47] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetOdPacketCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730289u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_OD_PACKET_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetOdPacketCtrl"
#endif
    },
    {               /*  [48] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetPclkLimit_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73028au,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_PCLK_LIMIT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetPclkLimit"
#endif
    },
    {               /*  [49] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x46u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificOrGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x46u)
        /*flags=*/      0x46u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73028bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_OR_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificOrGetInfo"
#endif
    },
    {               /*  [50] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetBacklightBrightness_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730291u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetBacklightBrightness"
#endif
    },
    {               /*  [51] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetBacklightBrightness_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730292u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_BACKLIGHT_BRIGHTNESS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetBacklightBrightness"
#endif
    },
    {               /*  [52] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiSinkCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730293u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_HDMI_SINK_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiSinkCaps"
#endif
    },
    {               /*  [53] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetMonitorPower_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730295u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_MONITOR_POWER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetMonitorPower"
#endif
    },
    {               /*  [54] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73029au,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_HDMI_FRL_LINK_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig"
#endif
    },
    {               /*  [55] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificApplyEdidOverrideV2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a1u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificApplyEdidOverrideV2"
#endif
    },
    {               /*  [56] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetHdmiGpuCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a2u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_GPU_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetHdmiGpuCaps"
#endif
    },
    {               /*  [57] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificDisplayChange_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a4u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_DISPLAY_CHANGE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificDisplayChange"
#endif
    },
    {               /*  [58] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetHdmiScdcData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a6u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_SCDC_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetHdmiScdcData"
#endif
    },
    {               /*  [59] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificIsDirectmodeDisplay_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a7u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_IS_DIRECTMODE_DISPLAY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificIsDirectmodeDisplay"
#endif
    },
    {               /*  [60] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a8u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_HDMI_FRL_CAPACITY_COMPUTATION_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation"
#endif
    },
    {               /*  [61] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificSetSharedGenericPacket_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302a9u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_SET_SHARED_GENERIC_PACKET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificSetSharedGenericPacket"
#endif
    },
    {               /*  [62] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302aau,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_ACQUIRE_SHARED_GENERIC_PACKET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificAcquireSharedGenericPacket"
#endif
    },
    {               /*  [63] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302abu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_RELEASE_SHARED_GENERIC_PACKET_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificReleaseSharedGenericPacket"
#endif
    },
    {               /*  [64] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificDispI2cReadWrite_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302acu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_DISP_I2C_READ_WRITE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificDispI2cReadWrite"
#endif
    },
    {               /*  [65] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302adu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_GET_VALID_HEAD_WINDOW_ASSIGNMENT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment"
#endif
    },
    {               /*  [66] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x7302aeu,
        /*paramSize=*/  sizeof(NV0073_CTRL_SPECIFIC_DEFAULT_ADAPTIVESYNC_DISPLAY_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay"
#endif
    },
    {               /*  [67] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdInternalGetHotplugUnplugState_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730401u,
        /*paramSize=*/  sizeof(NV0073_CTRL_SYSTEM_GET_HOTPLUG_UNPLUG_STATE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdInternalGetHotplugUnplugState"
#endif
    },
    {               /*  [68] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdInternalVRRSetRgLineActive_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730402u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_SYSTEM_VRR_SET_RGLINE_ACTIVE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdInternalVRRSetRgLineActive"
#endif
    },
    {               /*  [69] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdInternalDfpGetDispMuxStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730404u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdInternalDfpGetDispMuxStatus"
#endif
    },
    {               /*  [70] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdInternalDfpSwitchDispMux_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
        /*flags=*/      0xc0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730460u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdInternalDfpSwitchDispMux"
#endif
    },
    {               /*  [71] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdFrlConfigMacroPad_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x730502u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_FRL_CONFIG_MACRO_PAD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdFrlConfigMacroPad"
#endif
    },
    {               /*  [72] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*flags=*/      0x4au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731140u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetInfo"
#endif
    },
    {               /*  [73] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetDisplayportDongleInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731142u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_DISPLAYPORT_DONGLE_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetDisplayportDongleInfo"
#endif
    },
    {               /*  [74] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpSetEldAudioCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731144u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_SET_ELD_AUDIO_CAP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpSetEldAudioCaps"
#endif
    },
    {               /*  [75] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetSpreadSpectrum_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73114cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_SPREAD_SPECTRUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetSpreadSpectrum"
#endif
    },
    {               /*  [76] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpUpdateDynamicDfpCache_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73114eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpUpdateDynamicDfpCache"
#endif
    },
    {               /*  [77] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpSetAudioEnable_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731150u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_SET_AUDIO_ENABLE_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpSetAudioEnable"
#endif
    },
    {               /*  [78] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpAssignSor_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731152u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_ASSIGN_SOR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpAssignSor"
#endif
    },
    {               /*  [79] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetPadlinkMask_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731153u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_PADLINK_MASK_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetPadlinkMask"
#endif
    },
    {               /*  [80] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetLcdGpioPinNum_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731154u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_LCD_GPIO_PIN_NUM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetLcdGpioPinNum"
#endif
    },
    {               /*  [81] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpConfigTwoHeadOneOr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731156u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_CONFIG_TWO_HEAD_ONE_OR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpConfigTwoHeadOneOr"
#endif
    },
    {               /*  [82] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpDscCrcControl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731157u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpDscCrcControl"
#endif
    },
    {               /*  [83] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpInitMuxData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731158u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_INIT_MUX_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpInitMuxData"
#endif
    },
    {               /*  [84] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpSwitchDispMux_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731160u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_SWITCH_DISP_MUX_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpSwitchDispMux"
#endif
    },
    {               /*  [85] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpRunPreDispMuxOperations_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731161u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_RUN_PRE_DISP_MUX_OPERATIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpRunPreDispMuxOperations"
#endif
    },
    {               /*  [86] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpRunPostDispMuxOperations_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731162u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_RUN_POST_DISP_MUX_OPERATIONS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpRunPostDispMuxOperations"
#endif
    },
    {               /*  [87] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetDispMuxStatus_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731163u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetDispMuxStatus"
#endif
    },
    {               /*  [88] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetDsiModeTiming_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731166u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DFP_GET_DSI_MODE_TIMING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetDsiModeTiming"
#endif
    },
    {               /*  [89] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x42u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpGetFixedModeTiming_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x42u)
        /*flags=*/      0x42u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731172u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_GET_FIXED_MODE_TIMING_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpGetFixedModeTiming"
#endif
    },
    {               /*  [90] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpEdpDriverUnload_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
        /*flags=*/      0x4au,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731176u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_EDP_DRIVER_UNLOAD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpEdpDriverUnload"
#endif
    },
    {               /*  [91] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpSetForceBlackPixels_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
        /*flags=*/      0x48u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731179u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DFP_SET_FORCE_BLACK_PIXELS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpSetForceBlackPixels"
#endif
    },
    {               /*  [92] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpAuxchCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*flags=*/      0x844u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731341u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_AUXCH_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpAuxchCtrl"
#endif
    },
    {               /*  [93] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*flags=*/      0x844u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731343u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpCtrl"
#endif
    },
    {               /*  [94] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetLaneData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731345u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_LANE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetLaneData"
#endif
    },
    {               /*  [95] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetLaneData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731346u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_LANE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetLaneData"
#endif
    },
    {               /*  [96] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetTestpattern_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731347u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetTestpattern"
#endif
    },
    {               /*  [97] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetTestpattern_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731348u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_TESTPATTERN_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetTestpattern"
#endif
    },
    {               /*  [98] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731351u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_SET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data"
#endif
    },
    {               /*  [99] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731352u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_PREEMPHASIS_DRIVECURRENT_POSTCURSOR2_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data"
#endif
    },
    {               /*  [100] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpMainLinkCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731356u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpMainLinkCtrl"
#endif
    },
    {               /*  [101] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetAudioMuteStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731359u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_SET_AUDIO_MUTESTREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetAudioMuteStream"
#endif
    },
    {               /*  [102] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpASSRCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73135au,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_ASSR_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpASSRCtrl"
#endif
    },
    {               /*  [103] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpTopologyAllocateDisplayId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73135bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpTopologyAllocateDisplayId"
#endif
    },
    {               /*  [104] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpTopologyFreeDisplayId_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73135cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpTopologyFreeDisplayId"
#endif
    },
    {               /*  [105] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetLinkConfig_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731360u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetLinkConfig"
#endif
    },
    {               /*  [106] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetEDPData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731361u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_EDP_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetEDPData"
#endif
    },
    {               /*  [107] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731362u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigStream"
#endif
    },
    {               /*  [108] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetRateGov_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731363u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetRateGov"
#endif
    },
    {               /*  [109] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetManualDisplayPort_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731365u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetManualDisplayPort"
#endif
    },
    {               /*  [110] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetEcf_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731366u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_ECF_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetEcf"
#endif
    },
    {               /*  [111] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSendACT_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731367u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSendACT"
#endif
    },
    {               /*  [112] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x46u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetCaps_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x46u)
        /*flags=*/      0x46u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731369u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetCaps"
#endif
    },
    {               /*  [113] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGenerateFakeInterrupt_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
        /*flags=*/      0x4u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73136bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_GENERATE_FAKE_INTERRUPT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGenerateFakeInterrupt"
#endif
    },
    {               /*  [114] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigRadScratchReg_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73136cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigRadScratchReg"
#endif
    },
    {               /*  [115] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigSingleHeadMultiStream_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73136eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigSingleHeadMultiStream"
#endif
    },
    {               /*  [116] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetTriggerSelect_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73136fu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetTriggerSelect"
#endif
    },
    {               /*  [117] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetTriggerAll_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731370u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetTriggerAll"
#endif
    },
    {               /*  [118] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpRetrieveDpRingBuffer_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
        /*flags=*/      0x0u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731371u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_RETRIEVE_DP_RING_BUFFER_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpRetrieveDpRingBuffer"
#endif
    },
    {               /*  [119] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetAuxLogData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731373u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_GET_AUXLOGGER_BUFFER_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetAuxLogData"
#endif
    },
    {               /*  [120] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigIndexedLinkRates_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731377u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigIndexedLinkRates"
#endif
    },
    {               /*  [121] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetStereoMSAProperties_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731378u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetStereoMSAProperties"
#endif
    },
    {               /*  [122] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigureFec_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137au,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigureFec"
#endif
    },
    {               /*  [123] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpConfigMacroPad_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137bu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_CONFIG_MACRO_PAD_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpConfigMacroPad"
#endif
    },
    {               /*  [124] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpAuxchI2cTransferCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_AUXCH_I2C_TRANSFER_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpAuxchI2cTransferCtrl"
#endif
    },
    {               /*  [125] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpEnableVrr_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*flags=*/      0x844u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137du,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpEnableVrr"
#endif
    },
    {               /*  [126] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetGenericInfoframe_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137eu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_GENERIC_INFOFRAME_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetGenericInfoframe"
#endif
    },
    {               /*  [127] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetMsaAttributes_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73137fu,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_MSA_ATTRIBUTES_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetMsaAttributes"
#endif
    },
    {               /*  [128] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpInternalLcdOverdrive_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731380u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_AUXCH_OD_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpInternalLcdOverdrive"
#endif
    },
    {               /*  [129] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetMSAPropertiesv2_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731381u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_V2_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetMSAPropertiesv2"
#endif
    },
    {               /*  [130] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDp2xLinkTrain_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
        /*flags=*/      0x844u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731383u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP2X_LINK_TRAINING_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDp2xLinkTrain"
#endif
    },
    {               /*  [131] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDp2xGetLaneData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731384u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP2X_LANE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDp2xGetLaneData"
#endif
    },
    {               /*  [132] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDp2xSetLaneData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731385u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP2X_LANE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDp2xSetLaneData"
#endif
    },
    {               /*  [133] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDfpVariableBacklightCtrl_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
        /*flags=*/      0x40u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731386u,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_DP_AUXCH_VBL_CTRL_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDfpVariableBacklightCtrl"
#endif
    },
    {               /*  [134] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpSetLevelInfoTableData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731387u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_SET_LEVEL_INFO_TABLE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpSetLevelInfoTableData"
#endif
    },
    {               /*  [135] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDpGetLevelInfoTableData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731388u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_GET_LEVEL_INFO_TABLE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDpGetLevelInfoTableData"
#endif
    },
    {               /*  [136] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDp2xSetLevelInfoTableData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731389u,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP2X_SET_LEVEL_INFO_TABLE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDp2xSetLevelInfoTableData"
#endif
    },
    {               /*  [137] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDp2xGetLevelInfoTableData_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73138au,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP2X_GET_LEVEL_INFO_TABLE_DATA_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDp2xGetLevelInfoTableData"
#endif
    },
    {               /*  [138] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdCalculateDpImp_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100u)
        /*flags=*/      0x100u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73138cu,
        /*paramSize=*/  sizeof(NV0073_CTRL_CMD_CALCULATE_DP_IMP_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdCalculateDpImp"
#endif
    },
    {               /*  [139] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdDPGetCableIDInfoFromMacro_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x73138du,
        /*paramSize=*/  sizeof(NV0073_CTRL_DP_USBC_CABLEID_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdDPGetCableIDInfoFromMacro"
#endif
    },
    {               /*  [140] */
#if NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*pFunc=*/      (void (*)(void)) NULL,
#else
        /*pFunc=*/      (void (*)(void)) dispcmnCtrlCmdPsrGetSrPanelInfo_IMPL,
#endif // NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
        /*flags=*/      0x44u,
        /*accessRight=*/0x0u,
        /*methodId=*/   0x731602u,
        /*paramSize=*/  sizeof(NV0073_CTRL_PSR_GET_SR_PANEL_INFO_PARAMS),
        /*pClassInfo=*/ &(__nvoc_class_def_DispCommon.classInfo),
#if NV_PRINTF_STRINGS_ALLOWED
        /*func=*/       "dispcmnCtrlCmdPsrGetSrPanelInfo"
#endif
    },

};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__DispCommon __nvoc_metadata__DispCommon = {
    .rtti.pClassDef = &__nvoc_class_def_DispCommon,    // (dispcmn) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispCommon,
    .rtti.offset    = 0,
    .metadata__DisplayApi.rtti.pClassDef = &__nvoc_class_def_DisplayApi,    // (dispapi) super
    .metadata__DisplayApi.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.rtti.offset    = NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi),
    .metadata__DisplayApi.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__DisplayApi.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.rtti.offset    = NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource),
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__DisplayApi.metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super^2
    .metadata__DisplayApi.metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__Notifier.rtti.offset    = NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_Notifier),
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^3
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__dispcmnControl__ = &__nvoc_up_thunk_DisplayApi_dispcmnControl,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControl__ = &dispapiControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_DisplayApi_resControl,    // virtual
    .vtable.__dispcmnControl_Prologue__ = &__nvoc_up_thunk_DisplayApi_dispcmnControl_Prologue,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControl_Prologue__ = &dispapiControl_Prologue_IMPL,    // virtual override (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl_Prologue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Prologue,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__dispcmnControl_Epilogue__ = &__nvoc_up_thunk_DisplayApi_dispcmnControl_Epilogue,    // virtual inherited (dispapi) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControl_Epilogue__ = &dispapiControl_Epilogue_IMPL,    // virtual override (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &__nvoc_down_thunk_DisplayApi_rmresControl_Epilogue,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__dispcmnAccessCallback__ = &__nvoc_up_thunk_RmResource_dispcmnAccessCallback,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiAccessCallback__ = &__nvoc_up_thunk_RmResource_dispapiAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__dispcmnShareCallback__ = &__nvoc_up_thunk_RmResource_dispcmnShareCallback,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiShareCallback__ = &__nvoc_up_thunk_RmResource_dispapiShareCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresShareCallback__ = &rmresShareCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__dispcmnGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispcmnGetMemInterMapParams,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispapiGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__dispcmnCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispcmnCheckMemInterUnmap,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispapiCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__dispcmnGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispcmnGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispapiGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__dispcmnControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispcmnControlSerialization_Prologue,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__dispcmnControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispcmnControlSerialization_Epilogue,    // virtual inherited (rmres) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispapiControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__dispcmnCanCopy__ = &__nvoc_up_thunk_RsResource_dispcmnCanCopy,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiCanCopy__ = &__nvoc_up_thunk_RsResource_dispapiCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__dispcmnIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispcmnIsDuplicate,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispapiIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__dispcmnPreDestruct__ = &__nvoc_up_thunk_RsResource_dispcmnPreDestruct,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiPreDestruct__ = &__nvoc_up_thunk_RsResource_dispapiPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__dispcmnControlFilter__ = &__nvoc_up_thunk_RsResource_dispcmnControlFilter,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiControlFilter__ = &__nvoc_up_thunk_RsResource_dispapiControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__dispcmnMap__ = &__nvoc_up_thunk_RsResource_dispcmnMap,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiMap__ = &__nvoc_up_thunk_RsResource_dispapiMap,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &resMap_IMPL,    // virtual
    .vtable.__dispcmnUnmap__ = &__nvoc_up_thunk_RsResource_dispcmnUnmap,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiUnmap__ = &__nvoc_up_thunk_RsResource_dispapiUnmap,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &resUnmap_IMPL,    // virtual
    .vtable.__dispcmnIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispcmnIsPartialUnmapSupported,    // inline virtual inherited (res) base (dispapi) body
    .metadata__DisplayApi.vtable.__dispapiIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispapiIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__dispcmnMapTo__ = &__nvoc_up_thunk_RsResource_dispcmnMapTo,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiMapTo__ = &__nvoc_up_thunk_RsResource_dispapiMapTo,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__dispcmnUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispcmnUnmapFrom,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispapiUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__dispcmnGetRefCount__ = &__nvoc_up_thunk_RsResource_dispcmnGetRefCount,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetRefCount__ = &__nvoc_up_thunk_RsResource_dispapiGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__dispcmnAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispcmnAddAdditionalDependants,    // virtual inherited (res) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispapiAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__DisplayApi.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__DisplayApi.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__dispcmnGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispcmnGetNotificationListPtr,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__dispcmnGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispcmnGetNotificationShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__dispcmnSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispcmnSetNotificationShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispapiSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__dispcmnUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispcmnUnregisterEvent,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispapiUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__dispcmnGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispcmnGetOrAllocNotifShare,    // virtual inherited (notify) base (dispapi)
    .metadata__DisplayApi.vtable.__dispapiGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispapiGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__DisplayApi.metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DisplayApi.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DispCommon = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__DispCommon.rtti,    // [0]: (dispcmn) this
        &__nvoc_metadata__DispCommon.metadata__DisplayApi.rtti,    // [1]: (dispapi) super
        &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__Notifier.rtti,    // [6]: (notify) super^2
        &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^3
    }
};

// 26 up-thunk(s) defined to bridge methods in DispCommon to superclasses

// dispcmnControl: virtual inherited (dispapi) base (dispapi)
NV_STATUS __nvoc_up_thunk_DisplayApi_dispcmnControl(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return dispapiControl((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi)), pCallContext, pParams);
}

// dispcmnControl_Prologue: virtual inherited (dispapi) base (dispapi)
NV_STATUS __nvoc_up_thunk_DisplayApi_dispcmnControl_Prologue(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    return dispapiControl_Prologue((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi)), pCallContext, pRsParams);
}

// dispcmnControl_Epilogue: virtual inherited (dispapi) base (dispapi)
void __nvoc_up_thunk_DisplayApi_dispcmnControl_Epilogue(struct DispCommon *pDisplayApi, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pRsParams) {
    dispapiControl_Epilogue((struct DisplayApi *)(((unsigned char *) pDisplayApi) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi)), pCallContext, pRsParams);
}

// dispcmnAccessCallback: virtual inherited (rmres) base (dispapi)
NvBool __nvoc_up_thunk_RmResource_dispcmnAccessCallback(struct DispCommon *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispcmnShareCallback: virtual inherited (rmres) base (dispapi)
NvBool __nvoc_up_thunk_RmResource_dispcmnShareCallback(struct DispCommon *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return rmresShareCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispcmnGetMemInterMapParams: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispcmnGetMemInterMapParams(struct DispCommon *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pParams);
}

// dispcmnCheckMemInterUnmap: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispcmnCheckMemInterUnmap(struct DispCommon *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispcmnGetMemoryMappingDescriptor: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispcmnGetMemoryMappingDescriptor(struct DispCommon *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), ppMemDesc);
}

// dispcmnControlSerialization_Prologue: virtual inherited (rmres) base (dispapi)
NV_STATUS __nvoc_up_thunk_RmResource_dispcmnControlSerialization_Prologue(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispcmnControlSerialization_Epilogue: virtual inherited (rmres) base (dispapi)
void __nvoc_up_thunk_RmResource_dispcmnControlSerialization_Epilogue(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispcmnCanCopy: virtual inherited (res) base (dispapi)
NvBool __nvoc_up_thunk_RsResource_dispcmnCanCopy(struct DispCommon *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispcmnIsDuplicate: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnIsDuplicate(struct DispCommon *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispcmnPreDestruct: virtual inherited (res) base (dispapi)
void __nvoc_up_thunk_RsResource_dispcmnPreDestruct(struct DispCommon *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispcmnControlFilter: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnControlFilter(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispcmnMap: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnMap(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RS_CPU_MAP_PARAMS *pParams, RsCpuMapping *pCpuMapping) {
    return resMap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams, pCpuMapping);
}

// dispcmnUnmap: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnUnmap(struct DispCommon *pResource, struct CALL_CONTEXT *pCallContext, RsCpuMapping *pCpuMapping) {
    return resUnmap((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pCpuMapping);
}

// dispcmnIsPartialUnmapSupported: inline virtual inherited (res) base (dispapi) body
NvBool __nvoc_up_thunk_RsResource_dispcmnIsPartialUnmapSupported(struct DispCommon *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispcmnMapTo: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnMapTo(struct DispCommon *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispcmnUnmapFrom: virtual inherited (res) base (dispapi)
NV_STATUS __nvoc_up_thunk_RsResource_dispcmnUnmapFrom(struct DispCommon *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispcmnGetRefCount: virtual inherited (res) base (dispapi)
NvU32 __nvoc_up_thunk_RsResource_dispcmnGetRefCount(struct DispCommon *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispcmnAddAdditionalDependants: virtual inherited (res) base (dispapi)
void __nvoc_up_thunk_RsResource_dispcmnAddAdditionalDependants(struct RsClient *pClient, struct DispCommon *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispcmnGetNotificationListPtr: virtual inherited (notify) base (dispapi)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispcmnGetNotificationListPtr(struct DispCommon *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_Notifier)));
}

// dispcmnGetNotificationShare: virtual inherited (notify) base (dispapi)
struct NotifShare * __nvoc_up_thunk_Notifier_dispcmnGetNotificationShare(struct DispCommon *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_Notifier)));
}

// dispcmnSetNotificationShare: virtual inherited (notify) base (dispapi)
void __nvoc_up_thunk_Notifier_dispcmnSetNotificationShare(struct DispCommon *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_Notifier)), pNotifShare);
}

// dispcmnUnregisterEvent: virtual inherited (notify) base (dispapi)
NV_STATUS __nvoc_up_thunk_Notifier_dispcmnUnregisterEvent(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispcmnGetOrAllocNotifShare: virtual inherited (notify) base (dispapi)
NV_STATUS __nvoc_up_thunk_Notifier_dispcmnGetOrAllocNotifShare(struct DispCommon *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispCommon, __nvoc_base_DisplayApi.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DispCommon = 
{
    /*numEntries=*/     141,
    /*pExportEntries=*/ __nvoc_exported_method_def_DispCommon
};

void __nvoc_dtor_DisplayApi(DisplayApi*);
void __nvoc_dtor_DispCommon(DispCommon *pThis) {
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispCommon(DispCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DisplayApi(DisplayApi* , RmHalspecOwner* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_DispCommon(DispCommon *pThis, RmHalspecOwner *pRmhalspecowner, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispCommon_fail_DisplayApi;
    __nvoc_init_dataField_DispCommon(pThis);

    status = __nvoc_dispcmnConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispCommon_fail__init;
    goto __nvoc_ctor_DispCommon_exit; // Success

__nvoc_ctor_DispCommon_fail__init:
    __nvoc_dtor_DisplayApi(&pThis->__nvoc_base_DisplayApi);
__nvoc_ctor_DispCommon_fail_DisplayApi:
__nvoc_ctor_DispCommon_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DispCommon_1(DispCommon *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);

    // dispcmnCtrlCmdSystemGetVblankCounter -- exported (id=0x730105)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__dispcmnCtrlCmdSystemGetVblankCounter__ = &dispcmnCtrlCmdSystemGetVblankCounter_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetVblankEnable -- exported (id=0x730106)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSystemGetVblankEnable__ = &dispcmnCtrlCmdSystemGetVblankEnable_IMPL;
#endif

    // dispcmnCtrlCmdSystemCheckSidebandSrSupport -- exported (id=0x73014c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSystemCheckSidebandSrSupport__ = &dispcmnCtrlCmdSystemCheckSidebandSrSupport_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetHotplugState -- exported (id=0x73010a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSystemGetHotplugState__ = &dispcmnCtrlCmdSystemGetHotplugState_IMPL;
#endif

    // dispcmnCtrlCmdSystemNotifyDrrMscgWar -- exported (id=0x730159)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSystemNotifyDrrMscgWar__ = &dispcmnCtrlCmdSystemNotifyDrrMscgWar_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetInternalDisplays -- exported (id=0x730116)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x82004au)
    pThis->__dispcmnCtrlCmdSystemGetInternalDisplays__ = &dispcmnCtrlCmdSystemGetInternalDisplays_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetConnectorTable -- exported (id=0x73011d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSystemGetConnectorTable__ = &dispcmnCtrlCmdSystemGetConnectorTable_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetLoadVCounterInfo -- exported (id=0x730154)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispcmnCtrlCmdSystemGetLoadVCounterInfo__ = &dispcmnCtrlCmdSystemGetLoadVCounterInfo_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetCrashLockCounterInfo -- exported (id=0x730160)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x8u)
    pThis->__dispcmnCtrlCmdSystemGetCrashLockCounterInfo__ = &dispcmnCtrlCmdSystemGetCrashLockCounterInfo_IMPL;
#endif

    // dispcmnCtrlCmdSystemVrrDisplayInfo -- exported (id=0x73012c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSystemVrrDisplayInfo__ = &dispcmnCtrlCmdSystemVrrDisplayInfo_IMPL;
#endif

    // dispcmnCtrlCmdVRRSetRgLineActive -- exported (id=0x73014d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispcmnCtrlCmdVRRSetRgLineActive__ = &dispcmnCtrlCmdVRRSetRgLineActive_IMPL;
#endif

    // dispcmnCtrlCmdInternalVRRSetRgLineActive -- exported (id=0x730402)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__dispcmnCtrlCmdInternalVRRSetRgLineActive__ = &dispcmnCtrlCmdInternalVRRSetRgLineActive_IMPL;
#endif

    // dispcmnCtrlCmdDpEnableVrr -- exported (id=0x73137d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
    pThis->__dispcmnCtrlCmdDpEnableVrr__ = &dispcmnCtrlCmdDpEnableVrr_IMPL;
#endif

    // dispcmnCtrlCmdClearELVBlock -- exported (id=0x73012e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdClearELVBlock__ = &dispcmnCtrlCmdClearELVBlock_IMPL;
#endif

    // dispcmnCtrlCmdSpecificDisplayChange -- exported (id=0x7302a4)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificDisplayChange__ = &dispcmnCtrlCmdSpecificDisplayChange_IMPL;
#endif

    // dispcmnCtrlCmdDfpGetSpreadSpectrum -- exported (id=0x73114c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDfpGetSpreadSpectrum__ = &dispcmnCtrlCmdDfpGetSpreadSpectrum_IMPL;
#endif

    // dispcmnCtrlCmdDfpGetLcdGpioPinNum -- exported (id=0x731154)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDfpGetLcdGpioPinNum__ = &dispcmnCtrlCmdDfpGetLcdGpioPinNum_IMPL;
#endif

    // dispcmnCtrlCmdDpRetrieveDpRingBuffer -- exported (id=0x731371)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispcmnCtrlCmdDpRetrieveDpRingBuffer__ = &dispcmnCtrlCmdDpRetrieveDpRingBuffer_IMPL;
#endif

    // dispcmnCtrlCmdDpAuxchI2cTransferCtrl -- exported (id=0x73137c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDpAuxchI2cTransferCtrl__ = &dispcmnCtrlCmdDpAuxchI2cTransferCtrl_IMPL;
#endif

    // dispcmnCtrlCmdDpASSRCtrl -- exported (id=0x73135a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpASSRCtrl__ = &dispcmnCtrlCmdDpASSRCtrl_IMPL;
#endif

    // dispcmnCtrlCmdDpSetEcf -- exported (id=0x731366)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetEcf__ = &dispcmnCtrlCmdDpSetEcf_IMPL;
#endif

    // dispcmnCtrlCmdDfpRecordChannelRegisters -- exported (id=0x73014a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDfpRecordChannelRegisters__ = &dispcmnCtrlCmdDfpRecordChannelRegisters_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetBacklightBrightness -- exported (id=0x730291)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificGetBacklightBrightness__ = &dispcmnCtrlCmdSpecificGetBacklightBrightness_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetBacklightBrightness -- exported (id=0x730292)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificSetBacklightBrightness__ = &dispcmnCtrlCmdSpecificSetBacklightBrightness_IMPL;
#endif

    // dispcmnCtrlCmdPsrGetSrPanelInfo -- exported (id=0x731602)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdPsrGetSrPanelInfo__ = &dispcmnCtrlCmdPsrGetSrPanelInfo_IMPL;
#endif

    // dispcmnCtrlCmdDfpSwitchDispMux -- exported (id=0x731160)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__dispcmnCtrlCmdDfpSwitchDispMux__ = &dispcmnCtrlCmdDfpSwitchDispMux_IMPL;
#endif

    // dispcmnCtrlCmdInternalDfpSwitchDispMux -- exported (id=0x730460)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__dispcmnCtrlCmdInternalDfpSwitchDispMux__ = &dispcmnCtrlCmdInternalDfpSwitchDispMux_IMPL;
#endif

    // dispcmnCtrlCmdDfpRunPreDispMuxOperations -- exported (id=0x731161)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDfpRunPreDispMuxOperations__ = &dispcmnCtrlCmdDfpRunPreDispMuxOperations_IMPL;
#endif

    // dispcmnCtrlCmdDfpRunPostDispMuxOperations -- exported (id=0x731162)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDfpRunPostDispMuxOperations__ = &dispcmnCtrlCmdDfpRunPostDispMuxOperations_IMPL;
#endif

    // dispcmnCtrlCmdDfpGetDispMuxStatus -- exported (id=0x731163)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispcmnCtrlCmdDfpGetDispMuxStatus__ = &dispcmnCtrlCmdDfpGetDispMuxStatus_IMPL;
#endif

    // dispcmnCtrlCmdInternalDfpGetDispMuxStatus -- exported (id=0x730404)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__dispcmnCtrlCmdInternalDfpGetDispMuxStatus__ = &dispcmnCtrlCmdInternalDfpGetDispMuxStatus_IMPL;
#endif

    // dispcmnCtrlCmdDfpInternalLcdOverdrive -- exported (id=0x731380)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDfpInternalLcdOverdrive__ = &dispcmnCtrlCmdDfpInternalLcdOverdrive_IMPL;
#endif

    // dispcmnCtrlCmdDfpVariableBacklightCtrl -- exported (id=0x731386)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDfpVariableBacklightCtrl__ = &dispcmnCtrlCmdDfpVariableBacklightCtrl_IMPL;
#endif

    // dispcmnCtrlCmdSystemExecuteAcpiMethod -- exported (id=0x730120)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xau)
    pThis->__dispcmnCtrlCmdSystemExecuteAcpiMethod__ = &dispcmnCtrlCmdSystemExecuteAcpiMethod_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetAcpiIdMap -- exported (id=0x730115)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSystemGetAcpiIdMap__ = &dispcmnCtrlCmdSystemGetAcpiIdMap_IMPL;
#endif

    // dispcmnCtrlCmdSystemAcpiSubsystemActivated -- exported (id=0x730117)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__dispcmnCtrlCmdSystemAcpiSubsystemActivated__ = &dispcmnCtrlCmdSystemAcpiSubsystemActivated_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetAcpiIdMapping -- exported (id=0x730284)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__dispcmnCtrlCmdSpecificSetAcpiIdMapping__ = &dispcmnCtrlCmdSpecificSetAcpiIdMapping_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment -- exported (id=0x730285)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment__ = &dispcmnCtrlCmdSpecificGetAcpiDodDisplayPortAttachment_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetCapsV2 -- exported (id=0x730101)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSystemGetCapsV2__ = &dispcmnCtrlCmdSystemGetCapsV2_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetNumHeads -- exported (id=0x730102)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
    pThis->__dispcmnCtrlCmdSystemGetNumHeads__ = &dispcmnCtrlCmdSystemGetNumHeads_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetScanline -- exported (id=0x730104)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSystemGetScanline__ = &dispcmnCtrlCmdSystemGetScanline_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetSuppported -- exported (id=0x730107)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x82004au)
    pThis->__dispcmnCtrlCmdSystemGetSuppported__ = &dispcmnCtrlCmdSystemGetSuppported_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetConnectState -- exported (id=0x730108)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x848u)
    pThis->__dispcmnCtrlCmdSystemGetConnectState__ = &dispcmnCtrlCmdSystemGetConnectState_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetHotplugUnplugState -- exported (id=0x73012d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispcmnCtrlCmdSystemGetHotplugUnplugState__ = &dispcmnCtrlCmdSystemGetHotplugUnplugState_IMPL;
#endif

    // dispcmnCtrlCmdInternalGetHotplugUnplugState -- exported (id=0x730401)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__dispcmnCtrlCmdInternalGetHotplugUnplugState__ = &dispcmnCtrlCmdInternalGetHotplugUnplugState_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetHeadRoutingMap -- exported (id=0x73010b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSystemGetHeadRoutingMap__ = &dispcmnCtrlCmdSystemGetHeadRoutingMap_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetActive -- exported (id=0x73010c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSystemGetActive__ = &dispcmnCtrlCmdSystemGetActive_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetBootDisplays -- exported (id=0x73011e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSystemGetBootDisplays__ = &dispcmnCtrlCmdSystemGetBootDisplays_IMPL;
#endif

    // dispcmnCtrlCmdSystemQueryDisplayIdsWithMux -- exported (id=0x73013d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSystemQueryDisplayIdsWithMux__ = &dispcmnCtrlCmdSystemQueryDisplayIdsWithMux_IMPL;
#endif

    // dispcmnCtrlCmdSystemCheckSidebandI2cSupport -- exported (id=0x73014b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSystemCheckSidebandI2cSupport__ = &dispcmnCtrlCmdSystemCheckSidebandI2cSupport_IMPL;
#endif

    // dispcmnCtrlCmdSystemAllocateDisplayBandwidth -- exported (id=0x730143)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x0u)
    pThis->__dispcmnCtrlCmdSystemAllocateDisplayBandwidth__ = &dispcmnCtrlCmdSystemAllocateDisplayBandwidth_IMPL;
#endif

    // dispcmnCtrlCmdSystemInternalAllocateDisplayBandwidth -- exported (id=0x730157)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0xc0u)
    pThis->__dispcmnCtrlCmdSystemInternalAllocateDisplayBandwidth__ = &dispcmnCtrlCmdSystemInternalAllocateDisplayBandwidth_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetHotplugConfig -- exported (id=0x730109)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSystemGetHotplugConfig__ = &dispcmnCtrlCmdSystemGetHotplugConfig_IMPL;
#endif

    // dispcmnCtrlCmdSystemGetHotplugEventConfig -- exported (id=0x730144)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSystemGetHotplugEventConfig__ = &dispcmnCtrlCmdSystemGetHotplugEventConfig_IMPL;
#endif

    // dispcmnCtrlCmdSystemSetHotplugEventConfig -- exported (id=0x730145)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSystemSetHotplugEventConfig__ = &dispcmnCtrlCmdSystemSetHotplugEventConfig_IMPL;
#endif

    // dispcmnCtrlCmdSystemArmLightweightSupervisor -- exported (id=0x73012f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSystemArmLightweightSupervisor__ = &dispcmnCtrlCmdSystemArmLightweightSupervisor_IMPL;
#endif

    // dispcmnCtrlCmdSystemConfigVrrPstateSwitch -- exported (id=0x730134)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSystemConfigVrrPstateSwitch__ = &dispcmnCtrlCmdSystemConfigVrrPstateSwitch_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetType -- exported (id=0x730240)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x820046u)
    pThis->__dispcmnCtrlCmdSpecificGetType__ = &dispcmnCtrlCmdSpecificGetType_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetEdidV2 -- exported (id=0x730245)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSpecificGetEdidV2__ = &dispcmnCtrlCmdSpecificGetEdidV2_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetEdidV2 -- exported (id=0x730246)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSpecificSetEdidV2__ = &dispcmnCtrlCmdSpecificSetEdidV2_IMPL;
#endif

    // dispcmnCtrlCmdSpecificFakeDevice -- exported (id=0x730243)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSpecificFakeDevice__ = &dispcmnCtrlCmdSpecificFakeDevice_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetConnectorData -- exported (id=0x730250)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSpecificGetConnectorData__ = &dispcmnCtrlCmdSpecificGetConnectorData_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetHdmiEnable -- exported (id=0x730273)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiEnable__ = &dispcmnCtrlCmdSpecificSetHdmiEnable_IMPL;
#endif

    // dispcmnCtrlCmdSpecificCtrlHdmi -- exported (id=0x730274)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSpecificCtrlHdmi__ = &dispcmnCtrlCmdSpecificCtrlHdmi_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetAllHeadMask -- exported (id=0x730287)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSpecificGetAllHeadMask__ = &dispcmnCtrlCmdSpecificGetAllHeadMask_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetOdPacket -- exported (id=0x730288)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificSetOdPacket__ = &dispcmnCtrlCmdSpecificSetOdPacket_IMPL;
#endif

    // dispcmnCtrlCmdSpecificAcquireSharedGenericPacket -- exported (id=0x7302aa)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificAcquireSharedGenericPacket__ = &dispcmnCtrlCmdSpecificAcquireSharedGenericPacket_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetSharedGenericPacket -- exported (id=0x7302a9)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificSetSharedGenericPacket__ = &dispcmnCtrlCmdSpecificSetSharedGenericPacket_IMPL;
#endif

    // dispcmnCtrlCmdSpecificReleaseSharedGenericPacket -- exported (id=0x7302ab)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificReleaseSharedGenericPacket__ = &dispcmnCtrlCmdSpecificReleaseSharedGenericPacket_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetOdPacketCtrl -- exported (id=0x730289)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificSetOdPacketCtrl__ = &dispcmnCtrlCmdSpecificSetOdPacketCtrl_IMPL;
#endif

    // dispcmnCtrlCmdSpecificOrGetInfo -- exported (id=0x73028b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x46u)
    pThis->__dispcmnCtrlCmdSpecificOrGetInfo__ = &dispcmnCtrlCmdSpecificOrGetInfo_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetPclkLimit -- exported (id=0x73028a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificGetPclkLimit__ = &dispcmnCtrlCmdSpecificGetPclkLimit_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetHdmiSinkCaps -- exported (id=0x730293)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiSinkCaps__ = &dispcmnCtrlCmdSpecificSetHdmiSinkCaps_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetMonitorPower -- exported (id=0x730295)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificSetMonitorPower__ = &dispcmnCtrlCmdSpecificSetMonitorPower_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig -- exported (id=0x73029a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig__ = &dispcmnCtrlCmdSpecificSetHdmiFrlLinkConfig_IMPL;
#endif

    // dispcmnCtrlCmdSpecificApplyEdidOverrideV2 -- exported (id=0x7302a1)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificApplyEdidOverrideV2__ = &dispcmnCtrlCmdSpecificApplyEdidOverrideV2_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetI2cPortid -- exported (id=0x730211)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSpecificGetI2cPortid__ = &dispcmnCtrlCmdSpecificGetI2cPortid_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetHdmiGpuCaps -- exported (id=0x7302a2)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificGetHdmiGpuCaps__ = &dispcmnCtrlCmdSpecificGetHdmiGpuCaps_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetHdmiScdcData -- exported (id=0x7302a6)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificGetHdmiScdcData__ = &dispcmnCtrlCmdSpecificGetHdmiScdcData_IMPL;
#endif

    // dispcmnCtrlCmdSpecificIsDirectmodeDisplay -- exported (id=0x7302a7)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificIsDirectmodeDisplay__ = &dispcmnCtrlCmdSpecificIsDirectmodeDisplay_IMPL;
#endif

    // dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay -- exported (id=0x7302ae)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay__ = &dispcmnCtrlCmdSpecificDefaultAdaptivesyncDisplay_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation -- exported (id=0x7302a8)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation__ = &dispcmnCtrlCmdSpecificSetHdmiFrlCapacityComputation_IMPL;
#endif

    // dispcmnCtrlCmdSpecificDispI2cReadWrite -- exported (id=0x7302ac)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdSpecificDispI2cReadWrite__ = &dispcmnCtrlCmdSpecificDispI2cReadWrite_IMPL;
#endif

    // dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment -- exported (id=0x7302ad)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment__ = &dispcmnCtrlCmdSpecificGetValidHeadWindowAssignment_IMPL;
#endif

    // dispcmnCtrlCmdSpecificSetHdmiAudioMutestream -- exported (id=0x730275)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdSpecificSetHdmiAudioMutestream__ = &dispcmnCtrlCmdSpecificSetHdmiAudioMutestream_IMPL;
#endif

    // dispcmnCtrlCmdDfpEdpDriverUnload -- exported (id=0x731176)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
    pThis->__dispcmnCtrlCmdDfpEdpDriverUnload__ = &dispcmnCtrlCmdDfpEdpDriverUnload_IMPL;
#endif

    // dispcmnCtrlCmdDfpSetForceBlackPixels -- exported (id=0x731179)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdDfpSetForceBlackPixels__ = &dispcmnCtrlCmdDfpSetForceBlackPixels_IMPL;
#endif

    // dispcmnCtrlCmdDfpGetInfo -- exported (id=0x731140)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4au)
    pThis->__dispcmnCtrlCmdDfpGetInfo__ = &dispcmnCtrlCmdDfpGetInfo_IMPL;
#endif

    // dispcmnCtrlCmdDfpGetDisplayportDongleInfo -- exported (id=0x731142)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x48u)
    pThis->__dispcmnCtrlCmdDfpGetDisplayportDongleInfo__ = &dispcmnCtrlCmdDfpGetDisplayportDongleInfo_IMPL;
#endif

    // dispcmnCtrlCmdDfpSetEldAudioCaps -- exported (id=0x731144)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDfpSetEldAudioCaps__ = &dispcmnCtrlCmdDfpSetEldAudioCaps_IMPL;
#endif

    // dispcmnCtrlCmdDfpSetAudioEnable -- exported (id=0x731150)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDfpSetAudioEnable__ = &dispcmnCtrlCmdDfpSetAudioEnable_IMPL;
#endif

    // dispcmnCtrlCmdDfpUpdateDynamicDfpCache -- exported (id=0x73114e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDfpUpdateDynamicDfpCache__ = &dispcmnCtrlCmdDfpUpdateDynamicDfpCache_IMPL;
#endif

    // dispcmnCtrlCmdDfpAssignSor -- exported (id=0x731152)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDfpAssignSor__ = &dispcmnCtrlCmdDfpAssignSor_IMPL;
#endif

    // dispcmnCtrlCmdDfpDscCrcControl -- exported (id=0x731157)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDfpDscCrcControl__ = &dispcmnCtrlCmdDfpDscCrcControl_IMPL;
#endif

    // dispcmnCtrlCmdDfpInitMuxData -- exported (id=0x731158)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDfpInitMuxData__ = &dispcmnCtrlCmdDfpInitMuxData_IMPL;
#endif

    // dispcmnCtrlCmdDfpGetDsiModeTiming -- exported (id=0x731166)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDfpGetDsiModeTiming__ = &dispcmnCtrlCmdDfpGetDsiModeTiming_IMPL;
#endif

    // dispcmnCtrlCmdDfpConfigTwoHeadOneOr -- exported (id=0x731156)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDfpConfigTwoHeadOneOr__ = &dispcmnCtrlCmdDfpConfigTwoHeadOneOr_IMPL;
#endif

    // dispcmnCtrlCmdDfpGetPadlinkMask -- exported (id=0x731153)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDfpGetPadlinkMask__ = &dispcmnCtrlCmdDfpGetPadlinkMask_IMPL;
#endif

    // dispcmnCtrlCmdDfpGetFixedModeTiming -- exported (id=0x731172)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x42u)
    pThis->__dispcmnCtrlCmdDfpGetFixedModeTiming__ = &dispcmnCtrlCmdDfpGetFixedModeTiming_IMPL;
#endif

    // dispcmnCtrlCmdDpAuxchCtrl -- exported (id=0x731341)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
    pThis->__dispcmnCtrlCmdDpAuxchCtrl__ = &dispcmnCtrlCmdDpAuxchCtrl_IMPL;
#endif

    // dispcmnCtrlCmdDpCtrl -- exported (id=0x731343)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
    pThis->__dispcmnCtrlCmdDpCtrl__ = &dispcmnCtrlCmdDpCtrl_IMPL;
#endif

    // dispcmnCtrlCmdDp2xLinkTrain -- exported (id=0x731383)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x844u)
    pThis->__dispcmnCtrlCmdDp2xLinkTrain__ = &dispcmnCtrlCmdDp2xLinkTrain_IMPL;
#endif

    // dispcmnCtrlCmdDp2xGetLaneData -- exported (id=0x731384)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDp2xGetLaneData__ = &dispcmnCtrlCmdDp2xGetLaneData_IMPL;
#endif

    // dispcmnCtrlCmdDp2xSetLaneData -- exported (id=0x731385)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDp2xSetLaneData__ = &dispcmnCtrlCmdDp2xSetLaneData_IMPL;
#endif

    // dispcmnCtrlCmdCalculateDpImp -- exported (id=0x73138c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x100u)
    pThis->__dispcmnCtrlCmdCalculateDpImp__ = &dispcmnCtrlCmdCalculateDpImp_IMPL;
#endif

    // dispcmnCtrlCmdDpGetLaneData -- exported (id=0x731345)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpGetLaneData__ = &dispcmnCtrlCmdDpGetLaneData_IMPL;
#endif

    // dispcmnCtrlCmdDpSetLaneData -- exported (id=0x731346)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetLaneData__ = &dispcmnCtrlCmdDpSetLaneData_IMPL;
#endif

    // dispcmnCtrlCmdDpGetTestpattern -- exported (id=0x731348)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpGetTestpattern__ = &dispcmnCtrlCmdDpGetTestpattern_IMPL;
#endif

    // dispcmnCtrlCmdDpSetTestpattern -- exported (id=0x731347)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetTestpattern__ = &dispcmnCtrlCmdDpSetTestpattern_IMPL;
#endif

    // dispcmnCtrlCmdDpMainLinkCtrl -- exported (id=0x731356)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpMainLinkCtrl__ = &dispcmnCtrlCmdDpMainLinkCtrl_IMPL;
#endif

    // dispcmnCtrlCmdDpSetAudioMuteStream -- exported (id=0x731359)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x40u)
    pThis->__dispcmnCtrlCmdDpSetAudioMuteStream__ = &dispcmnCtrlCmdDpSetAudioMuteStream_IMPL;
#endif

    // dispcmnCtrlCmdDpGetLinkConfig -- exported (id=0x731360)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpGetLinkConfig__ = &dispcmnCtrlCmdDpGetLinkConfig_IMPL;
#endif

    // dispcmnCtrlCmdDpGetEDPData -- exported (id=0x731361)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpGetEDPData__ = &dispcmnCtrlCmdDpGetEDPData_IMPL;
#endif

    // dispcmnCtrlCmdDpTopologyAllocateDisplayId -- exported (id=0x73135b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpTopologyAllocateDisplayId__ = &dispcmnCtrlCmdDpTopologyAllocateDisplayId_IMPL;
#endif

    // dispcmnCtrlCmdDpTopologyFreeDisplayId -- exported (id=0x73135c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpTopologyFreeDisplayId__ = &dispcmnCtrlCmdDpTopologyFreeDisplayId_IMPL;
#endif

    // dispcmnCtrlCmdDpConfigStream -- exported (id=0x731362)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpConfigStream__ = &dispcmnCtrlCmdDpConfigStream_IMPL;
#endif

    // dispcmnCtrlCmdDpConfigSingleHeadMultiStream -- exported (id=0x73136e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpConfigSingleHeadMultiStream__ = &dispcmnCtrlCmdDpConfigSingleHeadMultiStream_IMPL;
#endif

    // dispcmnCtrlCmdDpSetRateGov -- exported (id=0x731363)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetRateGov__ = &dispcmnCtrlCmdDpSetRateGov_IMPL;
#endif

    // dispcmnCtrlCmdDpSendACT -- exported (id=0x731367)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSendACT__ = &dispcmnCtrlCmdDpSendACT_IMPL;
#endif

    // dispcmnCtrlCmdDpSetManualDisplayPort -- exported (id=0x731365)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetManualDisplayPort__ = &dispcmnCtrlCmdDpSetManualDisplayPort_IMPL;
#endif

    // dispcmnCtrlCmdDpGetCaps -- exported (id=0x731369)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x46u)
    pThis->__dispcmnCtrlCmdDpGetCaps__ = &dispcmnCtrlCmdDpGetCaps_IMPL;
#endif

    // dispcmnCtrlCmdDpSetMSAPropertiesv2 -- exported (id=0x731381)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetMSAPropertiesv2__ = &dispcmnCtrlCmdDpSetMSAPropertiesv2_IMPL;
#endif

    // dispcmnCtrlCmdDpSetStereoMSAProperties -- exported (id=0x731378)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetStereoMSAProperties__ = &dispcmnCtrlCmdDpSetStereoMSAProperties_IMPL;
#endif

    // dispcmnCtrlCmdDpGenerateFakeInterrupt -- exported (id=0x73136b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x4u)
    pThis->__dispcmnCtrlCmdDpGenerateFakeInterrupt__ = &dispcmnCtrlCmdDpGenerateFakeInterrupt_IMPL;
#endif

    // dispcmnCtrlCmdDpConfigRadScratchReg -- exported (id=0x73136c)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpConfigRadScratchReg__ = &dispcmnCtrlCmdDpConfigRadScratchReg_IMPL;
#endif

    // dispcmnCtrlCmdDpSetTriggerSelect -- exported (id=0x73136f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetTriggerSelect__ = &dispcmnCtrlCmdDpSetTriggerSelect_IMPL;
#endif

    // dispcmnCtrlCmdDpSetTriggerAll -- exported (id=0x731370)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetTriggerAll__ = &dispcmnCtrlCmdDpSetTriggerAll_IMPL;
#endif

    // dispcmnCtrlCmdDpGetAuxLogData -- exported (id=0x731373)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpGetAuxLogData__ = &dispcmnCtrlCmdDpGetAuxLogData_IMPL;
#endif

    // dispcmnCtrlCmdDpConfigIndexedLinkRates -- exported (id=0x731377)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpConfigIndexedLinkRates__ = &dispcmnCtrlCmdDpConfigIndexedLinkRates_IMPL;
#endif

    // dispcmnCtrlCmdDpConfigureFec -- exported (id=0x73137a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpConfigureFec__ = &dispcmnCtrlCmdDpConfigureFec_IMPL;
#endif

    // dispcmnCtrlCmdDpGetGenericInfoframe -- exported (id=0x73137e)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpGetGenericInfoframe__ = &dispcmnCtrlCmdDpGetGenericInfoframe_IMPL;
#endif

    // dispcmnCtrlCmdDpGetMsaAttributes -- exported (id=0x73137f)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpGetMsaAttributes__ = &dispcmnCtrlCmdDpGetMsaAttributes_IMPL;
#endif

    // dispcmnCtrlCmdFrlConfigMacroPad -- exported (id=0x730502)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdFrlConfigMacroPad__ = &dispcmnCtrlCmdFrlConfigMacroPad_IMPL;
#endif

    // dispcmnCtrlCmdDpConfigMacroPad -- exported (id=0x73137b)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpConfigMacroPad__ = &dispcmnCtrlCmdDpConfigMacroPad_IMPL;
#endif

    // dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data -- exported (id=0x731351)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data__ = &dispcmnCtrlCmdDpSetPreemphasisDrivecurrentPostcursor2Data_IMPL;
#endif

    // dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data -- exported (id=0x731352)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data__ = &dispcmnCtrlCmdDpGetPreemphasisDrivecurrentPostcursor2Data_IMPL;
#endif

    // dispcmnCtrlCmdDpSetLevelInfoTableData -- exported (id=0x731387)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpSetLevelInfoTableData__ = &dispcmnCtrlCmdDpSetLevelInfoTableData_IMPL;
#endif

    // dispcmnCtrlCmdDpGetLevelInfoTableData -- exported (id=0x731388)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDpGetLevelInfoTableData__ = &dispcmnCtrlCmdDpGetLevelInfoTableData_IMPL;
#endif

    // dispcmnCtrlCmdDp2xSetLevelInfoTableData -- exported (id=0x731389)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDp2xSetLevelInfoTableData__ = &dispcmnCtrlCmdDp2xSetLevelInfoTableData_IMPL;
#endif

    // dispcmnCtrlCmdDp2xGetLevelInfoTableData -- exported (id=0x73138a)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDp2xGetLevelInfoTableData__ = &dispcmnCtrlCmdDp2xGetLevelInfoTableData_IMPL;
#endif

    // dispcmnCtrlCmdDPGetCableIDInfoFromMacro -- exported (id=0x73138d)
#if !NVOC_EXPORTED_METHOD_DISABLED_BY_FLAG(0x44u)
    pThis->__dispcmnCtrlCmdDPGetCableIDInfoFromMacro__ = &dispcmnCtrlCmdDPGetCableIDInfoFromMacro_IMPL;
#endif
} // End __nvoc_init_funcTable_DispCommon_1 with approximately 141 basic block(s).


// Initialize vtable(s) for 167 virtual method(s).
void __nvoc_init_funcTable_DispCommon(DispCommon *pThis) {

    // Initialize vtable(s) with 141 per-object function pointer(s).
    __nvoc_init_funcTable_DispCommon_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DispCommon(DispCommon *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^3
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier;    // (notify) super^2
    pThis->__nvoc_pbase_DisplayApi = &pThis->__nvoc_base_DisplayApi;    // (dispapi) super
    pThis->__nvoc_pbase_DispCommon = pThis;    // (dispcmn) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__DisplayApi(&pThis->__nvoc_base_DisplayApi, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__Notifier.metadata__INotifier;    // (inotify) super^3
    pThis->__nvoc_base_DisplayApi.__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispCommon.metadata__DisplayApi.metadata__Notifier;    // (notify) super^2
    pThis->__nvoc_base_DisplayApi.__nvoc_metadata_ptr = &__nvoc_metadata__DispCommon.metadata__DisplayApi;    // (dispapi) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DispCommon;    // (dispcmn) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DispCommon(pThis);
}

NV_STATUS __nvoc_objCreate_DispCommon(DispCommon **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispCommon *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispCommon), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispCommon));

    pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__DispCommon(pThis, pRmhalspecowner);
    status = __nvoc_ctor_DispCommon(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispCommon_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispCommon_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_DisplayApi.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispCommon));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispCommon(DispCommon **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispCommon(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

