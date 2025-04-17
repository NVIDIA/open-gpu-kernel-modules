#define NVOC_KERNEL_CE_CONTEXT_H_PRIVATE_ACCESS_ALLOWED

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
#include "g_kernel_ce_context_nvoc.h"


#ifdef DEBUG
char __nvoc_class_id_uniqueness_check__0x2d0ee9 = 1;
#endif

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCeContext;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Object;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RsResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResourceCommon;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_RmResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_GpuResource;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_INotifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_Notifier;
extern const struct NVOC_CLASS_DEF __nvoc_class_def_ChannelDescendant;

// Forward declarations for KernelCeContext
void __nvoc_init__ChannelDescendant(ChannelDescendant*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init__KernelCeContext(KernelCeContext*, RmHalspecOwner *pRmhalspecowner);
void __nvoc_init_funcTable_KernelCeContext(KernelCeContext*);
NV_STATUS __nvoc_ctor_KernelCeContext(KernelCeContext*, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT *arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL *arg_pParams);
void __nvoc_init_dataField_KernelCeContext(KernelCeContext*);
void __nvoc_dtor_KernelCeContext(KernelCeContext*);

// Structures used within RTTI (run-time type information)
extern const struct NVOC_CASTINFO __nvoc_castinfo__KernelCeContext;
extern const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCeContext;

// Down-thunk(s) to bridge KernelCeContext methods from ancestors (if any)
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
NV_STATUS __nvoc_down_thunk_ChannelDescendant_rmresCheckMemInterUnmap(struct RmResource *pChannelDescendant, NvBool bSubdeviceHandleProvided);    // super

// Up-thunk(s) to bridge KernelCeContext methods to ancestors (if any)
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
NV_STATUS __nvoc_up_thunk_GpuResource_chandesControl(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesMap(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesUnmap(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // super
NvBool __nvoc_up_thunk_GpuResource_chandesShareCallback(struct ChannelDescendant *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesGetRegBaseOffsetAndSize(struct ChannelDescendant *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesGetMapAddrSpace(struct ChannelDescendant *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // super
NV_STATUS __nvoc_up_thunk_GpuResource_chandesInternalControlForward(struct ChannelDescendant *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // super
NvHandle __nvoc_up_thunk_GpuResource_chandesGetInternalObjectHandle(struct ChannelDescendant *pGpuResource);    // super
NvBool __nvoc_up_thunk_RmResource_chandesAccessCallback(struct ChannelDescendant *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // super
NV_STATUS __nvoc_up_thunk_RmResource_chandesGetMemInterMapParams(struct ChannelDescendant *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_chandesGetMemoryMappingDescriptor(struct ChannelDescendant *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // super
NV_STATUS __nvoc_up_thunk_RmResource_chandesControlSerialization_Prologue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_chandesControlSerialization_Epilogue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NV_STATUS __nvoc_up_thunk_RmResource_chandesControl_Prologue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
void __nvoc_up_thunk_RmResource_chandesControl_Epilogue(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_chandesCanCopy(struct ChannelDescendant *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_chandesIsDuplicate(struct ChannelDescendant *pResource, NvHandle hMemory, NvBool *pDuplicate);    // super
void __nvoc_up_thunk_RsResource_chandesPreDestruct(struct ChannelDescendant *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_chandesControlFilter(struct ChannelDescendant *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // super
NvBool __nvoc_up_thunk_RsResource_chandesIsPartialUnmapSupported(struct ChannelDescendant *pResource);    // super
NV_STATUS __nvoc_up_thunk_RsResource_chandesMapTo(struct ChannelDescendant *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // super
NV_STATUS __nvoc_up_thunk_RsResource_chandesUnmapFrom(struct ChannelDescendant *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // super
NvU32 __nvoc_up_thunk_RsResource_chandesGetRefCount(struct ChannelDescendant *pResource);    // super
void __nvoc_up_thunk_RsResource_chandesAddAdditionalDependants(struct RsClient *pClient, struct ChannelDescendant *pResource, RsResourceRef *pReference);    // super
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_chandesGetNotificationListPtr(struct ChannelDescendant *pNotifier);    // super
struct NotifShare * __nvoc_up_thunk_Notifier_chandesGetNotificationShare(struct ChannelDescendant *pNotifier);    // super
void __nvoc_up_thunk_Notifier_chandesSetNotificationShare(struct ChannelDescendant *pNotifier, struct NotifShare *pNotifShare);    // super
NV_STATUS __nvoc_up_thunk_Notifier_chandesUnregisterEvent(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // super
NV_STATUS __nvoc_up_thunk_Notifier_chandesGetOrAllocNotifShare(struct ChannelDescendant *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // super
NV_STATUS __nvoc_up_thunk_ChannelDescendant_kcectxGetSwMethods(struct KernelCeContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods);    // this
NvBool __nvoc_up_thunk_ChannelDescendant_kcectxIsSwMethodStalling(struct KernelCeContext *pChannelDescendant, NvU32 hHandle);    // this
NV_STATUS __nvoc_up_thunk_ChannelDescendant_kcectxCheckMemInterUnmap(struct KernelCeContext *pChannelDescendant, NvBool bSubdeviceHandleProvided);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxControl(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxMap(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxUnmap(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping);    // this
NvBool __nvoc_up_thunk_GpuResource_kcectxShareCallback(struct KernelCeContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxGetRegBaseOffsetAndSize(struct KernelCeContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxGetMapAddrSpace(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace);    // this
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxInternalControlForward(struct KernelCeContext *pGpuResource, NvU32 command, void *pParams, NvU32 size);    // this
NvHandle __nvoc_up_thunk_GpuResource_kcectxGetInternalObjectHandle(struct KernelCeContext *pGpuResource);    // this
NvBool __nvoc_up_thunk_RmResource_kcectxAccessCallback(struct KernelCeContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kcectxGetMemInterMapParams(struct KernelCeContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kcectxGetMemoryMappingDescriptor(struct KernelCeContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kcectxControlSerialization_Prologue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kcectxControlSerialization_Epilogue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NV_STATUS __nvoc_up_thunk_RmResource_kcectxControl_Prologue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
void __nvoc_up_thunk_RmResource_kcectxControl_Epilogue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_kcectxCanCopy(struct KernelCeContext *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kcectxIsDuplicate(struct KernelCeContext *pResource, NvHandle hMemory, NvBool *pDuplicate);    // this
void __nvoc_up_thunk_RsResource_kcectxPreDestruct(struct KernelCeContext *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kcectxControlFilter(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams);    // this
NvBool __nvoc_up_thunk_RsResource_kcectxIsPartialUnmapSupported(struct KernelCeContext *pResource);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kcectxMapTo(struct KernelCeContext *pResource, RS_RES_MAP_TO_PARAMS *pParams);    // this
NV_STATUS __nvoc_up_thunk_RsResource_kcectxUnmapFrom(struct KernelCeContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams);    // this
NvU32 __nvoc_up_thunk_RsResource_kcectxGetRefCount(struct KernelCeContext *pResource);    // this
void __nvoc_up_thunk_RsResource_kcectxAddAdditionalDependants(struct RsClient *pClient, struct KernelCeContext *pResource, RsResourceRef *pReference);    // this
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_kcectxGetNotificationListPtr(struct KernelCeContext *pNotifier);    // this
struct NotifShare * __nvoc_up_thunk_Notifier_kcectxGetNotificationShare(struct KernelCeContext *pNotifier);    // this
void __nvoc_up_thunk_Notifier_kcectxSetNotificationShare(struct KernelCeContext *pNotifier, struct NotifShare *pNotifShare);    // this
NV_STATUS __nvoc_up_thunk_Notifier_kcectxUnregisterEvent(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent);    // this
NV_STATUS __nvoc_up_thunk_Notifier_kcectxGetOrAllocNotifShare(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare);    // this

const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCeContext = 
{
    /*classInfo=*/ {
        /*size=*/               sizeof(KernelCeContext),
        /*classId=*/            classId(KernelCeContext),
        /*providerId=*/         &__nvoc_rtti_provider,
#if NV_PRINTF_STRINGS_ALLOWED
        /*name=*/               "KernelCeContext",
#endif
    },
    /*objCreatefn=*/        (NVOC_DYNAMIC_OBJ_CREATE) &__nvoc_objCreateDynamic_KernelCeContext,
    /*pCastInfo=*/          &__nvoc_castinfo__KernelCeContext,
    /*pExportInfo=*/        &__nvoc_export_info__KernelCeContext
};


// Metadata with per-class RTTI and vtable with ancestor(s)
static const struct NVOC_METADATA__KernelCeContext __nvoc_metadata__KernelCeContext = {
    .rtti.pClassDef = &__nvoc_class_def_KernelCeContext,    // (kcectx) this
    .rtti.dtor      = (NVOC_DYNAMIC_DTOR) &__nvoc_dtor_KernelCeContext,
    .rtti.offset    = 0,
    .metadata__ChannelDescendant.rtti.pClassDef = &__nvoc_class_def_ChannelDescendant,    // (chandes) super
    .metadata__ChannelDescendant.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.rtti.offset    = NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant),
    .metadata__ChannelDescendant.metadata__GpuResource.rtti.pClassDef = &__nvoc_class_def_GpuResource,    // (gpures) super^2
    .metadata__ChannelDescendant.metadata__GpuResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.rtti.offset    = NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource),
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.rtti.pClassDef = &__nvoc_class_def_RmResource,    // (rmres) super^3
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.rtti.offset    = NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource),
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.pClassDef = &__nvoc_class_def_RsResource,    // (res) super^4
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti.offset    = NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource),
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.pClassDef = &__nvoc_class_def_Object,    // (obj) super^5
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti.offset    = NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object),
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.pClassDef = &__nvoc_class_def_RmResourceCommon,    // (rmrescmn) super^4
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti.offset    = NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon),
    .metadata__ChannelDescendant.metadata__Notifier.rtti.pClassDef = &__nvoc_class_def_Notifier,    // (notify) super^2
    .metadata__ChannelDescendant.metadata__Notifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__Notifier.rtti.offset    = NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier),
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.rtti.pClassDef = &__nvoc_class_def_INotifier,    // (inotify) super^3
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.rtti.dtor      = &__nvoc_destructFromBase,
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.rtti.offset    = NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier),

    .vtable.__kcectxGetSwMethods__ = &__nvoc_up_thunk_ChannelDescendant_kcectxGetSwMethods,    // virtual inherited (chandes) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetSwMethods__ = &chandesGetSwMethods_IMPL,    // virtual
    .vtable.__kcectxIsSwMethodStalling__ = &__nvoc_up_thunk_ChannelDescendant_kcectxIsSwMethodStalling,    // virtual inherited (chandes) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesIsSwMethodStalling__ = &chandesIsSwMethodStalling_IMPL,    // virtual
    .vtable.__kcectxCheckMemInterUnmap__ = &__nvoc_up_thunk_ChannelDescendant_kcectxCheckMemInterUnmap,    // virtual inherited (chandes) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesCheckMemInterUnmap__ = &chandesCheckMemInterUnmap_IMPL,    // virtual override (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresCheckMemInterUnmap__ = &__nvoc_up_thunk_RmResource_gpuresCheckMemInterUnmap,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresCheckMemInterUnmap__ = &__nvoc_down_thunk_ChannelDescendant_rmresCheckMemInterUnmap,    // virtual
    .vtable.__kcectxControl__ = &__nvoc_up_thunk_GpuResource_kcectxControl,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControl__ = &__nvoc_up_thunk_GpuResource_chandesControl,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControl__ = &gpuresControl_IMPL,    // virtual override (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl__ = &__nvoc_up_thunk_RsResource_rmresControl,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl__ = &__nvoc_down_thunk_GpuResource_resControl,    // virtual
    .vtable.__kcectxMap__ = &__nvoc_up_thunk_GpuResource_kcectxMap,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesMap__ = &__nvoc_up_thunk_GpuResource_chandesMap,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresMap__ = &gpuresMap_IMPL,    // virtual override (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresMap__ = &__nvoc_up_thunk_RsResource_rmresMap,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMap__ = &__nvoc_down_thunk_GpuResource_resMap,    // virtual
    .vtable.__kcectxUnmap__ = &__nvoc_up_thunk_GpuResource_kcectxUnmap,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesUnmap__ = &__nvoc_up_thunk_GpuResource_chandesUnmap,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresUnmap__ = &gpuresUnmap_IMPL,    // virtual override (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmap__ = &__nvoc_up_thunk_RsResource_rmresUnmap,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmap__ = &__nvoc_down_thunk_GpuResource_resUnmap,    // virtual
    .vtable.__kcectxShareCallback__ = &__nvoc_up_thunk_GpuResource_kcectxShareCallback,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesShareCallback__ = &__nvoc_up_thunk_GpuResource_chandesShareCallback,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresShareCallback__ = &gpuresShareCallback_IMPL,    // virtual override (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresShareCallback__ = &__nvoc_down_thunk_GpuResource_rmresShareCallback,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resShareCallback__ = &__nvoc_down_thunk_RmResource_resShareCallback,    // virtual
    .vtable.__kcectxGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_kcectxGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetRegBaseOffsetAndSize__ = &__nvoc_up_thunk_GpuResource_chandesGetRegBaseOffsetAndSize,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetRegBaseOffsetAndSize__ = &gpuresGetRegBaseOffsetAndSize_IMPL,    // virtual
    .vtable.__kcectxGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_kcectxGetMapAddrSpace,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetMapAddrSpace__ = &__nvoc_up_thunk_GpuResource_chandesGetMapAddrSpace,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetMapAddrSpace__ = &gpuresGetMapAddrSpace_IMPL,    // virtual
    .vtable.__kcectxInternalControlForward__ = &__nvoc_up_thunk_GpuResource_kcectxInternalControlForward,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesInternalControlForward__ = &__nvoc_up_thunk_GpuResource_chandesInternalControlForward,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresInternalControlForward__ = &gpuresInternalControlForward_IMPL,    // virtual
    .vtable.__kcectxGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_kcectxGetInternalObjectHandle,    // virtual inherited (gpures) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetInternalObjectHandle__ = &__nvoc_up_thunk_GpuResource_chandesGetInternalObjectHandle,    // virtual inherited (gpures) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetInternalObjectHandle__ = &gpuresGetInternalObjectHandle_IMPL,    // virtual
    .vtable.__kcectxAccessCallback__ = &__nvoc_up_thunk_RmResource_kcectxAccessCallback,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesAccessCallback__ = &__nvoc_up_thunk_RmResource_chandesAccessCallback,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresAccessCallback__ = &__nvoc_up_thunk_RmResource_gpuresAccessCallback,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresAccessCallback__ = &rmresAccessCallback_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAccessCallback__ = &__nvoc_down_thunk_RmResource_resAccessCallback,    // virtual
    .vtable.__kcectxGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_kcectxGetMemInterMapParams,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_chandesGetMemInterMapParams,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetMemInterMapParams__ = &__nvoc_up_thunk_RmResource_gpuresGetMemInterMapParams,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemInterMapParams__ = &rmresGetMemInterMapParams_IMPL,    // virtual
    .vtable.__kcectxGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_kcectxGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_chandesGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetMemoryMappingDescriptor__ = &__nvoc_up_thunk_RmResource_gpuresGetMemoryMappingDescriptor,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetMemoryMappingDescriptor__ = &rmresGetMemoryMappingDescriptor_IMPL,    // virtual
    .vtable.__kcectxControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_kcectxControlSerialization_Prologue,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_chandesControlSerialization_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControlSerialization_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Prologue__ = &rmresControlSerialization_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Prologue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Prologue,    // virtual
    .vtable.__kcectxControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_kcectxControlSerialization_Epilogue,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_chandesControlSerialization_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControlSerialization_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControlSerialization_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlSerialization_Epilogue__ = &rmresControlSerialization_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlSerialization_Epilogue__ = &__nvoc_down_thunk_RmResource_resControlSerialization_Epilogue,    // virtual
    .vtable.__kcectxControl_Prologue__ = &__nvoc_up_thunk_RmResource_kcectxControl_Prologue,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControl_Prologue__ = &__nvoc_up_thunk_RmResource_chandesControl_Prologue,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControl_Prologue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Prologue,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Prologue__ = &rmresControl_Prologue_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Prologue__ = &__nvoc_down_thunk_RmResource_resControl_Prologue,    // virtual
    .vtable.__kcectxControl_Epilogue__ = &__nvoc_up_thunk_RmResource_kcectxControl_Epilogue,    // virtual inherited (rmres) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControl_Epilogue__ = &__nvoc_up_thunk_RmResource_chandesControl_Epilogue,    // virtual inherited (rmres) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControl_Epilogue__ = &__nvoc_up_thunk_RmResource_gpuresControl_Epilogue,    // virtual inherited (rmres) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControl_Epilogue__ = &rmresControl_Epilogue_IMPL,    // virtual override (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControl_Epilogue__ = &__nvoc_down_thunk_RmResource_resControl_Epilogue,    // virtual
    .vtable.__kcectxCanCopy__ = &__nvoc_up_thunk_RsResource_kcectxCanCopy,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesCanCopy__ = &__nvoc_up_thunk_RsResource_chandesCanCopy,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresCanCopy__ = &__nvoc_up_thunk_RsResource_gpuresCanCopy,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresCanCopy__ = &__nvoc_up_thunk_RsResource_rmresCanCopy,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resCanCopy__ = &resCanCopy_IMPL,    // virtual
    .vtable.__kcectxIsDuplicate__ = &__nvoc_up_thunk_RsResource_kcectxIsDuplicate,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesIsDuplicate__ = &__nvoc_up_thunk_RsResource_chandesIsDuplicate,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresIsDuplicate__ = &__nvoc_up_thunk_RsResource_gpuresIsDuplicate,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsDuplicate__ = &__nvoc_up_thunk_RsResource_rmresIsDuplicate,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsDuplicate__ = &resIsDuplicate_IMPL,    // virtual
    .vtable.__kcectxPreDestruct__ = &__nvoc_up_thunk_RsResource_kcectxPreDestruct,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesPreDestruct__ = &__nvoc_up_thunk_RsResource_chandesPreDestruct,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresPreDestruct__ = &__nvoc_up_thunk_RsResource_gpuresPreDestruct,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresPreDestruct__ = &__nvoc_up_thunk_RsResource_rmresPreDestruct,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resPreDestruct__ = &resPreDestruct_IMPL,    // virtual
    .vtable.__kcectxControlFilter__ = &__nvoc_up_thunk_RsResource_kcectxControlFilter,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesControlFilter__ = &__nvoc_up_thunk_RsResource_chandesControlFilter,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresControlFilter__ = &__nvoc_up_thunk_RsResource_gpuresControlFilter,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresControlFilter__ = &__nvoc_up_thunk_RsResource_rmresControlFilter,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resControlFilter__ = &resControlFilter_IMPL,    // virtual
    .vtable.__kcectxIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_kcectxIsPartialUnmapSupported,    // inline virtual inherited (res) base (chandes) body
    .metadata__ChannelDescendant.vtable.__chandesIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_chandesIsPartialUnmapSupported,    // inline virtual inherited (res) base (gpures) body
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_gpuresIsPartialUnmapSupported,    // inline virtual inherited (res) base (rmres) body
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresIsPartialUnmapSupported__ = &__nvoc_up_thunk_RsResource_rmresIsPartialUnmapSupported,    // inline virtual inherited (res) base (res) body
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resIsPartialUnmapSupported__ = &resIsPartialUnmapSupported_d69453,    // inline virtual body
    .vtable.__kcectxMapTo__ = &__nvoc_up_thunk_RsResource_kcectxMapTo,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesMapTo__ = &__nvoc_up_thunk_RsResource_chandesMapTo,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresMapTo__ = &__nvoc_up_thunk_RsResource_gpuresMapTo,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresMapTo__ = &__nvoc_up_thunk_RsResource_rmresMapTo,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resMapTo__ = &resMapTo_IMPL,    // virtual
    .vtable.__kcectxUnmapFrom__ = &__nvoc_up_thunk_RsResource_kcectxUnmapFrom,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesUnmapFrom__ = &__nvoc_up_thunk_RsResource_chandesUnmapFrom,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresUnmapFrom__ = &__nvoc_up_thunk_RsResource_gpuresUnmapFrom,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresUnmapFrom__ = &__nvoc_up_thunk_RsResource_rmresUnmapFrom,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resUnmapFrom__ = &resUnmapFrom_IMPL,    // virtual
    .vtable.__kcectxGetRefCount__ = &__nvoc_up_thunk_RsResource_kcectxGetRefCount,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetRefCount__ = &__nvoc_up_thunk_RsResource_chandesGetRefCount,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresGetRefCount__ = &__nvoc_up_thunk_RsResource_gpuresGetRefCount,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresGetRefCount__ = &__nvoc_up_thunk_RsResource_rmresGetRefCount,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resGetRefCount__ = &resGetRefCount_IMPL,    // virtual
    .vtable.__kcectxAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_kcectxAddAdditionalDependants,    // virtual inherited (res) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_chandesAddAdditionalDependants,    // virtual inherited (res) base (gpures)
    .metadata__ChannelDescendant.metadata__GpuResource.vtable.__gpuresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_gpuresAddAdditionalDependants,    // virtual inherited (res) base (rmres)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.vtable.__rmresAddAdditionalDependants__ = &__nvoc_up_thunk_RsResource_rmresAddAdditionalDependants,    // virtual inherited (res) base (res)
    .metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.vtable.__resAddAdditionalDependants__ = &resAddAdditionalDependants_IMPL,    // virtual
    .vtable.__kcectxGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_kcectxGetNotificationListPtr,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetNotificationListPtr__ = &__nvoc_up_thunk_Notifier_chandesGetNotificationListPtr,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifyGetNotificationListPtr__ = &notifyGetNotificationListPtr_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationListPtr__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationListPtr,    // pure virtual
    .vtable.__kcectxGetNotificationShare__ = &__nvoc_up_thunk_Notifier_kcectxGetNotificationShare,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetNotificationShare__ = &__nvoc_up_thunk_Notifier_chandesGetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifyGetNotificationShare__ = &notifyGetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifyGetNotificationShare,    // pure virtual
    .vtable.__kcectxSetNotificationShare__ = &__nvoc_up_thunk_Notifier_kcectxSetNotificationShare,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesSetNotificationShare__ = &__nvoc_up_thunk_Notifier_chandesSetNotificationShare,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifySetNotificationShare__ = &notifySetNotificationShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifySetNotificationShare__ = &__nvoc_down_thunk_Notifier_inotifySetNotificationShare,    // pure virtual
    .vtable.__kcectxUnregisterEvent__ = &__nvoc_up_thunk_Notifier_kcectxUnregisterEvent,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesUnregisterEvent__ = &__nvoc_up_thunk_Notifier_chandesUnregisterEvent,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifyUnregisterEvent__ = &notifyUnregisterEvent_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifyUnregisterEvent__ = &__nvoc_down_thunk_Notifier_inotifyUnregisterEvent,    // pure virtual
    .vtable.__kcectxGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_kcectxGetOrAllocNotifShare,    // virtual inherited (notify) base (chandes)
    .metadata__ChannelDescendant.vtable.__chandesGetOrAllocNotifShare__ = &__nvoc_up_thunk_Notifier_chandesGetOrAllocNotifShare,    // virtual inherited (notify) base (notify)
    .metadata__ChannelDescendant.metadata__Notifier.vtable.__notifyGetOrAllocNotifShare__ = &notifyGetOrAllocNotifShare_IMPL,    // virtual override (inotify) base (inotify)
    .metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.vtable.__inotifyGetOrAllocNotifShare__ = &__nvoc_down_thunk_Notifier_inotifyGetOrAllocNotifShare,    // pure virtual
};


// Dynamic down-casting information
const struct NVOC_CASTINFO __nvoc_castinfo__KernelCeContext = {
    .numRelatives = 9,
    .relatives = {
        &__nvoc_metadata__KernelCeContext.rtti,    // [0]: (kcectx) this
        &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.rtti,    // [1]: (chandes) super
        &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource.rtti,    // [2]: (gpures) super^2
        &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.rtti,    // [3]: (rmres) super^3
        &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.rtti,    // [4]: (res) super^4
        &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object.rtti,    // [5]: (obj) super^5
        &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon.rtti,    // [6]: (rmrescmn) super^4
        &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__Notifier.rtti,    // [7]: (notify) super^2
        &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier.rtti,    // [8]: (inotify) super^3
    }
};

// 32 up-thunk(s) defined to bridge methods in KernelCeContext to superclasses

// kcectxGetSwMethods: virtual inherited (chandes) base (chandes)
NV_STATUS __nvoc_up_thunk_ChannelDescendant_kcectxGetSwMethods(struct KernelCeContext *pChannelDescendant, const METHOD **ppMethods, NvU32 *pNumMethods) {
    return chandesGetSwMethods((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant)), ppMethods, pNumMethods);
}

// kcectxIsSwMethodStalling: virtual inherited (chandes) base (chandes)
NvBool __nvoc_up_thunk_ChannelDescendant_kcectxIsSwMethodStalling(struct KernelCeContext *pChannelDescendant, NvU32 hHandle) {
    return chandesIsSwMethodStalling((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant)), hHandle);
}

// kcectxCheckMemInterUnmap: virtual inherited (chandes) base (chandes)
NV_STATUS __nvoc_up_thunk_ChannelDescendant_kcectxCheckMemInterUnmap(struct KernelCeContext *pChannelDescendant, NvBool bSubdeviceHandleProvided) {
    return chandesCheckMemInterUnmap((struct ChannelDescendant *)(((unsigned char *) pChannelDescendant) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant)), bSubdeviceHandleProvided);
}

// kcectxControl: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxControl(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return gpuresControl((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pCallContext, pParams);
}

// kcectxMap: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxMap(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RS_CPU_MAP_PARAMS *pParams, struct RsCpuMapping *pCpuMapping) {
    return gpuresMap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pCallContext, pParams, pCpuMapping);
}

// kcectxUnmap: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxUnmap(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, struct RsCpuMapping *pCpuMapping) {
    return gpuresUnmap((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pCallContext, pCpuMapping);
}

// kcectxShareCallback: virtual inherited (gpures) base (chandes)
NvBool __nvoc_up_thunk_GpuResource_kcectxShareCallback(struct KernelCeContext *pGpuResource, struct RsClient *pInvokingClient, struct RsResourceRef *pParentRef, RS_SHARE_POLICY *pSharePolicy) {
    return gpuresShareCallback((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pInvokingClient, pParentRef, pSharePolicy);
}

// kcectxGetRegBaseOffsetAndSize: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxGetRegBaseOffsetAndSize(struct KernelCeContext *pGpuResource, struct OBJGPU *pGpu, NvU32 *pOffset, NvU32 *pSize) {
    return gpuresGetRegBaseOffsetAndSize((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pGpu, pOffset, pSize);
}

// kcectxGetMapAddrSpace: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxGetMapAddrSpace(struct KernelCeContext *pGpuResource, struct CALL_CONTEXT *pCallContext, NvU32 mapFlags, NV_ADDRESS_SPACE *pAddrSpace) {
    return gpuresGetMapAddrSpace((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), pCallContext, mapFlags, pAddrSpace);
}

// kcectxInternalControlForward: virtual inherited (gpures) base (chandes)
NV_STATUS __nvoc_up_thunk_GpuResource_kcectxInternalControlForward(struct KernelCeContext *pGpuResource, NvU32 command, void *pParams, NvU32 size) {
    return gpuresInternalControlForward((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)), command, pParams, size);
}

// kcectxGetInternalObjectHandle: virtual inherited (gpures) base (chandes)
NvHandle __nvoc_up_thunk_GpuResource_kcectxGetInternalObjectHandle(struct KernelCeContext *pGpuResource) {
    return gpuresGetInternalObjectHandle((struct GpuResource *)(((unsigned char *) pGpuResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource)));
}

// kcectxAccessCallback: virtual inherited (rmres) base (chandes)
NvBool __nvoc_up_thunk_RmResource_kcectxAccessCallback(struct KernelCeContext *pResource, struct RsClient *pInvokingClient, void *pAllocParams, RsAccessRight accessRight) {
    return rmresAccessCallback((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pInvokingClient, pAllocParams, accessRight);
}

// kcectxGetMemInterMapParams: virtual inherited (rmres) base (chandes)
NV_STATUS __nvoc_up_thunk_RmResource_kcectxGetMemInterMapParams(struct KernelCeContext *pRmResource, RMRES_MEM_INTER_MAP_PARAMS *pParams) {
    return rmresGetMemInterMapParams((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pParams);
}

// kcectxGetMemoryMappingDescriptor: virtual inherited (rmres) base (chandes)
NV_STATUS __nvoc_up_thunk_RmResource_kcectxGetMemoryMappingDescriptor(struct KernelCeContext *pRmResource, struct MEMORY_DESCRIPTOR **ppMemDesc) {
    return rmresGetMemoryMappingDescriptor((struct RmResource *)(((unsigned char *) pRmResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), ppMemDesc);
}

// kcectxControlSerialization_Prologue: virtual inherited (rmres) base (chandes)
NV_STATUS __nvoc_up_thunk_RmResource_kcectxControlSerialization_Prologue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControlSerialization_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kcectxControlSerialization_Epilogue: virtual inherited (rmres) base (chandes)
void __nvoc_up_thunk_RmResource_kcectxControlSerialization_Epilogue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControlSerialization_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kcectxControl_Prologue: virtual inherited (rmres) base (chandes)
NV_STATUS __nvoc_up_thunk_RmResource_kcectxControl_Prologue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return rmresControl_Prologue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kcectxControl_Epilogue: virtual inherited (rmres) base (chandes)
void __nvoc_up_thunk_RmResource_kcectxControl_Epilogue(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    rmresControl_Epilogue((struct RmResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource)), pCallContext, pParams);
}

// kcectxCanCopy: virtual inherited (res) base (chandes)
NvBool __nvoc_up_thunk_RsResource_kcectxCanCopy(struct KernelCeContext *pResource) {
    return resCanCopy((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kcectxIsDuplicate: virtual inherited (res) base (chandes)
NV_STATUS __nvoc_up_thunk_RsResource_kcectxIsDuplicate(struct KernelCeContext *pResource, NvHandle hMemory, NvBool *pDuplicate) {
    return resIsDuplicate((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), hMemory, pDuplicate);
}

// kcectxPreDestruct: virtual inherited (res) base (chandes)
void __nvoc_up_thunk_RsResource_kcectxPreDestruct(struct KernelCeContext *pResource) {
    resPreDestruct((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kcectxControlFilter: virtual inherited (res) base (chandes)
NV_STATUS __nvoc_up_thunk_RsResource_kcectxControlFilter(struct KernelCeContext *pResource, struct CALL_CONTEXT *pCallContext, struct RS_RES_CONTROL_PARAMS_INTERNAL *pParams) {
    return resControlFilter((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pCallContext, pParams);
}

// kcectxIsPartialUnmapSupported: inline virtual inherited (res) base (chandes) body
NvBool __nvoc_up_thunk_RsResource_kcectxIsPartialUnmapSupported(struct KernelCeContext *pResource) {
    return resIsPartialUnmapSupported((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kcectxMapTo: virtual inherited (res) base (chandes)
NV_STATUS __nvoc_up_thunk_RsResource_kcectxMapTo(struct KernelCeContext *pResource, RS_RES_MAP_TO_PARAMS *pParams) {
    return resMapTo((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kcectxUnmapFrom: virtual inherited (res) base (chandes)
NV_STATUS __nvoc_up_thunk_RsResource_kcectxUnmapFrom(struct KernelCeContext *pResource, RS_RES_UNMAP_FROM_PARAMS *pParams) {
    return resUnmapFrom((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pParams);
}

// kcectxGetRefCount: virtual inherited (res) base (chandes)
NvU32 __nvoc_up_thunk_RsResource_kcectxGetRefCount(struct KernelCeContext *pResource) {
    return resGetRefCount((struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)));
}

// kcectxAddAdditionalDependants: virtual inherited (res) base (chandes)
void __nvoc_up_thunk_RsResource_kcectxAddAdditionalDependants(struct RsClient *pClient, struct KernelCeContext *pResource, RsResourceRef *pReference) {
    resAddAdditionalDependants(pClient, (struct RsResource *)(((unsigned char *) pResource) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource)), pReference);
}

// kcectxGetNotificationListPtr: virtual inherited (notify) base (chandes)
PEVENTNOTIFICATION * __nvoc_up_thunk_Notifier_kcectxGetNotificationListPtr(struct KernelCeContext *pNotifier) {
    return notifyGetNotificationListPtr((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)));
}

// kcectxGetNotificationShare: virtual inherited (notify) base (chandes)
struct NotifShare * __nvoc_up_thunk_Notifier_kcectxGetNotificationShare(struct KernelCeContext *pNotifier) {
    return notifyGetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)));
}

// kcectxSetNotificationShare: virtual inherited (notify) base (chandes)
void __nvoc_up_thunk_Notifier_kcectxSetNotificationShare(struct KernelCeContext *pNotifier, struct NotifShare *pNotifShare) {
    notifySetNotificationShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)), pNotifShare);
}

// kcectxUnregisterEvent: virtual inherited (notify) base (chandes)
NV_STATUS __nvoc_up_thunk_Notifier_kcectxUnregisterEvent(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, NvHandle hEventClient, NvHandle hEvent) {
    return notifyUnregisterEvent((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, hEventClient, hEvent);
}

// kcectxGetOrAllocNotifShare: virtual inherited (notify) base (chandes)
NV_STATUS __nvoc_up_thunk_Notifier_kcectxGetOrAllocNotifShare(struct KernelCeContext *pNotifier, NvHandle hNotifierClient, NvHandle hNotifierResource, struct NotifShare **ppNotifShare) {
    return notifyGetOrAllocNotifShare((struct Notifier *)(((unsigned char *) pNotifier) + NV_OFFSETOF(KernelCeContext, __nvoc_base_ChannelDescendant.__nvoc_base_Notifier)), hNotifierClient, hNotifierResource, ppNotifShare);
}


const struct NVOC_EXPORT_INFO __nvoc_export_info__KernelCeContext = 
{
    /*numEntries=*/     0,
    /*pExportEntries=*/  0
};

void __nvoc_dtor_ChannelDescendant(ChannelDescendant*);
void __nvoc_dtor_KernelCeContext(KernelCeContext *pThis) {
    __nvoc_kcectxDestruct(pThis);
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
    PORT_UNREFERENCED_VARIABLE(pThis);
}

void __nvoc_init_dataField_KernelCeContext(KernelCeContext *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
}

NV_STATUS __nvoc_ctor_ChannelDescendant(ChannelDescendant* , RmHalspecOwner* , CALL_CONTEXT *, struct RS_RES_ALLOC_PARAMS_INTERNAL *, ENGDESCRIPTOR (*)(struct OBJGPU *, NvU32, void *));
NV_STATUS __nvoc_ctor_KernelCeContext(KernelCeContext *pThis, RmHalspecOwner *pRmhalspecowner, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams) {
    NV_STATUS status = NV_OK;
    status = __nvoc_ctor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner, arg_pCallContext, arg_pParams, kceGetEngineDescFromAllocParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCeContext_fail_ChannelDescendant;
    __nvoc_init_dataField_KernelCeContext(pThis);

    status = __nvoc_kcectxConstruct(pThis, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_ctor_KernelCeContext_fail__init;
    goto __nvoc_ctor_KernelCeContext_exit; // Success

__nvoc_ctor_KernelCeContext_fail__init:
    __nvoc_dtor_ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant);
__nvoc_ctor_KernelCeContext_fail_ChannelDescendant:
__nvoc_ctor_KernelCeContext_exit:

    return status;
}

// Vtable initialization
static void __nvoc_init_funcTable_KernelCeContext_1(KernelCeContext *pThis) {
    PORT_UNREFERENCED_VARIABLE(pThis);
} // End __nvoc_init_funcTable_KernelCeContext_1


// Initialize vtable(s) for 32 virtual method(s).
void __nvoc_init_funcTable_KernelCeContext(KernelCeContext *pThis) {
    __nvoc_init_funcTable_KernelCeContext_1(pThis);
}

// Initialize newly constructed object.
void __nvoc_init__KernelCeContext(KernelCeContext *pThis, RmHalspecOwner *pRmhalspecowner) {

    // Initialize pointers to inherited data.
    pThis->__nvoc_pbase_Object = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object;    // (obj) super^5
    pThis->__nvoc_pbase_RsResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource;    // (res) super^4
    pThis->__nvoc_pbase_RmResourceCommon = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_pbase_RmResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource;    // (rmres) super^3
    pThis->__nvoc_pbase_GpuResource = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource;    // (gpures) super^2
    pThis->__nvoc_pbase_INotifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier;    // (inotify) super^3
    pThis->__nvoc_pbase_Notifier = &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier;    // (notify) super^2
    pThis->__nvoc_pbase_ChannelDescendant = &pThis->__nvoc_base_ChannelDescendant;    // (chandes) super
    pThis->__nvoc_pbase_KernelCeContext = pThis;    // (kcectx) this

    // Recurse to superclass initialization function(s).
    __nvoc_init__ChannelDescendant(&pThis->__nvoc_base_ChannelDescendant, pRmhalspecowner);

    // Pointer(s) to metadata structures(s)
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource.metadata__Object;    // (obj) super^5
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RsResource;    // (res) super^4
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RmResourceCommon.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource.metadata__RmResourceCommon;    // (rmrescmn) super^4
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource.metadata__RmResource;    // (rmres) super^3
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__GpuResource;    // (gpures) super^2
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_base_INotifier.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__Notifier.metadata__INotifier;    // (inotify) super^3
    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_Notifier.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant.metadata__Notifier;    // (notify) super^2
    pThis->__nvoc_base_ChannelDescendant.__nvoc_metadata_ptr = &__nvoc_metadata__KernelCeContext.metadata__ChannelDescendant;    // (chandes) super
    pThis->__nvoc_metadata_ptr = &__nvoc_metadata__KernelCeContext;    // (kcectx) this

    // Initialize per-object vtables.
    __nvoc_init_funcTable_KernelCeContext(pThis);
}

NV_STATUS __nvoc_objCreate_KernelCeContext(KernelCeContext **ppThis, Dynamic *pParent, NvU32 createFlags, CALL_CONTEXT * arg_pCallContext, struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams)
{
    NV_STATUS status;
    Object *pParentObj = NULL;
    KernelCeContext *pThis;
    RmHalspecOwner *pRmhalspecowner;

    // Assign `pThis`, allocating memory unless suppressed by flag.
    status = __nvoc_handleObjCreateMemAlloc(createFlags, sizeof(KernelCeContext), (void**)&pThis, (void**)ppThis);
    if (status != NV_OK)
        return status;

    // Zero is the initial value for everything.
    portMemSet(pThis, 0, sizeof(KernelCeContext));

    pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.createFlags = createFlags;

    // pParent must be a valid object that derives from a halspec owner class.
    NV_ASSERT_OR_RETURN(pParent != NULL, NV_ERR_INVALID_ARGUMENT);

    // Link the child into the parent unless flagged not to do so.
    if (!(createFlags & NVOC_OBJ_CREATE_FLAGS_PARENT_HALSPEC_ONLY))
    {
        pParentObj = dynamicCast(pParent, Object);
        objAddChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);
    }
    else
    {
        pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object.pParent = NULL;
    }

    if ((pRmhalspecowner = dynamicCast(pParent, RmHalspecOwner)) == NULL)
        pRmhalspecowner = objFindAncestorOfType(RmHalspecOwner, pParent);
    NV_ASSERT_OR_RETURN(pRmhalspecowner != NULL, NV_ERR_INVALID_ARGUMENT);

    __nvoc_init__KernelCeContext(pThis, pRmhalspecowner);
    status = __nvoc_ctor_KernelCeContext(pThis, pRmhalspecowner, arg_pCallContext, arg_pParams);
    if (status != NV_OK) goto __nvoc_objCreate_KernelCeContext_cleanup;

    // Assignment has no effect if NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT is set.
    *ppThis = pThis;

    return NV_OK;

__nvoc_objCreate_KernelCeContext_cleanup:

    // Unlink the child from the parent if it was linked above.
    if (pParentObj != NULL)
        objRemoveChild(pParentObj, &pThis->__nvoc_base_ChannelDescendant.__nvoc_base_GpuResource.__nvoc_base_RmResource.__nvoc_base_RsResource.__nvoc_base_Object);

    // Do not call destructors here since the constructor already called them.
    if (createFlags & NVOC_OBJ_CREATE_FLAGS_IN_PLACE_CONSTRUCT)
        portMemSet(pThis, 0, sizeof(KernelCeContext));
    else
    {
        portMemFree(pThis);
        *ppThis = NULL;
    }

    // coverity[leaked_storage:FALSE]
    return status;
}

NV_STATUS __nvoc_objCreateDynamic_KernelCeContext(KernelCeContext **ppThis, Dynamic *pParent, NvU32 createFlags, va_list args) {
    NV_STATUS status;
    CALL_CONTEXT * arg_pCallContext = va_arg(args, CALL_CONTEXT *);
    struct RS_RES_ALLOC_PARAMS_INTERNAL * arg_pParams = va_arg(args, struct RS_RES_ALLOC_PARAMS_INTERNAL *);

    status = __nvoc_objCreate_KernelCeContext(ppThis, pParent, createFlags, arg_pCallContext, arg_pParams);

    return status;
}

