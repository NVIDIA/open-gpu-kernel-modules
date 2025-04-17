#define NVOC_DISP_CHANNEL_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_disp_channel_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xbd2ff3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;

// Forward declarations for DispChannel
void __nvoc_init__GpuResource(GpuResource*);
void __nvoc_init__Notifier(Notifier*);
void __nvoc_init__DispChannel(DispChannel*);
void __nvoc_init_funcTable_DispChannel(DispChannel*);
NV_STATUS __nvoc_ctor_DispChannel(DispChannel*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams, NvU32 arg_isDma);
void __nvoc_init_dataField_DispChannel(DispChannel*);
void __nvoc_dtor_DispChannel(DispChannel*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DispChannel;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DispChannel;

// Down-thunk(s) to bridge DispChannel methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this

// Up-thunk(s) to bridge DispChannel methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnControl(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnMap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnUnmap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_dispchnShareCallback(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnInternalControlForward(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle(struct DispChannel *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_dispchnAccessCallback(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControl_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchnControl_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchnCanCopy(struct DispChannel *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnIsDuplicate(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispchnPreDestruct(struct DispChannel *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnControlFilter(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported(struct DispChannel *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnMapTo(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnUnmapFrom(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispchnGetRefCount(struct DispChannel *pResource);    // this
void __nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr(struct DispChannel *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnGetNotificationShare(struct DispChannel *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispchnSetNotificationShare(struct DispChannel *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchnUnregisterEvent(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispChannel),
        /*classId=*/            classId(DispChannel),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispChannel",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispChannel,
    /*pCastInfo=*/          &__nvoc_castinfo__DispChannel,
    /*pExportInfo=*/        &__nvoc_export_info__DispChannel
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__DispChannel __nvoc_metadata__DispChannel = {
    .rtti.pClassDef = &__nvoc_class_def_DispChannel,    // (dispchn) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispChannel,
    .rtti.offset    = 0,
    .metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super
    .metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.rtti.offset    = NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource),
    .metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^2
    .metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^4
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^3
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super
    .metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.rtti.offset    = NV_OFFSETOF(DispChannel, __nvoc_base_Notifier),
    .metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^2
    .metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(DispChannel, __nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__dispchnGetRegBaseOffsetAndSize__ = &dispchnGetRegBaseOffsetAndSize_IMPL,    // virtual override (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize,    // virtual
    .vtable.__dispchnControl__ = &__nvoc_up_thunk_GpuResource_dispchnControl,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__dispchnMap__ = &__nvoc_up_thunk_GpuResource_dispchnMap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__dispchnUnmap__ = &__nvoc_up_thunk_GpuResource_dispchnUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__dispchnShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchnShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__dispchnGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__dispchnInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchnInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__dispchnGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__dispchnAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchnAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__dispchnGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__dispchnCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__dispchnGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__dispchnControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__dispchnControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__dispchnControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__dispchnControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__dispchnCanCopy__ = &__nvoc_up_thunk_RsResource_dispchnCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__dispchnIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchnIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__dispchnPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchnPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__dispchnControlFilter__ = &__nvoc_up_thunk_RsResource_dispchnControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__dispchnIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__dispchnMapTo__ = &__nvoc_up_thunk_RsResource_dispchnMapTo,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__dispchnUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchnUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__dispchnGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchnGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__dispchnAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__dispchnGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__dispchnGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__dispchnSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__dispchnUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchnUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__dispchnGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DispChannel = {
    .numRelatives = 8,
    .relatives = {
        &__nvoc_metadata__DispChannel.rtti,    // [0]: (dispchn) this
        &__nvoc_metadata__DispChannel.metadata__GpuResource.rtti,    // [1]: (gpures) super
        &__nvoc_metadata__DispChannel.metadata__GpuResource.metadata__RmResource.rtti,    // [2]: (rmres) super^2
        &__nvoc_metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [3]: (res) super^3
        &__nvoc_metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [4]: (obj) super^4
        &__nvoc_metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [5]: (rmrescmn) super^3
        &__nvoc_metadata__DispChannel.metadata__Notifier.rtti,    // [6]: (notify) super
        &__nvoc_metadata__DispChannel.metadata__Notifier.metadata__INotifier.rtti,    // [7]: (inotify) super^2
    }
};

// 1 down-thunk(s) defined to bridge methods in DispChannel from superclasses

// dispchnGetRegBaseOffsetAndSize: virtual override (gpures) base (gpures)
NV_STATUS __nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispchnGetRegBaseOffsetAndSize((struct DispChannel *)(((unsigned char *) pDispChannel) - NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}


// 29 up-thunk(s) defined to bridge methods in DispChannel to superclasses

// dispchnControl: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnControl(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pCallContext, pParams);
}

// dispchnMap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnMap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// dispchnUnmap: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnUnmap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// dispchnShareCallback: virtual inherited (gpures) base (gpures)
NvBool __nvoc_up_thunk_GpuResource_dispchnShareCallback(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispchnGetMapAddrSpace: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// dispchnInternalControlForward: virtual inherited (gpures) base (gpures)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnInternalControlForward(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)), command, pParams, size);
}

// dispchnGetInternalObjectHandle: virtual inherited (gpures) base (gpures)
NvHandle __nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle(struct DispChannel *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource)));
}

// dispchnAccessCallback: virtual inherited (rmres) base (gpures)
NvBool __nvoc_up_thunk_RmResource_dispchnAccessCallback(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispchnGetMemInterMapParams: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// dispchnCheckMemInterUnmap: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispchnGetMemoryMappingDescriptor: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// dispchnControlSerialization_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnControlSerialization_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnControl_Prologue: virtual inherited (rmres) base (gpures)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControl_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnControl_Epilogue: virtual inherited (rmres) base (gpures)
void __nvoc_up_thunk_RmResource_dispchnControl_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnCanCopy: virtual inherited (res) base (gpures)
NvBool __nvoc_up_thunk_RsResource_dispchnCanCopy(struct DispChannel *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnIsDuplicate: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnIsDuplicate(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispchnPreDestruct: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_dispchnPreDestruct(struct DispChannel *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnControlFilter: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnControlFilter(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispchnIsPartialUnmapSupported: inline virtual inherited (res) base (gpures) body
NvBool __nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported(struct DispChannel *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnMapTo: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnMapTo(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchnUnmapFrom: virtual inherited (res) base (gpures)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnUnmapFrom(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchnGetRefCount: virtual inherited (res) base (gpures)
NvU32 __nvoc_up_thunk_RsResource_dispchnGetRefCount(struct DispChannel *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnAddAdditionalDependants: virtual inherited (res) base (gpures)
void __nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannel, __nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispchnGetNotificationListPtr: virtual inherited (notify) base (notify)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr(struct DispChannel *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)));
}

// dispchnGetNotificationShare: virtual inherited (notify) base (notify)
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnGetNotificationShare(struct DispChannel *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)));
}

// dispchnSetNotificationShare: virtual inherited (notify) base (notify)
void __nvoc_up_thunk_Notifier_dispchnSetNotificationShare(struct DispChannel *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)), pNotifShare);
}

// dispchnUnregisterEvent: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_dispchnUnregisterEvent(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispchnGetOrAllocNotifShare: virtual inherited (notify) base (notify)
NV_STATUS __nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannel, __nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DispChannel = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_GpuResource(GpuResource*);
void __nvoc_dtor_Notifier(Notifier*);
void __nvoc_dtor_DispChannel(DispChannel *pThis) {
    __nvoc_dispchnDestruct(pThis);
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispChannel(DispChannel *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_GpuResource(GpuResource* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
NV_STATUS __nvoc_ctor_Notifier(Notifier* , struct CALL_CONTEXT *);
NV_STATUS __nvoc_ctor_DispChannel(DispChannel *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, NvU32 arg_isDma) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_GpuResource(&pThis->__nvoc_base_GpuResource, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispChannel_fail_GpuResource;
    status = __nvoc_ctor_Notifier(&pThis->__nvoc_base_Notifier, arg_pCallContext);
    if (status != NV_OK) goto __nvoc_ctor_DispChannel_fail_Notifier;
    __nvoc_init_dataField_DispChannel(pThis);

    status = __nvoc_dispchnConstruct(pThis, arg_pCallContext, arg_pParams, arg_isDma);
    if (status != NV_OK) goto __nvoc_ctor_DispChannel_fail__init;
    goto __nvoc_ctor_DispChannel_exit; // Success

__nvoc_ctor_DispChannel_fail__init:
    __nvoc_dtor_Notifier(&pThis->__nvoc_base_Notifier);
__nvoc_ctor_DispChannel_fail_Notifier:
    __nvoc_dtor_GpuResource(&pThis->__nvoc_base_GpuResource);
__nvoc_ctor_DispChannel_fail_GpuResource:
__nvoc_ctor_DispChannel_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DispChannel_1(DispChannel *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DispChannel_1


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_DispChannel(DispChannel *pThis) {
    __nvoc_init_funcTable_DispChannel_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DispChannel(DispChannel *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^4
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^3
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^2
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_GpuResource;    // (gpures) super
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^2
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_Notifier;    // (notify) super
    pThis->__nvoc_pbase_DispChannel = pThis;    // (dispchn) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__GpuResource(&pThis->__nvoc_base_GpuResource);
    __nvoc_init__Notifier(&pThis->__nvoc_base_Notifier);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^4
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^3
    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannel.metadata__GpuResource.metadata__RmResource;    // (rmres) super^2
    pThis->__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannel.metadata__GpuResource;    // (gpures) super
    pThis->__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannel.metadata__Notifier.metadata__INotifier;    // (inotify) super^2
    pThis->__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannel.metadata__Notifier;    // (notify) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DispChannel;    // (dispchn) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DispChannel(pThis);
}

NV_STATUS __nvoc_objCreate_DispChannel(DispChannel **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams, NvU32 arg_isDma)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispChannel *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispChannel), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispChannel));

    pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__DispChannel(pThis);
    status = __nvoc_ctor_DispChannel(pThis, arg_pCallContext, arg_pParams, arg_isDma);
    if (status != NV_OK) goto __nvoc_objCreate_DispChannel_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispChannel_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispChannel));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispChannel(DispChannel **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);
    NvU32 arg_isDma = va_arg(args, NvU32);

    status = __nvoc_objCreate_DispChannel(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams, arg_isDma);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x10dec3 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelPio;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel;

// Forward declarations for DispChannelPio
void __nvoc_init__DispChannel(DispChannel*);
void __nvoc_init__DispChannelPio(DispChannelPio*);
void __nvoc_init_funcTable_DispChannelPio(DispChannelPio*);
NV_STATUS __nvoc_ctor_DispChannelPio(DispChannelPio*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_DispChannelPio(DispChannelPio*);
void __nvoc_dtor_DispChannelPio(DispChannelPio*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DispChannelPio;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DispChannelPio;

// Down-thunk(s) to bridge DispChannelPio methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super^2
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super^2
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // super

// Up-thunk(s) to bridge DispChannelPio methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super^2
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnControl(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnMap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnUnmap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_GpuResource_dispchnShareCallback(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnInternalControlForward(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // super
NvHandle __nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle(struct DispChannel *pGpuResource);    // super
NvBool __nvoc_up_thunk_RmResource_dispchnAccessCallback(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControl_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispchnControl_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispchnCanCopy(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnIsDuplicate(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_dispchnPreDestruct(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnControlFilter(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnMapTo(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnUnmapFrom(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_dispchnGetRefCount(struct DispChannel *pResource);    // super
void __nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr(struct DispChannel *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnGetNotificationShare(struct DispChannel *pNotifier);    // super
void __nvoc_up_thunk_Notifier_dispchnSetNotificationShare(struct DispChannel *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispchnUnregisterEvent(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_DispChannel_dispchnpioGetRegBaseOffsetAndSize(struct DispChannelPio *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioControl(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioMap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioUnmap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_dispchnpioShareCallback(struct DispChannelPio *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioGetMapAddrSpace(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioInternalControlForward(struct DispChannelPio *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_dispchnpioGetInternalObjectHandle(struct DispChannelPio *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_dispchnpioAccessCallback(struct DispChannelPio *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioGetMemInterMapParams(struct DispChannelPio *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioCheckMemInterUnmap(struct DispChannelPio *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioGetMemoryMappingDescriptor(struct DispChannelPio *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Prologue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Epilogue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioControl_Prologue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchnpioControl_Epilogue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchnpioCanCopy(struct DispChannelPio *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioIsDuplicate(struct DispChannelPio *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispchnpioPreDestruct(struct DispChannelPio *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioControlFilter(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchnpioIsPartialUnmapSupported(struct DispChannelPio *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioMapTo(struct DispChannelPio *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioUnmapFrom(struct DispChannelPio *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispchnpioGetRefCount(struct DispChannelPio *pResource);    // this
void __nvoc_up_thunk_RsResource_dispchnpioAddAdditionalDependants(struct RsClient *pClient, struct DispChannelPio *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnpioGetNotificationListPtr(struct DispChannelPio *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnpioGetNotificationShare(struct DispChannelPio *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispchnpioSetNotificationShare(struct DispChannelPio *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchnpioUnregisterEvent(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchnpioGetOrAllocNotifShare(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelPio = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispChannelPio),
        /*classId=*/            classId(DispChannelPio),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispChannelPio",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispChannelPio,
    /*pCastInfo=*/          &__nvoc_castinfo__DispChannelPio,
    /*pExportInfo=*/        &__nvoc_export_info__DispChannelPio
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__DispChannelPio __nvoc_metadata__DispChannelPio = {
    .rtti.pClassDef = &__nvoc_class_def_DispChannelPio,    // (dispchnpio) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispChannelPio,
    .rtti.offset    = 0,
    .metadata__DispChannel.rtti.pClassDef = &__nvoc_class_def_DispChannel,    // (dispchn) super
    .metadata__DispChannel.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.rtti.offset    = NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel),
    .metadata__DispChannel.metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super^2
    .metadata__DispChannel.metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.rtti.offset    = NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource),
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^3
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^4
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^4
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__DispChannel.metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super^2
    .metadata__DispChannel.metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__Notifier.rtti.offset    = NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier),
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^3
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__dispchnpioGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_DispChannel_dispchnpioGetRegBaseOffsetAndSize,    // virtual inherited (dispchn) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetRegBaseOffsetAndSize__ = &dispchnGetRegBaseOffsetAndSize_IMPL,    // virtual override (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize,    // virtual
    .vtable.__dispchnpioControl__ = &__nvoc_up_thunk_GpuResource_dispchnpioControl,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControl__ = &__nvoc_up_thunk_GpuResource_dispchnControl,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__dispchnpioMap__ = &__nvoc_up_thunk_GpuResource_dispchnpioMap,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnMap__ = &__nvoc_up_thunk_GpuResource_dispchnMap,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__dispchnpioUnmap__ = &__nvoc_up_thunk_GpuResource_dispchnpioUnmap,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnUnmap__ = &__nvoc_up_thunk_GpuResource_dispchnUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__dispchnpioShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchnpioShareCallback,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchnShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__dispchnpioGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchnpioGetMapAddrSpace,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__dispchnpioInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchnpioInternalControlForward,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchnInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__dispchnpioGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchnpioGetInternalObjectHandle,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__dispchnpioAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchnpioAccessCallback,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchnAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__dispchnpioGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchnpioGetMemInterMapParams,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__dispchnpioCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchnpioCheckMemInterUnmap,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__dispchnpioGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchnpioGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__dispchnpioControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Prologue,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__dispchnpioControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Epilogue,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__dispchnpioControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnpioControl_Prologue,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__dispchnpioControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnpioControl_Epilogue,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__dispchnpioCanCopy__ = &__nvoc_up_thunk_RsResource_dispchnpioCanCopy,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnCanCopy__ = &__nvoc_up_thunk_RsResource_dispchnCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__dispchnpioIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchnpioIsDuplicate,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchnIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__dispchnpioPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchnpioPreDestruct,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchnPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__dispchnpioControlFilter__ = &__nvoc_up_thunk_RsResource_dispchnpioControlFilter,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControlFilter__ = &__nvoc_up_thunk_RsResource_dispchnControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__dispchnpioIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchnpioIsPartialUnmapSupported,    // inline virtual inherited (res) base (dispchn) body
    .metadata__DispChannel.vtable.__dispchnIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__dispchnpioMapTo__ = &__nvoc_up_thunk_RsResource_dispchnpioMapTo,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnMapTo__ = &__nvoc_up_thunk_RsResource_dispchnMapTo,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__dispchnpioUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchnpioUnmapFrom,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchnUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__dispchnpioGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchnpioGetRefCount,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchnGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__dispchnpioAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchnpioAddAdditionalDependants,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__dispchnpioGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchnpioGetNotificationListPtr,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__dispchnpioGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnpioGetNotificationShare,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__dispchnpioSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnpioSetNotificationShare,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__dispchnpioUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchnpioUnregisterEvent,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchnUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__dispchnpioGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchnpioGetOrAllocNotifShare,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DispChannelPio = {
    .numRelatives = 9,
    .relatives = {
        &__nvoc_metadata__DispChannelPio.rtti,    // [0]: (dispchnpio) this
        &__nvoc_metadata__DispChannelPio.metadata__DispChannel.rtti,    // [1]: (dispchn) super
        &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource.rtti,    // [2]: (gpures) super^2
        &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.rtti,    // [3]: (rmres) super^3
        &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [4]: (res) super^4
        &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [5]: (obj) super^5
        &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [6]: (rmrescmn) super^4
        &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__Notifier.rtti,    // [7]: (notify) super^2
        &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__Notifier.metadata__INotifier.rtti,    // [8]: (inotify) super^3
    }
};

// 30 up-thunk(s) defined to bridge methods in DispChannelPio to superclasses

// dispchnpioGetRegBaseOffsetAndSize: virtual inherited (dispchn) base (dispchn)
NV_STATUS __nvoc_up_thunk_DispChannel_dispchnpioGetRegBaseOffsetAndSize(struct DispChannelPio *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispchnGetRegBaseOffsetAndSize((struct DispChannel *)(((unsigned char *) pDispChannel) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel)), pGpu, pOffset, pSize);
}

// dispchnpioControl: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioControl(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pParams);
}

// dispchnpioMap: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioMap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// dispchnpioUnmap: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioUnmap(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// dispchnpioShareCallback: virtual inherited (gpures) base (dispchn)
NvBool __nvoc_up_thunk_GpuResource_dispchnpioShareCallback(struct DispChannelPio *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispchnpioGetMapAddrSpace: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioGetMapAddrSpace(struct DispChannelPio *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// dispchnpioInternalControlForward: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnpioInternalControlForward(struct DispChannelPio *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), command, pParams, size);
}

// dispchnpioGetInternalObjectHandle: virtual inherited (gpures) base (dispchn)
NvHandle __nvoc_up_thunk_GpuResource_dispchnpioGetInternalObjectHandle(struct DispChannelPio *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource)));
}

// dispchnpioAccessCallback: virtual inherited (rmres) base (dispchn)
NvBool __nvoc_up_thunk_RmResource_dispchnpioAccessCallback(struct DispChannelPio *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispchnpioGetMemInterMapParams: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioGetMemInterMapParams(struct DispChannelPio *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// dispchnpioCheckMemInterUnmap: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioCheckMemInterUnmap(struct DispChannelPio *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispchnpioGetMemoryMappingDescriptor: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioGetMemoryMappingDescriptor(struct DispChannelPio *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// dispchnpioControlSerialization_Prologue: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Prologue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnpioControlSerialization_Epilogue: virtual inherited (rmres) base (dispchn)
void __nvoc_up_thunk_RmResource_dispchnpioControlSerialization_Epilogue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnpioControl_Prologue: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchnpioControl_Prologue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnpioControl_Epilogue: virtual inherited (rmres) base (dispchn)
void __nvoc_up_thunk_RmResource_dispchnpioControl_Epilogue(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchnpioCanCopy: virtual inherited (res) base (dispchn)
NvBool __nvoc_up_thunk_RsResource_dispchnpioCanCopy(struct DispChannelPio *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnpioIsDuplicate: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioIsDuplicate(struct DispChannelPio *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispchnpioPreDestruct: virtual inherited (res) base (dispchn)
void __nvoc_up_thunk_RsResource_dispchnpioPreDestruct(struct DispChannelPio *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnpioControlFilter: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioControlFilter(struct DispChannelPio *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispchnpioIsPartialUnmapSupported: inline virtual inherited (res) base (dispchn) body
NvBool __nvoc_up_thunk_RsResource_dispchnpioIsPartialUnmapSupported(struct DispChannelPio *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnpioMapTo: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioMapTo(struct DispChannelPio *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchnpioUnmapFrom: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchnpioUnmapFrom(struct DispChannelPio *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchnpioGetRefCount: virtual inherited (res) base (dispchn)
NvU32 __nvoc_up_thunk_RsResource_dispchnpioGetRefCount(struct DispChannelPio *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchnpioAddAdditionalDependants: virtual inherited (res) base (dispchn)
void __nvoc_up_thunk_RsResource_dispchnpioAddAdditionalDependants(struct RsClient *pClient, struct DispChannelPio *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispchnpioGetNotificationListPtr: virtual inherited (notify) base (dispchn)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnpioGetNotificationListPtr(struct DispChannelPio *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)));
}

// dispchnpioGetNotificationShare: virtual inherited (notify) base (dispchn)
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnpioGetNotificationShare(struct DispChannelPio *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)));
}

// dispchnpioSetNotificationShare: virtual inherited (notify) base (dispchn)
void __nvoc_up_thunk_Notifier_dispchnpioSetNotificationShare(struct DispChannelPio *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)), pNotifShare);
}

// dispchnpioUnregisterEvent: virtual inherited (notify) base (dispchn)
NV_STATUS __nvoc_up_thunk_Notifier_dispchnpioUnregisterEvent(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispchnpioGetOrAllocNotifShare: virtual inherited (notify) base (dispchn)
NV_STATUS __nvoc_up_thunk_Notifier_dispchnpioGetOrAllocNotifShare(struct DispChannelPio *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelPio, __nvoc_base_DispChannel.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DispChannelPio = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_DispChannel(DispChannel*);
void __nvoc_dtor_DispChannelPio(DispChannelPio *pThis) {
    __nvoc_dtor_DispChannel(&pThis->__nvoc_base_DispChannel);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispChannelPio(DispChannelPio *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DispChannel(DispChannel* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, NvU32);
NV_STATUS __nvoc_ctor_DispChannelPio(DispChannelPio *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DispChannel(&pThis->__nvoc_base_DispChannel, arg_pCallContext, arg_pParams, NV_FALSE);
    if (status != NV_OK) goto __nvoc_ctor_DispChannelPio_fail_DispChannel;
    __nvoc_init_dataField_DispChannelPio(pThis);

    status = __nvoc_dispchnpioConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispChannelPio_fail__init;
    goto __nvoc_ctor_DispChannelPio_exit; // Success

__nvoc_ctor_DispChannelPio_fail__init:
    __nvoc_dtor_DispChannel(&pThis->__nvoc_base_DispChannel);
__nvoc_ctor_DispChannelPio_fail_DispChannel:
__nvoc_ctor_DispChannelPio_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DispChannelPio_1(DispChannelPio *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DispChannelPio_1


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_DispChannelPio(DispChannelPio *pThis) {
    __nvoc_init_funcTable_DispChannelPio_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DispChannelPio(DispChannelPio *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^4
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^3
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource;    // (gpures) super^2
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^3
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier;    // (notify) super^2
    pThis->__nvoc_pbase_DispChannel = &pThis->__nvoc_base_DispChannel;    // (dispchn) super
    pThis->__nvoc_pbase_DispChannelPio = pThis;    // (dispchnpio) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__DispChannel(&pThis->__nvoc_base_DispChannel);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^4
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource.metadata__RmResource;    // (rmres) super^3
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__Notifier.metadata__INotifier;    // (inotify) super^3
    pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelPio.metadata__DispChannel.metadata__Notifier;    // (notify) super^2
    pThis->__nvoc_base_DispChannel.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelPio.metadata__DispChannel;    // (dispchn) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelPio;    // (dispchnpio) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DispChannelPio(pThis);
}

NV_STATUS __nvoc_objCreate_DispChannelPio(DispChannelPio **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispChannelPio *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispChannelPio), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispChannelPio));

    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__DispChannelPio(pThis);
    status = __nvoc_ctor_DispChannelPio(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispChannelPio_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispChannelPio_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispChannelPio));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispChannelPio(DispChannelPio **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispChannelPio(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0xfe3d2e = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelDma;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannel;

// Forward declarations for DispChannelDma
void __nvoc_init__DispChannel(DispChannel*);
void __nvoc_init__DispChannelDma(DispChannelDma*);
void __nvoc_init_funcTable_DispChannelDma(DispChannelDma*);
NV_STATUS __nvoc_ctor_DispChannelDma(DispChannelDma*, struct CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_DispChannelDma(DispChannelDma*);
void __nvoc_dtor_DispChannelDma(DispChannelDma*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__DispChannelDma;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__DispChannelDma;

// Down-thunk(s) to bridge DispChannelDma methods from ancestors (if any)
NvBool __nvoc_down_thunk_RmResource_resAccessCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^3
NvBool __nvoc_down_thunk_RmResource_resShareCallback(struct RsResource *pResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControlSerialization_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControlSerialization_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_RmResource_resControl_Prologue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
void __nvoc_down_thunk_RmResource_resControl_Epilogue(struct RsResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^3
NV_STATUS __nvoc_down_thunk_GpuResource_resControl(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resMap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super^2
NV_STATUS __nvoc_down_thunk_GpuResource_resUnmap(struct RsResource *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super^2
NvBool __nvoc_down_thunk_GpuResource_rmresShareCallback(struct RmResource *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super^2
PEVENTNOTIFICATION * __nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr(struct INotifier *pNotifier);    // super^2
struct NotifShare * __nvoc_down_thunk_Notifier_inotifyGetNotificationShare(struct INotifier *pNotifier);    // super^2
void __nvoc_down_thunk_Notifier_inotifySetNotificationShare(struct INotifier *pNotifier, struct NotifShare *pNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyUnregisterEvent(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super^2
NV_STATUS __nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare(struct INotifier *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super^2
NV_STATUS __nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize(struct GpuResource *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // super

// Up-thunk(s) to bridge DispChannelDma methods to ancestors (if any)
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
NvBool __nvoc_up_thunk_RmResource_gpuresAccessCallback(struct GpuResource *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams(struct GpuResource *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap(struct GpuResource *pRmResource, NvBool bSubdeviceHandleProvided);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor(struct GpuResource *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RmResource_gpuresControl_Prologue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
void __nvoc_up_thunk_RmResource_gpuresControl_Epilogue(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresCanCopy(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresIsDuplicate(struct GpuResource *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super^2
void __nvoc_up_thunk_RsResource_gpuresPreDestruct(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresControlFilter(struct GpuResource *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super^2
NvBool __nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported(struct GpuResource *pResource);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresMapTo(struct GpuResource *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super^2
NV_STATUS __nvoc_up_thunk_RsResource_gpuresUnmapFrom(struct GpuResource *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super^2
NvU32 __nvoc_up_thunk_RsResource_gpuresGetRefCount(struct GpuResource *pResource);    // super^2
void __nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants(struct RsClient *pClient, struct GpuResource *pResource, RsResourceRef *pReference);    // super^2
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnControl(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnMap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnUnmap(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_GpuResource_dispchnShareCallback(struct DispChannel *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace(struct DispChannel *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_dispchnInternalControlForward(struct DispChannel *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // super
NvHandle __nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle(struct DispChannel *pGpuResource);    // super
NvBool __nvoc_up_thunk_RmResource_dispchnAccessCallback(struct DispChannel *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams(struct DispChannel *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap(struct DispChannel *pRmResource, NvBool bSubdeviceHandleProvided);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor(struct DispChannel *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_dispchnControl_Prologue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_dispchnControl_Epilogue(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispchnCanCopy(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnIsDuplicate(struct DispChannel *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_dispchnPreDestruct(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnControlFilter(struct DispChannel *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported(struct DispChannel *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnMapTo(struct DispChannel *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_dispchnUnmapFrom(struct DispChannel *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_dispchnGetRefCount(struct DispChannel *pResource);    // super
void __nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants(struct RsClient *pClient, struct DispChannel *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr(struct DispChannel *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_dispchnGetNotificationShare(struct DispChannel *pNotifier);    // super
void __nvoc_up_thunk_Notifier_dispchnSetNotificationShare(struct DispChannel *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispchnUnregisterEvent(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare(struct DispChannel *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_DispChannel_dispchndmaGetRegBaseOffsetAndSize(struct DispChannelDma *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaControl(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaMap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaUnmap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_dispchndmaShareCallback(struct DispChannelDma *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaGetMapAddrSpace(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaInternalControlForward(struct DispChannelDma *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_dispchndmaGetInternalObjectHandle(struct DispChannelDma *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_dispchndmaAccessCallback(struct DispChannelDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaGetMemInterMapParams(struct DispChannelDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaCheckMemInterUnmap(struct DispChannelDma *pRmResource, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaGetMemoryMappingDescriptor(struct DispChannelDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Prologue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Epilogue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaControl_Prologue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_dispchndmaControl_Epilogue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchndmaCanCopy(struct DispChannelDma *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaIsDuplicate(struct DispChannelDma *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_dispchndmaPreDestruct(struct DispChannelDma *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaControlFilter(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_dispchndmaIsPartialUnmapSupported(struct DispChannelDma *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaMapTo(struct DispChannelDma *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaUnmapFrom(struct DispChannelDma *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_dispchndmaGetRefCount(struct DispChannelDma *pResource);    // this
void __nvoc_up_thunk_RsResource_dispchndmaAddAdditionalDependants(struct RsClient *pClient, struct DispChannelDma *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchndmaGetNotificationListPtr(struct DispChannelDma *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_dispchndmaGetNotificationShare(struct DispChannelDma *pNotifier);    // this
void __nvoc_up_thunk_Notifier_dispchndmaSetNotificationShare(struct DispChannelDma *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchndmaUnregisterEvent(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_dispchndmaGetOrAllocNotifShare(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_DispChannelDma = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(DispChannelDma),
        /*classId=*/            classId(DispChannelDma),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "DispChannelDma",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_DispChannelDma,
    /*pCastInfo=*/          &__nvoc_castinfo__DispChannelDma,
    /*pExportInfo=*/        &__nvoc_export_info__DispChannelDma
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__DispChannelDma __nvoc_metadata__DispChannelDma = {
    .rtti.pClassDef = &__nvoc_class_def_DispChannelDma,    // (dispchndma) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_DispChannelDma,
    .rtti.offset    = 0,
    .metadata__DispChannel.rtti.pClassDef = &__nvoc_class_def_DispChannel,    // (dispchn) super
    .metadata__DispChannel.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.rtti.offset    = NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel),
    .metadata__DispChannel.metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super^2
    .metadata__DispChannel.metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.rtti.offset    = NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource),
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^3
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^4
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^4
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__DispChannel.metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super^2
    .metadata__DispChannel.metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__Notifier.rtti.offset    = NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier),
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^3
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__dispchndmaGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_DispChannel_dispchndmaGetRegBaseOffsetAndSize,    // virtual inherited (dispchn) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetRegBaseOffsetAndSize__ = &dispchnGetRegBaseOffsetAndSize_IMPL,    // virtual override (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &__nvoc_down_thunk_DispChannel_gpuresGetRegBaseOffsetAndSize,    // virtual
    .vtable.__dispchndmaControl__ = &__nvoc_up_thunk_GpuResource_dispchndmaControl,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControl__ = &__nvoc_up_thunk_GpuResource_dispchnControl,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__dispchndmaMap__ = &__nvoc_up_thunk_GpuResource_dispchndmaMap,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnMap__ = &__nvoc_up_thunk_GpuResource_dispchnMap,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__dispchndmaUnmap__ = &__nvoc_up_thunk_GpuResource_dispchndmaUnmap,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnUnmap__ = &__nvoc_up_thunk_GpuResource_dispchnUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__dispchndmaShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchndmaShareCallback,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnShareCallback__ = &__nvoc_up_thunk_GpuResource_dispchnShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__dispchndmaGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchndmaGetMapAddrSpace,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_dispchnGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__dispchndmaInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchndmaInternalControlForward,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnInternalControlForward__ = &__nvoc_up_thunk_GpuResource_dispchnInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__dispchndmaGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchndmaGetInternalObjectHandle,    // virtual inherited (gpures) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_dispchnGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__dispchndmaAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchndmaAccessCallback,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnAccessCallback__ = &__nvoc_up_thunk_RmResource_dispchnAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__dispchndmaGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchndmaGetMemInterMapParams,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_dispchnGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__dispchndmaCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchndmaCheckMemInterUnmap,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_dispchnCheckMemInterUnmap,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &rmresCheckMemInterUnmap_IMPL,    // virtual
    .vtable.__dispchndmaGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchndmaGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_dispchnGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__dispchndmaControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Prologue,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__dispchndmaControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Epilogue,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__dispchndmaControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchndmaControl_Prologue,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControl_Prologue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__dispchndmaControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchndmaControl_Epilogue,    // virtual inherited (rmres) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControl_Epilogue__ = &__nvoc_up_thunk_RmResource_dispchnControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__dispchndmaCanCopy__ = &__nvoc_up_thunk_RsResource_dispchndmaCanCopy,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnCanCopy__ = &__nvoc_up_thunk_RsResource_dispchnCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__dispchndmaIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchndmaIsDuplicate,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnIsDuplicate__ = &__nvoc_up_thunk_RsResource_dispchnIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__dispchndmaPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchndmaPreDestruct,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnPreDestruct__ = &__nvoc_up_thunk_RsResource_dispchnPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__dispchndmaControlFilter__ = &__nvoc_up_thunk_RsResource_dispchndmaControlFilter,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnControlFilter__ = &__nvoc_up_thunk_RsResource_dispchnControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__dispchndmaIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchndmaIsPartialUnmapSupported,    // inline virtual inherited (res) base (dispchn) body
    .metadata__DispChannel.vtable.__dispchnIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_dispchnIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__dispchndmaMapTo__ = &__nvoc_up_thunk_RsResource_dispchndmaMapTo,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnMapTo__ = &__nvoc_up_thunk_RsResource_dispchnMapTo,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__dispchndmaUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchndmaUnmapFrom,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnUnmapFrom__ = &__nvoc_up_thunk_RsResource_dispchnUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__dispchndmaGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchndmaGetRefCount,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetRefCount__ = &__nvoc_up_thunk_RsResource_dispchnGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__dispchndmaAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchndmaAddAdditionalDependants,    // virtual inherited (res) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_dispchnAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__DispChannel.metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__dispchndmaGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchndmaGetNotificationListPtr,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__dispchndmaGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchndmaGetNotificationShare,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__dispchndmaSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchndmaSetNotificationShare,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnSetNotificationShare__ = &__nvoc_up_thunk_Notifier_dispchnSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__dispchndmaUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchndmaUnregisterEvent,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnUnregisterEvent__ = &__nvoc_up_thunk_Notifier_dispchnUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__dispchndmaGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchndmaGetOrAllocNotifShare,    // virtual inherited (notify) base (dispchn)
    .metadata__DispChannel.vtable.__dispchnGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_dispchnGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__DispChannel.metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__DispChannel.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__DispChannelDma = {
    .numRelatives = 9,
    .relatives = {
        &__nvoc_metadata__DispChannelDma.rtti,    // [0]: (dispchndma) this
        &__nvoc_metadata__DispChannelDma.metadata__DispChannel.rtti,    // [1]: (dispchn) super
        &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource.rtti,    // [2]: (gpures) super^2
        &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.rtti,    // [3]: (rmres) super^3
        &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [4]: (res) super^4
        &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [5]: (obj) super^5
        &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [6]: (rmrescmn) super^4
        &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__Notifier.rtti,    // [7]: (notify) super^2
        &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__Notifier.metadata__INotifier.rtti,    // [8]: (inotify) super^3
    }
};

// 30 up-thunk(s) defined to bridge methods in DispChannelDma to superclasses

// dispchndmaGetRegBaseOffsetAndSize: virtual inherited (dispchn) base (dispchn)
NV_STATUS __nvoc_up_thunk_DispChannel_dispchndmaGetRegBaseOffsetAndSize(struct DispChannelDma *pDispChannel, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return dispchnGetRegBaseOffsetAndSize((struct DispChannel *)(((unsigned char *) pDispChannel) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel)), pGpu, pOffset, pSize);
}

// dispchndmaControl: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaControl(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pParams);
}

// dispchndmaMap: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaMap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// dispchndmaUnmap: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaUnmap(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// dispchndmaShareCallback: virtual inherited (gpures) base (dispchn)
NvBool __nvoc_up_thunk_GpuResource_dispchndmaShareCallback(struct DispChannelDma *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// dispchndmaGetMapAddrSpace: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaGetMapAddrSpace(struct DispChannelDma *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// dispchndmaInternalControlForward: virtual inherited (gpures) base (dispchn)
NV_STATUS __nvoc_up_thunk_GpuResource_dispchndmaInternalControlForward(struct DispChannelDma *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)), command, pParams, size);
}

// dispchndmaGetInternalObjectHandle: virtual inherited (gpures) base (dispchn)
NvHandle __nvoc_up_thunk_GpuResource_dispchndmaGetInternalObjectHandle(struct DispChannelDma *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource)));
}

// dispchndmaAccessCallback: virtual inherited (rmres) base (dispchn)
NvBool __nvoc_up_thunk_RmResource_dispchndmaAccessCallback(struct DispChannelDma *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// dispchndmaGetMemInterMapParams: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaGetMemInterMapParams(struct DispChannelDma *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// dispchndmaCheckMemInterUnmap: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaCheckMemInterUnmap(struct DispChannelDma *pRmResource, NvBool bSubdeviceHandleProvided) {
    return rmresCheckMemInterUnmap((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), bSubdeviceHandleProvided);
}

// dispchndmaGetMemoryMappingDescriptor: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaGetMemoryMappingDescriptor(struct DispChannelDma *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// dispchndmaControlSerialization_Prologue: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Prologue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchndmaControlSerialization_Epilogue: virtual inherited (rmres) base (dispchn)
void __nvoc_up_thunk_RmResource_dispchndmaControlSerialization_Epilogue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchndmaControl_Prologue: virtual inherited (rmres) base (dispchn)
NV_STATUS __nvoc_up_thunk_RmResource_dispchndmaControl_Prologue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchndmaControl_Epilogue: virtual inherited (rmres) base (dispchn)
void __nvoc_up_thunk_RmResource_dispchndmaControl_Epilogue(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// dispchndmaCanCopy: virtual inherited (res) base (dispchn)
NvBool __nvoc_up_thunk_RsResource_dispchndmaCanCopy(struct DispChannelDma *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchndmaIsDuplicate: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaIsDuplicate(struct DispChannelDma *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// dispchndmaPreDestruct: virtual inherited (res) base (dispchn)
void __nvoc_up_thunk_RsResource_dispchndmaPreDestruct(struct DispChannelDma *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchndmaControlFilter: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaControlFilter(struct DispChannelDma *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// dispchndmaIsPartialUnmapSupported: inline virtual inherited (res) base (dispchn) body
NvBool __nvoc_up_thunk_RsResource_dispchndmaIsPartialUnmapSupported(struct DispChannelDma *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchndmaMapTo: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaMapTo(struct DispChannelDma *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchndmaUnmapFrom: virtual inherited (res) base (dispchn)
NV_STATUS __nvoc_up_thunk_RsResource_dispchndmaUnmapFrom(struct DispChannelDma *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// dispchndmaGetRefCount: virtual inherited (res) base (dispchn)
NvU32 __nvoc_up_thunk_RsResource_dispchndmaGetRefCount(struct DispChannelDma *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// dispchndmaAddAdditionalDependants: virtual inherited (res) base (dispchn)
void __nvoc_up_thunk_RsResource_dispchndmaAddAdditionalDependants(struct RsClient *pClient, struct DispChannelDma *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// dispchndmaGetNotificationListPtr: virtual inherited (notify) base (dispchn)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_dispchndmaGetNotificationListPtr(struct DispChannelDma *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)));
}

// dispchndmaGetNotificationShare: virtual inherited (notify) base (dispchn)
struct NotifShare * __nvoc_up_thunk_Notifier_dispchndmaGetNotificationShare(struct DispChannelDma *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)));
}

// dispchndmaSetNotificationShare: virtual inherited (notify) base (dispchn)
void __nvoc_up_thunk_Notifier_dispchndmaSetNotificationShare(struct DispChannelDma *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)), pNotifShare);
}

// dispchndmaUnregisterEvent: virtual inherited (notify) base (dispchn)
NV_STATUS __nvoc_up_thunk_Notifier_dispchndmaUnregisterEvent(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// dispchndmaGetOrAllocNotifShare: virtual inherited (notify) base (dispchn)
NV_STATUS __nvoc_up_thunk_Notifier_dispchndmaGetOrAllocNotifShare(struct DispChannelDma *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(DispChannelDma, __nvoc_base_DispChannel.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__DispChannelDma = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_DispChannel(DispChannel*);
void __nvoc_dtor_DispChannelDma(DispChannelDma *pThis) {
    __nvoc_dtor_DispChannel(&pThis->__nvoc_base_DispChannel);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_DispChannelDma(DispChannelDma *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_DispChannel(DispChannel* , struct CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, NvU32);
NV_STATUS __nvoc_ctor_DispChannelDma(DispChannelDma *pThis, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_DispChannel(&pThis->__nvoc_base_DispChannel, arg_pCallContext, arg_pParams, NV_TRUE);
    if (status != NV_OK) goto __nvoc_ctor_DispChannelDma_fail_DispChannel;
    __nvoc_init_dataField_DispChannelDma(pThis);

    status = __nvoc_dispchndmaConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_DispChannelDma_fail__init;
    goto __nvoc_ctor_DispChannelDma_exit; // Success

__nvoc_ctor_DispChannelDma_fail__init:
    __nvoc_dtor_DispChannel(&pThis->__nvoc_base_DispChannel);
__nvoc_ctor_DispChannelDma_fail_DispChannel:
__nvoc_ctor_DispChannelDma_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_DispChannelDma_1(DispChannelDma *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_DispChannelDma_1


// Initialize vtable(s) for 30 virtual method(s).
void __nvoc_init_funcTable_DispChannelDma(DispChannelDma *pThis) {
    __nvoc_init_funcTable_DispChannelDma_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__DispChannelDma(DispChannelDma *pThis) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^4
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^3
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource;    // (gpures) super^2
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^3
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier;    // (notify) super^2
    pThis->__nvoc_pbase_DispChannel = &pThis->__nvoc_base_DispChannel;    // (dispchn) super
    pThis->__nvoc_pbase_DispChannelDma = pThis;    // (dispchndma) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__DispChannel(&pThis->__nvoc_base_DispChannel);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^4
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource.metadata__RmResource;    // (rmres) super^3
    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__Notifier.metadata__INotifier;    // (inotify) super^3
    pThis->__nvoc_base_DispChannel.__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelDma.metadata__DispChannel.metadata__Notifier;    // (notify) super^2
    pThis->__nvoc_base_DispChannel.__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelDma.metadata__DispChannel;    // (dispchn) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__DispChannelDma;    // (dispchndma) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_DispChannelDma(pThis);
}

NV_STATUS __nvoc_objCreate_DispChannelDma(DispChannelDma **ppThis, Dynamic *pParent, NvU32 createFlags, struct CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    DispChannelDma *pThis;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(DispChannelDma), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(DispChannelDma));

    pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // Link the child into the parent if there is one unless flagged not to do so.
    if (pParent != NULL && !(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    __nvoc_init__DispChannelDma(pThis);
    status = __nvoc_ctor_DispChannelDma(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_DispChannelDma_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_DispChannelDma_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_DispChannel.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(DispChannelDma));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_DispChannelDma(DispChannelDma **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    struct CALL_CONTEXT * arg_pCallContext = va_arg(args, struct CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_DispChannelDma(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

